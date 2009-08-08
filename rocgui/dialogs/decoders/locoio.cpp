/////////////////////////////////////////////////////////////////////////////
// Name:        locoio.cpp
// Purpose:
// Author:
// Modified by:
// Created:     Sat 12 Jan 2008 09:07:06 CET
// RCS-ID:
// Copyright:
// Licence:
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "locoio.h"
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
#include <wx/filedlg.h>

#include "locoio.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/strtok.h"
#include "rocs/public/string.h"
#include "rocs/public/file.h"


#include "rocgui/public/guiapp.h"

#include "rocrail/wrapper/public/Program.h"
#include "rocgui/wrapper/public/Gui.h"

////@begin XPM images
////@end XPM images


/*!
 * LocoIO type definition
 */

IMPLEMENT_DYNAMIC_CLASS( LocoIO, wxDialog )


/*!
 * LocoIO event table definition
 */

BEGIN_EVENT_TABLE( LocoIO, wxDialog )

////@begin LocoIO event table entries
    EVT_SPINCTRL( wxID_LOCOIO_LOW_ADDR, LocoIO::OnLocoioLowAddrUpdated )

    EVT_BUTTON( ID_BUTTON_GET_GEN, LocoIO::OnButtonGetGenClick )

    EVT_BUTTON( ID_BUTTON_SET_GEN, LocoIO::OnButtonSetGenClick )

    EVT_SPINCTRL( wxID_LOCOIO_SUB_ADDR, LocoIO::OnLocoioSubAddrUpdated )

    EVT_BUTTON( ID_BUTTON_RESET_LOCOIO, LocoIO::OnButtonResetLocoioClick )

    EVT_SPINCTRL( wxID_ANY, LocoIO::OnAnyUpdated )
    EVT_SPIN_UP( wxID_ANY, LocoIO::OnAnyUp )
    EVT_SPIN_DOWN( wxID_ANY, LocoIO::OnAnyDown )

    EVT_BUTTON( ID_BUTTON_PORT_GET, LocoIO::OnButtonPortGetClick )

    EVT_BUTTON( ID_BUTTON_PORT_SET, LocoIO::OnButtonPortSetClick )

    EVT_RADIOBOX( ID_LOCOIO_TYPE, LocoIO::OnLocoioTypeSelected )

    EVT_BUTTON( ID_BUTTON_OPC_GET, LocoIO::OnButtonOpcGetClick )

    EVT_BUTTON( ID_BUTTON_OPC_SET, LocoIO::OnButtonOpcSetClick )

    EVT_BUTTON( ID_BUTTON_SERVO_PORT_GET, LocoIO::OnButtonServoPortGetClick )

    EVT_BUTTON( ID_BUTTON_SERVO_PORT_SET, LocoIO::OnButtonServoPortSetClick )

    EVT_BUTTON( ID_EASYGETALL, LocoIO::OnEasygetallClick )

    EVT_BUTTON( ID_EASYSETALL, LocoIO::OnEasysetallClick )

    EVT_BUTTON( ID_BUTTON_EASY_SAVE, LocoIO::OnButtonEasySaveClick )

    EVT_BUTTON( ID_BUTTON_EASY_READ, LocoIO::OnButtonEasyReadClick )

    EVT_CHECKBOX( ID_LOCOIO_BOOSTER, LocoIO::OnBoosterClick )

    EVT_LISTBOX_DCLICK( ID_LN_ADDRESSLIST, LocoIO::OnLnAddresslistDoubleClicked )

    EVT_BUTTON( ID_QUEURY_ADDRESSES, LocoIO::OnQueuryAddressesClick )

    EVT_BUTTON( ID_BUTTON_MP_READ, LocoIO::OnButtonMpReadClick )

    EVT_BUTTON( ID_BUTTON_MP_WRITE, LocoIO::OnButtonMpWriteClick )

    EVT_BUTTON( wxID_CANCEL, LocoIO::OnCancelClick )

////@end LocoIO event table entries
    EVT_TIMER (ME_SVTimer, LocoIO::OnTimer)

END_EVENT_TABLE()


/*!
 * LocoIO constructors
 */

LocoIO::LocoIO()
{
    Init();
}

LocoIO::LocoIO( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    m_TabAlign = wxGetApp().getTabAlign();
    Create(parent, id, caption, pos, size, style);
    m_Queue = QueueOp.inst(100);
    m_SendedCmd = NULL;
    m_Timer = new wxTimer( this, ME_SVTimer );
    m_iLowAddress = m_LowAddr->GetValue();
    m_iSubAddress = m_SubAddr->GetValue();

    SetTitle( wxString::Format( _T("%s  %d-%d"), _T("LocoIO"), m_iLowAddress , m_iSubAddress ) );

}


void LocoIO::initLabels() {

}


void LocoIO::sendPacket() {
  if( !QueueOp.isEmpty(m_Queue) ) {
    iONode cmd = (iONode)QueueOp.get(m_Queue);
    wxGetApp().sendToRocrail( cmd );
    m_SendedCmd = cmd;
    this->SetCursor(wxCURSOR_WATCH);
    m_Timer->Start( 2000, wxTIMER_ONE_SHOT );
  }
  else {
    m_Timer->Stop();
    this->SetCursor(wxCURSOR_ARROW);
  }
}
void LocoIO::OnTimer(wxTimerEvent& event) {
  TraceOp.trc( "locoio", TRCLEVEL_WARNING, __LINE__, 9999, "timeout on reply...");
  if( m_SendedCmd != NULL ) {

    NodeOp.base.del(m_SendedCmd);
    m_SendedCmd = NULL;

    /* empty queue */
    iONode cmd = (iONode)QueueOp.get(m_Queue);
    while( cmd != NULL ) {
      NodeOp.base.del(cmd);
      cmd = (iONode)QueueOp.get(m_Queue);
    }
  }
}

/*!
 * LocoIO creator
 */

bool LocoIO::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin LocoIO creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end LocoIO creation
    return true;
}


/*!
 * LocoIO destructor
 */

LocoIO::~LocoIO()
{
////@begin LocoIO destruction
////@end LocoIO destruction
}


/*!
 * Member initialisation
 */

void LocoIO::Init()
{
////@begin LocoIO member initialisation
    m_GeneralPanel = NULL;
    m_AddressBox = NULL;
    m_labIID = NULL;
    m_IID = NULL;
    m_labLowAddr = NULL;
    m_LowAddr = NULL;
    m_GetGen = NULL;
    m_SetGen = NULL;
    m_labSubAddr = NULL;
    m_SubAddr = NULL;
    m_SetupBox = NULL;
    m_PortRefresh = NULL;
    m_FilpButtonCode = NULL;
    m_4PosServo = NULL;
    m_Port_5_12_Servos = NULL;
    m_labFlashFreq = NULL;
    m_FlashFreq = NULL;
    m_VersionBox = NULL;
    m_labVersion = NULL;
    m_Version = NULL;
    m_labModConfig = NULL;
    m_ModConfig = NULL;
    m_IOPanel = NULL;
    m_labPort = NULL;
    m_Port = NULL;
    m_PortGet = NULL;
    m_PortSet = NULL;
    m_PortSetupBox = NULL;
    m_labPortAddr = NULL;
    m_PortAddr = NULL;
    m_SensorContact = NULL;
    m_PortType = NULL;
    m_OutputSetupBox = NULL;
    m_LowAtStartup = NULL;
    m_HardwareReset = NULL;
    m_PulseContact = NULL;
    m_Flash = NULL;
    m_Multi = NULL;
    m_BlockDetector = NULL;
    m_RegisterBox = NULL;
    m_labConfig = NULL;
    m_Config = NULL;
    m_ConfigHex = NULL;
    m_labVal1 = NULL;
    m_Val1 = NULL;
    m_Val1Hex = NULL;
    m_labVal2 = NULL;
    m_Val2 = NULL;
    m_Val2Hex = NULL;
    m_SensorSetupBox = NULL;
    m_SensorActiveLow = NULL;
    m_SensorDelay = NULL;
    m_SensorTurnout = NULL;
    m_SensorPulse = NULL;
    m_SensorMessage = NULL;
    m_InputType = NULL;
    m_OPCPanel = NULL;
    m_labOPCPort = NULL;
    m_OPCPort = NULL;
    m_OPCGet = NULL;
    m_OPCSet = NULL;
    m_labOPC = NULL;
    m_OPC = NULL;
    m_labARG1 = NULL;
    m_ARG1 = NULL;
    m_labARG2 = NULL;
    m_ARG2 = NULL;
    m_labServoPort = NULL;
    m_ServoPort = NULL;
    m_ServoPortGet = NULL;
    m_ServoPortSet = NULL;
    m_ServoBox = NULL;
    m_labPos1 = NULL;
    m_Pos1 = NULL;
    m_labPos2 = NULL;
    m_Pos2 = NULL;
    m_labV = NULL;
    m_V = NULL;
    m_EasyPanel = NULL;
    m_labEasyPort = NULL;
    m_labEasyAddress = NULL;
    m_labEasyInput = NULL;
    m_labEasyOutput = NULL;
    m_EasyAddr1 = NULL;
    m_EasyInput1 = NULL;
    m_EasyBlock1 = NULL;
    m_EasyOutput1 = NULL;
    m_EasyPulse1 = NULL;
    m_EasyContact1 = NULL;
    m_EasyAddr2 = NULL;
    m_EasyInput2 = NULL;
    m_EasyBlock2 = NULL;
    m_EasyOutput2 = NULL;
    m_EasyPulse2 = NULL;
    m_EasyContact2 = NULL;
    m_EasyAddr3 = NULL;
    m_EasyInput3 = NULL;
    m_EasyBlock3 = NULL;
    m_EasyOutput3 = NULL;
    m_EasyPulse3 = NULL;
    m_EasyContact3 = NULL;
    m_EasyAddr4 = NULL;
    m_EasyInput4 = NULL;
    m_EasyBlock4 = NULL;
    m_EasyOutput4 = NULL;
    m_EasyPulse4 = NULL;
    m_EasyContact4 = NULL;
    m_EasyAddr5 = NULL;
    m_EasyInput5 = NULL;
    m_EasyBlock5 = NULL;
    m_EasyOutput5 = NULL;
    m_EasyPulse5 = NULL;
    m_EasyContact5 = NULL;
    m_EasyAddr6 = NULL;
    m_EasyInput6 = NULL;
    m_EasyBlock6 = NULL;
    m_EasyOutput6 = NULL;
    m_EasyPulse6 = NULL;
    m_EasyContact6 = NULL;
    m_EasyAddr7 = NULL;
    m_EasyInput7 = NULL;
    m_EasyBlock7 = NULL;
    m_EasyOutput7 = NULL;
    m_EasyPulse7 = NULL;
    m_EasyContact7 = NULL;
    m_EasyAddr8 = NULL;
    m_EasyInput8 = NULL;
    m_EasyBlock8 = NULL;
    m_EasyOutput8 = NULL;
    m_EasyPulse8 = NULL;
    m_EasyContact8 = NULL;
    m_EasyAddr9 = NULL;
    m_EasyInput9 = NULL;
    m_EasyBlock9 = NULL;
    m_EasyOutput9 = NULL;
    m_EasyPulse9 = NULL;
    m_EasyContact9 = NULL;
    m_EasyAddr10 = NULL;
    m_EasyInput10 = NULL;
    m_EasyBlock10 = NULL;
    m_EasyOutput10 = NULL;
    m_EasyPulse10 = NULL;
    m_EasyContact10 = NULL;
    m_EasyAddr11 = NULL;
    m_EasyInput11 = NULL;
    m_EasyBlock11 = NULL;
    m_EasyOutput11 = NULL;
    m_EasyPulse11 = NULL;
    m_EasyContact11 = NULL;
    m_EasyAddr12 = NULL;
    m_EasyInput12 = NULL;
    m_EasyBlock12 = NULL;
    m_EasyOutput12 = NULL;
    m_EasyPulse12 = NULL;
    m_EasyContact12 = NULL;
    m_EasyAddr13 = NULL;
    m_EasyInput13 = NULL;
    m_EasyBlock13 = NULL;
    m_EasyOutput13 = NULL;
    m_EasyPulse13 = NULL;
    m_EasyContact13 = NULL;
    m_EasyAddr14 = NULL;
    m_EasyInput14 = NULL;
    m_EasyBlock14 = NULL;
    m_EasyOutput14 = NULL;
    m_EasyPulse14 = NULL;
    m_EasyContact14 = NULL;
    m_EasyAddr15 = NULL;
    m_EasyInput15 = NULL;
    m_EasyBlock15 = NULL;
    m_EasyOutput15 = NULL;
    m_EasyPulse15 = NULL;
    m_EasyContact15 = NULL;
    m_EasyAddr16 = NULL;
    m_EasyInput16 = NULL;
    m_EasyBlock16 = NULL;
    m_EasyOutput16 = NULL;
    m_EasyPulse16 = NULL;
    m_EasyContact16 = NULL;
    m_EasyGetAll = NULL;
    m_EasySetAll = NULL;
    m_EasySaveAll = NULL;
    m_EasyReadAll = NULL;
    m_Booster = NULL;
    m_AddressesPanel = NULL;
    m_AddressList = NULL;
    m_QueryAddresses = NULL;
    m_MultiPortPanel = NULL;
    m_MP1 = NULL;
    m_MP2 = NULL;
    m_MP3 = NULL;
    m_MP4 = NULL;
    m_MP5 = NULL;
    m_MP6 = NULL;
    m_MP7 = NULL;
    m_MP8 = NULL;
    m_MP9 = NULL;
    m_MP10 = NULL;
    m_MP11 = NULL;
    m_MP12 = NULL;
    m_MP13 = NULL;
    m_MP14 = NULL;
    m_MP15 = NULL;
    m_MP16 = NULL;
    m_MPRead = NULL;
    m_MPWrite = NULL;
    m_Cancel = NULL;
////@end LocoIO member initialisation
}


