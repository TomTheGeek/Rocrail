/////////////////////////////////////////////////////////////////////////////
// Name:        opendcc.cpp
// Purpose:
// Author:
// Modified by:
// Created:     Sun 02 Mar 2008 11:51:45 CET
// RCS-ID:
// Copyright:
// Licence:
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "opendcc.h"
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

#include "opendcc.h"

////@begin XPM images
////@end XPM images
#include "rocgui/public/guiapp.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Program.h"



/*!
 * OpenDCCCtrlDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( OpenDCCCtrlDlg, wxDialog )


/*!
 * OpenDCCCtrlDlg event table definition
 */

BEGIN_EVENT_TABLE( OpenDCCCtrlDlg, wxDialog )

////@begin OpenDCCCtrlDlg event table entries
    EVT_BUTTON( ID_READCVS, OpenDCCCtrlDlg::OnReadcvsClick )

    EVT_BUTTON( ID_WRITECVS, OpenDCCCtrlDlg::OnWritecvsClick )

    EVT_BUTTON( wxID_OK, OpenDCCCtrlDlg::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, OpenDCCCtrlDlg::OnCancelClick )

    EVT_BUTTON( wxID_APPLY, OpenDCCCtrlDlg::OnApplyClick )

////@end OpenDCCCtrlDlg event table entries
    EVT_MENU( PT_EVENT, OpenDCCCtrlDlg::OnPTEvent)
    EVT_TIMER (ME_SOTimer, OpenDCCCtrlDlg::OnTimer)

END_EVENT_TABLE()


/*!
 * OpenDCCCtrlDlg constructors
 */

OpenDCCCtrlDlg::OpenDCCCtrlDlg()
{
    Init();
}

OpenDCCCtrlDlg::OpenDCCCtrlDlg( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create(parent, id, caption, pos, size, style);
}

OpenDCCCtrlDlg::OpenDCCCtrlDlg( wxWindow* parent, iONode props )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Init();
  Create(parent, -1, _T("OpenDCC"));
  m_Timer = new wxTimer( this, ME_SOTimer );
  m_Progress = NULL;
  m_bCleanUpProgress = false;
  m_bStartUpProgress = false;
  m_Props = props;
  MemOp.set( m_soValue, 0, sizeof(m_soValue) );


  initLabels();
  initValues();

  m_Notebook->SetAutoLayout(true);

  m_DecExceptionBox->Layout();

  m_GeneralPanel->GetSizer()->Layout();
  m_DecoderPanel->GetSizer()->Layout();
  m_S88Panel->GetSizer()->Layout();
  m_ProgrammingTab->GetSizer()->Layout();
  m_SCDPanel->GetSizer()->Layout();
  m_SwitchPanel->GetSizer()->Layout();

  m_GeneralPanel->Fit();
  m_DecoderPanel->Fit();
  m_S88Panel->Fit();
  m_ProgrammingTab->Fit();
  m_SCDPanel->Fit();
  m_SwitchPanel->Fit();

  m_Notebook->Layout();
  m_Notebook->Fit();

  //m_Notebook->FitInside();

  GetSizer()->Layout();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  this->Layout();
  this->Fit();

  m_DecoderPanel->Enable(false);
  m_S88Panel->Enable(false);
  m_ProgrammingTab->Enable(false);
  m_SCDPanel->Enable(false);
  m_SwitchPanel->Enable(false);

}

void OpenDCCCtrlDlg::startProgress() {
  if( m_Progress == NULL ) {
    m_Progress = new wxProgressDialog(wxGetApp().getMsg( "soget" ), wxGetApp().getMsg( "waitforso" ),
        1000, NULL, wxPD_CAN_ABORT | wxPD_AUTO_HIDE | wxPD_APP_MODAL );
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "bringing up the progress dialog...0x%08X", m_Progress);
    bool rc = m_Timer->Start( 100, wxTIMER_ONE_SHOT );
    m_Progress->ShowModal();
  }
}

void OpenDCCCtrlDlg::stopProgress() {
  if( m_Progress != NULL ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "end progress dialog" );
    m_bCleanUpProgress = true;
    m_Timer->Start( 10, wxTIMER_ONE_SHOT );
  }
}

void OpenDCCCtrlDlg::OnTimer(wxTimerEvent& event) {
  if( m_bStartUpProgress ) {
    m_bStartUpProgress = false;
    m_TimerCount = 0;
    sendGet(so_version);
    startProgress();
    return;
  }


  if( m_bCleanUpProgress ) {
    m_bCleanUpProgress = false;
    m_Progress->Destroy();
    m_Progress = NULL;
    return;
  }

  m_TimerCount++;
  if( m_TimerCount >= 1000 ) {
    TraceOp.trc( "opendcc", TRCLEVEL_WARNING, __LINE__, 9999, "timeout on SO acknowledge" );
    stopProgress();
  }
  else {
    if( m_Progress != NULL && !m_Progress->Pulse() ) {
      stopProgress();
      m_TimerCount = 1000;
    }
    else {
      TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "timer for SO acknowledge" );
      bool rc = m_Timer->Start( 100, wxTIMER_ONE_SHOT );
    }
  }
}


