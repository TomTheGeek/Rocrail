#include "fxdlg.h"

FxDlg::FxDlg( wxWindow* parent, int p_FX )
  :fxdlggen( parent )
{
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "FxDlg" );
  m_FX = p_FX;
  initLabels();
  initValues();
}

void FxDlg::onCancel( wxCommandEvent& event ) {
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "onCancel" );
  EndModal( 0 );
}


void FxDlg::onOK( wxCommandEvent& event ) {
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "onOK" );
  EndModal( wxID_OK );
}

int FxDlg::getConfig() {
  return m_FX;
}

void FxDlg::onFunction( wxCommandEvent& event ) [
}


void FxDlg::initLabels() {
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "initLabels" );
}

void FxDlg::initValues() {
  TraceOp.trc( "fxdlg", TRCLEVEL_INFO, __LINE__, 9999, "initValues" );
}
