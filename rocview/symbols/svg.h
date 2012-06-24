/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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
#ifndef SVG_H_
#define SVG_H_

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "rocs/public/list.h"
#include "rocs/public/map.h"

/*!
 * Includes
 */
#include "rocs/public/node.h"

class svgPoly {
  public:
  wxPoint* poly;
  int cnt;
  char* stroke;
  char* fill;
  bool arc;
};

class svgCircle {
  public:
  int cx;
  int cy;
  int r;
  char* stroke;
  char* fill;
};

class svgSymbol {
  public:
  iOList polyList;
  iOList circleList;
  char* name;
  int cx;
  int cy;
  int width;
  int height;
};

class svgReader {
  private:
  void addPoly2List( iOList polyList, int cnt, int xpoints[], int ypoints[], const char* stroke, const char* fill, bool arc );
  void addCircle2List( iOList circleList, int cx, int cy, int r, const char* stroke, const char* fill );
  bool parsePoly( const char* d, int xpoints[], int ypoints[], int* cnt );
  int evalCoord( const char* d, int* x, int* y );

  public:
  svgReader();
  iOMap readSvgSymbols( const char* path, iOMap map, const char* themespath );
  svgSymbol* parseSvgSymbol( const char* svgStr );
};



#endif /*SVG_H_*/
