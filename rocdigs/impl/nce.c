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

#include "rocutils/public/addr.h"


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
  int sens = rsp[1] + (rsp[0] * 256);
  int mask = rsp[3] + (rsp[2] * 256);
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


static void __evaluateRsp( iONCEData data, byte* out, int outsize, byte* in, int insize ) {
  switch( out[0] ) {
  case 0xA2:
    if( in[0] != '!' )
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Locomotive control command returned [%c]", in[0]);
    break;
  case 0xA9:
  case 0xA1:
    {
      iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      int cv  = out[1] * 156 + out[2];
      int val = in[0];

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "%s reading cv%d [%d]", in[1]=='!' ? "successful":"error", cv, val);

      wProgram.setvalue( node, val );
      wProgram.setcmd( node, wProgram.datarsp );
      wProgram.setcv( node, cv );
      if( data->iid != NULL )
        wProgram.setiid( node, data->iid );

      if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
    break;
  case 0xA8:
  case 0xA0:
  case 0xAE:
  {
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    int cv  = out[1] * 256 + out[2];
    int val = out[3];
    if(out[0] == 0xAE) {
      /* POM */
      cv  = out[3] * 256 + out[4];
      val = out[5];
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "%s writing cv%d [%d]", in[0]=='!' ? "successful":"error", cv, val);

    wProgram.setvalue( node, val );
    wProgram.setcmd( node, wProgram.datarsp );
    wProgram.setcv( node, cv );
    if( data->iid != NULL )
      wProgram.setiid( node, data->iid );

    if( data->listenerFun != NULL && data->listenerObj != NULL )
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
    break;
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
        if( rc ) {
          TraceOp.dump( NULL, TRCLEVEL_BYTE, in, insize );
          __evaluateRsp(data, out, outsize, in, insize);
        }
      }
    }
    MutexOp.post( data->mux );
  }
  return rc;
}


