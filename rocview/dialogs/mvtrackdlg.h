/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */
#ifndef _MVTRACKDLG_H_
#define _MVTRACKDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "mvtrackdlg.h"
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
#define ID_MVTRACKDLG 10135
#define SYMBOL_MVTRACKDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX|wxTAB_TRAVERSAL
#define SYMBOL_MVTRACKDLG_TITLE _("MVTrackDlg")
#define SYMBOL_MVTRACKDLG_IDNAME ID_MVTRACKDLG
#define SYMBOL_MVTRACKDLG_SIZE wxSize(400, 300)
#define SYMBOL_MVTRACKDLG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * MVTrackDlg class declaration
 */

class MVTrackDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( MVTrackDlg )
    DECLARE_EVENT_TABLE()

    void initLabels();
    void initValues();
    void evaluate();
    void initSensorCombos();
    iONode m_Props;
    
public:
    /// Constructors
    MVTrackDlg();
    MVTrackDlg( wxWindow* parent, iONode p_Props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_MVTRACKDLG_IDNAME, const wxString& caption = SYMBOL_MVTRACKDLG_TITLE, const wxPoint& pos = SYMBOL_MVTRACKDLG_POSITION, const wxSize& size = SYMBOL_MVTRACKDLG_SIZE, long style = SYMBOL_MVTRACKDLG_STYLE );

    /// Destructor
    ~MVTrackDlg();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin MVTrackDlg event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end MVTrackDlg event handler declarations

////@begin MVTrackDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end MVTrackDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin MVTrackDlg member variables
    wxStaticText* m_labGauge;
    wxStaticText* m_labGaugePrefix;
    wxSpinCtrl* m_Gauge;
    wxStaticText* m_labDistance;
    wxTextCtrl* m_Distance;
    wxStaticText* m_labMM;
    wxCheckBox* m_MPH;
    wxStaticText* m_labSensor1;
    wxComboBox* m_Sensor1;
    wxStaticText* m_labSensor2;
    wxComboBox* m_Sensor2;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end MVTrackDlg member variables
};

#endif
    // _MVTRACKDLG_H_
