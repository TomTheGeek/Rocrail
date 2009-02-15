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
#include "dtopswdlg.h"

#include "rocdigs/impl/loconet/dtopsw.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocgui/public/guiapp.h"

#include "rocrail/wrapper/public/Program.h"
#include "rocgui/wrapper/public/Gui.h"


DTOpSwDlg::DTOpSwDlg( wxWindow* parent )
  :dtopswdlg( parent )
{
  m_Queue = QueueOp.inst(100);
  m_SendedCmd = NULL;
  m_Timer = new wxTimer( this, DTOPSW_Timer );
  m_Timer->Connect( wxEVT_TIMER, wxTimerEventHandler( DTOpSwDlg::onTimer ), NULL, this );

  initLabels();
  m_BoardType->SetSelection(0);
  initValues();
}


void DTOpSwDlg::initLabels() {
  int i = 0;
  while( DT_BoardTypes[i] != NULL ) {
    m_BoardType->Append( wxString(DT_BoardTypes[i],wxConvUTF8) );
    i++;
  }
}


void DTOpSwDlg::initValues() {
  int selectedBoard = m_BoardType->GetSelection();
  m_OpSwList->Clear();

  int i = 0;
  while( DT_OpSw[selectedBoard][i] != NULL ) {
    m_OpSwList->Append( wxString::Format(_T("%02d"), DT_idxOpSw[selectedBoard][i]) + _T(" ") + wxString(DT_OpSw[selectedBoard][i],wxConvUTF8) );
    i++;
  }
}


void DTOpSwDlg::onBoardType( wxCommandEvent& event ) {
  initValues();
}


void DTOpSwDlg::onReadAll( wxCommandEvent& event ) {
  if( !QueueOp.isEmpty(m_Queue) ) {
    TraceOp.trc( "dtopsw", TRCLEVEL_WARNING, __LINE__, 9999, "queue not empty; pending operation...");
    return;
  }

  int selectedBoard = m_BoardType->GetSelection();
  int addr = m_BoardNumber->GetValue();

  int idx = 0;

  while(DT_idxOpSw[selectedBoard][idx] != 0) {
    TraceOp.trc( "dtopsw", TRCLEVEL_INFO, __LINE__, 9999, "%s opsw %d program command for board %d",
        "get", DT_idxOpSw[selectedBoard][idx], addr );

    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setlntype( cmd, wProgram.lntype_opsw );
    wProgram.setcmd( cmd, wProgram.lncvget );
    wProgram.setaddr( cmd, addr );
    wProgram.setmodid( cmd, DT_msgType[selectedBoard] );
    wProgram.setcv( cmd, DT_idxOpSw[selectedBoard][idx] );
    QueueOp.post( m_Queue, (obj)cmd, normal );

    idx++;
  }

  m_ReadAll->Enable(false);
  m_WriteAll->Enable(false);
  sendPacket();

}


void DTOpSwDlg::onWriteAll( wxCommandEvent& event ) {
  if( !QueueOp.isEmpty(m_Queue) ) {
    TraceOp.trc( "dtopsw", TRCLEVEL_WARNING, __LINE__, 9999, "queue not empty; pending operation...");
    return;
  }

  int selectedBoard = m_BoardType->GetSelection();
  int addr = m_BoardNumber->GetValue();

  int idx = 0;

  while(DT_idxOpSw[selectedBoard][idx] != 0) {
    TraceOp.trc( "dtopsw", TRCLEVEL_INFO, __LINE__, 9999, "%s opsw %d program command for board %d",
        "set", DT_idxOpSw[selectedBoard][idx], addr );

    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setlntype( cmd, wProgram.lntype_opsw );
    wProgram.setcmd( cmd, wProgram.lncvset );
    wProgram.setaddr( cmd, addr );
    wProgram.setmodid( cmd, DT_msgType[selectedBoard] );
    wProgram.setcv( cmd, DT_idxOpSw[selectedBoard][idx] );
    wProgram.setvalue( cmd, m_OpSwList->IsChecked(idx) ? 1:0 );
    QueueOp.post( m_Queue, (obj)cmd, normal );

    idx++;
  }

  m_ReadAll->Enable(false);
  m_WriteAll->Enable(false);
  sendPacket();

}

void DTOpSwDlg::onTimer(wxTimerEvent& event) {
  TraceOp.trc( "dtopsw", TRCLEVEL_WARNING, __LINE__, 9999, "timeout on reply...");
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
  m_ReadAll->Enable(true);
  m_WriteAll->Enable(true);

}


void DTOpSwDlg::sendPacket() {
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
    m_ReadAll->Enable(true);
    m_WriteAll->Enable(true);
  }

}

void DTOpSwDlg::evaluateEvent( int val ) {
  if( m_SendedCmd != NULL ) {
    int selectedBoard = m_BoardType->GetSelection();
    int opsw = wProgram.getcv( m_SendedCmd );
    int idx = 0;
    while(DT_idxOpSw[selectedBoard][idx] != 0) {

      if( DT_idxOpSw[selectedBoard][idx] == opsw ) {
        m_OpSwList->Check(idx, val?true:false);
        break;
      }
      idx++;
    }
  }
}

void DTOpSwDlg::event( iONode event ) {
  int addr     = wProgram.getaddr(event);
  int cmd      = wProgram.getcmd(event);
  int type     = wProgram.getlntype(event);
  int cv       = wProgram.getcv (event);
  int val      = wProgram.getvalue(event);

  TraceOp.trc( "dtopsw", TRCLEVEL_INFO, __LINE__, 9999,
      "lnopsw event for addr %d, opsw%d=%d", addr, cv, val );

  if( cmd == wProgram.datarsp )
    evaluateEvent( val );

  if( m_SendedCmd != NULL ) {
    NodeOp.base.del(m_SendedCmd);
    m_SendedCmd = NULL;
    ThreadOp.sleep(100);
    sendPacket();
  }

  /* clean up event node */
  NodeOp.base.del(event);

}



void DTOpSwDlg::onOK( wxCommandEvent& event ){
  EndModal( wxID_OK );
}

