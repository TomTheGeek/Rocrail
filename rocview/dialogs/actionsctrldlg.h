/** ------------------------------------------------------------
  * Module:
  * Object:
  * ------------------------------------------------------------
  * $Source: /cvsroot/rojav/rocgui/dialogs/rocgui-dialogs.pjd,v $
  * $Author: robvrs $
  * $Date: 2006/02/22 14:10:57 $
  * $Revision: 1.63 $
  * $Name:  $
  */
#ifndef _ACTIONSCTRLDLG_H_
#define _ACTIONSCTRLDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "actionsctrldlg.h"
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
class wxNotebook;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_ACTIONSCTRLDLG 10191
#define ID_ACTIONCTRL_LIST 10235
#define ID_RADIOBOX 10331
#define ID_ACTIONCTRL_ADD 10192
#define ID_ACTIONCTRL_DELETE 10193
#define ID_ACTIONCTRL_MODIFY 10194
#define ID_CONDITIONS 10329
#define ID_ACTIONCTRL_COND_TYPE 10342
#define ID_ACTIOINCTRL_COND_ID 10344
#define ID_ACTIONCTRL_COND_ADD 10376
#define ID_ACTIONCTRL_COND_DELETE 10377
#define ID_ACTIONCTRL_COND_MODIFY 10378
#define SYMBOL_ACTIONSCTRLDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_ACTIONSCTRLDLG_TITLE _("ActionsCtrlDlg")
#define SYMBOL_ACTIONSCTRLDLG_IDNAME ID_ACTIONSCTRLDLG
#define SYMBOL_ACTIONSCTRLDLG_SIZE wxSize(400, 300)
#define SYMBOL_ACTIONSCTRLDLG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * ActionsCtrlDlg class declaration
 */

class ActionsCtrlDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( ActionsCtrlDlg )
    DECLARE_EVENT_TABLE()

    void initIndex();
    void initLabels();
    void initValues();
    void initCondValues();
    void initCondIDs();
    void evaluate();
    void cleanUp();
    void apply();
    void deleteSelected();
    void addActionCtrl();
    
    void evaluateCond();
    void deleteSelectedCond();
    void addActionCond();
    iONode m_Props;

public:
    /// Constructors
    ActionsCtrlDlg();
    ActionsCtrlDlg(wxWindow* parent, iONode node );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_ACTIONSCTRLDLG_IDNAME, const wxString& caption = SYMBOL_ACTIONSCTRLDLG_TITLE, const wxPoint& pos = SYMBOL_ACTIONSCTRLDLG_POSITION, const wxSize& size = SYMBOL_ACTIONSCTRLDLG_SIZE, long style = SYMBOL_ACTIONSCTRLDLG_STYLE );

    /// Destructor
    ~ActionsCtrlDlg();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin ActionsCtrlDlg event handler declarations

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_ACTIONCTRL_LIST
    void OnActionctrlListSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_ADD
    void OnActionctrlAddClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_DELETE
    void OnActionctrlDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_MODIFY
    void OnActionctrlModifyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_CONDITIONS
    void OnConditionsSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHOICE_SELECTED event handler for ID_ACTIONCTRL_COND_TYPE
    void OnActionctrlCondTypeSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHOICE_SELECTED event handler for ID_ACTIOINCTRL_COND_ID
    void OnActioinctrlCondIdSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_COND_ADD
    void OnActionctrlCondAddClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_COND_DELETE
    void OnActionctrlCondDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONCTRL_COND_MODIFY
    void OnActionctrlCondModifyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
    void OnApplyClick( wxCommandEvent& event );

////@end ActionsCtrlDlg event handler declarations

////@begin ActionsCtrlDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end ActionsCtrlDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin ActionsCtrlDlg member variables
    wxNotebook* m_Notebook;
    wxPanel* m_IndexPanel;
    wxListBox* m_CtrlList;
    wxStaticText* m_labID;
    wxChoice* m_ID;
    wxStaticText* m_labState;
    wxTextCtrl* m_State;
    wxRadioBox* m_Auto;
    wxButton* m_Add;
    wxButton* m_Delete;
    wxButton* m_Modify;
    wxPanel* m_ConditionsPanel;
    wxListBox* m_Conditions;
    wxStaticText* m_labCondType;
    wxChoice* m_CondType;
    wxStaticText* m_labCondID;
    wxChoice* m_CondID;
    wxStaticText* m_labCondState;
    wxTextCtrl* m_CondState;
    wxButton* m_CondAdd;
    wxButton* m_CondDelete;
    wxButton* m_CondModify;
    wxButton* m_OK;
    wxButton* m_Cancel;
    wxButton* m_Apply;
////@end ActionsCtrlDlg member variables
};

#endif
    // _ACTIONSCTRLDLG_H_
