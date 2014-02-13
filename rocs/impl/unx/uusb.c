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

#if defined __linux__
/* sudo apt-get install libusb-1.0-0-dev */
#include <libusb-1.0/libusb.h>
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


static Boolean isWantedDevice( libusb_device *dev, int vendor, int product )
{
  struct libusb_device_descriptor desc;
  int r = libusb_get_device_descriptor( dev, &desc );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "ID 0x%04X:0x%04X [%s]",
      desc.idVendor, desc.idProduct,
      __usbDescription(desc.idVendor, desc.idProduct) );

  if( desc.idVendor == vendor && desc.idProduct == product ){
    return True;
  }

  return False;
}


void* rocs_usb_openUSB(int vendor, int product, int configNr, int interfaceNr) {
  void* husb = NULL;

#if defined __linux__
  // discover devices
  libusb_device **list;
  libusb_device *found = NULL;
  libusb_context *ctx = NULL;
  int attached = 0;

  libusb_init(&ctx);
  libusb_set_debug(ctx,3);
  ssize_t cnt = libusb_get_device_list(ctx, &list);
  ssize_t i = 0;
  int err = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%d USB devices found", cnt );

  // find our device
  for(i = 0; i < cnt; i++){
    libusb_device *device = list[i];
    if( isWantedDevice(device, vendor, product) ){
      found = device;
      break;
    }
  }

  if( found != NULL ) {
    libusb_device_handle *handle;
    err = libusb_open(found, &handle);

    if (err) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not open USB device: %s", libusb_error_name(err) );
      return False;
    }
    husb = handle;

    if ( libusb_kernel_driver_active(husb,0) ){
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "detach kernel driver..." );
      err = libusb_detach_kernel_driver(husb,0);
      if(err) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "detach kernel driver: %s", libusb_error_name(err) );
      }
    }
    else
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Device free from kernel" );

    err = libusb_claim_interface( husb, 0 );
    if (err) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not claim interface: %s", libusb_error_name(err) );
      libusb_close(handle);
      husb = NULL;
    }

  }

  libusb_free_device_list(list, 1);


#endif

  if( husb == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "wanted device 0x%04X:0x%04X not found", vendor, product );
  }
  return husb;
}



Boolean rocs_usb_closeUSB(void* husb, int interfaceNr) {
  int rc = 0;

#if defined __linux__
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "close USB (interface %d)", interfaceNr );
  if( husb != NULL ) {
    libusb_close( (libusb_device_handle *)husb );
    //libusb_exit( ctx );
  }
#endif

  return rc == 0 ? True:False;
}

#define BULK_EP_OUT     0x82
#define BULK_EP_IN      0x08

int rocs_usb_writeUSB(void* husb, byte* out, int len, int timeout) {
  int rc = 0;

#if defined __linux__
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "write %d...", len );
  TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
  if( husb != NULL ) {
    int transferred = 0;
    rc = libusb_bulk_transfer ((libusb_device_handle *)husb, BULK_EP_OUT, out, len, &transferred, timeout);
  }
#endif

  return rc;
}


int rocs_usb_readUSB(void* husb, byte* in, int len, int timeout) {
  int rc = -1;

#if defined __linux__
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "read %d...", len );
  if( husb != NULL ) {
    int transferred = 0;
    rc = libusb_bulk_transfer ((libusb_device_handle *)husb, BULK_EP_IN, in, len, &transferred, timeout);
    if( transferred == len )
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, len );
  }
#endif

  return rc;
}
