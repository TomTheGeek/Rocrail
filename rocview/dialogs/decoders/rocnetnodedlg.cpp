/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "rocnetnodedlg.h"
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/dnd.h>
#include <wx/filedlg.h>


#include "rocview/public/guiapp.h"
#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/Release.h"

#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/RocNet.h"
#include "rocrail/wrapper/public/RocNetNode.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/ZLevel.h"
#include "rocrail/wrapper/public/Macro.h"
#include "rocrail/wrapper/public/MacroLine.h"
#include "rocs/public/strtok.h"
#include "rocutils/public/vendors.h"

#include "rocview/res/icons.hpp"

static int __evaluate( const char* release, char** info ) {
  TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "parsing release info:\n%s", release );
  iODoc doc = DocOp.parse( release );
  iONode releaseNode = NULL;
  int version = 0;
  if( doc != NULL ) {
    releaseNode = DocOp.getRootNode(doc);
    if( releaseNode != NULL ) {
      version = atoi(wRelease.getversion(releaseNode));
      *info = StrOp.dup(wRelease.getremark(releaseNode));
      TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "release info %d %s", version, info );
    }
    else {
      TraceOp.trc( "updates", TRCLEVEL_WARNING, __LINE__, 9999, "empty release info?" );
    }
    DocOp.base.del(doc);
  }
  else {
    TraceOp.trc( "updates", TRCLEVEL_WARNING, __LINE__, 9999, "could not parse release info" );
  }
  return version;
}


static void __updateReaderThread( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  RocnetNodeDlg* o = (RocnetNodeDlg*)ThreadOp.getParm( th );
  bool newPatches = false;

  TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "updateReaderThread started" );
  if( o == NULL ) {
    ThreadOp.sleep(5000);
  }
  else
    ThreadOp.sleep(100);

  TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "updateReaderThread try to connect with %s", wGui.getupdatesserver(wxGetApp().getIni()) );
  iOSocket sh = SocketOp.inst( wGui.getupdatesserver(wxGetApp().getIni()), 80, False, False, False );
  if( SocketOp.connect( sh ) ) {
    TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "Connected to rocrail.net" );

    char* releasename = StrOp.dup(wGui.releasename);
    StrOp.replaceAll( releasename, ' ', '\0');
    // http://rocrail.net/software/rocrail-snapshot/raspi/rocnetnode.xml
    char* httpget = StrOp.fmt("GET /software/rocrail-snapshot/raspi/rocnetnode.xml HTTP/1.1\nHost: www.rocrail.net\n\n",
        StrOp.strlwr( releasename ) );
    TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "%s", httpget );
    SocketOp.write( sh, httpget, StrOp.len(httpget) );
    StrOp.free(releasename);
    StrOp.free(httpget);

    TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "Read response..." );
    char str[1024] = {'\0'};
    SocketOp.setRcvTimeout( sh, 1000 );
    /* Read first HTTP header line: */
    SocketOp.readln( sh, str );
    TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, str );
    /* Reading rest of HTTP header: */

    int contlen = 0;
    while( SocketOp.isConnected(sh) && !SocketOp.isBroken( sh ) && SocketOp.readln( sh, str ) ) {
      if( str[0] == '\r' || str[0] == '\n' ) {
        break;
      }
      if( StrOp.find( str, "Content-Length:" ) ) {
        char* p = StrOp.find( str, ": " ) + 2;
        contlen = atoi( p );
        TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "contlen = %d", contlen );
      }
      TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, str );
    };
    if( SocketOp.isConnected(sh) && !SocketOp.isBroken( sh ) && contlen > 0 ) {
      char* release = (char*)allocMem(contlen+1);
      SocketOp.read( sh, release, contlen );
      TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, release );
      int version = __evaluate( release, &o->m_VersionInfo );
      if( o != NULL ) {
        o->m_AvailableVersion = version;
        o->Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( RocnetNodeDlg::onUpdateVersion ), NULL, o );
        wxCommandEvent cmd( wxEVT_COMMAND_MENU_SELECTED, -1 );
        wxPostEvent( o, cmd );
      }
    }

    SocketOp.disConnect(sh);
  }
  else {
    TraceOp.trc( "updates", TRCLEVEL_WARNING, __LINE__, 9999, "could not connected to %s",
        wGui.getupdatesserver(wxGetApp().getIni()) );

  }

  SocketOp.base.del(sh);
  ThreadOp.base.del(th);
  TraceOp.trc( "updates", TRCLEVEL_INFO, __LINE__, 9999, "cleaned up thread" );
}



RocnetNodeDlg::RocnetNodeDlg( wxWindow* parent, iONode ini )
  :rocnetnodegen( parent )
{
  m_Ini = ini;
  m_Digint = NULL;
  m_Props = NULL;
  m_PortGroup = 0;
  m_ChannelGroup = 0;
  m_SortCol  = 0;
  m_NodeMap = MapOp.inst();
  m_TreeItemMap = MapOp.inst();
  m_TreeLocationMap = MapOp.inst();
  m_SelectedNode = NULL;
  m_SelectedZLevel = NULL;
  m_AvailableVersion = 0;
  m_VersionInfo = NULL;

  __initVendors();
  m_NodeBook->SetSelection(0);

  m_GCALogo->SetBitmap(*_img_gca);
  m_GCALogo->SetToolTip(wxT("http://www.phgiling.net/"));
  m_RocNetLogo->SetBitmap(*_img_rocnet_logo);
  m_RocNetLogo->SetToolTip(wxT("http://wiki.rocrail.net/"));


  initLabels();
  // Resize
  m_IndexPanel->GetSizer()->Layout();
  m_RocNetPanel->GetSizer()->Layout();
  m_OptionsPanel->GetSizer()->Layout();
  m_PortSetupPanel->GetSizer()->Layout();
  m_MacroPanel->GetSizer()->Layout();
  m_UpdatePanel->GetSizer()->Layout();

  m_NodeBook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);


  initListLabels();
  initNodeList();
}

RocnetNodeDlg::~RocnetNodeDlg() {
  if( m_VersionInfo != NULL )
    StrOp.free(m_VersionInfo);

  MapOp.base.del(m_NodeMap);
  MapOp.base.del(m_TreeItemMap);
  MapOp.base.del(m_TreeLocationMap);
}

void RocnetNodeDlg::onRocnetWrite( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.nnreq );
  wProgram.setvalue(cmd, m_ID->GetValue() );
  wProgram.setval1(cmd, m_Location->GetValue() );
  wProgram.setval2(cmd, wRocNetNode.getsubip(m_Props));
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void RocnetNodeDlg::initPorts() {
  wxRadioBox* l_Type[] = {NULL, m_Type1, m_Type2, m_Type3, m_Type4, m_Type5, m_Type6, m_Type7, m_Type8};
  wxSpinCtrl* l_Delay[] = { NULL, m_Delay1, m_Delay2, m_Delay3, m_Delay4, m_Delay5, m_Delay6, m_Delay7, m_Delay8};
  wxStaticText* l_labPort[] = { NULL, m_labPort1, m_labPort2, m_labPort3, m_labPort4, m_labPort5, m_labPort6, m_labPort7, m_labPort8 };
  wxCheckBox* l_Blink[] = {NULL, m_Blink1, m_Blink2, m_Blink3, m_Blink4, m_Blink5, m_Blink6, m_Blink7, m_Blink8};
  wxCheckBox* l_Invert[] = {NULL, m_PortInv1, m_PortInv2, m_PortInv3, m_PortInv4, m_PortInv5, m_PortInv6, m_PortInv7, m_PortInv8};
  wxCheckBox* l_Toggle[] = {NULL, m_PortToggle1, m_PortToggle2, m_PortToggle3, m_PortToggle4, m_PortToggle5, m_PortToggle6, m_PortToggle7, m_PortToggle8};
  wxSpinCtrl* l_EventID[] = {NULL, m_PortEventID1, m_PortEventID2, m_PortEventID3, m_PortEventID4, m_PortEventID5, m_PortEventID6, m_PortEventID7, m_PortEventID8};
  wxSpinCtrl* l_EventPort[] = {NULL, m_PortEventPort1, m_PortEventPort2, m_PortEventPort3, m_PortEventPort4, m_PortEventPort5, m_PortEventPort6, m_PortEventPort7, m_PortEventPort8};
  for( int i = 1; i < 9; i++ ) {
    l_labPort[i]->SetLabel( wxString::Format(wxT("%d"),i + m_PortGroup*8) );
    l_Type[i]->SetSelection(0);
    l_Delay[i]->SetValue(0);
    l_Blink[i]->SetValue(false);
    l_Invert[i]->SetValue(false);
    l_Toggle[i]->SetValue(false);
    l_EventID[i]->SetValue(0);
    l_EventPort[i]->SetValue(0);
  }
}

