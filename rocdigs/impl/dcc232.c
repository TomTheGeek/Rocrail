/** ------------------------------------------------------------
  * A U T O   G E N E R A T E D  (First time only!)
  * Generator: Rocs ogen (build Jun 11 2009 07:08:17)
  * Module: RocDigs
  * XML: $Source: /cvsroot/rojav/rocdigs/rocdigs.xml,v $
  * XML: $Revision: 1.14 $
  * Object: DCC232
  * Date: Thu Jun 11 11:57:39 2009
  * ------------------------------------------------------------
  * $Source$
  * $Author$
  * $Date$
  * $Revision$
  * $Name$
  */

#include "rocdigs/impl/dcc232_impl.h"

#include "rocdigs/impl/common/nmrapacket.h"

#include "rocs/public/str.h"
#include "rocs/public/mem.h"
#include "rocs/public/system.h"

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

#include "rocdigs/impl/common/fada.h"


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iODCC232Data data = Data(inst);
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

/** ----- ODCC232 ----- */

static int __getLocoSlot(iODCC232 dcc232, iONode node) {
  iODCC232Data data = Data(dcc232);
  int i    = 0;
  int addr = wLoc.getaddr(node);

  /* lookup slot for address: */
  for( i = 0; i < 128; i++ ) {
    if( data->slots[i].addr == addr || data->slots[i].addr == 0 ) {
      return i;
    }
  }
  return -1;
}

