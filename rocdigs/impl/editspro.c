/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocdigs/impl/editspro_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/CBus.h"
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

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOEditsProData data = Data(inst);
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

/** ----- OEditsPro ----- */


static iONode __translate( iOEditsPro edits, iONode node ) {
  iOEditsProData data = Data(edits);
  iONode rsp = NULL;

  if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      /* CS off */
      byte* cmd = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power OFF" );
      ThreadOp.post(data->writer, (obj)cmd);
    }
    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS off */
      byte* cmd = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power ON" );
      ThreadOp.post(data->writer, (obj)cmd);
    }

  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    byte* cmd = allocMem(32);
    int delay = wSwitch.getdelay(node) > 0 ? wSwitch.getdelay(node):data->swtime;

    cmd[0] = 2; // length
    cmd[1] = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node))?33:34;
    cmd[2] = wSwitch.getaddr1( node );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set switch %d to %s",
        wSwitch.getaddr1( node ), wSwitch.getcmd(node) );
    ThreadOp.post(data->writer, (obj)cmd);
    data->lastSwCmd = 0;
  }


  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    byte* cmd = allocMem(32);
    Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;
    int   gate = wOutput.getgate( node );

    if( on ) {
      cmd[0] = 2;
      cmd[1] = gate ? 34:33;
      cmd[2] = wSwitch.getaddr1( node );
    }
    else {
      cmd[0] = 1;
      cmd[1] = 32;
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d.%d %s",
        wOutput.getaddr( node ), wOutput.getgate(node), on?"ON":"OFF" );
    ThreadOp.post(data->writer, (obj)cmd);
    data->lastSwCmd = 0;
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
    byte* cmd = allocMem(32);

    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    Boolean fn  = wLoc.isfn( node );
    Boolean dir = wLoc.isdir( node ); /* True == forwards */

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * 15) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * 15) / wLoc.getV_max( node );
    }

    cmd[0] = 3;
    cmd[1] = speed;
    cmd[2] = addr;
    cmd[3] = dir?43:39;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco speed=%d dir=%s", speed, dir?"forwards":"reverse" );
    ThreadOp.post(data->writer, (obj)cmd);
  }

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOEditsProData data = Data(inst);
  iONode rsp = NULL;

  if( cmd != NULL ) {
    int bus = 0;
    rsp = __translate( (iOEditsPro)inst, cmd );
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
  iOEditsProData data = Data(inst);
  data->run = False;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOEditsProData data = Data(inst);
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
  iOEditsProData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static __evaluateState( iOEditsProData data, int mod, byte val ) {

  if( data->fb[mod] != val )  {
    int n = 0;
    int addr = 0;
    int state = 0;
    for( n = 0; n < 8; n++ ) {
      if( (data->fb[mod] & (0x01 << n)) != (val & (0x01 << n)) ) {
        addr = (mod-1) * 8 + (7-n) + 1;

        state = (val & (0x01 << n)) ? 1:0;
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "fb %d = %d", addr, state );
        {
          /* inform listener: Node3 */
          iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
          wFeedback.setaddr( nodeC, addr );
          wFeedback.setstate( nodeC, state?True:False );
          if( data->iid != NULL )
            wFeedback.setiid( nodeC, data->iid );

          data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
        }
      }
    }
  }
}




static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOEditsPro edits = (iOEditsPro)ThreadOp.getParm( th );
  iOEditsProData data = Data(edits);
  byte* cmd = NULL;
  Boolean ok = False;

  ThreadOp.sleep(500);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "transactor started." );
  while( data->run ) {
    byte * post = NULL;
    int len = 0;
    int i = 0;
    byte out[64] = {0};

    ThreadOp.sleep(10);
    post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      /* first byte is the message length */
      len = post[0];
      MemOp.copy( out, post+1, len);
      freeMem( post);

      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );

      for( i = 0; i < len; i++ ) {
        if( SerialOp.write( data->serial, &out[i], 1 ) ) {
          byte b = 0;
          if( SerialOp.read( data->serial, &b, 1 ) ) {
            if( b != out[i] ) {
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "read 0x%02X, expected 0x%02X", b, out[i] );
              ThreadOp.sleep(500);
              break;
            }
          }
        }
      }
      ThreadOp.sleep(10);
    }

    out[0] = 190;
    if( SerialOp.write( data->serial, &out[0], 1 ) ) {
      if( SerialOp.read( data->serial, &out[0], 1 ) ) {
        if( out[0] > 0 && out[0] < 65 ) {
          int mod = out[0];
          ThreadOp.sleep(5);
          out[0] = 191 + mod;
          if( SerialOp.write( data->serial, &out[0], 1 ) ) {
            if( SerialOp.read( data->serial, &out[0], 1 ) ) {
              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "fb module %d = 0x%02X ", mod, out[0] );
              __evaluateState(data, mod, out[0]);
            }
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "unexpected last changed response: %d", out[0] );
          ThreadOp.sleep(1000);
        }
      }
    }


  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "transactor ended." );

}

