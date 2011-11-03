/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "locseldlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "locseldlg.h"

////@begin XPM images
////@end XPM images

#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/FunDef.h"

#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/MIC.h"

#include "rocs/public/trace.h"
#include "rocs/public/system.h"

#include "rocview/xpm/nopict.xpm"


/*!
 * LocSelDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( LocSelDlg, wxDialog )

/*!
 * LocSelDlg event table definition
 */

BEGIN_EVENT_TABLE( LocSelDlg, wxDialog )

////@begin LocSelDlg event table entries
    EVT_BUTTON( ID_BITMAPBUTTON_SEL_LOC, LocSelDlg::OnBitmapbuttonSelLocClick )

    EVT_LISTBOX( ID_LISTBOX_SEL_LOC, LocSelDlg::OnListboxSelLocSelected )
    EVT_LISTBOX_DCLICK( ID_LISTBOX_SEL_LOC, LocSelDlg::OnListboxSelLocDoubleClicked )

////@end LocSelDlg event table entries
  EVT_LEFT_DOWN   (LocSelDlg::OnStop   )
  EVT_RIGHT_DOWN  (LocSelDlg::OnEscape )
  EVT_MIDDLE_DOWN (LocSelDlg::OnSpeed  )
  EVT_MOUSEWHEEL  (LocSelDlg::OnSpeed  )

END_EVENT_TABLE()

/*!
 * LocSelDlg constructors
 */

LocSelDlg::LocSelDlg( )
{
}

LocSelDlg::LocSelDlg( wxWindow* parent, iONode props, bool mic, const char* locid )
{
  m_Props = props;
  m_MICmode = mic;
  m_MICini = NULL;
  m_LocID = locid;
  if( m_MICmode ) {
    m_MICini = wGui.getmic( wxGetApp().getIni() );
    if( m_MICini == NULL ) {
      m_MICini = NodeOp.inst( wMIC.name(), wxGetApp().getIni(), ELEMENT_NODE );
      NodeOp.addChild( wxGetApp().getIni(), m_MICini );
    }
  } else {



  }


  Create( parent, -1, wxGetApp().getMsg("locseldlg") );
  InitIndex();
  GetSizer()->Layout();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

}

void LocSelDlg::SelectNext() {
  int cnt = m_List->GetCount();
  int sel = m_List->GetSelection();
  if( sel == wxNOT_FOUND )
    m_List->Select( 0 );
  else if( cnt > 0 && sel + 1 < cnt ) {
    m_List->Select( sel + 1 );
    m_List->SetFirstItem( sel + 1 );
  }

  m_Props = wxGetApp().getFrame()->findLoc( m_List->GetStringSelection().mb_str(wxConvUTF8) );
  InitValues();
}


void LocSelDlg::SelectPrev() {
  int cnt = m_List->GetCount();
  int sel = m_List->GetSelection();
  if( sel == wxNOT_FOUND )
    m_List->Select( 0 );
  else if( sel > 0 ) {
    m_List->Select( sel - 1 );
    m_List->SetFirstItem( sel - 1 );
  }

  m_Props = wxGetApp().getFrame()->findLoc( m_List->GetStringSelection().mb_str(wxConvUTF8) );
  InitValues();
}


