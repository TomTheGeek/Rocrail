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
#ifndef _BLOCKDIALOG_H_
#define _BLOCKDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "blockdialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
#include "wx/spinctrl.h"
////@end includes

#include "rocs/public/node.h"

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
#define ID_DIALOG_BLOCKS 10061
#define ID_NOTEBOOK_BLOCKS 10000
#define ID_PANEL_INDEX_BLOCKS 10060
#define ID_LISTBOX_BLOCKS 10062
#define ID_BUTTON_BLOCK_NEW 10063
#define ID_BUTTON_BLOCK_DELETE 10064
#define ID_PANEL_BK_GENERAL 10004
#define wxID_STATIC_ID_BLOCK 10001
#define ID_TEXTCTRL_ID_BLOCK 10005
#define wxID_STATIC_DESCRIPTION_BLOCK 10010
#define ID_TEXTCTRL_DESCRIPTION_BLOCK 10006
#define wxID_STATIC_LENGTH_BLOCK 10011
#define ID_TEXTCTRL_LENGTH_BLOCK 10007
#define wxID_STATIC_BK_LOCID 10017
#define ID_TEXTCTRL_BK_LOCID 10018
#define ID_BUTTON_BK_LOCPROPS 10019
#define ID_RADIOBOX_BK_COMMUTER 10067
#define wxID_ANY_BK_CONFIG 10066
#define ID_CHECKBOX_BK_ELECTRIFIED 10068
#define ID_CHECKBOX_BK_STATION 10069
#define ID_CHECKBOX_BK_CLOSED 10071
#define wxID_BLOCK_WAIT 10350
#define ID_CHECKBOX_BK_GOMANUAL 10325
#define ID_CHECKBOX_BK_ACCGHOST 10165
#define ID_PANEL_BK_LOCATION 10002
#define ID_LISTBOX_BLOCK_TURNOUTS 10331
#define ID_BUTTON_BLOCK_ADD_TURNOUT 10333
#define ID_BUTTON_BLOCK_REMOVE_TURNOUT 10334
#define ID_TEXTCTRL 10036
#define ID_TEXTCTRL1 10051
#define ID_TEXTCTRL2 10057
#define ID_PANEL_BLOCK_WIRERING 10009
#define ID_STATICTEXT_BK_SIGNAL 10031
#define ID_COMBOBOX_BK_SIGNAL 10032
#define ID_BUTTON_BK_SIGNAL 10033
#define wxID_STATIC_BK_WSIGNAL 10205
#define ID_COMBOBOX_BK_WSIGNAL 10206
#define ID_BUTTON_BK_WSIGNAL 10207
#define ID_BUTTON_BK_SIGNAL_R 10012
#define ID_BUTTON_BK_WSIGNAL_R 10015
#define ID_PANEL_BK_DETAILS 10065
#define ID_RADIOBOX_BK_TYPE 10073
#define ID_RADIOBOX_BK_INCLINE 10074
#define ID_RADIOBOX_BK_WAIT 10075
#define wxID_STATIC_BK_RANDOM_MIN 10076
#define ID_TEXTCTRL_BK_RANDOM_MIN 10026
#define wxID_STATIC_BK_RANDOM_MAX 10077
#define ID_TEXTCTRL_BK_RANDOM_MAX 10027
#define wxID_STATIC_BK_FIXED 10078
#define ID_TEXTCTRL_BK_FIXED 10028
#define ID_RADIOBOX_BK_SPEED 10072
#define ID_SPINCTRL1 10317
#define ID_PANEL_BK_ROUTES 10080
#define ID_LISTBOX_BK_ROUTES 10034
#define wxID_STATIC_SENSORS_TITLE 10261
#define ID_STATICTEXT_BLOCK_SENSORID 10043
#define ID_STATICTEXT_BLOCK_SENSOR_ACTION 10044
#define wxID_STATIC_BLOCK_SENSOR_END 10267
#define ID_STATICTEXT_BLOCK_SENSOR_FREE 10045
#define ID_COMBOBOX_BLOCK_SENSOR_ID1 10046
#define ID_COMBOBOX_BLOCK_SENSOR_ACTION1 10047
#define ID_CHECKBOX_BLOCK_SENSOR_END1 10268
#define ID_BUTTON_BLOCKS_SENSOR_PROPS1 10262
#define ID_COMBOBOX_BLOCK_SENSOR_ID2 10049
#define ID_COMBOBOX_BLOCK_SENSOR_ACTION2 10050
#define ID_CHECKBOX_BLOCK_SENSOR_END2 10269
#define ID_BUTTON_BLOCK_SENSOR_PROPS2 10263
#define ID_COMBOBOX_BLOCK_SENSOR_ID3 10052
#define ID_COMBOBOX_BLOCK_SENSOR_ACTION3 10053
#define ID_CHECKBOX_BLOCK_SENSOR_END3 10270
#define ID_BUTTON_BLOCKS_SENSOR_PROPS3 10264
#define ID_COMBOBOX_BLOCK_SENSOR_ID4 10055
#define ID_COMBOBOX_BLOCK_SENSOR_ACTION4 10056
#define ID_CHECKBOX_BLOCK_SENSOR_END4 10271
#define ID_BUTTON_BLOCKS_SENSOR_PROPS4 10265
#define ID_COMBOBOX_BLOCK_SENSOR_ID5 10058
#define ID_COMBOBOX_BLOCK_SENSOR_ACTION5 10059
#define ID_CHECKBOX_BLOCK_SENSOR_END5 10272
#define ID_BUTTON_BLOCKS_SENSOR_PROPS5 10266
#define wxID_STATIC_BK_TIMER 10164
#define ID_TEXTCTRL_BK_TIMER 10163
#define ID_BUTTON1 10083
#define ID_BUTTON2 10084
#define ID_PANEL_BK_INTERFACE 10016
#define wxID_STATIC_BK_IID 10037
#define ID_TEXTCTRL_BK_IID 10038
#define wxID_STATIC_BK_ADDRESS 10039
#define ID_TEXTCTRL_BK_ADDRESS 10040
#define wxID_STATIC_BK_PORT 10041
#define ID_TEXTCTRL_BK_PORT 10042
#define wxID_STATIC_BK_TD 10203
#define ID_CHECKBOX_BKTD 10202
#define ID__PANEL_BK_PERMISSIONS 10242
#define ID_LISTBOX_INCLUDE 10337
#define ID_LISTBOX_EXCLUDE 10338
#define SYMBOL_BLOCKDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_BLOCKDIALOG_TITLE _("Blocks")
#define SYMBOL_BLOCKDIALOG_IDNAME ID_DIALOG_BLOCKS
#define SYMBOL_BLOCKDIALOG_SIZE wxDefaultSize
#define SYMBOL_BLOCKDIALOG_POSITION wxDefaultPosition
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
 * BlockDialog class declaration
 */

class BlockDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( BlockDialog )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initIndex();
  void initValues();
  bool evaluate();
  bool m_bSave;
  void initSensors();
  void initSensorCombos();
  void evaluateSensors();
  int m_TabAlign;
  void initSignalCombos();
  void initSwitchCombo();
  void initTurntableCombo();
  void initLocPermissionList();

public:
    /// Constructors
    BlockDialog( wxWindow* parent, iONode p_Props, bool save=true );
    BlockDialog( );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_BLOCKDIALOG_IDNAME, const wxString& caption = SYMBOL_BLOCKDIALOG_TITLE, const wxPoint& pos = SYMBOL_BLOCKDIALOG_POSITION, const wxSize& size = SYMBOL_BLOCKDIALOG_SIZE, long style = SYMBOL_BLOCKDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();
    iONode getProperties(){ return m_Props;}
    void OnSelectPage( wxCommandEvent& event );
    void fbProps( const char* fbid );

////@begin BlockDialog event handler declarations

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_BLOCKS
    void OnListboxBlocksSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BLOCK_NEW
    void OnButtonBlockNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BLOCK_DELETE
    void OnButtonBlockDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_LOCPROPS
    void OnButtonBkLocpropsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for wxID_BLOCK_WAIT
    void OnBlockWaitClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_BLOCK_TURNOUTS
    void OnListboxBlockTurnoutsSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BLOCK_ADD_TURNOUT
    void OnButtonBlockAddTurnoutClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BLOCK_REMOVE_TURNOUT
    void OnButtonBlockRemoveTurnoutClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_SIGNAL
    void OnButtonBkSignalClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_WSIGNAL
    void OnButtonBkWsignalClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_SIGNAL_R
    void OnButtonBkSignalRClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BK_WSIGNAL_R
    void OnButtonBkWsignalRClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIOBOX_BK_WAIT
    void OnRadioboxBkWaitSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_BK_ROUTES
    void OnListboxBkRoutesSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_BLOCK_SENSOR_ID1
    void OnBlockSensor( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_BLOCKS_SENSOR_PROPS1
    void OnSensorProps( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON1
    void OnButtonBkRoutePropsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON2
    void OnButtonBkRouteTestClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
    void OnApplyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

////@end BlockDialog event handler declarations

////@begin BlockDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end BlockDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin BlockDialog member variables
    wxNotebook* m_Notebook;
    wxPanel* m_IndexPanel;
    wxListBox* m_List;
    wxButton* m_New;
    wxButton* m_Delete;
    wxPanel* m_General_Panel;
    wxStaticText* m_label_ID;
    wxTextCtrl* m_ID;
    wxStaticText* m_Label_Description;
    wxTextCtrl* m_Description;
    wxStaticText* m_Label_Length;
    wxTextCtrl* m_Length;
    wxStaticText* m_LabelLocID;
    wxTextCtrl* m_LocID;
    wxButton* m_LocProps;
    wxStaticText* m_labTurntableID;
    wxChoice* m_TurntableID;
    wxRadioBox* m_Commuter;
    wxStaticBoxSizer* m_Configuration;
    wxCheckBox* m_Electrified;
    wxCheckBox* m_Station;
    wxCheckBox* m_Closed;
    wxCheckBox* m_WaitFlag;
    wxCheckBox* m_SmallSymbol;
    wxCheckBox* m_GoManual;
    wxCheckBox* m_AcceptGhost;
    wxCheckBox* m_Swapplacing;
    wxCheckBox* m_Road;
    wxCheckBox* m_AllowChgDir;
    wxPanel* m_LocationPanel;
    wxStaticBox* m_labOverlappedTurnouts;
    wxListBox* m_TurnoutList;
    wxChoice* m_ChoiceTurnouts;
    wxButton* m_AddTurnout;
    wxButton* m_RemoveTurnout;
    wxStaticBox* m_Location;
    wxStaticText* m_LabelX;
    wxTextCtrl* m_x;
    wxStaticText* m_LabelY;
    wxTextCtrl* m_y;
    wxStaticText* m_LabelZ;
    wxTextCtrl* m_z;
    wxTextCtrl* m_Ori;
    wxPanel* m_PanelWirering;
    wxStaticBox* m_ForwardSignalsBox;
    wxStaticText* m_LabelSignal;
    wxComboBox* m_Signal;
    wxButton* m_SignalProps;
    wxStaticText* m_LabelWSignal;
    wxComboBox* m_WSignal;
    wxButton* m_WSignalProps;
    wxStaticBox* m_ReverseSignalsBox;
    wxStaticText* m_LabelSignalR;
    wxComboBox* m_SignalR;
    wxButton* m_SignalPropsR;
    wxStaticText* m_LabelWSignalR;
    wxComboBox* m_WSignalR;
    wxButton* m_WSignalPropsR;
    wxPanel* m_PanelDetails;
    wxRadioBox* m_Type;
    wxRadioBox* m_Incline;
    wxRadioBox* m_Wait;
    wxStaticBoxSizer* m_WaitDetails;
    wxStaticText* m_LabelRandomMin;
    wxTextCtrl* m_RandomMin;
    wxStaticText* m_LabelRandomMax;
    wxTextCtrl* m_RandomMax;
    wxStaticText* m_LabelFixed;
    wxTextCtrl* m_Fixed;
    wxStaticBox* m_Measurement;
    wxStaticText* m_labScale;
    wxTextCtrl* m_Scale;
    wxStaticText* m_labDistance;
    wxTextCtrl* m_Distance;
    wxCheckBox* m_MPH;
    wxStaticBox* m_SpeedBox;
    wxRadioBox* m_Speed;
    wxSpinCtrl* m_SpeedPercent;
    wxRadioBox* m_ExitSpeed;
    wxPanel* m_RoutesPanel;
    wxListBox* m_Routes;
    wxStaticText* m_LabelSensorsFromBlock;
    wxStaticText* m_LabelSensorIDs;
    wxStaticText* m_LabelSensorActions;
    wxStaticText* m_LabelSensorEnd;
    wxStaticText* m_LabelProps;
    wxComboBox* m_SensorID1;
    wxComboBox* m_SensorAction1;
    wxCheckBox* m_SensorEnd1;
    wxButton* m_SensorProps1;
    wxComboBox* m_SensorID2;
    wxComboBox* m_SensorAction2;
    wxCheckBox* m_SensorEnd2;
    wxButton* m_SensorProps2;
    wxComboBox* m_SensorID3;
    wxComboBox* m_SensorAction3;
    wxCheckBox* m_SensorEnd3;
    wxButton* m_SensorProps3;
    wxComboBox* m_SensorID4;
    wxComboBox* m_SensorAction4;
    wxCheckBox* m_SensorEnd4;
    wxButton* m_SensorProps4;
    wxComboBox* m_SensorID5;
    wxComboBox* m_SensorAction5;
    wxCheckBox* m_SensorEnd5;
    wxButton* m_SensorProps5;
    wxStaticText* m_labTimer;
    wxTextCtrl* m_Timer;
    wxButton* m_RouteProps;
    wxButton* m_RouteTest;
    wxPanel* m_Interface;
    wxStaticText* m_Labeliid;
    wxTextCtrl* m_iid;
    wxStaticText* m_LabelAddress;
    wxTextCtrl* m_Address;
    wxStaticText* m_LabelPort;
    wxTextCtrl* m_Port;
    wxStaticText* m_labTD;
    wxCheckBox* m_TD;
    wxPanel* m_PermissionsPanel;
    wxStaticText* m_labInclude;
    wxListBox* m_IncludeList;
    wxStaticText* m_labExclude;
    wxListBox* m_ExcludeList;
    wxButton* m_Cancel;
    wxButton* m_Apply;
    wxButton* m_OK;
    iONode m_Props;
////@end BlockDialog member variables
    iONode m_fbEvents[5];
    const char* m_FromBlockID;
};

#endif
    // _BLOCKDIALOG_H_
