/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>
 http://www.rocrail.net

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



#include "rocdigs/impl/cbus_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/CBus.h"
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

#include "rocdigs/impl/cbus/cbusdefs.h"
#include "rocdigs/impl/cbus/serial.h"
#include "rocdigs/impl/cbus/tcp.h"
#include "rocdigs/impl/cbus/flim.h"
#include "rocdigs/impl/cbus/utils.h"

static int instCnt = 0;

static iONode __translate( iOCBUS cbus, iONode node );
static void __broadcastFunction(iOCBUS cbus, iOSlot slot, int fn);

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOCBUSData data = Data(inst);
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

/** ----- OCBUS ----- */


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOCBUSData data = Data(inst);
  iONode rsp = NULL;

  if( cmd != NULL ) {
    int bus = 0;
    rsp = __translate( (iOCBUS)inst, cmd );
    cmd->base.del(cmd);
  }
  return rsp;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return NULL;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOCBUSData data = Data(inst);
  if( poweroff ) {
    byte cmd[2];
    byte* frame = allocMem(32);
    cmd[0] = OPC_RTOF;
    makeFrame(inst, frame, PRIORITY_NORMAL, cmd, 0 );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power OFF" );
    ThreadOp.post(data->writer, (obj)frame);
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  ThreadOp.sleep(200);
  data->run = False;
  ThreadOp.sleep(200);
  data->subDisconnect(inst);
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOCBUSData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _setRawListener( obj inst ,obj listenerObj ,const digint_rawlistener listenerRawFun ) {
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
  return False;
}


static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;
static int _version( obj inst ) {
  iOCBUSData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static int __getOPC(byte* frame) {
  int offset = (frame[1] == 'S') ? 0:4;
  int opc = HEXA2Byte(frame+7+offset);
  return opc;
}


static int __getDataLen(int OPC) {
  if( OPC < 0x20 )
    return 0;
  if( OPC < 0x40 )
    return 1;
  if( OPC < 0x60 )
    return 2;
  if( OPC < 0x80 )
    return 3;
  if( OPC < 0xA0 )
    return 4;
  if( OPC < 0xC0 )
    return 5;
  if( OPC < 0xE0 )
    return 6;
  return 7;
}


static iOSlot __getSlotByAddr(iOCBUSData data, int lcaddr) {
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


static iOSlot __getSlotBySession(iOCBUSData data, int session) {
  iOSlot slot = NULL;
  if( MutexOp.wait( data->lcmux ) ) {
    slot = (iOSlot)MapOp.first( data->lcmap);
    while( slot != NULL ) {
      if( slot->session == session ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot found for %s by session %d", slot->id, session );
        break;
      }
      slot = (iOSlot)MapOp.next( data->lcmap);
    };
    MutexOp.post(data->lcmux);
  }
  return slot;
}


static void __fg2fn(iOCBUS cbus, iOSlot slot, int fg, int fmask) {
  iOCBUSData data = Data(cbus);
  int i = 0;
  Boolean f = False;

  switch( fg ) {
  case 1: /* 0-4 */
    for( i = 0; i < 5; i++ ) {
      f = (fmask & (1 << i)) ? True:False;
      if( f != slot->f[i]) {
        slot->f[i] = f;
        __broadcastFunction(cbus, slot, i );
      }
    }
    break;
  case 2: /* 5-8 */
    for( i = 5; i < 9; i++ ) {
      f = (fmask & (1 << (i-5))) ? True:False;
      if( f != slot->f[i]) {
        slot->f[i] = f;
        __broadcastFunction(cbus, slot, i );
      }
    }
    break;
  case 3: /* 9-12 */
    for( i = 9; i < 13; i++ ) {
      f = (fmask & (1 << (i-9))) ? True:False;
      if( f != slot->f[i]) {
        slot->f[i] = f;
        __broadcastFunction(cbus, slot, i );
      }
    }
    break;
  case 4: /* 13-19 */
    for( i = 13; i < 20; i++ ) {
      f = (fmask & (1 << (i-13))) ? True:False;
      if( f != slot->f[i]) {
        slot->f[i] = f;
        __broadcastFunction(cbus, slot, i );
      }
    }
    break;
  case 5: /* 20-28 */
    for( i = 20; i < 29; i++ ) {
      f = (fmask & (1 << (i-20))) ? True:False;
      if( f != slot->f[i]) {
        slot->f[i] = f;
        __broadcastFunction(cbus, slot, i );
      }
    }
    break;
  }

}


static void __fx2fn(iOSlot slot, int fx) {
  int i = 0;
  for( i = 0; i < 28; i++ ) {
    slot->f[i] = (fx & (1 << i)) ? True:False;
  }
}


static iOSlot __getSlot(iOCBUS cbus, iONode node) {
  iOCBUSData data = Data(cbus);
  int    addr  = wLoc.getaddr(node);
  iOSlot slot  = NULL;
  int    speed = 0;

  slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot exist for %s", wLoc.getid(node) );
    return slot;
  }

  slot = allocMem( sizeof( struct slot) );
  slot->addr    = addr;
  slot->id      = StrOp.dup(wLoc.getid(node));
  slot->steps   = wLoc.getspcnt(node);
  slot->lights  = True;
  slot->dir     = wLoc.isdir(node);
  slot->session = 0;

  __fx2fn(slot, wLoc.getfx(node));


  if( MutexOp.wait( data->lcmux ) ) {
    MapOp.put( data->lcmap, wLoc.getid(node), (obj)slot);
    MutexOp.post(data->lcmux);
  }

  if( wLoc.getV( node ) != -1 ) {
    if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
      speed = (wLoc.getV( node ) * slot->steps) / 100;
    else if( wLoc.getV_max( node ) > 0 )
      speed = (wLoc.getV( node ) * slot->steps) / wLoc.getV_max( node );
  }

  {
    byte cmd[5];
    byte* frame = allocMem(32);
    cmd[0] = OPC_RLOC;
    cmd[1] = addr / 256;
    cmd[2] = addr % 256;
    makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 2 );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "request session for address %d", addr );
    ThreadOp.post(data->writer, (obj)frame);
  }


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for %s", wLoc.getid(node) );
  return slot;
}


