/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include "powermandlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocgui/public/guiapp.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Booster.h"
#include "rocrail/wrapper/public/BoosterList.h"
#include "rocrail/wrapper/public/Item.h"

#include "rocs/public/trace.h"


PowerManDlg::PowerManDlg( wxWindow* parent ):powermandlggen( parent )
{
  m_Props = NULL;
  initLabels();
  initIndex();

  m_IndexPanel->GetSizer()->Layout();
  m_GeneralPanel->GetSizer()->Layout();
  m_ModulesPanel->GetSizer()->Layout();
  m_BlocksPanel->GetSizer()->Layout();
  m_DetailsPanel->GetSizer()->Layout();

  m_BoosterBook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  m_BoosterList->SetFocus();

  m_BoosterBook->Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( PowerManDlg::onSetPage ), NULL, this );
  m_SetPage = 0;

  wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_BOOSTERBOOK );
  wxPostEvent( m_BoosterBook, event );
}


void PowerManDlg::onSetPage(wxCommandEvent& event) {
  TraceOp.trc( "boosterdlg", TRCLEVEL_INFO, __LINE__, 9999, "set page to %d", m_SetPage );
  m_BoosterBook->SetSelection( m_SetPage );
}


void PowerManDlg::initLabels() {
  TraceOp.trc( "boosterdlg", TRCLEVEL_INFO, __LINE__, 9999, "initLabels" );
  SetTitle(wxGetApp().getMsg( "boostertable" ));
  m_BoosterBook->SetPageText( 0, wxGetApp().getMsg( "index" ) );
  m_BoosterBook->SetPageText( 1, wxGetApp().getMsg( "general" ) );
  m_BoosterBook->SetPageText( 2, wxGetApp().getMsg( "modules" ) );
  m_BoosterBook->SetPageText( 3, wxGetApp().getMsg( "blocks" ) );
  m_BoosterBook->SetPageText( 4, wxGetApp().getMsg( "details" ) );

  // Index
  m_AddBooster->SetLabel( wxGetApp().getMsg( "new" ) );
  m_DelBooster->SetLabel( wxGetApp().getMsg( "delete" ) );

  // General
  m_labID->SetLabel( wxGetApp().getMsg( "id" ) );
  m_labDistrict->SetLabel( wxGetApp().getMsg( "district" ) );
}


void PowerManDlg::initIndex() {
  SetTitle(wxGetApp().getMsg( "boostertable" ));

  m_BoosterList->Clear();


  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode boosterlist = wPlan.getboosterlist( model );
    if( boosterlist != NULL ) {
      iONode booster = wBoosterList.getbooster( boosterlist );
      while( booster != NULL ) {
        const char* id = wBooster.getid( booster );
        m_BoosterList->Append( wxString(id,wxConvUTF8), booster );
        booster = wBoosterList.nextbooster( boosterlist, booster );
      }
    }
  }
}


void PowerManDlg::onSelect( wxCommandEvent& event ){
  if( m_BoosterList->GetSelection() != wxNOT_FOUND ) {
    m_Props = (iONode)m_BoosterList->GetClientData(m_BoosterList->GetSelection());
    if( m_Props != NULL )
      initValues();
    else
      TraceOp.trc( "boosterdlg", TRCLEVEL_INFO, __LINE__, 9999, "no selection..." );
  }
}



void PowerManDlg::initValues() {
  if( m_Props == NULL ) {
    TraceOp.trc( "boosterdlg", TRCLEVEL_DEBUG, __LINE__, 9999, "no booster selected" );
    return;
  }

  char* title = StrOp.fmt( "%s %s", (const char*)wxGetApp().getMsg("booster").mb_str(wxConvUTF8), wBooster.getid( m_Props ) );
  SetTitle( wxString(title,wxConvUTF8) );
  StrOp.free( title );

  // init General
  m_ID->SetValue( wxString(wBooster.getid( m_Props ),wxConvUTF8) );
  m_District->SetValue( wxString(wBooster.getdistrict( m_Props ),wxConvUTF8) );
}

bool PowerManDlg::evaluate() {
  if( m_Props == NULL )
    return false;

  TraceOp.trc( "boosterdlg", TRCLEVEL_INFO, __LINE__, 9999, "Evaluate %s", wBooster.getid( m_Props ) );

  if( m_ID->GetValue().Len() == 0 ) {
    wxMessageDialog( this, wxGetApp().getMsg("invalidid"), _T("Rocrail"), wxOK | wxICON_ERROR ).ShowModal();
    m_ID->SetValue( wxString(wBooster.getid( m_Props ),wxConvUTF8) );
    return false;
  }
  // evaluate General
  wItem.setprev_id( m_Props, wItem.getid(m_Props) );
  wBooster.setid( m_Props, m_ID->GetValue().mb_str(wxConvUTF8) );
  wBooster.setdistrict( m_Props, m_District->GetValue().mb_str(wxConvUTF8) );

  return true;
}


void PowerManDlg::OnAddBooster( wxCommandEvent& event )
{
	// TODO: Implement OnAddBooster
}

void PowerManDlg::OnDelBooster( wxCommandEvent& event )
{
	// TODO: Implement OnDelBooster
}

void PowerManDlg::OnAddModule( wxCommandEvent& event )
{
	// TODO: Implement OnAddModule
}

void PowerManDlg::OnDelModule( wxCommandEvent& event )
{
	// TODO: Implement OnDelModule
}

void PowerManDlg::OnAddBlock( wxCommandEvent& event )
{
	// TODO: Implement OnAddBlock
}

void PowerManDlg::OnDelBlock( wxCommandEvent& event )
{
	// TODO: Implement OnDelBlock
}

void PowerManDlg::OnApply( wxCommandEvent& event )
{
  if( m_Props == NULL )
    return;

  if( !evaluate() )
    return;

  if( !wxGetApp().isStayOffline() ) {
    /* Notify RocRail. */
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.modify );
    NodeOp.addChild( cmd, (iONode)m_Props->base.clone( m_Props ) );
    wxGetApp().sendToRocrail( cmd );
    cmd->base.del(cmd);
  }
  initIndex();
}

void PowerManDlg::OnCancel( wxCommandEvent& event )
{
  EndModal( 0 );
}

void PowerManDlg::OnOK( wxCommandEvent& event )
{
  OnApply(event);
  EndModal( wxID_OK );
}
