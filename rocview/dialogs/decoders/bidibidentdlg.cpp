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

#include "rocs/public/trace.h"

#include "rocview/public/guiapp.h"

#include "rocutils/public/vendors.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/BiDiB.h"
#include "rocrail/wrapper/public/BiDiBnode.h"
#include "rocdigs/impl/bidib/bidibutils.h"

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
}


void BidibIdentDlg::onCancel( wxCommandEvent& event ) {
  EndModal( 0 );
}


void BidibIdentDlg::onOK( wxCommandEvent& event ) {
  wxClipboard* cb = new wxClipboard();
  if( cb->Open() ) {
    cb->SetData( new wxTextDataObject(m_UID->GetValue()) );
    cb->Close();
  }
  delete cb;

  EndModal( wxID_OK );
}


void BidibIdentDlg::event(iONode node) {
  this->node = node;
  initValues();
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
  StrOp.fmtb(key, "%08X", wBiDiBnode.getuid(bidibnode) );
  wxTreeItemId item = m_Tree->AppendItem( root, wxString( key, wxConvUTF8));
  MapOp.put( nodeMap, key, (obj)bidibnode);
  return item;
}


void BidibIdentDlg::initLabels() {
  nodeMap  = MapOp.inst();
  nodeList = ListOp.inst();
  iONode l_RocrailIni = wxGetApp().getFrame()->getRocrailIni();
  if( l_RocrailIni != NULL ) {
    iONode digint = wRocRail.getdigint(l_RocrailIni);
    if( digint != NULL ) {
      iONode bidib = wDigInt.getbidib(digint);
      if( bidib != NULL ) {
        iONode bidibnode = wBiDiB.getbidibnode(bidib);
        while( bidibnode != NULL ) {
          ListOp.add(nodeList, (obj)bidibnode);
          bidibnode = wBiDiB.nextbidibnode( bidib, bidibnode );
        }
      }
    }
  }

  /*
  wxImageList* imgList = new wxImageList(32, 20);
  imgList->Add(wxIcon(pcb_xpm));
  m_Tree->AssignImageList(imgList);
  */
  m_Setup->Enable(false);

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

    for(int x=0; x<128; x++) {
      r2[x] = (iONode*)allocMem(128 * sizeof(iONode));
      r3[x] = (iONode**)allocMem(128 * sizeof(iONode*));
      for(int y=0; y<128; y++)
        r3[x][y] = (iONode*)allocMem(128 * sizeof(iONode));
    }

    TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999,"r1=%d r2=%d r3=%d", r1, r2, r3 );

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

    char key[32];
    int level = 0;
    StrOp.fmtb(key, "%08X", wBiDiBnode.getuid(bidibnode) );
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
      freeMem(r3[x]);
      for(int y=0; y<128; y++)
        freeMem(r3[x][y]);
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

}

void BidibIdentDlg::onTreeSelChanged( wxTreeEvent& event ) {
  wxString itemText = m_Tree->GetItemText(event.GetItem());
  const char* uid = itemText.mb_str(wxConvUTF8);
  iONode bidibnode = (iONode)MapOp.get( nodeMap, uid );
  TraceOp.trc( "bidibident", TRCLEVEL_INFO, __LINE__, 9999,"tree selection: %s", wBiDiBnode.getpath(bidibnode) );

  m_Path->SetValue( wxString( wBiDiBnode.getpath(bidibnode), wxConvUTF8) );
  //m_UID->SetValue( wxString( uid, wxConvUTF8 ) );
  m_UID->SetValue( wxString::Format(_T("%d"), wBiDiBnode.getuid(bidibnode) ) );
  m_VendorName->SetValue( wxString( m_Vendor[wBiDiBnode.getvendor(bidibnode)&0xFF],wxConvUTF8) );
  m_Class->SetValue( wxString( wBiDiBnode.getclass(bidibnode), wxConvUTF8) );
}


void BidibIdentDlg::initValues() {
  char* classname = bidibGetClassName(wProgram.getprod(node));
  m_Path->SetValue( wxString( wProgram.getfilename(node), wxConvUTF8) );
  m_UID->SetValue( wxString::Format(_T("%d"), wProgram.getmodid(node) ) );
  m_VendorName->SetValue( wxString( m_Vendor[wProgram.getmanu(node)&0xFF],wxConvUTF8) );
  m_Class->SetValue( wxString( classname, wxConvUTF8) );
  StrOp.free(classname);

  char key[32];
  StrOp.fmtb(key, "%08X", wProgram.getmodid(node) );
  wxTreeItemId item = findTreeItem( m_Tree->GetRootItem(), wxString( key, wxConvUTF8));
  if( item.IsOk() ) {
    m_Tree->SelectItem(item);
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


void BidibIdentDlg::onSetup( wxCommandEvent& event ) {

}
