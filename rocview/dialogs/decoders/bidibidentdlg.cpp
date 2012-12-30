/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 as published by the Free Software Foundation; either version 2
 modify it under the terms of the GNU General Public License
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "bidibidentdlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include <wx/clipbrd.h>
#include <wx/dataobj.h>
#include <wx/dnd.h>
#include <wx/filedlg.h>

#include "rocs/public/trace.h"

#include "rocview/public/guiapp.h"
#include "rocview/wrapper/public/Gui.h"

#include "rocutils/public/vendors.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/BiDiB.h"
#include "rocrail/wrapper/public/BiDiBnode.h"
#include "rocrail/wrapper/public/Macro.h"
#include "rocrail/wrapper/public/MacroLine.h"
#include "rocdigs/impl/bidib/bidibutils.h"
#include "rocdigs/impl/bidib/bidib_messages.h"

#include "rocview/res/icons.hpp"

/* XPM */
static const char * pcb_xpm[] = {
"32 20 11 1",
"   c None",
".  c #848484",
"+  c #FFFFFF",
"@  c #C6C6C6",
"#  c #000000",
"$  c #008400",
"%  c #00FF00",
"&  c #FF0000",
"*  c #840000",
"=  c #FFFF00",
"-  c #848400",
".+@@@@@@@@@@@@@@@@@@@@@@@@@@@#  ",
".+$$$@$$$$@$$$@$%@%$%@$$$$@$$#  ",
".+&$$@$$$$@$$$@$$@$$$@$$$$@$$#  ",
".+$$+@@#$+@@#$@$$@$$+@@@#$@@@#.#",
".+$$@..#$@..#$@$$@$$@...#$@$$#+#",
".+&$####$####$@$$@$$@...#$@$$#@#",
".+$$$$$$$$$$$$@@@@$$@...#$@@@#@#",
".+$$+@@#$+@@#$@$$$$$#####$@$$#@#",
".+&$@..#$@..#$@$$$$$$$$$$$@$$#@#",
".+$$####$####$@@@@%@@%@@%@@$$#@#",
".+$$$$$$$$$$$$@$$$$$$$$$$$$$$#@#",
".+$$+@@#$+@@#$@$$$@..#$@..#$$#@#",
".+$$@..#$@..#$@$$$####$####$$###",
".+$$####$####$@$$$$$$$$$$$$$$#  ",
".+$$$$@$$$$@$$@$$$$*$$*$$*$$$#  ",
".............................#  ",
"##############.=-=-=-=-=-#####  ",
"              .=-=-=-=-=-#      ",
"              .=-=-=-=-=-#      ",
"              ############      "};

BidibIdentDlg::BidibIdentDlg( wxWindow* parent ):BidibIdentDlgGen( parent )
{
  this->node = NULL;
  __initVendors();
  initLabels();
}

BidibIdentDlg::BidibIdentDlg( wxWindow* parent, iONode node ):BidibIdentDlgGen( parent )
{
  this->node = node;
  __initVendors();
  initLabels();
  initValues();
}

BidibIdentDlg::~BidibIdentDlg() {
  ListOp.base.del(nodeList);
  MapOp.base.del(nodeMap);
  MapOp.base.del(nodePathMap);
  if( this->node != NULL )
    NodeOp.base.del(this->node);
  clearFeatureList();
}


void BidibIdentDlg::onCancel( wxCommandEvent& event ) {
  EndModal( 0 );
}


void BidibIdentDlg::onOK( wxCommandEvent& event ) {
  //wxClipboard* cb = new wxClipboard();
  wxClipboard* cb = wxTheClipboard;
  if( cb != NULL ) {
    //cb->UsePrimarySelection();
    if( cb->Open() ) {
      wxString text( m_UID->GetValue() );
      if( !text.IsEmpty() ) {
        wxTextDataObject *data = new wxTextDataObject( text );
        cb->SetData( data );
      }
      cb->Close();
    }
    //delete cb;
  }

  EndModal( wxID_OK );
}

void BidibIdentDlg::event(iONode node) {
  TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999,"event cmd=%d", wProgram.getcmd( node ) );
  if(  wProgram.getcmd( node ) == wProgram.datarsp ) {
    handleFeature(node);
    NodeOp.base.del(node);
  }
  else if(  wProgram.getcmd( node ) == wProgram.nvget ) {
    m_PortType->SetSelection(wProgram.getporttype(node));
    m_ServoLeft->SetValue( wProgram.getval1(node));
    m_ServoRight->SetValue( wProgram.getval2(node));
    m_ServoSpeed->SetValue( wProgram.getval3(node));
    m_ServoReserved->SetValue( wProgram.getval4(node));
    m_ConfigL->SetValue( wProgram.getval1(node));
    m_ConfigR->SetValue( wProgram.getval2(node));
    m_ConfigV->SetValue( wProgram.getval3(node));
    m_ConfigS->SetValue( wProgram.getval4(node));
    NodeOp.base.del(node);
  }
  else if(  wProgram.getcmd( node ) == wProgram.writehex ) {
    m_UpdateStart->Enable(true);
    m_UpdateStart->SetLabel( wxGetApp().getMsg( "start" ) );
  }
  else if( wProgram.getcmd( node ) == wProgram.macro_get || wProgram.getcmd( node ) == wProgram.macro_getparams ) {
    handleMacro(node);
    NodeOp.base.del(node);
  }
  else if( wProgram.getcmd( node ) == wProgram.macro_set || wProgram.getcmd( node ) == wProgram.macro_setparams ) {
    handleMacro(node);
    NodeOp.base.del(node);
  }
  else if( wProgram.getcmd( node ) == wProgram.acc_getparam || wProgram.getcmd( node ) == wProgram.acc_setparam ) {
    handleAccessory(node);
    NodeOp.base.del(node);
  }
  else if( !StrOp.equals( wProgram.name(), NodeOp.getName(node) ) ) {
    if( this->node != NULL )
      NodeOp.base.del(this->node);
    this->node = node;
    initValues();
  }
}


static int __sortPath(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    return strcmp( wBiDiBnode.getpath(a), wBiDiBnode.getpath(b) );
}


int BidibIdentDlg::getLevel(const char* path ) {
  // 0.0.0.0
  if( path[0] == '0')
    return 0;
  if( path[2] == '0')
    return 1;
  if( path[4] == '0')
    return 2;
  if( path[6] == '0')
    return 3;
  return 4;
}

