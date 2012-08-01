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
#ifndef _SIGNALDIALOG_H_
#define _SIGNALDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "signaldialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
#include "wx/listctrl.h"
#include "wx/spinctrl.h"
#include "wx/statline.h"
////@end includes

#include "basedlg.h"

#include "rocs/public/node.h"

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
#define ID_DIALOG_SIGNAL 10081
#define ID_NOTEBOOK_SG 10045
#define ID_PANEL_SG_INDEX 10005
#define ID_LISTCTRLINDEX_SG 10408
#define ID_BUTTON_SG_NEW 10007
#define ID_BUTTON_SG_DELETE 10008
#define ID_BUTTON_SG_DOC 10373
#define ID_PANEL_SG_GENERAL 10009
#define wxID_STATIC_SG_ID 10010
#define ID_TEXTCTRL_SG_ID 10011
#define wxID_STATICSG_DESC 10224
#define ID_TEXTCTRL_SG_DESC 10223
#define wxID_STATIC_SG_BLOCKID 10170
#define ID_COMBOBOX_SG_BLOCKID 10171
#define wxID_STATIC_SG_STATE 10014
#define ID_TEXTCTRL_SG_STATE 10015
#define ID_SIGNAL_ACTIONS 10198
#define ID_PANEL_SG_LOCATION 10018
#define wxID_STATIC_SG_X 10019
#define ID_TEXTCTRL_SG_X 10000
#define wxID_STATIC_SG_Y 10001
#define ID_TEXTCTRL_SG_Y 10002
#define wxID_STATIC_SG_Z 10003
#define ID_TEXTCTRL_SG_Z 10004
#define wxID_STATIC_SG_SHOW 10214
#define ID_RADIOBOX_SG_ORI 10020
#define ID_PANEL_SG_INTERFACE 10021
#define wxID_STATIC_SG_IID 10022
#define ID_TEXTCTRL_SG_IID 10023
#define wxID_STATIC_SG_BUS 10258
#define ID_TEXTCTRL_SG_BUS 10257
#define wxID_STATIC_SG_PROT 10184
#define ID_CHOICE_SG_PROT 10183
#define ID_CHECKBOX_SG_PAIRGATES 10418
#define ID_SIGNALCONTROL 10393
#define ID_PANEL_SG_PROPS 10208
#define ID_RADIOBOX_SG_TYPE 10215
#define ID_RADIOBOX_SG_SGTYPE 10216
#define wxID_STATIC_SG_PATTERNS 10316
#define ID_STATICTEXT_SG_GREEN 10030
#define ID_RADIOBOX_SG_GREEN1 10031
#define ID_STATICTEXT_SG_RED 10033
#define ID_RADIOBOX_SG_RED1 10034
#define ID_STATICTEXT_SG_YELLOW 10012
#define ID_RADIOBOX_SG_YELLOW1 10013
#define ID_STATICTEXT_SG_WHITE 10017
#define ID_RADIOBOX_SG_WHITE1 10013
#define SYMBOL_SIGNALDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_SIGNALDIALOG_TITLE _("Signals")
#define SYMBOL_SIGNALDIALOG_IDNAME ID_DIALOG_SIGNAL
#define SYMBOL_SIGNALDIALOG_SIZE wxDefaultSize
#define SYMBOL_SIGNALDIALOG_POSITION wxDefaultPosition
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
 * SignalDialog class declaration
 */

class SignalDialog: public wxDialog, public BaseDialog
{
    DECLARE_DYNAMIC_CLASS( SignalDialog )
    DECLARE_EVENT_TABLE()

  void initLabels();
  bool initIndex();
  void initValues();
  bool evaluate();
  int m_TabAlign;

public:
    /// Constructors
    SignalDialog( wxWindow* parent, iONode p_Props );
    SignalDialog( );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SIGNALDIALOG_IDNAME, const wxString& caption = SYMBOL_SIGNALDIALOG_TITLE, const wxPoint& pos = SYMBOL_SIGNALDIALOG_POSITION, const wxSize& size = SYMBOL_SIGNALDIALOG_SIZE, long style = SYMBOL_SIGNALDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();
    iONode getProperties(){ return m_Props;}
    void OnSelectPage( wxCommandEvent& event );

////@begin SignalDialog event handler declarations

    /// wxEVT_COMMAND_LIST_ITEM_SELECTED event handler for ID_LISTCTRLINDEX_SG
    void OnListctrlindexSgSelected( wxListEvent& event );

