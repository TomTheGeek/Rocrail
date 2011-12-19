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
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/CBus.h"
#include "rocrail/wrapper/public/CBusNode.h"
#include "rocrail/wrapper/public/CBusNodeVar.h"
#include "rocrail/wrapper/public/CBusNodeEvent.h"

#include "rocs/public/trace.h"
#include "rocdigs/impl/cbus/cbusdefs.h"
#include "rocdigs/impl/cbus/rocrail.h"

#define ME_GC2Timer 4711

CBusNodeDlg::CBusNodeDlg( wxWindow* parent ):cbusnodedlggen( parent )
{
  init(NULL);
}

CBusNodeDlg::CBusNodeDlg( wxWindow* parent, iONode event ):cbusnodedlggen( parent )
{
  m_bGC2GetAll = false;
  m_bGC2SetAll = false;
  m_bGC7GetAll = false;
  m_bGC6GetAll = false;
  m_bGC6SetAll = false;
  m_bGC2SetAll = true;
  m_GC2SetIndex = 0;
  m_GC6SetIndex = 0;
  init(event);
}

void CBusNodeDlg::init( iONode event ) {
  MemOp.set( m_Ports, sizeof(Port), 0 );
  m_SOD = 0;
  m_SaveOutputState = false;
  m_ShortEvents = false;
  m_SODAll = false;
  m_PulseTime = 0;
  m_GC2IgnorePortTest = 0;
  m_CANID = 0;



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
  m_labEventAddr->SetLabel( wxGetApp().getMsg( "event" ) );
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

  m_FirmwarePanel->Enable(false);

  if( event != NULL ) {
    char ver[32] = {'0'};
    StrOp.fmtb( ver, "%d.%c", wProgram.getval7(event), wProgram.getval2(event));
    TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999,"version=%s", ver);

    m_IID->SetValue( wxString(wProgram.getiid(event),wxConvUTF8) );
    initType( wProgram.getval1(event),  wProgram.getmodid(event), ver );
    m_NodeNumber->SetValue(wProgram.getdecaddr(event));
    if( wProgram.getdecaddr(event) > 0 ) {
      getNode(wProgram.getdecaddr(event), wProgram.getmodid(event), wProgram.getval1(event), ver);
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
          wxString::Format(_T("node %d, event %d, index %d, value %d"),
              wCBusNodeEvent.getnodenr(evt), wCBusNodeEvent.getaddr(evt),
              wCBusNodeEvent.getevnr(evt), wCBusNodeEvent.getevval(evt) ),
          evt );
      evt = wCBusNode.nextcbnodeevent( node, evt );
    }
  }
}


iONode CBusNodeDlg::getNode(int nr, int mtype, int manu, const char* ver) {
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
        if( manu > 0 )
          wCBusNode.setmanuid( cbusnode, manu );
        if( ver != NULL ) {
          wCBusNode.setversion( cbusnode, ver );
        }
        initIndex();
        return cbusnode;
      }
    }
  }
  return NULL;
}


iONode CBusNodeDlg::getNodeVar(int nn, int mtype, int nr, int val) {
  iONode node = getNode(nn, mtype, 0, NULL);
  if( node != NULL ) {
    iONode cbusnodevar = wCBusNode.getcbnodevar(node);
    while( cbusnodevar != NULL ) {
      if( wCBusNodeVar.getnr(cbusnodevar) == nr ) {
        wCBusNodeVar.setval(cbusnodevar, val);
        initVarList(node);
        initGC2Var(nr, val);
        initGC6Var(nr, val);
        initGC7Var(nr, val);
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
    initGC6Var(nr, val);
    initGC7Var(nr, val);
    return cbusnodevar;
  }
  return NULL;
}


iONode CBusNodeDlg::getNodeEvent(int nn, int mtype, int evnn, int evaddr, int evnr, int evval) {
  TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999,
      "get node event nn=%d, mtype=%d, evnn=%d, evsddr=%d, evnr=%d, evval=%d",
      nn, mtype, evnn, evaddr, evnr, evval );

  iONode node = getNode(nn, mtype, 0, NULL);
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
        initGC6Event(evnr, evnn, evaddr);
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
    initGC6Event(evnr, evnn, evaddr);
    return cbusnodeevt;
  }
  return NULL;
}


void CBusNodeDlg::initType( int manu, int mtype, const char* ver ) {
  m_NodeType->SetValue( wxString(getTypeString(manu, mtype),wxConvUTF8) );
  if( mtype > 0 )
    m_NodeTypeNr->SetValue(mtype);
  if( manu > 0 )
    m_NodeManuNr->SetValue(manu);
  if( ver != NULL && StrOp.len(ver) > 0 ) {
    TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999,"version=%s", ver);
    m_Version->SetValue(wxString(ver,wxConvUTF8));
  }
}

