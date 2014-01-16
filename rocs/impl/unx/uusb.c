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

#include "rocs/impl/usb_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/thread.h"
#include "rocs/public/system.h"
#include "rocs/public/str.h"

#if defined __linux__ &! defined __APPLE__
/* sudo apt-get install libusb-dev */
#include <usb.h>
#endif


static const char* __usbDescription(int vendor, int product) {
  if( vendor == 0x16d0 ) {
    if( product == 0x04d3 ) return "Roco 10786 MultizentralePro";
  }
  if( vendor == 0x1d6b ) {
    return "Linux Foundation";
  }
  if( vendor == 0x0403 ) {
    /* FTDI */
    if( product == 0x6001 ) return "FTDI FT232 USB-Serial (UART) IC";
    if( product == 0x6007 ) return "FTDI Serial Converter";
    if( product == 0x6008 ) return "FTDI Serial Converter";
    if( product == 0x6009 ) return "FTDI Serial Converter";
    if( product == 0xbfd8 ) return "OpenDCC";
    if( product == 0xbfd9 ) return "OpenDCC Sniffer";
    if( product == 0xbfda ) return "OpenDCC Throttle";
    if( product == 0xbfdb ) return "OpenDCC Gateway";
    if( product == 0xbfdc ) return "OpenDCC GBM";
    if( product == 0xbfdd ) return "OpenDCC GBMBoost Master";
    return "FTDI";
  }
  if( vendor == 0x045E ) {
    return "Microsoft Corp.";
  }
  if( vendor == 0x046D ) {
    return "Logitech, Inc.";
  }
  if( vendor == 0x8087 ) {
    return "Intel Corp.";
  }
  return "-";
}


void* rocs_usb_openUSB(int vendor, int product, int configNr, int interfaceNr) {
  void* husb = NULL;

#if defined __linux__ &! defined __APPLE__
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


      if( dev->descriptor.idVendor == vendor && dev->descriptor.idProduct == product ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "found wanted device 0x%04X:0x%04X", dev->descriptor.idVendor, dev->descriptor.idProduct );
        husb = usb_open(dev);

        if( husb == NULL ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not open USB device" );
          break;
        }

        if(usb_kernel_driver_active(husb, 0) == 1) {
          if( usb_detach_kernel_driver(husb, 0) != 0 ) {
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not detach kernel driver" );
            usb_close((usb_dev_handle *)husb);
            husb = NULL;
            break;
          }
        }

        if( usb_set_configuration((usb_dev_handle *)husb, configNr) != 0 ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not set configuration %d", configNr );
          usb_close((usb_dev_handle *)husb);
          husb = NULL;
          break;
        }

        if( usb_claim_interface(husb, interfaceNr) != 0 ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not claim interface %d", interfaceNr );
          usb_close((usb_dev_handle *)husb);
          husb = NULL;
          break;
        }

        break;
      }
    }
  }
#endif

  if( husb == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "wanted device 0x%04X:0x%04X not found", vendor, product );
  }
  return husb;
}



Boolean rocs_usb_closeUSB(void* husb, int interfaceNr) {
  int rc = 0;

#if defined __linux__ &! defined __APPLE__
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "close USB (interface %d)", interfaceNr );
  if( husb != NULL ) {
    usb_release_interface((usb_dev_handle *)husb, interfaceNr);
    rc = usb_close((usb_dev_handle *)husb);
    usb_exit();
  }
#endif

  return rc == 0 ? True:False;
}


Boolean rocs_usb_writeUSB(void* husb, byte* out, int len) {
  int rc = 0;

#if defined __linux__ &! defined __APPLE__
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "write %d...", len );
  TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
  if( husb != NULL ) {
    rc = usb_bulk_write((usb_dev_handle *)husb, 1, out, len, 1000);
  }
#endif

  return rc == 0 ? True:False;
}


Boolean rocs_usb_readUSB(void* husb, byte* in, int len) {
  int rc = -1;

#if defined __linux__ &! defined __APPLE__
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "read %d...", len );
  if( husb != NULL ) {
    rc = usb_bulk_read((usb_dev_handle *)husb, 1, in, len, 1000);
    if( rc == 0 )
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, len );
  }
#endif

  return rc == 0 ? True:False;
}
