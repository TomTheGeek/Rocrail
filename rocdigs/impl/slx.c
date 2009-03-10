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

#include "rocdigs/impl/slx_impl.h"

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
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/FbInfo.h"
#include "rocrail/wrapper/public/FbMods.h"


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
    iOSLXData data = Data(inst);
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

/** ----- OSLX ----- */

static Boolean __setActiveBus( iOSLX slx, int bus ) {
  iOSLXData data = Data(slx);

  if( bus < 32 && data->activebus != bus ) {
    byte cmd[2];
    cmd[0] = 126;
    cmd[1] = bus;
    cmd[0] |= WRITE_FLAG;

    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)cmd, 2 );
    if( data->dummyio )
      return True;
    else
      return SerialOp.write( data->serial, (char*)cmd, 2 );
  }
  return True;
}


static Boolean __transact( iOSLX slx, byte* out, int outsize, byte* in, int insize, int bus ) {
  iOSLXData data = Data(slx);
  Boolean     ok = False;

  if( MutexOp.wait( data->mux ) ) {
    ok = __setActiveBus( slx, bus );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, outsize );
    if( data->dummyio )
      ok = True;
    else
      ok = SerialOp.write( data->serial, (char*)out, outsize );
    if( ok && insize > 0 ) {
      if( !data->dummyio )
        ok = SerialOp.read( data->serial, (char*)in, insize );
    }
    /* Release the mutex. */
    MutexOp.post( data->mux );
  }

  return ok;
}


/* fbmods is a comman separated address list of connected feedback modules. */
static void __updateFB( iOSLX slx, iONode fbInfo ) {
  iOSLXData data = Data(slx);
  int cnt = NodeOp.getChildCnt( fbInfo );
  int i = 0;

  char* str = NodeOp.base.toString( fbInfo );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "updateFB\n%s", str );
  StrOp.free( str );

  /* reset the list: */
  MemOp.set( data->fbmodcnt, 0, 32 * sizeof(int) );
  MemOp.set( data->fbmods, 0, 32*256 );

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


/**
 */
static int __translate( iOSLX slx, iONode node, byte* cmd, int* bus ) {
  iOSLXData data = Data(slx);
  *bus = 0;

  if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
    __updateFB( slx, node );
  }
  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    byte pin = 0x01 << ( wSwitch.getport1( node ) - 1 );
    byte mask = ~pin;
    *bus = wSwitch.getbus( node ) & 0x1F;
    cmd[0] = wSwitch.getaddr1( node );
    cmd[1] = 0x01 << ( wSwitch.getport1( node ) - 1 );
    cmd[0] |= WRITE_FLAG;

    /* reset pin to 0: */
    cmd[1] = data->swstate[*bus][cmd[0]] & mask;

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      cmd[1] |= pin;
    /* save new state: */
    data->swstate[*bus][cmd[0]] = cmd[1];
    return 2;
  }
  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
    return 0;
  }
  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    Boolean fn  = wLoc.isfn( node );
    Boolean dir = wLoc.isdir( node ); /* True == forwards */
    *bus  = wLoc.getbus( node ) & 0x1F;

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * 31) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * 31) / wLoc.getV_max( node );
    }

    cmd[0] = addr;
    cmd[0] |= WRITE_FLAG;
    cmd[1] = speed & 0x1F;
    cmd[1] |= dir ? 0x00:0x20;
    cmd[1] |= fn  ? 0x00:0x40;
    cmd[1] |= data->lcstate[*bus][addr] & 0x80;

    data->lcstate[*bus][addr] = cmd[1];

    return 2;
  }
  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );
    Boolean f1 = wFunCmd.isf1( node );
    Boolean f2 = wFunCmd.isf2( node );
    Boolean f3 = wFunCmd.isf3( node );
    Boolean f4 = wFunCmd.isf4( node );
    *bus  = wFunCmd.getbus( node ) & 0x1F;

    cmd[0] = addr;
    cmd[0] |= WRITE_FLAG;
    cmd[1] = data->lcstate[*bus][addr] & 0x7F;
    cmd[1] |= f1 ? 0x80:0x00;

    data->lcstate[*bus][addr] = cmd[1];

    return 2;
  }
  /* TODO: bus. System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      cmd[0] = 127;
      cmd[0] |= WRITE_FLAG;
      cmd[1] = 0x00;
      return 2;
    }
    if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      cmd[0] = 127;
      cmd[0] |= WRITE_FLAG;
      cmd[1] = 0x80;
      return 2;
    }
  }
  return 0;
}

static __evaluateFB( iOSLX slx, byte in, int addr, int bus ) {
  iOSLXData data = Data(slx);

  if( in != data->fbstate[bus][addr] ) {
    int n = 0;
    int pin = 0;
    int state = 0;
    for( n = 0; n < 8; n++ ) {
      if( (in & (0x01 << n)) != (data->fbstate[bus][addr] & (0x01 << n)) ) {
        pin = n;
        state = (in & (0x01 << n)) ? 1:0;
        TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)&in, 1 );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "fb %d = %d", addr*8+pin+1, state );
        {
          /* inform listener: Node3 */
          iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
          wFeedback.setaddr( nodeC, addr*8+pin+1 );
          wFeedback.setstate( nodeC, state?True:False );
          if( data->iid != NULL )
            wFeedback.setiid( nodeC, data->iid );

          data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
        }
      }
    }
    data->fbstate[bus][addr] = in;
  }
}


