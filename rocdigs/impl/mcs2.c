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

static Boolean __getFunctionState(iONode node, int fnchanged) {
  switch( fnchanged ) {
  case 0: return wFunCmd.isf0(node);
  case 1: return wFunCmd.isf1(node);
  case 2: return wFunCmd.isf2(node);
  case 3: return wFunCmd.isf3(node);
  case 4: return wFunCmd.isf4(node);
  case 5: return wFunCmd.isf5(node);
  case 6: return wFunCmd.isf6(node);
  case 7: return wFunCmd.isf7(node);
  case 8: return wFunCmd.isf8(node);
  case 9: return wFunCmd.isf9(node);
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
  byte*  out   = allocMem(32);
  byte*  out2  = allocMem(32);
  iONode rsp   = NULL;

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

  /* Switch command */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int module = wSwitch.getaddr1( node );
    if ( module == 0 )
      /* pada used, port will be actual cs2 lineair address */
      module = 1;

    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );
    Boolean dccswitch = StrOp.equals( wSwitch.getprot( node ), wSwitch.prot_N );
    if( port == 0 )
      fromFADA( module, &module, &port, &gate );

    long address = (( module - 1 ) * 4 ) + port - 1 + (dccswitch?0x3800:0x3000);
    /* cs 2 uses lineair addressing, address range 0x3000-0x33ff is for accessory decoders MM, 0x3800 for DCC, CS2 first address is 0, Rocrail's first is 1 */

    if ( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout )) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Switch %d (%s) to turnout", (address - (dccswitch?0x37FF:0x2FFF) ), dccswitch?"dcc":"mm" );
      __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 6, address, 0, 1);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Switch %d (%s) to straight", (address - (dccswitch?0x37FF:0x2FFF) ), dccswitch?"dcc":"mm" );
      __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 6, address, 1, 1);
    }

    ThreadOp.post( data->writer, (obj)out );
    return rsp;
  }

  /* Output command */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int module = wOutput.getaddr( node );
    if ( module == 0 )
      module = 1;

    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    Boolean dccoutput = StrOp.equals( wOutput.getprot( node ), wOutput.prot_N );
    if( port == 0 )    //fada used convert to address, port
      fromFADA( module, &module, &port, &gate );

    long address = (( module - 1 ) * 4 ) + port -1 + (dccoutput?0x3800:0x3000);

    if ( StrOp.equals( wOutput.getcmd( node ), wOutput.on )) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Output %d %s (%s) on", (address - (dccoutput?0x37FF:0x2FFF) ), gate?"b":"a", dccoutput?"dcc":"mm" );
      __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 6, address, gate, 1);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Output %d %s (%s) off", (address - (dccoutput?0x37FF:0x2FFF) ), gate?"b":"a", dccoutput?"dcc":"mm" );
      __setSysMsg(out, 0, CMD_ACC_SWITCH, False, 6, address, gate, 0);
    }

    ThreadOp.post( data->writer, (obj)out );
    return rsp;
  }

  /* Loco command */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    Boolean sw     = wLoc.issw( node );
    int addr       = wLoc.getaddr( node );
    int addroffset = 0;
    int dir        = 2 - wLoc.isdir( node );
    /* for cs2 1 is forwards, 2 is backwards, from server 1 = forwards, 0 is backwards */
    int speed      = 0;
    int speed1     = 0;
    int speed2     = 0;
    char prot[4];

    if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_N ) ) {
      addroffset = 0xC000;
      /* DCC loc adress range start */
      strcpy(prot,"dcc");
    } else if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_P ) ) {
      addroffset = 0x4000;
      /* MFX loc address range start */
      strcpy(prot,"mfx");
    } else {
      addroffset = 0x0000;
      /* MM loc address range start */
      strcpy(prot,"mm");
    }
    long address = addr + addroffset;

    if( wLoc.getV( node ) != -1 ) {
      /* cs2 ranges all speeds from 0 - 1000 regardless of number of steps */
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = wLoc.getV( node ) * 10;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * 1000) / wLoc.getV_max( node );
      speed1 = (speed & 0xFF00) >>8;
      speed2 = speed & 0x00FF;
    }

    if (sw) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d (%s) %s", addr, prot, (dir==1)?"forwards":"backwards" );
      __setSysMsg(out, 0, CMD_LOCO_DIRECTION, False, 5, address, dir, 0);
      __setSysMsg(out2, 0, CMD_LOCO_VELOCITY, False, 6, address, speed1, speed2);
      /* when changing direction cs2 set speed to 0 internally, so after direction change also send speed */
    } else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d (%s) speedstep=%d %s", addr, prot, (speed * wLoc.getspcnt( node ) / 1000), (dir==1)?"forwards":"backwards");
      __setSysMsg(out2, 0, CMD_LOCO_VELOCITY, False, 6, address, speed1, speed2);
      /* also send direction to prevent going wrong way when user has changed direction on the cs2 */
      __setSysMsg(out, 0, CMD_LOCO_DIRECTION, False, 5, address, dir, 0);
    }
    /* send direction first, speed as second, because of CS2 setting speed to 0 on dir change */
    ThreadOp.post( data->writer, (obj)out );
    ThreadOp.post( data->writer, (obj)out2 );
    return rsp;
  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int fnchanged  = wFunCmd.getfnchanged(node);
    int addr       = wFunCmd.getaddr( node );
    int addroffset = 0;
    char prot[4];

    if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_N ) ) {
      addroffset = 0xC000;
      /* DCC loc adress range start */
      strcpy(prot,"dcc");
    } else if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_P ) ) {
      addroffset = 0x4000;
      /* MFX loc address range start */
      strcpy(prot,"mfx");
    } else {
      addroffset = 0x0000;
      /* MM loc address range start */
      strcpy(prot,"mm");
    }
    long address = addr + addroffset;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "Loc %d (%s) function f%d to %s", addr, prot, fnchanged, __getFunctionState(node, fnchanged)?"on":"off" );

    if( fnchanged != -1 ) {
      __setSysMsg(out, 0, CMD_LOCO_FUNCTION , False, 6, address, fnchanged, __getFunctionState(node, fnchanged));
      ThreadOp.post( data->writer, (obj)out );
      return rsp;
    }
  }

  freeMem(out);
  freeMem(out2);
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
  iOMCS2Data data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  return 0;
}


