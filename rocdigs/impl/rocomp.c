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


#include "rocdigs/impl/rocomp_impl.h"
#include "rocdigs/impl/rocomp/rocomp-const.h"

#include "rocs/public/mem.h"
#include "rocs/public/usb.h"

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

#define VENDOR 0x16d0
#define PRODUCT 0x04d3
#define DEVCLASS 3
#define CONFIG  1
#define INTERFACE 0

static byte __makeXor(byte* buf, int len);

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iORocoMPData data = Data(inst);
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

/** ----- ORocoMP ----- */

static void __setLocAddr( int addr, byte* addrbytes ) {
  /*
  Für Lokadressen < 100 gilt:
  Highbyte der Lokadresse ist 0x00
  Lowbyte der Lokadresse ist 0x00 bis 0x63

  Für Lokadresse von 100 bis 9999 gilt:

  Highbyte der Lokadresse ist: AH = (ADR&0xFF00)+0xC000
  Lowbyte der Lokadresse ist: AL = (ADR&0x00FF)
  */
  if( addr < 100 ) {
    addrbytes[0] = 0;
    addrbytes[1] = addr & 0x00FF;
  }
  else {
    addrbytes[0] = ((addr & 0xFF00) >> 8) + 0xC0;
    addrbytes[1] = addr & 0x00FF;
  }
}