static void __broadcastSpeedDir(iOCBUS cbus, iOSlot slot, int speed, Boolean dir) {
  iOCBUSData data = Data(cbus);
  iONode nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "update speed[%d] for loco %s", speed, slot->id );

  slot->rawspeed = speed;
  slot->dir      = dir;

  if( data->iid != NULL )
    wLoc.setiid( nodeC, data->iid );
  wLoc.setid( nodeC, slot->id );
  wLoc.setaddr( nodeC, slot->addr );
  wLoc.setV_raw( nodeC, slot->rawspeed );
  wLoc.setV_rawMax( nodeC, slot->steps );
  wLoc.setfn( nodeC, slot->lights);
  wLoc.setdir( nodeC, slot->dir );
  wLoc.setcmd( nodeC, wLoc.direction );
  wLoc.setthrottleid( nodeC, "cbus" );
  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
}


static void __updateSpeedDir(iOCBUS cbus, byte* frame) {
  iOCBUSData data = Data(cbus);
  int offset  = (frame[1] == 'S') ? 0:4;
  int session = HEXA2Byte(frame + OFFSET_D1 + offset);
  int speed   = HEXA2Byte(frame + OFFSET_D2 + offset);
  Boolean dir = (speed & 0x80) ? True:False;

  iOSlot slot = __getSlotBySession(data, session);

  speed &= 0x7F;

  if( slot != NULL ) {
    __broadcastSpeedDir(cbus, slot, speed, dir);
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unmanaged loco: session %d", session );
  }
}


static void __broadcastFunction(iOCBUS cbus, iOSlot slot, int fn) {
  iOCBUSData data = Data(cbus);

  iONode nodeC = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "update functions for loco %s", slot->id );

  if( data->iid != NULL )
    wLoc.setiid( nodeC, data->iid );
  wFunCmd.setid( nodeC, slot->id );
  wFunCmd.setaddr( nodeC, slot->addr );
  wFunCmd.setfnchanged(nodeC, fn);
  wFunCmd.setgroup( nodeC, fn/4 + ((fn%4 > 0) ? 1:0) );

  wLoc.setthrottleid( nodeC, "cbus" );
  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
}


static void __updateFunction(iOCBUS cbus, byte* frame, Boolean fstate) {
  iOCBUSData data = Data(cbus);
  int offset  = (frame[1] == 'S') ? 0:4;
  int session = HEXA2Byte(frame + OFFSET_D1 + offset);
  int fn      = HEXA2Byte(frame + OFFSET_D2 + offset);

  iOSlot slot = __getSlotBySession(data, session);

  if( slot != NULL ) {
    slot->f[fn] = fstate;
    __broadcastFunction(cbus, slot, fn );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unmanaged loco: session %d", session );
  }
}


static void __updateFunctions(iOCBUS cbus, byte* frame) {
  iOCBUSData data = Data(cbus);
  int offset  = (frame[1] == 'S') ? 0:4;
  int session = HEXA2Byte(frame + OFFSET_D1 + offset);
  int fg      = HEXA2Byte(frame + OFFSET_D2 + offset);
  int fmask   = HEXA2Byte(frame + OFFSET_D3 + offset);

  iOSlot slot = __getSlotBySession(data, session);

  if( slot != NULL ) {
    __fg2fn(cbus, slot, fg, fmask);
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unmanaged loco: session %d", session );
  }
}


/* Frame ASCII format
 * :ShhhhNd0d1d2d3d4d5d6d7d; :XhhhhhhhhNd0d1d2d3d4d5d6d7d;
 */
static void __updateSlot(iOCBUS cbus, byte* frame) {
  iOCBUSData data = Data(cbus);
  int offset  = (frame[1] == 'S') ? 0:4;
  int session = HEXA2Byte(frame + OFFSET_D1 + offset);
  int addrh   = HEXA2Byte(frame + OFFSET_D2 + offset);
  int addrl   = HEXA2Byte(frame + OFFSET_D3 + offset);
  int addr    = addrh * 256 + addrl;

  int speed   =  HEXA2Byte(frame + OFFSET_D4 + offset);
  int f0_4    =  HEXA2Byte(frame + OFFSET_D5 + offset);
  int f5_8    =  HEXA2Byte(frame + OFFSET_D6 + offset);
  int f9_12   =  HEXA2Byte(frame + OFFSET_D7 + offset);

  Boolean dir = (speed & 0x80) ? True:False;

  speed &= speed & 0x7F;

  iOSlot slot = __getSlotByAddr(data, addr);
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "update slot for session %d, loco %s", session, slot->id );
    if( slot->session == 0 ) {
      byte cmd[5];
      byte* frame = allocMem(32);

      if( slot->session == 0 )
        slot->lastkeep = SystemOp.getTick();

      slot->session = session;

      cmd[0] = OPC_DFLG;
      cmd[1] = slot->session;
      if( slot->steps == 128 )
        cmd[2] = TMOD_SPD_128;
      else if( slot->steps == 14 )
        cmd[2] = TMOD_SPD_14;
      else if( slot->steps == 28 )
        cmd[2] = TMOD_SPD_28;
      cmd[2] |= (slot->lights ? 0x04:0x00);
      makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 2 );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "set engine speed steps to %d for loco %s", slot->steps, slot->id );
      ThreadOp.post(data->writer, (obj)frame);

      __broadcastSpeedDir(cbus, slot, speed, dir);
      __fg2fn(cbus, slot, 1, f0_4);
      __fg2fn(cbus, slot, 2, f5_8);
      __fg2fn(cbus, slot, 3, f9_12);


    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unmanaged loco: session %d, address %d", session, addr );
  }
}


