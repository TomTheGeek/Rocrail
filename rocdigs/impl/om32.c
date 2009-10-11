/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocdigs/impl/om32_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"

static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOOM32Data data = Data(inst);
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
  iOOM32Data data = Data(inst);
  return data->ini;
}

/** ----- OOM32 ----- */

/** ------------------------------------------------------------
 * csum = 128 | (128 - (sum(all_other_bytes) mod 128))
 */
static int __generateChecksum( byte* datagram ) {
  /* csum = 128 | (128 - (sum(all_other_bytes) mod 128)) */
  int cnt = 3;
  int checksum = 0;
  int i = 0;

  checksum = datagram[0];

  /* add the data bytes: */
  for( i = 0; i < cnt; i++ ) {
    checksum += datagram[i+1];
    datagram[i+1] |= 0x80; /* mark as data byte */
  }
  /* calculate the checksum: */
  checksum = 128 | (128 - (checksum % 128));

  return checksum | 0x80;
}


static void __translate(  iOOM32 inst, iONode node, byte* datagram ) {
  iOOM32Data data = Data(inst);


  /* Output command. */
  if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int  module = wOutput.getaddr( node );
    int  port   = wOutput.getport( node );
    byte param  = 10; /* level */

    if( StrOp.equals( wOutput.on, wOutput.getcmd( node ) ) ) {
      param = 10; /* level */
    }
    else {
      param = 0; /* level */
    }

    datagram[0] = (module << 2) | FIXED_FLAG;
    datagram[1] = 0x05;
    datagram[2] = port;
    datagram[3] = param;
    datagram[4] = (byte)__generateChecksum( datagram );
  }

  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    int  module = wSignal.getaddr( node );
    int  port   = wSignal.getport1( node );
    byte param  = 0x00; /* mask */

    if( StrOp.equals( wSignal.green, wSignal.getcmd( node ) ) )
      param = 0x01;
    else if( StrOp.equals( wSignal.yellow, wSignal.getcmd( node ) ) )
      param = 0x02;
    else
      param = 0x04;


    datagram[0] = (module << 2) | FIXED_FLAG;
    datagram[1] = 0x1D;
    datagram[2] = port;
    datagram[3] = param;
    datagram[4] = (byte)__generateChecksum( datagram );
  }

}


static void __sendToOM32( iOOM32 inst, iONode cmd ) {
  iOOM32Data data = Data(inst);
  byte datagram[32];
  __translate( inst, cmd, datagram );
  SerialOp.write( data->serial, (char*)datagram, 5 );

}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOOM32Data data = Data(inst);
  __sendToOM32( (iOOM32)inst, cmd );
  return 0;
}


/**  */
static void _halt( obj inst ) {
  iOOM32Data data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "halt command received" );
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOOM32Data data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _supportPT( obj inst ) {
  return False;
}


/* Status */
static int _state( obj inst ) {
  iOOM32Data data = Data(inst);
  int state = 0;
  return state;
}

/* external shortcut event */
static void _shortcut(obj inst) {
  iOOM32Data data = Data( inst );
}


/* VERSION: */
static int vmajor = 1;
static int vminor = 4;
static int patch  = 0;
static int _version( obj inst ) {
  iOOM32Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

/**  */
static struct OOM32* _inst( const iONode ini ,const iOTrace trc ) {
  iOOM32 __OM32 = allocMem( sizeof( struct OOM32 ) );
  iOOM32Data data = allocMem( sizeof( struct OOM32Data ) );
  MemOp.basecpy( __OM32, &OM32Op, 0, sizeof( struct OOM32 ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini = ini;
  data->iid = StrOp.dup( wDigInt.getiid( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "om32 %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Init serial %s", wDigInt.getdevice( ini ) );

  data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );

  SerialOp.setFlow( data->serial, -1 );
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, odd );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), 200 );


  if( SerialOp.open( data->serial ) ) {
    SystemOp.inst();

  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init om32 port! (%)", wDigInt.getdevice( ini ) );
  }

  instCnt++;
  return __OM32;
}

/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/om32.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
