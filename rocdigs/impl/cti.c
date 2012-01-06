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


/*
Command Acknowledge Message
0x00  The command has been processed successfully.
0x01  The command has been processed, but cannot be carried out at this time because the CTI network is offline.
      The command will take effect once the network is placed online.
0x02  The command addresses a control or sensor beyond the range of the current network hardware.
￼0x03  An unknown command message was received and discarded.


Service Request Message
￼0x81  A change in the state of one or more sensors has been observed.
￼￼￼0x82  Communications with the CTI network have been lost.

￼
            OPC    AddrH       AddrL
Activate   (0x01) (Bits 15:8) (Bits 7:0)
Deactivate (0x02) (Bits 15:8) (Bits 7:0)
Pulse ON   (0x03) (Bits 15:8) (Bits 7:0) (‘N’ 10’ths of seconds)
Pulse OFF  (0x04) (Bits 15:8) (Bits 7:0) (‘N’ 10’ths of seconds)
Blink      (0x05) (Bits 15:8) (Bits 7:0) (‘N’ 10’ths of seconds)
Blink rev. (0x06) (Bits 15:8) (Bits 7:0) (‘N’ 10’ths of seconds)
Control 4  (0x07) (Bits 15:8) (Bits 7:0) (state xxxx3210)
Control 8  (0x08) (Bits 15:8) (Bits 7:0) (state 76543210)
Control 16 (0x09) (Bits 15:8) (Bits 7:0) (state FEDCBA98) (state 76543210)

￼Throttle   (0x0A) (Bits 15:8) (Bits 7:0) Speed ￼￼Attributes
  Attribute Bits
  Bits (2:0) Momentum Control 000 (Minimum Inertia) through 111 (Maximum Inertia)
  Bit 3      Brake Control 0 = Brake Off 1 = Brake On
  Bit 4      Direction Control 0 = Forward 1 = Reverse
  Bit 5      Idle Voltage Control 0 = Do not apply an idling voltage
                                  1 = Maintain a small idling voltage for use with current sensors
  Bit 6      Unused -> 0
  Bit 7      Unused -> 0

Emergency stop (0x0B)
Signal 2       (0x0C) (Bits 15:8) (Bits 7:0) (aspect xxYY2211)
Signal 3       (0x0D) (Bits 15:8) (Bits 7:0) (aspect xx332211)
Signal 4       (0x0E) (Bits 15:8) (Bits 7:0) (aspect 44332211)

Configure sensors (0x10) (Bits 15:8) (Bits 7:0) (config)
Read all sensors  (0x14)

Get firmware version (0x19)
  ￼*/

#define OPC_ACTIVATE 0x01
#define OPC_DEACTIVATE 0x02
#define OPC_PULSEON 0x03
#define OPC_THROTTLE 0x0A
#define OPC_READALLSENS 0x14
#define OPC_QNETWORK 0x18


#include "rocdigs/impl/cti_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"
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
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Accessory.h"

#include "rocutils/public/addr.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOCTIData data = Data(inst);
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

/** ----- OCTI ----- */