void RocnetNodeDlg::onPortPrev( wxCommandEvent& event )
{
  if( m_PortGroup > 0 ) {
    m_PortGroup--;
    initPorts();
    wxCommandEvent cmd;
    onPortRead(cmd);
  }
}

void RocnetNodeDlg::onPortNext( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;
  int nrio = wRocNetNode.getnrio(m_Props);
  if(nrio > (m_PortGroup+1) * 8) {
    m_PortGroup++;
    initPorts();
    wxCommandEvent cmd;
    onPortRead(cmd);
  }
}

void RocnetNodeDlg::onPortRead( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.nvget );
  wProgram.setval1(cmd, 1 + m_PortGroup*8 ); // range
  wProgram.setval2(cmd, 8 + m_PortGroup*8 );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocnetNodeDlg::onPortWrite( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.nvset );

  wxRadioBox* l_Type[] = {NULL, m_Type1, m_Type2, m_Type3, m_Type4, m_Type5, m_Type6, m_Type7, m_Type8};
  wxSpinCtrl* l_Delay[] = { NULL, m_Delay1, m_Delay2, m_Delay3, m_Delay4, m_Delay5, m_Delay6, m_Delay7, m_Delay8};
  wxCheckBox* l_Blink[] = {NULL, m_Blink1, m_Blink2, m_Blink3, m_Blink4, m_Blink5, m_Blink6, m_Blink7, m_Blink8};
  wxCheckBox* l_Invert[] = {NULL, m_PortInv1, m_PortInv2, m_PortInv3, m_PortInv4, m_PortInv5, m_PortInv6, m_PortInv7, m_PortInv8};
  wxCheckBox* l_Toggle[] = {NULL, m_PortToggle1, m_PortToggle2, m_PortToggle3, m_PortToggle4, m_PortToggle5, m_PortToggle6, m_PortToggle7, m_PortToggle8};
  wxSpinCtrl* l_EventID[] = {NULL, m_PortEventID1, m_PortEventID2, m_PortEventID3, m_PortEventID4, m_PortEventID5, m_PortEventID6, m_PortEventID7, m_PortEventID8};
  wxSpinCtrl* l_EventPort[] = {NULL, m_PortEventPort1, m_PortEventPort2, m_PortEventPort3, m_PortEventPort4, m_PortEventPort5, m_PortEventPort6, m_PortEventPort7, m_PortEventPort8};

  char key[32] = {'\0'};
  for( int i = 0; i < 8; i++ ) {
    StrOp.fmtb(key, "val%d", 1 + i*4);
    NodeOp.setInt( cmd, key, m_PortGroup*8 + 1 + i);
    //StrOp.fmtb(key, "val%d", 2 + i*4);
    // reserved
    StrOp.fmtb(key, "val%d", 3 + i*4);
    NodeOp.setInt( cmd, key, l_Type[1 + i]->GetSelection() + (l_Blink[1 + i]->IsChecked()?0x80:0x00) +
        (l_Invert[1 + i]->IsChecked()?0x40:0x00) + (l_Toggle[1 + i]->IsChecked()?0x20:0x00) );
    StrOp.fmtb(key, "val%d", 4 + i*4);
    NodeOp.setInt( cmd, key, l_Delay[1 + i]->GetValue() );
  }

  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );

  ThreadOp.sleep(100);
  wProgram.setcmd( cmd, wProgram.evset );

  for( int i = 0; i < 8; i++ ) {
    StrOp.fmtb(key, "val%d", 1 + i*4);
    NodeOp.setInt( cmd, key, m_PortGroup*8 + 1 + i);
    StrOp.fmtb(key, "val%d", 2 + i*4);
    NodeOp.setInt( cmd, key, l_EventID[1 + i]->GetValue() / 256);
    StrOp.fmtb(key, "val%d", 3 + i*4);
    NodeOp.setInt( cmd, key, l_EventID[1 + i]->GetValue() % 256);
    StrOp.fmtb(key, "val%d", 4 + i*4);
    NodeOp.setInt( cmd, key, l_EventPort[1 + i]->GetValue() );
  }

  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocnetNodeDlg::onOK( wxCommandEvent& event ) {
  //EndModal( wxID_OK );
  wxGetApp().getFrame()->resetRocNetNodeRef();
  Destroy();
}

void RocnetNodeDlg::onClose( wxCloseEvent& event ) {
  //EndModal( wxID_OK );
  wxGetApp().getFrame()->resetRocNetNodeRef();
  Destroy();
}


void RocnetNodeDlg::initListLabels() {
  m_NodeList->InsertColumn(0, wxGetApp().getMsg( "id" ), wxLIST_FORMAT_RIGHT );
  m_NodeList->InsertColumn(1, wxGetApp().getMsg( "location" ), wxLIST_FORMAT_RIGHT );
  m_NodeList->InsertColumn(2, wxGetApp().getMsg( "vendor" ), wxLIST_FORMAT_LEFT );
  m_NodeList->InsertColumn(3, wxGetApp().getMsg( "class" ), wxLIST_FORMAT_LEFT );
  m_NodeList->InsertColumn(4, wxGetApp().getMsg( "revision" ), wxLIST_FORMAT_RIGHT );
  m_NodeList->InsertColumn(5, wxT("I/O"), wxLIST_FORMAT_RIGHT );
  m_NodeList->InsertColumn(6, wxT("Sub IP"), wxLIST_FORMAT_CENTER);
}


void RocnetNodeDlg::initLabels() {
  m_NodeBook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  //m_NodeBook->SetPageText( 1, wxGetApp().getMsg( "rocnet" ) );
  m_NodeBook->SetPageText( 2, wxGetApp().getMsg( "options" ) );
  m_NodeBook->SetPageText( 3, wxGetApp().getMsg( "portsetup" ) );
  m_NodeBook->SetPageText( 4, wxGetApp().getMsg( "channelsetup" ) );
  m_NodeBook->SetPageText( 5, wxGetApp().getMsg( "macro" ) );
  m_NodeBook->SetPageText( 6, wxGetApp().getMsg( "update" ) );

  iONode l_RocrailIni = wxGetApp().getFrame()->getRocrailIni();
  if( l_RocrailIni != NULL ) {
    iONode digint = wRocRail.getdigint(l_RocrailIni);
    while( digint != NULL ) {
      iONode rocnet = wDigInt.getrocnet(digint);
      if( rocnet != NULL ) {
        m_IID->SetValue( wxString( wDigInt.getiid(digint), wxConvUTF8) );
        m_Digint = digint;
        break;
      }
      digint = wRocRail.nextdigint(l_RocrailIni, digint);
    }
  }

  // Index
  m_Shutdown->SetLabel(wxGetApp().getMsg( "shutdownserver" ));
  m_ShutdownAll->SetLabel(wxGetApp().getMsg( "shutdownall" ));
  m_Show->SetLabel(wxGetApp().getMsg( "show" ));
  m_Query->SetLabel(wxGetApp().getMsg( "query" ));

  // RocNet
  m_labNodemap->SetLabel(wxGetApp().getMsg( "nodemap" ));
  m_labIID->SetLabel(wxGetApp().getMsg( "iid" ));
  m_labID->SetLabel(wxGetApp().getMsg( "id" ));
  m_labLocation->SetLabel(wxGetApp().getMsg( "location" ));
  m_labVendor->SetLabel(wxGetApp().getMsg( "vendor" ));
  m_labProduct->SetLabel(wxGetApp().getMsg( "class" ));
  m_labVersion->SetLabel(wxGetApp().getMsg( "revision" ));
  m_labUpdate->SetLabel(wxGetApp().getMsg( "newrevision" ));
  m_Update->SetLabel(wxGetApp().getMsg( "poll" ));
  m_RocnetWrite->SetLabel(wxGetApp().getMsg( "set" ));
  m_UpdateOffline->SetLabel(wxGetApp().getMsg( "file" ));

  // Options
  m_RocNetOptionBox->GetStaticBox()->SetLabel(wxGetApp().getMsg( "options" ));
  m_SecAck->SetLabel(wxGetApp().getMsg( "secureack" ));
  m_UsePB->SetLabel(wxGetApp().getMsg( "usepb" ));
  m_DCCType->SetLabel(wxGetApp().getMsg( "controller" ));
  m_DCCType->SetString( 0, wxGetApp().getMsg( "none" ) );
  m_DCCDevice->SetLabel(wxGetApp().getMsg( "device" ));
  m_NodeOptionsRead->SetLabel(wxGetApp().getMsg( "get" ));
  m_NodeOptionsWrite->SetLabel(wxGetApp().getMsg( "set" ));

  // Port setup
  m_labPort->SetLabel(wxGetApp().getMsg( "port" ));
  m_labType->SetLabel(wxGetApp().getMsg( "type" ));
  m_labDelay->SetLabel(wxGetApp().getMsg( "delay" ));
  m_labBlink->SetLabel(wxGetApp().getMsg( "blink" ));
  m_labPortInv->SetLabel(wxGetApp().getMsg( "invert" ));
  m_PortRead->SetLabel(wxGetApp().getMsg( "get" ));
  m_PortWrite->SetLabel(wxGetApp().getMsg( "set" ));
  m_PortRemove->SetLabel(wxGetApp().getMsg( "delete" ));
  m_labPortEventID->SetLabel(wxGetApp().getMsg( "id" ));
  m_labPortEventPort->SetLabel(wxGetApp().getMsg( "port" ));
  m_labPortEventID->SetToolTip(wxGetApp().getMsg( "event" ));
  m_labPortEventPort->SetToolTip(wxGetApp().getMsg( "event" ));
  wxCommandEvent cmdevt;
  onIOType(cmdevt);

  // Macro
  m_labMacroNr->SetLabel(wxGetApp().getMsg( "number" ));
  m_MacroLines->SetColLabelValue(0, wxGetApp().getMsg( "port" ));
  m_MacroLines->SetColLabelValue(1, wxGetApp().getMsg( "delay" ));
  m_MacroLines->SetColLabelValue(2, wxGetApp().getMsg( "type" ));
  m_MacroLines->SetColLabelValue(3, wxGetApp().getMsg( "value" ));
  m_MacroLines->SetColLabelValue(4, wxGetApp().getMsg( "blink" ));
  for( int i = 0; i < 8; i++ ) {
    m_MacroLines->SetCellRenderer(i, 4, new wxGridCellBoolRenderer);
  }
  m_MacroGet->SetLabel(wxGetApp().getMsg( "get" ));
  m_MacroSet->SetLabel(wxGetApp().getMsg( "set" ));
  m_MacroExport->SetLabel(wxGetApp().getMsg( "export" ) + wxT("..."));
  m_MacroImport->SetLabel(wxGetApp().getMsg( "import" ) + wxT("..."));
}