const char* CBusNodeDlg::getTypeString( int manu, int mtype ) {
  if( manu == MANU_ROCRAIL ) {
    switch( mtype ) {
    case MTYP_CANGC2:    return "ROCRAIL::CAN-GC2";
    case MTYP_CANGC6:    return "ROCRAIL::CAN-GC6";
    case MTYP_CANGC7:    return "ROCRAIL::CAN-GC7";
    }
  }
  else  {
    // manu == MANU_MERG
    switch( mtype ) {
    case MTYP_CANACC4:   return "MERG::CANACC4";
    case MTYP_CANACC5:   return "MERG::CANACC5";
    case MTYP_CANACC8:   return "MERG::CANACC8";
    case MTYP_CANACE3:   return "MERG::CANACE3";
    case MTYP_CANACE8C:  return "MERG::CANACE8C";
    case MTYP_CANLED:    return "MERG::CANLED";
    case MTYP_CANLED64:  return "MERG::CANLED64";
    case MTYP_CANACC4_2: return "MERG::CANACC4_2";
    case MTYP_CANCAB:    return "MERG::CANCAB";
    case MTYP_CANCMD:    return "MERG::CANCMD";
    case MTYP_CANSERVO:  return "MERG::CANSERVO";
    case MTYP_CANBC:     return "MERG::CANBC";
    case MTYP_CANRPI:    return "MERG::CANRPI";
    case MTYP_CANTTCA:   return "MERG::CANTTCA";
    case MTYP_CANTTCB:   return "MERG::CANTTCB";
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

  getNode(nn, m_NodeTypeNr->GetValue(), m_NodeManuNr->GetValue(), m_Version->GetValue().mb_str(wxConvUTF8) );
}

void CBusNodeDlg::onIndexSelect( wxCommandEvent& event ) {
  if( m_IndexList->GetSelection() != wxNOT_FOUND ) {
    iONode node = (iONode)m_IndexList->GetClientData(m_IndexList->GetSelection());
    if( node != NULL ) {
      initType(wCBusNode.getmanuid(node), wCBusNode.getmtyp(node), wCBusNode.getversion(node));
      m_NodeNumber->SetValue(wCBusNode.getnr(node));
      initVarList(node);
      initEvtList(node);
      char* title = StrOp.fmt("CBUS: %s %d",
          (const char*)m_NodeType->GetValue().mb_str(wxConvUTF8),
          wCBusNode.getnr(node) );
      SetTitle( wxString(title,wxConvUTF8) );
      StrOp.free(title);
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
      if( l_RocrailIni != NULL ) {
        iONode digint = wRocRail.getdigint(l_RocrailIni);
        iONode cbus = wDigInt.getcbus(digint);
        NodeOp.removeChild( cbus, node );
        m_NodeNumber->SetValue(0);
        m_NodeType->SetValue(_T(""));
        m_NodeTypeNr->SetValue(0);
        m_NodeManuNr->SetValue(0);
        initIndex();
        SetTitle( _T("CBUS") );
      }
    }
    else
      TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}

void CBusNodeDlg::onQuery( wxCommandEvent& event ) {
  iONode l_RocrailIni = wxGetApp().getFrame()->getRocrailIni();
  if( l_RocrailIni != NULL ) {
    iONode digint = wRocRail.getdigint(l_RocrailIni);
    iONode cbus = wDigInt.getcbus(digint);
    iONode node = wCBus.getcbnode(cbus);
    while( node != NULL ) {
      NodeOp.removeChild( cbus, node );
      node = wCBus.getcbnode(cbus);
    }
    initIndex();

    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setcmd( cmd, wProgram.query );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_cbus);
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void CBusNodeDlg::onVarSelect( wxCommandEvent& event ) {
  if( m_VarList->GetSelection() != wxNOT_FOUND ) {
    iONode var = (iONode)m_VarList->GetClientData(m_VarList->GetSelection());
    if( var != NULL ) {
      m_VarIndex->SetValue(wCBusNodeVar.getnr(var));
      m_VarValue->SetValue(wCBusNodeVar.getval(var));
      wxSpinEvent evt( 0, 0 );
      onVarValue(evt);
    }
    else
      TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}


void CBusNodeDlg::onVarValueText( wxCommandEvent& event ) {
  wxSpinEvent evt(0,0);
  onVarValue(evt);
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

void CBusNodeDlg::onEventSelect( wxCommandEvent& cmdevent ) {
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
      wxSpinEvent evt( 0, 0 );
      onEV(evt);
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

  iONode node = getNode(nn, m_NodeTypeNr->GetValue(), m_NodeManuNr->GetValue(), m_Version->GetValue().mb_str(wxConvUTF8));
  if( node != NULL ) {
    iONode cbusnodeevt = wCBusNode.getcbnodeevent(node);
    TraceOp.trc( "cbusnode", TRCLEVEL_INFO, __LINE__, 9999, "delete all events for node [%d:%d]", nn, m_NodeTypeNr->GetValue());
    while( cbusnodeevt != NULL ) {
      TraceOp.trc( "cbusnode", TRCLEVEL_INFO, __LINE__, 9999,
          "delete event for node [%d:%d] [addr=%d, evnr=%d]", nn, m_NodeTypeNr->GetValue(),
          wCBusNodeEvent.getaddr(cbusnodeevt), wCBusNodeEvent.getevnr(cbusnodeevt));

      if( NodeOp.removeChild(node, cbusnodeevt) != NULL ) {
        NodeOp.base.del(cbusnodeevt);
        cbusnodeevt = wCBusNode.getcbnodeevent(node);
        // ToDo: Work around!? Bug in the Node object?
        if( cbusnodeevt != wCBusNode.getcbnodeevent(node) )
          cbusnodeevt = wCBusNode.getcbnodeevent(node);
        else
          cbusnodeevt = NULL;
      }
      else {
        cbusnodeevt = NULL;
      }
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

  iONode node = getNode(nn, m_NodeTypeNr->GetValue(), m_NodeManuNr->GetValue(), m_Version->GetValue().mb_str(wxConvUTF8));
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

void CBusNodeDlg::onEVText( wxCommandEvent& event ) {
  wxSpinEvent evt(0,0);
  onEV(evt);
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
  TraceOp.trc( "cbusnode", TRCLEVEL_INFO, __LINE__, 9999, "event cmd=%d", wProgram.getcmd( event ));
  if( wProgram.getcmd( event ) == wProgram.nnreq  ) {
    init( event );
  }
  else if( wProgram.getcmd( event ) == wProgram.type  ) {
    int nn = wProgram.getdecaddr(event);
    int manu = wProgram.getmanu(event);
    int prod = wProgram.getprod(event);
    getNode(nn, prod, manu, NULL);
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
    TraceOp.trc( "cbusnode", TRCLEVEL_INFO, __LINE__, 9999, "node variable %d, val=%d", cv, val);

    iONode nv = getNodeVar(nn, m_NodeTypeNr->GetValue(), cv, val );
    if( m_bGC2GetAll ) {
      if( cv < 20 ) {
        varGet(cv+1);
      }
      else {
        m_bGC2GetAll = false;
        eventGetAll();
      }
    }
    if( m_bGC6GetAll ) {
      if( cv < 18 ) {
        varGet(cv+1);
      }
      else {
        m_bGC6GetAll = false;
        eventGetAll();
      }
    }
    if( m_bGC7GetAll ) {
      if( cv < 2 ) {
        varGet(cv+1);
      }
      else {
        m_bGC7GetAll = false;
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
  iONode node = getNode(nn, m_NodeTypeNr->GetValue(), m_NodeManuNr->GetValue(), m_Version->GetValue().mb_str(wxConvUTF8));
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

  wxRadioButton* gc2IR[] = {NULL,m_GC2IR1,m_GC2IR2,m_GC2IR3,m_GC2IR4
      ,m_GC2IR5,m_GC2IR6,m_GC2IR7,m_GC2IR8};

  wxRadioButton* gc2Block[] = {NULL,m_GC2Block1,m_GC2Block2,m_GC2Block3,m_GC2Block4,m_GC2Block5
      ,m_GC2Block6,m_GC2Block7,m_GC2Block8,m_GC2Block9,m_GC2Block10
      ,m_GC2Block11,m_GC2Block12,m_GC2Block13,m_GC2Block14,m_GC2Block15,m_GC2Block16};

  wxRadioButton* gc2Switch[] = {NULL,m_GC2Switch1,m_GC2Switch2,m_GC2Switch3,m_GC2Switch4,m_GC2Switch5
      ,m_GC2Switch6,m_GC2Switch7,m_GC2Switch8,m_GC2Switch9,m_GC2Switch10
      ,m_GC2Switch11,m_GC2Switch12,m_GC2Switch13,m_GC2Switch14,m_GC2Switch15,m_GC2Switch16};

  wxRadioButton* gc2Pulse[] = {NULL,m_GC2Pulse1,m_GC2Pulse2,m_GC2Pulse3,m_GC2Pulse4,m_GC2Pulse5
      ,m_GC2Pulse6,m_GC2Pulse7,m_GC2Pulse8,m_GC2Pulse9,m_GC2Pulse10
      ,m_GC2Pulse11,m_GC2Pulse12,m_GC2Pulse13,m_GC2Pulse14,m_GC2Pulse15,m_GC2Pulse16};

  wxCheckBox* gc2C2[] = {NULL,m_GC2c21,m_GC2c22,m_GC2c23,m_GC2c24,m_GC2c25
      ,m_GC2c26,m_GC2c27,m_GC2c28,m_GC2c29,m_GC2c210
      ,m_GC2c211,m_GC2c212,m_GC2c213,m_GC2c214,m_GC2c215,m_GC2c216};

  *nn   = gc2NN[port]->GetValue();
  *addr = gc2Addr[port]->GetValue();

  int input  = gc2Input[port]->GetValue() | gc2Block[port]->GetValue();
  int delay  = gc2Block[port]->GetValue();
  int output = gc2Switch[port]->GetValue() | gc2Pulse[port]->GetValue();
  int pulse  = gc2Pulse[port]->GetValue();
  int c2     = gc2C2[port]->IsChecked() ? 1:0;
  *conf = input | (delay << 1) | (pulse << 1) | (c2 << 2);

  if( port < 9 ) {
    int ir  = gc2IR[port]->GetValue();
    *conf = (delay << 1) | (pulse << 1) | (c2 << 2) | (ir << 3);
  }
  else {
    *conf = input | (delay << 1) | (pulse << 1) | (c2 << 2);
  }

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

  wxRadioButton* gc2IR[] = {NULL,m_GC2IR1,m_GC2IR2,m_GC2IR3,m_GC2IR4
      ,m_GC2IR5,m_GC2IR6,m_GC2IR7,m_GC2IR8};


  wxRadioButton* gc2Block[] = {NULL,m_GC2Block1,m_GC2Block2,m_GC2Block3,m_GC2Block4,m_GC2Block5
      ,m_GC2Block6,m_GC2Block7,m_GC2Block8,m_GC2Block9,m_GC2Block10
      ,m_GC2Block11,m_GC2Block12,m_GC2Block13,m_GC2Block14,m_GC2Block15,m_GC2Block16};

  wxRadioButton* gc2Switch[] = {NULL,m_GC2Switch1,m_GC2Switch2,m_GC2Switch3,m_GC2Switch4,m_GC2Switch5
      ,m_GC2Switch6,m_GC2Switch7,m_GC2Switch8,m_GC2Switch9,m_GC2Switch10
      ,m_GC2Switch11,m_GC2Switch12,m_GC2Switch13,m_GC2Switch14,m_GC2Switch15,m_GC2Switch16};

  wxRadioButton* gc2Pulse[] = {NULL,m_GC2Pulse1,m_GC2Pulse2,m_GC2Pulse3,m_GC2Pulse4,m_GC2Pulse5
      ,m_GC2Pulse6,m_GC2Pulse7,m_GC2Pulse8,m_GC2Pulse9,m_GC2Pulse10
      ,m_GC2Pulse11,m_GC2Pulse12,m_GC2Pulse13,m_GC2Pulse14,m_GC2Pulse15,m_GC2Pulse16};

  wxCheckBox* gc2C2[] = {NULL,m_GC2c21,m_GC2c22,m_GC2c23,m_GC2c24,m_GC2c25
      ,m_GC2c26,m_GC2c27,m_GC2c28,m_GC2c29,m_GC2c210
      ,m_GC2c211,m_GC2c212,m_GC2c213,m_GC2c214,m_GC2c215,m_GC2c216};


  if( nn != -1 )
    gc2NN[port]->SetValue(nn);
  if( addr != -1 )
    gc2Addr[port]->SetValue(addr);

  if( conf != -1 ) {
    int input  = (conf & 0x01) ? 1:0;
    int delay  = (conf & 0x02) ? 1:0;
    int ir     = (conf & 0x08) ? 1:0;

    if( ir && port < 9 )
      gc2IR[port]->SetValue(true);
    else if( input && delay )
      gc2Block[port]->SetValue(true);
    else if( input && !delay )
      gc2Input[port]->SetValue(true);
    else if( delay )
      gc2Pulse[port]->SetValue(true);
    else
      gc2Switch[port]->SetValue(true);

    gc2C2[port]->SetValue((conf & 0x04) ? true:false);
  }

}

void CBusNodeDlg::gc6SetServoEvent(int idx, int nn, int addr) {
  wxSpinCtrl* gc6SwNN[] = {NULL,m_GC6Servo1SwNN, m_GC6Servo2SwNN, m_GC6Servo3SwNN, m_GC6Servo4SwNN};
  wxSpinCtrl* gc6SwAddr[] = {NULL,m_GC6Servo1SwEvent, m_GC6Servo2SwEvent, m_GC6Servo3SwEvent, m_GC6Servo4SwEvent};
  wxSpinCtrl* gc6FbAddr[] = {NULL,m_GC6Servo1FbEvent, m_GC6Servo2FbEvent, m_GC6Servo3FbEvent, m_GC6Servo4FbEvent};

  if( idx < 4 ) {
    m_Servo[idx].swnn = nn;
    m_Servo[idx].swaddr = addr;
    gc6SwNN[idx+1]->SetValue(nn);
    gc6SwAddr[idx+1]->SetValue(addr);
  }
  else if( idx < 8 ) {
    m_Servo[idx-4].fbaddr = addr;
    gc6FbAddr[(idx-4)+1]->SetValue(addr);
  }

}


void CBusNodeDlg::gc6SetServoConf(int servo, int idx, int val) {
  wxCheckBox* gc6Relay[] = {m_GC6Servo1Relay,m_GC6Servo2Relay,m_GC6Servo3Relay,m_GC6Servo4Relay};
  wxCheckBox* gc6ExtFb[] = {m_GC6Servo1ExtFb,m_GC6Servo2ExtFb,m_GC6Servo3ExtFb,m_GC6Servo4ExtFb};

  wxSlider* gc6Left [] = {m_GC6Servo1LeftAng ,m_GC6Servo2LeftAng ,m_GC6Servo3LeftAng ,m_GC6Servo4LeftAng};
  wxSlider* gc6Right[] = {m_GC6Servo1RightAng,m_GC6Servo2RightAng,m_GC6Servo3RightAng,m_GC6Servo4RightAng};
  wxSlider* gc6Speed[] = {m_GC6Servo1Speed   ,m_GC6Servo2Speed   ,m_GC6Servo3Speed   ,m_GC6Servo4Speed};

  if( idx % 4 == 0 ) {
    m_Servo[servo].cfg = val;
    gc6Relay[servo]->SetValue(val&0x01?true:false);
    gc6ExtFb[servo]->SetValue(val&0x02?true:false);
  }
  if( idx % 4 == 1 ) {
    m_Servo[servo].left = val;
    gc6Left[servo]->SetValue(val);
  }
  if( idx % 4 == 2 ) {
    m_Servo[servo].right = val;
    gc6Right[servo]->SetValue(val);
  }
  if( idx % 4 == 3 ) {
    m_Servo[servo].speed = val;
    gc6Speed[servo]->SetValue(val);
  }


}

void CBusNodeDlg::initGC2Var( int nr, int val ) {
  wxButton* gc2Test[] = {NULL,m_GC2Test1,m_GC2Test2,m_GC2Test3,m_GC2Test4,m_GC2Test5
      ,m_GC2Test6,m_GC2Test7,m_GC2Test8,m_GC2Test9,m_GC2Test10
      ,m_GC2Test11,m_GC2Test12,m_GC2Test13,m_GC2Test14,m_GC2Test15,m_GC2Test16};

  if( nr == 1 ) {
    // node var1
    m_GC2SaveOutput->SetValue( (val&0x01) ? true:false );
    m_GC2ShortEvents->SetValue( (val&0x02) ? true:false );
    m_GC2SODAll->SetValue( (val&0x10) ? true:false );
    m_SaveOutputState = (val&0x01) ? true:false;
    m_ShortEvents = (val&0x02) ? true:false;
    m_SODAll = (val&0x10) ? true:false;
    int pulse = (val&0x0C) >> 2;
    m_GC2PulseTime->SetSelection(pulse);
    m_PulseTime = pulse;
  }
  else if( nr < 18 ) {
    // port config
    gc2SetPort(nr-1, val, -1, -1);
    m_Ports[nr-2].cfg = val;

  }
  else if( nr == 18 ) {
    // port status 1-8
    for( int i = 0; i < 8; i++) {
      int on = val & (0x01 << i);
      if( !(m_GC2IgnorePortTest == (i+1)) )
        gc2Test[i+1]->SetLabel(on > 0 ? _T("1"):_T("0"));
      else
        m_GC2IgnorePortTest = 0;
    }
    TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "port state group 1: 0x%02X", val);
  }
  else if( nr == 19 ) {
    // port status 9-16
    for( int i = 0; i < 8; i++) {
      int on = val & (0x01 << i);
      if( !(m_GC2IgnorePortTest == (i+1+8)) )
        gc2Test[i+1+8]->SetLabel(on > 0 ? _T("1"):_T("0"));
      else
        m_GC2IgnorePortTest = 0;
    }
    TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "port state group 2: 0x%02X", val);
  }
  else if( nr == 20 ) {
    m_CANID = val;
    m_GC2CanID->SetValue(val);
  }
}

void CBusNodeDlg::initGC6Var( int nr, int val ) {
  if( nr == 1 ) {
    // node var1
    m_GC6SaveServoPos->SetValue( (val&0x01) ? true:false );
    m_GC6ShortEvents->SetValue( (val&0x02) ? true:false );
    m_SaveOutputState = (val&0x01) ? true:false;
    m_ShortEvents = (val&0x02) ? true:false;
  }
  else if( nr == 2 ) {
    m_CANID = val;
    m_GC6CanID->SetValue(val);
  }
  else if( nr < 19 ) {
    // servo config
    gc6SetServoConf((nr-3)/4, nr-3, val);
  }
}

void CBusNodeDlg::initGC2Event( int idx, int nn, int addr ) {
  if( idx < 16 ) {
    // port event
    gc2SetPort(idx+1, -1, nn, addr);
    m_Ports[idx].nn = nn;
    m_Ports[idx].addr = addr;
  }
  else if( idx == 16 ) {
    // SOD
    m_GC2SOD->SetValue( addr );
    m_SOD = addr;
  }
}

void CBusNodeDlg::initGC6Event( int idx, int nn, int addr ) {
  if( idx < 8 ) {
    // servo event
    gc6SetServoEvent(idx, nn, addr);
  }
  else if( idx == 8 ) {
    // SOD
    m_GC6SOD->SetValue( addr );
    m_SOD = addr;
  }
}

void CBusNodeDlg::onGC2GetAll( wxCommandEvent& event ) {
  m_bGC2GetAll = true;
  varGet(1);
}

void CBusNodeDlg::onGC2Test( wxCommandEvent& event ) {
  wxSpinCtrl* gc2NN[] = {NULL,m_GC2EvtNN1,m_GC2EvtNN2,m_GC2EvtNN3,m_GC2EvtNN4,m_GC2EvtNN5
      ,m_GC2EvtNN6,m_GC2EvtNN7,m_GC2EvtNN8,m_GC2EvtNN9,m_GC2EvtNN10
      ,m_GC2EvtNN11,m_GC2EvtNN12,m_GC2EvtNN13,m_GC2EvtNN14,m_GC2EvtNN15,m_GC2EvtNN16};

  wxSpinCtrl* gc2Addr[] = {NULL,m_GC2EvtAddr1,m_GC2EvtAddr2,m_GC2EvtAddr3,m_GC2EvtAddr4,m_GC2EvtAddr5
      ,m_GC2EvtAddr6,m_GC2EvtAddr7,m_GC2EvtAddr8,m_GC2EvtAddr9,m_GC2EvtAddr10
      ,m_GC2EvtAddr11,m_GC2EvtAddr12,m_GC2EvtAddr13,m_GC2EvtAddr14,m_GC2EvtAddr15,m_GC2EvtAddr16};

  wxButton* gc2Test[] = {NULL,m_GC2Test1,m_GC2Test2,m_GC2Test3,m_GC2Test4,m_GC2Test5
      ,m_GC2Test6,m_GC2Test7,m_GC2Test8,m_GC2Test9,m_GC2Test10
      ,m_GC2Test11,m_GC2Test12,m_GC2Test13,m_GC2Test14,m_GC2Test15,m_GC2Test16};

  int bus  = 0;
  int addr = 0;
  int group = 0;
  const char* cmd = wOutput.on;

  if( event.GetEventType() != wxEVT_COMMAND_BUTTON_CLICKED ) {
    return;
  }

  bool isInput = false;
  int port = 0;

  for( int i = 0; i < 16; i++ ) {
    if( event.GetEventObject() == gc2Test[i+1] ) {
      int cfg, nn, en;
      gc2GetPort(i+1, &cfg, &nn, &en);
      port = i+1;
      isInput = cfg & 0x01;
      bus = gc2NN[i+1]->GetValue();
      addr = gc2Addr[i+1]->GetValue();
      if( gc2Test[i+1]->GetLabel().CompareTo(_T("1")) == 0 )
          cmd = wOutput.off;
      if( i > 7 )
        group = 1;
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "test button %d clicked, addr=%d, cmd=%s", i+1, addr, cmd);
      break;
    }
  }

  if( addr != 0 ) {
    iONode swcmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
    wOutput.setaddr( swcmd, addr );
    wOutput.setbus( swcmd, bus );
    wOutput.setiid( swcmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wOutput.setcmd( swcmd, cmd );
    TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "send test command for port %d...", port);
    wxGetApp().sendToRocrail( swcmd );
    swcmd->base.del( swcmd );
  }

  if( isInput ) {
    m_GC2IgnorePortTest = port;
    if( gc2Test[port]->GetLabel().CompareTo(_T("1")) == 0 )
      gc2Test[port]->SetLabel(_T("0"));
    else
      gc2Test[port]->SetLabel(_T("1"));
  }
  m_GC2SetIndex = 19+group;
  m_Timer->Start( 100, wxTIMER_ONE_SHOT );
}

void CBusNodeDlg::onGC2SetAll( wxCommandEvent& event ) {
  m_bGC2SetAll = true;
  m_GC2SetAll->Enable(false);
  m_GC2GetAll->Enable(false);
  m_GC2Set->Enable(false);
  m_GC2SetIndex = 0;
  m_Timer->Start( 100, wxTIMER_ONE_SHOT );
}


void CBusNodeDlg::onGC2Set( wxCommandEvent& event ) {
  for( int i = 0; i < 16; i++ ) {
    int conf = 0;
    int nn = 0;
    int addr = 0;
    gc2GetPort(i+1, &conf, &nn, &addr);
    if( m_Ports[i].nn != nn || m_Ports[i].addr != addr || m_Ports[i].cfg != conf ) {
      m_Ports[i].nn = nn;
      m_Ports[i].addr = addr;
      m_Ports[i].cfg = conf;
      m_GC2SetIndex = i + 2;
      m_bGC2SetAll = false;
      m_GC2Set->Enable(false);
      setLearn();
      m_Timer->Start( 100, wxTIMER_ONE_SHOT );
      return;
    }
  }

  if( m_SOD != m_GC2SOD->GetValue() ) {
    m_GC2SetIndex = 18;
    m_bGC2SetAll = false;
    m_GC2Set->Enable(false);
    setLearn();
    m_Timer->Start( 100, wxTIMER_ONE_SHOT );
    return;
  }

  if(m_GC2SaveOutput->IsChecked() != m_SaveOutputState ||
      m_GC2ShortEvents->IsChecked() != m_ShortEvents ||
      m_GC2SODAll->IsChecked() != m_SODAll ||
      m_GC2PulseTime->GetSelection() != m_PulseTime )
  {
    m_GC2SetIndex = 0;
    m_bGC2SetAll = false;
    m_GC2Set->Enable(false);
    m_Timer->Start( 100, wxTIMER_ONE_SHOT );
  }

  if(m_GC2CanID->GetValue() != m_CANID )
  {
    m_GC2SetIndex = 21;
    m_bGC2SetAll = false;
    m_GC2Set->Enable(false);
    m_Timer->Start( 100, wxTIMER_ONE_SHOT );
  }

}


void CBusNodeDlg::OnTimer(wxTimerEvent& event) {
  if( m_bGC6SetAll ) {
    if( m_GC6SetIndex == 0 ) {
      int nv1 = m_GC6SaveServoPos->IsChecked() ? 0x01:0x00;
      nv1 += m_GC6ShortEvents->IsChecked() ? 0x02:0x00;
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "nv1=0x%02X", nv1);
      varSet(1, nv1, false);
    }
    else if( m_GC6SetIndex == 1 ) {
      int canid = m_GC6CanID->GetValue();
      varSet(2, canid, false);
    }
    else if( m_GC6SetIndex == 2 ) {
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "set learn mode");
      setLearn();
    }
    else if( m_GC6SetIndex < 7 ) {
      // Swithch events
      int servo = m_GC6SetIndex - 3;
      eventSet( m_Servo[servo].swnn, m_Servo[servo].swaddr, m_GC6SetIndex-3, 0, false );
    }
    else if( m_GC6SetIndex < 11 ) {
      // Sensor events
      int servo = m_GC6SetIndex - 7;
      eventSet( 0, m_Servo[servo].fbaddr, m_GC6SetIndex-7, 0, false );
    }
    else if( m_GC6SetIndex == 11 ) {
      // SOD
      eventSet( 0, m_GC6SOD->GetValue(), 11, 0, false );
    }
    m_GC6SetIndex++;
    if( m_bGC6SetAll && m_GC6SetIndex < 11 ) {
      m_Timer->Start( 100, wxTIMER_ONE_SHOT );
    }
    else {
      m_bGC6SetAll = false;
      m_GC6SetAll->Enable(true);
      m_GC6GetAll->Enable(true);
      setUnlearn();
    }
  }
  else {
    if( m_GC2SetIndex == 0 ) {
      int nv1 = m_GC2SaveOutput->IsChecked() ? 0x01:0x00;
      nv1 += m_GC2ShortEvents->IsChecked() ? 0x02:0x00;
      nv1 += (m_GC2PulseTime->GetSelection() << 2);
      nv1 += m_GC2SODAll->IsChecked() ? 0x10:0x00;
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
      eventSet( 0, m_GC2SOD->GetValue(), 16, 0, false );
    }
    else if( m_GC2SetIndex == 19 ) {
      varGet(18);
      varGet(19);
    }
    else if( m_GC2SetIndex == 20 ) {
      varGet(19);
      varGet(18);
    }
    else if( m_GC2SetIndex == 21 ) {
      int canid = m_GC2CanID->GetValue();
      varSet(20, canid, false);
    }
    m_GC2SetIndex++;
    if( m_bGC2SetAll && m_GC2SetIndex < 21 ) {
      m_Timer->Start( 100, wxTIMER_ONE_SHOT );
    }
    else {
      m_bGC2SetAll = false;
      m_GC2SetAll->Enable(true);
      m_GC2GetAll->Enable(true);
      m_GC2Set->Enable(true);
      setUnlearn();
    }
  }
}

void CBusNodeDlg::onSoD( wxCommandEvent& event ) {
  iONode swcmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
  wOutput.setaddr( swcmd, m_GC2SOD->GetValue() );
  wOutput.setbus( swcmd, 0 );
  wOutput.setiid( swcmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wOutput.setcmd( swcmd, wOutput.sod );
  wxGetApp().sendToRocrail( swcmd );
  swcmd->base.del( swcmd );

}



void CBusNodeDlg::onGC7ShowTemp( wxCommandEvent& event ) {
  onGC7ShowDate(event);
}
void CBusNodeDlg::onGC7ShowDate( wxCommandEvent& event ) {
  int nv1 = m_GC7ShowDate->IsChecked() ? 0x10:0x00;
  nv1 += m_GC7PosDisplay->IsChecked() ? 0x20:0x00;
  nv1 += m_GC7ShowTemp->IsChecked() ? 0x40:0x00;
  nv1 += m_GC7IntensitySlider->GetValue();
  varSet(1, nv1, false);
}
void CBusNodeDlg::onGC7PosDisplay( wxCommandEvent& event ) {
  onGC7ShowDate(event);
}
void CBusNodeDlg::onGC7Intensity( wxScrollEvent& event ) {
  int nv1 = m_GC7ShowDate->IsChecked() ? 0x10:0x00;
  nv1 += m_GC7PosDisplay->IsChecked() ? 0x20:0x00;
  nv1 += m_GC7ShowTemp->IsChecked() ? 0x40:0x00;
  nv1 += m_GC7IntensitySlider->GetValue();
  varSet(1, nv1, false);
}
void CBusNodeDlg::onGC7GetAll( wxCommandEvent& event ) {
  m_bGC7GetAll = true;
  varGet(1);
}
void CBusNodeDlg::onGC7SetCanID( wxCommandEvent& event ) {
  varSet(2, m_GC7CanID->GetValue(), false);
}

void CBusNodeDlg::initGC7Var( int nr, int val ) {

  if( nr == 1 ) {
    // node var1
    m_GC7ShowDate->SetValue( (val&0x10) ? true:false );
    m_GC7PosDisplay->SetValue( (val&0x20) ? true:false );
    m_GC7ShowTemp->SetValue( (val&0x40) ? true:false );
    m_GC7IntensitySlider->SetValue( val&0x0F);
  }
  else if( nr == 2 ) {
    // node var1
    m_GC7CanID->SetValue( val);
  }
}



// GC6
void CBusNodeDlg::OnServoSelect( wxCommandEvent& event ) {
}
void CBusNodeDlg::OnServoLeftAngle( wxScrollEvent& event ) {
}
void CBusNodeDlg::OnServoRightAngle( wxScrollEvent& event ) {
}
void CBusNodeDlg::OnServoSpeed( wxScrollEvent& event ) {
}
void CBusNodeDlg::OnServoRelay( wxCommandEvent& event ) {
}
void CBusNodeDlg::OnExtSensors( wxCommandEvent& event ) {
}

void CBusNodeDlg::onGC6GetAll( wxCommandEvent& event ) {
  m_bGC6GetAll = true;
  varGet(1);
}

void CBusNodeDlg::onGC6SetAll( wxCommandEvent& event ) {
  m_bGC6SetAll = true;
  m_GC6SetAll->Enable(false);
  m_GC6GetAll->Enable(false);
  m_GC6SetIndex = 0;
  m_Timer->Start( 100, wxTIMER_ONE_SHOT );
}

