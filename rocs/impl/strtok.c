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

#include "rocs/impl/strtok_impl.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"


static int instCnt = 0;

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/* ------------------------------------------------------------
 * base.name()
 */
static const char* __name(void) {
  /* The objectname. */
  /* Conflict */
  return name;
}


/* ------------------------------------------------------------
 * base.serialize()
 */
static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}

/* ------------------------------------------------------------
 * base.deserialize()
 */
static void __deserialize(void* inst, unsigned char* a) {
}


/* ------------------------------------------------------------
 * base.toString()
 */
static char* __toString(void* inst) {
  iOStrTokData data = Data(inst);
  char* str = StrOp.fmt( "%s sep=%s", data->str, data->sep );
  return str;
}


/* ------------------------------------------------------------
 * base.del()
 */
static void __del(void* inst) {
  iOStrTok     strtok = inst;
  iOStrTokData   data = Data(inst);
  
  StrOp.freeID( data->str, RocsStrTokID );
  
  freeIDMem( data  , RocsStrTokID );
  freeIDMem( strtok, RocsStrTokID );
  
  instCnt--;
}
static int __count(void) {
  return instCnt;
}
static void* __properties(void* inst) {
  return NULL;
}
static struct OBase* __clone( void* inst ) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}


/* ------------------------------------------------------------
 * StrTokOp.countTokens()
 */
static int _countTokens( iOStrTok inst ) {
  iOStrTokData data = Data(inst);
  return data->countTokens;
}

static Boolean _hasMoreTokens( iOStrTok inst ) {
  iOStrTokData data = Data(inst);
  return data->nextToken ? True:False;
}

static const char* _nextToken( iOStrTok inst ) {
  iOStrTokData data = Data(inst);
  const char* token = data->nextToken;
  if( token != NULL ) {
    data->nextToken = StrOp.findc( token, data->sep );
    if( data->nextToken ) {
      *data->nextToken = '\0';
      data->nextToken++;
    }
  }
  return token;
}

static void __countTokens( iOStrTok inst ) {
  iOStrTokData data = Data(inst);
  data->countTokens = 0;
  if( data->str != NULL ) {
    const char* s = StrOp.findc( data->str, data->sep );
    data->countTokens = 1;
    while( s ) {
      s = StrOp.findc( s, data->sep );
      if( s ) {
        s++;
        data->countTokens++;
      }
    };
  }
}

/* ------------------------------------------------------------
 * StrTokOp.inst()
 */
static iOStrTok _inst( const char* str, char sep ) {
  iOStrTok     obj  = allocIDMem( sizeof( struct OStrTok     ), RocsStrTokID );
  iOStrTokData data = allocIDMem( sizeof( struct OStrTokData ), RocsStrTokID );

  /* OAttrData */
  data->sep = sep;

  if( str != NULL && StrOp.len(str) > 0 )
    data->str = StrOp.dupID( str, RocsStrTokID );
  
  data->nextToken = data->str;

  /* OBase operations */
  MemOp.basecpy( obj, &StrTokOp, 0, sizeof( struct OStrTok ), data );

  instCnt++;

  __countTokens( obj );
  
  return obj;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/strtok.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
