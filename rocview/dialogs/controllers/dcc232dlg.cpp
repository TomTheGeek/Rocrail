#include "dcc232dlg.h"

DCC232Dlg::DCC232Dlg( wxWindow* parent, iONode props )
  :dcc232gen( parent )
{
  m_Props = props;
  initLabels();
  initValues();
}


void DCC232Dlg::initLabels() {

}


void DCC232Dlg::initValues() {

}


void DCC232Dlg::evaluate() {

}


void DCC232Dlg::onOK( wxCommandEvent& event ) {
  evaluate();
  EndModal( wxID_OK );
}


void DCC232Dlg::onCancel( wxCommandEvent& event ) {
  EndModal( wxID_CANCEL );
}




