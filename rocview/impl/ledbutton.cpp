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

#include "rocview/public/ledbutton.h"


BEGIN_EVENT_TABLE(LEDButton, wxPanel)

    EVT_MOTION(LEDButton::mouseMoved)
    EVT_LEFT_DOWN(LEDButton::mouseDown)
    EVT_LEFT_UP(LEDButton::mouseReleased)
    EVT_RIGHT_DOWN(LEDButton::rightClick)
    EVT_LEAVE_WINDOW(LEDButton::mouseLeftWindow)
    EVT_KEY_DOWN(LEDButton::keyPressed)
    EVT_KEY_UP(LEDButton::keyReleased)
    EVT_MOUSEWHEEL(LEDButton::mouseWheelMoved)

    // catch paint events
    EVT_PAINT(LEDButton::OnPaint)

END_EVENT_TABLE()



LEDButton::LEDButton(wxPanel* parent, wxString text, int width, int height, bool useLED, bool textOnly)
  :wxPanel(parent, -1,  wxPoint(0, 0), wxSize(width,height), wxBORDER_NONE)
{
  buttonWidth = width;
  buttonHeight = height;
  SetSize( wxSize(buttonWidth, buttonHeight) );
  this->text = text;
  pressedDown = false;
  ON = false;
  Parent = parent;
  this->useLED = useLED;
  this->textOnly = textOnly;
}

/*
 * Called by the system of by wxWidgets when the panel needs
 * to be redrawn. You can also trigger this call by
 * calling Refresh()/Update().
 */

void LEDButton::OnPaint(wxPaintEvent& WXUNUSED(event))
{
  wxPaintDC dc(this);

  if( !IsShownOnScreen() )
    return;

  wxGraphicsContext* gc = wxGraphicsContext::Create(this);

  if( gc == NULL )
    return;

#ifdef wxANTIALIAS_DEFAULT
  gc->SetAntialiasMode(wxANTIALIAS_DEFAULT);
#endif
  // make a path that contains a circle and some lines
  if (pressedDown)
    gc->SetBrush( *wxGREY_BRUSH );
  else
    gc->SetBrush( *wxLIGHT_GREY_BRUSH );

  wxPen pen( wxColour(100, 100, 100), wxSOLID );
  pen.SetWidth(1);
  gc->SetPen( pen );

  gc->SetBrush( pressedDown ? *wxGREY_BRUSH:*wxLIGHT_GREY_BRUSH );
  gc->DrawRoundedRectangle(1, 1, buttonWidth-2, buttonHeight-2, 5.0);
  gc->DrawRoundedRectangle(0, 0, buttonWidth-2, buttonHeight-2, 5.0);

  if( useLED ) {
    gc->SetBrush( ON ? wxBrush(wxColour(255,255,0)):*wxLIGHT_GREY_BRUSH );
    gc->DrawEllipse(2.5, 2.5, 7.0, 7.0);
  }
  wxFont font(textOnly?14:10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  gc->SetFont(font,*wxBLACK);

  if(textOnly) {
    Display7Segement(gc);
  }
  else {
    double width;
    double height;
    double descent;
    double externalLeading;
    gc->GetTextExtent( text,(wxDouble*)&width,(wxDouble*)&height,(wxDouble*)&descent,(wxDouble*)&externalLeading);
    gc->DrawText( text, (buttonWidth-width)/2, (buttonHeight-height)/2 );
  }
  delete gc;

}


void LEDButton::draw7Segement(wxGraphicsContext* gc, int segval, int segoff) {
  int segwidth = (buttonWidth-12) / 4;
  int segheight = buttonHeight - 8;
  // horizontal
  wxPen pen( wxColour(100, 100, 100), wxSOLID );
  pen.SetWidth(1);
  gc->SetPen( pen );
  gc->SetBrush( *wxBLACK_BRUSH );

  if( segval == 0 || segval == 2 || segval == 3 || segval == 5 || segval == 6 || segval == 7 || segval == 8 || segval == 9 )
    gc->DrawRectangle(segoff, 4, segwidth, 2 );

  if( segval == 2 || segval == 3 || segval == 4  || segval == 5 || segval == 6 || segval == 8 || segval == 9 )
    gc->DrawRectangle(segoff, 4 + segheight/2 - 1, segwidth, 2 );

  if( segval == 0 || segval == 2 || segval == 3 || segval == 5 || segval == 6 || segval == 8 || segval == 9 )
    gc->DrawRectangle(segoff, 4 + segheight - 2, segwidth, 2 );



  // vertical
  if( segval == 0 || segval == 4 || segval == 5 || segval == 6 || segval == 8 || segval == 9 )
    gc->DrawRectangle(segoff, 4, 2, segheight/2 );

  if( segval == 0 || segval == 2 || segval == 6 || segval == 8 )
    gc->DrawRectangle(segoff, 4 + segheight/2, 2, segheight/2 );

  if( segval == 0 || segval == 1 || segval == 2 || segval == 3 || segval == 4 || segval == 7 || segval == 8 || segval == 9 )
    gc->DrawRectangle(segoff+segwidth-2, 4, 2, segheight/2 );

  if( segval == 0 || segval == 1 || segval == 3 || segval == 4 || segval == 5 || segval == 6 || segval == 7 || segval == 8 || segval == 9 )
    gc->DrawRectangle(segoff+segwidth-2, 4 + segheight/2, 2, segheight/2 );
}


void LEDButton::Display7Segement(wxGraphicsContext* gc) {
  int segwidth = (buttonWidth-12) / 4;
  int segheight = buttonHeight - 8;
  int seggap = segwidth / 4;
  int val = atoi(text.mb_str(wxConvUTF8));
  int val100 = val / 100;
  int val10 = (val % 100) / 10;
  int val1 = val % 10;


  if( val100 > 0 )
    draw7Segement(gc, val100, 6 + seggap);
  if( val100 > 0 || val10 > 0 )
    draw7Segement(gc, val10, 6 + seggap*2 + segwidth);
  draw7Segement(gc, val1, 6 + seggap*3 + segwidth*2);
}


void LEDButton::setLED(bool ON) {
  this->ON = ON;
  Refresh();
}

void LEDButton::SetLabel(const wxString &text) {
  this->text = text;
  Refresh();
}

void LEDButton::SetValue(const wxString& value) {
  SetLabel(value);
}

bool LEDButton::SetBackgroundColour(const wxColour& colour) {
}


void LEDButton::mouseDown(wxMouseEvent& event)
{
  if( textOnly )
    return;

    pressedDown = true;
    Refresh();
}
void LEDButton::mouseReleased(wxMouseEvent& event)
{
  if( textOnly )
    return;

    pressedDown = false;
    Refresh();

    wxCommandEvent bttn_event( wxEVT_COMMAND_BUTTON_CLICKED,-1 );
    bttn_event.SetId(-1);
    bttn_event.SetEventObject(this);
    wxPostEvent( Parent, bttn_event);
}
void LEDButton::mouseLeftWindow(wxMouseEvent& event)
{
    if (pressedDown)
    {
        pressedDown = false;
        Refresh();
    }
}

// currently unused events
void LEDButton::mouseMoved(wxMouseEvent& event) {}
void LEDButton::mouseWheelMoved(wxMouseEvent& event) {}
void LEDButton::rightClick(wxMouseEvent& event) {}
void LEDButton::keyPressed(wxKeyEvent& event) {}
void LEDButton::keyReleased(wxKeyEvent& event) {}