static void __translate( iORocoMP inst, iONode node ) {
  iORocoMPData data = Data(inst);

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      byte* outa = allocMem(65);
      outa[0] = 0x80 + 5;
      outa[1] = 5;
      outa[2] = USB_XPRESSNET;
      outa[3] = 0x21;
      outa[4] = 0x80;
      outa[5] = __makeXor(outa+1, 4);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      ThreadOp.post( data->transactor, (obj)outa );
    }
    else if( StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      byte* outa = allocMem(65);
      outa[0] = 0x80 + 4;
      outa[1] = 4;
      outa[2] = USB_XPRESSNET;
      outa[3] = 0x80;
      outa[4] = __makeXor(outa+1, 3);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Emergency break" );
      ThreadOp.post( data->transactor, (obj)outa );
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      byte* outa = allocMem(65);
      outa[0] = 0x80 + 5;
      outa[1] = 5;
      outa[2] = USB_XPRESSNET;
      outa[3] = 0x21;
      outa[4] = 0x81;
      outa[5] = __makeXor(outa+1, 4);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      ThreadOp.post( data->transactor, (obj)outa );
    }
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    byte* outa = allocMem(65);
    int  addr = wLoc.getaddr( node );
    int   dir = wLoc.isdir( node );
    int spcnt = wLoc.getspcnt( node );
    int speed = 0;

    int reqid = 0x12; /* default 28 speed steps */
    switch( spcnt ) {
      case 27:
        reqid = 0x11;
        break;
      case 14:
        reqid = 0x10;
        break;
      case 127:
      case 128:
        reqid = 0x13;
        spcnt = 127;
        break;
      default:
        reqid = 0x12;
        spcnt = 28;
        break;
    }

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * spcnt) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * spcnt) / wLoc.getV_max( node );
    }

    /* Speed and direction 09 40 E5 13 00 03 90 00 */
    outa = allocMem(65);
    outa[0] = 0x80 + 8;
    outa[1] = 8;
    outa[2] = USB_XPRESSNET;
    outa[3] = 0xE5;
    outa[4] = reqid; /* 128 steps */
    outa[5] = addr/256;
    outa[6] = addr%256;
    outa[7] = speed + (dir?0x80:0x00);
    outa[8] = 0;
    outa[9] = __makeXor(outa+1, 8);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %d velocity=%d dir=%s", addr, speed, (dir?"fwd":"rev") );
    ThreadOp.post( data->transactor, (obj)outa );
  }


  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int addr      = wFunCmd.getaddr( node );
    int fgroup    = wFunCmd.getgroup( node );
    int fnchanged = wFunCmd.getfnchanged( node );
    byte fn = 0;

    byte* outa = allocMem(65);
    outa[0] = 0x80 + 8;
    outa[1] = 8;
    outa[2] = USB_XPRESSNET;
    outa[3] = 0xE4;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %d change function=%d", addr, fnchanged );

    if( fnchanged < 5 ) {
      /* F1-F4 */
      fn |= wFunCmd.isf0(node) ? 0x10:0x00;
      fn |= wFunCmd.isf1(node) ? 0x01:0x00;
      fn |= wFunCmd.isf2(node) ? 0x02:0x00;
      fn |= wFunCmd.isf3(node) ? 0x04:0x00;
      fn |= wFunCmd.isf4(node) ? 0x08:0x00;
      outa[4] = 0x20;
    }
    else if( fnchanged < 9 ) {
      /* F5-F8 */
      fn |= wFunCmd.isf5(node) ? 0x01:0x00;
      fn |= wFunCmd.isf6(node) ? 0x02:0x00;
      fn |= wFunCmd.isf7(node) ? 0x04:0x00;
      fn |= wFunCmd.isf8(node) ? 0x08:0x00;
      outa[4] = 0x21;
    }
    else if( fnchanged < 13 ) {
      /* F9-F12 */
      fn |= wFunCmd.isf9 (node) ? 0x01:0x00;
      fn |= wFunCmd.isf10(node) ? 0x02:0x00;
      fn |= wFunCmd.isf11(node) ? 0x04:0x00;
      fn |= wFunCmd.isf12(node) ? 0x08:0x00;
      outa[4] = 0x22;
    }
    else if( fnchanged < 21 ) {
      /* F13-F20 */
      fn |= wFunCmd.isf13(node) ? 0x01:0x00;
      fn |= wFunCmd.isf14(node) ? 0x02:0x00;
      fn |= wFunCmd.isf15(node) ? 0x04:0x00;
      fn |= wFunCmd.isf16(node) ? 0x08:0x00;
      fn |= wFunCmd.isf17(node) ? 0x10:0x00;
      fn |= wFunCmd.isf18(node) ? 0x20:0x00;
      fn |= wFunCmd.isf19(node) ? 0x40:0x00;
      fn |= wFunCmd.isf20(node) ? 0x80:0x00;
      outa[4] = 0xF3;
    }
    else {
      /* F21-F28 */
      fn |= wFunCmd.isf21(node) ? 0x01:0x00;
      fn |= wFunCmd.isf22(node) ? 0x02:0x00;
      fn |= wFunCmd.isf23(node) ? 0x04:0x00;
      fn |= wFunCmd.isf24(node) ? 0x08:0x00;
      fn |= wFunCmd.isf25(node) ? 0x10:0x00;
      fn |= wFunCmd.isf26(node) ? 0x20:0x00;
      fn |= wFunCmd.isf27(node) ? 0x40:0x00;
      fn |= wFunCmd.isf28(node) ? 0x80:0x00;
      outa[4] = 0xF4;
    }

    outa[5] = addr/256;
    outa[6] = addr%256;
    outa[7] = fn;
    outa[8] = __makeXor(outa+1, 7);
    ThreadOp.post( data->transactor, (obj)outa );

  }


  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    byte* outb = allocMem(65);
    int addr = wSwitch.getaddr1( node );
    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );

    if( port == 0 )
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      AddrOp.fromPADA( port, &addr, &port );

    if( port > 0 ) port--;
    if( addr > 0 ) addr--;

    int gate1  = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 0x00:0x01; //0 = use gate 1, 1 = use gate 2
    int action = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 0x00:0x08; //0 = gate off, 8 = gate on

    outb[0] = 0x80 + 7;
    outb[1] = 7;
    outb[2] = USB_XPRESSNET;
    outb[3] = 0x53;
    outb[4] = addr/256;
    outb[5] = addr%256;

    if( wSwitch.issinglegate( node ) ) {
      //when single gate turn gate on (cmd straight) or off (cmd turnout)
      outb[6] = 0x90 | action | (port << 1) | gate; //first rocomotion trace shows roco uses 0x9 as high nibble against 0x8 as official xpressnet
      outb[7] = __makeXor(outb+1, 6);
      ThreadOp.post( data->transactor, (obj)outb );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "turnout gate %d %d %d %s", addr+1, port+1, gate, action==0?"off":"on" );
    } else {
      //otherwise turn gate 1 (cmd turnout) or gate 2 (cmd straight) on and 100 ms later off again
      outb[6] = 0x90 | 0x08 | (port << 1) | gate1;  //turn gate on
      outb[7] = __makeXor(outb+1, 6);
      ThreadOp.post( data->transactor, (obj)outb );

      ThreadOp.sleep(100);

      outb = allocMem(65);
      outb[0] = 0x80 + 7;
      outb[1] = 7;
      outb[2] = USB_XPRESSNET;
      outb[3] = 0x53;
      outb[4] = addr/256;
      outb[5] = addr%256;
      outb[6] = 0x90 | 0x00 | (port << 1) | gate1;  //turn gate off
      outb[7] = __makeXor(outb+1, 6);
      ThreadOp.post( data->transactor, (obj)outb );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "turnout %d %d %s", addr+1, port+1, wSwitch.getcmd( node ) );
    }
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {

    int addr   = wOutput.getaddr( node );
    int port   = wOutput.getport( node );
    int gate   = wOutput.getgate( node );

    if( port == 0 )
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      AddrOp.fromPADA( port, &addr, &port );

    if( port > 0 ) port--;
    if( addr > 0 ) addr--;

    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x08:0x00;

    // make message:
    byte* outb = allocMem(65);
    outb[0] = 0x80 + 7;
    outb[1] = 7;
    outb[2] = USB_XPRESSNET;
    outb[3] = 0x53;
    outb[4] = addr/256;
    outb[5] = addr%256;
    outb[6] = 0x90 | action | (port << 1) | gate;
    outb[7] = __makeXor(outb+1, 6);
    ThreadOp.post( data->transactor, (obj)outb );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d %d %d %s", addr+1, port+1, gate, wOutput.getcmd( node ) );
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {

    if( wProgram.getcmd( node ) == wProgram.get ) {
      int cv = wProgram.getcv( node );
      int addr = wProgram.getaddr( node );
      Boolean pom = wProgram.ispom(node);

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get CV%d on %s...", cv, pom?"POM":"PT" );

      if( pom ) {

      }
      else {
        if( wProgram.getmode(node) == wProgram.mode_register ) {
          byte* outb = allocMem(65);
          outb[0] = 0x80 + 5;
          outb[1] = 5;
          outb[2] = USB_XPRESSNET;
          outb[3] = 0x22;
          outb[4] = 0x11;
          outb[5] = cv & 0x0F;
          outb[6] = __makeXor(outb+1, 5);
          ThreadOp.post( data->transactor, (obj)outb );
        }
        else {
          byte* outb = allocMem(65);
          outb[0] = 0x80 + 5;
          outb[1] = 5;
          outb[2] = USB_XPRESSNET;
          outb[3] = 0x22;
          outb[4] = 0x15;
          outb[5] = cv & 0xFF;
          outb[6] = __makeXor(outb+1, 5);
          ThreadOp.post( data->transactor, (obj)outb );
        }
        byte* outb = allocMem(65);
        outb[0] = 0x80 + 5;
        outb[1] = 5;
        outb[2] = USB_XPRESSNET;
        outb[3] = 0x21;
        outb[4] = 0x10;
        outb[5] = 0x31;
        outb[6] = __makeXor(outb+1, 5);
        ThreadOp.post( data->transactor, (obj)outb );
      }
    }
    else if( wProgram.getcmd( node ) == wProgram.set ) {
      int cv = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int decaddr = wProgram.getdecaddr( node );
      Boolean pom = wProgram.ispom(node);

      if( pom ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "POM: set CV%d of loco %d to %d...", cv, decaddr, value );

        if( data->power) {
          if (cv > 0) cv--;

          // make message:
          byte* outb = allocMem(65);
          outb[0] = 0x80 + 9;
          outb[1] = 9;
          outb[2] = USB_XPRESSNET;
          outb[3] = 0xE6;
          outb[4] = 0x30;
          __setLocAddr( decaddr, outb+5 );
          outb[7] = ((cv & 0x0300) >> 8) + 0xEC;
          outb[8] = cv & 0x00FF;
          outb[9] = value & 0xFF;
          outb[10] = __makeXor(outb+1, 9);

          if ( cv != 0 && decaddr != 0)
            ThreadOp.post( data->transactor, (obj)outb );
          else if (decaddr == 0)
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "We do not POM to address == 0!");
          else
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "POM does not allow writing of address!");
        }
        else {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "POM: not processing; Power is OFF" );
        }

      }
      else {

        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set CV%d to %d...", cv, value );

        if( wProgram.getmode(node) == wProgram.mode_register ) {
          byte* outb = allocMem(65);
          outb[0] = 0x80 + 6;
          outb[1] = 6;
          outb[2] = USB_XPRESSNET;
          outb[3] = 0x23;
          outb[4] = 0x12;
          outb[5] = cv & 0x0F;
          outb[6] = value & 0xFF;
          outb[7] = __makeXor(outb+1, 6);
          ThreadOp.post( data->transactor, (obj)outb );
        }
        else {
          byte* outb = allocMem(65);
          outb[0] = 0x80 + 6;
          outb[1] = 6;
          outb[2] = USB_XPRESSNET;
          outb[3] = 0x23;
          outb[4] = 0x16;
          outb[5] = cv & 0xFF;
          outb[6] = value & 0xFF;
          outb[7] = __makeXor(outb+1, 6);
          ThreadOp.post( data->transactor, (obj)outb );
        }

        byte* outb = allocMem(65);
        outb[0] = 0x80 + 5;
        outb[1] = 5;
        outb[2] = USB_XPRESSNET;
        outb[3] = 0x21;
        outb[4] = 0x10;
        outb[5] = 0x31;
        outb[6] = __makeXor(outb+1, 5);
        ThreadOp.post( data->transactor, (obj)outb );

      }
    }
  }

}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iORocoMPData data = Data(inst);

  if( cmd != NULL ) {
    __translate( (iORocoMP)inst, cmd );

    /* Cleanup */
    NodeOp.base.del(cmd);
  }

  return NULL;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iORocoMPData data = Data(inst);
  data->run = False;
  ThreadOp.sleep(500);
  USBOp.close(data->usb);
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iORocoMPData data = Data(inst);
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


