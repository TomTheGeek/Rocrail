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

#include "rocdigs/impl/nmra/nmra.h"

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
#include "rocrail/wrapper/public/DCC232.h"

#include "rocutils/public/addr.h"


static int instCnt = 0;

static int __getcvbyte(iODCC232 inst, int cv);
static Boolean __setcvbyte(iODCC232 inst, int cv, int val);

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

static int __getLocoSlot(iODCC232 dcc232, iONode node, Boolean* isNew ) {
  iODCC232Data data = Data(dcc232);
  int i    = 0;
  int addr = wLoc.getaddr(node);

  /* lookup slot for address: */
  for( i = 0; i < 128; i++ ) {
    if( data->slots[i].addr == addr ) {
      *isNew = False;
      return i;
    }
  }
  for( i = 0; i < 128; i++ ) {
    if( data->slots[i].addr == 0 ) {
      *isNew = True;
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
    int packetlen = 0;
    byte dccpacket[64];
    byte* cmd = NULL;

    if( port == 0 ) {
      fada = addr;
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    }
    else if( addr == 0 && port > 0 ) {
      pada = port;
      AddrOp.fromPADA( port, &addr, &port );
    }

    if( fada == 0 )
      fada = AddrOp.toFADA( addr, port, gate );
    if( pada == 0 )
      pada = AddrOp.toPADA( addr, port );

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      dir = 0; /* thrown */

    if( wSwitch.issinglegate( node ) ) {
      dir = gate;
      if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.straight ) )
        action = 0;
    }

    packetlen = compAccessory(dccpacket, addr, port, dir, action);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "turnout %04d %d %-10.10s fada=%04d pada=%04d addr=%d port=%d gate=%d dir=%d action=%d packetlen=%d",
        addr, port, wSwitch.getcmd( node ), fada, pada, addr, port, gate, dir, action, packetlen );
    cmd = allocMem(64);
    cmd[0] = packetlen;
    MemOp.copy(cmd+1, dccpacket, packetlen );
    ThreadOp.post( data->writer, (obj)cmd );

  }
  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {

    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    int fada = 0;
    int pada = 0;
    int packetlen = 0;
    byte dccpacket[64];
    byte* cmd = NULL;
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

    if( port == 0 ) {
      fada = addr;
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    }
    else if( addr == 0 && port > 0 ) {
      pada = port;
      AddrOp.fromPADA( port, &addr, &port );
    }

    if( fada == 0 )
      fada = AddrOp.toFADA( addr, port, gate );
    if( pada == 0 )
      pada = AddrOp.toPADA( addr, port );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %04d %d %d fada=%04d pada=%04d",
        addr, port, gate, fada, pada );

    packetlen = compAccessory(dccpacket, addr, port, gate, action);
    cmd = allocMem(64);
    cmd[0] = packetlen;
    MemOp.copy(cmd+1, dccpacket, packetlen );
    ThreadOp.post( data->writer, (obj)cmd );
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    Boolean pom = wProgram.ispom( node );
    if( pom ) {
      byte dccpacket[64];
      byte* cmd = NULL;
      int packetlen = dccPOM(dccpacket, wProgram.getaddr(node), wProgram.islongaddr(node),
                          wProgram.getcv(node), wProgram.getvalue(node),  wProgram.getcmd( node ) == wProgram.get );
      cmd = allocMem(64);
      cmd[0] = packetlen;
      MemOp.copy(cmd+1, dccpacket, packetlen );
      ThreadOp.post( data->writer, (obj)cmd );
    }
    else {
      if(  wProgram.getcmd( node ) == wProgram.pton ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT ON" );
        data->ptflag = True;
        SerialOp.setRTS(data->serial, True);
      }
      else if(  wProgram.getcmd( node ) == wProgram.ptoff ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT OFF" );
        data->ptflag = False;
        SerialOp.setRTS(data->serial, False);
      }
      else if( wProgram.getcmd( node ) == wProgram.get && data->ptflag ) {
        rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
        if( data->iid != NULL )
          wProgram.setiid( rsp, data->iid );
        wProgram.setcmd( rsp, wProgram.datarsp );
        wProgram.setcv( rsp, wProgram.getcv( node ) );
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CV Read" );
        int value = __getcvbyte( dcc232, wProgram.getcv( node ) - 1);
        wProgram.setvalue( rsp, value );

      }

      else if( wProgram.getcmd( node ) == wProgram.set && data->ptflag ) {
        rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
        if( data->iid != NULL )
          wProgram.setiid( rsp, data->iid );
        wProgram.setcmd( rsp, wProgram.datarsp );
        wProgram.setcv( rsp, wProgram.getcv( node ) );
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CV Write" );
        Boolean ack = __setcvbyte(dcc232, wProgram.getcv( node ) - 1, wProgram.getvalue( node ));

        wProgram.setvalue( rsp, ack ? wProgram.getvalue( node ):-1 );
      }
    }
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int slot   = 0;
    int size   = 0;
    int packetlen = 0;
    byte dccpacket[64];
    byte* cmd = NULL;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Loco command for address %d", wLoc.getaddr( node ) );

    if( MutexOp.trywait( data->slotmux, 100 ) ) {
      Boolean isNew = False;
      slot =  __getLocoSlot( dcc232, node, &isNew );

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

        data->slots[slot].lcstream[0] = compSpeed(data->slots[slot].lcstream+1, data->slots[slot].addr,
                                                  data->slots[slot].longaddr  , data->slots[slot].dir,
                                                  data->slots[slot].V, data->slots[slot].steps);

        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "slot=%d addr=%d V=%d steps=%d dir=%d long=%d", slot,
          data->slots[slot].addr, data->slots[slot].V, data->slots[slot].steps, data->slots[slot].dir, data->slots[slot].longaddr );

        cmd = allocMem(64);
        MemOp.copy(cmd, data->slots[slot].lcstream, data->slots[slot].lcstream[0] + 1 );
        ThreadOp.post( data->writer, (obj)cmd );

        size = 0;
      }

      if( isNew ) {
        /* restore function group 1 for lights; the rocrail server is not informed if this loco was purged... */
        data->slots[slot].changedfgrp = 1;
        data->slots[slot].lights = wLoc.isfn( node );
        data->slots[slot].fn[ 0] = wLoc.isfn( node );
        data->slots[slot].fnstream[0] = compFunction(data->slots[slot].fnstream, data->slots[slot].addr,
                                                     data->slots[slot].longaddr, data->slots[slot].changedfgrp, data->slots[slot].fn);

        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "function group %d changed for loco %d", 1, data->slots[slot].addr );

        cmd = allocMem(64);
        MemOp.copy(cmd, data->slots[slot].fnstream, data->slots[slot].fnstream[0] + 1 );
        ThreadOp.post( data->writer, (obj)cmd );
      }

      /* Release the mutex. */
      MutexOp.post( data->slotmux );

    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get the slot mutex" );
    }
  }

  /* Function */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int addr  = wFunCmd.getaddr( node );
    int group = wFunCmd.getgroup( node );
    Boolean longaddr = StrOp.equals( wLoc.getprot( node ), wLoc.prot_L );
    int packetlen = 0;
    byte dccpacket[64];
    byte* cmd = NULL;

    if( MutexOp.trywait( data->slotmux, 100 ) ) {
      Boolean isNew = False;
      int slot =  __getLocoSlot( dcc232, node, &isNew);

      if( slot >= 0 ) {
        if( data->slots[slot].addr == 0 ) {
          /* first use of this slot */
          data->slots[slot].addr = addr;
          data->slots[slot].longaddr = wLoc.getaddr( node ) > 127 ? True:longaddr;
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

        data->slots[slot].fnstream[0] = compFunction(data->slots[slot].fnstream, data->slots[slot].addr,
                                                     data->slots[slot].longaddr, data->slots[slot].changedfgrp, data->slots[slot].fn);

        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "function group %d changed for loco %d", group, addr );

        cmd = allocMem(64);
        MemOp.copy(cmd, data->slots[slot].fnstream, data->slots[slot].fnstream[0] + 1 );
        ThreadOp.post( data->writer, (obj)cmd );

      }
      /* Release the mutex. */
      MutexOp.post( data->slotmux );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get the slot mutex" );
    }
  }


  return rsp;

}