/* comparator for sorting by id: */
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    int A = wRocNetNode.getid( a );
    int B = wRocNetNode.getid( b );
    if( A > B )
      return 1;
    if( A < B )
      return -1;
    return 0;
}
static int __sortLocation(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    int A = wRocNetNode.getlocation( a );
    int B = wRocNetNode.getlocation( b );
    if( A > B )
      return 1;
    if( A < B )
      return -1;
    return 0;
}
static int __sortSubIP(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    int A = wRocNetNode.getsubip( a );
    int B = wRocNetNode.getsubip( b );
    if( A > B )
      return 1;
    if( A < B )
      return -1;
    return 0;
}

iONode RocnetNodeDlg::getZLevel(int level, char* sLevel) {
  iONode zlevel = wPlan.getzlevel( wxGetApp().getModel() );
  while( zlevel != NULL ) {
    if( wZLevel.getz(zlevel) == level ) {
      StrOp.copy( sLevel, wZLevel.gettitle(zlevel) );
      return zlevel;
    }
    zlevel = wPlan.nextzlevel( wxGetApp().getModel(), zlevel );
  };

  StrOp.fmtb(sLevel, "%d", level);
  return NULL;
}

void RocnetNodeDlg::initNodeList() {
  m_NodeList->DeleteAllItems();

  if( m_Digint == NULL )
    return;

  iOList list = ListOp.inst();
  iONode rnnode = wRocNet.getrocnetnode(m_Digint);
  while( rnnode != NULL ) {
    ListOp.add(list, (obj)rnnode);
    rnnode = wRocNet.nextrocnetnode(m_Digint, rnnode);
  }

  if( m_SortCol == 1 ) {
    ListOp.sort(list, &__sortLocation);
  }
  else if( m_SortCol == 6 ) {
    ListOp.sort(list, &__sortSubIP);
  }
  else {
    ListOp.sort(list, &__sortID);
  }

  for( int index = 0; index < ListOp.size(list); index++ ) {
    iONode rnnode = (iONode)ListOp.get(list, index);
    m_NodeList->InsertItem( index, wxString::Format(_T("%d"), wRocNetNode.getid(rnnode)));
    m_NodeList->SetItem( index, 1, wxString::Format(_T("%d"), wRocNetNode.getlocation(rnnode)));
    m_NodeList->SetItem( index, 2, wxString( m_Vendor[wRocNetNode.getvendor(rnnode)&0xFF],wxConvUTF8) );
    m_NodeList->SetItem( index, 3, wxString(wRocNetNode.getmnemonic(rnnode),wxConvUTF8));
    m_NodeList->SetItem( index, 4, wxString::Format(_T("%d"), wRocNetNode.getrevision(rnnode)));
    m_NodeList->SetItem( index, 5, wxString::Format(_T("%d"), wRocNetNode.getnrio(rnnode)));
    m_NodeList->SetItem( index, 6, wxString::Format(_T("%d.%d"), wRocNetNode.getsubip(rnnode)/256, wRocNetNode.getsubip(rnnode)%256));
    m_NodeList->SetItemPtrData(index, (wxUIntPtr)rnnode);
  }

  for( int n = 0; n < 6; n++ ) {
    m_NodeList->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
    int autoheadersize = m_NodeList->GetColumnWidth(n);
    m_NodeList->SetColumnWidth(n, wxLIST_AUTOSIZE);
    int autosize = m_NodeList->GetColumnWidth(n);
    if(autoheadersize > autosize )
      m_NodeList->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
  }

  m_NodeTree->DeleteAllItems();
  MapOp.clear(m_NodeMap);
  MapOp.clear(m_TreeItemMap);
  MapOp.clear(m_TreeLocationMap);
  wxTreeItemId root  = m_NodeTree->AddRoot(wxString(wPlan.gettitle(wxGetApp().getModel()), wxConvUTF8));
  iOMap locationMap = MapOp.inst();
  for( int i = 0; i < ListOp.size(list); i++ ) {
    iONode rnnode = (iONode)ListOp.get(list, i);
    char location[256] = {'\0'};
    iONode zlevel = getZLevel(wRocNetNode.getlocation(rnnode), location);
    wxTreeItemId* plocation = (wxTreeItemId*)MapOp.get( locationMap, location );
    wxTreeItemId cat;
    if( plocation == NULL ) {
      cat = m_NodeTree->AppendItem( root, wxString( location, wxConvUTF8));
      plocation = &cat;
      MapOp.put(locationMap, location, (obj)new wxTreeItemId(cat.m_pItem) );
      MapOp.put(m_TreeLocationMap, location, (obj)zlevel );
    }
    else {
      cat = *plocation;
    }
    char key[256] = {'\0'};
    StrOp.fmtb(key, "[%s] %05d", wRocNetNode.getmnemonic(rnnode), wRocNetNode.getid(rnnode) );
    MapOp.put( m_NodeMap, key, (obj)rnnode);
    wxTreeItemId child = m_NodeTree->AppendItem( cat, wxString(key,wxConvUTF8) );
    MapOp.put(m_TreeItemMap, key, (obj)new wxTreeItemId(child.m_pItem) );
  }
  MapOp.base.del(locationMap);

  m_NodeTree->ExpandAll();

  /* clean up the temp. list */
  ListOp.base.del(list);

  if(m_NodeList->GetItemCount() > 0 ) {
    m_NodeList->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    m_Props = (iONode)m_NodeList->GetItemData(0);
  }

}

void RocnetNodeDlg::onIndexSelected( wxListEvent& event ) {
  int index = event.GetIndex();
  m_Props = (iONode)m_NodeList->GetItemData(index);
  if( m_Props != NULL ) {
    selChanged(m_Props);

    char key[256] = {'\0'};
    StrOp.fmtb(key, "[%s] %05d", wRocNetNode.getmnemonic(m_Props), wRocNetNode.getid(m_Props) );
    wxTreeItemId* pchild = (wxTreeItemId*)MapOp.get( m_TreeItemMap, key );
    if( pchild != NULL )
      m_NodeTree->SelectItem(*pchild);
  }
  else
    TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );

}

