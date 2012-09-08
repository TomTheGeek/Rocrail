/*
 Rocrail - Model Railroad Software

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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rocdigs/impl/p50_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/State.h"


static int instCnt = 0;

#define P50_POWERON 96
#define P50_POWEROFF 97

static int dir6021[81]; //internal representation of (probable) loco direction stored in 6021.


/*
 ***** OBase functions.
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
  iOP50Data data = Data(inst);
  return (char*)data->iid;
}
static void __del(void* inst) {
  iOP50Data data = Data(inst);
  freeMem( data );
  freeMem( inst );
  instCnt--;
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
static int __count(void) {
  return instCnt;
}



static Boolean __isCTS( iOP50Data o ) {
  int wait4cts = 0;
  while( wait4cts < o->ctsretry ) {
    if( SerialOp.isCTS( o->serial ) ) {
      if( wDigInt.getprotver(o->ini) == 1 ) {
        ThreadOp.sleep(10);
      }
      return True;
    }
    ThreadOp.sleep( 10 );
    wait4cts++;
  };
  return False;
}

/*
 ***** Private functions.
 */
static Boolean __transact( iOP50Data o, char* out, int outsize, char* in, int insize ) {
  if( MutexOp.wait( o->mux ) ) {
    Boolean     rc = False;
    p50state state = P50_OK;

    /* Transact */
    if( __isCTS( o ) ) {
      int i = 0;
      if( outsize > 0 ) {
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "transact write %d:", outsize );
        TraceOp.dump( name, TRCLEVEL_BYTE, (char*)out, outsize );

        for( i = 0; i < outsize && state == P50_OK; i++ ) {
          if( __isCTS(o) ) {
            if( !SerialOp.write( o->serial, out+i, 1 ) )
              state = P50_SNDERR;
          }
          else
            state = P50_CTSERR;
        }
      }

      if( state == P50_OK && insize > 0 ) {
        if( SerialOp.read( o->serial, in, insize ) ) {
          state = P50_OK;
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "transact read %d:", insize );
          TraceOp.dump( name, TRCLEVEL_BYTE, (char*)in, insize );
        }
        else
          state = P50_RCVERR;
      }
    }
    else {
      state = P50_CTSERR;
    }

    /* Release the mutex. */
    MutexOp.post( o->mux );

    if( state != o->state ) {
      /* inform listener: Node3 */
      int errLevel = 0;
      iONode nodeC = NodeOp.inst( wDigInt.name(), NULL, ELEMENT_NODE );
      wResponse.setsender( nodeC, o->device );

      o->state = state;

      switch( state ) {
      case P50_OK:
        wResponse.setmsg( nodeC, "OK." );
        wResponse.setstate( nodeC, wResponse.ok );
        errLevel = TRCLEVEL_INFO;
        break;
      case P50_CTSERR:
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "CTS timeout" );
        wResponse.setmsg( nodeC, "CTS error." );
        wResponse.setstate( nodeC, wResponse.ctserr );
        errLevel = TRCLEVEL_EXCEPTION;
        break;
      case P50_SNDERR:
        wResponse.setmsg( nodeC, "Write error." );
        wResponse.setstate( nodeC, wResponse.snderr );
        errLevel = TRCLEVEL_EXCEPTION;
        break;
      case P50_RCVERR:
        wResponse.setmsg( nodeC, "Read error." );
        wResponse.setstate( nodeC, wResponse.rcverr );
        errLevel = TRCLEVEL_EXCEPTION;
        break;
      }

      o->listenerFun( o->listenerObj, nodeC, errLevel );
    }

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_transact state=%d outsize=%d insize=%d",
                   state, outsize, insize );
    return state == P50_OK ? True:False;
  }
  else return False;
}


/**
 * Fahrstufe = (V * DecoderFahrstufen) / V_max
 * undef V = -1
 * undef V_max = -1
 * p50 only can handle 14 levels: speed = (V * 14) / V_max
 * if( V == -1 || V_max == -1 )
 *   speed = wLoc.getspeed();
 * else
 *   speed = (V * 14) / V_max;

 * @param node <sw unit="1" pin="1" cmd="straight"/>
 * @param node <lc addr="1" speed="0" fn="false" sw="false"/>
 * @param node <fn addr="1" f1="false" f2="false" f3="false" f4="false"/>
 * @param node <fb unit="1"/>
 * @param node <sys cmd="stop"/>
 * @param p50 Request buffer.
 * @param insize Responce size.
 * @return Request size.
 */
