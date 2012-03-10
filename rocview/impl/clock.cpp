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
#include <wx/graphics.h>


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
                  : wxPanel(parent, id,  wxPoint(x, y), wxSize(110,110), wxBORDER_NONE)
{
  start = true;
  run   = true;
  deviderchanged = false;
  m_Plate = _img_plate;
  m_Logo  = _img_logo;
  m_Temp = 20;

	int clockpicwidth = m_Plate->GetWidth();
  SetSize(wxSize(clockpicwidth, clockpicwidth));
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

	calculate();

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

    //TraceOp.trc( "clock", TRCLEVEL_INFO, __LINE__, 9999, "clock paint" );
		//wxPaintDC dc(this);
    drawClock();
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
  ltime = (p_time / 60) * 60 - 60; // Filter out the seconds.
  if( run ) {
    calculate();
    Refresh(true);
  }
}

void Clock::calculate() {
  if( 1 ) {
    ltime++;
    datetime->Set( ltime );
  }
  else {
    ltime = wxDateTime::GetTimeNow();
    datetime->Set( ltime );
  }
  SetToolTip( datetime->FormatISOTime());

  /*
  if ((datetime->GetSecond() == 0) || start)
    Refresh(true);
  start = false;
*/
  x = sm_angle(datetime->GetSecond());
  y = sm_angle(datetime->GetMinute());
  z = h_angle(datetime->GetHour(),datetime->GetMinute());
  hours   = datetime->GetHour();
  minutes = datetime->GetMinute();
}

void Clock::Timer(wxTimerEvent& WXUNUSED(event))
{
  if( deviderchanged ) {
    deviderchanged = false;
    if( this->devider <= 10 ) {
      WxTimer->Start(TIMER/devider);
    }
    else {
      WxTimer->Start((TIMER*60)/devider);
    }
  }

  if( run ) {
    calculate();
    Refresh(false);
  }
}



void Clock::drawClock() {
  int width, height;
  wxPaintDC dc(this);

  if( !IsShownOnScreen() )
    return;

  wxGraphicsContext* gc = wxGraphicsContext::Create(this);
#ifdef wxANTIALIAS_DEFAULT
  gc->SetAntialiasMode(wxANTIALIAS_DEFAULT);
#endif


  GetSize(&width, &height);

  if( height < width )
    width = height;

  double c = width/2;

  gc->SetPen(*wxBLACK_PEN);
  gc->SetBrush(*wxWHITE_BRUSH);
#if defined __linux__ || defined __APPLE__
  gc->DrawEllipse(0, 0, width-1, width-1);
#endif

  int i;
  for (i = 0; i < 60; i++) {
    double k = sm_angle( i );

    wxPen pen( wxColour(0, 0, 0), wxSOLID );
    pen.SetWidth(1);
    gc->SetPen( pen );
    wxGraphicsPath path = gc->CreatePath();
    path.MoveToPoint(c + 0.85 * c * cos(k), c - 0.85 * c * sin(k));
    path.AddLineToPoint(c + 0.90 * c * cos(k), c - 0.90 * c * sin(k));
    gc->StrokePath(path);

    if( i%5 == 0 ) {
      pen.SetWidth(4);
      gc->SetPen( pen );
      wxGraphicsPath path = gc->CreatePath();
      path.MoveToPoint(c + 0.75 * c * cos(k), c - 0.75 * c * sin(k));
      path.AddLineToPoint(c + 0.90 * c * cos(k), c - 0.90 * c * sin(k));
      gc->StrokePath(path);
    }
  }

  gc->SetPen(*wxBLACK_PEN);

  if( type > 0 ) {
    wxString timestring;
    if( type == 1 && hours < 12 )
      timestring = wxString::Format(_T("AM %d:%02d"), hours, minutes);
    else if( type == 1 && hours >= 12 )
      timestring = wxString::Format(_T("PM %d:%02d"), hours-12, minutes);
    else
      timestring = wxString::Format(_T("%02d:%02d"), hours, minutes);

    wxFont font(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    gc->SetFont(font,*wxBLACK);

    double w;
    double h;
    double descent;
    double externalLeading;
    gc->GetTextExtent( timestring,(wxDouble*)&w,(wxDouble*)&h,(wxDouble*)&descent,(wxDouble*)&externalLeading);
    gc->DrawText( timestring, (width/2)-(w/2),width*0.6 );


  }

  if(m_Logo != NULL && m_Logo->Ok()) {
    int w = m_Logo->GetWidth();
    int h = m_Logo->GetHeight();
    gc->DrawBitmap(wxBitmap(*m_Logo),(width/2)-(w/2),width*0.3,w,h);
  }


  // hour
  wxPen blackPen( wxColour(0, 0, 0), wxSOLID );
  blackPen.SetWidth(4);
  gc->SetPen( blackPen );
  wxGraphicsPath hpath = gc->CreatePath();
  hpath.MoveToPoint(c, c);
  hpath.AddLineToPoint(c + 0.6 * c * cos(z), c - 0.6  * c * sin(z));
  gc->StrokePath(hpath);


  // minute
  //dc.DrawLine((int)c, (int)c, (int)(c + 0.85 * c * cos(y)), (int)(c - 0.85  * c * sin(y))); // minute hand
  wxGraphicsPath mpath = gc->CreatePath();
  mpath.MoveToPoint(c, c);
  mpath.AddLineToPoint(c + 0.85 * c * cos(y), c - 0.85  * c * sin(y));
  gc->StrokePath(mpath);


  // second
  if( this->devider <= 10 ) {
    wxBrush brush( wxColour(255, 0, 0), wxSOLID );
    gc->SetBrush( brush );
    wxPen redPen( wxColour(255, 0, 0), wxSOLID );
    redPen.SetWidth(2);
    gc->SetPen( redPen );

    //dc.DrawLine((int)c, (int)c, (int)(c + 0.52 * c * cos(x)), (int)(c - 0.52 * c * sin(x))); // second hand
    wxGraphicsPath path = gc->CreatePath();
    path.MoveToPoint(c, c);
    path.AddLineToPoint(c + 0.52 * c * cos(x), c - 0.52 * c * sin(x));
    gc->StrokePath(path);


    gc->SetBrush(*wxTRANSPARENT_BRUSH);
    //dc.DrawCircle((int)(c + 0.60 * c * cos(x)), (int)(c - 0.60 * c * sin(x)), 4); // second hand
    gc->DrawEllipse(c - 4 + 0.60 * c * cos(x), c - 4 - 0.60 * c * sin(x), 8, 8);

    //dc.DrawLine((int)(c + 0.68 * c * cos(x)), (int)(c - 0.68 * c * sin(x)), (int)(c + 0.90 * c * cos(x)), (int)(c - 0.90 * c * sin(x))); // second hand
    path = gc->CreatePath();
    path.MoveToPoint(c + 0.68 * c * cos(x), c - 0.68 * c * sin(x));
    path.AddLineToPoint(c + 0.90 * c * cos(x), c - 0.90 * c * sin(x));
    gc->StrokePath(path);
  }
  gc->SetBrush(*wxRED_BRUSH);
  gc->DrawEllipse(c-2, c-2, 4, 4);


  delete gc;


}

