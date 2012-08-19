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
#include "rocrail/wrapper/public/Accessory.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/Text.h"

#include "rocdigs/impl/cbus/cbusdefs.h"
#include "rocdigs/impl/cbus/rocrail.h"
#include "rocdigs/impl/cbus/serial.h"
#include "rocdigs/impl/cbus/tcp.h"
#include "rocdigs/impl/cbus/flim.h"
#include "rocdigs/impl/cbus/utils.h"
#include "rocdigs/impl/cbus/cbusmon.h"
#include "rocdigs/impl/cbus/cbuscmd.h"

#include <time.h>

static int instCnt = 0;

static iONode __translate( iOCBUS cbus, iONode node );
static void __broadcastFunction(iOCBUS cbus, int session, iOSlot slot, int fn, Boolean on);
static void __releaseSessions(iOCBUS cbus);

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

  if( cmd != NULL )
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "cbus cmd = %s", NodeOp.getName(cmd) );
  if( !data->bootmode && cmd != NULL ) {
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
  __releaseSessions((iOCBUS)inst);
  if( poweroff ) {
    byte cmd[2];
    byte* frame = allocMem(32);
    cmd[0] = OPC_RTOF;
    makeFrame(frame, PRIORITY_NORMAL, cmd, 0, data->cid, False );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "power OFF" );
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
  int offset = (frame[1] != 'X') ? 0:4;
  int opc = HEXA2Byte(frame+7+offset);
  return opc;
}


static int __getDataLen(int OPC, byte frametype) {
  if( frametype != 'Y' ) {
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
  else {
    if( OPC == 1 )
      return 4;
    if( OPC == 2 )
      return 3;
  }
  return 0;
}

static void __QueryLoco(iOCBUSData data, int session) {
  // OPC_QLOC
  byte cmd[4];
  byte* frame = allocMem(32);
  cmd[0] = OPC_QLOC;
  cmd[1] = session;
  makeFrame(frame, PRIORITY_NORMAL, cmd, 1, data->cid, False );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "query loco for session %d", session );
  ThreadOp.post(data->writer, (obj)frame);

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


static void __fg2fn(iOCBUS cbus, int session, iOSlot slot, int fg, int fmask) {
  iOCBUSData data = Data(cbus);
  int i = 0;
  Boolean f = False;
  iONode nodeC = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
  wLoc.setthrottleid( nodeC, "cbus" );
  if( data->iid != NULL )
    wLoc.setiid( nodeC, data->iid );

  if( slot != NULL ) {
    wFunCmd.setid( nodeC, slot->id );
    wFunCmd.setaddr( nodeC, slot->addr );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "update functions(0x%02X) in group %d for loco %s", fmask, fg, slot->id );
  }
  else {
    wFunCmd.setaddr( nodeC, data->cab[session] );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "update functions(0x%02X) in group %d for session %d", fmask, fg, session );
  }

  wFunCmd.setgroup( nodeC, fg );


  switch( fg ) {
  case 1: /* 0-4 */
    __broadcastFunction(cbus, session, slot, 0, (fmask & 0x10) ? True:False);
    wFunCmd.setf0(nodeC, (fmask & 0x10) ? True:False);
    wFunCmd.setf1(nodeC, (fmask & 0x01) ? True:False);
    wFunCmd.setf2(nodeC, (fmask & 0x02) ? True:False);
    wFunCmd.setf3(nodeC, (fmask & 0x04) ? True:False);
    wFunCmd.setf4(nodeC, (fmask & 0x08) ? True:False);
    break;
  case 2: /* 5-8 */
    wFunCmd.setf5(nodeC, (fmask & 0x01) ? True:False);
    wFunCmd.setf6(nodeC, (fmask & 0x02) ? True:False);
    wFunCmd.setf7(nodeC, (fmask & 0x04) ? True:False);
    wFunCmd.setf8(nodeC, (fmask & 0x08) ? True:False);
    break;
  case 3: /* 9-12 */
    wFunCmd.setf9(nodeC, (fmask & 0x01) ? True:False);
    wFunCmd.setf10(nodeC, (fmask & 0x02) ? True:False);
    wFunCmd.setf11(nodeC, (fmask & 0x04) ? True:False);
    wFunCmd.setf12(nodeC, (fmask & 0x08) ? True:False);
    break;
  case 4: /* 13-20 */
    wFunCmd.setf13(nodeC, (fmask & 0x01) ? True:False);
    wFunCmd.setf14(nodeC, (fmask & 0x02) ? True:False);
    wFunCmd.setf15(nodeC, (fmask & 0x04) ? True:False);
    wFunCmd.setf16(nodeC, (fmask & 0x08) ? True:False);
    wFunCmd.setf17(nodeC, (fmask & 0x10) ? True:False);
    wFunCmd.setf18(nodeC, (fmask & 0x20) ? True:False);
    wFunCmd.setf19(nodeC, (fmask & 0x40) ? True:False);
    wFunCmd.setf20(nodeC, (fmask & 0x80) ? True:False);
    break;
  case 5: /* 21-28 */
    wFunCmd.setgroup( nodeC, 6 );
    wFunCmd.setf21(nodeC, (fmask & 0x01) ? True:False);
    wFunCmd.setf22(nodeC, (fmask & 0x02) ? True:False);
    wFunCmd.setf23(nodeC, (fmask & 0x04) ? True:False);
    wFunCmd.setf24(nodeC, (fmask & 0x08) ? True:False);
    wFunCmd.setf25(nodeC, (fmask & 0x10) ? True:False);
    wFunCmd.setf26(nodeC, (fmask & 0x20) ? True:False);
    wFunCmd.setf27(nodeC, (fmask & 0x40) ? True:False);
    wFunCmd.setf28(nodeC, (fmask & 0x80) ? True:False);
    break;
  }

  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
}


static void __fx2fn(iOSlot slot, int fx) {
  int i = 0;
  for( i = 0; i < 28; i++ ) {
    slot->f[i] = (fx & (1 << i)) ? True:False;
  }
}


static int __normalizeSteps(int insteps ) {
  /* SPEEDSTEPS: vaild: 14, 28, 128 */
  if( insteps < 20 )
    return 14;
  if( insteps > 100 )
    return 127;
  return 28;
}


