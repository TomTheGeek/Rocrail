/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

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

#include "rocdigs/impl/zimocan_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"
#include "rocs/public/map.h"
#include "rocs/public/list.h"

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

#include "rocdigs/impl/zimocan/zimocan_const.h"
#include "rocdigs/impl/zimocan/serial.h"
#include "rocdigs/impl/zimocan/udp.h"

#include "rocutils/public/crc.h"


static int instCnt = 0;


static int __makePacket( byte* msg, int group, int cmd, int mode, int dlc, int id, int addr, int d2, int d3, int d4, int d5, int d6, int d7 );
static int __getNID(byte* msg);

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOZimoCANData data = Data(inst);
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

/** ----- OZimoCAN ----- */


static iOSlot __getSlot(iOZimoCAN inst, iONode node) {
  iOZimoCANData data = Data(inst);
  int    addr  = wLoc.getaddr(node);
  iOSlot slot  = NULL;
  byte* msg    = NULL;
  byte M1      = 0;
  byte M2      = 0;

  slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot exist for %s", wLoc.getid(node) );
    return slot;
  }

  slot = allocMem( sizeof( struct slot) );
  slot->addr    = addr;
  slot->nid     = addr;
  slot->id      = StrOp.dup(wLoc.getid(node));
  slot->lights  = wLoc.isfn(node);
  slot->dir     = wLoc.isdir(node);
  slot->steps   = wLoc.getspcnt(node);
  slot->fncnt   = wLoc.getfncnt(node);

  if( StrOp.equals( wLoc.prot_M, wLoc.getprot( node ) ) ) {
    /* Motorola */
    slot->prot = 2;
    slot->nid += 0x2800;
  }
  else if( StrOp.equals( wLoc.prot_F, wLoc.getprot( node ) ) ) {
    /* MFX */
    slot->prot = 4;
    slot->nid += 0x8000;
  }
  else /* Default DCC */
    slot->prot = 1;

  if( MutexOp.wait( data->lcmux ) ) {
    MapOp.put( data->lcmap, wLoc.getid(node), (obj)slot);
    MutexOp.post(data->lcmux);
  }

  msg = allocMem(32);
  msg[0] = __makePacket(msg+1, MOBILE_CONTROL_GROUP, MOBILE_STATE, MODE_REQ, 2, data->NID, slot->nid, 0, 0, 0, 0, 0, 0);
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "query state for loco %s with NID=0x%X", slot->id, slot->nid );
  ThreadOp.post(data->writer, (obj)msg);

  msg = allocMem(32);
  M1 = slot->steps + (slot->prot << 4);
  M2 = slot->fncnt;
  msg[0] = __makePacket(msg+1, MOBILE_CONTROL_GROUP, MOBILE_MODE, MODE_CMD, 4, data->NID, slot->nid, M1, M2, 0, 0, 0, 0);
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set mode parameters, M1=%d M2=%d, for loco %s", M1, M2, slot->id );
  ThreadOp.post(data->writer, (obj)msg);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for %s", wLoc.getid(node) );
  return slot;
}


static iOSlot __getSlotByNID(iOZimoCAN inst, int nid) {
  iOZimoCANData data = Data(inst);
  iOSlot slot = NULL;
  if( MutexOp.wait( data->lcmux ) ) {
    slot = (iOSlot)MapOp.first( data->lcmap);
    while( slot != NULL ) {
      if( slot->nid == nid ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot found for %s by NID %d", slot->id, nid );
        break;
      }
      slot = (iOSlot)MapOp.next( data->lcmap);
    };
    MutexOp.post(data->lcmux);
  }
  return slot;
}


static iOSlot __getSlotByAddr(iOZimoCAN inst, int addr) {
  iOZimoCANData data = Data(inst);
  iOSlot slot = NULL;
  if( MutexOp.wait( data->lcmux ) ) {
    slot = (iOSlot)MapOp.first( data->lcmap);
    while( slot != NULL ) {
      if( slot->addr == addr ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot found for %s by address %d", slot->id, addr );
        break;
      }
      slot = (iOSlot)MapOp.next( data->lcmap);
    };
    MutexOp.post(data->lcmux);
  }
  return slot;
}