void OpenDCCCtrlDlg::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "decoders" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "s88" ) );
  m_Notebook->SetPageText( 3, wxGetApp().getMsg( "pt" ) );
  m_Notebook->SetPageText( 4, wxGetApp().getMsg( "shortcutchecking" ) );
  m_Notebook->SetPageText( 5, wxGetApp().getMsg( "switches" ) );

  // General
  m_labVersion->SetLabel( wxGetApp().getMsg( "version" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "device" ) );
  m_Version->SetToolTip( wxGetApp().getTip( "version" ) );
  m_Version->SetEditable(false);
  m_Baudrate->SetLabel( wxGetApp().getMsg( "bps" ) );
  m_Baudrate->SetToolTip( wxGetApp().getTip( "bps" ) );

  // Decoders
  m_DecExceptionBox->SetLabel( wxGetApp().getMsg( "exceptions" ) );
  m_DecExceptions->InsertColumn(0, wxGetApp().getMsg( "address" ) );
  m_DecExceptions->InsertColumn(1, wxGetApp().getMsg( "dccformat" ) );
  m_DecSpeedSteps->SetLabel( wxGetApp().getMsg( "speedsteps" ) );
  m_DecSpeedSteps->SetToolTip( wxGetApp().getTip( "speedsteps" ) );

  // S88
  m_labS88Timing->SetLabel( wxGetApp().getMsg( "timing" ) );
  m_S88Timing->SetToolTip( wxGetApp().getTip( "timing" ) );

  m_S88BusBox->SetLabel( wxGetApp().getMsg( "busses" ) );
  m_labS88Bus1->SetLabel( wxGetApp().getMsg( "bus" ) + _T(" 1") );
  m_S88Bus1->SetToolTip( wxGetApp().getTip( "bus" ) + _T(" 1") );
  m_labS88Bus2->SetLabel( wxGetApp().getMsg( "bus" ) + _T(" 2") );
  m_S88Bus2->SetToolTip( wxGetApp().getTip( "bus" ) + _T(" 2") );
  m_labS88Bus3->SetLabel( wxGetApp().getMsg( "bus" ) + _T(" 3") );
  m_S88Bus3->SetToolTip( wxGetApp().getTip( "bus" ) + _T(" 3") );

  m_S88SettingsBox->SetLabel( wxGetApp().getMsg( "mode" ) );
  m_S88Active->SetLabel( wxGetApp().getMsg( "activate" ) );
  m_S88Active->SetToolTip( wxGetApp().getTip( "activate" ) );
  m_S88TurnoutSensors->SetLabel( wxGetApp().getMsg( "swsensors" ) );
  m_S88TurnoutSensors->SetToolTip( wxGetApp().getTip( "swsensors" ) );

  // PT
  m_labPTResets->SetLabel( wxGetApp().getMsg( "ptreset" ) );
  m_PTResets->SetToolTip( wxGetApp().getTip( "ptreset" ) );
  m_labPTCommands->SetLabel( wxGetApp().getMsg( "ptcommands" ) );
  m_PTCommands->SetToolTip( wxGetApp().getTip( "ptcommands" ) );
  m_labPTPOMrepeat->SetLabel( wxGetApp().getMsg( "ptpom" ) );
  m_PTPOMrepeat->SetToolTip( wxGetApp().getTip( "ptpom" ) );

  // SCD
  m_LabSCtimeMain->SetLabel( wxGetApp().getMsg( "scdmain" ) );
  m_SCTimeMain->SetToolTip( wxGetApp().getTip( "scdmain" ) );
  m_labSCTimePT->SetLabel( wxGetApp().getMsg( "scdpt" ) );
  m_SCTimePT->SetToolTip( wxGetApp().getTip( "scdpt" ) );

  // Switches
  m_SwitchInvert->SetLabel( wxGetApp().getMsg( "swinvert" ) );
  m_SwitchInvert->SetToolTip( wxGetApp().getTip( "swinvert" ) );
  m_labSwitchCommands->SetLabel( wxGetApp().getMsg( "swcommands" ) );
  m_SwitchCommands->SetToolTip( wxGetApp().getTip( "swcommands" ) );
  m_labSwitchTime->SetLabel( wxGetApp().getMsg( "switchtime" ) );
  m_SwitchTime->SetToolTip( wxGetApp().getTip( "switchtime" ) );
  m_labSwitchSensorOffset->SetLabel( wxGetApp().getMsg( "swsensoroffset" ) );
  m_SwitchSensorOffset->SetToolTip( wxGetApp().getTip( "swsensoroffset" ) );
}

