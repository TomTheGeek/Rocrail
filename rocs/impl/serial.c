/*
 Rocs - OS independent C library

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <stdlib.h>
#include <string.h>

#include "rocs/impl/serial_impl.h"
#include "rocs/public/system.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"

static int instCnt = 0;

/*
 ***** __Private functions.
 */
/* OS dependent: windows(wsocket.c) (os/2)OSerial.c (unix)usocket.o */
Boolean rocs_serial_open( iOSerial inst );
Boolean rocs_serial_close( iOSerial inst );
Boolean rocs_serial_write( iOSerial inst, const char* buf, int size );
Boolean rocs_serial_read( iOSerial inst, char* buf, int size );
Boolean rocs_serial_avail( iOSerial inst );
Boolean rocs_serial_isCTS( iOSerial inst );
Boolean rocs_serial_isDSR( iOSerial inst );
Boolean rocs_serial_isRI( iOSerial inst );
Boolean rocs_serial_isUartEmpty( iOSerial inst, Boolean soft );
void rocs_serial_setSerialMode( iOSerial inst, serial_mode mode );
void rocs_serial_waitMM( iOSerial inst, int usperiod, int uspause );
void rocs_serial_setRTS( iOSerial inst, Boolean rts );
void rocs_serial_setDTR( iOSerial inst, Boolean dtr );
void rocs_serial_setCTS( iOSerial inst, Boolean cts );
int rocs_serial_getWaiting( iOSerial inst );
void rocs_serial_setOutputFlow( iOSerial inst, Boolean flow );
void rocs_serial_flush( iOSerial inst );

/*
 ***** OBase functions.
 */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static const char* __name(void) {
  return name;
}
static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}
static void __deserialize(void* inst, unsigned char* a) {
}
static char* __toString(void* inst) {
  iOSerialData data = Data(inst);
  return data->device;
}
static void __del(void* inst) {
  iOSerialData data = Data(inst);
  rocs_serial_close( inst );
  StrOp.freeID( data->device, RocsSerialID );
  freeIDMem( data, RocsSerialID );
  freeIDMem( inst, RocsSerialID );
  instCnt--;
}
static int __count(void) {
  return instCnt;
}
static void* __properties(void* inst) {
  return NULL;
}
static struct OBase* __clone( void* inst ) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

/*
 ***** OSerial public functions.
 */
static void _setBlocking( iOSerial inst, Boolean blocking ) {
  iOSerialData data = Data(inst);
  data->blocking=blocking;	
}

static void _setLine( iOSerial inst, int bps,serial_databits bits,
                      serial_stopbits stopbits, serial_parity parity, Boolean rtsdisabled ) {
  iOSerialData data = Data(inst);
  data->line.bps      = bps;
  data->line.bits     = bits;
  data->line.stopbits = stopbits;
  data->line.parity   = parity;
  data->line.rtsdisabled = rtsdisabled;
}

static void _setFlow( iOSerial inst, serial_flow flow ) {
  iOSerialData data = Data(inst);
  data->line.flow = flow;
}

static void _setTimeout( iOSerial inst, int wtimeout, int rtimeout ) {
  iOSerialData data = Data(inst);
  data->timeout.write = wtimeout;
  data->timeout.read  = rtimeout;
}

static void _setDivisor( iOSerial inst, int divisor ) {
  iOSerialData data = Data(inst);
  data->divisor = divisor;
}

static Boolean _writecSerial( iOSerial inst, char c ) {
  return rocs_serial_write( inst, &c, 1 );
}


static char _readcSerial( iOSerial inst ) {
  char buf[1] = { '\0' };
  rocs_serial_read( inst, buf, 1 );
  return buf[0];
}

static int _available( iOSerial inst ) {
  return rocs_serial_avail( inst );
}

static int _getRc( iOSerial inst ) {
  iOSerialData data = Data(inst);
  return data->rc;
}

static int _getReadCnt( iOSerial inst ) {
  iOSerialData data = Data(inst);
  return data->read;
}

static void _setPortBase( iOSerial inst, int addr ) {
  iOSerialData data = Data(inst);
  data->portbase = addr;
}


static iOSerial _inst( const char* device ) {
  iOSerial     serial = allocIDMem( sizeof( struct OSerial ), RocsSerialID );
  iOSerialData data   = allocIDMem( sizeof( struct OSerialData ), RocsSerialID );

  MemOp.basecpy( serial, &SerialOp, 0, sizeof( struct OSerial ), data );

  data->device = StrOp.dupID( device, RocsSerialID );
  data->blocking = True;
  instCnt++;

  return serial;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/serial.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/

