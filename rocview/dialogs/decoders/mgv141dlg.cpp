/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "mgv141dlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/Program.h"
#include "rocview/wrapper/public/Gui.h"

#include "rocs/public/trace.h"

MGV141Dlg::MGV141Dlg( wxWindow* parent )
    :mgv141gen( parent )
{
  TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "init dialog");
  m_TabAlign = wxGetApp().getTabAlign();
  m_Queue = QueueOp.inst(100);
  m_SendedCmd = NULL;
  m_Timer = new wxTimer( this, ME_SVTimer );
  m_iLowAddress = m_UnitHigh->GetValue();
  m_iSubAddress = m_UnitLow->GetValue();

}

void MGV141Dlg::initLabels() {
  TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "initLabels" );
  SetTitle(wxGetApp().getMsg( "cartable" ));
  m_NoteBook->SetPageText( 0, wxGetApp().getMsg( "query" ) );
  m_NoteBook->SetPageText( 1, wxGetApp().getMsg( "setup" ) );

  // Buttons
  m_StdButtonsOK->SetLabel( wxGetApp().getMsg( "ok" ) );
}

void MGV141Dlg::onUnitSet( wxCommandEvent& event )
{
  if( !QueueOp.isEmpty(m_Queue) ) {
    TraceOp.trc( "GCA141", TRCLEVEL_WARNING, __LINE__, 9999, "queue not empty; pending operation...");
    return;
  }

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  //m_iLowAddress = m_LowAddr->GetValue();
  //m_iSubAddress = m_SubAddr->GetValue();
  wProgram.setlntype( cmd, wProgram.lntype_sv );
  wProgram.setcmd( cmd, wProgram.lncvset );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setaddr( cmd, m_iLowAddress );
  wProgram.setmodid( cmd, m_iSubAddress );

  if( m_iLowAddress != m_UnitHigh->GetValue() ) {
    // low address
    iONode cmdH = (iONode)NodeOp.base.clone(cmd);
    wProgram.setcv( cmdH, 1 );
    wProgram.setvalue( cmdH, m_UnitHigh->GetValue() );
    m_iLowAddress = -1;
    QueueOp.post( m_Queue, (obj)cmdH, normal );
  }

  if( m_iSubAddress != m_UnitLow->GetValue() ) {
    // sub address
    iONode cmdL = (iONode)NodeOp.base.clone(cmd);
    wProgram.setaddr( cmdL, m_iLowAddress ); // replace low address with new one
    wProgram.setcv( cmdL, 2 );
    wProgram.setvalue( cmdL, m_UnitLow->GetValue() );
    m_iSubAddress = -1;
    QueueOp.post( m_Queue, (obj)cmdL, normal );
  }

  NodeOp.base.del(cmd);

  sendPacket();
}

void MGV141Dlg::onWriteAll( wxCommandEvent& event )
{
  wxSpinCtrl* CA[] = {NULL,m_CounterAddress1,m_CounterAddress2,m_CounterAddress3,m_CounterAddress4,
                        m_CounterAddress5,m_CounterAddress6, m_CounterAddress7,m_CounterAddress8};

  for( int i = 1; i <= 8; i++ ) {
    int config = 0;
    int val1 = CA[i]->GetValue();
    makePacket( 3, i, val1, true, false);
  }
  sendPacket();
  m_UnitSet->Enable(false);
  m_WriteAll->Enable(false);
  m_ReadAll->Enable(false);
}


