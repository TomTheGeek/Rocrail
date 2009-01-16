/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifndef _ROCRAILINIDIALOG_H_
#define _ROCRAILINIDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "rocrailinidialog.cpp"
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
#define ID_DIALOG_RR_INI 10090
#define ID_NOTEBOOK_RR 10091
#define ID_PANEL_RR_INI 10092
#define wxID_STATIC_RR_PLANFILE 10093
#define ID_TEXTCTRL_RR_PLANFILE 10094
#define wxID_STATIC_RR_LIBPATH 10095
#define ID_TEXTCTRL_RR_LIBPATH 10096
#define wxID_STATIC_RR_TRACEFILE 10097
#define ID_TEXTCTRL_RR_TRACEFILE 10101
#define ID_PANEL_RR_TRACE 10102
#define ID_CHECKBOX_RR_AUTO 10006
#define ID_CHECKBOX_RR_MONITOR 10232
#define ID_CHECKBOX_RR_BYTE 10007
#define ID_CHECKBOX_RR_DEBUG 10005
#define ID_PANEL_RR_SERVICE 10107
#define wxID_STATIC_RR_HTTPPORT 10003
#define ID_TEXTCTRL_RR_HTTPPORT 10004
#define wxID_STATIC_RR_SEVICE_REFRESH 10319
#define ID_TEXTCTRL_RR_SERVICE_REFRESH 10320
#define wxID_STATIC_RR_WEBSERVICE_PORT 10321
#define ID_TEXTCTRL_RR_WEBSERVICE_PORT 10322
#define wxID_STATIC_RR_WEBSERVICE_REFRESH 10323
#define ID_TEXTCTRL_RR_WEBSERVICE_REFRESH 10324
#define wxID_STATIC_RR_CLOCK_DEVIDER 10103
#define ID_TEXTCTRL_RR_CLOCK_DEVIDER 10104
#define wxID_STATIC_RR_CLOCK_HOUR 10155
#define ID_TEXTCTRL_RR_CLOCK_HOUR 10157
#define wxID_STATIC_RR_CLOCK_MINUTE 10158
#define ID_TEXTCTRL_RR_CLOCK_MINUTE 10159
#define wxID_STATIC_RR_CLIENTPORT 10108
#define ID_TEXTCTRL_RR_CLIENTPORT 10109
#define wxID_STATIC_RR_MAXCON 10110
#define ID_TEXTCTRL_RR_MAXCONN 10111
#define ID_PANEL_RR_AUTO 10146
#define wxID_STATIC_RR_SWTIME 10147
#define ID_TEXTCTRL_RR_SWTIME 10148
#define wxID_STATIC__MINBKLEN 10149
#define ID_TEXTCTRL_RR_MINBKLEN 10150
#define wxID_STATIC_RR_IGNEVT 10151
#define ID_TEXTCTRL_RR_IGNEVT 10152
#define wxID_STATIC_RR_INITPAUSE 10311
#define ID_TEXTCTRL_RR_PAUSEINIT 10298
#define ID_PANEL_RR_CONTROLLERS 10153
#define ID_LISTBOX_RR_CONTROLLERS 10154
#define ID_BUTTON_RR_DELETE 10105
#define ID_BUTTON_RR_PROPS 10156
#define ID_COMBOBOX_RR_LIB 10001
#define ID_BUTTON_RR_ADD 10000
#define SYMBOL_ROCRAILINIDIALOG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_ROCRAILINIDIALOG_TITLE _("Rocrail Properties")
#define SYMBOL_ROCRAILINIDIALOG_IDNAME ID_DIALOG_RR_INI
#define SYMBOL_ROCRAILINIDIALOG_SIZE wxDefaultSize
#define SYMBOL_ROCRAILINIDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * RocrailIniDialog class declaration
 */

class RocrailIniDialog: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( RocrailIniDialog )
    DECLARE_EVENT_TABLE()

    void initLabels();
    void initValues();
    void initControllerList();
    void evaluate();
    int m_TabAlign;
    wxDialog* m_CSDialog;

