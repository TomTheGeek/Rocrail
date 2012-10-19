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

static Boolean __transact( iORmxData data, byte* out, byte* in, byte opcode );


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


static void __updateFB( iORmxData data, iONode fbInfo ) {
  int cnt = NodeOp.getChildCnt( fbInfo );
  int i = 0;

  char* str = NodeOp.base.toString( fbInfo );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "updateFB\n%s", str );
  StrOp.free( str );

  /* reset the list: */
  MemOp.set( data->fbmodcnt, 0, 2 * sizeof(int) );
  MemOp.set( data->fbmods, 0, 2*128 );

  for( i = 0; i < cnt; i++ ) {
    iONode fbmods = NodeOp.getChild( fbInfo, i );
    const char* mods = wFbMods.getmodules( fbmods );
    int bus = wFbMods.getbus( fbmods );
    if( bus > 1 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unsupported bus=%d", bus );
    }
    else if( mods != NULL && StrOp.len( mods ) > 0 ) {

      iOStrTok tok = StrTokOp.inst( mods, ',' );
      int idx = 0;
      while( StrTokOp.hasMoreTokens( tok ) ) {
        int addr = atoi( StrTokOp.nextToken(tok) );
        data->fbmods[bus][idx] = addr & 0x7f;
        idx++;
      };
      data->fbmodcnt[bus] = idx;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "updateFB bus=%d count=%d", bus, idx );
    }
  }
}



static iOSlot __getSlotByAddr(iORmxData data, int addr) {
  int i = 0;
  iOSlot slot = (iOSlot)MapOp.first( data->lcmap );
  while( slot != NULL ) {
    if( slot->addr == addr )
      return slot;
    slot = (iOSlot)MapOp.next( data->lcmap );
  };

  return NULL;
}


static int __normalizeSteps(int insteps ) {
  /* SPEEDSTEPS: vaild: 14, 28, 126 */
  if( insteps < 20 )
    return 14;
  if( insteps > 100 )
    return 126;
  return 28;
}


static iOSlot __getRmxSlot(iORmxData data, iONode node) {
  iOSlot slot = NULL;
  byte cmd[32] = {0};
  byte rsp[32] = {0};
  int addr  = wLoc.getaddr(node);

  cmd[0] = PCKT;
  cmd[1] = 0x06;
  cmd[2] = OPC_RMXCHANEL;
  cmd[3] = addr / 256;
  cmd[4] = addr % 256;
  if( __transact( data, cmd, rsp, OPC_RMXCHANEL ) ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "got RMX Chanel %d for %s", rsp[5], wLoc.getid(node) );
    slot = allocMem( sizeof( struct slot) );
    slot->addr = addr;
    slot->index = rsp[5];
    slot->protocol = rsp[4];
    slot->steps = rsp[7];
    slot->sx1 = rsp[6] < 7 ? True:False;
    slot->bus = wLoc.getbus(node);
    slot->id = StrOp.dup(wLoc.getid(node));
    if( MutexOp.wait( data->lcmux ) ) {
      MapOp.put( data->lcmap, wLoc.getid(node), (obj)slot);
      MutexOp.post(data->lcmux);
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for %s", wLoc.getid(node) );
  }
  return slot;
}


static iOSlot __getSlot(iORmxData data, iONode node) {
  int steps = wLoc.getspcnt(node);
  int addr  = wLoc.getaddr(node);
  int fncnt = wLoc.getfncnt(node);
  Boolean sx1 = False;
  byte index = 0xFF;
  iOSlot slot = NULL;
  byte rsp[32] = {0};
  byte cmd[32] = {PCKT, 0x08, OPC_LOCOINFO}; /* TODO: ShortID support */

  /* get the slot from the map */
  slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot exist for %s", wLoc.getid(node) );
    return slot;
  }

  /* check if the loco is already defined */
  slot = __getRmxSlot(data, node);
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for existing loco %s", wLoc.getid(node) );
    return slot;
  }

  /* the loco is undefined; add it to the CS */
  if( StrOp.equals( wLoc.prot_S, wLoc.getprot(node) ) ) {
    cmd[6] = 2;
    steps = 31;
    sx1 = True;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sx1, steps=%d, fncnt=%d", steps, fncnt );
  }
  else if( StrOp.equals( wLoc.prot_N, wLoc.getprot(node) ) || StrOp.equals( wLoc.prot_L, wLoc.getprot(node) ) ) {
    steps = __normalizeSteps(wLoc.getspcnt(node));
    if( wLoc.getaddr(node) > 127 ) {
      if( steps == 14 )
        cmd[6] = 10;
      else if( steps == 28 )
        cmd[6] = 13;
      else
        cmd[6] = 16;
    }
    else {
      if( steps == 14 )
        cmd[6] = 9;
      else if( steps == 28 )
        cmd[6] = 12;
      else
        cmd[6] = 15;
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "DCC, steps=%d, fncnt=%d", steps, fncnt );
  }
  /* default SX2 */
  else {
    cmd[6] = 7;
    steps = 127;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "default SX2, steps=%d, fncnt=%d", steps, fncnt );
  }

  if( addr > 127 ) {
    cmd[3] = addr / 256;
    cmd[4] = addr % 256;
    cmd[5] = 0;
  }
  else {
    cmd[3] = 0;
    cmd[4] = 0;
    cmd[5] = addr;
  }

  /* get the RMX chanel for the newly defined loco */
  if( __transact( data, cmd, rsp, OPC_LOCOINFO ) ) {
    byte opmode = cmd[6];
    slot = __getRmxSlot(data, node);
  }

  return slot;

}



