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
#include "cbusnodedlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/Program.h"
#include "rocview/wrapper/public/Gui.h"

#include "rocs/public/trace.h"

CBusNodeDlg::CBusNodeDlg( wxWindow* parent ):cbusnodedlggen( parent )
{
  init(NULL);
}

CBusNodeDlg::CBusNodeDlg( wxWindow* parent, iONode event ):cbusnodedlggen( parent )
{
  init(event);
}

void CBusNodeDlg::init( iONode event ) {
  if( event != NULL ) {
    m_IID->SetValue( wxString(wProgram.getiid(event),wxConvUTF8) );

    if( wProgram.getmodid(event) == 3 ) {
      m_NodeType->SetValue(_T("CANACC8"));
    }
    else if( wProgram.getmodid(event) == 5 ) {
      m_NodeType->SetValue(_T("CANACE8C"));
    }
  }

  m_SetPage = 0;
  m_NoteBook->Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CBusNodeDlg::onSetPage ), NULL, this );
  wxCommandEvent cmd( wxEVT_COMMAND_MENU_SELECTED, wxID_CBUSNODE_BOOK );
  wxPostEvent( m_NoteBook, cmd );
}

void CBusNodeDlg::onSetPage(wxCommandEvent& event) {
  TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999, "set page to %d", m_SetPage );
  m_NoteBook->SetSelection( m_SetPage );
}


void CBusNodeDlg::onOK( wxCommandEvent& event )
{
  EndModal( wxID_OK );
}

void CBusNodeDlg::onSetNodeNumber( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.nnreq );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wxGetApp().sendToRocrail( cmd );
}

void CBusNodeDlg::onIndexSelect( wxCommandEvent& event ) {

}

void CBusNodeDlg::onIndexDelete( wxCommandEvent& event ) {

}

void CBusNodeDlg::onVarValue( wxSpinEvent& event ) {

}

void CBusNodeDlg::onVarBit( wxCommandEvent& event ) {

}

void CBusNodeDlg::onVarGet( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.get );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wProgram.setcv( cmd, m_VarIndex->GetValue() );
  wxGetApp().sendToRocrail( cmd );
}

void CBusNodeDlg::onVarSet( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.set );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wProgram.setcv( cmd, m_VarIndex->GetValue() );
  wProgram.setvalue( cmd, m_VarValue->GetValue() );
  wxGetApp().sendToRocrail( cmd );
}

void CBusNodeDlg::onEventSelect( wxCommandEvent& event ) {

}

void CBusNodeDlg::onEventGetAll( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.evgetall );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wxGetApp().sendToRocrail( cmd );
}

void CBusNodeDlg::onEventAdd( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.evset );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wProgram.setval2(cmd, m_EventNodeNr->GetValue()); // nn
  wProgram.setval3(cmd, m_EventAddress->GetValue()); // addr
  wProgram.setval1(cmd, m_EventIndex->GetValue() ); // idx
  wProgram.setval4(cmd, m_EventVar->GetValue() ); // val
  wxGetApp().sendToRocrail( cmd );
}

void CBusNodeDlg::onEventDelete( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.evdelete );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wProgram.setval2(cmd, m_EventNodeNr->GetValue()); // nn
  wProgram.setval3(cmd, m_EventAddress->GetValue()); // addr
  wxGetApp().sendToRocrail( cmd );
}

void CBusNodeDlg::onEV( wxSpinEvent& event ) {

}

void CBusNodeDlg::onEVBit( wxCommandEvent& event ) {

}

void CBusNodeDlg::onLearn( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.learn );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wxGetApp().sendToRocrail( cmd );
}

void CBusNodeDlg::onUnlearn( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.unlearn );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wxGetApp().sendToRocrail( cmd );
}


void CBusNodeDlg::event( iONode event ) {
  if( wProgram.getcmd( event ) == wProgram.nnreq  ) {
    init( event );
  }
  else if( wProgram.getcmd( event ) == wProgram.evget  ) {
    // ToDo: Add event to the list.
    int nn   = wProgram.getdecaddr(event);
    int ennr = wProgram.getval1(event);
    int ennn = wProgram.getval2(event);
    int addr = wProgram.getval3(event);
    m_EventList->Append( wxString::Format(_T("index %d, nodenr %d, address %d"), ennr, ennn, addr),
        event->base.clone(event) );
  }
}

void CBusNodeDlg::onEvtClearAll( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.evclrall );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wxGetApp().sendToRocrail( cmd );
}