static void __releaseSessions(iOCBUS cbus) {
  iOCBUSData data = Data(cbus);
  iOSlot slot = (iOSlot)MapOp.first( data->lcmap );
  if( MutexOp.trywait( data->lcmux, 500 ) ) {
    while( slot != NULL ) {
      if( slot->session > 0 ) {
        byte cmd[5];
        byte* frame = allocMem(32);
        cmd[0] = OPC_KLOC;
        cmd[1] = slot->session;
        makeFrame(frame, PRIORITY_NORMAL, cmd, 1, data->cid, False );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "release session %d for address %d", slot->session, slot->addr );
        ThreadOp.post(data->writer, (obj)frame);
        slot->session = 0;
        ThreadOp.sleep(50);
      }
      slot = (iOSlot)MapOp.next( data->lcmap );
    }
    MutexOp.post(data->lcmux);
  }
}


static void __requestSession(iOCBUS cbus, int addr) {
  iOCBUSData data = Data(cbus);
  byte cmd[5];
  byte* frame = allocMem(32);
  cmd[0] = OPC_RLOC;
  cmd[1] = addr / 256;
  cmd[2] = addr % 256;
  if( addr > 127 ) {
    cmd[1] |= 0xC0;
  }
  makeFrame(frame, PRIORITY_NORMAL, cmd, 2, data->cid, False );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "request session for address %d", addr );
  ThreadOp.post(data->writer, (obj)frame);
}

static iOSlot __getSlot(iOCBUS cbus, iONode node) {
  iOCBUSData data = Data(cbus);
  int    addr  = wLoc.getaddr(node);
  iOSlot slot  = NULL;
  int    speed = 0;

  slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot exist for %s", wLoc.getid(node) );
    if( slot->session == 0 ) {
      __requestSession(cbus, slot->addr);
    }
    return slot;
  }

  slot = allocMem( sizeof( struct slot) );
  slot->addr    = addr;
  slot->id      = StrOp.dup(wLoc.getid(node));
  slot->steps   = __normalizeSteps( wLoc.getspcnt(node) );
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

  __requestSession(cbus, slot->addr);


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for %s", wLoc.getid(node) );
  return slot;
}


static void __broadcastSpeedDir(iOCBUS cbus, int session, iOSlot slot, int speed, Boolean dir) {
  iOCBUSData data = Data(cbus);
  iONode nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );

  wLoc.setV_raw( nodeC, speed );
  wLoc.setdir( nodeC, dir );
  wLoc.setcmd( nodeC, wLoc.direction );
  if( data->iid != NULL )
    wLoc.setiid( nodeC, data->iid );


  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "update speed[%d] for loco %s", speed, slot->id );

    slot->rawspeed = speed;
    slot->dir      = dir;

    wLoc.setid( nodeC, slot->id );
    wLoc.setaddr( nodeC, slot->addr );
    wLoc.setV_rawMax( nodeC, slot->steps );
    wLoc.setfn( nodeC, slot->lights);
    wLoc.setthrottleid( nodeC, "cbus" );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "update speed[%d] for CAB session %d", speed, session );
    wLoc.setaddr( nodeC, data->cab[session] );
    wLoc.setthrottleid( nodeC, "cancab" );
  }

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
    __broadcastSpeedDir(cbus, session, slot, speed, dir);
  }
  else if( data->cab[session] != 0 ) {
    __broadcastSpeedDir(cbus, session, NULL, speed, dir);
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unmanaged loco: session %d", session );
    __QueryLoco(data, session);
  }
}


static void __broadcastFunction(iOCBUS cbus, int session, iOSlot slot, int fn, Boolean on) {
  iOCBUSData data = Data(cbus);

  iONode nodeC = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
  int group = fn/4 + ((fn%4 > 0) ? 1:0);

  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "update function %d in group %d for loco %s", fn, group, slot->id );
    wFunCmd.setid( nodeC, slot->id );
    wFunCmd.setaddr( nodeC, slot->addr );
    wLoc.setthrottleid( nodeC, "cbus" );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "update function %d in group %d for session %d", fn, group, session );
    wFunCmd.setaddr( nodeC, data->cab[session] );
    wLoc.setthrottleid( nodeC, "CAB" );
  }

  if( data->iid != NULL )
    wLoc.setiid( nodeC, data->iid );
  wFunCmd.setfnchanged(nodeC, fn);
  wFunCmd.setgroup( nodeC, group );

  switch(fn) {
  case  0: wFunCmd.setf0 (nodeC, on); break;
  case  1: wFunCmd.setf1 (nodeC, on); break;
  case  2: wFunCmd.setf2 (nodeC, on); break;
  case  3: wFunCmd.setf3 (nodeC, on); break;
  case  4: wFunCmd.setf4 (nodeC, on); break;
  case  5: wFunCmd.setf5 (nodeC, on); break;
  case  6: wFunCmd.setf6 (nodeC, on); break;
  case  7: wFunCmd.setf7 (nodeC, on); break;
  case  8: wFunCmd.setf8 (nodeC, on); break;
  case  9: wFunCmd.setf9 (nodeC, on); break;
  case 10: wFunCmd.setf10(nodeC, on); break;
  case 11: wFunCmd.setf11(nodeC, on); break;
  case 12: wFunCmd.setf12(nodeC, on); break;
  case 13: wFunCmd.setf13(nodeC, on); break;
  case 14: wFunCmd.setf14(nodeC, on); break;
  case 15: wFunCmd.setf15(nodeC, on); break;
  case 16: wFunCmd.setf16(nodeC, on); break;
  case 17: wFunCmd.setf17(nodeC, on); break;
  case 18: wFunCmd.setf18(nodeC, on); break;
  case 19: wFunCmd.setf19(nodeC, on); break;
  case 20: wFunCmd.setf20(nodeC, on); break;
  case 21: wFunCmd.setf21(nodeC, on); break;
  case 22: wFunCmd.setf22(nodeC, on); break;
  case 23: wFunCmd.setf23(nodeC, on); break;
  case 24: wFunCmd.setf24(nodeC, on); break;
  case 25: wFunCmd.setf25(nodeC, on); break;
  case 26: wFunCmd.setf26(nodeC, on); break;
  case 27: wFunCmd.setf27(nodeC, on); break;
  case 28: wFunCmd.setf28(nodeC, on); break;
  }

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
    __broadcastFunction(cbus, session, slot, fn, fstate );
  }
  else if(data->cab[session] != 0) {
    __broadcastFunction(cbus, session, NULL, fn, fstate );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unmanaged loco: session %d", session );
    __QueryLoco(data, session);
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
    __fg2fn(cbus, session, slot, fg, fmask);
  }
  else if(data->cab[session] != 0) {
    __fg2fn(cbus, session, NULL, fg, fmask);
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unmanaged loco: session %d", session );
    __QueryLoco(data, session);
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
  addr &= 0x3FFF;

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
      if( slot->steps >= 127 )
        cmd[2] = TMOD_SPD_128;
      else if( slot->steps == 14 )
        cmd[2] = TMOD_SPD_14;
      else if( slot->steps == 28 )
        cmd[2] = TMOD_SPD_28;
      cmd[2] |= (slot->lights ? 0x04:0x00);
      makeFrame(frame, PRIORITY_NORMAL, cmd, 2, data->cid, False );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "set engine speed steps to %d for loco %s", slot->steps, slot->id );
      ThreadOp.post(data->writer, (obj)frame);

      __broadcastSpeedDir(cbus, session, slot, speed, dir);
      __fg2fn(cbus, session, slot, 1, f0_4);
      __fg2fn(cbus, session, slot, 2, f5_8);
      __fg2fn(cbus, session, slot, 3, f9_12);


    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unmanaged loco: session %d, address %d", session, addr );
    data->cab[session] = addr;
  }
}


