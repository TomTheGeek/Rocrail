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
#ifndef __ROCRAIL_rocview_CELLRENDERER_H
#define __ROCRAIL_rocview_CELLRENDERER_H

#include <wx/grid.h>
#include <wx/image.h>
#include <wx/bitmap.h>
#include "rocview/symbols/renderer.h"


class CellRenderer: public wxGridCellStringRenderer
{
  char* imageName;
  wxBitmap* imageBitmap;
  SymbolRenderer* m_Renderer;
  bool m_bDidResize;
  double m_Scale;
  int m_RowSize;
  char* m_Tip;

public:
  CellRenderer(const char* imageName);
  CellRenderer(const char* imageName, SymbolRenderer* l_Renderer, double scale=1.0, const char* tip=NULL);
  CellRenderer(wxBitmap* bitmap);

  ~CellRenderer();
  virtual void Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected);
  void updateImage(const wxRect& rect);
  wxSize GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, int row, int col);
  int GetRowSize() { return m_RowSize;}
  const char* GetTip() { return m_Tip;}
};

#endif