void LocSelDlg::InitValues() {
  TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, "InitValues %s", wLoc.getid( m_Props ) );
  // Init General
  if( wLoc.getimage( m_Props ) != NULL ) {
    wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
    if( StrOp.endsWithi( wLoc.getimage( m_Props ), ".gif" ) )
      bmptype = wxBITMAP_TYPE_GIF;
    else if( StrOp.endsWithi( wLoc.getimage( m_Props ), ".png" ) )
      bmptype = wxBITMAP_TYPE_PNG;

    const char* imagepath = wGui.getimagepath(wxGetApp().getIni());
    const char* imagename = FileOp.ripPath( wLoc.getimage( m_Props ) );
    static char pixpath[256];
    StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), imagename );

    if( imagename != NULL && StrOp.len(imagename) > 0 && FileOp.exist(pixpath)) {
      TraceOp.trc( "locdlg", TRCLEVEL_INFO, __LINE__, 9999, "picture [%s]", pixpath );
      m_LocImageIndex->SetBitmapLabel( wxBitmap(wxString(pixpath,wxConvUTF8), bmptype) );
    }
    else {
      TraceOp.trc( "locdlg", TRCLEVEL_WARNING, __LINE__, 9999, "picture [%s] not found", pixpath );
      m_LocImageIndex->SetBitmapLabel( wxBitmap(nopict_xpm) );
    }
    m_LocImageIndex->SetToolTip(wxString(wLoc.getdesc( m_Props ),wxConvUTF8));


    //m_LocImage->SetBitmapLabel( wxBitmap(wxString(wLoc.getimage( m_Props ),wxConvUTF8), bmptype) );
    //m_LocImageIndex->SetBitmapLabel( wxBitmap(wxString(wLoc.getimage( m_Props ),wxConvUTF8), bmptype) );
  }
  else {
    m_LocImageIndex->SetBitmapLabel( wxBitmap(nopict_xpm) );
    //m_LocImageIndex->SetBitmapLabel( wxBitmap(nopict_xpm) );
  }
  m_LocImageIndex->Refresh();

}

void LocSelDlg::InitIndex() {
  TraceOp.trc( "app", TRCLEVEL_INFO, __LINE__, 9999, "InitIndex" );
  m_List->Clear();
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode lclist = wPlan.getlclist( model );
    if( lclist != NULL ) {
      int cnt = NodeOp.getChildCnt( lclist );
      for( int i = 0; i < cnt; i++ ) {
        iONode lc = NodeOp.getChild( lclist, i );
        const char* id = wLoc.getid( lc );
        if( id != NULL && wLoc.isshow(lc) ) {
          m_List->Append( wxString(id,wxConvUTF8) );
        }
      }
      if( m_Props != NULL ) {
        m_List->SetStringSelection( wxString(wLoc.getid( m_Props ),wxConvUTF8) );
        m_List->SetFirstItem( wxString(wLoc.getid( m_Props ),wxConvUTF8) );
        InitValues();
      }
      else if( m_LocID != NULL && StrOp.len(m_LocID) > 0 ) {
        m_List->SetStringSelection( wxString(m_LocID,wxConvUTF8) );
        m_List->SetFirstItem( wxString(m_LocID,wxConvUTF8) );
        m_Props = wxGetApp().getFrame()->findLoc( m_List->GetStringSelection().mb_str(wxConvUTF8) );
        InitValues();
      }
    }
  }
}


/*!
 * LocSelDlg creator
 */

bool LocSelDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin LocSelDlg member initialisation
    m_LocImageIndex = NULL;
    m_List = NULL;
////@end LocSelDlg member initialisation

////@begin LocSelDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end LocSelDlg creation
    return true;
}

/*!
 * Control creation for LocSelDlg
 */

