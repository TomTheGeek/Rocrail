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

#ifndef _DINAMODLG_H_
#define _DINAMODLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "dinamodlg.h"
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
#define ID_DIALOG_DINAMO_SETTINGS 10029
#define ID_PANEL1 10008
#define ID_STATICTEXT_DINAMO_IID 10009
#define ID_TEXTCTRL_DINAMO_IID 10010
#define ID_STATICTEXT_DINAMO_DEV 10011
#define ID_TEXTCTRL_DINAMO_DEV 10012
#define ID_STATICTEXT_DINAMO_SWTIME 10013
#define ID_TEXTCTRL_DINAMO_SWTIME 10014
#define SYMBOL_DINAMOCTRLDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_DINAMOCTRLDLG_TITLE _("Dinamo")
#define SYMBOL_DINAMOCTRLDLG_IDNAME ID_DIALOG_DINAMO_SETTINGS
#define SYMBOL_DINAMOCTRLDLG_SIZE wxSize(400, 300)
#define SYMBOL_DINAMOCTRLDLG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * DinamoCtrlDlg class declaration
 */

class DinamoCtrlDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( DinamoCtrlDlg )
    DECLARE_EVENT_TABLE()
  void initLabels();
  void initValues();
  void evaluate();
  iONode m_Props;

public:
    /// Constructors
    DinamoCtrlDlg( );
    DinamoCtrlDlg( wxWindow* parent, iONode props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DINAMOCTRLDLG_IDNAME, const wxString& caption = SYMBOL_DINAMOCTRLDLG_TITLE, const wxPoint& pos = SYMBOL_DINAMOCTRLDLG_POSITION, const wxSize& size = SYMBOL_DINAMOCTRLDLG_SIZE, long style = SYMBOL_DINAMOCTRLDLG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin DinamoCtrlDlg event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end DinamoCtrlDlg event handler declarations

////@begin DinamoCtrlDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end DinamoCtrlDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin DinamoCtrlDlg member variables
    wxStaticText* m_labIID;
    wxTextCtrl* m_IID;
    wxStaticText* m_labDevice;
    wxTextCtrl* m_Device;
    wxStaticText* m_labSwTime;
    wxTextCtrl* m_SwTime;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end DinamoCtrlDlg member variables
};

#endif
    // _DINAMODLG_H_