static void __cpFunctions( byte* fn, iONode node ) {
  fn[ 0] = wFunCmd.isf0 ( node );
  fn[ 1] = wFunCmd.isf1 ( node );
  fn[ 2] = wFunCmd.isf2 ( node );
  fn[ 3] = wFunCmd.isf3 ( node );
  fn[ 4] = wFunCmd.isf4 ( node );
  fn[ 5] = wFunCmd.isf5 ( node );
  fn[ 6] = wFunCmd.isf6 ( node );
  fn[ 7] = wFunCmd.isf7 ( node );
  fn[ 8] = wFunCmd.isf8 ( node );
  fn[ 9] = wFunCmd.isf9 ( node );
  fn[10] = wFunCmd.isf10( node );
  fn[11] = wFunCmd.isf11( node );
  fn[12] = wFunCmd.isf12( node );
  fn[13] = wFunCmd.isf13( node );
  fn[14] = wFunCmd.isf14( node );
  fn[15] = wFunCmd.isf15( node );
  fn[16] = wFunCmd.isf16( node );
  fn[17] = wFunCmd.isf17( node );
  fn[18] = wFunCmd.isf18( node );
  fn[19] = wFunCmd.isf19( node );
  fn[20] = wFunCmd.isf20( node );
  fn[21] = wFunCmd.isf21( node );
  fn[22] = wFunCmd.isf22( node );
  fn[23] = wFunCmd.isf23( node );
  fn[24] = wFunCmd.isf24( node );
  fn[25] = wFunCmd.isf25( node );
  fn[26] = wFunCmd.isf26( node );
  fn[27] = wFunCmd.isf27( node );
  fn[28] = wFunCmd.isf28( node );
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
      AddrOp.fromFADA( mod, &mod, &pin, &gate );
    else if( mod == 0 && pin > 0 )
      AddrOp.fromPADA( pin, &mod, &pin );

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
    return 5;
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
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d speed=%d steps=%d lights=%s dir=%s",
        addr, speed, spcnt, fn?"on":"off", dir?"forwards":"reverse" );

    if( spcnt < 128 ) {
      cmd = dir ? 2 : 1;
    }
    else {
      cmd = dir ? 4 : 3;
    }

    if( addr > 127 || StrOp.equals( wLoc.prot_L, wLoc.getprot(node) ) )
      addr += 0xC000;

    out[0] = 0xA2;
    out[1] = addr / 256;
    out[2] = addr & 0xFF;
    out[3] = cmd;
    out[4] = speed;

    *insize = 1; /* Return code from NCE. */
    return 5;
  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );
    int  group = wFunCmd.getgroup(node);
    byte fn [32];
    byte info = 0;
    byte op_1 = 0;

    __cpFunctions(fn, node);
    if( group > 0 )
    group--;

    switch( group ) {
      case 0:
        info = (fn[0]?0x10:0) + (fn[1]?0x01:0) + (fn[2]?0x02:0) + (fn[3]?0x04:0) + (fn[4]?0x08:0);
        op_1 = 7;
        break;
      case 1:
        info = (fn[5]?0x01:0) + (fn[6]?0x02:0) + (fn[7]?0x04:0) + (fn[8]?0x08:0);
        op_1 = 8;
        break;
      case 2:
        info = (fn[9]?0x01:0) + (fn[10]?0x02:0) + (fn[11]?0x04:0) + (fn[12]?0x08:0);
        op_1 = 9;
        break;
      case 3:
      case 4:
        info = (fn[13]?0x01:0) + (fn[14]?0x02:0) + (fn[15]?0x04:0) + (fn[16]?0x08:0 + fn[17]?0x10:0) + (fn[18]?0x20:0) + (fn[19]?0x40:0) + (fn[20]?0x80:0);
        op_1 = 15;
        break;
      case 5:
      case 6:
        info = (fn[21]?0x01:0) + (fn[22]?0x02:0) + (fn[23]?0x04:0) + (fn[24]?0x08:0 + fn[25]?0x10:0) + (fn[26]?0x20:0) + (fn[27]?0x40:0) + (fn[28]?0x80:0);
        op_1 = 16;
        break;
    }

    if( addr > 127 || StrOp.equals( wLoc.prot_L, wLoc.getprot(node) ) )
      addr += 0xC000;

    out[0] = 0xA2;
    out[1] = addr / 256;
    out[2] = addr & 0xFF;
    out[3] = op_1;
    out[4] = info;
    *insize = 1; /* Return code from NCE. */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d function group=%d fn=%02x", addr, group, info );
    return 5;
  }


  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) || StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      out[0] = 0x8B;
      *insize = 1; /* Return code from NCE. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      return 1;
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      out[0] = 0x89;
      *insize = 1; /* Return code from NCE. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      return 1;
    }
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    if(  wProgram.getcmd( node ) == wProgram.pton ) {
      out[0] = 0x9E;
      *insize = 1; /* Return code from NCE. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT ON" );
      return 1;
    }
    else if( wProgram.getcmd( node ) == wProgram.ptoff ) {
      out[0] = 0x9F;
      *insize = 1; /* Return code from NCE. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT OFF" );
      return 1;
    }
    else if( wProgram.getcmd( node ) == wProgram.get ) {
      Boolean direct = wProgram.isdirect( node );
      int cv = wProgram.getcv( node );
      out[0] = direct ? 0xA9:0xA1;
      out[1] = cv / 256;
      out[2] = cv & 0xFF;
      *insize = 2; /* Return code and data from NCE. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get CV%d", cv );
      return 3;
    }
    else if( wProgram.getcmd( node ) == wProgram.set ) {
      Boolean direct = wProgram.isdirect( node );
      int cv = wProgram.getcv( node );
      int val = wProgram.getvalue( node );
      int decaddr = wProgram.getdecaddr( node );
      int addr = decaddr == 0 ? wProgram.getaddr( node ):decaddr;
      int nrbytes = 0;

      if(wProgram.ispom( node )) {
        out[0] = 0xAE;        // NCE ops mode loco command
        out[1] = addr / 256;  // loco high address
        out[2] = addr & 0xFF; // loco low address
        out[3] = cv / 256;    // CV high address
        out[4] = cv & 0xFF;   // CV low address
        out[5] = val;         // CV data
        nrbytes = 6;
      }
      else {
        out[0] = direct ? 0xA8:0xA0;
        out[1] = cv / 256;
        out[2] = cv & 0xFF;
        out[3] = val;
        nrbytes = 4;
      }
      *insize = 1; /* Return code from NCE. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set %d CV%d to %d", addr, cv, val );
      return nrbytes;
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
static void _halt( obj inst, Boolean poweroff) {
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


static Boolean _setRawListener(obj inst, obj listenerObj, const digint_rawlistener listenerFun ) {
  return True;
}

static byte* _cmdRaw( obj inst, const byte* cmd ) {
  return NULL;
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
static int patch  = 0;
static int _version( obj inst ) {
  iONCEData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __pollerThread( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iONCE nce = (iONCE)ThreadOp.getParm( th );
  iONCEData data = Data(nce);
  byte cmd = 0;
  byte rev[4];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "AIU poller started." );
  ThreadOp.sleep( 1000 );

  cmd = 0xAA;
  if( __transact( data, &cmd, 1, rev, 3 ) ) {
    /* software revision */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "NCE Software revision %d.%d.%d", rev[0], rev[1], rev[2] );
  }


  do {
    unsigned char out[32];
    unsigned char in [32];
    int i = 0;

    ThreadOp.sleep( 100 );

    for( i = 0; i < data->aiucnt; i++ ) {

      out[0] = 0x8A;
      out[1] = i + data->aiuaddr;
      if( __transact( data, out, 2, in, 4 ) ) {
        /* inform listener */
        __handleAIU( data, i + data->aiuaddr, in);
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

  TraceOp.set( trc );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "initializing nce library..." );

  /* Initialize data->xxx members... */
  data->mux     = MutexOp.inst( NULL, True );

  data->ini     = ini;
  data->iid     = StrOp.dup( wDigInt.getiid( ini ) );
  data->device  = StrOp.dup( wDigInt.getdevice( ini ) );
  data->aiucnt  = wDigInt.getfbmod( ini );
  data->aiuaddr = wDigInt.getfboffset( ini );
  data->run     = True;

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
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, 0, wDigInt.isrtsdisabled( ini ) );
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
