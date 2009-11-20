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
#ifndef _MODPLANDLG_H_
#define _MODPLANDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "modplandlg.h"
#endif

/*!
 * Includes
 */
#include "rocs/public/node.h"

////@begin includes
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_MODPLANDLG 10352
#define ID_MODPLAN_SAVE 10374
#define SYMBOL_MODPLANDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_MODPLANDLG_TITLE _("ModPlan")
#define SYMBOL_MODPLANDLG_IDNAME ID_MODPLANDLG
#define SYMBOL_MODPLANDLG_SIZE wxSize(400, 300)
#define SYMBOL_MODPLANDLG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * ModPlanDlg class declaration
 */

class ModPlanDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( ModPlanDlg )
    DECLARE_EVENT_TABLE()

    void initLabels();
    void initValues();
    void evaluate();
    int m_TabAlign;
    iONode m_Props;
    
public:
    /// Constructors
    ModPlanDlg();
    ModPlanDlg( wxWindow* parent, iONode p_Props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_MODPLANDLG_IDNAME, const wxString& caption = SYMBOL_MODPLANDLG_TITLE, const wxPoint& pos = SYMBOL_MODPLANDLG_POSITION, const wxSize& size = SYMBOL_MODPLANDLG_SIZE, long style = SYMBOL_MODPLANDLG_STYLE );

    /// Destructor
    ~ModPlanDlg();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin ModPlanDlg event handler declarations

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_MODPLAN_SAVE
    void OnModplanSaveClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end ModPlanDlg event handler declarations

////@begin ModPlanDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end ModPlanDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin ModPlanDlg member variables
    wxStaticText* m_labTitle;
    wxTextCtrl* m_Title;
    wxStaticText* m_labLocoFile;
    wxTextCtrl* m_LocoFile;
    wxStaticText* m_labRoutesFile;
    wxTextCtrl* m_RoutesFile;
    wxStaticBox* m_OptionsBox;
    wxCheckBox* m_InitField;
    wxCheckBox* m_Save;
    wxCheckBox* m_SaveModules;
    wxCheckBox* m_ModRoutes;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end ModPlanDlg member variables
};

#endif
    // _MODPLANDLG_H_
