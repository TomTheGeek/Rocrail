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
#ifndef _LOCDIALOG_H_
#define _LOCDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "locdialog.cpp"
#endif

/*!
 * Includes
 */
#include "rocs/public/node.h"

////@begin includes
#include "wx/notebook.h"
#include "wx/statline.h"
#include "wx/spinctrl.h"
#include "wx/grid.h"
////@end includes

#include "basedlg.h"


/*!
 * Forward declarations
 */

////@begin forward declarations
class wxNotebook;
class wxBoxSizer;
class wxSpinCtrl;
class wxGrid;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_LOC 10000
#define ID_BITMAPBUTTON 10123
#define ID_NOTEBOOK 10001
#define ID_PANEL_INDEX 10060
#define ID_LISTBOX 10120
#define ID_BUTTON_LOC_NEW 10121
#define ID_BUTTON_DELETE 10122
#define ID_BUTTON_LOCO_DOC 10353
#define ID_LOC_COPY 10280
#define ID_BUTTON_IMPORTLOCOS 10253
#define ID_PANEL_GENERAL 10004
#define wxID_STATIC_ID 10017
#define ID_TEXTCTRL_ID 10005
#define wxID_STATIC_DESCRIPTION 10018
#define ID_TEXTCTRL_DESCRIPTION 10006
#define wxID_STATIC_LENGTH 10019
#define ID_TEXTCTRL_LENGTH 10007
#define wxID_STATIC_CATALOGNR 10020
#define ID_TEXTCTRL_CATALOGNR 10008
#define wxID_STATIC_PURCHASED 10021
#define ID_TEXTCTRL_PURCHASED 10009
#define ID_STATICTEXT 10063
#define ID_STATICTEXT1 10064
#define ID_LC_SERVICED 10313
#define ID_LOC_ACTIONS 10380
#define ID_PANEL_INTERFACE 10010
#define wxID_STATIC_ADDRESS 10023
#define ID_TEXTCTRL_ADDRESS 10011
#define wxID_STATIC_IID 10024
#define ID_TEXTCTRL_IID 10012
#define wxID_STATIC_LC_BUS 10250
#define ID_TEXTCTRL_LC_BUS 10249
#define wxID_STATIC_PROTOCOL 10025
#define ID_CHOICE_PROTOCOL 10013
#define wxID_STATIC_PROT_VER 10176
#define ID_TEXTCTRL_PROTVER 10175
#define wxID_STATIC_SPEEDSTEPS 10177
#define ID_TEXTCTRL_SPEEDSTEPS 10178
#define wxID_STATIC_FUNCNT 10179
#define ID_TEXTCTRL_FUNCNT 10180
#define wxID_STATIC_V_MIN 10026
#define ID_TEXTCTRL_V_MIN 10014
#define wxID_STATIC_V_MAX 10027
#define ID_TEXTCTRL_V_MAX 10015
#define wxID_STATIC_LOC_V_STEP 10033
#define ID_TEXTCTRL_LOC_ACCEL 10034
#define wxID_STATIC_LOC_V_MODE 10116
#define ID_CHECKBOX_LOC_CONTROLLED 10117
#define wxID_STATIC_MASS 10028
#define ID_TEXTCTRL_MASS 10029
#define ID_LOC_REGULATED 10375
#define wxID_STATIC_LOC_PLACING 10118
#define ID_CHECKBOX_LOC_PLACING 10119
#define ID_PANEL_DETAILS 10030
#define wxID_STATIC_BLOCKWAIT 10031
#define ID_TEXTCTRL_BLOCKWAIT 10032
#define ID_CHECKBOX_COMMUTER 10114
#define ID_CHECKBOX_LC_SAMEDIR 10162
#define ID_CHECKBOX_LC_FORCESAMEDIR 10163
#define ID_RADIOBOX_ENGINE 10002
#define ID_RADIOBOX_CARGO 10003
#define ID_PANEL_FUNCTIONS 10035
#define ID_BUTTON_F0 10061
#define ID_LOC_FN_GROUP_PREV 10054
#define ID_LOC_FN_GROUP 10276
#define wxID_STATIC_F1 10044
#define ID_TEXTCTRL_F1 10036
#define ID_BUTTON_F1 10037
#define wxID_STATIC_F2 10045
#define ID_TEXTCTRL_F2 10038
#define ID_BUTTON_F2 10039
#define wxID_STATIC_F3 10046
#define ID_TEXTCTRL_F3 10040
#define ID_BUTTON_F3 10041
#define wxID_STATIC_F4 10047
#define ID_TEXTCTRL_F4 10042
#define ID_BUTTON_F4 10043
#define ID_PANEL_LOC_CONSIST 10200
#define ID_BUTTON_LOC_CONSIST_ADD 10201
#define ID_BUTTON_LOC_CONSIST_DELETE 10213
#define ID_PANEL_LOC_CV 10345
#define ID_GRID_LOC_CV 10346
#define ID_BUTTON_LC_CV_DESC 10347
#define SYMBOL_LOCDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_LOCDIALOG_TITLE _("Dialog")
#define SYMBOL_LOCDIALOG_IDNAME ID_DIALOG_LOC
#define SYMBOL_LOCDIALOG_SIZE wxDefaultSize
#define SYMBOL_LOCDIALOG_POSITION wxDefaultPosition
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
 * LocDialog class declaration
 */

