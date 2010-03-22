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


#include "rocdigs/impl/massoth_impl.h"

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
    iOMassothData data = Data(inst);
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

/** ----- OMassoth ----- */
static int __addChecksum(byte* out) {
  int len = out[0] >> 5;
  int i = 0;
  byte bXor = out[0];

  for ( i = 0; i < len; i++ ) {
    bXor ^= out[i+2];
  }
  out[1] = bXor;

  return len;
}


static Boolean __readPacket( iOMassothData data, byte* in ) {
  Boolean rc = data->dummyio;

  if( !data->dummyio ) {
    Boolean isInfo = False;
    int insize = 0;
    int offset = 0;
    rc = SerialOp.read( data->serial, in, 1 );
    if( rc ) {

      if( (in[0] & 0xE0) == 0 ) {
        /* info or answer received */
        rc = SerialOp.read( data->serial, in+1, 2 );
        insize = in[2];
        offset = 3;
        isInfo = True;
      }
      else {
        /* command received */
        insize = in[0] >> 5;
        insize++; /* XOR byte */
        offset = 1;
      }

      if( rc ) {
        rc = SerialOp.read( data->serial, in+offset, insize );
        if( rc ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s packet read:", isInfo ? "info":"command" );
          TraceOp.dump( name, TRCLEVEL_INFO, in, insize+offset );
        }
        else {
          /* error reading data */
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "error reading data" );
        }
      }
    }
    else {
      /* error reading header */
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "error reading header" );
    }


  }

  return rc;
}


static Boolean __transact( iOMassothData data, byte* out, byte* in, byte id ) {
  Boolean rc = data->dummyio;

  if( MutexOp.wait( data->mux ) ) {
    int outsize = (out[0] >> 5) + 2;
    int insize  = 0;
    __addChecksum(out);

    TraceOp.dump( name, TRCLEVEL_BYTE, out, outsize );
    if( !data->dummyio ) {
      if( rc = SerialOp.write( data->serial, out, outsize ) ) {
        if( in != NULL ) {
          int wait = 0;
          do {
            if( __readPacket( data, in ) ) {
              if( in[0] == id ) {
                /* id match */
                TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "got wanted id[0x%02X]", id );
                break;
              }
              /* evaluate response */
            }
            wait++;
          } while( wait < 5 );
          if( wait > 4 )
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "wanted id[0x%02X] not seen within 5 reads", id );

        }
      }
    }
    MutexOp.post( data->mux );
  }
  return rc;
}


static int __normalizeSteps(int insteps ) {
  /* SPEEDSTEPS: vaild: 14, 28, 128 */
  if( insteps < 20 )
    return 14;
  if( insteps > 100 )
    return 128;
  return 28;
}


static iOSlot __getSlot(iOMassothData data, iONode node) {
  int steps = wLoc.getspcnt(node);
  int addr  = wLoc.getaddr(node);
  int fncnt = wLoc.getfncnt(node);
  iOSlot slot = NULL;
  byte rsp[32] = {0};
  byte cmd[32] = {0};

  slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot exist for %s", wLoc.getid(node) );
    return slot;
  }
  cmd[0] = 0x64;
  cmd[1] = 0; /*xor*/
  cmd[2] = addr >> 8;
  cmd[3] = addr & 0x00FF;
  cmd[4] = 0x81;

  if( __transact( data, cmd, rsp, 0x40 ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "announcement response for %s", wLoc.getid(node) );
    slot = allocMem( sizeof( struct slot) );
    slot->addr = addr;
    slot->steps = __normalizeSteps(steps);
    slot->id = StrOp.dup(wLoc.getid(node));
    if( MutexOp.wait( data->lcmux ) ) {
      MapOp.put( data->lcmap, wLoc.getid(node), (obj)slot);
      MutexOp.post(data->lcmux);
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for %s", wLoc.getid(node) );
  }

  return slot;
}