static int __translate( iOP50Data o, iONode node, unsigned char* p50, int* insize ) {
  *insize = 0;
  /* Switch command. */
  if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int mod = wSwitch.getaddr1( node );
    int pin = wSwitch.getport1( node );
    int addr = 0;
    int cmd = 33;

    addr = (mod-1) * 4 + pin;

    if( mod < 1 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "invalid addressing: %d:%d", mod, pin);
      return 0;
    }
    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      cmd = 34;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "switch addr=%d cmd=%s", addr, wSwitch.getcmd( node ) );

    p50[0] = (unsigned char)cmd;
    p50[1] = (unsigned char)addr;
    return 2;
  }
  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
    return 0;
  }
  /* TODO: Output */
  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    int   info = 0;
    Boolean fn = wLoc.isfn( node );
    Boolean sw = wLoc.issw( node );
    int    dir = wLoc.isdir( node );

    if (dir6021[addr]!=dir) {
      sw=True;
    }

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * 14) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * 14) / wLoc.getV_max( node );
    }

    info = speed + (fn?16:0);

    if( sw ) {
      /* Set first speed to 0, 3 times to be sure, then switch. */
      info = 0 + (fn?16:0);
      p50[0] = (unsigned char)info;
      p50[1] = (unsigned char)addr;
      info = 15 + (fn?16:0);
      p50[2] = (unsigned char)info;
      p50[3] = (unsigned char)addr;
      info = speed + (fn?16:0);
      p50[4] = (unsigned char)info;
      p50[5] = (unsigned char)addr;
      dir6021[addr]=dir6021[addr] ^ 1; //invert internal representation
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "reversing loco=%d speed=%d, 6021 dir=%s, rocrail dir=%d lights=%s",
		   addr, speed, dir6021[addr]==0?"forward":"reverse",dir, fn?"on":"off");
      return 6;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco addr=%d speed=%d dir=%s lights=%s",
          addr, speed, dir?"forwards":"reverse", fn?"on":"off" );
      p50[0] = (unsigned char)info;
      p50[1] = (unsigned char)addr;
      return 2;
    }
  }
  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );
    Boolean f1 = wFunCmd.isf1( node );
    Boolean f2 = wFunCmd.isf2( node );
    Boolean f3 = wFunCmd.isf3( node );
    Boolean f4 = wFunCmd.isf4( node );
    int   info = 64 + (f1?1:0) + (f2?2:0) + (f3?4:0) + (f4?8:0);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
            "loco function addr=%d f1=%s f2=%s f3=%s f4=%s",
            addr, f1?"on":"off", f2?"on":"off", f3?"on":"off", f4?"on":"off");
    p50[0] = (unsigned char)info;
    p50[1] = (unsigned char)addr;
    return 2;
  }
  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) || StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      p50[0] = (unsigned char)P50_POWEROFF;

      iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
      if( o->iid != NULL )
        wState.setiid( node, o->iid );
      wState.setpower( node, False );
      wState.settrackbus( node, False );
      wState.setaccessorybus( node, False );
      if( o->fbmod > 0 )
        wState.setsensorbus( node, o->readfb );
      o->listenerFun( o->listenerObj, node, TRCLEVEL_INFO );

      return 1;
    }
    if( StrOp.equals( cmd, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      p50[0] = (unsigned char)P50_POWERON;

      iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
      if( o->iid != NULL )
        wState.setiid( node, o->iid );
      wState.setpower( node, True );
      wState.settrackbus( node, True );
      wState.setaccessorybus( node, True );
      if( o->fbmod > 0 )
        wState.setsensorbus( node, o->readfb );
      o->listenerFun( o->listenerObj, node, TRCLEVEL_INFO );

      return 1;
    }
  }

  /* Feedback command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    if( wFeedback.isactivelow(node) )
      wFeedback.setstate( node, !state);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    o->listenerFun( o->listenerObj, (iONode)NodeOp.base.clone( node ), TRCLEVEL_INFO );

  }
  return 0;
}

/*
 ***** Public functions.
 */
