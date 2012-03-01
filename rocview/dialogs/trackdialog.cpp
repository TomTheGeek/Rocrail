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
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "trackdialog.cpp"
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
#include "wx/imaglist.h"
////@end includes

#include "trackdialog.h"

#include "rocrail/wrapper/public/Track.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocview/public/guiapp.h"

////@begin XPM images
////@end XPM images

/*!
 * TrackDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( TrackDialog, wxDialog )

/*!
 * TrackDialog event table definition
 */

BEGIN_EVENT_TABLE( TrackDialog, wxDialog )

////@begin TrackDialog event table entries
    EVT_BUTTON( wxID_OK, TrackDialog::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, TrackDialog::OnCancelClick )

////@end TrackDialog event table entries

END_EVENT_TABLE()

/*!
 * TrackDialog constructors
 */

TrackDialog::TrackDialog( )
{
}

TrackDialog::TrackDialog( wxWindow* parent, iONode l_Props )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxGetApp().getMsg("track") );
  m_Props = l_Props;
  initLabels();
  initValues();

  m_GeneralPanel->GetSizer()->Layout();
  m_LocationPanel->GetSizer()->Layout();
  m_Notebook->Fit();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
}

/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}



void TrackDialog::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "position" ) );

  // General
  m_LabelID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_Road->SetLabel( wxGetApp().getMsg( "road" ) );
  m_LabelType->SetLabel( wxGetApp().getMsg( "type" ) );
  m_LabelBlockID->SetLabel( wxGetApp().getMsg( "blockid" ) );
  m_labRouteIDs->SetLabel( wxGetApp().getMsg( "routeids" ) );
  m_labTrackNr->SetLabel( wxGetApp().getMsg( "number" ) );

  m_BlockID->Append( _T("") );
  iONode model = wxGetApp().getModel();
  iOList list = ListOp.inst();

  if( model != NULL ) {
    iONode bklist = wPlan.getbklist( model );
    if( bklist != NULL ) {
      int cnt = NodeOp.getChildCnt( bklist );
      for( int i = 0; i < cnt; i++ ) {
        iONode bk = NodeOp.getChild( bklist, i );
        ListOp.add(list, (obj)wBlock.getid( bk ));
      }
    }
    iONode fblist = wPlan.getfblist( model );
    if( fblist != NULL ) {
      int cnt = NodeOp.getChildCnt( fblist );
      for( int i = 0; i < cnt; i++ ) {
        iONode fb = NodeOp.getChild( fblist, i );
        ListOp.add(list, (obj)wFeedback.getid( fb ));
      }
    }

    ListOp.sort(list, &__sortStr);
    int cnt = ListOp.size( list );
    for( int i = 0; i < cnt; i++ ) {
      const char* id = (const char*)ListOp.get( list, i );
      m_BlockID->Append( wxString(id,wxConvUTF8) );
    }

  }

  ListOp.base.del(list);

  m_Type->Append( wxString(wTrack.straight,wxConvUTF8) );
  m_Type->Append( wxString(wTrack.curve,wxConvUTF8) );
  m_Type->Append( wxString(wTrack.dir,wxConvUTF8) );
  m_Type->Append( wxString(wTrack.dirall,wxConvUTF8) );
  m_Type->Append( wxString(wTrack.buffer,wxConvUTF8) );
  m_Type->Append( wxString(wTrack.connector,wxConvUTF8) );
  m_Type->Append( wxString(wTrack.tracknr,wxConvUTF8) );

  // Location
  m_LabelX->SetLabel( wxGetApp().getMsg( "x" ) );
  m_LabelY->SetLabel( wxGetApp().getMsg( "y" ) );
  m_LabelZ->SetLabel( wxGetApp().getMsg( "z" ) );
  m_ori->SetLabel( wxGetApp().getMsg( "orientation" ) );
  m_ori->SetString( 0, wxGetApp().getMsg( "north" ) );
  m_ori->SetString( 1, wxGetApp().getMsg( "east" ) );
  m_ori->SetString( 2, wxGetApp().getMsg( "south" ) );
  m_ori->SetString( 3, wxGetApp().getMsg( "west" ) );

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
}


