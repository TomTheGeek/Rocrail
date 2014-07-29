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

#include "trackpickerdlg.h"
#include "rocview/public/guiapp.h"
#include "rocview/public/cellrenderer.h"
#include "rocview/res/icons.hpp"
#include "rocview/symbols/sym.h"

#include "rocrail/wrapper/public/Track.h"

#include <wx/dnd.h>

TrackPickerDlg::TrackPickerDlg( wxWindow* parent ):TrackPickerDlgGen( parent )
{

  int itemidps = 7;
  int textps = 10;
  m_Renderer = new SymbolRenderer( NULL, this, wxGetApp().getFrame()->getSymMap(), itemidps, textps );

  m_GridTrack->EnableEditing(false);
  m_GridTrack->EnableDragGridSize(false);
  m_GridTrack->ClearGrid();
  m_GridTrack->DeleteRows( 0, m_GridTrack->GetNumberRows() );

  wxFont* font = new wxFont( m_GridTrack->GetDefaultCellFont() );
  font->SetPointSize(1);
  m_GridTrack->SetDefaultCellFont( *font );

  char* symname = NULL;

  m_GridTrack->AppendRows();
  TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d", m_GridTrack->GetNumberRows() );
  symname = StrOp.fmt("%s:%s", wTrack.name(), wTrack.straight );
  m_GridTrack->SetCellValue(m_GridTrack->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
  StrOp.free(symname);
  m_GridTrack->SetCellRenderer(m_GridTrack->GetNumberRows()-1, 0, new CellRenderer(tracktype::straight, m_Renderer) );

  m_GridTrack->AppendRows();
  TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d", m_GridTrack->GetNumberRows() );
  symname = StrOp.fmt("%s:%s", wTrack.name(), wTrack.curve );
  m_GridTrack->SetCellValue(m_GridTrack->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
  StrOp.free(symname);
  m_GridTrack->SetCellRenderer(m_GridTrack->GetNumberRows()-1, 0, new CellRenderer(tracktype::curve, m_Renderer) );

  m_GridTrack->AppendRows();
  TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d", m_GridTrack->GetNumberRows() );
  symname = StrOp.fmt("%s:%s", wTrack.name(), wTrack.buffer );
  m_GridTrack->SetCellValue(m_GridTrack->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
  StrOp.free(symname);
  m_GridTrack->SetCellRenderer(m_GridTrack->GetNumberRows()-1, 0, new CellRenderer(tracktype::buffer, m_Renderer) );

  m_GridTrack->AutoSizeColumns();
  m_GridTrack->AutoSizeRows();
}


void TrackPickerDlg::onTrackCellLeftClick( wxGridEvent& event ) {
  wxString str = ((wxGrid*)event.GetEventObject())->GetCellValue( event.GetRow(), 0 );

  wxString my_text = wxT("addsymbol:")+str;
  wxTextDataObject my_data(my_text);
  wxDropSource dragSource( this );
  dragSource.SetData( my_data );
  wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
}
