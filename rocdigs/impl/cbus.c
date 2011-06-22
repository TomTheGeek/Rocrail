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

static int instCnt = 0;
/*
 * CBUS Priorities
 */
#define PRIORITY_HIGH 0
#define PRIORITY_ABOVE 1
#define PRIORITY_NORMAL 2
#define PRIORITY_LOW 3

#define OFFSET_OPC 7
#define OFFSET_D1 9
#define OFFSET_D2 11
#define OFFSET_D3 13
#define OFFSET_D4 15
#define OFFSET_D5 17
#define OFFSET_D6 19
#define OFFSET_D7 21

static void __translate( iOCBUS cbus, iONode node );
static int __makeFrame(iOCBUSData data, byte* frame, int prio, byte* cmd, int datalen );

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

  if( cmd != NULL ) {
    int bus = 0;
    __translate( (iOCBUS)inst, cmd );
    cmd->base.del(cmd);
  }
  return NULL;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return NULL;
}


static int __makeFrame(iOCBUSData data, byte* frame, int prio, byte* cmd, int datalen ) {
  int i = 0;
  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "makeframe for OPC=0x%02X", cmd[0] );

  StrOp.fmtb( frame+1, ":S%02X%02XN%02X;", (0x80 + (prio << 5) + (data->cid >> 3)) &0xFF, (data->cid << 5) & 0xFF, cmd[0] );

  if( datalen > 0 ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "datalen=%d", datalen );
    for( i = 0; i < datalen; i++ ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "makeframe: %s", frame+1 );
      StrOp.fmtb( frame+1+9+i*2, "%02X;", cmd[i+1] );
    }
  }

  frame[0] = StrOp.len(frame+1);

  return frame[0];
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOCBUSData data = Data(inst);
  data->run = False;
  if( poweroff ) {
    byte cmd[2];
    byte* frame = allocMem(32);
    cmd[0] = OPC_TOF;
    __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 0 );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power OFF" );
    ThreadOp.post(data->writer, (obj)frame);
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  ThreadOp.sleep(100);
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


static byte __HEXA2Byte( const char* s ) {
  char val[3] = {0};
  val[0] = s[0];
  val[1] = s[1];
  val[2] = '\0';
  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "HEXA=[%s]", val );
  return (unsigned char)(strtol( val, NULL, 16)&0xFF);
}




static int __getOPC(byte* frame) {
  int offset = (frame[1] == 'S') ? 0:4;
  int opc = __HEXA2Byte(frame+7+offset);
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


static iOSlot __getSlot(iOCBUSData data, iONode node) {
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
  slot->fx      = wLoc.getfx(node);
  slot->session = 0;


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
    __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 2 );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "request session for address %d", addr );
    ThreadOp.post(data->writer, (obj)frame);
  }


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for %s", wLoc.getid(node) );
  return slot;
}




/* Frame ASCII format
 * :ShhhhNd0d1d2d3d4d5d6d7d; :XhhhhhhhhNd0d1d2d3d4d5d6d7d;
 */
static void __updateSlot(iOCBUS cbus, byte* frame) {
  iOCBUSData data = Data(cbus);
  int offset  = (frame[1] == 'S') ? 0:4;
  int session = __HEXA2Byte(frame + OFFSET_D1 + offset);
  int addrh   = __HEXA2Byte(frame + OFFSET_D2 + offset);
  int addrl   = __HEXA2Byte(frame + OFFSET_D3 + offset);
  int addr    = addrh * 256 + addrl;
  iOSlot slot = __getSlotByAddr(data, addr);
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "update slot for session %d, address %d", session, addr );
    if( slot->session == 0 ) {
      byte cmd[5];
      byte* frame = allocMem(32);

      slot->session = session;

      cmd[0] = OPC_DFLG;
      cmd[1] = slot->session;
      if( slot->steps == 128 )
        cmd[2] = 0;
      else if( slot->steps == 14 )
        cmd[2] = 1;
      else if( slot->steps == 28 )
        cmd[2] = 3;
      cmd[2] |= (slot->lights ? 0x04:0x00);
      __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 2 );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set engine flags for loco %s", slot->id );
      ThreadOp.post(data->writer, (obj)frame);

    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "un-managed loco: session %d, address %d", session, addr );
  }
}


