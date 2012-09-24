/*
 Rocrail - Model Railroad Control System

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include "rocdigs/impl/z21_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOZ21Data data = Data(inst);
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

/** ----- OZ21 ----- */


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  if( cmd != NULL ) {
    NodeOp.base.del(cmd);
  }
  return NULL;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOZ21Data data = Data(inst);
  data->run = False;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOZ21Data data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _setRawListener( obj inst ,obj listenerObj ,const digint_rawlistener listenerRawFun ) {
  return 0;
}


/** external shortcut event */
static void _shortcut( obj inst ) {
  return;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  return 0;
}


/**  */
static Boolean _supportPT( obj inst ) {
  return 0;
}


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;
static int _version( obj inst ) {
  iOZ21Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __reader( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iOZ21     z21  = (iOZ21)ThreadOp.getParm( th );
  iOZ21Data data = Data(z21);


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 UDP reader started." );

  do {
    byte packet[256];
    MemOp.set( packet, 0, 256);

    int packetSize = SocketOp.recvfrom( data->readUDP, packet, 256, NULL, NULL );

    if( packetSize > 0 && packetSize < 256 ) {
      TraceOp.dump ( name, TRCLEVEL_INFO, (char*)packet, packetSize );
      //Boolean rc = SocketOp.sendto( data->writeUDP, packet, packetSize, NULL, 0 );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unexpected packet size %d received", packetSize );
      ThreadOp.sleep(10);
    }

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 UDP reader stopped." );
}


/**  */
static struct OZ21* _inst( const iONode ini ,const iOTrace trc ) {
  iOZ21 __Z21 = allocMem( sizeof( struct OZ21 ) );
  iOZ21Data data = allocMem( sizeof( struct OZ21Data ) );
  MemOp.basecpy( __Z21, &Z21Op, 0, sizeof( struct OZ21 ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  data->ini = ini;
  data->run = True;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "z21 %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "multicast address [%s]", wDigInt.gethost( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "multicast port    [%d]", wDigInt.getport( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->readUDP = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getport(data->ini), False, True, False );
  SocketOp.bind(data->readUDP);
  data->writeUDP = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getport(data->ini), False, True, False );

  data->reader = ThreadOp.inst( "z21reader", &__reader, __Z21 );
  ThreadOp.start( data->reader );

  instCnt++;
  return __Z21;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/z21.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