class LocDialog: public wxDialog, public BaseDialog
{
    DECLARE_EVENT_TABLE()

  void initLabels();
  void InitIndex();
  void InitValues();
  void initFunctions();
  bool Evaluate();
  bool evaluateFunctions();
  bool m_bSave;
  void EditFunction( int nr, wxString txt );
  int m_TabAlign;
  void initCVDesc();
  const char* m_CVDesc[257];
  int m_iSelectedCV;
  iONode m_CVNodes[257];
  int m_iFunGroup;

public:
    /// Constructors
    LocDialog( wxWindow* parent, iONode p_Props, bool save=true);

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_LOCDIALOG_IDNAME, const wxString& caption = SYMBOL_LOCDIALOG_TITLE, const wxPoint& pos = SYMBOL_LOCDIALOG_POSITION, const wxSize& size = SYMBOL_LOCDIALOG_SIZE, long style = SYMBOL_LOCDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();
    iONode getProperties(){ return m_Props;}
    void OnSelectPage( wxCommandEvent& event );
    void SelectNext();
    void SelectPrev();

////@begin LocDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BITMAPBUTTON
    void OnBitmapbuttonClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX
    void OnListboxSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOC_NEW
    void OnButtonLocNewClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_DELETE
    void OnButtonDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOCO_DOC
    void OnButtonLocoDocClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LOC_COPY
    void OnLocCopyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_IMPORTLOCOS
    void OnButtonImportlocosClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LC_SERVICED
    void OnLcServicedClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LOC_ACTIONS
    void OnLocActionsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_TEXTCTRL_SPEEDSTEPS
    void OnTextctrlSpeedstepsTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_TEXT_ENTER event handler for ID_TEXTCTRL_SPEEDSTEPS
    void OnTextctrlSpeedstepsEnter( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_LOC_REGULATED
    void OnLocRegulatedClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F0
    void OnButtonF0Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LOC_FN_GROUP_PREV
    void OnLocFnGroupPrevClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_LOC_FN_GROUP
    void OnLocFnGroupClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F1
    void OnButtonF1Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F2
    void OnButtonF2Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F3
    void OnButtonF3Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_F4
    void OnButtonF4Click( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOC_CONSIST_ADD
    void OnButtonLocConsistAddClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LOC_CONSIST_DELETE
    void OnButtonLocConsistDeleteClick( wxCommandEvent& event );

    /// wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_GRID_LOC_CV
    void OnCVCellLeftClick( wxGridEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_LC_CV_DESC
    void OnButtonLcCvDescClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
    void OnApplyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

////@end LocDialog event handler declarations

////@begin LocDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end LocDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin LocDialog member variables
    wxBitmapButton* m_LocImage;
    wxNotebook* m_Notebook;
    wxPanel* m_IndexPanel;
    wxListBox* m_List;
    wxButton* m_New;
    wxButton* m_Delete;
    wxButton* m_Doc;
    wxButton* m_Copy;
    wxButton* m_ImportLocos;
    wxPanel* m_General_Panel;
    wxBoxSizer* m_GeneralSizer;
    wxStaticText* m_label_ID;
    wxTextCtrl* m_ID;
    wxStaticText* m_labRoadname;
    wxTextCtrl* m_Roadname;
    wxStaticText* m_labNumber;
    wxTextCtrl* m_Number;
    wxStaticText* m_Label_Description;
    wxTextCtrl* m_Description;
    wxStaticText* m_labImageName;
    wxTextCtrl* m_ImageName;
    wxSpinCtrl* m_ImageNr;
    wxStaticText* m_Label_Length;
    wxTextCtrl* m_Length;
    wxStaticText* m_Label_CatalogNr;
    wxTextCtrl* m_CatNr;
    wxStaticText* m_Label_Purchased;
    wxTextCtrl* m_Purchased;
    wxStaticText* m_labShortID;
    wxTextCtrl* m_ShortID;
    wxStaticText* m_labIdent;
    wxTextCtrl* m_Ident;
    wxStaticText* m_Label_Runtime;
    wxTextCtrl* m_RunTime;
    wxStaticText* m_labMTime;
    wxTextCtrl* m_MTime;
    wxCheckBox* m_Show;
    wxCheckBox* m_UseShortID;
    wxButton* m_Serviced;
    wxStaticText* m_labMInt;
    wxSpinCtrl* m_MInt;
    wxStaticText* m_labRemark;
    wxTextCtrl* m_Remark;
    wxButton* m_Actions;
    wxPanel* m_Interface_Panel;
    wxStaticText* m_Label_Address;
    wxTextCtrl* m_Address;
    wxStaticText* m_Label_IID;
    wxTextCtrl* m_IID;
    wxStaticText* m_Label_Bus;
    wxTextCtrl* m_Bus;
    wxStaticText* m_Label_Protocol;
    wxChoice* m_Protocol;
    wxStaticText* m_LabelProtVersion;
    wxTextCtrl* m_ProtVersion;
    wxStaticText* m_LabelSpeedSteps;
    wxTextCtrl* m_SpeedSteps;
    wxStaticText* m_LabelFunCnt;
    wxTextCtrl* m_FunCnt;
    wxStaticText* m_Label_V_min;
    wxTextCtrl* m_V_min;
    wxStaticText* m_Label_V_Rmin;
    wxTextCtrl* m_V_Rmin;
    wxStaticText* m_Label_V_mid;
    wxTextCtrl* m_V_mid;
    wxStaticText* m_Label_V_Rmid;
    wxTextCtrl* m_V_Rmid;
    wxStaticText* m_Label_V_max;
    wxTextCtrl* m_V_max;
    wxStaticText* m_Label_V_Rmax;
    wxTextCtrl* m_V_Rmax;
    wxStaticText* m_LabelV_step;
    wxTextCtrl* m_Accel;
    wxStaticText* m_LabelV_mode;
    wxCheckBox* m_V_mode_percent;
    wxStaticText* m_Label_Mass;
    wxTextCtrl* m_Mass;
    wxCheckBox* m_Regulated;
    wxStaticText* m_LabelPlacing;
    wxCheckBox* m_Placing;
    wxCheckBox* m_RestoreFx;
    wxStaticText* m_labDirPause;
    wxTextCtrl* m_DirPause;
    wxPanel* m_DetailsPanel;
    wxStaticText* m_Label_Blockwait;
    wxTextCtrl* m_Blockwait;
    wxStaticText* m_labEventTimer;
    wxTextCtrl* m_EventTimer;
    wxStaticText* m_labEvtCorrection;
    wxSpinCtrl* m_EvtCorrection;
    wxStaticText* m_labPriority;
    wxSpinCtrl* m_Priority;
    wxCheckBox* m_Commuter;
    wxCheckBox* m_SameDir;
    wxCheckBox* m_OppositeDir;
    wxCheckBox* m_ForceSameDir;
    wxCheckBox* m_UseScheduleTime;
    wxCheckBox* m_secondNextBlock;
    wxCheckBox* m_ShortIn;
    wxCheckBox* m_InAtPre2In;
    wxCheckBox* m_UseManualRoutes;
    wxRadioBox* m_EngineBox;
    wxRadioBox* m_CargoBox;
    wxPanel* m_FunctionPanel;
    wxStaticText* m_Label_f0;
    wxTextCtrl* m_f0;
    wxSpinCtrl* m_TimerF0;
    wxButton* m_Button_f0;
    wxStaticText* m_labFnGroup;
    wxButton* m_FunctionGroup;
    wxStaticText* m_Label_f1;
    wxTextCtrl* m_f1;
    wxSpinCtrl* m_TimerF1;
    wxButton* m_Button_f1;
    wxStaticText* m_Label_f2;
    wxTextCtrl* m_f2;
    wxSpinCtrl* m_TimerF2;
    wxButton* m_Button_f2;
    wxStaticText* m_Label_f3;
    wxTextCtrl* m_f3;
    wxSpinCtrl* m_TimerF3;
    wxButton* m_Button_f3;
    wxStaticText* m_Label_f4;
    wxTextCtrl* m_f4;
    wxSpinCtrl* m_TimerF4;
    wxButton* m_Button_f4;
    wxPanel* m_ConsistsPanel;
    wxStaticBox* m_labDetails;
    wxCheckBox* m_LightsOff;
    wxStaticText* m_labConsistList;
    wxListBox* m_ConsistList;
    wxStaticText* m_labConsistLocID;
    wxComboBox* m_ConsistLocID;
    wxButton* m_AddConsistLoc;
    wxButton* m_DeleteConsistLoc;
    wxPanel* m_CVPanel;
    wxGrid* m_CVList;
    wxStaticText* m_labCVDesc;
    wxTextCtrl* m_CVDescription;
    wxButton* m_CVDescModify;
    wxButton* m_Cancel;
    wxButton* m_Apply;
    wxButton* m_OK;
    iONode m_Props;
////@end LocDialog member variables
};

/*!
 * LocDialog class declaration
 */


#endif
    // _LOCDIALOG_H_
