/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
#include "rocs/public/rocs.h"
#include "rocs/public/objbase.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"

#include "rocdigs/impl/loconet_impl.h"

#include "rocdigs/impl/loconet/lnconst.h"
#include "rocdigs/impl/loconet/lnmaster.h"

#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/LNSlotServer.h"

/* Slot services */


struct __lnslot {
  Boolean inuse;// slot status1
  int status2;  // slot status2
  int steps;    // decoder speed steps
  int format;   // decoder format: "dcc,mm" 0,1
  int addr;     // decoder address
  int speed;    // current speed
  Boolean dir;  // direction
  Boolean f0;
  Boolean f1;
  Boolean f2;
  Boolean f3;
  Boolean f4;
  Boolean f5;
  Boolean f6;
  Boolean f7;
  Boolean f8;
  int idl;       // throttle ID low part
  int idh;       // throttle ID high part
  // fast clock
  int divider;
  int minutes;
  int hours;
  int init;
  time_t accessed;
};


/* prototyping handlers */
static int __locoaddress(iOLocoNet loconet, byte* msg, struct __lnslot* slot);
static int __getslotdata(iOLocoNet loconet, byte* msg, struct __lnslot* slot);
static int __moveslots  (iOLocoNet loconet, byte* msg, struct __lnslot* slot, int* dispatchedslot );
static int __slotstatus1(iOLocoNet loconet, byte* msg, struct __lnslot* slot);
static int __locodirf   (iOLocoNet loconet, byte* msg, struct __lnslot* slot);
static int __locosound  (iOLocoNet loconet, byte* msg, struct __lnslot* slot);
static int __locospeed  (iOLocoNet loconet, byte* msg, struct __lnslot* slot);
static int __setslotdata(iOLocoNet loconet, byte* msg, struct __lnslot* slot);
static iONode __sysCmd(iOLocoNet loconet, const char* cmd);
static iONode __locCmd(iOLocoNet loconet, int slotnr, struct __lnslot* slot, Boolean toLoco);
static iONode __funCmd(iOLocoNet loconet, int slotnr, struct __lnslot* slot);
static iONode __swCmd(iOLocoNet loconet, byte* req );


void lnmasterThread( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOLocoNet loconet = (iOLocoNet)ThreadOp.getParm( th );
  iOLocoNetData data = Data(loconet);

  struct __lnslot* slot = allocMem( 128 * sizeof(struct __lnslot) );
  int dispatchedslot = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet SlotServer started." );

  do {
    time_t currtime = time(NULL);
    obj post = ThreadOp.getPost( th );
    if( post != NULL ) {
      iONode node = (iONode)post;

      if( StrOp.equals( "quit", NodeOp.getName( node ) ) ) {
        node->base.del( node );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stopping SlotServer..." );
        break;
      }
      /* supposing a command node */
      {
        byte msg[64];
        int slotnr = -1;
        const char* arg = wCommand.getarg(node);
        int strlen = StrOp.len(arg);
        LocoNetOp.ascii2byte( arg, strlen, msg );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "message = [%s] len=%d", arg, strlen );
        TraceOp.dump ( name, TRCLEVEL_DEBUG, (char*)msg, strlen/2 );

        switch( msg[0] ) {
          case OPC_GPON:
            data->listenerFun( data->listenerObj, __sysCmd( loconet, wSysCmd.go), TRCLEVEL_INFO );
            break;
          case OPC_GPOFF:
          case OPC_IDLE:
            data->listenerFun( data->listenerObj, __sysCmd( loconet, wSysCmd.stop), TRCLEVEL_INFO );
            break;
          case OPC_SW_REQ:
            data->listenerFun( data->listenerObj, __swCmd(loconet, msg ), TRCLEVEL_INFO );
            break;
          case OPC_LOCO_ADR:
            slotnr = __locoaddress(loconet,msg,slot);
            break;
          case OPC_RQ_SL_DATA:
            slotnr = __getslotdata(loconet,msg,slot);
            break;
          case OPC_MOVE_SLOTS:
            slotnr = __moveslots(loconet,msg,slot,&dispatchedslot);
            break;
          case OPC_SLOT_STAT1:
            slotnr = __slotstatus1(loconet,msg,slot);
            break;
          case OPC_LOCO_DIRF:
            slotnr = __locodirf(loconet,msg,slot);
            break;
          case OPC_LOCO_SND:
            slotnr = __locosound(loconet,msg,slot);
            break;
          case OPC_LOCO_SPD:
            slotnr = __locospeed(loconet,msg,slot);
            break;
          case OPC_WR_SL_DATA:
            slotnr = __setslotdata(loconet,msg,slot);
            break;
        }

        if( slotnr != -1 ) {
          slot[slotnr].accessed = currtime;
        }

      }


      node->base.del( node );
    }

    /* check slots for setting to idle: purge */
    if( wLNSlotServer.ispurge(data->slotserver) && data->purgetime > 0 ) {
      int i = 0;
      for( i = 1; i < 120; i++ ) {
        if( slot[i].inuse && ((currtime - slot[i].accessed) > (data->purgetime)) ) {
          slot[i].inuse = False;
          slot[i].addr = 0;
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "slot# %d is purged", i );
          if( wLNSlotServer.isstopatpurge(data->slotserver) ) {
            slot[i].speed = 0;
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d speed is set to zero", slot[i].addr );
            data->listenerFun( data->listenerObj, __locCmd( loconet, i, slot, False), TRCLEVEL_INFO );
          }
        }
      }
    }


    ThreadOp.sleep(10);

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet SlotServer ended." );
}


