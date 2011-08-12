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
#ifndef _TURNTABLEDIALOG_H_
#define _TURNTABLEDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "turntabledialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
#include "wx/spinctrl.h"
#include "wx/statline.h"
#include "wx/grid.h"
////@end includes

#include "rocs/public/node.h"

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
#define ID_TURNTABLE_DIALOG 10126
#define ID_NOTEBOOK_TURNTABLE 10003
#define ID_PANEL_TT_GENERAL 10000
#define wxID_STATIC_TT_ID 10001
#define ID_TEXTCTRL_TT_ID 10002
#define wxID_STATIC_TT_DESC 10133
#define ID_TEXTCTRL_TT_DESC 10129
#define ID_PANEL_TT_LOCATION 10006
#define wxID_STATIC_TT_X 10010
#define ID_TEXTCTRL_TT_X 10011
#define wxID_STATIC_TT_Y 10012
#define ID_TEXTCTRL_TT_Y 10013
#define wxID_STATIC_TT_Z 10014
#define ID_TEXTCTRL_TT_Z 10015
#define ID_PANEL_TT_INTERFACE 10130
#define wxID_STATIC_TT_IID 10127
#define ID_TEXTCTRL_TT_IID 10128
#define wxID_STATIC_TT_BUS 10256
#define ID_TEXTCTRL_TT_BUS 10255
#define wxID_STATIC_TT_ADDRESS 10131
#define ID_TEXTCTRL_TT_ADDRESS 10132
#define wxID_STATIC_TT_TYPE 10004
#define ID_COMBOBOX_TT_TYPE 10005
#define ID_MULTIPORTPANEL 10277
#define ID_PANEL_TT_TRACKS 10007
#define ID_GRID 10008
#define ID_BUTTON_TT_ADDTRACK 10106
#define ID_BUTTON_TT_DELTRACK 10112
#define ID_BUTTON_TT_MODIFYTRACK 10113
#define SYMBOL_TURNTABLEDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_TURNTABLEDIALOG_TITLE _("TurntableDialog")
#define SYMBOL_TURNTABLEDIALOG_IDNAME ID_TURNTABLE_DIALOG
#define SYMBOL_TURNTABLEDIALOG_SIZE wxDefaultSize
#define SYMBOL_TURNTABLEDIALOG_POSITION wxDefaultPosition
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
 * TurntableDialog class declaration
 */

class TurntableDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( TurntableDialog )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initValues();
  bool evaluate();
  void OnProps(wxCommandEvent& event);
  void OnTTTrackAdd(wxCommandEvent& event);
  void OnTTTrackDelete(wxCommandEvent& event);
  int m_TabAlign;


