#include "lococtrldlg.h"

LocoCtrlDlg::LocoCtrlDlg( wxWindow* parent ):LocoCtrlDlgGen( parent )
{

}

void LocoCtrlDlg::onLB1( wxMouseEvent& event )
{
  m_LB1->mouseLeftWindow(event);
  m_LB1->setLED(!m_LB1->ON);
}