void OpenDCCCtrlDlg::evaluate() {
  TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "evaluate" );

  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.setdevice( m_Props, m_Device->GetValue().mb_str(wxConvUTF8) );

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
}

void OpenDCCCtrlDlg::initValues() {
  m_IID->SetValue( wxString(wDigInt.getiid(m_Props),wxConvUTF8) );
  m_Device->SetValue( wxString(wDigInt.getdevice(m_Props),wxConvUTF8) );
  switch(wDigInt.getbps(m_Props)) {
  case 2400:
    m_Baudrate->SetSelection(0);
    break;
  case 4800:
    m_Baudrate->SetSelection(1);
    break;
  case 9600:
    m_Baudrate->SetSelection(2);
    break;
  case 19200:
    m_Baudrate->SetSelection(3);
    break;
  case 38400:
    m_Baudrate->SetSelection(4);
    break;
  case 57600:
    m_Baudrate->SetSelection(5);
    break;
  case 115200:
    m_Baudrate->SetSelection(6);
    break;
  }

}


void OpenDCCCtrlDlg::sendGet( int so ) {
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_cs );
  wProgram.setcmd( cmd, wProgram.get );
  wProgram.setiid( cmd, wDigInt.getiid(m_Props) );
  wProgram.setcv( cmd, so );
  wProgram.setvalue( cmd, 0 );
  wxGetApp().sendToRocrail( cmd );
}


void OpenDCCCtrlDlg::sendSet( int so, int value ) {
  TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "sending XSOSet=%d value=%d",
      so, value );
  iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setlntype( cmd, wProgram.lntype_cs );
  wProgram.setcmd( cmd, wProgram.set );
  wProgram.setiid( cmd, wDigInt.getiid(m_Props) );
  wProgram.setcv( cmd, so );
  wProgram.setvalue( cmd, value );
  wxGetApp().sendToRocrail( cmd );
}


void OpenDCCCtrlDlg::writeAll() {
  TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "write all" );

  if( m_soValue[so_bps] != m_Baudrate->GetSelection() ) {
    sendSet( so_bps, m_Baudrate->GetSelection() );
  }
  if( m_soValue[so_dcc_format] != m_DecSpeedSteps->GetSelection() ) {
    sendSet( so_dcc_format, m_DecSpeedSteps->GetSelection() );
  }
  if( m_soValue[so_s88_timing] != m_S88Timing->GetValue() ) {
    sendSet( so_s88_timing, m_S88Timing->GetValue() );
  }
  if( m_soValue[so_s88_bus1] != m_S88Bus1->GetValue() ) {
    sendSet( so_s88_bus1, m_S88Bus1->GetValue() );
  }
  if( m_soValue[so_s88_bus2] != m_S88Bus2->GetValue() ) {
    sendSet( so_s88_bus2, m_S88Bus2->GetValue() );
  }
  if( m_soValue[so_s88_bus3] != m_S88Bus3->GetValue() ) {
    sendSet( so_s88_bus3, m_S88Bus3->GetValue() );
  }

  int mode = m_S88Active->IsChecked() ? 0x01:0x00;
  mode |= m_S88TurnoutSensors->IsChecked() ? 0x02:0x00;
  if( m_soValue[so_s88_mode] != mode ) {
    sendSet( so_s88_mode, mode );
  }

  if( m_soValue[so_pt_resets] != m_PTResets->GetValue() ) {
    sendSet( so_pt_resets, m_PTResets->GetValue() );
  }
  if( m_soValue[so_pt_commands] != m_PTCommands->GetValue() ) {
    sendSet( so_pt_commands, m_PTCommands->GetValue() );
  }
  if( m_soValue[so_pt_pom] != m_PTPOMrepeat->GetValue() ) {
    sendSet( so_pt_pom, m_PTPOMrepeat->GetValue() );
  }
  if( m_soValue[so_scd_main] != m_SCTimeMain->GetValue() ) {
    sendSet( so_scd_main, m_SCTimeMain->GetValue() );
  }
  if( m_soValue[so_scd_pt] != m_SCTimePT->GetValue() ) {
    sendSet( so_scd_pt, m_SCTimePT->GetValue() );
  }

  int swinvert = m_SwitchInvert->IsChecked() ? 0x01:0x00;
  if( m_soValue[so_sw_invert] != swinvert ) {
    sendSet( so_sw_invert, swinvert );
  }

  if( m_soValue[so_sw_commands] != m_SwitchCommands->GetValue() ) {
    sendSet( so_sw_commands, m_SwitchCommands->GetValue() );
  }
  if( m_soValue[so_sw_time] != m_SwitchTime->GetValue() ) {
    sendSet( so_sw_time, m_SwitchTime->GetValue() );
  }
  if( m_soValue[so_sw_sensor_mode] != m_SwitchSensorMode->GetSelection() ) {
    sendSet( so_sw_sensor_mode, m_SwitchSensorMode->GetSelection() );
  }

}


