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
#include <stdio.h>
#include <string.h>

#include "rocs/impl/msg_impl.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"


static int instCnt = 0;

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
  return NULL;
}

static void __del(void* inst) {
  iOMsgData data = Data(inst);
  freeIDMem( data, RocsMsgID );
  freeIDMem( inst, RocsMsgID );
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
 ***** OMsg operations.
 */
static int _getEvent( iOMsg inst ) {
  if( inst == NULL )
    return 0;
  return Data(inst)->event;
}


static void _setEvent( iOMsg inst, int event ) {
  if( inst == NULL )
    return;
  Data(inst)->event = event;
}


static int _getTimer( iOMsg inst ) {
  if( inst == NULL )
    return 0;
  return Data(inst)->timer;
}


static void _setTimer( iOMsg inst, int timer ) {
  if( inst == NULL )
    return;
  Data(inst)->timer = timer;
}


static obj _getSender( iOMsg inst ) {
  if( inst == NULL )
    return 0;
  return Data(inst)->sender;
}

/* UserData */
static void _setUsrData( iOMsg inst, void* usrdata, usrdatatype type ) {
  iOMsgData data = Data(inst);
  data->usrdata  = usrdata;
  data->type     = type;
}
static void* _getUsrData( iOMsg inst ) {
  iOMsgData data = Data(inst);
  return data->usrdata;
}
static usrdatatype _getUsrDataType( iOMsg inst ) {
  iOMsgData data = Data(inst);
  return data->type;
}


static iOMsg _inst( obj sender, int event ) {
  iOMsg     obj  = allocIDMem( sizeof( struct OMsg     ), RocsMsgID );
  iOMsgData data = allocIDMem( sizeof( struct OMsgData ), RocsMsgID );

  /* OMsgData */
  data->sender = sender;
  data->event  = event;

  /* OBase operations */
  MemOp.basecpy( obj, &MsgOp, 0, sizeof( struct OMsg ), data );

  instCnt++;

  return obj;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/msg.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/