static iONode __swCmd(iOLocoNet loconet, byte* req ) {
  iOLocoNetData data = Data(loconet);

  int addr = (((req[2] & 0x0f) * 128) + (req[1] & 0x7f));
  int value = (req[2] & 0x10) >> 4;
  int port  = (req[2] & 0x20) >> 5;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sw request %d=%s", addr, port?"straight":"thrown" );
  {
    iONode nodeCmd = NodeOp.inst( wCommand.name(), NULL, ELEMENT_NODE );
    iONode nodeSw  = NodeOp.inst( wSwitch.name(), nodeCmd, ELEMENT_NODE );

    NodeOp.addChild( nodeCmd, nodeSw );

    wSwitch.setaddr1( nodeSw, addr );
    wSwitch.setport1( nodeSw, port );

    wSwitch.setiid( nodeSw, wLNSlotServer.getiid( data->slotserver ) );

    wSwitch.setcmd( nodeSw, port?wSwitch.straight:wSwitch.turnout );

    return nodeCmd;
  }
}


static iONode __sysCmd(iOLocoNet loconet, const char* cmd) {
  iOLocoNetData data = Data(loconet);
  iONode nodeCmd = NodeOp.inst( wCommand.name(), NULL, ELEMENT_NODE );
  iONode nodeSys = NodeOp.inst( wSysCmd.name(), nodeCmd, ELEMENT_NODE );

  NodeOp.addChild( nodeCmd, nodeSys );
  wSysCmd.setcmd( nodeSys, cmd );

  wCommand.setiid( nodeCmd, wLNSlotServer.getiid( data->slotserver ) );
  wSysCmd.setiid( nodeSys, wLNSlotServer.getiid( data->slotserver ) );

  return nodeCmd;
}