void OpenDCCCtrlDlg::evaluateSet( int so, int value ) {

}


void OpenDCCCtrlDlg::evaluateGet( int so, int value ) {
  m_soValue[so] = value;

  if( so == so_version ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "version = %d.%d", value/100, value%100 );
    m_Version->SetValue( wxString::Format(_T("%d.%d"), value/100, value%100) );
    sendGet( so_bps );
  }
  else if( so == so_bps ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "baudrate = %d", value );
    m_Baudrate->SetSelection(value);
    sendGet( so_dcc_format );
  }
  else if( so == so_dcc_format ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "dcc format = %d", value );
    /*
    0:   DCC mit 14 Fahrstufen
    1:   reserviert (DCC mit 27 Fahrstufen, nicht implementiert)
    2:   DCC mit 28 Fahrstufen (Voreinstellung)
    3:   DCC mit 128 Fahrstufen (bzw. 126, um genau zu sein)
    */
    m_DecSpeedSteps->SetSelection(value);
    sendGet( so_s88_timing );
  }
  else if( so == so_s88_timing ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "s88 timing = %d", value );
    m_S88Timing->SetValue(value);
    sendGet( so_s88_bus1 );
  }
  else if( so == so_s88_bus1 ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "s88 bus1 = %d", value );
    m_S88Bus1->SetValue(value);
    sendGet( so_s88_bus2 );
  }
  else if( so == so_s88_bus2 ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "s88 bus2 = %d", value );
    m_S88Bus2->SetValue(value);
    sendGet( so_s88_bus3 );
  }
  else if( so == so_s88_bus3 ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "s88 bus3 = %d", value );
    m_S88Bus3->SetValue(value);
    sendGet( so_s88_mode );
  }
  else if( so == so_s88_mode ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "s88 mode = %d", value );
    m_S88Active->SetValue( (value&0x01) ? true:false );
    m_S88TurnoutSensors->SetValue( (value&0x02) ? true:false );
    sendGet( so_pt_resets );
  }
  else if( so == so_pt_resets ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "pt resets = %d", value );
    m_PTResets->SetValue( value );
    sendGet( so_pt_commands );
  }
  else if( so == so_pt_commands ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "pt commands = %d", value );
    m_PTCommands->SetValue( value );
    sendGet( so_pt_pom );
  }
  else if( so == so_pt_pom ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "pt pom = %d", value );
    m_PTPOMrepeat->SetValue( value );
    sendGet( so_scd_main );
  }
  else if( so == so_scd_main ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "scd main = %d", value );
    m_SCTimeMain->SetValue( value );
    sendGet( so_scd_pt );
  }
  else if( so == so_scd_pt ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "scd pt = %d", value );
    m_SCTimePT->SetValue( value );
    sendGet( so_sw_invert );
  }
  else if( so == so_sw_invert ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "sw invert = %d", value );
    m_SwitchInvert->SetValue( (value&0x01) ? true:false );
    sendGet( so_sw_commands );
  }
  else if( so == so_sw_commands ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "sw commands = %d", value );
    m_SwitchCommands->SetValue( value );
    sendGet( so_sw_time );
  }
  else if( so == so_sw_time ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "sw time = %d", value );
    m_SwitchTime->SetValue( value );
    sendGet( so_sw_sensor_offset );
  }
  else if( so == so_sw_sensor_offset ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "sw sensor offset = %d", value );
    m_SwitchSensorOffset->SetValue( value );
    sendGet( so_sw_sensor_mode );
  }
  else if( so == so_sw_sensor_mode ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "sw sensor mode = %d", value );
    m_SwitchSensorMode->SetSelection( value );
    sendGet( so_loco_format_low );
  }
  // TODO: get list of loco decoder exceptions
  // 64 pairs starting with: so_loco_format_low, so_loco_format_high
  else if( so >= so_loco_format_low && so <= so_loco_format_high + 64*2 ) {
    TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "loco format %d = %d", so, value );
    m_SwitchSensorMode->SetSelection( value );

    if( so % 2 == 1) {
      int index = ( so - so_loco_format_low ) / 2;
      int locoFormatLow  = m_soValue[so-1];
      int locoFormatHigh = m_soValue[so];
      int address = locoFormatLow + ((locoFormatHigh & 0x3F) << 8);
      int format  = (locoFormatHigh & 0xC0) >> 6;

      if( address > 0 ) {
        m_DecExceptions->InsertItem( index, wxString::Format(_T("%d"), address));
        if( format == 0 )
          m_DecExceptions->SetItem( index, 1, _T("DCC 14"));
        else if( format == 1 )
          m_DecExceptions->SetItem( index, 1, _T("DCC 27"));
        else if( format == 2 )
          m_DecExceptions->SetItem( index, 1, _T("DCC 28"));
        else if( format == 3 )
          m_DecExceptions->SetItem( index, 1, _T("DCC 128"));
        else
          m_DecExceptions->SetItem( index, 1, _T("DCC ???"));

        if( index < 64 )
          sendGet( so + 1 );
      }
      else {
        stopProgress();
      }
    }
    else
      sendGet( so + 1 );
  }
  else {
    stopProgress();
    m_DecoderPanel->Enable(true);
    m_S88Panel->Enable(true);
    m_ProgrammingTab->Enable(true);
    m_SCDPanel->Enable(true);
    m_SwitchPanel->Enable(true);
  }

}

