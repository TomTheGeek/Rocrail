/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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




#include "rocdigs/impl/easydcc_impl.h"
#include "rocdigs/impl/nmra/nmrapacket.h"

#include "rocs/public/mem.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOEasyDCCData data = Data(inst);
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

/** ----- OEasyDCC ----- */

static void __reportState(iOEasyDCCData data) {
  if( data->listenerFun != NULL && data->listenerObj != NULL ) {
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );

    if( data->iid != NULL )
      wState.setiid( node, data->iid );
    wState.setpower( node, data->power );

    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}



static Boolean __sendCommand(iOEasyDCCData data, char* cmd) {
  Boolean rc = False;
  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "send command: %s", cmd );
  if( !data->dummyio ) {
    rc = SerialOp.write( data->serial, cmd, StrOp.len(cmd) );
  }
  return rc;
}


static int __normalizeSteps(int insteps ) {
  /* SPEEDSTEPS: vaild: 14, 28, 128 */
  if( insteps < 20 )
    return 14;
  if( insteps > 100 )
    return 128;
  return 28;
}

static void __makeMessage(char* buffer, char* cmd, byte* retVal, int len) {
  char* s;
  int idx = 0;
  int i = 0;

  for( i = 0; i < StrOp.len(cmd); i++) {
    buffer[idx] = cmd[i];
    idx++;
  }

  TraceOp.dump( name, TRCLEVEL_BYTE, (char*)retVal, len );

  for( i = 0; i < len; i++ ) {
    buffer[idx] = ' ';
    idx++;
    s = StrOp.byteToStr( &retVal[i], 1);
    buffer[idx] = s[0];
    idx++;
    buffer[idx] = s[1];
    idx++;
    StrOp.free(s);
  }
  buffer[idx] = '\r';
}


static iONode __translate( iOEasyDCCData data, iONode node ) {
  iONode rsp = NULL;
  const char* cmd = wSysCmd.getcmd( node );
  char buffer[256];

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "translate command: %s:%s", NodeOp.getName(node), cmd );

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    if( StrOp.equals( cmd, wSysCmd.stop ) || StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      StrOp.fmtb( buffer, "K\r" );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Kill main track" );
      __sendCommand(data, buffer);
      data->power = False;
      __reportState(data);
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      StrOp.fmtb( buffer, "E\r" );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Enable main track" );
      __sendCommand(data, buffer);
      data->power = True;
      __reportState(data);
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int addr = wSwitch.getaddr1(node);
    int outputChannel = StrOp.equals( wSwitch.turnout, wSwitch.getcmd(node)) ? 1:0;
    int len = 0;
    byte retVal[32];
    len = accDecoderPkt2(retVal, addr, True, outputChannel);

    __makeMessage(buffer, "S 02", retVal, len);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "turnout %d %s", addr, wSwitch.getcmd(node) );
    __sendCommand(data, buffer);
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int addr = wOutput.getaddr(node);
    int outputChannel = wOutput.getgate(node);
    int len = 0;
    byte retVal[32];
    len = accDecoderPkt2(retVal, addr, StrOp.equals(wOutput.on, wOutput.getcmd(node)), outputChannel);

    __makeMessage(buffer, "S 02", retVal, len);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d %s", addr, wSwitch.getcmd(node) );
    __sendCommand(data, buffer);
  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int     steps = __normalizeSteps( wLoc.getspcnt(node) );
    int     addr  = wLoc.getaddr(node);
    Boolean fn    = wLoc.isfn( node );
    Boolean longAddr = StrOp.equals( wLoc.getprot( node ), wLoc.prot_L );
    int     dir   = wLoc.isdir( node );

    int len = 0;
    byte retVal[32];
    int  speed = 0;

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * steps) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * steps) / wLoc.getV_max( node );
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %d speed=%d lights=%s dir=%s",
        addr, speed, fn?"on":"off", dir?"forwards":"reverse" );

    if( steps == 128 )
      len = speedStep128Packet( retVal, addr, longAddr, speed, dir );
    else if( steps == 28 )
      len = speedStep28Packet( retVal, addr, longAddr, speed, dir );
    else
      len = speedStep14Packet( retVal, addr, longAddr, speed, dir, fn );

    __makeMessage(buffer, "Q", retVal, len);

    __sendCommand(data, buffer);

  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int addr  = wLoc.getaddr(node);
    Boolean longAddr = StrOp.equals( wLoc.getprot( node ), wLoc.prot_L );
    int fg = wFunCmd.getgroup(node);
    int len = 0;
    byte retVal[32];

    if( fg == 0 || fg == 1 )
      len = function0Through4Packet( retVal, addr, longAddr, wFunCmd.isf0(node), wFunCmd.isf1(node), wFunCmd.isf2(node), wFunCmd.isf3(node), wFunCmd.isf4(node) );

    __makeMessage(buffer, "S 01", retVal, len);

    __sendCommand(data, buffer);
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
  }


  return rsp;
}