void MGV141Dlg::makePacket( int offset, int port, int val1, bool sv_set, bool sendnow )
{
  if( sendnow && !QueueOp.isEmpty(m_Queue) ) {
    TraceOp.trc( "GCA141", TRCLEVEL_WARNING, __LINE__, 9999, "queue not empty; pending operation...");
    return;
  }

  TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "%s lnsv program command for %d/%d %d",
      sv_set?"set":"get", m_iLowAddress, m_iSubAddress, port );

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  m_iLowAddress = m_UnitHigh->GetValue();
  m_iSubAddress = m_UnitLow->GetValue();


  wProgram.setlntype( cmd, wProgram.lntype_sv );
  wProgram.setcmd( cmd, sv_set ? wProgram.lncvset:wProgram.lncvget );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setaddr( cmd, m_iLowAddress );
  wProgram.setmodid( cmd, m_iSubAddress );
  wProgram.setcv( cmd, (port-1)*2 + offset );
  wProgram.setvalue( cmd, val1 % 256 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  cmd = (iONode)NodeOp.base.clone(cmd);
  wProgram.setcv( cmd, (port-1) * 2 + offset+1 );
  wProgram.setvalue( cmd, val1/256 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  if( sendnow )
    sendPacket();

}



void MGV141Dlg::onReadAll( wxCommandEvent& event )
{
  for( int i = 1; i <= 8; i++ ) {
    makePacket( 3, i, 0, false, false);
  }
  sendPacket();
  m_UnitSet->Enable(false);
  m_WriteAll->Enable(false);
  m_ReadAll->Enable(false);
}

void MGV141Dlg::onOK( wxCommandEvent& event )
{
  m_Timer->Stop();
  EndModal( wxID_OK );
}

void MGV141Dlg::onUnitSelected( wxCommandEvent& event ) {
  if( m_AddressList->GetSelection() == wxNOT_FOUND )
    return;

  // "%03d/%03d ver:%d"
  char* s = StrOp.dup(m_AddressList->GetStringSelection().mb_str(wxConvUTF8));
  TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "Selected MGV141 = [%s]", s );
  // 000/000
  s[3] = '\0';
  s[7] = '\0';
  m_UnitHigh->SetValue( atoi(s) );
  m_UnitLow->SetValue( atoi(s+4) );
  StrOp.free(s);

  m_iLowAddress = m_UnitHigh->GetValue();
  m_iSubAddress = m_UnitLow->GetValue();
  SetTitle( wxString::Format( _T("%s  %d-%d"), _T("GCA141"), m_iLowAddress , m_iSubAddress ) );

}

void MGV141Dlg::onQuery( wxCommandEvent& event ) {
  // Clear list and query
  m_AddressList->Clear();
  queryAddresses();
}

void MGV141Dlg::queryAddresses()
{
  if( !QueueOp.isEmpty(m_Queue) ) {
    TraceOp.trc( "locoio", TRCLEVEL_WARNING, __LINE__, 9999, "queue not empty; pending operation...");
    return;
  }

  TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "%s lnsv program command for %d/%d",
      "get", 0, 1 );

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );

  wProgram.setlntype( cmd, wProgram.lntype_sv );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setaddr( cmd, 0 );
  wProgram.setmodid( cmd, 0 );
  wProgram.setcv( cmd, 0 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  sendPacket();

}

void MGV141Dlg::sendPacket() {
  if( !QueueOp.isEmpty(m_Queue) ) {
    iONode cmd = (iONode)QueueOp.get(m_Queue);
    wxGetApp().sendToRocrail( cmd );
    m_SendedCmd = cmd;
    this->SetCursor(wxCURSOR_WATCH);
    m_Timer->Start( 3000, wxTIMER_ONE_SHOT );
  }
  else {
    m_Timer->Stop();
    this->SetCursor(wxCURSOR_ARROW);
  }
}


void MGV141Dlg::OnTimer(wxTimerEvent& event) {
  TraceOp.trc( "GCA141", TRCLEVEL_WARNING, __LINE__, 9999, "timeout on reply...");
  this->SetCursor(wxCURSOR_ARROW);
  if( m_SendedCmd != NULL ) {

    NodeOp.base.del(m_SendedCmd);
    m_SendedCmd = NULL;

    /* empty queue */
    iONode cmd = (iONode)QueueOp.get(m_Queue);
    while( cmd != NULL ) {
      NodeOp.base.del(cmd);
      cmd = (iONode)QueueOp.get(m_Queue);
    }
  }
  m_UnitSet->Enable(true);
  m_WriteAll->Enable(true);
  m_ReadAll->Enable(true);
}

