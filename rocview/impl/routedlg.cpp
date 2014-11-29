/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 


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
#include "rocrail/wrapper/public/Item.h"

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
             wxDEFAULT_DIALOG_STYLE )
{

  wxBoxSizer *sizer2 = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer *sizer1 = new wxBoxSizer(wxHORIZONTAL);

  m_Test = new wxButton( this, -1, wxGetApp().getMsg("test") );
  m_Force = new wxButton( this, -1, wxGetApp().getMsg("force") );
  m_Lock = new wxButton( this, -1, wxGetApp().getMsg("lock") );
  m_Quit = new wxButton( this, -1, wxGetApp().getMsg("cancel") );

  m_Grid = new wxGrid( this, -1, wxDefaultPosition, wxSize(-1,400) ) ;
  m_Grid->CreateGrid( 1, 4 );
  m_Grid->SetBackgroundColour( m_Grid->GetLabelBackgroundColour() );
  m_Grid->SetColLabelSize(0);
  m_Grid->SetColLabelSize(m_Grid->GetRowHeight(0));

  m_Grid->EnableEditing(false);

  m_Grid->SetColLabelValue(0, wxGetApp().getMsg("streetid"));
  m_Grid->SetColLabelValue(1, wxGetApp().getMsg("fromblock"));
  m_Grid->SetColLabelValue(2, wxGetApp().getMsg("toblock"));
  m_Grid->SetColLabelValue(3, wxGetApp().getMsg("commands"));

  init();
  m_Grid->SetSelectionMode(wxGrid::wxGridSelectRows);
  m_Grid->AutoSizeColumns();

  sizer2->Add( m_Grid, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 5);

  sizer1->Add( m_Quit, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 5 );
  sizer1->Add( m_Test, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 5 );
  sizer1->Add( m_Force, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 5 );
  sizer1->Add( m_Lock, 0, wxALIGN_CENTER | wxEXPAND | wxALL, 5 );

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
    wxString str = m_Grid->GetCellValue( m_Grid->GetGridCursorRow(), 0 );
    TraceOp.trace( NULL, TRCLEVEL_INFO, 0, "Testing %s", (const char*)str.mb_str(wxConvUTF8) );
    {
      iONode cmd = NodeOp.inst( wRoute.name(), NULL, ELEMENT_NODE );
      wRoute.setcmd( cmd, wRoute.test );
      wRoute.setid( cmd, str.mb_str(wxConvUTF8) );
      wRoute.setstatus( cmd, wRoute.status_free);
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }
  }
  else if( event.GetEventObject() == m_Force ) {
    wxString str = m_Grid->GetCellValue( m_Grid->GetGridCursorRow(), 0 );
    TraceOp.trace( NULL, TRCLEVEL_INFO, 0, "Forcing %s", (const char*)str.mb_str(wxConvUTF8) );
    {
      iONode cmd = NodeOp.inst( wRoute.name(), NULL, ELEMENT_NODE );
      wRoute.setcmd( cmd, wRoute.force );
      wRoute.setid( cmd, str.mb_str(wxConvUTF8) );
      wRoute.setstatus( cmd, wRoute.status_free);
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }
  }
  else if( event.GetEventObject() == m_Lock ) {
    wxString str = m_Grid->GetCellValue( m_Grid->GetGridCursorRow(), 0 );
    TraceOp.trace( NULL, TRCLEVEL_INFO, 0, "Lock %s", (const char*)str.mb_str(wxConvUTF8) );
    {
      iONode cmd = NodeOp.inst( wRoute.name(), NULL, ELEMENT_NODE );
      wRoute.setcmd( cmd, wRoute.test );
      wRoute.setid( cmd, str.mb_str(wxConvUTF8) );
      wRoute.setstatus( cmd, wRoute.status_locked);
      wxGetApp().sendToRocrail( cmd );
      cmd->base.del(cmd);
    }
  }
  else
  {
    event.Skip();
  }
}

/* comparator for sorting by id: */
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getid( a );
    const char* idB = wItem.getid( b );
    return strcmp( idA, idB );
}

void RouteCtrlDlg::fillTable( iONode node ) {
  int cnt = NodeOp.getChildCnt( node );
  if( m_Grid->GetNumberRows() > 0 )
    m_Grid->DeleteRows( 0, m_Grid->GetNumberRows() );
  m_Grid->AppendRows( cnt );

  iOList list = ListOp.inst();
  for( int i = 0; i < cnt; i++ ) {
    iONode st = NodeOp.getChild( node, i );
    const char* id = wRoute.getid( st );
    if( id != NULL ) {
      ListOp.add(list, (obj)st);
    }
  }

  ListOp.sort(list, &__sortID);

  for( int i = 0; i < cnt; i++ ) {
    iONode child = (iONode)ListOp.get(list, i); //NodeOp.getChild( node, i );

    m_Grid->SetCellValue( i, 0, wxString(wRoute.getid( child ),wxConvUTF8) );
    m_Grid->SetCellValue( i, 1, wxString(wRoute.getbka( child ),wxConvUTF8) );
    m_Grid->SetCellValue( i, 2, wxString(wRoute.getbkb( child ),wxConvUTF8) );
    char* val = StrOp.fmt( "%d", NodeOp.getChildCnt( child ) );
    m_Grid->SetCellValue( i, 3, wxString(val,wxConvUTF8) );
    StrOp.free( val);
  }
  if( m_Grid->GetNumberRows() > 0 )
    m_Grid->SelectRow(0);
}

void RouteCtrlDlg::init() {
  iONode model = wxGetApp().getModel();
  if( model != NULL ) {
    iONode streetdb = wPlan.getstlist( model );
    if( streetdb != NULL )
      fillTable( streetdb );
  }
  else {
    m_Test->Enable( false );
    m_Force->Enable( false );
    m_Lock->Enable( false );
  }
}




void RouteCtrlDlg::OnClose(wxCloseEvent& event) {
  Destroy();
}