static __evaluateAcc( iOCBUS cbus, byte* frame, Boolean state ) {
  iOCBUSData data = Data(cbus);

  int offset  = (frame[1] == 'S') ? 0:4;
  int nodeh   = HEXA2Byte(frame + OFFSET_D1 + offset);
  int nodel   = HEXA2Byte(frame + OFFSET_D2 + offset);
  int node    = nodeh * 256 + nodel;

  int eventh   = HEXA2Byte(frame + OFFSET_D3 + offset);
  int eventl   = HEXA2Byte(frame + OFFSET_D4 + offset);
  int event    = eventh * 256 + eventl;

  int addr = event;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "%sevent %d:%d %s",
      data->shortevents?"short ":"", node, addr, state?"ON":"OFF" );

  /* inform listener: Node3 */
  iONode nodeC = NodeOp.inst( wAccessory.name(), NULL, ELEMENT_NODE );
  wAccessory.setnodenr( nodeC, data->shortevents?0:node );
  wAccessory.setdevid( nodeC, addr );
  wAccessory.setval1( nodeC, state );
  wAccessory.setaccevent( nodeC, True );
  if( data->iid != NULL )
    wAccessory.setiid( nodeC, data->iid );

  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
}


static __evaluateErr( iOCBUS cbus, byte* frame ) {
  iOCBUSData data = Data(cbus);
  iONode node = NULL;

  int offset  = (frame[1] == 'S') ? 0:4;
  int addrh   = HEXA2Byte(frame + OFFSET_D1 + offset);
  int addrl   = HEXA2Byte(frame + OFFSET_D2 + offset);
  int err     = HEXA2Byte(frame + OFFSET_D3 + offset);

  int addr = (addrh&0x3F) * 256 + addrl;

  const char* errStr = "?";

  switch( err ) {
  case ERR_LOCO_STACK_FULL:
    errStr = "loco stack full";
    break;
  case ERR_LOCO_ADDR_TAKEN:
    errStr = "loco address taken";
    break;
  case ERR_SESSION_NOT_PRESENT:
    {
      errStr = "session not present";
      iOSlot slot = __getSlotBySession(data, addrh);
      if( slot != NULL ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "session %d timed out for %s", addrh, slot->id );
        slot->session = 0;

        byte cmd[5];
        byte* frame = allocMem(32);
        cmd[0] = OPC_RLOC;
        cmd[1] = slot->addr / 256;
        cmd[2] = slot->addr % 256;
        if( addr > 127 ) {
          cmd[1] |= 0xC0;
        }
        makeFrame(frame, PRIORITY_NORMAL, cmd, 2, data->cid, False );

        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request session for address %d", addr );
        ThreadOp.post(data->writer, (obj)frame);
      }
    }
    break;
  case ERR_CONSIST_EMPTY:
    errStr = "consist empty";
    break;
  case ERR_LOCO_NOT_FOUND:
    errStr = "loco not found";
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
  case CMDERR_TOO_MANY_EVENTS:
    errStr = "too many events";
    break;
  case CMDERR_INVALID_EVENT:
    errStr = "invalid event";
    break;
  case CMDERR_INV_EV_IDX:
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
  int cv    = cvh * 256 + cvl;

  int value = HEXA2Byte(frame + OFFSET_D4 + offset);

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "cv %d has a value of %d", cv, value );

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
  char ident[64] = {'\0'};
  Boolean isZero = False;

  int offset  = (frame[1] == 'S') ? 0:4;
  int addrh   = HEXA2Byte(frame + OFFSET_D1 + offset);
  int addrl   = HEXA2Byte(frame + OFFSET_D2 + offset);
  int addr    = addrh * 256 + addrl;

  int rfid1  = HEXA2Byte(frame + OFFSET_D3 + offset);
  int rfid2  = HEXA2Byte(frame + OFFSET_D4 + offset);
  int rfid3  = HEXA2Byte(frame + OFFSET_D5 + offset);
  int rfid4  = HEXA2Byte(frame + OFFSET_D6 + offset);
  int rfid5  = HEXA2Byte(frame + OFFSET_D7 + offset);

  if( rfid1 == 0 && rfid2 == 0 && rfid3 == 0 && rfid4 == 0 && rfid5 == 0 )
    isZero = True;


  StrOp.fmtb( ident, "%d.%d.%d.%d.%d", rfid1, rfid2, rfid3, rfid4, rfid5);
  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "RFID %s",ident);
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "RFID %sevent %d id=%s",
      data->shortevents?"short ":"", addr, ident );

  /* inform listener: Node3 */
  iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
  wFeedback.setaddr( nodeC, addr );
  wFeedback.setstate( nodeC, isZero ? False:True );
  wFeedback.setfbtype( nodeC, wFeedback.fbtype_rfid );
  wFeedback.setidentifier( nodeC, ident );
  if( data->iid != NULL )
    wFeedback.setiid( nodeC, data->iid );

  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
}



