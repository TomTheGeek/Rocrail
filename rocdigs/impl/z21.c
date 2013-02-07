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
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/FbInfo.h"
#include "rocrail/wrapper/public/FbMods.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Accessory.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/Text.h"

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

Boolean __getFState(iONode fcmd, int fn) {
  switch( fn ) {
    case 0 : return (wFunCmd.isf0 (fcmd) | wLoc.isfn(fcmd));
    case 1 : return wFunCmd.isf1 (fcmd);
    case 2 : return wFunCmd.isf2 (fcmd);
    case 3 : return wFunCmd.isf3 (fcmd);
    case 4 : return wFunCmd.isf4 (fcmd);
    case 5 : return wFunCmd.isf5 (fcmd);
    case 6 : return wFunCmd.isf6 (fcmd);
    case 7 : return wFunCmd.isf7 (fcmd);
    case 8 : return wFunCmd.isf8 (fcmd);
    case 9 : return wFunCmd.isf9 (fcmd);
    case 10: return wFunCmd.isf10(fcmd);
    case 11: return wFunCmd.isf11(fcmd);
    case 12: return wFunCmd.isf12(fcmd);
    case 13: return wFunCmd.isf13(fcmd);
    case 14: return wFunCmd.isf14(fcmd);
    case 15: return wFunCmd.isf15(fcmd);
    case 16: return wFunCmd.isf16(fcmd);
    case 17: return wFunCmd.isf17(fcmd);
    case 18: return wFunCmd.isf18(fcmd);
    case 19: return wFunCmd.isf19(fcmd);
    case 20: return wFunCmd.isf20(fcmd);
    case 21: return wFunCmd.isf21(fcmd);
    case 22: return wFunCmd.isf22(fcmd);
    case 23: return wFunCmd.isf23(fcmd);
    case 24: return wFunCmd.isf24(fcmd);
    case 25: return wFunCmd.isf25(fcmd);
    case 26: return wFunCmd.isf26(fcmd);
    case 27: return wFunCmd.isf27(fcmd);
    case 28: return wFunCmd.isf28(fcmd);
  }
  return False;
}



static iONode __translate(iOZ21 inst, iONode node) {
  iOZ21Data data = Data(inst);
  iONode rsp = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cmd=%s", NodeOp.getName( node ) );

  if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      /* CS off */
      byte* packet = allocMem(32);
      packet[0] = 0x07;
      packet[1] = 0x00;
      packet[2] = 0x40;
      packet[3] = 0x00;
      packet[4] = 0x21;
      packet[5] = 0x80;
      packet[6] = 0xA1;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_SET_TRACK_POWER_OFF" );
      ThreadOp.post(data->writer, (obj)packet);
    }

    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS on */
      byte* packet = allocMem(32);
      packet[0] = 0x07;
      packet[1] = 0x00;
      packet[2] = 0x40;
      packet[3] = 0x00;
      packet[4] = 0x21;
      packet[5] = 0x81;
      packet[6] = 0xA0;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_SET_TRACK_POWER_ON" );
      ThreadOp.post(data->writer, (obj)packet);
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int addr = wSwitch.getaddr1( node );
    Boolean turnout = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node));
    Boolean active = True;
    byte* packet = allocMem(32);
    packet[0] = 0x09;
    packet[1] = 0x00;
    packet[2] = 0x40;
    packet[3] = 0x00;
    packet[4] = 0x53;
    packet[5] = addr / 256; /*MSB*/
    packet[6] = addr % 256; /*LSB*/
    packet[7] = 0x88 + (active?0x08:0x00) + (turnout?0x01:0x00); /*1000A00P*/
    packet[8] = packet[4] ^ packet[5] ^ packet[6] ^ packet[7]; /*xor*/
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "dual gate switch %d: %s", addr, wSwitch.getcmd(node) );
    ThreadOp.post(data->writer, (obj)packet);
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int addr = wOutput.getaddr( node );
    int gate = wOutput.getgate( node );
    Boolean active = StrOp.equals( wOutput.getcmd( node ), wOutput.on );
    byte* packet = allocMem(32);
    packet[0] = 0x09;
    packet[1] = 0x00;
    packet[2] = 0x40;
    packet[3] = 0x00;
    packet[4] = 0x53;
    packet[5] = addr / 256; /*MSB*/
    packet[6] = addr % 256; /*LSB*/
    packet[7] = 0x88 + (active?0x08:0x00) + gate; /*1000A00P*/
    packet[8] = packet[4] ^ packet[5] ^ packet[6] ^ packet[7]; /*xor*/
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d.%d: %s", addr, gate, wSwitch.getcmd(node) );
    ThreadOp.post(data->writer, (obj)packet);
  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int addr = wLoc.getaddr( node );
    int speed = 0;
    byte* packet = allocMem(32);

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * 127) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * 127) / wLoc.getV_max( node );
    }

    packet[0] = 0x0A;
    packet[1] = 0x00;
    packet[2] = 0x40;
    packet[3] = 0x00;
    packet[4] = 0xE4;
    packet[5] = 0x13; /*128 speed steps*/
    packet[6] = addr / 256; /*MSB*/
    packet[7] = addr % 256; /*LSB*/
    packet[8] = (wLoc.isdir( node )?0x10:0x00) + speed;
    packet[9] = packet[4] ^ packet[5] ^ packet[6] ^ packet[7] ^ packet[8]; /*xor*/
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %d step=%d", addr, speed );
    ThreadOp.post(data->writer, (obj)packet);
  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int addr      = wFunCmd.getaddr( node );
    int fnchanged = wFunCmd.getfnchanged(node);
    Boolean fnstate = __getFState(node, fnchanged);
    byte* packet = allocMem(32);
    packet[0] = 0x0A;
    packet[1] = 0x00;
    packet[2] = 0x40;
    packet[3] = 0x00;
    packet[4] = 0xE4;
    packet[5] = 0xF8;
    packet[6] = addr / 256; /*MSB*/
    packet[7] = addr % 256; /*LSB*/
    packet[8] = (fnstate?0x40:0x00) + fnchanged;
    packet[9] = packet[4] ^ packet[5] ^ packet[6] ^ packet[7] ^ packet[8]; /*xor*/
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %d function %d %s", addr, fnchanged, fnstate?"on":"off" );
    ThreadOp.post(data->writer, (obj)packet);
  }

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOZ21Data data = Data(inst);
  iONode rsp = NULL;
  if( cmd != NULL ) {
    rsp = __translate( (iOZ21)inst, cmd );
    NodeOp.base.del(cmd);
  }
  return rsp;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOZ21Data data = Data(inst);
  byte* packet = allocMem(32);
  packet[0] = 0x04;
  packet[1] = 0x00;
  packet[2] = 0x30;
  packet[3] = 0x00;
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_LOGOFF" );
  ThreadOp.post(data->writer, (obj)packet);
  ThreadOp.sleep(200);
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