static __evaluateFB( iOCBUS cbus, byte* frame, Boolean state ) {
  iOCBUSData data = Data(cbus);

  int offset  = (frame[1] == 'S') ? 0:4;
  int nodeh   = HEXA2Byte(frame + OFFSET_D1 + offset);
  int nodel   = HEXA2Byte(frame + OFFSET_D2 + offset);
  int node    = nodeh * 256 + nodel;

  int eventh   = HEXA2Byte(frame + OFFSET_D3 + offset);
  int eventl   = HEXA2Byte(frame + OFFSET_D4 + offset);
  int event    = eventh * 256 + eventl;

  int addr = event;

  if( !data->shortevents )
    addr += (node << 16);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sevent %d %s (node=%d)",
      data->shortevents?"short ":"", addr, state?"ON":"OFF", node );

  /* inform listener: Node3 */
  iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
  wFeedback.setaddr( nodeC, addr );
  wFeedback.setstate( nodeC, state?True:False );
  if( data->iid != NULL )
    wFeedback.setiid( nodeC, data->iid );

  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
}


static __evaluateErr( iOCBUS cbus, byte* frame ) {
  iOCBUSData data = Data(cbus);
  iONode node = NULL;

  int offset  = (frame[1] == 'S') ? 0:4;
  int addrh   = HEXA2Byte(frame + OFFSET_D1 + offset);
  int addrl   = HEXA2Byte(frame + OFFSET_D2 + offset);
  int err     = HEXA2Byte(frame + OFFSET_D3 + offset);

  int addr = addrh * 256 + addrl;

  const char* errStr = "?";

  switch( err ) {
  case ERR_LOCO_STACK_FULL:
    errStr = "loco stack full";
    break;
  case ERR_LOCO_ADDR_TAKEN:
    errStr = "loco address taken";
    break;
  case ERR_SESSION_NOT_PRESENT:
    errStr = "session not present";
    break;
  case ERR_NO_MORE_ENGINES:
    errStr = "no more engines";
    break;
  case ERR_ENGINE_NOT_FOUND:
    errStr = "engine not found";
    break;
  }

  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "error=%d \"%s\" for address %d", err, errStr, addr );

}


static __evaluateCmdErr( iOCBUS cbus, byte* frame ) {
  iOCBUSData data = Data(cbus);
  iONode node = NULL;

  int offset  = (frame[1] == 'S') ? 0:4;
  int addrh   = HEXA2Byte(frame + OFFSET_D1 + offset);
  int addrl   = HEXA2Byte(frame + OFFSET_D2 + offset);
  int err     = HEXA2Byte(frame + OFFSET_D3 + offset);

  int addr = addrh * 256 + addrl;

  const char* errStr = "?";

  /*
#define CMDERR_INV_CMD      1
#define CMDERR_NOT_LRN      2
#define CMDERR_NOT_SETUP    3
#define CMDERR_TOO_MANY_EVS   4
#define CMDERR_NO_EV      5
#define CMDERR_INV_EV_IDX   6
#define CMDERR_NO_EVENTS    7
#define CMDERR_INV_EN_IDX   8
#define CMDERR_INV_PARAM_IDX  9
*/

  switch( err ) {
  case CMDERR_INV_CMD:
    errStr = "invalid command";
    break;
  case CMDERR_NOT_LRN:
    errStr = "not in learn";
    break;
  case CMDERR_NOT_SETUP:
    errStr = "not in setup";
    break;
  case CMDERR_TOO_MANY_EVS:
    errStr = "too many events";
    break;
  case CMDERR_NO_EV:
  case CMDERR_NO_EVENTS:
    errStr = "no events";
    break;
  case CMDERR_INV_EV_IDX:
  case CMDERR_INV_EN_IDX:
    errStr = "invalid event index";
    break;
  case CMDERR_INV_PARAM_IDX:
    errStr = "invalid parameter index";
    break;
  }

  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "error=%d \"%s\" for node %d", err, errStr, addr );

}


static __evaluateCV( iOCBUS cbus, byte* frame ) {
  iOCBUSData data = Data(cbus);
  iONode node = NULL;

  int offset  = (frame[1] == 'S') ? 0:4;
  int cvh   = HEXA2Byte(frame + OFFSET_D2 + offset);
  int cvl   = HEXA2Byte(frame + OFFSET_D3 + offset);
  int cv    = cvh * 256 + cv;

  int value = HEXA2Byte(frame + OFFSET_D4 + offset);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cv %d has a value of %d", cv, value );

  node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setcv( node, cv );
  wProgram.setvalue( node, value );
  wProgram.setcmd( node, wProgram.datarsp );
  if( data->iid != NULL )
    wProgram.setiid( node, data->iid );

  if( data->listenerFun != NULL && data->listenerObj != NULL )
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
}


/*
    E3 RFID tag read (RFID)

    Format: <E3><NNhigh><NNlow><RFID4><RFID3><RFID2><RFID1><RFID0>

    The RFID read event, contains the RFID tag value (Big Endian).

    EM4102 format readers return a 40 bit number when a tag is read.
    Although read only tags are usually quoted as being 64 bits,
    the unique tag number is only 40 bits, the other bits are used
    for the header and for row and column parity.
 */
