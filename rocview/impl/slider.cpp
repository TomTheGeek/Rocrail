/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 - Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/log.h"
#endif

#include "wx/print.h"
#include <wx/graphics.h>

#include "rocview/public/slider.h"
#include "rocs/public/system.h"
#include "rocs/public/trace.h"


BEGIN_EVENT_TABLE(Slider, wxPanel)

    EVT_MOTION(Slider::mouseMoved)
    EVT_LEFT_DOWN(Slider::mouseDown)
    EVT_LEFT_UP(Slider::mouseReleased)
    EVT_RIGHT_DOWN(Slider::rightClick)
    EVT_LEAVE_WINDOW(Slider::mouseLeftWindow)
    EVT_KEY_DOWN(Slider::keyPressed)
    EVT_KEY_UP(Slider::keyReleased)
    EVT_MOUSEWHEEL(Slider::mouseWheelMoved)

    // catch paint events
    EVT_PAINT(Slider::paintEvent)

END_EVENT_TABLE()



Slider::Slider(wxPanel* parent, int width, int height) : wxPanel(parent)
{
  Width = width;
  Height = height;
  SetMinSize( wxSize(Width, Height) );
  Parent = parent;
  InitSet = false;
  Min = 0;
  Max = 100;
  Value = 0;
  ThumbHeight = 16;
  Drag = false;
  ThumbRange = Height - ThumbHeight;
  ThumbOffset = ThumbHeight / 2;
  ThumbPos = ThumbRange;
  PrevThumbPos = ThumbPos;
  PrevWheelTime = 0;
  Step = (double)ThumbRange / (double)Max;
  TraceOp.trc( "slider", TRCLEVEL_INFO, __LINE__, 9999, "Height=%d Step=%f ThumbPos=%d ThumbRange=%d", Height, Step, ThumbPos, ThumbRange );
}


void Slider::paintEvent(wxPaintEvent & evt)
{
    // depending on your system you may need to look at double-buffered dcs
    wxPaintDC dc(this);
    render(dc);
}

/*
 * Alternatively, you can use a clientDC to paint on the panel
 * at any time. Using this generally does not free you from
 * catching paint events, since it is possible that e.g. the window
 * manager throws away your drawing when the window comes to the
 * background, and expects you will redraw it when the window comes
 * back (by sending a paint event).
 */
void Slider::paintNow()
{
    // depending on your system you may need to look at double-buffered dcs
    wxClientDC dc(this);
    render(dc);
}


/*
 * Here we do the actual rendering. I put it in a separate
 * method so that it can work no matter what type of DC
 * (e.g. wxPaintDC or wxClientDC) is used.
 */
void Slider::render(wxDC&  dc)
{

  if( !IsShownOnScreen() )
    return;

  wxGraphicsContext* gc = wxGraphicsContext::Create(this);
  if (gc) {
#ifdef wxANTIALIAS_DEFAULT
    gc->SetAntialiasMode(wxANTIALIAS_DEFAULT);
#endif
    gc->SetPen( *wxGREY_PEN );
    gc->SetBrush( *wxLIGHT_GREY_BRUSH );

    double tick = (double)ThumbRange / 10.0;
    for( int i = 0; i < 10; i++ ) {

      wxGraphicsPath path = gc->CreatePath();
      path.MoveToPoint(Width/2+4, ThumbRange - (i * tick));
      path.AddLineToPoint(Width/2+4 + 2+i*2, ThumbRange - (i * tick));
      gc->StrokePath(path);

    }

    // make a path that contains a circle and some lines
    gc->SetPen( *wxGREY_PEN );
    gc->SetBrush( *wxLIGHT_GREY_BRUSH );
    gc->DrawRoundedRectangle(Width/2-2, ThumbHeight/2, 4, Height-ThumbHeight, 1.0);

    gc->SetBrush( *wxLIGHT_GREY_BRUSH );
    gc->DrawRoundedRectangle(2+1, ThumbPos+1, Width-4, ThumbHeight, 5.0);
    gc->DrawRoundedRectangle(2, ThumbPos, Width-4, ThumbHeight, 5.0);

    gc->SetPen(*wxGREY_PEN);

    wxGraphicsPath path = gc->CreatePath();
    path.MoveToPoint(4, ThumbPos+4);
    path.AddLineToPoint(4 + Width-8, ThumbPos+4);

    path.MoveToPoint(4, ThumbPos+8);
    path.AddLineToPoint(4 + Width-8, ThumbPos+8);

    path.MoveToPoint(4, ThumbPos+12);
    path.AddLineToPoint(4 + Width-8, ThumbPos+12);
    gc->StrokePath(path);



    delete gc;
  }
}

