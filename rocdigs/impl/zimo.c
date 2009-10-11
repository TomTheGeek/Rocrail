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


#include "rocdigs/impl/zimo_impl.h"

#include "rocs/public/mem.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Program.h"

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
    iOZimoData data = Data(inst);
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

/** ----- OZimo ----- */

static Boolean __isCTS( iOZimoData o ) {
  int wait4cts = 0;
  while( wait4cts < o->ctsretry ) {
    if( SerialOp.isCTS( o->serial ) ) {
      return True;
    }
    ThreadOp.sleep( 10 );
    wait4cts++;
  };
  return False;
}


static iONode __transact( iOZimo zimo, char* out, int outsize, char* in, int insize ) {
  iOZimoData data = Data(zimo);
  Boolean rc = False;
  iONode rsp = NULL;

  if( MutexOp.wait( data->mux ) ) {

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                 "_transact out=%s outsize=%d insize=%d",
                 out, outsize, insize );

    /* Transact */
    if( rc = SerialOp.write( data->serial, out, outsize ) ) {
      if( insize > 0 && in != NULL ) {
        char c;
        int i = 0;
        do {
          rc = SerialOp.read( data->serial, &c, 1 );
          in[i] = c;
          in[i+1] = '\0';
          i++;
        } while( rc && c != '\r' );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_transact in=%s", in );

        if( rc ) {
          /* check for a Q response... */
          if( in[0] == 'Q' ) {
            int rc = 0;
            int cv = 0;
            int vl = 0;
            int offset = 1;

            if( in[1] == 'N' || in[1] == 'M' )
              offset++;

            sscanf(&in[offset], "%2X%2X%2X", &rc, &cv, &vl);

            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                 "Q: rc=%d cv=%d value=%d", rc, cv, vl );

            rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
            if( data->iid != NULL )
              wProgram.setiid( rsp, data->iid );
            wProgram.setcmd( rsp, wProgram.datarsp );
            wProgram.setcv( rsp, cv );
            wProgram.setvalue( rsp, vl );
          }
        }
      }
    }
    /* Release the mutex. */
    MutexOp.post( data->mux );

  }
  return rsp;
}


static int __translate( iOZimo zimo, iONode node, char* out, int* insize ) {
  iOZimoData data = Data(zimo);
  *insize = 0;

  /* Switch command. */
  if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int mod  = wSwitch.getaddr1( node );
    int pin  = wSwitch.getport1( node );
    int output = pin * 2;
    int d1 = 0;

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      output += 1;

    d1  = 0x08; /* on */
    d1 |= output; /* port */

    sprintf( out, "M%s%02X%02X\r", wSwitch.getprot( node ), mod, d1 );
    return StrOp.len(out);
  }

  /* TODO: Output command */

  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
    return 0;
  }

  /* Loc command. */
  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ||
           StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int spdcnt = wLoc.getspcnt( node );
    int  speed = 0;
    int     d1 = 0;
    int     d2 = 0;
    int     d3 = 0;

    /* databyte 2: f1-f8 */
    d2 |= (wFunCmd.isf1( node ) << 0);
    d2 |= (wFunCmd.isf2( node ) << 1);
    d2 |= (wFunCmd.isf3( node ) << 2);
    d2 |= (wFunCmd.isf4( node ) << 3);
    d2 |= (wFunCmd.isf5( node ) << 4);
    d2 |= (wFunCmd.isf6( node ) << 5);
    d2 |= (wFunCmd.isf7( node ) << 6);
    d2 |= (wFunCmd.isf8( node ) << 7);
    /* databyte 3: f9-f12 */
    d3 = 0;

    /* databyte 1 */
    d1  = wLoc.isdir(node)?0x00:0x20; /* direction bit */
    d1 |= wLoc.isfn (node)?0x10:0x00; /* lights bit    */
    switch( spdcnt ) {
      case 14:
        d1 |= 0x04;
        break;
      case 28:
        d1 |= 0x08;
        break;
      case 127:
      case 128:
        d1 |= 0x0C;
        break;
      default:
        d1 |= 0x04;
        break;
    }

    if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
      speed = (wLoc.getV( node ) * spdcnt) / 100;
    else if( wLoc.getV_max( node ) > 0 )
      speed = (wLoc.getV( node ) * spdcnt) / wLoc.getV_max( node );

    sprintf( out, "F%s%02X%02X%02X%02X\r", wLoc.getprot( node ), addr, speed, d1, d2 );
    return StrOp.len(out);
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      sprintf( out, "SA\r" );
      return StrOp.len(out);
    }
    if( StrOp.equals( cmd, wSysCmd.go ) ) {
      sprintf( out, "SE\r" );
      return StrOp.len(out);
    }
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    const char* Q = "Q112233\r";
    int cv  = wProgram.getcv( node );

    if( wProgram.getcmd( node ) == wProgram.get ) {
      sprintf( out, "Q%02X\r", cv );
      *insize = StrOp.len(Q);
      return StrOp.len(out);
    }
    else if( wProgram.getcmd( node ) == wProgram.set ) {
      int val = wProgram.getvalue( node );
      sprintf( out, "R%s%02X%02X\r", "N", cv, val );
      *insize = StrOp.len(Q);
      return StrOp.len(out);
    }

  }


  return 0;
}


