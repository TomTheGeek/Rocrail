/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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

#ifndef _DDXDLG_H_
#define _DDXDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "ddldlg.h"
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
class wxFlexGridSizer;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG_DDL_SETTINGS 10086
#define ID_PANEL_DDL_GEN 10000
#define ID_STATICTEXT_DDL_IID 10001
#define ID_TEXTCTRL_DDL_IID 10002
#define ID_STATICTEXT_DDL_DEVICE 10003
#define ID_PANEL_DDL_S88 10046
#define ID_STATICTEXT_DDL_PORT 10005
#define ID_STATICTEXT_DDL_BUSSES 10007
#define ID_STATICTEXT_DDL_BUS0 10009
#define ID_STATICTEXT_DDL_BUS1 10011
#define ID_STATICTEXT_DDL_BUS2 10013
#define ID_STATICTEXT_DDL_BUS3 10015
#define ID_PANEL_DDL_DETAIL 10047
#define ID_CHECKBOX_DDL_ENABLE_SCD 10017
#define ID_CHECKBOX_DDL_INVERSE 10018
#define ID_STATICTEXT_DDL_DELAY 10019
#define ID_TEXTCTRL_DDL_DELAY 10020
#define SYMBOL_DDXCTRLDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_DDXCTRLDLG_TITLE _("Dialog")
#define SYMBOL_DDXCTRLDLG_IDNAME ID_DIALOG_DDL_SETTINGS
#define SYMBOL_DDXCTRLDLG_SIZE wxSize(400, 300)
#define SYMBOL_DDXCTRLDLG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * DDLCtrlDlg class declaration
 */

class DDXCtrlDlg: public wxDialog
{
    DECLARE_DYNAMIC_CLASS( DDXCtrlDlg )
    DECLARE_EVENT_TABLE()

  void initLabels();
  void initValues();
  void evaluate();
  iONode m_Props;
  const char* m_Devices;
  iONode m_SubProps;
  int m_TabAlign;

public:
    /// Constructors
    DDXCtrlDlg( );
    DDXCtrlDlg( wxWindow* parent, iONode props, const char* devices=NULL );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_DDXCTRLDLG_IDNAME, const wxString& caption = SYMBOL_DDXCTRLDLG_TITLE, const wxPoint& pos = SYMBOL_DDXCTRLDLG_POSITION, const wxSize& size = SYMBOL_DDXCTRLDLG_SIZE, long style = SYMBOL_DDXCTRLDLG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin DDXCtrlDlg event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end DDXCtrlDlg event handler declarations

////@begin DDXCtrlDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end DDXCtrlDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin DDXCtrlDlg member variables
    wxNotebook* m_Notebook;
    wxPanel* m_GenerelPanel;
    wxStaticText* m_labIID;
    wxTextCtrl* m_IID;
    wxStaticText* m_labDevice;
    wxComboBox* m_Device;
    wxStaticText* m_labPortBase;
    wxTextCtrl* m_PortBase;
    wxPanel* m_S88Panel;
    wxStaticText* m_labPort;
    wxTextCtrl* m_Port;
    wxStaticText* m_labBusses;
    wxSpinCtrl* m_Busses;
    wxStaticText* m_labBus0;
    wxSpinCtrl* m_Bus0;
    wxStaticText* m_labBus1;
    wxSpinCtrl* m_Bus1;
    wxStaticText* m_labBus2;
    wxSpinCtrl* m_Bus2;
    wxStaticText* m_labBus3;
    wxSpinCtrl* m_Bus3;
    wxPanel* m_DetailsPanel;
    wxStaticBoxSizer* m_SCDBox;
    wxFlexGridSizer* m_SCDbox;
    wxCheckBox* m_SCD;
    wxCheckBox* m_InvDSR;
    wxStaticText* m_labDelay;
    wxTextCtrl* m_Delay;
    wxStaticBox* m_LocoRefreshBox;
    wxCheckBox* m_QueueCheck;
    wxCheckBox* m_FastCVGet;
    wxStaticBox* m_GenerateBox;
    wxCheckBox* m_GenDCC;
    wxCheckBox* m_GenMM;
    wxCheckBox* m_GenMMA;
    wxCheckBox* m_MMLongPause;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end DDXCtrlDlg member variables
};

#endif
    // _DDXDLG_H_