/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOEasyDCCData data = Data(inst);
  char out[32];
  char in [32];
  iONode reply = NULL;

  if( cmd != NULL ) {
    reply = __translate( data, cmd );
  }

  /* Cleanup Node1 */
  cmd->base.del(cmd);

  return reply;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOEasyDCCData data = Data(inst);
  data->run = False;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "shutting down <%s>...", data->iid );
  ThreadOp.sleep(100);
  if( data->serial != NULL )
    SerialOp.close( data->serial );
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOEasyDCCData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "listener set" );
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
  iOEasyDCCData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOEasyDCC easydcc = (iOEasyDCC)ThreadOp.getParm( th );
  iOEasyDCCData data = Data(easydcc);
  char buffer[256];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "EasyDCC reader started." );
  ThreadOp.sleep( 100 );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "trying to get the version..." );
  StrOp.fmtb( buffer, "V\r" );
  __sendCommand(data, buffer);

  ThreadOp.sleep( 100 );

  while( data->run ) {
    Boolean ok = True;

    if( SerialOp.available(data->serial) > 0 ) {
      if( SerialOp.read( data->serial, buffer, 1 ) ) {
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "read command: %c", buffer[0] );
        if( buffer[0] == 'V' ) {
          if( SerialOp.read( data->serial, buffer, 13 ) ) {
            buffer[12] = '\0';
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Version: %s", buffer );
          }
        }
        else if( buffer[0] == 'O' ) {
          if( SerialOp.read( data->serial, buffer, 1 ) ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "EasyDCC is ready" );
          }
        }
      }
    }

    ThreadOp.sleep( 10 );
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "EasyDCC reader ended." );
}



/**  */
static struct OEasyDCC* _inst( const iONode ini ,const iOTrace trc ) {
  iOEasyDCC __EasyDCC = allocMem( sizeof( struct OEasyDCC ) );
  iOEasyDCCData data = allocMem( sizeof( struct OEasyDCCData ) );
  MemOp.basecpy( __EasyDCC, &EasyDCCOp, 0, sizeof( struct OEasyDCC ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  SystemOp.inst();

  data->ini  = ini;
  data->iid  = StrOp.dup( wDigInt.getiid( ini ) );
  data->dummyio  = wDigInt.isdummyio(ini);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "EasyDCC %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid      = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device   = %s", wDigInt.getdevice( ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps      = %d", wDigInt.getbps( ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "flow     = %d", wDigInt.getflow( ini )  );


  data->serialOK = False;
  if( !data->dummyio ) {
    data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );
    SerialOp.setFlow( data->serial, none );
    if( StrOp.equals( wDigInt.dsr, wDigInt.getflow(ini) ) )
      SerialOp.setFlow( data->serial, dsr );
    else if( StrOp.equals( wDigInt.cts, wDigInt.getflow(ini) ) )
      SerialOp.setFlow( data->serial, cts );
    else if( StrOp.equals( wDigInt.xon, wDigInt.getflow(ini) ) )
      SerialOp.setFlow( data->serial, xon );
    SerialOp.setLine( data->serial, wDigInt.getbps(ini), 8, 1, none, wDigInt.isrtsdisabled( ini ) );
    SerialOp.setTimeout( data->serial, wDigInt.gettimeout(ini), wDigInt.gettimeout(ini) );
    data->serialOK = SerialOp.open( data->serial );
  }

  if(data->serialOK) {
    data->run = True;
    data->reader = ThreadOp.inst( "easyReader", &__reader, __EasyDCC );
    ThreadOp.start( data->reader );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unable to initialize device; switch to dummy mode" );
    data->dummyio = True;
  }


  instCnt++;
  return __EasyDCC;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/easydcc.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
