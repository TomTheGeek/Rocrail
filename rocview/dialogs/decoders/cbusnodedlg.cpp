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
#include "cbusnodedlg.h"


#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif
#include <wx/filedlg.h>

#include "rocview/public/guiapp.h"

#include "rocview/wrapper/public/Gui.h"

#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/CBus.h"
#include "rocrail/wrapper/public/CBusNode.h"
#include "rocrail/wrapper/public/CBusNodeVar.h"
#include "rocrail/wrapper/public/CBusNodeEvent.h"

#include "rocs/public/trace.h"
#include "rocs/public/strtok.h"
#include "rocdigs/impl/cbus/cbusdefs.h"
#include "rocdigs/impl/cbus/rocrail.h"

#include "rocview/res/icons.hpp"

#define ME_GC2Timer 4711

CBusNodeDlg::CBusNodeDlg( wxWindow* parent ):cbusnodedlggen( parent )
{
  initLabels();
  init(NULL);
}

CBusNodeDlg::CBusNodeDlg( wxWindow* parent, iONode event ):cbusnodedlggen( parent )
{
  initLabels();
  init(event);
}

void CBusNodeDlg::initLabels() {
  m_CBusLogo->SetBitmap(*_img_cbus);

  m_SOD = 0;
  m_SaveOutputState = false;
  m_ShortEvents = false;
  m_SODAll = false;
  m_PulseTime = 0;
  m_GC2IgnorePortTest = 0;
  m_CANID = 0;
  m_CBus = NodeOp.inst(wCBus.name(), NULL, ELEMENT_NODE);

  /*TEST
  iONode node = NodeOp.inst(wCBusNode.name(),m_CBus,ELEMENT_NODE);
  wCBusNode.setnr(node,1);
  wCBusNode.setcanid(node,178);
  wCBusNode.setmanuid(node,MANU_ROCRAIL);
  wCBusNode.setmtyp(node,MTYP_CANGC6);
  NodeOp.addChild(m_CBus, node);
  node = NodeOp.inst(wCBusNode.name(),m_CBus,ELEMENT_NODE);
  wCBusNode.setnr(node,4);
  wCBusNode.setcanid(node,173);
  wCBusNode.setmanuid(node,MANU_ROCRAIL);
  wCBusNode.setmtyp(node,MTYP_CANGC2);
  NodeOp.addChild(m_CBus, node);
  TEST*/

  m_bGC8GetAll = false;
  m_bGC8SetAll = false;
  m_bGCLNGetAll = false;
  m_bGCLNSetAll = false;
  m_bGC1eGetAll = false;
  m_bGC1eSetAll = false;
  m_bGC2GetAll = false;
  m_bGC2SetAll = false;
  m_bGC7GetAll = false;
  m_bGC4GetAll = false;
  m_bGC4SetAll = false;
  m_bGC6GetAll = false;
  m_bGC6SetAll = false;
  m_bGC2SetAll = true;
  m_GC2SetIndex = 0;
  m_GC6SetIndex = 0;
  m_GC4SetIndex = 0;
  m_GC1eSetIndex = 0;
  m_GCLNSetIndex = 0;
  m_SortCol = 0;

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
  m_labNumber->SetLabel( wxGetApp().getMsg( "nodenumber" ) );
  m_SetNodeNumber->SetLabel( wxGetApp().getMsg( "set" ) );

  // Index tab
  //m_IndexDelete->SetLabel( wxGetApp().getMsg( "delete" ) );
  m_labIndexIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_QueryNN->SetLabel( wxGetApp().getMsg( "query" ) );

  m_IndexList2->InsertColumn(0, wxGetApp().getMsg( "nodenumber" ), wxLIST_FORMAT_RIGHT );
  m_IndexList2->InsertColumn(1, wxGetApp().getMsg( "CANID" ), wxLIST_FORMAT_RIGHT );
  m_IndexList2->InsertColumn(2, wxGetApp().getMsg( "manufactured_ID" ), wxLIST_FORMAT_LEFT );
  m_IndexList2->InsertColumn(3, wxGetApp().getMsg( "type" ), wxLIST_FORMAT_RIGHT );
  m_IndexList2->InsertColumn(4, wxGetApp().getMsg( "name" ), wxLIST_FORMAT_LEFT, 150 );
  m_IndexList2->InsertColumn(5, wxGetApp().getMsg( "description" ), wxLIST_FORMAT_LEFT, 250 );


  // Variables tab
  m_labVarIndex->SetLabel( wxGetApp().getMsg( "index" ) );
  m_labVarValue->SetLabel( wxGetApp().getMsg( "value" ) );
  m_VarGet->SetLabel( wxGetApp().getMsg( "get" ) );
  m_VarSet->SetLabel( wxGetApp().getMsg( "set" ) );

  // Events tab
  m_labEventNode->SetLabel( wxGetApp().getMsg( "nodenumber" ) );
  m_labEventAddr->SetLabel( wxGetApp().getMsg( "eventnumber" ) );
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

  // GC2 tab
  m_lab1GC2Port->SetLabel( wxGetApp().getMsg( "port" ) );
  m_lab1GC2EvtNN->SetLabel( wxGetApp().getMsg( "nodenumber" ) );
  m_lab1GC2EvtAddr->SetLabel( wxGetApp().getMsg( "eventnumber" ) );
  m_lab2GC2Port->SetLabel( wxGetApp().getMsg( "port" ) );
  m_lab2GC2EvtNN->SetLabel( wxGetApp().getMsg( "nodenumber" ) );
  m_lab2GC2EvtAddr->SetLabel( wxGetApp().getMsg( "eventnumber" ) );

  // Buttons
  m_stdButtonOK->SetLabel( wxGetApp().getMsg( "ok" ) );

  // Resize
  m_NodeNumberPanel->GetSizer()->Layout();
  m_IndexPanel->GetSizer()->Layout();
  m_VarPanel->GetSizer()->Layout();
  m_EventsPanel->GetSizer()->Layout();
  m_FirmwarePanel->GetSizer()->Layout();
  m_CANGC1ePanel->GetSizer()->Layout();
  m_CANGC2Panel->GetSizer()->Layout();
  m_CANGC4Panel->GetSizer()->Layout();
  m_CANGC6Panel->GetSizer()->Layout();
  m_CANGC7Panel->GetSizer()->Layout();

  m_NoteBook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  m_GC1eCanID->Enable(false);
}


void CBusNodeDlg::init( iONode event ) {
  MemOp.set( m_Ports, sizeof(Port), 0 );

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


static int __sortNr(obj* _a, obj* _b) {
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    if (wCBusNode.getnr(a) > wCBusNode.getnr(b) )
      return 1;
    if (wCBusNode.getnr(a) < wCBusNode.getnr(b) )
      return -1;
    else
      return 0;
}
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    if (wCBusNode.getcanid(a) > wCBusNode.getcanid(b) )
      return 1;
    if (wCBusNode.getcanid(a) < wCBusNode.getcanid(b) )
      return -1;
    else
      return 0;
}
static int __sortType(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = CBusNodeDlg::getType(wCBusNode.getmanuid(a), wCBusNode.getmtyp(a));
    const char* idB = CBusNodeDlg::getType(wCBusNode.getmanuid(b), wCBusNode.getmtyp(b));
    return strcmp( idA, idB );
}
static int __sortManu(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = CBusNodeDlg::getManu(wCBusNode.getmanuid(a));
    const char* idB = CBusNodeDlg::getManu(wCBusNode.getmanuid(b));
    return strcmp( idA, idB );
}
static int __sortDesc(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = CBusNodeDlg::getTypeDesc(wCBusNode.getmanuid(a), wCBusNode.getmtyp(a));
    const char* idB = CBusNodeDlg::getTypeDesc(wCBusNode.getmanuid(b), wCBusNode.getmtyp(b));
    return strcmp( idA, idB );
}


