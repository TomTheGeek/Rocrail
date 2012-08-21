/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */
#ifndef _SELECTDIALOG_H_
#define _SELECTDIALOG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "selectdialog.h"
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
#define ID_SELECTDIALOG 10246
#define ID_SELECT_ACTION 10247
#define SYMBOL_SELECTDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_SELECTDIALOG_TITLE _("Select")
#define SYMBOL_SELECTDIALOG_IDNAME ID_SELECTDIALOG
#define SYMBOL_SELECTDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_SELECTDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * SelectDialog class declaration
 */

class SelectDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( SelectDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    SelectDialog();
    SelectDialog( wxWindow* parent, iONode sel );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SELECTDIALOG_IDNAME, const wxString& caption = SYMBOL_SELECTDIALOG_TITLE, const wxPoint& pos = SYMBOL_SELECTDIALOG_POSITION, const wxSize& size = SYMBOL_SELECTDIALOG_SIZE, long style = SYMBOL_SELECTDIALOG_STYLE );

    /// Destructor
    ~SelectDialog();

    /// Initialises member variables
    void Init();
    void initLabels();
    void initValues( iONode sel );
    void evaluate();
    iONode getSelection();
    int m_X;
    int m_Y;
    int m_CX;
    int m_CY;
    int m_destX;
    int m_destY;
    int m_destZ;
    const char* m_destTitle;
    int m_action;

    /// Creates the controls and sizers
    void CreateControls();

////@begin SelectDialog event handler declarations

    /// wxEVT_COMMAND_RADIOBOX_SELECTED event handler for ID_SELECT_ACTION
    void OnSelectActionSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

////@end SelectDialog event handler declarations

////@begin SelectDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end SelectDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin SelectDialog member variables
    wxStaticBox* m_RangeBox;
    wxStaticText* m_labRangeX;
    wxTextCtrl* m_RangeX;
    wxStaticText* m_labRangeY;
    wxTextCtrl* m_RangeY;
    wxStaticText* m_labRangCX;
    wxSpinCtrl* m_RangeCX;
    wxStaticText* m_labRangeCY;
    wxSpinCtrl* m_RangeCY;
    wxStaticBox* m_TargetBox;
    wxStaticText* m_labTargetX;
    wxSpinCtrl* m_TargetX;
    wxStaticText* m_labTargetY;
    wxSpinCtrl* m_TargetY;
    wxStaticText* m_labLevel;
    wxChoice* m_Level;
    wxRadioBox* m_ActionBox;
    wxStaticText* m_labRouteIDs;
    wxTextCtrl* m_RouteIDs;
    wxStaticText* m_labBlockID;
    wxComboBox* m_BlockID;
    wxButton* m_Cancel;
    wxButton* m_OK;
////@end SelectDialog member variables
};

#endif
    // _SELECTDIALOG_H_