static __evaluateIR( iOCBUS cbus, byte* frame ) {
  iOCBUSData data = Data(cbus);

  int offset  = (frame[1] == 'S') ? 0:4;
  int nodeh   = HEXA2Byte(frame + OFFSET_D1 + offset);
  int nodel   = HEXA2Byte(frame + OFFSET_D2 + offset);
  int node    = nodeh * 256 + nodel;

  int addrh   = HEXA2Byte(frame + OFFSET_D3 + offset);
  int addrl   = HEXA2Byte(frame + OFFSET_D4 + offset);
  int lissyaddr = addrh * 256 + addrl;

  int ir1  = HEXA2Byte(frame + OFFSET_D5 + offset);
  int ir2  = HEXA2Byte(frame + OFFSET_D6 + offset);
  int sensdata = ir1 * 256 + ir2;
  int flags = HEXA2Byte(frame + OFFSET_D7 + offset);
  Boolean wheelcnt = ((flags & 0x02) ? True:False);
  Boolean dir = ((flags & 0x01) ? True:False);

  char ident[32];
  StrOp.fmtb(ident, "%d", sensdata);

  /* inform listener: Node3 */
  iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );

  if(wheelcnt) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "wheelcounter=%d count=%d", lissyaddr, sensdata );
    wFeedback.setfbtype( nodeC, wFeedback.fbtype_wheelcounter );
    wFeedback.setwheelcount( nodeC, sensdata );
  }
  else {
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "IR %sevent %d loco=%d dir=%s (node=%d)",
      data->shortevents?"short ":"", lissyaddr, sensdata, dir?"fwd":"rev", node );
    wFeedback.setfbtype( nodeC, wFeedback.fbtype_lissy );

    wFeedback.setidentifier( nodeC, ident );
    wFeedback.setdirection( nodeC, dir );
  }

  wFeedback.setbus( nodeC, node );
  wFeedback.setaddr( nodeC, lissyaddr );
  wFeedback.setstate( nodeC, True );

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
  byte reqrsp  = HEXA2Byte(frame+2);
  byte cargo   = HEXA2Byte(frame+8);
  byte cmd     = HEXA2Byte(frame+21);

  if( (reqrsp & 0x80) == 0 ) {
    /* Commands:
    :X00080004N000000000D000000; NOP
    :X00080004N000000000D010000; Reset
    :X00080004N000000000D020000; Init
    :X00080004N000000000D030000; Check
    :X00080004N000000000D040000; Test
    :X00080005N0000000000000000; Data
    */
    if( cargo == 5 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bootloader data frame [%s]", frame+11 );
    }
    else {
      if( cmd == 0x00 )
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bootloader command NOP: 0x%02X", cmd );
      else if( cmd == 0x01 )
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bootloader command RESET: 0x%02X", cmd );
      else if( cmd == 0x02 )
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bootloader command INIT: 0x%02X", cmd );
      else if( cmd == 0x03 )
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bootloader command CHECK: 0x%02X", cmd );
      else if( cmd == 0x04 )
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bootloader command TEST: 0x%02X", cmd );
      else
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unknown bootloader command: 0x%02X", cmd );
    }
  }

  else if( reqrsp & 0x80 ) {
    byte rc = HEXA2Byte(frame+11);
    /* Responses:
    :X80080004N00; Error, on response of a Check command.
    :X80080004N01; OK, on response of a Check command.
    :X80080004N02; Boot mode confirm, on response of a Test command.
    */
    /* ToDo: Generate program nodes. */
    if(rc == 0) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "error response from bootloader" );
    }
    else if(rc == 1) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "OK response from bootloader" );
    }
    else if(rc == 2) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "boot mode confirmed" );
    }
  }

  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unknown extended frame: %s", frame );
  }

  return NULL;
}


static const char* PTSTATUS[] = {"Reserved", "No Acknowledge", "Overload", "Write Acknowledge", "Busy", "CV out of range"};

