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
#ifndef _SELTABDLG_H_
#define _SELTABDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "seltabdlg.h"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/notebook.h"
#include "wx/spinctrl.h"
#include "wx/grid.h"
////@end includes

#include "rocs/public/node.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxNotebook;
class wxSpinCtrl;
class wxGrid;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_SENTAB_DIALOG 10126
#define ID_NOTEBOOK_TURNTABLE 10003
#define ID_FY_SHAREDFB 10248
#define ID_RADIOBOX_FY_ORI 10016
#define ID_SELTAB_TYPE 10138
#define ID_SELTAB_GRID 10008
#define ID_BUTTON_SELTAB_ADDTRACK 10106
#define ID_BUTTON_SELTAB_DELTRACK 10112
#define ID_BUTTON_SELTAB_MODIFYTRACK 10113
#define ID_PANEL_SELTAB_ROUTES 10080
#define ID_LISTBOX_SELTAB_ROUTES 10034
#define wxID_SELTAB_ROUTESENSOR_1 10362
#define wxID_SELTAB_ROUTEEVENT_1 10366
#define wxID_SELTAB_ROUTEGHOST_1 10370
#define wxID_SELTAB_ROUTESENSOR_2 10363
#define wxID_SELTAB_ROUTEEVENT_2 10367
#define wxID_SELTAB_ROUTEGHOST_2 10371
#define wxID_SELTAB_ROUTESENSOR_3 10364
#define wxID_SELTAB_ROUTEEVENT_3 10368
#define wxID_SELTAB_ROUTEGHOST_3 10372
#define wxID_SELTAB_ROUTESENSOR_4 10365
#define wxID_SELTAB_ROUTEEVENT_4 10369
#define wxID_SELTAB_ROUTEGHOST_4 10373
#define SYMBOL_SELTABDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_SELTABDIALOG_TITLE _("SelTabDialog")
#define SYMBOL_SELTABDIALOG_IDNAME ID_SENTAB_DIALOG
#define SYMBOL_SELTABDIALOG_SIZE wxDefaultSize
#define SYMBOL_SELTABDIALOG_POSITION wxDefaultPosition
////@end control identifiers


/*!
 * SelTabDialog class declaration
 */

class SelTabDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( SelTabDialog )
    DECLARE_EVENT_TABLE()
    
    void initLabels();
    void initValues();
    void evaluate();
    int m_TabAlign;
    iONode m_Props;
    int m_TrackNr;
    void initSensors();
    void initSensorCombos();
    void evaluateSensors();

public:
    /// Constructors
    SelTabDialog();
    SelTabDialog( wxWindow* parent, iONode p_Props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SELTABDIALOG_IDNAME, const wxString& caption = SYMBOL_SELTABDIALOG_TITLE, const wxPoint& pos = SYMBOL_SELTABDIALOG_POSITION, const wxSize& size = SYMBOL_SELTABDIALOG_SIZE, long style = SYMBOL_SELTABDIALOG_STYLE );

    /// Destructor
    ~SelTabDialog();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