static iOPoint __getPoint(iOZimoCAN inst, iONode node) {
  iOZimoCANData data = Data(inst);
  int     bus   = wSwitch.getbus(node);
  int     addr  = wSwitch.getaddr1(node);
  char    key[256] = {'\0'};
  iOPoint point = NULL;

  StrOp.fmtb(key, "%d-%d", bus, addr );

  point = (iOPoint)MapOp.get( data->swmap, key );
  if( point != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "point exist for %s", key );
    return point;
  }
  point = allocMem( sizeof( struct point) );
  point->addr = addr;
  point->nid  = addr + 0x3000;
  point->id   = StrOp.dup(wSwitch.getid(node));

  if( MutexOp.wait( data->swmux ) ) {
    MapOp.put( data->swmap, key, (obj)point);
    MutexOp.post(data->swmux);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "point created for %s", key );
  return point;
}


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



static iONode __translate( iOZimoCAN inst, iONode node ) {
  iOZimoCANData data = Data(inst);
  iONode rsp = NULL;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "translate: %s", NodeOp.getName( node ) );

  if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      /* CS off */
      byte* msg = allocMem(32);
      msg[0] = __makePacket(msg+1, SYSTEM_CONTROL_GROUP, SYSTEM_POWER, MODE_CMD, 4, data->NID, data->masterNID, SYSTEM_POWER_TRACK1, SYSTEM_POWER_OFF, 0, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power OFF" );
      ThreadOp.post(data->writer, (obj)msg);
    }

    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS on */
      byte* msg = allocMem(32);
      msg[0] = __makePacket(msg+1, SYSTEM_CONTROL_GROUP, SYSTEM_POWER, MODE_CMD, 4, data->NID, data->masterNID, SYSTEM_POWER_TRACK1, SYSTEM_POWER_ON, 0, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power ON" );
      ThreadOp.post(data->writer, (obj)msg);
    }
  }

  /* Loco command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    iOSlot slot = __getSlot( inst, node );
    if( slot != NULL ) {
      byte* msg = allocMem(32);
      int Div = 1; /* Speed divider */
      int V = 0;
      Boolean dir = wLoc.isdir(node);

      if( wLoc.getV( node ) != -1 ) {
        if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
          V = (wLoc.getV( node ) * 1023) / 100;
        else if( wLoc.getV_max( node ) > 0 )
          V = (wLoc.getV( node ) * 1023) / wLoc.getV_max( node );
      }

      msg[0] = __makePacket(msg+1, MOBILE_CONTROL_GROUP, MOBILE_SPEED, MODE_CMD, 6, data->NID, slot->nid, (V&0xFF), (V>>8) | (dir?0x00:0x04), Div, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Set loco speed to %d, dir=%s for %s", V, dir?"fwd":"rev", slot->id );
      ThreadOp.post(data->writer, (obj)msg);
    }
  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    iOSlot slot = __getSlot( inst, node );
    if( slot != NULL ) {
      byte* msg = allocMem(32);
      int fn = wFunCmd.getfnchanged(node);
      int fs = __getFState(node, fn);
      msg[0] = __makePacket(msg+1, MOBILE_CONTROL_GROUP, MOBILE_FUNCTION, MODE_CMD, 6, data->NID, slot->nid, (fn&0xFF), (fn>>8), (fs&0xFF), (fs>>8), 0, 0);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Set loco function %d to %d for %s", fn, fs, slot->id );
      ThreadOp.post(data->writer, (obj)msg);
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    iOPoint point = __getPoint(inst, node);
    if( point != NULL ) {
      byte* msg = allocMem(32);
      int port = wSwitch.getport1( node );
      int val  = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? 1:0;
      msg[0] = __makePacket(msg+1, ACCESSORY_COMMAND_GROUP, ACCESSORY_PORT, MODE_CMD, 4, data->NID, point->nid, port, val, 0, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Set switch %d:%d to %d", point->addr, port, val );
      ThreadOp.post(data->writer, (obj)msg);
    }
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    iOPoint point = __getPoint(inst, node);
    if( point != NULL ) {
      byte* msg = allocMem(32);
      int port = wOutput.getport( node );
      int val  = StrOp.equals(wOutput.on, wOutput.getcmd(node)) ? 1:0;
      msg[0] = __makePacket(msg+1, ACCESSORY_COMMAND_GROUP, ACCESSORY_PORT, MODE_CMD, 4, data->NID, point->nid, port, val, 0, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Set output %d:%d to %d", point->addr, port, val );
      ThreadOp.post(data->writer, (obj)msg);
    }
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    int addr = wProgram.getaddr(node);
    int cv   = wProgram.getcv( node );
    iOSlot slot = __getSlotByAddr(inst, addr);

    if( slot != NULL ) {
      if( wProgram.getcmd( node ) == wProgram.get ) {
        byte* msg = allocMem(32);
        msg[0] = __makePacket(msg+1, TRACK_CONFIG_GROUP, TRACK_CONFIG_READ, MODE_CMD, 7, data->NID, slot->nid, cv&0xFF, (cv&0xFF00)>>8, cv>>16, 0, 0, 0);
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "read cv=%d nid=0x%04X", cv, slot->nid );
        ThreadOp.post(data->writer, (obj)msg);
      }
      else if( wProgram.getcmd( node ) == wProgram.set ) {
        int val = wProgram.getvalue( node );
        byte* msg = allocMem(32);
        msg[0] = __makePacket(msg+1, TRACK_CONFIG_GROUP, TRACK_CONFIG_WRITE, MODE_CMD, 8, data->NID, slot->nid, cv&0xFF, (cv&0xFF00)>>8, cv>>16, val, 0, 0);
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "write cv=%d value=%d nid=0x%04X", cv, val, slot->nid );
        ThreadOp.post(data->writer, (obj)msg);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "uninitialised loco %d; unable to program", addr );
    }
  }

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOZimoCANData data = Data(inst);
  iONode rsp = NULL;

  if( cmd != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "zimocan cmd = %s", NodeOp.getName(cmd) );
    rsp = __translate( (iOZimoCAN)inst, cmd );
    cmd->base.del(cmd);
  }
  return rsp;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOZimoCANData data = Data(inst);
  data->run = False;
  ThreadOp.sleep(100);
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOZimoCANData data = Data(inst);
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
  iOZimoCANData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/*
Header Size  Group Cmd+Mode NId    Data0...7 CRC16 Tail
16Bit  8Bit  8Bit  8Bit     16Bit  8x8Bit    16Bit 16 Bit
 */