static iONode __evaluateASCIIFrame(iOCBUS cbus, byte* frame, int opc) {
  iOCBUSData data = Data(cbus);
  int offset = (frame[1] != 'X') ? 0:4;


  if( frame[1] == 'S' ) {
    cbusMon(frame, opc);
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
    case OPC_KLOC: {
      int offset  = (frame[1] == 'S') ? 0:4;
      int session = HEXA2Byte(frame + OFFSET_D1 + offset);
      data->cab[session] = 0;
      break;
    }
    case OPC_RLOC:
      if( data->slotserver )
        slotServer((obj)cbus, opc, frame);
      break;
    case OPC_DSPD:
      __updateSpeedDir(cbus, frame);
      break;
    case OPC_DFNON:
      __updateFunction(cbus, frame, True);
      break;
    case OPC_DFNOF:
      __updateFunction(cbus, frame, False);
      break;
    case OPC_DFUN:
      __updateFunctions(cbus, frame);
      break;
    case OPC_ACON:
    case OPC_ASON:
    case OPC_ARON:
      __evaluateAcc(cbus, frame, True);
      break;
    case OPC_ACOF:
    case OPC_ASOF:
    case OPC_AROF:
      __evaluateAcc(cbus, frame, False);
      break;
    case OPC_ACDAT:
    case OPC_DDES:
      __evaluateRFID(cbus, frame);
      break;
    case OPC_ACON3:
      __evaluateIR(cbus, frame);
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
    case OPC_SSTAT:
    {
      int offset  = (frame[1] == 'S') ? 0:4;
      int session = HEXA2Byte(frame + OFFSET_D1 + offset);
      int status  = HEXA2Byte(frame + OFFSET_D2 + offset);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "service track session %d: status=%d, \"%s\"", session, status, (status < 6) ? PTSTATUS[status]:"Unknown"  );

      if( data->listenerFun != NULL && data->listenerObj != NULL ) {
        iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
        wProgram.setcv( node, data->cvnr );
        wProgram.setvalue( node, -1 );
        wProgram.setcmd( node, wProgram.datarsp );
        if( data->iid != NULL )
          wProgram.setiid( node, data->iid );
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
      }
      break;
    }

    /* All FLiM OPCs to this case. */
    case OPC_NNACK:
    case OPC_NNREL:
    case OPC_NNLRN:
    case OPC_NNULN:
    case OPC_NNCLR:
    case OPC_NNEVN:
    case OPC_NERD:
    case OPC_RQEVN:
    case OPC_WRACK:
    case OPC_RQDAT:
    case OPC_BOOT:
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
    case OPC_PNN:
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
  else if( frame[1] == 'Y' ) {
    /* Ethernet frame: CAN-GC1e */
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,"CAN-GC1e status: %s opc=%d", frame, opc);
    switch(opc) {
      case 0:
        break;
      case 1: /* status report */
      {
        byte rc = HEXA2Byte(frame + OFFSET_D1);
        TraceOp.trc( name, rc==0?TRCLEVEL_MONITOR:TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Ethernet status: rc=%d con=%d maxcanQ=%d maxethQ=%d", rc,
            HEXA2Byte(frame + OFFSET_D2), HEXA2Byte(frame + OFFSET_D3), HEXA2Byte(frame + OFFSET_D4) );
        break;
      }
      case 2: /* command ack */
      {
        byte rc  = HEXA2Byte(frame + OFFSET_D1);
        byte ackopc = HEXA2Byte(frame + OFFSET_D2);
        byte cmdticker = HEXA2Byte(frame + OFFSET_D3);
        TraceOp.trc( name, rc==0?TRCLEVEL_MONITOR:TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Ethernet command Ack: rc=%d opc=0x%02X cmdticker=%d", rc, ackopc, cmdticker );
        /* ToDo: process the Ack. */
        if( ackopc == data->lastSendOPC ) {
          data->wait4Ack = False;
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

static void __stressRunner( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOCBUS cbus = (iOCBUS)ThreadOp.getParm( th );
  iOCBUSData data = Data(cbus);

  ThreadOp.sleep(5000);
  if( data->stress )
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CBUS stress runner started." );

  /* try to get the system status: */
  while( data->run && data->stress ) {
    /* get all input states */
    if( data->sodaddr > 0 ) {
      byte cmd[8];
      byte* frame = allocMem(64);
      cmd[0] = OPC_ASRQ;
      cmd[1] = 0;
      cmd[2] = 0;
      cmd[3] = data->sodaddr / 256;
      cmd[4] = data->sodaddr % 256;
      makeFrame(frame, PRIORITY_NORMAL, cmd, 4, data->cid, False );
      ThreadOp.post(data->writer, (obj)frame);
    }
    ThreadOp.sleep(50);

  };

  if( data->stress )
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CBUS stress runner ended." );
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
    makeFrame(frame, PRIORITY_NORMAL, cmd, 4, data->cid, False );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "query input state" );
    ThreadOp.post(data->writer, (obj)frame);
  }

  if( 1 ) {
    byte cmd[8];
    byte* frame = allocMem(64);
    cmd[0] = OPC_RSTAT;
    makeFrame(frame, PRIORITY_NORMAL, cmd, 0, data->cid, False );
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

    if( data->connOK && data->subAvailable( (obj)cbus) ) {
      if( data->subRead( (obj)cbus, frame, 1) ) {
        if( frame[0] == ':' ) {
          if( data->subRead( (obj)cbus, frame+1, 1) ) {
            if( frame[1] == 'S' || frame[1] == 'Y' ) {
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
                      int datalen = __getDataLen(opc, frame[1]);
                      TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, 2 + OFFSET_OPC + offset );
                      if( data->subRead( (obj)cbus, frame + 2 + OFFSET_OPC + offset, datalen*2 + 1 ) ) {
                        TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, 2 + OFFSET_OPC + offset + datalen*2 + 1 );
                        __evaluateASCIIFrame(cbus, frame, opc);
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

            /* binary standard frame CAN-GC2 */
            else if( frame[1] == 's' ) {
              int n = 2;
              TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "reading binary frame" );
              if( data->subRead( (obj)cbus, frame + n, 4 ) ) {
                int datalen = __getDataLen(frame[5], frame[1]);
                n+=4;
                TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "binary frame: OPC=0x%02X data=%d",
                    frame[5], datalen );
                if( data->subRead( (obj)cbus, frame + n, datalen + 1 ) ) {
                  byte cmd[32];
                  int hh   = frame[2];
                  int hl   = frame[3];
                  int canid = ((hh&0x1F) << 3) + ((hl&0xE0) >> 5);

                  n += datalen +1;
                  makeFrame(cmd, PRIORITY_NORMAL, frame+5, datalen, canid, False );
                  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "binary standard frame: %s", cmd+1 );
                  __evaluateASCIIFrame(cbus, cmd+1, frame[5]);
                }
              }
            }

            /* binary extended frame CAN-GC2 */
            else if( frame[1] == 'x' ) {
              int n = 2;
              TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "reading extended frame" );
              while( data->subRead( (obj)cbus, frame + n, 1 ) && n < 20 ) {
                if( frame[n] == ';' ) {
                  byte cmd[32];
                  frame[n+1] = '\0';
                  TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, n+1 );
                  makeFrame(cmd, PRIORITY_NORMAL, frame+7, 7, 0, False );
                  __evaluateXFrame(cbus, cmd);
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
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no start byte [0x%02X]", frame[0] );
        }
      }
      else {
        /* signals somethingh to read but there was not... */
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "I/O Error. USB device disconnected?" );
        data->connOK = False;
        data->subDisconnect((obj)cbus);
        ThreadOp.sleep(1000);
      }
    }
    else {
      /* Sleep only if there was nothing to read. */
      ThreadOp.sleep(10);
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader ended." );
}




static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOCBUS cbus = (iOCBUS)ThreadOp.getParm( th );
  iOCBUSData data = Data(cbus);
  byte* cmd = NULL;
  int ackwait = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer started." );
  while( data->run ) {
    byte * post = NULL;
    int len = 0;
    byte out[64] = {0};

    ThreadOp.sleep(10);
    if( data->connOK && data->buson && !data->wait4Ack ) {
      ackwait = 0;
      post = (byte*)ThreadOp.getPost( th );

      if (post != NULL) {
        /* first byte is the message length */
        len = post[0];
        MemOp.copy( out, post+1, len);
        freeMem( post);

        if( !data->bootmode ) {
          data->lastSendOPC = __getOPC(out);
          cbusMon(out, __getOPC(out));
        }

        if( data->subWrite((obj)cbus, out, len) ) {
          if( data->commandAck && out[1] == 'S' )
            data->wait4Ack = True;
        }
        else {
          /* sleep and send it again? */
        }
      }
    }
    else if( data->wait4Ack && data->commandAck ) {
      ackwait++;
      if( ackwait > 50 ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "time out on command ack 0x%02X", data->lastSendOPC );
        ackwait = 0;
        data->wait4Ack = False;
      }
    }
    else if( !data->connOK ) {
      ThreadOp.sleep(5000);
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "CBUS is disconnected, try a reconnect..." );
      data->connOK = data->subConnect((obj)cbus);
    }
    else {
      ThreadOp.sleep(1000);
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "CBUS commandAck=%d wait4Ack=%d buson=%d", data->commandAck, data->wait4Ack, data->buson );
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
        if( slot->session > 0 ) {
          if( slot->releasereq && slot->speed == 0 ) {
            byte cmd[5];
            byte* frame = allocMem(32);
            cmd[0] = OPC_KLOC;
            cmd[1] = slot->session;
            makeFrame(frame, PRIORITY_NORMAL, cmd, 1, data->cid, False );
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "release session %d for loco %d", slot->session, slot->addr );
            ThreadOp.post(data->writer, (obj)frame);
            slot->session = 0;
            slot->releasereq = False;
          }
          else if( ( (tick - slot->lastkeep) > (data->purgetime*90) ) && data->purgetime > 0 ) {
            byte cmd[5];
            byte* frame = allocMem(32);
            cmd[0] = OPC_DKEEP;
            cmd[1] = slot->session;
            makeFrame(frame, PRIORITY_NORMAL, cmd, 1, data->cid, False );

            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                "keep session %d for %s: tick=%ld, lastkeep=%ld, purgetime=%d",
                slot->session, slot->id, tick, slot->lastkeep, data->purgetime*90 );
            ThreadOp.post(data->writer, (obj)frame);

            slot->lastkeep = tick;
          }
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
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
          "new timed command time=%d delay=%d tick=%d frame=%s",
          cmd->time, cmd->delay, SystemOp.getTick(), cmd->out+1 );
      cbusMon(cmd->out+1, __getOPC(cmd->out+1));
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