/* external shortcut event */
static void _shortcut(obj inst) {
  iOMCS2Data data = Data( inst );
}


/**  */
static Boolean _supportPT( obj inst ) {
  return False;
}

static void __feedbackMCS2Reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);
  int mod = 0;
  long dummy = 0x5263526C;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 feedbackpoll started, polling %d S88 units", data->fbmod );
  do {
    ThreadOp.sleep( 250 );

    if( data->fbmod == 0 )
      continue;

    for( mod = 0; mod < data->fbmod; mod++ ) {
      byte* out = allocMem(16);
      __setSysMsg(out, 0, 0x10, False, 5, dummy, mod, 0);
      /* unofficial command 0x10 request status of feedback module mod, one module has 16 inputs */
      ThreadOp.post( data->writer, (obj)out );
      out = NULL;
      freeMem( out );
    }
  } while( data->run );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback MCS2 reader ended." );
}

static void __evaluateMCS2S88( iOMCS2Data mcs2, byte* in, unsigned char* prev ) {
  int s88base = in[9] * 16;
  int n = 0;
  int addr = 0;
  int state = 0;
  int t = 0;
  for( t = 0; t < 2; t++) {
    for( n = 0; n < 8; n++ ) {
      addr = s88base + n + 1 + (t * 8);
      state = (in[11 - t] & (0x01 << n)) ? 1:0;
      /* cs2 uses big endian, in11 contains lower 8 inputs, in10 the higher 8 */
      if( prev[addr - 1] != state ) {
        /* this feedback changed state since previous poll */
        prev[addr - 1] = state;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "fb %d = %d", addr, state );
        {
          /* inform listener: Node */
          iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
          wFeedback.setaddr( nodeC, addr );
          wFeedback.setstate( nodeC, state?True:False );
          if( mcs2->iid != NULL )
            wFeedback.setiid( nodeC, mcs2->iid );
          mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
        }
      }
    }
  }
}

