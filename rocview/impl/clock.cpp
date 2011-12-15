/**
* Name:    wxAnalogClock.h
* Purpose:     Analog clock
* Author:      emarti, Murat Özdemir  e-mail: dtemarti<add>gmail<dot>com
* Created:     15.04.2006
* Copyright:   (c) emarti
* Licence:     wxWindows license
*/

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
    #pragma implementation "rocgui.cpp"
    #pragma interface "rocgui.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif


#include "rocs/public/node.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"
#include "rocs/public/file.h"

#include "rocrail/wrapper/public/Clock.h"

#include "rocview/public/clock.h"
#include "rocview/dialogs/clockdialog.h"
#include "rocview/public/guiapp.h"
#include "rocview/public/guiframe.h"
#include "rocview/res/icons.hpp"

BEGIN_EVENT_TABLE(Clock, wxPanel)
  EVT_PAINT(Clock::OnPaint)
  EVT_TIMER(ID_WXTIMER, Clock::Timer)
  EVT_RIGHT_UP(Clock::OnPopup)
  EVT_MENU( ME_AdjustTime , Clock::OnAdjustTime )
  EVT_MENU( ME_FreezeTime , Clock::OnFreezeTime )
  EVT_MENU( ME_ResumeTime , Clock::OnResumeTime )
END_EVENT_TABLE()

Clock::Clock(wxWindow *parent, wxWindowID id, int x, int y,int handwidth, int p_devider, int clocktype)
                  : wxPanel(parent, id,  wxPoint(x, y), wxSize(100,100), wxBORDER_NONE)
{
  start = true;
  run   = true;
  deviderchanged = false;
  m_Plate = _img_plate;
  m_Logo  = _img_logo;
  m_Temp = 20;

	clockpicwidth = m_Plate->GetWidth();
  SetSize(wxSize(clockpicwidth, clockpicwidth));
  hw = handwidth;
  type = clocktype;

  devider = p_devider;
  if( devider <= 0 )
    devider = 1;

  datetime = new wxDateTime();
  datetime->SetToCurrent();
  ltime = datetime->GetTimeNow();

	WxTimer = new wxTimer();
	WxTimer->SetOwner(this, ID_WXTIMER);
	WxTimer->Start(TIMER/devider);
  TraceOp.trc( "clock", TRCLEVEL_INFO, __LINE__, 9999, "clock instantiated" );
}

int modulal(int val)
{
	do{
		val +=360;
	}while(val<0);
	return val;
}

int kactane(int x, int y)
{
	int times=0;
	do{
		x-=y;
		times++;
	}while(x>=y);
	return times;
}

double sm_angle(int m)
{
	// Values of m must be minute or second.
	return ((modulal((90 - (m * 6))) * M_PI) / 180);
}

double h_angle(int m, int n)
{
	// Values of m must be hour, values of n must be minute.
	return ((modulal((90 - (m * 30) - (kactane(n, 6) * 3))) * M_PI) / 180);
}

void Clock::OnPopup(wxMouseEvent& event) {

  wxMenu menu( wxGetApp().getMenu("clock") );
  menu.Append( ME_AdjustTime, wxGetApp().getMenu("adjusttime") );
  menu.Append( ME_FreezeTime, wxGetApp().getMenu("freezetime") );
  menu.Append( ME_ResumeTime, wxGetApp().getMenu("resumetime") );
  PopupMenu(&menu, event.GetX(), event.GetY() );
}

void Clock::OnAdjustTime(wxCommandEvent& event) {
  ClockDialog* dlg = new ClockDialog( this );
  dlg->setClock( devider, datetime->GetHour(), datetime->GetMinute(), m_Temp );
  if( wxID_OK == dlg->ShowModal() ) {
    int hour, minute;
    dlg->getClock( &devider, &hour, &minute, &m_Temp );

    datetime->SetHour( hour );
    datetime->SetMinute( minute );

    ltime = datetime->GetTicks();

    // send to rocrail
    iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
    wClock.setdivider( tick, devider );
    wClock.settime( tick, ltime );
    wClock.settemp( tick, m_Temp );
    wxGetApp().sendToRocrail( tick, false );
  }
}


void Clock::OnFreezeTime(wxCommandEvent& event) {
  // send to rocrail
  iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
  wClock.setcmd( tick, wClock.freeze );
  wxGetApp().sendToRocrail( tick, false );
}


void Clock::OnResumeTime(wxCommandEvent& event) {
  // send to rocrail
  iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
  wClock.setcmd( tick, wClock.go );
  wxGetApp().sendToRocrail( tick, false );
}