void RocnetNodeDlg::initValues() {
  m_ID->SetValue( wRocNetNode.getid(m_Props) );
  m_Location->SetValue( wRocNetNode.getlocation(m_Props) );
  m_VendorName->SetValue( wxString( m_Vendor[wRocNetNode.getvendor(m_Props)&0xFF],wxConvUTF8) );
  m_ProductName->SetValue( wxString(wRocNetNode.getclass(m_Props),wxConvUTF8) );
  m_Version->SetValue( wxString::Format(_T("%d"), wRocNetNode.getrevision(m_Props)) );
  m_SubIP->SetValue(wxString::Format(_T("%d.%d"), wRocNetNode.getsubip(m_Props)/256, wRocNetNode.getsubip(m_Props)%256));
  m_IO->SetValue( wxString::Format(_T("%d"), wRocNetNode.getnrio(m_Props)) );
}

void RocnetNodeDlg::event(iONode node) {
  char* s = NodeOp.toEscString(node);
  TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999, "event: \n%s", s );
  StrOp.free(s);
  if( StrOp.equals( wProgram.name(), NodeOp.getName(node)) && wProgram.getlntype(node) == wProgram.lntype_rocnet ) {
    // Ports
    wxRadioBox* l_Type[] = {NULL, m_Type1, m_Type2, m_Type3, m_Type4, m_Type5, m_Type6, m_Type7, m_Type8};
    wxSpinCtrl* l_Delay[] = { NULL, m_Delay1, m_Delay2, m_Delay3, m_Delay4, m_Delay5, m_Delay6, m_Delay7, m_Delay8};
    wxCheckBox* l_Blink[] = {NULL, m_Blink1, m_Blink2, m_Blink3, m_Blink4, m_Blink5, m_Blink6, m_Blink7, m_Blink8};
    wxCheckBox* l_Invert[] = {NULL, m_PortInv1, m_PortInv2, m_PortInv3, m_PortInv4, m_PortInv5, m_PortInv6, m_PortInv7, m_PortInv8};
    wxCheckBox* l_Toggle[] = {NULL, m_PortToggle1, m_PortToggle2, m_PortToggle3, m_PortToggle4, m_PortToggle5, m_PortToggle6, m_PortToggle7, m_PortToggle8};
    wxButton* l_PortTest[] = {NULL, m_PortTest1, m_PortTest2, m_PortTest3, m_PortTest4, m_PortTest5, m_PortTest6, m_PortTest7, m_PortTest8};
    wxSpinCtrl* l_EventID[] = {NULL, m_PortEventID1, m_PortEventID2, m_PortEventID3, m_PortEventID4, m_PortEventID5, m_PortEventID6, m_PortEventID7, m_PortEventID8};
    wxSpinCtrl* l_EventPort[] = {NULL, m_PortEventPort1, m_PortEventPort2, m_PortEventPort3, m_PortEventPort4, m_PortEventPort5, m_PortEventPort6, m_PortEventPort7, m_PortEventPort8};

    // Channels
    wxSpinCtrl* l_OffPos[] = {NULL, m_OffPos1, m_OffPos2, m_OffPos3, m_OffPos4, m_OffPos5, m_OffPos6, m_OffPos7, m_OffPos8};
    wxSpinCtrl* l_OnPos[] = {NULL, m_OnPos1, m_OnPos2, m_OnPos3, m_OnPos4, m_OnPos5, m_OnPos6, m_OnPos7, m_OnPos8};
    wxSpinCtrl* l_OffSteps[] = {NULL, m_OffSteps1, m_OffSteps2, m_OffSteps3, m_OffSteps4, m_OffSteps5, m_OffSteps6, m_OffSteps7, m_OffSteps8};
    wxSpinCtrl* l_OnSteps[] = {NULL, m_OnSteps1, m_OnSteps2, m_OnSteps3, m_OnSteps4, m_OnSteps5, m_OnSteps6, m_OnSteps7, m_OnSteps8};
    wxCheckBox* l_ChannelBlink[] = {NULL, m_ChannelBlink1, m_ChannelBlink2, m_ChannelBlink3, m_ChannelBlink4, m_ChannelBlink5, m_ChannelBlink6, m_ChannelBlink7, m_ChannelBlink8};
    wxCheckBox* l_Report[] = {NULL, m_ChannelReport1, m_ChannelReport2, m_ChannelReport3, m_ChannelReport4, m_ChannelReport5, m_ChannelReport6, m_ChannelReport7, m_ChannelReport8};
    wxSpinCtrl* l_ChannelDelay[] = {NULL, m_ChannelDelay1, m_ChannelDelay2, m_ChannelDelay3, m_ChannelDelay4, m_ChannelDelay5, m_ChannelDelay6, m_ChannelDelay7, m_ChannelDelay8};

    char key[32] = {'\0'};
    if( wProgram.getcmd(node) == wProgram.nvget && wProgram.getporttype(node) == 0 ) {
      for( int i = 0; i < 8; i++ ) {
        StrOp.fmtb(key, "val%d", 1 + i*4);
        int port = NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 2 + i*4);
        int value = NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 3 + i*4);
        int type = NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 4 + i*4);
        int delay = NodeOp.getInt( node, key, 0);
        bool blink = (type&0x80)?true:false;
        bool invert = (type&0x40)?true:false;
        bool toggle = (type&0x20)?true:false;
        type &= 0x0F;

        if( port > 0 + m_PortGroup*8 && (port-m_PortGroup*8) < 9) {
          l_Delay[port-m_PortGroup*8]->SetValue(delay);
          l_Type[port-m_PortGroup*8]->SetSelection(type);
          l_Blink[port-m_PortGroup*8]->SetValue(blink);
          l_Invert[port-m_PortGroup*8]->SetValue(invert);
          l_Toggle[port-m_PortGroup*8]->SetValue(toggle);
          l_PortTest[port-m_PortGroup*8]->SetLabel(value==0 ? wxT("0"):wxT("1"));
        }
      }

      iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
      wProgram.setcmd( cmd, wProgram.evget );
      wProgram.setval1(cmd, 1 + m_PortGroup*8 ); // range
      wProgram.setval2(cmd, 8 + m_PortGroup*8 );
      wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
      wProgram.setlntype(cmd, wProgram.lntype_rocnet);
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }

    else if( wProgram.getcmd(node) == wProgram.nvget && wProgram.getporttype(node) == wProgram.porttype_servo ) {
      for( int i = 0; i < 8; i++ ) {
        StrOp.fmtb(key, "val%d", 1 + i*6);
        int channel = NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 2 + i*6);
        int offpos = NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 3 + i*6);
        int onpos = NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 4 + i*6);
        int offsteps = NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 5 + i*6);
        int onsteps = NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 6 + i*6);
        int options = NodeOp.getInt( node, key, 0);

        bool blink = (options&0x80)?true:false;
        bool report = (options&0x40)?true:false;
        int delay = (options&0x0F);

        if( channel > 0 + m_ChannelGroup*8 && (channel-m_ChannelGroup*8) < 9) {
          l_OffPos[channel-m_ChannelGroup*8]->SetValue(offpos);
          l_OnPos[channel-m_ChannelGroup*8]->SetValue(onpos);
          l_OffSteps[channel-m_ChannelGroup*8]->SetValue(offsteps);
          l_OnSteps[channel-m_ChannelGroup*8]->SetValue(onsteps);
          l_ChannelBlink[channel-m_ChannelGroup*8]->SetValue(blink);
          l_Report[channel-m_ChannelGroup*8]->SetValue(report);
          l_ChannelDelay[channel-m_ChannelGroup*8]->SetValue(delay);
        }
      }
    }

    else if( wProgram.getcmd(node) == wProgram.evget ) {
      for( int i = 0; i < 8; i++ ) {
        StrOp.fmtb(key, "val%d", 1 + i*4);
        int port = NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 2 + i*4);
        int eventid = NodeOp.getInt( node, key, 0) * 256;
        StrOp.fmtb(key, "val%d", 3 + i*4);
        eventid += NodeOp.getInt( node, key, 0);
        StrOp.fmtb(key, "val%d", 4 + i*4);
        int eventport = NodeOp.getInt( node, key, 0);

        if( port > 0 + m_PortGroup*8 && (port-m_PortGroup*8) < 9) {
          l_EventID[port-m_PortGroup*8]->SetValue(eventid);
          l_EventPort[port-m_PortGroup*8]->SetValue(eventport);
        }
      }
    }
    else if( wProgram.getcmd(node) == wProgram.getoptions ) {
      int iotype = wProgram.getval1(node);
      if( iotype > 1 )
        iotype = 0;
      m_IOType->SetSelection( iotype );
      m_SecAck->SetValue(wProgram.getval2(node)&0x01?true:false);
      m_RFID->SetValue(wProgram.getval2(node)&0x02?true:false);
      m_UsePB->SetValue(wProgram.getval2(node)&0x04?true:false);
      m_TraceLevelInfo->SetValue(wProgram.getval2(node)&0x10?true:false);
      m_TraceLevelMonitor->SetValue(wProgram.getval2(node)&0x20?true:false);
      m_DCCType->SetSelection( wProgram.getval3(node) );
      m_DCCDevice->SetSelection( wProgram.getval4(node) );
      wxCommandEvent cmdevt;
      onIOType(cmdevt);
    }
    else if( wProgram.getcmd(node) == wProgram.identify ) {
      if( wProgram.getvalue(node) == 1 ) {
        int id = wProgram.getmodid(node);
        iONode rnnode = wRocNet.getrocnetnode(m_Digint);
        while( rnnode != NULL ) {
          if( wRocNetNode.getid(rnnode) == id ) {
            NodeOp.removeChild( m_Digint, rnnode);
            break;
          }
          rnnode = wRocNet.nextrocnetnode(m_Digint, rnnode);
        }
        initNodeList();
      }
      else if( NodeOp.getChildCnt(node) > 0 ) {
        iONode newrnnode = NodeOp.getChild(node, 0);
        if( newrnnode != NULL ) {
          iONode rnnode = wRocNet.getrocnetnode(m_Digint);
          while( rnnode != NULL ) {
            if( wRocNetNode.getid(rnnode) == wRocNetNode.getid(newrnnode) ) {
              NodeOp.removeChild( m_Digint, rnnode);
              break;
            }
            rnnode = wRocNet.nextrocnetnode(m_Digint, rnnode);
          }
          NodeOp.addChild(m_Digint, (iONode)NodeOp.base.clone(newrnnode));
          m_NodeBook->SetSelection(0);
          initNodeList();
        }
      }
    }
    else if( wProgram.getcmd(node) == wProgram.show ) {
      // Select idex.
      m_NodeBook->SetSelection(0);

      if(m_NodeList->GetItemCount() > 0 ) {
        for( int i = 0; i < m_NodeList->GetItemCount(); i++ ) {
          iONode l_Props = (iONode)m_NodeList->GetItemData(i);
          TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999, "show[%d]->%d", wRocNetNode.getid(l_Props), wProgram.getmodid(node) );
          if( wRocNetNode.getid(l_Props) == wProgram.getmodid(node) ) {
            m_NodeList->SetItemState(i, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            break;
          }
        }
      }

    }
    else if( wProgram.getcmd(node) == wProgram.macro_get ) {
      initMacro(node);
    }


  }
}

