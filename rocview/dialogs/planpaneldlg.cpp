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
#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "planpaneldlg.h"
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

#include "planpaneldlg.h"
#include "rocs/public/str.h"
#include "rocrail/wrapper/public/ZLevel.h"
#include "rocrail/wrapper/public/ModelCmd.h"

#include "rocview/public/guiapp.h"

////@begin XPM images
////@end XPM images

/*!
 * PlanPanelProps type definition
 */

IMPLEMENT_DYNAMIC_CLASS( PlanPanelProps, wxDialog )

/*!
 * PlanPanelProps event table definition
 */

BEGIN_EVENT_TABLE( PlanPanelProps, wxDialog )

////@begin PlanPanelProps event table entries
    EVT_BUTTON( wxID_OK, PlanPanelProps::OnOkClick )

    EVT_BUTTON( wxID_CANCEL, PlanPanelProps::OnCancelClick )

////@end PlanPanelProps event table entries

END_EVENT_TABLE()

/*!
 * PlanPanelProps constructors
 */

PlanPanelProps::PlanPanelProps( )
{
}

PlanPanelProps::PlanPanelProps( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

PlanPanelProps::PlanPanelProps( wxWindow* parent, iONode zlevel )
{
  Create(parent, -1, wxGetApp().getMsg("planprops") );
  m_Props = zlevel;

  if( m_Props == NULL ) {
    m_Props = NodeOp.inst( wZLevel.name(), NULL, ELEMENT_NODE );
  }
  initLabels();
  initValues();
  GetSizer()->Layout();
}


void PlanPanelProps::initLabels() {
  m_LabelTitle->SetLabel( wxGetApp().getMsg( "title" ) );
  m_LabelZLevel->SetLabel( wxGetApp().getMsg( "level" ) );

  // Buttons
  m_OK->SetLabel( wxGetApp().getMsg( "ok" ) );
  m_Cancel->SetLabel( wxGetApp().getMsg( "cancel" ) );
}


void PlanPanelProps::initValues() {
  // General
  m_Title->SetValue( wxString( wZLevel.gettitle( m_Props ),wxConvUTF8 ) );
  char* val = StrOp.fmt( "%d", wZLevel.getz( m_Props ) );
  m_ZLevel->SetValue( wxString(val,wxConvUTF8 ) );
  //m_ZLevel->Enable(false);
  StrOp.free( val );
}


void PlanPanelProps::evaluate() {
  if( m_Props == NULL )
    return;
  // General
  wZLevel.settitle( m_Props, m_Title->GetValue().mb_str(wxConvUTF8) );
  wZLevel.setz( m_Props, atoi( m_ZLevel->GetValue().mb_str(wxConvUTF8) ) );
}




/*!
 * PlanPanelProps creator
 */

bool PlanPanelProps::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin PlanPanelProps member initialisation
    m_LabelTitle = NULL;
    m_Title = NULL;
    m_LabelZLevel = NULL;
    m_ZLevel = NULL;
    m_OK = NULL;
    m_Cancel = NULL;
////@end PlanPanelProps member initialisation

////@begin PlanPanelProps creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end PlanPanelProps creation
    return true;
}

/*!
 * Control creation for PlanPanelProps
 */

void PlanPanelProps::CreateControls()
{
////@begin PlanPanelProps content construction
    PlanPanelProps* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxFlexGridSizer* itemFlexGridSizer3 = new wxFlexGridSizer(2, 2, 0, 0);
    itemFlexGridSizer3->AddGrowableCol(1);
    itemBoxSizer2->Add(itemFlexGridSizer3, 0, wxGROW|wxALL, 5);

    m_LabelTitle = new wxStaticText( itemDialog1, ID_STATICTEXT_PP_TITLE, _("Title"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_LabelTitle, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_Title = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_PP_TITLE, _T(""), wxDefaultPosition, wxSize(200, -1), 0 );
    m_Title->SetMaxLength(40);
    itemFlexGridSizer3->Add(m_Title, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    m_LabelZLevel = new wxStaticText( itemDialog1, ID_STATICTEXT_PP_ZLEVEL, _("zLevel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer3->Add(m_LabelZLevel, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    m_ZLevel = new wxTextCtrl( itemDialog1, ID_TEXTCTRL_PP_ZLEVEL, _("0"), wxDefaultPosition, wxDefaultSize, wxTE_CENTRE );
    itemFlexGridSizer3->Add(m_ZLevel, 0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStdDialogButtonSizer* itemStdDialogButtonSizer8 = new wxStdDialogButtonSizer;

    itemBoxSizer2->Add(itemStdDialogButtonSizer8, 0, wxALIGN_RIGHT|wxALL, 5);
    m_OK = new wxButton( itemDialog1, wxID_OK, _("&OK"), wxDefaultPosition, wxDefaultSize, 0 );
    m_OK->SetDefault();
    itemStdDialogButtonSizer8->AddButton(m_OK);

    m_Cancel = new wxButton( itemDialog1, wxID_CANCEL, _("&Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
    itemStdDialogButtonSizer8->AddButton(m_Cancel);

    itemStdDialogButtonSizer8->Realize();

////@end PlanPanelProps content construction
}

/*!
 * Should we show tooltips?
 */

bool PlanPanelProps::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap PlanPanelProps::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin PlanPanelProps bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end PlanPanelProps bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon PlanPanelProps::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin PlanPanelProps icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end PlanPanelProps icon retrieval
}
/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_OK
 */

void PlanPanelProps::OnOkClick( wxCommandEvent& event )
{
  evaluate();
  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  EndModal( wxID_OK );
}

/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for wxID_CANCEL
 */

void PlanPanelProps::OnCancelClick( wxCommandEvent& event )
{
  EndModal( 0 );
}


