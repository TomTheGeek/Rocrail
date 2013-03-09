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
#include "rocrail/wrapper/public/LocList.h"
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

#include "rocdigs/impl/z21/z21.h"
#include "rocdigs/impl/loconet/lnconst.h"

static int instCnt = 0;

#define csEmergencyStop 0x01
#define csTrackVoltageOff 0x02
#define csShortCircuit 0x04
#define csProgrammingModeActive 0x20

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

static int __normalizeSteps(int insteps, byte* conf ) {
  /* SPEEDSTEPS: vaild: 14, 28, 128 */
  if( insteps < 20 ) {
    *conf = 1;
    return 14;
  }
  if( insteps > 100 ) {
    *conf = 3;
    return 127;
  }
  *conf = 2;
  return 28;
}


static void __fx2fn(iOSlot slot, int fx) {
  int i = 0;
  for( i = 0; i < 28; i++ ) {
    slot->f[i] = (fx & (1 << i)) ? True:False;
  }
}


static iOSlot __getSlotByAddr(iOZ21 inst, int lcaddr) {
  iOZ21Data data = Data(inst);
  iOSlot slot = NULL;
  if( MutexOp.wait( data->lcmux ) ) {
    slot = (iOSlot)MapOp.first( data->lcmap);
    while( slot != NULL ) {
      if( slot->addr == lcaddr ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot found for %s by address %d", slot->id, lcaddr );
        break;
      }
      slot = (iOSlot)MapOp.next( data->lcmap);
    };
    MutexOp.post(data->lcmux);
  }
  return slot;
}


static iOPoint __getPointByAddr(iOZ21 inst, int swaddr) {
  iOZ21Data data = Data(inst);
  iOPoint point = NULL;
  if( MutexOp.wait( data->swmux ) ) {
    point = (iOPoint)MapOp.first( data->swmap);
    while( point != NULL ) {
      if( point->addr == swaddr ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "point found for %s by address %d", point->id, swaddr );
        break;
      }
      point = (iOPoint)MapOp.next( data->swmap);
    };
    MutexOp.post(data->swmux);
  }
  return point;
}


static iOPoint __getPoint(iOZ21 inst, iONode node) {
  iOZ21Data data = Data(inst);
  int     addr  = wSwitch.getaddr1(node);
  char    key[256] = {'\0'};
  iOPoint point = NULL;

  StrOp.fmtb(key, "%s-%d", wSwitch.getid(node), addr );

  point = (iOPoint)MapOp.get( data->swmap, key );
  if( point != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "point exist for %s", key );
    return point;
  }
  point = allocMem( sizeof( struct point) );
  point->addr = addr;
  point->id  = StrOp.dup(wSwitch.getid(node));

  if( MutexOp.wait( data->swmux ) ) {
    MapOp.put( data->swmap, key, (obj)point);
    MutexOp.post(data->swmux);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "point created for %s", key );
  return point;
}


static iOSlot __getSlot(iOZ21 inst, iONode node) {
  iOZ21Data data = Data(inst);
  int    addr  = wLoc.getaddr(node);
  iOSlot slot  = NULL;
  byte* packet = NULL;

  slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot exist for %s", wLoc.getid(node) );
    return slot;
  }

  slot = allocMem( sizeof( struct slot) );
  slot->addr    = addr;
  slot->id      = StrOp.dup(wLoc.getid(node));
  slot->lights  = wLoc.isfn(node);
  slot->dir     = wLoc.isdir(node);

  __fx2fn(slot, wLoc.getfx(node));


  if( MutexOp.wait( data->lcmux ) ) {
    MapOp.put( data->lcmap, wLoc.getid(node), (obj)slot);
    MutexOp.post(data->lcmux);
  }

  packet = allocMem(32);
  packet[0] = 0x09;
  packet[1] = 0x00;
  packet[2] = 0x40;
  packet[3] = 0x00;
  packet[4] = 0xE3;
  packet[5] = 0xF0;
  packet[6] = addr / 256;
  packet[7] = addr % 256;
  packet[8] = packet[4] ^ packet[5] ^ packet[6] ^ packet[7];
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "subscribe loco %d info", addr );
  ThreadOp.post(data->writer, (obj)packet);


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for %s", wLoc.getid(node) );
  return slot;
}


