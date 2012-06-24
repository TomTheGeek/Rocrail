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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "barjutdlg.h"
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

#include "barjutdlg.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocview/public/guiapp.h"

////@begin XPM images
////@end XPM images

/*!
 * BarJuTCntrlDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( BarJuTCntrlDlg, wxDialog )

/*!
 * BarJuTCntrlDlg event table definition
 */

BEGIN_EVENT_TABLE( BarJuTCntrlDlg, wxDialog )

////@begin BarJuTCntrlDlg event table entries
    EVT_BUTTON( wxID_OK, BarJuTCntrlDlg::OnOKClick )

    EVT_BUTTON( wxID_CANCEL, BarJuTCntrlDlg::OnCANCELClick )

////@end BarJuTCntrlDlg event table entries

END_EVENT_TABLE()

/*!
 * BarJuTCntrlDlg constructors
 */

BarJuTCntrlDlg::BarJuTCntrlDlg()
{
}

BarJuTCntrlDlg::BarJuTCntrlDlg( wxWindow* parent, iONode props )
{
  int isSPROGII = StrOp.equals( wDigInt.sprog, wDigInt.getlib(props) );
  int isDCC232  = StrOp.equals( wDigInt.dcc232, wDigInt.getlib(props) );
  Create(parent, -1, isSPROGII ? _T("SPROG II"):(isDCC232?_T("DCC232"):_T("BarJut")));
  m_Props = props;
  initLabels();
  initValues();

  m_labPolling->Enable(!isSPROGII && !isDCC232);
  m_Polling->Enable(!isSPROGII && !isDCC232);

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  GetSizer()->Layout();

}

void BarJuTCntrlDlg::initLabels() {
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "port" ) );
}

void BarJuTCntrlDlg::initValues() {
  if( m_Props == NULL )
    return;

  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );
  m_Device->SetValue( wxString( wDigInt.getdevice( m_Props ), wxConvUTF8 ) );

  char* val = StrOp.fmt( "%d", wDigInt.gettimeout( m_Props ) );
  m_Polling->SetValue( wxString( val, wxConvUTF8 ) );
  StrOp.free( val );
}


void BarJuTCntrlDlg::evaluate() {
  if( m_Props == NULL )
    return;
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setdevice( m_Props, m_Device->GetValue().mb_str(wxConvUTF8) );
  wDigInt.settimeout( m_Props, atoi( m_Polling->GetValue().mb_str(wxConvUTF8) ) );
}


/*!
 * BarJuTCntrlDlg creator
 */

bool BarJuTCntrlDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin BarJuTCntrlDlg member initialisation
    m_labIID = NULL;
    m_IID = NULL;
    m_labDevice = NULL;
    m_Device = NULL;
    m_labPolling = NULL;
    m_Polling = NULL;
////@end BarJuTCntrlDlg member initialisation

////@begin BarJuTCntrlDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end BarJuTCntrlDlg creation
    return true;
}

/*!
 * BarJuTCntrlDlg destructor
 */

void BarJuTCntrlDlg::CreateControls()
{
////@begin BarJuTCntrlDlg content construction
    BarJuTCntrlDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( itemDialog1, ID_PANEL_BARJUT, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel3, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    itemPanel3->SetSizer(itemBoxSizer4);

    wxFlexGridSizer* itemFlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer5->AddGrowableCol(1);
    itemBoxSizer4->Add(itemFlexGridSizer5, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    m_labIID = new wxStaticText( itemPanel3, ID_STATICTEXT_BARJUT_IID, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_IID = new wxTextCtrl( itemPanel3, ID_TEXTCTRL_BARJUT_IID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_IID, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDevice = new wxStaticText( itemPanel3, ID_STATICTEXT_BARJUT_DEVICE, _("Device"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labDevice, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Device = new wxTextCtrl( itemPanel3, ID_TEXTCTRL_BARJUT_DEVICE, wxEmptyString, wxDefaultPosition, wxSize(120, -1), 0 );
    itemFlexGridSizer5->Add(m_Device, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labPolling = new wxStaticText( itemPanel3, ID_STATICTEXT_BARJUT_POLLING, _("Pollingrate"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labPolling, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Polling = new wxTextCtrl( itemPanel3, ID_TEXTCTRL_BARJUT_POLLING, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_Polling, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer12 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer12, 0, wxALIGN_RIGHT|wxALL, 5);
    wxButton* itemButton13 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer12->AddButton(itemButton13);

    wxButton* itemButton14 = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer12->AddButton(itemButton14);

    itemStdDialogButtonSizer12->Realize();

////@end BarJuTCntrlDlg content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void BarJuTCntrlDlg::OnOKClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void BarJuTCntrlDlg::OnCANCELClick( wxCommandEvent& event )
{
  EndModal( 0 );
}
/*!
 * Should we show tooltips?
 */

bool BarJuTCntrlDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap BarJuTCntrlDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin BarJuTCntrlDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end BarJuTCntrlDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon BarJuTCntrlDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin BarJuTCntrlDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end BarJuTCntrlDlg icon retrieval
}



