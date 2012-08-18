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
#include "rocdigs/impl/xpressnet/lieth.h"
#include "rocdigs/impl/xpressnet/li101.h"
#include "rocrail/wrapper/public/DigInt.h"

Boolean liethConnectInternal(obj xpressnet);
Boolean liethConnect(obj xpressnet);

static void __timeoutwd( void* threadinst ) {
  iOThread        th = (iOThread)threadinst;
  iOXpressNet     xpressnet = (iOXpressNet)ThreadOp.getParm(th);
  iOXpressNetData data = Data(xpressnet);

  int retry = 0;

  TraceOp.trc( "lieth", TRCLEVEL_INFO, __LINE__, 9999, "timeout watchdog started" );

  while( data->run ) {
    if( data->socket == NULL ) {
      TraceOp.trc( "lieth", TRCLEVEL_DEBUG, __LINE__, 9999, "connecting..." );
      if( !data->connectpending && retry < 60 && !liethConnectInternal((obj)xpressnet) ) {
        retry++;
      }
      else {
        retry = 0;
      }
    }
    else {
      long t = time(NULL);
      if( t - data->lastcmd >= 30 ) {
        /* send a packet: (0xFF 0xFE) 0xF1 0x01 (0xF0) */
        byte* outa = allocMem(32);
        outa[0] = 0xF1;
        outa[1] = 0x01;
        TraceOp.trc( "lieth", TRCLEVEL_INFO, __LINE__, 9999, "posting a keep alive packet" );
        ThreadOp.post( data->transactor, (obj)outa );
      }
      retry = 0;
    }
    ThreadOp.sleep(1000);
  }

  TraceOp.trc( "lieth", TRCLEVEL_INFO, __LINE__, 9999, "timeout watchdog ended" );
}


static void __availwd( void* threadinst ) {
  iOThread        th = (iOThread)threadinst;
  iOXpressNet     xpressnet = (iOXpressNet)ThreadOp.getParm(th);
  iOXpressNetData data = Data(xpressnet);

  ThreadOp.sleep(10);
  TraceOp.trc( "lieth", TRCLEVEL_INFO, __LINE__, 9999, "available watchdog started" );

  while( data->run ) {
    if( MutexOp.wait( data->serialmux ) ) {
      if( !data->availFlag && data->socket != NULL && !SocketOp.isBroken(data->socket)) {
        char buffer[2];
        data->availFlag = SocketOp.read( data->socket, buffer, 2 );
      }
      MutexOp.post( data->serialmux );
      if( data->socket != NULL && SocketOp.isBroken(data->socket)) {
        TraceOp.trc( "lieth", TRCLEVEL_EXCEPTION, __LINE__, 9999, "problem reading LiEth: Disconnect" );
        liethDisConnect((obj)xpressnet);
        ThreadOp.sleep(1000);
      }
    }
    ThreadOp.sleep(10);
  }

  TraceOp.trc( "lieth", TRCLEVEL_INFO, __LINE__, 9999, "available watchdog ended" );
}

Boolean liethConnect( obj xpressnet ) {
  iOXpressNetData data = Data(xpressnet);
  data->connectpending = False;
  data->availFlag = False;
  data->socket = NULL;
  if( data->timeOutWD == NULL ) {
    data->timeOutWD = ThreadOp.inst( "timeoutwd", &__timeoutwd, xpressnet );
    ThreadOp.start( data->timeOutWD );
    ThreadOp.sleep(100); /* time to connect... */
  }
  if( data->availWD == NULL ) {
    data->availWD = ThreadOp.inst( "availwd", &__availwd, xpressnet );
    ThreadOp.start( data->availWD );
  }
  return True;
}