static void __checkDecMode( iOZ21 inst, iONode node ) {
  iOZ21Data data = Data(inst);

  if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) || StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int addr = wLoc.getaddr(node);

    if( addr < 256 ) {
      int mode = StrOp.equals(wLoc.prot_M, wLoc.getprot(node) ) ? DECMODE_MM:DECMODE_DCC;
      if( data->lcmode[addr] != mode ) {
        byte* packet = allocMem(32);
        data->lcmode[addr] = mode;
        packet[0] = 0x07;
        packet[1] = 0x00;
        packet[2] = 0x61;
        packet[3] = 0x00;
        packet[4] = addr / 256;
        packet[5] = addr % 256;
        packet[6] = (mode >> 1);
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set loco %d mode to [%s]", addr, (mode == DECMODE_MM)?"MM":"DCC"  );
        ThreadOp.post(data->writer, (obj)packet);
        ThreadOp.sleep(100);
      }
    }
  }
  else {
    int addr = 0;
    if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
      addr = wSwitch.getaddr1(node);
    }
    else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
      addr = wOutput.getaddr(node);
    }
    if( addr < 256 ) {
      int mode = StrOp.equals(wLoc.prot_M, wSwitch.getprot(node) ) ? DECMODE_MM:DECMODE_DCC;
      if( data->swmode[addr] != mode ) {
        byte* packet = allocMem(32);
        data->swmode[addr] = mode;
        packet[0] = 0x07;
        packet[1] = 0x00;
        packet[2] = 0x71;
        packet[3] = 0x00;
        packet[4] = addr / 256;
        packet[5] = addr % 256;
        packet[6] = (mode >> 1);
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set switch %d mode to [%s]", addr, (mode == DECMODE_MM)?"MM":"DCC"  );
        ThreadOp.post(data->writer, (obj)packet);
      }
    }
  }
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

    else if( StrOp.equals( cmdstr, wSysCmd.ebreak ) ) {
      /* emergency break */
      byte* packet = allocMem(32);
      packet[0] = 0x06;
      packet[1] = 0x00;
      packet[2] = 0x40;
      packet[3] = 0x00;
      packet[4] = 0x80;
      packet[5] = 0x80;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_SET_STOP" );
      ThreadOp.post(data->writer, (obj)packet);
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int addr  = wSwitch.getaddr1( node );
    int delay = wSwitch.getdelay(node) > 0 ? wSwitch.getdelay(node):data->swtime;
    Boolean turnout = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node));
    Boolean active = True;
    iOPoint point = __getPoint(inst, node);
    if( !point->timerpending ) {
      byte* packet = allocMem(32);

      __checkDecMode( inst, node );

      if( wSwitch.getbus(node) == 1 ) {
        /* Loconet command */
        packet[0] = 0x08;
        packet[1] = 0x00;
        packet[2] = LAN_LOCONET_FROM_LAN;
        packet[3] = 0x00;

        packet[4] = OPC_SW_REQ;
        packet[5]  = (unsigned short int) (addr & 0x007f);
        packet[6]  = (unsigned short int) (( addr >> 7) & 0x000f);
        packet[6] |= (unsigned short int) ( (turnout & 0x0001) << 5);
        packet[6] |= (unsigned short int) ( (active & 0x0001) << 4);
        packet[7] = packet[4] ^ packet[5] ^ packet[6]; /*xor*/
      }
      else {
        packet[0] = 0x09;
        packet[1] = 0x00;
        packet[2] = 0x40;
        packet[3] = 0x00;
        packet[4] = 0x53;
        packet[5] = addr / 256; /*MSB*/
        packet[6] = addr % 256; /*LSB*/
        packet[7] = 0x80 + (active?0x08:0x00) + (turnout?0x00:0x01); /*1000A00P*/
        packet[8] = packet[4] ^ packet[5] ^ packet[6] ^ packet[7]; /*xor*/
      }
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "dual gate switch %d: %s", addr, wSwitch.getcmd(node) );
      ThreadOp.post(data->writer, (obj)packet);

      if( wSwitch.isactdelay(node) ) {
        iQCmd cmd = allocMem(sizeof(struct QCmd));
        point->timerpending = True;
        cmd->time   = SystemOp.getTick();
        cmd->delay  = delay / 10;
        active = False;
        cmd->out[0] = 0x09;
        cmd->out[1] = 0x00;
        cmd->out[2] = 0x40;
        cmd->out[3] = 0x00;
        cmd->out[4] = 0x53;
        cmd->out[5] = addr / 256; /*MSB*/
        cmd->out[6] = addr % 256; /*LSB*/
        cmd->out[7] = 0x80 + (active?0x08:0x00) + (turnout?0x00:0x01); /*1000A00P*/
        cmd->out[8] = cmd->out[4] ^ cmd->out[5] ^ cmd->out[6] ^ cmd->out[7]; /*xor*/
        ThreadOp.post(data->timedqueue, (obj)cmd);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "reject switch %s command: timed off is pending", wSwitch.getid(node) );
    }
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int addr = wOutput.getaddr( node );
    int gate = wOutput.getgate( node );
    Boolean active = StrOp.equals( wOutput.getcmd( node ), wOutput.on );
    byte* packet = allocMem(32);

    __checkDecMode( inst, node );

    packet[0] = 0x09;
    packet[1] = 0x00;
    packet[2] = 0x40;
    packet[3] = 0x00;
    packet[4] = 0x53;
    packet[5] = addr / 256; /*MSB*/
    packet[6] = addr % 256; /*LSB*/
    packet[7] = 0x80 + (active?0x08:0x00) + gate; /*1000A00P*/
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
    int addr  = wLoc.getaddr( node );
    byte conf  = 0;
    iOSlot slot = __getSlot(inst, node);
    int spcnt = __normalizeSteps(wLoc.getspcnt( node ), &conf);
    int speed = 0;
    Boolean fnstate = wLoc.isfn(node);
    Boolean dir = wLoc.isdir( node );
    byte* packet = allocMem(32);

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * spcnt) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * spcnt) / wLoc.getV_max( node );
    }

    __checkDecMode(inst, node);

    if( slot->Vraw != speed || slot->dir != dir) {
      slot->Vraw = speed;
      slot->dir = dir;
      packet[0] = 0x0A;
      packet[1] = 0x00;
      packet[2] = 0x40;
      packet[3] = 0x00;
      packet[4] = 0xE4;
      packet[5] = 0x10+conf; /* speed steps*/
      packet[6] = addr / 256; /*MSB*/
      packet[7] = addr % 256; /*LSB*/
      packet[8] = (dir?0x80:0x00) + speed;
      packet[9] = packet[4] ^ packet[5] ^ packet[6] ^ packet[7] ^ packet[8]; /*xor*/
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %d step=%d dir=%s (0x%02X)", addr, speed, dir?"fwd":"rev", packet[8] );
      ThreadOp.post(data->writer, (obj)packet);
    }

  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int addr      = wFunCmd.getaddr( node );
    int fnchanged = wFunCmd.getfnchanged(node);
    Boolean fnstate = __getFState(node, fnchanged);
    iOSlot slot = __getSlot(inst, node);
    byte* packet = allocMem(32);

    __checkDecMode(inst, node);

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

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    Boolean direct = wProgram.getmode(node) == wProgram.mode_direct;
    int cv = wProgram.getcv( node );
    int value = wProgram.getvalue( node );
    int addr = wProgram.getaddr( node );

    if( wProgram.getcmd( node ) == wProgram.get ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get CV%d on %s...", cv, wProgram.ispom(node)?"POM":"PT" );

      if( wProgram.ispom(node) ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CV get by POM is not supported" );
      }
      else {
        byte* packet = allocMem(32);
        if( cv > 0 )
          cv--;
        packet[0] = 0x09;
        packet[1] = 0x00;
        packet[2] = 0x40;
        packet[3] = 0x00;
        packet[4] = 0x23;
        packet[5] = 0x11;
        packet[6] = cv / 256;
        packet[7] = cv % 256;
        packet[8] = packet[4] ^ packet[5] ^ packet[6] ^ packet[7]; /*xor*/
        ThreadOp.post(data->writer, (obj)packet);

      }
    }
    else if( wProgram.getcmd( node ) == wProgram.set ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set CV%d=%d %s...", cv, value, wProgram.ispom(node)?"POM":"PT" );

      if( wProgram.ispom(node) ) {
        byte* packet = allocMem(32);
        if( cv > 0 )
          cv--;
        packet[0] = 0x0C;
        packet[1] = 0x00;
        packet[2] = 0x40;
        packet[3] = 0x00;
        packet[4] = 0xE6;
        packet[5] = 0x30;
        packet[6] = addr / 256;
        packet[7] = addr % 256;
        packet[8] = 0xEC + cv / 256;
        packet[9] = cv % 256;
        packet[10] = value;
        packet[11] = packet[4] ^ packet[5] ^ packet[6] ^ packet[7] ^ packet[8] ^ packet[9] ^ packet[10]; /*xor*/
        ThreadOp.post(data->writer, (obj)packet);
      }
      else {
        byte* packet = allocMem(32);
        if( cv > 0 )
          cv--;
        packet[0] = 0x0A;
        packet[1] = 0x00;
        packet[2] = 0x40;
        packet[3] = 0x00;
        packet[4] = 0x24;
        packet[5] = 0x12;
        packet[6] = cv / 256;
        packet[7] = cv % 256;
        packet[8] = value;
        packet[9] = packet[4] ^ packet[5] ^ packet[6] ^ packet[7] ^ packet[8]; /*xor*/
        ThreadOp.post(data->writer, (obj)packet);
      }
    }
  }

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOZ21Data data = Data(inst);
  iONode rsp = NULL;
  if( cmd != NULL ) {
    if( StrOp.equals( NodeOp.getName( cmd ), wLocList.name() ) ) {
      data->locolist = cmd;
    }
    else {
      rsp = __translate( (iOZ21)inst, cmd );
      NodeOp.base.del(cmd);
    }
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
  byte* packet = NULL;

  if( poweroff ) {
    iONode cmd = NodeOp.inst(wSysCmd.name(), NULL, ELEMENT_NODE);
    wSysCmd.setcmd(cmd, wSysCmd.stop);
    Z21Op.cmd(inst, cmd);
    ThreadOp.sleep(100);
  }

  packet = allocMem(32);
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
    wState.setload( node, data->load );
    wState.setptload( node, data->ptload );
    wState.settemp( node, data->temp );
    wState.setvolt( node, data->volt );
    wState.settrackbus( node, data->serialOK );
    wState.setsensorbus( node, True );
    wState.setaccessorybus( node, True );

    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}