wxTreeItemId BidibIdentDlg::addTreeChild( const wxTreeItemId& root, iONode bidibnode) {
  char key[32];
  StrOp.fmtb(key, "[%s] %08X", wBiDiBnode.getclassmnemonic(bidibnode), wBiDiBnode.getuid(bidibnode) );
  wxTreeItemId item = m_Tree->AppendItem( root, wxString( key, wxConvUTF8));
  MapOp.put( nodeMap, key, (obj)bidibnode);
  MapOp.put( nodePathMap, wBiDiBnode.getpath(bidibnode), (obj)bidibnode);
  return item;
}


void BidibIdentDlg::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "features" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "accessory" ) );
  m_Notebook->SetPageText( 3, wxGetApp().getMsg( "portsetup" ) );
  m_Notebook->SetPageText( 4, wxGetApp().getMsg( "macro" ) );
  m_Notebook->SetPageText( 5, wxGetApp().getMsg( "firmware" ) );

  m_MacroPanel->Enable(false);

  m_BiDiBlogo->SetBitmap(*_img_bidib);

  nodeMap  = MapOp.inst();
  nodePathMap  = MapOp.inst();
  nodeList = ListOp.inst();
  bidibnode = NULL;
  macro = 0;
  macroline = 0;
  macrosize = 0;
  macrolevel = 0;
  macroparam = 0;
  macrosave = false;
  macroapply = false;

  iONode l_RocrailIni = wxGetApp().getFrame()->getRocrailIni();
  if( l_RocrailIni != NULL ) {
    iONode digint = wRocRail.getdigint(l_RocrailIni);
    while( digint != NULL ) {
      iONode bidib = wDigInt.getbidib(digint);
      if( bidib != NULL ) {
        m_IID->SetValue( wxString( wDigInt.getiid(digint), wxConvUTF8) );
        iONode bidibnode = wBiDiB.getbidibnode(bidib);
        while( bidibnode != NULL ) {
          ListOp.add(nodeList, (obj)bidibnode);
          bidibnode = wBiDiB.nextbidibnode( bidib, bidibnode );
        }
        break;
      }
      digint = wRocRail.nextdigint(l_RocrailIni, digint);
    }
  }

  /*
  wxImageList* imgList = new wxImageList(32, 20);
  imgList->Add(wxIcon(pcb_xpm));
  m_Tree->AssignImageList(imgList);
  */
  if( ListOp.size(nodeList) > 0 ) {
    ListOp.sort(nodeList, &__sortPath);

    /*
     *       0.0.0.0    Level 0 Root
     *     1.0.0.0      Level 1
     *   1.1.0.0        Level 2
     * 1.1.1.0          Level 3
     *     2.0.0.0      Level 1
     *   1.2.0.0        Level 2
     */


    iONode* r1 = (iONode*)allocMem(128 * sizeof(iONode));
    iONode** r2 = (iONode**)allocMem(128 * sizeof(iONode*));
    iONode*** r3 = (iONode***)allocMem(128 * sizeof(iONode**));

    // ToDo: Maybe alloc first when needed?
    for(int x=0; x<128; x++) {
      r2[x] = (iONode*)allocMem(128 * sizeof(iONode));
      r3[x] = (iONode**)allocMem(128 * sizeof(iONode*));
      for(int y=0; y<128; y++)
        r3[x][y] = (iONode*)allocMem(128 * sizeof(iONode));
    }

    for( int i = 1; i < ListOp.size(nodeList); i++ ) {
      iONode bidibnode = (iONode)ListOp.get( nodeList, i );
      TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999,"%s", wBiDiBnode.getpath(bidibnode) );
      int childLevel = getLevel(wBiDiBnode.getpath(bidibnode));
      if( childLevel == 1 ) {
        int n = wBiDiBnode.getpath(bidibnode)[0]-'0';
        TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999,"[%d]", n );
        r1[n] = bidibnode;
      }
      if( childLevel == 2 ) {
        int n = wBiDiBnode.getpath(bidibnode)[2]-'0';
        int o = wBiDiBnode.getpath(bidibnode)[0]-'0';
        TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999,"[%d][%d]", n, o );
        r2[n][o] = bidibnode;
      }
      if( childLevel == 3 ) {
        int n = wBiDiBnode.getpath(bidibnode)[4]-'0';
        int o = wBiDiBnode.getpath(bidibnode)[2]-'0';
        int p = wBiDiBnode.getpath(bidibnode)[0]-'0';
        TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999,"[%d][%d][%d]", n, o, p );
        r3[n][o][p] = bidibnode;
      }
    }


    iONode bidibnode = (iONode)ListOp.get( nodeList, 0 );
    iONode rootnode  = (iONode)ListOp.get( nodeList, 0 );

    char key[32];
    int level = 0;
    StrOp.fmtb(key, "[%s] %08X", wBiDiBnode.getclassmnemonic(bidibnode), wBiDiBnode.getuid(bidibnode) );
    wxTreeItemId root  = m_Tree->AddRoot(wxString( key, wxConvUTF8));
    MapOp.put( nodeMap, key, (obj)bidibnode);

    for(int n = 1; n < 127; n++ ) {
      if( r1[n] == NULL )
        break;
      wxTreeItemId child1 = addTreeChild( root, r1[n]);
      for(int o = 1; o < 127; o++ ) {
        if( r2[n][o] == NULL )
          break;
        wxTreeItemId child2 = addTreeChild( child1, r2[n][o]);
        for(int p = 1; p < 127; p++ ) {
          if( r3[n][o][p] == NULL )
            break;
          wxTreeItemId child3 = addTreeChild( child2, r3[n][o][p]);
        }
      }
    }

    for(int x=0; x<128; x++) {
      freeMem(r2[x]);
      for(int y=0; y<128; y++)
        freeMem(r3[x][y]);
      freeMem(r3[x]);
    }

    freeMem(r1);
    freeMem(r2);
    freeMem(r3);



    m_Tree->ExpandAll();
    if( node == NULL ) {
      m_Tree->SelectItem(root, false);
      m_Tree->SelectItem(root, true);
    }
  }

  m_MacroLines->SetColLabelValue(0, wxGetApp().getMsg("delay") );
  m_MacroLines->SetColLabelValue(1, wxGetApp().getMsg("type") );
  m_MacroLines->SetColLabelValue(2, wxGetApp().getMsg("port") );
  m_MacroLines->SetColLabelValue(3, wxGetApp().getMsg("value") );
  m_MacroLines->AutoSizeColumns(false);

  m_MacroType->Append( wxGetApp().getMsg( "default" ) );
  m_MacroType->Append( wxGetApp().getMsg( "lights" ) );
  m_MacroType->Append( wxGetApp().getMsg( "servo" ) );
  m_MacroType->Append( wxGetApp().getMsg( "sound" ) );
  m_MacroType->Append( wxGetApp().getMsg( "motor" ) );
  m_MacroType->Append( wxGetApp().getMsg( "analog" ) );

}

