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

#include <wx/dcbuffer.h>


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

Clock::Clock(wxWindow *parent, wxWindowID id, int x, int y,int handwidth, int p_devider, int clocktype, bool showsecondhand)
                  : wxPanel(parent, id,  wxPoint(x, y), wxSize(110,110), wxBORDER_NONE)
{
  start = true;
  run   = true;
  deviderchanged = false;
  this->showsecondhand = showsecondhand;
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
    datetime->SetSecond( 0 );

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

#define USENEWLOOK
void Clock::OnPaint(wxPaintEvent& event)
{
#ifdef USENEWLOOK
  drawNewClock();
#else
  drawClock();
#endif

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
  //ltime = (p_time / 60) * 60; // Filter out the seconds.
  ltime = p_time; // Filter out the seconds.
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
    TraceOp.trc( "clock", TRCLEVEL_DEBUG, __LINE__, 9999, "clock timer" );
    Refresh(false);
  }
}



void Clock::drawClock() {
  int width, height;
  wxPaintDC dc(this);
  GetSize(&width, &height);

  TraceOp.trc( "clock", TRCLEVEL_DEBUG, __LINE__, 9999, "draw clock" );

  if( height < width )
    width = height;

  double c = width/2;

#if defined __APPLE__
  wxPen borderPen( wxColour(0, 0, 0), wxSOLID );
  borderPen.SetWidth(1);
  dc.SetPen( borderPen );
  dc.SetBrush(*wxWHITE_BRUSH);
  dc.DrawCircle( c, c, c );

  int i;
  for (i = 0; i < 60; i++) {
    double k = sm_angle( i );

    wxPen pen( wxColour(0, 0, 0), wxSOLID );
    pen.SetWidth(1);
    dc.SetPen( pen );

    dc.DrawLine((int)(c + 0.85 * c * cos(k)), (int)(c - 0.85 * c * sin(k)), (int)(c + 0.90 * c * cos(k)), (int)(c - 0.90 * c * sin(k)));

    if( i%5 == 0 ) {
      pen.SetWidth(4);
      dc.SetPen( pen );
      dc.DrawLine((int)(c + 0.75 * c * cos(k)), (int)(c - 0.75 * c * sin(k)), (int)(c + 0.90 * c * cos(k)), (int)(c - 0.90 * c * sin(k)));
    }
  }
#else
  width = m_Plate->GetWidth();
  c = width/2;

  dc.SetPen(*wxWHITE_PEN);
  dc.SetBrush(*wxWHITE_BRUSH);
  dc.DrawCircle( c-1, c-1, c );

  // draw now
  if(m_Plate != NULL)
    dc.DrawBitmap(wxBitmap(*m_Plate),0,0,true);
#endif

  dc.SetPen(*wxBLACK_PEN);

  if( type > 0 ) {
    wxString timestring;
    if( type == 1 && hours < 12 )
      timestring = wxString::Format(_T("AM %d:%02d"), hours, minutes);
    else if( type == 1 && hours >= 12 )
      timestring = wxString::Format(_T("PM %d:%02d"), hours-12, minutes);
    else
      timestring = wxString::Format(_T("%02d:%02d"), hours, minutes);
    int w = 0;
    int h = 0;
    dc.GetTextExtent(timestring, &w, &h);
    dc.DrawText(timestring, (width/2)-(w/2),width*0.6);
  }

  if(m_Logo != NULL && m_Logo->Ok()) {
    int w = m_Logo->GetWidth();
    dc.DrawBitmap(wxBitmap(*m_Logo),(width/2)-(w/2),width*0.3,true);
  }


  // hour
  wxPen blackPen( wxColour(0, 0, 0), wxSOLID );
  blackPen.SetWidth(4);
  dc.SetPen( blackPen );
  dc.DrawLine((int)c, (int)c, (int)(c + 0.6 * c * cos(z)), (int)(c - 0.6  * c * sin(z))); // hour hand


  // minute
  dc.DrawLine((int)c, (int)c, (int)(c + 0.85 * c * cos(y)), (int)(c - 0.85  * c * sin(y))); // minute hand


  // second
  if( this->devider <= 10 ) {
    wxBrush brush( wxColour(255, 0, 0), wxSOLID );
    dc.SetBrush( brush );
    wxPen redPen( wxColour(255, 0, 0), wxSOLID );
    redPen.SetWidth(2);
    dc.SetPen( redPen );
    //dc.DrawLine((int)c, (int)c, (int)(c + 0.90 * c * cos(x)), (int)(c - 0.90 * c * sin(x))); // second hand
#if defined __APPLE__
    dc.DrawLine((int)c, (int)c, (int)(c + 0.52 * c * cos(x)), (int)(c - 0.52 * c * sin(x))); // second hand
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawCircle((int)(c + 0.60 * c * cos(x)), (int)(c - 0.60 * c * sin(x)), 4); // second hand
    dc.DrawLine((int)(c + 0.68 * c * cos(x)), (int)(c - 0.68 * c * sin(x)), (int)(c + 0.90 * c * cos(x)), (int)(c - 0.90 * c * sin(x))); // second hand
#else
    dc.DrawLine((int)c, (int)c, (int)(c + 0.90 * c * cos(x)), (int)(c - 0.90 * c * sin(x))); // second hand
#endif
  }
  dc.SetBrush(*wxRED_BRUSH);
  dc.DrawCircle( c, c, 2 );


}