static void __handleLocoNet(iOZ21 inst, byte* packet) {
  iOZ21Data data = Data(inst);

  switch( packet[4]) {
  case OPC_GPOFF:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Loconet: Global power off" );
    break;
  default:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Loconet: OPC=0x%02X", packet[4] & 0xFF );
    break;
  }
}

static void __evaluatePacket(iOZ21 inst, byte* packet, int packetSize) {
  iOZ21Data data = Data(inst);
  int packetIdx = 0;

  while( packetIdx < packetSize ) {

    if( packet[packetIdx+2] == LAN_LOCONET_Z21_TX || packet[packetIdx+2] == LAN_LOCONET_Z21_RX ) {
      __handleLocoNet(inst, packet+packetIdx);
    }

    else if( packet[packetIdx] == 0x08 && packet[packetIdx+2] == 0x10 ) {
      int sn = packet[packetIdx+4] + (packet[packetIdx+5] << 8) + (packet[packetIdx+6] << 16) + (packet[packetIdx+7] << 24);
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
        /* Shortcut */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_BC_TRACK_SHORT_CIRCUIT" );
      }
      else if( packet[packetIdx+4] == 0x81 && packet[packetIdx+5] == 0x00 ) {
        /* Emergency break */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_BC_STOPPED" );
      }
      else if( packet[packetIdx+4] == 0x61 && packet[packetIdx+5] == 0x12 ) {
        /* PT short cut */
        iONode cmd = NodeOp.inst(wSysCmd.name(), NULL, ELEMENT_NODE);
        wSysCmd.setcmd(cmd, wSysCmd.go);
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_CV_NACK_SC" );
        Z21Op.cmd((obj)inst, cmd);
      }
      else if( packet[packetIdx+4] == 0x61 && packet[packetIdx+5] == 0x13 ) {
        /* PT no ack */
        iONode cmd = NodeOp.inst(wSysCmd.name(), NULL, ELEMENT_NODE);
        wSysCmd.setcmd(cmd, wSysCmd.go);
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_CV_NACK" );
        Z21Op.cmd((obj)inst, cmd);
      }
      else if( packet[packetIdx+4] == 0x61 && packet[packetIdx+5] == 0x02 ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_BC_PROGRAMMING_MODE" );
      }
    }

    else if( packet[packetIdx] == 0x09 && packet[packetIdx+2] == 0x40 ) {
      if(packet[packetIdx+4] == 0xF3 && packet[packetIdx+5] == 0x0A ) {
        int mv = packet[packetIdx+6];
        int lv = packet[packetIdx+7];
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
            "Z21 firmware version = %d.%d%d", mv & 0x0F, (lv & 0xF0) >> 4, lv & 0x0F );
      }

      else if( packet[packetIdx+4] == 0x43 ) {
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
          wSwitch.setstate( nodeC, zz==0x02?"straight":"turnout" );
          data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
        }
      }
    }

    else if( packet[packetIdx] == 0x0F && packet[packetIdx+2] == 0x80 ) {
      int grp = packet[packetIdx+4];
      int i = 0;
      int n = 0;

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "evaluate sensor group %d", grp);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "%02X %02X %02X %02X %02X %02X %02X %02X %02X %02X",
          packet[packetIdx+5+0], packet[packetIdx+5+1], packet[packetIdx+5+2], packet[packetIdx+5+3], packet[packetIdx+5+4],
          packet[packetIdx+5+5], packet[packetIdx+5+6], packet[packetIdx+5+7], packet[packetIdx+5+8], packet[packetIdx+5+9] );
      for( i = 0; i < 10; i++ ) {
        int idx  = grp * 10 + i;
        byte status = packet[packetIdx+5+i];
        for( n = 0; n < 8; n++ ) {
          int addr = 1 + grp * 10 + i * 8 + n;
          byte mask = (1 << n);
          if( (status & mask) != (data->sensor[idx] & mask) ) {
            iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
            wFeedback.setaddr( nodeC, addr );
            wFeedback.setstate( nodeC, (status & mask) ? True:False );
            if( data->iid != NULL )
              wFeedback.setiid( nodeC, data->iid );
            if( data->listenerFun != NULL && data->listenerObj != NULL )
              data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Sensor[%d] %d=%s", idx, addr, (status & mask)?"on":"off");
          }
        }
        data->sensor[idx] = status;
      }
    }

    /* PT */
    else if( packet[packetIdx] == 0x0A && packet[packetIdx+2] == 0x40 ) {
      if( packet[packetIdx+4] == 0x64 && packet[packetIdx+5] == 0x14 ) {
        int cv = packet[packetIdx+6] * 256 + packet[packetIdx+7] + 1;
        int value = packet[packetIdx+8];
        iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
        wProgram.setcv( node, cv );
        wProgram.setvalue( node, value );
        wProgram.setcmd( node, wProgram.datarsp );
        if( data->iid != NULL )
          wProgram.setiid( node, data->iid );

        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CV %d=%d", cv, value);
        if( data->listenerFun != NULL && data->listenerObj != NULL )
          data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
      }
    }

    /* System state */
    else if( packet[packetIdx] == 0x14 && packet[packetIdx+2] == 0x84 ) {
      byte stat1 = packet[packetIdx+16];
      data->load = packet[packetIdx+5] * 256 + packet[packetIdx+4];
      data->ptload = packet[packetIdx+7] * 256 + packet[packetIdx+6];
      data->temp = packet[packetIdx+11] * 256 + packet[packetIdx+10];
      data->volt = packet[packetIdx+15] * 256 + packet[packetIdx+14];

      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999,
          "System state changed: main=%dmV %dmA pt=%dmA temp=%dC", data->volt, data->load, data->ptload, data->temp);
      data->power = (stat1 & csTrackVoltageOff) ? False:True;
      __reportState(inst);
    }

    /* RailCom */
    else if( packet[packetIdx+2] == 0x88 ) {
      int addr  = (packet[packetIdx+4] & 0x3F) * 256 + packet[packetIdx+5];
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "RailCom detecetd loco %d (global)", addr);
    }

    /* Loco info */
    else if( packet[packetIdx+2] == 0x40 && packet[packetIdx+4] == 0xEF ) {
      iONode nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      iONode nodeD = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );

      int addr  = (packet[packetIdx+5] & 0x3F) * 256 + packet[packetIdx+6];
      int steps = packet[packetIdx+7] & 0x07; /*0=14, 2=28, 4=128*/
      int speed = packet[packetIdx+8] & 0x7F;
      Boolean dir = ((packet[packetIdx+8] & 0x80) ? True:False);
      Boolean fn = ((packet[packetIdx+9] & 0x10) ? True:False);
      iOSlot slot = __getSlotByAddr(inst, addr);

      if( steps == 0 ) steps = 14;
      else if( steps == 2 ) steps = 28;
      else if( steps == 4 ) steps = 127;

      if( slot != NULL ) {
        slot->Vraw = speed;
        slot->steps = steps;
        slot->dir = dir;
        slot->lights = fn;
      }

      if( data->iid != NULL )
        wLoc.setiid( nodeC, data->iid );
      wLoc.setaddr( nodeC, addr );
      wLoc.setcmd( nodeC, wLoc.dirfun);
      wLoc.setdir( nodeC, dir );
      wLoc.setfn( nodeC, fn );
      wLoc.setV_raw( nodeC, speed );
      wLoc.setV_rawMax( nodeC, steps );
      wLoc.setthrottleid( nodeC, "x-bus" );
      wLoc.setcmd( nodeC, wLoc.velocity );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "Loco %d:%d V_raw=%d dir=%s lights=%s", addr, steps, speed, dir?"fwd":"rev", fn?"on":"off");
      data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

      wFunCmd.setaddr(nodeD, addr);
      wLoc.setthrottleid( nodeD, "x-bus" );
      wFunCmd.setf0(nodeD, fn);
      wFunCmd.setf1(nodeD, (packet[packetIdx+9] & 0x01) ? True:False);
      wFunCmd.setf2(nodeD, (packet[packetIdx+9] & 0x02) ? True:False);
      wFunCmd.setf3(nodeD, (packet[packetIdx+9] & 0x04) ? True:False);
      wFunCmd.setf4(nodeD, (packet[packetIdx+9] & 0x08) ? True:False);

      wFunCmd.setf5 (nodeD, (packet[packetIdx+10] & 0x01) ? True:False);
      wFunCmd.setf6 (nodeD, (packet[packetIdx+10] & 0x02) ? True:False);
      wFunCmd.setf7 (nodeD, (packet[packetIdx+10] & 0x04) ? True:False);
      wFunCmd.setf8 (nodeD, (packet[packetIdx+10] & 0x08) ? True:False);
      wFunCmd.setf9 (nodeD, (packet[packetIdx+10] & 0x10) ? True:False);
      wFunCmd.setf10(nodeD, (packet[packetIdx+10] & 0x20) ? True:False);
      wFunCmd.setf11(nodeD, (packet[packetIdx+10] & 0x40) ? True:False);
      wFunCmd.setf12(nodeD, (packet[packetIdx+10] & 0x80) ? True:False);

      wFunCmd.setf13(nodeD, (packet[packetIdx+11] & 0x01) ? True:False);
      wFunCmd.setf14(nodeD, (packet[packetIdx+11] & 0x02) ? True:False);
      wFunCmd.setf15(nodeD, (packet[packetIdx+11] & 0x04) ? True:False);
      wFunCmd.setf16(nodeD, (packet[packetIdx+11] & 0x08) ? True:False);
      wFunCmd.setf17(nodeD, (packet[packetIdx+11] & 0x10) ? True:False);
      wFunCmd.setf18(nodeD, (packet[packetIdx+11] & 0x20) ? True:False);
      wFunCmd.setf19(nodeD, (packet[packetIdx+11] & 0x40) ? True:False);
      wFunCmd.setf20(nodeD, (packet[packetIdx+11] & 0x80) ? True:False);

      wFunCmd.setf21(nodeD, (packet[packetIdx+12] & 0x01) ? True:False);
      wFunCmd.setf22(nodeD, (packet[packetIdx+12] & 0x02) ? True:False);
      wFunCmd.setf23(nodeD, (packet[packetIdx+12] & 0x04) ? True:False);
      wFunCmd.setf24(nodeD, (packet[packetIdx+12] & 0x08) ? True:False);
      wFunCmd.setf25(nodeD, (packet[packetIdx+12] & 0x10) ? True:False);
      wFunCmd.setf26(nodeD, (packet[packetIdx+12] & 0x20) ? True:False);
      wFunCmd.setf27(nodeD, (packet[packetIdx+12] & 0x40) ? True:False);
      wFunCmd.setf28(nodeD, (packet[packetIdx+12] & 0x80) ? True:False);

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "Loco %d functions update...", addr);
      data->listenerFun( data->listenerObj, nodeD, TRCLEVEL_INFO );

    }

    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "unhandled packet at index %d", packetIdx);
      TraceOp.dump ( name, TRCLEVEL_INFO, (char*)packet, packetSize );
    }

    packetIdx += packet[packetIdx];
  }
}