void LocSelDlg::CreateControls()
{
////@begin LocSelDlg content construction
    LocSelDlg* itemDialog1 = this;

    wxFlexGridSizer* itemFlexGridSizer2 = new wxFlexGridSizer(0, 1, 0, 0);
    itemFlexGridSizer2->AddGrowableRow(1);
    itemDialog1->SetSizer(itemFlexGridSizer2);

    m_LocImageIndex = new wxBitmapButton( itemDialog1, ID_BITMAPBUTTON_SEL_LOC, wxNullBitmap, wxDefaultPosition, wxSize(300, 80), wxBU_AUTODRAW );
    itemFlexGridSizer2->Add(m_LocImageIndex, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_ListStrings;
    m_List = new wxListBox( itemDialog1, ID_LISTBOX_SEL_LOC, wxDefaultPosition, wxSize(-1, 200), m_ListStrings, wxLB_SINGLE|wxLB_ALWAYS_SB|wxLB_SORT );
    itemFlexGridSizer2->Add(m_List, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end LocSelDlg content construction
}

/*!
 * wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_SEL_LOC
 */

void LocSelDlg::OnListboxSelLocSelected( wxCommandEvent& event )
{
  /* jmf: whats this?
  if(!m_MICmode) {
    event.Skip();
    return;
  }
  */

  int sel = m_List->GetSelection();
  if( sel == wxNOT_FOUND )
    return;

  m_Props = wxGetApp().getFrame()->findLoc( m_List->GetStringSelection().mb_str(wxConvUTF8) );
  InitValues();

}

/*!
 * Should we show tooltips?
 */

bool LocSelDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap LocSelDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin LocSelDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end LocSelDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon LocSelDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin LocSelDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end LocSelDlg icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_SEL_LOC
 */

void LocSelDlg::OnBitmapbuttonSelLocClick( wxCommandEvent& event )
{
  if(!m_MICmode) {
    m_Props = wxGetApp().getFrame()->findLoc( m_List->GetStringSelection().mb_str(wxConvUTF8) );
    EndModal( wxID_OK );
    return;
  }

  int sel = m_List->GetSelection();
  if( sel == wxNOT_FOUND )
    return;

  m_Props = wxGetApp().getFrame()->findLoc( m_List->GetStringSelection().mb_str(wxConvUTF8) );

  if( m_Props != NULL ) {
    // Entering MIC mode!
    wxCursor cursor = wxCursor(wxCURSOR_BULLSEYE);
    SetCursor( cursor );
    CaptureMouse();
  }
}

void LocSelDlg::OnStop(wxMouseEvent& event) {
  if(!m_MICmode || m_Props == NULL) {
    event.Skip();
    return;
  }

  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, wLoc.getid( m_Props ) );
  wLoc.setV( cmd, 0 );
  wLoc.setfn( cmd, wLoc.isfn(m_Props) );
  wLoc.setdir( cmd, wLoc.isdir(m_Props) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}

void LocSelDlg::OnEscape(wxMouseEvent& event) {
  if(!m_MICmode || m_Props == NULL) {
    event.Skip();
    return;
  }
  wxCursor cursor = wxCursor(wxCURSOR_ARROW);
  SetCursor( cursor );
  ReleaseMouse();

  if( wMIC.isexitstop(m_MICini) ) {
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( cmd, wLoc.getid( m_Props ) );
    wLoc.setV( cmd, 0 );
    wLoc.setfn( cmd, wLoc.isfn(m_Props) );
    wLoc.setdir( cmd, wLoc.isdir(m_Props) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  m_Props = NULL;
}

void LocSelDlg::OnSpeed(wxMouseEvent& event) {
  if(!m_MICmode || m_Props == NULL) {
    event.Skip();
    return;
  }

  bool dirchange = event.MiddleDown();
  int rotation   = event.GetWheelRotation();
  int V_max      = wLoc.getV_max(m_Props);
  int V          = wLoc.getV(m_Props);

  if( dirchange ) {
    wLoc.setdir(m_Props, wLoc.isdir(m_Props)?False:True );
    wLoc.setV( m_Props, 0);
  }
  else if( rotation > 0 && V < V_max ) {
    wLoc.setV( m_Props, V+wMIC.getstep(m_MICini));
  }
  else if( rotation < 0 && V > 0) {
    wLoc.setV( m_Props, V-wMIC.getstep(m_MICini));
  }

  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  wLoc.setid( cmd, wLoc.getid( m_Props ) );
  wLoc.setV( cmd, wLoc.getV(m_Props) );
  wLoc.setfn( cmd, wLoc.isfn(m_Props) );
  wLoc.setdir( cmd, wLoc.isdir(m_Props) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}



/*!
 * wxEVT_COMMAND_LISTBOX_DOUBLECLICKED event handler for ID_LISTBOX_SEL_LOC
 */

void LocSelDlg::OnListboxSelLocDoubleClicked( wxCommandEvent& event )
{
  OnListboxSelLocSelected(event);
  wxCommandEvent l_Event( wxEVT_COMMAND_BUTTON_CLICKED, ID_BITMAPBUTTON_SEL_LOC );
  wxPostEvent( this, l_Event );
}

