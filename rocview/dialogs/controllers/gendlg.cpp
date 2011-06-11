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
#pragma implementation "gendlg.h"
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

#include "gendlg.h"

////@begin XPM images
////@end XPM images

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/DigInt.h"

/*!
 * GenericCtrlDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( GenericCtrlDlg, wxDialog )

/*!
 * GenericCtrlDlg event table definition
 */

BEGIN_EVENT_TABLE( GenericCtrlDlg, wxDialog )

////@begin GenericCtrlDlg event table entries
    EVT_BUTTON( wxID_OK, GenericCtrlDlg::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, GenericCtrlDlg::OnCancelClick )

////@end GenericCtrlDlg event table entries

END_EVENT_TABLE()

/*!
 * GenericCtrlDlg constructors
 */

GenericCtrlDlg::GenericCtrlDlg( )
{
}

GenericCtrlDlg::GenericCtrlDlg( wxWindow* parent, iONode props, const char* controllername, int bps, const char* hs )
{
  Create(parent, -1, wxGetApp().getMsg(controllername));
  m_Props = props;
  initLabels();

  if( bps > 0 ) {
    wDigInt.setbps( m_Props, bps );
    m_Baudrate->Enable(false);
  }
  if( hs != NULL ) {
    wDigInt.setflow( m_Props, hs );
    m_HardwareFlow->Enable(false);
  }
  initValues();

  m_Panel->GetSizer()->Fit(this);
  m_Panel->GetSizer()->SetSizeHints(this);
  m_Panel->GetSizer()->Layout();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  GetSizer()->Layout();

}

void GenericCtrlDlg::initLabels() {
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labLib->SetLabel( wxGetApp().getMsg( "type" ) );
  m_labTimeout->SetLabel( wxGetApp().getMsg( "timeout" ) );
  m_Baudrate->SetLabel( wxGetApp().getMsg( "bps" ) );
  m_labFeedbackBox->SetLabel( wxGetApp().getMsg( "sensors" ) );
  m_labFbMod->SetLabel( wxGetApp().getMsg( "number" ) );
  m_labFbOffset->SetLabel( wxGetApp().getMsg( "offset" ) );
  m_FbPoll->SetLabel( wxGetApp().getMsg( "poll" ) );
  m_FbReset->SetLabel( wxGetApp().getMsg( "reset" ) );
  m_OptionsBox->SetLabel( wxGetApp().getMsg( "options" ) );
  m_PTSupport->SetLabel( wxGetApp().getMsg( "pt" ) );
  m_SystemInfo->SetLabel( wxGetApp().getMsg( "systeminfo" ) );
}

void GenericCtrlDlg::initValues() {
  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );
  m_Device->SetValue( wxString( wDigInt.getdevice( m_Props ), wxConvUTF8 ) );
  m_Lib->SetValue( wxString( wDigInt.getlib( m_Props ), wxConvUTF8 ) );

  m_Timeout->SetValue( wDigInt.gettimeout( m_Props ) );

  m_FbMod->SetValue( wDigInt.getfbmod( m_Props ) );
  m_FbOffset->SetValue( wDigInt.getfboffset( m_Props ) );
  m_FbPoll->SetValue( wDigInt.isfbpoll( m_Props ) );
  m_FbReset->SetValue( wDigInt.isfbreset( m_Props ) );
  m_PTSupport->SetValue( wDigInt.isptsupport( m_Props ) );
  m_SystemInfo->SetValue( wDigInt.issysteminfo( m_Props ) );

  // flow control
  {
    const char* flow = wDigInt.getflow( m_Props );
    m_HardwareFlow->SetSelection(0);

    if( StrOp.equals( flow, wDigInt.cts ) )
      m_HardwareFlow->SetSelection(1);
    else if( StrOp.equals( flow, wDigInt.dsr ) )
      m_HardwareFlow->SetSelection(2);
    else if( StrOp.equals( flow, wDigInt.xon ) )
      m_HardwareFlow->SetSelection(3);
  }

  if( wDigInt.getbps( m_Props ) == 2400 )
    m_Baudrate->SetSelection(0);
  else if( wDigInt.getbps( m_Props ) == 4800 )
    m_Baudrate->SetSelection(1);
  else if( wDigInt.getbps( m_Props ) == 9600 )
    m_Baudrate->SetSelection(2);
  else if( wDigInt.getbps( m_Props ) == 19200 )
    m_Baudrate->SetSelection(3);
  else if( wDigInt.getbps( m_Props ) == 38400 )
    m_Baudrate->SetSelection(4);
  else if( wDigInt.getbps( m_Props ) == 57600 )
    m_Baudrate->SetSelection(5);
  else if( wDigInt.getbps( m_Props ) == 115200 )
    m_Baudrate->SetSelection(6);
  else if( wDigInt.getbps( m_Props ) == 230400 )
    m_Baudrate->SetSelection(7);
  else if( wDigInt.getbps( m_Props ) == 500000 )
    m_Baudrate->SetSelection(8);
  else if( wDigInt.getbps( m_Props ) == 1000000 )
    m_Baudrate->SetSelection(9);
  else
    m_Baudrate->SetSelection(2);



}