static int __translate( iORmxData data, iONode node, byte* out, byte* opcode ) {
  /* Feedback configuration update */
  if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
    __updateFB( data, node );
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int bus   = wSwitch.getbus( node ) & 0x01;
    int addr  = wSwitch.getaddr1( node ) & 0x7F;
    byte pin  = 0x01 << ( wSwitch.getport1( node ) - 1 );
    byte mask = ~pin;

    out[0] = PCKT;
    out[1] = 6;
    out[2] = OPC_WRITESX;
    out[3] = bus;
    out[4] = addr;
    out[5] = 0x01 << ( wSwitch.getport1( node ) - 1 );
    /* reset pin to 0: */
    out[5] = data->swstate[bus][addr] & mask;
    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      out[5] |= pin;
    /* save new state: */
    data->swstate[bus][addr] = out[5];

    *opcode = OPC_OK;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "switch addr %d, port %d, cmd %s", addr, wSwitch.getport1( node ), wSwitch.getcmd( node ) );
    return 7;
  }

  /* Output command */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int bus  = wOutput.getbus( node ) & 0x01;
    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;
    byte pin = 0x01 << ( port - 1 );
    byte mask = ~pin;

    out[0] = PCKT;
    out[1] = 6;
    out[2] = OPC_WRITESX;
    out[3] = bus;
    out[4] = addr;
    out[5] = 0x01 << ( port - 1 );
    /* reset pin to 0: */
    out[5] = data->swstate[bus][addr] & mask;
    if( StrOp.equals( wOutput.getcmd( node ), wOutput.on ) )
      out[5] |= pin;
    /* save new state: */
    data->swstate[bus][addr] = out[5];

    *opcode = OPC_OK;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "switch addr %d, port %d, cmd %s", addr, wSwitch.getport1( node ), wSwitch.getcmd( node ) );
    return 7;
  }




  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) || StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      out[0] = PCKT;
      out[1] = 6;
      out[2] = OPC_MODE;
      out[3] = 0;
      out[4] = 0x40;
      *opcode = OPC_STATUS;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      return 6;
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      out[0] = PCKT;
      out[1] = 6;
      out[2] = OPC_MODE;
      out[3] = 0;
      out[4] = 0x80;
      *opcode = OPC_STATUS;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      return 6;
    }
  }

  /* Loc command.*/
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int  speed = 0;
    byte in = 0;
    Boolean fn = wLoc.isfn( node );
    int    dir = wLoc.isdir( node );
    int  spcnt = wLoc.getspcnt( node );

    int index = 0;

    iOSlot slot = __getSlot(data, node );

    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get slot for loco %s", wLoc.getid(node) );
      return 0;
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

    out[0] = PCKT;
    out[1] = 0x07;
    out[2] = OPC_LOCOV;
    out[3] = slot->index;
    out[4] = speed + (dir?0x00:0x80);
    out[5] = (dir?0x00:0x01);
    return 7;

  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    Boolean f0  = False;
    Boolean f1  = wFunCmd.isf1 ( node );
    Boolean f2  = wFunCmd.isf2 ( node );
    Boolean f3  = wFunCmd.isf3 ( node );
    Boolean f4  = wFunCmd.isf4 ( node );
    Boolean f5  = wFunCmd.isf5 ( node );
    Boolean f6  = wFunCmd.isf6 ( node );
    Boolean f7  = wFunCmd.isf7 ( node );
    Boolean f8  = wFunCmd.isf8 ( node );
    Boolean f9  = wFunCmd.isf9 ( node );
    Boolean f10 = wFunCmd.isf10( node );
    Boolean f11 = wFunCmd.isf11( node );
    Boolean f12 = wFunCmd.isf12( node );
    Boolean f13 = wFunCmd.isf13( node );
    Boolean f14 = wFunCmd.isf14( node );
    Boolean f15 = wFunCmd.isf15( node );
    Boolean f16 = wFunCmd.isf16( node );
    Boolean f17 = wFunCmd.isf17( node );
    Boolean f18 = wFunCmd.isf18( node );
    Boolean f19 = wFunCmd.isf19( node );
    Boolean f20 = wFunCmd.isf20( node );
    Boolean f21 = wFunCmd.isf21( node );
    Boolean f22 = wFunCmd.isf22( node );
    Boolean f23 = wFunCmd.isf23( node );

    iOSlot slot = __getSlot(data, node );

    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get slot for loco %s", wLoc.getid(node) );
      return 0;
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "function command for %s", wLoc.getid(node) );

    f0 = slot->lights;

    out[0] = PCKT;
    out[1] = 0x08;
    out[2] = OPC_LOCOF;
    out[3] = slot->index;
    out[4] = (f0 << 0 | f1 << 1 | f2 << 2 | f3 << 3 | f4 << 4 | f5 << 5 | f6 << 6 | f7 << 7);
    out[5] = (f8 << 0 | f9 << 1 | f10 << 2 | f11 << 3 | f12 << 4 | f13 << 5 | f14 << 6 | f15 << 7);
    out[6] = (f16 << 0 | f17 << 1 | f18 << 2 | f19 << 3 | f20 << 4 | f21 << 5 | f22 << 6 | f23 << 7);
    return 7;

  }

  return 0;
}


