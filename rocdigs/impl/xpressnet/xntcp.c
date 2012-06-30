/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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

  data->socket = SocketOp.inst( wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ), False, False, False );
  SocketOp.setRcvTimeout( data->socket, wDigInt.gettimeout(data->ini) / 1000);

  if( data->socket != NULL ) {
    SocketOp.setNodelay(data->socket, True);
  }
  else {
    return False;
  }

  if ( SocketOp.connect( data->socket ) ) {
    return True;
  }
  else {
    SocketOp.base.del( data->socket );
    data->socket = NULL;
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

Boolean xntcpAvail(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  char msgStr[32];
  if( SocketOp.isBroken(data->socket) ) {
    return False;
  }
  return SocketOp.peek( data->socket, msgStr, 1 );
}

void xntcpInit(obj xpressnet) {
  li101Init(xpressnet);
}
int xntcpRead(obj xpressnet, byte* buffer, Boolean* rspreceived) {
  iOXpressNetData data = Data(xpressnet);
  if( !SocketOp.isBroken(data->socket) && SocketOp.read( data->socket, buffer, 1 ) ) {
    int len = (buffer[0] & 0x0F) + 1;
    if( SocketOp.read( data->socket, buffer+1, len ) )
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)buffer, len+1 );
      return len;
  }
  return 0;
}
Boolean xntcpWrite(obj xpressnet, byte* out, Boolean* rspexpected) {
  iOXpressNetData data = Data(xpressnet);

  int len = 0;
  int i = 0;
  Boolean rc = False;
  byte bXor = 0;
  
  if( data->socket == NULL || SocketOp.isBroken(data->socket) ) {
    return False;
  }

  *rspexpected = True; /* XnTcp or CS will confirm every command? */

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

  if( data->socket != NULL && MutexOp.wait( data->serialmux ) ) {
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
    rc = SocketOp.write( data->socket, out, len );
  }
  return rc;
}
