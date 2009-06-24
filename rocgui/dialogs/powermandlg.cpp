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

#include "rocs/public/trace.h"


PowerManDlg::PowerManDlg( wxWindow* parent ):powermandlggen( parent )
{
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


void PowerManDlg::initValues() {
}

void PowerManDlg::evaluate() {
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
	// TODO: Implement OnApply
}

void PowerManDlg::OnCancel( wxCommandEvent& event )
{
	// TODO: Implement OnCancel
}

void PowerManDlg::OnOK( wxCommandEvent& event )
{
	// TODO: Implement OnOK
}
