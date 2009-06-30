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


#include "rocdigs/impl/nce_impl.h"

#include "rocs/public/str.h"
#include "rocs/public/mem.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"

#include "rocdigs/impl/common/fada.h"


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iONCEData data = Data(inst);
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

/** ----- ONCE ----- */

static void __handleAIU( iONCEData data, int aiu, byte* rsp ) {
  int offset = aiu * 16;
  int sens = rsp[1] + (rsp[0] * 16);
  int mask = rsp[3] + (rsp[2] * 16);
  Boolean state = False;
  int i = 0;
  for( i = 0; i < 14; i++ ) {
    if( mask & (1 << i) ) {
    state = sens & (1 << i) ? True:False;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "fb %d = %d", offset+i, state );
      {
        /* inform listener: Node3 */
        iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
        wFeedback.setaddr( nodeC, offset + i );
        wFeedback.setstate( nodeC, state );
        if( data->iid != NULL )
          wFeedback.setiid( nodeC, data->iid );
        if( data->listenerFun != NULL && data->listenerObj != NULL )
          data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
      }
    }
  }
}


static Boolean __transact( iONCEData data, byte* out, int outsize, byte* in, int insize ) {
  Boolean rc = False;
  if( MutexOp.wait( data->mux ) ) {
    TraceOp.dump( NULL, TRCLEVEL_BYTE, out, outsize );
    if( rc = SerialOp.write( data->serial, out, outsize ) ) {
      if( insize > 0 ) {
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "insize=%d", insize);
        rc = SerialOp.read( data->serial, in, insize );
        if( rc )
          TraceOp.dump( NULL, TRCLEVEL_BYTE, in, insize );
      }
    }
    MutexOp.post( data->mux );
  }
  return rc;
}


static int __translate( iONCEData data, iONode node, byte* out, int *insize ) {
  *insize = 0;

  /* Switch command. */
  if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int mod = wSwitch.getaddr1( node );
    int pin = wSwitch.getport1( node );
    byte cmd = 3;
    int addr = 0;
    int gate = wSwitch.getgate1( node );

    if( pin == 0 )
      fromFADA( mod, &mod, &pin, &gate );
    else if( mod == 0 && pin > 0 )
      fromPADA( pin, &mod, &pin );

    addr = (mod-1) * 4 + pin;

    if( addr < 1 )
      return 0;

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ) {
      cmd = 4;
    }

    out[0] = 0xAD;
    out[1] = addr / 256;
    out[2] = addr & 0xFF;
    out[3] = cmd;
    out[4] = 0;
    
    *insize = 1; /* Return code from NCE. */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "turnout %d %s", addr, wSwitch.getcmd( node ) );
    return 4;
  }
  
  /* Loc command. 
  
  01 0-7f Reverse 28 speed command
  02 0-7f Forward 28 speed command
  03 0-7f Reverse 128 speed command
  04 0-7f Forward 128 speed command
  */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    byte cmd = 0;
    Boolean fn = wLoc.isfn( node );
    int    dir = wLoc.isdir( node );
    int  spcnt = wLoc.getspcnt( node );

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * 127) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * 127) / wLoc.getV_max( node );
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d speed=%d lights=%s dir=%s",
        addr, speed, fn?"on":"off", dir?"forwards":"reverse" );
        
    if( spcnt == 28 ) {
      cmd = dir ? 2 : 1;
    }
    else {
      cmd = dir ? 4 : 3;
    }

    out[0] = 0xA2;
    out[1] = addr / 256;
    out[2] = addr & 0xFF;
    out[3] = cmd;
    out[4] = speed;
    
    *insize = 1; /* Return code from NCE. */
    return 6;
  }
  
  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );
    Boolean f0 = wFunCmd.isf0( node );
    Boolean f1 = wFunCmd.isf1( node );
    Boolean f2 = wFunCmd.isf2( node );
    Boolean f3 = wFunCmd.isf3( node );
    Boolean f4 = wFunCmd.isf4( node );
    
    byte info = (f0?0x10:0) + (f1?0x01:0) + (f2?0x02:0) + (f3?0x04:0) + (f4?0x08:0);

    out[0] = 0xA2;
    out[1] = addr / 256;
    out[2] = addr & 0xFF;
    out[3] = 7;
    out[4] = info;
    *insize = 1; /* Return code from NCE. */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
            "loc %d f0=%s f1=%s f2=%s f3=%s f4=%s",
            addr, f0?"on":"off", f1?"on":"off", f2?"on":"off", f3?"on":"off", f4?"on":"off" );
    return 5;
  }
  
  
  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      out[0] = 0x89;
      *insize = 1; /* Return code from NCE. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      return 1;
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      out[0] = 0x8B;
      *insize = 1; /* Return code from NCE. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      return 1;
    }
  }  

  return 0;
}


/**  */
static iONode _cmd( obj inst ,const iONode nodeA ) {
  iONCEData data = Data(inst);
  unsigned char out[32];
  unsigned char in [32];
  int    insize    = 0;
  iONode nodeB     = NULL;

  MemOp.set( in, 0x00, sizeof( in ) );

  if( nodeA != NULL ) {
    int size = __translate( data, nodeA, out, &insize );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, out, size );
    if( __transact( data, (char*)out, size, (char*)in, insize ) ) {
    }
  }
  
  return nodeB;
}


/**  */
static void _halt( obj inst ) {
  iONCEData data = Data(inst);
  data->run = False;
  SerialOp.close( data->serial );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iONCEData data = Data(inst);
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


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 1;
static int vminor = 3;
static int patch  = 0;
static int _version( obj inst ) {
  iONCEData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __pollerThread( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iONCE nce = (iONCE)ThreadOp.getParm( th );
  iONCEData data = Data(nce);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "AIU poller started." );
  ThreadOp.sleep( 1000 );
  do {
    unsigned char out[32];
    unsigned char in [32];
    int i = 0;

    ThreadOp.sleep( 100 );
    
    for( i = 0; i < data->aiucnt; i++ ) {

      out[0] = 0x8A;
      out[1] = i;
      if( __transact( data, out, 2, in, 4 ) ) {
        /* inform listener */
        __handleAIU( data, i, in);
      }
      ThreadOp.sleep( 0 );
    }
    
  } while( data->run );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "AIU poller ended." );
}




/**  */
static struct ONCE* _inst( const iONode ini ,const iOTrace trc ) {
  iONCE __NCE = allocMem( sizeof( struct ONCE ) );
  iONCEData data = allocMem( sizeof( struct ONCEData ) );
  MemOp.basecpy( __NCE, &NCEOp, 0, sizeof( struct ONCE ), data );

  /* Initialize data->xxx members... */
  data->mux     = MutexOp.inst( NULL, True );

  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->device = StrOp.dup( wDigInt.getdevice( ini ) );
  data->aiucnt = wDigInt.getfbmod( ini );

  data->run    = True;

  SystemOp.inst();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "nce %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     = [%s]", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = [%s]", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps     = [%d]", wDigInt.getbps( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout = [%d]ms", wDigInt.gettimeout( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, none );
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, 0 );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );
  SerialOp.open( data->serial );
  
  data->poller = ThreadOp.inst( "poller", &__pollerThread, __NCE );
  ThreadOp.start( data->poller );


  instCnt++;
  return __NCE;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/nce.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