static byte __makeXor(byte* buf, int len) {
  int i = 0;
  int xor = 0;

  for(i = 2; i < len; i++) {
    xor ^= buf[i];
  }
  return xor;
}


static void __reportState(iORocoMP inst) {
  iORocoMPData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "power=%s temp=%d load=%d", data->power?"ON":"OFF", data->temp, data->current );

  if( data->listenerFun != NULL && data->listenerObj != NULL ) {
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );

    if( data->iid != NULL )
      wState.setiid( node, data->iid );
    wState.setpower( node, data->power );
    wState.settemp( node, data->temp );
    wState.setload( node, data->current );

    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}


static void __handleSystemState(iORocoMP roco, byte* packet) {
  iORocoMPData data = Data(roco);
  int current  = packet[2] + packet[3] * 256;
  int fcurrent = packet[4] + packet[5] * 256;
  int temp     = packet[6] + packet[7] * 256;
  int state    = packet[8];
  char* stateStr = NULL;
  /*
#define csEmergencyStop           0x01  // Der Nothalt ist eingeschaltet
#define csTrackVoltageOff         0x02  // Die Gleisspannung ist abgeschaltet
#define csShortCircuit            0x04  // Kurschluss am Gleisausgang
#define csNotDefined              0x08  // Nicht definiert bzw. unbekannt
#define csAutoMode                0x10  // Der Automatische Startmodus ist aktiv
#define csProgrammingModeActive   0x20  // Der Programmiermodus ist aktiv
#define csColdStart               0x40  // Kaltstart
#define csRamError                0x80  // RAM Fehler in der Zentrale
   */

  if( data->state != state || (data->current < fcurrent && fcurrent - data->current >= 2) ||
      (data->current > fcurrent && data->current - fcurrent > 2) || data->temp != temp ) {
    data->power   = ((state & csTrackVoltageOff) == csTrackVoltageOff) ? False:True;
    data->state   = state;
    data->current = fcurrent;
    data->temp    = temp;
    __reportState(roco);
  }


  if( data->systemstate != state ) {
    data->systemstate = state;
    if( state & csEmergencyStop )
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Emergency stop. state=0x%02X", state );
    if( state & csTrackVoltageOff )
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Rail power is off. state=0x%02X", state );
    if( state & csShortCircuit )
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "short circuit detected. state=0x%02X", state );
    if( state & csAutoMode )
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Automatic start mode is active. state=0x%02X", state );
    if( state & csProgrammingModeActive )
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Programming mode is active. state=0x%02X", state );
    if( state & csColdStart )
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Cold start. state=0x%02X", state );
    if( state & csRamError )
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "RAM error!!! state=0x%02X", state );
    if( state & csNotDefined )
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Undefined error. state=0x%02X", state );
  }
}


