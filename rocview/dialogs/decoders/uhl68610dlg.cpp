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

#include "uhl68610dlg.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/Program.h"
#include "rocview/wrapper/public/Gui.h"


Uhl68610Dlg::Uhl68610Dlg( wxWindow* parent )
:uhl68610dlggen( parent )
{

  m_Queue = QueueOp.inst(100);
  m_SendedCmd = NULL;
  m_Timer = new wxTimer( this, DTOPSW_Timer );
  m_Timer->Connect( wxEVT_TIMER, wxTimerEventHandler( Uhl68610Dlg::onTimer ), NULL, this );
  initLabels();
}

void Uhl68610Dlg::initLabels() {
}

void Uhl68610Dlg::onReporting( wxCommandEvent& event ) {
  bool enable = m_Reporting->GetSelection() == 0 ? true:false;
  m_Scale->Enable(enable);
  m_labGap->Enable(enable);
  m_Gap->Enable(enable);
}

void Uhl68610Dlg::onRead( wxCommandEvent& event ) {
  if( !QueueOp.isEmpty(m_Queue) ) {
    TraceOp.trc( "uhl68610", TRCLEVEL_WARNING, __LINE__, 9999, "queue not empty; pending operation...");
    return;
  }

  int addr = m_Addr1->GetValue();

  // start programming mode
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "Starting programming mode..." );
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlncvcmd( cmd, wProgram.lncvstart );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setcv( cmd, 0 );
  wProgram.setvalue( cmd, 65535 );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );


  // address 1, cv 0 (also module address)
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "%s lncv %d program command for module %d", "get", 0, addr );
  cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setcv( cmd, 0 );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  // address 2, cv 1
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "%s lncv %d program command for module %d", "get", 1, addr );
  cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setcv( cmd, 1 );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  // reporting, cv 2
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "%s lncv %d program command for module %d", "get", 2, addr );
  cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setcv( cmd, 2 );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  // scaling factor for calculating velocity in case of double sensor mode, cv 14
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "%s lncv %d program command for module %d", "get", 14, addr );
  cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setcv( cmd, 14 );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  // report format, cv 15
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "%s lncv %d program command for module %d", "get", 15, addr );
  cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setcv( cmd, 15 );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  // stop programming mode
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "Starting programming mode..." );
  cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlncvcmd( cmd, wProgram.lncvend );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setcv( cmd, 0 );
  wProgram.setvalue( cmd, 65535 );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );



  m_Read->Enable(false);
  m_Write->Enable(false);
  sendPacket();

}

void Uhl68610Dlg::onWrite( wxCommandEvent& event ) {
  if( !QueueOp.isEmpty(m_Queue) ) {
    TraceOp.trc( "uhl68610", TRCLEVEL_WARNING, __LINE__, 9999, "queue not empty; pending operation...");
    return;
  }

  int addr = m_Addr1->GetValue();


  // start programming mode
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "Starting programming mode..." );
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlncvcmd( cmd, wProgram.lncvstart );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setcv( cmd, 0 );
  wProgram.setvalue( cmd, 65535 );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );


  // address 1, cv 0 (also module address)
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "%s lncv %d program command for module %d", "get", 0, addr );
  cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvset );
  wProgram.setcv( cmd, 0 );
  wProgram.setvalue( cmd, m_Addr1->GetValue() );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  // address 2, cv 1
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "%s lncv %d program command for module %d", "get", 1, addr );
  cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvset );
  wProgram.setcv( cmd, 1 );
  wProgram.setvalue( cmd, m_Addr2->GetValue() );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  // reporting, cv 2
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "%s lncv %d program command for module %d", "get", 2, addr );
  cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvset );
  wProgram.setcv( cmd, 2 );
  wProgram.setvalue( cmd, m_Reporting->GetSelection() );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  // scaling factor for calculating velocity in case of double sensor mode, cv 14
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "%s lncv %d program command for module %d", "get", 14, addr );
  cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvset );
  wProgram.setcv( cmd, 14 );
  wProgram.setvalue( cmd, getFactor() );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  // report format, cv 15
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "%s lncv %d program command for module %d", "get", 15, addr );
  cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvset );
  wProgram.setcv( cmd, 15 );
  wProgram.setvalue( cmd, m_Format->GetSelection() );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  // stop programming mode
  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "Starting programming mode..." );
  cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlncvcmd( cmd, wProgram.lncvend );
  wProgram.setlntype( cmd, wProgram.lntype_cv );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setcv( cmd, 0 );
  wProgram.setvalue( cmd, 65535 );
  wProgram.setaddr( cmd, addr );
  wProgram.setmodid( cmd, 6861 );
  QueueOp.post( m_Queue, (obj)cmd, normal );


  m_Read->Enable(false);
  m_Write->Enable(false);
  sendPacket();

}

