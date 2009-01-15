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
#ifndef _LOCCONTROLDLG_H_
#define _LOCCONTROLDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "loccontroldlg.h"
#endif

/*!
 * Includes
 */
#include "rocs/public/node.h"
#include "rocs/public/map.h"

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_LOCCONTROL 10185
#define ID_BITMAPBUTTON_LOCCTRL_IMAGE 10000
#define ID_TEXTCTRL_LOCCTRL_SPEED 10003
#define ID_SLIDER_LOCCTRL_SPEED 10001
#define ID_BUTTON_LOCCTRL_F1 10005
#define ID_BUTTON_LOCCTRL_F2 10006
#define ID_BUTTON_LOCCTRL_F3 10007
#define ID_BUTTON_LOCCTRL_F4 10008
#define ID_BUTTON_LOCCTRL_F5 10187
#define ID_BUTTON_LOCCTRL_F6 10189
#define ID_BUTTON_LCCTRL_F7 10190
#define ID_BUTTON_LCCTRL_F8 10231
#define ID_BUTTON_LCCTRL_9 10349
#define ID_BUTTON_LCCTRL_F10 10012
#define ID_BUTTON_LCCTRL_F11 10014
#define ID_BUTTON_LCCTRL_F12 10015
#define ID_BUTTON_LOCCTRL_FN 10004
#define ID_BITMAPBUTTON_LOCCTRL_DIR 10009
#define ID_COMBOBOX_LOCCTRL_LOC 10002
#define ID_BUTTON_LOCCTRL_CANCEL 10010
#define ID_BUTTON_LOCCTRL_STOP 10011
#define ID_BUTTON_LOCCTRL_BREAK 10013
#define SYMBOL_LOCCONTROLDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_LOCCONTROLDIALOG_TITLE _("Loc Control")
#define SYMBOL_LOCCONTROLDIALOG_IDNAME ID_LOCCONTROL
#define SYMBOL_LOCCONTROLDIALOG_SIZE wxDefaultSize
#define SYMBOL_LOCCONTROLDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * LocControlDialog class declaration
 */

class LocControlDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( LocControlDialog )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initValues();
  void initLocMap(const char* locid);
  void updateFnText( wxString& selVal );
  bool setButtonColor( wxButton* button, bool state );
  void funCmd();
  void speedCmd(bool sendCmd);
  void initFx(iONode lc);

public:
  void modelEvent( iONode evt );
    /// Constructors
    LocControlDialog( );
    LocControlDialog( wxWindow* parent, iOList list, iOMap map, const char* locid=NULL );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_LOCCONTROLDIALOG_IDNAME, const wxString& caption = SYMBOL_LOCCONTROLDIALOG_TITLE, const wxPoint& pos = SYMBOL_LOCCONTROLDIALOG_POSITION, const wxSize& size = SYMBOL_LOCCONTROLDIALOG_SIZE, long style = SYMBOL_LOCCONTROLDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin LocControlDialog event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_LOCCONTROL
    void OnCloseWindow( wxCloseEvent& event );

    /// wxEVT_DESTROY event handler for ID_LOCCONTROL
    void OnDestroy( wxWindowDestroyEvent& event );

    /// wxEVT_KEY_DOWN event handler for ID_LOCCONTROL
    void OnKeyDown( wxKeyEvent& event );

    /// wxEVT_KEY_UP event handler for ID_LOCCONTROL
    void OnKeyUp( wxKeyEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_LOCCTRL_IMAGE
    void OnBitmapbuttonLocctrlImageClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_SLIDER_UPDATED event handler for ID_SLIDER_LOCCTRL_SPEED
    void OnSliderLocctrlSpeedUpdated( wxScrollEvent& event );

    /// wxEVT_SCROLL_* event handler for ID_SLIDER_LOCCTRL_SPEED
    void OnSliderLocctrlSpeedScroll( wxScrollEvent& event );

    /// wxEVT_SCROLL_ENDSCROLL event handler for ID_SLIDER_LOCCTRL_SPEED
    void OnSliderLocctrlSpeedEndScroll( wxScrollEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F1
    void OnButtonLocctrlF1Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F2
    void OnButtonLocctrlF2Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F3
    void OnButtonLocctrlF3Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F4
    void OnButtonLocctrlF4Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F5
    void OnButtonLocctrlF5Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_F6
    void OnButtonLocctrlF6Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_F7
    void OnButtonLcctrlF7Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_F8
    void OnButtonLcctrlF8Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_9
    void OnButtonLcctrl9Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_F10
    void OnButtonLcctrlF10Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_F11
    void OnButtonLcctrlF11Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LCCTRL_F12
    void OnButtonLcctrlF12Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_FN
    void OnButtonLocctrlFnClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_LOCCTRL_DIR
    void OnBitmapbuttonLocctrlDirClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_LOCCTRL_LOC
    void OnComboboxLocctrlLocSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_CANCEL
    void OnButtonLocctrlCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_STOP
    void OnButtonLocctrlStopClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCCTRL_BREAK
    void OnButtonLocctrlBreakClick( wxCommandEvent& event );

////@end LocControlDialog event handler declarations

////@begin LocControlDialog member function declarations

    iOMap GetLcMap() const { return m_lcMap ; }
    void SetLcMap(iOMap value) { m_lcMap = value ; }

    int GetFn() const { return m_bFn ; }
    void SetFn(int value) { m_bFn = value ; }

    int GetF1() const { return m_bF1 ; }
    void SetF1(int value) { m_bF1 = value ; }

    int GetF2() const { return m_bF2 ; }
    void SetF2(int value) { m_bF2 = value ; }

    int GetF3() const { return m_bF3 ; }
    void SetF3(int value) { m_bF3 = value ; }

    int GetF4() const { return m_bF4 ; }
    void SetF4(int value) { m_bF4 = value ; }

    bool GetDirection() const { return m_bDir ; }
    void SetDirection(bool value) { m_bDir = value ; }

    int GetISpeed() const { return m_iSpeed ; }
    void SetISpeed(int value) { m_iSpeed = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end LocControlDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin LocControlDialog member variables
    wxBitmapButton* m_Image;
    wxTextCtrl* m_Speed;
    wxSlider* m_SpeedCtrl;
    wxButton* m_F1;
    wxButton* m_F2;
    wxButton* m_F3;
    wxButton* m_F4;
    wxButton* m_F5;
    wxButton* m_F6;
    wxButton* m_F7;
    wxButton* m_F8;
    wxButton* m_F9;
    wxButton* m_F10;
    wxButton* m_F11;
    wxButton* m_F12;
    wxButton* m_Fn;
    wxBitmapButton* m_Dir;
    wxComboBox* m_LcList;
    wxButton* m_Cancel;
    wxButton* m_Stop;
    wxButton* m_Break;
    iOMap m_lcMap;
    int m_bFn;
    int m_bF1;
    int m_bF2;
    int m_bF3;
    int m_bF4;
    bool m_bDir;
    int m_iSpeed;
////@end LocControlDialog member variables
    int m_bF5;
    int m_bF6;
    int m_bF7;
    int m_bF8;
    int m_bF9;
    int m_bF10;
    int m_bF11;
    int m_bF12;
    iOList m_DlgList;
    iOMap m_DlgMap;
};

#endif
    // _LOCCONTROLDLG_H_