void Clock::drawNewClock() {
  int width, height;

  if( !IsShownOnScreen() )
    return;

  TraceOp.trc( "clock", TRCLEVEL_DEBUG, __LINE__, 9999, "draw new clock" );
#if defined __WIN32
  SetBackgroundStyle(wxBG_STYLE_CUSTOM);
  wxBufferedPaintDC dc(this);
  wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
#else
  wxPaintDC dc(this);
  wxGraphicsContext* gc = wxGraphicsContext::Create(dc);
#endif

#ifdef wxANTIALIAS_DEFAULT
  gc->SetAntialiasMode(wxANTIALIAS_DEFAULT);
#endif

  GetSize(&width, &height);

#if defined __WIN32
  //Background workaround
  gc->SetBrush(this->GetBackgroundColour());
  gc->DrawRectangle(0, 0, width, height);
#endif

  if( height < width )
    width = height;

  double c = width/2;

  wxPen borderPen( wxColour(0, 0, 0), wxSOLID );
  borderPen.SetWidth(1);
  gc->SetPen( borderPen );
  gc->SetBrush(*wxWHITE_BRUSH);

  gc->DrawEllipse(0, 0, width-1, width-1);

  drawSecondHand(gc, c, true);


  int i;
  wxGraphicsPath platePath = gc->CreatePath();
  wxPen platePen( wxColour(0, 0, 0), wxSOLID );
  for (i = 0; i < 60; i++) {
    double k = sm_angle( i );

    platePath.MoveToPoint(c + 0.85 * c * cos(k), c - 0.85 * c * sin(k));
    platePath.AddLineToPoint(c + 0.90 * c * cos(k), c - 0.90 * c * sin(k));
    gc->StrokePath(platePath);

  }
  platePen.SetWidth(1);
  gc->SetPen( platePen );
  gc->StrokePath(platePath);

  platePath = gc->CreatePath();
  for (i = 0; i < 60; i++) {
    double k = sm_angle( i );

    if( i%5 == 0 ) {
      platePath.MoveToPoint(c + 0.75 * c * cos(k), c - 0.75 * c * sin(k));
      platePath.AddLineToPoint(c + 0.90 * c * cos(k), c - 0.90 * c * sin(k));
    }
  }
  platePen.SetWidth(4);
  gc->SetPen( platePen );
  gc->StrokePath(platePath);

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
  drawSecondHand(gc, c);

  gc->SetBrush(*wxRED_BRUSH);
  gc->DrawEllipse(c-2, c-2, 4, 4);


  delete gc;


}

void Clock::drawSecondHand(wxGraphicsContext* gc, double c, bool erase) {
  // second
  if( showsecondhand && this->devider <= 10 ) {
    gc->SetBrush( erase?*wxWHITE_BRUSH:wxColour(255, 0, 0) );
    wxPen redPen( erase?wxColour(255, 255, 255):wxColour(255, 0, 0), wxSOLID );
    redPen.SetWidth(2);
    gc->SetPen( redPen );

    //dc.DrawLine((int)c, (int)c, (int)(c + 0.52 * c * cos(x)), (int)(c - 0.52 * c * sin(x))); // second hand
    wxGraphicsPath path = gc->CreatePath();
    path.MoveToPoint(c, c);
    path.AddLineToPoint(c + 0.52 * c * cos(x), c - 0.52 * c * sin(x));
    gc->StrokePath(path);


    gc->SetBrush(erase?*wxWHITE_BRUSH:*wxTRANSPARENT_BRUSH);
    //dc.DrawCircle((int)(c + 0.60 * c * cos(x)), (int)(c - 0.60 * c * sin(x)), 4); // second hand
    gc->DrawEllipse(c - 4 + 0.60 * c * cos(x), c - 4 - 0.60 * c * sin(x), 8, 8);

    //dc.DrawLine((int)(c + 0.68 * c * cos(x)), (int)(c - 0.68 * c * sin(x)), (int)(c + 0.90 * c * cos(x)), (int)(c - 0.90 * c * sin(x))); // second hand
    path = gc->CreatePath();
    path.MoveToPoint(c + 0.68 * c * cos(x), c - 0.68 * c * sin(x));
    path.AddLineToPoint(c + 0.90 * c * cos(x), c - 0.90 * c * sin(x));
    gc->StrokePath(path);
  }
}