static void __traceError(iORmxData data, byte* in) {
  switch(in[3]) {
  case ERR_UNKNOWN:
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Unknown OPCODE" ); break;
  case ERR_NORMXCH:
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "No RMX channel" ); break;
  case ERR_UNDEFLC:
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Loco not in database" ); break;
  case ERR_INPUT:
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Input error" ); break;
  case ERR_MODE:
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Mode not equal to 0x01" ); break;
  case ERR_RMXOFF:
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "RMX-Request CS Off" ); break;
  case ERR_LCDBFULL:
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Loco database full" ); break;
  case ERR_ALLUSED:
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "All channels in use" ); break;
  case ERR_XOR:
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Checksum error" ); break;
  }
}



static void __evaluateFB( iORmxData data ) {
  int bus = 0;
  int mod = 0;

  TraceOp.trc( name, data->dummyio ? TRCLEVEL_INFO:TRCLEVEL_DEBUG, __LINE__, 9999, "evaluate sensors..." );

  for( bus = 0; bus < 2; bus++ ) {
    if( data->fbmodcnt[bus] == 0 )
      continue;

    for( mod = 0; mod < data->fbmodcnt[bus]; mod++ ) {
      int addr = data->fbmods[bus][mod];
      byte in = data->sx1[bus][addr];

      if( in != data->fbstate[bus][addr] ) {
        int n = 0;
        int port = 0;
        int state = 0;
        for( n = 0; n < 8; n++ ) {
          if( (in & (0x01 << n)) != (data->fbstate[bus][addr] & (0x01 << n)) ) {
            port = n;
            state = (in & (0x01 << n)) ? 1:0;
            TraceOp.dump ( name, data->dummyio ? TRCLEVEL_INFO:TRCLEVEL_BYTE, (char*)&in, 1 );
            TraceOp.trc( name, data->dummyio ? TRCLEVEL_INFO:TRCLEVEL_DEBUG, __LINE__, 9999, "fb %d = %d", addr*8+port+1, state );
            {
              /* inform listener: Node3 */
              iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
              wFeedback.setaddr( nodeC, addr*8+port+1 );
              wFeedback.setbus( nodeC, bus );
              wFeedback.setstate( nodeC, state?True:False );
              if( data->iid != NULL )
                wFeedback.setiid( nodeC, data->iid );

              data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
            }
          }
        }
        data->fbstate[bus][addr] = in;
      }
    }
  }

}




static void __evaluateSX(iORmxData data, byte* in) {
  int bus  = in[3];
  int addr = in[4];
  int val  = in[5];
  data->sx1[bus][addr] = val;
  __evaluateFB(data);
}


