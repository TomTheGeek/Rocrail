/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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
#include "rocview/public/guiapp.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "sensorevents.h"

SensorEventsDlg::SensorEventsDlg( wxWindow* parent )
    :SensorEventsGen( parent )
{
  initLabels();
  initValues();
}


void SensorEventsDlg::initLabels() {
  SetTitle(wxGetApp().getMsg( "sensormonitor" ));
  m_EventList->InsertColumn(0, wxGetApp().getMsg( "bus" ), wxLIST_FORMAT_LEFT );
  m_EventList->InsertColumn(1, wxGetApp().getMsg( "address" ), wxLIST_FORMAT_LEFT );
  m_EventList->InsertColumn(2, wxGetApp().getMsg( "state" ), wxLIST_FORMAT_LEFT );
  m_EventList->InsertColumn(3, wxGetApp().getMsg( "identifier" ), wxLIST_FORMAT_LEFT );
  m_Refresh->SetLabel(wxGetApp().getMsg( "refresh" ));
}


void SensorEventsDlg::initValues() {
  m_EventList->DeleteAllItems();
  iOList list = wxGetApp().getSensorEvents();

  int cnt = ListOp.size( list );
  for( int i = 0; i < cnt; i++ ) {
    iONode fbevent = (iONode)ListOp.get( list, i );
    m_EventList->InsertItem( i, wxString::Format(wxT("%d"), wFeedback.getbus(fbevent) ) );
    m_EventList->SetItem( i, 1, wxString::Format(wxT("%d"), wFeedback.getaddr(fbevent) ) );
    m_EventList->SetItem( i, 2, wxString::Format(wxT("%d"), wFeedback.isstate(fbevent)?1:0 ) );
    m_EventList->SetItem( i, 3, wxString(wFeedback.getidentifier(fbevent), wxConvUTF8) );
  }
}


void SensorEventsDlg::onRefresh( wxCommandEvent& event ) {
  initValues();
}

void SensorEventsDlg::onOK( wxCommandEvent& event ) {
  EndModal(wxID_OK);
}