void GenericCtrlDlg::evaluate() {
  if( m_Props == NULL )
    return;
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setdevice( m_Props, m_Device->GetValue().mb_str(wxConvUTF8) );
  //wDigInt.setswtime( m_Props, atoi( m_SwTime->GetValue().mb_str(wxConvUTF8) ) );

  wDigInt.settimeout( m_Props, m_Timeout->GetValue() );


  wDigInt.setfbmod( m_Props, m_FbMod->GetValue() );
  wDigInt.setfboffset( m_Props, m_FbOffset->GetValue() );
  wDigInt.setfbpoll( m_Props, m_FbPoll->IsChecked()?True:False );
  wDigInt.setfbreset( m_Props, m_FbReset->IsChecked()?True:False );
  wDigInt.setptsupport( m_Props, m_PTSupport->IsChecked()?True:False );
  wDigInt.setsysteminfo( m_Props, m_SystemInfo->IsChecked()?True:False );

  // flow control
  {
    if( m_HardwareFlow->GetSelection() == 0 )
      wDigInt.setflow( m_Props, wDigInt.none );
    else if( m_HardwareFlow->GetSelection() == 1 )
      wDigInt.setflow( m_Props, wDigInt.cts );
    else if( m_HardwareFlow->GetSelection() == 2 )
      wDigInt.setflow( m_Props, wDigInt.dsr );
    else if( m_HardwareFlow->GetSelection() == 3 )
      wDigInt.setflow( m_Props, wDigInt.xon );

  }

  if( m_Baudrate->GetSelection() == 0 )
    wDigInt.setbps( m_Props, 2400 );
  else if( m_Baudrate->GetSelection() == 1 )
    wDigInt.setbps( m_Props, 4800 );
  else if( m_Baudrate->GetSelection() == 2 )
    wDigInt.setbps( m_Props, 9600 );
  else if( m_Baudrate->GetSelection() == 3 )
    wDigInt.setbps( m_Props, 19200 );
  else if( m_Baudrate->GetSelection() == 4 )
    wDigInt.setbps( m_Props, 38400 );
  else if( m_Baudrate->GetSelection() == 5 )
    wDigInt.setbps( m_Props, 57600 );
  else if( m_Baudrate->GetSelection() == 6 )
    wDigInt.setbps( m_Props, 115200 );
  else if( m_Baudrate->GetSelection() == 7 )
    wDigInt.setbps( m_Props, 230400 );
  else if( m_Baudrate->GetSelection() == 8 )
    wDigInt.setbps( m_Props, 500000 );
  else if( m_Baudrate->GetSelection() == 9 )
    wDigInt.setbps( m_Props, 1000000 );
}


/*!
 * Dialog creator
 */

bool GenericCtrlDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin GenericCtrlDlg member initialisation
    m_Panel = NULL;
    m_labIID = NULL;
    m_IID = NULL;
    m_labDevice = NULL;
    m_Device = NULL;
    m_labLib = NULL;
    m_Lib = NULL;
    m_Baudrate = NULL;
    m_HardwareFlow = NULL;
    m_labTimeout = NULL;
    m_Timeout = NULL;
    m_labMS = NULL;
    m_labFeedbackBox = NULL;
    m_labFbMod = NULL;
    m_FbMod = NULL;
    m_labFbOffset = NULL;
    m_FbOffset = NULL;
    m_FbPoll = NULL;
    m_FbReset = NULL;
    m_OptionsBox = NULL;
    m_PTSupport = NULL;
    m_SystemInfo = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end GenericCtrlDlg member initialisation

////@begin GenericCtrlDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end GenericCtrlDlg creation
    return true;
}

/*!
 * Control creation for Dialog
 */

