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

#ifndef _BARJUTDLG_H_
#define _BARJUTDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "barjutdlg.h"
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
#define ID_DIALOG_BARJUT_SETTINGS 10034
#define ID_PANEL_BARJUT 10038
#define ID_STATICTEXT_BARJUT_IID 10000
#define ID_TEXTCTRL_BARJUT_IID 10001
#define ID_STATICTEXT_BARJUT_DEVICE 10002
#define ID_TEXTCTRL_BARJUT_DEVICE 10003
#define ID_STATICTEXT_BARJUT_POLLING 10004
#define ID_TEXTCTRL_BARJUT_POLLING 10006
#define SYMBOL_BARJUTCNTRLDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_BARJUTCNTRLDLG_TITLE _("BarJuT")
#define SYMBOL_BARJUTCNTRLDLG_IDNAME ID_DIALOG_BARJUT_SETTINGS
#define SYMBOL_BARJUTCNTRLDLG_SIZE wxDefaultSize
#define SYMBOL_BARJUTCNTRLDLG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * BarJuTCntrlDlg class declaration
 */

class BarJuTCntrlDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( BarJuTCntrlDlg )
    DECLARE_EVENT_TABLE()
  void initLabels();
  void initValues();
  void evaluate();
  iONode m_Props;

public:
    /// Constructors
    BarJuTCntrlDlg( );
    BarJuTCntrlDlg( wxWindow* parent, iONode props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_BARJUTCNTRLDLG_IDNAME, const wxString& caption = SYMBOL_BARJUTCNTRLDLG_TITLE, const wxPoint& pos = SYMBOL_BARJUTCNTRLDLG_POSITION, const wxSize& size = SYMBOL_BARJUTCNTRLDLG_SIZE, long style = SYMBOL_BARJUTCNTRLDLG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin BarJuTCntrlDlg event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOKClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCANCELClick( wxCommandEvent& event );

////@end BarJuTCntrlDlg event handler declarations

////@begin BarJuTCntrlDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end BarJuTCntrlDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin BarJuTCntrlDlg member variables
    wxStaticText* m_labIID;
    wxTextCtrl* m_IID;
    wxStaticText* m_labDevice;
    wxTextCtrl* m_Device;
    wxStaticText* m_labPolling;
    wxTextCtrl* m_Polling;
////@end BarJuTCntrlDlg member variables
};

#endif
    // _BARJUTDLG_H_
