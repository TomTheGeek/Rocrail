/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

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
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/event.h"
#endif

#ifdef __linux__
#include <sys/resource.h>
#endif

#include <wx/image.h>
#include <wx/filedlg.h>

#include "rocview/public/guiapp.h"
#include "rocview/wrapper/public/Gui.h"
#include "rocrail/wrapper/public/CVByte.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Loc.h"

#include "rocs/public/system.h"

#include "rocprodlg.h"

#include "rocview/dialogs/speedcurvedlg.h"
#include "rocview/dialogs/decconfigdlg.h"

#include "rocview/xpm/nopict.xpm"

RocProDlg::RocProDlg( wxWindow* parent )
:
RocProDlgGen( parent )
{
  m_DecFilename = NULL;
  m_LocoProps = NULL;
  m_CVMap = MapOp.inst();
  m_LocoImage->SetBitmap( wxBitmap(nopict_xpm) );
  initLocMap();
}

void RocProDlg::onTreeSelChanged( wxTreeEvent& event )
{
  wxString itemText = m_DecTree->GetItemText(event.GetItem());
  const char* desc = itemText.mb_str(wxConvUTF8);
  iONode cv = (iONode)MapOp.get( m_CVMap, desc );
  if( cv != NULL ) {
    m_Info->SetValue(wxString( wCVByte.getinfo(cv), wxConvUTF8));
    m_Nr->SetValue( wxString::Format(_T("%d"), wCVByte.getnr(cv)) );

    iONode lococv = getLocoCV(wCVByte.getnr(cv));
    if( lococv != NULL ) {
      setCVVal(wCVByte.getvalue(lococv));
    }
  }
  else {
    // catagory
    m_Info->SetValue(wxString( "", wxConvUTF8));
    m_Nr->SetValue( wxString::Format(_T("%d"), 0) );
  }
}

void RocProDlg::onOpen( wxCommandEvent& event )
{
  wxString ms_FileExt = wxGetApp().getMsg("planfiles"); // ToDo: "decfiles"
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "openpath=%s", l_openpath );
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("opendecfile"), wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxFD_OPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    if( m_DecFilename != NULL )
      StrOp.free(m_DecFilename);
    m_DecFilename = StrOp.dup(fdlg->GetPath().mb_str(wxConvUTF8));
    wGui.setopenpath( wxGetApp().getIni(), m_DecFilename );
    wGui.setopenpath( wxGetApp().getIni(), FileOp.getPath(wGui.getopenpath( wxGetApp().getIni() ) ) );
    m_DecFile->SetValue(wxString(FileOp.ripPath(m_DecFilename),wxConvUTF8));

    if( parseDecFile() ) {
      m_DecTree->DeleteAllItems();
      MapOp.clear(m_CVMap);
      wxTreeItemId root  = m_DecTree->AddRoot(wxString( NodeOp.getStr(m_DecNode, "type", "?"), wxConvUTF8));
      iOMap catMap = MapOp.inst();
      int cnt = NodeOp.getChildCnt(m_DecNode);
      for( int i = 0; i < cnt; i++ ) {
        iONode cv = NodeOp.getChild(m_DecNode, i);
        const char* catName = wCVByte.getcat(cv);
        wxTreeItemId* pcat = (wxTreeItemId*)MapOp.get( catMap, catName );
        wxTreeItemId cat;
        if( pcat == NULL ) {
          cat = m_DecTree->AppendItem( root, wxString( catName, wxConvUTF8));
          pcat = &cat;
          MapOp.put(catMap, catName, (obj)new wxTreeItemId(cat.GetID()) );
        }
        else {
          cat = *pcat;
        }
        m_DecTree->AppendItem( cat, wxString( wCVByte.getdesc(cv), wxConvUTF8));
        MapOp.put( m_CVMap, wCVByte.getdesc(cv), (obj)cv);
      }
      m_DecTree->ExpandAll();
    }
  }
  fdlg->Destroy();
}


bool RocProDlg::parseDecFile() {
  if( StrOp.len(m_DecFilename) > 0 && FileOp.exist(m_DecFilename) ) {
    iOFile f = FileOp.inst( m_DecFilename, OPEN_READONLY );
    char* buffer = (char*)allocMem( FileOp.size( f ) +1 );
    FileOp.read( f, buffer, FileOp.size( f ) );
    FileOp.base.del( f );
    iODoc doc = DocOp.parse( buffer );
    if( doc != NULL ) {
      m_DecNode = DocOp.getRootNode( doc );
      DocOp.base.del( doc );
      return true;
    }
    else {
      TraceOp.trc( "frame", TRCLEVEL_EXCEPTION, __LINE__, 9999, "DecFile [%s] is not parseable!", m_DecFilename );
    }
  }
  return false;
}


void RocProDlg::onOK( wxCommandEvent& event )
{
  EndModal( wxID_OK );
}

