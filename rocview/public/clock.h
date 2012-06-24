/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

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

#ifndef __Clock_H__
#define __Clock_H__

#define ID_WXTIMER	999
#define TIMER		1000

#include <wx/datetime.h>
#include <wx/image.h>
#include <wx/math.h>
#include <wx/timer.h>
#include <wx/panel.h>

class Clock : public wxPanel{
	public:
    Clock(wxWindow *parent, wxWindowID id, int x, int y, int handwidth, int devider=1, int clocktype=0, bool showsecondhand=true);

    void OnPaint(wxPaintEvent& event);
    void Timer(wxTimerEvent& event);
    void SetDevider(int devider);
    void SetTime(long rocrailtime);
    void OnPopup(wxMouseEvent& event);
    void OnAdjustTime(wxCommandEvent& event);
    void OnFreezeTime(wxCommandEvent& event);
    void OnResumeTime(wxCommandEvent& event);
    void SyncClock( iONode node );
    void stopTimer();
    void calculate();
    void drawClock();
    void drawNewClock();
    void drawSecondHand(wxGraphicsContext* gc, double c, bool erase=false);

	private:
		wxBitmap* m_Plate;
    wxBitmap* m_Logo;
		wxTimer *WxTimer;
		double x,y,z;
		bool start;
    wxDateTime* datetime;
    time_t ltime;
    int devider;
    int m_Temp;
    bool deviderchanged;
    bool run;
    bool showsecondhand;
    int type;
    int hours, minutes;

		DECLARE_EVENT_TABLE()
};

enum {
  ME_AdjustTime = 201,
  ME_FreezeTime,
  ME_ResumeTime,
};

#endif

