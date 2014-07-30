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

#include "rocview/public/cellrenderer.h"
#include <wx/dc.h>
#include "rocs/public/str.h"
#include "rocs/public/trace.h"
#include "rocs/public/system.h"
#include "rocs/public/strtok.h"

#include "rocview/public/guiapp.h"
#include "rocview/wrapper/public/Gui.h"

#include "rocview/xpm/nopict.xpm"
#include "rocrail/wrapper/public/Item.h"

// m_grid->SetCellRenderer(0, 0, new CellRenderer(cellImage) );

CellRenderer::CellRenderer(const char* imageName) : wxGridCellStringRenderer() {
  this->imageName = StrOp.dup(imageName);
  imageBitmap = NULL;
  m_Renderer = NULL;
  m_bDidResize = false;
  m_Scale = 1.0;
}

CellRenderer::CellRenderer(const char* imageName, SymbolRenderer* l_Renderer, double scale) : wxGridCellStringRenderer() {
  m_Renderer = l_Renderer;
  this->imageName = StrOp.dup(imageName);
  imageBitmap = NULL;
  m_bDidResize = false;
  m_Scale = scale;
}


CellRenderer::CellRenderer(wxBitmap* bitmap) : wxGridCellStringRenderer() {
  this->imageName = StrOp.dup("dummy");
  imageBitmap = bitmap;
  m_Renderer = NULL;
  m_bDidResize = false;
  m_Scale = 1.0;
}

CellRenderer::~CellRenderer() {
  if( imageBitmap != NULL )
    delete imageBitmap;
  StrOp.free(imageName);
}

void CellRenderer::Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected)
{
  wxGridCellStringRenderer::Draw(grid, attr, dc, rect, row, col, false);

  if( m_Renderer != NULL && imageName != NULL && StrOp.len(imageName) > 0 ) {
    int cx = 0;
    int cy = 0;
    //dc.SetUserScale( m_Scale, m_Scale );

    m_Renderer->drawSvgSym( (wxPaintDC&)dc, rect.x, rect.y, imageName, wItem.west, &cx, &cy );
    if( grid.GetColSize(col) <  cx * 32 )
      grid.SetColSize(col, cx * 32 );
    if( grid.GetRowSize(row) <  cy * 32 + 4 )
      grid.SetRowSize(row, cy * 32 + 4 );
    TraceOp.trc( "cellrenderer", TRCLEVEL_INFO, __LINE__, 9999, "image: %s dc=%X row=%d col=%d cx=%d cy=%d", imageName, &dc, row, col, cx, cy );
  }
  else if( imageName != NULL && StrOp.len(imageName) > 0 ) {
    if( imageBitmap == NULL )
      updateImage(rect);
    if( imageBitmap != NULL ) {
      dc.DrawBitmap(*imageBitmap, rect.x, rect.y);
      if( !m_bDidResize ) {
        grid.AutoSizeColumn(col);
        m_bDidResize = true;
      }
    }
  }
}

wxSize CellRenderer::GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, int row, int col) {
  if( imageBitmap != NULL )
    return( wxSize(imageBitmap->GetWidth(), imageBitmap->GetHeight()) );
  else
    return wxGridCellStringRenderer::GetBestSize(grid, attr, dc, row, col);
}


void CellRenderer::updateImage(const wxRect& rect) {
  TraceOp.trc( "cellrenderer", TRCLEVEL_INFO, __LINE__, 9999, "update image" );
  wxBitmapType bmptype = wxBITMAP_TYPE_XPM;
  if( StrOp.endsWithi( imageName, ".gif" ) )
    bmptype = wxBITMAP_TYPE_GIF;
  else if( StrOp.endsWithi( imageName, ".png" ) )
    bmptype = wxBITMAP_TYPE_PNG;

  const char* imagepath = wGui.getimagepath(wxGetApp().getIni());
  static char pixpath[256];
  StrOp.fmtb( pixpath, "%s%c%s", imagepath, SystemOp.getFileSeparator(), FileOp.ripPath( imageName ) );

  if( FileOp.exist(pixpath)) {
    TraceOp.trc( "cellrenderer", TRCLEVEL_INFO, __LINE__, 9999, "picture [%s]", pixpath );
    wxImage img(wxString(pixpath,wxConvUTF8), bmptype);
    if( img.GetHeight() > rect.height ) {
      int h = img.GetHeight();
      int w = img.GetWidth();
      float scale = (float)h / (float)rect.height;
      float width = (float)w / scale;
      imageBitmap = new wxBitmap(img.Scale((int)width, rect.height, wxIMAGE_QUALITY_HIGH));
    }
    else {
      imageBitmap = new wxBitmap(img);
    }
  }
}


