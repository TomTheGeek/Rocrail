/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocdigs/impl/clock_impl.h"

#include "rocs/public/mem.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Clock.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOClockData data = Data(inst);
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
  return NULL;
}

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OClock ----- */


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOClockData data = Data(inst);

  /* called every scale minute */
  if( StrOp.equals( wClock.name(), NodeOp.getName(cmd) ) ) {
    if( StrOp.equals( wClock.getcmd(cmd), wClock.freeze ) ) {
      /* freeze command: not possible to implement */
    }
    else {
      data->tick = !data->tick;
      SerialOp.setDTR(data->serial, data->tick);
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%s...", data->tick?"tick":"tack" );
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "not supported [%s]", NodeOp.getName(cmd) );
  }

  return NULL;
}


/**  */
static void _halt( obj inst ) {
  iOClockData data = Data(inst);
  data->run = False;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down [%s]...", data->iid );
  SerialOp.close( data->serial );
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOClockData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  iOClockData data = Data(inst);
  return 0;
}


/**  */
static Boolean _supportPT( obj inst ) {
  iOClockData data = Data(inst);
  return False;
}


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 0;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOClockData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**  */
static struct OClock* _inst( const iONode ini ,const iOTrace trc ) {
  iOClock __Clock = allocMem( sizeof( struct OClock ) );
  iOClockData data = allocMem( sizeof( struct OClockData ) );
  MemOp.basecpy( __Clock, &ClockOp, 0, sizeof( struct OClock ), data );

  TraceOp.set( trc );
  /* Initialize data->xxx members... */

  /* make a clone of the ini node: it could be replaced by rocrailDialog in the rocgui... */
  data->ini = (iONode)NodeOp.base.clone( ini );

  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->device = StrOp.dup( wDigInt.getdevice( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Clock Driver");
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "clock %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     = [%s]", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = [%s]", data->device );

  data->run = True;

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, none );
  SerialOp.setLine( data->serial, 9600, 8, 1, 0 );
  SerialOp.open( data->serial );

  instCnt++;
  return __Clock;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/clock.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
