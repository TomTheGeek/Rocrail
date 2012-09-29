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
#ifndef _OUTPUTDIALOG_H_
#define _OUTPUTDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "outputdialog.h"
#endif

/*!
 * Includes
 */
#include "basedlg.h"
#include "rocs/public/node.h"

////@begin includes
#include "wx/notebook.h"
#include "wx/listctrl.h"
#include "wx/spinctrl.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxNotebook;
class wxListCtrl;
class wxSpinCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_OUTPUT_DLG 10210
#define ID_NOTEBOOK_CO 10012
#define ID_PANEL_CO_INDEX 10005
#define ID_LISTCTRLINDEX_CO 10409
#define ID_BUTTON_CO_NEW 10007
#define ID_BUTTON_CO_DELETE 10008
#define ID_PANEL_CO_GENERAL 10009
#define wxID_STATIC_CO_ID 10010
#define ID_TEXTCTRL_CO_ID 10011
#define wxID_STATIC_CO_DESC 10221
#define ID_TEXTCTRL_CO_DESC 10222
#define ID_CHECKBOX_CO_SHOW 10017
#define ID_OUTPUT_ACTIONS 10199
#define ID_PANEL_CO_INTERFACE 10021
#define wxID_STATIC_CO_IID 10022
#define ID_TEXTCTRL_CO_IID 10023
#define wxID_STATIC_CO_BUS 10260
#define ID_TEXTCTRL_CO_BUS 10259
#define wxID_STATIC_CO_PROT 10030
#define ID_CHOICE_CO_PROT 10031
#define wxID_STATIC_CO_ADDRESS 10024
#define ID_TEXTCTRL_CO_ADDRESS 10025
#define wxID_STATIC_CO_PORT 10026
#define ID_TEXTCTRL_CO_PORT 10027
#define wxID_STATIC_CO_GATE 10326
#define ID_OUTPUT_SWITCH 10199
#define ID_CO_PORT_TYPE 10414
#define ID_PANEL_CO_LOCATION 10013
#define wxID_STATIC_CO_X 10014
#define ID_TEXTCTRL_CO_X 10000
#define wxID_STATIC_CO_Y 10001
#define ID_TEXTCTRL_CO_Y 10002
#define wxID_STATIC_CO_Z 10003
#define ID_TEXTCTRL_CO_Z 10004
#define ID_RADIOBOX_CO_ORI 10015
#define SYMBOL_OUTPUTDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_OUTPUTDIALOG_TITLE _("OutputTable")
#define SYMBOL_OUTPUTDIALOG_IDNAME ID_OUTPUT_DLG
#define SYMBOL_OUTPUTDIALOG_SIZE wxDefaultSize
#define SYMBOL_OUTPUTDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * OutputDialog class declaration
 */

class OutputDialog: public wxDialog, public BaseDialog
{
    DECLARE_DYNAMIC_CLASS( OutputDialog )
    DECLARE_EVENT_TABLE()

  void initLabels();
  bool initIndex();
  void initValues();
  bool evaluate();
  int m_TabAlign;

public:
    /// Constructors
    OutputDialog( );
    OutputDialog( wxWindow* parent, iONode props);

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_OUTPUTDIALOG_IDNAME, const wxString& caption = SYMBOL_OUTPUTDIALOG_TITLE, const wxPoint& pos = SYMBOL_OUTPUTDIALOG_POSITION, const wxSize& size = SYMBOL_OUTPUTDIALOG_SIZE, long style = SYMBOL_OUTPUTDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();
    iONode getProperties(){ return m_Props;}
    void OnSelectPage( wxCommandEvent& event );

////@begin OutputDialog event handler declarations

    /// wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED event handler for ID_NOTEBOOK_CO
    void OnNotebookCoPageChanged( wxNotebookEvent& event );

    /// wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRLINDEX_CO
    void OnListctrlindexCoSelected( wxListEvent& event );

    /// wxEVT_COMMAND_LIST_COL_CLICK event handler for ID_LISTCTRLINDEX_CO
    void OnListctrlindexCoColLeftClick( wxListEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CO_NEW
    void OnButtonCoNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_CO_DELETE
    void OnButtonCoDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_OUTPUT_ACTIONS
    void OnOutputActionsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_OUTPUT_SWITCH
    void OnOutputSwitchClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
    void OnApplyClick( wxCommandEvent& event );

////@end OutputDialog event handler declarations

////@begin OutputDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end OutputDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin OutputDialog member variables
    wxNotebook* m_Notebook;
    wxPanel* m_IndexPanel;
    wxListCtrl* m_List2;
    wxButton* m_New;
    wxButton* m_Delete;
    wxPanel* m_GeneralPanel;
    wxStaticText* m_LabelID;
    wxTextCtrl* m_ID;
    wxStaticText* m_LabelDescription;
    wxTextCtrl* m_Description;
    wxStaticText* m_labSVG;
    wxSpinCtrl* m_SVG;
    wxStaticText* m_labBlockID;
    wxComboBox* m_BlockID;
    wxStaticText* m_labRouteIDs;
    wxTextCtrl* m_RouteIDs;
    wxCheckBox* m_Show;
    wxCheckBox* m_TriState;
    wxRadioBox* m_Type;
    wxButton* m_Actions;
    wxPanel* m_InterfacePanel;
    wxStaticText* m_Labeliid;
    wxTextCtrl* m_iid;
    wxStaticText* m_Label_Bus;
    wxTextCtrl* m_Bus;
    wxStaticText* m_LabelProt;
    wxChoice* m_Prot;
    wxStaticText* m_LabelAddress;
    wxTextCtrl* m_Address;
    wxStaticText* m_LabelPort;
    wxTextCtrl* m_Port;
    wxStaticText* m_labGate;
    wxRadioBox* m_Gate;
    wxStaticBox* m_OptionsBox;
    wxCheckBox* m_AsSwitch;
    wxCheckBox* m_Invert;
    wxRadioBox* m_PortType;
    wxPanel* m_LocationPanel;
    wxStaticText* m_LabelX;
    wxTextCtrl* m_x;
    wxStaticText* m_LabelY;
    wxTextCtrl* m_y;
    wxStaticText* m_LabelZ;
    wxTextCtrl* m_z;
    wxRadioBox* m_ori;
    wxButton* m_Cancel;
    wxButton* m_OK;
    wxButton* m_Apply;
    iONode m_Props;
////@end OutputDialog member variables
};

#endif
    // _OUTPUTDIALOG_H_
