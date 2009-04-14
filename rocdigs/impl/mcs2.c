/*
 Rocrail - Model Railroad Control System

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

/** ------------------------------------------------------------
  * Module: RocDigs
  * Object: MCS2
  * ------------------------------------------------------------
  */

/*
 * CAN over TCP/IP 13 byte format:
 *
 *  |  0   |  1    | 2 | 3 |  4  | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 |
 *  | PRIO CMD RSP | HASH  | DLC |             DATA                 |
 *
 *
 *
 *
 */

#include "rocdigs/impl/mcs2_impl.h"
#include "rocdigs/impl/mcs2/mcs2-const.h"

#include "rocs/public/mem.h"

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
#include "rocrail/wrapper/public/BinCmd.h"

#include "rocdigs/impl/common/fada.h"


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOMCS2Data data = Data(inst);
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

/** ----- OMCS2 ----- */

static void __setSysMsg( byte* msg, int prio, int cmd, Boolean rsp, int len, long addr, int subcmd, int subcmd2 ) {
  msg[0]  = (prio << 1);
  msg[0] |= (cmd >> 7);
  msg[1]  = ((cmd & 0x7F) << 1 );
  msg[1] |= rsp;
  msg[2]  = 0x03;
  msg[3]  = 0x00;
  msg[4]  = len;
  msg[5]  = (addr & 0xFF000000) >> 24;
  msg[6]  = (addr & 0x00FF0000) >> 16;
  msg[7]  = (addr & 0x0000FF00) >> 8;
  msg[8]  = (addr & 0x000000FF);
  msg[9]  = subcmd;
  msg[10] = subcmd2;
}

