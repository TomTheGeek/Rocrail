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


static void __translate( iORocoMP inst, iONode node ) {
  iORocoMPData data = Data(inst);

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      byte* outa = allocMem(32);
      outa[0] = 0x80 + 5;
      outa[1] = 5;
      outa[2] = 0x40;
      outa[3] = 0x21;
      outa[4] = 0x80;
      outa[5] = __makeXor(outa+1, 4);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      ThreadOp.post( data->transactor, (obj)outa );
    }
    else if( StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      byte* outa = allocMem(32);
      outa[0] = 0x80 + 4;
      outa[1] = 4;
      outa[2] = 0x40;
      outa[3] = 0x80;
      outa[4] = __makeXor(outa+1, 3);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Emergency break" );
      ThreadOp.post( data->transactor, (obj)outa );
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      byte* outa = allocMem(32);
      outa[0] = 0x80 + 5;
      outa[1] = 5;
      outa[2] = 0x40;
      outa[3] = 0x21;
      outa[4] = 0x81;
      outa[5] = __makeXor(outa+1, 4);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      ThreadOp.post( data->transactor, (obj)outa );
    }
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    byte* outa = allocMem(32);
    int  addr = wLoc.getaddr( node );
    int   dir = wLoc.isdir( node );
    int    fn = wLoc.isfn( node );
    int spcnt = wLoc.getspcnt( node );
    int speed = 0;

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * spcnt) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * spcnt) / wLoc.getV_max( node );
    }

    /* PC control 07 40 E3 F0 00 03 */
    outa[0] = 0x80 + 7;
    outa[1] = 7;
    outa[2] = 0x40;
    outa[3] = 0xE3;
    outa[4] = 0xF0;
    outa[5] = addr/256;
    outa[6] = addr%256;
    outa[7] = __makeXor(outa+1, 6);
    ThreadOp.post( data->transactor, (obj)outa );

    /* Speed and direction 09 40 E5 13 00 03 90 00 */
    outa = allocMem(32);
    outa[0] = 0x80 + 9;
    outa[1] = 9;
    outa[2] = 0x40;
    outa[3] = 0xE5;
    outa[4] = 0x13;
    outa[5] = addr/256;
    outa[6] = addr%256;
    outa[7] = speed + (dir?0x80:0x00);
    outa[8] = 0x00;
    outa[9] = __makeXor(outa+1, 8);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d velocity=%d dir=%s fn=%d", addr, speed, (dir?"fwd":"rev"), fn );
    ThreadOp.post( data->transactor, (obj)outa );

    /* Light function 08 40 E4 F8 00 01 00 */
    outa = allocMem(32);
    outa[0] = 0x80 + 8;
    outa[1] = 8;
    outa[2] = 0x40;
    outa[3] = 0xE4;
    outa[4] = 0xF8;
    outa[5] = addr/256;
    outa[6] = addr%256;
    outa[7] = fn;
    outa[8] = __makeXor(outa+1, 7);
    ThreadOp.post( data->transactor, (obj)outa );

  }


  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    byte* outb = allocMem(32);
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
    outb[2] = 0x40;
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

      outb = allocMem(32);
      outb[0] = 0x80 + 7;
      outb[1] = 7;
      outb[2] = 0x40;
      outb[3] = 0x53;
      outb[4] = addr/256;
      outb[5] = addr%256;
      outb[6] = 0x90 | 0x00 | (port << 1) | gate1;  //turn gate off
      outb[7] = __makeXor(outb+1, 6);
      ThreadOp.post( data->transactor, (obj)outb );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "turnout %d %d %s", addr+1, port+1, wSwitch.getcmd( node ) );
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
    /* set auto inform flags */
    { byte cmd[8] = {0x06,0x50,0x00,0x00,0x01,0x07}; USBOp.write(data->usb, cmd, 6); }

    /* Xpressnet: Power ON */
    { byte cmd[8] = {0x05,0x40,0x21,0x81,0xA0}; USBOp.write(data->usb, cmd, 5); USBOp.read(data->usb, in, 64);}

    /* Get RMBus data */
    { byte cmd[8] = {0x03,0x81,0x00}; USBOp.write(data->usb, cmd, 3); USBOp.read(data->usb, in, 64);}
    { byte cmd[8] = {0x03,0x81,0x01}; USBOp.write(data->usb, cmd, 3); USBOp.read(data->usb, in, 64);}

    /* Xpressnet: Get version */
    { byte cmd[8] = {0x05,0x40,0x21,0x21,0x00}; USBOp.write(data->usb, cmd, 5); USBOp.read(data->usb, in, 64);}

    /* Get version */
    { byte cmd[8] = {0x02,0x01}; USBOp.write(data->usb, cmd, 2); USBOp.read(data->usb, in, 64);}

    /* Get Code */
    { byte cmd[8] = {0x02,0x18}; USBOp.write(data->usb, cmd, 2); USBOp.read(data->usb, in, 64);}

  }


  while( data->run && data->usbOK ) {
    Boolean doRead = False;
    Boolean didRead = False;

    byte* post = (byte*)ThreadOp.getPost( th );
    if( post != NULL ) {
      USBOp.write(data->usb, post+1, post[0]&0x7F);
      doRead = (post[0] & 0x80) ? True:False;
      freeMem(post);
    }

    if( doRead ) {
      MemOp.set(in, 0, sizeof(in));
      didRead = USBOp.read(data->usb, in, 64);
    }

    if( didRead ) {
      /* evaluate */
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "evaluate packet..." );
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
  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocomp %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  ID %04X:%04X", VENDOR, PRODUCT );
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
