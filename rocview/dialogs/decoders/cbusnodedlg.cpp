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
#include <wx/filedlg.h>

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

#define ME_GC2Timer 4711

CBusNodeDlg::CBusNodeDlg( wxWindow* parent ):cbusnodedlggen( parent )
{
  init(NULL);
}

CBusNodeDlg::CBusNodeDlg( wxWindow* parent, iONode event ):cbusnodedlggen( parent )
{
  m_bGC2GetAll = false;
  m_bGC2SetAll = false;
  init(event);
}

void CBusNodeDlg::init( iONode event ) {
  m_Timer = new wxTimer( this, ME_GC2Timer );
  Connect( wxEVT_TIMER, wxTimerEventHandler( CBusNodeDlg::OnTimer ), NULL, this );

  // Init labels.
  m_NoteBook->SetPageText( 0, wxGetApp().getMsg( "node" ) );
  m_NoteBook->SetPageText( 1, wxGetApp().getMsg( "index" ) );
  m_NoteBook->SetPageText( 2, wxGetApp().getMsg( "variables" ) );
  m_NoteBook->SetPageText( 3, wxGetApp().getMsg( "events" ) );
  m_NoteBook->SetPageText( 4, wxGetApp().getMsg( "firmware" ) );

  // Node tab
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labNodeType->SetLabel( wxGetApp().getMsg( "type" ) );
  m_labNumber->SetLabel( wxGetApp().getMsg( "number" ) );
  m_SetNodeNumber->SetLabel( wxGetApp().getMsg( "set" ) );

  // Index tab
  m_IndexDelete->SetLabel( wxGetApp().getMsg( "delete" ) );

  // Variables tab
  m_labVarIndex->SetLabel( wxGetApp().getMsg( "index" ) );
  m_labVarValue->SetLabel( wxGetApp().getMsg( "value" ) );
  m_VarGet->SetLabel( wxGetApp().getMsg( "get" ) );
  m_VarSet->SetLabel( wxGetApp().getMsg( "set" ) );

  // Events tab
  m_labEventNode->SetLabel( wxGetApp().getMsg( "node" ) );
  m_labEventAddr->SetLabel( wxGetApp().getMsg( "address" ) );
  m_labEventIndex->SetLabel( wxGetApp().getMsg( "index" ) );
  m_labEventVar->SetLabel( wxGetApp().getMsg( "variable" ) );
  m_EvtGetVar->SetLabel( wxGetApp().getMsg( "get" ) );
  m_EventGetAll->SetLabel( wxGetApp().getMsg( "getall" ) );
  m_EventAdd->SetLabel( wxGetApp().getMsg( "add" ) );
  m_EventDelete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_EvtClearAll->SetLabel( wxGetApp().getMsg( "clearall" ) );
  m_EvtLearn->SetLabel( wxGetApp().getMsg( "learn" ) );
  m_EvtUnlearn->SetLabel( wxGetApp().getMsg( "unlearn" ) );

  // Firmware tab
  m_HexFile->SetLabel( _T("HEX ") + wxGetApp().getMsg( "file" ) + _T("...") );
  m_HEXFileSend->SetLabel( wxGetApp().getMsg( "send" ) );
  m_BootMode->SetLabel( wxGetApp().getMsg( "bootmode" ) );
  m_ResetBoot->SetLabel( wxGetApp().getMsg( "reset" ) );

  // Buttons
  m_stdButtonOK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_stdButtonCancel->SetLabel( wxGetApp().getMsg( "cancel" ) );

  // Resize
  m_NodeNumberPanel->GetSizer()->Layout();
  m_IndexPanel->GetSizer()->Layout();
  m_VarPanel->GetSizer()->Layout();
  m_EventsPanel->GetSizer()->Layout();
  m_FirmwarePanel->GetSizer()->Layout();
  m_CANGC2Panel->GetSizer()->Layout();

  m_NoteBook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);


  if( event != NULL ) {
    m_IID->SetValue( wxString(wProgram.getiid(event),wxConvUTF8) );
    initType( wProgram.getval1(event),  wProgram.getmodid(event) );
    m_NodeNumber->SetValue(wProgram.getdecaddr(event));
    if( wProgram.getdecaddr(event) > 0 ) {
      getNode(wProgram.getdecaddr(event), wProgram.getmodid(event), wProgram.getval1(event));
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
          char* s = StrOp.fmt("number %d, type %d (%s)",
              wCBusNode.getnr(cbusnode), wCBusNode.getmtyp(cbusnode),
              getTypeString(wCBusNode.getmanuid(cbusnode), wCBusNode.getmtyp(cbusnode)) );

          m_IndexList->Append( wxString(s,wxConvUTF8), cbusnode );
          StrOp.free(s);
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


iONode CBusNodeDlg::getNode(int nr, int mtype, int manu) {
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
        wCBusNode.setmanuid( cbusnode, manu );
        initIndex();
        return cbusnode;
      }
    }
  }
  return NULL;
}


