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
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/Text.h"

#include "rocview/wrapper/public/Gui.h"
#include "rocview/wrapper/public/Window.h"

#include "rocs/public/strtok.h"

#include <wx/dnd.h>
/* ToDo: Check this code in the TrackPickerDlgGen class and disable the last line:
 *
  #ifndef __WXGTK__ // Small icon style not supported in GTK
  wxListView* m_TrackBookListView = m_TrackBook->GetListView();
  long m_TrackBookFlags = m_TrackBookListView->GetWindowStyleFlag();
  m_TrackBookFlags = ( m_TrackBookFlags & ~wxLC_ICON ) | wxLC_SMALL_ICON;
  //m_TrackBookListView->SetWindowStyleFlag( m_TrackBookFlags ); // This will assert on Windows and OSX!
  #endif
 */


TrackPickerDlg::TrackPickerDlg( wxWindow* parent ):TrackPickerDlgGen( parent )
{

  m_GridTrack->EnableEditing(false);
  m_GridTrack->EnableDragGridSize(false);
  m_GridTrack->ClearGrid();
  m_GridTrack->DeleteRows( 0, m_GridTrack->GetNumberRows() );
  m_GridTrack->SetCellTextColour(*wxWHITE);

  int itemidps = 7;
  int textps =  m_GridTrack->GetDefaultCellFont().GetPointSize();
  m_Renderer = new SymbolRenderer( NULL, this, wxGetApp().getFrame()->getSymMap(), itemidps, textps, wxGetApp().getFrame()->getScale() );


  m_GridSwitch->EnableEditing(false);
  m_GridSwitch->EnableDragGridSize(false);
  m_GridSwitch->ClearGrid();
  m_GridSwitch->DeleteRows( 0, m_GridSwitch->GetNumberRows() );
  m_GridSwitch->SetCellTextColour(*wxWHITE);

  m_GridSignal->EnableEditing(false);
  m_GridSignal->EnableDragGridSize(false);
  m_GridSignal->ClearGrid();
  m_GridSignal->DeleteRows( 0, m_GridSignal->GetNumberRows() );
  m_GridSignal->SetCellTextColour(*wxWHITE);

  m_GridBlock->EnableEditing(false);
  m_GridBlock->EnableDragGridSize(false);
  m_GridBlock->ClearGrid();
  m_GridBlock->DeleteRows( 0, m_GridBlock->GetNumberRows() );
  m_GridBlock->SetCellTextColour(*wxWHITE);

  m_GridSensor->EnableEditing(false);
  m_GridSensor->EnableDragGridSize(false);
  m_GridSensor->ClearGrid();
  m_GridSensor->DeleteRows( 0, m_GridSensor->GetNumberRows() );
  m_GridSensor->SetCellTextColour(*wxWHITE);

  m_GridAccessory->EnableEditing(false);
  m_GridAccessory->EnableDragGridSize(false);
  m_GridAccessory->ClearGrid();
  m_GridAccessory->DeleteRows( 0, m_GridAccessory->GetNumberRows() );
  m_GridAccessory->SetCellTextColour(*wxWHITE);

  m_GridRoad->EnableEditing(false);
  m_GridRoad->EnableDragGridSize(false);
  m_GridRoad->ClearGrid();
  m_GridRoad->DeleteRows( 0, m_GridRoad->GetNumberRows() );
  m_GridRoad->SetCellTextColour(*wxWHITE);

  initSymbols();

  m_TrackBook->SetPageText( 0, wxGetApp().getMsg( "track" ) );
  m_TrackBook->SetPageText( 1, wxGetApp().getMsg( "turnout" ) );
  m_TrackBook->SetPageText( 2, wxGetApp().getMsg( "signal" ) );
  m_TrackBook->SetPageText( 3, wxGetApp().getMsg( "block" ) );
  m_TrackBook->SetPageText( 4, wxGetApp().getMsg( "sensor" ) );
  m_TrackBook->SetPageText( 5, wxGetApp().getMsg( "accessory" ) );
  m_TrackBook->SetPageText( 6, wxGetApp().getMsg( "road" ) );
  SetTitle( wxGetApp().getMsg("additem") );

  m_TrackBook->SetSelection(0);
  GetSizer()->Fit(this);
  GetSizer()->SetSizeHints(this);
}