/**  */
static iONode _cmd( obj inst ,const iONode nodeA ) {
  iODCC232Data data = Data(inst);
  iONode rsp = NULL;

  char outa[100] = {'\0'};

  if( nodeA != NULL ) {
    rsp = __translate( (iODCC232)inst, nodeA, outa );
    nodeA->base.del(nodeA);
  }

  return rsp;
}


/**  */
static void _halt( obj inst, Boolean poweroff ) {
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
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iODCC232Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static Boolean __transmit( iODCC232 dcc232, char* bitstream, int bitstreamsize, Boolean longIdle ) {
  iODCC232Data data = Data(dcc232);
  Boolean     rc = False;
  byte idlestream[100];
  int idlestreamsize = 0;

  idlestreamsize = idlePacket(idlestream, longIdle);

  SerialOp.setSerialMode(data->serial,dcc);

  if( bitstreamsize > 0 ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "transmit size=%d", bitstreamsize );
    rc = SerialOp.write( data->serial, bitstream, bitstreamsize );
    if( rc )
      rc = SerialOp.write( data->serial, idlestream, idlestreamsize );
    if( rc )
      rc = SerialOp.write( data->serial, bitstream, bitstreamsize );
    if( rc )
      rc = SerialOp.write( data->serial, idlestream, idlestreamsize );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "transmit size=%d", idlestreamsize );
    rc = SerialOp.write( data->serial, idlestream, idlestreamsize );
  }


  if( !rc ) {
    /* error */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "transmit error=%d (Power Off)", SerialOp.getRc(data->serial) );
    data->power = False;
    SerialOp.setDTR(data->serial, False);
    __stateChanged(dcc232);
  }
  else {
    int remaining = SerialOp.getWaiting( data->serial );
    if (remaining>2 ) {
      /* If directIO, wait sligthly less than remaining*0,52ms
       * else wait sligthly more than remaining*0,52ms*/
      ThreadOp.sleep(remaining*502/1000-1);
    }

  }

  return rc;
}

