/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net




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

#ifndef WX_PRECOMP
  #include "wx/wx.h"
#endif

#include <wx/dcbuffer.h>
#include <wx/graphics.h>
#include <wx/math.h>



#include "rocs/public/node.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"
#include "rocs/public/file.h"

#include "rocview/public/meter.h"

Meter::Meter(wxWindow *parent, wxWindowID id, int x, int y)
                  : wxPanel(parent, id,  wxPoint(x, y), wxSize(80,80), wxBORDER_NONE)
{
  m_Parent = parent;
  Connect( wxEVT_PAINT, wxPaintEventHandler( Meter::OnPaint ) );
  //Disconnect( wxEVT_PAINT, wxPaintEventHandler( MyPanel1::OnPaint ) );
  TraceOp.trc( "meter", TRCLEVEL_INFO, __LINE__, 9999, "meter instantiated" );
}


static int modulal(int val)
{
  do{
    val +=360;
  }while(val<0);
  return val;
}

static double getRadians( double degrees ) {
  static double PI= 3.14159265358979;
  return (degrees / 360) * (2.0 * PI);
}


void Meter::OnPaint(wxPaintEvent& event) {
  int width, height;

  //if( !IsShownOnScreen() )
    //return;

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

  TraceOp.trc( "meter", TRCLEVEL_INFO, __LINE__, 9999, "width=%d height=%d", width, height );

  if( height < width )
    width = height;

  double c = width/2;

  wxPen borderPen( wxColour(0, 0, 0), wxSOLID );
  borderPen.SetWidth(1);
  gc->SetPen( borderPen );
  gc->SetBrush(*wxWHITE_BRUSH);

  gc->DrawEllipse(0, 0, width-1, width-1);


  int i;
  wxGraphicsPath platePath = gc->CreatePath();
  wxPen platePen( wxColour(0, 0, 0), wxSOLID );
  for (i = 0; i < 25; i++) {
    double k = ((modulal((90 - (i * 6))) * M_PI) / 180);

    if( i%6 == 0 ) {
      platePath.MoveToPoint(c + 0.75 * c * cos(k), c - 0.75 * c * sin(k));
      platePath.AddLineToPoint(c + 0.95 * c * cos(k), c - 0.95 * c * sin(k));
    }
  }
  for (i = 36; i < 60; i++) {
    double k = ((modulal((90 - (i * 6))) * M_PI) / 180);

    if( i%6 == 0 ) {
      platePath.MoveToPoint(c + 0.75 * c * cos(k), c - 0.75 * c * sin(k));
      platePath.AddLineToPoint(c + 0.95 * c * cos(k), c - 0.95 * c * sin(k));
    }
  }
  platePen.SetWidth(2);
  gc->SetPen( platePen );
  gc->StrokePath(platePath);

  platePath = gc->CreatePath();
  for (i = 0; i < 25; i++) {
    double k = ((modulal((90 - (i * 6))) * M_PI) / 180);

    if( i%3 == 0 ) {
      platePath.MoveToPoint(c + 0.75 * c * cos(k), c - 0.75 * c * sin(k));
      platePath.AddLineToPoint(c + 0.85 * c * cos(k), c - 0.85 * c * sin(k));
    }
  }
  for (i = 36; i < 60; i++) {
    double k = ((modulal((90 - (i * 6))) * M_PI) / 180);

    if( i%3 == 0 ) {
      platePath.MoveToPoint(c + 0.75 * c * cos(k), c - 0.75 * c * sin(k));
      platePath.AddLineToPoint(c + 0.85 * c * cos(k), c - 0.85 * c * sin(k));
    }
  }
  platePen.SetWidth(1);
  gc->SetPen( platePen );
  gc->StrokePath(platePath);

  platePath = gc->CreatePath();
  platePath.AddArc(width/2, width/2, width/2-8, getRadians(240), getRadians(120), true);
  platePen.SetWidth(1);
  gc->SetPen( platePen );
  gc->StrokePath(platePath);


  double b = width / 100.0;
  wxFont font(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
  gc->SetFont(font,*wxBLACK);
  gc->DrawText( wxT("0"), b * 25.0, b * 70.0 );

  double speed = 36;
  drawNeedle(gc, c, ((modulal((90 - (speed * 6))) * M_PI) / 180));

  delete gc;

}


void Meter::drawNeedle(wxGraphicsContext* gc, double c, double speed, bool erase) {
  gc->SetBrush( erase?*wxWHITE_BRUSH:wxColour(255, 0, 0) );
  wxPen redPen( erase?wxColour(255, 255, 255):wxColour(255, 0, 0), wxSOLID );
  redPen.SetWidth(2);
  gc->SetPen( redPen );

  wxGraphicsPath path = gc->CreatePath();
  path.MoveToPoint(c, c);
  path.AddLineToPoint(c + 0.80 * c * cos(speed), c - 0.80 * c * sin(speed));
  gc->StrokePath(path);

  wxPen blackPen( wxColour(0, 0, 0), wxSOLID );
  blackPen.SetWidth(1);
  gc->SetPen( blackPen );
  gc->SetBrush(*wxBLACK_BRUSH);
  gc->DrawEllipse(c-3, c-3, 6, 6);

}




