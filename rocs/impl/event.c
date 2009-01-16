/*
 Rocs - OS independent C library

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
#include <string.h>

#include "rocs/impl/event_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"

static int instCnt = 0;

/*
 ***** __Private functions.
 */
/* OS dependent */
Boolean rocs_event_create( iOEventData data );
Boolean rocs_event_open  ( iOEventData data );
Boolean rocs_event_wait  ( iOEventData data, int t );
Boolean rocs_event_set   ( iOEventData data );
Boolean rocs_event_reset ( iOEventData data );
Boolean rocs_event_close ( iOEventData data );


/*
 ***** OBase operations.
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
  iOEventData data = Data(inst);
  return strcat( "OEvent: ", data->name != NULL ? data->name:"<unnamed>" );
}
static void __del(void* inst) {
  iOEventData data = Data(inst);
  rocs_event_close( data );
  StrOp.freeID( data->name, RocsEventID );
  freeIDMem( data, RocsEventID );
  freeIDMem( inst, RocsEventID );
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
 ***** _OEvent operations.
 */
static Boolean _trywait( iOEvent inst, int t ) {
  return rocs_event_wait( Data(inst), t );
}

static Boolean _wait( iOEvent inst ) {
  return rocs_event_wait( Data(inst), -1 );
}

static Boolean _reset( iOEvent inst ) {
  return rocs_event_reset( Data(inst) );
}

static Boolean _set( iOEvent inst ) {
  return rocs_event_set( Data(inst) );
}

static iOEvent _inst( const char* name, Boolean create ) {
  iOEvent     event = allocIDMem( sizeof( struct OEvent ), RocsEventID );
  iOEventData data  = allocIDMem( sizeof( struct OEventData ), RocsEventID );

  MemOp.basecpy( event, &EventOp, 0, sizeof( struct OEvent ), data );

  data->name = StrOp.dupID( name, RocsEventID );

  if( create )
    rocs_event_create( data );
  else
    rocs_event_open( data );

  instCnt++;

  return event;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/event.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