static void __reportState(iOZ21 inst) {
  iOZ21Data data = Data(inst);

  if( data->listenerFun != NULL && data->listenerObj != NULL ) {
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );

    if( data->iid != NULL )
      wState.setiid( node, data->iid );
    wState.setpower( node, data->power );

    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}


static void __evaluatePacket(iOZ21 inst, byte* packet, int packetSize) {
  iOZ21Data data = Data(inst);
  int packetIdx = 0;

  while( packetIdx < packetSize ) {

    if( packet[packetIdx] == 0x08 && packet[packetIdx+2] == 0x10 ) {
      int sn = packet[packetIdx+4] + (packet[packetIdx+5] << 8) + (packet[packetIdx+6] << 16) + (packet[packetIdx+6] << 24);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Z21 serialnumber = %d", sn );
    }

    else if( packet[packetIdx] == 0x07 && packet[packetIdx+2] == 0x40 ) {
      if( packet[packetIdx+4] == 0x61 && packet[packetIdx+5] == 0x00 ) {
        data->power = False;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_BC_TRACK_POWER_OFF" );
        __reportState(inst);
      }
      else if( packet[packetIdx+4] == 0x61 && packet[packetIdx+5] == 0x01 ) {
        data->power = True;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_BC_TRACK_POWER_ON" );
        __reportState(inst);
      }
      else if( packet[packetIdx+4] == 0x61 && packet[packetIdx+5] == 0x08 ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_BC_TRACK_SHORT_CIRCUIT" );
      }
      else if( packet[packetIdx+4] == 0x81 && packet[packetIdx+5] == 0x00 ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_BC_STOPPED" );
      }
    }

    else if( packet[packetIdx] == 0x09 && packet[packetIdx+2] == 0x40 ) {
      if( packet[packetIdx+4] == 0x43 ) {
        int addr = (packet[packetIdx+5] << 8) + packet[packetIdx+6];
        int zz = packet[packetIdx+7];
        /*
        ZZ=01 P=0, see LAN_X_SET_TURNOUT
        ZZ=10 P=1, see LAN_X_SET_TURNOUT
         */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_TURNOUT_INFO: addr=%d zz=%02X", addr, zz );
        if( zz == 0x01 || zz == 0x02 ) {
          iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
          wSwitch.setaddr1( nodeC, addr );
          if( data->iid != NULL )
            wSwitch.setiid( nodeC, data->iid );
          wSwitch.setstate( nodeC, zz==0x01?"straight":"turnout" );
          data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
        }
      }
    }

    else if( packet[packetIdx] == 0x0F && packet[packetIdx+2] == 0x80 ) {
      int grp = packet[packetIdx+4];
      int i = 0;
      int n = 0;

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "evaluate sensor group %d", grp);
      for( i = 0; i < 10; i++ ) {
        int idx  = grp * 10 + i;
        byte status = packet[packetIdx+5+i];
        for( n = 0; i < 8; i++ ) {
          int addr = grp * 10 + i * 8 + n;
          byte mask = (1 << n);
          if( status & mask != data->sensor[idx] & mask ) {
            iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
            wFeedback.setaddr( nodeC, addr );
            wFeedback.setstate( nodeC, (status & mask) ? True:False );
            if( data->iid != NULL )
              wFeedback.setiid( nodeC, data->iid );
            if( data->listenerFun != NULL && data->listenerObj != NULL )
              data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Sensor %d=%s", addr, (status & mask)?"on":"off");
          }
        }
        data->sensor[idx] = status;
      }
    }

    packetIdx += packet[packetIdx];
  }
}


