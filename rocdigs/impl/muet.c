/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>
 http://www.rocrail.net

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



#include "rocdigs/impl/muet_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
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

#include "rocdigs/impl/muet/muet.h"


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOMuetData data = Data(inst);
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

/** ----- OMuet ----- */


static iOSlot __getSlot(iOMuetData data, iONode node) {
  int addr  = wLoc.getaddr(node);
  iOSlot slot = NULL;

  slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot exist for %s", wLoc.getid(node) );
    return slot;
  }

  slot = allocMem( sizeof( struct slot) );
  slot->addr = addr;
  slot->bus = wLoc.getbus(node);
  slot->id = StrOp.dup(wLoc.getid(node));
  if( MutexOp.wait( data->lcmux ) ) {
    MapOp.put( data->lcmap, wLoc.getid(node), (obj)slot);
    MutexOp.post(data->lcmux);
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for %s", wLoc.getid(node) );
  return slot;
}




/* fbmods is a comman separated address list of connected feedback modules. */
static void __updateFB( iOMuet muet, iONode fbInfo ) {
  iOMuetData data = Data(muet);
  int cnt = NodeOp.getChildCnt( fbInfo );
  int i = 0;

  char* str = NodeOp.base.toString( fbInfo );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "updateFB\n%s", str );
  StrOp.free( str );

  /* reset the list: */
  MemOp.set( data->fbmodcnt, 0, 2 * sizeof(int) );
  MemOp.set( data->fbmods, 0, 2*256 );

  for( i = 0; i < cnt; i++ ) {
    iONode fbmods = NodeOp.getChild( fbInfo, i );
    const char* mods = wFbMods.getmodules( fbmods );
    int bus = wFbMods.getbus( fbmods );
    if( mods != NULL && StrOp.len( mods ) > 0 ) {

      iOStrTok tok = StrTokOp.inst( mods, ',' );
      int idx = 0;
      while( StrTokOp.hasMoreTokens( tok ) ) {
        int addr = atoi( StrTokOp.nextToken(tok) );
        data->fbmods[bus][idx] = addr & 0x7f;
        idx++;
      };
      data->fbmodcnt[bus] = idx;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "updateFB bus=%d count=%d", bus, idx );
    }
  }

}





static Boolean __transact( iOMuet muet, byte* out, int outsize, byte* in, int insize, int bus ) {
  iOMuetData data = Data(muet);
  Boolean     ok = False;

  if( MutexOp.wait( data->mux ) ) {
    //ok = __setActiveBus( slx, bus );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, outsize );
    if( data->dummyio )
      ok = True;
    else
      ok = SerialOp.write( data->serial, (char*)out, outsize );
    if( ok && insize > 0 ) {
      if( !data->dummyio ) {
        ok = SerialOp.read( data->serial, (char*)in, insize );
        if(ok)
          TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, insize );
      }
    }
    /* Release the mutex. */
    MutexOp.post( data->mux );
  }

  return ok;
}


static int __translate( iOMuet muet, iONode node, byte* cmd, int* bus ) {
  iOMuetData data = Data(muet);
  *bus = 0;

  if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
    __updateFB( muet, node );
  }
  
  return 0;
}


/**  */
static iONode _cmd( obj inst ,const iONode nodeA ) {
  iOMuetData data = Data(inst);
  byte cmd[32];

  if( nodeA != NULL ) {
    int bus = 0;
    int outsize = __translate( (iOMuet)inst, nodeA, cmd, &bus );
    /*TraceOp.dump( NULL, TRCLEVEL_BYTE, out, size );*/
    if( outsize > 0 )
      __transact( (iOMuet)inst, cmd, outsize, NULL, 0, bus );

    /* Cleanup Node1 */
    nodeA->base.del(nodeA);
  }

  return NULL;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOMuetData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  data->run = False;
  SerialOp.close( data->serial );
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOMuetData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
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


static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOMuetData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMuet muet = (iOMuet)ThreadOp.getParm( th );
  iOMuetData data = Data(muet);
  unsigned char* fb = allocMem(256);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader started." );
  
  while( data->run ) {
    int i, n = 0;

    ThreadOp.sleep( 100 );
  }
  
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader ended." );
}


/**  */
static struct OMuet* _inst( const iONode ini ,const iOTrace trc ) {
  iOMuet __Muet = allocMem( sizeof( struct OMuet ) );
  iOMuetData data = allocMem( sizeof( struct OMuetData ) );
  MemOp.basecpy( __Muet, &MuetOp, 0, sizeof( struct OMuet ), data );

  TraceOp.set( trc );
  /* Initialize data->xxx members... */

  /* Evaluate attributes. */
  data->device   = StrOp.dup( wDigInt.getdevice( ini ) );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  data->bps      = wDigInt.getbps( ini );
  data->timeout  = wDigInt.gettimeout( ini );
  data->swtime   = wDigInt.getswtime( ini );
  data->dummyio  = wDigInt.isdummyio( ini );

  data->run      = True;

  data->serialOK = False;
  data->initOK   = False;

  data->activebus = 0;

  data->mux = MutexOp.inst( StrOp.fmt( "serialMux%08X", data ), True );
  data->lcmux   = MutexOp.inst( NULL, True );
  data->lcmap   = MapOp.inst();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "muet %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid[%s]: %s,%d,%d",
        wDigInt.getiid( ini ) != NULL ? wDigInt.getiid( ini ):"",
        data->device, data->bps, data->timeout );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, -1 );
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, none, wDigInt.isrtsdisabled( ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );

  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {

    data->feedbackReader = ThreadOp.inst( "muetreader", &__reader, __Muet );
    ThreadOp.start( data->feedbackReader );

  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init Muet port!" );
  }

  instCnt++;
  return __Muet;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/muet.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
