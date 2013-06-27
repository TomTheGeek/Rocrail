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
#include "sensorevents.h"

#include "rocview/public/guiapp.h"
#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/SensorMonitor.h"

#include "rocrail/wrapper/public/Feedback.h"


SensorEventsDlg::SensorEventsDlg( wxWindow* parent )
    :SensorEventsGen( parent )
{
  initLabels();
  initValues();
  GetSizer()->Layout();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
  wxGetApp().setSensorEventListener(this);

  iONode ini = wxGetApp().getIni();
  iONode sensmon = wGui.getsensormonitor(ini);
  if( sensmon != NULL ) {
    if( wSensorMonitor.getcx(sensmon) > 0 && wSensorMonitor.getcy(sensmon) > 0 ) {
      SetSize(wSensorMonitor.getx(sensmon), wSensorMonitor.gety(sensmon), wSensorMonitor.getcx(sensmon), wSensorMonitor.getcy(sensmon));
    }
    else
      SetSize(wSensorMonitor.getx(sensmon), wSensorMonitor.gety(sensmon));
  }
}


void SensorEventsDlg::initLabels() {
  SetTitle(wxGetApp().getMsg( "sensormonitor" ));
  m_EventList->InsertColumn(0, wxGetApp().getMsg( "id" ), wxLIST_FORMAT_LEFT );
  m_EventList->InsertColumn(1, wxGetApp().getMsg( "bus" ), wxLIST_FORMAT_RIGHT );
  m_EventList->InsertColumn(2, wxGetApp().getMsg( "address" ), wxLIST_FORMAT_RIGHT );
  m_EventList->InsertColumn(3, wxGetApp().getMsg( "state" ), wxLIST_FORMAT_CENTER );
  m_EventList->InsertColumn(4, wxGetApp().getMsg( "identifier" ), wxLIST_FORMAT_LEFT );
  m_EventList->InsertColumn(5, wxGetApp().getMsg( "counter" ), wxLIST_FORMAT_RIGHT );
  m_EventList->InsertColumn(6, wxGetApp().getMsg( "wheelcount" ), wxLIST_FORMAT_RIGHT );
  m_EventList->InsertColumn(7, wxGetApp().getMsg( "countedcars" ), wxLIST_FORMAT_RIGHT );
  m_EventList->InsertColumn(8, wxGetApp().getMsg( "stamp" ), wxLIST_FORMAT_LEFT );
  m_Refresh->SetLabel(wxGetApp().getMsg( "refresh" ));
}


void SensorEventsDlg::initValues() {
  m_EventList->DeleteAllItems();
  iOList list = wxGetApp().getSensorEvents();

  int cnt = ListOp.size( list );
  for( int i = 0; i < cnt; i++ ) {
    iONode fbevent = (iONode)ListOp.get( list, i );
    m_EventList->InsertItem( i, wxString(wFeedback.getid(fbevent), wxConvUTF8) );
    m_EventList->SetItem( i, 1, wxString::Format(wxT("%d"), wFeedback.getbus(fbevent) ) );
    m_EventList->SetItem( i, 2, wxString::Format(wxT("%d"), wFeedback.getaddr(fbevent) ) );
    m_EventList->SetItem( i, 3, wxString::Format(wxT("%d"), wFeedback.isstate(fbevent)?1:0 ) );
    m_EventList->SetItem( i, 4, wxString(wFeedback.getidentifier(fbevent), wxConvUTF8) );
    m_EventList->SetItem( i, 5, wxString::Format(wxT("%d"), wFeedback.getcounter(fbevent) ) );
    m_EventList->SetItem( i, 6, wxString::Format(wxT("%d"), wFeedback.getwheelcount(fbevent) ) );
    m_EventList->SetItem( i, 7, wxString::Format(wxT("%d"), wFeedback.getcountedcars(fbevent) ) );
    m_EventList->SetItem( i, 8, wxString(NodeOp.getStr(fbevent, "stamp", ""), wxConvUTF8) );
  }
  // resize
  for( int n = 0; n < 9; n++ ) {
    m_EventList->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
    int autoheadersize = m_EventList->GetColumnWidth(n);
    m_EventList->SetColumnWidth(n, wxLIST_AUTOSIZE);
    int autosize = m_EventList->GetColumnWidth(n);
    if(autoheadersize > autosize )
      m_EventList->SetColumnWidth(n, wxLIST_AUTOSIZE_USEHEADER);
  }
}


void SensorEventsDlg::onRefresh( wxCommandEvent& event ) {
  initValues();
}

bool SensorEventsDlg::Validate() {
  wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED );
  event.SetEventObject(m_Refresh);
  wxPostEvent( m_Refresh, event );
  return false;
}

void SensorEventsDlg::onOK( wxCommandEvent& event ) {
  int x,y;
  GetPosition(&x,&y);
  int cx,cy;
  GetSize(&cx,&cy);

  iONode ini = wxGetApp().getIni();
  iONode sensmon = wGui.getsensormonitor(ini);
  if( sensmon == NULL ) {
    sensmon = NodeOp.inst(wSensorMonitor.name(), ini, ELEMENT_NODE);
    NodeOp.addChild(ini, sensmon);
  }
  wSensorMonitor.setx(sensmon,x);
  wSensorMonitor.sety(sensmon,y);
  wSensorMonitor.setcx(sensmon,cx);
  wSensorMonitor.setcy(sensmon,cy);

  wxGetApp().setSensorEventListener(NULL);
  wxGetApp().getFrame()->resetSensorMonitorRef();
  Destroy();

}

void SensorEventsDlg::onClose( wxCloseEvent& event ) {
  wxCommandEvent e;
  onOK(e);
}