static __evaluateRFID( iOCBUS cbus, byte* frame ) {
  iOCBUSData data = Data(cbus);

  int offset  = (frame[1] == 'S') ? 0:4;
  int addrh   = HEXA2Byte(frame + OFFSET_D1 + offset);
  int addrl   = HEXA2Byte(frame + OFFSET_D2 + offset);
  int addr    = addrh * 256 + addrl;

  int rfid1  = HEXA2Byte(frame + OFFSET_D3 + offset);
  int rfid2  = HEXA2Byte(frame + OFFSET_D4 + offset);
  int rfid3  = HEXA2Byte(frame + OFFSET_D5 + offset);
  int rfid4  = HEXA2Byte(frame + OFFSET_D6 + offset);
  int rfid5  = HEXA2Byte(frame + OFFSET_D7 + offset);

  long rfid = rfid5 * 0x1 + rfid4 * 0x100 + rfid3 * 0x10000 + rfid2 * 0x1000000 + rfid1 * 0x100000000;

  Boolean state = False;

  /* inform listener: Node3 */
  iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
  wFeedback.setaddr( nodeC, addr );
  wFeedback.setstate( nodeC, state?True:False );
  wFeedback.setidentifier( nodeC, rfid );
  if( data->iid != NULL )
    wFeedback.setiid( nodeC, data->iid );

  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
}



static void __reportState(iOCBUS cbus) {
  iOCBUSData data = Data(cbus);

  if( data->listenerFun != NULL && data->listenerObj != NULL ) {
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );

    if( data->iid != NULL )
      wState.setiid( node, data->iid );
    wState.setpower( node, data->power );
    wState.settrackbus( node, data->buson );

    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}


static iONode __evaluateXFrame(iOCBUS cbus, byte* frame) {
  iOCBUSData data = Data(cbus);
  if( frame[2] == '0' ) {
    /* Commands:
    :X00080004N000000000D000000; NOP
    :X00080004N000000000D100000; Reset
    :X00080004N000000000D200000; Init
    :X00080004N000000000D300000; Check
    :X00080004N000000000D400000; Test
    :X00080005N0000000000000000; Data
    */
  }

  else if( frame[2] == '8' ) {
    /* Responses:
    :X80080004N00; Error, on response of a Check command.
    :X80080004N01; OK, on response of a Check command.
    :X80080004N02; Boot mode confirm, on response of a Test command.
    */
    /* ToDo: Generate program nodes. */
    if(frame[12] == '0') {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "error response from bootloader" );
    }
    else if(frame[12] == '1') {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "OK response from bootloader" );
    }
    else if(frame[12] == '2') {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "boot mode confirmed" );
    }
  }

  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unknown extended frame" );
  }

  return NULL;
}


static iONode __evaluateFrame(iOCBUS cbus, byte* frame, int opc) {
  iOCBUSData data = Data(cbus);
  int offset = (frame[1] == 'S') ? 0:4;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "evaluate OPC=0x%02X", opc );

  if( frame[1] == 'S' ) {
    switch(opc) {
    case OPC_ERR:
      __evaluateErr(cbus, frame);
      break;
    case OPC_CMDERR:
      __evaluateCmdErr(cbus, frame);
      break;
    case OPC_PLOC:
      __updateSlot(cbus, frame);
      break;
    case OPC_DSPD:
      __updateSpeedDir(cbus, frame);
      break;
    case OPC_FNON:
      __updateFunction(cbus, frame, True);
      break;
    case OPC_FNOF:
      __updateFunction(cbus, frame, False);
      break;
    case OPC_DFUN:
      __updateFunctions(cbus, frame);
      break;
    case OPC_ACON:
    case OPC_ASON:
    case OPC_ARSPO:
      __evaluateFB(cbus, frame, True);
      break;
    case OPC_ACOF:
    case OPC_ASOF:
    case OPC_ARSPN:
      __evaluateFB(cbus, frame, False);
      break;
    case OPC_ACDAT:
      __evaluateRFID(cbus, frame);
      break;
    case OPC_PCVS:
      __evaluateCV(cbus, frame);
      break;
    case OPC_TON:
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "power ON" );
      data->power = True;
      __reportState(cbus);
      break;
    case OPC_TOF:
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "power OFF" );
      data->power = False;
      __reportState(cbus);
      break;
    case OPC_ESTOP:
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Emergency break!" );
      break;
    case OPC_HLT:
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "bus halt" );
      data->buson = False;
      __reportState(cbus);
      break;
    case OPC_BON:
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "bus on" );
      data->buson = True;
      __reportState(cbus);
      break;
    case OPC_STAT:
      {
        int offset  = (frame[1] == 'S') ? 0:4;
        int status   = HEXA2Byte(frame + OFFSET_D1 + offset);
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "status 0x%02X", status );
        break;
      }

    /* All FLiM OPCs to this case. */
    case OPC_NNACK:
    case OPC_NNREL:
    case OPC_NNREF:
    case OPC_NNLRN:
    case OPC_NNULN:
    case OPC_NNCLR:
    case OPC_NNEVN:
    case OPC_NERD:
    case OPC_RQEVN:
    case OPC_WRACK:
    case OPC_RQDAT:
    case OPC_BOOT:
    case OPC_RQNN:
    case OPC_RQNP:
    case OPC_SNN:
    case OPC_NVRD:
    case OPC_NENRD:
    case OPC_RQNPN:
    case OPC_NUMEV:
    case OPC_EVULN:
    case OPC_NVSET:
    case OPC_NVANS:
    case OPC_PARAN:
    case OPC_REVAL:
    case OPC_REQEV:
    case OPC_NEVAL:
    case OPC_EVLRN:
    case OPC_EVANS:
    case OPC_PARAMS:
    case OPC_ENRSP:
    case OPC_EVLRNI:
      {
        byte* extraMsg = NULL;
        iONode rsp = processFLiM((obj)cbus, opc, frame, &extraMsg);
        if( rsp != NULL ) {
          if( data->listenerFun != NULL && data->listenerObj != NULL )
            data->listenerFun( data->listenerObj, rsp, TRCLEVEL_INFO );
        }
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "extraMsg=0x%08X", extraMsg );
        if( extraMsg != NULL ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "extra frame from flim..." );
          ThreadOp.post(data->writer, (obj)extraMsg);
        }
        break;
      }
    }
  }
  else {
    /* Extended frame: boot loader */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Extended frame: boot loader..." );
  }


  return NULL;
}