////@begin SelTabDialog event handler declarations

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_FY_SHAREDFB
    void OnFySharedfbClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for ID_SELTAB_TYPE
    void OnSeltabTypeSelected( wxCommandEvent& event );

    /// wxEVT_GRID_CELL_LEFT_CLICK event handler for ID_SELTAB_GRID
    void OnCellLeftClick( wxGridEvent& event );

    /// wxEVT_GRID_CELL_RIGHT_CLICK event handler for ID_SELTAB_GRID
    void OnCellRightClick( wxGridEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SELTAB_ADDTRACK
    void OnButtonSeltabAddtrackClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SELTAB_DELTRACK
    void OnButtonSeltabDeltrackClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_SELTAB_MODIFYTRACK
    void OnButtonSeltabModifytrackClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_SELTAB_ROUTES
    void OnListboxSeltabRoutesSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_COMBOBOX_SELECTED event handler for wxID_SELTAB_ROUTESENSOR_1
    void OnBlockSensor( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end SelTabDialog event handler declarations

////@begin SelTabDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end SelTabDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin SelTabDialog member variables
    wxNotebook* m_Notebook;
    wxPanel* m_GeneralPanel;
    wxStaticText* m_LabelID;
    wxTextCtrl* m_ID;
    wxStaticText* m_LabelDesc;
    wxTextCtrl* m_Desc;
    wxStaticText* m_labNrTracks;
    wxSpinCtrl* m_NrTracks;
    wxCheckBox* m_SharedFB;
    wxCheckBox* m_Manager;
    wxStaticText* m_labMinOcc;
    wxSpinCtrl* m_MinOcc;
    wxStaticText* m_labMoveDelay;
    wxSpinCtrl* m_MoveDelay;
    wxPanel* m_LocationPanel;
    wxStaticText* m_LabelX;
    wxTextCtrl* m_x;
    wxStaticText* m_LabelY;
    wxTextCtrl* m_y;
    wxStaticText* m_LabelZ;
    wxTextCtrl* m_z;
    wxRadioBox* m_ori;
    wxPanel* m_Interface;
    wxStaticText* m_Labeliid;
    wxTextCtrl* m_IID;
    wxStaticText* m_LabelType;
    wxComboBox* m_Type;
    wxCheckBox* m_Invert;
    wxStaticText* m_LabelAddress0;
    wxTextCtrl* m_Address0;
    wxStaticText* m_labPort0;
    wxTextCtrl* m_Port0;
    wxStaticText* m_LabelAddress1;
    wxTextCtrl* m_Address1;
    wxStaticText* m_labPort1;
    wxTextCtrl* m_Port1;
    wxStaticText* m_LabelAddress2;
    wxTextCtrl* m_Address2;
    wxStaticText* m_labPort2;
    wxTextCtrl* m_Port2;
    wxStaticText* m_LabelAddress3;
    wxTextCtrl* m_Address3;
    wxStaticText* m_labPort3;
    wxTextCtrl* m_Port3;
    wxStaticText* m_labPosSen;
    wxComboBox* m_PosSen;
    wxStaticText* m_labb0Sen;
    wxComboBox* m_b0Sen;
    wxStaticText* m_labb1Sen;
    wxComboBox* m_b1Sen;
    wxStaticText* m_labb2Sen;
    wxComboBox* m_b2Sen;
    wxStaticText* m_labb3Sen;
    wxComboBox* m_b3Sen;
    wxStaticBox* m_NewPosBox;
    wxStaticText* m_LabelAddress4;
    wxTextCtrl* m_Address4;
    wxStaticText* m_labPort4;
    wxTextCtrl* m_Port4;
    wxCheckBox* m_InvertNew;
    wxPanel* m_TracksPanel;
    wxGrid* m_TracksGrid;
    wxButton* m_AddTrack;
    wxButton* m_DelTrack;
    wxButton* m_ModifyTrack;
    wxPanel* m_RoutesPanel;
    wxListBox* m_Routes;
    wxStaticText* m_LabelSensorsFromBlock;
    wxStaticText* m_LabelSensorIDs;
    wxStaticText* m_LabelSensorActions;
    wxStaticText* m_LabelGhost;
    wxComboBox* m_SensorID1;
    wxComboBox* m_SensorAction1;
    wxCheckBox* m_Ghost1;
    wxComboBox* m_SensorID2;
    wxComboBox* m_SensorAction2;
    wxCheckBox* m_Ghost2;
    wxComboBox* m_SensorID3;
    wxComboBox* m_SensorAction3;
    wxCheckBox* m_Ghost3;
    wxComboBox* m_SensorID4;
    wxComboBox* m_SensorAction4;
    wxCheckBox* m_Ghost4;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end SelTabDialog member variables
    iONode m_fbEvents[4];
    const char* m_FromBlockID;
};

#endif
    // _SELTABDLG_H_
