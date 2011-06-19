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

#include "rocdigs/impl/cbus/cbus-const.h"

static int instCnt = 0;

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

  StrOp.fmtb( frame+1, ":S%02X%02XN%02X;", 0x80 + (prio << 5) + (data->cid >> 3), (data->cid << 5), cmd[0] );

  if( datalen > 0 ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "datalen=%d", datalen );
    for( i = 0; i < datalen; i++ ) {
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
    cmd[0] = CBUS_TOF;
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


static int __getOPC(byte* frame) {
  int offset = (frame[1] == 'S') ? 0:4;
  int opc = (frame[7+offset] - 0x30) * 10 + (frame[8+offset] - 0x30);
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
    cmd[0] = CBUS_RLOC;
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
  int session = (frame[9+offset] - 0x30) * 10 + (frame[10+offset] - 0x30);
  int addrh   = (frame[11+offset] - 0x30) * 10 + (frame[12+offset] - 0x30);
  int addrl   = (frame[13+offset] - 0x30) * 10 + (frame[14+offset] - 0x30);
  int addr    = addrh * 256 + addrl;
  iOSlot slot = __getSlotByAddr(data, addr);
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "update slot for session %d, address %d", session, addr );
    slot->session = session;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "un-managed loco: session %d, address %d", session, addr );
  }
}


static __evaluateFB( iOCBUS cbus, byte* frame ) {
  iOCBUSData data = Data(cbus);

  int offset  = (frame[1] == 'S') ? 0:4;
  int addrh   = (frame[9+offset] - 0x30) * 10 + (frame[10+offset] - 0x30);
  int addrl   = (frame[11+offset] - 0x30) * 10 + (frame[11+offset] - 0x30);
  int addr    = addrh * 256 + addrl;

  Boolean state = False;

  /* inform listener: Node3 */
  iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
  wFeedback.setaddr( nodeC, addr );
  wFeedback.setstate( nodeC, state?True:False );
  if( data->iid != NULL )
    wFeedback.setiid( nodeC, data->iid );

  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
}




static void __evaluateFrame(iOCBUS cbus, byte* frame, int opc) {
  iOCBUSData data = Data(cbus);
  int offset = (frame[1] == 'S') ? 0:4;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "evaluate OPC=%d", opc );

  switch(opc) {
  case CBUS_PLOC:
    __updateSlot(cbus, frame);
    break;
  case CBUS_ACON:
  case CBUS_ACOF:
    __evaluateFB(cbus, frame);
    break;
  }
}


static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOCBUS cbus = (iOCBUS)ThreadOp.getParm( th );
  iOCBUSData data = Data(cbus);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader started." );

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
              if( SerialOp.read(data->serial, frame + 2, 7 + offset ) ) {
                int opc = __getOPC(frame);
                int datalen = __getDataLen(opc);
                if( SerialOp.read(data->serial, frame + 2 + 7 + offset, datalen + 1 ) ) {
                  TraceOp.dump( name, TRCLEVEL_BYTE, (char*)frame, 2 + 7 + offset + datalen + 1 );
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
      cmd[0] = CBUS_TOF;
      __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 0 );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power OFF" );
      ThreadOp.post(data->writer, (obj)frame);
    }

    else if( StrOp.equals( cmdstr, wSysCmd.ebreak ) ) {
      /* CS ebreak */
      byte cmd[2];
      byte* frame = allocMem(32);
      cmd[0] = CBUS_ESTOP;
      __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 0 );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "emergency break" );
      ThreadOp.post(data->writer, (obj)frame);
    }

    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS on */
      byte cmd[2];
      byte* frame = allocMem(32);
      cmd[0] = CBUS_TON;
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

    cmd[0] = CBUS_ACON;
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
    cmd[0] = CBUS_ACOF;
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

    cmd[0] = on ? CBUS_ACON:CBUS_ACOF;
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
      cmd[0] = CBUS_DSPD;
      cmd[1] = slot->session;
      cmd[2] = speed | (slot->dir ? 0x80:0x00);
      __makeFrame(data, frame, PRIORITY_NORMAL, cmd, 2 );
      ThreadOp.post(data->writer, (obj)frame);
    }
    else {
      iQCmd qcmd = allocMem(sizeof(struct QCmd));
      qcmd->wait4session  = True;
      qcmd->slot = slot;
      cmd[0] = CBUS_DSPD;
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
      cmd[0] = CBUS_DFUN;
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
      cmd[0] = CBUS_DFUN;
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

  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->cid    = wCBus.getcid(data->cbusini);
  data->run    = True;
  data->device = wDigInt.getdevice( data->ini );
  data->swtime = wDigInt.getswtime( ini );

  data->run      = True;
  data->mux      = MutexOp.inst( NULL, True );
  data->lcmux    = MutexOp.inst( NULL, True );
  data->lcmap    = MapOp.inst();


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CBUS %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "http://www.merg.org.uk/resources/lcb.html" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid        = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cid        = %d", data->cid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device     = %s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps        = %d", wDigInt.getbps( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switchtime = %d", data->swtime );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, none );
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, none, wDigInt.isrtsdisabled( ini ) );
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