static void __swTimeWatcher( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOEditsPro edits = (iOEditsPro)ThreadOp.getParm( th );
  iOEditsProData data = Data(edits);
  do {
    ThreadOp.sleep( 10 );
    if( data->lastSwCmd != -1 && data->lastSwCmd >= data->swtime ) {
      byte* cmd = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                    "swTimeWatcher() END SWITCHTIME %dms", data->lastSwCmd );

      cmd[0] = 1; // length
      cmd[1] = 32;

      ThreadOp.post(data->writer, (obj)cmd);
    }
    if( data->lastSwCmd != -1 ) {
      data->lastSwCmd += 10;
    }
  } while( data->run );
}




static Boolean __flush( iOEditsProData data ) {
  /* Read all pending information on serial port. Interface Hickups if data is pending from previous init! */
  int bAvail = SerialOp.available(data->serial);
  if( bAvail > 0 && bAvail < 1000 ) {
    char c;
    TraceOp.trc(name, TRCLEVEL_WARNING, __LINE__, 9999, "Flushing %d bytes...", bAvail);
    while( SerialOp.available(data->serial) > 0 ) {
      SerialOp.read( data->serial, &c, 1 );
    };
  }
  else if(bAvail >= 1000) {
    TraceOp.trc(name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Can not flush %d bytes, check your hardware!", bAvail);
    return False;
  }
  else {
    TraceOp.trc(name, TRCLEVEL_DEBUG, __LINE__, 9999, "flushed");
  }
  return True;
}






/**  */
static struct OEditsPro* _inst( const iONode ini ,const iOTrace trc ) {
  iOEditsPro __EditsPro = allocMem( sizeof( struct OEditsPro ) );
  iOEditsProData data = allocMem( sizeof( struct OEditsProData ) );
  MemOp.basecpy( __EditsPro, &EditsProOp, 0, sizeof( struct OEditsPro ), data );

  TraceOp.set( trc );
  SystemOp.inst();

  /* Initialize data->xxx members... */
  data->device   = StrOp.dup( wDigInt.getdevice( ini ) );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  data->fbmod    = wDigInt.getfbmod( ini );
  data->swtime   = wDigInt.getswtime( ini );
  data->readfb   = wDigInt.isreadfb( ini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "EDiTS PRO %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid       = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device    = %s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "baudrate  = %d", 9600 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "handshake = %s", wDigInt.getflow(ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "swtime    = %d", wDigInt.getswtime(ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fbmodules = %d", wDigInt.getfbmod(ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "read fb   = %s", wDigInt.isreadfb(ini)?"yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, StrOp.equals( wDigInt.cts, wDigInt.getflow( data->ini ) ) ? cts:none );
  SerialOp.setLine( data->serial, 9600, 8, 1, none, wDigInt.isrtsdisabled( ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );
  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {
    char* thname = NULL;
    data->run = True;

    __flush(data);

    thname = StrOp.fmt("editstx%X", __EditsPro);
    data->writer = ThreadOp.inst( thname, &__writer, __EditsPro );
    StrOp.free(thname),
    ThreadOp.start( data->writer );

    data->swTimeWatcher = ThreadOp.inst( "swTimeWatcher", &__swTimeWatcher, __EditsPro );
    ThreadOp.start( data->swTimeWatcher );
  }
  else
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init rclink port!" );

  instCnt++;
  return __EditsPro;
}

iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/editspro.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