static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOCBUS cbus = (iOCBUS)ThreadOp.getParm( th );
  iOCBUSData data = Data(cbus);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader started." );

  ThreadOp.sleep(1000);

  /* get all input states */
  if( data->sodaddr > 0 ) {
    byte cmd[8];
    byte* frame = allocMem(64);
    cmd[0] = OPC_ASRQ;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = data->sodaddr / 256;
    cmd[4] = data->sodaddr % 256;
    makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 4 );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "query input state" );
    ThreadOp.post(data->writer, (obj)frame);
  }

  if( 1 ) {
    byte cmd[8];
    byte* frame = allocMem(64);
    cmd[0] = OPC_RSTAT;
    makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 0 );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "query cs state" );
    ThreadOp.post(data->writer, (obj)frame);
  }

  while( data->run ) {
    byte frame[32] = {0};
    /* Frame ASCII format
     * :ShhhhNd0d1d2d3d4d5d6d7d; :XhhhhhhhhNd0d1d2d3d4d5d6d7d; :ShhhhR; :SB020N;
     * :S    -> S=Standard X=extended start CAN Frame
     * hhhh  -> SIDH<bit7,6,5,4=Prio bit3,2,1,0=high 4 part of ID> SIDL<bit7,6,5=low 3 part of ID>
     * Nd    -> N=normal R=RTR
     * 0d    -> OPC 2 byte HEXA
     * 1d-7d -> data 2 byte HEXA
     * ;     -> end of frame
     */

    if( data->subAvailable( (obj)cbus) ) {
      if( data->subRead( (obj)cbus, frame, 1) ) {
        if( frame[0] == ':' ) {
          if( data->subRead( (obj)cbus, frame+1, 1) ) {
            if( frame[1] == 'S' ) {
              int offset = 0;

              if( data->subRead( (obj)cbus, frame + 2, OFFSET_TYPE + offset ) ) {
                if( frame[OFFSET_TYPE+offset] == 'N' ) {
                  if( frame[OFFSET_TYPE+offset+1] == ';' ) {
                    /* some kind of empty frame... */
                    TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, 1 + OFFSET_TYPE + offset + 1 );
                  }
                  else {
                    if( data->subRead( (obj)cbus, frame + 2 + OFFSET_TYPE + offset, OFFSET_OPC - OFFSET_TYPE ) ) {
                      int opc = __getOPC(frame);
                      int datalen = __getDataLen(opc);
                      TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, 2 + OFFSET_OPC + offset );
                      if( data->subRead( (obj)cbus, frame + 2 + OFFSET_OPC + offset, datalen*2 + 1 ) ) {
                        TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, 2 + OFFSET_OPC + offset + datalen*2 + 1 );
                        __evaluateFrame(cbus, frame, opc);
                      }
                    }
                  }
                }
                else if( frame[OFFSET_TYPE+offset] == 'R' ) {
                  int n = 1;
                  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "reading frame type R" );
                  if( frame[OFFSET_TYPE+offset+n] == ';' ) {
                    TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, 1 + OFFSET_TYPE + offset + n );
                  }
                  else {
                    while( data->subRead( (obj)cbus, frame + 1 + OFFSET_TYPE + offset + n, 1 ) && n < 20 ) {
                      if( frame[OFFSET_TYPE+offset+n] == ';' ) {
                        TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, 1 + OFFSET_TYPE + offset + n );
                        break;
                      }
                      else {
                        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "read byte [%c] for frame type R at index[%d]",
                            frame[OFFSET_TYPE+offset+n], OFFSET_TYPE+offset+n );
                      }
                      n++;
                    }
                  }
                }
                else {
                  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unknown frame type: [%c]", frame[OFFSET_TYPE+offset] );
                  TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, 2 + OFFSET_TYPE + offset );
                }
              }
            }

            /* extended frame */
            else if( frame[1] == 'X' ) {
              int n = 2;
              TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "reading extended frame" );
              while( data->subRead( (obj)cbus, frame + n, 1 ) && n < 32 ) {
                if( frame[n] == ';' ) {
                  frame[n+1] = '\0';
                  TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, n+1 );
                  __evaluateXFrame(cbus, frame);
                  break;
                }
                else {
                  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "read byte [%c] for extended frame at index[%d]",
                      frame[n], n );
                }
                n++;
              }
            }

            else {
              /* junk */
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "wrong frame byte [%c]", frame[1] );
            }
          }
        }
        else {
          /* junk */
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no start byte [%c]", frame[0] );
        }
      }
    }

    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader ended." );
}