void __stateChanged( iODCC232 dcc232 ) {
  iODCC232Data data = Data(dcc232);
  iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
  wState.setiid( node, wDigInt.getiid( data->ini ) );
  wState.setpower( node, data->power );
  wState.setprogramming( node, False );
  wState.settrackbus( node, False );
  wState.setsensorbus( node, False );
  wState.setaccessorybus( node, False );
  if( data->listenerFun != NULL ) {
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}



static iONode __translate( iODCC232 dcc232, iONode node, char* outa ) {
  iODCC232Data data = Data(dcc232);
  iONode rsp = NULL;

  outa[0] = '\0';


  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      data->power = False;
      SerialOp.setDTR(data->serial, False);
      __stateChanged(dcc232);
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      SerialOp.setDTR(data->serial, True);
      SerialOp.setOutputFlow(data->serial, True);
      data->power = True;
      __stateChanged(dcc232);
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {

    int addr = wSwitch.getaddr1( node );
    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );
    int fada = 0;
    int pada = 0;
    int dir  = 1;
    int action = 1;
    int cmdsize = 0;
    byte dcc[12];
    char cmd[32] = {0};

    if( port == 0 ) {
      fada = addr;
      fromFADA( addr, &addr, &port, &gate );
    }
    else if( addr == 0 && port > 0 ) {
      pada = port;
      fromPADA( port, &addr, &port );
    }

    if( fada == 0 )
      fada = toFADA( addr, port, gate );
    if( pada == 0 )
      pada = toPADA( addr, port );

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      dir = 0; /* thrown */

    if( wSwitch.issinglegate( node ) ) {
      dir = gate;
      if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.straight ) )
        action = 0;
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "turnout %04d %d %-10.10s fada=%04d pada=%04d addr=%d port=%d gate=%d dir=%d action=%d",
        addr, port, wSwitch.getcmd( node ), fada, pada, addr, port, gate, dir, action );

    cmdsize = accDecoderPkt2(dcc, fada, action, dir);
    /* TODO */
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "DCC out: %s", outa );
  }
  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {

    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    int fada = 0;
    int pada = 0;
    int cmdsize = 0;
    byte dcc[12];
    char cmd[32] = {0};
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

    if( port == 0 ) {
      fada = addr;
      fromFADA( addr, &addr, &port, &gate );
    }
    else if( addr == 0 && port > 0 ) {
      pada = port;
      fromPADA( port, &addr, &port );
    }

    if( fada == 0 )
      fada = toFADA( addr, port, gate );
    if( pada == 0 )
      pada = toPADA( addr, port );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %04d %d %d fada=%04d pada=%04d",
        addr, port, gate, fada, pada );

    cmdsize = accDecoderPkt(dcc, fada, action);
    /* TODO: */
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "DCC out: %s", outa );
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    Boolean pom = wProgram.ispom( node );
    if( pom ) {
      if( wProgram.getcmd( node ) == wProgram.set ) {
        byte dcc[12];
        char cmd[32] = {0};
        int cmdsize = opsCvWriteByte(dcc, wProgram.getaddr(node), wProgram.islongaddr(node), wProgram.getcv(node), wProgram.getvalue(node) );
        /* TODO: */
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "POM DCC out: %s", outa );
      }
    }
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int slot   = 0;
    int size   = 0;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Loco command for address %d", wLoc.getaddr( node ) );

    slot =  __getLocoSlot( dcc232, node);

    if( slot >= 0 ) {
      int V = 0;
      int steps = wLoc.getspcnt( node );
      Boolean longaddr = StrOp.equals( wLoc.getprot( node ), wLoc.prot_L );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Loco slot=%d", slot );

      if( wLoc.getV( node ) != -1 ) {
        if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
          V = (wLoc.getV( node ) * steps) / 100;
        else if( wLoc.getV_max( node ) > 0 )
          V = (wLoc.getV( node ) * steps) / wLoc.getV_max( node );
      }

      /* keep this value for the ping thread */
      data->slots[slot].dir = wLoc.isdir( node );
      data->slots[slot].V = (V > 127 ? 127:V);
      data->slots[slot].steps = steps;
      data->slots[slot].addr = wLoc.getaddr( node );
      data->slots[slot].longaddr = wLoc.getaddr( node ) > 127 ? True:longaddr;
      data->slots[slot].lights = wLoc.isfn( node );
      data->slots[slot].fn[0]  = wLoc.isfn( node );
      data->slots[slot].changedfgrp = wLoc.isfn( node ) ? 1:-1;
      data->slots[slot].idle = SystemOp.getTick();

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "slot=%d addr=%d V=%d steps=%d dir=%d long=%d", slot,
          data->slots[slot].addr, data->slots[slot].V, data->slots[slot].steps, data->slots[slot].dir, data->slots[slot].longaddr );

      size = 0;
    }
  }

  /* Function */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int addr  = wFunCmd.getaddr( node );
    int group = wFunCmd.getgroup( node );

    int slot =  __getLocoSlot( dcc232, node);

    if( slot >= 0 ) {
      if( data->slots[slot].addr == 0 ) {
        /* first use of this slot */
        data->slots[slot].addr = addr;
        data->slots[slot].longaddr = wLoc.getaddr( node ) > 127 ? True:False;
      }
      data->slots[slot].changedfgrp = group;
      data->slots[slot].lights = wFunCmd.isf0 ( node );
      data->slots[slot].fn[ 0] = wFunCmd.isf0 ( node );
      data->slots[slot].fn[ 1] = wFunCmd.isf1 ( node );
      data->slots[slot].fn[ 2] = wFunCmd.isf2 ( node );
      data->slots[slot].fn[ 3] = wFunCmd.isf3 ( node );
      data->slots[slot].fn[ 4] = wFunCmd.isf4 ( node );
      data->slots[slot].fn[ 5] = wFunCmd.isf5 ( node );
      data->slots[slot].fn[ 6] = wFunCmd.isf6 ( node );
      data->slots[slot].fn[ 7] = wFunCmd.isf7 ( node );
      data->slots[slot].fn[ 8] = wFunCmd.isf8 ( node );
      data->slots[slot].fn[ 9] = wFunCmd.isf9 ( node );
      data->slots[slot].fn[10] = wFunCmd.isf10( node );
      data->slots[slot].fn[11] = wFunCmd.isf11( node );
      data->slots[slot].fn[12] = wFunCmd.isf12( node );
      data->slots[slot].fn[13] = wFunCmd.isf13( node );
      data->slots[slot].fn[14] = wFunCmd.isf14( node );
      data->slots[slot].fn[15] = wFunCmd.isf15( node );
      data->slots[slot].fn[16] = wFunCmd.isf16( node );
      data->slots[slot].fn[17] = wFunCmd.isf17( node );
      data->slots[slot].fn[18] = wFunCmd.isf18( node );
      data->slots[slot].fn[19] = wFunCmd.isf19( node );
      data->slots[slot].fn[20] = wFunCmd.isf20( node );
      data->slots[slot].fn[21] = wFunCmd.isf21( node );
      data->slots[slot].fn[22] = wFunCmd.isf22( node );
      data->slots[slot].fn[23] = wFunCmd.isf23( node );
      data->slots[slot].fn[24] = wFunCmd.isf24( node );
      data->slots[slot].fn[25] = wFunCmd.isf25( node );
      data->slots[slot].fn[26] = wFunCmd.isf26( node );
      data->slots[slot].fn[27] = wFunCmd.isf27( node );
      data->slots[slot].fn[28] = wFunCmd.isf28( node );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "lc=%d function group %d changed for loco %d", group, addr );
    }
  }


  return rsp;

}


/**  */
static iONode _cmd( obj inst ,const iONode nodeA ) {
  iODCC232Data data = Data(inst);

  char outa[100] = {'\0'};

  if( nodeA != NULL ) {
    __translate( (iODCC232)inst, nodeA, outa );
    nodeA->base.del(nodeA);
  }

  return NULL;
}