static Boolean _setListener( obj inst, obj listenerObj,
                             const digint_listener listenerFun) {
  iOP50Data data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}

static Boolean _setRawListener(obj inst, obj listenerObj, const digint_rawlistener listenerFun ) {
  return True;
}

static byte* _cmdRaw( obj inst, const byte* cmd ) {
  return NULL;
}

static iONode _cmd( obj inst, const iONode nodeA ) {
  iOP50Data o = Data(inst);
  unsigned char out[256];
  unsigned char in [512];
  int insize = 0;
  iONode nodeB = NULL;

  if( nodeA != NULL ) {
    int size = __translate( o, nodeA, out, &insize );
    if( __transact( o, (char*)out, size, (char*)in, insize ) ) {
      /* Inform timer. */
      if( StrOp.equals( NodeOp.getName( nodeA ), wSwitch.name() ) ) {
        o->lastSwCmd = 0;
        ThreadOp.sleep( 100 );
      }
      /* inform listener */
      if( insize > 0 ) {
        char* s = StrOp.byteToStr( in, insize );
        nodeB = NodeOp.inst( NodeOp.getName( nodeA ), NULL, ELEMENT_NODE );
        wResponse.setdata( nodeB, s );
        StrOp.free(s);
      }
    }
    /* Cleanup Node1 */
    nodeA->base.del(nodeA);
  }
  /* return Node2 */
  return nodeB;
}

static void _halt( obj inst, Boolean poweroff ) {
  iOP50Data data = Data(inst);
  unsigned char p50[2];

  data->run = False;
  if( poweroff ) {
    p50[0] = (unsigned char)97;
    __transact( data, (char*)p50, 1, NULL, 0 );
  }
  SerialOp.close( data->serial );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
}

static Boolean _supportPT( obj inst ) {
  iOP50Data data = Data(inst);
  return False;
}

static __evaluateState( iOP50Data o, unsigned char* fb1, unsigned char* fb2, int size ) {
  int i = 0;
  for( i = 0; i < size; i++ ) {
    if( fb1[i] != fb2[i] ) {
      int n = 0;
      int addr = 0;
      int state = 0;
      for( n = 0; n < 8; n++ ) {
        if( (fb1[i] & (0x01 << n)) != (fb2[i] & (0x01 << n)) ) {
          addr = i * 8 + (7-n);

          state = (fb2[i] & (0x01 << n)) ? 1:0;
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "fb2[%d] i=%d, n=%d", i - i%2, i, n );
          TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)&fb2[i-i%2], 2 );
          addr++;
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "fb %d = %d", addr, state );
          {
            /* inform listener: Node3 */
            iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
            wFeedback.setaddr( nodeC, addr );
            wFeedback.setstate( nodeC, state?True:False );
            if( o->iid != NULL )
              wFeedback.setiid( nodeC, o->iid );

            o->listenerFun( o->listenerObj, nodeC, TRCLEVEL_INFO );
          }
        }
      }
    }
  }
}

static void __feedbackReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOP50 p50 = (iOP50)ThreadOp.getParm( th );
  iOP50Data o = Data(p50);
  unsigned char* fb = allocMem(256);

  do {
    unsigned char out[256];
    unsigned char in [512];

    ThreadOp.sleep( o->psleep );
    if( o->fbmod == 0 )
      continue;

    out[0] = (unsigned char)(128 + o->fbmod);
    if( __transact( o, (char*)out, 1, (char*)in, o->fbmod * 2 ) ) {
      if( memcmp( fb, in, o->fbmod * 2 ) != 0 ) {
        /* inform listener */
        __evaluateState( o, fb, in, o->fbmod * 2);
        memcpy( fb, in, o->fbmod * 2 );
      }
    }
  } while( o->run );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback reader ended." );
}