void RocnetNodeDlg::onNodeOptionsRead( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.getoptions );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocnetNodeDlg::onNodeOptionsWrite( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.setoptions );
  wProgram.setval1( cmd, m_IOType->GetSelection());
  wProgram.setval2( cmd, (m_SecAck->IsChecked()?0x01:0x00) | (m_RFID->IsChecked()?0x02:0x00) | (m_UsePB->IsChecked()?0x04:0x00) |
                          (m_TraceLevelInfo->IsChecked()?0x10:0x00) | (m_TraceLevelMonitor->IsChecked()?0x20:0x00) );
  wProgram.setval3( cmd, m_DCCType->GetSelection());
  wProgram.setval4( cmd, m_DCCDevice->GetSelection());
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocnetNodeDlg::onShutdown( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  int action = wxMessageDialog( this, wxString::Format(wxGetApp().getMsg( "shutdownnode" ), wRocNetNode.getid(m_Props)),
      _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION | wxNO_DEFAULT ).ShowModal();
  if( action == wxID_NO ) {
    return;
  }

  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, wSysCmd.shutdownnode );
  wSysCmd.setbus(cmd, wRocNetNode.getid(m_Props));
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void RocnetNodeDlg::onShutdownAll( wxCommandEvent& event ) {
  int action = wxMessageDialog( this, wxGetApp().getMsg( "shutdownallnodes" ),
      _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION | wxNO_DEFAULT ).ShowModal();
  if( action == wxID_NO ) {
    return;
  }

  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, wSysCmd.shutdownnode );
  wSysCmd.setbus(cmd, 0);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void RocnetNodeDlg::onShow( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.show );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999,"show: %d", wProgram.getmodid(cmd) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void RocnetNodeDlg::onGCALogo( wxMouseEvent& event ) {
  wxLaunchDefaultBrowser(wxT("http://www.phgiling.net/"), wxBROWSER_NEW_WINDOW );
}

void RocnetNodeDlg::onRocNetLogo( wxMouseEvent& event ) {
  wxLaunchDefaultBrowser(wxT("http://wiki.rocrail.net/doku.php?id=rocnet:rocnetnode-en"), wxBROWSER_NEW_WINDOW );
}

