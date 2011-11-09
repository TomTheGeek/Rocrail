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

#include "rocutils/public/addr.h"


#define INITBYTE 0x37
#define FORWARD 0
#define REVERSE 1
#define LOCCTRLBYTE 0x08
#define ACCCTRLBYTE 0x0C
#define ACCOUTPUT1 0x00
#define ACCOUTPUT2 0x03
#define ACCOUTPUT3 0x0C
#define ACCOUTPUT4 0x0F
#define ACCOUTPUT5 0x30
#define ACCOUTPUT6 0x33
#define ACCOUTPUT7 0x3C
#define ACCOUTPUT8 0x3F



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
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power OFF" );
    }
    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS off */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power ON" );
    }

  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int addr = wSwitch.getaddr1( node );
    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );

    if( port == 0 )
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      AddrOp.fromPADA( port, &addr, &port );

    addr = (addr-1) * 4 + (port-1);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set switch %d to %s",
        wSwitch.getaddr1( node ), wSwitch.getcmd(node) );

    if( data->v10 ) {
      int v10addr = wSwitch.getaddr1( node );
      if( v10addr < 81 && port < 5 ) {
        byte* cmd = allocMem(32);
        cmd[0] = 4; // length
        cmd[1] = 0x3F;
        cmd[2] = 0x0C;
        cmd[3] = data->addr[v10addr];
        cmd[4] = 0xC0 + data->accgate[port-1][StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node))?0:1];
        ThreadOp.post(data->writer, (obj)cmd);

        /* ToDo: Use a timed writer for deactivating. */
        ThreadOp.sleep(data->swtime);
        cmd = allocMem(32);
        cmd[0] = 4; // length
        cmd[1] = 0x3F;
        cmd[2] = 0x0C;
        cmd[3] = data->addr[v10addr];
        cmd[4] = 0x00 + data->accgate[port-1][StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node))?0:1];
        ThreadOp.post(data->writer, (obj)cmd);
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "switch addr=%d(80) or port=%d(4) out of range", v10addr, port );
      }
    }
    else {
      byte* cmd = allocMem(32);
      cmd[0] = 2; // length
      cmd[1] = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node))?33:34;
      cmd[2] = addr;
      ThreadOp.post(data->writer, (obj)cmd);
      data->lastSwCmd = 0;
    }
  }


  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;
    int gate = wOutput.getgate( node );
    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );

    if( port == 0 )
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      AddrOp.fromPADA( port, &addr, &port );

    addr = (addr-1) * 4 + (port-1);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d.%d %s",
        wOutput.getaddr( node ), wOutput.getgate(node), on?"ON":"OFF" );

    if( data->v10 ) {
      int v10addr = wSwitch.getaddr1( node );
      if( v10addr < 81 && port < 5 ) {
        byte* cmd = allocMem(32);
        cmd[0] = 4; // length
        cmd[1] = 0x3F;
        cmd[2] = 0x0C;
        cmd[3] = data->addr[v10addr];
        cmd[4] = (on ? 0xC0:0x00) + data->accgate[port-1][gate?1:0];
        ThreadOp.post(data->writer, (obj)cmd);
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "switch addr=%d(80) or port=%d(4) out of range", v10addr, port );
      }
    }
    else {
      byte* cmd = allocMem(32);
      if( on ) {
        cmd[0] = 2;
        cmd[1] = gate ? 34:33;
        cmd[2] = wSwitch.getaddr1( node );
      }
      else {
        cmd[0] = 1;
        cmd[1] = 32;
      }
      ThreadOp.post(data->writer, (obj)cmd);
      data->lastSwCmd = 0;
    }
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

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco speed=%d dir=%s", speed, dir?"forwards":"reverse" );

    if( data->v10 ) {
      if( speed < 16 && addr < 81 ) {
        byte* cmd = allocMem(32);
        cmd[0] = 4;
        cmd[1] = 0x37;
        cmd[2] = 0x08;
        cmd[3] = data->addr[addr];
        cmd[4] = data->speed[dir?0:1][speed];
        ThreadOp.post(data->writer, (obj)cmd);
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "loco addr=%d(80) or speed=%d(15) out of range", addr, speed );
      }
    }
    else {
      byte* cmd = allocMem(32);
      cmd[0] = 3;
      cmd[1] = speed;
      cmd[2] = addr;
      cmd[3] = dir?43:39;
      ThreadOp.post(data->writer, (obj)cmd);
    }

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
static int patch  = 98;
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
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fb %d = %d", addr, state );
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


