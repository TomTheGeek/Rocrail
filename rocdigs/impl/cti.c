/** ------------------------------------------------------------
  * A U T O   G E N E R A T E D  (First time only!)
  * Generator: Rocs ogen (build Dec 21 2011 08:11:37)
  * Module: RocDigs
  * XML: $Source: /cvsroot/rojav/rocdigs/rocdigs.xml,v $
  * XML: $Revision: 1.14 $
  * Object: CTI
  * Date: Thu Dec 29 14:23:11 2011
  * ------------------------------------------------------------
  * $Source$
  * $Author$
  * $Date$
  * $Revision$
  * $Name$
  */

#include "rocdigs/impl/cti_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"
#include "rocs/public/system.h"

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

#include "rocutils/public/addr.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOCTIData data = Data(inst);
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

/** ----- OCTI ----- */

static void __translate( iOCTI inst, iONode node ) {
  iOCTIData data = Data(inst);


  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.ebreak ) ) {
      /* CS ebreak */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request emergency break" );
      byte* cmd = allocMem(32);
      cmd[ 0] = 1;
      cmd[ 1] = 0x0B;
      ThreadOp.post(data->writer, (obj)cmd);
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int addr = wSwitch.getaddr1( node );
    int gate = wSwitch.getgate1( node );

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ) {
      addr += 1;
    }

    byte* cmd = allocMem(32);
    cmd[ 0] = 4;
    cmd[ 1] = 0x03;
    cmd[ 2] = addr / 256;
    cmd[ 3] = addr % 256;
    cmd[ 4] = 25;
    ThreadOp.post(data->writer, (obj)cmd);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "switch %d %s", addr, wSwitch.getcmd( node ) );
  }



}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOCTIData data = Data(inst);

  if( cmd != NULL ) {
    int bus = 0;
    __translate( (iOCTI)inst, cmd );
    cmd->base.del(cmd);
  }
  return NULL;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOCTIData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  data->run = False;
  ThreadOp.sleep(500);
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOCTIData data = Data(inst);
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
  iOCTIData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOCTI cti = (iOCTI)ThreadOp.getParm( th );
  iOCTIData data = Data(cti);
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

      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
      if( !SerialOp.write( data->serial, (char*)out, len ) ) {
        /* sleep and send it again? */
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer ended." );
}



static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOCTI cti = (iOCTI)ThreadOp.getParm( th );
  iOCTIData data = Data(cti);
  byte* cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader started." );
  ThreadOp.sleep(100);

  while( data->run ) {
    byte in[32] = {0};

    ThreadOp.sleep(10);
    if( SerialOp.available(data->serial) ) {
      SerialOp.read(data->serial, in, 1);
      TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)in, 1 );
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader ended." );
}



/**
 * The COM port should be set for operation at 9600 Baud, with 8 data bits, 1 stop bit, and no parity.
 *
 */
static struct OCTI* _inst( const iONode ini ,const iOTrace trc ) {
  iOCTI __CTI = allocMem( sizeof( struct OCTI ) );
  iOCTIData data = allocMem( sizeof( struct OCTIData ) );
  MemOp.basecpy( __CTI, &CTIOp, 0, sizeof( struct OCTI ), data );

  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */

  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CTI %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = %s", wDigInt.getdevice( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->run = True;

  data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );
  SerialOp.setFlow( data->serial, none );
  SerialOp.setLine( data->serial, 9600, 8, 1, 0, wDigInt.isrtsdisabled( ini ) );
  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {
    data->reader = ThreadOp.inst( "ctireader", &__reader, __CTI );
    ThreadOp.start( data->reader );
    data->writer = ThreadOp.inst( "ctiwriter", &__writer, __CTI );
    ThreadOp.start( data->writer );
  }
  else
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init CTI port!" );

  instCnt++;
  return __CTI;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/cti.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
