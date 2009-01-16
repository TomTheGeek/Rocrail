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

#ifndef _HSI88DLG_H_
#define _HSI88DLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "hsi88dlg.h"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes
#include "rocs/public/node.h"


/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_HSI_SETTINGS 10025
#define SYMBOL_HSI88CNTRLDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_HSI88CNTRLDLG_TITLE _("HSI88")
#define SYMBOL_HSI88CNTRLDLG_IDNAME ID_DIALOG_HSI_SETTINGS
#define SYMBOL_HSI88CNTRLDLG_SIZE wxSize(400, 300)
#define SYMBOL_HSI88CNTRLDLG_POSITION wxDefaultPosition
#define ID_PANEL 10000
#define ID_STATICTEXT_HSI_IID 10001
#define ID_TEXTCTRL_HSI_IID 10002
#define ID_STATICTEXT_HSI_DEVICE 10003
#define ID_TEXTCTRL_HSI_DEVICE 10004
#define wxID_STATIC_HSI_LEFT 10026
#define ID_TEXTCTRL_HSI_LEFT 10005
#define wxID_STATIC_HSI_MID 10027
#define ID_TEXTCTRL_HSI_MID 10006
#define wxID_STATIC_HSI_RIGHT 10028
#define ID_TEXTCTRL_HSI_RIGHT 10007
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * Hsi88CntrlDlg class declaration
 */

class Hsi88CntrlDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( Hsi88CntrlDlg )
    DECLARE_EVENT_TABLE()
  void initLabels();
  void initValues();
  void evaluate();
  iONode m_Props;

public:
    /// Constructors
    Hsi88CntrlDlg( );
    Hsi88CntrlDlg( wxWindow* parent, iONode props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_HSI88CNTRLDLG_IDNAME, const wxString& caption = SYMBOL_HSI88CNTRLDLG_TITLE, const wxPoint& pos = SYMBOL_HSI88CNTRLDLG_POSITION, const wxSize& size = SYMBOL_HSI88CNTRLDLG_SIZE, long style = SYMBOL_HSI88CNTRLDLG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin Hsi88CntrlDlg event handler declarations
    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end Hsi88CntrlDlg event handler declarations

////@begin Hsi88CntrlDlg member function declarations
    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end Hsi88CntrlDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin Hsi88CntrlDlg member variables
    wxStaticText* m_labIID;
    wxTextCtrl* m_IID;
    wxStaticText* m_labDevice;
    wxTextCtrl* m_Device;
    wxStaticText* m_labLeft;
    wxTextCtrl* m_Left;
    wxStaticText* m_labMid;
    wxTextCtrl* m_Mid;
    wxStaticText* m_labRight;
    wxTextCtrl* m_Right;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end Hsi88CntrlDlg member variables
};

#endif
    // _HSI88DLG_H_
