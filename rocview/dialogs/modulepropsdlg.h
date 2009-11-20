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
#ifndef _MODULEPROPSDLG_H_
#define _MODULEPROPSDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "modulepropsdlg.h"
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
#define ID_MODULEPROPSDLG 10358
#define ID_NOTEBOOK_MODPROPS 10359
#define ID_PANEL_MODPROPS_GEN 10360
#define ID_PANEL_MODPROPS_CONN 10361
#define SYMBOL_MODULEPROPSDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_MODULEPROPSDLG_TITLE _("ModuleProperties")
#define SYMBOL_MODULEPROPSDLG_IDNAME ID_MODULEPROPSDLG
#define SYMBOL_MODULEPROPSDLG_SIZE wxSize(400, 300)
#define SYMBOL_MODULEPROPSDLG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * ModulePropsDlg class declaration
 */

class ModulePropsDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( ModulePropsDlg )
    DECLARE_EVENT_TABLE()
    
    void initLabels();
    void initValues();
    void evaluate();
    void deleteConnection( const char* side );
    void addConnection( const char* side, const char* modid );
    int m_TabAlign;
    iONode m_Props;

public:
    /// Constructors
    ModulePropsDlg();
    ModulePropsDlg( wxWindow* parent, iONode p_Props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_MODULEPROPSDLG_IDNAME, const wxString& caption = SYMBOL_MODULEPROPSDLG_TITLE, const wxPoint& pos = SYMBOL_MODULEPROPSDLG_POSITION, const wxSize& size = SYMBOL_MODULEPROPSDLG_SIZE, long style = SYMBOL_MODULEPROPSDLG_STYLE );

    /// Destructor
    ~ModulePropsDlg();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin ModulePropsDlg event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end ModulePropsDlg event handler declarations

////@begin ModulePropsDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end ModulePropsDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin ModulePropsDlg member variables
    wxNotebook* m_NoteBook;
    wxPanel* m_GeneralPanel;
    wxStaticText* m_labTitle;
    wxTextCtrl* m_Title;
    wxStaticText* m_labID;
    wxTextCtrl* m_ModuleID;
    wxStaticText* m_labFile;
    wxTextCtrl* m_File;
    wxStaticText* m_labX;
    wxSpinCtrl* m_X;
    wxStaticText* m_labY;
    wxSpinCtrl* m_Y;
    wxRadioBox* m_Rotation;
    wxPanel* m_ConnectionPanel;
    wxStaticBox* m_NorthBox;
    wxStaticText* m_labNorthModID;
    wxChoice* m_NorthModID;
    wxStaticBox* m_EastBox;
    wxStaticText* m_labEastModID;
    wxChoice* m_EastModID;
    wxStaticBox* m_SouthBox;
    wxStaticText* m_labSouthModID;
    wxChoice* m_SouthModID;
    wxStaticBox* m_WestBox;
    wxStaticText* m_labWestModID;
    wxChoice* m_WestModID;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end ModulePropsDlg member variables
};

#endif
    // _MODULEPROPSDLG_H_
