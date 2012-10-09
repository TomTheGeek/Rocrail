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

#include "rocprodlg.h"

RocProDlg::RocProDlg( wxWindow* parent )
:
RocProDlgGen( parent )
{
  m_DecFilename = NULL;
  m_CVMap = MapOp.inst();
}

void RocProDlg::onTreeSelChanged( wxTreeEvent& event )
{
  wxString itemText = m_DecTree->GetItemText(event.GetItem());
  const char* desc = itemText.mb_str(wxConvUTF8);
  iONode cv = (iONode)MapOp.get( m_CVMap, desc );
  if( cv != NULL ) {
    m_Info->SetValue(wxString( NodeOp.getStr(cv, "desc", "?"), wxConvUTF8));
  }
  else {
    // catagory
    m_Info->SetValue(wxString( "", wxConvUTF8));
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
      wxTreeItemId root  = m_DecTree->AddRoot(wxString( NodeOp.getStr(m_DecNode, "type", "?"), wxConvUTF8));
      iOMap catMap = MapOp.inst();
      int cnt = NodeOp.getChildCnt(m_DecNode);
      for( int i = 0; i < cnt; i++ ) {
        iONode cv = NodeOp.getChild(m_DecNode, i);
        const char* catName = NodeOp.getStr(cv, "cat", "?");
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
        m_DecTree->AppendItem( cat, wxString( NodeOp.getStr(cv, "desc", "?"), wxConvUTF8));
        MapOp.put( m_CVMap, NodeOp.getStr(cv, "desc", "?"), (obj)cv);
      }
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
