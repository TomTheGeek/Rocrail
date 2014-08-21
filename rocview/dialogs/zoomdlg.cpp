/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 This program is free software; you can redistribute it and/or
 as published by the Free Software Foundation; either version 2
 modify it under the terms of the GNU General Public License
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "zoomdlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"


ZoomDlg::ZoomDlg( wxWindow* parent, int percent ):ZoomDlgGen( parent )
{
  m_ZoomValue->SetValue(percent);
  m_ZoomSlider->SetValue(percent);
  SetTitle(wxGetApp().getMsg( "zoom" ));
}

void ZoomDlg::onCancel( wxCommandEvent& event )
{
  EndModal( 0 );
}

void ZoomDlg::onOK( wxCommandEvent& event )
{
  EndModal( wxID_OK );
}

void ZoomDlg::onZoomSpin( wxSpinEvent& event ) {
  m_ZoomSlider->SetValue( m_ZoomValue->GetValue() );
}


void ZoomDlg::onZoomThumb( wxScrollEvent& event ) {
  m_ZoomValue->SetValue( m_ZoomSlider->GetValue() );
}


void ZoomDlg::onZoomRelease( wxScrollEvent& event ) {
  int percent = m_ZoomSlider->GetValue();
  int rest = percent % 5;
  percent -= percent % 5;
  if( rest > 2 )
    percent += 5;
  m_ZoomValue->SetValue(percent);
  m_ZoomSlider->SetValue(percent);
}


int ZoomDlg::GetValue() {
  return m_ZoomValue->GetValue();
}