static int __makePacket( byte* msg, int group, int cmd, int mode, int dlc, int nid, int nidTarget, int d2, int d3, int d4, int d5, int d6, int d7 ) {
  int crc = 0;
  int idx = 0;

  msg[0]  = 0x5A;
  msg[1]  = 0x32;
  msg[2]  = dlc;
  msg[3]  = group;
  msg[4]  = (cmd << 2);
  msg[4] |= (mode & 0x03);
  msg[5]  = (nid & 0xFF);
  msg[6]  = ((nid >> 8) & 0xFF);

  if( dlc >= 2 ) { /* d0 + d1 */
    idx = 7; /* offset */
    msg[idx]  = (nidTarget & 0xFF);
    idx++;
    msg[idx]  = ((nidTarget >> 8) & 0xFF);
    idx++;
  }

  if( dlc >= 3 ) {
    msg[idx] = d2;
    idx++;
  }
  if( dlc >= 4 ) {
    msg[idx] = d3;
    idx++;
  }
  if( dlc >= 5 ) {
    msg[idx] = d4;
    idx++;
  }
  if( dlc >= 6 ) {
    msg[idx] = d5;
    idx++;
  }
  if( dlc >= 7 ) {
    msg[idx] = d6;
    idx++;
  }
  if( dlc >= 8 ) {
    msg[idx] = d7;
    idx++;
  }

  crc = CRCOp.checkSum16(msg+2, 5+dlc);
  msg[7+dlc] = (crc & 0xFF);
  msg[8+dlc] = ((crc >> 8) & 0xFF);

  msg[ 9+dlc] = 0x32;
  msg[10+dlc] = 0x5A;

  return (11 + dlc);
}

static int __getNID(byte* msg) {
  int nid = msg[5] + (msg[6] * 256);
  return nid;
}


static int __getObjectNID(byte* msg) {
  int nid = msg[7] + (msg[8] * 256);
  return nid;
}