static iONode __locCmd(iOLocoNet loconet, int slotnr, struct __lnslot* slot, Boolean toLoco) {
  iOLocoNetData data = Data(loconet);
  iONode nodeCmd = toLoco? NULL:NodeOp.inst( wCommand.name(), NULL, ELEMENT_NODE );
  iONode nodeSpd = NodeOp.inst( wLoc.name(), nodeCmd, ELEMENT_NODE );

  float speed = slot[slotnr].speed;
  speed *=100.0;
  speed /= 127.0;
  if( speed - ((int)speed) >= .5 )
    speed++;

  if( !toLoco )
   NodeOp.addChild( nodeCmd, nodeSpd );

  wLoc.setaddr( nodeSpd, slot[slotnr].addr );
  wLoc.setdir( nodeSpd, slot[slotnr].dir );
  wLoc.setV( nodeSpd, (int)speed );
  wLoc.setV_mode( nodeSpd, wLoc.V_mode_percent );
  wLoc.setfn( nodeSpd, slot[slotnr].f0 );
  wFunCmd.setf1( nodeSpd, slot[slotnr].f1 );
  wFunCmd.setf2( nodeSpd, slot[slotnr].f2 );
  wFunCmd.setf3( nodeSpd, slot[slotnr].f3 );
  wFunCmd.setf4( nodeSpd, slot[slotnr].f4 );
  wFunCmd.setf5( nodeSpd, slot[slotnr].f5 );
  wFunCmd.setf6( nodeSpd, slot[slotnr].f6 );
  wFunCmd.setf7( nodeSpd, slot[slotnr].f7 );
  wFunCmd.setf8( nodeSpd, slot[slotnr].f8 );

  wLoc.setprot( nodeSpd, slot[slotnr].format == 0 ? wLoc.prot_N:wLoc.prot_M );
  wLoc.setspcnt( nodeSpd, slot[slotnr].steps == 0 ? 128:slot[slotnr].steps );

  {
    char* throttleid = StrOp.fmt("%d", slot[slotnr].idl + slot[slotnr].idh * 127 );
    data->locothrottle[slotnr] = slot[slotnr].idl + slot[slotnr].idh * 127;
    wLoc.setthrottleid( nodeSpd, throttleid );
    StrOp.free(throttleid);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "slot# %d format=%d steps=%d speed=%d(%d) dir=%s inuse=%d throttleID=%d",
      slotnr, slot[slotnr].format, slot[slotnr].steps, slot[slotnr].speed, (int)speed,
      slot[slotnr].dir?"fwd":"rev", slot[slotnr].inuse, wLoc.getthrottleid( nodeSpd ) );

  /* TODO: send a wLoc node only to inform the loco object who knows all its settings,
   * the iid in this node must overwrite the one set for the loco. */
  wCommand.setiid( nodeCmd, wLNSlotServer.getiid( data->slotserver ) );
  wLoc.setiid( nodeSpd, wLNSlotServer.getiid( data->slotserver ) );

  return toLoco?nodeSpd:nodeCmd;
}


static iONode __funCmd(iOLocoNet loconet, int slotnr, struct __lnslot* slot) {
  iOLocoNetData data = Data(loconet);
  iONode nodeCmd = NodeOp.inst( wCommand.name(), NULL, ELEMENT_NODE );
  iONode nodeFun = NodeOp.inst( wFunCmd.name(), nodeCmd, ELEMENT_NODE );

  NodeOp.addChild( nodeCmd, nodeFun );
  wFunCmd.setaddr( nodeFun, slot[slotnr].addr );
  wFunCmd.setf0( nodeFun, slot[slotnr].f0 );
  wFunCmd.setf1( nodeFun, slot[slotnr].f1 );
  wFunCmd.setf2( nodeFun, slot[slotnr].f2 );
  wFunCmd.setf3( nodeFun, slot[slotnr].f3 );
  wFunCmd.setf4( nodeFun, slot[slotnr].f4 );
  wFunCmd.setf5( nodeFun, slot[slotnr].f5 );
  wFunCmd.setf6( nodeFun, slot[slotnr].f6 );
  wFunCmd.setf7( nodeFun, slot[slotnr].f7 );
  wFunCmd.setf8( nodeFun, slot[slotnr].f8 );

  wLoc.setdir( nodeFun, slot[slotnr].dir );
  wLoc.setV( nodeFun, (slot[slotnr].speed*100)/0x7F );
  wLoc.setV_mode( nodeFun, wLoc.V_mode_percent );
  wLoc.setfn( nodeFun, slot[slotnr].f0 );

  wLoc.setprot( nodeFun, slot[slotnr].format == 0 ? wLoc.prot_N:wLoc.prot_M );
  wLoc.setspcnt( nodeFun, slot[slotnr].steps == 0 ? 128:slot[slotnr].steps );

  {
    char* throttleid = StrOp.fmt("%d", slot[slotnr].idl + slot[slotnr].idh * 127 );
    data->locothrottle[slotnr] = slot[slotnr].idl + slot[slotnr].idh * 127;
    wLoc.setthrottleid( nodeFun, throttleid );
    StrOp.free(throttleid);
  }

  wCommand.setiid( nodeCmd, wLNSlotServer.getiid( data->slotserver ) );
  wCommand.setiid( nodeFun, wLNSlotServer.getiid( data->slotserver ) );

  return nodeCmd;
}


int lnLocoAddr(int addrH, int addrL) {
  return (((addrH & 0x7f) * 128) + (addrL & 0x7f));
}

