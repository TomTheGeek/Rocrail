/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis & Hagen Burchardt, Rocrail.net

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


#include "rocdigs/impl/ucons88_impl.h"

#include "rocs/public/mem.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/State.h"

static int instCnt = 0;

static void __reportState(iOuConS88Data data);

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOuConS88Data data = Data(inst);
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

/** ----- OuConS88 ----- */

static void __SensorQuery( iOuConS88 inst ) {
  iOuConS88Data data = Data(inst);
  byte* packet = NULL;

  packet = allocMem(32);
  packet[0] = 0x01;
  packet[1] = 0x30;
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "uConS88_QUERY_S88" );
  ThreadOp.post(data->writer, (obj)packet);
}

static void __translate( iOuConS88 inst, iONode node ) {
  iOuConS88Data data = Data(inst);

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SysCmd %s", cmd );

    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      data->power = False;
      __reportState(data);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
    }
    else if( StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Emergency break" );
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      data->power = True;
      __reportState(data);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
    }
    else if( StrOp.equals( cmd, wSysCmd.sod ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Start of Day [%s]", data->iid );
      __SensorQuery( inst );
    }
  }

}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOuConS88Data data = Data(inst);

  if( cmd != NULL ) {
    __translate( (iOuConS88)inst, cmd );

    /* Cleanup */
    NodeOp.base.del(cmd);
  }

  return NULL;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOuConS88Data data = Data(inst);
  byte* packet = NULL;

  packet = allocMem(32);
  packet[0] = 0x02;
  packet[1] = 0x00;
  packet[2] = 0x00;
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "uConS88_DEACTIVATE_S88" );
  ThreadOp.post(data->writer, (obj)packet);
  ThreadOp.sleep(200);
  data->run = False;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOuConS88Data data = Data(inst);
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
  iOuConS88Data data = Data(inst);
  int state = 0;
  state |= data->power << 0;
  //state |= data->pt << 1;
  state |= data->connAck << 2;
  return state;
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
  return vmajor*10000 + vminor*100 + patch;
}

static void __reportState(iOuConS88Data data) {
  if( wDigInt.isreportstate(data->ini) && data->listenerFun != NULL && data->listenerObj != NULL ) {
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );

    if( data->iid != NULL )
      wState.setiid( node, data->iid );

    wState.setpower( node, data->power );
    wState.settrackbus( node, data->power );
    wState.setsensorbus( node, data->connAck );
    wState.setaccessorybus( node, True );

    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}

static void __evaluatePacket(iOuConS88 inst, byte* packet, int packetSize) {
  iOuConS88Data data = Data(inst);
  int packetIdx = 0;
  int record = 0;
  
  if (packetSize) {
    if (packet[0] == 0x21) {
      //got Ack
      if( !data->connAck )
      {
        data->connAck = True;
        __reportState(data);
      }
      switch (packet[1]) {
        case 0x00:
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "uConS88_ACTIVE" );
          break;
        case 0x0F:
          if (packet[2]==6)
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "uConS88 connected MajorVer = %d, MinorVer = %d", packet[3], packet[4] );
          else
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "uConS88 wrong type %d MajorVer = %d, MinorVer = %d", packet[2], packet[3], packet[4] );
          break;
        case 0x34:
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "uConS88_CONFIG_ACCEPTED" );
          break;
        case 0x31:
          packetIdx = 3;
          for ( record=0; record<packet[2]; record++ )
          {
            int addr;
            if ( (packetIdx+2) >= packetSize ) {
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "uConS88 Sensor Record exceeds packet size" );
              break;
            }
            addr = (packet[packetIdx]-1)*16+packet[packetIdx+1];
            iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
            wFeedback.setaddr( nodeC, addr );
            wFeedback.setstate( nodeC, (packet[packetIdx+2]) ? True:False );
            if( data->iid != NULL )
              wFeedback.setiid( nodeC, data->iid );
            if( data->listenerFun != NULL && data->listenerObj != NULL )
              data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sensor %d = %s", addr, (packet[packetIdx+2])?"on":"off");
            packetIdx += 3;
          }
          break;
        case 0x30:
          packetIdx = 3;
          for ( record=0; record<packet[2]; record++ )
          {
            int bit;
            byte mask;
            if ( (packetIdx+2) >= packetSize ) {
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "uConS88 Sensor Record exceeds packet size" );
              break;
            }
            mask = 0x01;
            for ( bit=0; bit<8; bit++ ) {
              int addr;
              addr = record*16+bit+1;
              iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
              wFeedback.setaddr( nodeC, addr );
              wFeedback.setstate( nodeC, (packet[packetIdx+2]&mask) ? True:False );
              if( data->iid != NULL )
                wFeedback.setiid( nodeC, data->iid );
              if( data->listenerFun != NULL && data->listenerObj != NULL )
                data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sensor %d = %s", addr, (packet[packetIdx+2]&mask)?"on":"off");
              mask <<= 1;
            }
            mask = 0x01;
            for ( bit=0; bit<8; bit++ ) {
              int addr;
              addr = record*16+8+bit+1;
              iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
              wFeedback.setaddr( nodeC, addr );
              wFeedback.setstate( nodeC, (packet[packetIdx+1]&mask) ? True:False );
              if( data->iid != NULL )
                wFeedback.setiid( nodeC, data->iid );
              if( data->listenerFun != NULL && data->listenerObj != NULL )
                data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sensor %d = %s", addr, (packet[packetIdx+2]&mask)?"on":"off");
              mask <<= 1;
            }
            packetIdx += 3;
          }
          break;
      }
    }
    else if (packet[0] == 0x3F) {
      //NACK
      if( data->connAck )
      {
        data->connAck = False;
        __reportState(data);
      }
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "uConS88_NACK" );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "uConS88 invalid packet");
      TraceOp.dump ( name, TRCLEVEL_INFO, (char*)packet, packetSize );
    }
  }
}

