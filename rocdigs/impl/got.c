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


/*
    =====Position data=====
    Data is delivered in a comma-separated string. For every receiver there is an ID, distance
    and level:
    <Time>, <Sender ID>, <x>, <y>, <z>, <Receiver ID>, <Distance>, <Level>
    <Time> - Milliseconds after start”
    <Sender ID> - The specific transmitter ID, can be seen on the label
    <valid measurement> - 1 for valid measurement, 0 for invalid
    <x> - The x-coordinate in mm
    <y> - The y-coordinate in mm
    <z> - The z-coordinate in mm (is normally negative in a right handed coordinate system)
    <Receiver ID> - The specific receiver ID, can be seen on the label
    <Distance> - The measured distance in mm
    <Level> - The level of the measured ultrasound signal at the receiver. From 0-1000, with
    0 being the lowest.

    An example of a line of strings can be seen below:

    55924,11000,1,1190,871,-1435,20104,2059,918,20103,1727,966,20105,1498,999;
    56074,11015,1,1055,712,-1430,20104,1937,912,20103,1756,922,20105,1312,865;
    56224,11000,1,1189,873,-1434,20104,2056,917,20103,1726,960,20105,1503,987;
    56374,11015,1,1052,716,-1430,20104,1930,910,20103,1756,925,20105,1320,860;

 */


#include "rocdigs/impl/got_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Feedback.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOGOTData data = Data(inst);
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

/** ----- OGOT ----- */

static iONode __translate( iOGOT inst, iONode node ) {
  iOGOTData data = Data(inst);
  iONode rsp = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cmd=%s", NodeOp.getName( node ) );

  return rsp;
}

/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOGOTData data = Data(inst);
  iONode rsp = NULL;

  if( cmd != NULL ) {
    rsp = __translate((iOGOT)inst, cmd);
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
  iOGOTData data = Data(inst);
  data->run = False;
  ThreadOp.sleep(100);
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOGOTData data = Data(inst);
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
  iOGOTData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static Boolean __connectToGoT(iOGOT got) {
  iOGOTData data = Data(got);

  data->socket = SocketOp.inst( wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ), False, False, False );
  SocketOp.setNodelay( data->socket, True );
  SocketOp.setBlocking( data->socket, True );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "trying to connect to %s:%d...",
      wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );
  if ( SocketOp.connect( data->socket ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "connected to %s:%d",
        wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );
    return True;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unable to connect to %s:%d; check the network...",
        wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );
    if( data->socket != NULL ) {
      SocketOp.base.del( data->socket );
      data->socket = NULL;
    }
  }
  return False;
}


static void __evaluateRecord(iOGOT got, const char* msg) {
  iOGOTData data = Data(got);
  iOStrTok tok = StrTokOp.inst( msg, ',' );
  char ident[32];
  int idx = 0;
  int t = 0;
  int x = 0;
  int y = 0;
  int z = 0;
  int sid = 0;
  int valid = 0;

  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "record: [%s]", msg );

  /* report to the Rocrail server */
  while( StrTokOp.hasMoreTokens( tok ) ) {
    int val = atoi( StrTokOp.nextToken(tok) );
    switch( idx ) {
    case 0: t = val; break;
    case 1: sid = val; break;
    case 2: valid = val; break;
    case 3: x = val; break;
    case 4: y = val; break;
    case 5: z = val; break;
    }
    idx++;
  };
  StrTokOp.base.del(tok);

  StrOp.fmtb(ident, "%d", sid);

  if( valid ) {
    Boolean newSid = False;
    iOSender sender = (iOSender)MapOp.get(data->sidMap, ident );

    if( sender == NULL ) {
      sender = allocMem( sizeof( struct sender) );
      sender->sid = sid;
      sender->x = x;
      sender->y = y;
      sender->z = z;
      MapOp.put(data->sidMap, ident, (obj)sender);
      newSid = True;
    }

    if( newSid || sender->x != x || sender->y != y || sender->z != z  ) {
      iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
      wFeedback.setgpstime( nodeC, t );
      wFeedback.setgpssid( nodeC, sid );
      wFeedback.setgpsx( nodeC, x );
      wFeedback.setgpsy( nodeC, y );
      wFeedback.setgpsz( nodeC, z );
      wFeedback.setfbtype( nodeC, wFeedback.fbtype_gps );
      if( data->iid != NULL )
        wFeedback.setiid( nodeC, data->iid );
      wFeedback.setstate( nodeC, True );

      wFeedback.setidentifier( nodeC, ident);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "t=%d sid=%d x=%d y=%d z=%d", t, sid, x, y, z );
      data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

      sender->x = x;
      sender->y = y;
      sender->z = z;
    }
  }
}


/**
 * Reading the GameOnTrack Central directly over USB.
 * This serial reading is reengineerd and the protocol is not public; I did ask GOT.
 * Under Mac OSX the FDTI device driver will crash the system if the continous byte stream is not read permanently. (Buffer over flow?)
 */
