/////////////////////////////////////////////////////////////////////////////
// Name:        63350.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     Sat 28 Apr 2007 06:55:51 CEST
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _63350_H_
#define _63350_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "63350.h"
#endif

/*!
 * Includes
 */

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
#define ID_DIALOG_LN_63350 10000
#define ID_PANEL1 10001
#define ID_RADIOBOX_63350_CONFIGURE 10002
#define SYMBOL_UHL_63350_DLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_UHL_63350_DLG_TITLE _("Uhlenbrock 63350")
#define SYMBOL_UHL_63350_DLG_IDNAME ID_DIALOG_LN_63350
#define SYMBOL_UHL_63350_DLG_SIZE wxSize(400, 300)
#define SYMBOL_UHL_63350_DLG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * Uhl_63350_Dlg class declaration
 */

class Uhl_63350_Dlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( Uhl_63350_Dlg )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    Uhl_63350_Dlg();
    Uhl_63350_Dlg( wxWindow* parent, wxWindowID id = SYMBOL_UHL_63350_DLG_IDNAME, const wxString& caption = SYMBOL_UHL_63350_DLG_TITLE, const wxPoint& pos = SYMBOL_UHL_63350_DLG_POSITION, const wxSize& size = SYMBOL_UHL_63350_DLG_SIZE, long style = SYMBOL_UHL_63350_DLG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_UHL_63350_DLG_IDNAME, const wxString& caption = SYMBOL_UHL_63350_DLG_TITLE, const wxPoint& pos = SYMBOL_UHL_63350_DLG_POSITION, const wxSize& size = SYMBOL_UHL_63350_DLG_SIZE, long style = SYMBOL_UHL_63350_DLG_STYLE );

    /// Destructor
    ~Uhl_63350_Dlg();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin Uhl_63350_Dlg event handler declarations

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_RADIOBOX_63350_CONFIGURE
    void OnRadiobox63350ConfigureSelected( wxCommandEvent& event );

////@end Uhl_63350_Dlg event handler declarations

////@begin Uhl_63350_Dlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end Uhl_63350_Dlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin Uhl_63350_Dlg member variables
    wxStaticText* m_labModuleAddress;
    wxTextCtrl* m_ModuleAddress;
    wxStaticText* m_labReportAddress;
    wxTextCtrl* m_ReportAddress;
    wxStaticBox* m_BoxConfig;
    wxCheckBox* m_AutoAddress;
    wxCheckBox* m_IndividualDelays;
    wxCheckBox* m_ReportAtPoweron;
    wxRadioBox* m_Configure;
    wxTextCtrl* m_Input1;
    wxTextCtrl* m_Input2;
    wxTextCtrl* m_Input3;
    wxTextCtrl* m_Input4;
    wxTextCtrl* m_Input5;
    wxTextCtrl* m_Input6;
    wxTextCtrl* m_Input7;
    wxTextCtrl* m_Input8;
    wxTextCtrl* m_Input9;
    wxTextCtrl* m_Input10;
    wxTextCtrl* m_Input11;
    wxTextCtrl* m_Input12;
    wxTextCtrl* m_Input13;
    wxTextCtrl* m_Input14;
    wxTextCtrl* m_Input15;
    wxTextCtrl* m_Input16;
////@end Uhl_63350_Dlg member variables
};

#endif
    // _63350_H_