static void __swTimeWatcher( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOP50 p50 = (iOP50)ThreadOp.getParm( th );
  iOP50Data o = Data(p50);
  do {
    ThreadOp.sleep( 10 );
    if( o->lastSwCmd != -1 && o->lastSwCmd >= o->swtime ) {
      unsigned char out[2];
      out[0] = 32;
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                    "swTimeWatcher() END SWITCHTIME %dms", o->lastSwCmd );
      if( __transact( o, (char*)out, 1, NULL, 0 ) ) {
        o->lastSwCmd = -1;
      }
      else
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "swTimeWatcher() Could not send reset byte!" );
    }
    if( o->lastSwCmd != -1 ) {
      o->lastSwCmd += 10;
    }
  } while( o->run );
}

/* Status */
static int _state( obj inst ) {
  iOP50Data data = Data(inst);
  int state = 0;
  return state;
}

/* external shortcut event */
static void _shortcut(obj inst) {
  iOP50Data data = Data( inst );
}


/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOP50Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

static iOP50 _inst( const iONode settings, const iOTrace trace ) {
  iOP50     p50  = allocMem( sizeof( struct OP50 ) );
  iOP50Data data = allocMem( sizeof( struct OP50Data ) );
  iOAttr attr = NULL;
  const char* flow = NULL;
  const char* parity = NULL;

  int i;
  for (i=0; i<81; i++) {
    dir6021[i]=1;
  }

  TraceOp.set( trace );

  /* OBase */
  MemOp.basecpy( p50, &P50Op, 0, sizeof( struct OP50 ), data );

  data->mux = MutexOp.inst( StrOp.fmt( "serialMux%08X", data ), True );

  /* Evaluate attributes. */
  data->ini      = settings;
  data->device   = StrOp.dup( wDigInt.getdevice( settings ) );
  data->iid      = StrOp.dup( wDigInt.getiid( settings ) );

  data->bps      = wDigInt.getbps( settings );
  data->bits     = wDigInt.getbits( settings );
  data->stopBits = wDigInt.getstopbits( settings );
  data->timeout  = wDigInt.gettimeout( settings );
  data->fbmod    = wDigInt.getfbmod( settings );
  data->swtime   = wDigInt.getswtime( settings );
  data->psleep   = wDigInt.getpsleep( settings);
  data->dummyio  = wDigInt.isdummyio( settings );
  data->ctsretry = wDigInt.getctsretry( settings );
  data->readfb   = wDigInt.isreadfb( settings );
  data->run      = True;

  data->serialOK = False;
  data->initOK = False;

  data->lastSwCmd = -1;

  parity      = wDigInt.getparity( settings );
  flow        = wDigInt.getflow( settings );

  if( StrOp.equals( wDigInt.even, parity ) )
    data->parity = even;
  else if( StrOp.equals( wDigInt.odd, parity ) )
    data->parity = odd;
  else if( StrOp.equals( wDigInt.none, parity ) )
    data->parity = none;

  if( StrOp.equals( wDigInt.dsr, flow ) )
    data->flow = dsr;
  else if( StrOp.equals( wDigInt.cts, flow ) )
    data->flow = cts;
  else if( StrOp.equals( wDigInt.xon, flow ) )
    data->flow = xon;
  else
    data->flow = none;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "p50 %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid        = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device     = %s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "baudrate   = %d", data->bps );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bits       = %d", data->bits );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stop bits  = %d", data->stopBits );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "flow       = %s", data->flow );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout    = %d", data->timeout );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ctsretry   = %d", data->ctsretry );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "readfb     = %d", data->readfb );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fbmod      = %d", data->fbmod );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "poll sleep = %d", data->psleep );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "swtime     = %d", data->swtime );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, data->flow );
  SerialOp.setLine( data->serial, data->bps, data->bits, data->stopBits, data->parity, wDigInt.isrtsdisabled( settings ) );
  SerialOp.setTimeout( data->serial, data->timeout, data->timeout );
  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {
    if( data->readfb ) {
      data->feedbackReader = ThreadOp.inst( "feedbackReader", &__feedbackReader, p50 );
      ThreadOp.start( data->feedbackReader );
    }

    data->swTimeWatcher = ThreadOp.inst( "swTimeWatcher", &__swTimeWatcher, p50 );
    ThreadOp.start( data->swTimeWatcher );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init p50 port!" );
  }

  instCnt++;

  return p50;
}

/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
	return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/p50.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
