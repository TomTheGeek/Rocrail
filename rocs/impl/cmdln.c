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

#include "rocs/impl/cmdln_impl.h"
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
  iOCmdLnData data = Data(inst);
  return StrOp.dup( data->argStr );
}


/* ------------------------------------------------------------
 * base.del()
 */
static void __del(void* inst) {
  iOCmdLnData data = Data(inst);
  data->argMap->base.del(data->argMap);
  freeIDMem( data, RocsCmdLnID );
  freeIDMem( inst, RocsCmdLnID );
  instCnt--;
}


/* ------------------------------------------------------------
 * base.count()
 */
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
 * CmdLnOp.getStr()
 */
static const char* _getStr( iOCmdLn inst, const char* key ) {
  iOCmdLnData data = Data(inst);
  if( data == NULL )
    return NULL;
  else
    return (char*)MapOp.get( data->argMap, key );
}


/* ------------------------------------------------------------
 * CmdLnOp.getStrDef()
 */
static const char* _getStrDef( iOCmdLn inst, const char* key, const char* defVal ) {
  const char* val = _getStr( inst, key );
  return val != NULL ? val:defVal;
}


/* ------------------------------------------------------------
 * CmdLnOp.getIntDef()
 */
static int _getIntDef( iOCmdLn inst, const char* key, int defVal ) {
  const char* val = _getStr( inst, key );
  int ival = defVal;
  if( val != NULL ) {
    ival = atoi( val );
  }
  return ival;
}


/* ------------------------------------------------------------
 * CmdLnOp.hasKey()
 */
static Boolean _hasKey( iOCmdLn inst, const char* key ) {
  iOCmdLnData data = Data(inst);
  if( data == NULL )
    return False;
  return MapOp.get( data->argMap, key ) == NULL ? False:True;
}


/* ------------------------------------------------------------
 * private __mapArgs()
 */
static void __mapArgs( iOCmdLnData data ) {
  int i = 0;
  const char* key = NULL;
  const char* val = NULL;
  static const char* keyval = "key";

  data->argStr = StrOp.fmtID( RocsCmdLnID, "OCmdLn: " );

  for( i = 0; i < data->argc; i++ ) {
    if( data->argv[i][0] == '-' ) {
      /*key*/
      if( key != NULL ) {
        /* key without value */
        char* mapItemStr = StrOp.fmtID( RocsCmdLnID, "key=%s,  ", key );
        data->argStr = StrOp.catID( data->argStr, mapItemStr, RocsCmdLnID );
        StrOp.freeID( mapItemStr, RocsCmdLnID );

        MapOp.put( data->argMap, key, (obj)keyval );
        key = NULL;
      }
      /* Next key. */
      key = data->argv[i];
    }
    else {
      val = data->argv[i];

      if( key != NULL ) {

        char* mapItemStr = StrOp.fmtID( RocsCmdLnID, "key=%s value=%s, ", key, val );
        data->argStr = StrOp.catID( data->argStr, mapItemStr, RocsCmdLnID );
        StrOp.freeID( mapItemStr, RocsCmdLnID );

        MapOp.put( data->argMap, key, (obj)val );
        key = NULL;
        val = NULL;
      }
    }
  }

  /* Is last value a key? */
  if( key != NULL ) {

    char* mapItemStr = StrOp.fmtID( RocsCmdLnID, "key=%s, ", key );
    data->argStr = StrOp.catID( data->argStr, mapItemStr, RocsCmdLnID );
    StrOp.freeID( mapItemStr, RocsCmdLnID );

    /* key without value */
    MapOp.put( data->argMap, key, (obj)keyval );
    key = NULL;
  }

}


/* ------------------------------------------------------------
 * CmdLnOp.inst()
 */
static iOCmdLn _inst( int argc, const char** argv ) {
  iOCmdLn     cmdln = allocIDMem( sizeof( struct OCmdLn ), RocsCmdLnID );
  iOCmdLnData data  = allocIDMem( sizeof( struct OCmdLnData ), RocsCmdLnID );


  MemOp.basecpy( cmdln, &CmdLnOp, 0, sizeof( struct OCmdLn ), data );

  data->argc = argc;
  data->argv = argv;

  data->argMap = MapOp.inst();
  __mapArgs( data );

  instCnt++;

  return cmdln;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/cmdln.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