void RocnetNodeDlg::onQuery( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.query );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void RocnetNodeDlg::onMacroNumber( wxSpinEvent& event ) {
  m_MacroLines->ClearGrid();
  for( int i = 0; i < 8; i++ ) {
    m_MacroLines->SetCellBackgroundColour(i, 0, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    m_MacroLines->SetCellBackgroundColour(i, 1, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    m_MacroLines->SetCellBackgroundColour(i, 2, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    m_MacroLines->SetCellBackgroundColour(i, 3, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
  }
  wxCommandEvent cmd;
  onMacroGet(cmd);
}


void RocnetNodeDlg::onMacroGet( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.macro_get );
  wProgram.setvalue( cmd, m_MacroNr->GetValue() );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void RocnetNodeDlg::onMacroSet( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.macro_set );
  wProgram.setvalue( cmd, m_MacroNr->GetValue() );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);

  char key[32] = {'\0'};
  for( int i = 0; i < 8; i++ ) {
    StrOp.fmtb(key, "val%d", 1 + i*4);
    NodeOp.setInt( cmd, key, atoi(m_MacroLines->GetCellValue(i, 0).mb_str(wxConvUTF8)));
    StrOp.fmtb(key, "val%d", 2 + i*4);
    NodeOp.setInt( cmd, key, atoi(m_MacroLines->GetCellValue(i, 1).mb_str(wxConvUTF8)) );
    StrOp.fmtb(key, "val%d", 3 + i*4);
    NodeOp.setInt( cmd, key, atoi(m_MacroLines->GetCellValue(i, 2).mb_str(wxConvUTF8)) +
        (atoi(m_MacroLines->GetCellValue(i, 4).mb_str(wxConvUTF8))?0x80:0x00) );
    StrOp.fmtb(key, "val%d", 4 + i*4);
    NodeOp.setInt( cmd, key, atoi(m_MacroLines->GetCellValue(i, 3).mb_str(wxConvUTF8)) );
  }

  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void RocnetNodeDlg::initMacro( iONode node ) {
  m_MacroNr->SetValue( wProgram.getvalue(node) );
  char key[32] = {'\0'};
  for( int i = 0; i < 8; i++ ) {
    m_MacroLines->SetCellBackgroundColour(i, 0, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    m_MacroLines->SetCellBackgroundColour(i, 1, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    m_MacroLines->SetCellBackgroundColour(i, 2, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    m_MacroLines->SetCellBackgroundColour(i, 3, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    StrOp.fmtb(key, "val%d", 1 + i*4);
    m_MacroLines->SetCellValue(i, 0, wxString::Format(wxT("%d"), NodeOp.getInt(node, key, 0)));
    StrOp.fmtb(key, "val%d", 2 + i*4);
    m_MacroLines->SetCellValue(i, 1, wxString::Format(wxT("%d"), NodeOp.getInt(node, key, 0)));
    StrOp.fmtb(key, "val%d", 3 + i*4);
    m_MacroLines->SetCellValue(i, 2, wxString::Format(wxT("%d"), NodeOp.getInt(node, key, 0)&0x7F));
    m_MacroLines->SetCellValue(i, 4, wxString::Format(wxT("%d"), (NodeOp.getInt(node, key, 0)&0x80?1:0)));
    StrOp.fmtb(key, "val%d", 4 + i*4);
    m_MacroLines->SetCellValue(i, 3, wxString::Format(wxT("%d"), NodeOp.getInt(node, key, 0)));
  }
}

void RocnetNodeDlg::onIOType( wxCommandEvent& event ) {
}

void RocnetNodeDlg::onMacroLineChange( wxGridEvent& event ) {
  int row = event.GetRow();
  int col = event.GetCol();
  int val = atoi(m_MacroLines->GetCellValue(row, col).mb_str(wxConvUTF8));
  m_MacroLines->SetCellBackgroundColour(row, col, wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
  if( col == 0 && (val < 0 || val > 128) ) {
    m_MacroLines->SetCellValue(row, col, wxT("0") );
    m_MacroLines->SetCellBackgroundColour(row, col, wxColour(255,200,200));
  }
  else if( val < 0 || val > 255 ) {
    m_MacroLines->SetCellValue(row, col, wxT("0") );
    m_MacroLines->SetCellBackgroundColour(row, col, wxColour(255,200,200));
  }
}

void RocnetNodeDlg::onUpdate( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  if( m_UpdateRevision->IsEmpty() ) {
    m_Update->Enable(false);
    iOThread updateReader = ThreadOp.inst( "update", __updateReaderThread, this );
    ThreadOp.start( updateReader );
    return;
  }

  int revision = atoi(m_UpdateRevision->GetValue().mb_str(wxConvUTF8));
  int action = wxMessageDialog( this, wxString::Format(wxGetApp().getMsg( "updatenode" ),
      wRocNetNode.getid(m_Props), revision ),
      _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION | wxNO_DEFAULT ).ShowModal();
  if( action == wxID_NO ) {
    return;
  }

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.update );
  wProgram.setvalue( cmd, revision );
  wProgram.setval1( cmd, m_UpdateOffline->IsChecked()?1:0 );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void RocnetNodeDlg::onPortTest( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  wxRadioBox* l_Type[] = {NULL, m_Type1, m_Type2, m_Type3, m_Type4, m_Type5, m_Type6, m_Type7, m_Type8};
  wxButton* l_PortTest[] = {NULL, m_PortTest1, m_PortTest2, m_PortTest3, m_PortTest4, m_PortTest5, m_PortTest6, m_PortTest7, m_PortTest8};

  if( event.GetEventType() != wxEVT_COMMAND_BUTTON_CLICKED ) {
    return;
  }

  bool isInput = false;
  int port = 0;
  const char* cmd = wOutput.on;

  for( int i = 1; i < 9; i++ ) {
    if( event.GetEventObject() == l_PortTest[i] ) {
      port = i + m_PortGroup*8;
      isInput = (l_Type[i]->GetSelection() == 1 ? true:false);
      if( l_PortTest[i]->GetLabel().CompareTo(_T("1")) == 0 ) {
          cmd = wOutput.off;
          l_PortTest[i]->SetLabel(wxT("0"));
      }
      else {
        cmd = wOutput.on;
        l_PortTest[i]->SetLabel(wxT("1"));
      }
      TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999, "test button %d clicked", port);
      break;
    }
  }

  if( !isInput && port >  0 ) {
    iONode swcmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
    wOutput.setaddr( swcmd, port );
    wOutput.setbus( swcmd, wRocNetNode.getid(m_Props) );
    wOutput.setiid( swcmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wOutput.setcmd( swcmd, cmd );
    TraceOp.trc( "cbusdlg", TRCLEVEL_INFO, __LINE__, 9999, "send test command for port %d:%d...", wRocNetNode.getid(m_Props), port);
    wxGetApp().sendToRocrail( swcmd );
    swcmd->base.del( swcmd );
  }

}

void RocnetNodeDlg::onListColClick( wxListEvent& event ) {
  m_SortCol = event.GetColumn();
  initNodeList();
}


void RocnetNodeDlg::onItemActivated( wxTreeEvent& event ) {
  event.Skip(true);
}


void RocnetNodeDlg::onMenu( wxCommandEvent& event ) {
  int menuItem = event.GetId();
  if( m_SelectedNode != NULL ) {
    TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999, "action %d", menuItem );
    if( menuItem == 1001) {
      m_Props = m_SelectedNode;
      onShow(event);
    }
    else if( menuItem == 1002) {
      m_Props = m_SelectedNode;
      onShutdown(event);
    }
  }

  if( m_SelectedZLevel != NULL ) {
    if( menuItem == 2002) {
      shutdownLocation();
    }
  }

  if( menuItem == 3002) {
    onShutdownAll(event);
  }
}


void RocnetNodeDlg::onTreeItemRightClick( wxTreeEvent& event ) {
  wxString itemText = m_NodeTree->GetItemText(event.GetItem());
  const char* key = itemText.mb_str(wxConvUTF8);
  m_SelectedNode = (iONode)MapOp.get( m_NodeMap, key );
  if( m_SelectedNode != NULL ) {
    wxMenu menu( wxString(key,wxConvUTF8) );
    menu.Append( 1001, wxGetApp().getMenu("show") );
    menu.Append( 1002, wxGetApp().getMenu("shutdownserver") );
    menu.Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( RocnetNodeDlg::onMenu ), NULL, this );
    PopupMenu(&menu );
    return;
  }

  m_SelectedZLevel = (iONode)MapOp.get( m_TreeLocationMap, key );
  if( m_SelectedZLevel != NULL ) {
    wxMenu menu( wxString(key,wxConvUTF8) );
    menu.Append( 2002, wxGetApp().getMenu("shutdownserver") );
    menu.Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( RocnetNodeDlg::onMenu ), NULL, this );
    PopupMenu(&menu );
    return;
  }

  if( StrOp.equals(key, wPlan.gettitle(wxGetApp().getModel())) ) {
    wxMenu menu( wxString(key,wxConvUTF8) );
    menu.Append( 3002, wxGetApp().getMenu("shutdownall") );
    menu.Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( RocnetNodeDlg::onMenu ), NULL, this );
    PopupMenu(&menu );
    return;
  }
}


void RocnetNodeDlg::onTreeSelChanged( wxTreeEvent& event ) {
  wxString itemText = m_NodeTree->GetItemText(event.GetItem());
  char* key = StrOp.dup(itemText.mb_str(wxConvUTF8));
  iONode rnnode = (iONode)MapOp.get( m_NodeMap, key);
  TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999,"tree selection: %s", key );
  selChanged(rnnode);
}


void RocnetNodeDlg::selChanged( iONode rnnode ) {
  if( rnnode != NULL ) {
    m_Props = rnnode;
    m_PortGroup = 0;
    m_ChannelGroup = 0;
    initPorts();
    initChannels();
    initValues();
    m_MacroLines->ClearGrid();
    m_MacroNr->SetValue(1);
    SetTitle(wxT("RocNetNode: ") + wxString::Format(_T("%d"), wRocNetNode.getid(m_Props) ) );
    wxCommandEvent cmd;
    onNodeOptionsRead(cmd);
    onMacroGet(cmd);
    onPortRead(cmd);
    onChannelRead(cmd);
  }
}

void RocnetNodeDlg::shutdownLocation() {
  if( m_SelectedZLevel == NULL )
    return;

  int action = wxMessageDialog( this, wxString::Format(wxGetApp().getMsg( "shutdownlocation" ), wZLevel.gettitle(m_SelectedZLevel)),
      _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION | wxNO_DEFAULT ).ShowModal();
  if( action == wxID_NO ) {
    return;
  }

  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, wSysCmd.shutdownnode );

  iONode rnnode = wRocNet.getrocnetnode(m_Digint);
  while( rnnode != NULL ) {
    wSysCmd.setbus(cmd, wRocNetNode.getid(rnnode));
    wxGetApp().sendToRocrail( cmd );
    rnnode = wRocNet.nextrocnetnode(m_Digint, rnnode);
  }

  cmd->base.del(cmd);

}

void RocnetNodeDlg::onUpdateVersion( wxCommandEvent& event ) {
  m_UpdateRevision->SetValue( wxString::Format(_T("%d"), m_AvailableVersion) );
  if( m_VersionInfo != NULL ) {
    TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999, "release info: %s", m_VersionInfo );
    m_RevisionInfo->SetValue( wxString(m_VersionInfo,wxConvUTF8) );
  }
}

void RocnetNodeDlg::onNewRevisionNumber( wxCommandEvent& event ) {
  if( m_UpdateRevision->IsEmpty() )
    m_Update->SetLabel(wxGetApp().getMsg( "poll" ));
  else {
    m_Update->Enable(true);
    m_Update->SetLabel(wxGetApp().getMsg( "update" ));
  }
}

void RocnetNodeDlg::onReport( wxCommandEvent& event ) {
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxString ms_FileExt = _T("RocNet-Report (*.csv)|*.csv");
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("saveas"), wxString(l_openpath,wxConvUTF8),
                       wxString::Format( _T("rocnet-report.csv")), ms_FileExt, wxFD_SAVE);
  if( fdlg->ShowModal() == wxID_OK ) {
    iONode model = wxGetApp().getModel();
    // Check for existence.
    wxString path = fdlg->GetPath();
    if( FileOp.exist( path.mb_str(wxConvUTF8) ) ) {
      int action = wxMessageDialog( this, wxGetApp().getMsg("fileexistwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
      if( action == wxID_NO ) {
        fdlg->Destroy();
        return;
      }
    }
    if( !path.Contains( _T(".csv") ) )
      path.Append( _T(".csv") );

    iOFile l_ReportFile = FileOp.inst( path.mb_str(wxConvUTF8), OPEN_WRITE );
    FileOp.fmt(l_ReportFile, "\"id\",\"port\",\"type\",\"RocrailID\"\n");
    FileOp.flush(l_ReportFile);

    iONode rnnode = (iONode)MapOp.first( m_NodeMap );
    while( rnnode != NULL ) {
      int rnid = wRocNetNode.getid(rnnode);
      TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999,"reporting %d...", rnid );
      for( int i = 0; i < 128; i++ ) {
        const char* type = "";
        const char* id = wxGetApp().findID( false, i+1, rnid, 0, &type );
        if( StrOp.equals( id, "not used" ) ) {
          id = wxGetApp().findID( true, i+1, rnid, 0, &type );
        }
        if( !StrOp.equals( id, "not used" ) ) {
          FileOp.fmt(l_ReportFile, "\"%d\",\"%d\",\"%s\",\"%s\"\n",
              rnid, i+1, type, id);
        }
      }
      rnnode = (iONode)MapOp.next( m_NodeMap );
    }

    TraceOp.trc( "rocnetnode", TRCLEVEL_INFO, __LINE__, 9999,"open report %s...", FileOp.getFilename(l_ReportFile) );
#if wxCHECK_VERSION(3, 0, 0)
    wxLaunchDefaultApplication(wxString(FileOp.getFilename(l_ReportFile), wxConvUTF8));
#else
    wxShell(wxString(FileOp.getFilename(l_ReportFile), wxConvUTF8));
#endif
    FileOp.base.del(l_ReportFile);
  }
  fdlg->Destroy();

}

void RocnetNodeDlg::onBeginDrag( wxTreeEvent& event ) {
  wxString itemText = m_NodeTree->GetItemText(event.GetItem());
  char* key = StrOp.dup(itemText.mb_str(wxConvUTF8));
  iONode rnnode = (iONode)MapOp.get( m_NodeMap, key);

  wxString my_text = _T("bus:")+wxString::Format(_T("%d"), wRocNetNode.getid(rnnode) );
  wxTextDataObject my_data(my_text);
  wxDropSource dragSource( this );
  dragSource.SetData( my_data );
  wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);

}

