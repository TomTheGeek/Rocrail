#include "uhl68610dlg.h"

Uhl68610Dlg::Uhl68610Dlg( wxWindow* parent )
:uhl68610dlggen( parent ) {
  initLabels();
}

void Uhl68610Dlg::initLabels() {
}

void Uhl68610Dlg::onReporting( wxCommandEvent& event ) {
  bool enable = m_Reporting->GetSelection() == 0 ? true:false;
  m_Scale->Enable(enable);
  m_labGap->Enable(enable);
  m_Gap->Enable(enable);
}

void Uhl68610Dlg::onRead( wxCommandEvent& event ) {
}

void Uhl68610Dlg::onWrite( wxCommandEvent& event ) {
}

void Uhl68610Dlg::onCancel( wxCommandEvent& event ) {
  EndModal( 0 );
}

void Uhl68610Dlg::onOK( wxCommandEvent& event ) {
  EndModal( wxID_OK );
}