/**  */
static void _halt( obj inst ) {
  iODCC232Data data = Data(inst);
  data->run = False;
  data->power = False;
  SerialOp.setDTR(data->serial, False);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down [%s]...", data->iid );
  SerialOp.close( data->serial );
  __stateChanged((iODCC232)inst);
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iODCC232Data data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/** external shortcut event */
static void _shortcut( obj inst ) {
  iODCC232Data data = Data(inst);
  data->power = False;
  SerialOp.setDTR(data->serial, False);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "external shortcut event [%s]...", data->iid );
  __stateChanged((iODCC232)inst);
  return;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  iODCC232Data data = Data(inst);
  int state = 0;
  state |= data->power << 0;
  return state;
}


/**  */
static Boolean _supportPT( obj inst ) {
  return False;
}


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 1;
static int vminor = 3;
static int patch  = 99;
static int _version( obj inst ) {
  iODCC232Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static int __bytesToBitStream( byte* bits, byte* dcc, int size ) {
  return createStream(bits, dcc, size);
}



static Boolean __transmit( iODCC232 dcc232, char* dcc, int size ) {
  iODCC232Data data = Data(dcc232);
  Boolean     rc = False;
  byte bitstream[100];
  int bitstreamsize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "transmit size=%d", size );

  bitstreamsize = __bytesToBitStream(bitstream, dcc, size);

  rc = SerialOp.write( data->serial, bitstream, bitstreamsize );
  while( !SerialOp.isUartEmpty(data->serial, True) ) {
    ThreadOp.sleep(0);
  };
  if( !rc ) {
    /* error */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "transmit error=%d (Power Off)", SerialOp.getRc(data->serial) );
    data->power = False;
    SerialOp.setDTR(data->serial, False);
    __stateChanged(dcc232);
  }

  return rc;
}

static void __watchDog( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iODCC232 dcc232 = (iODCC232)ThreadOp.getParm( th );
  iODCC232Data data = Data(dcc232);

  int scdelay = 0;
  Boolean scdetected = False;
  Boolean inversedsr = False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DCC232 watchdog started." );

  while(data->run) {

    ThreadOp.sleep(100);

    if( data->power ) {

      if ( ( SerialOp.isDSR(data->serial) && !inversedsr ) ) {
        TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "shortcut detected" );

        if( scdetected && scdelay > 10 ) {
          TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "shortcut detected!" );
          scdelay = 0;
          scdetected = False;
          data->power = False;
          SerialOp.setDTR(data->serial, False);
          __stateChanged(dcc232);
        }
        else if(!scdetected) {
          TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "shortcut timer started [%dms]", 1000 );
          scdelay++;
          scdetected = True;
        }
        else if(scdetected) {
          scdelay++;
        }
      }
      else {
        scdelay = 0;
        scdetected = False;
      }
    }

  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DCC232 watchdog ended." );

}



