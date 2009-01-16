/////////////////////////////////////////////////////////////////////////////
// Name:        opendecoder.h
// Purpose:     
// Author:      
// Modified by: 
// Created:     Sat 31 May 2008 07:25:32 CEST
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _OPENDECODER_H_
#define _OPENDECODER_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "opendecoder.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
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
#define ID_OPENDECODERDLG 10013
#define ID_NOTEBOOK_DCC 10014
#define ID_PANEL_DCC_GEN 10015
#define ID_BUTTON_GEN_GET 10025
#define ID_BUTTON_GEN_SET 10026
#define ID_PANEL_DCC_CONFIG 10023
#define ID_PANEL_DCC_TIME 10024
#define SYMBOL_OPENDECODERDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxDIALOG_MODAL|wxTAB_TRAVERSAL
#define SYMBOL_OPENDECODERDLG_TITLE _("OpenDecoder 2")
#define SYMBOL_OPENDECODERDLG_IDNAME ID_OPENDECODERDLG
#define SYMBOL_OPENDECODERDLG_SIZE wxSize(400, 300)
#define SYMBOL_OPENDECODERDLG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * OpenDecoderDlg class declaration
 */

class OpenDecoderDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( OpenDecoderDlg )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    OpenDecoderDlg();
    OpenDecoderDlg( wxWindow* parent, wxWindowID id = SYMBOL_OPENDECODERDLG_IDNAME, const wxString& caption = SYMBOL_OPENDECODERDLG_TITLE, const wxPoint& pos = SYMBOL_OPENDECODERDLG_POSITION, const wxSize& size = SYMBOL_OPENDECODERDLG_SIZE, long style = SYMBOL_OPENDECODERDLG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_OPENDECODERDLG_IDNAME, const wxString& caption = SYMBOL_OPENDECODERDLG_TITLE, const wxPoint& pos = SYMBOL_OPENDECODERDLG_POSITION, const wxSize& size = SYMBOL_OPENDECODERDLG_SIZE, long style = SYMBOL_OPENDECODERDLG_STYLE );

    /// Destructor
    ~OpenDecoderDlg();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin OpenDecoderDlg event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_GEN_GET
    void OnButtonGenGetClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_GEN_SET
    void OnButtonGenSetClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end OpenDecoderDlg event handler declarations

////@begin OpenDecoderDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end OpenDecoderDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin OpenDecoderDlg member variables
    wxPanel* m_GenerelPanel;
    wxStaticText* m_labAddress;
    wxSpinCtrl* m_Address;
    wxStaticText* m_labVersion;
    wxTextCtrl* m_Version;
    wxStaticText* m_labVendor;
    wxTextCtrl* m_Vendor;
    wxButton* m_GenGet;
    wxButton* m_GenSet;
    wxPanel* m_ConfigPanel;
    wxPanel* m_TimePanel;
    wxButton* m_Cancel;
////@end OpenDecoderDlg member variables
};

#endif
    // _OPENDECODER_H_
