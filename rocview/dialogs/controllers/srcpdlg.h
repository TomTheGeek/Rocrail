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

#ifndef _SRCPDLG_H_
#define _SRCPDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "srcpdlg.h"
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
#define ID_DIALOG_SRCP_SETTINGS 10021
#define ID_NOTEBOOK2 10044
#define ID_PANEL_SRCP_GENEREL 10018
#define ID_STATICTEXT_SRCP_IID 10020
#define ID_TEXTCTRL_SRCP_IID 10022
#define ID_STATICTEXT_SRCP_HOST 10023
#define ID_TEXTCTRL_SRCP_HOST 10024
#define wxID_STATIC_SRCP_CMDPORT 10030
#define ID_TEXTCTRL_SRCP_CmdPort 10035
#define wxID_STATIC_SRCP_SENSPORT 10031
#define ID_TEXTCTRL_SRCP_SENSPORT 10036
#define wxID_STATIC_SRCP_INFPORT 10032
#define ID_TEXTCTRL_SRCP_INFPORT 10037
#define ID_PANEL_SRCP_BUSSES 10045
#define SYMBOL_SRCPCTRLDLG_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxCLOSE_BOX
#define SYMBOL_SRCPCTRLDLG_TITLE _("SRCP")
#define SYMBOL_SRCPCTRLDLG_IDNAME ID_DIALOG_SRCP_SETTINGS
#define SYMBOL_SRCPCTRLDLG_SIZE wxSize(400, 300)
#define SYMBOL_SRCPCTRLDLG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif

/*!
 * SRCPCtrlDlg class declaration
 */

class SRCPCtrlDlg: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( SRCPCtrlDlg )
    DECLARE_EVENT_TABLE()
  void initLabels();
  void initValues();
  void evaluate();
  iONode m_Props;
  int m_TabAlign;

public:
    /// Constructors
    SRCPCtrlDlg( );
    SRCPCtrlDlg( wxWindow* parent, iONode props );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_SRCPCTRLDLG_IDNAME, const wxString& caption = SYMBOL_SRCPCTRLDLG_TITLE, const wxPoint& pos = SYMBOL_SRCPCTRLDLG_POSITION, const wxSize& size = SYMBOL_SRCPCTRLDLG_SIZE, long style = SYMBOL_SRCPCTRLDLG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin SRCPCtrlDlg event handler declarations

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
    void OnOkClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
    void OnCancelClick( wxCommandEvent& event );

////@end SRCPCtrlDlg event handler declarations

////@begin SRCPCtrlDlg member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end SRCPCtrlDlg member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin SRCPCtrlDlg member variables
    wxNotebook* m_Notebook;
    wxPanel* m_GenerelPanel;
    wxStaticText* m_labIID;
    wxTextCtrl* m_IID;
    wxStaticText* m_labHost;
    wxTextCtrl* m_Host;
    wxStaticText* m_labCmdPort;
    wxTextCtrl* m_CmdPort;
    wxStaticText* m_labSensPort;
    wxTextCtrl* m_SensPort;
    wxStaticText* m_labInfPort;
    wxTextCtrl* m_InfPort;
    wxCheckBox* m_UDP;
    wxPanel* m_BussesPanel;
    wxStaticText* m_labBusServer;
    wxSpinCtrl* m_BusServer;
    wxStaticText* m_labGLm;
    wxSpinCtrl* m_GLm;
    wxStaticText* m_labGLDCCs;
    wxSpinCtrl* m_GLDCCs;
    wxStaticText* m_labGLDCCl;
    wxSpinCtrl* m_GLDCCl;
    wxStaticText* m_labGLPS;
    wxSpinCtrl* m_GLPS;
    wxStaticText* m_labGAm;
    wxSpinCtrl* m_GAm;
    wxStaticText* m_labGAd;
    wxSpinCtrl* m_GAd;
    wxStaticText* m_labGAPS;
    wxSpinCtrl* m_GAPS;
    wxStaticText* m_labFBs88;
    wxSpinCtrl* m_FBs88;
    wxStaticText* m_labFBm6051;
    wxSpinCtrl* m_FBm6051;
    wxStaticText* m_labFBi8255;
    wxSpinCtrl* m_FBi8255;
    wxButton* m_OK;
    wxButton* m_Cancel;
////@end SRCPCtrlDlg member variables
};

#endif
    // _SRCPDLG_H_