void TrackPickerDlg::initGrid() {
  for( int i = 0; i < ListOp.size(m_SymbolList); i++) {
    char* symbol = (char*)ListOp.get(m_SymbolList, i);
    iOStrTok tok = StrTokOp.inst(symbol, ',');
    const char* symname = StrTokOp.nextToken(tok);
    const char* svg     = StrTokOp.nextToken(tok);
    if( StrOp.find( symname, "road" ) != NULL ) {
      m_GridRoad->AppendRows();
      TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridRoad->GetNumberRows(), symname, svg );
      m_GridRoad->SetCellValue(m_GridRoad->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
      m_GridRoad->SetCellRenderer(m_GridRoad->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer, wxGetApp().getFrame()->getScale()) );
    }
    else if( StrOp.startsWith( symname, wTrack.name() ) ) {
      m_GridTrack->AppendRows();
      TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridTrack->GetNumberRows(), symname, svg );
      m_GridTrack->SetCellValue(m_GridTrack->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
      m_GridTrack->SetCellRenderer(m_GridTrack->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer, wxGetApp().getFrame()->getScale()) );
    }
    else if( StrOp.startsWith( symname, wSwitch.name() ) ) {
      if( StrOp.find( symname, wSwitch.accessory ) != NULL ) {
        m_GridAccessory->AppendRows();
        TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridAccessory->GetNumberRows(), symname, svg );
        m_GridAccessory->SetCellValue(m_GridAccessory->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
        m_GridAccessory->SetCellRenderer(m_GridAccessory->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer, wxGetApp().getFrame()->getScale()) );
      }
      else {
        m_GridSwitch->AppendRows();
        TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridSwitch->GetNumberRows(), symname, svg );
        m_GridSwitch->SetCellValue(m_GridSwitch->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
        m_GridSwitch->SetCellRenderer(m_GridSwitch->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer, wxGetApp().getFrame()->getScale()) );
      }
    }
    else if( StrOp.startsWith( symname, wSignal.name() ) ) {
      m_GridSignal->AppendRows();
      TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridSignal->GetNumberRows(), symname, svg );
      m_GridSignal->SetCellValue(m_GridSignal->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
      m_GridSignal->SetCellRenderer(m_GridSignal->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer, wxGetApp().getFrame()->getScale()) );
    }
    else if( StrOp.startsWith( symname, wOutput.name() ) ) {
      m_GridAccessory->AppendRows();
      TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridAccessory->GetNumberRows(), symname, svg );
      m_GridAccessory->SetCellValue(m_GridAccessory->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
      m_GridAccessory->SetCellRenderer(m_GridAccessory->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer, wxGetApp().getFrame()->getScale()) );
    }
    else if( StrOp.startsWith( symname, wText.name() ) ) {
      m_GridAccessory->AppendRows();
      TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridAccessory->GetNumberRows(), symname, svg );
      m_GridAccessory->SetCellValue(m_GridAccessory->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
      m_GridAccessory->SetCellRenderer(m_GridAccessory->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer, wxGetApp().getFrame()->getScale()) );
    }
    else if( StrOp.startsWith( symname, wBlock.name() ) || StrOp.startsWith( symname, wStage.name() ) ||
        StrOp.startsWith( symname, wSelTab.name() ) || StrOp.startsWith( symname, wTurntable.name() ) )
    {
      m_GridBlock->AppendRows();
      TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridBlock->GetNumberRows(), symname, svg );
      m_GridBlock->SetCellValue(m_GridBlock->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
      m_GridBlock->SetCellRenderer(m_GridBlock->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer, wxGetApp().getFrame()->getScale()) );
    }
    else if( StrOp.startsWith( symname, wFeedback.name() ) )
    {
      m_GridSensor->AppendRows();
      TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "row: %d %s %s", m_GridSensor->GetNumberRows(), symname, svg );
      m_GridSensor->SetCellValue(m_GridSensor->GetNumberRows()-1, 0, wxString(symname,wxConvUTF8) );
      m_GridSensor->SetCellRenderer(m_GridSensor->GetNumberRows()-1, 0, new CellRenderer(svg, m_Renderer, wxGetApp().getFrame()->getScale()) );
    }
    tok->base.del(tok);
  }

  m_GridTrack->AutoSizeColumns();
  m_GridTrack->AutoSizeRows();
  m_GridSwitch->AutoSizeColumns();
  m_GridSwitch->AutoSizeRows();
  m_GridSignal->AutoSizeColumns();
  m_GridSignal->AutoSizeRows();
  m_GridBlock->AutoSizeColumns();
  m_GridBlock->AutoSizeRows();
  m_GridSensor->AutoSizeColumns();
  m_GridSensor->AutoSizeRows();
  m_GridAccessory->AutoSizeColumns();
  m_GridAccessory->AutoSizeRows();
  m_GridRoad->AutoSizeColumns();
  m_GridRoad->AutoSizeRows();

  m_GridTrack->ForceRefresh();
  m_GridSwitch->ForceRefresh();
  m_GridSignal->ForceRefresh();
  m_GridBlock->ForceRefresh();
  m_GridSensor->ForceRefresh();
  m_GridAccessory->ForceRefresh();
  m_GridRoad->ForceRefresh();
}