    /// wxEVT_COMMAND_LIST_COL_CLICK event handler for ID_LISTCTRLINDEX_SG
    void OnListctrlindexSgColLeftClick( wxListEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SG_NEW
    void OnButtonSgNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SG_DELETE
    void OnButtonSgDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SG_DOC
    void OnButtonSgDocClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_SIGNAL_ACTIONS
    void OnSignalActionsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_CHECKBOX_SG_PAIRGATES
    void OnCheckboxSgPairgatesClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_SIGNALCONTROL
    void OnSignalcontrolSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
    void OnApplyClick( wxCommandEvent& event );

////@end SignalDialog event handler declarations

////@begin SignalDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end SignalDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin SignalDialog member variables
    wxNotebook* m_Notebook;
    wxPanel* m_IndexPanel;
    wxListCtrl* m_List2;
    wxButton* m_New;
    wxButton* m_Delete;
    wxButton* m_Doc;
    wxPanel* m_GeneralPanel;
    wxStaticText* m_LabelID;
    wxTextCtrl* m_ID;
    wxStaticText* m_LabelDescription;
    wxTextCtrl* m_Description;
    wxStaticText* m_LabelBlockID;
    wxComboBox* m_BlockID;
    wxStaticText* m_labRouteIDs;
    wxTextCtrl* m_RouteIDs;
    wxStaticText* m_LabelState;
    wxTextCtrl* m_State;
    wxCheckBox* m_Manual;
    wxCheckBox* m_Road;
    wxButton* m_Actions;
    wxPanel* m_LocationPanel;
    wxStaticText* m_LabelX;
    wxTextCtrl* m_x;
    wxStaticText* m_LabelY;
    wxTextCtrl* m_y;
    wxStaticText* m_LabelZ;
    wxTextCtrl* m_z;
    wxRadioBox* m_ori;
    wxPanel* m_InterfacePanel;
    wxStaticText* m_Labeliid;
    wxTextCtrl* m_iid;
    wxStaticText* m_Label_Bus;
    wxTextCtrl* m_Bus;
    wxStaticText* m_LabelProt;
    wxChoice* m_Prot;
    wxStaticBox* m_RedBox;
    wxStaticText* m_labAddress;
    wxStaticText* m_labPort;
    wxTextCtrl* m_Address;
    wxTextCtrl* m_Port;
    wxRadioBox* m_Gate1;
    wxStaticBox* m_YellowBox;
    wxTextCtrl* m_Address2;
    wxTextCtrl* m_Port2;
    wxRadioBox* m_Gate2;
    wxStaticBox* m_GreenBox;
    wxTextCtrl* m_Address3;
    wxTextCtrl* m_Port3;
    wxRadioBox* m_Gate3;
    wxStaticBox* m_WhiteBox;
    wxTextCtrl* m_Address4;
    wxTextCtrl* m_Port4;
    wxRadioBox* m_Gate4;
    wxCheckBox* m_Invert;
    wxCheckBox* m_PairGates;
    wxCheckBox* m_AsSwitch;
    wxStaticText* m_labCmdTime;
    wxSpinCtrl* m_CmdTime;
    wxRadioBox* m_SignalControl;
    wxPanel* m_PropsPanel;
    wxRadioBox* m_Type;
    wxRadioBox* m_Signal;
    wxRadioBox* m_Aspects;
    wxCheckBox* m_Dwarf;
    wxStaticText* m_labAspect;
    wxStaticText* m_labPatternAddr1;
    wxStaticText* m_labPatternAddr2;
    wxStaticText* m_labAspectNr;
    wxStaticText* m_labGreen;
    wxRadioBox* m_Green1;
    wxRadioBox* m_Green2;
    wxSpinCtrl* m_GreenNr;
    wxStaticText* m_labRed;
    wxRadioBox* m_Red1;
    wxRadioBox* m_Red2;
    wxSpinCtrl* m_RedNr;
    wxStaticText* m_labYellow;
    wxRadioBox* m_Yellow1;
    wxRadioBox* m_Yellow2;
    wxSpinCtrl* m_YellowNr;
    wxStaticText* m_labWhite;
    wxRadioBox* m_White1;
    wxRadioBox* m_White2;
    wxSpinCtrl* m_WhiteNr;
    wxStaticText* m_labBlank;
    wxRadioBox* m_Blank1;
    wxRadioBox* m_Blank2;
    wxSpinCtrl* m_BlankNr;
    wxButton* m_Cancel;
    wxButton* m_OK;
    wxButton* m_Apply;
    iONode m_Props;
////@end SignalDialog member variables
};

#endif
    // _SIGNALDIALOG_H_