void Uhl68610Dlg::onCancel( wxCommandEvent& event ) {
  EndModal( 0 );
}

void Uhl68610Dlg::onOK( wxCommandEvent& event ) {
  EndModal( wxID_OK );
}


void Uhl68610Dlg::onTimer(wxTimerEvent& event) {
  TraceOp.trc( "uhl68610", TRCLEVEL_WARNING, __LINE__, 9999, "timeout on reply...");
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
  m_Read->Enable(true);
  m_Write->Enable(true);

}


void Uhl68610Dlg::sendPacket() {
  if( !QueueOp.isEmpty(m_Queue) ) {
    iONode cmd = (iONode)QueueOp.get(m_Queue);
    wxGetApp().sendToRocrail( cmd );
    m_SendedCmd = cmd;
    this->SetCursor(wxCURSOR_WATCH);
    m_Timer->Start( 2000, wxTIMER_ONE_SHOT );
  }
  else {
    m_Timer->Stop();
    this->SetCursor(wxCURSOR_ARROW);
    m_Read->Enable(true);
    m_Write->Enable(true);
  }

}

void Uhl68610Dlg::evaluateEvent( int cv, int val ) {
  if( m_SendedCmd != NULL && val != 65535 ) {
    switch( cv ) {
    case 0:
      m_Addr1->SetValue(val);
      break;
    case 1:
      m_Addr2->SetValue(val);
      break;
    case 2:
      m_Reporting->SetSelection(val);
      m_Scale->Enable(val == 0 ? true:false);
      m_labGap->Enable(val == 0 ? true:false);
      m_Gap->Enable(val == 0 ? true:false);
      break;
    case 14:
      m_labGapVal->SetLabel(wxString::Format(_T("(%d)"),val));
      break;
    case 15:
      m_Format->SetSelection(val);
      break;
    }
  }
}

void Uhl68610Dlg::event( iONode event ) {
  int addr     = wProgram.getmodid(event);
  int cmd      = wProgram.getcmd(event);
  int cv       = wProgram.getcv (event);
  int val      = wProgram.getvalue(event);

  TraceOp.trc( "uhl68610", TRCLEVEL_INFO, __LINE__, 9999,
      "lncv event for module %d, lncv%d=%d", addr, cv, val );

  if( cmd == wProgram.datarsp && addr > 0 )
    evaluateEvent( cv, val );

  if( m_SendedCmd != NULL ) {
    NodeOp.base.del(m_SendedCmd);
    m_SendedCmd = NULL;
    ThreadOp.sleep(100);
    sendPacket();
  }

  /* clean up event node */
  NodeOp.base.del(event);

}

int Uhl68610Dlg::getFactor() {
  int factor = 0;
  int scale = m_Scale->GetSelection();
  switch( scale ) {
    case 0: factor = 18 *  32; break;
    case 1: factor = 18 *  45; break;
    case 2: factor = 18 *  87; break;
    case 3: factor = 18 * 120; break;
    case 4: factor = 18 * 160; break;
    case 5: factor = 18 * 220; break;
  }
  m_labGapVal->SetLabel(wxString::Format(_T("(%d)"),m_Gap->GetValue() * factor));
  return m_Gap->GetValue() * factor;
}

void Uhl68610Dlg::onScale( wxCommandEvent& event ) {
  getFactor();
}

void Uhl68610Dlg::onGap( wxSpinEvent& event ) {
  getFactor();
}
