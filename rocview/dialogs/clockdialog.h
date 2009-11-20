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
#ifndef _CLOCKDIALOG_H_
#define _CLOCKDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "clockdialog.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/spinctrl.h"
////@end includes

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
#define ID_CLOCKDIALOG 10315
#define SYMBOL_CLOCKDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_CLOCKDIALOG_TITLE _("Dialog")
#define SYMBOL_CLOCKDIALOG_IDNAME ID_CLOCKDIALOG
#define SYMBOL_CLOCKDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_CLOCKDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * ClockDialog class declaration
 */

class ClockDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( ClockDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    ClockDialog();
    ClockDialog( wxWindow* parent, wxWindowID id = SYMBOL_CLOCKDIALOG_IDNAME, const wxString& caption = SYMBOL_CLOCKDIALOG_TITLE, const wxPoint& pos = SYMBOL_CLOCKDIALOG_POSITION, const wxSize& size = SYMBOL_CLOCKDIALOG_SIZE, long style = SYMBOL_CLOCKDIALOG_STYLE );

    void setClock( int divider, int hours, int minutes );
    void getClock( int* divider, int* hours, int* minutes );
    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_CLOCKDIALOG_IDNAME, const wxString& caption = SYMBOL_CLOCKDIALOG_TITLE, const wxPoint& pos = SYMBOL_CLOCKDIALOG_POSITION, const wxSize& size = SYMBOL_CLOCKDIALOG_SIZE, long style = SYMBOL_CLOCKDIALOG_STYLE );

    /// Destructor
    ~ClockDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin ClockDialog event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end ClockDialog event handler declarations

////@begin ClockDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end ClockDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin ClockDialog member variables
    wxStaticBoxSizer* m_ClockBox;
    wxStaticText* m_labDevider;
    wxSpinCtrl* m_Divider;
    wxStaticText* m_labHour;
    wxSpinCtrl* m_Hour;
    wxStaticText* m_labMinute;
    wxSpinCtrl* m_Minute;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end ClockDialog member variables
};

#endif
    // _CLOCKDIALOG_H_
