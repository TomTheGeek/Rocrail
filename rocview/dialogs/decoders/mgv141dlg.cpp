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
#include "mgv141dlg.h"

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/defs.h"
#endif

#include "rocview/public/guiapp.h"

#include "rocs/public/trace.h"

MGV141Dlg::MGV141Dlg( wxWindow* parent )
    :mgv141gen( parent )
{

}

void MGV141Dlg::initLabels() {
  TraceOp.trc( "mgv141", TRCLEVEL_INFO, __LINE__, 9999, "initLabels" );
  SetTitle(wxGetApp().getMsg( "cartable" ));
  m_NoteBook->SetPageText( 0, wxGetApp().getMsg( "query" ) );
  m_NoteBook->SetPageText( 1, wxGetApp().getMsg( "setup" ) );

  // Buttons
  m_StdButtonsOK->SetLabel( wxGetApp().getMsg( "ok" ) );
}

void MGV141Dlg::onUnitSet( wxCommandEvent& event )
{
	// TODO: Implement onUnitSet
}

void MGV141Dlg::onWriteAll( wxCommandEvent& event )
{
	// TODO: Implement onWriteAll
}

void MGV141Dlg::onReadAll( wxCommandEvent& event )
{
	// TODO: Implement onReadAll
}

void MGV141Dlg::onOK( wxCommandEvent& event )
{
  EndModal( wxID_OK );
}

void MGV141Dlg::onUnitSelected( wxCommandEvent& event ) {

}

void MGV141Dlg::onQuery( wxCommandEvent& event ) {

}

