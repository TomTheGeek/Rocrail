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

#include "rocs/impl/js_impl.h"

#include "rocs/public/mem.h"

static int instCnt = 0;

/* OS dependent */
int rocs_js_init( iOJSData o, int* devicemap);
Boolean rocs_js_read( iOJSData o, int devnr, int* type, int* number, int* value, unsigned long* msec  );

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOJSData data = Data(inst);
    int i = 0;
    data->run = False;

    /* Cleanup data->xxx members...*/
    for( i = 0; i < 4; i++ )
      ListOp.base.del( data->listeners[i] );

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
  return NULL;
}

/** ----- OHid ----- */




/** Object creator. */
static iOJS _inst( void ) {
  iOJS __JS = allocMem( sizeof( struct OJS ) );
  iOJSData data = allocMem( sizeof( struct OJSData ) );
  MemOp.basecpy( __JS, &JSOp, 0, sizeof( struct OJS ), data );

  /* Initialize data->xxx members... */
  {
    int i = 0;
    for( i = 0; i < 4; i++ )
      data->listeners[i] = ListOp.inst();
  }

  instCnt++;
  return __JS;
}


/** Init JS */
static int _init( iOJS inst, int* devicemap ) {
  if( inst != NULL ) {
    iOJSData data = Data(inst);
    return rocs_js_init( data, devicemap );
  }
  return 0;
}


static void __reader( void* threadinst ) {
  if( threadinst != NULL ) {
    iOThread    th = (iOThread)threadinst;
    iOJS        js = (iOJS)ThreadOp.getParm(th);
    iOJSData  data = Data(js);

    while( data->run ) {
      int i = 0;
      for( i = 0; i < 4 && data->run; i++ ) {
        int type, number, value;
        unsigned long msec;
        Boolean ok = rocs_js_read( data, i, &type, &number, &value, &msec  );
        if( ok && data->run && !(type==0&&number==0&&value==0&&msec==0) ) {
          jsListener listener = (jsListener)ListOp.first( data->listeners[i] );
          while( listener != NULL ) {
            listener( i, type, number, value, msec );
            listener = (jsListener)ListOp.next( data->listeners[i] );
          }
        }
        ThreadOp.sleep( 0 );
      }

      if( data->devcnt == 0 ) {
        int map = 0;
        ThreadOp.sleep( 5000 );
        JSOp.init( js, &map );
      }

      ThreadOp.sleep( 10 );
    };
  }
}


static void _start( iOJS inst ) {
  if( inst != NULL ) {
    iOJSData data = Data(inst);
    if( data->reader == NULL ) {
      data->reader = ThreadOp.inst( "js-reader", __reader, inst );
      data->run = True;
      ThreadOp.start( data->reader );
    }
  }
}


/** Set an JS listener. */
static Boolean _setListener( iOJS inst ,jsListener listener ,int devnr ) {
  if( inst != NULL && devnr < 4 ) {
    iOJSData data = Data(inst);
    ListOp.add( data->listeners[devnr], (obj)listener );
    return True;
  }
  return False;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/js.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