static Boolean __getFunState(iONode node) {
  int fnchanged = wFunCmd.getfnchanged(node);

  switch( fnchanged ) {
  case  0: return wFunCmd.isf0(node);
  case  1: return wFunCmd.isf1(node);
  case  2: return wFunCmd.isf2(node);
  case  3: return wFunCmd.isf3(node);
  case  4: return wFunCmd.isf4(node);
  case  5: return wFunCmd.isf5(node);
  case  6: return wFunCmd.isf6(node);
  case  7: return wFunCmd.isf7(node);
  case  8: return wFunCmd.isf8(node);
  case  9: return wFunCmd.isf9(node);
  case 10: return wFunCmd.isf10(node);
  case 11: return wFunCmd.isf11(node);
  case 12: return wFunCmd.isf12(node);
  case 13: return wFunCmd.isf13(node);
  case 14: return wFunCmd.isf14(node);
  case 15: return wFunCmd.isf15(node);
  case 16: return wFunCmd.isf16(node);
  case 17: return wFunCmd.isf17(node);
  case 18: return wFunCmd.isf18(node);
  case 19: return wFunCmd.isf19(node);
  case 20: return wFunCmd.isf20(node);
  case 21: return wFunCmd.isf21(node);
  case 22: return wFunCmd.isf22(node);
  case 23: return wFunCmd.isf23(node);
  case 24: return wFunCmd.isf24(node);
  case 25: return wFunCmd.isf25(node);
  case 26: return wFunCmd.isf26(node);
  case 27: return wFunCmd.isf27(node);
  case 28: return wFunCmd.isf28(node);
  }

  return False;
}


static Boolean __translate( iOMassothData data, iONode node, byte* out ) {
  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      out[0] = 0x11;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      return True;
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      out[0] = 0x10;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      return True;
    }
  }


  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int addr  = wSwitch.getaddr1( node );

    out[0] = 0x4A;
    out[1] = 0; /*xor*/
    out[2] = (addr >> 6);
    out[3] = (addr << 2) & 0xFC;
    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.straight ) )
      out[3] |= 1;
    out[3] |= 0x02; /* port on */

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "switch addr %d, cmd %s", addr, wSwitch.getcmd( node ) );
    return True;
  }

  /* Output command */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int addr   = wOutput.getaddr( node );
    int gate   = wOutput.getgate( node );
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

    out[0] = 0x4A;
    out[1] = 0; /*xor*/
    out[2] = (addr >> 6);
    out[3] = (addr << 2) & 0xFC;
    if( StrOp.equals( wOutput.getcmd( node ), wOutput.on ) )
      out[3] |= 1;
    out[3] |= gate;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "output addr %d, gate %d, cmd %s", addr, gate, wSwitch.getcmd( node ) );
    return True;
  }


  /* Loc command.*/
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int  speed = 0;
    Boolean fn = wLoc.isfn( node );
    int    dir = wLoc.isdir( node );
    int  spcnt = wLoc.getspcnt( node );

    int index = 0;

    iOSlot slot = __getSlot(data, node );

    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get slot for loco %s", wLoc.getid(node) );
      return False;
    }

    spcnt = slot->steps;

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * spcnt) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * spcnt) / wLoc.getV_max( node );
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc=%s addr=%d speed=%d steps=%d lights=%s dir=%s",
        wLoc.getid(node), wLoc.getaddr(node), speed, spcnt, fn?"on":"off", dir?"forwards":"reverse" );

    out[0] = 0x61;
    out[1] = 0; /*xor*/
    out[2] = slot->addr >> 8;
    out[3] = slot->addr & 0x00FF;
    out[4] = speed;
    out[4] |= dir ? 0x80:0x00;

    if( slot->lights != fn ) {
      if( __transact( data, out, NULL, 0 ) ) {
        out[0] = 0x62;
        out[1] = 0; /*xor*/
        out[2] = slot->addr >> 8;
        out[3] = slot->addr & 0x00FF;
        out[4] = fn ? 0x80:0x00;
        slot->lights = fn;
      }
    }

    return True;

  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int fnchanged = wFunCmd.getfnchanged(node);
    int addr      = wFunCmd.getaddr(node);
    Boolean fon   = False;

    iOSlot slot = __getSlot(data, node );

    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get slot for loco %s", wLoc.getid(node) );
      return 0;
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "function %d command for %s", fnchanged, wLoc.getid(node) );

    fon = __getFunState(node);

    out[0] = 0x62;
    out[1] = 0; /*xor*/
    out[2] = slot->addr >> 8;
    out[3] = slot->addr & 0x00FF;
    out[4] = fnchanged;
    out[4] |= fon ? 0x20:0x00;

    return True;

  }


  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "command [%s] not(jet) supported", NodeOp.getName( node ) );


  return False;
}

/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOMassothData data = Data(inst);
  byte out[256];
  iONode reply = NULL;

  if( cmd != NULL ) {
    byte opcode = 0;
    if( __translate( data, cmd, out ) ) {
      if( __transact( data, out, NULL, 0 ) ) {
      }
    }
  }

  /* Cleanup Node1 */
  cmd->base.del(cmd);

  return reply;
}


/**  */
static void _halt( obj inst ) {
  iOMassothData data = Data(inst);
  data->run = False;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "shutting down <%s>...", data->iid );
  if( data->serial != NULL )
    SerialOp.close( data->serial );
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOMassothData data = Data(inst);
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