static void __reader( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iOuConS88     s88  = (iOuConS88)ThreadOp.getParm( th );
  iOuConS88Data data = Data(s88);
  int flags = 0;
  byte* packet = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "uConS88 UDP reader started." );
  ThreadOp.sleep(500);

  packet = allocMem(32);
  packet[0] = 0x01;
  packet[1] = 0x0F;
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "uConS88_GET_FIRMWARE_VERSION" );
  ThreadOp.post(data->writer, (obj)packet);
  ThreadOp.sleep(100);

  packet = allocMem(32);
  packet[0] = 0x0B;
  packet[1] = 0x34;
  packet[2] = 0xAA;
  packet[3] = 0x55;
  packet[4] = data->fbB1;
  packet[5] = data->fbB2;
  packet[6] = data->fbB3;
  packet[7] = 0x00;
  packet[8] = 0x00;
  packet[9] = 0x00;
  packet[10] = 0x00;
  packet[11] = 0x00;
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "uConS88_CONFIGURE_S88" );
  ThreadOp.post(data->writer, (obj)packet);
  ThreadOp.sleep(100);

  packet = allocMem(32);
  packet[0] = 0x02;
  packet[1] = 0x00;
  packet[2] = 0x01;
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "uConS88_ACTIVATE_S88" );
  ThreadOp.post(data->writer, (obj)packet);
  ThreadOp.sleep(100);

  do {
    byte packet[256];
    MemOp.set( packet, 0, 256);

    int packetSize = SocketOp.recvfrom( data->socket, (char*)packet, 256, NULL, NULL );

    if( packetSize > 0 && packetSize < 256 ) {
      TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)packet, packetSize );
      __evaluatePacket(s88, packet, packetSize);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unexpected packet size %d received", packetSize );
      ThreadOp.sleep(10);
    }

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "uCons88 UDP reader stopped." );
}

static void __writer( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iOuConS88     s88  = (iOuConS88)ThreadOp.getParm( th );
  iOuConS88Data data = Data(s88);


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "uConS88 UDP writer started." );

  do {
    byte packet[256];
    byte * post = NULL;
    int len = 0;

    post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      /* first byte is the message length */
      len = post[0];
      MemOp.copy( packet, post+1, len);
      freeMem( post);
      TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)packet, len );
      data->connOK = SocketOp.sendto( data->socket, (char*)packet, len, NULL, 0 );
    }

    ThreadOp.sleep(10);

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "uCons88 UDP writer stopped." );
}

/**  */
static struct OuConS88* _inst( const iONode ini ,const iOTrace trc ) {
  iOuConS88 __uConS88 = allocMem( sizeof( struct OuConS88 ) );
  iOuConS88Data data = allocMem( sizeof( struct OuConS88Data ) );
  MemOp.basecpy( __uConS88, &uConS88Op, 0, sizeof( struct OuConS88 ), data );

  TraceOp.set( trc );
  /* Initialize data->xxx members... */
  data->ini     = ini;
  data->iid     = StrOp.dup( wDigInt.getiid( ini ) );
//  data->timeout = wDigInt.gettimeout( ini );
  if( data->fbB1 == 0 && data->fbB2 == 0 && data->fbB3 == 0){
    data->fbB1 = 5;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "µConS88 %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  bus 1 length %d", data->fbB1 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  bus 2 length %d", data->fbB2 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  bus 3 length %d", data->fbB3 );
//  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  timeout %dms", data->timeout );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  if( StrOp.len(wDigInt.gethost( data->ini )) == 0 ) {
    wDigInt.sethost( data->ini, "192.168.1.88");
  }

  if( wDigInt.getport( data->ini ) == 0 ) {
    wDigInt.setport( data->ini, 11082);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "host    = %s", wDigInt.gethost( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "port    = %d", wDigInt.getport( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  data->socket = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getport(data->ini), False, True, False );
  if( SocketOp.bind(data->socket) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "socket bound" );
    data->connOK = True;
  }

  data->reader = ThreadOp.inst( "ucons88reader", &__reader, __uConS88 );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( "ucons88writer", &__writer, __uConS88 );
  ThreadOp.start( data->writer );

  data->run = True;

  instCnt++;
  return __uConS88;
}

/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/ucons88.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
