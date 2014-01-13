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



#include "rocdigs/impl/xnmm_impl.h"

#include "rocs/public/str.h"
#include "rocs/public/mem.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOXNMMData data = Data(inst);
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

/** ----- OXNMM ----- */


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


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOXNMMData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOXNMM xnmm = (iOXNMM)ThreadOp.getParm( th );
  iOXNMMData data = Data(xnmm);

  byte out[256] = {0xFF, 0xFE};
  byte* pout = out;
  int len = 0;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "XNMM writer started." );
  ThreadOp.sleep( 100 );

  do {
    ThreadOp.sleep( data->commOK ? 1000:5000 );
    if( data->commOK && MutexOp.wait( data->mux ) ) {
      out[2+0] = 0x21;
      out[2+1] = 0x21;
      out[2+2] = 0x00;
      if( data->useHeader ) {
        pout = out;
        len = 5;
      }
      else {
        pout = out + 2;
        len = 3;
      }
      /*
      if( SerialOp.write(data->serial, pout, len) ) {
        TraceOp.dump( NULL, TRCLEVEL_INFO, (char*)pout, len );
      }
      */
      /* Release the mutex. */
      MutexOp.post( data->mux );
    }

  } while(data->run);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "XNMM writer ended." );
}


static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOXNMM xnmm = (iOXNMM)ThreadOp.getParm( th );
  iOXNMMData data = Data(xnmm);

  byte in[256] = {0};
  int idx = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "XNMM reader started." );
  ThreadOp.sleep( 100 );

  do {
    ThreadOp.sleep( data->commOK ? 10:5000 );

    if( data->commOK && MutexOp.wait( data->mux ) ) {
      int available = SerialOp.available(data->serial);
      if( available > 0 ) {
        if( SerialOp.read(data->serial, (char*)in, available) ) {
          if( available > 2 && in[0] == 0xFF && in[1] == 0xFE ) {
            data->useHeader = True;
          }
          TraceOp.dump( NULL, TRCLEVEL_INFO, (char*)in, available );
        }
      }
      else if( available == -1 || SerialOp.getRc(data->serial) > 0 ) {
        /* device error */
        data->commOK = False;
        SerialOp.close(data->serial);
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error" );
      }
      /* Release the mutex. */
      MutexOp.post( data->mux );
    }
    else if(!data->commOK) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "trying to open %d", data->device );
      data->commOK = SerialOp.open( data->serial );
    }


  } while(data->run);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "XNMM reader ended." );
}

/**  */
static struct OXNMM* _inst( const iONode ini ,const iOTrace trc ) {
  iOXNMM __XNMM = allocMem( sizeof( struct OXNMM ) );
  iOXNMMData data = allocMem( sizeof( struct OXNMMData ) );
  MemOp.basecpy( __XNMM, &XNMMOp, 0, sizeof( struct OXNMM ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->mux     = MutexOp.inst( NULL, True );

  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->device = StrOp.dup( wDigInt.getdevice( ini ) );
  data->run    = True;

  SystemOp.inst();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "xnmm %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     = [%s]", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = [%s]", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, 0 );
  SerialOp.setLine( data->serial, wDigInt.getbps(ini), 8, 1, 0, wDigInt.isrtsdisabled( ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );

  data->commOK = SerialOp.open( data->serial );
  if( !data->commOK ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device = [%s] not ready", data->device );
  }

  data->reader = ThreadOp.inst( "xnmmreader", &__reader, __XNMM );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( "xnmmwriter", &__writer, __XNMM );
  ThreadOp.start( data->writer );

  instCnt++;
  return __XNMM;
}



iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/xnmm.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