iONode CBusNodeDlg::getNodeVar(int nn, int mtype, int nr, int val) {
  iONode node = getNode(nn, mtype, 0);
  if( node != NULL ) {
    iONode cbusnodevar = wCBusNode.getcbnodevar(node);
    while( cbusnodevar != NULL ) {
      if( wCBusNodeVar.getnr(cbusnodevar) == nr ) {
        wCBusNodeVar.setval(cbusnodevar, val);
        initVarList(node);
        initGC2Var(nr, val);
        return cbusnodevar;
      }
      cbusnodevar = wCBusNode.nextcbnodevar( node, cbusnodevar );
    }
    cbusnodevar = NodeOp.inst( wCBusNodeVar.name(), node, ELEMENT_NODE );
    NodeOp.addChild(node, cbusnodevar);
    wCBusNodeVar.setnr( cbusnodevar, nr );
    wCBusNodeVar.setval( cbusnodevar, val );
    initVarList(node);
    initGC2Var(nr, val);
    return cbusnodevar;
  }
  return NULL;
}


iONode CBusNodeDlg::getNodeEvent(int nn, int mtype, int evnn, int evaddr, int evnr, int evval) {
  TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999,
      "get node event nn=%d, mtype=%d, evnn=%d, evsddr=%d, evnr=%d, evval=%d",
      nn, mtype, evnn, evaddr, evnr, evval );

  iONode node = getNode(nn, mtype, 0);
  if( node != NULL ) {
    iONode cbusnodeevt = wCBusNode.getcbnodeevent(node);
    while( cbusnodeevt != NULL ) {
      if( wCBusNodeEvent.getnodenr(cbusnodeevt) == evnn &&
          wCBusNodeEvent.getaddr(cbusnodeevt) == evaddr &&
          wCBusNodeEvent.getevnr(cbusnodeevt) == evnr )
      {
        wCBusNodeEvent.setevval(cbusnodeevt, evval);
        initEvtList(node);
        initGC2Event(evnr, evnn, evaddr);
        TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999,"update event value to %d", evval);
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
    initGC2Event(evnr, evnn, evaddr);
    return cbusnodeevt;
  }
  return NULL;
}


void CBusNodeDlg::initType( int manu, int mtype ) {
  m_NodeType->SetValue( wxString(getTypeString(manu, mtype),wxConvUTF8) );
  if( mtype > 0 )
    m_NodeTypeNr->SetValue(mtype);
  if( manu > 0 )
    m_NodeManuNr->SetValue(manu);
}