static void __evaluateMCS2Loc( iOMCS2Data mcs2, byte* in ) {
  int addr  = ( ( in[7] & 0x0F ) << 8 ) + in[8];
  /* mask left nibble of high byte because this is protocol dependent 0x0 for MM, 0x4 for MFX, 0xC for DCC */
  int speed = (in[9] << 8) + in[10];
  int dir   = in[9];
  iONode nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  if( mcs2->iid != NULL )
    wLoc.setiid( nodeC, mcs2->iid );
  wLoc.setaddr( nodeC, addr );
  if( in[1] == 0x0A ) {
    /* loc command was a direction command.
       The cs2 sets loc speed to 0 internally on dir change and does not broadcast this. Since the command was not
       send by Rocrail, we have to assume that speed of loc is now set to zero. */
    wLoc.setV_raw( nodeC, 0 );
    wLoc.setV_rawMax( nodeC, 1000 );
    wLoc.setdir( nodeC, dir==1 );
    wLoc.setthrottleid( nodeC, "CS2" );
    /* 1 means forwards, 2 means reverse in cs2 message, in Rocrail true=forward, false=reverse */
//    wLoc.setcmd( nodeC, wLoc.velocity );
    wLoc.setcmd( nodeC, wLoc.direction );
  } else {
    /* loc message was speed message */
    wLoc.setV_raw( nodeC, speed );
    wLoc.setV_rawMax( nodeC, 1000 );
    /* all cs2 speeds are on a scale of 0-1000, regardless of actual locdecoder speedsteps */
    wLoc.setcmd( nodeC, wLoc.velocity );
    wLoc.setthrottleid( nodeC, "CS2" );
  }
  mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
}

