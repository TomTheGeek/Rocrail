/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------
#ifdef __GNUG__
    #pragma implementation "rocgui.cpp"
    #pragma interface "rocgui.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "rocview/public/guiapp.h"
#include "rocview/public/routedlg.h"
#include "rocview/public/base.h"

#include "rocs/public/str.h"
#include "rocs/public/mem.h"

#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Route.h"

BEGIN_EVENT_TABLE(RouteCtrlDlg, wxDialog)
    EVT_BUTTON(-1, RouteCtrlDlg::OnButton)
    EVT_CLOSE(RouteCtrlDlg::OnClose)
END_EVENT_TABLE()



// ----------------------------------------------------------------------------
// LcDlg
// ----------------------------------------------------------------------------

RouteCtrlDlg::RouteCtrlDlg(wxWindow *parent)
             : wxDialog(parent, -1, wxString( wxGetApp().getMsg("stctrl") ),
             wxDefaultPosition, wxDefaultSize,
             wxDEFAULT_DIALOG_STYLE|wxDIALOG_MODELESS )
{

  wxBoxSizer *sizer2 = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *sizer1 = new wxBoxSizer(wxHORIZONTAL);

  m_Test = new wxButton( this, -1, wxGetApp().getMsg("test") );
  m_Quit = new wxButton( this, -1, wxGetApp().getMsg("cancel") );

  m_Grid = new wxGrid( this, -1, wxDefaultPosition, wxSize(-1,400), wxSUNKEN_BORDER ) ;
  m_Grid->CreateGrid( 0, 4 );
  m_Grid->SetCellBackgroundColour( m_Grid->GetLabelBackgroundColour() );
  m_Grid->SetLabelSize(wxVERTICAL, 0);
  m_Grid->SetLabelSize(wxHORIZONTAL, m_Grid->GetRowHeight(0));

  m_Grid->SetEditable(FALSE);

  m_Grid->SetLabelValue(wxHORIZONTAL, wxGetApp().getMsg("streetid"), 0);
  m_Grid->SetLabelValue(wxHORIZONTAL, wxGetApp().getMsg("fromblock"), 1);
  m_Grid->SetLabelValue(wxHORIZONTAL, wxGetApp().getMsg("toblock"), 2);
  m_Grid->SetLabelValue(wxHORIZONTAL, wxGetApp().getMsg("commands"), 3);

  init();
  m_Grid->SetSelectionMode(wxGrid::wxGridSelectRows);
  m_Grid->AutoSizeColumns();

  sizer2->Add( m_Grid, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 5);

  sizer1->Add( m_Quit, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 5 );
  sizer1->Add( m_Test, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 5 );

  sizer2->Add( sizer1 , 0, wxALIGN_CENTER | wxEXPAND | wxALL, 0 );

  SetAutoLayout(TRUE);
  SetSizer(sizer2);

  sizer2->SetSizeHints(this);
  sizer2->Fit(this);

  m_Quit->SetDefault();
	Centre();
}

void RouteCtrlDlg::OnButton(wxCommandEvent& event)
{
  if ( event.GetEventObject() == m_Quit ) {
    Destroy();
    //EndModal(0);
  }
  else if( event.GetEventObject() == m_Test ) {
    wxString str = m_Grid->GetCellValue( m_Grid->GetCursorRow(), 0 );
    TraceOp.trace( NULL, TRCLEVEL_INFO, 0, "Testing %s", (const char*)str.mb_str(wxConvUTF8) );
    {
      iONode cmd = NodeOp.inst( wRoute.name(), NULL, ELEMENT_NODE );
      wRoute.setcmd( cmd, wRoute.test );
      wRoute.setid( cmd, str.mb_str(wxConvUTF8) );
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }
  }
  else
  {
    event.Skip();
  }
}

void RouteCtrlDlg::fillTable( iONode node ) {
  int cnt = NodeOp.getChildCnt( node );
  if( m_Grid->GetRows() > 0 )
    m_Grid->DeleteRows( 0, m_Grid->GetRows() );
  m_Grid->AppendRows( cnt );

  for( int i = 0; i < cnt; i++ ) {
    iONode child = NodeOp.getChild( node, i );

    m_Grid->SetCellValue( wxString(wRoute.getid( child ),wxConvUTF8), i, 0 );
    m_Grid->SetCellValue( wxString(wRoute.getbka( child ),wxConvUTF8), i, 1 );
    m_Grid->SetCellValue( wxString(wRoute.getbkb( child ),wxConvUTF8), i, 2 );
    char* val = StrOp.fmt( "%d", NodeOp.getChildCnt( child ) );
    m_Grid->SetCellValue( wxString(val,wxConvUTF8), i, 3 );
    StrOp.free( val);
  }
  if( m_Grid->GetRows() > 0 )
    m_Grid->SelectRow(0);
}

void RouteCtrlDlg::init() {
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode streetdb = wPlan.getstlist( model );
    if( streetdb != NULL )
      fillTable( streetdb );
  }
  else
    m_Test->Enable( false );
}




void RouteCtrlDlg::OnClose(wxCloseEvent& event) {
  Destroy();
}