static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOCBUS cbus = (iOCBUS)ThreadOp.getParm( th );
  iOCBUSData data = Data(cbus);
  byte* cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer started." );
  while( data->run ) {
    byte * post = NULL;
    int len = 0;
    byte out[64] = {0};

    ThreadOp.sleep(10);
    if( data->buson ) {
      post = (byte*)ThreadOp.getPost( th );

      if (post != NULL) {
        /* first byte is the message length */
        len = post[0];
        MemOp.copy( out, post+1, len);
        freeMem( post);

        TraceOp.dump( name, TRCLEVEL_BYTE, (char*)out, len );
        if( !data->subWrite((obj)cbus, out, len) ) {
          /* sleep and send it again? */
        }
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer ended." );

}


static void __keep( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOCBUS cbus = (iOCBUS)ThreadOp.getParm( th );
  iOCBUSData data = Data(cbus);
  byte* cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "keeper started." );
  ThreadOp.sleep(1000);

  while( data->run ) {
    if( MutexOp.wait( data->lcmux ) ) {
      iOSlot slot = (iOSlot)MapOp.first( data->lcmap);
      while( slot != NULL ) {
        long tick = SystemOp.getTick();
        if( slot->session > 0  && ( (tick - slot->lastkeep) > (data->purgetime*90) ) && data->purgetime > 0  ) {
          byte cmd[5];
          byte* frame = allocMem(32);

          cmd[0] = OPC_KEEP;
          cmd[1] = slot->session;
          makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 1 );

          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
              "keep session %d for %s: tick=%ld, lastkeep=%ld, purgetime=%d",
              slot->session, slot->id, tick, slot->lastkeep, data->purgetime*90 );
          ThreadOp.post(data->writer, (obj)frame);

          slot->lastkeep = tick;
        }
        slot = (iOSlot)MapOp.next( data->lcmap);
      };
      MutexOp.post(data->lcmux);
    }

    ThreadOp.sleep(100);
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "keeper ended." );

}