void Clock::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  if(m_Plate && m_Plate->Ok())
  {
    //TraceOp.trc( "clock", TRCLEVEL_INFO, __LINE__, 9999, "clock paint" );
		wxPaintDC dc(this);
		double c = clockpicwidth / 2;

    wxPen pen(GetBackgroundColour(),hw,wxSOLID);
    // clear pre-now
    dc.SetPen(pen);
    dc.DrawLine((int)c, (int)c, (int)(c + 0.90 * c * cos(xpre)), (int)(c - 0.90 * c * sin(xpre))); // pre-second hand

    pen.SetColour(*wxBLACK);
    dc.SetPen(pen);

    if( type > 0 ) {
  		wxString timestring;
  		if( type == 1 && hours < 12 )
  		  timestring = wxString::Format(_T("AM"), hours, minutes);
  		else if( type == 1 && hours >= 12 )
        timestring = wxString::Format(_T("PM"), hours, minutes);
  		else if( type == 2 )
        timestring = wxString::Format(_T("%02d:%02d"), hours, minutes);
  		int w = 0;
      int h = 0;
  		dc.GetTextExtent(timestring, &w, &h);
  		dc.DrawText(timestring, 50-(w/2),63);
		}

    if(m_Logo != NULL && m_Logo->Ok()) {
      int w = m_Logo->GetWidth();
      dc.DrawBitmap(wxBitmap(*m_Logo),50-(w/2),28,true);
    }

		// draw now
    if(m_Plate != NULL)
      dc.DrawBitmap(wxBitmap(*m_Plate),0,0,true);

		pen.SetColour(*wxBLACK);
		pen.SetWidth(hw*2);
		dc.SetPen(pen);
		dc.DrawLine((int)c, (int)c, (int)(c + 0.6 * c * cos(z)), (int)(c - 0.6  * c * sin(z))); // hour hand
		pen.SetWidth(hw*2);
		dc.SetPen(pen);
		dc.DrawLine((int)c, (int)c, (int)(c + 0.85 * c * cos(y)), (int)(c - 0.85  * c * sin(y))); // minute hand
		pen.SetWidth(hw);
		pen.SetColour(*wxRED);
		dc.SetPen(pen);
		dc.DrawLine((int)c, (int)c, (int)(c + 0.90 * c * cos(x)), (int)(c - 0.90 * c * sin(x))); // second hand

		pen.SetColour(*wxWHITE);
    dc.SetPen(pen);
    dc.DrawPoint(50,50);
  }
  else {
    TraceOp.trc( "clock", TRCLEVEL_WARNING, __LINE__, 9999, "no valid plate" );
  }
}

void Clock::stopTimer() {
  WxTimer->Stop();
}

void Clock::SyncClock(iONode node) {
  if( StrOp.equals( wClock.getcmd(node), wClock.freeze )) {
    run = false;
  }
  else if( StrOp.equals( wClock.getcmd(node), wClock.go )) {
    run = true;
    SetDevider( wClock.getdivider(node) );
    SetTime( wClock.gettime(node) );
  }
  else {
    SetDevider( wClock.getdivider(node) );
    SetTime( wClock.gettime(node) );
    m_Temp = wClock.gettemp(node);
  }
}


void Clock::SetDevider(int p_devider) {
  if( devider != p_devider ) {
    devider = p_devider;
    TraceOp.trc( "clock", TRCLEVEL_INFO, __LINE__, 9999, "devider set to %d.", devider );
  }
  deviderchanged = true;
}

void Clock::SetTime(long p_time) {
  ltime = p_time;
}


void Clock::Timer(wxTimerEvent& WXUNUSED(event))
{
  if( deviderchanged ) {
    deviderchanged = false;
    WxTimer->Start(TIMER/devider);
  }

  if( !run ) {
    return;
  }

  //if( devider > 1 ) {
  if( 1 ) {
    ltime++;
    datetime->Set( ltime );
  }
  else {
    ltime = wxDateTime::GetTimeNow();
    datetime->Set( ltime );
  }
  SetToolTip( datetime->FormatISOTime());

  if ((datetime->GetSecond() == 0) || start) Refresh(true);
  start = false;

  x = sm_angle(datetime->GetSecond());
  xpre = sm_angle(datetime->GetSecond()-1);
  y = sm_angle(datetime->GetMinute());
  z = h_angle(datetime->GetHour(),datetime->GetMinute());
  hours   = datetime->GetHour();
  minutes = datetime->GetMinute();

  Refresh(true);
}
