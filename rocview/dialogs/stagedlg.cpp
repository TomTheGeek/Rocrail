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
#include "stagedlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"

StageDlg::StageDlg( wxWindow* parent, iONode p_Props ):stagedlggen( parent )
{
  TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "stagedlg" );
  m_TabAlign = wxGetApp().getTabAlign();
  m_Props = p_Props;
  initLabels();

  m_General->GetSizer()->Layout();
  m_Sections->GetSizer()->Layout();

  m_Notebook->Fit();

  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);

  m_Notebook->Connect( wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( StageDlg::OnSetPage ), NULL, this );
  m_SetPage = 0;

  if( m_Props != NULL ) {
    initValues();
    m_SetPage = 0;
  }
  wxCommandEvent event( wxEVT_COMMAND_MENU_SELECTED, ID_STAGEBOOK );
  wxPostEvent( m_Notebook, event );

}


void StageDlg::OnSetPage(wxCommandEvent& event) {
  TraceOp.trc( "stagedlg", TRCLEVEL_INFO, __LINE__, 9999, "set page to %d", m_SetPage );
  m_Notebook->SetSelection( m_SetPage );
}


void StageDlg::initLabels() {

}

bool StageDlg::evaluate() {

}

void StageDlg::initSections() {

}

void StageDlg::initValues() {

}


void StageDlg::OnSectionList( wxCommandEvent& event )
{
	// TODO: Implement OnSectionList
}

void StageDlg::OnSectionAdd( wxCommandEvent& event )
{
	// TODO: Implement OnSectionAdd
}

void StageDlg::OnSectionModify( wxCommandEvent& event )
{
	// TODO: Implement OnSectionModify
}

void StageDlg::OnSectionDelete( wxCommandEvent& event )
{
	// TODO: Implement OnSectionDelete
}

void StageDlg::OnOK( wxCommandEvent& event )
{
  EndModal( wxID_OK );
}
void StageDlg::OnCancel( wxCommandEvent& event )
{
  EndModal( 0 );
}
