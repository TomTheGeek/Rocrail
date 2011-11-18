#include "toursdlg.h"

ToursDlg::ToursDlg( wxWindow* parent )
  :toursdlggen( parent )
{
  m_Props = NULL;

}


ToursDlg::ToursDlg( wxWindow* parent, iONode tour, bool save )
  :toursdlggen( parent )
{
  m_Props = tour;
}