static void __watchDog( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iODCC232 dcc232 = (iODCC232)ThreadOp.getParm( th );
  iODCC232Data data = Data(dcc232);

  int scdelay = 0;
  Boolean scdetected = False;
  Boolean inversedsr = wDCC232.isinversedsr(data->dcc232);
;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DCC232 watchdog started." );

  while(data->run) {

    ThreadOp.sleep(100);

    if( data->power ) {

      if ( ( SerialOp.isDSR(data->serial) && !inversedsr ) ) {
        TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "shortcut detected" );

        if( scdetected && scdelay > (data->shortcutdelay / 100) ) {
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

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DCC232 writer started. (0x%08X)", dcc232 );

  ThreadOp.setHigh( th );
  SerialOp.setSerialMode(data->serial,dcc);


  while(data->run) {

    if( data->power && !data->ptflag ) {
      byte * post = NULL;
      byte dccpacket[64] = {0};
      post = (byte*)ThreadOp.getPost( th );

      if (post != NULL) {
        while (post != NULL) {
          MemOp.copy( dccpacket, post, 64);
          freeMem( post);
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "processing posted packet, size=%d", dccpacket[0] );
          __transmit( dcc232, dccpacket+1, dccpacket[0], False );
          post = (byte*)ThreadOp.getPost( th );
        }
      }
      else if( data->slots[slotidx].addr > 0 ) {
        if( MutexOp.trywait( data->slotmux, 5 ) ) {
          int size = 0;
          byte dccpacket[64];
          char cmd[32] = {0};
          char out[64] = {0};
          char in [64] = {0};
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot[%d] refresh for %d", slotidx, data->slots[slotidx].addr );

          /* check if the slot should be purged */
          if( data->purge && data->slots[slotidx].V == 0 && data->slots[slotidx].changedfgrp == 0 ) {
            if( data->slots[slotidx].idle + (data->purgetime*100) < SystemOp.getTick() ) {
              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
                  "slot %d purged for loco address %d", slotidx, data->slots[slotidx].addr );
              data->slots[slotidx].addr = 0;
              data->slots[slotidx].idle = 0;
              data->slots[slotidx].fgrp = 0;
              data->slots[slotidx].changedfgrp = 0;
              data->slots[slotidx].V_prev = 0;
              data->slots[slotidx].V = 0;
              data->slots[slotidx].refreshcnt = 0;
              MemOp.set( data->slots[slotidx].lcstream, 0, 64 );
              MemOp.set( data->slots[slotidx].fnstream, 0, 64 );
              slotidx++;
              MutexOp.post( data->slotmux );
              continue;
            }
          }
          else {
            data->slots[slotidx].V_prev = data->slots[slotidx].V;
            data->slots[slotidx].fgrp = data->slots[slotidx].changedfgrp;
            data->slots[slotidx].changedfgrp = 0;
            data->slots[slotidx].idle = SystemOp.getTick();
          }


          /* refresh speed packet */
          __transmit( dcc232, data->slots[slotidx].lcstream+1, data->slots[slotidx].lcstream[0], False );
          data->slots[slotidx].refreshcnt++;

          if( data->slots[slotidx].fgrp > 0 || data->slots[slotidx].refreshcnt > 10 ) {
            if( data->slots[slotidx].fnstream[0] > 0 ) {
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot[%d] refresh function for %d", slotidx, data->slots[slotidx].addr );
              data->slots[slotidx].refreshcnt = 0;
              /* transmit big idle packet */
              __transmit( dcc232, NULL, 0, True );
              /* transmit last function packet */
              __transmit( dcc232, data->slots[slotidx].fnstream+1, data->slots[slotidx].fnstream[0], False );
            }
          }

          MutexOp.post( data->slotmux );

        }
        else {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get the slot mutex" );
        }
      }
      else if(slotidx < 127) {
        slotidx++;
        ThreadOp.sleep(0);
        continue;
      }

      slotidx++;
      if(slotidx >= 128) {
        slotidx = 0;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "recycle" );
      }

     /* transmit big idle packet */
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "big idle packet..." );
      __transmit( dcc232, NULL, 0, True );

    }
    else {
      ThreadOp.sleep(10);
    }
  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DCC232 writer ended." );
}


