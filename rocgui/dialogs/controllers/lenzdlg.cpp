/////////////////////////////////////////////////////////////////////////////
// Name:        lenzdlg.cpp
// Purpose:
// Author:
// Modified by:
// Created:     Tue 06 Jan 2009 04:49:35 PM CET
// RCS-ID:
// Copyright:
// Licence:
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "lenzdlg.h"
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

#include "lenzdlg.h"

////@begin XPM images
////@end XPM images

#include "rocgui/public/guiapp.h"
#include "rocrail/wrapper/public/DigInt.h"

/*!
 * LenzDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( LenzDlg, wxDialog )


/*!
 * LenzDlg event table definition
 */

BEGIN_EVENT_TABLE( LenzDlg, wxDialog )

////@begin LenzDlg event table entries
    EVT_BUTTON( wxID_OK, LenzDlg::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, LenzDlg::OnCancelClick )

////@end LenzDlg event table entries

END_EVENT_TABLE()


/*!
 * LenzDlg constructors
 */

LenzDlg::LenzDlg()
{
    Init();
}

LenzDlg::LenzDlg( wxWindow* parent, iONode props )
{
    Init();
    Create(parent, -1, _T("Lenz"));

    m_Props = props;
    initLabels();
    initValues();

    GetSizer()->Fit(this);
    GetSizer()->SetSizeHints(this);

    GetSizer()->Layout();
}

void LenzDlg::initLabels() {
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "port" ) );
  m_Type->SetLabel( wxGetApp().getMsg( "type" ) );
  m_BPS->SetLabel( wxGetApp().getMsg( "bps" ) );
  m_labSensor->SetLabel( wxGetApp().getMsg( "sensors" ) );
  m_labSensorOffset->SetLabel( wxGetApp().getMsg( "offset" ) );
  m_labSensorDebounce->SetLabel( wxGetApp().getMsg( "debounce" ) );
  m_labSwitch->SetLabel( wxGetApp().getMsg( "switches" ) );
  m_labSwitchTime->SetLabel( wxGetApp().getMsg( "switchtime" ) );
  m_labPower->SetLabel( wxGetApp().getMsg( "power" ) );
  m_PowerAtStartup->SetLabel( wxGetApp().getMsg( "startup" ) );
}

void LenzDlg::initValues() {
  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );
  m_Device->SetValue( wxString( wDigInt.getdevice( m_Props ), wxConvUTF8 ) );

  if( StrOp.equals( wDigInt.sublib_usb, wDigInt.getsublib(m_Props) ))
    m_Type->SetSelection(2);
  else if( StrOp.equals( wDigInt.sublib_lenz_elite, wDigInt.getsublib(m_Props) ))
    m_Type->SetSelection(1);
  else
    m_Type->SetSelection(0);

  m_SensorOffset->SetValue( wDigInt.getfboffset(m_Props));
  m_SensorDebounce->SetValue( wDigInt.getsensordebounce(m_Props));
  m_SwitchTime->SetValue( wDigInt.getswtime(m_Props));
  m_PowerAtStartup->SetValue( wDigInt.isstartpwstate(m_Props)?true:false);

  if( wDigInt.getbps( m_Props ) == 9600 )
    m_BPS->SetSelection(0);
  else if( wDigInt.getbps( m_Props ) == 19200 )
    m_BPS->SetSelection(1);
  else if( wDigInt.getbps( m_Props ) == 38400 )
    m_BPS->SetSelection(2);
  else if( wDigInt.getbps( m_Props ) == 57600 )
    m_BPS->SetSelection(3);
  else
    m_BPS->SetSelection(1);



}

void LenzDlg::evaluate() {
  if( m_Props == NULL )
    return;
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setdevice( m_Props, m_Device->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setswtime( m_Props, m_SwitchTime->GetValue() );
  wDigInt.setfboffset( m_Props, m_SensorOffset->GetValue() );
  wDigInt.setsensordebounce( m_Props, m_SensorDebounce->GetValue() );
  wDigInt.setstartpwstate(m_Props, m_PowerAtStartup->IsChecked()?True:False);

  if( m_Type->GetSelection() == 2 )
    wDigInt.setsublib(m_Props, wDigInt.sublib_usb );
  else if( m_Type->GetSelection() == 1 )
    wDigInt.setsublib(m_Props, wDigInt.sublib_lenz_elite );
  else
    wDigInt.setsublib(m_Props, wDigInt.sublib_default );


  if( m_BPS->GetSelection() == 0 )
    wDigInt.setbps( m_Props, 9600 );
  else if( m_BPS->GetSelection() == 1 )
    wDigInt.setbps( m_Props, 19200 );
  else if( m_BPS->GetSelection() == 2 )
    wDigInt.setbps( m_Props, 38400 );
  else if( m_BPS->GetSelection() == 3 )
    wDigInt.setbps( m_Props, 57600 );
}



/*!
 * LenzDlg creator
 */

bool LenzDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin LenzDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end LenzDlg creation
    return true;
}


/*!
 * LenzDlg destructor
 */

LenzDlg::~LenzDlg()
{
////@begin LenzDlg destruction
////@end LenzDlg destruction
}


/*!
 * Member initialisation
 */

