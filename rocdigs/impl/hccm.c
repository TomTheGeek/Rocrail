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

#include "rocdigs/impl/hccm_impl.h"

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
#include "rocutils/public/addr.h"


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOHCCMData data = Data(inst);
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

/** ----- OHCCM ----- */


static iONode __translate(obj inst , iONode node) {
  iOHCCMData data = Data(inst);
  iONode rsp = NULL;

  /* Loc command. */
  if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node ); /* loc decoder address */
    int  block = wLoc.getport( node ); /* block number*/
    int    dir = wLoc.isdir( node );
    int  Vstep = wLoc.getV_step( node );
    int  speed = 0;
    Boolean analog = StrOp.equals( wLoc.prot_A, wLoc.getprot( node ) );
    int  range = analog ? 16:28;

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = ( wLoc.getV( node ) * range) / 100;
      else
        speed = (wLoc.getV( node ) * range) / wLoc.getV_max( node );
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "loco %s (%s %d) speed=%d dir=%s trackport=%d",
        wLoc.getid(node), analog?"analog":"DCC", addr, speed, dir?"fwd":"rev", block );

    if( analog ) {
      byte* cmd = allocMem(32);
      cmd[0] = 2; /* packet size */
      cmd[1] = block;
      cmd[2] = speed;
      ThreadOp.post( data->transactor, (obj)cmd );
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    byte* cmd = allocMem(32);
    int addr = wSwitch.getaddr1( node );
    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );

    cmd[0] = 2;
    cmd[1] = addr;
    cmd[2] = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 0x01:0x00;
    ThreadOp.post( data->transactor, (obj)cmd );
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    byte* cmd = allocMem(32);
    int addr = wOutput.getaddr( node );

    cmd[0] = 2;
    cmd[1] = addr;
    cmd[2] = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;
    ThreadOp.post( data->transactor, (obj)cmd );
  }

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOHCCMData data = Data(inst);
  iONode rsp = __translate(inst, cmd);
  NodeOp.base.del(cmd);
  return rsp;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOHCCMData data = Data(inst);
  data->run = False;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOHCCMData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
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
static int patch  = 99;
static int _version( obj inst ) {
  return vmajor*10000 + vminor*100 + patch;
}


static void __evaluatePacket(iOHCCM hccm, byte* in) {
  iOHCCMData data = Data(hccm);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "evaluate 0x%02X 0x%02X", in[0], in[1] );

  /* example to generate a sensor event */
  if( in[0] == 0xFF ) {
    int addr = in[1];
    iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
    wFeedback.setaddr( nodeC, addr );
    wFeedback.setstate( nodeC, (in[0] & 0x01 ) ? True:False );
    if( data->iid != NULL )
      wFeedback.setiid( nodeC, data->iid );
    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }

}


static void __transactor( void* threadinst ) {
  iOThread     th = (iOThread)threadinst;
  iOHCCM     hccm = (iOHCCM)ThreadOp.getParm(th);
  iOHCCMData data = Data(hccm);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "transactor started" );

  while( data->run && !data->serialOK) {
    data->serialOK = SerialOp.open( data->serial );
    ThreadOp.sleep(1000);
  }

  while( data->run ) {
    byte in[32];

    byte* post = (byte*)ThreadOp.getPost( th );
    if( post != NULL ) {
      SerialOp.write(data->serial, (char*)(post+1), post[0]);
      freeMem(post);
    }

    if( SerialOp.available(data->serial) ) {
      if( SerialOp.read(data->serial, (char*)in, 2) ) {
        __evaluatePacket(hccm, in);
      }
    }

    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "transactor stopped" );
}


/**  */
static struct OHCCM* _inst( const iONode ini ,const iOTrace trc ) {
  iOHCCM __HCCM = allocMem( sizeof( struct OHCCM ) );
  iOHCCMData data = allocMem( sizeof( struct OHCCMData ) );
  MemOp.basecpy( __HCCM, &HCCMOp, 0, sizeof( struct OHCCM ), data );

  SystemOp.inst();
  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini     = ini;
  data->iid     = StrOp.dup( wDigInt.getiid( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "hccm %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  IDD %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  device   [%s]", wDigInt.getdevice( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  timeout  [%d]", wDigInt.gettimeout( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  baudrate [%d]", wDigInt.getbps( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );

  SerialOp.setFlow( data->serial, cts );
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, none, wDigInt.isrtsdisabled( ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );
  SerialOp.setDTR(data->serial, True);

  data->run = True;
  data->transactor = ThreadOp.inst( "transactor", &__transactor, __HCCM );
  ThreadOp.start( data->transactor );

  instCnt++;
  return __HCCM;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/hccm.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