void RocnetNodeDlg::onBeginListDrag( wxListEvent& event ) {
  int index = event.GetIndex();
  if( m_Props != NULL ) {
    wxString my_text = _T("bus:")+wxString::Format(_T("%d"), wRocNetNode.getid(m_Props) );
    wxTextDataObject my_data(my_text);
    wxDropSource dragSource( this );
    dragSource.SetData( my_data );
    wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
  }
}


void RocnetNodeDlg::onPort1Drag( wxMouseEvent& event ) {
  wxStaticText* port = (wxStaticText*)event.GetEventObject();

  wxString my_text = _T("bus:")+wxString::Format(_T("%d:"), wRocNetNode.getid(m_Props) ) + port->GetLabel();
  wxTextDataObject my_data(my_text);
  wxDropSource dragSource( this );
  dragSource.SetData( my_data );
  wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
}


void RocnetNodeDlg::onMacroExport( wxCommandEvent& event ) {
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxString ms_FileExt = _T("Macro (*.xml)|*.xml");
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("export"), wxString(l_openpath,wxConvUTF8),
                       wxString::Format( _T("rocnet-macro-%d.xml"), m_MacroNr->GetValue()), ms_FileExt, wxFD_SAVE);
  if( fdlg->ShowModal() == wxID_OK ) {
    iONode model = wxGetApp().getModel();
    // Check for existence.
    wxString path = fdlg->GetPath();
    if( FileOp.exist( path.mb_str(wxConvUTF8) ) ) {
      int action = wxMessageDialog( this, wxGetApp().getMsg("fileexistwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
      if( action == wxID_NO ) {
        fdlg->Destroy();
        return;
      }
    }
    if( !path.Contains( _T(".xml") ) )
      path.Append( _T(".xml") );

    iOFile f = FileOp.inst( path.mb_str(wxConvUTF8), OPEN_WRITE );
    if( f != NULL ) {
      iONode macro = NodeOp.inst( wMacro.name(), NULL, ELEMENT_NODE );
      wMacro.setuid(macro, m_ID->GetValue() );
      wMacro.setnr(macro, m_MacroNr->GetValue());

      for( int i = 0; i < 8; i++ ) {
        iONode macroline = NodeOp.inst( wMacroLine.name(), macro, ELEMENT_NODE );
        NodeOp.addChild(macro, macroline);
        wMacroLine.setnr(macroline, i);
        // ToDo: Set line values.
        int val1 = atoi( m_MacroLines->GetCellValue( i, 0 ).mb_str(wxConvUTF8) );
        int val2 = atoi( m_MacroLines->GetCellValue( i, 1 ).mb_str(wxConvUTF8) );
        int val3 = atoi( m_MacroLines->GetCellValue( i, 2 ).mb_str(wxConvUTF8) );
        int val4 = atoi( m_MacroLines->GetCellValue( i, 3 ).mb_str(wxConvUTF8) );
        int val5 = atoi( m_MacroLines->GetCellValue( i, 4 ).mb_str(wxConvUTF8) );

        wMacroLine.setport( macroline, val1 );
        wMacroLine.setdelay( macroline, val2 );
        wMacroLine.setporttype( macroline, val3 );
        wMacroLine.setstatus( macroline, val4 );
        wMacroLine.setblink( macroline, val5?True:False );
      }

      char* s = NodeOp.base.toString(macro);
      FileOp.writeStr(f, s);
      StrOp.free(s);
      NodeOp.base.del(macro);
      FileOp.base.del( f );
    }

  }

  fdlg->Destroy();

}


void RocnetNodeDlg::onMacroImport( wxCommandEvent& event ) {
  wxString ms_FileExt = _T("Macro (*.xml)|*.xml");
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("import"), wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {

    wGui.setopenpath( wxGetApp().getIni(), fdlg->GetPath().mb_str(wxConvUTF8) );
    // strip filename:
    wGui.setopenpath( wxGetApp().getIni(), FileOp.getPath(wGui.getopenpath( wxGetApp().getIni() ) ) );

    TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "reading [%s]...", (const char*)fdlg->GetPath().mb_str(wxConvUTF8));
    iOFile f = FileOp.inst( fdlg->GetPath().mb_str(wxConvUTF8), OPEN_READONLY );
    if( f != NULL ) {
      TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "file opened...");
      char* macroXml = (char*)allocMem( FileOp.size( f ) + 1 );
      FileOp.read( f, macroXml, FileOp.size( f ) );
      FileOp.close( f );
      FileOp.base.del(f );

      iODoc macroDoc = DocOp.parse( macroXml );
      freeMem( macroXml );
      if( macroDoc != NULL ) {
        iONode macro = DocOp.getRootNode( macroDoc );
        if( macro != NULL ) {

          int idx = 0;
          iONode macroline = wMacro.getmacroline(macro);
          while( macroline != NULL ) {
            m_MacroLines->SetCellValue(idx, 0, wxString::Format(_T("%d"), wMacroLine.getport(macroline)) );
            m_MacroLines->SetCellValue(idx, 1, wxString::Format(_T("%d"), wMacroLine.getdelay(macroline)) );
            m_MacroLines->SetCellValue(idx, 2, wxString::Format(_T("%d"), wMacroLine.getporttype(macroline)) );
            m_MacroLines->SetCellValue(idx, 3, wxString::Format(_T("%d"), wMacroLine.getstatus(macroline)) );
            m_MacroLines->SetCellValue(idx, 4, wxString::Format(_T("%d"), wMacroLine.isblink(macroline)?1:0) );
            idx++;
            macroline = wMacro.nextmacroline(macro, macroline);
          }

          NodeOp.base.del(macro);
        }
        DocOp.base.del(macroDoc);
      }
    }
  }

  fdlg->Destroy();

}


