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
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Stage.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Output.h"

#include "rocs/public/strtok.h"

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

  m_GridSwitch->EnableEditing(false);
  m_GridSwitch->EnableDragGridSize(false);
  m_GridSwitch->ClearGrid();
  m_GridSwitch->DeleteRows( 0, m_GridSwitch->GetNumberRows() );
  font = new wxFont( m_GridSwitch->GetDefaultCellFont() );
  font->SetPointSize(1);
  m_GridSwitch->SetDefaultCellFont( *font );

  m_GridSignal->EnableEditing(false);
  m_GridSignal->EnableDragGridSize(false);
  m_GridSignal->ClearGrid();
  m_GridSignal->DeleteRows( 0, m_GridSignal->GetNumberRows() );
  font = new wxFont( m_GridSignal->GetDefaultCellFont() );
  font->SetPointSize(1);
  m_GridSignal->SetDefaultCellFont( *font );

  m_GridBlock->EnableEditing(false);
  m_GridBlock->EnableDragGridSize(false);
  m_GridBlock->ClearGrid();
  m_GridBlock->DeleteRows( 0, m_GridBlock->GetNumberRows() );
  font = new wxFont( m_GridBlock->GetDefaultCellFont() );
  font->SetPointSize(1);
  m_GridBlock->SetDefaultCellFont( *font );

  m_GridAccessory->EnableEditing(false);
  m_GridAccessory->EnableDragGridSize(false);
  m_GridAccessory->ClearGrid();
  m_GridAccessory->DeleteRows( 0, m_GridAccessory->GetNumberRows() );
  font = new wxFont( m_GridAccessory->GetDefaultCellFont() );
  font->SetPointSize(1);
  m_GridAccessory->SetDefaultCellFont( *font );

  initSymbols();

  m_TrackBook->SetPageText( 0, wxGetApp().getMsg( "track" ) );
  m_TrackBook->SetPageText( 1, wxGetApp().getMsg( "turnout" ) );
  m_TrackBook->SetPageText( 2, wxGetApp().getMsg( "signal" ) );
  m_TrackBook->SetPageText( 3, wxGetApp().getMsg( "block" ) );

  m_TrackBook->SetSelection(0);

  SetTitle( wxGetApp().getMsg("additem") );
}


void TrackPickerDlg::initGrid() {
  for( int i = 0; i < ListOp.size(m_SymbolList); i++) {
    char* symbol = (char*)ListOp.get(m_SymbolList, i);
    iOStrTok tok = StrTokOp.inst(symbol, ',');
    const char* symname = StrTokOp.nextToken(tok);
    const char* svg     = StrTokOp.nextToken(tok);
    if( StrOp.startsWith( symname, wTrack.name() ) ) {
      m_GridTrack->AppendRows();
      TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridTrack->GetNumberRows(), symname, svg );
      m_GridTrack->SetCellValue(m_GridTrack->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
      m_GridTrack->SetCellRenderer(m_GridTrack->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer) );
    }
    else if( StrOp.startsWith( symname, wSwitch.name() ) ) {
      if( StrOp.find( symname, wSwitch.accessory ) != NULL ) {
        m_GridAccessory->AppendRows();
        TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridAccessory->GetNumberRows(), symname, svg );
        m_GridAccessory->SetCellValue(m_GridAccessory->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
        m_GridAccessory->SetCellRenderer(m_GridAccessory->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer) );
      }
      else {
        m_GridSwitch->AppendRows();
        TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridSwitch->GetNumberRows(), symname, svg );
        m_GridSwitch->SetCellValue(m_GridSwitch->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
        m_GridSwitch->SetCellRenderer(m_GridSwitch->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer) );
      }
    }
    else if( StrOp.startsWith( symname, wSignal.name() ) ) {
      m_GridSignal->AppendRows();
      TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridSignal->GetNumberRows(), symname, svg );
      m_GridSignal->SetCellValue(m_GridSignal->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
      m_GridSignal->SetCellRenderer(m_GridSignal->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer) );
    }
    else if( StrOp.startsWith( symname, wBlock.name() ) || StrOp.startsWith( symname, wStage.name() ) ||
        StrOp.startsWith( symname, wFeedback.name() ) || StrOp.startsWith( symname, wOutput.name() ) )
    {
      m_GridBlock->AppendRows();
      TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridBlock->GetNumberRows(), symname, svg );
      m_GridBlock->SetCellValue(m_GridBlock->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
      m_GridBlock->SetCellRenderer(m_GridBlock->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer) );
    }
    tok->base.del(tok);
  }
  m_GridTrack->AutoSizeColumns();
  m_GridTrack->AutoSizeRows();
}