static iONode __translate( iOMCS2 inst, iONode node ) {
  iOMCS2Data data = Data(inst);
  byte*  out = allocMem(32);
  byte*  f0  = allocMem(32);
  byte*  f1  = allocMem(32);
  byte*  f2  = allocMem(32);
  byte*  f3  = allocMem(32);
  byte*  f4  = allocMem(32);
  byte*  f5  = allocMem(32);
  byte*  f6  = allocMem(32);
  byte*  f7  = allocMem(32);
  byte*  f8  = allocMem(32);
  byte*  f9  = allocMem(32);
  byte*  f10 = allocMem(32);
  byte*  f11 = allocMem(32);
  byte*  f12 = allocMem(32);
  iONode rsp = NULL;

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "System STOP" );
      __setSysMsg(out, 0, CMD_SYSTEM, False, 5, 0, CMD_SYSSUB_STOP, 0);
      ThreadOp.post( data->writer, (obj)out );
      return rsp;
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "System GO" );
      __setSysMsg(out, 0, CMD_SYSTEM, False, 5, 0, CMD_SYSSUB_GO, 0);
      ThreadOp.post( data->writer, (obj)out );
      return rsp;
    }
  }

  /* Turnout command */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int module = wSwitch.getaddr1( node );
    if ( module == 0 ) //pada used, port will be actual cs2 lineair address
      module = 1;

    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );
    if( port == 0 )    //fada used, convert to address, port
      fromFADA( module, &module, &port, &gate );

    long address = (( module - 1 ) * 4 ) + port - 1 + 0x3000;  //cs 2 uses lineair addressing, address range 0x3000-0x33ff is for accessory decoders address 00 is called 1 by cs2

    if ( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout )) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Turnout %d to turnout", (address - 0x2FFF) );
      __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 6, address, 0, 1);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Turnout %d to straight", (address - 0x2FFF) );
      __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 6, address, 1, 1);
    }
    ThreadOp.post( data->writer, (obj)out );
    return rsp;
  }

  /* Output command */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int module = wOutput.getaddr( node );
    if ( module == 0 ) //pada used, port will be actual cs2 lineair address
      module = 1;
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    if( port == 0 )    //fada used convert to address, port
      fromFADA( module, &module, &port, &gate );
    long address = (( module - 1 ) * 4 ) + port -1 + 0x3000;  //cs 2 uses lineair addressing, address range 0x3000-0x33ff is for accessory decoders

    if ( StrOp.equals( wOutput.getcmd( node ), wOutput.on )) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Ouput %d %s on", (address - 0x2FFF), gate?"b":"a" );
      __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 6, address, gate, 1);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Output %d %s off", (address - 0x2FFF), gate?"b":"a" );
      __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 6, address, gate, 0);
    }
    ThreadOp.post( data->writer, (obj)out );
    return rsp;
  }

  /* Loco command */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int addr   = wLoc.getaddr( node );
    int dir    = 2 - wLoc.isdir( node );  // for cs2 1 is forwards, 2 is backwards, from server 1 = forwards, 0 is backwards
    int speed  = 0;
    int speed1 = 0;
    int speed2 = 0;
    long address = addr + 0x0000;  //cs2 address range 0x0000-0x03ff is for MM1/2 loc/function decoders
    Boolean sw = wLoc.issw( node );
    if (sw) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d %s", addr, (dir==1)?"forwards":"backwards" );
      __setSysMsg(out, 0, CMD_LOCO_DIRECTION, False, 5, address, dir, 0);  //cs2 reverses direction and sets speed to 0
    } else {
      if( wLoc.getV( node ) != -1 ) {
        if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )   //cs2 ranges all speeds from 0 - 1000 regardless of number of steps
          speed = wLoc.getV( node ) * 10;
        else if( wLoc.getV_max( node ) > 0 )
          speed = (wLoc.getV( node ) * 1000) / wLoc.getV_max( node );
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d speedstep=%d", addr, (speed * wLoc.getspcnt( node ) / 1000));
        speed1 = (speed & 0xFF00) >>8;
        speed2 = speed & 0x00FF;
        __setSysMsg(out, 0, CMD_LOCO_VELOCITY, False, 6, address, speed1, speed2);
      }
    }
    ThreadOp.post( data->writer, (obj)out );
    return rsp;
  }

 /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );
    long address = addr + 0x0000;  //cs2 address range 0x0000-0x03ff is for MM1/2 loc/function decoders
    Boolean fn0 = wFunCmd.isf0( node );
    Boolean fn1 = wFunCmd.isf1( node );
    Boolean fn2 = wFunCmd.isf2( node );
    Boolean fn3 = wFunCmd.isf3( node );
    Boolean fn4 = wFunCmd.isf4( node );
    Boolean fn5 = wFunCmd.isf5( node );
    Boolean fn6 = wFunCmd.isf6( node );
    Boolean fn7 = wFunCmd.isf7( node );
    Boolean fn8 = wFunCmd.isf8( node );
    Boolean fn9  = wFunCmd.isf9 ( node );
    Boolean fn10 = wFunCmd.isf10( node );
    Boolean fn11 = wFunCmd.isf11( node );
    Boolean fn12 = wFunCmd.isf12( node );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "function %d light=%s f1=%s f2=%s f3=%s f4=%s f5=%s f6=%s f7=%s f8=%s f9=%s f10=%s f11=%s f12=%s",
        addr, (fn0?"ON":"OFF"), (fn1?"ON":"OFF"), (fn2?"ON":"OFF"), (fn3?"ON":"OFF"), (fn4?"ON":"OFF"),
        (fn5?"ON":"OFF"), (fn6?"ON":"OFF"), (fn7?"ON":"OFF"), (fn8?"ON":"OFF"),
        (fn9?"ON":"OFF"), (fn10?"ON":"OFF"), (fn11?"ON":"OFF"), (fn12?"ON":"OFF") );
    __setSysMsg(f0, 0, CMD_LOCO_FUNCTION , False, 6, address, 0, fn0);
    ThreadOp.post( data->writer, (obj)f0 );
    __setSysMsg(f1, 0, CMD_LOCO_FUNCTION , False, 6, address, 1, fn1);
    ThreadOp.post( data->writer, (obj)f1 );
    __setSysMsg(f2, 0, CMD_LOCO_FUNCTION , False, 6, address, 2, fn2);
    ThreadOp.post( data->writer, (obj)f2 );
    __setSysMsg(f3, 0, CMD_LOCO_FUNCTION , False, 6, address, 3, fn3);
    ThreadOp.post( data->writer, (obj)f3 );
    __setSysMsg(f4, 0, CMD_LOCO_FUNCTION , False, 6, address, 4, fn4);
    ThreadOp.post( data->writer, (obj)f4 );
    __setSysMsg(f5, 0, CMD_LOCO_FUNCTION , False, 6, address, 5, fn5);
    ThreadOp.post( data->writer, (obj)f5 );
    __setSysMsg(f6, 0, CMD_LOCO_FUNCTION , False, 6, address, 6, fn6);
    ThreadOp.post( data->writer, (obj)f6 );
    __setSysMsg(f7, 0, CMD_LOCO_FUNCTION , False, 6, address, 7, fn7);
    ThreadOp.post( data->writer, (obj)f7 );
    __setSysMsg(f8, 0, CMD_LOCO_FUNCTION , False, 6, address, 8, fn8);
    ThreadOp.post( data->writer, (obj)f8 );
    __setSysMsg(f9, 0, CMD_LOCO_FUNCTION , False, 6, address, 9, fn9);
    ThreadOp.post( data->writer, (obj)f9 );
    __setSysMsg(f10, 0, CMD_LOCO_FUNCTION , False, 6, address, 10, fn10);
    ThreadOp.post( data->writer, (obj)f10 );
    __setSysMsg(f11, 0, CMD_LOCO_FUNCTION , False, 6, address, 11, fn11);
    ThreadOp.post( data->writer, (obj)f11 );
    __setSysMsg(f12, 0, CMD_LOCO_FUNCTION , False, 6, address, 12, fn12);
    ThreadOp.post( data->writer, (obj)f12 );
    return rsp;
  }

  freeMem(out);
  freeMem(f0);
  freeMem(f1);
  freeMem(f2);
  freeMem(f3);
  freeMem(f4);
  freeMem(f5);
  freeMem(f6);
  freeMem(f7);
  freeMem(f8);
  freeMem(f9);
  freeMem(f10);
  freeMem(f11);
  freeMem(f12);
  return NULL;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOMCS2Data data = Data(inst);
  iONode rsp = __translate( (iOMCS2)inst, cmd );

  /* Cleanup Node1 */
  cmd->base.del(cmd);

  return rsp;
}


