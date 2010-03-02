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

#include "rocdigs/impl/xpressnet_impl.h"
#include "rocdigs/impl/xpressnet/liusb.h"
#include "rocdigs/impl/xpressnet/li101.h"
#include "rocrail/wrapper/public/DigInt.h"


Boolean liusbConnect(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init serial port for LI-USB..." );
  data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
  SerialOp.setFlow( data->serial, cts );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );
  SerialOp.setLine( data->serial, 57600, 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
  return SerialOp.open( data->serial );
}

void liusbDisConnect(obj xpressnet) {
  li101DisConnect(xpressnet);
}

Boolean liusbAvail(obj xpressnet) {
  return li101Avail(xpressnet);
}

void liusbInit(obj xpressnet) {
  li101Init(xpressnet);
}

int liusbRead(obj xpressnet, byte* buffer, Boolean* rspreceived) {
  iOXpressNetData data = Data(xpressnet);
  int len = 0;
  Boolean ok = False;

  if( data->dummyio )
    return 0;

  if( MutexOp.wait( data->serialmux ) ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "reading bytes from LI-USB..." );
    if( SerialOp.read( data->serial, buffer, 2 ) ) {
      /* TODO: check if it is the expected frame */
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)buffer, 2 );
      if( SerialOp.read( data->serial, buffer, 1 ) ) {
        len = (buffer[0] & 0x0f) + 1;
        ok = SerialOp.read( data->serial, (char*)buffer+1, len );
        TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)buffer, len + 1 );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "could not read header byte from LI-USB..." );
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "could not read frame from LI-USB..." );
    }
    MutexOp.post( data->serialmux );
  }

  return ok ? len:0;
}

Boolean liusbWrite(obj xpressnet, byte* outin, Boolean* rspexpected) {
  iOXpressNetData data = Data(xpressnet);

  int len = 0;
  Boolean rc = False;
  unsigned char out[256];

  if( data->dummyio )
    return True;

  *rspexpected = 1; /* LIUSB or CS will confirm every command */

  len = makeChecksum(outin);

  if( len == 0 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "zero bytes to write LI-USB" );
    return False;
  }

  /* make extra header for LI-USB*/
  MemOp.copy( out+2, outin, len );

  len = len+2;
  out[0] = 0xFF;
  out[1] = 0xFE;

  if( MutexOp.wait( data->serialmux ) ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "writing bytes to LI-USB" );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
    rc = SerialOp.write( data->serial, (char*)out, len );
    MutexOp.post( data->serialmux );
  }

  return rc;
}

