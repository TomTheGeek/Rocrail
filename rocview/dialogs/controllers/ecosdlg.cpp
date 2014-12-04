/*
 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 

 */
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "ecosdlg.h"
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

#include "ecosdlg.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/MCS2.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocview/public/guiapp.h"
#include "rocs/public/strtok.h"

////@begin XPM images
////@end XPM images

/*!
 * ECoSCtrlDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( ECoSCtrlDialog, wxDialog )

/*!
 * ECoSCtrlDialog event table definition
 */

BEGIN_EVENT_TABLE( ECoSCtrlDialog, wxDialog )

////@begin ECoSCtrlDialog event table entries
    EVT_RADIOBOX( ID_SUBLIB, ECoSCtrlDialog::OnSublibSelected )
    EVT_BUTTON( ID_BUTTON_SET_FBADDR, ECoSCtrlDialog::OnButtonSetFbaddrClick )
    EVT_BUTTON( wxID_OK, ECoSCtrlDialog::OnOkClick )
    EVT_BUTTON( wxID_CANCEL, ECoSCtrlDialog::OnCancelClick )
    EVT_BUTTON( wxID_HELP, ECoSCtrlDialog::OnHelpClick )
////@end ECoSCtrlDialog event table entries

END_EVENT_TABLE()

/*!
 * ECoSCtrlDialog constructors
 */

ECoSCtrlDialog::ECoSCtrlDialog()
{
    Init();
}

ECoSCtrlDialog::ECoSCtrlDialog( wxWindow* parent, iONode props, const char* devices )
{
  Create(parent, -1, wxString::From8BitData(wDigInt.getlib(props)).Upper());
  m_Props = props;
  m_Devices = devices;
  initLabels();
  initValues();

  if( StrOp.equals( wDigInt.mcs2, wDigInt.getlib( m_Props ) ) || StrOp.equals( wDigInt.mgbox, wDigInt.getlib( m_Props ) ) ) {
    m_Port->Enable(false);
    this->SetTitle( _T("CS2") );
    if( wDigInt.getmcs2(m_Props) == NULL ) {
      iONode mcs2 = NodeOp.inst(wMCS2.name(), m_Props, ELEMENT_NODE);
      NodeOp.addChild(m_Props, mcs2);
    }
  }
  else {
    m_FBMod->Enable(false);
    m_Discovery->Enable(false);
    m_SLCAN->Enable(false);
  }

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  GetSizer()->Layout();
}


void ECoSCtrlDialog::initLabels() {
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "device" ) );
  m_labHost->SetLabel( wxGetApp().getMsg( "host" ) );
  m_labPort->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labUserID->SetLabel( wxGetApp().getMsg( "userid" ) );
  m_labUDPPorts->SetLabel( wxGetApp().getMsg( "udpports" ) );
  m_labFBMod->SetLabel( wxGetApp().getMsg( "sensors" ) );
  m_labVersion->SetLabel( wxGetApp().getMsg( "version" ) );
  m_SubLib->SetLabel( wxGetApp().getMsg( "type" ) );
  m_OptionsBox->SetLabel( wxGetApp().getMsg( "options" ) );
  m_SystemInfo->SetLabel( wxGetApp().getMsg( "systeminfo" ) );
  m_LocoList->SetLabel( wxGetApp().getMsg( "loctable" ) );
  m_Discovery->SetLabel( wxGetApp().getMsg( "discover" ) );
  m_Bind->SetLabel( wxGetApp().getMsg( "bind" ) );
  m_labSwTime->SetLabel( wxGetApp().getMsg( "switchtime" ) );
  m_labCmdPause->SetLabel( wxGetApp().getMsg( "cmdpause" ) );
  m_labSensorRange->SetLabel( wxGetApp().getMsg( "sensorrange" ) );
  m_ReportState->SetLabel( wxGetApp().getMsg( "reportstate" ) );
}