void GenericCtrlDlg::CreateControls()
{
////@begin GenericCtrlDlg content construction
    GenericCtrlDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Panel = new wxPanel( itemDialog1, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(m_Panel, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    m_Panel->SetSizer(itemBoxSizer4);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer5, 1, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer6, 0, wxGROW|wxALL, 5);

    m_labIID = new wxStaticText( m_Panel, ID_STATICTEXT, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_IID = new wxTextCtrl( m_Panel, ID_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_IID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDevice = new wxStaticText( m_Panel, ID_STATICTEXT1, _("Device"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labDevice, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Device = new wxTextCtrl( m_Panel, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_Device, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labLib = new wxStaticText( m_Panel, wxID_ANY, _("Library"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labLib, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Lib = new wxTextCtrl( m_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    m_Lib->Enable(false);
    itemFlexGridSizer6->Add(m_Lib, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    itemFlexGridSizer6->AddGrowableCol(1);

    wxBoxSizer* itemBoxSizer13 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer13, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_BaudrateStrings;
    m_BaudrateStrings.Add(_("&2400"));
    m_BaudrateStrings.Add(_("&4800"));
    m_BaudrateStrings.Add(_("&9600"));
    m_BaudrateStrings.Add(_("&19200"));
    m_BaudrateStrings.Add(_("&38400"));
    m_BaudrateStrings.Add(_("&57600"));
    m_BaudrateStrings.Add(_("&115200"));
    m_BaudrateStrings.Add(_("&230400"));
    m_BaudrateStrings.Add(_("&500000"));
    m_BaudrateStrings.Add(_("&1000000"));
    m_Baudrate = new wxRadioBox( m_Panel, ID_RADIOBOX, _("Baudrate"), wxDefaultPosition, wxDefaultSize, m_BaudrateStrings, 1, wxRA_SPECIFY_COLS );
    m_Baudrate->SetSelection(0);
    itemBoxSizer13->Add(m_Baudrate, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_HardwareFlowStrings;
    m_HardwareFlowStrings.Add(_("&none"));
    m_HardwareFlowStrings.Add(_("&cts"));
    m_HardwareFlowStrings.Add(_("&dsr"));
    m_HardwareFlowStrings.Add(_("&xon"));
    m_HardwareFlow = new wxRadioBox( m_Panel, ID_RADIOBOX1, _("Hardware Flow"), wxDefaultPosition, wxDefaultSize, m_HardwareFlowStrings, 1, wxRA_SPECIFY_COLS );
    m_HardwareFlow->SetSelection(0);
    itemBoxSizer13->Add(m_HardwareFlow, 0, wxALIGN_TOP|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxFlexGridSizer* itemFlexGridSizer16 = new wxFlexGridSizer(0, 3, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer16, 0, wxGROW|wxALL, 5);

    m_labTimeout = new wxStaticText( m_Panel, wxID_ANY, _("Timeout"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer16->Add(m_labTimeout, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Timeout = new wxSpinCtrl( m_Panel, wxID_ANY, _T("1000"), wxDefaultPosition, wxSize(120, -1), wxSP_ARROW_KEYS, 0, 10000, 1000 );
    itemFlexGridSizer16->Add(m_Timeout, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labMS = new wxStaticText( m_Panel, wxID_ANY, _("mS"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer16->Add(m_labMS, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    wxStaticLine* itemStaticLine20 = new wxStaticLine( m_Panel, wxID_STATIC, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
    itemBoxSizer4->Add(itemStaticLine20, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer21 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer21, 1, wxGROW|wxALL, 5);

    m_labFeedbackBox = new wxStaticBox(m_Panel, wxID_ANY, _("Feedback"));
    wxStaticBoxSizer* itemStaticBoxSizer22 = new wxStaticBoxSizer(m_labFeedbackBox, wxVERTICAL);
    itemBoxSizer21->Add(itemStaticBoxSizer22, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer23 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer22->Add(itemFlexGridSizer23, 0, wxGROW, 5);

    m_labFbMod = new wxStaticText( m_Panel, wxID_STATIC, _("Number"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_labFbMod, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_FbMod = new wxSpinCtrl( m_Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer23->Add(m_FbMod, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labFbOffset = new wxStaticText( m_Panel, wxID_ANY, _("Offset"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_labFbOffset, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_FbOffset = new wxSpinCtrl( m_Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer23->Add(m_FbOffset, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_FbPoll = new wxCheckBox( m_Panel, wxID_ANY, _("Poll"), wxDefaultPosition, wxDefaultSize, 0 );
    m_FbPoll->SetValue(false);
    itemStaticBoxSizer22->Add(m_FbPoll, 0, wxALIGN_LEFT|wxALL, 5);

    m_FbReset = new wxCheckBox( m_Panel, wxID_ANY, _("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
    m_FbReset->SetValue(false);
    itemStaticBoxSizer22->Add(m_FbReset, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_OptionsBox = new wxStaticBox(m_Panel, wxID_ANY, _("Options"));
    wxStaticBoxSizer* itemStaticBoxSizer30 = new wxStaticBoxSizer(m_OptionsBox, wxVERTICAL);
    itemBoxSizer21->Add(itemStaticBoxSizer30, 0, wxGROW|wxALL, 5);

    m_PTSupport = new wxCheckBox( m_Panel, wxID_ANY, _("PT Support"), wxDefaultPosition, wxDefaultSize, 0 );
    m_PTSupport->SetValue(false);
    itemStaticBoxSizer30->Add(m_PTSupport, 0, wxALIGN_LEFT|wxALL, 5);

    m_SystemInfo = new wxCheckBox( m_Panel, wxID_ANY, _("System info"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SystemInfo->SetValue(false);
    itemStaticBoxSizer30->Add(m_SystemInfo, 0, wxALIGN_LEFT|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer33 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer33, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer33->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer33->AddButton(m_Cancel);

    itemStdDialogButtonSizer33->Realize();

////@end GenericCtrlDlg content construction
}

/*!
 * Should we show tooltips?
 */

bool GenericCtrlDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap GenericCtrlDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin GenericCtrlDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end GenericCtrlDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon GenericCtrlDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin GenericCtrlDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end GenericCtrlDlg icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void GenericCtrlDlg::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void GenericCtrlDlg::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