static iONode __translate( iOCTI inst, iONode node ) {
  iOCTIData data = Data(inst);
  iONode rsp = NULL;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "translate: %s", NodeOp.getName(node) );


  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.ebreak ) ) {
      /* CS ebreak */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request emergency break" );
      byte* cmd = allocMem(32);
      cmd[ 0] = 1;
      cmd[ 1] = 0x0B;
      ThreadOp.post(data->writer, (obj)cmd);
    }
    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS on */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power ON" );
      byte* cmd = allocMem(32);
      cmd[ 0] = 1;
      cmd[ 1] = 0x16;
      ThreadOp.post(data->writer, (obj)cmd);
    }
    else if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      /* CS off */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power OFF" );
      byte* cmd = allocMem(32);
      cmd[ 0] = 1;
      cmd[ 1] = 0x17;
      ThreadOp.post(data->writer, (obj)cmd);
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int addr = wSwitch.getaddr1( node );
    int gate = wSwitch.getgate1( node );

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ) {
      addr += 1;
    }

    byte* cmd = allocMem(32);
    cmd[ 0] = 4;
    cmd[ 1] = 0x03;
    cmd[ 2] = addr / 256;
    cmd[ 3] = addr % 256;
    cmd[ 4] = 25;
    ThreadOp.post(data->writer, (obj)cmd);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "switch %d %s", addr, wSwitch.getcmd( node ) );
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on );
    int addr = wOutput.getaddr( node );

    byte* cmd = allocMem(32);
    cmd[ 0] = 3;
    cmd[ 1] = on ? OPC_ACTIVATE:OPC_DEACTIVATE;
    cmd[ 2] = addr / 256;
    cmd[ 3] = addr % 256;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d %s",
        wOutput.getaddr( node ), on?"ON":"OFF" );
    ThreadOp.post(data->writer, (obj)cmd);
  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );
  }

  /* Loc command.
   * ￼Throttle   (0x0A) (Bits 15:8) (Bits 7:0) Speed ￼￼Attributes
        Attribute Bits
        Bits (2:0) Momentum Control 000 (Minimum Inertia) through 111 (Maximum Inertia)
        Bit 3      Brake Control 0 = Brake Off 1 = Brake On
        Bit 4      Direction Control 0 = Forward 1 = Reverse
        Bit 5      Idle Voltage Control 0 = Do not apply an idling voltage
                                        1 = Maintain a small idling voltage for use with current sensors
   *
   */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    Boolean fn  = wLoc.isfn( node );
    Boolean dir = wLoc.isdir( node ); /* True == forwards */

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * wLoc.getspcnt(node)) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * wLoc.getspcnt(node)) / wLoc.getV_max( node );
    }

    byte* cmd = allocMem(32);
    cmd[ 0] = 5;
    cmd[ 1] = OPC_THROTTLE;
    cmd[ 2] = addr / 256;
    cmd[ 3] = addr % 256;
    cmd[ 4] = speed;
    cmd[ 5] = wLoc.getmass(node) + (dir << 4);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco speed=%d dir=%s", speed, dir?"forwards":"reverse" );
    ThreadOp.post(data->writer, (obj)cmd);

  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    Boolean direct = wProgram.isdirect(node);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "program type %d...", wProgram.getlntype(node) );

    if( wProgram.getlntype(node) == wProgram.lntype_sv && wProgram.getcmd( node ) == wProgram.lncvget &&
        wProgram.getcv(node) == 0 && wProgram.getmodid(node) == 0 && wProgram.getaddr(node) == 0 )
    {
      /* This construct is used to to query all LocoIOs, but is here recycled for query all CAN-GC2s. */
      byte* cmd = allocMem(32);
      cmd[ 0] = 1;
      cmd[ 1] = OPC_QNETWORK;

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "query network configuration..." );
      ThreadOp.post(data->writer, (obj)cmd);
    }
  }

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOCTIData data = Data(inst);
  iONode rsp = NULL;

  if( cmd != NULL ) {
    int bus = 0;
    rsp = __translate( (iOCTI)inst, cmd );
    cmd->base.del(cmd);
  }
  return rsp;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOCTIData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  data->run = False;
  ThreadOp.sleep(500);
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOCTIData data = Data(inst);
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
  iOCTIData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOCTI cti = (iOCTI)ThreadOp.getParm( th );
  iOCTIData data = Data(cti);
  byte* cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer started." );
  ThreadOp.sleep(1000);

  while( data->run ) {
    byte * post = NULL;
    int len = 0;
    byte out[32] = {0};

    ThreadOp.sleep(10);
    post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      /* first byte is the message length */
      len = post[0];
      MemOp.copy( out, post+1, len);
      freeMem( post);

      TraceOp.dump( NULL, TRCLEVEL_INFO, (char*)out, len );
      if( !SerialOp.write( data->serial, (char*)out, len ) ) {
        /* sleep and send it again? */
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer ended." );
}



static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOCTI cti = (iOCTI)ThreadOp.getParm( th );
  iOCTIData data = Data(cti);
  byte* cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader started." );
  ThreadOp.sleep(100);

  { /* Start of day */
    byte* cmd = allocMem(32);
    cmd[ 0] = 1;
    cmd[ 1] = OPC_READALLSENS;
    ThreadOp.post(data->writer, (obj)cmd);
  }

  while( data->run ) {
    byte in[32] = {0};

    ThreadOp.sleep(10);
    if( SerialOp.available(data->serial) ) {
      SerialOp.read(data->serial, in, 1);
      TraceOp.dump ( name, TRCLEVEL_INFO, (char*)in, 1 );
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader ended." );
}



/**
 * The COM port should be set for operation at 9600 Baud, with 8 data bits, 1 stop bit, and no parity.
 *
 */
static struct OCTI* _inst( const iONode ini ,const iOTrace trc ) {
  iOCTI __CTI = allocMem( sizeof( struct OCTI ) );
  iOCTIData data = allocMem( sizeof( struct OCTIData ) );
  MemOp.basecpy( __CTI, &CTIOp, 0, sizeof( struct OCTI ), data );

  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */

  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CTI %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = %s", wDigInt.getdevice( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->run = True;

  data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );
  SerialOp.setFlow( data->serial, none );
  SerialOp.setLine( data->serial, 9600, 8, 1, 0, wDigInt.isrtsdisabled( ini ) );
  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {
    data->reader = ThreadOp.inst( "ctireader", &__reader, __CTI );
    ThreadOp.start( data->reader );
    data->writer = ThreadOp.inst( "ctiwriter", &__writer, __CTI );
    ThreadOp.start( data->writer );
  }
  else
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init CTI port!" );

  instCnt++;
  return __CTI;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/cti.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
