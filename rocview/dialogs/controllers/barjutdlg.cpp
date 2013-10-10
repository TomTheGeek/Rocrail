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
#include "rocs/public/strtok.h"

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

BarJuTCntrlDlg::BarJuTCntrlDlg( wxWindow* parent, iONode props, const char* devices )
{
  int isSPROGII = StrOp.equals( wDigInt.sprog, wDigInt.getlib(props) );
  int isDCC232  = StrOp.equals( wDigInt.dcc232, wDigInt.getlib(props) );
  Create(parent, -1, isSPROGII ? _T("SPROG"):(isDCC232?_T("DCC232"):_T("BarJut")));
  m_Props = props;
  m_Devices = devices;
  initLabels();

  GetSizer()->Layout();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  initValues();
  m_Polling->Enable(!isSPROGII && !isDCC232);
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
  if( m_Devices != NULL ) {
    iOStrTok tok = StrTokOp.inst(m_Devices, ',');
    while( StrTokOp.hasMoreTokens(tok) ) {
      m_Device->Append( wxString( StrTokOp.nextToken(tok), wxConvUTF8 ) );
    }
    StrTokOp.base.del(tok);
  }

  m_Polling->SetValue( wDigInt.gettimeout( m_Props ) );
}


void BarJuTCntrlDlg::evaluate() {
  if( m_Props == NULL )
    return;
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setdevice( m_Props, m_Device->GetValue().mb_str(wxConvUTF8) );
  wDigInt.settimeout( m_Props, m_Polling->GetValue() );
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

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer2->Add(itemFlexGridSizer3, 0, wxGROW|wxALL, 5);

    m_labIID = new wxStaticText( itemDialog1, ID_STATICTEXT_BARJUT_IID, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_labIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_IID = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_BARJUT_IID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_IID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDevice = new wxStaticText( itemDialog1, ID_STATICTEXT_BARJUT_DEVICE, _("Device"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_labDevice, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxArrayString m_DeviceStrings;
    m_Device = new wxComboBox( itemDialog1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), m_DeviceStrings, wxCB_DROPDOWN );
    itemFlexGridSizer3->Add(m_Device, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labPolling = new wxStaticText( itemDialog1, ID_STATICTEXT_BARJUT_POLLING, _("Pollingrate"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_labPolling, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Polling = new wxSpinCtrl( itemDialog1, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 10000, 0 );
    itemFlexGridSizer3->Add(m_Polling, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer3->AddGrowableCol(1);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer10 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer10, 0, wxALIGN_RIGHT|wxALL, 5);
    wxButton* itemButton11 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer10->AddButton(itemButton11);

    wxButton* itemButton12 = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer10->AddButton(itemButton12);

    itemStdDialogButtonSizer10->Realize();

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