const char* CBusNodeDlg::getTypeString( int manu, int mtype ) {
  if( manu == MANU_ROCRAIL ) {
    switch( mtype ) {
    case MTYP_CANGC2:    return "CANGC2";
    }
  }
  else  {
    // manu == MANU_MERG
    switch( mtype ) {
    case MTYP_CANACC4:   return "CANACC4";
    case MTYP_CANACC5:   return "CANACC5";
    case MTYP_CANACC8:   return "CANACC8";
    case MTYP_CANACE3:   return "CANACE3";
    case MTYP_CANACE8C:  return "CANACE8C";
    case MTYP_CANLED:    return "CANLED";
    case MTYP_CANLED64:  return "CANLED64";
    case MTYP_CANACC4_2: return "CANACC4_2";
    case MTYP_CANCAB:    return "CANCAB";
    case MTYP_CANCMD:    return "CANCMD";
    case MTYP_CANSERVO:  return "CANSERVO";
    case MTYP_CANBC:     return "CANBC";
    case MTYP_CANRPI:    return "CANRPI";
    case MTYP_CANTTCA:   return "CANTTCA";
    case MTYP_CANTTCB:   return "CANTTCB";
    }
  }
  TraceOp.trc( "cbusnodedlg", TRCLEVEL_WARNING, __LINE__, 9999, "unknown type: manu=%d, mtype=%d ", manu, mtype );
  return "UNKNOWN";
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

  getNode(nn, m_NodeTypeNr->GetValue(), m_NodeManuNr->GetValue());
}

void CBusNodeDlg::onIndexSelect( wxCommandEvent& event ) {
  if( m_IndexList->GetSelection() != wxNOT_FOUND ) {
    iONode node = (iONode)m_IndexList->GetClientData(m_IndexList->GetSelection());
    if( node != NULL ) {
      initType(wCBusNode.getmanuid(node), wCBusNode.getmtyp(node));
      m_NodeNumber->SetValue(wCBusNode.getnr(node));
      initVarList(node);
      initEvtList(node);
      SetTitle(_T("CBUS: ") + m_NodeType->GetValue() );
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
      m_NodeManuNr->SetValue(0);
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
  varGet( m_VarIndex->GetValue() );
}

void CBusNodeDlg::varGet( int var ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.get );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wProgram.setcv( cmd, var );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void CBusNodeDlg::onVarSet( wxCommandEvent& event ) {
  varSet( m_VarIndex->GetValue(), m_VarValue->GetValue(), true );
}

void CBusNodeDlg::varSet( int idx, int val, bool update ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.set );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wProgram.setcv( cmd, idx );
  wProgram.setvalue( cmd, val );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
  if( update )
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
  eventGetAll();
}
void CBusNodeDlg::eventGetAll() {
  int nn = m_NodeNumber->GetValue();

  iONode node = getNode(nn, m_NodeTypeNr->GetValue(), m_NodeManuNr->GetValue());
  if( node != NULL ) {
    iONode cbusnodeevt = wCBusNode.getcbnodeevent(node);
    while( cbusnodeevt != NULL ) {
      TraceOp.trc( "cbusnode", TRCLEVEL_INFO, __LINE__, 9999, "delete event for node [%d:%d]", nn, m_NodeTypeNr->GetValue());
      NodeOp.removeChild(node, cbusnodeevt);
      NodeOp.base.del(cbusnodeevt);
      cbusnodeevt = wCBusNode.getcbnodeevent(node);
    }
    initEvtList(node);
  }
  else {
    TraceOp.trc( "cbusnode", TRCLEVEL_WARNING, __LINE__, 9999, "node not found [%d:%d]", nn, m_NodeTypeNr->GetValue());
  }

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
  eventSet(m_EventNodeNr->GetValue(), m_EventAddress->GetValue(),
      m_EventIndex->GetValue(), m_EventVar->GetValue(), true );
}
void CBusNodeDlg::eventSet( int nn, int addr, int idx, int val, bool update ) {
  int nodenr = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.evset );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nodenr );
  wProgram.setval2(cmd, nn); // nn
  wProgram.setval3(cmd, addr); // addr
  wProgram.setval1(cmd, idx ); // idx
  wProgram.setval4(cmd, val ); // val
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
  if( update ) {
    getNodeEvent(nn, m_NodeTypeNr->GetValue(), m_EventNodeNr->GetValue(),
        m_EventAddress->GetValue(), m_EventIndex->GetValue(), m_EventVar->GetValue() );
  }
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

  iONode node = getNode(nn, m_NodeTypeNr->GetValue(), m_NodeManuNr->GetValue());
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
  setLearn();
}
void CBusNodeDlg::setLearn() {
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
  setUnlearn();
}
void CBusNodeDlg::setUnlearn() {
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

    iONode nv = getNodeVar(nn, m_NodeTypeNr->GetValue(), cv, val );
    if( m_bGC2GetAll ) {
      if( cv < 17 ) {
        varGet(cv+1);
      }
      else {
        m_bGC2GetAll = false;
        eventGetAll();
      }
    }
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
  iONode node = getNode(nn, m_NodeTypeNr->GetValue(), m_NodeManuNr->GetValue());
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


void CBusNodeDlg::onHexFile( wxCommandEvent& event ) {
  wxString ms_FileExt = _T("PIC HEX (*.HEX;*.hex)|*.HEX;*.hex");
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("openhexfile"),
      wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    //fdlg->GetPath();
    wGui.setopenpath( wxGetApp().getIni(), fdlg->GetPath().mb_str(wxConvUTF8) );
    // strip filename:
    wGui.setopenpath( wxGetApp().getIni(), FileOp.getPath(wGui.getopenpath( wxGetApp().getIni() ) ) );

    TraceOp.trc( "cbusnode", TRCLEVEL_INFO, __LINE__, 9999, "reading [%s]...", (const char*)fdlg->GetPath().mb_str(wxConvUTF8));
    iOFile f = FileOp.inst( fdlg->GetPath().mb_str(wxConvUTF8), OPEN_READONLY );
    if( f != NULL ) {
      TraceOp.trc( "cbusnode", TRCLEVEL_INFO, __LINE__, 9999, "file opened...");
      m_HEXFileName->SetValue(fdlg->GetPath());
      FILE* fs = FileOp.getStream(f);
      char str[256];
      fgets( str, 256, fs );

      /* until end of stream or error: */
      while( !ferror(fs) && !feof(fs) ) {
        TraceOp.trc( "cbusnode", TRCLEVEL_INFO, __LINE__, 9999, "line=[%s]", str);
        m_HEXFileText->AppendText(wxString(str,wxConvUTF8));
        fgets( str, 256, fs );
      };

      FileOp.base.del( f );
    }
  }
  fdlg->Destroy();
}


