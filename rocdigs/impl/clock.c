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
      ThreadOp.post( data->driver, (obj)cmd );
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "not supported [%s]", NodeOp.getName(cmd) );
  }

  return NULL;
}


/**  */
static void _halt( obj inst, Boolean poweroff ) {
  iOClockData data = Data(inst);
  iONode quitNode = NodeOp.inst( "quit", NULL, ELEMENT_NODE );
  data->run = False;
  ThreadOp.post( data->driver, (obj)quitNode );
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


/* external shortcut event */
static void _shortcut(obj inst) {
  iOClockData data = Data( inst );
}


/**  */
static Boolean _supportPT( obj inst ) {
  iOClockData data = Data(inst);
  return False;
}


static void __driverThread( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOClock inst = (iOClock)ThreadOp.getParm( th );
  iOClockData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Clock driver started." );

  while( data->run ) {
    obj post = ThreadOp.getPost( th );
    if( post != NULL ) {
      iONode node = (iONode)post;

      if( StrOp.equals( "quit", NodeOp.getName( node ) ) ) {
        node->base.del( node );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stopping clock driver..." );
        break;
      }

      data->tick = !data->tick;
      SerialOp.setDTR(data->serial, data->tick);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s...", data->tick?"tick":"tack" );
      node->base.del( node );

    }

    ThreadOp.sleep(100);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Clock driver ended." );
}


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 1;
static int vminor = 4;
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
  SerialOp.setLine( data->serial, 9600, 8, 1, 0, wDigInt.isrtsdisabled( ini ) );
  SerialOp.open( data->serial );

  data->driver = ThreadOp.inst( "clckdrv", &__driverThread, __Clock );
  ThreadOp.start( data->driver );

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
