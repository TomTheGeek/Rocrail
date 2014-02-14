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

#include "rocs/public/mem.h"


void* rocs_usb_openUSB(int vendor, int product, int configNr, int interfaceNr, int* input, int* output);
Boolean rocs_usb_closeUSB(void* husb, int interfaceNr);
int rocs_usb_writeUSB(void* husb, int endpoint, byte* out, int len, int timeout);
int rocs_usb_readUSB(void* husb, int endpoint, byte* in, int len, int timeout);


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOUSBData data = Data(inst);
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

/** ----- OUSB ----- */


/**  */
static Boolean _close( struct OUSB* inst ) {
  iOUSBData data = Data(inst);
  return rocs_usb_closeUSB(data->husb, data->interfaceNr);
}


/**  */
static Boolean _open( struct OUSB* inst ,int vendor ,int product ,int configNr ,int interfaceNr ) {
  iOUSBData data = Data(inst);
  data->interfaceNr = interfaceNr;
  data->husb = rocs_usb_openUSB(vendor, product, configNr, interfaceNr, &data->input_ep, &data->output_ep);
  return data->husb != NULL ? True:False;
}


/**  */
static int _read( struct OUSB* inst ,unsigned char* buf ,int len, int timeout ) {
  iOUSBData data = Data(inst);
  return rocs_usb_readUSB(data->husb, data->input_ep, buf, len, timeout);
}


/**  */
static int _write( struct OUSB* inst ,unsigned char* buf ,int len, int timeout ) {
  iOUSBData data = Data(inst);
  return rocs_usb_writeUSB(data->husb, data->output_ep, buf, len, timeout);
}


/** Object creator. */
static struct OUSB* _inst( void ) {
  iOUSB __USB = allocMem( sizeof( struct OUSB ) );
  iOUSBData data = allocMem( sizeof( struct OUSBData ) );
  MemOp.basecpy( __USB, &USBOp, 0, sizeof( struct OUSB ), data );

  /* Initialize data->xxx members... */

  instCnt++;
  return __USB;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/usb.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