void TrackPickerDlg::initSymbols() {
  m_SymbolList = ListOp.inst();

  // Track
  char* symname = StrOp.fmt("%s:%s,%s", wTrack.name(), wTrack.straight, tracktype::straight );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s,%s", wTrack.name(), wTrack.curve, tracktype::curve );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s,%s", wTrack.name(), wTrack.buffer, tracktype::buffer );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s,%s", wTrack.name(), wTrack.dir, tracktype::dir );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s,%s", wTrack.name(), wTrack.dirall, tracktype::dirall );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s,%s", wTrack.name(), wTrack.connector, tracktype::connector );
  ListOp.add( m_SymbolList, (obj) symname );

  // Switch
  symname = StrOp.fmt("%s:%s,%s", wSwitch.name(), wSwitch.left, switchtype::turnoutleft );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s,%s", wSwitch.name(), wSwitch.right, switchtype::turnoutright );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s,%s", wSwitch.name(), wSwitch.threeway, switchtype::threeway );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s,%s", wSwitch.name(), wSwitch.twoway, switchtype::twoway_tr );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s,%s", wSwitch.name(), wSwitch.ccrossing, switchtype::ccrossing );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s,%s", wSwitch.name(), wSwitch.crossing, switchtype::crossing );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:left,%s", wSwitch.name(), wSwitch.crossing, switchtype::crossingright );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:right,%s", wSwitch.name(), wSwitch.crossing, switchtype::crossingleft );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:left,%s", wSwitch.name(), wSwitch.dcrossing, switchtype::dcrossingright );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:right,%s", wSwitch.name(), wSwitch.dcrossing, switchtype::dcrossingleft );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s,%s", wSwitch.name(), wSwitch.decoupler, switchtype::decoupler );
  ListOp.add( m_SymbolList, (obj) symname );

  // Signal
  symname = StrOp.fmt("%s:%s:%s,%s", wSignal.name(), wSignal.light, wSignal.distant, signaltype::signaldistant_2_r );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:%s,%s", wSignal.name(), wSignal.light, wSignal.main, signaltype::signalmain_r );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:%s:%s,%s", wSignal.name(), wSignal.light, wSignal.main, "dwarf", signaltype::signalmain_dwarf_r );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:%s,%s", wSignal.name(), wSignal.light, wSignal.shunting, signaltype::signalshunting_2_r );
  ListOp.add( m_SymbolList, (obj) symname );

  // Block
  symname = StrOp.fmt("%s,%s", wBlock.name(), blocktype::block );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s,%s", wStage.name(), stagetype::stage );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s,%s", wFeedback.name(), feedbacktype::sensor_off );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s::%s,%s", wFeedback.name(), "curve", feedbacktype::curve_sensor_off );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s,%s", wOutput.name(), outputtype::button );
  ListOp.add( m_SymbolList, (obj) symname );

  // Accessory
  symname = StrOp.fmt("%s:%s:10,%s", wSwitch.name(), wSwitch.accessory, "accessory-10-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:11,%s", wSwitch.name(), wSwitch.accessory, "accessory-11-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:12,%s", wSwitch.name(), wSwitch.accessory, "accessory-12-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:51,%s", wSwitch.name(), wSwitch.accessory, "accessory-51-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:52,%s", wSwitch.name(), wSwitch.accessory, "accessory-52-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:53,%s", wSwitch.name(), wSwitch.accessory, "accessory-53-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:54,%s", wSwitch.name(), wSwitch.accessory, "accessory-54-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );

  initGrid();

  for( int i = 0; i < ListOp.size(m_SymbolList); i++) {
    StrOp.free((char*)ListOp.get(m_SymbolList, i));
  }
  ListOp.base.del(m_SymbolList);
}

void TrackPickerDlg::onTrackCellLeftClick( wxGridEvent& event ) {
  wxString str = ((wxGrid*)event.GetEventObject())->GetCellValue( event.GetRow(), 0 );

  wxString my_text = wxT("addsymbol:")+str;
  wxTextDataObject my_data(my_text);
  wxDropSource dragSource( this );
  dragSource.SetData( my_data );
  wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
}
