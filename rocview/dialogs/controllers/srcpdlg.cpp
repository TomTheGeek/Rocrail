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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "srcpdlg.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
#include "wx/imaglist.h"
////@end includes

#include "srcpdlg.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SRCP.h"
#include "rocview/public/guiapp.h"

////@begin XPM images
////@end XPM images

/*!
 * SRCPCtrlDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( SRCPCtrlDlg, wxDialog )

/*!
 * SRCPCtrlDlg event table definition
 */

BEGIN_EVENT_TABLE( SRCPCtrlDlg, wxDialog )

////@begin SRCPCtrlDlg event table entries
    EVT_BUTTON( wxID_OK, SRCPCtrlDlg::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, SRCPCtrlDlg::OnCancelClick )

////@end SRCPCtrlDlg event table entries

END_EVENT_TABLE()

/*!
 * SRCPCtrlDlg constructors
 */

SRCPCtrlDlg::SRCPCtrlDlg( )
{
}

SRCPCtrlDlg::SRCPCtrlDlg( wxWindow* parent, iONode props )
{
  m_TabAlign = wxGetApp().getTabAlign();
  Create(parent, -1, wxT("SRCP"));
  m_Props = props;
  initLabels();
  initValues();

  m_GenerelPanel->GetSizer()->Layout();
  m_BussesPanel->GetSizer()->Layout();
  
  m_Notebook->Fit();
  
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  GetSizer()->Layout();
}


void SRCPCtrlDlg::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "busses" ) );
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labHost->SetLabel( wxGetApp().getMsg( "host" ) );
}

void SRCPCtrlDlg::initValues() {
  if( m_Props == NULL )
    return;
    
  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );
  m_Host->SetValue( wxString( wDigInt.gethost( m_Props ), wxConvUTF8 ) );

  iONode srcpini = wDigInt.getsrcp(m_Props);
  if( srcpini == NULL ) {
    srcpini = NodeOp.inst( wSRCP.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild(m_Props, srcpini );
  }

  char* val = StrOp.fmt( "%d", wSRCP.getcmdport( srcpini ) );
  m_CmdPort->SetValue( wxString( val, wxConvUTF8 ) );
  StrOp.free( val );
  val = StrOp.fmt( "%d", wSRCP.getfbackport( srcpini ) );
  m_SensPort->SetValue( wxString( val, wxConvUTF8 ) );
  StrOp.free( val );
  val = StrOp.fmt( "%d", wSRCP.getinfoport( srcpini ) );
  m_InfPort->SetValue( wxString( val, wxConvUTF8 ) );
  StrOp.free( val );
  
  // Busses
  m_BusServer->SetValue(wSRCP.getsrcpbus_server( srcpini ));
  m_GLm->SetValue(wSRCP.getsrcpbusGL_m( srcpini ));
  m_GLDCCs->SetValue(wSRCP.getsrcpbusGL_ns( srcpini ));
  m_GLDCCl->SetValue(wSRCP.getsrcpbusGL_nl( srcpini ));
  m_GLPS->SetValue(wSRCP.getsrcpbusGL_ps( srcpini ));
  m_GAm->SetValue(wSRCP.getsrcpbusGA_m( srcpini ));
  m_GAd->SetValue(wSRCP.getsrcpbusGA_n( srcpini ));
  m_GAPS->SetValue(wSRCP.getsrcpbusGA_ps( srcpini ));
  m_FBs88->SetValue(wSRCP.getsrcpbusFB_s88( srcpini ));
  m_FBm6051->SetValue(wSRCP.getsrcpbusFB_m6051( srcpini ));
  m_FBi8255->SetValue(wSRCP.getsrcpbusFB_i8255( srcpini ));
}


void SRCPCtrlDlg::evaluate() {
  if( m_Props == NULL )
    return;
  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );
  wDigInt.sethost( m_Props, m_Host->GetValue().mb_str(wxConvUTF8) );

  iONode srcpini = wDigInt.getsrcp(m_Props);
  if( srcpini == NULL ) {
    srcpini = NodeOp.inst( wSRCP.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild(m_Props, srcpini );
  }

  wSRCP.setcmdport( srcpini, atoi( m_CmdPort->GetValue().mb_str(wxConvUTF8) ) );
  wSRCP.setfbackport( srcpini, atoi( m_SensPort->GetValue().mb_str(wxConvUTF8) ) );
  wSRCP.setinfoport( srcpini, atoi( m_InfPort->GetValue().mb_str(wxConvUTF8) ) );

  // Busses
  wSRCP.setsrcpbus_server( srcpini, m_BusServer->GetValue( ));
  wSRCP.setsrcpbusGL_m( srcpini, m_GLm->GetValue( ));
  wSRCP.setsrcpbusGL_ns( srcpini, m_GLDCCs->GetValue( ));
  wSRCP.setsrcpbusGL_nl( srcpini, m_GLDCCl->GetValue( ));
  wSRCP.setsrcpbusGL_ps( srcpini, m_GLPS->GetValue( ));
  wSRCP.setsrcpbusGA_m( srcpini, m_GAm->GetValue( ));
  wSRCP.setsrcpbusGA_n( srcpini, m_GAd->GetValue( ));
  wSRCP.setsrcpbusGA_ps( srcpini, m_GAPS->GetValue( ));
  wSRCP.setsrcpbusFB_s88( srcpini, m_FBs88->GetValue( ));
  wSRCP.setsrcpbusFB_m6051( srcpini, m_FBm6051->GetValue( ));
  wSRCP.setsrcpbusFB_i8255( srcpini, m_FBi8255->GetValue( ));
}


  