static void __evauluateAccessoryData( iOZimoCAN zimocan, byte* msg ) {
  iOZimoCANData data    = Data(zimocan);
  int cmd  = (msg[4] >> 2);
  int mode = (msg[4] &0x03);
  int nid  = __getNID(msg);
  int port = msg[9];
  int type = msg[10];
  int addr = msg[11] + (msg[12] * 256);
  iONode nodeC = NULL;
  char ident[32];

  StrOp.fmtb(ident, "%d", addr );

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "nid=%X port=%d type=%d loco=%d", nid, port, type, addr );

  if( nid >= 0x5080 && nid <= 0x50BF && type >= 0x11 && type <= 0x14) {
    /* MX9 */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "MX9 nid=%X port=%d type=%d loco=%d", nid, port, type, addr );
    nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wFeedback.setiid( nodeC, data->iid );
    wFeedback.setstate( nodeC, addr > 0 ? True:False );
    wFeedback.setbus( nodeC, nid );
    wFeedback.setaddr( nodeC, port );
    wFeedback.setidentifier( nodeC, ident );
  }
  else if( nid >= 0xD000 && nid <= 0xDFFF && type >= 0x20 && type <= 0x3F ) {
    /* StEin */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "StEin nid=%X port=%d type=%d loco=%d", nid, port, type, addr );
    nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wFeedback.setiid( nodeC, data->iid );
    wFeedback.setstate( nodeC, addr > 0 ? True:False );
    wFeedback.setbus( nodeC, nid );
    wFeedback.setaddr( nodeC, port );
    wFeedback.setidentifier( nodeC, ident );
  }

  if( nodeC != NULL ) {
    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}


static void __evauluateAccessoryPort( iOZimoCAN zimocan, byte* msg ) {
  iOZimoCANData data    = Data(zimocan);
  int cmd  = (msg[4] >> 2);
  int mode = (msg[4] &0x03);
  int nid  = __getNID(msg);
  int port = msg[9];
  int val  = msg[10];
  iONode nodeC = NULL;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "nid=%X port=%d value=%d", nid, port, val );

  if( nid >= 0x5080 && nid <= 0x50BF ) {
    /* MX9 */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "MX9 nid=%X port=%d value=%d", nid, port, val );
    nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wFeedback.setiid( nodeC, data->iid );
    wFeedback.setstate( nodeC, val );
    wFeedback.setbus( nodeC, nid );
    wFeedback.setaddr( nodeC, port );
  }
  else if( nid >= 0xD000 && nid <= 0xDFFF ) {
    /* StEin */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "StEin nid=%X port=%d value=%d", nid, port, val );
    nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wFeedback.setiid( nodeC, data->iid );
    wFeedback.setstate( nodeC, val );
    wFeedback.setbus( nodeC, nid );
    wFeedback.setaddr( nodeC, port );
  }

  if( nodeC != NULL ) {
    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}


static void __evauluateAccessoryGroup( iOZimoCAN zimocan, byte* msg ) {
  iOZimoCANData data    = Data(zimocan);
  int cmd  = (msg[4] >> 2);
  int mode = (msg[4] &0x03);

  switch( cmd ) {
  case ACCESSORY_PORT:
    if( mode & 0x02 ) {
      __evauluateAccessoryPort(zimocan, msg);
    }
    break;
  case ACCESSORY_DATA:
    __evauluateAccessoryData(zimocan, msg);
    break;
  }
}


static void __evauluateNetworkGroup( iOZimoCAN zimocan, byte* msg ) {
  iOZimoCANData data    = Data(zimocan);
  int cmd  = (msg[4] >> 2);
  int mode = (msg[4] &0x03);

  switch( cmd ) {
  case NETWORK_PING:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "NETWORK PING" );
    if( !data->connAck ) {
      byte* msgInfo = allocMem(32);
      data->connAck = True;
      data->masterNID = __getNID(msg);
      /* Get software info */
      msgInfo[0] = __makePacket(msgInfo+1, NETWORK_GROUP, NETWORK_MODULINFO, MODE_REQ, 4, data->NID, data->masterNID, 2, 0, 0, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request info" );
      ThreadOp.post(data->writer, (obj)msgInfo);
    }
    break;
  case NETWORK_MODULINFO:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "NETWORK MODULINFO: nid:%d %d %d %d", msg[9], msg[10], msg[11], msg[12] );
    break;
  }
}


