/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "rocrail/impl/car_impl.h"

#include "rocrail/public/app.h"

#include "rocs/public/mem.h"

#include "rocrail/wrapper/public/Car.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOCarData data = Data(inst);
    /* Cleanup data->xxx members...*/

    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  return;
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  return;
}

static char* __toString( void* inst ) {
  return NULL;
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  iOCarData data = Data((iOCar)inst);
  return data->props;
}

static const char* __id( void* inst ) {
  iOCarData data = Data((iOCar)inst);
  return wCar.getid(data->props);
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OCar ----- */


/**  */
static char* _getForm( void* object ) {
  return 0;
}


/**  */
static char* _postForm( void* object ,const char* data ) {
  return 0;
}


/**  */
static const char* _tableHdr( void ) {
  return 0;
}


/**  */
static char* _toHtml( void* object ) {
  return 0;
}


/**  */
static struct OCar* _inst( iONode ini ) {
  iOCar __Car = allocMem( sizeof( struct OCar ) );
  iOCarData data = allocMem( sizeof( struct OCarData ) );
  MemOp.basecpy( __Car, &CarOp, 0, sizeof( struct OCar ), data );

  /* Initialize data->xxx members... */
  data->props = ini;

  instCnt++;
  return __Car;
}


/**  */
static void _addWaybill( struct OCar* inst ,iONode waybill ) {
  iOCarData data = Data(inst);
}


/**  */
static const char* _getIdent( struct OCar* inst ) {
  iOCarData data = Data(inst);
  return wCar.getident(data->props);
}


/**  */
static void _setLocality( struct OCar* inst, const char* id ) {
  iOCarData data = Data(inst);
  wCar.setlocation(data->props, id);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "car [%s] arrived in block [%s]", CarOp.base.id(inst), id );
}


/**  */
static void _modify( struct OCar* inst ,iONode props ) {
  iOCarData data = Data(inst);
  int cnt = NodeOp.getAttrCnt( props );
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );

    if( StrOp.equals("id", name) && StrOp.equals( value, wCar.getid(data->props) ) )
      continue; /* skip to avoid making invalid pointers */

    NodeOp.setStr( data->props, name, value );
  }

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( data->props );
    AppOp.broadcastEvent( clone );
  }
  props->base.del(props);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/car.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
