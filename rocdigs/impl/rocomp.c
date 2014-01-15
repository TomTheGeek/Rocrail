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

#include "rocs/public/mem.h"

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

#if defined __linux__
/* sudo apt-get install libusb-dev */
#include <usb.h>
#endif

#define VENDOR 0x16d0
#define PRODUCT 0x04d3
#define DEVCLASS 3
#define CONFIG  1
#define INTERFACE 0

static Boolean __openUSB(iORocoMP inst);
static Boolean __closeUSB(iORocoMP inst);
static Boolean __writeUSB(iORocoMP inst, byte* out, int len);
static Boolean __readUSB(iORocoMP inst, byte* in, int len);
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
  __closeUSB((iORocoMP)inst);
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


static const char* __usbDescription(int vendor, int product) {
  if( vendor == 0x0403 ) {
    /* FTDI */
    if( product == 0x6001 ) return "FDTI FT232 USB-Serial (UART) IC";
    if( product == 0x6007 ) return "FDTI Serial Converter";
    if( product == 0x6008 ) return "FDTI Serial Converter";
    if( product == 0x6009 ) return "FDTI Serial Converter";
    if( product == 0xbfd8 ) return "OpenDCC";
    if( product == 0xbfd9 ) return "OpenDCC Sniffer";
    if( product == 0xbfda ) return "OpenDCC Throttle";
    if( product == 0xbfdb ) return "OpenDCC Gateway";
    if( product == 0xbfdc ) return "OpenDCC GBM";
    if( product == 0xbfdd ) return "OpenDCC GBMBoost Master";
    return "FTDI";
  }
  if( vendor == VENDOR ) {
    if( product == PRODUCT ) return "Roco 10786 Multizentrale";
  }
  return "-";
}

/* ToDo: Move the USB related calls to the Rocs library. */

static Boolean __openUSB(iORocoMP inst) {
  iORocoMPData data = Data(inst);

#if defined __linux__
  struct usb_bus *busses;
  struct usb_bus *bus;

  usb_init();
  usb_find_busses();
  usb_find_devices();

  busses = usb_get_busses();

  for (bus = busses; bus; bus = bus->next) {
    struct usb_device *dev;

    for (dev = bus->devices; dev; dev = dev->next) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "bus %s device %s class %02X:%02X ID 0x%04X:0x%04X protocol=%02d maxpacket=%02d [%s]",
          bus->dirname, dev->filename, dev->descriptor.bDeviceClass, dev->config->interface->altsetting->bInterfaceClass,
          dev->descriptor.idVendor, dev->descriptor.idProduct, dev->descriptor.bDeviceProtocol,
          dev->descriptor.bMaxPacketSize0, __usbDescription(dev->descriptor.idVendor, dev->descriptor.idProduct) );


      if( dev->descriptor.idVendor == VENDOR && dev->descriptor.idProduct == PRODUCT ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "found RocoMP" );
        data->husb = usb_open(dev);

        if( data->husb == NULL ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not open USB device" );
          break;
        }

        if(usb_kernel_driver_active(data->husb, 0) == 1) {
          if( usb_detach_kernel_driver(data->husb, 0) != 0 ) {
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not detach kernel driver" );
            usb_close((usb_dev_handle *)data->husb);
            data->husb = NULL;
            break;
          }
        }

        if( usb_set_configuration((usb_dev_handle *)data->husb, CONFIG) != 0 ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not set configuration %d", CONFIG );
          usb_close((usb_dev_handle *)data->husb);
          data->husb = NULL;
          break;
        }

        if( usb_claim_interface(data->husb, INTERFACE) != 0 ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not claim interface %d", INTERFACE );
          usb_close((usb_dev_handle *)data->husb);
          data->husb = NULL;
          break;
        }

        break;
      }
    }
  }
#endif

  if( data->husb == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no RocoMP found" );
  }
  return data->husb == NULL ? False:True;
}



static Boolean __closeUSB(iORocoMP inst) {
  iORocoMPData data = Data(inst);
  int rc = 0;

#if defined __linux__
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "close USB of RocoMP" );
  if( data->husb != NULL ) {
    usb_release_interface((usb_dev_handle *)data->husb, INTERFACE);
    rc = usb_close((usb_dev_handle *)data->husb);
    usb_exit();
  }
#endif

  return rc == 0 ? True:False;
}


static Boolean __writeUSB(iORocoMP inst, byte* out, int len) {
  iORocoMPData data = Data(inst);
  int rc = 0;

#if defined __linux__
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "write %d...", len );
  TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
  if( data->husb != NULL ) {
    rc = usb_bulk_write((usb_dev_handle *)data->husb, 1, out, len, 1000);
  }
#endif

  return rc == 0 ? True:False;
}


static Boolean __readUSB(iORocoMP inst, byte* in, int len) {
  iORocoMPData data = Data(inst);
  int rc = -1;

#if defined __linux__
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "read %d...", len );
  if( data->husb != NULL ) {
    rc = usb_bulk_read((usb_dev_handle *)data->husb, 1, in, len, 1000);
    if( rc == 0 )
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, len );
  }
#endif

  return rc == 0 ? True:False;
}


static void __transactor( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iORocoMP roco = (iORocoMP)ThreadOp.getParm(th);
  iORocoMPData data = Data(roco);
  byte in[128];

  ThreadOp.setDescription( th, "Transactor for RocoMP" );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor started." );

  while( data->run ) {
    Boolean doRead = False;
    Boolean didRead = False;

    byte* post = (byte*)ThreadOp.getPost( th );
    if( post != NULL ) {
      __writeUSB(roco, post+1, post[0]&0x7F);
      doRead = (post[0] & 0x80) ? True:False;
      freeMem(post);
    }

    if( doRead ) {
      MemOp.set(in, 0, sizeof(in));
      didRead = __readUSB(roco, in, 64);
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

  __openUSB(__RocoMP);

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
