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

#ifndef __Meter_H__
#define __Meter_H__

#define ID_WXTIMER  999
#define TIMER   1000

#include <wx/datetime.h>
#include <wx/image.h>
#include <wx/math.h>
#include <wx/timer.h>
#include <wx/panel.h>

class Meter : public wxPanel{
  public:
    Meter(wxWindow *parent, wxWindowID id, int x, int y);
    void OnPaint(wxPaintEvent& event);
    void drawNeedle(wxGraphicsContext* gc, double c, bool erase=false);
    void setSpeed(int speed);
  private:
    wxWindow* m_Parent;
    int m_iSpeed;
    double speed;

};

#endif