static void __handleRMBus(iORocoMP roco, byte* packet) {
  iORocoMPData data = Data(roco);
  int grp = packet[2];
  int i = 0;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "evaluate sensor group %d: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", grp,
      packet[3+0], packet[3+1], packet[3+2], packet[3+3], packet[3+4],
      packet[3+5], packet[3+6], packet[3+7], packet[3+8], packet[3+9] );

  for( i = 0; i < 10; i++ ) {
    int n   = 0;
    int idx = grp * 10 + i;
    byte status = packet[3+i];
    for( n = 0; n < 8; n++ ) {
      int addr = 1 + grp * 10 + i * 8 + n;
      byte mask = (1 << n);
      if( (status & mask) != (data->sensor[idx] & mask) ) {
        iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
        wFeedback.setaddr( nodeC, addr );
        wFeedback.setstate( nodeC, (status & mask) ? True:False );
        if( data->iid != NULL )
          wFeedback.setiid( nodeC, data->iid );
        if( data->listenerFun != NULL && data->listenerObj != NULL )
          data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Sensor[%d] %d=%s", idx, addr, (status & mask)?"on":"off");
      }
    }
    data->sensor[idx] = status;
  }
}


static void __evaluateXpressnet(iORocoMP roco, byte* in) {
  iORocoMPData data = Data(roco);
  int xn = in[2];

  switch( xn ) {
  case 0x61:
    if( in[3] == 0x01 )
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Xpressnet normal operation resumed" );
    else if( in[3] == 0x00 )
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Xpressnet track power off" );
    else if( in[3] == 0x02 )
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Xpressnet service mode entry" );
    break;
  case 0x63:
    if( in[3] == 0x21 )
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Xpressnet version: %d.%d device type is 0x%X", (in[4]>>4), in[4]&0x0F, in[5] );
    break;
  case 0x64:
    if( in[3] == 0x14 ) {
      int cv  = in[4] * 256 + in[5] + 1;
      int val = in[6];
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Xpressnet service mode: cv=%d value=%d", cv, val );

      iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      wProgram.setcv( node, cv );
      wProgram.setvalue( node, val );
      wProgram.setcmd( node, wProgram.datarsp );
      if( data->iid != NULL )
        wProgram.setiid( node, data->iid );

      if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
    break;
  case 0xEF:
  {
    /*
    20141207.102137.953 r9999I transact ORocoMP  0723 unhandled Xpressnet packet: header=0xEF
    20141207.102137.953 r0000I transact (null)   *trace dump( 0x72548E50: length=19 )
        offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|
        --------------------------------------------------------- |----------------|
        00000000: 13 40 EF C9 64 04 80 10 00 00 00 00 00 00 00 00 |.@..d...........|
        00000010: 00 00 D6                                        |...             |
     */
    int     addr   = (in[3]&0x3F) * 256 + in[4];
    int     speed  = in[6]&0x7F;
    int     steps  = in[5]&0x07;
    Boolean dir    = (in[6]&0x80) ? True:False;
    Boolean lights = (in[7]&0x10) ? True:False;
    int     F0     = in[7] & 0x0F;
    int     F1     = in[8];
    int     F2     = in[9];
    int     F3     = in[10];
    iONode  nodeC  = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );

    if( steps == 0 ) steps = 14;
    else if( steps == 0x01 ) steps = 27;
    else if( steps == 0x02 ) steps = 28;
    else steps = 127;

    if( steps == 27 || steps == 28 ) {
      int bit4 = (speed & 0x10) >> 4;
      speed &= 0x0F;
      speed = (speed << 1) + bit4;
    }

    if( speed > 0 ) {
      /* remove ebreak */
      speed--;
    }


    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "loco %d dir=%s fn=%s speed=%d steps=%d F0=0x%02X F1=0x%02X F2=0x%02X F3=0x%02X",
        addr, dir?"fwd":"rev", lights ? "on":"off", speed, steps, F0, F1, F2, F3 );

    wLoc.setaddr( nodeC, addr );
    wLoc.setV_raw( nodeC, speed );
    wLoc.setV_rawMax( nodeC, steps );
    wLoc.setspcnt( nodeC, steps );
    wLoc.setdir( nodeC, dir );
    wLoc.setcmd( nodeC, wLoc.dirfun );
    wLoc.setfn( nodeC, lights );
    wLoc.setthrottleid( nodeC, "xpressnet" );
    if( data->iid != NULL )
      wLoc.setiid( nodeC, data->iid );
    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }
  break;
  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unhandled Xpressnet packet: header=0x%02X", xn );
    TraceOp.dump( NULL, TRCLEVEL_INFO, (char*)in, in[0] );
    break;
  }
}


