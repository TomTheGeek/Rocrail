/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocdigs/impl/inter10_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"
#include "rocs/public/map.h"


#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/CustomCmd.h"

static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOInter10Data data = Data(inst);
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

/** ----- OInter10 ----- */


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOInter10Data data = Data(inst);
  /* re-init command? 0x25 */
  /* stop command? 0x88 */
  return NULL;
}


/**  */
static void _halt( obj inst , Boolean poweroff) {
  iOInter10Data data = Data(inst);
  data->run = False;
  SerialOp.close( data->serial );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOInter10Data data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _supportPT( obj inst ) {
  iOInter10Data data = Data(inst);
  return False;
}

static Boolean __flushPort( iOInter10 inst ) {
  iOInter10 pInter10 = inst;
  iOInter10Data data = Data(pInter10);

  /* Read all pending information on serial port. Interface Hickups if data is pending from previous init! */
  {
    int bAvail = SerialOp.available(data->serial);
    if (bAvail > 0) {
      char c;
      int extra = 0;

      TraceOp.trc(name, TRCLEVEL_WARNING, __LINE__, 9999, "Tossing %d bytes to wastebasket...", bAvail);

      while( SerialOp.available(data->serial) > 0 ) {
        SerialOp.read( data->serial, &c, 1 );
      }

      do {
        ThreadOp.sleep(50);
        bAvail = SerialOp.available(data->serial);
        if( bAvail > 0 ) {
          SerialOp.read( data->serial, &c, 1 );
          extra++;
        }
      } while( bAvail > 0 );
      if( extra > 0 )
        TraceOp.trc(name, TRCLEVEL_WARNING, __LINE__, 9999, "More bytes flushed: %d", extra);

      return True;
    }

  }
  return False;
}


static void __RFIReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOInter10 pInter10 = (iOInter10)ThreadOp.getParm( th );
  iOInter10Data data = Data(pInter10);
  Boolean ok = False;

  /* IO buffer */
  byte buffer[64];
  /* already sended RIF's */
  iOMap map = MapOp.inst();

  data->initOK = False;

  while( data->run ) {
    ThreadOp.sleep( 10 );

    if( !data->initOK ) {
      buffer[0] = 0x25; /* Interface Start */
      data->initOK = SerialOp.write(data->serial, (char*)buffer, 1 );
      __flushPort(pInter10);
      if( !data->initOK ) {
        ThreadOp.sleep( 1000 );
        continue;
      }
    }


    if( SerialOp.available(data->serial) )
      ok = SerialOp.read( data->serial, (char*)buffer, 2 );
    else
      continue;

    if( ok && buffer[1] == 0x70 ) {
      /* reader is occupied and will send this message every cycle until no longer occupied */
      ok = SerialOp.read( data->serial, (char*)buffer+2, 1 );
      if( ok ) {
        /* */
        int reader = buffer[0];
        int ident  = buffer[2];
        /* create a key: */
        char* key = StrOp.fmt( "%d_%d", reader, ident );

        /* check if key is already in map: every read cycle is the occupancy send */
        if( MapOp.get( map, key ) == NULL ) {
          iONode evt = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
          MapOp.put( map, key, (obj)"RFI" );

          wFeedback.setstate( evt, True );
          wFeedback.setaddr( evt, reader );
          wFeedback.setbus( evt, 5 );
          wFeedback.setidentifier( evt, ident );
          if( data->iid != NULL )
            wFeedback.setiid( evt, data->iid );

          data->listenerFun( data->listenerObj, evt, TRCLEVEL_INFO );
        }
        else {
          /* ignore event; already sended */
        }
        StrOp.free( key);
      }
    }
    else if( ok && buffer[1] == 0x80 ) {
      /* reader is no longer occupied */
      ok = SerialOp.read( data->serial, (char*)buffer+2, 1 );
      if( ok ) {
        /* */
        int reader = buffer[0];
        int ident  = buffer[2];
        iONode evt = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
        char* key = StrOp.fmt( "%d_%d", reader, ident );
        /* check if key is already in map */
        if( MapOp.get( map, key ) != NULL ) {
          MapOp.remove( map, key );
        }

        wFeedback.setstate( evt, False );
        wFeedback.setaddr( evt, reader );
        wFeedback.setbus( evt, 5 );
        wFeedback.setidentifier( evt, 0 );
        if( data->iid != NULL )
          wFeedback.setiid( evt, data->iid );

        data->listenerFun( data->listenerObj, evt, TRCLEVEL_INFO );

        StrOp.free( key);
      }
    }

  }
}


/* Status */
static int _state( obj inst ) {
  iOInter10Data data = Data(inst);
  int state = 0;
  return state;
}


/* external shortcut event */
static void _shortcut(obj inst) {
  iOInter10Data data = Data( inst );
}


/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;
static int _version( obj inst ) {
  iOInter10Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

/**  */
static struct OInter10* _inst( const iONode ini ,const iOTrace trc ) {
  iOInter10 __Inter10 = allocMem( sizeof( struct OInter10 ) );
  iOInter10Data data = allocMem( sizeof( struct OInter10Data ) );
  MemOp.basecpy( __Inter10, &Inter10Op, 0, sizeof( struct OInter10 ), data );

  /* Initialize data->xxx members... */
  data->device   = StrOp.dup( wDigInt.getdevice( ini ) );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  data->bps      = wDigInt.getbps( ini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "inter10 %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "mode: event started by PC (J2 + J3 open)" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "inter10[%s]: %s,%d",
    wDigInt.getiid( ini ) != NULL ? wDigInt.getiid( ini ):"", data->device, data->bps);

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, -1 );
  SerialOp.setLine( data->serial, data->bps, 8, 1, none, wDigInt.isrtsdisabled( ini ) );
  /*SerialOp.setTimeout( data->serial, data->timeout, 0 );*/
  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {
    data->run = True;

    SystemOp.inst();
    data->reader = ThreadOp.inst( "inter10", &__RFIReader, __Inter10 );
    ThreadOp.start( data->reader );
  }
  else
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init inter10 port!" );


  instCnt++;
  return __Inter10;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/inter10.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
