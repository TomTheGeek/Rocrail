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
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "modplandlg.h"
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
////@end includes

#include "modplandlg.h"

#include "rocs/public/node.h"

#include "rocgui/public/guiapp.h"

#include "rocrail/wrapper/public/ModPlan.h"
#include "rocrail/wrapper/public/SysCmd.h"

////@begin XPM images
////@end XPM images


/*!
 * ModPlanDlg type definition
 */

IMPLEMENT_DYNAMIC_CLASS( ModPlanDlg, wxDialog )


/*!
 * ModPlanDlg event table definition
 */

BEGIN_EVENT_TABLE( ModPlanDlg, wxDialog )

////@begin ModPlanDlg event table entries
    EVT_CHECKBOX( ID_MODPLAN_SAVE, ModPlanDlg::OnModplanSaveClick )

    EVT_BUTTON( wxID_OK, ModPlanDlg::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, ModPlanDlg::OnCancelClick )

////@end ModPlanDlg event table entries

END_EVENT_TABLE()


/*!
 * ModPlanDlg constructors
 */

ModPlanDlg::ModPlanDlg()
{
    Init();
}

ModPlanDlg::ModPlanDlg( wxWindow* parent, iONode p_Props )
{
  m_Props = p_Props;
  m_TabAlign = wxGetApp().getTabAlign();
  Init();
  Create(parent, -1, wxGetApp().getMsg("modplan") );

  initLabels();
  initValues();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

}

void ModPlanDlg::initLabels() {
  m_labTitle->SetLabel( wxGetApp().getMsg( "title" ) );
  m_labLocoFile->SetLabel( wxGetApp().getMsg( "loctable" ) );
  m_labRoutesFile->SetLabel( wxGetApp().getMsg( "routetable" ) );
  m_OptionsBox->SetLabel( wxGetApp().getMsg( "option" ) );
  m_InitField->SetLabel( wxGetApp().getMsg( "initfield" ) );
  m_Save->SetLabel( wxGetApp().getMsg( "save" ) );
  m_ModRoutes->SetLabel( wxGetApp().getMsg( "modroutes" ) );

  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
}

void ModPlanDlg::initValues() {
  if( m_Props == NULL )
    return;

  m_Title->SetValue( wxString( wModPlan.gettitle( m_Props ),wxConvUTF8 ) );
  m_LocoFile->SetValue( wxString( wModPlan.getlocs( m_Props ),wxConvUTF8 ) );
  m_RoutesFile->SetValue( wxString( wModPlan.getroutes( m_Props ),wxConvUTF8 ) );

  m_InitField->SetValue(wModPlan.isinitfield( m_Props ));
  m_Save->SetValue(wModPlan.issavemodplan( m_Props ));
  m_SaveModules->SetValue(wModPlan.issavemodules( m_Props ));
  m_ModRoutes->SetValue(wModPlan.ismodroutes( m_Props ));
}


void ModPlanDlg::evaluate() {
  if( m_Props == NULL )
    return;

  wModPlan.settitle( m_Props, m_Title->GetValue().mb_str(wxConvUTF8) );
  wModPlan.setlocs( m_Props, m_LocoFile->GetValue().mb_str(wxConvUTF8) );
  wModPlan.setroutes( m_Props, m_RoutesFile->GetValue().mb_str(wxConvUTF8) );

  wModPlan.setinitfield( m_Props, m_InitField->IsChecked() ? True:False );
  wModPlan.setsavemodplan( m_Props, m_Save->IsChecked() ? True:False );
  wModPlan.setsavemodules( m_Props, m_SaveModules->IsChecked() ? True:False );
  wModPlan.setmodroutes( m_Props, m_ModRoutes->IsChecked() ? True:False );
}



/*!
 * ModPlanDlg creator
 */

bool ModPlanDlg::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin ModPlanDlg creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end ModPlanDlg creation
    return true;
}


/*!
 * ModPlanDlg destructor
 */

ModPlanDlg::~ModPlanDlg()
{
////@begin ModPlanDlg destruction
////@end ModPlanDlg destruction
}