static void __evaluatePacket(iORocoMP roco, byte* in) {
  iORocoMPData data = Data(roco);
  int len = in[0];
  int usb = in[1];

  switch( usb ) {
  case USB_FIRMWARE_INFO:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Firmware version %d.%d.%d.%d", in[8], in[9], in[10], in[11] );
    break;
  case USB_XPRESSNET:
    __evaluateXpressnet(roco, in);
    break;
  case USB_RMBUS_DATACHANGED:
    __handleRMBus(roco, in);
    break;
  case USB_SYSTEMSTATE_DATACHANGED:
    __handleSystemState(roco, in);
    break;
  default:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "unhandled packet: len=%d usb=0x%02X", len, usb );
    TraceOp.dump ( name, TRCLEVEL_INFO, (char*)in, len );
    break;
  }
}


static void __transactor( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iORocoMP roco = (iORocoMP)ThreadOp.getParm(th);
  iORocoMPData data = Data(roco);
  byte in[128];

  ThreadOp.setDescription( th, "Transactor for RocoMP" );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor started." );

  while( data->run && !data->usbOK ) {
    data->usbOK = USBOp.open(data->usb, VENDOR, PRODUCT, CONFIG, INTERFACE);
    ThreadOp.sleep(1000);
  }

  if( data->run && data->usbOK ) {
    /* Init sequence:
     */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Initialize the CS..." );

    {
      byte cmd[64] = {0x06,USB_SETAUTOINFORMFLAGS,0x07,0x01,0x00,0x00};
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Set auto inform flags" );
      USBOp.write(data->usb, cmd, 64, data->timeout);
      if( USBOp.read(data->usb, cmd, 64, data->timeout) == 0 )
        __evaluatePacket(roco, cmd);
    }

    /* Xpressnet: Power ON */
    {
      byte cmd[64] = {0x05,USB_XPRESSNET,0x21,0x81,0xA0};
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Xpressnet: Power ON" );
      USBOp.write(data->usb, cmd, 64, data->timeout);
      if( USBOp.read(data->usb, cmd, 64, data->timeout) == 0 )
        __evaluatePacket(roco, cmd);
    }

    /* Get RMBus data */
    {
      byte cmd[64] = {0x03,USB_RMBUS_GETDATA,0x00};
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Get RMBus data 0" );
      USBOp.write(data->usb, cmd, 64, data->timeout);
      if( USBOp.read(data->usb, cmd, 64, data->timeout) == 0 )
        __evaluatePacket(roco, cmd);
    }

    {
      byte cmd[64] = {0x03,USB_RMBUS_GETDATA,0x01};
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Get RMBus data 1" );
      USBOp.write(data->usb, cmd, 64, data->timeout);
      if( USBOp.read(data->usb, cmd, 64, data->timeout) == 0 )
        __evaluatePacket(roco, cmd);
    }

    /* Xpressnet: Get version */
    {
      byte cmd[64] = {0x05,USB_XPRESSNET,0x21,0x21,0x00};
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Xpressnet: Get version" );
      USBOp.write(data->usb, cmd, 64, data->timeout);
      if( USBOp.read(data->usb, cmd, 64, data->timeout) == 0 )
        __evaluatePacket(roco, cmd);
    }

    /* Get version */
    {
      byte cmd[64] = {0x02,USB_FIRMWARE_INFO};
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Get version" );
      USBOp.write(data->usb, cmd, 64, data->timeout);
      if( USBOp.read(data->usb, cmd, 64, data->timeout) == 0 )
        __evaluatePacket(roco, cmd);
    }

  }


  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Start transactor loop." );
  while( data->run && data->usbOK ) {
    Boolean doRead = False;
    int didRead = 0;

    byte* post = (byte*)ThreadOp.getPost( th );
    if( post != NULL ) {
      USBOp.write(data->usb, post+1, 64, data->timeout);
      doRead = (post[0] & 0x80) ? True:False;
      freeMem(post);
    }

    MemOp.set(in, 0, sizeof(in));
    didRead = USBOp.read(data->usb, in, 64, 10);

    if( didRead == 0 ) {
      /* evaluate */
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "evaluate packet..." );
      __evaluatePacket(roco, in);
    }

    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor ended." );
}


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;
static int _version( obj inst ) {
  return vmajor*10000 + vminor*100 + patch;
}


/**  */
static struct ORocoMP* _inst( const iONode ini ,const iOTrace trc ) {
  iORocoMP __RocoMP = allocMem( sizeof( struct ORocoMP ) );
  iORocoMPData data = allocMem( sizeof( struct ORocoMPData ) );
  MemOp.basecpy( __RocoMP, &RocoMPOp, 0, sizeof( struct ORocoMP ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini     = ini;
  data->iid     = StrOp.dup( wDigInt.getiid( ini ) );
  data->timeout = wDigInt.gettimeout( ini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocomp %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  ID %04X:%04X", VENDOR, PRODUCT );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  timeout %dms", data->timeout );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->usb = USBOp.inst();
  data->usbOK = USBOp.open(data->usb, VENDOR, PRODUCT, CONFIG, INTERFACE);

  data->run = True;

  data->transactor = ThreadOp.inst( "transactor", &__transactor, __RocoMP );
  ThreadOp.start( data->transactor );

  instCnt++;
  return __RocoMP;
}



/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/rocomp.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