void ECoSCtrlDialog::initValues() {
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

  m_Host->SetValue( wxString( wDigInt.gethost( m_Props ), wxConvUTF8 ) );
  m_UserID->SetValue( wxString( wDigInt.getuserid( m_Props ), wxConvUTF8 ) );

  char* val = StrOp.fmt( "%d", wDigInt.getport( m_Props ) );
  m_Port->SetValue( wxString( val, wxConvUTF8 ) );
  StrOp.free( val );
  m_PortTX->SetValue( wxString::Format(wxT("%d"), wDigInt.getudpportTX( m_Props ) ) );
  m_PortRX->SetValue( wxString::Format(wxT("%d"), wDigInt.getudpportRX( m_Props ) ) );
  m_FBMod->SetValue( wDigInt.getfbmod( m_Props ) );
  m_Version->SetValue( wDigInt.getprotver( m_Props ) );

  if( StrOp.equals( wDigInt.sublib_serial, wDigInt.getsublib(m_Props) )) {
    m_SubLib->SetSelection(1);
  }
  m_SystemInfo->SetValue( wDigInt.issysteminfo( m_Props ) );
  m_LocoList->SetValue( wDigInt.islocolist( m_Props ) );

  m_CTS->SetValue(  StrOp.equals( wDigInt.getflow( m_Props ), wDigInt.cts ) );
  m_SLCAN->SetValue( wDigInt.isasciiprotocol( m_Props ) );
  m_Discovery->SetValue(false);
  m_Bind->SetValue(false);
  m_ReportState->SetValue( wDigInt.isreportstate(m_Props)?true:false );

  if( StrOp.equals( wDigInt.mcs2, wDigInt.getlib( m_Props ) ) || StrOp.equals( wDigInt.mgbox, wDigInt.getlib( m_Props ) ) ) {
    m_Discovery->SetValue( wMCS2.isdiscovery(wDigInt.getmcs2( m_Props )) );
    m_Bind->SetValue( wMCS2.isbind(wDigInt.getmcs2( m_Props )) );
  }
  m_SwTime->SetValue( wDigInt.getswtime( m_Props ) );
  m_CmdPause->SetValue( wMCS2.getcmdpause(wDigInt.getmcs2( m_Props ))  );
  m_SensorBegin->SetValue( wMCS2.getsensorbegin(wDigInt.getmcs2( m_Props ))  );
  m_SensorEnd->SetValue( wMCS2.getsensorend(wDigInt.getmcs2( m_Props ))  );

  SublibSelected();

}


