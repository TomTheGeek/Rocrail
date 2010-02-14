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
#include "rocdigs/impl/xpressnet/xntcp.h"
#include "rocdigs/impl/xpressnet/li101.h"

#include "rocrail/wrapper/public/DigInt.h"

Boolean xntcpConnect(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);

  TraceOp.trc( "xntcp", TRCLEVEL_INFO, __LINE__, 9999, "XnTcp at %s:%d",
      wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );

  data->socket = SocketOp.inst( wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ), False, False );

  if ( SocketOp.connect( data->socket ) ) {
    return True;
  }
  else {
    SocketOp.base.del( data->socket );
    return False;
  }
}

void xntcpDisConnect(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  if( data->socket != NULL ) {
    SocketOp.disConnect( data->socket );
    SocketOp.base.del( data->socket );
    data->socket = NULL;
  }
}

Boolean xntcpsAvail(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  char msgStr[32];
  return SocketOp.peek( data->socket, msgStr, 1 );
}

void xntcpInit(obj xpressnet) {
  li101Init(xpressnet);
}
int xntcpRead(obj xpressnet, byte* buffer) {
  iOXpressNetData data = Data(xpressnet);
  if( SocketOp.read( data->socket, buffer, 1 ) ) {
    int len = (buffer[0] & 0x0F) + 1;
    if( SocketOp.read( data->socket, buffer+1, len ) )
      return len;
  }
  return 0;
}
Boolean xntcpWrite(obj xpressnet, byte* out, int* rspexpected) {
  iOXpressNetData data = Data(xpressnet);

  int len = 0;
  int i = 0;
  Boolean rc = False;
  byte bXor = 0;

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
    rc = SocketOp.write( data->socket, out, len );
    MutexOp.post( data->serialmux );
  }
  return rc;
}