void Slider::SetValue(int value) {
  if( InitSet && SystemOp.getMillis() - PrevWheelTime < 500 ) {
    return;
  }

  InitSet = true;

  Value = value;
  if( Value < 0 )
    Value = 0;
  if( Value > Max )
    Value = Max;

  Step = (double)ThumbRange / (double)Max;
  // MaxPos = 0, MinPos = ThumbRange
  ThumbPos = ThumbRange - (Value * Step);
  TraceOp.trc( "slider", TRCLEVEL_INFO, __LINE__, 9999, "SET Value=%d Step=%f ThumbPos=%d ThumbRange=%d", Value, Step, ThumbPos, ThumbRange );
  Refresh();
}

int Slider::GetValue() {
  // MaxPos = 0, MinPos = ThumbRange
  double v = (double)(ThumbRange - ThumbPos) / Step;
  Value = v;
  TraceOp.trc( "slider", TRCLEVEL_INFO, __LINE__, 9999, "GET Value=%d Step=%f ThumbPos=%d ThumbRange=%d", Value, Step, ThumbPos, ThumbRange );
  return Value;
}

void Slider::SetRange(int minValue, int maxValue) {
  Min = minValue;
  Max = maxValue;
  SetValue(Value);
}


void Slider::mouseDown(wxMouseEvent& event)
{
  Move = event.m_y;
  Drag = true;
  ThumbPos = Move - (ThumbHeight/2);

  if( ThumbPos < 0 )
    ThumbPos = 0;
  if( ThumbPos > ThumbRange )
    ThumbPos = ThumbRange;

  Refresh();
}
void Slider::mouseReleased(wxMouseEvent& event)
{
  Drag = false;
  moveThumb();
}
void Slider::mouseLeftWindow(wxMouseEvent& event)
{
  if(Drag)
    mouseReleased(event);
}

// currently unused events
void Slider::mouseMoved(wxMouseEvent& event) {
  if( Drag ) {
    int diff = Move - event.m_y;
    Move = event.m_y;

    ThumbPos -= diff;

    if( ThumbPos < 0 )
      ThumbPos = 0;
    if( ThumbPos > ThumbRange )
      ThumbPos = ThumbRange;

    Refresh();
  }
}


void Slider::mouseWheelMoved(wxMouseEvent& event) {
  TraceOp.trc( "slider", TRCLEVEL_INFO, __LINE__, 9999, "mouseWheelMoved %d %d", event.m_wheelDelta, event.m_wheelRotation );
  if( event.m_wheelRotation < 0 ) {
    ThumbPos++;
  }
  else {
    ThumbPos--;
  }

  PrevWheelTime = SystemOp.getMillis();
  moveThumb();
}


void Slider::rightClick(wxMouseEvent& event) {}
void Slider::keyPressed(wxKeyEvent& event) {

}
void Slider::keyReleased(wxKeyEvent& event) {
  if( event.GetKeyCode() == WXK_DOWN ) {
    ThumbPos++;
  }
  else if( event.GetKeyCode() == WXK_UP ) {
    ThumbPos--;
  }
  else {
    return;
  }

  moveThumb();

}

void Slider::moveThumb() {
  if( ThumbPos < 0 )
    ThumbPos = 0;
  if( ThumbPos > ThumbRange )
    ThumbPos = ThumbRange;

  Refresh();
  PrevThumbPos = ThumbPos;
  wxCommandEvent cmdevent( wxEVT_SCROLL_THUMBRELEASE,-1 );
  cmdevent.SetId(-1);
  cmdevent.SetEventObject(this);
  wxPostEvent( Parent, cmdevent);
}


