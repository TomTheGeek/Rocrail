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
/* ------------------------------------------------------------
 * libc interfaces.
 */
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>

/* ------------------------------------------------------------
 * rocs interfaces.
 */
#include "svg.h"

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/file.h"
#include "rocs/public/system.h"
#include "rocs/public/cmdln.h"
#include "rocs/public/dir.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/str.h"

static const int __MAXPOINTS = 64;



svgReader::svgReader() {
}


int svgReader::evalCoord( const char* d, int* x, int* y ) {
  char val[32] = {'\0'};
  int hadfirst = 0;
  int validx = 0;
  int idx = 1; /* index 0 = 'M' or 'L' */
  int len = StrOp.len( d );
  int lastwasnum = 0;
  
  for( ;idx < len; idx++ ) {
    if( d[idx] >= '0' && d[idx] <= '9' ) {
      if( !lastwasnum )
        validx = 0;
      val[validx] = d[idx];
      validx++;
      val[validx] = '\0';
      lastwasnum=1;
    }
    else {
      if( lastwasnum ) {
        int coord = atoi(val);
        TraceOp.trc( "svg", TRCLEVEL_DEBUG, __LINE__, 9999, "idx=%d %c=%d", idx, hadfirst?'y':'x', coord );
        lastwasnum=0;
        if(hadfirst) {
          *y = coord;
          break;
        }
        hadfirst=1;
        *x = coord;
      }
    }
  }
  return idx-1; 
}

bool svgReader::parsePoly( const char* d, int xpoints[], int ypoints[], int* cnt ){
  bool arc = false;
  int len = StrOp.len( d );
  int i, x, y;
  *cnt = 0;
  
  for( i = 0; i < len && *cnt < __MAXPOINTS; i++ ) {
    if( d[i] == 'M' ) {
      /* Moveto */
      i += evalCoord( d+i, &x, &y );
      TraceOp.trc( "svg", TRCLEVEL_DEBUG, __LINE__, 9999, "Moveto[%d] %d,%d", *cnt, x, y );
      xpoints[*cnt]=x;
      ypoints[*cnt]=y;
      *cnt = *cnt+1;
    }
    else if( d[i] == 'L' ) {
      /* Lineto */
      i += evalCoord( d+i, &x, &y );
      TraceOp.trc( "svg", TRCLEVEL_DEBUG, __LINE__, 9999, "Lineto[%d] %d,%d", *cnt, x, y );
      xpoints[*cnt]=x;
      ypoints[*cnt]=y;
      *cnt = *cnt+1;
    }
    else if( d[i] == 'C' ) {
      /* arc */
      arc = true;
      i += evalCoord( d+i, &x, &y );
      TraceOp.trc( "svg", TRCLEVEL_DEBUG, __LINE__, 9999, "Arc[%d] %d,%d", *cnt, x, y );
      xpoints[*cnt]=x;
      ypoints[*cnt]=y;
      *cnt = *cnt+1;
      i += evalCoord( d+i, &x, &y );
      TraceOp.trc( "svg", TRCLEVEL_DEBUG, __LINE__, 9999, "Arc[%d] %d,%d", *cnt, x, y );
      xpoints[*cnt]=x;
      ypoints[*cnt]=y;
      *cnt = *cnt+1;
      i += evalCoord( d+i, &x, &y );
      TraceOp.trc( "svg", TRCLEVEL_DEBUG, __LINE__, 9999, "Arc[%d] %d,%d", *cnt, x, y );
      xpoints[*cnt]=x;
      ypoints[*cnt]=y;
      *cnt = *cnt+1;
    }
    else if( d[i] == 'z' ) {
      /* end */
      TraceOp.trc( "svg", TRCLEVEL_DEBUG, __LINE__, 9999, "End of path, %d coordinates", *cnt );
      break;
    }
  }
  return arc;
}


void svgReader::addPoly2List( iOList polyList, int cnt, int xpoints[], int ypoints[], const char* stroke, const char* fill, bool arc ) {
  svgPoly* svgpoly = new svgPoly();
  svgpoly->poly = new wxPoint[cnt];
  svgpoly->cnt = cnt;
  svgpoly->arc = arc;
  int i;
  for( i = 0; i < cnt; i++ ) {
    svgpoly->poly[i].x = xpoints[i];
    svgpoly->poly[i].y = ypoints[i];
  }
  
  if( stroke != NULL )
    svgpoly->stroke = StrOp.dup(stroke);
  else
    svgpoly->stroke = StrOp.dup("black");
  
  if( fill != NULL )
    svgpoly->fill = StrOp.dup(fill);
  else
    svgpoly->fill = StrOp.dup("none");
    
  ListOp.add( polyList, (obj)svgpoly );
}


void svgReader::addCircle2List( iOList circleList, int cx, int cy, int r, const char* stroke, const char* fill ) {
  svgCircle* svgcircle = new svgCircle();
  svgcircle->cx = cx;
  svgcircle->cy = cy;
  svgcircle->r  = r;
  
  if( stroke != NULL )
    svgcircle->stroke = StrOp.dup(stroke);
  else
    svgcircle->stroke = StrOp.dup("black");
  
  if( fill != NULL )
    svgcircle->fill = StrOp.dup(fill);
  else
    svgcircle->fill = StrOp.dup("none");
    
  ListOp.add( circleList, (obj)svgcircle );
}


