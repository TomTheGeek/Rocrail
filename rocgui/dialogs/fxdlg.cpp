#include "fxdlg.h"

FxDlg::FxDlg( wxWindow* parent, int p_FX )
  :fxdlggen( parent )
{
  m_FX = p_FX;
}

void FxDlg::onCancel( wxCommandEvent& event )
{
	// TODO: Implement onCancel
}

void FxDlg::onOK( wxCommandEvent& event )
{
	// TODO: Implement onOK
}

int FxDlg::getConfig() {
  return m_FX;
}



void FxDlg::initLabels() {
}

void FxDlg::initValues() {
}
