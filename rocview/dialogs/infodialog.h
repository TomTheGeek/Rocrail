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
#ifndef _INFODIALOG_H_
#define _INFODIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "infodialog.cpp"
#endif

/*!
 * Includes
 */

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
#define ID_DIALOG 10115
#define ID_BITMAPBUTTON_INFO_SPLASH 10172
#define wxID_STATIC_INFO_VERSION 10001
#define wxID_STATIC_INFO_NAME 10244
#define wxID_STATIC_INFO_BUILD 10002
#define wxID_STATIC_INFO_HOME 10003
#define wxID_STATIC_INFO_SUPPORT 10004
#define WX_STATICTEXT_INFO_THANKS 10289
#define ID_TEXTCTRL_INFO_THANKS 10273
#define SYMBOL_INFODIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_INFODIALOG_TITLE _("Info")
#define SYMBOL_INFODIALOG_IDNAME ID_DIALOG
#define SYMBOL_INFODIALOG_SIZE wxDefaultSize
#define SYMBOL_INFODIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * InfoDialog class declaration
 */

class InfoDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( InfoDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    InfoDialog( );
    InfoDialog( wxWindow* parent, wxWindowID id = SYMBOL_INFODIALOG_IDNAME, const wxString& caption = SYMBOL_INFODIALOG_TITLE, const wxPoint& pos = SYMBOL_INFODIALOG_POSITION, const wxSize& size = SYMBOL_INFODIALOG_SIZE, long style = SYMBOL_INFODIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_INFODIALOG_IDNAME, const wxString& caption = SYMBOL_INFODIALOG_TITLE, const wxPoint& pos = SYMBOL_INFODIALOG_POSITION, const wxSize& size = SYMBOL_INFODIALOG_SIZE, long style = SYMBOL_INFODIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin InfoDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON_INFO_SPLASH
    void OnBitmapbuttonInfoSplashClick( wxCommandEvent& event );

////@end InfoDialog event handler declarations

////@begin InfoDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end InfoDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin InfoDialog member variables
    wxBitmapButton* m_Splash;
    wxStaticText* m_Version;
    wxStaticText* m_Name;
    wxStaticText* m_Build;
    wxStaticText* m_Home;
    wxStaticText* m_Support;
    wxStaticText* m_ThanksLine;
    wxTextCtrl* m_Thanks;
////@end InfoDialog member variables
};

#endif
    // _INFODIALOG_H_