svgSymbol* svgReader::parseSvgSymbol( const char* svgStr ) {
  iOList polyList = NULL;
  iOList circleList = NULL;
  svgSymbol* sym = new svgSymbol();
  sym->circleList = NULL;
  sym->polyList = NULL;
  
  /*char* vga = "<svg width=\"32\" height=\"32\"><g><path style=\"fill:none;\" d=\"M 10,10 L 20,20L34,54 z\"/></g></svg>";*/
  /* <circle cx="600" cy="200" r="100" fill="red" stroke="blue"/> */
  
  iODoc doc = DocOp.parse( svgStr );
  if( doc == NULL ) {
    TraceOp.trc( NULL, TRCLEVEL_EXCEPTION, __LINE__, 9999, "svg not parsed\n %60.60s", svgStr );
    return NULL;
  }
  else {
    polyList = ListOp.inst();
    circleList = ListOp.inst();
    
    iONode svg = DocOp.getRootNode( doc );
    // clean up
    DocOp.base.del(doc);
    
    sym->width  = NodeOp.getInt( svg, "width", 0 );
    sym->height = NodeOp.getInt( svg, "height", 0 );
    iONode g = NodeOp.findNode( svg, "g" );
    TraceOp.trc( "svg", TRCLEVEL_DEBUG, __LINE__, 9999, "svg size = %d, %d", sym->width, sym->height );
    if( g != NULL ) {
      iONode path = NodeOp.findNode( g, "path" );
      while( path != NULL ) {
        const char* fill = NodeOp.getStr( path, "fill", NULL );
        const char* stroke = NodeOp.getStr( path, "stroke", NULL );
        const char* d = NodeOp.getStr( path, "d", NULL );
        if( d != NULL ) {
          int xpoints[__MAXPOINTS];
          int ypoints[__MAXPOINTS];
          int cnt;
          TraceOp.trc( "svg", TRCLEVEL_DEBUG, __LINE__, 9999, "d=[%s]", d );
          bool arc = parsePoly(d, xpoints, ypoints, &cnt);
          TraceOp.trc( "svg", TRCLEVEL_DEBUG, __LINE__, 9999, "%d wxPoints", cnt );
          addPoly2List( polyList, cnt, xpoints, ypoints, stroke, fill, arc );
        }  
        path = NodeOp.findNextNode( g, path );
      };

      iONode circle = NodeOp.findNode( g, "circle" );
      while( circle != NULL ) {
        const char* fill = NodeOp.getStr( circle, "fill", NULL );
        const char* stroke = NodeOp.getStr( circle, "stroke", NULL );
        addCircle2List( circleList, NodeOp.getInt( circle, "cx", 0 ), NodeOp.getInt( circle, "cy", 0 ), 
                        NodeOp.getInt( circle, "r", 0 ), stroke, fill );
        
        circle = NodeOp.findNextNode( g, circle );
      };


    }
    NodeOp.base.del(svg);
  }
  sym->polyList = polyList;
  sym->circleList = circleList;
  return sym;
}


iOMap svgReader::readSvgSymbols( const char* path, iOMap map ) {
  
  iODir            dir = DirOp.inst( path );
  const char* fileName = NULL;
  iOMap symbolMap = NULL;
  
  if( dir == NULL ) {
    /* Invalid path. */
    return NULL;
  }
 TraceOp.trc( "svg", TRCLEVEL_INFO, __LINE__, 9999, "scanning %s", path );
  
  if( map == NULL )
    symbolMap = MapOp.inst();
  else
    symbolMap = map;
  
  /* Get the first directory entry. */
  fileName = DirOp.read( dir );

  /* Iterate all directory entries. */
  while( fileName != NULL ) {

    /* Check for recover files. */
    if( StrOp.endsWithi( fileName, ".svg" ) ) {
      const char* key = StrOp.strlwr( fileName );
      obj o = MapOp.get( symbolMap, key );
      if( o == NULL ) {
        char* pathfileName = StrOp.fmt( "%s%c%s", path, SystemOp.getFileSeparator(), fileName );
        long size = FileOp.fileSize( pathfileName );
        char* svgStr = (char*)allocMem( size+1 );
        iOFile f = FileOp.inst( pathfileName, OPEN_READONLY );
        TraceOp.trc( "svg", TRCLEVEL_DEBUG, __LINE__, 9999, "reading %s", pathfileName );
        FileOp.read( f, svgStr, size );
        FileOp.base.del( f );
        svgSymbol* sym = parseSvgSymbol( svgStr );
        if(sym != NULL) {
          TraceOp.trc( "svg", TRCLEVEL_INFO, __LINE__, 9999, "add [%s] to symbolMap", key );
          MapOp.put( symbolMap, key, (obj)sym );
        }
      }
      else {
        TraceOp.trc( "svg", TRCLEVEL_INFO, __LINE__, 9999, "symbol [%s] already in symbolMap; skipping", key );
      }
    }
    
    /* Get the next directory entry. */
    fileName = DirOp.read( dir );
  };
  
  return symbolMap;
}


