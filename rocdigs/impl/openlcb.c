/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */

#include "rocdigs/impl/openlcb_impl.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/OpenDCC.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/BinCmd.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Clock.h"

#include "rocs/public/mem.h"
#include "rocs/public/lib.h"
#include "rocs/public/system.h"

#include "rocutils/public/addr.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOOpenLCBData data = Data(inst);
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

/** ----- OOpenLCB ----- */


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  return 0;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  return 0;
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


/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOOpenLCBData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**  */
static struct OOpenLCB* _inst( const iONode ini ,const iOTrace trc ) {
  iOOpenLCB __OpenLCB = allocMem( sizeof( struct OOpenLCB ) );
  iOOpenLCBData data = allocMem( sizeof( struct OOpenLCBData ) );
  MemOp.basecpy( __OpenLCB, &OpenLCBOp, 0, sizeof( struct OOpenLCB ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );

  /* clone the ini node for further use and reference */
  data->ini = (iONode)NodeOp.base.clone(ini);
  data->iid = wDigInt.getiid( data->ini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "openlcb %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "http://www.openlcb.org/" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid    = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );


  instCnt++;
  return __OpenLCB;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/openlcb.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
