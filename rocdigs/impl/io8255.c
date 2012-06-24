/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 */

#include "rocdigs/impl/io8255_impl.h"

#include "rocs/public/mem.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOio8255Data data = Data(inst);
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

/** ----- Oio8255 ----- */


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst, Boolean poweroff ) {
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  return 0;
}


static Boolean _setRawListener(obj inst, obj listenerObj, const digint_rawlistener listenerFun ) {
  return True;
}

static byte* _cmdRaw( obj inst, const byte* cmd ) {
  return NULL;
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
static int _version( obj inst ) {
  return 0;
}


/**  */
static struct Oio8255* _inst( const iONode ini ,const iOTrace trc ) {
  iOio8255 __io8255 = allocMem( sizeof( struct Oio8255 ) );
  iOio8255Data data = allocMem( sizeof( struct Oio8255Data ) );
  MemOp.basecpy( __io8255, &io8255Op, 0, sizeof( struct Oio8255 ), data );

  TraceOp.set( trc );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "initializing io8255 library..." );

  /* Initialize data->xxx members... */

  instCnt++;
  return __io8255;
}

/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/io8255.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
