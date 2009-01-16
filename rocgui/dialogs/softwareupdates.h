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
#ifndef _SOFTWAREUPDATES_H_
#define _SOFTWAREUPDATES_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "softwareupdates.h"
#endif

/*!
 * Includes
 */

////@begin includes
////@end includes
#include "wx/timer.h"
#include "wx/progdlg.h"

#include "rocs/public/node.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_SOFTWAREUPDATES 10332
#define SYMBOL_SOFTWAREUPDATES_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxDIALOG_MODAL|wxTAB_TRAVERSAL
#define SYMBOL_SOFTWAREUPDATES_TITLE _("Software updates")
#define SYMBOL_SOFTWAREUPDATES_IDNAME ID_SOFTWAREUPDATES
#define SYMBOL_SOFTWAREUPDATES_SIZE wxSize(400, 300)
#define SYMBOL_SOFTWAREUPDATES_POSITION wxDefaultPosition
////@end control identifiers

#define ME_UpdateTimer 10002

void updateReaderThread( void* threadinst );


/*!
 * Softwareupdates class declaration
 */

class Softwareupdates: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( Softwareupdates )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    Softwareupdates();
    Softwareupdates( wxWindow* parent, wxWindowID id = SYMBOL_SOFTWAREUPDATES_IDNAME, const wxString& caption = SYMBOL_SOFTWAREUPDATES_TITLE, const wxPoint& pos = SYMBOL_SOFTWAREUPDATES_POSITION, const wxSize& size = SYMBOL_SOFTWAREUPDATES_SIZE, long style = SYMBOL_SOFTWAREUPDATES_STYLE );

    void initLabels();
    void initValues();
    void OnTimer(wxTimerEvent& event);
    void setReady(bool ok);
    void setConnected(bool ok);
    void checkForNew();

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SOFTWAREUPDATES_IDNAME, const wxString& caption = SYMBOL_SOFTWAREUPDATES_TITLE, const wxPoint& pos = SYMBOL_SOFTWAREUPDATES_POSITION, const wxSize& size = SYMBOL_SOFTWAREUPDATES_SIZE, long style = SYMBOL_SOFTWAREUPDATES_STYLE );

    /// Destructor
    ~Softwareupdates();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin Softwareupdates event handler declarations

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for wxID_ANY
    void OnAnySelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKLISTBOX_TOGGLED event handler for wxID_ANY
    void OnAnyToggled( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end Softwareupdates event handler declarations

////@begin Softwareupdates member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end Softwareupdates member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin Softwareupdates member variables
    wxStaticText* m_labUpdates;
    wxCheckListBox* m_Updates;
    wxStaticText* m_labInfo;
    wxTextCtrl* m_Info;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end Softwareupdates member variables
    
    int m_TimerCount;
    wxTimer* m_Timer;
    wxProgressDialog* m_Progress;
    bool m_Ready;
    bool m_DownloadOK;
    char* m_Release;
    iONode m_ReleaseNode;
    bool m_Connected;
};

#endif
    // _SOFTWAREUPDATES_H_