void BidibIdentDlg::onTreeSelChanged( wxTreeEvent& event ) {
  wxString itemText = m_Tree->GetItemText(event.GetItem());
  char* uid = StrOp.dup(itemText.mb_str(wxConvUTF8));
  bidibnode = (iONode)MapOp.get( nodeMap, uid );
  if( bidibnode != NULL ) {
    TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999,"tree selection: %s (%s)", wBiDiBnode.getpath(bidibnode), uid );
    TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999,"uid=%d", wBiDiBnode.getuid(bidibnode) );

    m_Path->SetValue( wxString( wBiDiBnode.getpath(bidibnode), wxConvUTF8) );
    //m_UID->SetValue( wxString( uid, wxConvUTF8 ) );
    m_UID->SetValue( wxString::Format(_T("%d"), wBiDiBnode.getuid(bidibnode) ) );
    m_UIDX->SetValue( wxString::Format(_T("0x%08X"), wBiDiBnode.getuid(bidibnode) ) );
    m_VendorName->SetValue( wxString( m_Vendor[wBiDiBnode.getvendor(bidibnode)&0xFF],wxConvUTF8) );
    m_Class->SetValue( wxString( wBiDiBnode.getclass(bidibnode), wxConvUTF8) );
    m_Version->SetValue( wxString( wBiDiBnode.getversion(bidibnode), wxConvUTF8) );

    SetTitle(wxT("BiDiB: ") + wxString::Format(_T("%08X"), wBiDiBnode.getuid(bidibnode) ) + wxT(" ") + wxString( wBiDiBnode.getclass(bidibnode), wxConvUTF8) );
  }
  else {
    TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999,"node not found: %s", uid );
    TraceOp.dump ( "bidibident", TRCLEVEL_INFO, (char*)uid, StrOp.len(uid) );
  }
  StrOp.free(uid);
}


void BidibIdentDlg::initValues() {
  if( node != NULL && StrOp.equals(wProgram.name(), NodeOp.getName(node)) ) {
    char mnemonic[32];
    char* classname = bidibGetClassName(wProgram.getprod(node), mnemonic);
    TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999, "%s", classname );

    m_Path->SetValue( wxString( wProgram.getfilename(node), wxConvUTF8) );
    m_UID->SetValue( wxString::Format(_T("%d"), wProgram.getmodid(node) ) );
    m_UIDX->SetValue( wxString::Format(_T("0x%08X"), wProgram.getmodid(node) ) );
    m_VendorName->SetValue( wxString( m_Vendor[wProgram.getmanu(node)&0xFF],wxConvUTF8) );
    m_Class->SetValue( wxString( classname, wxConvUTF8) );
    StrOp.free(classname);
    m_Version->SetValue( wxString( wProgram.getstrval1(node), wxConvUTF8) );

    char key[32];
    StrOp.fmtb(key, "[%s] %08X", mnemonic, wProgram.getmodid(node) );
    wxTreeItemId item = findTreeItem( m_Tree->GetRootItem(), wxString( key, wxConvUTF8));
    if( item.IsOk() ) {
      m_Tree->SelectItem(item);
      m_Tree->ScrollTo(item);
    }
  }
  else if( node != NULL && StrOp.equals(wBiDiBnode.name(), NodeOp.getName(node)) ) {
    m_Path->SetValue( wxString( wBiDiBnode.getpath(node), wxConvUTF8) );
    //m_UID->SetValue( wxString( uid, wxConvUTF8 ) );
    m_UID->SetValue( wxString::Format(_T("%d"), wBiDiBnode.getuid(node) ) );
    m_UIDX->SetValue( wxString::Format(_T("0x%08X"), wBiDiBnode.getuid(node) ) );
    m_VendorName->SetValue( wxString( m_Vendor[wBiDiBnode.getvendor(node)&0xFF],wxConvUTF8) );
    m_Class->SetValue( wxString( wBiDiBnode.getclass(node), wxConvUTF8) );
    m_Version->SetValue( wxString( wBiDiBnode.getversion(node), wxConvUTF8) );

    SetTitle(wxT("BiDiB: ") + wxString::Format(_T("%08X"), wBiDiBnode.getuid(node) ) + wxT(" ") + wxString( wBiDiBnode.getclass(node), wxConvUTF8) );
  }
  else {
    TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999, "no node set" );
  }
}


wxTreeItemId BidibIdentDlg::findTreeItem( const wxTreeItemId& root, const wxString& text)
{
  wxTreeItemId item = root;
  wxTreeItemId child;
  wxTreeItemIdValue cookie;
  wxString findtext(text), itemtext;
  bool bFound;

  while(item.IsOk())
  {
    itemtext = m_Tree->GetItemText(item);
    bFound = itemtext == findtext;
    if(bFound)
      return item;
    child = m_Tree->GetFirstChild(item, cookie);
    if(child.IsOk())
      child = findTreeItem(child, text);
    if(child.IsOk())
      return child;
    item = m_Tree->GetNextSibling(item);
  }

  return item;
}


void BidibIdentDlg::onItemActivated( wxTreeEvent& event ) {
}


void BidibIdentDlg::onBeginDrag( wxTreeEvent& event ) {
  wxString itemText = m_Tree->GetItemText(event.GetItem());
  const char* uid = itemText.mb_str(wxConvUTF8);
  iONode bidibnode = (iONode)MapOp.get( nodeMap, uid );

  wxString my_text = _T("bus:")+wxString::Format(_T("%d"), wBiDiBnode.getuid(bidibnode) );
  wxTextDataObject my_data(my_text);
  wxDropSource dragSource( this );
  dragSource.SetData( my_data );
  wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
  //event.Allow();
}