/*!
 * SRCPCtrlDlg creator
 */

bool SRCPCtrlDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin SRCPCtrlDlg member initialisation
    m_Notebook = NULL;
    m_GenerelPanel = NULL;
    m_labIID = NULL;
    m_IID = NULL;
    m_labHost = NULL;
    m_Host = NULL;
    m_labCmdPort = NULL;
    m_CmdPort = NULL;
    m_labSensPort = NULL;
    m_SensPort = NULL;
    m_labInfPort = NULL;
    m_InfPort = NULL;
    m_UDP = NULL;
    m_BussesPanel = NULL;
    m_labBusServer = NULL;
    m_BusServer = NULL;
    m_labGLm = NULL;
    m_GLm = NULL;
    m_labGLDCCs = NULL;
    m_GLDCCs = NULL;
    m_labGLDCCl = NULL;
    m_GLDCCl = NULL;
    m_labGLPS = NULL;
    m_GLPS = NULL;
    m_labGAm = NULL;
    m_GAm = NULL;
    m_labGAd = NULL;
    m_GAd = NULL;
    m_labGAPS = NULL;
    m_GAPS = NULL;
    m_labFBs88 = NULL;
    m_FBs88 = NULL;
    m_labFBm6051 = NULL;
    m_FBm6051 = NULL;
    m_labFBi8255 = NULL;
    m_FBi8255 = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end SRCPCtrlDlg member initialisation

////@begin SRCPCtrlDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end SRCPCtrlDlg creation
    return true;
}

/*!
 * Control creation for SRCPCtrlDlg
 */

