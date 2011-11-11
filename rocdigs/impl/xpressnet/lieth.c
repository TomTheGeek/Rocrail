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
#include "rocdigs/impl/xpressnet/lieth.h"
#include "rocdigs/impl/xpressnet/li101.h"
#include "rocrail/wrapper/public/DigInt.h"


Boolean liethConnect(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  TraceOp.trc( "lieth", TRCLEVEL_INFO, __LINE__, 9999, "LI-ETH at %s:%d",
      wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );

  if( wDigInt.gethost( data->ini ) == NULL || StrOp.len(wDigInt.gethost( data->ini )) == 0 ) {
    wDigInt.sethost( data->ini, "192.168.0.200" );
  }
  if( wDigInt.getport( data->ini ) == 0 ) {
    wDigInt.setport( data->ini, 5550 );
  }

  data->socket = SocketOp.inst( wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ), False, False, False );
  SocketOp.setRcvTimeout( data->socket, wDigInt.gettimeout(data->ini) / 1000);

  if ( SocketOp.connect( data->socket ) ) {
    return True;
  }
  else {
    SocketOp.base.del( data->socket );
    data->socket = NULL;
    return False;
  }
}

void liethDisConnect(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  if( data->socket != NULL ) {
    SocketOp.disConnect( data->socket );
    SocketOp.base.del( data->socket );
    data->socket = NULL;
  }
}

Boolean liethAvail(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  char msgStr[32];
  if( SocketOp.isBroken(data->socket) ) {
    return False;
  }
  return SocketOp.peek( data->socket, msgStr, 1 );
}

void liethInit(obj xpressnet) {
  li101Init(xpressnet);
}

int liethRead(obj xpressnet, byte* buffer, Boolean* rspreceived) {
  iOXpressNetData data = Data(xpressnet);
  int len = 0;


  if( !SocketOp.isBroken(data->socket) && SocketOp.read( data->socket, buffer, 2 ) ) {
    SocketOp.read( data->socket, buffer, 1 );
    len = (buffer[0] & 0x0F) + 1;
    if( SocketOp.read( data->socket, buffer+1, len ) )
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)buffer, len+1 );
      return len;
  }
  return 0;
}

Boolean liethWrite(obj xpressnet, byte* outin, Boolean* rspexpected) {
  iOXpressNetData data = Data(xpressnet);

  ThreadOp.sleep( 50 );

  int len = 0;
  Boolean rc = False;
  unsigned char out[256];

  *rspexpected = True; /* LIUSB/ETH or CS will confirm every command */

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

  if( data->dummyio ) {
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
    *rspexpected = False;
    return True;
  }

  if( data->socket != NULL && MutexOp.wait( data->serialmux ) ) {
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
    rc = SocketOp.write( data->socket, out, len );
  }

  return rc;
}