static Boolean scanACK(iOSerial serial) {
  Boolean ack = SerialOp.isRI(serial);
  if( ack ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "PT: ACK detected.");
  }

  return ack;
}

static int __getcvbyte(iODCC232 inst, int cv) {
   /* direct cv access */
  iODCC232Data data = Data(inst);
   char SendStream[2048];

   int ack;
   int start, i;
   int value = 0;
   int sendsize = 0;
   int fastcvget = data->fastcvget;

   TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: cvget for %d", cv);

   /* no special error handling, it's job of the clients */
   if (cv<0 || cv>1024) {
     TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "PT: CV[%d] out of range", cv);
     return;
   }

   TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: enable booster output");
   /* enable booster output */
   SerialOp.setDTR(data->serial,True);

   /**
    * NMRA RP 9.2.3 section E
    * Power On Cycle -  Upon applying power to the track, the Command Station/Programmer must
    * transmit at least 20 valid packets to the Digital Decoder to allow it time to stabilize
    * internal operation before any Service Mode operations are initiated.
    */
   TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: power on cycle");

   TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: start polling...");

   start = 1;
   do {
     SerialOp.flush(data->serial);
     sendsize = createCVgetpacket(cv, value, SendStream, start);
     if( value % 10 == 0 || !fastcvget )
       TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: sending %d bytes checking value %d...", sendsize, value);
     SerialOp.write(data->serial,SendStream,sendsize);
     if (start)
       ThreadOp.sleep(240);
     else if( !fastcvget )
       ThreadOp.sleep(40);
     ack = 0;
     /* wait for UART: */
     ack = scanACK(data->serial);
     for( i = 0; i < (fastcvget ? 5:120) && !ack; i++ ) {
       ack = scanACK(data->serial);
       /* Some USB2Serial adapter dont give a chance to detect uart empty
        * We use waitMM as a fix for this. If detection is impossible,
        * first wait argument is used, else second wait argument is used.
        * This will cause pauses between values, where output will drain.
        * For real Uart, this will not happen.
        *ACK is 6-8ms long, poll att least every 6ms*/
       if( !fastcvget )
         SerialOp.waitMM(data->serial,5000,100);
     }


     /* init for next loop: */
     start = 0;
     if(ack==0) {
       value++;
     }
     else {
       /* 1 or more Reset Packets if an acknowledgement is detected */
       int rsSize = 0;
       char* resetstream = getResetStream(&rsSize);
       for( i = 0; i < 3; i++ ) {
         SerialOp.write( data->serial, resetstream, rsSize);
       }
     }

     TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "PT: next value %d...", value);
   } while( !ack && value < 256);


   TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: ack = %d", ack);

   TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: disable booster output");
   /* disable booster output */
   SerialOp.setDTR(data->serial,False);

   if( ack == 0 )
     value = -1;
   return value;
}