void BidibIdentDlg::onMenu( wxCommandEvent& event ) {
  int path = event.GetId();
  char pathStr[32] = {'\0'};
  StrOp.fmtb( pathStr, "%d.%d.%d.%d", path/1000, (path%1000)/100, (path%100)/10, (path%10) );
  TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999, "%d [%s]", path, pathStr );
  iONode bidibnode = (iONode)MapOp.get( nodePathMap, pathStr );
  wxLaunchDefaultBrowser(wxT("http://www.opendcc.de/bidib/overview/overview.html"), wxBROWSER_NEW_WINDOW );
}

void BidibIdentDlg::onItemRightClick( wxTreeEvent& event ) {
  wxString itemText = m_Tree->GetItemText(event.GetItem());
  const char* uid = itemText.mb_str(wxConvUTF8);
  iONode bidibnode = (iONode)MapOp.get( nodeMap, uid );
  const char* pathStr = wBiDiBnode.getpath(bidibnode);
  int path = (pathStr[0]-'0')*1000 + (pathStr[2]-'0')*100 + (pathStr[4]-'0')*10 + (pathStr[6]-'0');
  wxMenu menu( wxString(wBiDiBnode.getpath( bidibnode ),wxConvUTF8) );
  menu.Append( path, wxGetApp().getMenu("info") );
  menu.Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( BidibIdentDlg::onMenu ), NULL, this );

  PopupMenu(&menu );
}


void BidibIdentDlg::onFeatureSelect( wxCommandEvent& event ) {
  int sel = m_FeatureList->GetSelection();
  if( sel == wxNOT_FOUND )
    return;
  iONode node = (iONode)m_FeatureList->GetClientData(sel);
  if( node != NULL ) {
    m_Feature->SetValue(wProgram.getcv(node));
    m_FeatureValue->SetValue(wProgram.getvalue(node));
  }
}


void BidibIdentDlg::clearFeatureList() {
  while( m_FeatureList->GetCount() > 0 ) {
    iONode node = (iONode)m_FeatureList->GetClientData(0);
    NodeOp.base.del(node);
    m_FeatureList->Delete(0);
  }
}


void BidibIdentDlg::onFeaturesGet( wxCommandEvent& event ) {
  // Get all features.
  clearFeatureList();

  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
    wProgram.setcmd( cmd, wProgram.evgetall );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_bidib);
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void BidibIdentDlg::onFeatureSet( wxCommandEvent& event ) {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
    wProgram.setcmd( cmd, wProgram.set );
    wProgram.setcv( cmd, m_Feature->GetValue() );
    wProgram.setvalue( cmd, m_FeatureValue->GetValue() );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_bidib);
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void BidibIdentDlg::handleFeature(iONode node) {
  char mnemonic[32] = {'\0'};
  char* classname = bidibGetClassName(wProgram.getprod(node), mnemonic);
  StrOp.free(classname);

  char uidKey[32];
  StrOp.fmtb(uidKey, "[%s] %08X", mnemonic, wProgram.getmodid(node) );
  iONode l_bidibnode = (iONode)MapOp.get( nodeMap, uidKey );
  if( l_bidibnode != NULL ) {
    iONode program = (iONode)NodeOp.base.clone(node);
    int feature = wProgram.getcv(node);
    int value   = wProgram.getvalue(node);
    const char* featureName = bidibGetFeatureName(feature);
    m_FeatureList->Append( wxString(featureName,wxConvUTF8), program);

    if( feature == FEATURE_CTRL_MAC_COUNT ) {
      m_MacroList->Clear();
      for( int i = 0; i < value; i++ ) {
        m_MacroList->Append( wxString::Format(_T("macro %d"), i+1));
      }
      if( value > 0 ) {
        m_MacroList->Select(0);
        m_MacroList->SetFirstItem(0);
        m_MacroPanel->Enable(true);
      }
      else {
        m_MacroPanel->Enable(false);
      }
    }
    else if( feature == FEATURE_CTRL_MAC_SIZE ) {
      macrosize = value;
    }
    else if( feature == FEATURE_CTRL_MAC_LEVEL ) {
      macrolevel = value;
    }
  }
  else {
    TraceOp.trc( "bidibident", TRCLEVEL_WARNING, __LINE__, 9999,"bidib node \"%s\" not found", uidKey );
  }
}


void BidibIdentDlg::onServoGet( wxCommandEvent& event ) {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
    wProgram.setcmd( cmd, wProgram.nvget );
    wProgram.setcv( cmd, m_ServoPort->GetValue() );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_bidib);
    wProgram.setporttype(cmd, wProgram.porttype_servo);
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void BidibIdentDlg::onServoSet() {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
    wProgram.setcmd( cmd, wProgram.nvset );
    wProgram.setcv( cmd, m_ServoPort->GetValue() );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_bidib);
    wProgram.setporttype(cmd, m_PortType->GetSelection());
    wProgram.setval1(cmd, m_ServoLeft->GetValue());
    wProgram.setval2(cmd, m_ServoRight->GetValue());
    wProgram.setval3(cmd, m_ServoSpeed->GetValue());
    wProgram.setval4(cmd, 0);
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}

void BidibIdentDlg::onServoLeft( wxScrollEvent& event ) {
  m_ConfigL->SetValue(m_ServoLeft->GetValue());
  onServoSet();
}

void BidibIdentDlg::onServoRight( wxScrollEvent& event ) {
  m_ConfigR->SetValue(m_ServoRight->GetValue());
  onServoSet();
}

void BidibIdentDlg::onServoSpeed( wxScrollEvent& event ) {
  m_ConfigV->SetValue(m_ServoSpeed->GetValue());
  onServoSet();
}

void BidibIdentDlg::onServoReserved( wxScrollEvent& event ) {
  m_ConfigS->SetValue(m_ServoReserved->GetValue());
  onServoSet();
}

void BidibIdentDlg::onServoPort( wxSpinEvent& event ) {
}

void BidibIdentDlg::onServoLeftTest( wxCommandEvent& event ) {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
    wOutput.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wOutput.setbus( cmd, wBiDiBnode.getuid(bidibnode) );
    wOutput.setaddr( cmd, m_ServoPort->GetValue()+1 );
    wOutput.setaccessory( cmd, False );
    wOutput.setporttype(cmd, m_PortType->GetSelection());
    wOutput.setcmd( cmd, wOutput.on );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}