static void __timedqueue( void* threadinst ) {
  iOThread     th = (iOThread)threadinst;
  iOCBUS     cbus = (iOCBUS)ThreadOp.getParm(th);
  iOCBUSData data = Data(cbus);

  iOList list = ListOp.inst();
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timed queue started" );

  while( data->run ) {
    iQCmd cmd = (iQCmd)ThreadOp.getPost( th );
    if (cmd != NULL) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "new timed command time=%d delay=%d tick=%d frame=%s",
          cmd->time, cmd->delay, SystemOp.getTick(), cmd->out+1 );
      ListOp.add(list, (obj)cmd);
    }

    int i = 0;
    for( i = 0; i < ListOp.size(list); i++ ) {
      iQCmd cmd = (iQCmd)ListOp.get(list, i);
      if( cmd->wait4session ) {
        /* Wait for session number. */
        if( cmd->slot->session > 0 ) {
          byte* outa = allocMem(32);
          /* :SCFE0N47008C; */
          Byte2HEXA( cmd->out+10, cmd->slot->session);
          MemOp.copy( outa, cmd->out, 32 );
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
              "got session nr %d: sending frame %s for %s",
              cmd->slot->session, cmd->out+1, cmd->slot->id );
          ThreadOp.post( data->writer, (obj)outa );
          ListOp.removeObj(list, (obj)cmd);
          freeMem(cmd);
          break;
        }
      }
      else if( (cmd->time + cmd->delay) <= SystemOp.getTick() ) {
        byte* outa = allocMem(32);
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




Boolean __getFState(iONode fcmd, int fn) {
  switch( fn ) {
    case 0 : return wFunCmd.isf0 (fcmd);
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


static void __makeDFUN(iOSlot slot, iONode node, byte* cmd) {
  int fnchanged = wFunCmd.getfnchanged(node);
  int i = 0;
  int fmask = 0;

  cmd[0] = OPC_DFUN;
  if( fnchanged < 5 ) {
    cmd[1] = 1;
    for( i = 0; i < 5; i++ ) {
      if( __getFState(node, i) )
        fmask |= (1 << i);
    }
    cmd[2] = fmask;
  }
  else if( fnchanged < 9 ) {
    cmd[1] = 2;
    for( i = 5; i < 9; i++ ) {
      if( __getFState(node, i) )
        fmask |= (1 << (i-5));
    }
    cmd[2] = fmask;
  }
  else if( fnchanged < 13 ) {
    cmd[1] = 3;
    for( i = 9; i < 13; i++ ) {
      if( __getFState(node, i) )
        fmask |= (1 << (i-9));
    }
    cmd[2] = fmask;
  }
  else if( fnchanged < 20 ) {
    cmd[1] = 4;
    for( i = 13; i < 20; i++ ) {
      if( __getFState(node, i) )
        fmask |= (1 << (i-13));
    }
    cmd[2] = fmask;
  }
  else if( fnchanged < 29 ) {
    cmd[1] = 5;
    for( i = 20; i < 29; i++ ) {
      if( __getFState(node, i) )
        fmask |= (1 << (i-20));
    }
    cmd[2] = fmask;
  }
}


static iONode __translate( iOCBUS cbus, iONode node ) {
  iOCBUSData data = Data(cbus);
  iONode rsp = NULL;

  if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      /* CS off */
      byte cmd[2];
      byte* frame = allocMem(32);
      cmd[0] = OPC_RTOF;
      makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 0 );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "request power OFF" );
      ThreadOp.post(data->writer, (obj)frame);
    }

    else if( StrOp.equals( cmdstr, wSysCmd.ebreak ) ) {
      /* CS ebreak */
      byte cmd[2];
      byte* frame = allocMem(32);
      cmd[0] = OPC_RESTP;
      makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 0 );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "request emergency break" );
      ThreadOp.post(data->writer, (obj)frame);
    }

    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS on */
      byte cmd[2];
      byte* frame = allocMem(32);
      cmd[0] = OPC_RTON;
      makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 0 );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "request power ON" );
      ThreadOp.post(data->writer, (obj)frame);
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    byte cmd[5];
    byte* frame = allocMem(32);
    int delay = wSwitch.getdelay(node) > 0 ? wSwitch.getdelay(node):data->swtime;

    cmd[0] = OPC_ACON;
    cmd[1] = wSwitch.getaddr1( node ) / 256;
    cmd[2] = wSwitch.getaddr1( node ) % 256;
    cmd[3] = wSwitch.getport1( node ) / 256;
    cmd[4] = wSwitch.getport1( node ) % 256;
    makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 4 );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch %d:%d", wSwitch.getaddr1( node ), wSwitch.getport1( node ) );
    ThreadOp.post(data->writer, (obj)frame);

    /* deactivate the gate to be used */
    iQCmd qcmd = allocMem(sizeof(struct QCmd));
    qcmd->time   = SystemOp.getTick();
    qcmd->delay  = delay / 10;
    cmd[0] = OPC_ACOF;
    cmd[1] = wSwitch.getaddr1( node ) / 256;
    cmd[2] = wSwitch.getaddr1( node ) % 256;
    cmd[3] = wSwitch.getport1( node ) / 256;
    cmd[4] = wSwitch.getport1( node ) % 256;
    makeFrame((obj)cbus, qcmd->out, PRIORITY_NORMAL, cmd, 4 );
    ThreadOp.post( data->timedqueue, (obj)qcmd );

  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    byte cmd[5];
    byte* frame = allocMem(32);
    Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

    cmd[0] = on ? OPC_ACON:OPC_ACOF;
    cmd[1] = wOutput.getaddr( node ) / 256;
    cmd[2] = wOutput.getaddr( node ) % 256;
    cmd[3] = wOutput.getport( node ) / 256;
    cmd[4] = wOutput.getport( node ) % 256;
    makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 4 );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "output %d:%d %s",
        wOutput.getaddr( node ), wOutput.getport( node ), on?"ON":"OFF" );
    ThreadOp.post(data->writer, (obj)frame);

  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );
  }


  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    byte cmd[5];
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    Boolean fn  = wLoc.isfn( node );
    Boolean dir = wLoc.isdir( node ); /* True == forwards */

    iOSlot slot = __getSlot(cbus, node );

    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get slot for loco %s", wLoc.getid(node) );
      return;
    }

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * slot->steps) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * slot->steps) / wLoc.getV_max( node );
    }

    slot->speed  = speed;
    slot->dir    = wLoc.isdir(node);
    slot->lights = wLoc.isfn(node);


    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco speed=%d dir=%s", slot->speed, slot->dir?"forwards":"reverse" );

    if( slot->session > 0 ) {
      byte* frame = allocMem(32);
      cmd[0] = OPC_DSPD;
      cmd[1] = slot->session;
      cmd[2] = speed | (slot->dir ? 0x80:0x00);
      makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 2 );
      slot->lastkeep = SystemOp.getTick();
      ThreadOp.post(data->writer, (obj)frame);
    }
    else {
      iQCmd qcmd = allocMem(sizeof(struct QCmd));
      qcmd->wait4session  = True;
      qcmd->slot = slot;
      cmd[0] = OPC_DSPD;
      cmd[1] = slot->session;
      cmd[2] = speed | (slot->dir ? 0x80:0x00);
      makeFrame((obj)cbus, qcmd->out, PRIORITY_NORMAL, cmd, 2 );
      ThreadOp.post( data->timedqueue, (obj)qcmd );
    }

  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    byte cmd[5];
    int     addr      = wFunCmd.getaddr( node );
    int     fnchanged = wFunCmd.getfnchanged(node);
    Boolean fstate    = __getFState(node, fnchanged);

    iOSlot slot = __getSlot(cbus, node );

    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get slot for loco %s", wLoc.getid(node) );
      return;
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco fn=%d state=%s", fnchanged, fstate?"ON":"OFF" );

    if( slot->session > 0 ) {
      byte* frame = allocMem(32);
      if( data->fonfof ) {
        cmd[0] = fstate?OPC_FNON:OPC_FNOF;
        cmd[1] = slot->session;
        cmd[2] = fnchanged;
        makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 2 );
      }
      else {
        __makeDFUN(slot, node, cmd);
        makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 3 );
      }
      slot->lastkeep = SystemOp.getTick();
      ThreadOp.post(data->writer, (obj)frame);
    }
    else {
      byte fmask = 0;
      iQCmd qcmd = allocMem(sizeof(struct QCmd));
      qcmd->wait4session  = True;
      qcmd->slot = slot;
      if( data->fonfof ) {
        cmd[0] = fstate?OPC_FNON:OPC_FNOF;
        cmd[1] = slot->session;
        cmd[2] = fnchanged;
        makeFrame((obj)cbus, qcmd->out, PRIORITY_NORMAL, cmd, 2 );
      }
      else {
        __makeDFUN(slot, node, cmd);
        makeFrame((obj)cbus, qcmd->out, PRIORITY_NORMAL, cmd, 3 );
      }
      ThreadOp.post( data->timedqueue, (obj)qcmd );
    }

  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    Boolean direct = wProgram.isdirect(node);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "program type %d...", wProgram.getlntype(node) );


    if( wProgram.getlntype(node) == wProgram.lntype_cbus ) {
      byte* frame = programFLiM((obj)cbus, node );
      if( frame != NULL ) {
        ThreadOp.post(data->writer, (obj)frame);
      }
    }
    else if( wProgram.getcmd( node ) == wProgram.get ) {
      int cv = wProgram.getcv( node );
      int addr = wProgram.getaddr( node );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get CV%d on %s...", cv, wProgram.ispom(node)?"POM":"PT" );

      if( wProgram.ispom(node) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*** not supported *** POM: read CV%d of loc %d...", cv, addr );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "read CV%d of loc %d...", cv, addr );
        byte cmd[5];
        byte* frame = allocMem(32);
        Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

        cmd[0] = OPC_QCVS;
        cmd[1] = 0;
        cmd[2] = cv / 256;
        cmd[3] = cv % 256;
        cmd[4] = direct?CVMODE_DIRECTBYTE:CVMODE_PAGE; /* Programming mode; Default is paged. */
        makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 4 );

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "output %d:%d %s",
            wOutput.getaddr( node ), wOutput.getport( node ), on?"ON":"OFF" );
        ThreadOp.post(data->writer, (obj)frame);
      }
    }

    else if( wProgram.getcmd( node ) == wProgram.set ) {
      int cv = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int decaddr = wProgram.getdecaddr( node );


      if( wProgram.ispom(node) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "POM: set CV%d of loc %d to %d...", cv, decaddr, value );
        byte cmd[10];
        byte* frame = allocMem(32);
        Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

        cmd[0] = OPC_WCVOA;
        cmd[1] = decaddr / 256;
        cmd[2] = decaddr % 256;
        cmd[3] = cv / 256;
        cmd[4] = cv % 256;
        cmd[5] = direct?CVMODE_DIRECTBYTE:CVMODE_PAGE; /* Programming mode; Default is paged. */
        cmd[6] = value;
        makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 6 );

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "output %d:%d %s",
            wOutput.getaddr( node ), wOutput.getport( node ), on?"ON":"OFF" );
        ThreadOp.post(data->writer, (obj)frame);
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set CV%d of loc %d to %d...", cv, decaddr, value );
        byte cmd[10];
        byte* frame = allocMem(32);
        Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

        cmd[0] = OPC_WCVS;
        cmd[1] = 0;
        cmd[2] = cv / 256;
        cmd[3] = cv % 256;
        cmd[4] = direct?CVMODE_DIRECTBYTE:CVMODE_PAGE; /* Programming mode; Default is paged. */
        cmd[5] = value;
        makeFrame((obj)cbus, frame, PRIORITY_NORMAL, cmd, 5 );

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "output %d:%d %s",
            wOutput.getaddr( node ), wOutput.getport( node ), on?"ON":"OFF" );
        ThreadOp.post(data->writer, (obj)frame);
      }
    }
    else if(  wProgram.getcmd( node ) == wProgram.pton ) {
    }
    else if( wProgram.getcmd( node ) == wProgram.ptoff ) {
    }

    /* FLiM: NV0 = NN */
    else if( wProgram.getcmd( node ) == wProgram.nvget ) {
      /* Node Value get */
      int nv  = wProgram.getcv( node );
      int val = wProgram.getvalue( node );
      int nn  = wProgram.getdecaddr( node );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get NN%d NV%d...", nn, nv );
    }
    else if( wProgram.getcmd( node ) == wProgram.nvset ) {
      /* Node Value set */
      int nv  = wProgram.getcv( node );
      int val = wProgram.getvalue( node );
      int nn  = wProgram.getdecaddr( node );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set NN%d NV%d to %d...", nn, nv, val );
    }

  }

  return rsp;
}