static int __getLocoAddr(int nid) {
  /* DCC */
  if( nid < 0x2800 )
    return nid;
  /* MM */
  if( nid < 0x2900 )
    return nid - 0x2800;
  /* MFX */
  if( nid >= 0x8000 && nid <= 0xBFFF )
    return nid - 0x8000;

  return nid;
}


static void __evauluateMobileControlGroup( iOZimoCAN zimocan, byte* msg ) {
  iOZimoCANData data    = Data(zimocan);
  int cmd  = (msg[4] >> 2);
  int mode = (msg[4] &0x03);

  int nid = __getObjectNID(msg);
  int addr = __getLocoAddr(nid);
  iOSlot slot = __getSlotByNID(zimocan, nid);

  switch( cmd ) {
  case MOBILE_SPEED:
    {
      int V = (msg[9] + (msg[10] * 256)) & 0x03FF;
      Boolean Dir = (msg[10] & 0x04) ? False:True;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MOBILE SPEED: [%s] NID=%d addr=%d speed=%d dir=%s", slot==NULL?"-":slot->id, nid, addr, V, Dir?"fwd":"rev" );
      iONode nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      if( data->iid != NULL )
        wLoc.setiid( nodeC, data->iid );
      if( slot != NULL ) {
        wLoc.setid( nodeC, slot->id );
        wLoc.setaddr( nodeC, slot->addr );
      }
      else {
        wLoc.setaddr( nodeC, addr );
      }
      wLoc.setdir( nodeC, Dir );
      wLoc.setV_raw( nodeC, V );
      wLoc.setV_rawMax( nodeC, 1023 );
      wLoc.setthrottleid( nodeC, "zimo" );
      wLoc.setcmd( nodeC, wLoc.velocity );
      data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
    }
    break;

  case MOBILE_FUNCTION:
    {
      int fx   = msg[9] + (msg[10] * 256);
      int fval = msg[11] + (msg[12] * 256);
      char fxStr[32];
      iONode nodeC = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );

      wFunCmd.setfnchanged(nodeC, fx);
      if( data->iid != NULL )
        wLoc.setiid( nodeC, data->iid );
      if( slot != NULL ) {
        wFunCmd.setid( nodeC, slot->id );
        wFunCmd.setaddr( nodeC, slot->addr );
      }
      else {
        wFunCmd.setaddr( nodeC, addr );
      }
      StrOp.fmtb(fxStr, "f%d", fx);
      NodeOp.setBool(nodeC, fxStr, fval?True:False);
      wLoc.setthrottleid( nodeC, "zimo" );
      data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
    }
    break;
  }
}


static void __evauluateTrackConfigGroup( iOZimoCAN zimocan, byte* msg ) {
  iOZimoCANData data    = Data(zimocan);
  int cmd  = (msg[4] >> 2);
  int mode = (msg[4] &0x03);
  int nid  = __getNID(msg);

  switch( cmd ) {
  case TRACK_CONFIG_INFO:
    if( mode == 0x03 ) {
      int state = msg[12];
      int code  = msg[13];
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "track config info: state=0x%02X code=0x%02X", state, code );
    }
    break;
  case TRACK_CONFIG_READ:
  case TRACK_CONFIG_WRITE:
    if( mode == 0x03 ) {
      int loconid = msg[9] + (msg[10] * 256);
      int cv  = msg[11] + msg[12]*0xFF + msg[13]*0xFFFF;
      int val = msg[14];
      iOSlot slot = __getSlotByNID(zimocan, loconid);
      iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      wProgram.setvalue( node, val );
      wProgram.setcmd( node, wProgram.datarsp );
      wProgram.setcv( node, cv+1 );
      wProgram.setiid( node, data->iid );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "track config read: loco=%s(0x%04X) cv=%d value=%d", slot!=NULL?slot->id:"_", loconid, cv, val );
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
    break;
  }
}


