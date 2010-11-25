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

  MapOp.clear(data->fbmap);

  for( i = 0; i < cnt; i++ ) {
    iONode fbmods = NodeOp.getChild( fbInfo, i );
    const char* mods = wFbMods.getmodules( fbmods );
    int bus = wFbMods.getbus( fbmods );
    if( mods != NULL && StrOp.len( mods ) > 0 ) {

      iOStrTok tok = StrTokOp.inst( mods, ',' );
      int idx = 0;
      while( StrTokOp.hasMoreTokens( tok ) ) {
        byte *cmd = NULL;
        char key[32] = {'\0'};
        int addr = atoi( StrTokOp.nextToken(tok) );
        data->fbmods[bus][idx] = addr & 0x7f;

        /* create key */
        StrOp.fmtb(key, "%d_%d", bus, addr );
        MapOp.put( data->fbmap, key, (obj)&data->fbmods[bus][idx]); /* dummy object */
        /* control register for ident */
        StrOp.fmtb(key, "%d_%d", bus, addr+1 );
        MapOp.put( data->identmap, key, (obj)&data->fbmods[bus][idx]); /* dummy object */

        /* activate monitoring for the unit occupation */
        cmd = allocMem(32);
        cmd[0] = bus;
        cmd[1] = 3;
        cmd[2] = MONITORING;
        cmd[3] = MONITORING_ADD;
        cmd[4] = addr & 0x7F;
        ThreadOp.post(data->writer, (obj)cmd);

        /* activate monitoring for the unit control register */
        cmd = allocMem(32);
        cmd[0] = bus;
        cmd[1] = 3;
        cmd[2] = MONITORING;
        cmd[3] = MONITORING_ADD;
        cmd[4] = (addr+1) & 0x7F;
        ThreadOp.post(data->writer, (obj)cmd);

        idx++;
      };
      data->fbmodcnt[bus] = idx;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "updateFB bus=%d count=%d", bus, idx );
    }
  }

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

    /* Cleanup Node1 */
    nodeA->base.del(nodeA);
  }

  return NULL;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOMuetData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );

  /* CS off */
  byte* cmd = allocMem(32);
  cmd[0] = 0;
  cmd[1] = 2;
  cmd[2] = CS_SET_STATUS;
  cmd[3] = CS_OFF;
  ThreadOp.post(data->writer, (obj)cmd);
  ThreadOp.sleep(500);
  data->run = False;
  ThreadOp.sleep(100);
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


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMuet muet = (iOMuet)ThreadOp.getParm( th );
  iOMuetData data = Data(muet);
  byte* cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer started." );

  /* monitoring off and query active bus */
  cmd = allocMem(32);
  cmd[0] = 0;
  cmd[1] = 3;
  cmd[2] = MONITORING;
  cmd[3] = MONITORING_OFF;
  cmd[4] = SX_GET_BUS;
  ThreadOp.post(th, (obj)cmd);

  /* CS on */
  cmd = allocMem(32);
  cmd[0] = 0;
  cmd[1] = 2;
  cmd[2] = CS_SET_STATUS;
  cmd[3] = CS_ON;
  ThreadOp.post(th, (obj)cmd);

  /* monitoring bus 0 */
  cmd = allocMem(32);
  cmd[0] = 0;
  cmd[1] = 3;
  cmd[2] = MONITORING;
  cmd[3] = MONITORING_ON;
  cmd[4] = SX_BUS0;
  ThreadOp.post(th, (obj)cmd);



  while( data->run ) {
    byte * post = NULL;
    int len = 0;
    int bus = 0;
    byte out[64] = {0};

    ThreadOp.sleep(10);
    post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      /* first byte is the message length */
      bus = post[0];
      len = post[1];
      MemOp.copy( out, post+2, len);
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
  iOMuet muet = (iOMuet)ThreadOp.getParm( th );
  iOMuetData data = Data(muet);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader started." );
  
  while( data->run ) {
    byte c = 0;
    if( SerialOp.available(data->serial) ) {
      if( SerialOp.read(data->serial, &c, 1) ) {
        TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)&c, 1 );
        if(c == 126 ) {
          if( SerialOp.read(data->serial, &c, 1) ) {
            data->activebus = c;
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "active bus=%d.", data->activebus );
          }
        }
        else if(c >= 128 && c <= 128 + 31 ) {
          byte tmp[4];
          if( SerialOp.read(data->serial, tmp, 2) ) {
            char key[32] = {'\0'};
            int bus = (c-128)&0x7F;
            int addr = tmp[0] & 0x7F;
            int val = tmp[1] & 0xFF;

            StrOp.fmtb(key, "%d_%d", bus, addr );
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                "monitor event: bus=%d, addr=%d val=%02X key=%s.", bus, addr, val, key );

            if( MapOp.haskey( data->identmap, key) ) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "key=%s is a ident sensor unit.", key );
              /* get the loco number */
              byte *cmd = allocMem(32);
              cmd[0] = bus;
              cmd[1] = 1;
              cmd[2] = addr+1;
              ThreadOp.post(data->writer, (obj)cmd);
            }

          }
        }
        else if(c < 126 ) {
          int addr = c & 0x7F;
          if( SerialOp.read(data->serial, &c, 1) ) {
            char key[32] = {'\0'};
            int val = c & 0x7F;
            StrOp.fmtb(key, "%d_%d", data->activebus, addr-1 );
            if( MapOp.haskey( data->identmap, key) ) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco address for unit %d is %d", addr, val );
            }
            else {
              StrOp.fmtb(key, "%d_%d", data->activebus, addr );
              if( MapOp.haskey( data->fbmap, key) ) {
                TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "occupation for unit %d is %02X", addr, val );
              }
            }
          }
        }
      }
    }
    else {
      ThreadOp.sleep(10);
    }
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
  data->fbmap   = MapOp.inst();
  data->identmap= MapOp.inst();

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

    data->reader = ThreadOp.inst( "muetreader", &__reader, __Muet );
    ThreadOp.start( data->reader );
    data->writer = ThreadOp.inst( "muetwriter", &__writer, __Muet );
    ThreadOp.start( data->writer );

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