void TrackDialog::initValues() {
  // General
  m_ID->SetValue( wxString(wTrack.getid( m_Props ),wxConvUTF8) );
  m_Type->SetStringSelection( wxString(wTrack.gettype( m_Props ),wxConvUTF8) );
  m_Road->SetValue( wItem.isroad( m_Props )?true:false );
  m_BlockID->SetStringSelection( wTrack.getblockid( m_Props ) == NULL ?
                                _T(""):wxString(wTrack.getblockid( m_Props ),wxConvUTF8)  );
  m_RouteIDs->SetValue( wxString(wItem.getrouteids( m_Props ),wxConvUTF8) );
  m_TrackNr->SetValue(wTrack.gettknr( m_Props ));

  // Location
  char* str = StrOp.fmt( "%d", wTrack.getx( m_Props ) );
  m_x->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wTrack.gety( m_Props ) );
  m_y->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  str = StrOp.fmt( "%d", wTrack.getz( m_Props ) );
  m_z->SetValue( wxString(str,wxConvUTF8) ); StrOp.free( str );
  if( StrOp.equals( wItem.north, wTrack.getori( m_Props ) ) )
    m_ori->SetSelection( 0 );
  else if( StrOp.equals( wItem.east, wTrack.getori( m_Props ) ) )
    m_ori->SetSelection( 1 );
  else if( StrOp.equals( wItem.south, wTrack.getori( m_Props ) ) )
    m_ori->SetSelection( 2 );
  else
    m_ori->SetSelection( 3 );

}


bool TrackDialog::evaluate() {
  if( m_Props == NULL )
    return false;

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wTrack.getid( m_Props ),wxConvUTF8) );
    return false;
  }

  // General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wTrack.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wTrack.setblockid( m_Props, m_BlockID->GetStringSelection().mb_str(wxConvUTF8) );
  wTrack.settype( m_Props, m_Type->GetStringSelection().mb_str(wxConvUTF8) );
  wItem.setroad( m_Props, m_Road->IsChecked()?True:False );
  wItem.setrouteids( m_Props, m_RouteIDs->GetValue().mb_str(wxConvUTF8) );
  wTrack.settknr( m_Props, m_TrackNr->GetValue());

  // Location
  wTrack.setx( m_Props, atoi( m_x->GetValue().mb_str(wxConvUTF8) ) );
  wTrack.sety( m_Props, atoi( m_y->GetValue().mb_str(wxConvUTF8) ) );
  wTrack.setz( m_Props, atoi( m_z->GetValue().mb_str(wxConvUTF8) ) );
  int ori = m_ori->GetSelection();
  if( ori == 0 )
    wTrack.setori( m_Props, wItem.north );
  else if( ori == 1 )
    wTrack.setori( m_Props, wItem.east );
  else if( ori == 2 )
    wTrack.setori( m_Props, wItem.south );
  else if( ori == 3 )
    wTrack.setori( m_Props, wItem.west );

  return true;
}


/*!
 * MyDialog creator
 */

bool TrackDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin TrackDialog member initialisation
    m_Props = NULL;
    m_Notebook = NULL;
    m_GeneralPanel = NULL;
    m_LabelID = NULL;
    m_ID = NULL;
    m_LabelBlockID = NULL;
    m_BlockID = NULL;
    m_labRouteIDs = NULL;
    m_RouteIDs = NULL;
    m_LabelType = NULL;
    m_Type = NULL;
    m_labTrackNr = NULL;
    m_TrackNr = NULL;
    m_Road = NULL;
    m_LocationPanel = NULL;
    m_LabelX = NULL;
    m_x = NULL;
    m_LabelY = NULL;
    m_y = NULL;
    m_LabelZ = NULL;
    m_z = NULL;
    m_ori = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end TrackDialog member initialisation

////@begin TrackDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end TrackDialog creation
    return true;
}

/*!
 * Control creation for MyDialog
 */