void SRCPCtrlDlg::CreateControls()
{    
////@begin SRCPCtrlDlg content construction
    SRCPCtrlDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, ID_NOTEBOOK2, wxDefaultPosition, wxDefaultSize, wxBK_DEFAULT );

    m_GenerelPanel = new wxPanel( m_Notebook, ID_PANEL_SRCP_GENEREL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_GenerelPanel->SetSizer(itemBoxSizer5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer6->AddGrowableCol(1);
    itemBoxSizer5->Add(itemFlexGridSizer6, 0, wxGROW|wxALL, 5);
    m_labIID = new wxStaticText( m_GenerelPanel, ID_STATICTEXT_SRCP_IID, _("IID"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_IID = new wxTextCtrl( m_GenerelPanel, ID_TEXTCTRL_SRCP_IID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_IID, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labHost = new wxStaticText( m_GenerelPanel, ID_STATICTEXT_SRCP_HOST, _("Host"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labHost, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Host = new wxTextCtrl( m_GenerelPanel, ID_TEXTCTRL_SRCP_HOST, wxEmptyString, wxDefaultPosition, wxSize(120, -1), 0 );
    itemFlexGridSizer6->Add(m_Host, 1, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labCmdPort = new wxStaticText( m_GenerelPanel, wxID_STATIC_SRCP_CMDPORT, _("command port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labCmdPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_CmdPort = new wxTextCtrl( m_GenerelPanel, ID_TEXTCTRL_SRCP_CmdPort, _("12345"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_CmdPort->SetMaxLength(5);
    itemFlexGridSizer6->Add(m_CmdPort, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labSensPort = new wxStaticText( m_GenerelPanel, wxID_STATIC_SRCP_SENSPORT, _("sensor port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labSensPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SensPort = new wxTextCtrl( m_GenerelPanel, ID_TEXTCTRL_SRCP_SENSPORT, _("12346"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_SensPort->SetMaxLength(5);
    itemFlexGridSizer6->Add(m_SensPort, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labInfPort = new wxStaticText( m_GenerelPanel, wxID_STATIC_SRCP_INFPORT, _("info port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labInfPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_InfPort = new wxTextCtrl( m_GenerelPanel, ID_TEXTCTRL_SRCP_INFPORT, _("12347"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_InfPort->SetMaxLength(5);
    itemFlexGridSizer6->Add(m_InfPort, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_UDP = new wxCheckBox( m_GenerelPanel, wxID_ANY, _("UDP"), wxDefaultPosition, wxDefaultSize, 0 );
    m_UDP->SetValue(false);
    itemBoxSizer5->Add(m_UDP, 0, wxALIGN_LEFT|wxALL, 5);

    m_Notebook->AddPage(m_GenerelPanel, _("Generel"));

    m_BussesPanel = new wxPanel( m_Notebook, ID_PANEL_SRCP_BUSSES, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxFlexGridSizer* itemFlexGridSizer19 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer19->AddGrowableCol(1);
    m_BussesPanel->SetSizer(itemFlexGridSizer19);

    m_labBusServer = new wxStaticText( m_BussesPanel, wxID_ANY, _("server"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labBusServer, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_BusServer = new wxSpinCtrl( m_BussesPanel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 10, 0 );
    itemFlexGridSizer19->Add(m_BusServer, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

    m_labGLm = new wxStaticText( m_BussesPanel, wxID_ANY, _("GL Motorola"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labGLm, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_GLm = new wxSpinCtrl( m_BussesPanel, wxID_ANY, _T("1"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 10, 1 );
    itemFlexGridSizer19->Add(m_GLm, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labGLDCCs = new wxStaticText( m_BussesPanel, wxID_ANY, _("GL DCC short"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labGLDCCs, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_GLDCCs = new wxSpinCtrl( m_BussesPanel, wxID_ANY, _T("2"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 10, 2 );
    itemFlexGridSizer19->Add(m_GLDCCs, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labGLDCCl = new wxStaticText( m_BussesPanel, wxID_ANY, _("GL DCC long"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labGLDCCl, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_GLDCCl = new wxSpinCtrl( m_BussesPanel, wxID_ANY, _T("3"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 10, 3 );
    itemFlexGridSizer19->Add(m_GLDCCl, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labGLPS = new wxStaticText( m_BussesPanel, wxID_ANY, _("GL Protocol by server"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labGLPS, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_GLPS = new wxSpinCtrl( m_BussesPanel, wxID_ANY, _T("4"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 10, 4 );
    itemFlexGridSizer19->Add(m_GLPS, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labGAm = new wxStaticText( m_BussesPanel, wxID_ANY, _("GA Motorola"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labGAm, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_GAm = new wxSpinCtrl( m_BussesPanel, wxID_ANY, _T("5"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 10, 5 );
    itemFlexGridSizer19->Add(m_GAm, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labGAd = new wxStaticText( m_BussesPanel, wxID_ANY, _("GA DCC"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labGAd, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_GAd = new wxSpinCtrl( m_BussesPanel, wxID_ANY, _T("6"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 10, 6 );
    itemFlexGridSizer19->Add(m_GAd, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labGAPS = new wxStaticText( m_BussesPanel, wxID_ANY, _("GA Protocol by server"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labGAPS, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_GAPS = new wxSpinCtrl( m_BussesPanel, wxID_ANY, _T("7"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 10, 7 );
    itemFlexGridSizer19->Add(m_GAPS, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labFBs88 = new wxStaticText( m_BussesPanel, wxID_ANY, _("FB s88"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labFBs88, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_FBs88 = new wxSpinCtrl( m_BussesPanel, wxID_ANY, _T("8"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 10, 8 );
    itemFlexGridSizer19->Add(m_FBs88, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labFBm6051 = new wxStaticText( m_BussesPanel, wxID_ANY, _("FB m6051"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labFBm6051, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_FBm6051 = new wxSpinCtrl( m_BussesPanel, wxID_ANY, _T("9"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 10, 9 );
    itemFlexGridSizer19->Add(m_FBm6051, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_labFBi8255 = new wxStaticText( m_BussesPanel, wxID_ANY, _("FB i8255"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer19->Add(m_labFBi8255, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_FBi8255 = new wxSpinCtrl( m_BussesPanel, wxID_ANY, _T("10"), wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 10, 10 );
    itemFlexGridSizer19->Add(m_FBi8255, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_Notebook->AddPage(m_BussesPanel, _("Busses"));

    itemBoxSizer2->Add(m_Notebook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer42 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer42, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer42->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer42->AddButton(m_Cancel);

    itemStdDialogButtonSizer42->Realize();

////@end SRCPCtrlDlg content construction
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void SRCPCtrlDlg::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void SRCPCtrlDlg::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}

/*!
 * Should we show tooltips?
 */

bool SRCPCtrlDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap SRCPCtrlDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin SRCPCtrlDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end SRCPCtrlDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon SRCPCtrlDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin SRCPCtrlDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end SRCPCtrlDlg icon retrieval
}
