/** ------------------------------------------------------------
  * Module: RocRail
  * Object: Traverser
  */

#include "rocrail/impl/traverser_impl.h"

#include "rocs/public/mem.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOTraverserData data = Data(inst);
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

/** ----- OTraverser ----- */


/**  */
static char* _getForm( void* object ) {
  return 0;
}


/**  */
static char* _postForm( void* object ,const char* data ) {
  return 0;
}


/**  */
static const char* _tableHdr( void ) {
  return 0;
}


/**  */
static char* _toHtml( void* object ) {
  return 0;
}


/**  */
static Boolean _cmd( struct OTraverser* inst ,iONode cmd ,Boolean update ,int* error ) {
  return 0;
}


/**  */
static char* _createAddrKey( int bus ,int addr ,int port ,const char* iid ) {
  return 0;
}


/**  */
static const char* _getAddrKey( struct OTraverser* inst ) {
  return 0;
}


/**  */
static struct OTraverser* _inst( iONode ini ) {
  iOTraverser __Traverser = allocMem( sizeof( struct OTraverser ) );
  iOTraverserData data = allocMem( sizeof( struct OTraverserData ) );
  MemOp.basecpy( __Traverser, &TraverserOp, 0, sizeof( struct OTraverser ), data );

  /* Initialize data->xxx members... */

  instCnt++;
  return __Traverser;
}


/**  */
static Boolean _isLocked( struct OTraverser* inst ,const char* locid ) {
  return 0;
}


/** true if the sensor state matches the wanted state */
static Boolean _isSet( struct OTraverser* inst ) {
  return 0;
}


/**  */
static Boolean _isState( struct OTraverser* inst ,const char* state ) {
  return 0;
}


/**  */
static Boolean _lock( struct OTraverser* inst ,const char* locid ,iORoute route ) {
  return 0;
}


/**  */
static void _modify( struct OTraverser* inst ,iONode mod ) {
  return;
}


/**  */
static void _reset( struct OTraverser* inst ) {
  return;
}


/**  */
static Boolean _unLock( struct OTraverser* inst ,const char* locid ) {
  return 0;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/traverser.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