void LenzDlg::Init()
{
////@begin LenzDlg member initialisation
    m_MainPanel = NULL;
    m_labIID = NULL;
    m_IID = NULL;
    m_labDevice = NULL;
    m_Device = NULL;
    m_Type = NULL;
    m_BPS = NULL;
    m_labSensor = NULL;
    m_labSensorOffset = NULL;
    m_SensorOffset = NULL;
    m_labSensorDebounce = NULL;
    m_SensorDebounce = NULL;
    m_labSwitch = NULL;
    m_labSwitchTime = NULL;
    m_SwitchTime = NULL;
    m_labPower = NULL;
    m_PowerAtStartup = NULL;
////@end LenzDlg member initialisation
}


/*!
 * Control creation for LenzDlg
 */

void LenzDlg::CreateControls()
{
////@begin LenzDlg content construction
    LenzDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_MainPanel = new wxPanel( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(m_MainPanel, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    m_MainPanel->SetSizer(itemBoxSizer4);

    wxFlexGridSizer* itemFlexGridSizer5 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer5->AddGrowableCol(1);
    itemBoxSizer4->Add(itemFlexGridSizer5, 0, wxGROW, 5);

    m_labIID = new wxStaticText( m_MainPanel, wxID_ANY, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_IID = new wxTextCtrl( m_MainPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_IID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDevice = new wxStaticText( m_MainPanel, wxID_ANY, _("Device"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_labDevice, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Device = new wxTextCtrl( m_MainPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer5->Add(m_Device, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_TypeStrings;
    m_TypeStrings.Add(_("&Lenz"));
    m_TypeStrings.Add(_("&Hornby"));
    m_TypeStrings.Add(_("&USB"));
    m_Type = new wxRadioBox( m_MainPanel, wxID_ANY, _("SubType"), wxDefaultPosition, wxDefaultSize, m_TypeStrings, 1, wxRA_SPECIFY_ROWS );
    m_Type->SetSelection(0);
    itemBoxSizer4->Add(m_Type, 0, wxGROW|wxALL, 5);

    wxArrayString m_BPSStrings;
    m_BPSStrings.Add(_("&9600"));
    m_BPSStrings.Add(_("&19200"));
    m_BPSStrings.Add(_("&38400"));
    m_BPSStrings.Add(_("&57600"));
    m_BPS = new wxRadioBox( m_MainPanel, wxID_ANY, _("BPS"), wxDefaultPosition, wxDefaultSize, m_BPSStrings, 1, wxRA_SPECIFY_COLS );
    m_BPS->SetSelection(0);
    itemBoxSizer4->Add(m_BPS, 0, wxGROW|wxALL, 5);

    m_labSensor = new wxStaticBox(m_MainPanel, wxID_ANY, _("Sensor"));
    wxStaticBoxSizer* itemStaticBoxSizer12 = new wxStaticBoxSizer(m_labSensor, wxVERTICAL);
    itemBoxSizer4->Add(itemStaticBoxSizer12, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer13 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer13->AddGrowableRow(1);
    itemStaticBoxSizer12->Add(itemFlexGridSizer13, 0, wxGROW|wxALL, 5);

    m_labSensorOffset = new wxStaticText( m_MainPanel, wxID_ANY, _("Offset"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer13->Add(m_labSensorOffset, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SensorOffset = new wxSpinCtrl( m_MainPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 99, 0 );
    itemFlexGridSizer13->Add(m_SensorOffset, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSensorDebounce = new wxStaticText( m_MainPanel, wxID_ANY, _("Debounce"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer13->Add(m_labSensorDebounce, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SensorDebounce = new wxSpinCtrl( m_MainPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 1000, 0 );
    itemFlexGridSizer13->Add(m_SensorDebounce, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSwitch = new wxStaticBox(m_MainPanel, wxID_ANY, _("Switch"));
    wxStaticBoxSizer* itemStaticBoxSizer18 = new wxStaticBoxSizer(m_labSwitch, wxVERTICAL);
    itemBoxSizer4->Add(itemStaticBoxSizer18, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer19 = new wxFlexGridSizer(2, 2, 0, 0);
    itemStaticBoxSizer18->Add(itemFlexGridSizer19, 0, wxGROW|wxALL, 5);

    m_labSwitchTime = new wxStaticText( m_MainPanel, wxID_ANY, _("Time (ms)"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labSwitchTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SwitchTime = new wxSpinCtrl( m_MainPanel, wxID_ANY, _T("250"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 1000, 250 );
    itemFlexGridSizer19->Add(m_SwitchTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPower = new wxStaticBox(m_MainPanel, wxID_ANY, _("Power"));
    wxStaticBoxSizer* itemStaticBoxSizer22 = new wxStaticBoxSizer(m_labPower, wxVERTICAL);
    itemBoxSizer4->Add(itemStaticBoxSizer22, 0, wxGROW|wxALL, 5);

    m_PowerAtStartup = new wxCheckBox( m_MainPanel, wxID_ANY, _("On at startup"), wxDefaultPosition, wxDefaultSize, 0 );
    m_PowerAtStartup->SetValue(false);
    itemStaticBoxSizer22->Add(m_PowerAtStartup, 0, wxALIGN_LEFT|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer24 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer24, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    wxButton* itemButton25 = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer24->AddButton(itemButton25);

    wxButton* itemButton26 = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer24->AddButton(itemButton26);

    itemStdDialogButtonSizer24->Realize();

////@end LenzDlg content construction
}


/*!
 * Should we show tooltips?
 */

bool LenzDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap LenzDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin LenzDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end LenzDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon LenzDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin LenzDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end LenzDlg icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void LenzDlg::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void LenzDlg::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}

