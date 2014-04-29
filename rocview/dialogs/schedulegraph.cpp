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


#include "schedulegraph.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"

#include "rocs/public/trace.h"
#include "rocs/public/map.h"

ScheduleGraph::ScheduleGraph(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
  wxPanel::Create(parent, id, pos, size, style);
  m_Schedule = NULL;
  this->Connect( wxEVT_PAINT, wxPaintEventHandler( ScheduleGraph::OnPaint ), NULL, this );
}


ScheduleGraph::~ScheduleGraph()
{
  this->Disconnect( wxEVT_PAINT, wxPaintEventHandler( ScheduleGraph::OnPaint ), NULL, this );
}

void ScheduleGraph::OnPaint(wxPaintEvent& event)
{
  wxPaintDC dc(this);
  dc.SetBackground(*wxWHITE_BRUSH);
  dc.Clear();

  if( m_Schedule == NULL )
    return;

  int w = 0;
  int h = 0;
  GetSize(&w, &h);

  int nrentries = 0;
  iOMap map = MapOp.inst();
  iONode scentry = wSchedule.getscentry( m_Schedule );
  while( scentry != NULL ) {
    const char* location = wScheduleEntry.getlocation(scentry);
    const char* block = wScheduleEntry.getblock(scentry);
    if( location != NULL && StrOp.len(location) > 0 ) {
      if( !MapOp.haskey(map, location) ) {
        MapOp.put(map, location, (obj)scentry);
        nrentries++;
      }
    }
    else if( block != NULL && StrOp.len(block) > 0 ) {
      if( !MapOp.haskey(map, block) ) {
        MapOp.put(map, block, (obj)scentry);
        nrentries++;
      }
    }
    scentry = wSchedule.nextscentry( m_Schedule, scentry );
  };


  if( nrentries > 1 ) {
    dc.SetPen( *wxGREY_PEN );
    wxPen pen = dc.GetPen();
    pen.SetWidth(1);
    dc.SetPen(pen);

    float margin = 10.0;
    float v = (float)((w-2*margin) / (nrentries-1));
    iONode scentry = (iONode)MapOp.first(map);
    for( int i = 0; i < nrentries; i++) {
      const char* location = wScheduleEntry.getlocation(scentry);
      const char* block = wScheduleEntry.getblock(scentry);
      dc.DrawLine( i*v+margin, 50, i*v+margin, h );
      if( location != NULL && StrOp.len(location) > 0 )
        dc.DrawRotatedText( wxString(location,wxConvUTF8), i*v+margin-10, 50, 90 );
      else
        dc.DrawRotatedText( wxString(block,wxConvUTF8), i*v+margin-10, 50, 90 );
      scentry = (iONode)MapOp.next(map);
    }
  }

  MapOp.base.del(map);
}

void ScheduleGraph::setSchedule(iONode schedule) {
  TraceOp.trc( "scgraph", TRCLEVEL_INFO, __LINE__, 9999, "Set Schedule: %s", wSchedule.getid(schedule) );
  m_Schedule = schedule;
  Refresh();
}

