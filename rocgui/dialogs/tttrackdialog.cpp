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
#pragma implementation "tttrackdialog.h"
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

#include "tttrackdialog.h"

#include "rocs/public/str.h"
#include "rocrail/wrapper/public/TTTrack.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocgui/public/guiapp.h"

////@begin XPM images
////@end XPM images

/*!
 * TTTrackDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( TTTrackDialog, wxDialog )

/*!
 * TTTrackDialog event table definition
 */

BEGIN_EVENT_TABLE( TTTrackDialog, wxDialog )

////@begin TTTrackDialog event table entries
    EVT_BUTTON( wxID_OK, TTTrackDialog::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, TTTrackDialog::OnCancelClick )

////@end TTTrackDialog event table entries

END_EVENT_TABLE()

/*!
 * TTTrackDialog constructors
 */

TTTrackDialog::TTTrackDialog( )
{
}


TTTrackDialog::TTTrackDialog( wxWindow* parent, iONode p_Props )
{
  Create(parent, -1, wxGetApp().getMsg("turntabletrack") );
  m_Props = p_Props;
  initLabels();
  initValues();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
}

TTTrackDialog::TTTrackDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
  Create(parent, id, caption, pos, size, style);
}


void TTTrackDialog::initLabels() {
  m_LabelTrackNr->SetLabel( wxGetApp().getMsg( "tracknr" ) );
  m_labDecoder->SetLabel( wxGetApp().getMsg("decoder") + _T(" ") + wxGetApp().getMsg("tracknr") );
  m_LabelDesc->SetLabel( wxGetApp().getMsg( "description" ) );
  m_LabelPosFB->SetLabel( wxGetApp().getMsg( "posfb" ) );

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
}


void TTTrackDialog::initValues() {
  if( m_Props == NULL )
    return;
  // General
  m_TrackNr->SetValue( wTTTrack.getnr( m_Props ) );
  m_DecTrackNr->SetValue( wTTTrack.getdecnr( m_Props ) );
  m_Desc->SetValue( wxString( wTTTrack.getdesc( m_Props ),wxConvUTF8) );

  iONode model = wxGetApp().getModel();

  if( model != NULL ) {
    m_PosFB->Append( _T("") );
    iONode fblist = wPlan.getfblist( model );
    if( fblist != NULL ) {
      int cnt = NodeOp.getChildCnt( fblist );
      for( int i = 0; i < cnt; i++ ) {
        iONode fb = NodeOp.getChild( fblist, i );
        const char* id = wFeedback.getid( fb );
        m_PosFB->Append( wxString(id,wxConvUTF8) );
      }
      m_PosFB->SetStringSelection( wxString(wTTTrack.getposfb(m_Props),wxConvUTF8) );

    }
  }

}

void TTTrackDialog::evaluate() {
  if( m_Props == NULL )
    return;
  // General
  wTTTrack.setnr( m_Props, m_TrackNr->GetValue() );
  wTTTrack.setdecnr( m_Props, m_DecTrackNr->GetValue() );
  wTTTrack.setdesc( m_Props, m_Desc->GetValue().mb_str(wxConvUTF8) );
  wTTTrack.setposfb( m_Props, m_PosFB->GetStringSelection().mb_str(wxConvUTF8) );
}

/*!
 * TTTrackDialog creator
 */

bool TTTrackDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin TTTrackDialog member initialisation
    m_Props = NULL;
    m_LabelTrackNr = NULL;
    m_TrackNr = NULL;
    m_labDecoder = NULL;
    m_DecTrackNr = NULL;
    m_LabelDesc = NULL;
    m_Desc = NULL;
    m_LabelPosFB = NULL;
    m_PosFB = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end TTTrackDialog member initialisation

////@begin TTTrackDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end TTTrackDialog creation
    return true;
}

/*!
 * Control creation for TTTrackDialog
 */

void TTTrackDialog::CreateControls()
{
////@begin TTTrackDialog content construction
    TTTrackDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer3->AddGrowableCol(1);
    itemBoxSizer2->Add(itemFlexGridSizer3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_LabelTrackNr = new wxStaticText( itemDialog1, wxID_STATIC_TTT_NR, _("Track Nr."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_LabelTrackNr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_TrackNr = new wxSpinCtrl( itemDialog1, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 47, 0 );
    itemFlexGridSizer3->Add(m_TrackNr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDecoder = new wxStaticText( itemDialog1, wxID_STATIC, _("Decoder Nr."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_labDecoder, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_DecTrackNr = new wxSpinCtrl( itemDialog1, ID_SPINCTRL, _T("-1"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, -1, 47, -1 );
    itemFlexGridSizer3->Add(m_DecTrackNr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelDesc = new wxStaticText( itemDialog1, wxID_STATIC_TTT_DESC, _("Description"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_LabelDesc, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Desc = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_TTT_DESC, _T(""), wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer3->Add(m_Desc, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelPosFB = new wxStaticText( itemDialog1, wxID_STATIC_TTT_POSFB, _("PosFB"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_LabelPosFB, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString m_PosFBStrings;
    m_PosFB = new wxComboBox( itemDialog1, ID_COMBOBOX_TTT_POSFB, _T(""), wxDefaultPosition, wxDefaultSize, m_PosFBStrings, wxCB_DROPDOWN );
    itemFlexGridSizer3->Add(m_PosFB, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer12 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer12, 0, wxGROW|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer12->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer12->AddButton(m_Cancel);

    itemStdDialogButtonSizer12->Realize();

////@end TTTrackDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool TTTrackDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap TTTrackDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin TTTrackDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end TTTrackDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon TTTrackDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin TTTrackDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end TTTrackDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void TTTrackDialog::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void TTTrackDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


