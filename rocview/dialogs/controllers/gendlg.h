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

#ifndef _GENDLG_H_
#define _GENDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "gendlg.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/spinctrl.h"
#include "wx/statline.h"
////@end includes

#include "rocs/public/node.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxSpinCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_GEN_SETTINGS 10087
#define ID_PANEL 10000
#define ID_STATICTEXT 10001
#define ID_TEXTCTRL 10002
#define ID_STATICTEXT1 10003
#define ID_RADIOBOX 10011
#define ID_RADIOBOX1 10019
#define SYMBOL_GENERICCTRLDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_GENERICCTRLDLG_TITLE _("P50/P50x")
#define SYMBOL_GENERICCTRLDLG_IDNAME ID_DIALOG_GEN_SETTINGS
#define SYMBOL_GENERICCTRLDLG_SIZE wxSize(400, 300)
#define SYMBOL_GENERICCTRLDLG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * GenericCtrlDlg class declaration
 */

class GenericCtrlDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( GenericCtrlDlg )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initValues();
  void evaluate();

public:
    /// Constructors
    GenericCtrlDlg( );
    GenericCtrlDlg( wxWindow* parent, iONode props, const char* controllername, int bps=0, const char* hs=NULL, const char* devices=NULL );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_GENERICCTRLDLG_IDNAME, const wxString& caption = SYMBOL_GENERICCTRLDLG_TITLE, const wxPoint& pos = SYMBOL_GENERICCTRLDLG_POSITION, const wxSize& size = SYMBOL_GENERICCTRLDLG_SIZE, long style = SYMBOL_GENERICCTRLDLG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin GenericCtrlDlg event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end GenericCtrlDlg event handler declarations

////@begin GenericCtrlDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end GenericCtrlDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin GenericCtrlDlg member variables
    wxPanel* m_Panel;
    wxStaticText* m_labIID;
    wxTextCtrl* m_IID;
    wxStaticText* m_labDevice;
    wxComboBox* m_Device;
    wxStaticText* m_labLib;
    wxTextCtrl* m_Lib;
    wxRadioBox* m_Baudrate;
    wxRadioBox* m_HardwareFlow;
    wxStaticText* m_labTimeout;
    wxSpinCtrl* m_Timeout;
    wxStaticText* m_labMS;
    wxStaticBox* m_labFeedbackBox;
    wxStaticText* m_labFbMod;
    wxSpinCtrl* m_FbMod;
    wxStaticText* m_labFbOffset;
    wxSpinCtrl* m_FbOffset;
    wxStaticText* m_labPollSleep;
    wxSpinCtrl* m_PollSleep;
    wxCheckBox* m_FbPoll;
    wxCheckBox* m_FbReset;
    wxCheckBox* m_Bidi;
    wxStaticBox* m_OptionsBox;
    wxCheckBox* m_PTSupport;
    wxCheckBox* m_SystemInfo;
    wxCheckBox* m_LocoList;
    wxCheckBox* m_SwitchList;
    wxCheckBox* m_Overrule;
    wxStaticText* m_labVersion;
    wxSpinCtrl* m_Version;
    wxStaticText* m_labSwTime;
    wxSpinCtrl* m_SwTime;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end GenericCtrlDlg member variables
    iONode m_Props;
    const char* m_Devices;
};

#endif
    // _GENDLG_H_