void RocProDlg::onClose( wxCloseEvent& event ) {
  EndModal(0);
}

void RocProDlg::event(iONode node) {

}

static int locComparator(obj* o1, obj* o2) {
  if( *o1 == NULL || *o2 == NULL )
    return 0;
  return strcmp( wLoc.getid( (iONode)*o1 ), wLoc.getid( (iONode)*o2 ) );
}

void RocProDlg::initLocMap(const char* locid) {
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
      int i;
      int cnt = NodeOp.getChildCnt( lclist );
      iOList list = ListOp.inst();

      for( i = 0; i < cnt; i++ ) {
        iONode lc = NodeOp.getChild( lclist, i );
        ListOp.add( list, (obj)lc );
      }
      // Sort the list:
      ListOp.sort( list, locComparator );

      for( i = 0; i < ListOp.size( list ); i++ ) {
        iONode lc = (iONode)ListOp.get( list, i );
        if( lc == NULL )
          continue;
        const char* id = wLoc.getid( lc );
      }

      for( i = 0; i < cnt; i++ ) {
        iONode lc = (iONode)ListOp.get( list, i );
        if( lc == NULL )
          continue;
        const char* id = wLoc.getid( lc );
        if( id != NULL && wLoc.getaddr(lc) > 0 && wLoc.isshow(lc) ) {
          m_LocoList->Append( wxString(id,wxConvUTF8), (void*)lc );
        }
      }
      if( cnt > 0 ) {
        if( locid == NULL )
          m_LocoList->SetSelection(0);
        else
          m_LocoList->SetStringSelection(wxString(locid,wxConvUTF8));
        wxCommandEvent event( 0, -1 );
        onLocoList(event);
      }

      ListOp.base.del( list );
    }
  }
}



void RocProDlg::onLocoList(wxCommandEvent& event) {
  if( m_LocoList->GetSelection() == wxNOT_FOUND )
    return;

  m_LocoProps = (iONode)m_LocoList->GetClientData(m_LocoList->GetSelection());

  if( m_LocoProps != NULL && wLoc.getimage( m_LocoProps ) != NULL && StrOp.len(wLoc.getimage( m_LocoProps )) > 0 ) {
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( wLoc.getimage( m_LocoProps ), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( wLoc.getimage( m_LocoProps ), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;

    const char* imagepath = wGui.getimagepath(wxGetApp().getIni());
    static char pixpath[256];
    StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( wLoc.getimage( m_LocoProps ) ) );

    if( FileOp.exist(pixpath)) {
      TraceOp.trc( "locdialog", TRCLEVEL_INFO, __LINE__, 9999, "picture [%s]", pixpath );
      m_LocoImage->SetBitmap( wxBitmap(wxString(pixpath,wxConvUTF8), bmptype) );
    }
    else {
      TraceOp.trc( "locdialog", TRCLEVEL_WARNING, __LINE__, 9999, "picture [%s] not found", pixpath );
      m_LocoImage->SetBitmap( wxBitmap(nopict_xpm) );
    }
    m_LocoImage->SetToolTip(wxString(wLoc.getdesc( m_LocoProps ),wxConvUTF8));
  }
  else {
    m_LocoImage->SetBitmap( wxBitmap(nopict_xpm) );
  }
  m_LocoImage->Refresh();

}

iONode RocProDlg::getLocoCV(int nr) {
  if( m_LocoProps != NULL ) {
    iONode cv = wLoc.getcvbyte(m_LocoProps);
    while( cv != NULL ) {
      if( wCVByte.getnr(cv) == nr )
        return cv;
      cv = wLoc.nextcvbyte(m_LocoProps, cv);
    }
  }
  return NULL;
}

void RocProDlg::setCVVal(int val) {
  m_Value->SetValue(val);
  m_ValueSlider->SetValue(val);
  m_Bit0->SetValue(val&0x01?true:false);
  m_Bit1->SetValue(val&0x02?true:false);
  m_Bit2->SetValue(val&0x04?true:false);
  m_Bit3->SetValue(val&0x08?true:false);
  m_Bit4->SetValue(val&0x10?true:false);
  m_Bit5->SetValue(val&0x20?true:false);
  m_Bit6->SetValue(val&0x40?true:false);
  m_Bit7->SetValue(val&0x80?true:false);
}


void RocProDlg::onConfig( wxCommandEvent& event ) {
  DecConfigDlg*  dlg = new DecConfigDlg(this, 0 );
  int rc = dlg->ShowModal();
  if( rc == wxID_OK ) {
    int val = dlg->getConfig();
  }
  dlg->Destroy();
}

void RocProDlg::onVCurve( wxCommandEvent& event ) {
  int m_Curve[28];
  SpeedCurveDlg*  dlg = new SpeedCurveDlg(this, m_Curve );
  if( wxID_OK == dlg->ShowModal() ) {
    int* newCurve = dlg->getCurve();
  }
  dlg->Destroy();
}
