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
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/CBus.h"
#include "rocrail/wrapper/public/CBusNode.h"
#include "rocrail/wrapper/public/CBusNodeVar.h"
#include "rocrail/wrapper/public/CBusNodeEvent.h"

#include "rocs/public/trace.h"
#include "rocdigs/impl/cbus/cbusdefs.h"


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
    initType( wProgram.getmodid(event) );
    m_NodeNumber->SetValue(wProgram.getdecaddr(event));
    if( wProgram.getdecaddr(event) > 0 ) {
      getNode(wProgram.getdecaddr(event), wProgram.getmodid(event));
    }
  }

  initIndex();

  m_SetPage = 0;
  m_NoteBook->Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( CBusNodeDlg::onSetPage ), NULL, this );
  wxCommandEvent cmd( wxEVT_COMMAND_MENU_SELECTED, wxID_CBUSNODE_BOOK );
  wxPostEvent( m_NoteBook, cmd );
}

void CBusNodeDlg::initIndex() {
  m_IndexList->Clear();
  iONode l_RocrailIni = wxGetApp().getFrame()->getRocrailIni();
  if( l_RocrailIni != NULL ) {
    // ToDo: Init index.
    iONode digint = wRocRail.getdigint(l_RocrailIni);
    if( digint != NULL ) {
      iONode cbus = wDigInt.getcbus(digint);
      if( cbus != NULL ) {
        iONode cbusnode = wCBus.getcbnode(cbus);
        while( cbusnode != NULL ) {
          m_IndexList->Append(
              wxString::Format(_T("number %d, type %d"),
                  wCBusNode.getnr(cbusnode), wCBusNode.getmtyp(cbusnode) ),
              cbusnode );
          cbusnode = wCBus.nextcbnode( cbus, cbusnode );
        }
      }
    }
  }
  else {
    wxMessageDialog( this, wxGetApp().getMsg("cbusrocrailini"), _T("Rocrail"), wxOK ).ShowModal();
  }
}

void CBusNodeDlg::initVarList(iONode node) {
  m_VarList->Clear();
  if( node != NULL ) {
    iONode var = wCBusNode.getcbnodevar(node);
    while( var != NULL ) {
      m_VarList->Append(
          wxString::Format(_T("number %d, value %d"),
              wCBusNodeVar.getnr(var), wCBusNodeVar.getval(var) ),
          var );
      var = wCBusNode.nextcbnodevar( node, var );
    }
  }
}

void CBusNodeDlg::initEvtList(iONode node) {
  m_EventList->Clear();
  if( node != NULL ) {
    iONode evt = wCBusNode.getcbnodeevent(node);
    while( evt != NULL ) {
      m_EventList->Append(
          wxString::Format(_T("node %d, addr %d, index %d, value %d"),
              wCBusNodeEvent.getnodenr(evt), wCBusNodeEvent.getaddr(evt),
              wCBusNodeEvent.getevnr(evt), wCBusNodeEvent.getevval(evt) ),
          evt );
      evt = wCBusNode.nextcbnodeevent( node, evt );
    }
  }
}


iONode CBusNodeDlg::getNode(int nr, int mtype) {
  iONode l_RocrailIni = wxGetApp().getFrame()->getRocrailIni();
  if( l_RocrailIni != NULL ) {
    iONode digint = wRocRail.getdigint(l_RocrailIni);
    if( digint != NULL ) {
      iONode cbus = wDigInt.getcbus(digint);
      if( cbus != NULL ) {
        iONode cbusnode = wCBus.getcbnode(cbus);
        while( cbusnode != NULL ) {
          if( wCBusNode.getnr(cbusnode) == nr )
            return cbusnode;
          cbusnode = wCBus.nextcbnode( cbus, cbusnode );
        }
        cbusnode = NodeOp.inst( wCBusNode.name(), cbus, ELEMENT_NODE );
        NodeOp.addChild(cbus, cbusnode);
        wCBusNode.setnr( cbusnode, nr );
        wCBusNode.setmtyp( cbusnode, mtype );
        initIndex();
        return cbusnode;
      }
    }
  }
  return NULL;
}


