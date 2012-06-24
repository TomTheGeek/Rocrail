/*
 Rocs - OS independent C library

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rocs/public/wutils.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/mem.h"
#include "rocs/public/trace.h"

/*
 ***** _Public functions.
 */

static Boolean __checkAttrRangeLong( const char* range, long val ) {
  Boolean ok = True;

  if( range[0] == '*' ) {
    /* everything is allowed */
    ok = True;
  }
  else if( strchr( range, '-' ) ) {
    /* range */
    iOStrTok tok = StrTokOp.inst( range, '-' );
    const char* start = StrTokOp.nextToken( tok );
    const char*   end = StrTokOp.nextToken( tok );
    if( val < atol( start ) )
      ok = False;
    else if( !StrOp.equals( "*", end ) ) {
      if( val > atol( end ) )
        ok = False;
    }
    StrTokOp.base.del( tok );
  }
  else if( strchr( range, ',' ) ) {
    /* enum */
    iOStrTok tok = StrTokOp.inst( range, '-' );
    ok = False;
    while( StrTokOp.hasMoreTokens( tok ) ) {
      const char* enumval = StrTokOp.nextToken( tok );
      if( val == atol( enumval ) ) {
        ok = True;
        break;
      }
    };
    StrTokOp.base.del( tok );
  }
  else {
    /* unknown range */
    TraceOp.trc( "param", TRCLEVEL_WARNING, __LINE__, 9999, "Range [%s] is in an unknown format! Using [*] as default.", range );
    return True;
  }

  return ok;
}


static Boolean __checkAttrRangeFloat( const char* range, double val ) {
  Boolean ok = True;

  if( range[0] == '*' ) {
    /* everything is allowed */
    ok = True;
  }
  else if( strchr( range, '-' ) ) {
    /* range */
    iOStrTok tok = StrTokOp.inst( range, '-' );
    const char* start = StrTokOp.nextToken( tok );
    const char*   end = StrTokOp.nextToken( tok );
    if( val < atof( start ) )
      ok = False;
    else if( !StrOp.equals( "*", end ) ) {
      if( val > atof( end ) )
        ok = False;
    }
    StrTokOp.base.del( tok );
  }
  else if( strchr( range, ',' ) ) {
    /* enum */
    iOStrTok tok = StrTokOp.inst( range, '-' );
    ok = False;
    while( StrTokOp.hasMoreTokens( tok ) ) {
      const char* enumval = StrTokOp.nextToken( tok );
      if( val == atof( enumval ) ) {
        ok = True;
        break;
      }
    };
    StrTokOp.base.del( tok );
  }
  else {
    /* unknown range */
    TraceOp.trc( "param", TRCLEVEL_WARNING, __LINE__, 9999, "Range [%s] is in an unknown format!", range );
    return False;
  }

  return ok;
}


static Boolean __checkAttrRangeStr( const char* range, const char* val ) {
  Boolean ok = True;

  if( range[0] == '*' ) {
    /* everything is allowed */
    ok = True;
  }
  else if( strchr( range, '-' ) ) {
    /* range */
    iOStrTok tok = StrTokOp.inst( range, '-' );
    const char* start = StrTokOp.nextToken( tok );
    const char*   end = StrTokOp.nextToken( tok );
    if( strcmp(val,start) < 0 )
      ok = False;
    else if( !StrOp.equals( "*", end ) ) {
      if( strcmp(val,end) > 0 )
        ok = False;
    }
    StrTokOp.base.del( tok );
  }
  else if( strchr( range, ',' ) ) {
    /* enum */
    iOStrTok tok = StrTokOp.inst( range, ',' );
    ok = False;
    while( StrTokOp.hasMoreTokens( tok ) ) {
      const char* enumval = StrTokOp.nextToken( tok );
      if( enumval[0] == '*' || StrOp.equalsi( val, enumval ) ) {
        ok = True;
        break;
      }
    };
    StrTokOp.base.del( tok );
  }
  else {
    /* unknown range */
    TraceOp.trc( "param", TRCLEVEL_WARNING, __LINE__, 9999, "Range [%s] is in an unknown format!", range );
    return False;
  }

  return ok;
}


static Boolean _checkAttrRange( const char* nodeName, const char* attrname, const char* vartype, const char* range, const char* valstr ) {
  Boolean ok = True;

  if( StrOp.equalsi( "int", vartype ) ) {
    int val = atoi(valstr);
    if( ! ( ok = __checkAttrRangeLong(range,(long)val) ) ) {
      TraceOp.trc( "param", TRCLEVEL_EXCEPTION, __LINE__, 9999, "Attribute %s.%s is out of range! %d -> [%s]", nodeName, attrname, val, range );
    }
  }
  else if( StrOp.equalsi( "long", vartype ) ) {
    long val = atol(valstr);
    if( ! ( ok = __checkAttrRangeLong(range,val) ) ) {
      TraceOp.trc( "param", TRCLEVEL_EXCEPTION, __LINE__, 9999, "Attribute %s.%s is out of range! %ld -> [%s]", nodeName, attrname, val, range );
    }
  }
  else if( StrOp.equalsi( "float", vartype ) ) {
    double val = atof(valstr);
    if( ! ( ok = __checkAttrRangeFloat(range,val) ) ) {
      TraceOp.trc( "param", TRCLEVEL_EXCEPTION, __LINE__, 9999, "Attribute %s.%s is out of range! %f -> [%s]", nodeName, attrname, val, range );
    }
  }
  else if( StrOp.equalsi( "bool", vartype ) ) {
    if( !StrOp.equalsi( "true", valstr ) && !StrOp.equalsi( "false", valstr ) ) {
      TraceOp.trc( "param", TRCLEVEL_EXCEPTION, __LINE__, 9999, "Attribute %s.%s is out of range! %s -> [true,false]", nodeName, attrname, valstr );
    }
  }
  else { /* string = default*/
    if( ! ( ok = __checkAttrRangeStr(range,valstr) ) ) {
      TraceOp.trc( "param", TRCLEVEL_EXCEPTION, __LINE__, 9999, "Attribute %s.%s is out of range! %s -> [%s]", nodeName, attrname, valstr, range );
    }
  }
  
  if( ok ) 
    TraceOp.trc( "param", TRCLEVEL_PARAM, __LINE__, 9999, "%s.%s=[%s]", nodeName, attrname, valstr );
  
  return ok;
}



/* global struct */
struct __wUtils wUtils = {
  _checkAttrRange,
};