static int __findSlot4Addr( int addr, struct __lnslot* slot, int* firstavail ) {
  int i = 0;
  *firstavail = -1;
  for( i = 1; i < 120; i++ ) {
    if( slot[i].addr == addr ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot# %d has address %d", i, addr );
      return i;
    }
    if( *firstavail == -1 && !slot[i].inuse && slot[i].addr == 0)
      *firstavail = i;
  }
  return -1; // send LACK back <B4> <3F> <0> <CHK>
}


static byte __getstat1byte(struct __lnslot* slot, int slotnr) {
  byte stat = 0;

  if( slot[slotnr].idl != 0 && slot[slotnr].idh != 0 ) {
    slot[slotnr].inuse = True;
  }

  stat |= (slot[slotnr].inuse?LOCO_IN_USE:0x00);

  if( slot[slotnr].format == 1 ) {
    stat |= DEC_MODE_28TRI;
  }
  else {
    if( slot[slotnr].steps == 14  ) stat |= DEC_MODE_14;
    if( slot[slotnr].steps == 28  ) stat |= DEC_MODE_28;
    if( slot[slotnr].steps == 128 ) stat |= DEC_MODE_128;
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot# %d inuse=%d", slotnr, slot[slotnr].inuse );
  return stat;
}


static int __LOCO_STAT(int s)   { // encode loco status as a string
   return ((s & LOCOSTAT_MASK) == LOCO_IN_USE) ? LOCO_IN_USE :
              ( ((s & LOCOSTAT_MASK) == LOCO_IDLE)   ? LOCO_IDLE :
        ( ((s & LOCOSTAT_MASK) == LOCO_COMMON) ? LOCO_COMMON:LOCO_FREE));
   }

static Boolean __setstat1byte(struct __lnslot* slot, int slotnr, byte stat) {
  Boolean toLoco = False;
  if( __LOCO_STAT(stat) == LOCO_IDLE ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot# %d released", slotnr );
    slot[slotnr].inuse = False;
    slot[slotnr].idl = 0;
    slot[slotnr].idh = 0;
    toLoco = True;
  }
  else
    slot[slotnr].inuse = __LOCO_STAT(stat) == LOCO_IN_USE ? True:False;

  slot[slotnr].format = 0;

  if( stat & DEC_MODE_128 ) {
    slot[slotnr].steps == 128;
  }
  else if( stat & DEC_MODE_28 ) {
    slot[slotnr].steps == 28;
  }
  else if( stat & DEC_MODE_14 ) {
    slot[slotnr].steps == 14;
  }
  else if( stat & DEC_MODE_28TRI )  {
    slot[slotnr].format = 1;
    slot[slotnr].steps == 28;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "set stat1byte for slot# %d format=%d steps=%d inuse=%d", slotnr, slot[slotnr].format, slot[slotnr].steps, slot[slotnr].inuse );

  return toLoco;
}


static byte __getdirfbyte(struct __lnslot* slot, int slotnr) {
  byte dirf = 0;
  dirf |= (slot[slotnr].dir?0x00:DIRF_DIR);
  dirf |= (slot[slotnr].f0?DIRF_F0:0x00);
  dirf |= (slot[slotnr].f1?DIRF_F1:0x00);
  dirf |= (slot[slotnr].f2?DIRF_F2:0x00);
  dirf |= (slot[slotnr].f3?DIRF_F3:0x00);
  dirf |= (slot[slotnr].f4?DIRF_F4:0x00);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot# %d dir=%d f0=%d", slotnr, slot[slotnr].dir, slot[slotnr].f0 );
  return dirf;
}


static byte __getsndbyte(struct __lnslot* slot, int slotnr) {
  byte snd = 0;
  snd |= (slot[slotnr].f5?SND_F5:0x00);
  snd |= (slot[slotnr].f6?SND_F6:0x00);
  snd |= (slot[slotnr].f7?SND_F7:0x00);
  snd |= (slot[slotnr].f8?SND_F8:0x00);
  return snd;
}


static byte __gettrkbyte(iOLocoNet loconet) {
  iOLocoNetData data = Data(loconet);
  byte trk = 0;
  trk |= (data->power?GTRK_POWER:0x00);
  trk |= (data->power?GTRK_IDLE:0x00);
  trk |= GTRK_MLOK1;
  return trk;
}


static void __slotdataRsp( iOLocoNet loconet, struct __lnslot* slot, int slotnr ) {
  byte rsp[32];
  rsp[0] = OPC_SL_RD_DATA;
  rsp[1] = 0x0E;
  rsp[2] = slotnr;
  rsp[3] = __getstat1byte(slot, slotnr);
  rsp[4] = slot[slotnr].addr & 0x7F;
  rsp[5] = slot[slotnr].speed;
  rsp[6] = __getdirfbyte(slot, slotnr);
  rsp[7] = __gettrkbyte(loconet);
  rsp[8] = 0x00;
  rsp[9] = (slot[slotnr].addr / 128) & 0x7F;
  rsp[10] = __getsndbyte(slot, slotnr);
  rsp[11] = slot[slotnr].idl;
  rsp[12] = slot[slotnr].idh;
  rsp[13] = LocoNetOp.checksum( rsp, 13);
  LocoNetOp.write( loconet, rsp, 14 );
}




static void __slotclockRsp( iOLocoNet loconet, struct __lnslot* slot ) {
  byte rsp[32];
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fast clock slot response" );
  rsp[0] = OPC_SL_RD_DATA;
  rsp[1] = 0x0E;
  rsp[2] = FC_SLOT;
  rsp[3] = slot[FC_SLOT].divider;
  rsp[4] = 0x7F;
  rsp[5] = 0x7F;
  rsp[6] = slot[FC_SLOT].minutes;
  rsp[7] = __gettrkbyte(loconet);
  rsp[8] = slot[FC_SLOT].hours;
  rsp[9] = 0;
  rsp[10] = 0x70;
  rsp[11] = 0x7F;
  rsp[12] = 0x70;
  rsp[13] = LocoNetOp.checksum( rsp, 13);
  LocoNetOp.write( loconet, rsp, 14 );
}


/*B4 6F 7F*/
static void __longAck( iOLocoNet loconet, int opc, int rc ) {
  byte rsp[32];
  rsp[0] = OPC_LONG_ACK;
  rsp[1] = (opc & 0x7F);
  rsp[2] = (rc & 0x7F);
  rsp[3] = LocoNetOp.checksum( rsp, 3);
  LocoNetOp.write( loconet, rsp, 4 );
}


static int __locoaddress(iOLocoNet loconet, byte* msg, struct __lnslot* slot) {
  iOLocoNetData data = Data(loconet);
  byte rsp[32];
  int addr = lnLocoAddr(msg[1], msg[2]);
  int avail = -1;
  int slotnr = __findSlot4Addr( addr, slot, &avail );

  if( slotnr == -1 && avail != -1 ) {
    slotnr = avail;
    /* set the slot */
    slot[slotnr].addr = addr;
    slot[slotnr].dir = True;
    /*slot[slotnr].inuse = True;*/
  }
  else if( slotnr == -1 && avail == -1 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no slot free for address %d", addr );
    /* send LACK */
    __longAck( loconet, OPC_LOCO_ADR, 0);
    return -1;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot# %d for loco address %d", slotnr, addr );
  /* send slot data */
  __slotdataRsp( loconet, slot, slotnr );
  return slotnr;
}


static int __getslotdata(iOLocoNet loconet, byte* msg, struct __lnslot* slot) {
  iOLocoNetData data = Data(loconet);
  int slotnr = msg[1] & 0x7F;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get slot# %d data", msg[1] );
  if( slotnr == FC_SLOT )
    __slotclockRsp( loconet, slot );
  else
    __slotdataRsp( loconet, slot, slotnr );
  return slotnr;
}



static int __moveslots(iOLocoNet loconet, byte* msg, struct __lnslot* slot, int* dispatchedslot) {
  iOLocoNetData data = Data(loconet);
  int src = msg[1] & 0x7F;
  int dst = msg[2] & 0x7F;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "move slots %d, %d (dps=%d)", src, dst, *dispatchedslot );

  if( src == 0 ) {
    /* DISPATCH GET */
    /* send slot data */
    __slotdataRsp( loconet, slot, *dispatchedslot );
    *dispatchedslot = 0;
  }
  else if( src == dst ) {
    /* NULL move: set slot inuse */
    slot[src].inuse = True;
    /* send slot data */
    __slotdataRsp( loconet, slot, src );
  }
  else if( dst == 0 ) {
    /* DISPATCH PUT */
    *dispatchedslot = src;
    /* send slot data */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set slot# %d dispatched", src );
    slot[src].inuse = True;
    __slotdataRsp( loconet, slot, *dispatchedslot );
  }
  return -1;
}


static int __slotstatus1(iOLocoNet loconet, byte* msg, struct __lnslot* slot) {
  iOLocoNetData data = Data(loconet);
  int slotnr = msg[1] & 0x7F;
  if(slotnr == 0 || slot[slotnr].addr == 0 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "illegal slot# %d addr %d", slotnr, slot[slotnr].addr );
    return slotnr;
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set slot# %d status", slotnr );
  data->listenerFun( data->listenerObj, __locCmd( loconet, slotnr, slot, __setstat1byte( slot, slotnr, msg[3]) ), TRCLEVEL_INFO );
  return slotnr;
}


static int __locodirf(iOLocoNet loconet, byte* msg, struct __lnslot* slot) {
  iOLocoNetData data = Data(loconet);
  int slotnr = msg[1] & 0x7F;
  if(slotnr == 0 || slot[slotnr].addr == 0 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "illegal slot# %d addr %d", slotnr, slot[slotnr].addr );
    return slotnr;
  }

  slot[slotnr].dir = (msg[2] & DIRF_DIR) ? False:True;
  slot[slotnr].f0  = (msg[2] & DIRF_F0) ? True:False;
  slot[slotnr].f1  = (msg[2] & DIRF_F1) ? True:False;
  slot[slotnr].f2  = (msg[2] & DIRF_F2) ? True:False;
  slot[slotnr].f3  = (msg[2] & DIRF_F3) ? True:False;
  slot[slotnr].f4  = (msg[2] & DIRF_F4) ? True:False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "set slot# %d dirf; dir=%s fn=%s", slotnr, slot[slotnr].dir?"fwd":"rev", slot[slotnr].f0?"on":"off" );
  data->listenerFun( data->listenerObj, __locCmd( loconet, slotnr, slot, False), TRCLEVEL_INFO );
  data->listenerFun( data->listenerObj, __funCmd( loconet, slotnr, slot), TRCLEVEL_INFO );
  return slotnr;
}