void TrackPickerDlg::initSymbols() {
  m_SymbolList = ListOp.inst();

  // Track
  char* symname = StrOp.fmt("%s:%s::::,%s", wTrack.name(), wTrack.straight, tracktype::straight );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s::::,%s", wTrack.name(), wTrack.curve, tracktype::curve );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s::::,%s", wTrack.name(), wTrack.buffer, tracktype::buffer );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s::::,%s", wTrack.name(), wTrack.dir, tracktype::dir );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s::::,%s", wTrack.name(), wTrack.dirall, tracktype::dirall );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s::::,%s", wTrack.name(), wTrack.connector, tracktype::connector );
  ListOp.add( m_SymbolList, (obj) symname );

  // Switch
  symname = StrOp.fmt("%s:%s::::,%s", wSwitch.name(), wSwitch.left, switchtype::turnoutleft );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s::::,%s", wSwitch.name(), wSwitch.right, switchtype::turnoutright );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s::::,%s", wSwitch.name(), wSwitch.threeway, switchtype::threeway );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s::::,%s", wSwitch.name(), wSwitch.twoway, switchtype::twoway_tr );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s::::,%s", wSwitch.name(), wSwitch.ccrossing, switchtype::ccrossing );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s::::,%s", wSwitch.name(), wSwitch.crossing, switchtype::crossing );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:left:::,%s", wSwitch.name(), wSwitch.crossing, switchtype::crossingright );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:right:::,%s", wSwitch.name(), wSwitch.crossing, switchtype::crossingleft );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:left:::,%s", wSwitch.name(), wSwitch.dcrossing, switchtype::dcrossingright );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:right:::,%s", wSwitch.name(), wSwitch.dcrossing, switchtype::dcrossingleft );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s::::,%s", wSwitch.name(), wSwitch.decoupler, switchtype::decoupler );
  ListOp.add( m_SymbolList, (obj) symname );

  // Signal
  symname = StrOp.fmt("%s:%s:%s:::,%s", wSignal.name(), wSignal.light, wSignal.distant, signaltype::signaldistant_2_r );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:%s:::,%s", wSignal.name(), wSignal.light, wSignal.main, signaltype::signalmain_r );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:%s:%s::,%s", wSignal.name(), wSignal.light, wSignal.main, "dwarf", signaltype::signalmain_dwarf_r );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:%s:::,%s", wSignal.name(), wSignal.light, wSignal.shunting, signaltype::signalshunting_2_r );
  ListOp.add( m_SymbolList, (obj) symname );

  symname = StrOp.fmt("%s:%s:%s:::,%s", wSignal.name(), wSignal.semaphore, wSignal.distant, signaltype::semaphoredistant_2_r );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:%s:::,%s", wSignal.name(), wSignal.semaphore, wSignal.main, signaltype::semaphoremain_r );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:%s:::,%s", wSignal.name(), wSignal.semaphore, wSignal.shunting, signaltype::semaphoreshunting_2_r );
  ListOp.add( m_SymbolList, (obj) symname );

  // Block
  symname = StrOp.fmt("%s:::::,%s", wBlock.name(), blocktype::block );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:::::,%s", wStage.name(), stagetype::stage );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:::::,%s", wSelTab.name(), seltabtype::seltab );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:::::,%s", wTurntable.name(), turntabletype::turntable );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s::::,%s", wTurntable.name(), "traverser", traversertype::traverser );
  ListOp.add( m_SymbolList, (obj) symname );

  // Sensor
  symname = StrOp.fmt("%s::::::,%s", wFeedback.name(), feedbacktype::sensor_off );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s::%s:::,%s", wFeedback.name(), "curve", feedbacktype::curve_sensor_off );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s::4:::,%s", wFeedback.name(), "accessory-4-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:::::,%s", wOutput.name(), outputtype::button );
  ListOp.add( m_SymbolList, (obj) symname );

  // Accessory
  symname = StrOp.fmt("%s:::::,%s", wText.name(), "text" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:10:::,%s", wSwitch.name(), wSwitch.accessory, "accessory-10-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:11:::,%s", wSwitch.name(), wSwitch.accessory, "accessory-11-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:12:::,%s", wSwitch.name(), wSwitch.accessory, "accessory-12-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:51:::,%s", wSwitch.name(), wSwitch.accessory, "accessory-51-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:52:::,%s", wSwitch.name(), wSwitch.accessory, "accessory-52-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:53:::,%s", wSwitch.name(), wSwitch.accessory, "accessory-53-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:54:::,%s", wSwitch.name(), wSwitch.accessory, "accessory-54-off.svg" );
  ListOp.add( m_SymbolList, (obj) symname );

  // Road
  symname = StrOp.fmt("%s:%s:::road:,%s", wTrack.name(), wTrack.straight, tracktype::road_straight );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:::road:,%s", wTrack.name(), wTrack.curve, tracktype::road_curve );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:::road:,%s", wTrack.name(), wTrack.dir, tracktype::road_dir );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:::road:,%s", wTrack.name(), wTrack.dirall, tracktype::road_dirall );
  ListOp.add( m_SymbolList, (obj) symname );

  symname = StrOp.fmt("%s:%s:::road:,%s", wSwitch.name(), wSwitch.left, switchtype::road_turnoutleft );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:::road:,%s", wSwitch.name(), wSwitch.right, switchtype::road_turnoutright );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:::road:,%s", wSwitch.name(), wSwitch.crossing, switchtype::road_crossing90 );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:::road:,%s", wSwitch.name(), wSwitch.threeway, switchtype::road_rect_threeway );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:::road:,%s", wSwitch.name(), wSwitch.dcrossing, switchtype::road_dcrossingright );
  ListOp.add( m_SymbolList, (obj) symname );

  symname = StrOp.fmt("%s:%s:%s::road:,%s", wSignal.name(), wSignal.light, wSignal.main, signaltype::road_signalmain_r );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s:%s:%s:%s:road:,%s", wSignal.name(), wSignal.light, wSignal.main, "dwarf", signaltype::road_signalmain_dwarf_r );
  ListOp.add( m_SymbolList, (obj) symname );

  symname = StrOp.fmt("%s::::road:,%s", wBlock.name(), blocktype::road_block );
  ListOp.add( m_SymbolList, (obj) symname );
  symname = StrOp.fmt("%s::::road:,%s", wFeedback.name(), feedbacktype::road_sensor_off );
  ListOp.add( m_SymbolList, (obj) symname );


  initGrid();

  for( int i = 0; i < ListOp.size(m_SymbolList); i++) {
    StrOp.free((char*)ListOp.get(m_SymbolList, i));
  }
  ListOp.base.del(m_SymbolList);
}

