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
#include "rocdigs/impl/xpressnet/li101.h"
#include "rocdigs/impl/xpressnet/common.h"
#include "rocrail/wrapper/public/DigInt.h"

Boolean li101Connect(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
  SerialOp.setFlow( data->serial, StrOp.equals( wDigInt.cts, wDigInt.getflow( data->ini ) ) ? cts:none );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );
  SerialOp.setLine( data->serial, wDigInt.getbps( data->ini ), 8, 1, 0, wDigInt.isrtsdisabled( data->ini ) );
  return SerialOp.open( data->serial );
}

void li101DisConnect(obj xpressnet) {
}

Boolean li101Avail(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  return SerialOp.available(data->serial) > 0 ? True:False;
}



void li101Init(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);

  /* XpressNet
     Asking for Interface version*/
  byte* out = allocMem(32);
  out[0] = 0xF0;
  ThreadOp.post( data->transactor, (obj)out );

  /* Asking for CS version */
  out = allocMem(32);
  out[0] = 0x21;
  out[1] = 0x21;
  out[2] = 0x00;
  ThreadOp.post( data->transactor, (obj)out );



  if( data->startpwstate) {
    /* ALL ON */
    out = allocMem(32);
    out[0] = 0x21;
    out[1] = 0x81;
    out[2] = 0xA0;
    ThreadOp.post( data->transactor, (obj)out );
  } else {
    /* ALL OFF*/
    out = allocMem(32);
    out[0] = 0x21;
    out[1] = 0x80;
    out[2] = 0xA1;
    ThreadOp.post( data->transactor, (obj)out );
  }
}

int li101Read(obj xpressnet, byte* buffer) {
  iOXpressNetData data = Data(xpressnet);
  int len = 0;
  Boolean ok = False;

  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "trying to read..." );
  if( MutexOp.wait( data->serialmux ) ) {
    if( SerialOp.read( data->serial, buffer, 1 ) ) {
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "header byte read = 0x%02X", buffer[0] );
      len = (buffer[0] & 0x0f) + 1;
      ok = SerialOp.read( data->serial, (char*)buffer+1, len );
    }
    MutexOp.post( data->serialmux );
  }

  return ok ? len:0;
}


Boolean li101Write(obj xpressnet, byte* out, int* rspexpected) {
  iOXpressNetData data = Data(xpressnet);

  int len = 0;
  int i = 0;
  Boolean rc = False;
  byte bXor = 0;

  *rspexpected = rspExpected(out);

  len = out[0] & 0x0f;
  len++; /* header */

  if( out[0] == 0x00 ) {
    return False;
  }

  for ( i = 0; i < len; i++ ) {
    bXor ^= out[i];
  }
  out[i] = bXor;
  len++; /* checksum */

  if( MutexOp.wait( data->serialmux ) ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "out buffer" );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
    if( !data->dummyio ) {
      rc = SerialOp.write( data->serial, (char*)out, len );
    }

    MutexOp.post( data->serialmux );
  }

  return rc;
}