static void __reader( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iOZ21     z21  = (iOZ21)ThreadOp.getParm( th );
  iOZ21Data data = Data(z21);
  int flags = 0;
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
  packet[0] = 0x07;
  packet[1] = 0x00;
  packet[2] = 0x40;
  packet[3] = 0x00;
  packet[4] = 0xF1;
  packet[5] = 0x0A;
  packet[6] = 0xFB;
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_X_GET_FIRMWARE_VERSION" );
  ThreadOp.post(data->writer, (obj)packet);
  ThreadOp.sleep(100);

  flags = bcAll + bcRBus + bcRailcom + bcXPressNetAll + bcLocoNet + bcLocoNetLocos + bcLocoNetSwitches;
  if( wDigInt.issysteminfo(data->ini) )
    flags += bcSystemInfo;

  packet = allocMem(32);
  packet[0] = 0x08;
  packet[1] = 0x00;
  packet[2] = 0x50;
  packet[3] = 0x00;
  packet[4] = (flags & 0x000000FF); /* lsb */
  packet[5] = ((flags & 0x0000FF00) >> 8);
  packet[6] = ((flags & 0x00FF0000) >> 16);
  packet[7] = ((flags & 0xFF000000) >> 24); /* msb */
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
  ThreadOp.sleep(100);

  // SoD RailCom
  packet = allocMem(32);
  packet[0] = 0x04;
  packet[1] = 0x00;
  packet[2] = 0x89;
  packet[3] = 0x00;
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LAN_RAILCOM_GETDATA" );
  ThreadOp.post(data->writer, (obj)packet);



  do {
    byte packet[256];
    MemOp.set( packet, 0, 256);

    int packetSize = SocketOp.recvfrom( data->rwUDP, packet, 256, NULL, NULL );

    if( packetSize > 0 && packetSize < 256 ) {
      TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)packet, packetSize );
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
      TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)packet, len );
      data->serialOK = SocketOp.sendto( data->rwUDP, packet, len, NULL, 0 );
      if(data->serialOK) {
        data->lastcmd = time(NULL);
      }
    }

    ThreadOp.sleep(10);

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 UDP writer stopped." );
}