void CBusNodeDlg::onHEXFileSend( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.writehex );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wProgram.setfilename( cmd,  m_HEXFileName->GetValue().mb_str(wxConvUTF8) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void CBusNodeDlg::onBootmode( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.bootmode );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void CBusNodeDlg::onResetBoot( wxCommandEvent& event ) {
  int nn = m_NodeNumber->GetValue();
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.reset );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_cbus);
  wProgram.setdecaddr( cmd, nn );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void CBusNodeDlg::gc2GetPort(int port, int* conf, int* nn, int* addr) {
  wxSpinCtrl* gc2NN[] = {NULL,m_GC2EvtNN1,m_GC2EvtNN2,m_GC2EvtNN3,m_GC2EvtNN4,m_GC2EvtNN5
      ,m_GC2EvtNN6,m_GC2EvtNN7,m_GC2EvtNN8,m_GC2EvtNN9,m_GC2EvtNN10
      ,m_GC2EvtNN11,m_GC2EvtNN12,m_GC2EvtNN13,m_GC2EvtNN14,m_GC2EvtNN15,m_GC2EvtNN16};

  wxSpinCtrl* gc2Addr[] = {NULL,m_GC2EvtAddr1,m_GC2EvtAddr2,m_GC2EvtAddr3,m_GC2EvtAddr4,m_GC2EvtAddr5
      ,m_GC2EvtAddr6,m_GC2EvtAddr7,m_GC2EvtAddr8,m_GC2EvtAddr9,m_GC2EvtAddr10
      ,m_GC2EvtAddr11,m_GC2EvtAddr12,m_GC2EvtAddr13,m_GC2EvtAddr14,m_GC2EvtAddr15,m_GC2EvtAddr16};

  wxRadioButton* gc2Input[] = {NULL,m_GC2Input1,m_GC2Input2,m_GC2Input3,m_GC2Input4,m_GC2Input5
      ,m_GC2Input6,m_GC2Input7,m_GC2Input8,m_GC2Input9,m_GC2Input10
      ,m_GC2Input11,m_GC2Input12,m_GC2Input13,m_GC2Input14,m_GC2Input15,m_GC2Input16};

  wxRadioButton* gc2Block[] = {NULL,m_GC2Block1,m_GC2Block2,m_GC2Block3,m_GC2Block4,m_GC2Block5
      ,m_GC2Block6,m_GC2Block7,m_GC2Block8,m_GC2Block9,m_GC2Block10
      ,m_GC2Block11,m_GC2Block12,m_GC2Block13,m_GC2Block14,m_GC2Block15,m_GC2Block16};

  wxRadioButton* gc2Switch[] = {NULL,m_GC2Switch1,m_GC2Switch2,m_GC2Switch3,m_GC2Switch4,m_GC2Switch5
      ,m_GC2Switch6,m_GC2Switch7,m_GC2Switch8,m_GC2Switch9,m_GC2Switch10
      ,m_GC2Switch11,m_GC2Switch12,m_GC2Switch13,m_GC2Switch14,m_GC2Switch15,m_GC2Switch16};

  wxRadioButton* gc2Pulse[] = {NULL,m_GC2Pulse1,m_GC2Pulse2,m_GC2Pulse3,m_GC2Pulse4,m_GC2Pulse5
      ,m_GC2Pulse6,m_GC2Pulse7,m_GC2Pulse8,m_GC2Pulse9,m_GC2Pulse10
      ,m_GC2Pulse11,m_GC2Pulse12,m_GC2Pulse13,m_GC2Pulse14,m_GC2Pulse15,m_GC2Pulse16};

  *nn   = gc2NN[port]->GetValue();
  *addr = gc2Addr[port]->GetValue();

  int input  = gc2Input[port]->GetValue() | gc2Block[port]->GetValue();
  int delay  = gc2Block[port]->GetValue();
  int output = gc2Switch[port]->GetValue() | gc2Pulse[port]->GetValue();
  int pulse  = gc2Pulse[port]->GetValue();

  *conf = input | (delay << 1) | (pulse << 1);

}


