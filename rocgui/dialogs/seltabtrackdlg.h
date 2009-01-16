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
#ifndef _SELTABTRACKDLG_H_
#define _SELTABTRACKDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "seltabtrackdlg.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/spinctrl.h"
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
#define ID_DIALOG_SELTABTRACK 10134
#define SYMBOL_SELTABTRACKDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_SELTABTRACKDIALOG_TITLE _("SelTabTrackDialog")
#define SYMBOL_SELTABTRACKDIALOG_IDNAME ID_DIALOG_SELTABTRACK
#define SYMBOL_SELTABTRACKDIALOG_SIZE wxDefaultSize
#define SYMBOL_SELTABTRACKDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * SelTabTrackDialog class declaration
 */

class SelTabTrackDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( SelTabTrackDialog )
    DECLARE_EVENT_TABLE()
    
    iONode m_Props;
    void initLabels();
    void initValues();
    void evaluate();

public:
    /// Constructors
    SelTabTrackDialog();
    SelTabTrackDialog( wxWindow* parent, iONode p_Props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SELTABTRACKDIALOG_IDNAME, const wxString& caption = SYMBOL_SELTABTRACKDIALOG_TITLE, const wxPoint& pos = SYMBOL_SELTABTRACKDIALOG_POSITION, const wxSize& size = SYMBOL_SELTABTRACKDIALOG_SIZE, long style = SYMBOL_SELTABTRACKDIALOG_STYLE );

    /// Destructor
    ~SelTabTrackDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin SelTabTrackDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end SelTabTrackDialog event handler declarations

////@begin SelTabTrackDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end SelTabTrackDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin SelTabTrackDialog member variables
    wxStaticText* m_LabelTrackNr;
    wxSpinCtrl* m_TrackNr;
    wxStaticText* m_LabelDesc;
    wxTextCtrl* m_Desc;
    wxStaticText* m_labBlockID;
    wxComboBox* m_BlockID;
    wxStaticText* m_labSen1;
    wxComboBox* m_Sen1;
    wxStaticText* m_labSen2;
    wxComboBox* m_Sen2;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end SelTabTrackDialog member variables
};

#endif
    // _SELTABTRACKDLG_H_