static void __evaluateRMX(iORmxData data, byte* in) {
  int addr = in[3] * 256 + in[4];
  int index = in[5];
  if( index == 0 ) {
    iOSlot slot = __getSlotByAddr(data, addr);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "%s with address %d has been purged", slot!=NULL?slot->id:"unknown loco", addr );
    if( slot != NULL ) {
      if( MutexOp.wait( data->lcmux ) ) {
        MapOp.remove(data->lcmap, slot->id );
        freeMem(slot);
        MutexOp.post(data->lcmux);
      }
    }
  }
}


static void __evaluateStatus(iORmxData data, byte* in) {
  iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
  byte status = in[3];
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Monitoring bus 0 is %s", status & 0x20 ? "ON":"OFF" );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Monitoring bus 1 is %s", status & 0x40 ? "ON":"OFF" );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Track power is %s", status & 0x80 ? "ON":"OFF" );

  if( data->iid != NULL )
    wState.setiid( node, data->iid );
  wState.setpower( node, status & 0x80 ? True:False );
  wState.settrackbus( node, status & 0x80 ? True:False );
  wState.setsensorbus( node, status & 0x60 ? True:False );
  wState.setaccessorybus( node, status & 0x80 ? True:False );

  if( data->listenerFun != NULL && data->listenerObj != NULL )
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
}


static Boolean __evaluateRsp( iORmxData data, byte* out, int outsize, byte* in, int insize, byte opcode ) {
  switch(in[2]) {
  case OPC_OK: /* 0x00 */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "OK" );
    break;
  case OPC_ERR: /* 0x01 */
    __traceError(data, in);
    break;
  case OPC_STATUS: /* 0x04 */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Status" );
    __evaluateStatus(data, in);
    break;
  case OPC_READSX: /* 0x06 */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "SX value" );
    __evaluateSX(data, in);
    break;
  case OPC_LOCOINFO: /* 0x08 */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Loco info" );
    break;
  case OPC_RMXCHANEL: /* 0x20 */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "RMX chanel" );
    __evaluateRMX(data, in);
    break;
  case OPC_LOCOV: /* 0x24 */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Loco velocity" );
    break;
  case OPC_LOCOF: /* 0x28 */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Loco functions" );
    break;
  case OPC_PT: /* 0x28 */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT" );
    break;
  }
  return (in[2] == opcode);
}


static Boolean __readPacket( iORmxData data, byte* in ) {
  Boolean rc = data->dummyio;

  if( !data->dummyio ) {
    rc = SerialOp.read( data->serial, in, 2 );
    if( rc && in[0] == 0x7D) {
      int insize = in[1];
      rc = SerialOp.read( data->serial, in+2, insize - 2 );
      if( rc ) {
        TraceOp.dump( name, TRCLEVEL_BYTE, in, insize );
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

  return rc;
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
          if( __readPacket( data, in ) ) {
            int retries = 0;
            rc = False;
            while( !rc && retries < 128 ) {
              rc = __evaluateRsp(data, out, outsize, in, insize, opcode);
              if( !rc )
                ThreadOp.sleep(10);
              retries++;

            }
          }
        }
      }
    }
    MutexOp.post( data->mux );
  }
  return rc;
}




/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iORmxData data = Data(inst);
  byte out[32];
  byte in [32];
  iONode reply     = NULL;

  MemOp.set( in, 0x00, sizeof( in ) );

  if( cmd != NULL ) {
    byte opcode = 0;
    int size = __translate( data, cmd, out, &opcode );
    if( size > 0 ) {
      if( __transact( data, out, in, opcode ) ) {
      }
    }
  }

  /* Cleanup Node1 */
  cmd->base.del(cmd);

  return reply;
}


/**  */
static void _halt( obj inst, Boolean poweroff ) {
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


static Boolean _setRawListener(obj inst, obj listenerObj, const digint_rawlistener listenerFun ) {
  return True;
}

static byte* _cmdRaw( obj inst, const byte* cmd ) {
  return NULL;
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
static int patch  = 0;
static int _version( obj inst ) {
  iORmxData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
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
        { /* bus 0 */
          byte out[] = { PCKT,0x06,OPC_MODE,0x00,0x20,0x00 };
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Monitoring bus 0 request..." );
          __transact(data, out, buffer, OPC_STATUS );
        }
        { /* bus 1 */
          byte out[] = { PCKT,0x06,OPC_MODE,0x01,0x20,0x00 };
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Monitoring bus 1 request..." );
          __transact(data, out, buffer, OPC_STATUS );
        }
      }
    }
    
    if( MutexOp.wait( data->mux ) ) {
      /* checking for unsolicited packets */
      if( SerialOp.available(data->serial) ) {
        if( __readPacket( data, buffer ) ) {
        }
      }
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