static void __poller( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOEditsPro edits = (iOEditsPro)ThreadOp.getParm( th );
  iOEditsProData data = Data(edits);
  byte out[64] = {0};
  int fbmodIdx = 0;

  ThreadOp.sleep(500);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "poller started." );
  while( data->run ) {
    if( MutexOp.wait( data->mux ) ) {

      if( data->v10 ) {
        /* V1.0 */
        if( data->fbmod > 0 ) {

          fbmodIdx++;
          if( fbmodIdx >= data->fbmod )
            fbmodIdx = 0;

          out[0] = 0x37;
          out[1] = 0x20 + fbmodIdx;
          if( SerialOp.write( data->serial, &out[0], 1 ) ) {
            if( SerialOp.read( data->serial, &out[0], 1 ) ) {
              if( SerialOp.write( data->serial, &out[1], 1 ) ) {
                if( SerialOp.read( data->serial, &out[1], 1 ) ) {
                  if( SerialOp.read( data->serial, &out[2], 1 ) ) {
                    /* the data byte... */
                    __evaluateState(data, fbmodIdx+1, out[2]);
                  }
                }
              }
            }
          }
        }
      }
      else {
        /* V1.2 */
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

      /* Release the mutex. */
      MutexOp.post( data->mux );
    }
    ThreadOp.sleep(30);
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "poller ended." );
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

      if( MutexOp.wait( data->mux ) ) {

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

        /* Release the mutex. */
        MutexOp.post( data->mux );
      }
      ThreadOp.sleep(10);
    }


  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "transactor ended." );

}