static void __timedqueue( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iOZ21     z21  = (iOZ21)ThreadOp.getParm(th);
  iOZ21Data data = Data(z21);

  iOList list = ListOp.inst();
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timed queue started" );

  while( data->run ) {
    iQCmd cmd = (iQCmd)ThreadOp.getPost( th );
    if (cmd != NULL) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
          "new timed command time=%d delay=%d tick=%d frame=%s",
          cmd->time, cmd->delay, SystemOp.getTick(), cmd->out+1 );
      ListOp.add(list, (obj)cmd);
    }

    int i = 0;
    for( i = 0; i < ListOp.size(list); i++ ) {
      iQCmd cmd = (iQCmd)ListOp.get(list, i);
      if( (cmd->time + cmd->delay) <= SystemOp.getTick() ) {
        byte* outa = allocMem(32);

        if( cmd->out[0] == 0x09 && cmd->out[2] == 0x40 && cmd->out[4] == 0x53 ) {
          int addr = cmd->out[5] = addr * 256 + cmd->out[6];
          iOPoint point = __getPointByAddr(z21, addr);
          if( point != NULL ) {
            point->timerpending = False;
          }
        }

        MemOp.copy( outa, cmd->out, 32 );
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "timed command" );
        ThreadOp.post( data->writer, (obj)outa );
        ListOp.removeObj(list, (obj)cmd);
        freeMem(cmd);
        break;
      }
    }

    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timed queue ended" );
}