static Boolean __setcvbyte(iODCC232 inst, int cv, int val) {
   /* direct cv access */
  iODCC232Data data = Data(inst);
  Boolean ack = False;
  char SendStream[2048];

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: cvset for %d=%d", cv, val);

  int sendsize = createCVsetpacket(cv, val, SendStream, True);


  SerialOp.flush(data->serial);
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: sending %d bytes setting cv %d to value %d...", sendsize, cv, val);
  SerialOp.write(data->serial,SendStream,sendsize);

  ack = scanACK(data->serial);
  int i = 0;
  for( i = 0; i < (data->fastcvget ? 5:120) && !ack; i++ ) {
    ack = scanACK(data->serial);
    if( !data->fastcvget )
      SerialOp.waitMM(data->serial,5000,100);
  }
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: ack = %d", ack);

  return ack;
}



/**  */
static struct ODCC232* _inst( const iONode ini ,const iOTrace trc ) {
  iODCC232 __DCC232 = allocMem( sizeof( struct ODCC232 ) );
  iODCC232Data data = allocMem( sizeof( struct ODCC232Data ) );
  MemOp.basecpy( __DCC232, &DCC232Op, 0, sizeof( struct ODCC232 ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->mux     = MutexOp.inst( NULL, True );
  data->slotmux = MutexOp.inst( NULL, True );

  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );

  data->dcc232 = wDigInt.getdcc232(ini);
  if( data->dcc232 == NULL ) {
    data->dcc232 = NodeOp.inst( wDCC232.name(), ini, ELEMENT_NODE );
    NodeOp.addChild( ini, data->dcc232 );
  }

  data->purge = wDCC232.ispurge(data->dcc232);
  data->purgetime = wDCC232.getpurgetime(data->dcc232);
  data->shortcut = wDCC232.isshortcut(data->dcc232);
  data->shortcutdelay = wDCC232.getshortcutdelay(data->dcc232);
  data->fastcvget = wDCC232.isfastcvget(data->dcc232);

  data->device = StrOp.dup( wDCC232.getport( data->dcc232 ) );
  data->run    = True;

  MemOp.set( data->slots, 0, 128 * sizeof( struct slot ) );

  SystemOp.inst();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "dcc232 %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid             = [%s]"    , data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device          = [%s]"    , data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "purge           = [%s]"    , data->purge?"yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "purge idle time = [%d]s"   , data->purgetime );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "shortcut check  = [%s]"    , data->shortcut?"yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "shortcut delay  = [%d]ms"  , data->shortcutdelay );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "invert DSR      = [%s]"    , wDCC232.isinversedsr(data->dcc232)?"yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setBlocking( data->serial, False );
  SerialOp.setLine( data->serial, 19200, 8, 0, 0, True );
  SerialOp.setCTS( data->serial, False); /*Don't use CTS handshake*/
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );
  SerialOp.open( data->serial );

  SerialOp.setOutputFlow(data->serial,False);          /* suspend output */
  SerialOp.setRTS(data->serial,True);  /* +12V for ever on RTS   */
  SerialOp.setDTR(data->serial,False); /* disable booster output */

  if( data->shortcut ) {
    data->watchdog = ThreadOp.inst( "watchdog", &__watchDog, __DCC232 );
    ThreadOp.start( data->watchdog );
  }
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