public:
    /// Constructors
    RocrailIniDialog( );
    RocrailIniDialog( wxWindow* parent, iONode props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_ROCRAILINIDIALOG_IDNAME, const wxString& caption = SYMBOL_ROCRAILINIDIALOG_TITLE, const wxPoint& pos = SYMBOL_ROCRAILINIDIALOG_POSITION, const wxSize& size = SYMBOL_ROCRAILINIDIALOG_SIZE, long style = SYMBOL_ROCRAILINIDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin RocrailIniDialog event handler declarations

    /// wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED event handler for ID_NOTEBOOK_RR
    void OnNotebookRrPageChanged( wxNotebookEvent& event );

    /// wxEVT_COMMAND_LISTBOX_SELECTED event handler for ID_LISTBOX_RR_CONTROLLERS
    void OnListboxRrControllersSelected( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_RR_DELETE
    void OnButtonRrDeleteClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_RR_PROPS
    void OnButtonRrPropsClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON_RR_ADD
    void OnButtonRrAddClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_APPLY
    void OnApplyClick( wxCommandEvent& event );

////@end RocrailIniDialog event handler declarations

////@begin RocrailIniDialog member function declarations

    iONode GetProps() const { return m_Props ; }
    void SetProps(iONode value) { m_Props = value ; }

    iONode GetController() const { return m_Controller ; }
    void SetController(iONode value) { m_Controller = value ; }

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end RocrailIniDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin RocrailIniDialog member variables
    wxNotebook* m_RRNotebook;
    wxPanel* m_GeneralPanel;
    wxStaticText* m_LabelPlanfile;
    wxTextCtrl* m_PlanFile;
    wxStaticText* m_labPlanType;
    wxRadioBox* m_PlanType;
    wxStaticText* m_LabelLibPath;
    wxTextCtrl* m_LibPath;
    wxStaticText* m_labImgPath;
    wxTextCtrl* m_ImgPath;
    wxStaticText* m_LabelTraceFile;
    wxTextCtrl* m_TraceFile;
    wxStaticText* m_labPTIID;
    wxTextCtrl* m_PTIID;
    wxStaticText* m_labLCIID;
    wxTextCtrl* m_LCIID;
    wxStaticText* m_labDPIID;
    wxTextCtrl* m_DPIID;
    wxStaticText* m_labSVIID;
    wxTextCtrl* m_SVIID;
    wxPanel* m_TracePanel;
    wxStaticBoxSizer* m_TraceLevel;
    wxCheckBox* m_Auto;
    wxCheckBox* m_Monitor;
    wxStaticBox* m_DevTraceLevel;
    wxCheckBox* m_Byte;
    wxCheckBox* m_Debug;
    wxPanel* m_ServicePanel;
    wxStaticBoxSizer* m_HttpBox;
    wxStaticText* m_LabelHttpPort;
    wxTextCtrl* m_HttpPort;
    wxStaticText* m_LabServiceRefresh;
    wxTextCtrl* m_ServiceRefresh;
    wxStaticBoxSizer* m_WebServiceBox;
    wxStaticText* m_LabWebServicePort;
    wxTextCtrl* m_WebServicePort;
    wxStaticText* m_LabWebServiceRefresh;
    wxTextCtrl* m_WebServiceRefresh;
    wxStaticBoxSizer* m_ClockBox;
    wxStaticText* m_labDevider;
    wxTextCtrl* m_Divider;
    wxStaticText* m_labHour;
    wxTextCtrl* m_Hour;
    wxStaticText* m_labMinute;
    wxTextCtrl* m_Minute;
    wxStaticBoxSizer* m_ClientBox;
    wxStaticText* m_LabelClientPort;
    wxTextCtrl* m_ClientPort;
    wxStaticText* m_LabelMaxCon;
    wxTextCtrl* m_MaxCon;
    wxCheckBox* m_OnlyFirstMaster;
    wxPanel* m_AtomatPanel;
    wxStaticText* m_LabelSwTime;
    wxTextCtrl* m_SwTime;
    wxStaticText* m_LabelMinBKLen;
    wxTextCtrl* m_MinBKLen;
    wxStaticText* m_LabelIgnEvt;
    wxTextCtrl* m_IgnEvt;
    wxStaticText* m_labInitPause;
    wxTextCtrl* m_PauseInit;
    wxStaticText* m_labInitPower;
    wxCheckBox* m_InitfieldPower;
    wxStaticText* m_labSeed;
    wxSpinCtrl* m_Seed;
    wxStaticText* m_labSavePosTime;
    wxSpinCtrl* m_SavePosTime;
    wxStaticText* m_labEventTimeout;
    wxTextCtrl* m_EventTimeout;
    wxStaticText* m_labSignalReset;
    wxSpinCtrl* m_SignalReset;
    wxStaticText* m_labRouteSwTime;
    wxSpinCtrl* m_RouteSwTime;
    wxStaticText* m_labSemaphoreWait;
    wxSpinCtrl* m_SemaphoreWait;
    wxCheckBox* m_Check2In;
    wxCheckBox* m_secondNextBlock;
    wxCheckBox* m_EnableSwFb;
    wxCheckBox* m_DisableRouteVreduce;
    wxCheckBox* m_GreenAspect;
    wxCheckBox* m_SkipSetSw;
    wxRadioBox* m_DefAspect;
    wxPanel* m_ControllersPanel;
    wxListBox* m_Controllers;
    wxButton* m_ControllerDelete;
    wxButton* m_ControllerProps;
    wxStaticBoxSizer* m_AddControllerBox;
    wxComboBox* m_Lib;
    wxButton* m_ControllerAdd;
    wxButton* m_OK;
    wxButton* m_Cancel;
    wxButton* m_Apply;
    iONode m_Props;
    iONode m_Controller;
////@end RocrailIniDialog member variables
    void event( iONode event );

};

#endif
    // _ROCRAILINIDIALOG_H_