static void __serialReader( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iOGOT     got  = (iOGOT)ThreadOp.getParm( th );
  iOGOTData data = Data(got);
  Boolean   serialOK = False;
  int idx = 0;

  /* 35 init bytes which activates the transmitter and the receivers which depends on the IDs of the hardware in question.
   * In this case the receivers are C856 and CB56, the sender is C347.
   */
  byte msg[1014] = {0x02, 0x28, 0x01, 0x10, 0x22, 0x0A, 0xCA, 0x03, 0x02, 0x2A, 0x00, 0x01, 0xC8, 0x56, 0x00, 0xB6, 0x03, 0x02, 0x2A, 0x01, 0x01, 0xCB,
                    0x56, 0x00, 0xB2, 0x03, 0x02, 0xAA, 0x00, 0x01, 0x47, 0x29, 0x00, 0xE4, 0x03};

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GoT serial reader started." );

  ThreadOp.sleep(50); /* resume some time to get it all being setup */

  data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
  SerialOp.setFlow( data->serial, cts );
  SerialOp.setLine( data->serial, 115200, 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout(data->ini), wDigInt.gettimeout(data->ini) );
  serialOK = SerialOp.open( data->serial );

  if( serialOK ) {
    SerialOp.write( data->serial, (char*)msg, 35 );
  }

  while( serialOK && data->run ) {
    Boolean eol = False;

    /*
     02 43 0A 00 10 22 00 C3 47 29 00 7D 03 02 84 B6
     C8 56 00 4E 22 40 00 F7 03 02 04 B4 CB 56 00 73
     2F 10 36 00 6E 03 02 C1 47 29 00 0A 40 43 10 22
     3F 03
     */
    if( SerialOp.available( data->serial ) ) {
      int rc = SerialOp.read( data->serial, (char*)msg+idx, 1 );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "GoT read: idx=%d 0x%02X", idx, msg[idx] );

      if( idx >= 50 ) {
        /* ToDo: How to determine the start or end of the packet... */
        TraceOp.dump( name, TRCLEVEL_INFO, (char*)msg, idx );
        idx = 0;
      }
      else
        idx++;

    }

    ThreadOp.sleep(10);
  }

  SerialOp.close( data->serial );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GoT serial reader ended." );

}


static void __socketReader( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iOGOT     got  = (iOGOT)ThreadOp.getParm( th );
  iOGOTData data = Data(got);

  char msg[1014];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GoT socket reader started." );

  ThreadOp.sleep(50); /* resume some time to get it all being setup */

  /* Test:
  StrOp.copy(msg, "55924,11000,1,1190,871,-1435,20104,2059,918,20103,1727,966,20105,1498,999;");
  __evaluateRecord(got, msg);
  */

  while( data->run ) {
    int idx = 0;
    Boolean eol = False;

    if( data->socket == NULL ) {
      if( !__connectToGoT(got) ) {
        ThreadOp.sleep(5000);
        continue;
      }
    }

    /*
    <Time>, <Sender ID>, <x>, <y>, <z>, <Receiver ID>, <Distance>, <Level>
    <Time> - Milliseconds after start”
    <Sender ID> - The specific transmitter ID, can be seen on the label
    <valid measurement> - 1 for valid measurement, 0 for invalid
    <x> - The x-coordinate in mm
    <y> - The y-coordinate in mm
    <z> - The z-coordinate in mm (is normally negative in a right handed coordinate system)
    <Receiver ID> - The specific receiver ID, can be seen on the label
    <Distance> - The measured distance in mm
    <Level> - The level of the measured ultrasound signal at the receiver. From 0-1000, with
    0 being the lowest.

    Time  SID   v x    y   z       RID1             RID2             RID3
    55924,11000,1,1190,871,-1435,  20104,2059,918,  20103,1727,966,  20105,1498,999;
    */

    while( !eol && !SocketOp.isBroken(data->socket) ) {
      SocketOp.read( data->socket, msg+idx, 1 );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "GoT read: %c(0x%02X)", msg[idx], msg[idx] );

      if( msg[idx] == '\r' ) {
        eol = True;
      }
      else {
        idx++;
      }
      msg[idx] = '\0';
    }

    if( SocketOp.isBroken(data->socket) ) {
      SocketOp.base.del(data->socket);
      data->socket = NULL;
    }
    else if( eol ) {
      StrOp.replaceAll(msg, '\n', '\0');
      StrOp.replaceAll(msg, '\r', '\0');
      __evaluateRecord(got, msg);
    }

    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GoT socket reader ended." );
}


/**  */
static struct OGOT* _inst( const iONode ini ,const iOTrace trc ) {
  iOGOT __GOT = allocMem( sizeof( struct OGOT ) );
  iOGOTData data = allocMem( sizeof( struct OGOTData ) );
  MemOp.basecpy( __GOT, &GOTOp, 0, sizeof( struct OGOT ), data );

  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */
  data->ini      = ini;
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );
  data->sidMap   = MapOp.inst();
  data->run      = True;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GamesOnTrack %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid      = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib   = %s", wDigInt.getsublib( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "host     = %s", wDigInt.gethost(data->ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "port     = %d", wDigInt.getport(data->ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device   = %s", wDigInt.getdevice( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  if( StrOp.equals( wDigInt.sublib_serial, wDigInt.getsublib( data->ini ) ) ) {
    data->reader = ThreadOp.inst( "gotreader", &__serialReader, __GOT );
    ThreadOp.start( data->reader );
  }
  else {
    data->reader = ThreadOp.inst( "gotreader", &__socketReader, __GOT );
    ThreadOp.start( data->reader );
  }

  instCnt++;
  return __GOT;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/got.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