void BidibIdentDlg::onServoRightTest( wxCommandEvent& event ) {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
    wOutput.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wOutput.setbus( cmd, wBiDiBnode.getuid(bidibnode) );
    wOutput.setaddr( cmd, m_ServoPort->GetValue()+1 );
    wOutput.setaccessory( cmd, False );
    wOutput.setporttype(cmd, m_PortType->GetSelection());
    wOutput.setcmd( cmd, wOutput.off );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}

void BidibIdentDlg::onConfigL( wxSpinEvent& event ) {
  m_ServoLeft->SetValue(m_ConfigL->GetValue());
}


void BidibIdentDlg::onConfigR( wxSpinEvent& event ) {
  m_ServoRight->SetValue(m_ConfigR->GetValue());
}


void BidibIdentDlg::onConfigV( wxSpinEvent& event ) {
  m_ServoSpeed->SetValue(m_ConfigV->GetValue());
}

void BidibIdentDlg::onConfigS( wxSpinEvent& event ) {
  m_ServoReserved->SetValue(m_ConfigS->GetValue());
}

void BidibIdentDlg::onConfigLtxt( wxCommandEvent& event ) {
  m_ServoLeft->SetValue(m_ConfigL->GetValue());
}
void BidibIdentDlg::onConfigRtxt( wxCommandEvent& event ) {
  m_ServoRight->SetValue(m_ConfigR->GetValue());
}
void BidibIdentDlg::onConfigVtxt( wxCommandEvent& event ) {
  m_ServoSpeed->SetValue(m_ConfigV->GetValue());
}
void BidibIdentDlg::onConfigStxt( wxCommandEvent& event ) {
  m_ServoReserved->SetValue(m_ConfigS->GetValue());
}



void BidibIdentDlg::onPortType( wxCommandEvent& event ) {
}


void BidibIdentDlg::onSelectUpdateFile( wxCommandEvent& event ) {
  wxString ms_FileExt = _T("PIC HEX (*.HEX;*.hex)|*.HEX;*.hex");
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("openhexfile"),
      wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    //fdlg->GetPath();
    wGui.setopenpath( wxGetApp().getIni(), fdlg->GetPath().mb_str(wxConvUTF8) );
    // strip filename:
    wGui.setopenpath( wxGetApp().getIni(), FileOp.getPath(wGui.getopenpath( wxGetApp().getIni() ) ) );

    TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "reading [%s]...", (const char*)fdlg->GetPath().mb_str(wxConvUTF8));
    iOFile f = FileOp.inst( fdlg->GetPath().mb_str(wxConvUTF8), OPEN_READONLY );
    if( f != NULL ) {
      TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "file opened...");
      m_UpdateFile->SetValue(fdlg->GetPath());
      FILE* fs = FileOp.getStream(f);
      char str[256];
      fgets( str, 256, fs );

      /* until end of stream or error: */
      m_HEXFileText->Clear();
      int lines = 0;
      while( !ferror(fs) && !feof(fs) && lines < 50 ) {
        StrOp.replaceAll(str, '\r', ' ');
        TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "line=[%s]", str);
        m_HEXFileText->AppendText(wxString(str,wxConvUTF8));
        fgets( str, 256, fs );
        lines++;
      };

      m_HEXFileText->ShowPosition(0);

      FileOp.base.del( f );
    }
  }
  fdlg->Destroy();
}

void BidibIdentDlg::onUpdateStart( wxCommandEvent& event ) {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setcmd( cmd, wProgram.writehex );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_bidib);
    wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
    wProgram.setfilename( cmd,  m_UpdateFile->GetValue().mb_str(wxConvUTF8) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
    m_UpdateStart->Enable(false);
    m_UpdateStart->SetLabel( wxGetApp().getMsg( "wait" )+wxT("...") );
  }
}


void BidibIdentDlg::onMacroList( wxCommandEvent& event ) {
  macro = m_MacroList->GetSelection();
}


void BidibIdentDlg::onMacroLineSelected( wxGridEvent& event ) {
  macroline = event.GetRow();
  m_MacroLines->SelectRow(event.GetRow());
  m_MacroLines->MakeCellVisible( event.GetRow(), 0 );
  int val1 = atoi( m_MacroLines->GetCellValue( event.GetRow(), 0 ).mb_str(wxConvUTF8) );
  int val2 = atoi( m_MacroLines->GetCellValue( event.GetRow(), 1 ).mb_str(wxConvUTF8) );
  int val3 = atoi( m_MacroLines->GetCellValue( event.GetRow(), 2 ).mb_str(wxConvUTF8) );
  int val4 = atoi( m_MacroLines->GetCellValue( event.GetRow(), 3 ).mb_str(wxConvUTF8) );

  m_MacroDelay->SetValue(val1);
  if( val2 < 6 )
    m_MacroType->SetSelection(val2);
  else
    m_MacroType->SetSelection(0);
  m_MacroPort->SetValue(val3);
  m_MacroValue->SetValue(val4);
}


void BidibIdentDlg::onMacroApply( wxCommandEvent& event ) {
  macro = m_MacroList->GetSelection();
  macrosave = false;
  macroapply = true;

  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
    wProgram.setcmd( cmd, wProgram.macro_set );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_bidib);
    wProgram.setval1( cmd, macro );
    wProgram.setval2( cmd, macroline );
    wProgram.setval3( cmd, m_MacroDelay->GetValue() );
    wProgram.setval4( cmd, m_MacroType->GetSelection() );
    wProgram.setval5( cmd, m_MacroPort->GetValue() );
    wProgram.setval6( cmd, m_MacroValue->GetValue() );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void BidibIdentDlg::onMacroReload( wxCommandEvent& event ) {
  macro = m_MacroList->GetSelection();
  macroline = 0;
  macroparam = 0;

  m_MacroLines->ClearGrid();
  if( m_MacroLines->GetNumberRows() > 0 )
    m_MacroLines->DeleteRows( 0, m_MacroLines->GetNumberRows() );

  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
    wProgram.setcmd( cmd, wProgram.macro_get );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_bidib);
    wProgram.setval1( cmd, macro );
    wProgram.setval2( cmd, macroline );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void BidibIdentDlg::onMacroSave( wxCommandEvent& event ) {
  macro = m_MacroList->GetSelection();

  if( bidibnode != NULL && m_MacroLines->GetNumberRows() > 0 ) {
    macrosave = true;
    macroline = 0;
    macroparam = BIDIB_MACRO_PARA_START_CLK; // avoid reading the parameters

    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
    wProgram.setcmd( cmd, wProgram.macro_set );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_bidib);
    wProgram.setval1( cmd, macro );
    wProgram.setval2( cmd, macroline );

    int val1 = atoi( m_MacroLines->GetCellValue( macroline, 0 ).mb_str(wxConvUTF8) );
    int val2 = atoi( m_MacroLines->GetCellValue( macroline, 1 ).mb_str(wxConvUTF8) );
    int val3 = atoi( m_MacroLines->GetCellValue( macroline, 2 ).mb_str(wxConvUTF8) );
    int val4 = atoi( m_MacroLines->GetCellValue( macroline, 3 ).mb_str(wxConvUTF8) );
    wProgram.setval3( cmd, val1 );
    wProgram.setval4( cmd, val2 );
    wProgram.setval5( cmd, val3 );
    wProgram.setval6( cmd, val4 );

    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }

}


