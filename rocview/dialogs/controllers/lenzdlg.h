/////////////////////////////////////////////////////////////////////////////
// Name:        lenzdlg.h
// Purpose:
// Author:
// Modified by:
// Created:     Tue 06 Jan 2009 04:49:35 PM CET
// RCS-ID:
// Copyright:
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _LENZDLG_H_
#define _LENZDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "lenzdlg.h"
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
#define ID_LENZDLG 10048
#define ID_LENZTYPE 10048
#define SYMBOL_LENZDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_LENZDLG_TITLE _("Lenz")
#define SYMBOL_LENZDLG_IDNAME ID_LENZDLG
#define SYMBOL_LENZDLG_SIZE wxSize(400, 300)
#define SYMBOL_LENZDLG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * LenzDlg class declaration
 */

class LenzDlg: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( LenzDlg )
    DECLARE_EVENT_TABLE()

    void initLabels();
    void initValues();
    void evaluate();

public:
    /// Constructors
    LenzDlg();
    LenzDlg( wxWindow* parent, iONode props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_LENZDLG_IDNAME, const wxString& caption = SYMBOL_LENZDLG_TITLE, const wxPoint& pos = SYMBOL_LENZDLG_POSITION, const wxSize& size = SYMBOL_LENZDLG_SIZE, long style = SYMBOL_LENZDLG_STYLE );

    /// Destructor
    ~LenzDlg();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin LenzDlg event handler declarations

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_LENZTYPE
    void OnLenztypeSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end LenzDlg event handler declarations

////@begin LenzDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end LenzDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin LenzDlg member variables
    wxPanel* m_MainPanel;
    wxStaticText* m_labIID;
    wxTextCtrl* m_IID;
    wxStaticText* m_labDevice;
    wxTextCtrl* m_Device;
    wxStaticText* m_labHost;
    wxTextCtrl* m_Host;
    wxStaticText* m_labPort;
    wxSpinCtrl* m_Port;
    wxRadioBox* m_BPS;
    wxRadioBox* m_HardwareFlow;
    wxRadioBox* m_Type;
    wxStaticBox* m_labPower;
    wxCheckBox* m_PowerAtStartup;
    wxCheckBox* m_FastClock;
    wxStaticText* m_labSensorOffset;
    wxSpinCtrl* m_SensorOffset;
    wxStaticText* m_labSwitchTime;
    wxSpinCtrl* m_SwitchTime;
////@end LenzDlg member variables
    iONode m_Props;
};

#endif
    // _LENZDLG_H_
