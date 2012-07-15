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
#include "wx/grid.h"
////@end includes

#include "rocs/public/node.h"
#include "rocs/public/list.h"
#include "basedlg.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxNotebook;
class wxSpinCtrl;
class wxGrid;
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
#define ID_BUTTON_ST_COPY 10401
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
#define ID_COND_GRID 10388
#define ID_ROUTE_CONDITION_ADD 10389
#define ID_ROUTE_CONDITION_MODIFY 10390
#define ID_ROUTE_CONDITION_DEL 10395
#define ID_ROUTE_WIRING 10340
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
  bool evaluate();
  int m_TabAlign;
  void initBlockCombos();
  void initSwitchCombo();
  void initOutputCombo();
  void initLocPermissionList();
  void initCondList();

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

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_ST_COPY
    void OnButtonStCopyClick( wxCommandEvent& event );

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

    /// wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_COND_GRID
    void OnCondCellLeftClick( wxGridEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ROUTE_CONDITION_ADD
    void OnRouteConditionAddClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ROUTE_CONDITION_MODIFY
    void OnRouteConditionModifyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ROUTE_CONDITION_DEL
    void OnRouteConditionDelClick( wxCommandEvent& event );

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
    wxButton* m_CopyRoute;
    wxPanel* m_GeneralPanel;
    wxStaticText* m_LabelId;
    wxTextCtrl* m_Id;
    wxStaticText* m_labModID;
    wxTextCtrl* m_ModID;
    wxStaticText* m_LabelBlockA;
    wxComboBox* m_BlockA;
    wxCheckBox* m_FromSide;
    wxStaticText* m_LabelBlockB;
    wxComboBox* m_BlockB;
    wxCheckBox* m_ToSide;
    wxStaticText* m_labCrossingBlock;
    wxTextCtrl* m_BlockC;
    wxRadioBox* m_Speed;
    wxSpinCtrl* m_SpeedPercent;
    wxCheckBox* m_ReduceV;
    wxRadioBox* m_Dir;
    wxRadioBox* m_FromSignals;
    wxRadioBox* m_RunDir;
    wxRadioBox* m_ToSignals;
    wxStaticText* m_PlaceHolderX;
    wxRadioBox* m_CountCars;
    wxStaticBox* m_DetailsBox;
    wxCheckBox* m_Swap;
    wxCheckBox* m_SwapPost;
    wxCheckBox* m_CrossingblockSignals;
    wxCheckBox* m_Show;
    wxCheckBox* m_Manual;
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
    wxCheckBox* m_Lock;
    wxPanel* m_SensorPanel;
    wxListBox* m_SensorList;
    wxComboBox* m_SensorCombo;
    wxButton* m_AddSensor;
    wxButton* m_DelSensor;
    wxPanel* m_PermissionsPanel;
    wxStaticText* m_labInclude;
    wxListBox* m_IncludeList;
    wxStaticText* m_labExclude;
    wxListBox* m_ExcludeList;
    wxRadioBox* m_PermType;
    wxStaticText* m_labMaxLen;
    wxSpinCtrl* m_MaxLen;
    wxCheckBox* m_Commuter;
    wxPanel* m_ConditionsPanel;
    wxGrid* m_CondGrid;
    wxCheckBox* m_CondNotFromBlock;
    wxComboBox* m_CondFromBlock;
    wxComboBox* m_CondType;
    wxCheckBox* m_CondCommuter;
    wxCheckBox* m_CondChangeDir;
    wxCheckBox* m_CondAllowSchedules;
    wxButton* m_CondAdd;
    wxButton* m_CondModify;
    wxButton* m_CondDelete;
    wxPanel* m_LocationPanel;
    wxStaticText* m_LabelX;
    wxTextCtrl* m_x;
    wxStaticText* m_LabelY;
    wxTextCtrl* m_y;
    wxStaticText* m_LabelZ;
    wxTextCtrl* m_z;
    wxRadioBox* m_ori;
    wxPanel* m_WiringPanel;
    wxStaticBox* m_CTCBox;
    wxStaticText* m_labCTCbutton;
    wxStaticText* m_labCTCIID;
    wxStaticText* m_labCTCAddr;
    wxStaticText* m_labOutput;
    wxStaticText* m_labCTCButton1;
    wxTextCtrl* m_CTCIID1;
    wxSpinCtrl* m_CTCAddr1;
    wxComboBox* m_Output1;
    wxStaticText* m_labCTCButton2;
    wxTextCtrl* m_CTCIID2;
    wxSpinCtrl* m_CTCAddr2;
    wxComboBox* m_Output2;
    wxStaticText* m_labCTCButton3;
    wxTextCtrl* m_CTCIID3;
    wxSpinCtrl* m_CTCAddr3;
    wxComboBox* m_Output3;
    wxButton* m_Cancel;
    wxButton* m_OK;
    wxButton* m_Apply;
    iONode m_Props;
////@end RouteDialog member variables
    int m_CondNr;
    iOList m_CondList;
};

#endif
    // _RouteDialog_H_
