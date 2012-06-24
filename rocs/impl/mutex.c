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

#include "rocs/impl/mutex_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"

static int instCnt = 0;

/*
 ***** __Private functions.
 */
/* OS dependent */
Boolean rocs_mutex_create( iOMutexData o );
Boolean rocs_mutex_open( iOMutexData o );
Boolean rocs_mutex_release( iOMutexData o );
Boolean rocs_mutex_wait( iOMutexData o, int t );
Boolean rocs_mutex_close( iOMutexData o );


/*
 ***** _objbase functions.
 */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static const char* __name(void) {
  return name;
}
static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}
static void __deserialize(void* inst, unsigned char* a) {
}
static char* __toString(void* inst) {
  iOMutexData data = Data(inst);
  return strcat( "OMutex: ", data->name != NULL ? data->name:"<unnamed>" );
}
static void __del(void* inst) {
  iOMutexData data = Data(inst);
  rocs_mutex_close( data );
  StrOp.freeID( data->name, RocsMutexID );
  freeIDMem( data, RocsMutexID );
  freeIDMem( inst, RocsMutexID );
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


/*
 ***** OMutex public functions.
 */
static Boolean _trywait( iOMutex inst, int t ) {
  iOMutexData data = Data(inst);
  Boolean ok = rocs_mutex_wait( data, t );
  if( !ok )
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, data->rc, "Error on mutex trywait." );
  return ok;
}

static Boolean _wait( iOMutex inst ) {
  if( inst != NULL ) {
    iOMutexData data = Data(inst);
    Boolean ok = rocs_mutex_wait( data, -1 );
    if( !ok )
      TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, data->rc, "Error on mutex wait." );
    return ok;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Mutex not initialized!" );
    return False;
  }
}

static Boolean _post( iOMutex inst ) {
  if( inst != NULL ) {
    iOMutexData data = Data(inst);
    Boolean ok = rocs_mutex_release( data );
    if( !ok )
      TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, data->rc, "Error on mutex post." );
    return ok;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Mutex not initialized!" );
    return False;
  }
}

static int _getRc( iOMutex inst ) {
  iOMutexData data = Data(inst);
  return data->rc;
}

static iOMutex _inst( const char* name, Boolean create ) {
  iOMutex     mutex = allocIDMem( sizeof( struct OMutex ), RocsMutexID );
  iOMutexData data  = allocIDMem( sizeof( struct OMutexData ), RocsMutexID );

  Boolean ok = False;

  MemOp.basecpy( mutex, &MutexOp, 0, sizeof( struct OMutex ), data );

  data->name = StrOp.dupID( name, RocsMutexID );
  if( data->name == NULL ) {
    data->name = StrOp.fmtID( RocsMutexID, "MUX%08X", data );
  }

  if( create )
    ok = rocs_mutex_create( data );
  else
    ok = rocs_mutex_open( data );

  if( !ok ) {
    /* error */
    fprintf( stderr, "Error Mutex: %s rc=%d", data->name, data->rc);
    __del( mutex );
    return NULL;
  }

  instCnt++;

  return mutex;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/mutex.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
