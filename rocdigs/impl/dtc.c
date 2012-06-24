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

#include "rocdigs/impl/dtc_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Program.h"


#define DEVICE 193
#define OPC_START 1
#define OPC_SETPOS 5
#define OPC_SETTYPE 26
#define EVT_STATUS 10


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iODTCData data = Data(inst);
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

/** ----- ODTC ----- */

static byte __checksum(byte* datagram, int len) {
  int i = 0;
  byte chk = datagram[0];
  for( i = 1; i < len; i++) {
    chk = chk ^ datagram[i];
  }
  return chk % 128;
}

static iONode __translate( iODTC inst, iONode node ) {
  iODTCData data = Data(inst);
  iONode rsp = NULL;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "translate: %s", NodeOp.getName(node) );


  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS on */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power ON" );
      byte* cmd = allocMem(32);
      cmd[ 0] = 4;
      cmd[ 1] = DEVICE;
      cmd[ 2] = OPC_START;
      cmd[ 3] = 0;
      cmd[ 4] = __checksum(cmd+1, 3);
      ThreadOp.post(data->writer, (obj)cmd);
    }

    /* PT */
    else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
      if( wProgram.getcmd( node ) == wProgram.set ) {
        byte* cmd = allocMem(32);
        cmd[ 0] = 7;
        cmd[ 1] = DEVICE;
        cmd[ 2] = OPC_SETPOS;
        cmd[ 3] = 3;
        cmd[ 4] = wProgram.getval1( node ); /* Direction: 1 or 2 */
        cmd[ 5] = wProgram.getvalue( node ); /* Position: 1-48 */
        cmd[ 6] = 0;
        cmd[ 7] = __checksum(cmd+1, 6);
        ThreadOp.post(data->writer, (obj)cmd);
      }
    }
  }

  return rsp;
}

/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iODTCData data = Data(inst);
  iONode rsp = NULL;

  if( cmd != NULL ) {
    int bus = 0;
    rsp = __translate( (iODTC)inst, cmd );
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
  iODTCData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  data->run = False;
  ThreadOp.sleep(500);
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iODTCData data = Data(inst);
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
  iODTCData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iODTC dtc = (iODTC)ThreadOp.getParm( th );
  iODTCData data = Data(dtc);
  byte* cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer started." );
  ThreadOp.sleep(1000);

  while( data->run ) {
    byte * post = NULL;
    int len = 0;
    byte out[32] = {0};

    ThreadOp.sleep(10);
    post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      /* first byte is the message length */
      len = post[0];
      MemOp.copy( out, post+1, len);
      freeMem( post);

      TraceOp.dump( NULL, TRCLEVEL_INFO, (char*)out, len );
      if( !SerialOp.write( data->serial, (char*)out, len ) ) {
        /* sleep and send it again? */
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer ended." );
}


static void __evaluateDatagram(iODTC dtc, byte* datagram) {
  iODTCData data = Data(dtc);
  switch( datagram[1] ) {
  case OPC_START:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "START confirmation." );
    if( wDigInt.getprotver(data->ini) != 0 ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power ON" );
      byte* cmd = allocMem(32);
      cmd[ 0] = 6;
      cmd[ 1] = DEVICE;
      cmd[ 2] = OPC_SETTYPE;
      cmd[ 3] = 2;
      cmd[ 4] = wDigInt.getprotver(data->ini);
      cmd[ 5] = 0;
      cmd[ 6] = __checksum(cmd+1, 5);
      ThreadOp.post(data->writer, (obj)cmd);
    }
    break;
  case EVT_STATUS:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "status=%d pos=%d relais=%d", datagram[3], datagram[4], datagram[5] );
    if( datagram[3] == 0 ) {
      iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
      wFeedback.setaddr( nodeC, 10001 );
      if( data->iid != NULL )
        wFeedback.setiid( nodeC, data->iid );
      wFeedback.setstate( nodeC, True );
      wFeedback.setcounter( nodeC, datagram[4] );

      data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
    }
    break;
  }
}


static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iODTC dtc = (iODTC)ThreadOp.getParm( th );
  iODTCData data = Data(dtc);
  byte* cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader started." );
  ThreadOp.sleep(100);

  while( data->run ) {
    byte in[32] = {0};
    int len = 4;

    ThreadOp.sleep(10);
    if( SerialOp.available(data->serial) ) {
      SerialOp.read(data->serial, in, 1);
      if( in[0] & 0x80 ) {
        MemOp.set(in+1, 0, 31);
        SerialOp.read(data->serial, in+1, 3);
        if( in[2] > 0 ) {
          len += in[2];
          SerialOp.read(data->serial, in+4, in[2]);
        }
        TraceOp.dump ( name, TRCLEVEL_INFO, (char*)in, len );
        __evaluateDatagram(dtc, in);
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader ended." );
}



/**  */
static struct ODTC* _inst( const iONode ini ,const iOTrace trc ) {
  iODTC __DTC = allocMem( sizeof( struct ODTC ) );
  iODTCData data = allocMem( sizeof( struct ODTCData ) );
  MemOp.basecpy( __DTC, &DTCOp, 0, sizeof( struct ODTC ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */

  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DTC %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = %s", wDigInt.getdevice( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->run = True;

  data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );
  SerialOp.setFlow( data->serial, cts );
  SerialOp.setLine( data->serial, 19200, 8, 1, 0, wDigInt.isrtsdisabled( ini ) );
  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {
    data->reader = ThreadOp.inst( "dtcreader", &__reader, __DTC );
    ThreadOp.start( data->reader );
    data->writer = ThreadOp.inst( "dtcwriter", &__writer, __DTC );
    ThreadOp.start( data->writer );
  }
  else
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init DTC port!" );

  instCnt++;
  return __DTC;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/dtc.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
