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

#ifndef _LOCONETDLG_H_
#define _LOCONETDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "loconetdlg.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
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
#define ID_DIALOG_LOCONET_SETTINGS 10033
#define ID_NOTEBOOK1 10039
#define ID_COMBOBOX_LOCONET_CMDSTN 10040
#define ID_CHECKBOX1 10041
#define SYMBOL_LOCONETCTRLDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_LOCONETCTRLDLG_TITLE _("LocoNet")
#define SYMBOL_LOCONETCTRLDLG_IDNAME ID_DIALOG_LOCONET_SETTINGS
#define SYMBOL_LOCONETCTRLDLG_SIZE wxSize(400, 300)
#define SYMBOL_LOCONETCTRLDLG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * LocoNetCtrlDlg class declaration
 */

class LocoNetCtrlDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( LocoNetCtrlDlg )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initValues();
  void evaluate();
  void initOptions();
  void initSublib();
  int m_TabAlign;

public:
    /// Constructors
    LocoNetCtrlDlg();
    LocoNetCtrlDlg( wxWindow* parent, iONode props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_LOCONETCTRLDLG_IDNAME, const wxString& caption = SYMBOL_LOCONETCTRLDLG_TITLE, const wxPoint& pos = SYMBOL_LOCONETCTRLDLG_POSITION, const wxSize& size = SYMBOL_LOCONETCTRLDLG_SIZE, long style = SYMBOL_LOCONETCTRLDLG_STYLE );

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin LocoNetCtrlDlg event handler declarations

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for wxID_ANY
    void OnSublibSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_LOCONET_CMDSTN
    void OnComboboxLoconetCmdstnSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end LocoNetCtrlDlg event handler declarations

////@begin LocoNetCtrlDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end LocoNetCtrlDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin LocoNetCtrlDlg member variables
    wxPanel* m_InterfacePanel;
    wxStaticText* m_labIID;
    wxTextCtrl* m_IID;
    wxStaticText* m_labDevice;
    wxTextCtrl* m_Device;
    wxStaticText* m_labHost;
    wxTextCtrl* m_Host;
    wxStaticText* m_labPort;
    wxTextCtrl* m_Port;
    wxRadioBox* m_Baudrate;
    wxRadioBox* m_SubLib;
    wxCheckBox* m_Flow;
    wxPanel* m_OptionsPanel;
    wxComboBox* m_CmdStn;
    wxCheckListBox* m_OptionsList;
    wxCheckBox* m_SetOptions;
    wxPanel* m_DetailsPanel;
    wxStaticText* m_labSWTime;
    wxTextCtrl* m_SWTime;
    wxStaticText* m_labPurgeTime;
    wxTextCtrl* m_PurgeTime;
    wxStaticBox* m_FastClockBox;
    wxCheckBox* m_UseFC;
    wxCheckBox* m_SyncFC;
    wxStaticBox* m_SlotServerBox;
    wxCheckBox* m_SlotServerActive;
    wxCheckBox* m_MobileOnly;
    wxCheckBox* m_PurgeSlots;
    wxCheckBox* m_StopAtPurge;
    wxStaticText* m_labDispatchIID;
    wxTextCtrl* m_DispatchIID;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end LocoNetCtrlDlg member variables
    iONode m_Props;
    int optidx[256];
};

#endif
    // _LOCONETDLG_H_
