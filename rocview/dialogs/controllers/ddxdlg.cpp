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
#pragma implementation "ddldlg.h"
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

#include "ddxdlg.h"

#include "rocview/public/guiapp.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/DDX.h"
#include "rocs/public/strtok.h"

////@begin XPM images
////@end XPM images

/*!
 * DDXCtrlDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( DDXCtrlDlg, wxDialog )

/*!
 * DDXCtrlDlg event table definition
 */

BEGIN_EVENT_TABLE( DDXCtrlDlg, wxDialog )

////@begin DDXCtrlDlg event table entries
    EVT_BUTTON( wxID_OK, DDXCtrlDlg::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, DDXCtrlDlg::OnCancelClick )

////@end DDXCtrlDlg event table entries

END_EVENT_TABLE()

/*!
 * DDXCtrlDlg constructors
 */

DDXCtrlDlg::DDXCtrlDlg( )
{
}

DDXCtrlDlg::DDXCtrlDlg( wxWindow* parent, iONode props, const char* devices )
{
  m_TabAlign = wxGetApp().getTabAlign();
  m_Props = props;
  m_Devices = devices;

  // check if subnode ddl exist:
  m_SubProps = wDigInt.getddx( m_Props );

  if( m_SubProps == NULL ) {
    m_SubProps = NodeOp.inst( wDDX.name(), m_Props, ELEMENT_NODE );
    NodeOp.addChild( m_Props, m_SubProps );
  }

  Create(parent, -1, wxGetApp().getMsg( NodeOp.getName(m_SubProps) ));

  initLabels();
  initValues();

  m_GenerelPanel->GetSizer()->Layout();
  m_S88Panel->GetSizer()->Layout();
  m_DetailsPanel->GetSizer()->Layout();

  m_Notebook->Fit();

  GetSizer()->Layout();
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  Fit();
}

void DDXCtrlDlg::initLabels() {
  m_Notebook->SetPageText( 0, wxGetApp().getMsg( "general" ) );
  m_Notebook->SetPageText( 1, wxGetApp().getMsg( "s88" ) );
  m_Notebook->SetPageText( 2, wxGetApp().getMsg( "details" ) );
  m_labIID->SetLabel( wxGetApp().getMsg( "iid" ) );
  m_labDevice->SetLabel( wxGetApp().getMsg( "port" ) );
  m_labPortBase->SetLabel( wxGetApp().getMsg( "portbase" ) );
  m_SCDBox->GetStaticBox()->SetLabel( wxGetApp().getMsg( "shortcutchecking" ) );
  m_SCD->SetLabel( wxGetApp().getMsg( "enable" ) );
  m_InvDSR->SetLabel( wxGetApp().getMsg( "inversedsr" ) );
  m_labDelay->SetLabel( wxGetApp().getMsg( "delay" ) );
  m_LocoRefreshBox->SetLabel( wxGetApp().getMsg( "options" ) );
  m_QueueCheck->SetLabel( wxGetApp().getMsg( "queuecheck" ) );

  m_labBusses->SetLabel( wxGetApp().getMsg( "chains" ) );
  m_labBus0->SetLabel( wxGetApp().getMsg( "chain" ) + _T(" 0") );
  m_labBus1->SetLabel( wxGetApp().getMsg( "chain" ) + _T(" 1") );
  m_labBus2->SetLabel( wxGetApp().getMsg( "chain" ) + _T(" 2") );
  m_labBus3->SetLabel( wxGetApp().getMsg( "chain" ) + _T(" 3") );

}