static void __evaluateMsg( iOZimoCAN zimocan, byte* msg ) {
  iOZimoCANData data    = Data(zimocan);
  int group = msg[3];
  int id    = msg[5] + (msg[6] * 256);
  int addr  = msg[7] + (msg[8] * 256);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Message from 0x%04X addr=0x%04X", id, addr );

  switch( group ) {
  case SYSTEM_CONTROL_GROUP:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SYSTEM CONTROL GROUP" );
    break;
  case NETWORK_GROUP:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "NETWORK GROUP" );
    __evauluateNetworkGroup(zimocan, msg);
    break;
  case ACCESSORY_COMMAND_GROUP:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ACCESSORY COMMAND GROUP" );
    __evauluateAccessoryGroup(zimocan, msg);
    break;
  case TRACK_CONFIG_GROUP:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "TRACK CONFIG GROUP" );
    __evauluateTrackConfigGroup(zimocan, msg);
    break;
  case MOBILE_CONTROL_GROUP:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MOBILE CONTROL GROUP" );
    __evauluateMobileControlGroup(zimocan, msg);
    break;
  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GROUP: 0x%02X", group );
    break;
  }
}


static void __reader( void* threadinst ) {
  iOThread      th      = (iOThread)threadinst;
  iOZimoCAN     zimocan = (iOZimoCAN)ThreadOp.getParm( th );
  iOZimoCANData data    = Data(zimocan);
  byte msg[256];
  int size = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ZimoCAN reader started." );

  while( data->run ) {
    if( !data->subAvailable( (obj)zimocan) ) {
      ThreadOp.sleep( 10 );
      continue;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CAN message available" );
    }

    size = data->subRead( (obj)zimocan, msg );
    __evaluateMsg(zimocan, msg);
    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ZimoCAN reader ended." );
}


static void __writer( void* threadinst ) {
  iOThread      th      = (iOThread)threadinst;
  iOZimoCAN     zimocan = (iOZimoCAN)ThreadOp.getParm( th );
  iOZimoCANData data    = Data(zimocan);
  int connAckTimer = 0;
  byte eyeCatcher[] = {'Z','2','2','Z',0};
  byte msg[256];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ZimoCAN writer started." );

  while( data->run ) {
    byte * post = NULL;
    int len = 0;

    post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      /* first byte is the message length */
      len = post[0];
      MemOp.copy( msg, post+1, len);
      freeMem( post);
      if( data->subWrite((obj)zimocan, msg, len) ) {

      }
    }

    if( !data->connAck ) {
      connAckTimer++;
      if( connAckTimer > 5000 ) {
        if( data->subWrite((obj)zimocan, eyeCatcher, 4) ) {

        }
      }
    }

    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ZimoCAN writer ended." );
}


/**  */
static struct OZimoCAN* _inst( const iONode ini ,const iOTrace trc ) {
  iOZimoCAN __ZimoCAN = allocMem( sizeof( struct OZimoCAN ) );
  iOZimoCANData data = allocMem( sizeof( struct OZimoCANData ) );
  MemOp.basecpy( __ZimoCAN, &ZimoCANOp, 0, sizeof( struct OZimoCAN ), data );

  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */

  data->ini = ini;
  data->iid = StrOp.dup( wDigInt.getiid( ini ) );
  data->NID = 0xC200;
  data->masterNID = 0xAFFA; /* Dummy */
  data->lcmap  = MapOp.inst();
  data->lcmux  = MutexOp.inst( NULL, True );
  data->swmap  = MapOp.inst();
  data->swmux  = MutexOp.inst( NULL, True );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ZimoCAN %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid    = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib = %s", wDigInt.getsublib( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  /* choose interface: */
  if( StrOp.equals( wDigInt.sublib_serial, wDigInt.getsublib( ini ) ) ) {
    /* serial */
    data->subConnect    = SerialConnect;
    data->subDisconnect = SerialDisconnect;
    data->subRead       = SerialRead;
    data->subWrite      = SerialWrite;
    data->subAvailable  = SerialAvailable;
  }
  else  {
    /* UDP */
    data->subConnect    = UDPConnect;
    data->subDisconnect = UDPDisconnect;
    data->subRead       = UDPRead;
    data->subWrite      = UDPWrite;
    data->subAvailable  = UDPAvailable;
  }

  data->connOK = data->subConnect((obj)__ZimoCAN);
  data->run = True;

  data->reader = ThreadOp.inst( "zcanreader", &__reader, __ZimoCAN );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( "zcanwriter", &__writer, __ZimoCAN );
  ThreadOp.start( data->writer );


  instCnt++;
  return __ZimoCAN;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/zimocan.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