void RocnetNodeDlg::onPortRemove( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  int action = wxMessageDialog( this, wxGetApp().getMsg( "delete" ),
      _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION | wxNO_DEFAULT ).ShowModal();
  if( action == wxID_NO ) {
    return;
  }

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.unlearn );

  char key[32] = {'\0'};
  for( int i = 0; i < 8; i++ ) {
    StrOp.fmtb(key, "val%d", 1 + i);
    NodeOp.setInt( cmd, key, m_PortGroup*8 + 1 + i);
  }

  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );

  cmd->base.del(cmd);

  initPorts();
}


void RocnetNodeDlg::onShell( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;
  wxExecute(wxString::Format(wxT("putty 192.168.%d.%d"),  wRocNetNode.getsubip(m_Props)/256, wRocNetNode.getsubip(m_Props)%256 ));
}





void RocnetNodeDlg::onChannelPrev( wxCommandEvent& event ) {
  if( m_ChannelGroup > 0 ) {
    m_ChannelGroup--;
    initChannels();
    wxCommandEvent cmd;
    onChannelRead(cmd);
  }
}


void RocnetNodeDlg::onChannelNext( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;
  if( 128 > (m_ChannelGroup+1) * 8) {
    m_ChannelGroup++;
    initChannels();
    wxCommandEvent cmd;
    onChannelRead(cmd);
  }
}


void RocnetNodeDlg::onChannelRemove( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  int action = wxMessageDialog( this, wxGetApp().getMsg( "delete" ),
      _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION | wxNO_DEFAULT ).ShowModal();
  if( action == wxID_NO ) {
    return;
  }

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.unlearn );
  wProgram.setporttype( cmd, wProgram.porttype_servo );

  char key[32] = {'\0'};
  for( int i = 0; i < 8; i++ ) {
    StrOp.fmtb(key, "val%d", 1 + i);
    NodeOp.setInt( cmd, key, m_ChannelGroup*8 + 1 + i);
  }

  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );

  cmd->base.del(cmd);

  initChannels();
}


void RocnetNodeDlg::onChannelRead( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.nvget );
  wProgram.setporttype( cmd, wProgram.porttype_servo );
  wProgram.setval1(cmd, 1 + m_ChannelGroup*8 ); // range
  wProgram.setval2(cmd, 8 + m_ChannelGroup*8 );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void RocnetNodeDlg::onChannelWrite( wxCommandEvent& event ) {
  if( m_Props == NULL )
    return;

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wRocNetNode.getid(m_Props));
  wProgram.setcmd( cmd, wProgram.nvset );
  wProgram.setporttype( cmd, wProgram.porttype_servo );

  wxSpinCtrl* l_OffPos[] = {NULL, m_OffPos1, m_OffPos2, m_OffPos3, m_OffPos4, m_OffPos5, m_OffPos6, m_OffPos7, m_OffPos8};
  wxSpinCtrl* l_OnPos[] = {NULL, m_OnPos1, m_OnPos2, m_OnPos3, m_OnPos4, m_OnPos5, m_OnPos6, m_OnPos7, m_OnPos8};
  wxSpinCtrl* l_OffSteps[] = {NULL, m_OffSteps1, m_OffSteps2, m_OffSteps3, m_OffSteps4, m_OffSteps5, m_OffSteps6, m_OffSteps7, m_OffSteps8};
  wxSpinCtrl* l_OnSteps[] = {NULL, m_OnSteps1, m_OnSteps2, m_OnSteps3, m_OnSteps4, m_OnSteps5, m_OnSteps6, m_OnSteps7, m_OnSteps8};
  wxCheckBox* l_Blink[] = {NULL, m_ChannelBlink1, m_ChannelBlink2, m_ChannelBlink3, m_ChannelBlink4, m_ChannelBlink5, m_ChannelBlink6, m_ChannelBlink7, m_ChannelBlink8};
  wxCheckBox* l_Report[] = {NULL, m_ChannelReport1, m_ChannelReport2, m_ChannelReport3, m_ChannelReport4, m_ChannelReport5, m_ChannelReport6, m_ChannelReport7, m_ChannelReport8};
  wxSpinCtrl* l_Delay[] = {NULL, m_ChannelDelay1, m_ChannelDelay2, m_ChannelDelay3, m_ChannelDelay4, m_ChannelDelay5, m_ChannelDelay6, m_ChannelDelay7, m_ChannelDelay8};

  char key[32] = {'\0'};
  for( int i = 0; i < 8; i++ ) {
    StrOp.fmtb(key, "val%d", 1 + i*6);
    NodeOp.setInt( cmd, key, m_ChannelGroup*8 + 1 + i);
    StrOp.fmtb(key, "val%d", 2 + i*6);
    NodeOp.setInt( cmd, key, l_OffPos[i+1]->GetValue() );
    StrOp.fmtb(key, "val%d", 3 + i*6);
    NodeOp.setInt( cmd, key, l_OnPos[i+1]->GetValue() );
    StrOp.fmtb(key, "val%d", 4 + i*6);
    NodeOp.setInt( cmd, key, l_OffSteps[i+1]->GetValue() );
    StrOp.fmtb(key, "val%d", 5 + i*6);
    NodeOp.setInt( cmd, key, l_OnSteps[i+1]->GetValue() );
    StrOp.fmtb(key, "val%d", 6 + i*6);
    NodeOp.setInt( cmd, key, (l_Blink[1 + i]->IsChecked()?0x80:0x00) + (l_Report[1 + i]->IsChecked()?0x40:0x00) + (l_Delay[i+1]->GetValue() & 0x0F) );
  }

  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_rocnet);
  wxGetApp().sendToRocrail( cmd );
}


void RocnetNodeDlg::initChannels() {
  wxStaticText* l_labChannel[] = { NULL, m_labChannel1, m_labChannel2, m_labChannel3, m_labChannel4, m_labChannel5, m_labChannel6, m_labChannel7, m_labChannel8 };
  wxSpinCtrl* l_OffPos[] = {NULL, m_OffPos1, m_OffPos2, m_OffPos3, m_OffPos4, m_OffPos5, m_OffPos6, m_OffPos7, m_OffPos8};
  wxSpinCtrl* l_OnPos[] = {NULL, m_OnPos1, m_OnPos2, m_OnPos3, m_OnPos4, m_OnPos5, m_OnPos6, m_OnPos7, m_OnPos8};
  wxSpinCtrl* l_OffSteps[] = {NULL, m_OffSteps1, m_OffSteps2, m_OffSteps3, m_OffSteps4, m_OffSteps5, m_OffSteps6, m_OffSteps7, m_OffSteps8};
  wxSpinCtrl* l_OnSteps[] = {NULL, m_OnSteps1, m_OnSteps2, m_OnSteps3, m_OnSteps4, m_OnSteps5, m_OnSteps6, m_OnSteps7, m_OnSteps8};
  wxCheckBox* l_Blink[] = {NULL, m_ChannelBlink1, m_ChannelBlink2, m_ChannelBlink3, m_ChannelBlink4, m_ChannelBlink5, m_ChannelBlink6, m_ChannelBlink7, m_ChannelBlink8};
  wxCheckBox* l_Report[] = {NULL, m_ChannelReport1, m_ChannelReport2, m_ChannelReport3, m_ChannelReport4, m_ChannelReport5, m_ChannelReport6, m_ChannelReport7, m_ChannelReport8};
  wxSpinCtrl* l_Delay[] = {NULL, m_ChannelDelay1, m_ChannelDelay2, m_ChannelDelay3, m_ChannelDelay4, m_ChannelDelay5, m_ChannelDelay6, m_ChannelDelay7, m_ChannelDelay8};

  for( int i = 1; i < 9; i++ ) {
    l_labChannel[i]->SetLabel( wxString::Format(wxT("%d"),i + m_ChannelGroup*8) );
    l_OffPos[i]->SetValue(0);
    l_OnPos[i]->SetValue(0);
    l_OffSteps[i]->SetValue(0);
    l_OnSteps[i]->SetValue(0);
    l_Blink[i]->SetValue(false);
    l_Report[i]->SetValue(false);
    l_Delay[i]->SetValue(0);
  }
}


void RocnetNodeDlg::onChannelDrag( wxMouseEvent& event ) {
  wxStaticText* channel = (wxStaticText*)event.GetEventObject();

  wxString my_text = _T("bus:")+wxString::Format(_T("%d:"), wRocNetNode.getid(m_Props) ) + channel->GetLabel() + wxT(":2");
  wxTextDataObject my_data(my_text);
  wxDropSource dragSource( this );
  dragSource.SetData( my_data );
  wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
}