/*!
 * Control creation for LocoIO
 */

void LocoIO::CreateControls()
{
////@begin LocoIO content construction
    LocoIO* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxNotebook* itemNotebook3 = new wxNotebook( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_DEFAULT|m_TabAlign );

    m_GeneralPanel = new wxPanel( itemNotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_GeneralPanel->SetSizer(itemBoxSizer5);

    m_AddressBox = new wxStaticBox(m_GeneralPanel, wxID_ANY, _("Address"));
    wxStaticBoxSizer* itemStaticBoxSizer6 = new wxStaticBoxSizer(m_AddressBox, wxVERTICAL);
    itemBoxSizer5->Add(itemStaticBoxSizer6, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer7 = new wxFlexGridSizer(1, 2, 0, 0);
    itemFlexGridSizer7->AddGrowableCol(1);
    itemStaticBoxSizer6->Add(itemFlexGridSizer7, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);
    m_labIID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labIID, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_IID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_IID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer10 = new wxFlexGridSizer(2, 4, 0, 0);
    itemStaticBoxSizer6->Add(itemFlexGridSizer10, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    m_labLowAddr = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Low"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer10->Add(m_labLowAddr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_LowAddr = new wxSpinCtrl( m_GeneralPanel, wxID_LOCOIO_LOW_ADDR, _T("81"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 1, 127, 81 );
    itemFlexGridSizer10->Add(m_LowAddr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_GetGen = new wxButton( m_GeneralPanel, ID_BUTTON_GET_GEN, _("Get"), wxDefaultPosition, wxSize(50, 25), 0 );
    itemFlexGridSizer10->Add(m_GetGen, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SetGen = new wxButton( m_GeneralPanel, ID_BUTTON_SET_GEN, _("Set"), wxDefaultPosition, wxSize(50, 25), 0 );
    itemFlexGridSizer10->Add(m_SetGen, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labSubAddr = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Sub"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer10->Add(m_labSubAddr, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    m_SubAddr = new wxSpinCtrl( m_GeneralPanel, wxID_LOCOIO_SUB_ADDR, _T("1"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 1, 126, 1 );
    itemFlexGridSizer10->Add(m_SubAddr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxButton* itemButton17 = new wxButton( m_GeneralPanel, ID_BUTTON_RESET_LOCOIO, _("Reset"), wxDefaultPosition, wxSize(50, 25), 0 );
    itemButton17->SetBackgroundColour(wxColour(255, 186, 186));
    itemFlexGridSizer10->Add(itemButton17, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_SetupBox = new wxStaticBox(m_GeneralPanel, wxID_ANY, _("Setup"));
    wxStaticBoxSizer* itemStaticBoxSizer18 = new wxStaticBoxSizer(m_SetupBox, wxVERTICAL);
    itemBoxSizer5->Add(itemStaticBoxSizer18, 0, wxGROW|wxALL, 5);
    m_PortRefresh = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Port refresh"), wxDefaultPosition, wxDefaultSize, 0 );
    m_PortRefresh->SetValue(false);
    itemStaticBoxSizer18->Add(m_PortRefresh, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_FilpButtonCode = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Alternating code for push buttons"), wxDefaultPosition, wxDefaultSize, 0 );
    m_FilpButtonCode->SetValue(false);
    itemStaticBoxSizer18->Add(m_FilpButtonCode, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_4PosServo = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("4 Position Servo"), wxDefaultPosition, wxDefaultSize, 0 );
    m_4PosServo->SetValue(false);
    itemStaticBoxSizer18->Add(m_4PosServo, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Port_5_12_Servos = new wxCheckBox( m_GeneralPanel, wxID_ANY, _("Port 5-12 Servos"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Port_5_12_Servos->SetValue(false);
    itemStaticBoxSizer18->Add(m_Port_5_12_Servos, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxFlexGridSizer* itemFlexGridSizer23 = new wxFlexGridSizer(2, 2, 0, 0);
    itemStaticBoxSizer18->Add(itemFlexGridSizer23, 0, wxGROW|wxTOP|wxBOTTOM, 5);
    m_labFlashFreq = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Flash freq."), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer23->Add(m_labFlashFreq, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_FlashFreq = new wxSpinCtrl( m_GeneralPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 15, 0 );
    itemFlexGridSizer23->Add(m_FlashFreq, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_VersionBox = new wxStaticBox(m_GeneralPanel, wxID_ANY, _("Registers"));
    wxStaticBoxSizer* itemStaticBoxSizer26 = new wxStaticBoxSizer(m_VersionBox, wxVERTICAL);
    itemBoxSizer5->Add(itemStaticBoxSizer26, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer27 = new wxFlexGridSizer(2, 4, 0, 0);
    itemStaticBoxSizer26->Add(itemFlexGridSizer27, 0, wxGROW, 5);
    m_labVersion = new wxStaticText( m_GeneralPanel, wxID_ANY, _("version"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer27->Add(m_labVersion, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_Version = new wxTextCtrl( m_GeneralPanel, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_READONLY|wxTE_CENTRE );
    m_Version->Enable(false);
    itemFlexGridSizer27->Add(m_Version, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labModConfig = new wxStaticText( m_GeneralPanel, wxID_ANY, _("config"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer27->Add(m_labModConfig, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_ModConfig = new wxTextCtrl( m_GeneralPanel, wxID_ANY, _("0"), wxDefaultPosition, wxSize(60, -1), wxTE_READONLY|wxTE_CENTRE );
    m_ModConfig->Enable(false);
    itemFlexGridSizer27->Add(m_ModConfig, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook3->AddPage(m_GeneralPanel, _("General"));

    m_IOPanel = new wxPanel( itemNotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer33 = new wxBoxSizer(wxVERTICAL);
    m_IOPanel->SetSizer(itemBoxSizer33);

    wxFlexGridSizer* itemFlexGridSizer34 = new wxFlexGridSizer(2, 4, 0, 0);
    itemBoxSizer33->Add(itemFlexGridSizer34, 0, wxGROW|wxALL, 5);
    m_labPort = new wxStaticText( m_IOPanel, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer34->Add(m_labPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_Port = new wxSpinCtrl( m_IOPanel, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 1, 16, 1 );
    itemFlexGridSizer34->Add(m_Port, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_PortGet = new wxButton( m_IOPanel, ID_BUTTON_PORT_GET, _("Get"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer34->Add(m_PortGet, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_PortSet = new wxButton( m_IOPanel, ID_BUTTON_PORT_SET, _("Set"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer34->Add(m_PortSet, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_PortSetupBox = new wxStaticBox(m_IOPanel, wxID_ANY, _("Setup"));
    wxStaticBoxSizer* itemStaticBoxSizer39 = new wxStaticBoxSizer(m_PortSetupBox, wxVERTICAL);
    itemBoxSizer33->Add(itemStaticBoxSizer39, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    wxFlexGridSizer* itemFlexGridSizer40 = new wxFlexGridSizer(2, 2, 0, 0);
    itemStaticBoxSizer39->Add(itemFlexGridSizer40, 0, wxALIGN_LEFT|wxALL, 5);
    m_labPortAddr = new wxStaticText( m_IOPanel, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer40->Add(m_labPortAddr, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_PortAddr = new wxSpinCtrl( m_IOPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(80, -1), wxSP_ARROW_KEYS, 0, 4095, 0 );
    itemFlexGridSizer40->Add(m_PortAddr, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_SensorContactStrings;
    m_SensorContactStrings.Add(_("&1"));
    m_SensorContactStrings.Add(_("&2"));
    m_SensorContact = new wxRadioBox( m_IOPanel, wxID_ANY, _("Contact"), wxDefaultPosition, wxDefaultSize, m_SensorContactStrings, 1, wxRA_SPECIFY_ROWS );
    m_SensorContact->SetSelection(0);
    itemFlexGridSizer40->Add(m_SensorContact, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxArrayString m_PortTypeStrings;
    m_PortTypeStrings.Add(_("&Output"));
    m_PortTypeStrings.Add(_("&Input"));
    m_PortType = new wxRadioBox( m_IOPanel, ID_LOCOIO_TYPE, _("Type"), wxDefaultPosition, wxDefaultSize, m_PortTypeStrings, 1, wxRA_SPECIFY_ROWS );
    m_PortType->SetSelection(0);
    itemFlexGridSizer40->Add(m_PortType, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxBoxSizer* itemBoxSizer45 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer33->Add(itemBoxSizer45, 0, wxGROW|wxTOP|wxBOTTOM, 5);
    wxBoxSizer* itemBoxSizer46 = new wxBoxSizer(wxVERTICAL);
    itemBoxSizer45->Add(itemBoxSizer46, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    m_OutputSetupBox = new wxStaticBox(m_IOPanel, wxID_ANY, _("Output setup"));
    wxStaticBoxSizer* itemStaticBoxSizer47 = new wxStaticBoxSizer(m_OutputSetupBox, wxVERTICAL);
    itemBoxSizer46->Add(itemStaticBoxSizer47, 1, wxGROW, 5);
    m_LowAtStartup = new wxCheckBox( m_IOPanel, wxID_ANY, _("Low at startup"), wxDefaultPosition, wxDefaultSize, 0 );
    m_LowAtStartup->SetValue(false);
    itemStaticBoxSizer47->Add(m_LowAtStartup, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_HardwareReset = new wxCheckBox( m_IOPanel, wxID_ANY, _("Hardware reset"), wxDefaultPosition, wxDefaultSize, 0 );
    m_HardwareReset->SetValue(false);
    itemStaticBoxSizer47->Add(m_HardwareReset, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_PulseContact = new wxCheckBox( m_IOPanel, wxID_ANY, _("Pulse contact"), wxDefaultPosition, wxDefaultSize, 0 );
    m_PulseContact->SetValue(false);
    itemStaticBoxSizer47->Add(m_PulseContact, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Flash = new wxCheckBox( m_IOPanel, wxID_ANY, _("Flash"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Flash->SetValue(false);
    itemStaticBoxSizer47->Add(m_Flash, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_Multi = new wxCheckBox( m_IOPanel, wxID_ANY, _("Multi"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Multi->SetValue(false);
    itemStaticBoxSizer47->Add(m_Multi, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_BlockDetector = new wxCheckBox( m_IOPanel, wxID_ANY, _("Block detector"), wxDefaultPosition, wxDefaultSize, 0 );
    m_BlockDetector->SetValue(false);
    itemStaticBoxSizer47->Add(m_BlockDetector, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_RegisterBox = new wxStaticBox(m_IOPanel, wxID_ANY, _("Registers"));
    wxStaticBoxSizer* itemStaticBoxSizer54 = new wxStaticBoxSizer(m_RegisterBox, wxVERTICAL);
    itemBoxSizer46->Add(itemStaticBoxSizer54, 0, wxGROW, 5);
    wxFlexGridSizer* itemFlexGridSizer55 = new wxFlexGridSizer(2, 3, 0, 0);
    itemStaticBoxSizer54->Add(itemFlexGridSizer55, 0, wxGROW|wxTOP|wxBOTTOM, 5);
    m_labConfig = new wxStaticText( m_IOPanel, wxID_ANY, _("config"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer55->Add(m_labConfig, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Config = new wxTextCtrl( m_IOPanel, wxID_ANY, _("0"), wxDefaultPosition, wxSize(40, -1), wxTE_READONLY|wxTE_RIGHT );
    m_Config->Enable(false);
    itemFlexGridSizer55->Add(m_Config, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_ConfigHex = new wxTextCtrl( m_IOPanel, wxID_ANY, _("00"), wxDefaultPosition, wxSize(40, -1), wxTE_READONLY|wxTE_RIGHT );
    m_ConfigHex->Enable(false);
    itemFlexGridSizer55->Add(m_ConfigHex, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_labVal1 = new wxStaticText( m_IOPanel, wxID_ANY, _("val1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer55->Add(m_labVal1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Val1 = new wxTextCtrl( m_IOPanel, wxID_ANY, _("0"), wxDefaultPosition, wxSize(40, -1), wxTE_READONLY|wxTE_RIGHT );
    m_Val1->Enable(false);
    itemFlexGridSizer55->Add(m_Val1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Val1Hex = new wxTextCtrl( m_IOPanel, wxID_ANY, _("00"), wxDefaultPosition, wxSize(40, -1), wxTE_READONLY|wxTE_RIGHT );
    m_Val1Hex->Enable(false);
    itemFlexGridSizer55->Add(m_Val1Hex, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_labVal2 = new wxStaticText( m_IOPanel, wxID_ANY, _("val2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer55->Add(m_labVal2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Val2 = new wxTextCtrl( m_IOPanel, wxID_ANY, _("0"), wxDefaultPosition, wxSize(40, -1), wxTE_READONLY|wxTE_RIGHT );
    m_Val2->Enable(false);
    itemFlexGridSizer55->Add(m_Val2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_Val2Hex = new wxTextCtrl( m_IOPanel, wxID_ANY, _("00"), wxDefaultPosition, wxSize(40, -1), wxTE_READONLY|wxTE_RIGHT );
    m_Val2Hex->Enable(false);
    itemFlexGridSizer55->Add(m_Val2Hex, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL, 5);

    m_SensorSetupBox = new wxStaticBox(m_IOPanel, wxID_ANY, _("Input setup"));
    wxStaticBoxSizer* itemStaticBoxSizer65 = new wxStaticBoxSizer(m_SensorSetupBox, wxVERTICAL);
    itemBoxSizer45->Add(itemStaticBoxSizer65, 0, wxGROW, 5);
    m_SensorActiveLow = new wxCheckBox( m_IOPanel, wxID_ANY, _("Active high"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SensorActiveLow->SetValue(false);
    itemStaticBoxSizer65->Add(m_SensorActiveLow, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_SensorDelay = new wxCheckBox( m_IOPanel, wxID_ANY, _("Switch-off delay"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SensorDelay->SetValue(false);
    itemStaticBoxSizer65->Add(m_SensorDelay, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_SensorTurnout = new wxCheckBox( m_IOPanel, wxID_ANY, _("Turnout sensor"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SensorTurnout->SetValue(false);
    itemStaticBoxSizer65->Add(m_SensorTurnout, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    m_SensorPulse = new wxCheckBox( m_IOPanel, wxID_ANY, _("Pulse"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SensorPulse->SetValue(false);
    itemStaticBoxSizer65->Add(m_SensorPulse, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    wxArrayString m_SensorMessageStrings;
    m_SensorMessageStrings.Add(_("&Report"));
    m_SensorMessageStrings.Add(_("&Request"));
    m_SensorMessage = new wxRadioBox( m_IOPanel, wxID_ANY, _("Message"), wxDefaultPosition, wxDefaultSize, m_SensorMessageStrings, 1, wxRA_SPECIFY_ROWS );
    m_SensorMessage->SetSelection(0);
    itemStaticBoxSizer65->Add(m_SensorMessage, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    wxArrayString m_InputTypeStrings;
    m_InputTypeStrings.Add(_("&Sensor"));
    m_InputTypeStrings.Add(_("&Button"));
    m_InputTypeStrings.Add(_("&Switch"));
    m_InputType = new wxRadioBox( m_IOPanel, wxID_ANY, _("Type"), wxDefaultPosition, wxDefaultSize, m_InputTypeStrings, 1, wxRA_SPECIFY_COLS );
    m_InputType->SetSelection(0);
    itemStaticBoxSizer65->Add(m_InputType, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    itemNotebook3->AddPage(m_IOPanel, _("I/O"));

    m_OPCPanel = new wxPanel( itemNotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer73 = new wxBoxSizer(wxVERTICAL);
    m_OPCPanel->SetSizer(itemBoxSizer73);

    wxFlexGridSizer* itemFlexGridSizer74 = new wxFlexGridSizer(2, 4, 0, 0);
    itemBoxSizer73->Add(itemFlexGridSizer74, 0, wxGROW|wxALL, 5);
    m_labOPCPort = new wxStaticText( m_OPCPanel, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer74->Add(m_labOPCPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_OPCPort = new wxSpinCtrl( m_OPCPanel, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 1, 16, 1 );
    itemFlexGridSizer74->Add(m_OPCPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_OPCGet = new wxButton( m_OPCPanel, ID_BUTTON_OPC_GET, _("Get"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer74->Add(m_OPCGet, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_OPCSet = new wxButton( m_OPCPanel, ID_BUTTON_OPC_SET, _("Set"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer74->Add(m_OPCSet, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxFlexGridSizer* itemFlexGridSizer79 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer73->Add(itemFlexGridSizer79, 0, wxGROW|wxALL, 5);
    m_labOPC = new wxStaticText( m_OPCPanel, wxID_ANY, _("Opcode"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer79->Add(m_labOPC, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_OPC = new wxSpinCtrl( m_OPCPanel, wxID_ANY, _T("128"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 128, 255, 128 );
    itemFlexGridSizer79->Add(m_OPC, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labARG1 = new wxStaticText( m_OPCPanel, wxID_ANY, _("Argument 1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer79->Add(m_labARG1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ARG1 = new wxSpinCtrl( m_OPCPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 127, 0 );
    itemFlexGridSizer79->Add(m_ARG1, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labARG2 = new wxStaticText( m_OPCPanel, wxID_ANY, _("Argument 2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer79->Add(m_labARG2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ARG2 = new wxSpinCtrl( m_OPCPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 127, 0 );
    itemFlexGridSizer79->Add(m_ARG2, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook3->AddPage(m_OPCPanel, _("OPC"));

    wxPanel* itemPanel86 = new wxPanel( itemNotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer87 = new wxBoxSizer(wxVERTICAL);
    itemPanel86->SetSizer(itemBoxSizer87);

    wxFlexGridSizer* itemFlexGridSizer88 = new wxFlexGridSizer(2, 4, 0, 0);
    itemBoxSizer87->Add(itemFlexGridSizer88, 0, wxGROW|wxALL, 5);
    m_labServoPort = new wxStaticText( itemPanel86, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer88->Add(m_labServoPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_ServoPort = new wxSpinCtrl( itemPanel86, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 1, 8, 1 );
    itemFlexGridSizer88->Add(m_ServoPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_ServoPortGet = new wxButton( itemPanel86, ID_BUTTON_SERVO_PORT_GET, _("Get"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer88->Add(m_ServoPortGet, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_ServoPortSet = new wxButton( itemPanel86, ID_BUTTON_SERVO_PORT_SET, _("Set"), wxDefaultPosition, wxSize(40, -1), 0 );
    itemFlexGridSizer88->Add(m_ServoPortSet, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_ServoBox = new wxStaticBox(itemPanel86, wxID_ANY, _("Servo"));
    wxStaticBoxSizer* itemStaticBoxSizer93 = new wxStaticBoxSizer(m_ServoBox, wxVERTICAL);
    itemBoxSizer87->Add(itemStaticBoxSizer93, 0, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer94 = new wxFlexGridSizer(2, 2, 0, 0);
    itemStaticBoxSizer93->Add(itemFlexGridSizer94, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    m_labPos1 = new wxStaticText( itemPanel86, wxID_ANY, _("Pos1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer94->Add(m_labPos1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Pos1 = new wxSpinCtrl( itemPanel86, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 1, 127, 1 );
    itemFlexGridSizer94->Add(m_Pos1, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labPos2 = new wxStaticText( itemPanel86, wxID_ANY, _("Pos2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer94->Add(m_labPos2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Pos2 = new wxSpinCtrl( itemPanel86, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 1, 127, 1 );
    itemFlexGridSizer94->Add(m_Pos2, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labV = new wxStaticText( itemPanel86, wxID_ANY, _("V"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer94->Add(m_labV, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_V = new wxSpinCtrl( itemPanel86, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 5, 0 );
    itemFlexGridSizer94->Add(m_V, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    itemNotebook3->AddPage(itemPanel86, _("Servo"));

    m_EasyPanel = new wxPanel( itemNotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer102 = new wxBoxSizer(wxVERTICAL);
    m_EasyPanel->SetSizer(itemBoxSizer102);

    wxFlexGridSizer* itemFlexGridSizer103 = new wxFlexGridSizer(2, 7, 0, 0);
    itemBoxSizer102->Add(itemFlexGridSizer103, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    m_labEasyPort = new wxStaticText( m_EasyPanel, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(m_labEasyPort, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labEasyAddress = new wxStaticText( m_EasyPanel, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(m_labEasyAddress, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labEasyInput = new wxStaticText( m_EasyPanel, wxID_ANY, _("Input"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(m_labEasyInput, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxStaticText* itemStaticText107 = new wxStaticText( m_EasyPanel, wxID_ANY, _("Block"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText107, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labEasyOutput = new wxStaticText( m_EasyPanel, wxID_ANY, _("Switch"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(m_labEasyOutput, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxStaticText* itemStaticText109 = new wxStaticText( m_EasyPanel, wxID_ANY, _("Pulse"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText109, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxStaticText* itemStaticText110 = new wxStaticText( m_EasyPanel, wxID_ANY, _("C2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText110, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxStaticText* itemStaticText111 = new wxStaticText( m_EasyPanel, wxID_ANY, _("1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText111, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr1 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 1 );
    itemFlexGridSizer103->Add(m_EasyAddr1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput1 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput1->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock1 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock1->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput1 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput1->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse1 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse1->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact1 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact1->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText118 = new wxStaticText( m_EasyPanel, wxID_ANY, _("2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText118, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr2 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("2"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 2 );
    itemFlexGridSizer103->Add(m_EasyAddr2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput2 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput2->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock2 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock2->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput2 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput2->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse2 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse2->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact2 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact2->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText125 = new wxStaticText( m_EasyPanel, wxID_ANY, _("3"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText125, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr3 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("3"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 3 );
    itemFlexGridSizer103->Add(m_EasyAddr3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput3 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput3->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock3 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock3->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput3 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput3->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse3 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse3->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact3 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact3->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText132 = new wxStaticText( m_EasyPanel, wxID_ANY, _("4"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText132, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr4 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("4"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 4 );
    itemFlexGridSizer103->Add(m_EasyAddr4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput4 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput4->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock4 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock4->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput4 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput4->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse4 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse4->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact4 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact4->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText139 = new wxStaticText( m_EasyPanel, wxID_ANY, _("5"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText139, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr5 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("5"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 5 );
    itemFlexGridSizer103->Add(m_EasyAddr5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput5 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput5->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock5 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock5->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput5 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput5->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse5 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse5->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact5 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact5->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText146 = new wxStaticText( m_EasyPanel, wxID_ANY, _("6"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText146, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr6 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("6"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 6 );
    itemFlexGridSizer103->Add(m_EasyAddr6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput6 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput6->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock6 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock6->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput6 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput6->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse6 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse6->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact6 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact6->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText153 = new wxStaticText( m_EasyPanel, wxID_ANY, _("7"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText153, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr7 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("7"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 7 );
    itemFlexGridSizer103->Add(m_EasyAddr7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput7 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput7->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock7 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock7->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput7 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput7->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse7 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse7->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact7 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact7->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText160 = new wxStaticText( m_EasyPanel, wxID_ANY, _("8"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText160, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr8 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("8"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 8 );
    itemFlexGridSizer103->Add(m_EasyAddr8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput8 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput8->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock8 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock8->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput8 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput8->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse8 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse8->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact8 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact8->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText167 = new wxStaticText( m_EasyPanel, wxID_ANY, _("9"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText167, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr9 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("9"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 9 );
    itemFlexGridSizer103->Add(m_EasyAddr9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput9 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput9->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock9 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock9->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput9 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput9->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse9 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse9->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact9 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact9->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText174 = new wxStaticText( m_EasyPanel, wxID_ANY, _("10"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText174, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr10 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("10"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 10 );
    itemFlexGridSizer103->Add(m_EasyAddr10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput10 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput10->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock10 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock10->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput10 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput10->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse10 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse10->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact10 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact10->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText181 = new wxStaticText( m_EasyPanel, wxID_ANY, _("11"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText181, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr11 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("11"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 11 );
    itemFlexGridSizer103->Add(m_EasyAddr11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput11 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput11->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock11 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock11->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput11 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput11->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse11 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse11->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact11 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact11->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText188 = new wxStaticText( m_EasyPanel, wxID_ANY, _("12"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText188, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr12 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("12"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 12 );
    itemFlexGridSizer103->Add(m_EasyAddr12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput12 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput12->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock12 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock12->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput12 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput12->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse12 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse12->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact12 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact12->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText195 = new wxStaticText( m_EasyPanel, wxID_ANY, _("13"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText195, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr13 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("13"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 13 );
    itemFlexGridSizer103->Add(m_EasyAddr13, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput13 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput13->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput13, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock13 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock13->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock13, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput13 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput13->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput13, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse13 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse13->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse13, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact13 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact13->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact13, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText202 = new wxStaticText( m_EasyPanel, wxID_ANY, _("14"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText202, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr14 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("14"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 14 );
    itemFlexGridSizer103->Add(m_EasyAddr14, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput14 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput14->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput14, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock14 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock14->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock14, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput14 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput14->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput14, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse14 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse14->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse14, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact14 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact14->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact14, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText209 = new wxStaticText( m_EasyPanel, wxID_ANY, _("15"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText209, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr15 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("15"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 15 );
    itemFlexGridSizer103->Add(m_EasyAddr15, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput15 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput15->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput15, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock15 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock15->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock15, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput15 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput15->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput15, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse15 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse15->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse15, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact15 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact15->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact15, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxStaticText* itemStaticText216 = new wxStaticText( m_EasyPanel, wxID_ANY, _("16"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer103->Add(itemStaticText216, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyAddr16 = new wxSpinCtrl( m_EasyPanel, wxID_ANY, _T("16"), wxDefaultPosition, wxSize(75, -1), wxSP_ARROW_KEYS, 1, 4095, 16 );
    itemFlexGridSizer103->Add(m_EasyAddr16, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyInput16 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, wxRB_GROUP );
    m_EasyInput16->SetValue(true);
    itemFlexGridSizer103->Add(m_EasyInput16, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyBlock16 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyBlock16->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyBlock16, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyOutput16 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyOutput16->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyOutput16, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyPulse16 = new wxRadioButton( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyPulse16->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyPulse16, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_EasyContact16 = new wxCheckBox( m_EasyPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_EasyContact16->SetValue(false);
    itemFlexGridSizer103->Add(m_EasyContact16, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    wxBoxSizer* itemBoxSizer223 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer102->Add(itemBoxSizer223, 0, wxGROW, 5);
    m_EasyGetAll = new wxButton( m_EasyPanel, ID_EASYGETALL, _("Get All"), wxDefaultPosition, wxSize(60, 25), 0 );
    itemBoxSizer223->Add(m_EasyGetAll, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_EasySetAll = new wxButton( m_EasyPanel, ID_EASYSETALL, _("Set All"), wxDefaultPosition, wxSize(60, 25), 0 );
    itemBoxSizer223->Add(m_EasySetAll, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_EasySaveAll = new wxButton( m_EasyPanel, ID_BUTTON_EASY_SAVE, _("Save..."), wxDefaultPosition, wxSize(60, 25), 0 );
    itemBoxSizer223->Add(m_EasySaveAll, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_EasyReadAll = new wxButton( m_EasyPanel, ID_BUTTON_EASY_READ, _("Read..."), wxDefaultPosition, wxSize(60, 25), 0 );
    itemBoxSizer223->Add(m_EasyReadAll, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    m_Booster = new wxCheckBox( m_EasyPanel, ID_LOCOIO_BOOSTER, _("Booster"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Booster->SetValue(false);
    itemBoxSizer223->Add(m_Booster, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook3->AddPage(m_EasyPanel, _("Easy Setup"));

    m_AddressesPanel = new wxPanel( itemNotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer230 = new wxBoxSizer(wxVERTICAL);
    m_AddressesPanel->SetSizer(itemBoxSizer230);

    wxArrayString m_AddressListStrings;
    m_AddressList = new wxListBox( m_AddressesPanel, ID_LN_ADDRESSLIST, wxDefaultPosition, wxDefaultSize, m_AddressListStrings, wxLB_SINGLE|wxLB_ALWAYS_SB );
    itemBoxSizer230->Add(m_AddressList, 1, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer232 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer230->Add(itemBoxSizer232, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    m_QueryAddresses = new wxButton( m_AddressesPanel, ID_QUEURY_ADDRESSES, _("Query"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer232->Add(m_QueryAddresses, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook3->AddPage(m_AddressesPanel, _("Addresses"));

    m_MultiPortPanel = new wxPanel( itemNotebook3, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer235 = new wxBoxSizer(wxVERTICAL);
    m_MultiPortPanel->SetSizer(itemBoxSizer235);

    wxFlexGridSizer* itemFlexGridSizer236 = new wxFlexGridSizer(4, 8, 0, 0);
    itemBoxSizer235->Add(itemFlexGridSizer236, 0, wxGROW|wxALL, 5);
    wxStaticText* itemStaticText237 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText237, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText238 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText238, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText239 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("3"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText239, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText240 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("4"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText240, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText241 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("5"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText241, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText242 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("6"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText242, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText243 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("7"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText243, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText244 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("8"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText244, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_MP1 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP1->SetValue(false);
    itemFlexGridSizer236->Add(m_MP1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP2 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP2->SetValue(false);
    itemFlexGridSizer236->Add(m_MP2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP3 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP3->SetValue(false);
    itemFlexGridSizer236->Add(m_MP3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP4 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP4->SetValue(false);
    itemFlexGridSizer236->Add(m_MP4, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP5 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP5->SetValue(false);
    itemFlexGridSizer236->Add(m_MP5, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP6 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP6->SetValue(false);
    itemFlexGridSizer236->Add(m_MP6, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP7 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP7->SetValue(false);
    itemFlexGridSizer236->Add(m_MP7, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP8 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP8->SetValue(false);
    itemFlexGridSizer236->Add(m_MP8, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxStaticText* itemStaticText253 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("9"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText253, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText254 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("10"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText254, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText255 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("11"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText255, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText256 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("12"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText256, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText257 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("13"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText257, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText258 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("14"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText258, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText259 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("15"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText259, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText260 = new wxStaticText( m_MultiPortPanel, wxID_ANY, _("16"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer236->Add(itemStaticText260, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_MP9 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP9->SetValue(false);
    itemFlexGridSizer236->Add(m_MP9, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP10 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP10->SetValue(false);
    itemFlexGridSizer236->Add(m_MP10, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP11 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP11->SetValue(false);
    itemFlexGridSizer236->Add(m_MP11, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP12 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP12->SetValue(false);
    itemFlexGridSizer236->Add(m_MP12, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP13 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP13->SetValue(false);
    itemFlexGridSizer236->Add(m_MP13, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP14 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP14->SetValue(false);
    itemFlexGridSizer236->Add(m_MP14, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP15 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP15->SetValue(false);
    itemFlexGridSizer236->Add(m_MP15, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    m_MP16 = new wxCheckBox( m_MultiPortPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    m_MP16->SetValue(false);
    itemFlexGridSizer236->Add(m_MP16, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer* itemBoxSizer269 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer235->Add(itemBoxSizer269, 0, wxGROW|wxALL, 5);
    m_MPRead = new wxButton( m_MultiPortPanel, ID_BUTTON_MP_READ, _("Read"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer269->Add(m_MPRead, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_MPWrite = new wxButton( m_MultiPortPanel, ID_BUTTON_MP_WRITE, _("Write"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer269->Add(m_MPWrite, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    itemNotebook3->AddPage(m_MultiPortPanel, _("MultiPort"));

    itemBoxSizer2->Add(itemNotebook3, 1, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer272 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer272, 0, wxALIGN_RIGHT|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Cancel->SetDefault();
    itemStdDialogButtonSizer272->AddButton(m_Cancel);

    itemStdDialogButtonSizer272->Realize();

////@end LocoIO content construction
}


/*!
 * Should we show tooltips?
 */

bool LocoIO::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap LocoIO::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin LocoIO bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end LocoIO bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon LocoIO::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin LocoIO icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end LocoIO icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_GET_GEN
 */

void LocoIO::OnButtonGetGenClick( wxCommandEvent& event )
{
  if( !QueueOp.isEmpty(m_Queue) ) {
    TraceOp.trc( "locoio", TRCLEVEL_WARNING, __LINE__, 9999, "queue not empty; pending operation...");
    return;
  }

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  m_iLowAddress = m_LowAddr->GetValue();
  m_iSubAddress = m_SubAddr->GetValue();
  wProgram.setlntype( cmd, wProgram.lntype_sv );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setaddr( cmd, m_iLowAddress );
  wProgram.setmodid( cmd, m_iSubAddress );
  wProgram.setcv( cmd, 0 );
  wProgram.setvalue( cmd, 0 );
  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "get lnsv program command for %d,%d,%d",
      m_iLowAddress, m_iSubAddress, wProgram.getcv(cmd) );

  QueueOp.post( m_Queue, (obj)cmd, normal );
  cmd = (iONode)NodeOp.base.clone(cmd);
  wProgram.setcv( cmd, 2 );
  QueueOp.post( m_Queue, (obj)cmd, normal );
  sendPacket();
  SetTitle( wxString::Format( _T("%s  %d-%d"), _T("LocoIO"), m_iLowAddress , m_iSubAddress ) );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SET_GEN
 */

void LocoIO::OnButtonSetGenClick( wxCommandEvent& event )
{
  if( !QueueOp.isEmpty(m_Queue) ) {
    TraceOp.trc( "locoio", TRCLEVEL_WARNING, __LINE__, 9999, "queue not empty; pending operation...");
    return;
  }

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  //m_iLowAddress = m_LowAddr->GetValue();
  //m_iSubAddress = m_SubAddr->GetValue();
  wProgram.setlntype( cmd, wProgram.lntype_sv );
  wProgram.setcmd( cmd, wProgram.lncvset );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setaddr( cmd, m_iLowAddress );
  wProgram.setmodid( cmd, m_iSubAddress );
  wProgram.setcv( cmd, 0 );
  wProgram.setvalue( cmd, makeSV0() );
  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "set lnsv program command for %d,%d,%d",
      m_iLowAddress, m_iSubAddress, wProgram.getcv(cmd) );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  if( m_iLowAddress != m_LowAddr->GetValue() ) {
    // low address
    cmd = (iONode)NodeOp.base.clone(cmd);
    wProgram.setcv( cmd, 1 );
    wProgram.setvalue( cmd, m_LowAddr->GetValue() );
    m_iLowAddress = -1;
    QueueOp.post( m_Queue, (obj)cmd, normal );
  }

  if( m_iSubAddress != m_SubAddr->GetValue() ) {
    // sub address
    cmd = (iONode)NodeOp.base.clone(cmd);
    wProgram.setaddr( cmd, m_iLowAddress ); // replace low address with new one
    wProgram.setcv( cmd, 2 );
    wProgram.setvalue( cmd, m_SubAddr->GetValue() );
    m_iSubAddress = -1;
    QueueOp.post( m_Queue, (obj)cmd, normal );
  }

  sendPacket();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PORT_GET
 */

void LocoIO::OnButtonPortSetClick( wxCommandEvent& event )
{
  make3Packet(3, m_Port->GetValue(), makeSVConfigPort(), makeSVParam1Port(), makeSVParam2Port(), true);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PORT_SET
 */

void LocoIO::OnButtonPortGetClick( wxCommandEvent& event )
{
  make3Packet(3, m_Port->GetValue(), 0, 0, 0, false);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void LocoIO::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


void LocoIO::event( iONode event ) {
  int lowaddr  = wProgram.getdecaddr(event);
  int subaddr  = wProgram.getmodid(event);
  int cmd      = wProgram.getcmd(event);
  int type     = wProgram.getlntype(event);
  int cv       = wProgram.getcv (event);
  int val      = wProgram.getvalue(event);
  int ver      = wProgram.getversion(event);
  bool datarsp = false;

  if( cmd == wProgram.datarsp )
    datarsp = true;

  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "lnsv event for addr %d.%d, sv%d=%d ver=%d",
      lowaddr, subaddr, cv, val, ver );
  if( lowaddr == 80 ) {
    TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "not evaluating the LocoBuffer..." );
  }
  else if( (m_iLowAddress == lowaddr && m_iSubAddress == subaddr) || m_iLowAddress == -1 || m_iSubAddress == -1 ) {
    evaluateEvent( type, lowaddr, subaddr, cv, val, ver );
    if( m_SendedCmd != NULL && cv == wProgram.getcv (m_SendedCmd) ) {
      TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "reply matches the sended request");
      NodeOp.base.del(m_SendedCmd);
      m_SendedCmd = NULL;
      ThreadOp.sleep(100);
      sendPacket();
    }
  }

  if( lowaddr != 80 && lowaddr > 0 ) {
    wxString item = wxString::Format(_T("%03d/%03d ver:%d"), lowaddr, subaddr, ver );
    if( !m_AddressList->SetStringSelection(item) )
      m_AddressList->Append( item );
    else {
      const char* product = "LocoIO";
      if( ver < 10 )
        product = "LocoBooster";
      if( ver < 110 && ver >= 100 )
        product = "LocoServo";
      TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "address %03d/%03d ver:%d already in list..,", lowaddr, subaddr, ver );
    }
  }

  /* clean up event node */
  NodeOp.base.del(event);
}

void LocoIO::evaluateEvent( int type, int low, int sub, int sv, int val, int ver ) {
  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "evaluating sv%d=%d", sv, val );

  wxString s; s.Printf( _T("%d"), ver );
  m_Version->SetValue( s );

  if( type == wProgram.lntype_mp ) {
    // evaluate multi port
    m_MP1->SetValue( val & 0x0001 ? true:false );
    m_MP2->SetValue( val & 0x0002 ? true:false );
    m_MP3->SetValue( val & 0x0004 ? true:false );
    m_MP4->SetValue( val & 0x0008 ? true:false );
    m_MP5->SetValue( val & 0x0010 ? true:false );
    m_MP6->SetValue( val & 0x0020 ? true:false );
    m_MP7->SetValue( val & 0x0040 ? true:false );
    m_MP8->SetValue( val & 0x0080 ? true:false );
    m_MP9->SetValue( val & 0x0100 ? true:false );
    m_MP10->SetValue( val & 0x0200 ? true:false );
    m_MP11->SetValue( val & 0x0400 ? true:false );
    m_MP12->SetValue( val & 0x0800 ? true:false );
    m_MP13->SetValue( val & 0x1000 ? true:false );
    m_MP14->SetValue( val & 0x2000 ? true:false );
    m_MP15->SetValue( val & 0x4000 ? true:false );
    m_MP16->SetValue( val & 0x8000 ? true:false );
    return;
  }

  if( sv == 0 ) {
    // setup
    TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "setup" );
    wxString s; s.Printf( _T("%d"), val );
    m_ModConfig->SetValue(s);
    m_PortRefresh->SetValue( val&0x01 ? true:false );
    m_FilpButtonCode->SetValue( val&0x02 ? true:false );
    m_4PosServo->SetValue( val&0x03 ? true:false );
    m_Port_5_12_Servos->SetValue( val&0x04 ? true:false );
    m_FlashFreq->SetValue( (val & 0xF0) >> 4 );
  }
  else if( sv == 1 ) {
    // low address
    TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "low address %d", val );
    m_iLowAddress = val;
    m_LowAddr->SetValue(m_iLowAddress);
    SetTitle( wxString::Format( _T("%s  %d-%d"), _T("LocoIO"), m_iLowAddress , m_iSubAddress ) );
  }
  else if( sv == 2 ) {
    // sub address
    TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "sub address %d", val );
    m_iSubAddress = val;
    m_SubAddr->SetValue(m_iSubAddress);
    SetTitle( wxString::Format( _T("%s  %d-%d"), _T("LocoIO"), m_iLowAddress , m_iSubAddress ) );
  }
  else if( sv >= 3 && sv <= 50 ) {
    // config port
    TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "config port[%d][%d]", sv / 3, (sv-3) % 3 );
    evaluatePort( sv, val );
  }
  else if( sv >= 51 && sv <= 98 ) {
    // config OPC
    TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "config OPC[%d][%d]", (sv-101+3) / 3, (sv-101+3) % 3 );
    evaluateOPC( sv, val );
  }
  else if( sv >= 101 && sv <= 124 ) {
    // config servo
    TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "config servo[%d][%d]", (sv-101+3) / 3, (sv-101+3) % 3 );
    evaluateServo( sv, val );
  }
  else {
    TraceOp.trc( "locoio", TRCLEVEL_WARNING, __LINE__, 9999, "sv%d is not supported", sv );
  }
}


int LocoIO::makeSVConfigPort() {
  int val = 0;

  if( m_PortType->GetSelection() == 0 ) {
    // output
    val |= 0x80;
    val |= m_LowAtStartup->IsChecked() ? 0x01:0x00;
    val |= m_HardwareReset->IsChecked() ? 0x04:0x00;
    val |= m_PulseContact->IsChecked() ? 0x08:0x00;
    val |= m_Flash->IsChecked() ? 0x10:0x00;
    val |= m_Multi->IsChecked() ? 0x20:0x00;
    val |= m_BlockDetector->IsChecked() ? 0x40:0x00;
  }
  else {
    // input
    val = 0x03;
    val |= m_SensorActiveLow->IsChecked() ? 0x40:0x00;
    val |= m_SensorDelay->IsChecked() ? 0x00:0x04;
    val |= m_SensorMessage->GetSelection() == 0 ? 0x00:0x08;
    if( m_InputType->GetSelection() == 0 ) {
      // sensor
      val |= 0x10;
    }
    else {
      if( m_InputType->GetSelection() == 1 ) {
        // button
        val |= 0x20;
      }
    }
  }
  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "Config = %d (0x%02X)", val, val );
  return val;
}


int LocoIO::makeSVParam1Port() {
  int val = 0;
  if( m_PortType->GetSelection() == 0 ) {
    // output
    val = ((m_PortAddr->GetValue() - 1) & 0x7F);
  }
  else {
    // input
    val = ((m_PortAddr->GetValue() - 1) & 0xFF);
    val = val >> 1;
  }
  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "Value-1 = %d (0x%02X)", val, val );
  return val;
}


int LocoIO::makeSVParam2Port() {
  int val = 0;
  if( m_PortType->GetSelection() == 0 ) {
    // output
    val = ((m_PortAddr->GetValue()-1) & 0x0780) >> 7;
    val |= (m_SensorContact->GetSelection()==1)?0x20:0x00;
  }
  else {
    // input
    val = ((m_PortAddr->GetValue()-1) & 0xF00) >> 8;
    val |= ( (m_PortAddr->GetValue()-1)&0x0001 ) ? 0x20:0x00;
    val |= m_SensorTurnout->IsChecked()?0x40:0x00;
  }
  val |= m_SensorPulse->IsChecked()?0x00:0x10;
  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "Value-2 = %d (0x%02X)", val, val );
  return val;
}


int LocoIO::makeSV0() {
  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "make sv0" );
  int sv0 = 0;

  sv0 |= m_PortRefresh->IsChecked()?0x01:0x00;
  sv0 |= m_FilpButtonCode->IsChecked()?0x02:0x00;
  sv0 |= m_4PosServo->IsChecked()?0x04:0x00;
  sv0 |= m_Port_5_12_Servos->IsChecked()?0x08:0x00;
  sv0 |= (m_FlashFreq->GetValue() & 0x0F) << 4;
  return sv0;
}


void LocoIO::evaluateServo( int sv, int val ) {
  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "evaluating SERVO sv%d=%d", sv, val );
  int port = (sv-101+3) / 3;
  int idx = (sv-101+3) % 3;

  if( idx == 0 ) {
    // Pos1
    m_Pos1->SetValue(val);
  }
  else if( idx == 1 ) {
    // Pos2
    m_Pos2->SetValue(val);
  }
  else if( idx == 2 ) {
    // V
    m_V->SetValue(val);
  }
}


void LocoIO::evaluateOPC( int sv, int val ) {
  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "evaluating OPC sv%d=%d", sv, val );
  int port = (sv-51+3) / 3;
  int idx = (sv-51+3) % 3;

  if( idx == 0 ) {
    // opcode
    m_OPC->SetValue(val);
  }
  else if( idx == 1 ) {
    // argument 1
    m_ARG1->SetValue(val);
  }
  else if( idx == 2 ) {
    // argument 2
    m_ARG2->SetValue(val);
  }
}


void LocoIO::evaluatePort( int sv, int val ) {
  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "evaluating IO sv%d=%d", sv, val );
  int port = sv / 3;
  int idx = (sv-3) % 3;
  wxSpinCtrl* easyPort[] = {NULL,m_EasyAddr1,m_EasyAddr2,m_EasyAddr3,m_EasyAddr4,m_EasyAddr5,m_EasyAddr6,
                            m_EasyAddr7,m_EasyAddr8,m_EasyAddr9,m_EasyAddr10,m_EasyAddr11,
                            m_EasyAddr12,m_EasyAddr13,m_EasyAddr14,m_EasyAddr15,m_EasyAddr16};
  wxRadioButton* easyOutput[] = {NULL,m_EasyOutput1,m_EasyOutput2,m_EasyOutput3,m_EasyOutput4,m_EasyOutput5,
                                 m_EasyOutput6,m_EasyOutput7,m_EasyOutput8,m_EasyOutput9,m_EasyOutput10,
                                 m_EasyOutput11,m_EasyOutput12,m_EasyOutput13,m_EasyOutput14,m_EasyOutput15,m_EasyOutput16};
  wxRadioButton* easyPulse[] = {NULL,m_EasyPulse1,m_EasyPulse2,m_EasyPulse3,m_EasyPulse4,m_EasyPulse5,
                                 m_EasyPulse6,m_EasyPulse7,m_EasyPulse8,m_EasyPulse9,m_EasyPulse10,
                                 m_EasyPulse11,m_EasyPulse12,m_EasyPulse13,m_EasyPulse14,m_EasyPulse15,m_EasyPulse16};
  wxRadioButton* easyInput[] = {NULL,m_EasyInput1,m_EasyInput2,m_EasyInput3,m_EasyInput4,m_EasyInput5,
                                 m_EasyInput6,m_EasyInput7,m_EasyInput8,m_EasyInput9,m_EasyInput10,
                                 m_EasyInput11,m_EasyInput12,m_EasyInput13,m_EasyInput14,m_EasyInput15,m_EasyInput16};
  wxRadioButton* easyBlock[] = {NULL,m_EasyBlock1,m_EasyBlock2,m_EasyBlock3,m_EasyBlock4,m_EasyBlock5,
                                 m_EasyBlock6,m_EasyBlock7,m_EasyBlock8,m_EasyBlock9,m_EasyBlock10,
                                 m_EasyBlock11,m_EasyBlock12,m_EasyBlock13,m_EasyBlock14,m_EasyBlock15,m_EasyBlock16};
  wxCheckBox* easyContact[] = {NULL,m_EasyContact1,m_EasyContact2,m_EasyContact3,m_EasyContact4,m_EasyContact5,
                                 m_EasyContact6,m_EasyContact7,m_EasyContact8,m_EasyContact9,m_EasyContact10,
                                 m_EasyContact11,m_EasyContact12,m_EasyContact13,m_EasyContact14,m_EasyContact15,m_EasyContact16};

  m_Port->SetValue(port);

  if( idx == 0 ) {
    // config
    wxString s; s.Printf( _T("%d"), val );
    m_Config->SetValue(s);
    s.Printf( _T("%02X"), val );
    m_ConfigHex->SetValue(s);

    m_PortType->SetSelection( (val&0x80) ? 0:1);

    if( val & 0x80 ) {
      // output
      m_OutputSetupBox->Enable(true);
      m_SensorSetupBox->Enable(false);
      m_SensorContact->Enable(true);
      m_LowAtStartup->SetValue( val & 0x01 ? true:false );
      m_HardwareReset->SetValue( val & 0x04 ? true:false );
      m_PulseContact->SetValue( val & 0x08 ? true:false );
      m_Flash->SetValue( val & 0x10 ? true:false );
      m_Multi->SetValue( val & 0x20 ? true:false );
      m_BlockDetector->SetValue( val & 0x40 ? true:false );

      if( m_PulseContact->IsChecked() )
        easyPulse[port]->SetValue(true);
      else
        easyOutput[port]->SetValue(true);
    }
    else {
      // input
      m_SensorContact->Enable(false);
      if(val&0x10) // sensor
        m_InputType->SetSelection( 0 );
      else
        m_InputType->SetSelection( val&0x20?1:2 );

      m_OutputSetupBox->Enable(false);
      m_SensorSetupBox->Enable(true);
      m_SensorActiveLow->SetValue( val & 0x40 ? true:false );
      m_SensorDelay->SetValue( val & 0x04 ? false:true );
      m_SensorMessage->SetSelection( val&0x08 ? 1:0 );

      if( m_SensorDelay->IsChecked() )
        easyBlock[port]->SetValue(true);
      else
        easyInput[port]->SetValue(true);
    }

  }
  else if( idx == 1 ) {
    // param 1
    wxString s; s.Printf( _T("%d"), val );
    m_Val1->SetValue(s);
    s.Printf( _T("%02X"), val );
    m_Val1Hex->SetValue(s);

    if( m_PortType->GetSelection() == 0 ) // output
      m_PortAddr->SetValue( val + 1 );
    else
      m_PortAddr->SetValue( val << 1 );
  }
  else if( idx == 2 ) {
    // param 2
    wxString s; s.Printf( _T("%d"), val );
    m_Val2->SetValue(s);
    s.Printf( _T("%02X"), val );
    m_Val2Hex->SetValue(s);

    m_SensorTurnout->SetValue( val & 0x40 ? true:false );
    m_SensorPulse->SetValue( val & 0x10 ? false:true );
    m_SensorContact->SetSelection( val&0x20 ? 1:0 );

    int portaddr = m_PortAddr->GetValue();
    if( m_PortType->GetSelection() == 0 ) {
      // output address
      portaddr += (int)(val&0x0F) << 7;
      m_PortAddr->SetValue( portaddr );

      bool c2 = val&0x20 ? true:false;
      if( m_PulseContact->IsChecked() )
        c2 = !c2;
      easyContact[port]->SetValue(c2);
    }
    else {
      // input address
      portaddr += (int)(val&0x0F) << 8;
      m_PortAddr->SetValue( portaddr + 1 + ((val&0x20) >> 5));
      easyContact[port]->SetValue(false);
    }
    easyPort[port]->SetValue(m_PortAddr->GetValue());

  }

}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PORT_GET
 */

void LocoIO::OnButtonServoPortGetClick( wxCommandEvent& event )
{
  make3Packet(101, m_ServoPort->GetValue(), 0, 0, 0, false);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_PORT_SET
 */

void LocoIO::OnButtonServoPortSetClick( wxCommandEvent& event )
{
  make3Packet(101, m_ServoPort->GetValue(), m_Pos1->GetValue(), m_Pos2->GetValue(), m_V->GetValue(), true);
}



/*!
 * wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_LOCOIO_TYPE
 */

void LocoIO::OnLocoioTypeSelected( wxCommandEvent& event )
{
  if( m_PortType->GetSelection() == 0 ) {
    m_OutputSetupBox->Enable(true);
    m_SensorSetupBox->Enable(false);
    m_SensorContact->Enable(true);
  }
  else {
    m_OutputSetupBox->Enable(false);
    m_SensorSetupBox->Enable(true);
    m_SensorContact->Enable(false);
  }
}


void LocoIO::OnButtonOpcGetClick( wxCommandEvent& event )
{
  make3Packet(51, m_OPCPort->GetValue(), 0, 0, 0, false);
}


void LocoIO::OnButtonOpcSetClick( wxCommandEvent& event )
{
  make3Packet(51, m_OPCPort->GetValue(), m_OPC->GetValue(), m_ARG1->GetValue(), m_ARG2->GetValue(), true);
}


void LocoIO::make3Packet( int sv_offset, int port, int val1, int val2, int val3, bool sv_set, bool sendnow )
{
  if( sendnow && !QueueOp.isEmpty(m_Queue) ) {
    TraceOp.trc( "locoio", TRCLEVEL_WARNING, __LINE__, 9999, "queue not empty; pending operation...");
    return;
  }

  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "%s lnsv program command for %d/%d %d[%d]",
      sv_set?"set":"get", m_iLowAddress, m_iSubAddress, port, sv_offset );

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  m_iLowAddress = m_LowAddr->GetValue();
  m_iSubAddress = m_SubAddr->GetValue();

  wProgram.setlntype( cmd, wProgram.lntype_sv );
  wProgram.setcmd( cmd, sv_set ? wProgram.lncvset:wProgram.lncvget );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setaddr( cmd, m_iLowAddress );
  wProgram.setmodid( cmd, m_iSubAddress );
  wProgram.setcv( cmd, (port-1) * 3 + sv_offset );
  wProgram.setvalue( cmd, val1 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  cmd = (iONode)NodeOp.base.clone(cmd);
  wProgram.setcv( cmd, (port-1) * 3 + sv_offset+1 );
  wProgram.setvalue( cmd, val2 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  cmd = (iONode)NodeOp.base.clone(cmd);
  wProgram.setcv( cmd, (port-1) * 3 + sv_offset+2 );
  wProgram.setvalue( cmd, val3 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  if( sendnow )
    sendPacket();

}


void LocoIO::queryAddresses()
{
  if( !QueueOp.isEmpty(m_Queue) ) {
    TraceOp.trc( "locoio", TRCLEVEL_WARNING, __LINE__, 9999, "queue not empty; pending operation...");
    return;
  }

  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "%s lnsv program command for %d/%d",
      "get", 0, 1 );

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );

  wProgram.setlntype( cmd, wProgram.lntype_sv );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setaddr( cmd, 0 );
  wProgram.setmodid( cmd, 0 );
  wProgram.setcv( cmd, 0 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  sendPacket();

}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_EASYGETALL
 */

void LocoIO::OnEasygetallClick( wxCommandEvent& event )
{
  m_Booster->SetValue(false);
  wxCommandEvent cbevent( wxEVT_COMMAND_CHECKBOX_CLICKED, ID_LOCOIO_BOOSTER );
  wxPostEvent( this, cbevent );

  for( int i = 1; i <= 16; i++ ) {
    make3Packet(3, i, 0, 0, 0, false, false);
  }
  sendPacket();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_EASYSETALL
 */

void LocoIO::easyGetPort( int port, int* conf, int* val1, int* val2 )
{
  wxSpinCtrl* easyPort[] = {NULL,m_EasyAddr1,m_EasyAddr2,m_EasyAddr3,m_EasyAddr4,m_EasyAddr5,m_EasyAddr6,
                            m_EasyAddr7,m_EasyAddr8,m_EasyAddr9,m_EasyAddr10,m_EasyAddr11,
                            m_EasyAddr12,m_EasyAddr13,m_EasyAddr14,m_EasyAddr15,m_EasyAddr16};
  wxRadioButton* easyOutput[] = {NULL,m_EasyOutput1,m_EasyOutput2,m_EasyOutput3,m_EasyOutput4,m_EasyOutput5,
                                 m_EasyOutput6,m_EasyOutput7,m_EasyOutput8,m_EasyOutput9,m_EasyOutput10,
                                 m_EasyOutput11,m_EasyOutput12,m_EasyOutput13,m_EasyOutput14,m_EasyOutput15,m_EasyOutput16};
  wxRadioButton* easyPulse[] = {NULL,m_EasyPulse1,m_EasyPulse2,m_EasyPulse3,m_EasyPulse4,m_EasyPulse5,
                                 m_EasyPulse6,m_EasyPulse7,m_EasyPulse8,m_EasyPulse9,m_EasyPulse10,
                                 m_EasyPulse11,m_EasyPulse12,m_EasyPulse13,m_EasyPulse14,m_EasyPulse15,m_EasyPulse16};
  wxRadioButton* easyInput[] = {NULL,m_EasyInput1,m_EasyInput2,m_EasyInput3,m_EasyInput4,m_EasyInput5,
                                 m_EasyInput6,m_EasyInput7,m_EasyInput8,m_EasyInput9,m_EasyInput10,
                                 m_EasyInput11,m_EasyInput12,m_EasyInput13,m_EasyInput14,m_EasyInput15,m_EasyInput16};
  wxRadioButton* easyBlock[] = {NULL,m_EasyBlock1,m_EasyBlock2,m_EasyBlock3,m_EasyBlock4,m_EasyBlock5,
                                 m_EasyBlock6,m_EasyBlock7,m_EasyBlock8,m_EasyBlock9,m_EasyBlock10,
                                 m_EasyBlock11,m_EasyBlock12,m_EasyBlock13,m_EasyBlock14,m_EasyBlock15,m_EasyBlock16};
  wxCheckBox* easyContact[] = {NULL,m_EasyContact1,m_EasyContact2,m_EasyContact3,m_EasyContact4,m_EasyContact5,
                                 m_EasyContact6,m_EasyContact7,m_EasyContact8,m_EasyContact9,m_EasyContact10,
                                 m_EasyContact11,m_EasyContact12,m_EasyContact13,m_EasyContact14,m_EasyContact15,m_EasyContact16};

  int addr = easyPort[port]->GetValue() - 1;
  int config = 0;
  int value1 = 0;
  int value2 = 0;
  if( easyOutput[port]->GetValue() ) {
    // output
    config = 129;
    value1 = (addr & 0x007F);
    value2 = (addr & 0x0780) >> 7;
    value2 |= 0x10;
    value2 |= easyContact[port]->IsChecked() ? 0x20:0x00;
  }
  else if( easyPulse[port]->GetValue() ) {
    // pulse
    config = 140;
    value1 = (addr & 0x007F);
    value2 = (addr & 0x0780) >> 7;
    value2 |= easyContact[port]->IsChecked() ? 0x00:0x20;
  }
  else if( easyInput[port]->GetValue() ) {
    // input
    config = 31;
    value1 = addr & 0xFF;
    value1 = value1 >> 1;
    value2 = (addr & 0xF00) >> 8;
    value2 |= ( addr&0x0001 ) ? 0x20:0x00;
    value2 |= 0x10;
  }
  else if( easyBlock[port]->GetValue() ) {
    // block
    config = 27;
    value1 = addr & 0xFF;
    value1 = value1 >> 1;
    value2 = (addr & 0xF00) >> 8;
    value2 |= ( addr&0x0001 ) ? 0x20:0x00;
    value2 |= 0x10;
  }

  *conf = config;
  *val1 = value1;
  *val2 = value2;
}


void LocoIO::OnEasysetallClick( wxCommandEvent& event )
{

  for( int i = 1; i <= 16; i++ ) {
    int config = 0;
    int value1 = 0;
    int value2 = 0;
    easyGetPort( i, &config, &value1, &value2 );
    make3Packet(3, i, config, value1, value2, true, false);
  }
  sendPacket();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_RESET_LOCOIO
 */

void LocoIO::OnButtonResetLocoioClick( wxCommandEvent& event )
{
  if( !QueueOp.isEmpty(m_Queue) ) {
    TraceOp.trc( "locoio", TRCLEVEL_WARNING, __LINE__, 9999, "queue not empty; pending operation...");
    return;
  }

  int action = wxMessageDialog( this, _T("This action will reset **ALL** connected LocoIO based modules to the address 81/1."),
      _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION | wxNO_DEFAULT ).ShowModal();
  if( action == wxID_NO ) {
    return;
  }

  /*
    Init Action:

    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |
    --------------------------------------------------------- |
    00000000: E5 10 50 00 01 00 01 00 00 00 00 00 00 00 00 5A | set sv 0 to 0 for LocoIO address 0/1

    00000000: E5 10 51 50 01 04 01 00 14 7B 00 01 00 00 00 61 |

    00000000: E5 10 50 00 01 00 01 01 00 51 00 00 00 00 00 0A | set sv 1 to 0x51 (81) for LocoIO address 0/1

    00000000: E5 10 51 50 01 04 01 01 14 7B 00 01 00 00 51 31 |

    00000000: E5 10 50 00 01 00 01 02 00 01 00 00 00 00 00 59 | set sv 2 to 0x01 (1) for LocoIO address 0/1

    00000000: E5 10 51 50 01 04 01 02 14 7B 00 01 00 00 01 62 |

    00000000: E5 10 50 51 01 00 02 00 00 00 00 01 00 00 00 09 | get sv 0 for LocoIO 81/1

    00000000: E5 10 51 50 01 04 02 00 14 7B 00 01 00 51 01 32 |
  */

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_sv );
  wProgram.setcmd( cmd, wProgram.lncvset );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setaddr( cmd, 0 );
  wProgram.setmodid( cmd, 1 );
  wProgram.setcv( cmd, 0 );
  wProgram.setvalue( cmd, 0 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  cmd = (iONode)NodeOp.base.clone(cmd);
  wProgram.setcv( cmd, 1 );
  wProgram.setvalue( cmd, 81 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  cmd = (iONode)NodeOp.base.clone(cmd);
  wProgram.setcv( cmd, 2 );
  wProgram.setvalue( cmd, 1 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  m_iLowAddress = -1;
  m_iLowAddress = -1;


  sendPacket();


}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_QUEURY_ADDRESSES
 */

void LocoIO::OnQueuryAddressesClick( wxCommandEvent& event )
{
  // Clear list and query
  m_AddressList->Clear();
  queryAddresses();
}




/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_EASY_READ
 */

void LocoIO::OnButtonEasyReadClick( wxCommandEvent& event )
{
  wxString ms_FileExt = _T("LocoIO (*.SV)|*.SV");
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("openlocoiofile"), wxString(l_openpath,wxConvUTF8) , _T(""), ms_FileExt, wxOPEN);
  if( fdlg->ShowModal() == wxID_OK ) {
    //fdlg->GetPath();
    wGui.setopenpath( wxGetApp().getIni(), fdlg->GetPath().mb_str(wxConvUTF8) );
    // strip filename:
    wGui.setopenpath( wxGetApp().getIni(), FileOp.getPath(wGui.getopenpath( wxGetApp().getIni() ) ) );

    TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "reading [%s]...", (const char*)fdlg->GetPath().mb_str(wxConvUTF8));
    iOFile f = FileOp.inst( fdlg->GetPath().mb_str(wxConvUTF8), OPEN_READONLY );
    if( f != NULL ) {
      TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "file opened...");
      FILE* fs = FileOp.getStream(f);
      char str[32];
      fgets( str, 32, fs );

      /* until end of stream or error: */
      while( !ferror(fs) && !feof(fs) ) {
        TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "line=[%s]", str);
        int sv  = 0;
        int val = 0;
        iOStrTok strtok = StrTokOp.inst( str, ',' );
        if( StrTokOp.hasMoreTokens( strtok ) ) {
          const char* ssv  = StrTokOp.nextToken( strtok );
          if( StrTokOp.hasMoreTokens( strtok ) ) {
            const char* sval = StrTokOp.nextToken( strtok );
            sv = atoi( ssv );
            val = atoi( sval );
            m_SV[sv] = val;
            // call event method...
            iONode event = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
            wProgram.setdecaddr(event, m_SV[1]);
            wProgram.setmodid(event, m_SV[2]);
            wProgram.setcmd(event, wProgram.lncvget);
            wProgram.setcv (event, sv);
            wProgram.setvalue(event, val);
            wProgram.setversion(event, m_SV[100]);
            this->event(event);

            TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "read sv=%d val=%d", sv, val);
          }
        }
        StrTokOp.base.del( strtok );

        fgets( str, 32, fs );
      };

      FileOp.base.del( f );
    }
  }
  fdlg->Destroy();
}



/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_EASY_SAVE
 */

void LocoIO::OnButtonEasySaveClick( wxCommandEvent& event )
{
  const char* l_openpath = wGui.getopenpath( wxGetApp().getIni() );
  wxString ms_FileExt = _T("LocoIO (*.SV)|*.SV");
  wxFileDialog* fdlg = new wxFileDialog(this, wxGetApp().getMenu("savelocoiofileas"), wxString(l_openpath,wxConvUTF8),
                       wxString::Format( _T("Lio%03d%03d.SV"), m_LowAddr->GetValue(), m_SubAddr->GetValue()), ms_FileExt, wxSAVE);
  if( fdlg->ShowModal() == wxID_OK ) {
    iONode model = wxGetApp().getModel();
    // Check for existence.
    wxString path = fdlg->GetPath();
    if( FileOp.exist( path.mb_str(wxConvUTF8) ) ) {
      int action = wxMessageDialog( this, wxGetApp().getMsg("fileexistwarning"), _T("Rocrail"), wxYES_NO | wxICON_EXCLAMATION ).ShowModal();
      if( action == wxID_NO ) {
        fdlg->Destroy();
        return;
      }
    }
    if( !path.Contains( _T(".SV") ) )
      path.Append( _T(".SV") );

    iOFile f = FileOp.inst( path.mb_str(wxConvUTF8), OPEN_WRITE );
    if( f != NULL ) {
      char s[32];
      // sv 100 is version: undocumented
      StrOp.fmtb( s, " %d, %s\r\n", 100, (const char*)m_Version->GetValue().mb_str(wxConvUTF8) );
      FileOp.writeStr( f, s );
      StrOp.fmtb( s, " 0, %d\r\n", makeSV0() );
      FileOp.writeStr( f, s );
      StrOp.fmtb( s, " 1, %d\r\n", m_LowAddr->GetValue() );
      FileOp.writeStr( f, s );
      StrOp.fmtb( s, " 2, %d\r\n", m_SubAddr->GetValue() );
      FileOp.writeStr( f, s );

      for( int i = 1; i <= 16; i++ ) {
        // iterate all easy ports
        int config = 0;
        int value1 = 0;
        int value2 = 0;
        easyGetPort( i, &config, &value1, &value2 );

        StrOp.fmtb( s, " %d, %d\r\n", i * 3 + 0, config );
        FileOp.writeStr( f, s );
        StrOp.fmtb( s, " %d, %d\r\n", i * 3 + 1, value1 );
        FileOp.writeStr( f, s );
        StrOp.fmtb( s, " %d, %d\r\n", i * 3 + 2, value2 );
        FileOp.writeStr( f, s );
      }

      FileOp.base.del( f );
    }
  }
  fdlg->Destroy();
}


/*!
 * wxEVT_SCROLL_LINEUP event handler for wxID_ANY
 */

void LocoIO::OnAnyUp( wxSpinEvent& spinevent )
{
  int sv = m_Port->GetValue() * 3;
  iONode event = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setdecaddr(event, m_SV[1]);
  wProgram.setmodid(event, m_SV[2]);
  wProgram.setcmd(event, wProgram.lncvget);
  wProgram.setcv (event, sv);
  wProgram.setvalue(event, m_SV[sv]);
  wProgram.setversion(event, m_SV[100]);
  this->event(event);
  sv++;
  event = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setdecaddr(event, m_SV[1]);
  wProgram.setmodid(event, m_SV[2]);
  wProgram.setcmd(event, wProgram.lncvget);
  wProgram.setcv (event, sv);
  wProgram.setvalue(event, m_SV[sv]);
  wProgram.setversion(event, m_SV[100]);
  this->event(event);
  sv++;
  event = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setdecaddr(event, m_SV[1]);
  wProgram.setmodid(event, m_SV[2]);
  wProgram.setcmd(event, wProgram.lncvget);
  wProgram.setcv (event, sv);
  wProgram.setvalue(event, m_SV[sv]);
  wProgram.setversion(event, m_SV[100]);
  this->event(event);
}


/*!
 * wxEVT_SCROLL_LINEDOWN event handler for wxID_ANY
 */

void LocoIO::OnAnyDown( wxSpinEvent& event )
{
  OnAnyUp(event);
}


/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for wxID_ANY
 */

void LocoIO::OnAnyUpdated( wxSpinEvent& event )
{
  OnAnyUp(event);
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_MP_READ
 */

void LocoIO::OnButtonMpReadClick( wxCommandEvent& event )
{
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  m_iLowAddress = m_LowAddr->GetValue();
  m_iSubAddress = m_SubAddr->GetValue();

  wProgram.setlntype( cmd, wProgram.lntype_mp );
  wProgram.setcmd( cmd, wProgram.lncvget );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setaddr( cmd, m_iLowAddress );
  wProgram.setmodid( cmd, m_iSubAddress );
  wProgram.setcv( cmd, 0xFFFF );
  wProgram.setvalue( cmd, 0 );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  sendPacket();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_MP_WRITE
 */

void LocoIO::OnButtonMpWriteClick( wxCommandEvent& event )
{
  int val = 0;

  val |= m_MP1->IsChecked() ? 0x0001:0;
  val |= m_MP2->IsChecked() ? 0x0002:0;
  val |= m_MP3->IsChecked() ? 0x0004:0;
  val |= m_MP4->IsChecked() ? 0x0008:0;
  val |= m_MP5->IsChecked() ? 0x0010:0;
  val |= m_MP6->IsChecked() ? 0x0020:0;
  val |= m_MP7->IsChecked() ? 0x0040:0;
  val |= m_MP8->IsChecked() ? 0x0080:0;
  val |= m_MP9->IsChecked() ? 0x0100:0;
  val |= m_MP10->IsChecked() ? 0x0200:0;
  val |= m_MP11->IsChecked() ? 0x0400:0;
  val |= m_MP12->IsChecked() ? 0x0800:0;
  val |= m_MP13->IsChecked() ? 0x1000:0;
  val |= m_MP14->IsChecked() ? 0x2000:0;
  val |= m_MP15->IsChecked() ? 0x4000:0;
  val |= m_MP16->IsChecked() ? 0x8000:0;

  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  m_iLowAddress = m_LowAddr->GetValue();
  m_iSubAddress = m_SubAddr->GetValue();

  wProgram.setlntype( cmd, wProgram.lntype_mp );
  wProgram.setcmd( cmd, wProgram.lncvset );
  wProgram.setiid( cmd, m_IID->GetValue().mb_str(wxConvUTF8) );
  wProgram.setaddr( cmd, m_iLowAddress );
  wProgram.setmodid( cmd, m_iSubAddress );
  wProgram.setcv( cmd, 0xFFFF );
  wProgram.setvalue( cmd, val );
  QueueOp.post( m_Queue, (obj)cmd, normal );

  sendPacket();
}


/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for wxID_LOCOIO_LOW_ADDR
 */

void LocoIO::OnLocoioLowAddrUpdated( wxSpinEvent& event )
{
  //m_iLowAddress = m_LowAddr->GetValue();
  //m_iSubAddress = m_SubAddr->GetValue();
  SetTitle( wxString::Format( _T("%s  %d-%d"), _T("LocoIO"), m_iLowAddress , m_iSubAddress ) );
}


/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for wxID_LOCOIO_SUB_ADDR
 */

void LocoIO::OnLocoioSubAddrUpdated( wxSpinEvent& event )
{
  //m_iLowAddress = m_LowAddr->GetValue();
  //m_iSubAddress = m_SubAddr->GetValue();
  SetTitle( wxString::Format( _T("%s  %d-%d"), _T("LocoIO"), m_iLowAddress , m_iSubAddress ) );
}


/*!
 * wxEVT_COMMAND_LISTBOX_DOUBLECLICKED event handler for ID_LN_ADDRESSLIST
 */

void LocoIO::OnLnAddresslistDoubleClicked( wxCommandEvent& event )
{
  if( m_AddressList->GetSelection() == wxNOT_FOUND )
    return;

  // "%03d/%03d ver:%d"
  char* s = StrOp.dup(m_AddressList->GetStringSelection().mb_str(wxConvUTF8));
  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999, "Selected LocoIO = [%s]", s );
  // 000/000
  s[3] = '\0';
  s[7] = '\0';
  m_LowAddr->SetValue( atoi(s) );
  m_SubAddr->SetValue( atoi(s+4) );
  StrOp.free(s);

  m_iLowAddress = m_LowAddr->GetValue();
  m_iSubAddress = m_SubAddr->GetValue();
  SetTitle( wxString::Format( _T("%s  %d-%d"), _T("LocoIO"), m_iLowAddress , m_iSubAddress ) );
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_LOCOIO_BOOSTER
 */

void LocoIO::OnBoosterClick( wxCommandEvent& event )
{
  if( m_Booster->IsChecked() ) {
    m_EasyBlock14->SetValue(true);
    m_EasyPulse15->SetValue(true);
    m_EasyOutput16->SetValue(true);
    m_EasyContact16->SetValue(true);

    m_EasyInput14->Enable(false);
    m_EasyPulse14->Enable(false);
    m_EasyOutput14->Enable(false);
    m_EasyContact14->Enable(false);

    m_EasyInput15->Enable(false);
    m_EasyBlock15->Enable(false);
    m_EasyOutput15->Enable(false);
    m_EasyContact15->Enable(false);

    m_EasyInput16->Enable(false);
    m_EasyPulse16->Enable(false);
    m_EasyBlock16->Enable(false);
    m_EasyContact16->Enable(false);

  }
  else {
    m_EasyInput14->Enable(true);
    m_EasyPulse14->Enable(true);
    m_EasyOutput14->Enable(true);
    m_EasyContact14->Enable(true);

    m_EasyInput15->Enable(true);
    m_EasyBlock15->Enable(true);
    m_EasyOutput15->Enable(true);
    m_EasyContact15->Enable(true);

    m_EasyInput16->Enable(true);
    m_EasyPulse16->Enable(true);
    m_EasyBlock16->Enable(true);
    m_EasyContact16->Enable(true);
  }

}