void TrackDialog::CreateControls()
{
////@begin TrackDialog content construction
    TrackDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, ID_NOTEBOOK_TRACK, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_GeneralPanel = new wxPanel( m_Notebook, ID_PANEL_TK_GENERAL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_GeneralPanel->SetSizer(itemBoxSizer5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer6->AddGrowableCol(1);
    itemBoxSizer5->Add(itemFlexGridSizer6, 0, wxGROW|wxALL, 5);
    m_LabelID = new wxStaticText( m_GeneralPanel, wxID_STATIC_TK_ID, _("id"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_LabelID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ID = new wxTextCtrl( m_GeneralPanel, ID_TEXTCTRL_TK_ID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_ID, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelBlockID = new wxStaticText( m_GeneralPanel, wxID_STATIC_TRACK_BLOCKID, _("BlockID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_LabelBlockID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_BlockIDStrings;
    m_BlockID = new wxComboBox( m_GeneralPanel, ID_COMBOBOX_TRACK_BLOCKID, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_BlockIDStrings, wxCB_READONLY );
    itemFlexGridSizer6->Add(m_BlockID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labRouteIDs = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Route IDs"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labRouteIDs, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RouteIDs = new wxTextCtrl( m_GeneralPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_RouteIDs, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelType = new wxStaticText( m_GeneralPanel, wxID_STATIC_TK_TYPE, _("type"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_LabelType, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_TypeStrings;
    m_Type = new wxComboBox( m_GeneralPanel, ID_COMBOBOX_TK_TYPE, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_TypeStrings, wxCB_READONLY );
    itemFlexGridSizer6->Add(m_Type, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labTrackNr = new wxStaticText( m_GeneralPanel, wxID_ANY, _("number"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labTrackNr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_TrackNr = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 99, 0 );
    itemFlexGridSizer6->Add(m_TrackNr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer17 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer17, 0, wxGROW|wxALL, 5);
    m_Road = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Road"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Road->SetValue(false);
    itemFlexGridSizer17->Add(m_Road, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_GeneralPanel, _("General"));

    m_LocationPanel = new wxPanel( m_Notebook, ID_PANEL_TK_LOCATION, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    m_LocationPanel->SetSizer(itemBoxSizer20);

    wxFlexGridSizer* itemFlexGridSizer21 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer20->Add(itemFlexGridSizer21, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    m_LabelX = new wxStaticText( m_LocationPanel, wxID_STATIC_TK_X, _("x"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer21->Add(m_LabelX, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_x = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TK_X, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer21->Add(m_x, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelY = new wxStaticText( m_LocationPanel, wxID_STATIC_TK_Y, _("y"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer21->Add(m_LabelY, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_y = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TK_Y, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer21->Add(m_y, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelZ = new wxStaticText( m_LocationPanel, wxID_STATIC_TK_Z, _("z"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer21->Add(m_LabelZ, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_z = new wxTextCtrl( m_LocationPanel, ID_TEXTCTRL_TK_Z, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer21->Add(m_z, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_oriStrings;
    m_oriStrings.Add(_("&north"));
    m_oriStrings.Add(_("&east"));
    m_oriStrings.Add(_("&south"));
    m_oriStrings.Add(_("&west"));
    m_ori = new wxRadioBox( m_LocationPanel, ID_RADIOBOX_TK_ORI, _("Orientation"), wxDefaultPosition, wxDefaultSize, m_oriStrings, 1, wxRA_SPECIFY_COLS );
    m_ori->SetSelection(0);
    itemBoxSizer20->Add(m_ori, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_LocationPanel, _("Location"));

    itemBoxSizer2->Add(m_Notebook, 0, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer29 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer29, 0, wxGROW|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer29->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer29->AddButton(m_Cancel);

    itemStdDialogButtonSizer29->Realize();

////@end TrackDialog content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void TrackDialog::OnOkClick( wxCommandEvent& event )
{
  if( !m_Props )
    return;
  if( !evaluate() )
    return;
  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  else {
    wxGetApp().setLocalModelModified(true);
  }
 
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void TrackDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}

/*!
 * Should we show tooltips?
 */

bool TrackDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap TrackDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin TrackDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end TrackDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon TrackDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin TrackDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end TrackDialog icon retrieval
}
