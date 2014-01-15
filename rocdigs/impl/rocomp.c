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

static Boolean __openUSB(iORocoMP inst);
static Boolean __closeUSB(iORocoMP inst);
static Boolean __writeUSB(iORocoMP inst, byte* out, int len);
static Boolean __readUSB(iORocoMP inst, byte* in, int len);

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
          "USB class %d 0x%04X:0x%04X", dev->descriptor.bDeviceClass, dev->descriptor.idVendor, dev->descriptor.idProduct );

      if( dev->descriptor.bDeviceClass == DEVCLASS && dev->descriptor.idVendor == VENDOR && dev->descriptor.idProduct == PRODUCT ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "found RocoMP" );
        data->husb = usb_open(dev);
        break;
      }
    }
  }
#endif

  return data->husb == NULL ? False:True;
}



static Boolean __closeUSB(iORocoMP inst) {
  iORocoMPData data = Data(inst);
  int rc = 0;

#if defined __linux__
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "close USB of RocoMP" );
  if( data->husb != NULL ) {
    rc = usb_close((usb_dev_handle *)data->husb);
  }
#endif

  return rc == 0 ? True:False;
}


static Boolean __writeUSB(iORocoMP inst, byte* out, int len) {
  iORocoMPData data = Data(inst);
  int rc = 0;

#if defined __linux__
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "write %d", len );
  if( data->husb != NULL ) {
    rc = usb_bulk_write((usb_dev_handle *)data->husb, 1, out, len, 1000);
  }
#endif

  return rc == 0 ? True:False;
}


static Boolean __readUSB(iORocoMP inst, byte* in, int len) {
  iORocoMPData data = Data(inst);
  int rc = 0;

#if defined __linux__
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "write %d", len );
  if( data->husb != NULL ) {
    rc = usb_bulk_read((usb_dev_handle *)data->husb, 1, in, len, 1000);
  }
#endif

  return rc == 0 ? True:False;
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
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  __openUSB(__RocoMP);

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