void DDXCtrlDlg::initValues() {
  if( m_SubProps == NULL )
    return;

  m_IID->SetValue( wxString( wDigInt.getiid( m_Props ), wxConvUTF8 ) );

  m_Device->SetValue( wxString( wDDX.getport( m_SubProps ), wxConvUTF8 ) );
  if( m_Devices != NULL ) {
    iOStrTok tok = StrTokOp.inst(m_Devices, ',');
    while( StrTokOp.hasMoreTokens(tok) ) {
      m_Device->Append( wxString( StrTokOp.nextToken(tok), wxConvUTF8 ) );
    }
    StrTokOp.base.del(tok);
  }

  m_PortBase->SetValue( wxString( wDDX.getportbase( m_SubProps ), wxConvUTF8 ) );

  m_Port->SetValue( wxString( wDDX.gets88port( m_SubProps ), wxConvUTF8 ) );

  m_Busses->SetValue( wDDX.gets88busses( m_SubProps ) );
  m_Bus0->SetValue( wDDX.gets88b0modcnt( m_SubProps ) );
  m_Bus1->SetValue( wDDX.gets88b1modcnt( m_SubProps ) );
  m_Bus2->SetValue( wDDX.gets88b2modcnt( m_SubProps ) );
  m_Bus3->SetValue( wDDX.gets88b3modcnt( m_SubProps ) );

  m_InvDSR->SetValue( wDDX.isinversedsr( m_SubProps ) ? true:false );

  char* val = StrOp.fmt( "%d", wDDX.getshortcutdelay( m_SubProps ) );
  m_Delay->SetValue( wxString( val, wxConvUTF8 ) );
  StrOp.free(val);


  m_SCD->SetValue( wDDX.isshortcutchecking( m_SubProps ) ? true:false );


  m_GenDCC->SetValue( wDDX.isdcc( m_SubProps ) ? true:false );
  m_GenMM->SetValue( wDDX.ismotorolarefresh( m_SubProps) ? true:false );
  m_GenMMA->SetValue( wDDX.ismotorola( m_SubProps) ? true:false );
  m_QueueCheck->SetValue( wDDX.isqueuecheck( m_SubProps) ? true:false );
  m_FastCVGet->SetValue( wDDX.isfastcvget( m_SubProps)?true:false );
  m_MMLongPause->SetValue( wDDX.ismmlongpause( m_SubProps) ? true:false );

}


void DDXCtrlDlg::evaluate() {
  if( m_SubProps == NULL )
    return;

  wDigInt.setiid( m_Props, m_IID->GetValue().mb_str(wxConvUTF8) );

  wDDX.setport( m_SubProps, m_Device->GetValue().mb_str(wxConvUTF8) );
  wDDX.setportbase( m_SubProps, m_PortBase->GetValue().mb_str(wxConvUTF8) );
  wDDX.sets88port( m_SubProps, m_Port->GetValue().mb_str(wxConvUTF8) );

  wDDX.sets88busses( m_SubProps, m_Busses->GetValue() );
  wDDX.sets88b0modcnt( m_SubProps, m_Bus0->GetValue() );
  wDDX.sets88b1modcnt( m_SubProps, m_Bus1->GetValue() );
  wDDX.sets88b2modcnt( m_SubProps, m_Bus2->GetValue() );
  wDDX.sets88b3modcnt( m_SubProps, m_Bus3->GetValue() );
  wDDX.setshortcutdelay( m_SubProps, atoi( m_Delay->GetValue().mb_str(wxConvUTF8) ) );
  wDDX.setshortcutchecking( m_SubProps, m_SCD->IsChecked()?True:False );
  wDDX.setinversedsr( m_SubProps, m_InvDSR->IsChecked()?True:False );

  wDDX.setdcc( m_SubProps, m_GenDCC->IsChecked() ? True:False );
  wDDX.setmotorolarefresh( m_SubProps, m_GenMM->IsChecked() ? True:False );
  wDDX.setmotorola( m_SubProps, m_GenMMA->IsChecked() ? True:False );
  wDDX.setqueuecheck( m_SubProps, m_QueueCheck->IsChecked() ? True:False );
  wDDX.setfastcvget( m_SubProps, m_FastCVGet->IsChecked()?True:False );
  wDDX.setmmlongpause( m_SubProps, m_MMLongPause->IsChecked() ? True:False );
}

/*!
 * Dialog creator
 */

bool DDXCtrlDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin DDXCtrlDlg member initialisation
    m_Notebook = NULL;
    m_GenerelPanel = NULL;
    m_labIID = NULL;
    m_IID = NULL;
    m_labDevice = NULL;
    m_Device = NULL;
    m_labPortBase = NULL;
    m_PortBase = NULL;
    m_S88Panel = NULL;
    m_labPort = NULL;
    m_Port = NULL;
    m_labBusses = NULL;
    m_Busses = NULL;
    m_labBus0 = NULL;
    m_Bus0 = NULL;
    m_labBus1 = NULL;
    m_Bus1 = NULL;
    m_labBus2 = NULL;
    m_Bus2 = NULL;
    m_labBus3 = NULL;
    m_Bus3 = NULL;
    m_DetailsPanel = NULL;
    m_SCDBox = NULL;
    m_SCDbox = NULL;
    m_SCD = NULL;
    m_InvDSR = NULL;
    m_labDelay = NULL;
    m_Delay = NULL;
    m_LocoRefreshBox = NULL;
    m_QueueCheck = NULL;
    m_FastCVGet = NULL;
    m_GenerateBox = NULL;
    m_GenDCC = NULL;
    m_GenMM = NULL;
    m_GenMMA = NULL;
    m_MMLongPause = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end DDXCtrlDlg member initialisation