void TrackPickerDlg::onTrackCellLeftClick( wxGridEvent& event ) {
  wxString str = ((wxGrid*)event.GetEventObject())->GetCellValue( event.GetRow(), 0 );
  ((wxGrid*)event.GetEventObject())->SetGridCursor(event.GetRow(), 0);

  m_Tip->SetValue(str);

  wxString my_text = wxT("addsymbol:") + str;
  TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "drag [%s]", (const char*)my_text.mb_str(wxConvUTF8) );
  wxTextDataObject my_data(my_text);
  wxDropSource dragSource( this );
  dragSource.SetData( my_data );
  wxDragResult result = dragSource.DoDragDrop(wxDrag_CopyOnly);
  Raise();
  event.Skip();
}

void TrackPickerDlg::onPageChanged( wxListbookEvent& event ) {
  int selpage = event.GetSelection();
  TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "selected page %d", selpage );
  wxGrid* grid = NULL;
  switch( selpage ) {
  case 1: grid = m_GridSwitch; break;
  case 2: grid = m_GridSignal; break;
  case 3: grid = m_GridBlock; break;
  case 4: grid = m_GridSensor; break;
  case 5: grid = m_GridAccessory; break;
  case 6: grid = m_GridRoad; break;
  default: grid = m_GridTrack; break;
  }

  for( int i = 0; i < grid->GetNumberRows(); i++ ) {
    CellRenderer* renderer = (CellRenderer*)grid->GetCellRenderer(i,0);
    grid->SetRowSize(i,renderer->GetRowSize());
    TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "rowsize=%d page=%d row=%d", renderer->GetRowSize(), selpage, i );
  }
  grid->ForceRefresh();
  m_Tip->SetValue(wxT(""));
}

void TrackPickerDlg::onClose( wxCloseEvent& event ) {
  wxGetApp().getFrame()->resetTrackPickerRef();
  iONode window = wGui.getwindow(wxGetApp().getFrame()->m_Ini);
  if( window == NULL ) {
    window = NodeOp.inst( wWindow.name(), wGui.getwindow(wxGetApp().getFrame()->m_Ini), ELEMENT_NODE);
    NodeOp.addChild( wGui.getwindow(wxGetApp().getFrame()->m_Ini), window );
  }
  int x,y;
  GetPosition(&x,&y);

  TraceOp.trc( "trackpicker", TRCLEVEL_INFO, __LINE__, 9999, "save position: x=%d, y=%d", x, y );
  wWindow.setxtp(window, x);
  wWindow.setytp(window, y);

  Destroy();
}