static void __evaluateMCS2Function( iOMCS2Data mcs2, byte* in ) {
  int addr     = 0;
  int function = in[9];
  int state    = in[10];
  addr = ( ( in[7] & 0x0F ) << 8 ) + in[8];
  /* mask left nibble of high byte because this is protocol dependent 0x0 for MM, 0x4 for MFX, 0xC for DCC */
  if( function <= 28 ) {
    /* cs2 supports 32 functions, Rocrail 28 */
    iONode nodeC = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
    if( mcs2->iid != NULL )
      wLoc.setiid( nodeC, mcs2->iid );
    wFunCmd.setaddr( nodeC, addr);
    wFunCmd.setfnchanged( nodeC, function);
    wLoc.setcmd( nodeC, wLoc.function );
    switch ( function ) {
      case 0 : wFunCmd.setf0( nodeC, ( state ? True : False)); break;
      case 1 : wFunCmd.setf1( nodeC, ( state ? True : False)); break;
      case 2 : wFunCmd.setf2( nodeC, ( state ? True : False)); break;
      case 3 : wFunCmd.setf3( nodeC, ( state ? True : False)); break;
      case 4 : wFunCmd.setf4( nodeC, ( state ? True : False)); break;
      case 5 : wFunCmd.setf5( nodeC, ( state ? True : False)); break;
      case 6 : wFunCmd.setf6( nodeC, ( state ? True : False)); break;
      case 7 : wFunCmd.setf7( nodeC, ( state ? True : False)); break;
      case 8 : wFunCmd.setf8( nodeC, ( state ? True : False)); break;
      case 9 : wFunCmd.setf9( nodeC, ( state ? True : False)); break;
      case 10 : wFunCmd.setf10( nodeC, ( state ? True : False)); break;
      case 11 : wFunCmd.setf11( nodeC, ( state ? True : False)); break;
      case 12 : wFunCmd.setf12( nodeC, ( state ? True : False)); break;
      case 13 : wFunCmd.setf13( nodeC, ( state ? True : False)); break;
      case 14 : wFunCmd.setf14( nodeC, ( state ? True : False)); break;
      case 15 : wFunCmd.setf15( nodeC, ( state ? True : False)); break;
      case 16 : wFunCmd.setf16( nodeC, ( state ? True : False)); break;
      case 17 : wFunCmd.setf17( nodeC, ( state ? True : False)); break;
      case 18 : wFunCmd.setf18( nodeC, ( state ? True : False)); break;
      case 19 : wFunCmd.setf19( nodeC, ( state ? True : False)); break;
      case 20 : wFunCmd.setf20( nodeC, ( state ? True : False)); break;
      case 21 : wFunCmd.setf21( nodeC, ( state ? True : False)); break;
      case 22 : wFunCmd.setf22( nodeC, ( state ? True : False)); break;
      case 23 : wFunCmd.setf23( nodeC, ( state ? True : False)); break;
      case 24 : wFunCmd.setf24( nodeC, ( state ? True : False)); break;
      case 25 : wFunCmd.setf25( nodeC, ( state ? True : False)); break;
      case 26 : wFunCmd.setf26( nodeC, ( state ? True : False)); break;
      case 27 : wFunCmd.setf27( nodeC, ( state ? True : False)); break;
      case 28 : wFunCmd.setf28( nodeC, ( state ? True : False)); break;
    }
    mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}

static void __evaluateMCS2Switch( iOMCS2Data mcs2, byte* in ) {
  int addr1 = in[7] & 0x0F;
  /* mask left nibble of high byte, this is not part of the actual address (always 0x3 for accessory) */
  int addr2 = in[8];
  int state = in[9];
  int port  = 0;
  int addr  = 0;

  if( addr1 > 7 )
    addr1 = addr1 - 8;
    /* address range start 0x3000 for MM, range start for DCC is 0x3800, so MM 0x3000 or DCC 0x3800 is Rocrail address 1, port 1 */
  addr2 = addr2 + (addr1 << 8) + 1;
  fromPADA( addr2, &addr, &port );

  iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
  if( mcs2->iid != NULL )
    wSwitch.setiid( nodeC, mcs2->iid );
  wSwitch.setaddr1( nodeC, ( addr ) );
  wSwitch.setport1( nodeC, ( port ) );
  wSwitch.setstate( nodeC, (state == 1)?"straight":"turnout" );
  mcs2->listenerFun( mcs2->listenerObj, nodeC, TRCLEVEL_INFO );
}

static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);
  char in[16];
  int mod = 0;
  unsigned char store[1024];
  for( mod = 0; mod < 1024; mod++) {
    store[mod] = 0;
    /* storage container for feedback states to check on changes */
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 reader started." );

  do {
    SocketOp.recvfrom( data->readUDP, in, 13 );
    /* CS2 communication consists of commands (command byte always even) and replies. Reply byte is equal to command byte but with
       response bit (lsb) set, so always odd. When Rocrail sends a command, this is not broadcasted by the CS2, only the reply
       is broadcasted. When a command is issued from the CS2 user interface, both the command and the reply is broadcasted.
       This means that when a command (even) is received, Rocrail did not send that command. */
    if( in[1] == 0x21 ) {
      /* unoffcial reply to unofficial polling command, don't care if the poll was from Rocrail or not, always good to have the S88 state. */
      TraceOp.dump( NULL, TRCLEVEL_BYTE, in, 13 );
      __evaluateMCS2S88( data, in, store );
    } else if( in[1] == 0x0A | in[1] == 0x08 ) {
      /* loc speed or direction comamnd, not from Rocrail. */
      TraceOp.dump( NULL, TRCLEVEL_BYTE, in, 13 );
      __evaluateMCS2Loc( data, in );
    } else if( in[1] == 0x0C ) {
      /* locfunction command, not from Rocrail. */
      TraceOp.dump( NULL, TRCLEVEL_BYTE, in, 13 );
      __evaluateMCS2Function( data, in );
    } else if( in[1] == 0x16 && in[10] == 0x01 ) {
      /* switch command gate activated, second command with gate deactivated again is ignored, not from Rocrail. */
      TraceOp.dump( NULL, TRCLEVEL_BYTE, in, 13 );
      __evaluateMCS2Switch( data, in );
    } else {
      TraceOp.dump( NULL, TRCLEVEL_BYTE, in, 13 );
    }
    ThreadOp.sleep(10);

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 reader stopped." );
}


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);
  byte* cmd = allocMem( 32 );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 writer started." );

  do {
    cmd = (byte*)ThreadOp.getPost( th );
    if (cmd != NULL) {
      TraceOp.dump( NULL, TRCLEVEL_BYTE, cmd, 13 );
      SocketOp.sendto( data->writeUDP, cmd, 13 );
      freeMem( cmd );
    }

    ThreadOp.sleep(10);
  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 writer stopped." );
}



/* VERSION: */
static int vmajor = 1;
static int vminor = 4;
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
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  s88 modules       [%d]", wDigInt.getfbmod( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->readUDP = SocketOp.inst( wDigInt.gethost(data->ini), 15730, False, True );
  SocketOp.bind(data->readUDP);
  data->writeUDP = SocketOp.inst( wDigInt.gethost(data->ini), 15731, False, True );
  data->fbmod    = wDigInt.getfbmod( ini );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );
  data->run = True;

  data->reader = ThreadOp.inst( "mcs2reader", &__reader, __MCS2 );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( "mcs2writer", &__writer, __MCS2 );
  ThreadOp.start( data->writer );

  if( data->fbmod > 0 ) {
    data->feedbackReader = ThreadOp.inst( "fbreader", &__feedbackMCS2Reader, __MCS2 );
    ThreadOp.start( data->feedbackReader );
  }

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