/**  */
static struct OCBUS* _inst( const iONode ini ,const iOTrace trc ) {
  iOCBUS __CBUS = allocMem( sizeof( struct OCBUS ) );
  iOCBUSData data = allocMem( sizeof( struct OCBUSData ) );
  MemOp.basecpy( __CBUS, &CBUSOp, 0, sizeof( struct OCBUS ), data );

  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */

  data->ini     = ini;
  data->cbusini = wDigInt.getcbus(ini);
  if(data->cbusini == NULL) {
    data->cbusini = NodeOp.inst( wCBus.name(), ini, ELEMENT_NODE );
    NodeOp.addChild( data->ini, data->cbusini );
  }

  data->iid         = StrOp.dup( wDigInt.getiid( ini ) );
  data->cid         = wCBus.getcid(data->cbusini);
  data->sodaddr     = wCBus.getsodaddr(data->cbusini);
  data->shortevents = wCBus.isshortevents(data->cbusini);
  data->fonfof      = wCBus.isfonfof(data->cbusini);
  data->purgetime   = wCBus.getpurgetime(data->cbusini);
  data->run         = True;
  data->device      = wDigInt.getdevice( data->ini );
  data->swtime      = wDigInt.getswtime( ini );

  data->run      = True;
  data->buson    = True;
  data->mux      = MutexOp.inst( NULL, True );
  data->lcmux    = MutexOp.inst( NULL, True );
  data->lcmap    = MapOp.inst();
  data->loaderMux= MutexOp.inst( NULL, True );

  if( StrOp.equals( wDigInt.sublib_usb, wDigInt.getsublib(data->ini) ))
    data->bps = 500000;
  else
    data->bps = 115200;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MERG CBUS %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "http://www.merg.org.uk" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid           = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "canid         = %d", data->cid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sod           = %d", data->sodaddr );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "short events  = %s", data->shortevents ? "yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "use FON/FOF   = %s", data->fonfof ? "yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib        = %s", wDigInt.getsublib(data->ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switchtime    = %d", data->swtime );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "purgetime     = %d", data->purgetime );


  /* choose interface: */
  if( StrOp.equals( wDigInt.sublib_tcp, wDigInt.getsublib( ini ) ) ) {
    /* tcp/ip */
    data->subConnect    = tcpConnect;
    data->subDisconnect = tcpDisconnect;
    data->subRead       = tcpRead;
    data->subWrite      = tcpWrite;
    data->subAvailable  = tcpAvailable;
  }
  else {
    /* usb or serial */
    data->subConnect    = serialConnect;
    data->subDisconnect = serialDisconnect;
    data->subRead       = serialRead;
    data->subWrite      = serialWrite;
    data->subAvailable  = serialAvailable;
  }

  data->connOK = data->subConnect((obj)__CBUS);

  if( data->connOK ) {

    data->reader = ThreadOp.inst( "cbreader", &__reader, __CBUS );
    ThreadOp.start( data->reader );
    data->writer = ThreadOp.inst( "cbwriter", &__writer, __CBUS );
    ThreadOp.start( data->writer );
    data->timedqueue = ThreadOp.inst( "cbtimedq", &__timedqueue, __CBUS );
    ThreadOp.start( data->timedqueue );
    data->keep = ThreadOp.inst( "keeper", &__keep, __CBUS );
    ThreadOp.start( data->keep );

  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init CBUS connection!" );
  }


  instCnt++;
  return __CBUS;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/cbus.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