void BidibIdentDlg::handleAccessory(iONode node) {
  int cv = wProgram.getcv(node);
  if( cv == BIDIB_ACCESSORY_SWITCH_TIME ) {
    int swtime = wProgram.getval1(node);
    m_AccessorySwitchTime->SetValue(swtime & 0x7F);
    m_AccessorySwitchTimeSeconds->SetValue((swtime & 0x80)?true:false);
  }
  else if( cv == BIDIB_ACCESSORY_PARA_MACROMAP ) {
    m_AccessoryMacro1->SetValue(wProgram.getval1(node));
    m_AccessoryMacro2->SetValue(wProgram.getval2(node));
    m_AccessoryMacro3->SetValue(wProgram.getval3(node));
    m_AccessoryMacro4->SetValue(wProgram.getval4(node));
    m_AccessoryMacro5->SetValue(wProgram.getval5(node));
    m_AccessoryMacro6->SetValue(wProgram.getval6(node));
    m_AccessoryMacro7->SetValue(wProgram.getval7(node));
    m_AccessoryMacro8->SetValue(wProgram.getval8(node));
    m_AccessoryMacro9->SetValue(wProgram.getval9(node));
    m_AccessoryMacro10->SetValue(wProgram.getval10(node));
    m_AccessoryMacro11->SetValue(wProgram.getval11(node));
    m_AccessoryMacro12->SetValue(wProgram.getval12(node));
    m_AccessoryMacro13->SetValue(wProgram.getval13(node));
    m_AccessoryMacro14->SetValue(wProgram.getval14(node));
    m_AccessoryMacro15->SetValue(wProgram.getval15(node));
    m_AccessoryMacro16->SetValue(wProgram.getval16(node));
  }
}


void BidibIdentDlg::handleMacro(iONode node) {
  if( macroapply ) {
    macroapply = false;
    int index = wProgram.getval1(node);
    int line = wProgram.getval2(node);
    m_MacroLines->SetCellValue(line, 0, wxString::Format(_T("%d"), wProgram.getval3(node)) );
    m_MacroLines->SetCellValue(line, 1, wxString::Format(_T("%d"), wProgram.getval4(node)) );
    m_MacroLines->SetCellValue(line, 2, wxString::Format(_T("%d"), wProgram.getval5(node)) );
    m_MacroLines->SetCellValue(line, 3, wxString::Format(_T("%d"), wProgram.getval6(node)) );
  }
  else if( wProgram.getcmd(node) == wProgram.macro_get && !macrosave ) {
    int index = wProgram.getval1(node);
    int line = wProgram.getval2(node);

    m_MacroLines->AppendRows();
    m_MacroLines->SetCellValue(m_MacroLines->GetNumberRows()-1, 0, wxString::Format(_T("%d"), wProgram.getval3(node)) );
    m_MacroLines->SetCellValue(m_MacroLines->GetNumberRows()-1, 1, wxString::Format(_T("%d"), wProgram.getval4(node)) );
    m_MacroLines->SetCellValue(m_MacroLines->GetNumberRows()-1, 2, wxString::Format(_T("%d"), wProgram.getval5(node)) );
    m_MacroLines->SetCellValue(m_MacroLines->GetNumberRows()-1, 3, wxString::Format(_T("%d"), wProgram.getval6(node)) );
    m_MacroLines->AutoSizeColumns(false);


    if( line < macrosize && macroline < macrosize) {
      macroline++;
      iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
      wProgram.setcmd( cmd, wProgram.macro_get );
      wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
      wProgram.setlntype(cmd, wProgram.lntype_bidib);
      wProgram.setval1( cmd, macro );
      wProgram.setval2( cmd, macroline );
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }
    else if( macrolevel == 2 && macroparam == 0 ) {
      macroparam = BIDIB_MACRO_PARA_SLOWDOWN;
      iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
      wProgram.setcmd( cmd, wProgram.macro_getparams );
      wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
      wProgram.setlntype(cmd, wProgram.lntype_bidib);
      wProgram.setval1( cmd, macro );
      wProgram.setval2( cmd, macroparam );
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }
  }
  else if( wProgram.getcmd(node) == wProgram.macro_getparams ) {
    int index = wProgram.getval1(node);
    int param = wProgram.getval2(node);
    int valLSB = wProgram.getval3(node);
    int valM1 = wProgram.getval4(node);
    int valM2 = wProgram.getval5(node);
    int valMSB = wProgram.getval6(node);
    int val = valLSB + (valM1 << 8) + (valM2 << 16)  + (valMSB << 24);

    if( param == BIDIB_MACRO_PARA_SLOWDOWN ) {
      m_MacroSlowdown->SetValue(val);
    }
    else if( param == BIDIB_MACRO_PARA_REPEAT ) {
      m_MacroCycles->SetValue(val);
    }
    else if( param == BIDIB_MACRO_PARA_START_CLK ) {
      m_MacroStart1->SetValue(valLSB == 60);
      m_MacroStart30->SetValue(valLSB == 61);
      m_MacroStart15->SetValue(valLSB == 62);
      m_MacroStartHourly->SetValue(valM1 == 24);
      m_MacroStartDaily->SetValue(valM2 == 7);

      if( valLSB > 23 )
        valLSB = 0;
      m_MacroHours->SetValue(valLSB);
      if( valM1 > 59 )
        valM1 = 0;
      m_MacroMinutes->SetValue(valM1);
      if( valM2 > 6 )
        valM2 = 0;
      m_MacroWDay->SetValue(valM2);
    }

    if( macroparam < BIDIB_MACRO_PARA_START_CLK ) {
      macroparam++;
      iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
      wProgram.setcmd( cmd, wProgram.macro_getparams );
      wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
      wProgram.setlntype(cmd, wProgram.lntype_bidib);
      wProgram.setval1( cmd, macro );
      wProgram.setval2( cmd, macroparam );
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }

  }

  else if( wProgram.getcmd(node) == wProgram.macro_get && macrosave ) {
    int index = wProgram.getval1(node);
    int line = wProgram.getval2(node);
    macroline++;
    if( m_MacroLines->GetNumberRows() > macroline ) {
      iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
      wProgram.setcmd( cmd, wProgram.macro_set );
      wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
      wProgram.setlntype(cmd, wProgram.lntype_bidib);
      wProgram.setval1( cmd, macro );
      wProgram.setval2( cmd, macroline );

      int val1 = atoi( m_MacroLines->GetCellValue( macroline, 0 ).mb_str(wxConvUTF8) );
      int val2 = atoi( m_MacroLines->GetCellValue( macroline, 1 ).mb_str(wxConvUTF8) );
      int val3 = atoi( m_MacroLines->GetCellValue( macroline, 2 ).mb_str(wxConvUTF8) );
      int val4 = atoi( m_MacroLines->GetCellValue( macroline, 3 ).mb_str(wxConvUTF8) );
      wProgram.setval3( cmd, val1 );
      wProgram.setval4( cmd, val2 );
      wProgram.setval5( cmd, val3 );
      wProgram.setval6( cmd, val4 );

      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }
    else {
      macrosave = false;
      iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
      wProgram.setcmd( cmd, wProgram.macro_setparams );
      wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
      wProgram.setlntype(cmd, wProgram.lntype_bidib);
      wProgram.setval1(cmd, macro );
      wProgram.setval2(cmd, BIDIB_MACRO_PARA_SLOWDOWN );
      wProgram.setval3(cmd, m_MacroSlowdown->GetValue() );
      wProgram.setval4(cmd, 0 );
      wProgram.setval5(cmd, 0 );
      wProgram.setval6(cmd, 0 );
      wxGetApp().sendToRocrail( cmd );

      wProgram.setval2(cmd, BIDIB_MACRO_PARA_REPEAT );
      wProgram.setval3(cmd, m_MacroCycles->GetValue() );
      wProgram.setval4(cmd, 0 );
      wProgram.setval5(cmd, 0 );
      wProgram.setval6(cmd, 0 );
      wxGetApp().sendToRocrail( cmd );

      wProgram.setval2(cmd, BIDIB_MACRO_PARA_START_CLK );
      wProgram.setval3(cmd, m_MacroStartHourly->IsChecked()?24:m_MacroHours->GetValue() );
      if( m_MacroStart1->IsChecked() )
        wProgram.setval4(cmd, 60 );
      if( m_MacroStart30->IsChecked() )
        wProgram.setval4(cmd, 61 );
      if( m_MacroStart15->IsChecked() )
        wProgram.setval4(cmd, 62 );
      else
        wProgram.setval4(cmd, m_MacroMinutes->GetValue() );
      wProgram.setval5(cmd, m_MacroStartDaily->IsChecked()?7:m_MacroWDay->GetValue() );
      wProgram.setval6(cmd, 0 );
      wxGetApp().sendToRocrail( cmd );

      cmd->base.del(cmd);
    }
  }

}