static void __swTimeWatcher( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOZimo zimo = (iOZimo)ThreadOp.getParm( th );
  iOZimoData data = Data(zimo);

  do {
    int i = 0;
    ThreadOp.sleep( 10 );
    /* TODO: Switch deactivating! */
  } while( True );
}


static void __feedbackReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOZimo zimo = (iOZimo)ThreadOp.getParm( th );
  iOZimoData data = Data(zimo);
  unsigned char* fb = allocMem(256);
  char out[256];
  char in [512];

  do {
    ThreadOp.sleep( 200 );
    /* TODO: Feedback polling? */
  } while( data->run );
}


/**  */
static iONode _cmd( obj inst ,const iONode nodeA ) {
  iOZimoData data = Data(inst);
  char out[256] = {'\0'};
  char in[256] = {'\0'};
  iONode nodeB = NULL;

  if( nodeA != NULL ) {
    int insize = 0;
    int len = __translate( (iOZimo)inst, nodeA, out, &insize );
    if( len > 0 ) {
      /* if out len == 0 the command has no translation. */
      nodeB = __transact( (iOZimo)inst, out, len, in, insize );
    }

    /* Cleanup Node1 */
    nodeA->base.del(nodeA);
  }
  /* return Node2 */
  return nodeB;
}


/**  */
static void _halt( obj inst ) {
  char out[32];
  sprintf( out, "SA;\r" );
  __transact( (iOZimo)inst, out, StrOp.len(out), NULL, 0 );
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOZimoData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _supportPT( obj inst ) {
  return True;
}


/* Status */
static int _state( obj inst ) {
  iOZimoData data = Data(inst);
  int state = 0;
  return state;
}

/* external shortcut event */
static void _shortcut(obj inst) {
  iOZimoData data = Data( inst );
}


/* VERSION: */
static int vmajor = 1;
static int vminor = 4;
static int patch  = 0;
static int _version( obj inst ) {
  iOZimoData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

/**  */
static struct OZimo* _inst( const iONode ini ,const iOTrace trc ) {
  iOZimo __Zimo = allocMem( sizeof( struct OZimo ) );
  iOZimoData data = allocMem( sizeof( struct OZimoData ) );
  MemOp.basecpy( __Zimo, &ZimoOp, 0, sizeof( struct OZimo ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini      = ini;
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );
  data->mux      = MutexOp.inst( NULL, True );
  data->fbmod    = wDigInt.getfbmod( ini );
  data->readfb   = wDigInt.isreadfb( ini );
  data->ctsretry = wDigInt.getctsretry( ini );
  data->swtime   = wDigInt.getswtime( ini );

  data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );

  MemOp.set( data->swTime0, -1, sizeof( data->swTime0 ) );
  MemOp.set( data->swTime1, -1, sizeof( data->swTime1 ) );
  MemOp.set( data->fbState, 0, sizeof( data->fbState ) );

  SerialOp.setFlow( data->serial, -1 );
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, none );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "zimo %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  if( !SerialOp.open( data->serial ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init Zimo port!" );
  }
  else {
    data->run = True;
    if( data->readfb && data->fbmod > 0 ) {
      data->feedbackReader = ThreadOp.inst( "feedbackReader", &__feedbackReader, __Zimo );
      ThreadOp.start( data->feedbackReader );
    }
    data->swTimeWatcher = ThreadOp.inst( "swTimeWatcher", &__swTimeWatcher, __Zimo );
    ThreadOp.start( data->swTimeWatcher );
  }


  instCnt++;
  return __Zimo;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/zimo.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