void CBusNodeDlg::initIndex() {
  m_IndexList2->DeleteAllItems();
  int index = 0;
  if( m_CBus != NULL ) {
    iOList nodeList = ListOp.inst();
    iONode cbusnode = wCBus.getcbnode(m_CBus);
    while( cbusnode != NULL ) {
      ListOp.add( nodeList, (obj)cbusnode );
      cbusnode = wCBus.nextcbnode( m_CBus, cbusnode );
    }

    if( m_SortCol == 1 ) {
      ListOp.sort(nodeList, &__sortID);
    }
    else if( m_SortCol == 2 ) {
      ListOp.sort(nodeList, &__sortManu);
    }
    else if( m_SortCol == 3 || m_SortCol == 4 ) {
      ListOp.sort(nodeList, &__sortType);
    }
    else if( m_SortCol == 5 ) {
      ListOp.sort(nodeList, &__sortDesc);
    }
    else {
      ListOp.sort(nodeList, &__sortNr);
    }

    for( int index = 0; index < ListOp.size(nodeList); index++) {
      cbusnode = (iONode)ListOp.get(nodeList, index);
      m_IndexList2->InsertItem( index, wxString::Format(_T("%d"), wCBusNode.getnr(cbusnode)));
      m_IndexList2->SetItem( index, 1, wxString::Format(_T("%d"), wCBusNode.getcanid(cbusnode)));
      m_IndexList2->SetItem( index, 2, wxString(getManu(wCBusNode.getmanuid(cbusnode)),wxConvUTF8));
      m_IndexList2->SetItem( index, 3, wxString::Format(_T("%d:%d"), wCBusNode.getmanuid(cbusnode), wCBusNode.getmtyp(cbusnode)));
      m_IndexList2->SetItem( index, 4, wxString(getType(wCBusNode.getmanuid(cbusnode), wCBusNode.getmtyp(cbusnode)),wxConvUTF8) );
      m_IndexList2->SetItem( index, 5, wxString(getTypeDesc(wCBusNode.getmanuid(cbusnode), wCBusNode.getmtyp(cbusnode)),wxConvUTF8) );
      m_IndexList2->SetItemPtrData(index, (wxUIntPtr)cbusnode);
    }
    ListOp.base.del(nodeList);
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


iONode CBusNodeDlg::getNode(int nr, int mtype, int manu, const char* ver, int canid) {
  iONode l_RocrailIni = wxGetApp().getFrame()->getRocrailIni();
  if( m_CBus != NULL ) {
    iONode cbusnode = wCBus.getcbnode(m_CBus);
    while( cbusnode != NULL ) {
      if( wCBusNode.getnr(cbusnode) == nr )
        return cbusnode;
      cbusnode = wCBus.nextcbnode( m_CBus, cbusnode );
    }
    cbusnode = NodeOp.inst( wCBusNode.name(), m_CBus, ELEMENT_NODE );
    NodeOp.addChild(m_CBus, cbusnode);
    wCBusNode.setcanid( cbusnode, canid );
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
        initGC4Var(nr, val);
        initGC6Var(nr, val);
        initGC7Var(nr, val);
        initGC1eVar(nr, val);
        initGCLNVar(nr, val);
        initGC8Var(nr, val);
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
    initGC4Var(nr, val);
    initGC6Var(nr, val);
    initGC7Var(nr, val);
    initGC1eVar(nr, val);
    initGCLNVar(nr, val);
    initGC8Var(nr, val);
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
        initGC4Event(evnr, evnn, evaddr);
        initGC6Event(evnr, evnn, evaddr);
        initGCLNEvent(evnr, evnn, evaddr);
        initGC8Event(evnr, evnn, evaddr);
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
    initGC4Event(evnr, evnn, evaddr);
    initGC6Event(evnr, evnn, evaddr);
    initGCLNEvent(evnr, evnn, evaddr);
    initGC8Event(evnr, evnn, evaddr);
    return cbusnodeevt;
  }
  return NULL;
}


void CBusNodeDlg::initType( int manu, int mtype, const char* ver ) {
  m_NodeType->SetValue( wxString(getManu(manu),wxConvUTF8) + _T("::")+ wxString(getType(manu, mtype),wxConvUTF8) );
  if( mtype > 0 )
    m_NodeTypeNr->SetValue(mtype);
  if( manu > 0 )
    m_NodeManuNr->SetValue(manu);
  if( ver != NULL && StrOp.len(ver) > 0 ) {
    TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999,"version=%s", ver);
    m_Version->SetValue(wxString(ver,wxConvUTF8));
  }
}

const char* CBusNodeDlg::getManu( int manu ) {
  switch( manu ) {
  case MANU_ROCRAIL: return "Rocrail";
  case MANU_MERG: return "MERG";
  }
  return "Unknown";
}


const char* CBusNodeDlg::getType( int manu, int mtype ) {
  if( manu == MANU_ROCRAIL ) {
    switch( mtype ) {
    case MTYP_CANGC2:    return "CAN-GC2";
    case MTYP_CANGC4:    return "CAN-GC4";
    case MTYP_CANGC6:    return "CAN-GC6";
    case MTYP_CANGC7:    return "CAN-GC7";
    case MTYP_CANGC8:    return "CAN-GC8";
    case MTYP_CANGC1E:   return "CAN-GC1e";
    case MTYP_CANGCLN:   return "CAN-GCLN";
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

const char* CBusNodeDlg::getTypeDesc( int manu, int mtype ) {
  if( manu == MANU_ROCRAIL ) {
    switch( mtype ) {
    case MTYP_CANGC2:    return "16 port I/O";
    case MTYP_CANGC4:    return "RFID concentrator";
    case MTYP_CANGC6:    return "4 Channel Servo";
    case MTYP_CANGC7:    return "Fast clock";
    case MTYP_CANGC8:    return "Display driver";
    case MTYP_CANGC1E:   return "Ethernet interface";
    case MTYP_CANGCLN:   return "CBUS-LocoNet bridge";
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
    case MTYP_CANCAB:    return "CAB";
    case MTYP_CANCMD:    return "DCC Command Station";
    case MTYP_CANSERVO:  return "Servo";
    case MTYP_CANBC:     return "CANBC";
    case MTYP_CANRPI:    return "CANRPI";
    case MTYP_CANTTCA:   return "CANTTCA";
    case MTYP_CANTTCB:   return "CANTTCB";
    }
  }
  TraceOp.trc( "cbusnodedlg", TRCLEVEL_WARNING, __LINE__, 9999, "unknown type: manu=%d, mtype=%d ", manu, mtype );
  return "UNKNOWN";
}

void CBusNodeDlg::selectPage4Type( int manu, int mtype ) {
  int offset = 0;
  if( manu == MANU_ROCRAIL ) {
    wxCommandEvent evt;
    switch( mtype ) {
    case MTYP_CANGC1E:   m_NoteBook->SetSelection( 5 - offset );  onGC1eGetAll( evt ); break;
    case MTYP_CANGC2:    m_NoteBook->SetSelection( 6 - offset);   onGC2GetAll( evt ); break;
    case MTYP_CANGC4:    m_NoteBook->SetSelection( 7 - offset );  onGC4GetAll( evt ); break;
    case MTYP_CANGC6:    m_NoteBook->SetSelection( 8 - offset );  onGC6GetAll( evt ); break;
    case MTYP_CANGC7:    m_NoteBook->SetSelection( 9 - offset );  onGC7GetAll( evt ); break;
    case MTYP_CANGC8:    m_NoteBook->SetSelection( 10 - offset );  onGC8GetAll( evt ); break;
    case MTYP_CANGCLN:   m_NoteBook->SetSelection( 11 - offset ); onGCLNGetAll( evt ); break;
    }
  }
}

void CBusNodeDlg::onSetPage(wxCommandEvent& event) {
  TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999, "set page to %d", m_SetPage );
  m_NoteBook->SetSelection( m_SetPage );
}


void CBusNodeDlg::onOK( wxCommandEvent& event )
{
  if( m_CBus != NULL ) {
    NodeOp.base.del(m_CBus);
  }
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
  cmd->base.del(cmd);

  getNode(nn, m_NodeTypeNr->GetValue(), m_NodeManuNr->GetValue(), m_Version->GetValue().mb_str(wxConvUTF8) );
}

void CBusNodeDlg::onIndexSelect2( wxListEvent& event ) {
  int index = event.GetIndex();
  iONode node = (iONode)m_IndexList2->GetItemData(index);
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

void CBusNodeDlg::onIndexActivated( wxListEvent& event ) {
  int index = event.GetIndex();
  iONode node = (iONode)m_IndexList2->GetItemData(index);
  if( node != NULL ) {
    selectPage4Type(wCBusNode.getmanuid(node), wCBusNode.getmtyp(node));
  }
  else
    TraceOp.trc( "cbusnodedlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
}

void CBusNodeDlg::onQuery( wxCommandEvent& event ) {
  if( m_CBus != NULL ) {
    iONode node = wCBus.getcbnode(m_CBus);
    while( node != NULL ) {
      NodeOp.removeChild( m_CBus, node );
      node = wCBus.getcbnode(m_CBus);
    }
    initIndex();

    m_IID->SetValue(m_IndexIID->GetValue());
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setcmd( cmd, wProgram.query );
    wProgram.setiid( cmd, m_IndexIID->GetValue().mb_str(wxConvUTF8) );
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
    int canid = wProgram.getmodid(event);
    int nn = wProgram.getdecaddr(event);
    int manu = wProgram.getmanu(event);
    int prod = wProgram.getprod(event);
    getNode(nn, prod, manu, NULL, canid);
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
    TraceOp.trc( "cbusnode", TRCLEVEL_INFO, __LINE__, 9999, "node variable %d, val=%d gc1e=%d gc2=%d gc4=%d gc6=%d gc7=%d gcln=%d gc8=%d",
        cv, val, m_bGC1eGetAll, m_bGC2GetAll, m_bGC4GetAll, m_bGC6GetAll, m_bGC7GetAll, m_bGCLNGetAll, m_bGC8GetAll);

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

    else if( m_bGC8GetAll ) {
      if( cv < 4 ) {
        varGet(cv+1);
      }
      else {
        m_bGC8GetAll = false;
        m_GC8SetAll->Enable(true);
        m_GC8GetAll->Enable(true);
        eventGetAll();
      }
    }

    else if( m_bGC6GetAll ) {
      if( cv < 22 ) {
        varGet(cv+1);
      }
      else {
        m_bGC6GetAll = false;
        m_GC6SetAll->Enable(true);
        m_GC6GetAll->Enable(true);
        eventGetAll();
      }
    }
    else if( m_bGCLNGetAll ) {
      if( cv < 2 ) {
        varGet(cv+1);
      }
      else {
        m_bGCLNGetAll = false;
        m_GCLNSetAll->Enable(true);
        m_GCLNGetAll->Enable(true);
        eventGetAll();
      }
    }
    else if( m_bGC4GetAll ) {
      if( cv < 27 ) {
        varGet(cv+1);
      }
      else {
        m_bGC4GetAll = false;
        m_GC4SetAll->Enable(true);
        m_GC4GetAll->Enable(true);
        eventGetAll();
      }
    }

    else if( m_bGC7GetAll ) {
      if( cv < 2 ) {
        varGet(cv+1);
      }
      else {
        m_bGC7GetAll = false;
        eventGetAll();
      }
    }

    else if( m_bGC1eGetAll ) {
      if( cv < 17 ) {
        varGet(cv+1);
      }
      else {
        m_GC1eSetAll->Enable(true);
        m_GC1eGetAll->Enable(true);
        m_bGC1eGetAll = false;
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
  wProgram.seteeprom( cmd, m_WriteEEprom->IsChecked()?True:False );
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

    if( input && delay )
      gc2Block[port]->SetValue(true);
    else if( input && !delay )
      gc2Input[port]->SetValue(true);
    else if( delay )
      gc2Pulse[port]->SetValue(true);
    else
      gc2Switch[port]->SetValue(true);

    gc2C2[port]->SetValue((conf & 0x04) ? true:false);
    gc2NN[port]->Enable(!input);
    if( input )
      gc2NN[port]->SetValue(0);
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
  else if( idx > 3 && idx < 8 ) {
    m_Servo[idx-4].fbaddr = addr;
    gc6FbAddr[(idx-4)+1]->SetValue(addr);
  }

}


void CBusNodeDlg::gc6UpdateServoEvent(int servo) {
  wxSpinCtrl* gc6SwNN  [] = {m_GC6Servo1SwNN, m_GC6Servo2SwNN, m_GC6Servo3SwNN, m_GC6Servo4SwNN};
  wxSpinCtrl* gc6SwAddr[] = {m_GC6Servo1SwEvent, m_GC6Servo2SwEvent, m_GC6Servo3SwEvent, m_GC6Servo4SwEvent};
  wxSpinCtrl* gc6FbAddr[] = {m_GC6Servo1FbEvent, m_GC6Servo2FbEvent, m_GC6Servo3FbEvent, m_GC6Servo4FbEvent};

  m_Servo[servo].swnn   = gc6SwNN  [servo]->GetValue();
  m_Servo[servo].swaddr = gc6SwAddr[servo]->GetValue();
  m_Servo[servo].fbaddr = gc6FbAddr[servo]->GetValue();

}


void CBusNodeDlg::gc6SetServoConf(int servo, int idx, int val) {
  wxCheckBox* gc6Relay[] = {m_GC6Servo1Relay,m_GC6Servo2Relay,m_GC6Servo3Relay,m_GC6Servo4Relay};
  wxCheckBox* gc6ExtFb[] = {m_GC6Servo1ExtFb,m_GC6Servo2ExtFb,m_GC6Servo3ExtFb,m_GC6Servo4ExtFb};
  wxCheckBox* gc6Bounce[] = {m_GC6Servo1Bounce,m_GC6Servo2Bounce,m_GC6Servo3Bounce,m_GC6Servo4Bounce};

  wxSlider* gc6Left [] = {m_GC6Servo1LeftAng ,m_GC6Servo2LeftAng ,m_GC6Servo3LeftAng ,m_GC6Servo4LeftAng};
  wxSlider* gc6Right[] = {m_GC6Servo1RightAng,m_GC6Servo2RightAng,m_GC6Servo3RightAng,m_GC6Servo4RightAng};
  wxSlider* gc6SpeedL[] = {m_GC6Servo1Speed  ,m_GC6Servo2Speed   ,m_GC6Servo3Speed   ,m_GC6Servo4Speed};
  wxSlider* gc6SpeedR[] = {m_GC6Servo1SpeedR ,m_GC6Servo2SpeedR  ,m_GC6Servo3SpeedR  ,m_GC6Servo4SpeedR};

  if( idx % 5 == 0 ) {
    m_Servo[servo].cfg = val;
    gc6Relay[servo]->SetValue(val&0x01?true:false);
    gc6ExtFb[servo]->SetValue(val&0x02?true:false);
    gc6Bounce[servo]->SetValue(val&0x04?true:false);
  }
  if( idx % 5 == 1 ) {
    m_Servo[servo].left = val;
    gc6Left[servo]->SetValue(val);
  }
  if( idx % 5 == 2 ) {
    m_Servo[servo].right = val;
    gc6Right[servo]->SetValue(val);
  }
  if( idx % 5 == 3 ) {
    m_Servo[servo].speedL = val;
    gc6SpeedL[servo]->SetValue(val);
  }
  if( idx % 5 == 4 ) {
    m_Servo[servo].speedR = val;
    gc6SpeedR[servo]->SetValue(val);
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
  else if( nr < 23 ) {
    // servo config
    gc6SetServoConf((nr-3)/5, nr-3, val);
  }
}

void CBusNodeDlg::initGCLNVar( int nr, int val ) {
  if( nr == 1 ) {
    // node var1
    m_GCLNReadOnly->SetValue((val & 0x01) ? true:false );
    m_GCLNCBusSensorEvents->SetValue((val & 0x02) ? true:false );
    m_GCLNSoD->SetValue((val & 0x04) ? true:false );
  }
  else if( nr == 2 ) {
    m_CANID = val;
    m_GCLNCANID->SetValue(val);
  }
}

void CBusNodeDlg::initGC8Var( int nr, int val ) {
  if( nr == 1 ) {
    // node var1
  }
  else if( nr == 2 ) {
    m_CANID = val;
    m_GC8CANID->SetValue(val);
  }
  else if( nr == 3 ) {
    m_GC8Display1Contrast->SetValue(val&0x0F);
    m_GC8Display1FirstDH->SetValue(val&0x10?true:false);
    m_GC8Display1Clock->SetValue(val&0x20?true:false);
  }
  else if( nr == 4 ) {
    m_GC8Display2Contrast->SetValue(val&0x0F);
    m_GC8Display2FirstDH->SetValue(val&0x10?true:false);
    m_GC8Display2Clock->SetValue(val&0x20?true:false);
  }
}

void CBusNodeDlg::initGC4Var( int nr, int val ) {
  if( nr == 1 ) {
    // node var1
    //#define CFG_SAVERFID 0x01
    //#define CFG_CHECKRFID 0x02
    m_GC4SaveRFID->SetValue((val & 0x01) ? true:false );
    m_GC4CheckRFID->SetValue((val & 0x02) ? true:false );
    m_GC4ReleaseRFID->SetValue((val & 0x04) ? true:false );
    m_GC4Acc4RFID->SetValue((val & 0x08) ? true:false );
    // reset allowed rfids
    for( int i = 0; i < 5; i++ )
      for( int n = 0; n < 5; n++ )
        m_GC4AllowedRFID[i][n] = 0;
  }
  else if( nr == 2 ) {
    m_CANID = val;
    m_GC4CANID->SetValue(val);
  }
  else if( nr < 28 ) {
    // allowed rfid
    int idx = (nr-3)/5;
    int offset = (nr-3)%5;
    m_GC4AllowedRFID[idx][offset] = val;
    if( offset == 4 ) {
      wxTextCtrl* allowedRFID[] = {m_GC4AllowedRFID1,m_GC4AllowedRFID2,m_GC4AllowedRFID3,m_GC4AllowedRFID4,m_GC4AllowedRFID5};
      allowedRFID[idx]->SetValue(wxString::Format(_T("%d.%d.%d.%d.%d"),
          m_GC4AllowedRFID[idx][0],m_GC4AllowedRFID[idx][1],m_GC4AllowedRFID[idx][2],m_GC4AllowedRFID[idx][3],m_GC4AllowedRFID[idx][4])
      );
    }
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

void CBusNodeDlg::initGC8Event( int idx, int nn, int addr ) {
  if( idx == 0 ) {
    m_GC8Display1Event->SetValue(addr);
  }
  else if( idx == 1 ) {
    m_GC8Display2Event->SetValue(addr);
  }
}

void CBusNodeDlg::initGC4Event( int idx, int nn, int addr ) {
  if( idx < 8 ) {
    // rfid event
    wxSpinCtrl* event[] = {m_GC4RFID1,m_GC4RFID2,m_GC4RFID3,m_GC4RFID4,m_GC4RFID5,m_GC4RFID6,m_GC4RFID7,m_GC4RFID8};
    event[idx]->SetValue(addr);
  }
  else if( idx < 16 ) {
    // block event
    wxSpinCtrl* event[] = {m_GC4BK1,m_GC4BK2,m_GC4BK3,m_GC4BK4,m_GC4BK5,m_GC4BK6,m_GC4BK7,m_GC4BK8};
    event[idx-8]->SetValue(addr);
  }
  else if( idx == 16 ) {
    // SOD
    m_GC4SOD->SetValue( addr );
    m_SOD = addr;
  }
}

void CBusNodeDlg::initGCLNEvent( int idx, int nn, int addr ) {
  if( idx == 0 ) {
    // SOD
    m_GCLNSoDAddr->SetValue( addr );
    m_SOD = addr;
  }
  else if( idx == 1 ) {
    // switch filter start
    m_GCLNSwitchFilterStart->SetValue( addr );
  }
  else if( idx == 2 ) {
    // switch filter end
    m_GCLNSwitchFilterEnd->SetValue( addr );
  }
  else if( idx == 3 ) {
    // sensor filter start
    m_GCLNSensorFilterStart->SetValue( addr );
  }
  else if( idx == 4 ) {
    // sensor filter end
    m_GCLNSensorFilterEnd->SetValue( addr );
  }
}

void CBusNodeDlg::onGC2GetAll( wxCommandEvent& event ) {
  m_bGC2GetAll = true;
  m_bGC4GetAll = false;
  m_bGC6GetAll = false;
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
  m_bGC1eSetAll = false;
  m_bGC2SetAll = true;
  m_bGC4SetAll = false;
  m_bGC6SetAll = false;
  m_GC2SetAll->Enable(false);
  m_GC2GetAll->Enable(false);
  m_GC2Set->Enable(false);
  m_GC2SetIndex = 0;
  m_Timer->Start( 100, wxTIMER_ONE_SHOT );
}


void CBusNodeDlg::onGC2Set( wxCommandEvent& event ) {
  m_bGC1eSetAll = false;
  m_bGC2SetAll = false;
  m_bGC4SetAll = false;
  m_bGC6SetAll = false;
  for( int i = 0; i < 16; i++ ) {
    int conf = 0;
    int nn = 0;
    int addr = 0;
    gc2GetPort(i+1, &conf, &nn, &addr);
    if( m_Ports[i].nn != nn || m_Ports[i].addr != addr || m_Ports[i].cfg != conf ) {
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999,
          "port %d nn=%d-%d addr=%d-%d cfg=%d-%d",
          i, m_Ports[i].nn, nn, m_Ports[i].addr, addr, m_Ports[i].cfg, conf);
      m_Ports[i].nn = nn;
      m_Ports[i].addr = addr;
      m_Ports[i].cfg = conf;
      m_GC2SetIndex = i + 2;
      m_bGC2SetAll = false;
      setLearn();
      m_Timer->Start( 100, wxTIMER_ONE_SHOT );
      return;
    }
  }

  if( m_SOD != m_GC2SOD->GetValue() ) {
    m_SOD = m_GC2SOD->GetValue();
    TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "GC2 new SoD=%d", m_SOD);
    m_GC2SetIndex = 18;
    m_bGC2SetAll = false;
    setLearn();
    m_Timer->Start( 100, wxTIMER_ONE_SHOT );
    return;
  }

  if(m_GC2SaveOutput->IsChecked() != m_SaveOutputState ||
      m_GC2ShortEvents->IsChecked() != m_ShortEvents ||
      m_GC2SODAll->IsChecked() != m_SODAll ||
      m_GC2PulseTime->GetSelection() != m_PulseTime )
  {
    m_SaveOutputState = m_GC2SaveOutput->IsChecked();
    m_ShortEvents = m_GC2ShortEvents->IsChecked();
    m_SODAll = m_GC2SODAll->IsChecked();
    m_PulseTime = m_GC2PulseTime->GetSelection();
    TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "GC2 new options");
    m_GC2SetIndex = 0;
    m_bGC2SetAll = false;
    m_Timer->Start( 100, wxTIMER_ONE_SHOT );
  }

  if(m_GC2CanID->GetValue() != m_CANID )
  {
    m_CANID = m_GC2CanID->GetValue();
    TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "GC2 new CanID=%d", m_CANID);
    m_GC2SetIndex = 21;
    m_bGC2SetAll = false;
    m_Timer->Start( 100, wxTIMER_ONE_SHOT );
  }

}


void CBusNodeDlg::OnTimer(wxTimerEvent& event) {
  TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999,
      "timer gc1e=%d[%d] gc2=%d[%d] gc4=%d[%d] gc6=%d[%d] gcln=%d[%d] gc8=%d[%d]",
      m_bGC1eSetAll, m_GC1eSetIndex, m_bGC2SetAll, m_GC2SetIndex, m_bGC4SetAll, m_GC4SetIndex,
      m_bGC6SetAll, m_GC6SetIndex, m_bGCLNSetAll, m_GCLNSetIndex, m_bGC8SetAll, m_GC8SetIndex);

  if( m_bGC1eSetAll ) {
    wxSpinCtrl* gc1eip[] = {m_GC1eIP1,m_GC1eIP2,m_GC1eIP3,m_GC1eIP4};
    wxSpinCtrl* gc1emask[] = {m_GC1eNetmask1,m_GC1eNetmask2,m_GC1eNetmask3,m_GC1eNetmask4};
    wxSpinCtrl* gc1emac[] = {m_GC1eMAC1,m_GC1eMAC2,m_GC1eMAC3,m_GC1eMAC4,m_GC1eMAC5,m_GC1eMAC6};
    if( m_GC1eSetIndex == 0 ) {
      int nv1 = m_GC1eIdleWD->IsChecked() ? 0x01:0x00;
      nv1 |= m_GC1ePowerOffAtIdle->IsChecked() ? 0x02:0x00;
      nv1 |= m_GC1eCommandAck->IsChecked() ? 0x04:0x00;
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc1e nv1=0x%02X", nv1);
      varSet(1, nv1, false);
    }
    else if( m_GC1eSetIndex == 1 ) {
      int canid = m_GC1eCanID->GetValue();
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc1e canid=0x%02X", canid);
      varSet(2, canid, false); // CANID
    }
    else if( m_GC1eSetIndex > 2 && m_GC1eSetIndex < 7) {
      // IP Address
      int nv = gc1eip[m_GC1eSetIndex-3]->GetValue();
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc1e nv%d=0x%02X", m_GC1eSetIndex, nv);
      varSet(m_GC1eSetIndex, nv, false);
    }
    else if( m_GC1eSetIndex > 6 && m_GC1eSetIndex < 11) {
      // Netmask
      int nv = gc1emask[m_GC1eSetIndex-7]->GetValue();
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc1e nv%d=0x%02X", m_GC1eSetIndex, nv);
      varSet(m_GC1eSetIndex, nv, false);
    }
    else if( m_GC1eSetIndex > 10 && m_GC1eSetIndex < 17) {
      // MAC
      int nv = gc1emac[m_GC1eSetIndex-11]->GetValue();
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc1e nv%d=0x%02X", m_GC1eSetIndex, nv);
      varSet(m_GC1eSetIndex, nv, false);
    }
    else if( m_GC1eSetIndex == 17 ) {
      int idletime = m_GC1eIdleTime->GetValue();
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc1e idletime=0x%02X", idletime);
      varSet(17, idletime, false);
    }


    m_GC1eSetIndex++;
    if( m_bGC1eSetAll && m_GC1eSetIndex < 18 ) {
      m_Timer->Start( 100, wxTIMER_ONE_SHOT );
    }
    else {
      m_bGC1eSetAll = false;
      m_GC1eSetAll->Enable(true);
      m_GC1eGetAll->Enable(true);
    }
  }

  else if( m_bGC8SetAll ) {
    if( m_GC8SetIndex == 1 ) {
      int canid = m_GC8CANID->GetValue();
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc8 canid=0x%02X", canid);
      varSet(2, canid, false); // CANID
    }
    if( m_GC8SetIndex == 2 ) {
      int var = m_GC8Display1Contrast->GetValue();
      var += m_GC8Display1FirstDH->IsChecked() ? 0x10:0x00;
      var += m_GC8Display1Clock->IsChecked() ? 0x20:0x00;
      varSet(3, var, false);
    }
    else if( m_GC8SetIndex == 3 ) {
      int var = m_GC8Display2Contrast->GetValue();
      var += m_GC8Display2FirstDH->IsChecked() ? 0x10:0x00;
      var += m_GC8Display2Clock->IsChecked() ? 0x20:0x00;
      varSet(4, var, false);
    }
    else if( m_GC8SetIndex == 4 ) {
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "set gc8 learn mode");
      setLearn();
    }
    else if( m_GC8SetIndex == 5 ) {
      eventSet( 0, m_GC8Display1Event->GetValue(), 0, 0, false );
    }
    else if( m_GC8SetIndex == 6 ) {
      eventSet( 0, m_GC8Display2Event->GetValue(), 1, 0, false );
    }

    m_GC8SetIndex++;
    if( m_bGC8SetAll && m_GC8SetIndex < 7 ) {
      m_Timer->Start( 100, wxTIMER_ONE_SHOT );
    }
    else {
      m_bGC8SetAll = false;
      m_GC8SetAll->Enable(true);
      m_GC8GetAll->Enable(true);
    }
  }


  else if( m_bGCLNSetAll ) {
    if( m_GCLNSetIndex == 0 ) {
      int nv1 = m_GCLNReadOnly->IsChecked() ? 0x01:0x00;
      nv1 += m_GCLNCBusSensorEvents->IsChecked() ? 0x02:0x00;
      nv1 += m_GCLNSoD->IsChecked() ? 0x04:0x00;
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gcln nv1=0x%02X", nv1);
      varSet(1, nv1, false);
    }
    else if( m_GCLNSetIndex == 1 ) {
      int canid = m_GCLNCANID->GetValue();
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gcln nv2=0x%02X", canid);
      varSet(2, canid, false);
    }
    else if( m_GCLNSetIndex == 2 ) {
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "set gcln learn mode");
      setLearn();
    }
    else if( m_GCLNSetIndex == 3 ) {
      // SoD
      eventSet( 0, m_GCLNSoDAddr->GetValue(), m_GCLNSetIndex-3, 0, false );
    }
    else if( m_GCLNSetIndex == 4 ) {
      // Switch start
      eventSet( 0, m_GCLNSwitchFilterStart->GetValue(), m_GCLNSetIndex-3, 0, false );
    }
    else if( m_GCLNSetIndex == 5 ) {
      // Switch end
      eventSet( 0, m_GCLNSwitchFilterEnd->GetValue(), m_GCLNSetIndex-3, 0, false );
    }
    else if( m_GCLNSetIndex == 6 ) {
      // Sensor start
      eventSet( 0, m_GCLNSensorFilterStart->GetValue(), m_GCLNSetIndex-3, 0, false );
    }
    else if( m_GCLNSetIndex == 7 ) {
      // Switch end
      eventSet( 0, m_GCLNSensorFilterEnd->GetValue(), m_GCLNSetIndex-3, 0, false );
    }

    m_GCLNSetIndex++;
    if( m_bGCLNSetAll && m_GCLNSetIndex < 8 ) {
      m_Timer->Start( 100, wxTIMER_ONE_SHOT );
    }
    else {
      m_bGCLNSetAll = false;
      m_GCLNSetAll->Enable(true);
      m_GCLNGetAll->Enable(true);
      setUnlearn();
    }
  }

  else if( m_bGC6SetAll ) {
    if( m_GC6SetIndex == 0 ) {
      int nv1 = m_GC6SaveServoPos->IsChecked() ? 0x01:0x00;
      nv1 += m_GC6ShortEvents->IsChecked() ? 0x02:0x00;
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc6 nv1=0x%02X", nv1);
      varSet(1, nv1, false);
    }
    else if( m_GC6SetIndex == 1 ) {
      int canid = m_GC6CanID->GetValue();
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc6 nv2=0x%02X", canid);
      varSet(2, canid, false);
    }
    else if( m_GC6SetIndex == 2 ) {
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "set gc6 learn mode");
      setLearn();
    }
    else if( m_GC6SetIndex > 2 && m_GC6SetIndex < 7 ) {
      // Swithch events
      int servo = m_GC6SetIndex - 3;
      gc6UpdateServoEvent(servo);
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999,
          "set gc6 switch event %d servo %d", m_GC6SetIndex-3, servo);
      eventSet( m_Servo[servo].swnn, m_Servo[servo].swaddr, m_GC6SetIndex-3, 0, false );
    }
    else if( m_GC6SetIndex > 6 && m_GC6SetIndex < 11 ) {
      // Sensor events
      int servo = m_GC6SetIndex - 7;
      gc6UpdateServoEvent(servo);
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "set gc6 sensor event %d servo %d", m_GC6SetIndex-3, servo);
      eventSet( 0, m_Servo[servo].fbaddr, m_GC6SetIndex-3, 0, false );
    }
    else if( m_GC6SetIndex == 11 ) {
      // SOD
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "set gc6 SOD event %d", m_GC6SetIndex-3);
      eventSet( 0, m_GC6SOD->GetValue(), 8, 0, false );
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
  else if( m_bGC4SetAll ) {
    if( m_GC4SetIndex == 0 ) {
      int nv1 = m_GC4SaveRFID->IsChecked() ? 0x01:0x00;
      nv1 += m_GC4CheckRFID->IsChecked() ? 0x02:0x00;
      nv1 += m_GC4ReleaseRFID->IsChecked() ? 0x04:0x00;
      nv1 += m_GC4Acc4RFID->IsChecked() ? 0x08:0x00;
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc4 nv1=0x%02X", nv1);
      varSet(1, nv1, false);
    }
    else if( m_GC4SetIndex == 1 ) {
      int canid = m_GC4CANID->GetValue();
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc4 nv2=0x%02X", canid);
      varSet(2, canid, false);
    }
    else if( m_GC4SetIndex < 27 ) {
      int rfid = (m_GC4SetIndex - 2) / 5;
      int idx  = (m_GC4SetIndex - 2) % 5;
      wxTextCtrl* allowedRFID[] = {m_GC4AllowedRFID1,m_GC4AllowedRFID2,m_GC4AllowedRFID3,m_GC4AllowedRFID4,m_GC4AllowedRFID5};

      char* sval = StrOp.dup(allowedRFID[rfid]->GetValue().mb_str(wxConvUTF8));
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc4 rfid%d[%d]=%s", rfid, idx, sval);
      iOStrTok tok = StrTokOp.inst(sval, '.');
      int n = 0;
      while( StrTokOp.hasMoreTokens(tok) ) {
        const char* tval = StrTokOp.nextToken(tok);
        if( n == idx) {
          int val = atoi(tval);
          TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "gc4 nv%d=0x%02X", m_GC4SetIndex+1, val);
          varSet(m_GC4SetIndex+1, val, false);
          break;
        }
        n++;
      }
      StrTokOp.base.del(tok);
      StrOp.free(sval);
    }
    else if( m_GC4SetIndex == 27 ) {
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "set gc4 learn mode");
      setLearn();
    }
    else if( m_GC4SetIndex > 27 && m_GC4SetIndex < 36 ) {
      // rfid events
      wxSpinCtrl* rfidAddr[] = {m_GC4RFID1,m_GC4RFID2,m_GC4RFID3,m_GC4RFID4,m_GC4RFID5,m_GC4RFID6,m_GC4RFID7,m_GC4RFID8};
      int rfid = m_GC4SetIndex - 28;
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999,
          "set gc4 event %d rfid %d", rfid, rfidAddr[rfid]->GetValue());
      eventSet( 0, rfidAddr[rfid]->GetValue(), m_GC4SetIndex-28, 0, false );
    }
    else if( m_GC4SetIndex > 35 && m_GC4SetIndex < 44 ) {
      // block events
      int block = m_GC4SetIndex - 36;
      wxSpinCtrl* blockAddr[] = {m_GC4BK1,m_GC4BK2,m_GC4BK3,m_GC4BK4,m_GC4BK5,m_GC4BK6,m_GC4BK7,m_GC4BK8};
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999,
          "set gc4 event %d rfid %d", block, blockAddr[block]->GetValue());
      eventSet( 0, blockAddr[block]->GetValue(), m_GC4SetIndex-28, 0, false );
    }
    else if( m_GC4SetIndex == 44 ) {
      // SOD
      TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "set gc4 SOD event %d", m_GC4SetIndex-28);
      eventSet( 0, m_GC4SOD->GetValue(), m_GC4SetIndex-28, 0, false );
    }

    m_GC4SetIndex++;
    if( m_bGC4SetAll && m_GC4SetIndex < 44 ) {
      m_Timer->Start( 100, wxTIMER_ONE_SHOT );
    }
    else {
      m_bGC4SetAll = false;
      m_GC4SetAll->Enable(true);
      m_GC4GetAll->Enable(true);
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
    if( m_bGC2SetAll && m_GC2SetIndex < 22 ) {
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



void CBusNodeDlg::initGC1eVar( int nr, int val ) {
  wxSpinCtrl* gc1eip[] = {m_GC1eIP1,m_GC1eIP2,m_GC1eIP3,m_GC1eIP4};
  wxSpinCtrl* gc1emask[] = {m_GC1eNetmask1,m_GC1eNetmask2,m_GC1eNetmask3,m_GC1eNetmask4};
  wxSpinCtrl* gc1emac[] = {m_GC1eMAC1,m_GC1eMAC2,m_GC1eMAC3,m_GC1eMAC4,m_GC1eMAC5,m_GC1eMAC6};

  if( nr == 1 ) {
    // node var1
    m_GC1eIdleWD->SetValue( (val&0x01) ? true:false );
    m_GC1ePowerOffAtIdle->SetValue( (val&0x02) ? true:false );
    m_GC1eCommandAck->SetValue( (val&0x04) ? true:false );
    m_GC1eIdleTime->Enable( m_GC1eIdleWD->IsChecked() );
    m_GC1ePowerOffAtIdle->Enable( m_GC1eIdleWD->IsChecked() );
  }
  else if( nr == 2 ) {
    // canid
    m_GC1eCanID->SetValue( val);
  }
  else if( nr > 2 && nr < 7 ) {
    // ip address
    gc1eip[nr-3]->SetValue( val);
  }
  else if( nr > 6 && nr < 11 ) {
    // netmask
    gc1emask[nr-7]->SetValue( val);
  }
  else if( nr > 10 && nr < 17 ) {
    // mac address
    gc1emac[nr-11]->SetValue( val);
  }
  else if( nr == 17 ) {
    // canid
    m_GC1eIdleTime->SetValue( val);
  }
}



// GC6
void CBusNodeDlg::OnServoSelect( wxCommandEvent& event ) {
}

void CBusNodeDlg::OnServoLeftAngle( wxScrollEvent& event ) {
  wxSlider* slider[] = {m_GC6Servo1LeftAng, m_GC6Servo2LeftAng, m_GC6Servo3LeftAng, m_GC6Servo4LeftAng};
  for( int i = 0; i < 4; i++ ) {
    if( event.GetEventObject() == slider[i] ) {
      varSet( 4 + i*5, slider[i]->GetValue(), false );
      break;
    }
  }
}

void CBusNodeDlg::OnServoRightAngle( wxScrollEvent& event ) {
  wxSlider* slider[] = {m_GC6Servo1RightAng, m_GC6Servo2RightAng, m_GC6Servo3RightAng, m_GC6Servo4RightAng};
  for( int i = 0; i < 4; i++ ) {
    if( event.GetEventObject() == slider[i] ) {
      varSet( 5 + i*5, slider[i]->GetValue(), false );
      break;
    }
  }
}

void CBusNodeDlg::OnServoSpeed( wxScrollEvent& event ) {
  wxSlider* slider[] = {m_GC6Servo1Speed, m_GC6Servo2Speed, m_GC6Servo3Speed, m_GC6Servo4Speed};
  for( int i = 0; i < 4; i++ ) {
    if( event.GetEventObject() == slider[i] ) {
      varSet( 6 + i*5, slider[i]->GetValue(), false );
      break;
    }
  }

  wxSlider* sliderR[] = {m_GC6Servo1SpeedR, m_GC6Servo2SpeedR, m_GC6Servo3SpeedR, m_GC6Servo4SpeedR};
  for( int i = 0; i < 4; i++ ) {
    if( event.GetEventObject() == sliderR[i] ) {
      varSet( 7 + i*5, sliderR[i]->GetValue(), false );
      break;
    }
  }
}

void CBusNodeDlg::OnServoRelay( wxCommandEvent& event ) {
  wxCheckBox* cbRelay[] = {m_GC6Servo1Relay, m_GC6Servo2Relay, m_GC6Servo3Relay, m_GC6Servo4Relay};
  wxCheckBox* cbExtFb[] = {m_GC6Servo1ExtFb, m_GC6Servo2ExtFb, m_GC6Servo3ExtFb, m_GC6Servo4ExtFb};
  wxCheckBox* cbBounce[] = {m_GC6Servo1Bounce, m_GC6Servo2Bounce, m_GC6Servo3Bounce, m_GC6Servo4Bounce};
  for( int i = 0; i < 4; i++ ) {
    if( event.GetEventObject() == cbRelay[i] || event.GetEventObject() == cbExtFb[i] || event.GetEventObject() == cbBounce[i] ) {
      int conf = cbRelay[i]->IsChecked() ? 0x01:0x00;
      conf += cbExtFb[i]->IsChecked()    ? 0x02:0x00;
      conf += cbBounce[i]->IsChecked()   ? 0x04:0x00;
      varSet( 3 + i*5, conf, false );
      break;
    }
  }
}

void CBusNodeDlg::OnExtSensors( wxCommandEvent& event ) {
  OnServoRelay( event );
}

void CBusNodeDlg::OnBounce( wxCommandEvent& event ) {
  OnServoRelay( event );
}

void CBusNodeDlg::onGC6GetAll( wxCommandEvent& event ) {
  m_bGC6GetAll = true;
  m_GC6SetAll->Enable(false);
  m_GC6GetAll->Enable(false);
  varGet(1);
}

void CBusNodeDlg::onGC6SetAll( wxCommandEvent& event ) {
  m_bGC1eSetAll = false;
  m_bGC2SetAll = false;
  m_bGC4SetAll = false;
  m_bGC6SetAll = true;
  m_GC6SetAll->Enable(false);
  m_GC6GetAll->Enable(false);
  m_GC6SetIndex = 0;
  m_Timer->Start( 100, wxTIMER_ONE_SHOT );
}

void CBusNodeDlg::onGC4GetAll( wxCommandEvent& event ) {
  m_bGC1eGetAll = false;
  m_bGC2GetAll = false;
  m_bGC4GetAll = true;
  m_bGC6GetAll = false;
  m_GC4SetAll->Enable(false);
  m_GC4GetAll->Enable(false);
  varGet(1);
}

void CBusNodeDlg::onGC4SetAll( wxCommandEvent& event ) {
  m_bGC1eSetAll = false;
  m_bGC2SetAll = false;
  m_bGC4SetAll = true;
  m_bGC6SetAll = false;
  m_GC4SetAll->Enable(false);
  m_GC4GetAll->Enable(false);
  m_GC4SetIndex = 0;
  m_Timer->Start( 100, wxTIMER_ONE_SHOT );
}

void CBusNodeDlg::onGC1eGetAll( wxCommandEvent& event ) {
  m_bGC1eGetAll = true;
  m_bGC2GetAll = false;
  m_bGC4GetAll = false;
  m_bGC6GetAll = false;
  m_GC1eSetAll->Enable(false);
  m_GC1eGetAll->Enable(false);
  varGet(1);
}

void CBusNodeDlg::onGC1eSetAll( wxCommandEvent& event ) {
  m_bGC1eSetAll = true;
  m_bGC2SetAll = false;
  m_bGC4SetAll = false;
  m_bGC6SetAll = false;
  m_GC1eSetAll->Enable(false);
  m_GC1eGetAll->Enable(false);
  m_GC1eSetIndex = 0;
  m_Timer->Start( 100, wxTIMER_ONE_SHOT );
}

void CBusNodeDlg::onGC1eIdleWatchDog( wxCommandEvent& event ) {
  m_GC1eIdleTime->Enable( m_GC1eIdleWD->IsChecked() );
  m_GC1ePowerOffAtIdle->Enable( m_GC1eIdleWD->IsChecked() );
}


void CBusNodeDlg::onGC2PortType1( wxCommandEvent& event ) {
  m_GC2EvtNN1->Enable(m_GC2Switch1->GetValue() | m_GC2Pulse1->GetValue());
  if( m_GC2Input1->GetValue() || m_GC2Block1->GetValue() )
    m_GC2EvtNN1->SetValue(0);
}
void CBusNodeDlg::onGC2PortType2( wxCommandEvent& event ) {
  m_GC2EvtNN2->Enable(m_GC2Switch2->GetValue() | m_GC2Pulse2->GetValue());
  if( m_GC2Input2->GetValue() || m_GC2Block2->GetValue() )
    m_GC2EvtNN2->SetValue(0);
}
void CBusNodeDlg::onGC2PortType3( wxCommandEvent& event ) {
  m_GC2EvtNN3->Enable(m_GC2Switch3->GetValue() | m_GC2Pulse3->GetValue());
  if( m_GC2Input3->GetValue() || m_GC2Block3->GetValue() )
    m_GC2EvtNN3->SetValue(0);
}
void CBusNodeDlg::onGC2PortType4( wxCommandEvent& event ) {
  m_GC2EvtNN4->Enable(m_GC2Switch4->GetValue() | m_GC2Pulse4->GetValue());
  if( m_GC2Input4->GetValue() || m_GC2Block4->GetValue() )
    m_GC2EvtNN4->SetValue(0);
}
void CBusNodeDlg::onGC2PortType5( wxCommandEvent& event ) {
  m_GC2EvtNN5->Enable(m_GC2Switch5->GetValue() | m_GC2Pulse5->GetValue());
  if( m_GC2Input5->GetValue() || m_GC2Block5->GetValue() )
    m_GC2EvtNN5->SetValue(0);
}
void CBusNodeDlg::onGC2PortType6( wxCommandEvent& event ) {
  m_GC2EvtNN6->Enable(m_GC2Switch6->GetValue() | m_GC2Pulse6->GetValue());
  if( m_GC2Input6->GetValue() || m_GC2Block6->GetValue() )
    m_GC2EvtNN6->SetValue(0);
}
void CBusNodeDlg::onGC2PortType7( wxCommandEvent& event ) {
  m_GC2EvtNN7->Enable(m_GC2Switch7->GetValue() | m_GC2Pulse7->GetValue());
  if( m_GC2Input7->GetValue() || m_GC2Block7->GetValue() )
    m_GC2EvtNN7->SetValue(0);
}
void CBusNodeDlg::onGC2PortType8( wxCommandEvent& event ) {
  m_GC2EvtNN8->Enable(m_GC2Switch8->GetValue() | m_GC2Pulse8->GetValue());
  if( m_GC2Input8->GetValue() || m_GC2Block8->GetValue() )
    m_GC2EvtNN8->SetValue(0);
}
void CBusNodeDlg::onGC2PortType9( wxCommandEvent& event ) {
  m_GC2EvtNN9->Enable(m_GC2Switch9->GetValue() | m_GC2Pulse9->GetValue());
  if( m_GC2Input9->GetValue() || m_GC2Block9->GetValue() )
    m_GC2EvtNN9->SetValue(0);
}
void CBusNodeDlg::onGC2PortType10( wxCommandEvent& event ) {
  m_GC2EvtNN10->Enable(m_GC2Switch10->GetValue() | m_GC2Pulse10->GetValue());
  if( m_GC2Input10->GetValue() || m_GC2Block10->GetValue() )
    m_GC2EvtNN10->SetValue(0);
}
void CBusNodeDlg::onGC2PortType11( wxCommandEvent& event ) {
  m_GC2EvtNN11->Enable(m_GC2Switch11->GetValue() | m_GC2Pulse11->GetValue());
  if( m_GC2Input11->GetValue() || m_GC2Block11->GetValue() )
    m_GC2EvtNN11->SetValue(0);
}
void CBusNodeDlg::onGC2PortType12( wxCommandEvent& event ) {
  m_GC2EvtNN12->Enable(m_GC2Switch12->GetValue() | m_GC2Pulse12->GetValue());
  if( m_GC2Input12->GetValue() || m_GC2Block12->GetValue() )
    m_GC2EvtNN12->SetValue(0);
}
void CBusNodeDlg::onGC2PortType13( wxCommandEvent& event ) {
  m_GC2EvtNN13->Enable(m_GC2Switch13->GetValue() | m_GC2Pulse13->GetValue());
  if( m_GC2Input13->GetValue() || m_GC2Block13->GetValue() )
    m_GC2EvtNN13->SetValue(0);
}
void CBusNodeDlg::onGC2PortType14( wxCommandEvent& event ) {
  m_GC2EvtNN14->Enable(m_GC2Switch14->GetValue() | m_GC2Pulse14->GetValue());
  if( m_GC2Input14->GetValue() || m_GC2Block14->GetValue() )
    m_GC2EvtNN14->SetValue(0);
}
void CBusNodeDlg::onGC2PortType15( wxCommandEvent& event ) {
  m_GC2EvtNN15->Enable(m_GC2Switch15->GetValue() | m_GC2Pulse15->GetValue());
  if( m_GC2Input15->GetValue() || m_GC2Block15->GetValue() )
    m_GC2EvtNN15->SetValue(0);
}
void CBusNodeDlg::onGC2PortType16( wxCommandEvent& event ) {
  m_GC2EvtNN16->Enable(m_GC2Switch16->GetValue() | m_GC2Pulse16->GetValue());
  if( m_GC2Input16->GetValue() || m_GC2Block16->GetValue() )
    m_GC2EvtNN16->SetValue(0);
}


void CBusNodeDlg::onGCLNGetAll( wxCommandEvent& event ) {
  m_bGC1eGetAll = false;
  m_bGC2GetAll = false;
  m_bGC4GetAll = false;
  m_bGC6GetAll = false;
  m_bGCLNGetAll = true;
  m_GCLNSetAll->Enable(false);
  m_GCLNGetAll->Enable(false);
  varGet(1);
}

void CBusNodeDlg::onGCLNSetAll( wxCommandEvent& event ) {
  m_bGC1eSetAll = false;
  m_bGC2SetAll = false;
  m_bGC4SetAll = false;
  m_bGC6SetAll = false;
  m_bGCLNSetAll = true;
  m_GCLNSetAll->Enable(false);
  m_GCLNGetAll->Enable(false);
  m_GCLNSetIndex = 0;
  m_Timer->Start( 100, wxTIMER_ONE_SHOT );
}


void CBusNodeDlg::onGC8GetAll( wxCommandEvent& event ) {
  m_bGC8GetAll = true;
  m_GC8SetAll->Enable(false);
  m_GC8GetAll->Enable(false);
  varGet(1);
}

void CBusNodeDlg::onGC8SetAll( wxCommandEvent& event ) {
  m_bGC8SetAll = true;
  m_GC8SetAll->Enable(false);
  m_GC8GetAll->Enable(false);
  m_GC8SetIndex = 0;
  m_Timer->Start( 100, wxTIMER_ONE_SHOT );
}



void CBusNodeDlg::sortOnColumn( int col ) {
  m_SortCol = col;
  initIndex();
}


void CBusNodeDlg::onIndexLeftClick( wxListEvent& event ) {
  sortOnColumn(event.GetColumn());
}
