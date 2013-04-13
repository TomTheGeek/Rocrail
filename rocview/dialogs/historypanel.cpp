/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  dc.SetPen( *wxBLACK_PEN );
  wxPen pen = dc.GetPen();
  pen.SetWidth(1);
  dc.SetPen(pen);

  int w = 0;
  int h = 0;
  GetSize(&w, &h);

  TraceOp.trc( "histopanel", TRCLEVEL_INFO, __LINE__, 9999, "width=%d height=%d", w, h );

  if( m_Booster != NULL ) {
    int x = 0;
    int y = h;
    iONode boosterevent = wBooster.getboosterevent(m_Booster);
    while( boosterevent != NULL ) {
      int mA = wBoosterEvent.getload(boosterevent);
      TraceOp.trc( "histopanel", TRCLEVEL_INFO, __LINE__, 9999, "line %d,%d %d,%d", x, y, x+1, h-mA );
      dc.DrawLine( x, y, x+1, h-mA );
      x+=1;
      y = h-mA;
      boosterevent = wBooster.nextboosterevent( m_Booster, boosterevent );
    }

  }
}


