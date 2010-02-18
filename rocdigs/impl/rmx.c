 /*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocdigs/impl/rmx_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"
#include "rocs/public/system.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/FbInfo.h"
#include "rocrail/wrapper/public/FbMods.h"

#include "rocdigs/impl/common/fada.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iORmxData data = Data(inst);
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

/** ----- ORmx ----- */


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iORmxData data = Data(inst);
  /* Cleanup Node1 */
  cmd->base.del(cmd);
  return 0;
}


/**  */
static void _halt( obj inst ) {
  iORmxData data = Data(inst);
  data->run = False;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "shutting down <%s>...", data->iid );
  if( data->serial != NULL )
    SerialOp.close( data->serial );
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iORmxData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "listener set" );
  return True;
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
static int vmajor = 1;
static int vminor = 4;
static int patch  = 99;
static int _version( obj inst ) {
  iORmxData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static Boolean __evaluateRsp( iORmxData data, byte* out, int outsize, byte* in, int insize ) {
  return True;
}

static Boolean __transact( iORmxData data, byte* out, int outsize, byte* in, int insize ) {
  Boolean rc = data->dummyio;

  if( MutexOp.wait( data->mux ) ) {
    TraceOp.dump( name, TRCLEVEL_BYTE, out, outsize );
    if( !data->dummyio ) {
      if( rc = SerialOp.write( data->serial, out, outsize ) ) {
        if( insize > 0 ) {
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "insize=%d", insize);
          rc = SerialOp.read( data->serial, in, insize );
          if( rc ) {
            TraceOp.dump( name, TRCLEVEL_BYTE, in, insize );
            rc = __evaluateRsp(data, out, outsize, in, insize);
          }
        }
      }
    }
    MutexOp.post( data->mux );
  }
  return rc;
}


static void __rmxReader( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iORmx     rmx  = (iORmx)ThreadOp.getParm( th );
  iORmxData data = Data(rmx);
  byte buffer[256];
  Boolean initialized = False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RMX reader started." );
  ThreadOp.sleep( 1000 );
  
  while( data->run ) {
    if( !initialized ) {
      byte out[] = { 0x7d,0x05,0x00,0x00,0x78, 0x00 };
      initialized = __transact(data, out, 5, buffer, 5 );
      if( !initialized ) {
        ThreadOp.sleep( 1000 );
        continue;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RMX connection is initialized." );
      }
    }
    
    ThreadOp.sleep( 100 );
  };
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RMX reader ended." );
}


/**  */
static struct ORmx* _inst( const iONode ini ,const iOTrace trc ) {
  iORmx __Rmx = allocMem( sizeof( struct ORmx ) );
  iORmxData data = allocMem( sizeof( struct ORmxData ) );
  MemOp.basecpy( __Rmx, &RmxOp, 0, sizeof( struct ORmx ), data );

  TraceOp.set( trc );
  SystemOp.inst();

  /* Initialize data->xxx members... */
  data->mux     = MutexOp.inst( NULL, True );
  data->lcmux   = MutexOp.inst( NULL, True );
  data->lcmap   = MapOp.inst();

  data->device   = StrOp.dup( wDigInt.getdevice( ini ) );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );
  data->dummyio  = wDigInt.isdummyio(ini);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RMX %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid      = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device   = %s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "baudrate = 57600 (fix)" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serialOK = False;
  if( !data->dummyio ) {
    data->serial = SerialOp.inst( data->device );
    SerialOp.setFlow( data->serial, none );
    SerialOp.setLine( data->serial, 57600, 8, 2, none, wDigInt.isrtsdisabled( ini ) );
    SerialOp.setTimeout( data->serial, wDigInt.gettimeout(ini), wDigInt.gettimeout(ini) );
    data->serialOK = SerialOp.open( data->serial );
  }

  if(data->serialOK) {
    data->run = True;
    data->sxReader = ThreadOp.inst( "rmxReader", &__rmxReader, __Rmx );
    ThreadOp.start( data->sxReader );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unable to initialize device; switch to dummy mode" );
    data->dummyio = True;
  }


  instCnt++;
  return __Rmx;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/rmx.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
