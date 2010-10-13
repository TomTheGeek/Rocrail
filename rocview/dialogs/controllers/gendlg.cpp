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

GenericCtrlDlg::GenericCtrlDlg( wxWindow* parent, iONode props, const char* controllername )
{
  Create(parent, -1, wxGetApp().getMsg(controllername));
  m_Props = props;
  initLabels();
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
  m_FbPoll->SetLabel( wxGetApp().getMsg( "poll" ) );
  m_FbReset->SetLabel( wxGetApp().getMsg( "reset" ) );
  m_PTSupport->SetLabel( wxGetApp().getMsg( "pt" ) );
}

void GenericCtrlDlg::initValues() {
  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );
  m_Device->SetValue( wxString( wDigInt.getdevice( m_Props ), wxConvUTF8 ) );
  m_Lib->SetValue( wxString( wDigInt.getlib( m_Props ), wxConvUTF8 ) );

  char* val = StrOp.fmt( "%d", wDigInt.gettimeout( m_Props ) );
  m_Timeout->SetValue( wxString( val, wxConvUTF8 ) );
  StrOp.free( val );

  char* str = StrOp.fmt("%d",wDigInt.getfbmod( m_Props ));
  m_FbMod->SetValue( wxString( str, wxConvUTF8 ) ); StrOp.free(str);
  m_FbPoll->SetValue( wDigInt.isfbpoll( m_Props ) );
  m_FbReset->SetValue( wDigInt.isfbreset( m_Props ) );
  m_PTSupport->SetValue( wDigInt.isptsupport( m_Props ) );

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
  else
    m_Baudrate->SetSelection(2);



}

void GenericCtrlDlg::evaluate() {
  if( m_Props == NULL )
    return;
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setdevice( m_Props, m_Device->GetValue().mb_str(wxConvUTF8) );
  //wDigInt.setswtime( m_Props, atoi( m_SwTime->GetValue().mb_str(wxConvUTF8) ) );

  wDigInt.settimeout( m_Props, atoi( m_Timeout->GetValue().mb_str(wxConvUTF8) ) );


  wDigInt.setfbmod( m_Props, atoi(m_FbMod->GetValue().mb_str(wxConvUTF8)) );
  wDigInt.setfbpoll( m_Props, m_FbPoll->IsChecked()?True:False );
  wDigInt.setfbreset( m_Props, m_FbReset->IsChecked()?True:False );
  wDigInt.setptsupport( m_Props, m_PTSupport->IsChecked()?True:False );

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
    m_FbPoll = NULL;
    m_FbReset = NULL;
    m_PTSupport = NULL;
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

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    m_Panel->SetSizer(itemBoxSizer4);

    wxFlexGridSizer* itemFlexGridSizer5 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer5->AddGrowableCol(1);
    itemBoxSizer4->Add(itemFlexGridSizer5, 0, wxGROW|wxALL, 5);

    m_labIID = new wxStaticText( m_Panel, ID_STATICTEXT, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_IID = new wxTextCtrl( m_Panel, ID_TEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_IID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labDevice = new wxStaticText( m_Panel, ID_STATICTEXT1, _("Device"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labDevice, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Device = new wxTextCtrl( m_Panel, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_Device, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labLib = new wxStaticText( m_Panel, wxID_ANY, _("Library"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labLib, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Lib = new wxTextCtrl( m_Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    m_Lib->Enable(false);
    itemFlexGridSizer5->Add(m_Lib, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxBoxSizer* itemBoxSizer12 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer4->Add(itemBoxSizer12, 0, wxGROW|wxLEFT|wxRIGHT|wxTOP, 5);

    wxArrayString m_BaudrateStrings;
    m_BaudrateStrings.Add(_("&2400"));
    m_BaudrateStrings.Add(_("&4800"));
    m_BaudrateStrings.Add(_("&9600"));
    m_BaudrateStrings.Add(_("&19200"));
    m_BaudrateStrings.Add(_("&38400"));
    m_BaudrateStrings.Add(_("&57600"));
    m_BaudrateStrings.Add(_("&115200"));
    m_BaudrateStrings.Add(_("&230400"));
    m_Baudrate = new wxRadioBox( m_Panel, ID_RADIOBOX, _("Baudrate"), wxDefaultPosition, wxDefaultSize, m_BaudrateStrings, 1, wxRA_SPECIFY_COLS );
    m_Baudrate->SetSelection(0);
    itemBoxSizer12->Add(m_Baudrate, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_HardwareFlowStrings;
    m_HardwareFlowStrings.Add(_("&none"));
    m_HardwareFlowStrings.Add(_("&cts"));
    m_HardwareFlowStrings.Add(_("&dsr"));
    m_HardwareFlowStrings.Add(_("&xon"));
    m_HardwareFlow = new wxRadioBox( m_Panel, ID_RADIOBOX1, _("Hardware Flow"), wxDefaultPosition, wxDefaultSize, m_HardwareFlowStrings, 1, wxRA_SPECIFY_COLS );
    m_HardwareFlow->SetSelection(0);
    itemBoxSizer12->Add(m_HardwareFlow, 0, wxALIGN_TOP|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxFlexGridSizer* itemFlexGridSizer15 = new wxFlexGridSizer(0, 3, 0, 0);
    itemBoxSizer4->Add(itemFlexGridSizer15, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labTimeout = new wxStaticText( m_Panel, wxID_ANY, _("Timeout"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(m_labTimeout, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Timeout = new wxTextCtrl( m_Panel, wxID_ANY, _("1000"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer15->Add(m_Timeout, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labMS = new wxStaticText( m_Panel, wxID_ANY, _("mS"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(m_labMS, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_labFeedbackBox = new wxStaticBox(m_Panel, wxID_ANY, _("Feedback"));
    wxStaticBoxSizer* itemStaticBoxSizer19 = new wxStaticBoxSizer(m_labFeedbackBox, wxVERTICAL);
    itemBoxSizer4->Add(itemStaticBoxSizer19, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer20 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer19->Add(itemBoxSizer20, 0, wxGROW|wxLEFT|wxRIGHT, 5);

    m_labFbMod = new wxStaticText( m_Panel, wxID_STATIC, _("Number"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer20->Add(m_labFbMod, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_FbMod = new wxTextCtrl( m_Panel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemBoxSizer20->Add(m_FbMod, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer23 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer19->Add(itemBoxSizer23, 0, wxGROW|wxALL, 5);

    m_FbPoll = new wxCheckBox( m_Panel, wxID_ANY, _("Poll"), wxDefaultPosition, wxDefaultSize, 0 );
    m_FbPoll->SetValue(false);
    itemBoxSizer23->Add(m_FbPoll, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_FbReset = new wxCheckBox( m_Panel, wxID_ANY, _("Reset"), wxDefaultPosition, wxDefaultSize, 0 );
    m_FbReset->SetValue(false);
    itemBoxSizer23->Add(m_FbReset, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_PTSupport = new wxCheckBox( m_Panel, wxID_ANY, _("PT Support"), wxDefaultPosition, wxDefaultSize, 0 );
    m_PTSupport->SetValue(false);
    itemBoxSizer4->Add(m_PTSupport, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer27 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer27, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer27->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer27->AddButton(m_Cancel);

    itemStdDialogButtonSizer27->Realize();

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