void CBusNodeDlg::gc2SetPort(int port, int conf, int nn, int addr) {
  wxSpinCtrl* gc2NN[] = {NULL,m_GC2EvtNN1,m_GC2EvtNN2,m_GC2EvtNN3,m_GC2EvtNN4,m_GC2EvtNN5
      ,m_GC2EvtNN6,m_GC2EvtNN7,m_GC2EvtNN8,m_GC2EvtNN9,m_GC2EvtNN10
      ,m_GC2EvtNN11,m_GC2EvtNN12,m_GC2EvtNN13,m_GC2EvtNN14,m_GC2EvtNN15,m_GC2EvtNN16};

  wxSpinCtrl* gc2Addr[] = {NULL,m_GC2EvtAddr1,m_GC2EvtAddr2,m_GC2EvtAddr3,m_GC2EvtAddr4,m_GC2EvtAddr5
      ,m_GC2EvtAddr6,m_GC2EvtAddr7,m_GC2EvtAddr8,m_GC2EvtAddr9,m_GC2EvtAddr10
      ,m_GC2EvtAddr11,m_GC2EvtAddr12,m_GC2EvtAddr13,m_GC2EvtAddr14,m_GC2EvtAddr15,m_GC2EvtAddr16};

  wxRadioButton* gc2Input[] = {NULL,m_GC2Input1,m_GC2Input2,m_GC2Input3,m_GC2Input4,m_GC2Input5
      ,m_GC2Input6,m_GC2Input7,m_GC2Input8,m_GC2Input9,m_GC2Input10
      ,m_GC2Input11,m_GC2Input12,m_GC2Input13,m_GC2Input14,m_GC2Input15,m_GC2Input16};

  wxRadioButton* gc2Block[] = {NULL,m_GC2Block1,m_GC2Block2,m_GC2Block3,m_GC2Block4,m_GC2Block5
      ,m_GC2Block6,m_GC2Block7,m_GC2Block8,m_GC2Block9,m_GC2Block10
      ,m_GC2Block11,m_GC2Block12,m_GC2Block13,m_GC2Block14,m_GC2Block15,m_GC2Block16};

  wxRadioButton* gc2Switch[] = {NULL,m_GC2Switch1,m_GC2Switch2,m_GC2Switch3,m_GC2Switch4,m_GC2Switch5
      ,m_GC2Switch6,m_GC2Switch7,m_GC2Switch8,m_GC2Switch9,m_GC2Switch10
      ,m_GC2Switch11,m_GC2Switch12,m_GC2Switch13,m_GC2Switch14,m_GC2Switch15,m_GC2Switch16};

  wxRadioButton* gc2Pulse[] = {NULL,m_GC2Pulse1,m_GC2Pulse2,m_GC2Pulse3,m_GC2Pulse4,m_GC2Pulse5
      ,m_GC2Pulse6,m_GC2Pulse7,m_GC2Pulse8,m_GC2Pulse9,m_GC2Pulse10
      ,m_GC2Pulse11,m_GC2Pulse12,m_GC2Pulse13,m_GC2Pulse14,m_GC2Pulse15,m_GC2Pulse16};

  if( nn != -1 )
    gc2NN[port]->SetValue(nn);
  if( addr != -1 )
    gc2Addr[port]->SetValue(addr);

  if( conf != -1 ) {
    int input  = (conf & 0x01) ? 1:0;
    int delay  = (conf & 0x02) ? 1:0;

    if( input && delay )
      gc2Block[port]->SetValue(true);
    else if( input && !delay )
      gc2Input[port]->SetValue(true);
    else if( delay )
      gc2Pulse[port]->SetValue(true);
    else
      gc2Switch[port]->SetValue(true);
  }

}