static void __dccWriter( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iODCC232 dcc232 = (iODCC232)ThreadOp.getParm( th );
  iODCC232Data data = Data(dcc232);
  int slotidx = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DCC232 writer started." );

  while(data->run) {

    ThreadOp.sleep(10);

    if( data->power ) {
      if( data->slots[slotidx].addr > 0 ) {
        byte dcc[12];
        char cmd[32] = {0};
        char out[64] = {0};
        char in [64] = {0};
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot refresh for %d", data->slots[slotidx].addr );

        if( data->slots[slotidx].V == data->slots[slotidx].V_prev && data->slots[slotidx].changedfgrp == 0 ) {
          if( data->slots[slotidx].idle + 8000 < SystemOp.getTick() ) {
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
                "slot %d purged for loco address %d", slotidx, data->slots[slotidx].addr );
            data->slots[slotidx].addr = 0;
            data->slots[slotidx].idle = 0;
            data->slots[slotidx].fgrp = 0;
            data->slots[slotidx].changedfgrp = 0;
            data->slots[slotidx].V_prev = 0;
            data->slots[slotidx].V = 0;
            slotidx++;
            continue;
          }
        }
        else {
          data->slots[slotidx].V_prev == data->slots[slotidx].V;
          data->slots[slotidx].fgrp = data->slots[slotidx].changedfgrp;
          data->slots[slotidx].changedfgrp = 0;
          data->slots[slotidx].idle = SystemOp.getTick();
        }


        if( data->slots[slotidx].steps == 128 )  {
          int size = speedStep128Packet(dcc, data->slots[slotidx].addr,
              data->slots[slotidx].longaddr, data->slots[slotidx].V, data->slots[slotidx].dir );
          __transmit( dcc232, dcc, size );
        }
        else if( data->slots[slotidx].steps == 28 )  {
          int size = speedStep28Packet(dcc, data->slots[slotidx].addr,
              data->slots[slotidx].longaddr, data->slots[slotidx].V, data->slots[slotidx].dir );
          __transmit( dcc232, dcc, size );
        }
        else {
          int size = speedStep14Packet(dcc, data->slots[slotidx].addr,
              data->slots[slotidx].longaddr, data->slots[slotidx].V,
              data->slots[slotidx].dir, data->slots[slotidx].lights );
          __transmit( dcc232, dcc, size );
        }

        if( data->slots[slotidx].fgrp > 0 ) {
          int size = 0;

          ThreadOp.sleep(10);
          if( data->slots[slotidx].fgrp == 1 ) {
            size = function0Through4Packet(dcc, data->slots[slotidx].addr,
                data->slots[slotidx].longaddr,
                data->slots[slotidx].fn[0],
                data->slots[slotidx].fn[1],
                data->slots[slotidx].fn[2],
                data->slots[slotidx].fn[3],
                data->slots[slotidx].fn[4] );
          }
          else if( data->slots[slotidx].fgrp == 2 ) {
            size = function5Through8Packet(dcc, data->slots[slotidx].addr,
                data->slots[slotidx].longaddr,
                data->slots[slotidx].fn[5],
                data->slots[slotidx].fn[6],
                data->slots[slotidx].fn[7],
                data->slots[slotidx].fn[8] );
          }
          else if( data->slots[slotidx].fgrp == 3 ) {
            size = function9Through12Packet(dcc, data->slots[slotidx].addr,
                data->slots[slotidx].longaddr,
                data->slots[slotidx].fn[9],
                data->slots[slotidx].fn[10],
                data->slots[slotidx].fn[11],
                data->slots[slotidx].fn[12] );
          }
          else if( data->slots[slotidx].fgrp == 4 || data->slots[slotidx].fgrp == 5 ) {
            size = function13Through20Packet(dcc, data->slots[slotidx].addr,
                data->slots[slotidx].longaddr,
                data->slots[slotidx].fn[13],
                data->slots[slotidx].fn[14],
                data->slots[slotidx].fn[15],
                data->slots[slotidx].fn[16],
                data->slots[slotidx].fn[17],
                data->slots[slotidx].fn[18],
                data->slots[slotidx].fn[19],
                data->slots[slotidx].fn[20] );
          }
          else if( data->slots[slotidx].fgrp == 6 || data->slots[slotidx].fgrp == 7 ) {
            size = function21Through28Packet(dcc, data->slots[slotidx].addr,
                data->slots[slotidx].longaddr,
                data->slots[slotidx].fn[21],
                data->slots[slotidx].fn[22],
                data->slots[slotidx].fn[23],
                data->slots[slotidx].fn[24],
                data->slots[slotidx].fn[25],
                data->slots[slotidx].fn[26],
                data->slots[slotidx].fn[27],
                data->slots[slotidx].fn[28] );
          }

          __transmit( dcc232, dcc, size );
        }

        slotidx++;
      }
      else {
        if( slotidx == 0 ) {
          /* send an idle packet */
          byte dcc[3] = {0xFF,0x00,0xFF};
          __transmit( dcc232, dcc, 3 );
        }
        slotidx = 0;
      }
    }
  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DCC232 writer ended." );
}


/**  */
static struct ODCC232* _inst( const iONode ini ,const iOTrace trc ) {
  iODCC232 __DCC232 = allocMem( sizeof( struct ODCC232 ) );
  iODCC232Data data = allocMem( sizeof( struct ODCC232Data ) );
  MemOp.basecpy( __DCC232, &DCC232Op, 0, sizeof( struct ODCC232 ), data );

  /* Initialize data->xxx members... */
  data->mux     = MutexOp.inst( NULL, True );
  data->slotmux = MutexOp.inst( NULL, True );

  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->device = StrOp.dup( wDigInt.getdevice( ini ) );
  data->run    = True;

  MemOp.set( data->slots, 0, 128 * sizeof( struct slot ) );

  SystemOp.inst();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "dcc232 %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     = [%s]", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = [%s]", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout = [%d]ms", wDigInt.gettimeout( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  //SerialOp.setBlocking( data->serial, False );
  SerialOp.setLine( data->serial, 19200, 8, 1, 0 );
  SerialOp.setCTS( data->serial, False); /*Don't use CTS handshake*/
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );
  SerialOp.open( data->serial );

  SerialOp.setOutputFlow(data->serial,False);          /* suspend output */
  SerialOp.setRTS(data->serial,True);  /* +12V for ever on RTS   */
  SerialOp.setDTR(data->serial,False); /* disable booster output */

  data->watchdog = ThreadOp.inst( "watchdog", &__watchDog, __DCC232 );
  ThreadOp.start( data->watchdog );
  data->writer = ThreadOp.inst( "dcc232tx", &__dccWriter, __DCC232 );
  ThreadOp.start( data->writer );


  instCnt++;
  return __DCC232;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/dcc232.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