static void __handleVehicle(iOMassothData data, byte* in) {
  if( in[0] == 0x60 ) {
    int addr = in[3] * 256 + in[4];
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "vehicle %d logged out from device %d", addr, in[5] );
  }
  else if( in[0] == 0x40 && in[2] == 0x08 ) {
    int addr = in[3] * 256 + in[4];
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "vehicle %d not in use", addr );
  }
  else if( in[0] == 0x40 && in[2] == 0x04 ) {
    int addr = in[4] * 256 + in[5];
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "vehicle %d in use by device %d", addr, in[6] );
  }
}


static void __handleSystem(iOMassothData data, byte* in) {
  if( in[2] == 0x01 ) {
    Boolean power = (in[3] & 0x03) == 0x02 ? True:False;

    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wState.setiid( node, data->iid );
    wState.setpower( node, power );
    wState.settrackbus( node, power );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "system status=0x%02X", in[3] );

    if( data->listenerFun != NULL && data->listenerObj != NULL )
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}


static void __handleSensor(iOMassothData data, byte* in) {
  iONode nodeC = NULL;
  Boolean state = in[3] & 0x01 ? True:False;
  int addr = in[2] << 6;
  addr += in[3] >> 2;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sensor report: addr=%d, state=%d", addr, state );
  nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
  wFeedback.setaddr( nodeC, addr );
  wFeedback.setstate( nodeC, state );
  if( data->iid != NULL )
    wFeedback.setiid( nodeC, data->iid );

  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
}


static void __reader( void* threadinst ) {
  iOThread      th      = (iOThread)threadinst;
  iOMassoth     massoth = (iOMassoth)ThreadOp.getParm( th );
  iOMassothData data    = Data(massoth);

  byte out[256];
  data->initialized = False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DiMAX reader started." );
  ThreadOp.sleep( 100 );


  /* sending the interface configuration: must be the first packet */
  out[0] = 0xB8;
  out[1] = 0x00; /* XOR */
  out[2] = 0x00;
  out[3] = 0x00;
  out[4] = 0x00;
  out[5] = 0x00;
  out[6] = 0x0C;

  while( data->run ) {
    if( !data->initialized ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sending interface configuration..." );
      data->initialized = __transact( data, out, NULL, 0 );
      if( !data->initialized ) {
        ThreadOp.sleep( 1000 );
        continue;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "interface configuration successfully send" );
      }
    }

    /* normal reading processing */
    if( MutexOp.wait( data->mux ) ) {

      if( SerialOp.available( data->serial ) ) {
        byte in[256];
        if( __readPacket(data, in) ) {
          switch( in[0] ) {
          case 0x00:
            /* system status */
            __handleSystem(data, in);
            break;
          case 0x40:
          case 0x60:
            /* system status */
            __handleVehicle(data, in);
            break;
          case 0x4B:
            /* sensor report */
            __handleSensor(data, in);
            break;
          default:
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "message 0x%02X not (jet) evaluated", in[0] );
            break;
          }
        }
      }

      MutexOp.post( data->mux );
    }

    ThreadOp.sleep( 10 );
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DiMAX reader ended." );
}



/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 1;
static int vminor = 4;
static int patch  = 99;
static int _version( obj inst ) {
  iOMassothData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**  */
static struct OMassoth* _inst( const iONode ini ,const iOTrace trc ) {
  iOMassoth __Massoth = allocMem( sizeof( struct OMassoth ) );
  iOMassothData data = allocMem( sizeof( struct OMassothData ) );
  MemOp.basecpy( __Massoth, &MassothOp, 0, sizeof( struct OMassoth ), data );

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
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Massoth %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid      = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device   = %s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "baudrate = 57600 (fix)" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serialOK = False;
  if( !data->dummyio ) {
    data->serial = SerialOp.inst( data->device );
    SerialOp.setFlow( data->serial, cts );
    SerialOp.setLine( data->serial, 57600, 8, 1, none, wDigInt.isrtsdisabled( ini ) );
    SerialOp.setTimeout( data->serial, wDigInt.gettimeout(ini), wDigInt.gettimeout(ini) );
    data->serialOK = SerialOp.open( data->serial );
  }

  if(data->serialOK) {
    data->run = True;
    data->reader = ThreadOp.inst( "dimaxreader", &__reader, __Massoth );
    ThreadOp.start( data->reader );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unable to initialize device; switch to dummy mode" );
    data->dummyio = True;
  }

  instCnt++;
  return __Massoth;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/massoth.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
