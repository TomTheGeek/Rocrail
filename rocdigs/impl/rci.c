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


#include "rocdigs/impl/rci_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iORCIData data = Data(inst);
    /* Cleanup data->xxx members...*/
    data->run = False;

    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  return;
}

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
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

/** ----- ORCI ----- */
static Boolean __transact( iORCIData o, byte* out, int outsize, byte* in, int insize ) {
  Boolean rc = False;
  if( MutexOp.wait( o->mux ) ) {
    rc = True;

    TraceOp.dump( NULL, TRCLEVEL_DEBUG, (char*)out, outsize );
    /* Transact */
    rc = SerialOp.write( o->serial, (char*)out, outsize );
    if( rc && insize > 0 ) {
      rc = SerialOp.read( o->serial, (char*)in, insize );
      TraceOp.dump( NULL, TRCLEVEL_DEBUG, (char*)in, insize );
    }

    /* Release the mutex. */
    MutexOp.post( o->mux );


    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_transact outsize=%d insize=%d",
                   outsize, insize );
  }
  return rc;
}


static int __translate( iORCIData o, iONode node, unsigned char* cmd, int* insize ) {
  *insize = 0;

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      cmd[0] = 0; /* broadcast */
      cmd[1] = 2; /* stop card */
      cmd[2] = 0;
      cmd[3] = 0;
      cmd[4] = cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3];
      *insize = 0;
      return 5;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      cmd[0] = 0; /* broadcast */
      cmd[1] = 3; /* start card */
      cmd[2] = 0;
      cmd[3] = 0;
      cmd[4] = cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3];
      *insize = 0;
      return 5;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.analog ) ) {
      cmd[0] = wSysCmd.getaddr( node );
      cmd[1] = 71; /* mode */
      cmd[2] = wSysCmd.getport( node );
      cmd[3] = 0x00; /* analog mode */
      cmd[4] = cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3];
      *insize = 1;
      return 5;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.dcc ) ) {
      cmd[0] = wSysCmd.getaddr( node );
      cmd[1] = 71; /* mode */
      cmd[2] = wSysCmd.getport( node );
      cmd[3] = 0x01; /* dcc mode */
      cmd[4] = cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3];
      *insize = 1;
      return 5;
    }
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node ); /* from the block properties: */
    int   port = wLoc.getport( node );
    int    dir = wLoc.isdir( node );
    int  speed = 0;

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = ( wLoc.getV( node ) * 95) / 100;
      else
        speed = (wLoc.getV( node ) * 95) / wLoc.getV_max( node );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "rci speed=%d", speed );
    }

    cmd[0] = addr;
    cmd[1] = 65; /* set target speed */
    cmd[2] = port & 0x07;
    cmd[3] = dir ? 0x80:0x00 | speed;
    cmd[4] = cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3];
    *insize = 1;
    return 5;
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int   addr = wSwitch.getaddr1( node );
    int   port = wSwitch.getport1( node );
    cmd[0] = addr;
    cmd[1] = 27; /* flip-flop puls */
    cmd[2] = port;
    cmd[3] = 25; /* puls duration = 25 * 0.01 = 250 ms */
    cmd[4] = cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3];
    *insize = 1;
    return 5;
  }

  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
  }

  return 0;
}


/**  */
static iONode _cmd( obj inst ,const iONode nodeA ) {
  iORCIData o = Data(inst);
  unsigned char out[256];
  unsigned char in [512];
  int insize = 0;
  iONode nodeB = NULL;

  if( nodeA != NULL ) {
    int size = __translate( o, nodeA, out, &insize );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, size );
     __transact( o, out, size, in, insize );

    /* Cleanup Node1 */
    nodeA->base.del(nodeA);
  }
  /* return Node2 */
  return nodeB;
}


/**  */
static void _halt( obj inst ) {
  iORCIData data = Data(inst);
  byte cmd[5];
  cmd[0] = 0; /* broadcast */
  cmd[1] = 2; /* stop card */
  cmd[2] = 0;
  cmd[3] = 0;
  cmd[4] = cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3];
  __transact( data, cmd, 5, NULL, 0 );
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iORCIData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _supportPT( obj inst ) {
  return False;
}


static void __evaluateFeedback( iORCI rci, int addr, byte fb, byte* fbstate ) {
  iORCIData o = Data(rci);
  int pin = 0;
  int state = 0;

  if( fb != fbstate[addr] ) {
    int n = 0;
    for( n = 0; n < 8; n++ ) {
      if( (fb & (0x01 << n)) != (fbstate[addr] & (0x01 << n)) ) {
        pin   = n;
        state = (fb & (0x01 << n)) ? 1:0;

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "fb %d,%d = %d", addr, pin, state );
        {
          /* inform listener: Node3 */
          iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
          wFeedback.setaddr( nodeC, addr*8 + pin );
          wFeedback.setstate( nodeC, state?True:False );
          if( o->iid != NULL )
            wFeedback.setiid( nodeC, o->iid );

          if( o->listenerFun != NULL && o->listenerObj != NULL )
            o->listenerFun( o->listenerObj, nodeC, TRCLEVEL_INFO );
        }
      }
    }
  }
}


static void __feedbackReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iORCI rci = (iORCI)ThreadOp.getParm( th );
  iORCIData o = Data(rci);
  unsigned char* fb = allocMem(256);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback reader started." );
  do {
    /*iOStrTok tok = StrTokOp.inst( wDigInt.getio24( o->ini ), ',' );*/
    iOStrTok tok = StrTokOp.inst( "", ',' );

    while ( StrTokOp.hasMoreTokens( tok ) ) {
      int addr = atoi( StrTokOp.nextToken( tok ) );
      byte reply[5];
      byte cmd[5];
      cmd[0] = addr;
      cmd[1] = 48; /* read bytes */
      cmd[2] = 0;
      cmd[3] = 0;
      cmd[4] = cmd[0] ^ cmd[1] ^ cmd[2] ^ cmd[3];
      __transact( o, cmd, 5, reply, 5 );
      __evaluateFeedback( rci, addr, reply[2], fb );
    };

    ThreadOp.sleep( 100 );


  } while( o->run );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback reader ended." );
}


/* Status */
static int _state( obj inst ) {
  iORCIData data = Data(inst);
  int state = 0;
  return state;
}

/* VERSION: */
static int vmajor = 0;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iORCIData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

/**  */
static struct ORCI* _inst( const iONode ini ,const iOTrace trc ) {
  iORCI __RCI = allocMem( sizeof( struct ORCI ) );
  iORCIData data = allocMem( sizeof( struct ORCIData ) );
  MemOp.basecpy( __RCI, &RCIOp, 0, sizeof( struct ORCI ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini = ini;
  data->iid = StrOp.dup( wDigInt.getiid( ini ) );
  data->mux = MutexOp.inst( StrOp.fmt( "serialMux%08X", data ), True );

  data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );

  SerialOp.setFlow( data->serial, -1 );
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, none );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );

  /* DTR and RTS should be set high for RS485 power: */
  SerialOp.setRTS( data->serial, True );
  SerialOp.setDTR( data->serial, True );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rci %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  if( !SerialOp.open( data->serial ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init RCI port!" );
  }
  else {
    data->run = True;
    /*if( StrOp.len( wDigInt.getio24( ini ) ) > 0  ) {*/
    if( StrOp.len( "" ) > 0  ) {
      data->feedbackReader = ThreadOp.inst( "feedbackReader", &__feedbackReader, __RCI );
      ThreadOp.start( data->feedbackReader );
    }
  }

  instCnt++;
  return __RCI;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/rci.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