static int __locosound(iOLocoNet loconet, byte* msg, struct __lnslot* slot) {
  iOLocoNetData data = Data(loconet);
  int slotnr = msg[1] & 0x7F;
  if(slotnr == 0 || slot[slotnr].addr == 0 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "illegal slot# %d addr %d", slotnr, slot[slotnr].addr );
    return slotnr;
  }
  slot[slotnr].f5  = (msg[2] & SND_F5) ? True:False;
  slot[slotnr].f6  = (msg[2] & SND_F6) ? True:False;
  slot[slotnr].f7  = (msg[2] & SND_F7) ? True:False;
  slot[slotnr].f8  = (msg[2] & SND_F8) ? True:False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set slot# %d snd", slotnr );
  data->listenerFun( data->listenerObj, __funCmd( loconet, slotnr, slot), TRCLEVEL_INFO );
  return slotnr;
}


static int __locospeed(iOLocoNet loconet, byte* msg, struct __lnslot* slot) {
  iOLocoNetData data = Data(loconet);
  int slotnr = msg[1] & 0x7F;
  if(slotnr == 0 || slot[slotnr].addr == 0 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "illegal slot# %d addr %d", slotnr, slot[slotnr].addr );
    return slotnr;
  }
  slot[slotnr].speed = msg[2] & 0x7F;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set slot# %d speed to %d", slotnr, slot[slotnr].speed );
  data->listenerFun( data->listenerObj, __locCmd( loconet, slotnr, slot, False), TRCLEVEL_INFO );
  return slotnr;
}