static void __reader( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iOZ21     z21  = (iOZ21)ThreadOp.getParm( th );
  iOZ21Data data = Data(z21);
  byte* packet = NULL;

  ThreadOp.sleep(500);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 UDP reader started." );

  packet = allocMem(32);
  packet[0] = 0x04;
  packet[1] = 0x00;
  packet[2] = 0x10;
  packet[3] = 0x00;
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_GET_SERIAL_NUMBER" );
  ThreadOp.post(data->writer, (obj)packet);
  ThreadOp.sleep(100);

  packet = allocMem(32);
  packet[0] = 0x08;
  packet[1] = 0x00;
  packet[2] = 0x50;
  packet[3] = 0x00;
  packet[4] = 0x07;
  packet[5] = 0x01;
  packet[6] = 0x00;
  packet[7] = 0x00;
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_SET_BROADCASTFLAGS" );
  ThreadOp.post(data->writer, (obj)packet);
  ThreadOp.sleep(100);

  // SoD Group 0
  packet = allocMem(32);
  packet[0] = 0x05;
  packet[1] = 0x00;
  packet[2] = 0x81;
  packet[3] = 0x00;
  packet[4] = 0x00; // group 0
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_RMBUS_GETDATA" );
  ThreadOp.post(data->writer, (obj)packet);
  ThreadOp.sleep(100);

  // SoD Group 1
  packet = allocMem(32);
  packet[0] = 0x05;
  packet[1] = 0x00;
  packet[2] = 0x81;
  packet[3] = 0x00;
  packet[4] = 0x01; // group 1
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_RMBUS_GETDATA" );
  ThreadOp.post(data->writer, (obj)packet);



  do {
    byte packet[256];
    MemOp.set( packet, 0, 256);

    int packetSize = SocketOp.recvfrom( data->readUDP, packet, 256, NULL, NULL );

    if( packetSize > 0 && packetSize < 256 ) {
      TraceOp.dump ( name, TRCLEVEL_INFO, (char*)packet, packetSize );
      __evaluatePacket(z21, packet, packetSize);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unexpected packet size %d received", packetSize );
      ThreadOp.sleep(10);
    }

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 UDP reader stopped." );
}


static void __writer( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iOZ21     z21  = (iOZ21)ThreadOp.getParm( th );
  iOZ21Data data = Data(z21);


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 UDP writer started." );

  do {
    byte packet[256];
    byte * post = NULL;
    int len = 0;

    post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      /* first byte is the message length */
      len = post[0];
      MemOp.copy( packet, post, len);
      freeMem( post);
      TraceOp.dump ( name, TRCLEVEL_INFO, (char*)packet, len );
      SocketOp.sendto( data->writeUDP, packet, len, NULL, 0 );
    }

    ThreadOp.sleep(10);

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 UDP writer stopped." );
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

  if( wDigInt.getport( data->ini ) == 0 ) {
    wDigInt.setport( data->ini, 21105 );
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 IP address [%s]", wDigInt.gethost( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 UDP port [%d]", wDigInt.getport( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->readUDP = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getport(data->ini), False, True, False );
  SocketOp.bind(data->readUDP);

  data->writeUDP = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getport(data->ini), False, True, False );

  data->reader = ThreadOp.inst( "z21reader", &__reader, __Z21 );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( "z21writer", &__writer, __Z21 );
  ThreadOp.start( data->writer );

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