public:
    /// Constructors
    TurntableDialog( );
    TurntableDialog( wxWindow* parent, iONode p_Props );
    TurntableDialog( wxWindow* parent, wxWindowID id = SYMBOL_TURNTABLEDIALOG_IDNAME, const wxString& caption = SYMBOL_TURNTABLEDIALOG_TITLE, const wxPoint& pos = SYMBOL_TURNTABLEDIALOG_POSITION, const wxSize& size = SYMBOL_TURNTABLEDIALOG_SIZE, long style = SYMBOL_TURNTABLEDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_TURNTABLEDIALOG_IDNAME, const wxString& caption = SYMBOL_TURNTABLEDIALOG_TITLE, const wxPoint& pos = SYMBOL_TURNTABLEDIALOG_POSITION, const wxSize& size = SYMBOL_TURNTABLEDIALOG_SIZE, long style = SYMBOL_TURNTABLEDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin TurntableDialog event handler declarations

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_COMBOBOX_TT_TYPE
    void OnTypeSelected( wxCommandEvent& event );

    /// wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID
    void OnCellLeftClick( wxGridEvent& event );

    /// wxEVT_GRID_CELL_RIGHT_CLICK event handler for ID_GRID
    void OnCellRightClick( wxGridEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_TT_ADDTRACK
    void OnButtonTtAddtrackClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_TT_DELTRACK
    void OnButtonTtDeltrackClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_TT_MODIFYTRACK
    void OnButtonTtModifytrackClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end TurntableDialog event handler declarations

////@begin TurntableDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    char* GetTrackNr() const { return m_TrackNr ; }
    void SetTrackNr(char* value) { m_TrackNr = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end TurntableDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin TurntableDialog member variables
    wxNotebook* m_Notebook;
    wxPanel* m_GeneralPanel;
    wxStaticText* m_LabelID;
    wxTextCtrl* m_ID;
    wxStaticText* m_LabelDesc;
    wxTextCtrl* m_Desc;
    wxStaticBoxSizer* m_OptionsBox;
    wxCheckBox* m_Manager;
    wxCheckBox* m_EmbeddedBlock;
    wxCheckBox* m_Traverser;
    wxPanel* m_LocationPanel;
    wxStaticText* m_LabelX;
    wxTextCtrl* m_x;
    wxStaticText* m_LabelY;
    wxTextCtrl* m_y;
    wxStaticText* m_LabelZ;
    wxTextCtrl* m_z;
    wxStaticText* m_labSize;
    wxSpinCtrl* m_Size;
    wxPanel* m_Interface;
    wxStaticText* m_Labeliid;
    wxTextCtrl* m_IID;
    wxStaticText* m_Label_Bus;
    wxTextCtrl* m_Bus;
    wxStaticText* m_LabelAddress;
    wxTextCtrl* m_Address;
    wxTextCtrl* m_AddressDir;
    wxStaticText* m_labPolAddr;
    wxTextCtrl* m_PolAddr;
    wxStaticText* m_labActFn;
    wxSpinCtrl* m_ActFn;
    wxStaticText* m_labMotorOffDelay;
    wxSpinCtrl* m_MotorOffDelay;
    wxStaticText* m_LabelType;
    wxComboBox* m_Type;
    wxStaticText* m_labV;
    wxSpinCtrl* m_V;
    wxStaticText* m_labDelay;
    wxSpinCtrl* m_Delay;
    wxStaticText* m_labPause;
    wxSpinCtrl* m_Pause;
    wxStaticText* m_labProt;
    wxComboBox* m_Prot;
    wxStaticText* m_labBridgeSensor1;
    wxComboBox* m_BridgeSensor1;
    wxStaticText* m_labBridgeSensor2;
    wxComboBox* m_BridgeSensor2;
    wxStaticText* m_labBridgeSensorMid;
    wxComboBox* m_BridgeSensorMid;
    wxStaticText* m_labBridgeSensorMid2;
    wxComboBox* m_BridgeSensorMid2;
    wxStaticText* m_labPSen;
    wxComboBox* m_PSen;
    wxCheckBox* m_SwapRotation;
    wxPanel* m_MultiPortPanel;
    wxStaticBox* m_PositionBox;
    wxStaticText* m_labAddr0;
    wxTextCtrl* m_Addr0;
    wxStaticText* m_labPort0;
    wxTextCtrl* m_Port0;
    wxRadioBox* m_Bit0Cmd;
    wxStaticText* m_labAddr1;
    wxTextCtrl* m_Addr1;
    wxStaticText* m_labPort1;
    wxTextCtrl* m_Port1;
    wxRadioBox* m_Bit1Cmd;
    wxStaticText* m_labAddr2;
    wxTextCtrl* m_Addr2;
    wxStaticText* m_labPort2;
    wxTextCtrl* m_Port2;
    wxRadioBox* m_Bit2Cmd;
    wxStaticText* m_labAddr3;
    wxTextCtrl* m_Addr3;
    wxStaticText* m_labPort3;
    wxTextCtrl* m_Port3;
    wxRadioBox* m_Bit3Cmd;
    wxStaticText* m_labAddr4;
    wxTextCtrl* m_Addr4;
    wxStaticText* m_labPort4;
    wxTextCtrl* m_Port4;
    wxRadioBox* m_Bit4Cmd;
    wxStaticText* m_labAddr6;
    wxTextCtrl* m_Addr6;
    wxStaticText* m_labPort6;
    wxTextCtrl* m_Port6;
    wxRadioBox* m_Bit5Cmd;
    wxCheckBox* m_InvertPos;
    wxStaticBox* m_NewPositionFlagBox;
    wxStaticText* m_labAddr5;
    wxTextCtrl* m_Addr5;
    wxStaticText* m_labPort5;
    wxTextCtrl* m_Port5;
    wxCheckBox* m_InvertNew;
    wxStaticBox* m_ResetFlagBox;
    wxStaticText* m_labResetAddr;
    wxTextCtrl* m_ResetAddr;
    wxStaticText* m_labResetPort;
    wxTextCtrl* m_ResetPort;
    wxRadioBox* m_ResetCmd;
    wxPanel* m_TracksPanel;
    wxGrid* m_TracksGrid;
    wxButton* m_AddTrack;
    wxButton* m_DelTrack;
    wxButton* m_ModifyTrack;
    wxButton* m_OK;
    wxButton* m_Cancel;
    iONode m_Props;
    char* m_TrackNr;
////@end TurntableDialog member variables
};

#endif
    // _TURNTABLEDIALOG_H_