static int __setslotdata(iOLocoNet loconet, byte* msg, struct __lnslot* slot) {
  iOLocoNetData data = Data(loconet);
  int slotnr = msg[2] & 0x7F;
  int addr = ((msg[9] & 0x7f) * 128) + (msg[4] & 0x7f);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "set slot# %d addr %d data (dir=%s)", slotnr, addr, ((msg[6] & DIRF_DIR) ? "rev":"fwd") );

  if( addr == 0 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "illegal address %d", addr );
    __longAck( loconet, OPC_WR_SL_DATA, 0 );
    return slotnr;
  }

  /* translating slotdata into the internal representation: */
  /* EF 0E 01 30 73 3C 00 04 00 00 00 25 14 55 */
  /* rwSlotData = (rwSlotDataMsg *) msgBuf;
   int command   = msg[0];
   int mesg_size = msg[1];     // size of the message in bytes
   int slot      = msg[2];     // slot number for this request
   int stat      = msg[3];     // slot status
   int adr       = msg[4];     // loco address
   int spd       = msg[5];     // command speed
   int dirf      = msg[6];     // direction and F0-F4 bits
   int trk       = msg[7];     // track status
   int ss2       = msg[8];     // slot status 2 (tells how to use ID1/ID2 & ADV Consist)
   int adr2      = msg[9];     // loco address high
   int snd       = msg[10];    // Sound 1-4 / F5-F8
   int id1       = msg[11];    // ls 7 bits of ID code
   int id2       = msg[12];    // ms 7 bits of ID code
   */

  if( slotnr == FC_SLOT ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set fast clock slot" );
    slot[slotnr].divider = msg[3];
    slot[slotnr].minutes = msg[6];
    slot[slotnr].hours   = msg[8];
  }
  else {
    slot[slotnr].addr  = addr;
    slot[slotnr].speed = msg[5];
    slot[slotnr].dir   = ((msg[6] & DIRF_DIR) != 0 ? False:True); /* True is fwd in Rocrail */
    slot[slotnr].f0    = ((msg[6] & DIRF_F0) != 0 ? True:False);
    slot[slotnr].f1    = ((msg[6] & DIRF_F1) != 0 ? True:False);
    slot[slotnr].f2    = ((msg[6] & DIRF_F2) != 0 ? True:False);
    slot[slotnr].f3    = ((msg[6] & DIRF_F3) != 0 ? True:False);
    slot[slotnr].f4    = ((msg[6] & DIRF_F4) != 0 ? True:False);
    slot[slotnr].f5    = ((msg[10] & SND_F5) != 0 ? True:False);
    slot[slotnr].f6    = ((msg[10] & SND_F6) != 0 ? True:False);
    slot[slotnr].f7    = ((msg[10] & SND_F7) != 0 ? True:False);
    slot[slotnr].f8    = ((msg[10] & SND_F8) != 0 ? True:False);
    slot[slotnr].idl   = msg[11];
    slot[slotnr].idh   = msg[12];

    data->listenerFun( data->listenerObj, __locCmd( loconet, slotnr, slot, __setstat1byte( slot, slotnr, msg[3])), TRCLEVEL_INFO );
    data->listenerFun( data->listenerObj, __funCmd( loconet, slotnr, slot), TRCLEVEL_INFO );
  }
  __longAck( loconet, OPC_WR_SL_DATA, -1 );

  if( slotnr == FC_SLOT && slot[slotnr].init == 0) {
    slot[slotnr].init = 1;
    __slotclockRsp(loconet, slot);
  }
  return slotnr;
}