static void __watchdog( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iOZ21     z21  = (iOZ21)ThreadOp.getParm(th);
  iOZ21Data data = Data(z21);
  Boolean subscribed = False;

  ThreadOp.sleep(1000);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 UDP watchdog started." );

  while( data->run ) {
    long t = time(NULL);
    if( t - data->lastcmd >= 30 ) {
      byte* outa = allocMem(32);
      outa[0] = 0x04;
      outa[1] = 0x00;
      outa[2] = 0x10;
      outa[3] = 0x00;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "posting a keep alive packet" );
      ThreadOp.post( data->writer, (obj)outa );
    }

    if( !subscribed && data->locolist != NULL ) {
      iONode lc = wLocList.getlc( data->locolist );
      while ( lc != NULL ) {
        __getSlot(z21, lc);
        lc = wLocList.nextlc(data->locolist, lc);
      }

    }

    ThreadOp.sleep(1000);
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 UDP watchdog stopped." );
}


/**  */
static struct OZ21* _inst( const iONode ini ,const iOTrace trc ) {
  iOZ21 __Z21 = allocMem( sizeof( struct OZ21 ) );
  iOZ21Data data = allocMem( sizeof( struct OZ21Data ) );
  MemOp.basecpy( __Z21, &Z21Op, 0, sizeof( struct OZ21 ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  SystemOp.inst();

  data->ini    = ini;
  data->run    = True;
  data->swtime = wDigInt.getswtime( ini );
  data->lcmap  = MapOp.inst();
  data->lcmux  = MutexOp.inst( NULL, True );
  data->swmap  = MapOp.inst();
  data->swmux  = MutexOp.inst( NULL, True );

  if( wDigInt.getport( data->ini ) == 0 ) {
    wDigInt.setport( data->ini, 21105 );
  }

  if( StrOp.len(wDigInt.gethost( data->ini )) == 0 ) {
    wDigInt.sethost( data->ini, "192.168.0.111");
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 IP address [%s]", wDigInt.gethost( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Z21 UDP port [%d]", wDigInt.getport( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->rwUDP = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getport(data->ini), False, True, False );
  SocketOp.bind(data->rwUDP);

  data->reader = ThreadOp.inst( "z21reader", &__reader, __Z21 );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( "z21writer", &__writer, __Z21 );
  ThreadOp.start( data->writer );

  data->timedqueue = ThreadOp.inst( "z21timedq", &__timedqueue, __Z21 );
  ThreadOp.start( data->timedqueue );

  data->watchdog = ThreadOp.inst( "z21watchdog", &__watchdog, __Z21 );
  ThreadOp.start( data->watchdog );

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
