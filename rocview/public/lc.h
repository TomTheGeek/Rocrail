/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
#ifndef __ROCRAIL_rocgui_LC_H
#define __ROCRAIL_rocgui_LC_H

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "cv.h"
#endif

/*!
 * Includes
 */

#include "wx/tglbtn.h"

#include "rocs/public/node.h"
#include "rocview/public/clock.h"
#include "rocview/public/ledbutton.h"
#include "rocview/public/slider.h"

#define USENEWLOOK

class LC
{
public:
    /// Constructors
    LC( wxPanel* parent );
    void OnButton(wxCommandEvent& event);
    void OnSlider(wxScrollEvent& event);
    void setLocProps( iONode props );
    void updateLoc( iONode node );
    void SyncClock( iONode node );
    void stopTimer();
    iONode getLocProps();

private:
    /// Creation
    bool Create();
    void init();
    void funCmd(int fidx);
    void speedCmd(bool sendCmd);
#ifdef USENEWLOOK
    bool setButtonColor( LEDButton* button, bool state );
#else
    bool setButtonColor( wxButton* button, bool state );
#endif
    /// Creates the controls and sizers
    void CreateControls();
    void setFLabels();

    wxPanel* m_Parent;
    iONode m_LocProps;

    wxBoxSizer* m_MainSizer;
    wxBoxSizer* m_ButtonSizer;
    wxBoxSizer* m_Button1Sizer;
    wxBoxSizer* m_Button2Sizer;
    wxBoxSizer* m_SliderSizer;
#ifdef USENEWLOOK
    LEDButton* m_FG;
    LEDButton* m_V;
    LEDButton* m_F0;
    LEDButton* m_F1;
    LEDButton* m_F2;
    LEDButton* m_F3;
    LEDButton* m_F4;
    LEDButton* m_Dir;
    Slider* m_Vslider;
    LEDButton* m_Stop;
#else
    wxButton* m_FG;
    wxTextCtrl* m_V;
    wxButton* m_F0;
    wxButton* m_F1;
    wxButton* m_F2;
    wxButton* m_F3;
    wxButton* m_F4;
    wxButton* m_Dir;
    wxSlider* m_Vslider;
    wxButton* m_Stop;
#endif

    Clock* m_Clock;

    bool m_bFn;
    bool m_bFx[32];
    bool m_bDir;
    int m_iSpeed;
    int m_iFnGroup;
};

#endif