void ECoSCtrlDialog::evaluate() {
  if( m_Props == NULL )
    return;
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setdevice( m_Props, m_Device->GetValue().mb_str(wxConvUTF8) );
  wDigInt.sethost( m_Props, m_Host->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setuserid( m_Props, m_UserID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setport( m_Props, atoi( m_Port->GetValue().mb_str(wxConvUTF8) ) );
  wDigInt.setudpportTX( m_Props, atoi( m_PortTX->GetValue().mb_str(wxConvUTF8) ) );
  wDigInt.setudpportRX( m_Props, atoi( m_PortRX->GetValue().mb_str(wxConvUTF8) ) );
  wDigInt.setfbmod( m_Props, m_FBMod->GetValue() );
  wDigInt.setprotver( m_Props, m_Version->GetValue() );
  if( m_SubLib->GetSelection() == 1 )
    wDigInt.setsublib(m_Props, wDigInt.sublib_serial );
  else
    wDigInt.setsublib(m_Props, wDigInt.sublib_default );
  wDigInt.setsysteminfo( m_Props, m_SystemInfo->IsChecked()?True:False );
  wDigInt.setlocolist( m_Props, m_LocoList->IsChecked()?True:False );
  wDigInt.setflow( m_Props, m_CTS->IsChecked()?wDigInt.cts:wDigInt.none );
  wDigInt.setasciiprotocol( m_Props, m_SLCAN->IsChecked()?True:False );

  if( StrOp.equals( wDigInt.mcs2, wDigInt.getlib( m_Props ) ) || StrOp.equals( wDigInt.mgbox, wDigInt.getlib( m_Props ) ) ) {
    wMCS2.setdiscovery(wDigInt.getmcs2( m_Props), m_Discovery->IsChecked()?True:False );
    wMCS2.setbind(wDigInt.getmcs2( m_Props), m_Bind->IsChecked()?True:False );
  }
  wDigInt.setswtime( m_Props, m_SwTime->GetValue() );
  wMCS2.setcmdpause(wDigInt.getmcs2( m_Props), m_CmdPause->GetValue() );
  wMCS2.setsensorbegin(wDigInt.getmcs2( m_Props), m_SensorBegin->GetValue() );
  wMCS2.setsensorend(wDigInt.getmcs2( m_Props), m_SensorEnd->GetValue() );
  wDigInt.setreportstate(m_Props, m_ReportState->IsChecked()?True:False );

}


/*!
 * Dialog creator
 */

bool ECoSCtrlDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin ECoSCtrlDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end ECoSCtrlDialog creation
    return true;
}

/*!
 * Dialog destructor
 */

ECoSCtrlDialog::~ECoSCtrlDialog()
{
////@begin ECoSCtrlDialog destruction
////@end ECoSCtrlDialog destruction
}

/*!
 * Member initialisation
 */

void ECoSCtrlDialog::Init()
{
////@begin ECoSCtrlDialog member initialisation
    m_labIID = NULL;
    m_IID = NULL;
    m_labDevice = NULL;
    m_Device = NULL;
    m_labHost = NULL;
    m_Host = NULL;
    m_labPort = NULL;
    m_Port = NULL;
    m_labUserID = NULL;
    m_UserID = NULL;
    m_labUDPPorts = NULL;
    m_PortTX = NULL;
    m_PortRX = NULL;
    m_labFBMod = NULL;
    m_FBMod = NULL;
    m_labSensorRange = NULL;
    m_SensorBegin = NULL;
    m_SensorEnd = NULL;
    m_labVersion = NULL;
    m_Version = NULL;
    m_SubLib = NULL;
    m_OptionsBox = NULL;
    m_SystemInfo = NULL;
    m_LocoList = NULL;
    m_CTS = NULL;
    m_SLCAN = NULL;
    m_Discovery = NULL;
    m_Bind = NULL;
    m_ReportState = NULL;
    m_SertFbAddr = NULL;
    m_FbAddr = NULL;
    m_labSwTime = NULL;
    m_SwTime = NULL;
    m_labCmdPause = NULL;
    m_CmdPause = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end ECoSCtrlDialog member initialisation
}

/*!
 * Control creation for Dialog
 */

void ECoSCtrlDialog::CreateControls()
{
////@begin ECoSCtrlDialog content construction
    ECoSCtrlDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxPanel* itemPanel3 = new wxPanel( itemDialog1, ID_PANEL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    itemBoxSizer2->Add(itemPanel3, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxHORIZONTAL);
    itemPanel3->SetSizer(itemBoxSizer4);

    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer5, 0, wxALIGN_TOP|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
    itemBoxSizer5->Add(itemFlexGridSizer6, 0, wxGROW|wxALL, 5);

    m_labIID = new wxStaticText( itemPanel3, wxID_ANY, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_IID = new wxTextCtrl( itemPanel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_IID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDevice = new wxStaticText( itemPanel3, wxID_ANY, _("Device"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labDevice, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_DeviceStrings;
    m_Device = new wxComboBox( itemPanel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_DeviceStrings, wxCB_DROPDOWN );
    itemFlexGridSizer6->Add(m_Device, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labHost = new wxStaticText( itemPanel3, wxID_ANY, _("Host"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labHost, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Host = new wxTextCtrl( itemPanel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), 0 );
    itemFlexGridSizer6->Add(m_Host, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labPort = new wxStaticText( itemPanel3, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Port = new wxTextCtrl( itemPanel3, wxID_ANY, _("15471"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_Port->SetMaxLength(5);
    itemFlexGridSizer6->Add(m_Port, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labUserID = new wxStaticText( itemPanel3, wxID_ANY, _("User ID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labUserID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_UserID = new wxTextCtrl( itemPanel3, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_UserID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labUDPPorts = new wxStaticText( itemPanel3, wxID_ANY, _("UDP Ports"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labUDPPorts, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer18 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer6->Add(itemFlexGridSizer18, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5);

    m_PortTX = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_PortTX->SetMaxLength(5);
    itemFlexGridSizer18->Add(m_PortTX, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_PortRX = new wxTextCtrl( itemPanel3, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_PortRX->SetMaxLength(5);
    itemFlexGridSizer18->Add(m_PortRX, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labFBMod = new wxStaticText( itemPanel3, wxID_ANY, _("FBmod"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labFBMod, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_FBMod = new wxSpinCtrl( itemPanel3, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 64, 0 );
    itemFlexGridSizer6->Add(m_FBMod, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labSensorRange = new wxStaticText( itemPanel3, wxID_ANY, _("Sensor range"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labSensorRange, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer24 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer6->Add(itemFlexGridSizer24, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_SensorBegin = new wxSpinCtrl( itemPanel3, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 16383, 0 );
    itemFlexGridSizer24->Add(m_SensorBegin, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SensorEnd = new wxSpinCtrl( itemPanel3, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 16383, 0 );
    itemFlexGridSizer24->Add(m_SensorEnd, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labVersion = new wxStaticText( itemPanel3, wxID_ANY, _("Version"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labVersion, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Version = new wxSpinCtrl( itemPanel3, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 10, 0 );
    itemFlexGridSizer6->Add(m_Version, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemFlexGridSizer6->AddGrowableCol(1);

    wxBoxSizer* itemBoxSizer29 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer4->Add(itemBoxSizer29, 0, wxALIGN_TOP|wxALL, 5);

    wxArrayString m_SubLibStrings;
    m_SubLibStrings.Add(_("&Ethernet"));
    m_SubLibStrings.Add(_("&Serial"));
    m_SubLib = new wxRadioBox( itemPanel3, ID_SUBLIB, _("SubType"), wxDefaultPosition, wxDefaultSize, m_SubLibStrings, 1, wxRA_SPECIFY_ROWS );
    m_SubLib->SetSelection(0);
    itemBoxSizer29->Add(m_SubLib, 0, wxALIGN_LEFT|wxALL, 5);

    m_OptionsBox = new wxStaticBox(itemPanel3, wxID_ANY, _("Options"));
    wxStaticBoxSizer* itemStaticBoxSizer31 = new wxStaticBoxSizer(m_OptionsBox, wxVERTICAL);
    itemBoxSizer29->Add(itemStaticBoxSizer31, 0, wxGROW|wxALL, 5);

    m_SystemInfo = new wxCheckBox( itemPanel3, wxID_ANY, _("System info"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SystemInfo->SetValue(false);
    itemStaticBoxSizer31->Add(m_SystemInfo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_LocoList = new wxCheckBox( itemPanel3, wxID_ANY, _("Loco list"), wxDefaultPosition, wxDefaultSize, 0 );
    m_LocoList->SetValue(false);
    itemStaticBoxSizer31->Add(m_LocoList, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_CTS = new wxCheckBox( itemPanel3, wxID_ANY, _("CTS"), wxDefaultPosition, wxDefaultSize, 0 );
    m_CTS->SetValue(false);
    itemStaticBoxSizer31->Add(m_CTS, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_SLCAN = new wxCheckBox( itemPanel3, wxID_ANY, _("SLCAN"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SLCAN->SetValue(false);
    itemStaticBoxSizer31->Add(m_SLCAN, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Discovery = new wxCheckBox( itemPanel3, wxID_ANY, _("Discovery"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Discovery->SetValue(false);
    itemStaticBoxSizer31->Add(m_Discovery, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_Bind = new wxCheckBox( itemPanel3, wxID_ANY, _("Bind"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Bind->SetValue(false);
    itemStaticBoxSizer31->Add(m_Bind, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_ReportState = new wxCheckBox( itemPanel3, wxID_ANY, _("Report state"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ReportState->SetValue(false);
    itemStaticBoxSizer31->Add(m_ReportState, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxFlexGridSizer* itemFlexGridSizer39 = new wxFlexGridSizer(0, 2, 0, 0);
    itemStaticBoxSizer31->Add(itemFlexGridSizer39, 0, wxALIGN_LEFT, 5);

    m_SertFbAddr = new wxButton( itemPanel3, ID_BUTTON_SET_FBADDR, _("Program FB"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer39->Add(m_SertFbAddr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_FbAddr = new wxSpinCtrl( itemPanel3, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 255, 0 );
    itemFlexGridSizer39->Add(m_FbAddr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labSwTime = new wxStaticText( itemPanel3, wxID_ANY, _("Switch time"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer39->Add(m_labSwTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_SwTime = new wxSpinCtrl( itemPanel3, wxID_ANY, wxT("0"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 0, 10000, 0 );
    itemFlexGridSizer39->Add(m_SwTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_labCmdPause = new wxStaticText( itemPanel3, wxID_ANY, _("Command pause"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer39->Add(m_labCmdPause, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_CmdPause = new wxSpinCtrl( itemPanel3, wxID_ANY, wxT("10"), wxDefaultPosition, wxSize(100, -1), wxSP_ARROW_KEYS, 10, 500, 10 );
    itemFlexGridSizer39->Add(m_CmdPause, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer46 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer46, 0, wxGROW|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer46->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer46->AddButton(m_Cancel);

    wxButton* itemButton49 = new wxButton( itemDialog1, wxID_HELP, _("&Help"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer46->AddButton(itemButton49);

    itemStdDialogButtonSizer46->Realize();

////@end ECoSCtrlDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool ECoSCtrlDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap ECoSCtrlDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin ECoSCtrlDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end ECoSCtrlDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon ECoSCtrlDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin ECoSCtrlDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end ECoSCtrlDialog icon retrieval
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void ECoSCtrlDialog::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void ECoSCtrlDialog::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


/*!
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_SUBLIB
 */

void ECoSCtrlDialog::OnSublibSelected( wxCommandEvent& event )
{
  SublibSelected();
}

void ECoSCtrlDialog::SublibSelected()
{
  if( m_SubLib->GetSelection() == 1 ) {
    m_Host->Enable(false);
    m_Port->Enable(false);
    m_Device->Enable(true);
    m_CTS->Enable(true);
    m_SLCAN->Enable(true);
  }
  else {
    m_Host->Enable(true);
    m_Port->Enable( (StrOp.equals( wDigInt.mcs2, wDigInt.getlib( m_Props ) ) || StrOp.equals( wDigInt.mgbox, wDigInt.getlib( m_Props ) ) )?false:true);
    m_Device->Enable(false);
    m_CTS->Enable(false);
    m_SLCAN->Enable(false);
  }
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SET_FBADDR
 */

void ECoSCtrlDialog::OnButtonSetFbaddrClick( wxCommandEvent& event )
{
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcmd( cmd, wProgram.fb_setaddr );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setdecaddr( cmd, m_FbAddr->GetValue() );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_HELP
 */

void ECoSCtrlDialog::OnHelpClick( wxCommandEvent& event )
{ // ecos, zimocan, z21, mcs2
  if( StrOp.equals( wDigInt.mcs2, wDigInt.getlib( m_Props ) ) )
    wxGetApp().openLink( "cs2:cs2" );
  else if( StrOp.equals( wDigInt.mgbox, wDigInt.getlib( m_Props ) ) )
    wxGetApp().openLink( "cs2:gbox" );
  else if( StrOp.equals( wDigInt.ecos, wDigInt.getlib( m_Props ) ) )
    wxGetApp().openLink( "ecos" );
  else if( StrOp.equals( wDigInt.z21, wDigInt.getlib( m_Props ) ) )
    wxGetApp().openLink( "roco:roco-z21" );
  else if( StrOp.equals( wDigInt.zimocan, wDigInt.getlib( m_Props ) ) )
    wxGetApp().openLink( "zimocan" );
}