static void __makeDFUN(iOSlot slot, iONode node, byte* cmd) {
  int fnchanged = wFunCmd.getfnchanged(node);
  int fngroup   = wFunCmd.getgroup(node);
  int i = 0;
  int fmask = 0;

  cmd[0] = OPC_DFUN;
  cmd[1] = slot->session;
  if( fnchanged < 5 || fngroup == 1) {
    cmd[2] = 1;
    fmask = __getFState(node, 0) ? 0x10:0x00;
    for( i = 1; i < 5; i++ ) {
      if( __getFState(node, i) )
        fmask |= (1 << (i-1));
    }
    cmd[3] = fmask;
  }
  else if( fnchanged < 9 || fngroup == 2 ) {
    cmd[2] = 2;
    for( i = 5; i < 9; i++ ) {
      if( __getFState(node, i) )
        fmask |= (1 << (i-5));
    }
    cmd[3] = fmask;
  }
  else if( fnchanged < 13 || fngroup == 3 ) {
    cmd[2] = 3;
    for( i = 9; i < 13; i++ ) {
      if( __getFState(node, i) )
        fmask |= (1 << (i-9));
    }
    cmd[3] = fmask;
  }
  else if( fnchanged < 20  || fngroup == 4 || fngroup == 5) {
    cmd[2] = 4;
    for( i = 13; i < 20; i++ ) {
      if( __getFState(node, i) )
        fmask |= (1 << (i-13));
    }
    cmd[3] = fmask;
  }
  else if( fnchanged < 29 || fngroup == 6 ) {
    cmd[2] = 5;
    for( i = 20; i < 29; i++ ) {
      if( __getFState(node, i) )
        fmask |= (1 << (i-20));
    }
    cmd[3] = fmask;
  }
}


static void __setFastClock(iOCBUS cbus, iONode node) {
  iOCBUSData data = Data(cbus);

  byte cmd[32];
  byte* frame = allocMem(32);
  int hours = 10;
  int mins  = 30;
  int wday  = 0;
  int mday  = 0;
  int mon   = 0;
  int div   = wClock.getdivider(node);
  char temp = wClock.gettemp(node);

  long l_time = wClock.gettime(node);
  struct tm* lTime = localtime( &l_time );

  mins  = lTime->tm_min;
  hours = lTime->tm_hour;
  wday  = lTime->tm_wday;
  mday  = lTime->tm_mday;
  mon   = lTime->tm_mon;

  cmd[0] = OPC_FCLK;
  cmd[1] = mins;
  cmd[2] = hours;
  cmd[3] = ((mon+1) << 4) + wday+1;
  cmd[4] = div;
  cmd[5] = mday;
  cmd[6] = temp;

  makeFrame(frame, PRIORITY_NORMAL, cmd, 6, data->cid, False );

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "fast clock sync %02d:%02d wday=%d divider=%d date=%d-%d temp=%d",
      hours, mins, wday, div, mday, mon+1, temp );
  ThreadOp.post(data->writer, (obj)frame);


}