void BidibIdentDlg::onMacroEveryMinute( wxCommandEvent& event ) {
  if( event.GetEventObject() == m_MacroStart1 && m_MacroStart1->IsChecked() ) {
    m_MacroStart30->SetValue(false);
    m_MacroStart15->SetValue(false);
  }
  else if( event.GetEventObject() == m_MacroStart15 && m_MacroStart15->IsChecked() ) {
    m_MacroStart1->SetValue(false);
    m_MacroStart30->SetValue(false);
  }
  else if( event.GetEventObject() == m_MacroStart30 && m_MacroStart30->IsChecked() ) {
    m_MacroStart1->SetValue(false);
    m_MacroStart15->SetValue(false);
  }
}


void BidibIdentDlg::onMacroExport( wxCommandEvent& event ) {
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxString ms_FileExt = _T("Macro (*.xml)|*.xml");
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("export"), wxString(l_openpath,wxConvUTF8),
                       wxString::Format( _T("bidib-macro-%d.xml"), m_MacroList->GetSelection()), ms_FileExt, wxFD_SAVE);
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
      wMacro.setuid(macro, atoi(m_UID->GetValue().mb_str(wxConvUTF8)) );
      wMacro.setnr(macro, m_MacroList->GetSelection());
      // Set level 2 parameters.
      wMacro.sethours(macro, m_MacroHours->GetValue());
      wMacro.setminutes(macro, m_MacroMinutes->GetValue());
      wMacro.setwday(macro, m_MacroWDay->GetValue());

      for( int i = 0; i < m_MacroLines->GetNumberRows(); i++ ) {
        iONode macroline = NodeOp.inst( wMacroLine.name(), macro, ELEMENT_NODE );
        NodeOp.addChild(macro, macroline);
        wMacroLine.setnr(macroline, i);
        // ToDo: Set line values.
        int val1 = atoi( m_MacroLines->GetCellValue( i, 0 ).mb_str(wxConvUTF8) );
        int val2 = atoi( m_MacroLines->GetCellValue( i, 1 ).mb_str(wxConvUTF8) );
        int val3 = atoi( m_MacroLines->GetCellValue( i, 2 ).mb_str(wxConvUTF8) );
        int val4 = atoi( m_MacroLines->GetCellValue( i, 3 ).mb_str(wxConvUTF8) );
        wMacroLine.setdelay( macroline, val1 );
        wMacroLine.setporttype( macroline, val2 );
        wMacroLine.setport( macroline, val3 );
        wMacroLine.setstatus( macroline, val4 );
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


void BidibIdentDlg::onMacroImport( wxCommandEvent& event ) {
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
          // Set level 2 parameters.
          m_MacroHours->SetValue( wMacro.gethours(macro) );
          m_MacroMinutes->SetValue( wMacro.getminutes(macro) );
          m_MacroWDay->SetValue( wMacro.getwday(macro) );

          m_MacroLines->ClearGrid();
          if( m_MacroLines->GetNumberRows() > 0 )
            m_MacroLines->DeleteRows( 0, m_MacroLines->GetNumberRows() );

          iONode macroline = wMacro.getmacroline(macro);
          while( macroline != NULL ) {
            m_MacroLines->AppendRows();
            m_MacroLines->SetCellValue(m_MacroLines->GetNumberRows()-1, 0, wxString::Format(_T("%d"), wMacroLine.getdelay(macroline)) );
            m_MacroLines->SetCellValue(m_MacroLines->GetNumberRows()-1, 1, wxString::Format(_T("%d"), wMacroLine.getporttype(macroline)) );
            m_MacroLines->SetCellValue(m_MacroLines->GetNumberRows()-1, 2, wxString::Format(_T("%d"), wMacroLine.getport(macroline)) );
            m_MacroLines->SetCellValue(m_MacroLines->GetNumberRows()-1, 3, wxString::Format(_T("%d"), wMacroLine.getstatus(macroline)) );
            m_MacroLines->AutoSizeColumns(false);

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


void BidibIdentDlg::onMacroSaveMacro( wxCommandEvent& event ) {
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
  wProgram.setcmd( cmd, wProgram.macro_save );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_bidib);
  wProgram.setval1( cmd, macro );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void BidibIdentDlg::onMacroDeleteMacro( wxCommandEvent& event ) {
  m_MacroLines->ClearGrid();
  if( m_MacroLines->GetNumberRows() > 0 )
    m_MacroLines->DeleteRows( 0, m_MacroLines->GetNumberRows() );

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
  wProgram.setcmd( cmd, wProgram.macro_delete );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_bidib);
  wProgram.setval1( cmd, macro );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void BidibIdentDlg::onMacroRestoreMacro( wxCommandEvent& event ) {
  m_MacroLines->ClearGrid();
  if( m_MacroLines->GetNumberRows() > 0 )
    m_MacroLines->DeleteRows( 0, m_MacroLines->GetNumberRows() );

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
  wProgram.setcmd( cmd, wProgram.macro_restore );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setlntype(cmd, wProgram.lntype_bidib);
  wProgram.setval1( cmd, macro );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


void BidibIdentDlg::onMacroTest( wxCommandEvent& event ) {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
    wOutput.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wOutput.setbus( cmd, wBiDiBnode.getuid(bidibnode) );
    wOutput.setaddr( cmd, macro+1 );
    wOutput.setporttype(cmd, wProgram.porttype_macro);
    wOutput.setcmd( cmd, wOutput.on );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void BidibIdentDlg::onAccessoryOnTest( wxCommandEvent& event ) {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
    wOutput.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wOutput.setbus( cmd, wBiDiBnode.getuid(bidibnode) );
    wOutput.setaddr( cmd, m_AccessoryPort->GetValue()+1 );
    wOutput.setaccessory( cmd, True );
    wOutput.setcmd( cmd, wOutput.on );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void BidibIdentDlg::onAccessoryOffTest( wxCommandEvent& event ) {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
    wOutput.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wOutput.setbus( cmd, wBiDiBnode.getuid(bidibnode) );
    wOutput.setaddr( cmd, m_AccessoryPort->GetValue()+1 );
    wOutput.setaccessory( cmd, True );
    wOutput.setcmd( cmd, wOutput.off );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void BidibIdentDlg::onAccessoryReadOptions( wxCommandEvent& event ) {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
    wProgram.setcmd( cmd, wProgram.acc_getparam );
    wProgram.setaddr( cmd, m_AccessoryPort->GetValue() );
    wProgram.setcv( cmd, BIDIB_ACCESSORY_SWITCH_TIME );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_bidib);
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void BidibIdentDlg::onAccessoryWriteOptions( wxCommandEvent& event ) {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
    wProgram.setcmd( cmd, wProgram.acc_setparam );
    wProgram.setaddr( cmd, m_AccessoryPort->GetValue() );
    wProgram.setcv( cmd, BIDIB_ACCESSORY_SWITCH_TIME );
    wProgram.setval1( cmd, m_AccessorySwitchTime->GetValue() + (m_AccessorySwitchTimeSeconds->IsChecked()?0x80:0x00) );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_bidib);
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void BidibIdentDlg::onAccessoryReadMacroMap( wxCommandEvent& event ) {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
    wProgram.setcmd( cmd, wProgram.acc_getparam );
    wProgram.setaddr( cmd, m_AccessoryPort->GetValue() );
    wProgram.setcv( cmd, BIDIB_ACCESSORY_PARA_MACROMAP );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_bidib);
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}


void BidibIdentDlg::onAccessoryWriteMacroMap( wxCommandEvent& event ) {
  if( bidibnode != NULL ) {
    iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(cmd, wBiDiBnode.getuid(bidibnode));
    wProgram.setcmd( cmd, wProgram.acc_setparam );
    wProgram.setaddr( cmd, m_AccessoryPort->GetValue() );
    wProgram.setcv( cmd, BIDIB_ACCESSORY_PARA_MACROMAP );
    wProgram.setval1( cmd, m_AccessoryMacro1->GetValue() );
    wProgram.setval2( cmd, m_AccessoryMacro2->GetValue() );
    wProgram.setval3( cmd, m_AccessoryMacro3->GetValue() );
    wProgram.setval4( cmd, m_AccessoryMacro4->GetValue() );
    wProgram.setval5( cmd, m_AccessoryMacro5->GetValue() );
    wProgram.setval6( cmd, m_AccessoryMacro6->GetValue() );
    wProgram.setval7( cmd, m_AccessoryMacro7->GetValue() );
    wProgram.setval8( cmd, m_AccessoryMacro8->GetValue() );
    wProgram.setval9( cmd, m_AccessoryMacro9->GetValue() );
    wProgram.setval10( cmd, m_AccessoryMacro10->GetValue() );
    wProgram.setval11( cmd, m_AccessoryMacro11->GetValue() );
    wProgram.setval12( cmd, m_AccessoryMacro12->GetValue() );
    wProgram.setval13( cmd, m_AccessoryMacro13->GetValue() );
    wProgram.setval14( cmd, m_AccessoryMacro14->GetValue() );
    wProgram.setval15( cmd, m_AccessoryMacro15->GetValue() );
    wProgram.setval16( cmd, m_AccessoryMacro16->GetValue() );
    wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
    wProgram.setlntype(cmd, wProgram.lntype_bidib);
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
}