void CBusNodeDlg::initGC2Var( int nr, int val ) {
  if( nr == 1 ) {
    // node var1
    m_GC2SaveOutput->SetValue( (val&0x01) ? true:false );
    m_GC2ShortEvents->SetValue( (val&0x02) ? true:false );
  }
  else if( nr < 18 ) {
    // port config
    gc2SetPort(nr-1, val, -1, -1);
  }
}

void CBusNodeDlg::initGC2Event( int idx, int nn, int addr ) {
  if( idx < 16 ) {
    // port event
    gc2SetPort(idx+1, -1, nn, addr);
  }
  else if( idx == 16 ) {
    // SOD
    m_GC2SOD->SetValue( addr );
  }
}

void CBusNodeDlg::onGC2GetAll( wxCommandEvent& event ) {
  m_bGC2GetAll = true;
  varGet(1);
}


void CBusNodeDlg::onGC2SetAll( wxCommandEvent& event ) {
  m_bGC2SetAll = true;
  m_GC2SetAll->Enable(false);
  m_GC2SetIndex = 0;
  m_Timer->Start( 100, wxTIMER_ONE_SHOT );
}


void CBusNodeDlg::OnTimer(wxTimerEvent& event) {
  if( m_GC2SetIndex == 0 ) {
    int nv1 = m_GC2SaveOutput->IsChecked() ? 0x01:0x00;
    nv1 += m_GC2ShortEvents->IsChecked() ? 0x02:0x00;
    TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "nv1=0x%02X", nv1);
    varSet(1, nv1, false);
  }
  else if( m_GC2SetIndex == 1 ) {
    TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "set learn mode");
    setLearn();
  }
  else if( m_GC2SetIndex < 18 ) {
    int conf = 0;
    int nn = 0;
    int addr = 0;
    gc2GetPort(m_GC2SetIndex-1, &conf, &nn, &addr);
    TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999,
        "nv%d=0x%02X nn=%d addr=%d", m_GC2SetIndex, conf, nn, addr);
    varSet(m_GC2SetIndex, conf, false);
    eventSet( nn, addr, m_GC2SetIndex-2, 0, false );
  }
  else if( m_GC2SetIndex == 18 ) {
    eventSet( 0, m_GC2SOD->GetValue(), 18, 0, false );
  }
  m_GC2SetIndex++;
  if( m_GC2SetIndex < 19 ) {
    m_Timer->Start( 100, wxTIMER_ONE_SHOT );
  }
  else {
    m_bGC2SetAll = false;
    m_GC2SetAll->Enable(true);
    setUnlearn();
  }
}




