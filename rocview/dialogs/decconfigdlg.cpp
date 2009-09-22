#include "decconfigdlg.h"

#include "rocs/public/trace.h"

DecConfigDlg::DecConfigDlg( wxWindow* parent, int p_Config )
  :decconfigdlggen( parent )
{
  TraceOp.trc( "deccfg", TRCLEVEL_INFO, __LINE__, 9999, "DecConfigDlg" );
  m_Config = p_Config;
  initLabels();
  initValues();
}

int DecConfigDlg::getConfig() {
  TraceOp.trc( "deccfg", TRCLEVEL_INFO, __LINE__, 9999, "getConfig" );
  m_Config = 0;
  m_Config |= m_Direction->IsChecked() ? 0x01:0x00;
  m_Config |= m_SpeedSteps->IsChecked() ? 0x02:0x00;
  m_Config |= m_DCOperation->IsChecked() ? 0x04:0x00;
  m_Config |= m_UserSpeedCurve->IsChecked() ? 0x10:0x00;
  m_Config |= m_ExtAddressing->IsChecked() ? 0x20:0x00;
  return m_Config;
}


void DecConfigDlg::initLabels() {
  TraceOp.trc( "deccfg", TRCLEVEL_INFO, __LINE__, 9999, "initLabels" );

}


void DecConfigDlg::initValues() {
  TraceOp.trc( "deccfg", TRCLEVEL_INFO, __LINE__, 9999, "initValues" );
  m_Direction->SetValue( (m_Config & 0x01) ? true:false );
  m_SpeedSteps->SetValue( (m_Config & 0x02) ? true:false );
  m_DCOperation->SetValue( (m_Config & 0x04) ? true:false );
  m_UserSpeedCurve->SetValue( (m_Config & 0x10) ? true:false );
  m_ExtAddressing->SetValue( (m_Config & 0x20) ? true:false );

}


void DecConfigDlg::onCancel( wxCommandEvent& event ) {
  TraceOp.trc( "deccfg", TRCLEVEL_INFO, __LINE__, 9999, "onCancel" );
  EndModal( 0 );
}


void DecConfigDlg::onOK( wxCommandEvent& event ) {
  TraceOp.trc( "deccfg", TRCLEVEL_INFO, __LINE__, 9999, "onOK" );
  EndModal( wxID_OK );
}