/*
 * DCS100 answers Fredi
 *

=====FREDI
20080901.223201.278 r9999c lnreader lnmon    0288 Request slot for loco address 54
20080901.223201.278 r0000B lnreader OLocoNet *trace dump( 0xB7C8632C: length=4 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
    00000000: BF 00 36 76                                     |..6v            |


=====DCS100
20080901.223201.288 r9999c lnreader lnmon    1327 Read slot=8 addr=54, Not Consisted, in use, 28, dir=fwd speed=1 fun=on,off,off,off,off,off,off,off,off
20080901.223201.288 r9999c lnreader lnmon    1347 track=OFF power=DOWN PT=avail
20080901.223201.288 r0000B lnreader OLocoNet *trace dump( 0xB7C8632C: length=14 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
    00000000: E7 0E 08 30 36 01 10 04 00 00 00 08 74 71       |...06.......tq  |
20080901.223201.308 r9999c lnreader lnmon    1327 Write slot=8 addr=54, Not Consisted, in use, 28, dir=fwd speed=1 fun=on,off,off,off,off,off,off,off,off
20080901.223201.308 r9999c lnreader lnmon    1347 track=OFF power=DOWN PT=avail
20080901.223201.309 r0000B lnreader OLocoNet *trace dump( 0xB7C8632C: length=14 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
    00000000: EF 0E 08 30 36 01 10 04 00 00 00 08 74 79       |...06.......ty  |
20080901.223201.309 r9999c lnreader lnmon    0483 LONG_ACK: Function not implemented, no reply will follow
20080901.223201.309 r0000B lnreader OLocoNet *trace dump( 0xB7C8632C: length=4 )
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
    --------------------------------------------------------- |----------------|
    00000000: B4 6F 7F 5B                                     |.o.[            |



*/
