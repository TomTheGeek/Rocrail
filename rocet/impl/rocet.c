/** ------------------------------------------------------------
  * A U T O   G E N E R A T E D  (First time only!)
  * Generator: Rocs ogen (build Feb 28 2011 16:23:43)
  * Module: Rocet
  * XML: 
  * XML: 
  * Object: Rocet
  * Date: Tue Mar  1 08:34:59 2011
  * ------------------------------------------------------------
  * $Source$
  * $Author$
  * $Date$
  * $Revision$
  * $Name$
  */

#include "rocet/impl/rocet_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iORocetData data = Data(inst);
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

/** ----- ORocet ----- */


/**  */
static int _Main( struct ORocet* inst ,int argc ,char** argv ) {
  iOTrace trc = TraceOp.inst( TRCLEVEL_WARNING | TRCLEVEL_INFO, "rocet", True );
  TraceOp.setAppID( trc, "t" );
  TraceOp.println( "--------------------------------------------------" );
  TraceOp.println( " Rocet: Rocrail Easy Throttle" );
  TraceOp.println( " Gateway between Ethernet and Digital Interface" );
  TraceOp.println( "--------------------------------------------------" );
  return 0;
}

int main(int argc ,char** argv) {
  iORocet rocet = RocetOp.inst();
  return RocetOp.Main( rocet, argc, argv );
}


/**  */
static struct ORocet* _inst( void ) {
  iORocet __Rocet = allocMem( sizeof( struct ORocet ) );
  iORocetData data = allocMem( sizeof( struct ORocetData ) );
  MemOp.basecpy( __Rocet, &RocetOp, 0, sizeof( struct ORocet ), data );

  /* Initialize data->xxx members... */

  instCnt++;
  return __Rocet;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocet/impl/rocet.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