Boolean liethConnectInternal(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  iOSocket socket = NULL;

  data->connectpending = True;

  if( wDigInt.gethost( data->ini ) == NULL || StrOp.len(wDigInt.gethost( data->ini )) == 0 ) {
    wDigInt.sethost( data->ini, "192.168.0.200" );
  }
  if( wDigInt.getport( data->ini ) == 0 ) {
    wDigInt.setport( data->ini, 5550 );
  }

  TraceOp.trc( "lieth", TRCLEVEL_INFO, __LINE__, 9999, "LI-ETH at %s:%d",
      wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );

  socket = SocketOp.inst( wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ), False, False, False );
  /*SocketOp.setRcvTimeout( socket, wDigInt.gettimeout(data->ini) / 1000);*/
  SocketOp.setNodelay( socket, True );
  SocketOp.setBlocking( socket, True );

  TraceOp.trc( "lieth", TRCLEVEL_INFO, __LINE__, 9999, "trying to connect to %s:%d...",
      wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );
  if ( SocketOp.connect( socket ) ) {
    data->socket = socket;
    data->connectpending = False;
    TraceOp.trc( "lieth", TRCLEVEL_INFO, __LINE__, 9999, "connected to %s:%d",
        wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );
    return True;
  }
  else {
    TraceOp.trc( "lieth", TRCLEVEL_WARNING, __LINE__, 9999, "unable to connect to %s:%d; check the network...",
        wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );
    if( socket != NULL ) {
      SocketOp.base.del( socket );
      socket = NULL;
    }
    data->connectpending = False;
    return False;
  }
}

void liethDisConnect(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  if( data->socket != NULL && MutexOp.wait( data->serialmux ) ) {
    iOSocket socket = data->socket;
    TraceOp.trc( "lieth", TRCLEVEL_INFO, __LINE__, 9999, "disconnecting..." );
    data->socket = NULL;
    SocketOp.disConnect( socket );
    SocketOp.base.del( socket );
    MutexOp.post( data->serialmux );
  }
}

Boolean liethAvail(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  return data->availFlag;
}

void liethInit(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  li101Init(xpressnet);
  data->availFlag = False;
}

int liethRead(obj xpressnet, byte* buffer, Boolean* rspreceived) {
  iOXpressNetData data = Data(xpressnet);
  int len = 0;

  if( data->socket == NULL ) {
    return 0;
  }

  if( data->socket != NULL && !SocketOp.isBroken(data->socket) && MutexOp.wait( data->serialmux ) ) {
    if( data->availFlag || SocketOp.read( data->socket, buffer, 2 ) ) {
      SocketOp.read( data->socket, buffer, 1 );
      len = (buffer[0] & 0x0F) + 1;
      if( SocketOp.read( data->socket, buffer+1, len ) ) {
        TraceOp.trc( "lieth", TRCLEVEL_BYTE, __LINE__, 9999, "read from LI-ETH" );
        TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)buffer, len+1 );
      }
      MutexOp.post( data->serialmux );
    }
    else {
      TraceOp.trc( "lieth", TRCLEVEL_EXCEPTION, __LINE__, 9999, "problem reading XpressNet: Disconnect" );
      MutexOp.post( data->serialmux );
      liethDisConnect(xpressnet);
      ThreadOp.sleep(1000);
    }
    data->availFlag = False;
  }

  return len;

}

Boolean liethWrite(obj xpressnet, byte* outin, Boolean* rspexpected) {
  iOXpressNetData data = Data(xpressnet);

  int len = 0;
  Boolean ok = False;
  unsigned char out[256];

  *rspexpected = False;
  if( data->socket == NULL ) {
    return 0;
  }

  ThreadOp.sleep( 50 );

  len = makeChecksum(outin);

  if( len == 0 ) {
    TraceOp.trc( "lieth", TRCLEVEL_WARNING, __LINE__, 9999, "zero bytes to write LI-ETH" );
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

  if( data->socket != NULL && !SocketOp.isBroken(data->socket) ) {
    TraceOp.trc( "lieth", TRCLEVEL_BYTE, __LINE__, 9999, "write to LI-ETH" );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
    ok = SocketOp.write( data->socket, out, len );
    data->lastcmd = time(NULL);
    if( ok ) {
      *rspexpected = True; /* LIUSB/ETH or CS will confirm every command */
    }
    else {
      TraceOp.trc( "lieth", TRCLEVEL_WARNING, __LINE__, 9999, "could not write to XpressNet" );
    }
  }

  return ok;
}

