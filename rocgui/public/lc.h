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
#include "rocgui/public/clock.h"

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
    iONode getLocProps();

private:
    /// Creation
    bool Create();
    void init();
    void funCmd();
    void speedCmd(bool sendCmd);
    bool setButtonColor( wxButton* button, bool state );

    /// Creates the controls and sizers
    void CreateControls();
    void setFLabels();

    wxPanel* m_Parent;
    iONode m_LocProps;

    wxBoxSizer* m_MainSizer;
    wxBoxSizer* m_ButtonSizer;
    wxButton* m_FG;
    wxTextCtrl* m_V;
    wxBoxSizer* m_Button1Sizer;
    wxButton* m_F0;
    wxButton* m_F1;
    wxButton* m_F2;
    wxBoxSizer* m_Button2Sizer;
    wxButton* m_F3;
    wxButton* m_F4;
    wxButton* m_Dir;
    wxBoxSizer* m_SliderSizer;
    wxSlider* m_Vslider;
    wxButton* m_Stop;

    Clock* m_Clock;

    bool m_bFn;
    bool m_bFx[32];
    bool m_bDir;
    int m_iSpeed;
    int m_iFnGroup;
};

#endif

