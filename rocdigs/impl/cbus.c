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


static void __evaluateFrame(iOCBUS cbus, byte* frame, int opc) {
  iOCBUSData data = Data(cbus);
  int offset = (frame[1] == 'S') ? 0:4;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "OPC=%d", opc );

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
      if( (cmd->time + cmd->delay) <= SystemOp.getTick() ) {
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

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
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
