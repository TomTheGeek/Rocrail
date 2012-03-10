/**
* Name:    wxAnalogClock.h
* Purpose:     Analog clock
* Author:      emarti, Murat Özdemir  e-mail: dtemarti<add>gmail<dot>com
* Created:     15.04.2006
* Copyright:   (c) emarti
* Licence:     wxWindows license
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
    Clock(wxWindow *parent, wxWindowID id, int x, int y, int handwidth, int devider=1, int clocktype=0);

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
    void drawSecondHand(wxGraphicsContext* gc, double c, bool erase=false);

    // some useful events
    void mouseMoved(wxMouseEvent& event);
    void mouseDown(wxMouseEvent& event);
    void mouseWheelMoved(wxMouseEvent& event);
    void mouseReleased(wxMouseEvent& event);
    void rightClick(wxMouseEvent& event);
    void mouseLeftWindow(wxMouseEvent& event);
    void keyPressed(wxKeyEvent& event);
    void keyReleased(wxKeyEvent& event);


	private:
		wxBitmap* m_Plate;
    wxBitmap* m_Logo;
		wxTimer *WxTimer;
		double x,y,z,xpre;
		bool start;
    wxDateTime* datetime;
    time_t ltime;
    int devider;
    int m_Temp;
    bool deviderchanged;
    bool run;
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