static void __swTimeWatcher( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOEditsPro edits = (iOEditsPro)ThreadOp.getParm( th );
  iOEditsProData data = Data(edits);
  do {
    ThreadOp.sleep( 100 );
    if( data->v10 ) {

    }
    else {
      if( data->lastSwCmd != -1 && data->lastSwCmd >= data->swtime ) {
        byte* cmd = allocMem(32);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                      "swTimeWatcher() END SWITCHTIME %dms", data->lastSwCmd );

        data->lastSwCmd = -1;

        cmd[0] = 1; // length
        cmd[1] = 32;
        ThreadOp.post(data->writer, (obj)cmd);
      }
      if( data->lastSwCmd != -1 ) {
        data->lastSwCmd += 10;
      }
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



__initAddrMap(iOEditsPro  edits) {
  iOEditsProData data = Data(edits);
  data->addr[ 0] = 0x00; data->addr[10] = 0x33; data->addr[20] = 0x11;
  data->addr[ 1] = 0x03; data->addr[11] = 0x31; data->addr[21] = 0x1C;
  data->addr[ 2] = 0x01; data->addr[12] = 0x3C; data->addr[22] = 0x1F;
  data->addr[ 3] = 0xC0; data->addr[13] = 0x3F; data->addr[23] = 0x1D;
  data->addr[ 4] = 0x0F; data->addr[14] = 0x3D; data->addr[24] = 0x14;
  data->addr[ 5] = 0x0D; data->addr[15] = 0x34; data->addr[25] = 0x17;
  data->addr[ 6] = 0x04; data->addr[16] = 0x37; data->addr[26] = 0x15;
  data->addr[ 7] = 0x07; data->addr[17] = 0x35; data->addr[27] = 0xC0;
  data->addr[ 8] = 0x05; data->addr[18] = 0x10; data->addr[28] = 0xC2;
  data->addr[ 9] = 0x30; data->addr[19] = 0x13; data->addr[29] = 0xC1;

  data->addr[30] = 0xCC; data->addr[40] = 0xFF; data->addr[50] = 0xDD;
  data->addr[31] = 0xCF; data->addr[41] = 0xFC; data->addr[51] = 0xD4;
  data->addr[32] = 0xCD; data->addr[42] = 0xF4; data->addr[52] = 0xD7;
  data->addr[33] = 0xC4; data->addr[43] = 0xF7; data->addr[53] = 0xD5;
  data->addr[34] = 0xC7; data->addr[44] = 0xF5; data->addr[54] = 0x40;
  data->addr[35] = 0xC5; data->addr[45] = 0xD0; data->addr[55] = 0x43;
  data->addr[36] = 0xF0; data->addr[46] = 0xD3; data->addr[56] = 0x41;
  data->addr[37] = 0xF3; data->addr[47] = 0xD1; data->addr[57] = 0x4C;
  data->addr[38] = 0xF1; data->addr[48] = 0xDC; data->addr[58] = 0x4F;
  data->addr[39] = 0xFC; data->addr[49] = 0xDF; data->addr[59] = 0x4D;

  data->addr[60] = 0x44; data->addr[70] = 0x77; data->addr[80] = 0x55;
  data->addr[61] = 0x47; data->addr[71] = 0x75;
  data->addr[62] = 0x45; data->addr[72] = 0x50;
  data->addr[63] = 0x70; data->addr[73] = 0x53;
  data->addr[64] = 0x73; data->addr[74] = 0x51;
  data->addr[65] = 0x71; data->addr[75] = 0x5C;
  data->addr[66] = 0x7C; data->addr[76] = 0x5F;
  data->addr[67] = 0x7F; data->addr[77] = 0x5D;
  data->addr[68] = 0x7D; data->addr[78] = 0x54;
  data->addr[69] = 0x74; data->addr[79] = 0x57;

  data->speed[FORWARD][ 0] = 136; data->speed[REVERSE][ 0] = 162;
  data->speed[FORWARD][ 1] = 137; data->speed[REVERSE][ 1] = 163;
  data->speed[FORWARD][ 2] = 140; data->speed[REVERSE][ 2] = 166;
  data->speed[FORWARD][ 3] = 141; data->speed[REVERSE][ 3] = 167;
  data->speed[FORWARD][ 4] = 152; data->speed[REVERSE][ 4] = 178;
  data->speed[FORWARD][ 5] = 153; data->speed[REVERSE][ 5] = 179;
  data->speed[FORWARD][ 6] = 156; data->speed[REVERSE][ 6] = 182;
  data->speed[FORWARD][ 7] = 157; data->speed[REVERSE][ 7] = 183;
  data->speed[FORWARD][ 8] =  72; data->speed[REVERSE][ 8] =  98;
  data->speed[FORWARD][ 9] =  73; data->speed[REVERSE][ 9] =  99;
  data->speed[FORWARD][10] =  76; data->speed[REVERSE][10] = 102;
  data->speed[FORWARD][11] =  77; data->speed[REVERSE][11] = 103;
  data->speed[FORWARD][12] =  88; data->speed[REVERSE][12] = 114;
  data->speed[FORWARD][13] =  89; data->speed[REVERSE][13] = 115;
  data->speed[FORWARD][14] =  92; data->speed[REVERSE][14] = 118;
  data->speed[FORWARD][15] =  93; data->speed[REVERSE][15] = 119;

  data->accgate[0][0] = ACCOUTPUT1;
  data->accgate[0][1] = ACCOUTPUT2;
  data->accgate[1][0] = ACCOUTPUT3;
  data->accgate[1][1] = ACCOUTPUT4;
  data->accgate[2][0] = ACCOUTPUT5;
  data->accgate[2][1] = ACCOUTPUT6;
  data->accgate[3][0] = ACCOUTPUT7;
  data->accgate[3][1] = ACCOUTPUT8;
}



/**  */
static struct OEditsPro* _inst( const iONode ini ,const iOTrace trc ) {
  iOEditsPro __EditsPro = allocMem( sizeof( struct OEditsPro ) );
  iOEditsProData data = allocMem( sizeof( struct OEditsProData ) );
  MemOp.basecpy( __EditsPro, &EditsProOp, 0, sizeof( struct OEditsPro ), data );

  TraceOp.set( trc );
  SystemOp.inst();

  /* Initialize data->xxx members... */
  data->mux = MutexOp.inst( StrOp.fmt( "serialMux%08X", data ), True );

  data->ini      = ini;
  data->device   = StrOp.dup( wDigInt.getdevice( ini ) );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  data->fbmod     = wDigInt.getfbmod( ini );
  data->swtime    = wDigInt.getswtime( ini );
  data->readfb    = wDigInt.isreadfb( ini );
  data->lastSwCmd = -1;
  data->v10       = wDigInt.getprotver(ini) == 10 ? True:False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "EDiTS PRO %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid       = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "prot.ver. = %s", data->v10 ? "1.0":"1.2" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device    = %s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "baudrate  = %d", 9600 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "handshake = %s", wDigInt.getflow(ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "swtime    = %d", wDigInt.getswtime(ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fbmodules = %d", wDigInt.getfbmod(ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "read fb   = %s", wDigInt.isreadfb(ini)?"yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  if(data->v10) {
    __initAddrMap(__EditsPro);
  }

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

    data->poller = ThreadOp.inst( "editspoller", &__poller, __EditsPro );
    ThreadOp.start( data->poller );
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
