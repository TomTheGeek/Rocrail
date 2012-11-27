/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 as published by the Free Software Foundation; either version 2
 modify it under the terms of the GNU General Public License
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "widgetspanel.h"
#include "locowidget.h"

#include "rocrail/wrapper/public/Loc.h"

#include "rocs/public/trace.h"

#include "rocview/public/guiapp.h"
#include "rocview/wrapper/public/Gui.h"

WidgetsPanel::WidgetsPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
 : wxScrolledWindow( parent, id, pos, size, style )
{

  /*
  m_LocoPanel = new wxScrolledWindow( m_StatNotebook, -1, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxHSCROLL|wxVSCROLL );
  m_LocoPanel->SetScrollbars(1, 10, 0, 0);
  wxBoxSizer* l_LocoTopSizer = new wxBoxSizer( wxVERTICAL );
  m_LocoGridSizer = new wxGridSizer(0,4,3,3);
  l_LocoTopSizer->Add(m_LocoGridSizer);
  m_LocoPanel->SetSizer(l_LocoTopSizer);
  */

  m_WidgetWidth = wGui.getlocowidgetwidth(wxGetApp().getIni());

  SetScrollbars(1, 10, 0, 0);

  wxBoxSizer* m_TopSizer = new wxBoxSizer( wxVERTICAL );

  m_GridSizer = new wxGridSizer( 0, 4, 3, 3 );
  m_TopSizer->Add( m_GridSizer );

  SetSizer( m_TopSizer );

  // Connect Events
  Connect( wxEVT_SIZE, wxSizeEventHandler( WidgetsPanel::OnPanelSize ) );

  m_Widgets = ListOp.inst();
}

WidgetsPanel::~WidgetsPanel()
{
  // Disconnect Events
  this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( WidgetsPanel::OnPanelSize ) );

  for( int i = 0; i < ListOp.size(m_Widgets); i++) {
    LocoWidget* w = (LocoWidget*)ListOp.get(m_Widgets, i);
    m_GridSizer->Detach(w);
    delete w;
  }

  ListOp.base.del(m_Widgets);
}


void WidgetsPanel::OnPanelSize( wxSizeEvent& event )
{
  int w,h;
  GetSize(&w, &h);
  if( w == 0 )
    return;

  int cols = w / m_WidgetWidth;

  if( w % m_WidgetWidth > (m_WidgetWidth - 10) )
    cols++;

  TraceOp.trc( "frame", TRCLEVEL_INFO, __LINE__, 9999, "Loco Panel w=%d h=%d columns=%d", w, h, cols );

  if( cols > 0 && cols != m_GridSizer->GetCols() ) {
    m_GridSizer->SetCols(cols);
    m_GridSizer->Layout();
    FitInside();
  }
}

void WidgetsPanel::initView(iOList list) {
  for( int i = 0; i < ListOp.size(m_Widgets); i++) {
    LocoWidget* w = (LocoWidget*)ListOp.get(m_Widgets, i);
    m_GridSizer->Detach(w);
    delete w;
  }

  ListOp.clear(m_Widgets);

  for( int i = 0; i < ListOp.size( list ); i++ ) {
    iONode lc = (iONode)ListOp.get( list, i );
    if( lc != NULL ) {
      LocoWidget* l_LocoWidget = new LocoWidget(this, lc, m_WidgetWidth);
      m_GridSizer->Add(l_LocoWidget);
      ListOp.add(m_Widgets, (obj)l_LocoWidget);
    }
  }

  m_GridSizer->Layout();
  FitInside();

}


void WidgetsPanel::updateLoco(iONode node) {
  LocoWidget* l_LocoWidget = (LocoWidget*)FindWindowByName(wxString(wLoc.getid( node ),wxConvUTF8));
  if( l_LocoWidget != NULL ) {
    l_LocoWidget->UpdateLoco(node);
  }

}


void WidgetsPanel::updateLocoImg(iONode node) {
  LocoWidget* l_LocoWidget = (LocoWidget*)FindWindowByName(wxString(wLoc.getid( node ),wxConvUTF8));
  if( l_LocoWidget != NULL ) {
    l_LocoWidget->UpdateLocoImg();
  }
}


