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
#ifndef _RouteDialog_H_
#define _RouteDialog_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "routedialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
#include "wx/spinctrl.h"
////@end includes

#include "rocs/public/node.h"
#include "basedlg.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxNotebook;
class wxSpinCtrl;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_STREET 10099
#define ID_NOTEBOOK_STREET 10100
#define ID_PANEL_ST_INDEX 10001
#define ID_LISTBOX_ST 10002
#define ID_BUTTON_ST_NEW 10003
#define ID_BUTTON_ST_DELETE_ROUTE 10017
#define ID_BUTTON_ST_TEST 10312
#define ID_BUTTON_ST_DOC 10353
#define ID_PANEL_ST_GENERAL 10005
#define wxID_STATIC_ST_ID 10006
#define ID_TEXTCTRL_ST_ID 10007
#define wxID_STATIC_ST_BLOCKA 10008
#define ID_COMBOBOX_ST_BLOCKA 10009
#define wxID_STATIC_BLOCKB 10010
#define ID_COMBOBOX_ST_BLOCKB 10011
#define wxID_STATIC_BLOCKC 10336
#define ID_ROUTE_VELOCITY 10004
#define ID_RADIOBOX_DIR 10013
#define ID_ROUTE_ACTIONS 10197
#define ID_PANEL_ST_COMMANDS 10014
#define ID_LISTBOX_COMMANDS 10015
#define ID_BUTTON_ST_DELETE 10016
#define ID_BUTTON_ST_MODIFY 10082
#define ID_COMBOBOX_ST_SWITCH_ID 10000
#define ID_BUTTON_ST_ADD 10004
#define ID_RADIOBOX_ST_SW_CMD 10018
#define ID_LIST_ROUTE_SENSORS 10357
#define ID_COMBOBOX_ROUTES_SENSORS 10335
#define ID_BUTTON_ROUTES_ADD_SENSOR 10355
#define ID_BUTTON_ROUTES_DEL_SENSOR 10356
#define SYMBOL_ROUTEDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_ROUTEDIALOG_TITLE _("Routes")
#define SYMBOL_ROUTEDIALOG_IDNAME ID_DIALOG_STREET
#define SYMBOL_ROUTEDIALOG_SIZE wxDefaultSize
#define SYMBOL_ROUTEDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * RouteDialog class declaration
 */

class RouteDialog: public wxDialog, public BaseDialog
{
    DECLARE_DYNAMIC_CLASS( RouteDialog )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initIndex();
  void initValues();
  void initCommands();
  void evaluate();
  int m_TabAlign;
  void initBlockCombos();
  void initSwitchCombo();
  void initOutputCombo();

public:
    /// Constructors
    RouteDialog( wxWindow* parent, iONode p_Props );
    RouteDialog( );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_ROUTEDIALOG_IDNAME, const wxString& caption = SYMBOL_ROUTEDIALOG_TITLE, const wxPoint& pos = SYMBOL_ROUTEDIALOG_POSITION, const wxSize& size = SYMBOL_ROUTEDIALOG_SIZE, long style = SYMBOL_ROUTEDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();
    iONode getProperties(){ return m_Props;}
    void OnSelectPage( wxCommandEvent& event );

////@begin RouteDialog event handler declarations

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_ST
    void OnListboxStSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_NEW
    void OnButtonStNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_DELETE_ROUTE
    void OnButtonStDeleteRouteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_TEST
    void OnButtonStTestClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_DOC
    void OnButtonStDocClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_ROUTE_VELOCITY
    void OnRouteVelocitySelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ROUTE_ACTIONS
    void OnRouteActionsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_COMMANDS
    void OnListboxCommandsSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_DELETE
    void OnButtonTurnoutDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_MODIFY
    void OnButtonTurnoutModifyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_ST_SWITCH_ID
    void OnComboboxStSwitchIdSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_ADD
    void OnButtonTurnoutAddClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LIST_ROUTE_SENSORS
    void OnListRouteSensorsSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ROUTES_ADD_SENSOR
    void OnButtonRoutesAddSensorClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ROUTES_DEL_SENSOR
    void OnButtonRoutesDelSensorClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
    void OnApplyClick( wxCommandEvent& event );

////@end RouteDialog event handler declarations

////@begin RouteDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end RouteDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin RouteDialog member variables
    wxNotebook* m_Notebook;
    wxPanel* m_IndexPanel;
    wxListBox* m_List;
    wxButton* m_New;
    wxButton* m_DeleteRoute;
    wxButton* m_Test;
    wxButton* m_Doc;
    wxPanel* m_GeneralPanel;
    wxStaticText* m_LabelId;
    wxTextCtrl* m_Id;
    wxStaticText* m_LabelBlockA;
    wxComboBox* m_BlockA;
    wxStaticText* m_LabelBlockB;
    wxComboBox* m_BlockB;
    wxStaticText* m_labCrossingBlock;
    wxTextCtrl* m_BlockC;
    wxStaticText* m_labModID;
    wxTextCtrl* m_ModID;
    wxRadioBox* m_Speed;
    wxSpinCtrl* m_SpeedPercent;
    wxCheckBox* m_ReduceV;
    wxRadioBox* m_Dir;
    wxRadioBox* m_RunDir;
    wxStaticBox* m_DetailsBox;
    wxCheckBox* m_Swap;
    wxButton* m_Actions;
    wxPanel* m_CommandPanel;
    wxListBox* m_Commands;
    wxButton* m_Delete;
    wxButton* m_Modify;
    wxStaticText* m_LabelSwitchId;
    wxComboBox* m_SwitchId;
    wxStaticText* m_labTrackNumber;
    wxSpinCtrl* m_TrackNumber;
    wxButton* m_Add;
    wxRadioBox* m_SwitchCmd;
    wxPanel* m_SensorPanel;
    wxListBox* m_SensorList;
    wxComboBox* m_SensorCombo;
    wxButton* m_AddSensor;
    wxButton* m_DelSensor;
    wxButton* m_Cancel;
    wxButton* m_OK;
    wxButton* m_Apply;
    iONode m_Props;
////@end RouteDialog member variables
};

#endif
    // _RouteDialog_H_