iONode CBusNodeDlg::getNodeVar(int nn, int mtype, int nr, int val) {
  iONode node = getNode(nn, mtype);
  if( node != NULL ) {
    iONode cbusnodevar = wCBusNode.getcbnodevar(node);
    while( cbusnodevar != NULL ) {
      if( wCBusNodeVar.getnr(cbusnodevar) == nr )
        return cbusnodevar;
      cbusnodevar = wCBusNode.nextcbnodevar( node, cbusnodevar );
    }
    cbusnodevar = NodeOp.inst( wCBusNodeVar.name(), node, ELEMENT_NODE );
    NodeOp.addChild(node, cbusnodevar);
    wCBusNodeVar.setnr( cbusnodevar, nr );
    wCBusNodeVar.setval( cbusnodevar, val );
    initVarList(node);
    return cbusnodevar;
  }
  return NULL;
}


iONode CBusNodeDlg::getNodeEvent(int nn, int mtype, int evnn, int evaddr, int evnr, int evval) {
  iONode node = getNode(nn, mtype);
  if( node != NULL ) {
    iONode cbusnodeevt = wCBusNode.getcbnodeevent(node);
    while( cbusnodeevt != NULL ) {
      if( wCBusNodeEvent.getnodenr(cbusnodeevt) == evnn && wCBusNodeEvent.getaddr(cbusnodeevt) == evaddr &&
          wCBusNodeEvent.getevnr(cbusnodeevt) == evnr ) {
        wCBusNodeEvent.setevnr(cbusnodeevt, evval);
        return cbusnodeevt;
      }
      cbusnodeevt = wCBusNode.nextcbnodeevent( node, cbusnodeevt );
    }
    cbusnodeevt = NodeOp.inst( wCBusNodeEvent.name(), node, ELEMENT_NODE );
    NodeOp.addChild(node, cbusnodeevt);
    wCBusNodeEvent.setnodenr( cbusnodeevt, evnn );
    wCBusNodeEvent.setaddr( cbusnodeevt, evaddr );
    wCBusNodeEvent.setevnr( cbusnodeevt, evnr );
    wCBusNodeEvent.setevval( cbusnodeevt, evval );
    initEvtList(node);
    return cbusnodeevt;
  }
  return NULL;
}


void CBusNodeDlg::initType( int mtype ) {
  switch( mtype ) {
  case MTYP_CANACC4:   m_NodeType->SetValue(_T("CANACC4")); break;
  case MTYP_CANACC5:   m_NodeType->SetValue(_T("CANACC5")); break;
  case MTYP_CANACC8:   m_NodeType->SetValue(_T("CANACC8")); break;
  case MTYP_CANACE3:   m_NodeType->SetValue(_T("CANACE3")); break;
  case MTYP_CANACE8C:  m_NodeType->SetValue(_T("CANACE8C")); break;
  case MTYP_CANLED:    m_NodeType->SetValue(_T("CANLED")); break;
  case MTYP_CANLED64:  m_NodeType->SetValue(_T("CANLED64")); break;
  case MTYP_CANACC4_2: m_NodeType->SetValue(_T("CANACC4_2")); break;
  case MTYP_CANCAB:    m_NodeType->SetValue(_T("CANCAB")); break;
  case MTYP_CANCMD:    m_NodeType->SetValue(_T("CANCMD")); break;
  case MTYP_CANSERVO:  m_NodeType->SetValue(_T("CANSERVO")); break;
  case MTYP_CANBC:     m_NodeType->SetValue(_T("CANBC")); break;
  case MTYP_CANRPI:    m_NodeType->SetValue(_T("CANRPI")); break;
  case MTYP_CANTTCA:   m_NodeType->SetValue(_T("CANTTCA")); break;
  case MTYP_CANTTCB:   m_NodeType->SetValue(_T("CANTTCB")); break;
  }
  if( mtype > 0 )
    m_NodeTypeNr->SetValue(mtype);
}

void CBusNodeDlg::onSetPage(wxCommandEvent& event) {
  TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999, "set page to %d", m_SetPage );
  m_NoteBook->SetSelection( m_SetPage );
}


