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
#ifndef _TIMEDACTIONS_H_
#define _TIMEDACTIONS_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "timedactions.h"
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
#define ID_TIMEDACTIONS 10139
#define ID_LB_TimedActions 10143
#define ID_ACTIONS_TYPE 10195
#define ID_COMBOBOX 10371
#define ID_ACTIONS_EXEC_CMD 10245
#define ID_BT_ADD 10217
#define ID_BT_DEL 10218
#define ID_BT_MODIFY 10219
#define SYMBOL_TIMEDACTIONS_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_TIMEDACTIONS_TITLE _("TimedActions")
#define SYMBOL_TIMEDACTIONS_IDNAME ID_TIMEDACTIONS
#define SYMBOL_TIMEDACTIONS_SIZE wxSize(400, 300)
#define SYMBOL_TIMEDACTIONS_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * TimedActions class declaration
 */

class TimedActions: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( TimedActions )
    DECLARE_EVENT_TABLE()

  void initIndex();
  void initLabels();
  void initValues();
  bool evaluate();
  void initOutputList();
  void initCommands();
  iONode m_Props;

public:
    /// Constructors
    TimedActions();
    TimedActions( wxWindow* parent );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_TIMEDACTIONS_IDNAME, const wxString& caption = SYMBOL_TIMEDACTIONS_TITLE, const wxPoint& pos = SYMBOL_TIMEDACTIONS_POSITION, const wxSize& size = SYMBOL_TIMEDACTIONS_SIZE, long style = SYMBOL_TIMEDACTIONS_STYLE );

    /// Destructor
    ~TimedActions();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin TimedActions event handler declarations

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LB_TimedActions
    void OnLBTimedActionsSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHOICE_SELECTED event handler for ID_ACTIONS_TYPE
    void OnActionsTypeSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ACTIONS_EXEC_CMD
    void OnActionsExecCmdClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BT_ADD
    void OnBtAddClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BT_DEL
    void OnBtDelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BT_MODIFY
    void OnBtModifyClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
    void OnApplyClick( wxCommandEvent& event );

////@end TimedActions event handler declarations

////@begin TimedActions member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end TimedActions member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin TimedActions member variables
    wxStaticText* m_labIndex;
    wxListBox* m_TimedActions;
    wxTextCtrl* m_ActionID;
    wxStaticText* m_labType;
    wxChoice* m_Type;
    wxStaticText* m_labID;
    wxChoice* m_ID;
    wxStaticText* m_labCommand;
    wxComboBox* m_Command;
    wxButton* m_ExecCmd;
    wxStaticText* m_labParam;
    wxTextCtrl* m_Parameter;
    wxStaticText* m_labDuration;
    wxSpinCtrl* m_ActTime;
    wxStaticText* m_labTimer;
    wxSpinCtrl* m_Timer;
    wxCheckBox* m_Timed;
    wxStaticText* m_labHour;
    wxSpinCtrl* m_Hour;
    wxStaticText* m_labMin;
    wxSpinCtrl* m_Min;
    wxButton* m_Add;
    wxButton* m_Del;
    wxButton* m_Modify;
    wxButton* m_OK;
    wxButton* m_Cancel;
    wxButton* m_Apply;
////@end TimedActions member variables
};

#endif
    // _TIMEDACTIONS_H_