////@begin DDXCtrlDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end DDXCtrlDlg creation
    return true;
}

/*!
 * Control creation for Dialog
 */

void DDXCtrlDlg::CreateControls()
{
////@begin DDXCtrlDlg content construction
    DDXCtrlDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    m_Notebook = new wxNotebook( itemDialog1, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_TabAlign );

    m_GenerelPanel = new wxPanel( m_Notebook, ID_PANEL_DDL_GEN, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer5 = new wxBoxSizer(wxVERTICAL);
    m_GenerelPanel->SetSizer(itemBoxSizer5);

    wxFlexGridSizer* itemFlexGridSizer6 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer6->AddGrowableCol(1);
    itemBoxSizer5->Add(itemFlexGridSizer6, 0, wxGROW|wxALL, 5);
    m_labIID = new wxStaticText( m_GenerelPanel, ID_STATICTEXT_DDL_IID, _("iid"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labIID, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_IID = new wxTextCtrl( m_GenerelPanel, ID_TEXTCTRL_DDL_IID, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_IID, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDevice = new wxStaticText( m_GenerelPanel, ID_STATICTEXT_DDL_DEVICE, _("device"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labDevice, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxArrayString m_DeviceStrings;
    m_Device = new wxComboBox( m_GenerelPanel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, m_DeviceStrings, wxCB_DROPDOWN );
    itemFlexGridSizer6->Add(m_Device, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labPortBase = new wxStaticText( m_GenerelPanel, wxID_ANY, _("portbase"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer6->Add(m_labPortBase, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_PortBase = new wxTextCtrl( m_GenerelPanel, wxID_ANY, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer6->Add(m_PortBase, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_GenerelPanel, _("Generel"));

    m_S88Panel = new wxPanel( m_Notebook, ID_PANEL_DDL_S88, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer14 = new wxBoxSizer(wxVERTICAL);
    m_S88Panel->SetSizer(itemBoxSizer14);

    wxFlexGridSizer* itemFlexGridSizer15 = new wxFlexGridSizer(0, 2, 0, 0);
    itemFlexGridSizer15->AddGrowableCol(1);
    itemBoxSizer14->Add(itemFlexGridSizer15, 0, wxGROW, 5);
    m_labPort = new wxStaticText( m_S88Panel, ID_STATICTEXT_DDL_PORT, _("port"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(m_labPort, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Port = new wxTextCtrl( m_S88Panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(m_Port, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labBusses = new wxStaticText( m_S88Panel, ID_STATICTEXT_DDL_BUSSES, _("busses"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(m_labBusses, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Busses = new wxSpinCtrl( m_S88Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 4, 0 );
    itemFlexGridSizer15->Add(m_Busses, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labBus0 = new wxStaticText( m_S88Panel, ID_STATICTEXT_DDL_BUS0, _("bus0 modules"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(m_labBus0, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Bus0 = new wxSpinCtrl( m_S88Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 62, 0 );
    itemFlexGridSizer15->Add(m_Bus0, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labBus1 = new wxStaticText( m_S88Panel, ID_STATICTEXT_DDL_BUS1, _("bus1 modules"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(m_labBus1, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Bus1 = new wxSpinCtrl( m_S88Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 62, 0 );
    itemFlexGridSizer15->Add(m_Bus1, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labBus2 = new wxStaticText( m_S88Panel, ID_STATICTEXT_DDL_BUS2, _("bus2 modules"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(m_labBus2, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Bus2 = new wxSpinCtrl( m_S88Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 62, 0 );
    itemFlexGridSizer15->Add(m_Bus2, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labBus3 = new wxStaticText( m_S88Panel, ID_STATICTEXT_DDL_BUS3, _("bus3 modules"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(m_labBus3, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Bus3 = new wxSpinCtrl( m_S88Panel, wxID_ANY, _T("0"), wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 0, 62, 0 );
    itemFlexGridSizer15->Add(m_Bus3, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_S88Panel, _("S88"));

    m_DetailsPanel = new wxPanel( m_Notebook, ID_PANEL_DDL_DETAIL, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTAB_TRAVERSAL );
    wxBoxSizer* itemBoxSizer29 = new wxBoxSizer(wxVERTICAL);
    m_DetailsPanel->SetSizer(itemBoxSizer29);

    wxStaticBox* itemStaticBoxSizer30Static = new wxStaticBox(m_DetailsPanel, wxID_ANY, _("shortcut checking"));
    m_SCDBox = new wxStaticBoxSizer(itemStaticBoxSizer30Static, wxVERTICAL);
    itemBoxSizer29->Add(m_SCDBox, 0, wxGROW|wxALL, 5);
    m_SCDbox = new wxFlexGridSizer(2, 2, 0, 0);
    m_SCDbox->AddGrowableCol(1);
    m_SCDBox->Add(m_SCDbox, 0, wxGROW, 5);
    m_SCD = new wxCheckBox( m_DetailsPanel, ID_CHECKBOX_DDL_ENABLE_SCD, _("enable"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_SCD->SetValue(false);
    m_SCDbox->Add(m_SCD, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_InvDSR = new wxCheckBox( m_DetailsPanel, ID_CHECKBOX_DDL_INVERSE, _("invers DSR"), wxDefaultPosition, wxDefaultSize, wxCHK_2STATE );
    m_InvDSR->SetValue(false);
    m_SCDbox->Add(m_InvDSR, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labDelay = new wxStaticText( m_DetailsPanel, ID_STATICTEXT_DDL_DELAY, _("delay"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SCDbox->Add(m_labDelay, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Delay = new wxTextCtrl( m_DetailsPanel, ID_TEXTCTRL_DDL_DELAY, _("1000"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    m_SCDbox->Add(m_Delay, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LocoRefreshBox = new wxStaticBox(m_DetailsPanel, wxID_ANY, _("Options"));
    wxStaticBoxSizer* itemStaticBoxSizer36 = new wxStaticBoxSizer(m_LocoRefreshBox, wxHORIZONTAL);
    itemBoxSizer29->Add(itemStaticBoxSizer36, 0, wxGROW|wxALL, 5);
    m_QueueCheck = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Check Tx"), wxDefaultPosition, wxDefaultSize, 0 );
    m_QueueCheck->SetValue(false);
    itemStaticBoxSizer36->Add(m_QueueCheck, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_FastCVGet = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("Fast CV Get"), wxDefaultPosition, wxDefaultSize, 0 );
    m_FastCVGet->SetValue(false);
    itemStaticBoxSizer36->Add(m_FastCVGet, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 5);

    m_GenerateBox = new wxStaticBox(m_DetailsPanel, wxID_ANY, _("Generate"));
    wxStaticBoxSizer* itemStaticBoxSizer39 = new wxStaticBoxSizer(m_GenerateBox, wxHORIZONTAL);
    itemBoxSizer29->Add(itemStaticBoxSizer39, 0, wxGROW|wxALL, 5);
    m_GenDCC = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("DCC"), wxDefaultPosition, wxDefaultSize, 0 );
    m_GenDCC->SetValue(false);
    itemStaticBoxSizer39->Add(m_GenDCC, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_GenMM = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("MM"), wxDefaultPosition, wxDefaultSize, 0 );
    m_GenMM->SetValue(false);
    itemStaticBoxSizer39->Add(m_GenMM, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_GenMMA = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("MMA"), wxDefaultPosition, wxDefaultSize, 0 );
    m_GenMMA->SetValue(false);
    itemStaticBoxSizer39->Add(m_GenMMA, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxTOP|wxBOTTOM, 5);

    m_MMLongPause = new wxCheckBox( m_DetailsPanel, wxID_ANY, _("MMLP"), wxDefaultPosition, wxDefaultSize, 0 );
    m_MMLongPause->SetValue(false);
    itemStaticBoxSizer39->Add(m_MMLongPause, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Notebook->AddPage(m_DetailsPanel, _("Details"));

    itemBoxSizer2->Add(m_Notebook, 1, wxGROW|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer44 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer44, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer44->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer44->AddButton(m_Cancel);

    itemStdDialogButtonSizer44->Realize();

////@end DDXCtrlDlg content construction
}

/*!
 * Should we show tooltips?
 */

bool DDXCtrlDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap DDXCtrlDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin DDXCtrlDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end DDXCtrlDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon DDXCtrlDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin DDXCtrlDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end DDXCtrlDlg icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void DDXCtrlDlg::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void DDXCtrlDlg::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