static void __feedbackReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOSLX slx = (iOSLX)ThreadOp.getParm( th );
  iOSLXData data = Data(slx);
  unsigned char* fb = allocMem(256);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback reader started." );

  /* TODO: bus support > 0 */

  while( data->run ) {
    int i, n = 0;

    ThreadOp.sleep( 100 );

    for( n=0; n < 32; n++ ) {
      if( data->fbmodcnt[n] == 0 )
        continue;

      for( i = 0; i < data->fbmodcnt[n]; i++ ) {
        byte cmd[2];
        byte  in[2];
        cmd[0] = data->fbmods[n][i] & 0x7F;
        cmd[1] = 0;
        if( __transact( slx, cmd, 2, in, 1, n) ) {
          __evaluateFB( slx, in[0], data->fbmods[n][i], n );
        }
      }
    }
  };
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback reader ended." );
}


/**  */
static iONode _cmd( obj inst ,const iONode nodeA ) {
  iOSLXData data = Data(inst);
  byte cmd[2];

  if( nodeA != NULL ) {
    int bus = 0;
    int outsize = __translate( (iOSLX)inst, nodeA, cmd, &bus );
    /*TraceOp.dump( NULL, TRCLEVEL_BYTE, out, size );*/
    if( outsize > 0 )
      __transact( (iOSLX)inst, cmd, outsize, NULL, 0, bus );

    /* Cleanup Node1 */
    nodeA->base.del(nodeA);
  }

  return NULL;
}


/**  */
static void _halt( obj inst ) {
  iOSLXData data = Data(inst);
  byte cmd[2];

  /* TODO: bus */
  data->run = False;
  cmd[0] = 127;
  cmd[1] = 0;
  __transact( (iOSLX)inst, cmd, 2, NULL, 0, 0 );
  SerialOp.close( data->serial );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOSLXData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _supportPT( obj inst ) {
  /* TODO: decoder programming support */
  return False;
}


/* Status */
static int _state( obj inst ) {
  iOSLXData data = Data(inst);
  int state = 0;
  return state;
}

/* VERSION: */
static int vmajor = 1;
static int vminor = 3;
static int patch  = 0;
static int _version( obj inst ) {
  iOSLXData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

/**  */
static struct OSLX* _inst( const iONode ini ,const iOTrace trc ) {
  iOSLX __SLX = allocMem( sizeof( struct OSLX ) );
  iOSLXData data = allocMem( sizeof( struct OSLXData ) );
  MemOp.basecpy( __SLX, &SLXOp, 0, sizeof( struct OSLX ), data );

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

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slx %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iOSLX[%s]: %s,%d,%d",
        wDigInt.getiid( ini ) != NULL ? wDigInt.getiid( ini ):"",
        data->device, data->bps, data->timeout );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, -1 );
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 2, none );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );

  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {

    data->feedbackReader = ThreadOp.inst( "feedbackReader", &__feedbackReader, __SLX );
    ThreadOp.start( data->feedbackReader );

    /* TODO: should the solenoid activation time be whatched and turned of? */
    /*
    data->swTimeWatcher = ThreadOp.inst( "swTimeWatcher", &__swTimeWatcher, __SLX );
    ThreadOp.start( data->swTimeWatcher );
    */
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init SLX port!" );
  }


  instCnt++;
  return __SLX;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/slx.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