/**  */
static void _halt( obj inst ) {
  iOMCS2Data data = Data(inst);
  data->run = False;
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  return 0;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  return 0;
}


/**  */
static Boolean _supportPT( obj inst ) {
  return False;
}


static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);
  char in[16];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 reader started." );

  do {

    SocketOp.recvfrom( data->readUDP, in, 13 );
    TraceOp.dump( NULL, TRCLEVEL_INFO, in, 13 );

    ThreadOp.sleep(0);

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 reader stopped." );
}


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 writer started." );

  do {
    byte* cmd = (byte*)ThreadOp.getPost( th );
    if (cmd != NULL) {
      TraceOp.dump( NULL, TRCLEVEL_INFO, cmd, 13 );
      SocketOp.sendto( data->writeUDP, cmd, 13 );
      freeMem( cmd );
    }

    ThreadOp.sleep(10);
  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 writer stopped." );
}



/* VERSION: */
static int vmajor = 1;
static int vminor = 3;
static int patch  = 0;
static int _version( obj inst ) {
  iOMCS2Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**
 * UDP Broadcast address = wDigInt.gethost()
 * UDP send port 15730 (fix)
 * UDP recv port 15731 (fix)
 *
 * */
static struct OMCS2* _inst( const iONode ini ,const iOTrace trc ) {
  iOMCS2 __MCS2 = allocMem( sizeof( struct OMCS2 ) );
  iOMCS2Data data = allocMem( sizeof( struct OMCS2Data ) );
  MemOp.basecpy( __MCS2, &MCS2Op, 0, sizeof( struct OMCS2 ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2(1.0) %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->ini = ini;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast address [%s]", wDigInt.gethost(data->ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast tx port [%d]", 15731 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast rx port [%d]", 15730 );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->readUDP = SocketOp.inst( wDigInt.gethost(data->ini), 15730, False, True );
  SocketOp.bind(data->readUDP);
  data->writeUDP = SocketOp.inst( wDigInt.gethost(data->ini), 15731, False, True );

  data->run = True;

  data->reader = ThreadOp.inst( "mcs2reader", &__reader, __MCS2 );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( "mcs2writer", &__writer, __MCS2 );
  ThreadOp.start( data->writer );

  instCnt++;
  return __MCS2;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/mcs2.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
