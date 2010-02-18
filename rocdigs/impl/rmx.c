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

#define PCKT          0x7D
/* OPCODES */
#define OPC_OK        0x00
#define OPC_ERR       0x01
#define OPC_MODE      0x03
#define OPC_STATUS    0x04
#define OPC_WRITESX   0x05
#define OPC_READSX    0x06
#define OPC_LOCOINFO  0x08
#define OPC_RMXCHANEL 0x20
#define OPC_LOCOV     0x24
#define OPC_LOCOF     0x28
#define OPC_PT        0xC0

/* ERRCODES */
#define ERR_OK        0x00
#define ERR_UNKNOWN   0x01
#define ERR_NORMXCH   0x02
#define ERR_UNDEFLC   0x03
#define ERR_INPUT     0x04
#define ERR_MODE      0x05
#define ERR_RMXOFF    0x06
#define ERR_LCDBFULL  0x07
#define ERR_ALLUSED   0x08
#define ERR_XOR       0x0F



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

static int __addChecksum(byte* out) {
  int len = out[1];
  int i = 0;
  byte bXor = 0;

  for ( i = 0; i < (len-1); i++ ) {
    bXor ^= out[i];
  }
  out[i] = bXor;

  return len;
}

static Boolean __isChecksum(byte* in) {
  int len = in[1];
  int i = 0;
  byte bXor = 0;

  for ( i = 0; i < (len-1); i++ ) {
    bXor ^= in[i];
  }

  if( in[i] != bXor ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Xor error: in[%d]=0x%02X xor=0x%02X", i, in[i], bXor );
    TraceOp.dump( name, TRCLEVEL_EXCEPTION, in, len );
    return False;
  }

  return True;
}



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

static Boolean __transact( iORmxData data, byte* out, byte* in, byte opcode ) {
  Boolean rc = data->dummyio;

  if( MutexOp.wait( data->mux ) ) {
    int outsize = out[1];
    int insize  = 0;
    __addChecksum(out);

    TraceOp.dump( name, TRCLEVEL_BYTE, out, outsize );
    if( !data->dummyio ) {
      if( rc = SerialOp.write( data->serial, out, outsize ) ) {
        if( in != NULL ) {
          rc = SerialOp.read( data->serial, in, 2 );
          if( rc && in[0] == 0x7D) {
            insize = in[1];
            rc = SerialOp.read( data->serial, in+2, insize - 2 );
            if( rc ) {
              TraceOp.dump( name, TRCLEVEL_BYTE, in, insize );
              rc = __evaluateRsp(data, out, outsize, in, insize);
            }
            else {
              /* error reading data */
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "error reading data" );
            }
          }
          else {
            /* error reading header */
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "error reading header" );
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
      byte out[] = { PCKT,0x05,OPC_OK,ERR_OK,0x00 };
      initialized = __transact(data, out, buffer, OPC_OK );
      if( !initialized ) {
        ThreadOp.sleep( 1000 );
        continue;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RMX connection is initialized." );
      }
    }
    
    if( MutexOp.wait( data->mux ) ) {
      MutexOp.post( data->mux );
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