void OpenDCCCtrlDlg::OnPTEvent( wxCommandEvent& event ) {
  // Get copied node:
  iONode node = (iONode)event.GetClientData();

  int so = wProgram.getcv(node);
  int value = wProgram.getvalue(node);

  TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "%s node received: [%d][%d]",
      NodeOp.getName(node), so, value );

  if( wProgram.getcmd(node) == wProgram.get ) {
    evaluateGet( so, value );
  }
  else if( wProgram.getcmd(node) == wProgram.set ) {
    evaluateSet( so, value );
  }
  else {
    TraceOp.trc( "opendcc", TRCLEVEL_WARNING, __LINE__, 9999, "command [%d] not expected...", wProgram.getcmd(node) );
  }


  /* clean up event node */
  NodeOp.base.del(node);
}

/*!
 * OpenDCCCtrlDlg creator
 */

bool OpenDCCCtrlDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin OpenDCCCtrlDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end OpenDCCCtrlDlg creation
    return true;
}


/*!
 * OpenDCCCtrlDlg destructor
 */

OpenDCCCtrlDlg::~OpenDCCCtrlDlg()
{
////@begin OpenDCCCtrlDlg destruction
////@end OpenDCCCtrlDlg destruction
}


/*!
 * Member initialisation
 */

void OpenDCCCtrlDlg::Init()
{
////@begin OpenDCCCtrlDlg member initialisation
    m_Notebook = NULL;
    m_GeneralPanel = NULL;
    m_labIID = NULL;
    m_IID = NULL;
    m_labVersion = NULL;
    m_Version = NULL;
    m_labDevice = NULL;
    m_Device = NULL;
    m_Baudrate = NULL;
    m_ReadCVs = NULL;
    m_WriteCVs = NULL;
    m_DecoderPanel = NULL;
    m_DecSpeedSteps = NULL;
    m_DecExceptionBox = NULL;
    m_DecExceptions = NULL;
    m_S88Panel = NULL;
    m_labS88Timing = NULL;
    m_S88Timing = NULL;
    m_S88BusBox = NULL;
    m_labS88Bus1 = NULL;
    m_S88Bus1 = NULL;
    m_labS88Bus2 = NULL;
    m_S88Bus2 = NULL;
    m_labS88Bus3 = NULL;
    m_S88Bus3 = NULL;
    m_S88SettingsBox = NULL;
    m_S88Active = NULL;
    m_S88TurnoutSensors = NULL;
    m_SwitchSensorMode = NULL;
    m_labSwitchSensorOffset = NULL;
    m_SwitchSensorOffset = NULL;
    m_ProgrammingTab = NULL;
    m_labPTResets = NULL;
    m_PTResets = NULL;
    m_labPTCommands = NULL;
    m_PTCommands = NULL;
    m_labPTPOMrepeat = NULL;
    m_PTPOMrepeat = NULL;
    m_SCDPanel = NULL;
    m_LabSCtimeMain = NULL;
    m_SCTimeMain = NULL;
    m_labSCTimePT = NULL;
    m_SCTimePT = NULL;
    m_SwitchPanel = NULL;
    m_labSwitchCommands = NULL;
    m_SwitchCommands = NULL;
    m_labSwitchTime = NULL;
    m_SwitchTime = NULL;
    m_SwitchInvert = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
    m_Apply = NULL;
////@end OpenDCCCtrlDlg member initialisation
}


/*!
 * Control creation for OpenDCCCtrlDlg
 */