void CBusNodeDlg::onOK( wxCommandEvent& event )
{
  iONode l_RocrailIni = wxGetApp().getFrame()->getRocrailIni();
  if( l_RocrailIni != NULL ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
    wSysCmd.setcmd( cmd, wSysCmd.setini );
    NodeOp.addChild( cmd, (iONode)l_RocrailIni->base.clone( l_RocrailIni ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  EndModal( wxID_OK );
}

void CBusNodeDlg::onCancel( wxCommandEvent& event )
{
  EndModal(0);
}

void CBusNodeDlg::onSetNodeNumber( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.nnreq );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  getNode(nn, m_NodeTypeNr->GetValue());
}

void CBusNodeDlg::onIndexSelect( wxCommandEvent& event ) {
  if( m_IndexList->GetSelection() != wxNOT_FOUND ) {
    iONode node = (iONode)m_IndexList->GetClientData(m_IndexList->GetSelection());
    if( node != NULL ) {
      initType(wCBusNode.getmtyp(node));
      m_NodeNumber->SetValue(wCBusNode.getnr(node));
      initVarList(node);
      initEvtList(node);
    }
    else
      TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}

void CBusNodeDlg::onIndexDelete( wxCommandEvent& event ) {
  if( m_IndexList->GetSelection() != wxNOT_FOUND ) {
    iONode node = (iONode)m_IndexList->GetClientData(m_IndexList->GetSelection());
    if( node != NULL ) {
      iONode l_RocrailIni = wxGetApp().getFrame()->getRocrailIni();
      iONode digint = wRocRail.getdigint(l_RocrailIni);
      iONode cbus = wDigInt.getcbus(digint);
      NodeOp.removeChild( cbus, node );
      m_NodeNumber->SetValue(0);
      m_NodeType->SetValue(_T(""));
      m_NodeTypeNr->SetValue(0);
      initIndex();
    }
    else
      TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}


void CBusNodeDlg::onVarSelect( wxCommandEvent& event ) {
  if( m_VarList->GetSelection() != wxNOT_FOUND ) {
    iONode var = (iONode)m_VarList->GetClientData(m_VarList->GetSelection());
    if( var != NULL ) {
      m_VarIndex->SetValue(wCBusNodeVar.getnr(var));
      m_VarValue->SetValue(wCBusNodeVar.getval(var));
    }
    else
      TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}


void CBusNodeDlg::onVarValue( wxSpinEvent& event ) {
  int val = m_VarValue->GetValue();

  m_VarBit0->SetValue( val&0x01? true:false );
  m_VarBit1->SetValue( val&0x02? true:false );
  m_VarBit2->SetValue( val&0x04? true:false );
  m_VarBit3->SetValue( val&0x08? true:false );
  m_VarBit4->SetValue( val&0x10? true:false );
  m_VarBit5->SetValue( val&0x20? true:false );
  m_VarBit6->SetValue( val&0x40? true:false );
  m_VarBit7->SetValue( val&0x80? true:false );
}

void CBusNodeDlg::onVarBit( wxCommandEvent& event ) {
  int val = 0;
  val |= m_VarBit0->IsChecked() ? 0x01:0;
  val |= m_VarBit1->IsChecked() ? 0x02:0;
  val |= m_VarBit2->IsChecked() ? 0x04:0;
  val |= m_VarBit3->IsChecked() ? 0x08:0;
  val |= m_VarBit4->IsChecked() ? 0x10:0;
  val |= m_VarBit5->IsChecked() ? 0x20:0;
  val |= m_VarBit6->IsChecked() ? 0x40:0;
  val |= m_VarBit7->IsChecked() ? 0x80:0;

  m_VarValue->SetValue( val );
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
  cmd->base.del(cmd);
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
  cmd->base.del(cmd);
  getNodeVar(nn, m_NodeTypeNr->GetValue(), m_VarIndex->GetValue(), m_VarValue->GetValue() );
}

void CBusNodeDlg::onEventSelect( wxCommandEvent& event ) {
  if( m_EventList->GetSelection() != wxNOT_FOUND ) {
    iONode event = (iONode)m_EventList->GetClientData(m_EventList->GetSelection());
    if( event != NULL ) {
      int evnn  = wCBusNodeEvent.getnodenr(event);
      int addr  = wCBusNodeEvent.getaddr(event);
      int evnr  = wCBusNodeEvent.getevnr(event);
      int evval = wCBusNodeEvent.getevval(event);
      m_EventNodeNr->SetValue(evnn);
      m_EventAddress->SetValue(addr);
      m_EventIndex->SetValue(evnr);
      m_EventVar->SetValue(evval);
    }
    else
      TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}

void CBusNodeDlg::onEventGetAll( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.evgetall );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void CBusNodeDlg::onEvtGetVar( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.evgetvar );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wProgram.setval2(cmd, m_EventNodeNr->GetValue()); // nn
  wProgram.setval3(cmd, m_EventAddress->GetValue()); // addr
  wProgram.setval1(cmd, m_EventIndex->GetValue() ); // idx
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
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
  cmd->base.del(cmd);
  getNodeEvent(nn, m_NodeTypeNr->GetValue(), m_EventNodeNr->GetValue(),
      m_EventAddress->GetValue(), m_EventIndex->GetValue(), m_EventVar->GetValue() );
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
  cmd->base.del(cmd);

  iONode node = getNode(nn, m_NodeTypeNr->GetValue());
  if( node != NULL ) {
    iONode event = getNodeEvent(nn, m_NodeTypeNr->GetValue(), m_EventNodeNr->GetValue(),
        m_EventAddress->GetValue(), m_EventIndex->GetValue(), m_EventVar->GetValue() );
    if( event != NULL ) {
      NodeOp.removeChild(node, event);
      NodeOp.base.del(event);
      initEvtList(node);
    }
  }
}

void CBusNodeDlg::onEV( wxSpinEvent& event ) {
  int val = m_EventVar->GetValue();

  m_EVBit0->SetValue( val&0x01? true:false );
  m_EVBit1->SetValue( val&0x02? true:false );
  m_EVBit2->SetValue( val&0x04? true:false );
  m_EVBit3->SetValue( val&0x08? true:false );
  m_EVBit4->SetValue( val&0x10? true:false );
  m_EVBit5->SetValue( val&0x20? true:false );
  m_EVBit6->SetValue( val&0x40? true:false );
  m_EVBit7->SetValue( val&0x80? true:false );
}

void CBusNodeDlg::onEVBit( wxCommandEvent& event ) {
  int val = 0;
  val |= m_EVBit0->IsChecked() ? 0x01:0;
  val |= m_EVBit1->IsChecked() ? 0x02:0;
  val |= m_EVBit2->IsChecked() ? 0x04:0;
  val |= m_EVBit3->IsChecked() ? 0x08:0;
  val |= m_EVBit4->IsChecked() ? 0x10:0;
  val |= m_EVBit5->IsChecked() ? 0x20:0;
  val |= m_EVBit6->IsChecked() ? 0x40:0;
  val |= m_EVBit7->IsChecked() ? 0x80:0;

  m_EventVar->SetValue( val );
}

void CBusNodeDlg::onLearn( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.learn );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void CBusNodeDlg::onUnlearn( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.unlearn );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void CBusNodeDlg::event( iONode event ) {
  if( wProgram.getcmd( event ) == wProgram.nnreq  ) {
    init( event );
  }
  else if( wProgram.getcmd( event ) == wProgram.evget || wProgram.getcmd( event ) == wProgram.evgetvar ) {
    // Add event to the list.
    int nn   = wProgram.getdecaddr(event);
    int ennr = wProgram.getval1(event);
    int ennn = wProgram.getval2(event);
    int addr = wProgram.getval3(event);
    int val  = wProgram.getval4(event);

    // get the event value...
    getNodeEvent(nn, m_NodeTypeNr->GetValue(), ennn, addr, ennr, val );
  }
  else if( wProgram.getcmd( event ) == wProgram.get  ) {
    int nn   = wProgram.getdecaddr(event);
    int cv = wProgram.getcv(event);
    int val = wProgram.getvalue(event);

    getNodeVar(nn, m_NodeTypeNr->GetValue(), cv, val );
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
  cmd->base.del(cmd);

  // Delete all child nodes:
  iONode node = getNode(nn, m_NodeTypeNr->GetValue());
  if( node != NULL ) {
    iONode cbusnodeevt = wCBusNode.getcbnodeevent(node);
    while( cbusnodeevt != NULL ) {
      NodeOp.removeChild(node, cbusnodeevt);
      NodeOp.base.del(cbusnodeevt);
      cbusnodeevt = wCBusNode.getcbnodeevent( node );
    }
    initEvtList(node);
  }
}