static __evaluateFB( iOCBUS cbus, byte* frame, Boolean state ) {
  iOCBUSData data = Data(cbus);

  int offset  = (frame[1] == 'S') ? 0:4;
  int nodeh   = __HEXA2Byte(frame + OFFSET_D1 + offset);
  int nodel   = __HEXA2Byte(frame + OFFSET_D2 + offset);
  int node    = nodeh * 256 + nodel;

  int eventh   = __HEXA2Byte(frame + OFFSET_D3 + offset);
  int eventl   = __HEXA2Byte(frame + OFFSET_D4 + offset);
  int event    = eventh * 256 + eventl;

  int addr = event;

  if( !data->shortevents )
    addr += (node << 16);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sevent %d", data->shortevents?"short ":"", addr );

  /* inform listener: Node3 */
  iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
  wFeedback.setaddr( nodeC, addr );
  wFeedback.setstate( nodeC, state?True:False );
  if( data->iid != NULL )
    wFeedback.setiid( nodeC, data->iid );

  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
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
  int addrh   = __HEXA2Byte(frame + OFFSET_D1 + offset);
  int addrl   = __HEXA2Byte(frame + OFFSET_D2 + offset);
  int addr    = addrh * 256 + addrl;

  int rfid1  = __HEXA2Byte(frame + OFFSET_D3 + offset);
  int rfid2  = __HEXA2Byte(frame + OFFSET_D4 + offset);
  int rfid3  = __HEXA2Byte(frame + OFFSET_D5 + offset);
  int rfid4  = __HEXA2Byte(frame + OFFSET_D6 + offset);
  int rfid5  = __HEXA2Byte(frame + OFFSET_D7 + offset);

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





static void __evaluateFrame(iOCBUS cbus, byte* frame, int opc) {
  iOCBUSData data = Data(cbus);
  int offset = (frame[1] == 'S') ? 0:4;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "evaluate OPC=0x%02X", opc );

  switch(opc) {
  case OPC_PLOC:
    __updateSlot(cbus, frame);
    break;
  case OPC_ACON:
  case OPC_ASON:
    __evaluateFB(cbus, frame, True);
    break;
  case OPC_ACOF:
  case OPC_ASOF:
    __evaluateFB(cbus, frame, False);
    break;
  case OPC_ACDAT:
    __evaluateRFID(cbus, frame);
    break;
  }
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
    byte* frame = allocMem(32);
    cmd[0] = OPC_ASRQ;
    cmd[1] = 0;
    cmd[2] = 0;
    cmd[3] = data->sodaddr / 256;
    cmd[4] = data->sodaddr % 256;
    __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 4 );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "query input state" );
    ThreadOp.post(data->writer, (obj)frame);
  }

  while( data->run ) {
    byte frame[32] = {0};
    /* Frame ASCII format
     * :ShhhhNd0d1d2d3d4d5d6d7d; :XhhhhhhhhNd0d1d2d3d4d5d6d7d;
     * :S    -> S=Standard X=extended start CAN Frame
     * hhhh  -> SIDH<bit7,6,5,4=Prio bit3,2,1,0=high 4 part of ID> SIDL<bit7,6,5=low 3 part of ID>
     * Nd    -> N=normal R=RTR
     * 0d    -> OPC 2 byte HEXA
     * 1d-7d -> data 2 byte HEXA
     * ;     -> end of frame
     */
    if( SerialOp.available(data->serial) ) {
      if( SerialOp.read(data->serial, frame, 1) ) {
        if( frame[0] == ':' ) {
          if( SerialOp.read(data->serial, frame+1, 1) ) {
            if( frame[1] == 'S' || frame[1] == 'X' ) {
              int offset = (frame[1] == 'S') ? 0:4;
              if( SerialOp.read(data->serial, frame + 2, OFFSET_OPC + offset ) ) {
                int opc = __getOPC(frame);
                int datalen = __getDataLen(opc);
                TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, 2 + OFFSET_OPC + offset );
                if( SerialOp.read(data->serial, frame + 2 + OFFSET_OPC + offset, datalen*2 + 1 ) ) {
                  TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, 2 + OFFSET_OPC + offset + datalen*2 + 1 );
                  __evaluateFrame(cbus, frame, opc);
                }
              }
            }
          }
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
    post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      /* first byte is the message length */
      len = post[0];
      MemOp.copy( out, post+1, len);
      freeMem( post);

      TraceOp.dump( name, TRCLEVEL_BYTE, (char*)out, len );
      if( !SerialOp.write( data->serial, (char*)out, len ) ) {
        /* sleep and send it again? */
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer ended." );

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
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "new timed command time=%d delay=%d tick=%d", cmd->time, cmd->delay, SystemOp.getTick() );
      ListOp.add(list, (obj)cmd);
    }

    int i = 0;
    for( i = 0; i < ListOp.size(list); i++ ) {
      iQCmd cmd = (iQCmd)ListOp.get(list, i);
      if( cmd->wait4session ) {
        /* TODO: Wait for session number. */
        if( cmd->slot->session > 0 ) {
          byte* outa = allocMem(32);
          cmd->out[1] = cmd->slot->session;
          MemOp.copy( outa, cmd->out, 32 );
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "wait4session %d command", cmd->slot->session );
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




static void __translate( iOCBUS cbus, iONode node ) {
  iOCBUSData data = Data(cbus);

  if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      /* CS off */
      byte cmd[2];
      byte* frame = allocMem(32);
      cmd[0] = OPC_TOF;
      __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 0 );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power OFF" );
      ThreadOp.post(data->writer, (obj)frame);
    }

    else if( StrOp.equals( cmdstr, wSysCmd.ebreak ) ) {
      /* CS ebreak */
      byte cmd[2];
      byte* frame = allocMem(32);
      cmd[0] = OPC_ESTOP;
      __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 0 );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "emergency break" );
      ThreadOp.post(data->writer, (obj)frame);
    }

    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS on */
      byte cmd[2];
      byte* frame = allocMem(32);
      cmd[0] = OPC_TON;
      __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 0 );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power ON" );
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
    __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 4 );

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
    __makeFrame(data, qcmd->out, PRIORITY_NORMAL, cmd, 4 );
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
    __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 4 );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "output %d:%d %s",
        wOutput.getaddr( node ), wOutput.getport( node ), on?"ON":"OFF" );
    ThreadOp.post(data->writer, (obj)frame);

  }


  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    byte cmd[5];
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    Boolean fn  = wLoc.isfn( node );
    Boolean dir = wLoc.isdir( node ); /* True == forwards */

    iOSlot slot = __getSlot(data, node );

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
      __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 2 );
      ThreadOp.post(data->writer, (obj)frame);
    }
    else {
      iQCmd qcmd = allocMem(sizeof(struct QCmd));
      qcmd->wait4session  = True;
      qcmd->slot = slot;
      cmd[0] = OPC_DSPD;
      cmd[1] = slot->session;
      cmd[2] = speed | (slot->dir ? 0x80:0x00);
      __makeFrame(data, qcmd->out, PRIORITY_NORMAL, cmd, 2 );
      ThreadOp.post( data->timedqueue, (obj)qcmd );
    }

  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    byte cmd[5];
    int   addr = wFunCmd.getaddr( node );

    iOSlot slot = __getSlot(data, node );

    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get slot for loco %s", wLoc.getid(node) );
      return;
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco speed=%d dir=%s", slot->speed, slot->dir?"forwards":"reverse" );

    if( slot->session > 0 ) {
      byte* frame = allocMem(32);
      cmd[0] = OPC_DFUN;
      cmd[1] = slot->session;
      /* TODO: Functions. */
      cmd[2] = 1; /* function range: 1=0-4, 2=5-8, 3=9-12, 4=13-19, 5=20-28*/
      cmd[3] = 0; /* the bits */
      __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 3 );
      ThreadOp.post(data->writer, (obj)frame);
    }
    else {
      iQCmd qcmd = allocMem(sizeof(struct QCmd));
      qcmd->wait4session  = True;
      qcmd->slot = slot;
      cmd[0] = OPC_DFUN;
      cmd[1] = slot->session;
      /* TODO: Functions. */
      cmd[2] = 1; /* function range: 1=0-4, 2=5-8, 3=9-12, 4=13-19, 5=20-28*/
      cmd[3] = 0; /* the bits */
      __makeFrame(data, qcmd->out, PRIORITY_NORMAL, cmd, 3 );
      ThreadOp.post( data->timedqueue, (obj)qcmd );
    }

  }

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
  data->run         = True;
  data->device      = wDigInt.getdevice( data->ini );
  data->swtime      = wDigInt.getswtime( ini );

  data->run      = True;
  data->mux      = MutexOp.inst( NULL, True );
  data->lcmux    = MutexOp.inst( NULL, True );
  data->lcmap    = MapOp.inst();

  if( StrOp.equals( wDigInt.sublib_usb, wDigInt.getsublib(data->ini) ))
    data->bps = 500000;
  else
    data->bps = 115200;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MERG CBUS %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "http://www.merg.org.uk" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid          = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "canid        = %d", data->cid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sod          = %d", data->sodaddr );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "short events = %s", data->shortevents ? "yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib       = %s", wDigInt.getsublib(data->ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device       = %s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps          = %d", data->bps );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switchtime   = %d", data->swtime );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, none );
  SerialOp.setLine( data->serial, data->bps, 8, 1, none, wDigInt.isrtsdisabled( ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );

  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {

    data->reader = ThreadOp.inst( "cbreader", &__reader, __CBUS );
    ThreadOp.start( data->reader );
    data->writer = ThreadOp.inst( "cbwriter", &__writer, __CBUS );
    ThreadOp.start( data->writer );
    data->timedqueue = ThreadOp.inst( "cbtimedq", &__timedqueue, __CBUS );
    ThreadOp.start( data->timedqueue );

  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init CBUS port!" );
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
