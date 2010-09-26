/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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
  m_TabAlign = wxGetApp().getTabAlign();

}

void MGV141Dlg::initLabels() {
  TraceOp.trc( "mgv141", TRCLEVEL_INFO, __LINE__, 9999, "initLabels" );
  SetTitle(wxGetApp().getMsg( "cartable" ));
  m_NoteBook->SetPageText( 0, wxGetApp().getMsg( "query" ) );
  m_NoteBook->SetPageText( 1, wxGetApp().getMsg( "setup" ) );

  // Buttons
  m_StdButtonsOK->SetLabel( wxGetApp().getMsg( "ok" ) );
}

void MGV141Dlg::onUnitSet( wxCommandEvent& event )
{
	// TODO: Implement onUnitSet
}

void MGV141Dlg::onWriteAll( wxCommandEvent& event )
{
	// TODO: Implement onWriteAll
}

void MGV141Dlg::onReadAll( wxCommandEvent& event )
{
	// TODO: Implement onReadAll
}

void MGV141Dlg::onOK( wxCommandEvent& event )
{
  EndModal( wxID_OK );
}

void MGV141Dlg::onUnitSelected( wxCommandEvent& event ) {

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

  TraceOp.trc( "mgv141", TRCLEVEL_INFO, __LINE__, 9999, "%s lnsv program command for %d/%d",
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
  TraceOp.trc( "mgv141", TRCLEVEL_WARNING, __LINE__, 9999, "timeout on reply...");
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

  TraceOp.trc( "mgv141", TRCLEVEL_INFO, __LINE__, 9999, "lnsv event for addr %d.%d, sv%d=%d ver=%d",
      lowaddr, subaddr, cv, val, ver );
  if( lowaddr == 80 ) {
    TraceOp.trc( "mgv141", TRCLEVEL_INFO, __LINE__, 9999, "not evaluating the LocoBuffer..." );
  }
  else if( (m_iLowAddress == lowaddr && m_iSubAddress == subaddr) ||
          m_iLowAddress == -1 || m_iSubAddress == -1 ||
          (lowaddr == -1 && subaddr == -1) )
  {
    evaluateEvent( type, lowaddr, subaddr, cv, val, ver );
    if( m_SendedCmd != NULL && cv == wProgram.getcv (m_SendedCmd) ) {
      TraceOp.trc( "mgv141", TRCLEVEL_INFO, __LINE__, 9999, "reply matches the sended request");
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
      TraceOp.trc( "mgv141", TRCLEVEL_INFO, __LINE__, 9999, "address %03d/%03d ver:%d already in list..,", lowaddr, subaddr, ver );
    }
  }

  /* clean up event node */
  NodeOp.base.del(event);
}

void MGV141Dlg::evaluateEvent( int type, int low, int sub, int sv, int val, int ver ) {
  TraceOp.trc( "mgv141", TRCLEVEL_INFO, __LINE__, 9999, "evaluating sv%d=%d", sv, val );

}