static iONode __translate( iOCBUS cbus, iONode node ) {
  iOCBUSData data = Data(cbus);
  iONode rsp = NULL;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "translate: %s", NodeOp.getName( node ) );

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
      makeFrame(frame, PRIORITY_NORMAL, cmd, 0, data->cid, False );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power OFF" );
      ThreadOp.post(data->writer, (obj)frame);
    }

    else if( StrOp.equals( cmdstr, wSysCmd.ebreak ) ) {
      /* CS ebreak */
      byte cmd[2];
      byte* frame = allocMem(32);
      cmd[0] = OPC_RESTP;
      makeFrame(frame, PRIORITY_NORMAL, cmd, 0, data->cid, False );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request emergency break" );
      ThreadOp.post(data->writer, (obj)frame);
    }

    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS on */
      byte cmd[2];
      byte* frame = allocMem(32);
      cmd[0] = OPC_RTON;
      makeFrame(frame, PRIORITY_NORMAL, cmd, 0, data->cid, False );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power ON" );
      ThreadOp.post(data->writer, (obj)frame);
    }

    else if( StrOp.equals( cmdstr, wSysCmd.sod ) ) {
      /* Start of Day */
      byte cmd[8];
      byte* frame = allocMem(32);
      cmd[0] = OPC_ASRQ;
      cmd[1] = 0;
      cmd[2] = 0;
      cmd[3] = data->sodaddr / 256;
      cmd[4] = data->sodaddr % 256;
      makeFrame(frame, PRIORITY_NORMAL, cmd, 4, data->cid, False );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SoD: query input state" );
      ThreadOp.post(data->writer, (obj)frame);
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    byte cmd[5];
    byte* frame = allocMem(32);
    int delay = wSwitch.getdelay(node) > 0 ? wSwitch.getdelay(node):data->swtime;

    int addr = wSwitch.getaddr1( node );
    if( addr == 0 ) {
      addr = wSwitch.getport1( node );
    }

    if( wSwitch.issinglegate( node ) ) {
      cmd[0] = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? OPC_ACON:OPC_ACOF;
      cmd[1] = wSwitch.getbus( node ) / 256;
      cmd[2] = wSwitch.getbus( node ) % 256;
      cmd[3] = addr / 256;
      cmd[4] = addr % 256;
      makeFrame(frame, PRIORITY_NORMAL, cmd, 4, data->cid, False );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "single gate switch %d:%d",
          wSwitch.getbus( node ), wSwitch.getaddr1( node ) );
      ThreadOp.post(data->writer, (obj)frame);
    }
    else {
      if( StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) )
        addr++;
      cmd[0] = OPC_ACON;
      cmd[1] = wSwitch.getbus( node ) / 256;
      cmd[2] = wSwitch.getbus( node ) % 256;
      cmd[3] = addr / 256;
      cmd[4] = addr % 256;
      makeFrame(frame, PRIORITY_NORMAL, cmd, 4, data->cid, False );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "dual gate switch %d:%d",
          wSwitch.getbus( node ), addr );
      ThreadOp.post(data->writer, (obj)frame);
    }

  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    byte cmd[5];
    byte* frame = allocMem(32);
    Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;
    int gate = wOutput.getgate( node );

    int addr = wOutput.getaddr( node );
    if( addr == 0 ) {
      addr = wOutput.getport( node );
    }

    addr += gate;

    if( StrOp.equals( wOutput.getcmd( node ), wOutput.sod ) )
      cmd[0] = OPC_ASRQ;
    else
      cmd[0] = on ? OPC_ACON:OPC_ACOF;

    cmd[1] = wOutput.getbus( node ) / 256;
    cmd[2] = wOutput.getbus( node ) % 256;
    cmd[3] = addr / 256;
    cmd[4] = addr % 256;
    makeFrame(frame, PRIORITY_NORMAL, cmd, 4, data->cid, False );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d:%d.%d %s",
        wOutput.getbus( node ), wOutput.getaddr( node ), wOutput.getgate(node), on?"ON":"OFF" );
    ThreadOp.post(data->writer, (obj)frame);

  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );
  }


  /* Loc dispatch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) && StrOp.equals(wLoc.dispatch, wLoc.getcmd(node)) ) {
    iOSlot slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );
    if( slot != NULL && slot->session > 0 ) {
      byte cmd[5];
      byte* frame = allocMem(32);
      cmd[0] = OPC_DSPLOC;
      cmd[1] = slot->session;
      makeFrame(frame, PRIORITY_NORMAL, cmd, 1, data->cid, False );
      slot->lastkeep = SystemOp.getTick();
      ThreadOp.post(data->writer, (obj)frame);
    }
    else if( slot != NULL && slot->session == 0 ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "no session for loco %d", slot->addr );
    }
  }

  /* Loc release command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) && StrOp.equals(wLoc.release, wLoc.getcmd(node)) ) {
    iOSlot slot = __getSlot(cbus, node );
    if( slot != NULL && slot->session > 0 ) {
      slot->releasereq = True;
    }
    else if( slot != NULL && slot->session == 0 ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "no session for loco %d", slot->addr );
    }
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
      return NULL;
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


    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco speed=%d dir=%s", slot->speed, slot->dir?"forwards":"reverse" );

    if( slot->session > 0 ) {
      byte* frame = allocMem(32);
      cmd[0] = OPC_DSPD;
      cmd[1] = slot->session;
      cmd[2] = speed | (slot->dir ? 0x80:0x00);
      makeFrame(frame, PRIORITY_NORMAL, cmd, 2, data->cid, False );
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
      makeFrame(qcmd->out, PRIORITY_NORMAL, cmd, 2, data->cid, False );
      ThreadOp.post( data->timedqueue, (obj)qcmd );
    }

  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    byte cmd[5];
    int     addr      = wFunCmd.getaddr( node );
    int     fnchanged = wFunCmd.getfnchanged(node);
    Boolean fnstate   = __getFState(node, fnchanged);

    iOSlot slot = __getSlot(cbus, node );


    Boolean lights = wLoc.isfn(node);
    Boolean f0 = wFunCmd.isf0( node );
    Boolean f1 = wFunCmd.isf1( node );
    Boolean f2 = wFunCmd.isf2( node );
    Boolean f3 = wFunCmd.isf3( node );
    Boolean f4 = wFunCmd.isf4( node );
    Boolean f5 = wFunCmd.isf5( node );
    Boolean f6 = wFunCmd.isf6( node );
    Boolean f7 = wFunCmd.isf7( node );
    Boolean f8 = wFunCmd.isf8( node );
    Boolean f9  = wFunCmd.isf9( node );
    Boolean f10 = wFunCmd.isf10( node );
    Boolean f11 = wFunCmd.isf11( node );
    Boolean f12 = wFunCmd.isf12( node );

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
            "decoder %d lights=%s f0=%s f1=%s f2=%s f3=%s f4=%s f5=%s f6=%s f7=%s f8=%s f9=%s f10=%s f11=%s f12=%s",
            addr, lights?"on":"off", f0?"on":"off", f1?"on":"off", f2?"on":"off", f3?"on":"off", f4?"on":"off",
            f5?"on":"off", f6?"on":"off", f7?"on":"off", f8?"on":"off",
            f9?"on":"off", f10?"on":"off", f11?"on":"off", f12?"on":"off" );


    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get slot for loco %s", wLoc.getid(node) );
      return NULL;
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco fn=%d state=%s", fnchanged, fnstate?"ON":"OFF" );

    if( slot->session > 0 ) {
      byte* frame = allocMem(32);
      if( data->fonfof ) {
        cmd[0] = fnstate?OPC_DFNON:OPC_DFNOF;
        cmd[1] = slot->session;
        cmd[2] = fnchanged;
        makeFrame(frame, PRIORITY_NORMAL, cmd, 2, data->cid, False );
      }
      else {
        __makeDFUN(slot, node, cmd);
        makeFrame(frame, PRIORITY_NORMAL, cmd, 3, data->cid, False );
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
        cmd[0] = fnstate?OPC_DFNON:OPC_DFNOF;
        cmd[1] = slot->session;
        cmd[2] = fnchanged;
        makeFrame(qcmd->out, PRIORITY_NORMAL, cmd, 2, data->cid, False );
      }
      else {
        __makeDFUN(slot, node, cmd);
        makeFrame(qcmd->out, PRIORITY_NORMAL, cmd, 3, data->cid, False );
      }
      ThreadOp.post( data->timedqueue, (obj)qcmd );
    }

  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    Boolean direct = wProgram.isdirect(node);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "program type %d...", wProgram.getlntype(node) );

    if( wProgram.getlntype(node) == wProgram.lntype_sv && wProgram.getcmd( node ) == wProgram.lncvget &&
        wProgram.getcv(node) == 0 && wProgram.getmodid(node) == 0 && wProgram.getaddr(node) == 0 )
    {
      /* This construct is used to to query all LocoIOs, but is here recycled for query all CAN-GC2s. */
      wProgram.setcmd(node, wProgram.query);
      byte* frame = programFLiM((obj)cbus, node );
      if( frame != NULL ) {
        ThreadOp.post(data->writer, (obj)frame);
      }
    }
    else if( wProgram.getlntype(node) == wProgram.lntype_cbus ) {
      byte* frame = programFLiM((obj)cbus, node );
      if( frame != NULL ) {
        ThreadOp.post(data->writer, (obj)frame);
      }
    }
    else if( wProgram.getcmd( node ) == wProgram.get ) {
      int cv = wProgram.getcv( node );
      int addr = wProgram.getaddr( node );
      data->cvnr = cv;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get CV%d on %s...", cv, wProgram.ispom(node)?"POM":"PT" );

      if( wProgram.ispom(node) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*** not supported *** POM: read CV%d of loc %d...", cv, addr );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "read CV%d of loc %d...", cv, addr );
        byte cmd[5];
        byte* frame = allocMem(32);
        Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

        cmd[0] = OPC_QCVS;
        cmd[1] = 0;
        cmd[2] = cv / 256;
        cmd[3] = cv % 256;
        cmd[4] = direct?CVMODE_DIRECTBYTE:CVMODE_PAGE; /* Programming mode; Default is paged. */
        makeFrame(frame, PRIORITY_NORMAL, cmd, 4, data->cid, False );
        ThreadOp.post(data->writer, (obj)frame);
      }
    }

    else if( wProgram.getcmd( node ) == wProgram.set ) {
      int cv = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int decaddr = wProgram.getdecaddr( node );
      data->cvnr = cv;


      if( wProgram.ispom(node) ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "POM: set CV%d of loc %d to %d...", cv, decaddr, value );
        byte cmd[10];
        byte* frame = allocMem(32);
        Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

        iOSlot slot = __getSlotByAddr(data, decaddr );
        if( slot != NULL && slot->session > 0 ) {
          cmd[0] = OPC_WCVO;
          cmd[1] = slot->session;
          cmd[2] = cv / 256;
          cmd[3] = cv % 256;
          cmd[4] = value;
          makeFrame(frame, PRIORITY_NORMAL, cmd, 4, data->cid, False );
          ThreadOp.post(data->writer, (obj)frame);
        }
        else {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "POM: no session for loc %d", decaddr );
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set CV%d of loc %d to %d...", cv, decaddr, value );
        byte cmd[10];
        byte* frame = allocMem(32);
        Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

        cmd[0] = OPC_WCVS;
        cmd[1] = 0;
        cmd[2] = cv / 256;
        cmd[3] = cv % 256;
        cmd[4] = direct?CVMODE_DIRECTBYTE:CVMODE_PAGE; /* Programming mode; Default is paged. */
        cmd[5] = value;
        makeFrame(frame, PRIORITY_NORMAL, cmd, 5, data->cid, False );

        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d:%d %s",
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
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get NN%d NV%d...", nn, nv );
    }
    else if( wProgram.getcmd( node ) == wProgram.nvset ) {
      /* Node Value set */
      int nv  = wProgram.getcv( node );
      int val = wProgram.getvalue( node );
      int nn  = wProgram.getdecaddr( node );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set NN%d NV%d to %d...", nn, nv, val );
    }

  }

  /* Clock command. */
  else if( StrOp.equals( NodeOp.getName( node ), wClock.name() ) && data->fastclock ) {
    /* Fast Clock */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "clock: %s", wClock.getcmd( node ) );

    if(  StrOp.equals( wClock.getcmd( node ), wClock.freeze ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "freeze clock" );
      wClock.setcmd( node, wClock.set );
      wClock.setdivider( node, 0 );
    }
    else if(  StrOp.equals( wClock.getcmd( node ), wClock.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "go clock" );
      wClock.setcmd( node, wClock.set );
    }
    else if(  StrOp.equals( wClock.getcmd( node ), wClock.sync ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sync clock" );
      if( !data->fcsync ) {
        wClock.setcmd( node, wClock.set );
        data->fcsync = True;
      }
    }

    if(  StrOp.equals( wClock.getcmd( node ), wClock.set ) || StrOp.equals( wClock.getcmd( node ), wClock.sync ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set clock" );
      __setFastClock(cbus, node);
    }
  }

  /* Text command. */
  else if( StrOp.equals( NodeOp.getName( node ), wText.name() ) ) {
    const char* text = wText.gettext(node);
    int addr = wText.getaddr(node);
    int display = wText.getdisplay(node);
    int len = StrOp.len( text ) + 1; /* send the terminating zero byte too */
    int packets = len / 4 + ((len % 4 > 0) ?1:0);
    int i = 0;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set display %d:%d to \"%s\"", addr, display, text );

    for( i = 0; i < packets; i++ ) {
      byte cmd[10];
      byte* frame = allocMem(32);
      cmd[0] = OPC_ACDAT;
      cmd[1] = addr / 256;
      cmd[2] = addr % 256;
      cmd[3] = display + (i << 4);
      cmd[4] = len > i*4+0 ? text[i*4+0]:0;
      cmd[5] = len > i*4+1 ? text[i*4+1]:0;
      cmd[6] = len > i*4+2 ? text[i*4+2]:0;
      cmd[7] = len > i*4+3 ? text[i*4+3]:0;
      makeFrame(frame, PRIORITY_NORMAL, cmd, 7, data->cid, False );

      ThreadOp.post(data->writer, (obj)frame);
      ThreadOp.sleep(10);
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
  data->fastclock   = wCBus.isfastclock(data->cbusini);
  data->fonfof      = wCBus.isfonfof(data->cbusini);
  data->slotserver  = wCBus.isslotserver(data->cbusini);
  data->purgetime   = wCBus.getpurgetime(data->cbusini);
  data->fcnode      = wCBus.getfcnode(data->cbusini);
  data->fcaddr      = wCBus.getfcaddr(data->cbusini);
  data->run         = True;
  data->device      = wDigInt.getdevice( data->ini );
  data->swtime      = wDigInt.getswtime( ini );

  data->run      = True;
  data->buson    = True;
  data->mux      = MutexOp.inst( NULL, True );
  data->lcmux    = MutexOp.inst( NULL, True );
  data->lcmap    = MapOp.inst();
  data->loaderMux= MutexOp.inst( NULL, True );
  data->stress   = wDigInt.isstress(ini);
  data->dummyio  = wDigInt.isdummyio(ini);
  data->wait4Ack = False;
  data->commandAck = wCBus.iscommandack(data->cbusini);

  if( data->purgetime < 1 || data->purgetime > 19 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "purgetime out of range: %d, reset to 10", data->purgetime );
    data->purgetime = 10;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MERG CBUS %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "http://www.merg.org.uk" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid           = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "canid         = %d", data->cid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SoD address   = %d", data->sodaddr );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "short events  = %s", data->shortevents ? "yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fast clock    = %s", data->fastclock ? "yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "use FON/FOF   = %s", data->fonfof ? "yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot server   = %s", data->slotserver ? "yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib        = %s", wDigInt.getsublib(data->ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switchtime    = %d", data->swtime );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "purgetime     = %d", data->purgetime );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "dummy I/O     = %s", data->dummyio ? "yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command ack   = %s", data->commandAck ? "yes":"no" );


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

    if( data->stress ) {
      data->stressRunner = ThreadOp.inst( "cbstress", &__stressRunner, __CBUS );
      ThreadOp.start( data->stressRunner );
    }

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
