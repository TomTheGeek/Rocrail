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
/** ------------------------------------------------------------
  * Module: rocgui/symbols
  * Object: renderer
  * ------------------------------------------------------------
  */
#ifndef _SYMBOL_RENDERER_H_
#define _SYMBOL_RENDERER_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

/*!
 * Includes
 */
#include "rocs/public/node.h"
#include "rocs/public/map.h"

#include "svg.h"

class Symbol;

class SymbolRenderer {
  iONode m_Props;
  wxWindow* m_Parent;
  wxPoint* rotateShape( wxPoint* poly, int cnt, const char* oriStr );
  wxPoint* rotateBridge( double ori );
  wxPoint* rotateBridgeSensors( double ori );
  wxPoint* rotateBridgeNose( double ori );
  int m_cx;
  int m_cy;
  bool m_bShowID;
  bool m_bRotateable;
  int m_iOccupied;
  bool m_bLabelChanged;
  const char* m_Ori;
  char* m_Label;
  double m_fText;
  double m_Scale;
  int m_iItemIDps;
  svgSymbol* m_SvgSym1; // straight, red
  svgSymbol* m_SvgSym2; // occupied, thrown, thrownleft, yellow
  svgSymbol* m_SvgSym3; // thrownright, green
  svgSymbol* m_SvgSym4; // thrownright, green
  svgSymbol* m_SvgSym5; // thrownright, green
  svgSymbol* m_SvgSym6; // thrownright, green
  svgSymbol* m_SvgSym7; // white
  svgSymbol* m_SvgSym8; // occupied white
  svgSymbol* m_SvgSym9 ; // route red (4 aspects)
  svgSymbol* m_SvgSym10; // route yellow (4 aspects)
  svgSymbol* m_SvgSym11; // route green (4 aspects)
  svgSymbol* m_SvgSym12; // route white (4 aspects)
  int m_iSymType;
  int m_iSymSubType;
  iOMap m_SymMap;
  wxBitmap* m_Bitmap;

public:
  void initSym();
  SymbolRenderer( iONode props, wxWindow* parent, iOMap symmap, int itemidps );
  void drawShape( wxPaintDC& dc, bool fill, bool occupied, bool actroute, double* bridgepos, bool showID );
  void sizeToScale( double size, double scale, double bktext, int* cx, int* cy );
  void setLabel( const char* label, int occupied );
  int getcx() {return m_cx;}
  int getcy() {return m_cy;}
  bool isRotateable() { return m_bRotateable; }

  void drawSvgSym( wxPaintDC& dc, svgSymbol* svgsym, const char* ori );
  wxPen* getPen( const char* stroke );
  wxBrush* getBrush( const char* fill, wxPaintDC& dc );

  void drawTrack( wxPaintDC& dc, bool fill, bool occupied, bool actroute, const char* ori );
  void drawSwitch( wxPaintDC& dc, bool fill, bool occupied, const char* ori );
  void drawTurnout( wxPaintDC& dc, bool fill, bool occupied, const char* ori );
  void drawCrossing( wxPaintDC& dc, bool fill, bool occupied, const char* ori );
  void drawCCrossing( wxPaintDC& dc, bool fill, bool occupied, const char* ori );
  void drawDCrossing( wxPaintDC& dc, bool fill, bool occupied, const char* ori );
  void drawThreeway( wxPaintDC& dc, bool fill, bool occupied, const char* ori );
  void drawSignal( wxPaintDC& dc, bool fill, bool occupied, bool actroute, const char* ori );
  void drawOutput( wxPaintDC& dc, bool fill, bool occupied, const char* ori );
  void drawSensor( wxPaintDC& dc, bool fill, bool occupied, bool actroute, const char* ori );
  void drawBlock( wxPaintDC& dc, bool fill, bool occupied, const char* ori );
  void drawSelTab( wxPaintDC& dc, bool fill, bool occupied, const char* ori );
  void drawText( wxPaintDC& dc, bool fill, bool occupied, const char* ori );
  void drawTurntable( wxPaintDC& dc, bool fill, bool occupied, double* bridgepos, const char* ori );
  void drawAccessory( wxPaintDC& dc, bool fill, bool occupied, const char* ori );

};

#endif