void MGV141Dlg::event( iONode event ) {
  int lowaddr  = wProgram.getdecaddr(event);
  int subaddr  = wProgram.getmodid(event);
  int cmd      = wProgram.getcmd(event);
  int type     = wProgram.getlntype(event);
  int cv       = wProgram.getcv (event);
  int val      = wProgram.getvalue(event);
  int ver      = wProgram.getversion(event);
  bool datarsp = false;

  if( cmd == wProgram.datarsp )
    datarsp = true;

  TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "lnsv event for addr %d.%d, sv%d=%d ver=%d",
      lowaddr, subaddr, cv, val, ver );
  if( lowaddr == 80 ) {
    TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "not evaluating the LocoBuffer..." );
  }
  else if( (m_iLowAddress == lowaddr && m_iSubAddress == subaddr) ||
          m_iLowAddress == -1 || m_iSubAddress == -1 ||
          (lowaddr == -1 && subaddr == -1) )
  {
    evaluateEvent( type, lowaddr, subaddr, cv, val, ver );
    if( m_SendedCmd != NULL && cv == wProgram.getcv (m_SendedCmd) ) {
      TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "reply matches the sended request");
      NodeOp.base.del(m_SendedCmd);
      m_SendedCmd = NULL;
      ThreadOp.sleep(100);
      sendPacket();
    }
  }

  if( lowaddr != 80 && lowaddr > 0 ) {
    wxString item = wxString::Format(_T("%03d/%03d ver:%d"), lowaddr, subaddr, ver );
    if( !m_AddressList->SetStringSelection(item) )
      m_AddressList->Append( item );
    else {
      const char* product = "LocoIO";
      if( ver < 10 )
        product = "LocoBooster";
      if( ver < 110 && ver >= 100 )
        product = "LocoServo";
      TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "address %03d/%03d ver:%d already in list..,", lowaddr, subaddr, ver );
    }
  }

  /* clean up event node */
  NodeOp.base.del(event);

  if( !QueueOp.isEmpty(m_Queue) ) {
    sendPacket();
  }
  else {
    m_UnitSet->Enable(true);
    m_WriteAll->Enable(true);
    m_ReadAll->Enable(true);
  }
}

void MGV141Dlg::evaluateEvent( int type, int low, int sub, int sv, int val, int ver ) {
  TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "evaluating sv%d=%d", sv, val );

  if( sv == 0 ) {
    // setup
    TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "setup" );
  }
  else if( sv == 1 ) {
    // low address
    TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "low address %d", val );
    m_iLowAddress = val;
    m_UnitHigh->SetValue(m_iLowAddress);
    SetTitle( wxString::Format( _T("%s  %d-%d"), _T("LocoIO"), m_iLowAddress , m_iSubAddress ) );
  }
  else if( sv == 2 ) {
    // sub address
    TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "sub address %d", val );
    m_iSubAddress = val;
    m_UnitLow->SetValue(m_iSubAddress);
    SetTitle( wxString::Format( _T("%s  %d-%d"), _T("LocoIO"), m_iLowAddress , m_iSubAddress ) );
  }
  else if( sv > 2 && sv < 20 ) {
    int port = (sv - 3) / 2;
    TraceOp.trc( "GCA141", TRCLEVEL_INFO, __LINE__, 9999, "counter %d address %d", port, val );
    wxSpinCtrl* CA[] = {m_CounterAddress1,m_CounterAddress2,m_CounterAddress3,m_CounterAddress4,
                          m_CounterAddress5,m_CounterAddress6, m_CounterAddress7,m_CounterAddress8};
    if( (sv-3) % 2 == 1 ) {
      // high part
      CA[port]->SetValue(CA[port]->GetValue() + val*256);
    }
    else {
      // low part
      CA[port]->SetValue(val);
    }
  }
}