/*!
 * Member initialisation
 */

void ModPlanDlg::Init()
{
////@begin ModPlanDlg member initialisation
    m_labTitle = NULL;
    m_Title = NULL;
    m_labLocoFile = NULL;
    m_LocoFile = NULL;
    m_labRoutesFile = NULL;
    m_RoutesFile = NULL;
    m_OptionsBox = NULL;
    m_InitField = NULL;
    m_Save = NULL;
    m_SaveModules = NULL;
    m_ModRoutes = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end ModPlanDlg member initialisation
}


/*!
 * Control creation for ModPlanDlg
 */

void ModPlanDlg::CreateControls()
{
////@begin ModPlanDlg content construction
    ModPlanDlg* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(2, 2, 0, 0);
    itemBoxSizer2->Add(itemFlexGridSizer3, 0, wxGROW|wxALL, 5);

    m_labTitle = new wxStaticText( itemDialog1, wxID_ANY, _("Title"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_labTitle, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Title = new wxTextCtrl( itemDialog1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(160, -1), 0 );
    itemFlexGridSizer3->Add(m_Title, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labLocoFile = new wxStaticText( itemDialog1, wxID_ANY, _("Loco file"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_labLocoFile, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LocoFile = new wxTextCtrl( itemDialog1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(160, -1), 0 );
    itemFlexGridSizer3->Add(m_LocoFile, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_labRoutesFile = new wxStaticText( itemDialog1, wxID_ANY, _("Routes file"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_labRoutesFile, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_RoutesFile = new wxTextCtrl( itemDialog1, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(160, -1), 0 );
    itemFlexGridSizer3->Add(m_RoutesFile, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_OptionsBox = new wxStaticBox(itemDialog1, wxID_ANY, _("Options"));
    wxStaticBoxSizer* itemStaticBoxSizer10 = new wxStaticBoxSizer(m_OptionsBox, wxHORIZONTAL);
    itemBoxSizer2->Add(itemStaticBoxSizer10, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer11 = new wxFlexGridSizer(2, 2, 0, 0);
    itemStaticBoxSizer10->Add(itemFlexGridSizer11, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_InitField = new wxCheckBox( itemDialog1, wxID_ANY, _("Init field"), wxDefaultPosition, wxDefaultSize, 0 );
    m_InitField->SetValue(false);
    itemFlexGridSizer11->Add(m_InitField, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_Save = new wxCheckBox( itemDialog1, ID_MODPLAN_SAVE, _("Save"), wxDefaultPosition, wxDefaultSize, 0 );
    m_Save->SetValue(false);
    itemFlexGridSizer11->Add(m_Save, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_SaveModules = new wxCheckBox( itemDialog1, wxID_ANY, _("Save modules"), wxDefaultPosition, wxDefaultSize, 0 );
    m_SaveModules->SetValue(false);
    itemFlexGridSizer11->Add(m_SaveModules, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_ModRoutes = new wxCheckBox( itemDialog1, wxID_ANY, _("Module routes"), wxDefaultPosition, wxDefaultSize, 0 );
    m_ModRoutes->SetValue(false);
    itemFlexGridSizer11->Add(m_ModRoutes, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer16 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer16, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer16->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer16->AddButton(m_Cancel);

    itemStdDialogButtonSizer16->Realize();

////@end ModPlanDlg content construction
}


/*!
 * Should we show tooltips?
 */

bool ModPlanDlg::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap ModPlanDlg::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin ModPlanDlg bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end ModPlanDlg bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon ModPlanDlg::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin ModPlanDlg icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end ModPlanDlg icon retrieval
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void ModPlanDlg::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void ModPlanDlg::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  /* Notify RocRail. */
  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, wSysCmd.setmodplan );
  NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
  wxGetApp().sendToRocrail( cmd );
  cmd->base.del(cmd);

  EndModal( wxID_OK );
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_MODPLAN_SAVE
 */

void ModPlanDlg::OnModplanSaveClick( wxCommandEvent& event )
{
  m_SaveModules->Enable( m_Save->IsChecked() );
}

