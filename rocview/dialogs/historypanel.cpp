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

#include "historypanel.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocrail/wrapper/public/Booster.h"
#include "rocrail/wrapper/public/BoosterEvent.h"

#include "rocs/public/trace.h"

HistoryPanel::HistoryPanel( wxWindow* parent ):wxPanel( parent, wxID_ANY, wxDefaultPosition, wxSize( 200,-1 ) )
{
  m_Booster = NULL;
  this->Connect( wxEVT_PAINT, wxPaintEventHandler( HistoryPanel::OnPaint ), NULL, this );
}

HistoryPanel::~HistoryPanel()
{
  this->Disconnect( wxEVT_PAINT, wxPaintEventHandler( HistoryPanel::OnPaint ), NULL, this );
}


void HistoryPanel::setBooster(iONode booster) {
  m_Booster = booster;
  Refresh();
}


void HistoryPanel::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
  dc.SetBackground(*wxBLACK_BRUSH);
  dc.Clear();

  int w = 0;
  int h = 0;
  GetSize(&w, &h);

  dc.SetPen( *wxLIGHT_GREY_PEN );
  wxPen pen = dc.GetPen();
  pen.SetWidth(1);
  pen.SetStyle(wxDOT);
  dc.SetPen(pen);
  float h10 = (float)h / 10.0;
  for( int i = 1; i < 10; i++) {
    dc.DrawLine( 0, i*h10, w, i*h10 );
  }

  TraceOp.trc( "histopanel", TRCLEVEL_INFO, __LINE__, 9999, "width=%d height=%d", w, h );

  if( m_Booster != NULL ) {
    int count = 0;
    int x = 0;
    int y = h-1;
    int max = 0;
    float scale = 1.0;
    int vy = h-1;
    float voltstep = (float)h / 24.0;
    int ty = h-1;
    float tempstep = (float)h / 100.0;

    iONode boosterevent = wBooster.getboosterevent(m_Booster);
    while( boosterevent != NULL ) {
      int mA = wBoosterEvent.getload(boosterevent);
      if( mA > max && max < 5000 )
        max = mA;
      count++;
      boosterevent = wBooster.nextboosterevent( m_Booster, boosterevent );
    }

    if( count == 0) {
      dc.SetTextBackground(*wxBLACK);
      dc.SetTextForeground(*wxWHITE);
      dc.DrawText(wxT("No data available..."), 10, h/2);
    }

    if( max > h )
      scale = (float)h / (float)(max+5);

    TraceOp.trc( "histopanel", TRCLEVEL_INFO, __LINE__, 9999, "max=%d scale=%f tempstep=%f", max, scale, tempstep );

    boosterevent = wBooster.getboosterevent(m_Booster);
    int skip = 0;
    if( count >= w ) {
      skip = count - (w+1);
    }
    bool first = true;
    while( boosterevent != NULL ) {
      if( skip == 0 ) {
        int volt = (wBoosterEvent.getvolt(boosterevent) * voltstep) / 1000;
        int temp = ((float)wBoosterEvent.gettemp(boosterevent) * tempstep);
        int mA = wBoosterEvent.getload(boosterevent) * scale;
        TraceOp.trc( "histopanel", TRCLEVEL_DEBUG, __LINE__, 9999, "line %d,%d %d,%d", x, y, x+1, (h-1)-mA );
        if( first ) {
          y  = (h-1)-mA;
          vy = (h-1)-volt;
          ty = (h-1)-temp;
          first = false;
        }

        dc.SetPen( *wxWHITE_PEN );
        wxPen pen = dc.GetPen();
        pen.SetWidth(1);
        dc.SetPen(pen);
        dc.DrawLine( x, y, x+1, (h-1)-mA );
        y = (h-1)-mA;

        dc.SetPen( *wxCYAN_PEN );
        pen = dc.GetPen();
        pen.SetWidth(1);
        dc.SetPen(pen);
        dc.DrawLine( x, vy, x+1, (h-1)-volt );
        vy = (h-1)-volt;

        dc.SetPen( *wxRED_PEN );
        pen = dc.GetPen();
        pen.SetWidth(1);
        dc.SetPen(pen);
        dc.DrawLine( x, ty, x+1, (h-1)-temp );
        ty = (h-1)-temp;

        x+=1;
      }
      else {
        skip--;
      }
      boosterevent = wBooster.nextboosterevent( m_Booster, boosterevent );
    }

  }
  else {
    TraceOp.trc( "histopanel", TRCLEVEL_INFO, __LINE__, 9999, "No booster selected..." );
    dc.SetTextBackground(*wxBLACK);
    dc.SetTextForeground(*wxWHITE);
    dc.DrawText(wxT("No booster selected..."), 10, h/2);
  }
}


