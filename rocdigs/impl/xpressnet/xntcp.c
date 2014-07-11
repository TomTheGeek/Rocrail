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

#include "rocdigs/impl/xpressnet_impl.h"
#include "rocdigs/impl/xpressnet/xntcp.h"
#include "rocdigs/impl/xpressnet/li101.h"

#include "rocrail/wrapper/public/DigInt.h"


static void __availwd( void* threadinst ) {
  iOThread        th = (iOThread)threadinst;
  iOXpressNet     xpressnet = (iOXpressNet)ThreadOp.getParm(th);
  iOXpressNetData data = Data(xpressnet);

  ThreadOp.sleep(10);
  TraceOp.trc( "xntcp", TRCLEVEL_INFO, __LINE__, 9999, "available watchdog started" );

  while( data->run ) {
    if( MutexOp.wait( data->serialmux ) ) {
      if( data->socket == NULL ) {
        xntcpConnect((obj)xpressnet);
      }
      if( !data->availFlag && data->socket != NULL && !SocketOp.isBroken(data->socket)) {
        data->startbyte = 0;
        data->availFlag = SocketOp.read( data->socket, (char*)&data->startbyte, 1 );
      }
      MutexOp.post( data->serialmux );
      if( data->socket != NULL && SocketOp.isBroken(data->socket)) {
        TraceOp.trc( "xntcp", TRCLEVEL_EXCEPTION, __LINE__, 9999, "problem reading XnTcp: Disconnect" );
        xntcpDisConnect((obj)xpressnet);
        ThreadOp.sleep(1000);
      }
    }
    ThreadOp.sleep(10);
  }

  TraceOp.trc( "xntcp", TRCLEVEL_INFO, __LINE__, 9999, "available watchdog ended" );
}




Boolean xntcpConnect(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);

  if( !data->enablecom ) {
    return False;
  }

  TraceOp.trc( "xntcp", TRCLEVEL_INFO, __LINE__, 9999, "XnTcp at %s:%d",
      wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );

  data->socket = SocketOp.inst( wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ), False, False, False );

  if( data->socket != NULL ) {
    SocketOp.setNodelay(data->socket, True);
    SocketOp.setBlocking( data->socket, True );
  }
  else {
    return False;
  }

  if ( SocketOp.connect( data->socket ) ) {
    if( data->availWD == NULL ) {
      data->availWD = ThreadOp.inst( "availwd", &__availwd, xpressnet );
      ThreadOp.start( data->availWD );
    }
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
  if( data->socket != NULL && MutexOp.wait( data->serialmux ) ) {
    iOSocket socket = data->socket;
    TraceOp.trc( "xntcp", TRCLEVEL_INFO, __LINE__, 9999, "disconnecting..." );
    data->socket = NULL;
    SocketOp.disConnect( socket );
    SocketOp.base.del( socket );
    MutexOp.post( data->serialmux );
  }
}

Boolean xntcpAvail(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  return data->availFlag;
}

void xntcpInit(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  li101Init(xpressnet);
  data->availFlag = False;
}


int xntcpRead(obj xpressnet, byte* buffer, Boolean* rspreceived) {
  iOXpressNetData data = Data(xpressnet);
  int len = 0;

  if( !data->enablecom ) {
    return 0;
  }
  if( data->socket != NULL && !SocketOp.isBroken(data->socket) && MutexOp.wait( data->serialmux ) ) {
    if( data->availFlag || SocketOp.read( data->socket, (char*)&data->startbyte, 1 ) ) {
      buffer[0] = data->startbyte;
      len = (buffer[0] & 0x0F) + 1;
      if( SocketOp.read( data->socket, (char*)(buffer+1), len ) )
        TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)buffer, len+1 );
      else
        len = 0;
      data->startbyte = 0;
      data->availFlag = False;
    }
    MutexOp.post( data->serialmux );
  }
  return len;
}


Boolean xntcpWrite(obj xpressnet, byte* out, Boolean* rspexpected) {
  iOXpressNetData data = Data(xpressnet);

  int len = 0;
  int i = 0;
  Boolean rc = False;
  byte bXor = 0;
  
  if( !data->enablecom ) {
    return False;
  }
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

  if( data->socket != NULL ) {
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
    rc = SocketOp.write( data->socket, (char*)out, len );
  }
  return rc;
}