void OpenDCCCtrlDlg::CreateControls()
{
////@begin OpenDCCCtrlDlg content construction
    OpenDCCCtrlDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_GeneralPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_GeneralPanel->SetSizer(itemBoxSizer5);

    wxBoxSizer* itemBoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer6, 1, wxGROW|wxALL, 5);
    wxFlexGridSizer* itemFlexGridSizer7 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer7->AddGrowableCol(1);
    itemBoxSizer6->Add(itemFlexGridSizer7, 1, wxGROW|wxALL, 5);
    m_labIID = new wxStaticText( m_GeneralPanel, wxID_ANY, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_IID = new wxTextCtrl( m_GeneralPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_IID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labVersion = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Version"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labVersion, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM, 5);

    m_Version = new wxTextCtrl( m_GeneralPanel, wxID_ANY, _T(""), wxDefaultPosition, wxSize(60, -1), wxTE_READONLY|wxTE_CENTRE );
    m_Version->Enable(false);
    itemFlexGridSizer7->Add(m_Version, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDevice = new wxStaticText( m_GeneralPanel, wxID_ANY, _("Device"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_labDevice, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Device = new wxTextCtrl( m_GeneralPanel, wxID_ANY, _T(""), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer7->Add(m_Device, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_BaudrateStrings;
    m_BaudrateStrings.Add(_("&2400"));
    m_BaudrateStrings.Add(_("&4800"));
    m_BaudrateStrings.Add(_("&9600"));
    m_BaudrateStrings.Add(_("&19200"));
    m_BaudrateStrings.Add(_("&38400"));
    m_BaudrateStrings.Add(_("&57600"));
    m_BaudrateStrings.Add(_("&115200"));
    m_Baudrate = new wxRadioBox( m_GeneralPanel, wxID_ANY, _("Baudrate"), wxDefaultPosition, wxDefaultSize, m_BaudrateStrings, 1, wxRA_SPECIFY_COLS );
    m_Baudrate->SetSelection(0);
    itemBoxSizer5->Add(m_Baudrate, 0, wxALIGN_LEFT|wxALL, 5);

    wxBoxSizer* itemBoxSizer15 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer5->Add(itemBoxSizer15, 0, wxGROW|wxALL, 5);
    m_ReadCVs = new wxButton( m_GeneralPanel, ID_READCVS, _("Read CVs"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(m_ReadCVs, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_WriteCVs = new wxButton( m_GeneralPanel, ID_WRITECVS, _("Write CVs"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer15->Add(m_WriteCVs, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_GeneralPanel, _("General"));

    m_DecoderPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer19 = new wxBoxSizer(wxVERTICAL);
    m_DecoderPanel->SetSizer(itemBoxSizer19);

    wxArrayString m_DecSpeedStepsStrings;
    m_DecSpeedStepsStrings.Add(_("&14"));
    m_DecSpeedStepsStrings.Add(_("&27"));
    m_DecSpeedStepsStrings.Add(_("&28"));
    m_DecSpeedStepsStrings.Add(_("&128"));
    m_DecSpeedSteps = new wxRadioBox( m_DecoderPanel, wxID_ANY, _("Default steps"), wxDefaultPosition, wxDefaultSize, m_DecSpeedStepsStrings, 1, wxRA_SPECIFY_ROWS );
    m_DecSpeedSteps->SetSelection(0);
    itemBoxSizer19->Add(m_DecSpeedSteps, 0, wxALIGN_LEFT|wxALL, 5);

    m_DecExceptionBox = new wxStaticBox(m_DecoderPanel, wxID_ANY, _("Exceptions"));
    wxStaticBoxSizer* itemStaticBoxSizer21 = new wxStaticBoxSizer(m_DecExceptionBox, wxVERTICAL);
    itemBoxSizer19->Add(itemStaticBoxSizer21, 1, wxGROW|wxALL, 5);
    m_DecExceptions = new wxListCtrl( m_DecoderPanel, ID_LISTCTRL_DECEXCEPTIONS, wxDefaultPosition, wxSize(100, 100), wxLC_REPORT|wxLC_HRULES|wxLC_VRULES );
    itemStaticBoxSizer21->Add(m_DecExceptions, 1, wxGROW|wxALL, 5);

    m_Notebook->AddPage(m_DecoderPanel, _("Decoders"));

    m_S88Panel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer24 = new wxBoxSizer(wxVERTICAL);
    m_S88Panel->SetSizer(itemBoxSizer24);

    wxFlexGridSizer* itemFlexGridSizer25 = new wxFlexGridSizer(2, 3, 0, 0);
    itemBoxSizer24->Add(itemFlexGridSizer25, 0, wxGROW|wxALL, 5);
    m_labS88Timing = new wxStaticText( m_S88Panel, wxID_ANY, _("Timing"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer25->Add(m_labS88Timing, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_S88Timing = new wxSpinCtrl( m_S88Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer25->Add(m_S88Timing, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    wxStaticText* itemStaticText28 = new wxStaticText( m_S88Panel, wxID_ANY, _("us"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer25->Add(itemStaticText28, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP, 5);

    m_S88BusBox = new wxStaticBox(m_S88Panel, wxID_ANY, _("Modules"));
    wxStaticBoxSizer* itemStaticBoxSizer29 = new wxStaticBoxSizer(m_S88BusBox, wxVERTICAL);
    itemBoxSizer24->Add(itemStaticBoxSizer29, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    wxFlexGridSizer* itemFlexGridSizer30 = new wxFlexGridSizer(2, 2, 0, 0);
    itemStaticBoxSizer29->Add(itemFlexGridSizer30, 0, wxGROW|wxALL, 5);
    m_labS88Bus1 = new wxStaticText( m_S88Panel, wxID_ANY, _("Bus 1"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer30->Add(m_labS88Bus1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_S88Bus1 = new wxSpinCtrl( m_S88Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer30->Add(m_S88Bus1, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labS88Bus2 = new wxStaticText( m_S88Panel, wxID_ANY, _("Bus 2"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer30->Add(m_labS88Bus2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_S88Bus2 = new wxSpinCtrl( m_S88Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer30->Add(m_S88Bus2, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labS88Bus3 = new wxStaticText( m_S88Panel, wxID_ANY, _("Bus 3"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer30->Add(m_labS88Bus3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_S88Bus3 = new wxSpinCtrl( m_S88Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer30->Add(m_S88Bus3, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_S88SettingsBox = new wxStaticBox(m_S88Panel, wxID_ANY, _("Mode"));
    wxStaticBoxSizer* itemStaticBoxSizer37 = new wxStaticBoxSizer(m_S88SettingsBox, wxVERTICAL);
    itemBoxSizer24->Add(itemStaticBoxSizer37, 0, wxGROW|wxLEFT|wxRIGHT, 5);
    m_S88Active = new wxCheckBox( m_S88Panel, wxID_ANY, _("Active"), wxDefaultPosition, wxDefaultSize, 0 );
    m_S88Active->SetValue(true);
    itemStaticBoxSizer37->Add(m_S88Active, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxTOP, 5);

    m_S88TurnoutSensors = new wxCheckBox( m_S88Panel, wxID_ANY, _("Turnout Sensors"), wxDefaultPosition, wxDefaultSize, 0 );
    m_S88TurnoutSensors->SetValue(false);
    itemStaticBoxSizer37->Add(m_S88TurnoutSensors, 0, wxALIGN_LEFT|wxLEFT|wxRIGHT|wxBOTTOM, 5);

    wxArrayString m_SwitchSensorModeStrings;
    m_SwitchSensorModeStrings.Add(_("&positive"));
    m_SwitchSensorModeStrings.Add(_("&negative"));
    m_SwitchSensorModeStrings.Add(_("&position"));
    m_SwitchSensorMode = new wxRadioBox( m_S88Panel, wxID_ANY, _("Switch sensor mode"), wxDefaultPosition, wxDefaultSize, m_SwitchSensorModeStrings, 1, wxRA_SPECIFY_ROWS );
    m_SwitchSensorMode->SetSelection(0);
    itemBoxSizer24->Add(m_SwitchSensorMode, 1, wxALIGN_LEFT|wxLEFT|wxRIGHT, 5);

    wxFlexGridSizer* itemFlexGridSizer41 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer24->Add(itemFlexGridSizer41, 0, wxGROW|wxLEFT|wxRIGHT|wxBOTTOM, 5);
    m_labSwitchSensorOffset = new wxStaticText( m_S88Panel, wxID_ANY, _("Switch sensor offset"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer41->Add(m_labSwitchSensorOffset, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SwitchSensorOffset = new wxSpinCtrl( m_S88Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 255, 0 );
    itemFlexGridSizer41->Add(m_SwitchSensorOffset, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_S88Panel, _("S88"));

    m_ProgrammingTab = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer45 = new wxBoxSizer(wxVERTICAL);
    m_ProgrammingTab->SetSizer(itemBoxSizer45);

    wxFlexGridSizer* itemFlexGridSizer46 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer45->Add(itemFlexGridSizer46, 0, wxGROW|wxALL, 5);
    m_labPTResets = new wxStaticText( m_ProgrammingTab, wxID_ANY, _("Resets"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer46->Add(m_labPTResets, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_PTResets = new wxSpinCtrl( m_ProgrammingTab, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 10, 0 );
    itemFlexGridSizer46->Add(m_PTResets, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labPTCommands = new wxStaticText( m_ProgrammingTab, wxID_ANY, _("Commands"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer46->Add(m_labPTCommands, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_PTCommands = new wxSpinCtrl( m_ProgrammingTab, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 10, 0 );
    itemFlexGridSizer46->Add(m_PTCommands, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labPTPOMrepeat = new wxStaticText( m_ProgrammingTab, wxID_ANY, _("POM repeat"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer46->Add(m_labPTPOMrepeat, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_PTPOMrepeat = new wxSpinCtrl( m_ProgrammingTab, wxID_ANY, _T("3"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 2, 10, 3 );
    itemFlexGridSizer46->Add(m_PTPOMrepeat, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_ProgrammingTab, _("Programming Track"));

    m_SCDPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer54 = new wxBoxSizer(wxVERTICAL);
    m_SCDPanel->SetSizer(itemBoxSizer54);

    wxFlexGridSizer* itemFlexGridSizer55 = new wxFlexGridSizer(2, 3, 0, 0);
    itemBoxSizer54->Add(itemFlexGridSizer55, 0, wxGROW|wxALL, 5);
    m_LabSCtimeMain = new wxStaticText( m_SCDPanel, wxID_ANY, _("SCTime Main"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer55->Add(m_LabSCtimeMain, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SCTimeMain = new wxSpinCtrl( m_SCDPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer55->Add(m_SCTimeMain, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText58 = new wxStaticText( m_SCDPanel, wxID_ANY, _("ms"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer55->Add(itemStaticText58, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_labSCTimePT = new wxStaticText( m_SCDPanel, wxID_ANY, _("SCTime PT"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer55->Add(m_labSCTimePT, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SCTimePT = new wxSpinCtrl( m_SCDPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer55->Add(m_SCTimePT, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText61 = new wxStaticText( m_SCDPanel, wxID_ANY, _("ms"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer55->Add(itemStaticText61, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_Notebook->AddPage(m_SCDPanel, _("SCD"));

    m_SwitchPanel = new wxPanel( m_Notebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer63 = new wxBoxSizer(wxVERTICAL);
    m_SwitchPanel->SetSizer(itemBoxSizer63);

    wxFlexGridSizer* itemFlexGridSizer64 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer64->AddGrowableCol(1);
    itemBoxSizer63->Add(itemFlexGridSizer64, 0, wxGROW|wxALL, 5);
    m_labSwitchCommands = new wxStaticText( m_SwitchPanel, wxID_ANY, _("Command repetitions"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer64->Add(m_labSwitchCommands, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SwitchCommands = new wxSpinCtrl( m_SwitchPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 100, 0 );
    itemFlexGridSizer64->Add(m_SwitchCommands, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labSwitchTime = new wxStaticText( m_SwitchPanel, wxID_ANY, _("Activation time"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer64->Add(m_labSwitchTime, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SwitchTime = new wxSpinCtrl( m_SwitchPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 255, 0 );
    itemFlexGridSizer64->Add(m_SwitchTime, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SwitchInvert = new wxCheckBox( m_SwitchPanel, wxID_ANY, _("Invert commands"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SwitchInvert->SetValue(false);
    itemBoxSizer63->Add(m_SwitchInvert, 0, wxALIGN_LEFT|wxALL, 5);

    m_Notebook->AddPage(m_SwitchPanel, _("Switches"));

    itemBoxSizer2->Add(m_Notebook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer70 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer70, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer70->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer70->AddButton(m_Cancel);

    m_Apply = new wxButton( itemDialog1, wxID_APPLY, _("&Apply"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer70->AddButton(m_Apply);

    itemStdDialogButtonSizer70->Realize();

////@end OpenDCCCtrlDlg content construction
}


/*!
 * Should we show tooltips?
 */

bool OpenDCCCtrlDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap OpenDCCCtrlDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin OpenDCCCtrlDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end OpenDCCCtrlDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon OpenDCCCtrlDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin OpenDCCCtrlDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end OpenDCCCtrlDlg icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_OPENDCC_DECSET
 */

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_OPENDCC_READALL
 */

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void OpenDCCCtrlDlg::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
 */

void OpenDCCCtrlDlg::OnApplyClick( wxCommandEvent& event )
{
  // TODO: check for diffs between dialog item values and the values read in the m_soValue[] array...
  evaluate();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void OpenDCCCtrlDlg::OnOkClick( wxCommandEvent& event )
{
  OnApplyClick(event);
  EndModal( wxID_OK );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_WRITECVS
 */

void OpenDCCCtrlDlg::OnWritecvsClick( wxCommandEvent& event )
{
  writeAll();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_READCVS
 */

void OpenDCCCtrlDlg::OnReadcvsClick( wxCommandEvent& event )
{
  TraceOp.trc( "opendcc", TRCLEVEL_INFO, __LINE__, 9999, "initValues" );
  m_bStartUpProgress = true;
  m_Timer->Start( 1000, wxTIMER_ONE_SHOT );
}

