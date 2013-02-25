/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

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

#include "rocdigs/impl/bidib_impl.h"

#include "rocdigs/impl/bidib/udp.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/thread.h"
#include "rocs/public/serial.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"

#include "rocdigs/impl/bidib/bidib_messages.h"
#include "rocdigs/impl/bidib/bidibutils.h"


Boolean udpInit( obj inst ) {
  iOBiDiBData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "host    = %s", wDigInt.gethost( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "port    = %d", wDigInt.getport( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->rwUDP = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getport(data->ini), False, True, False );
  if( SocketOp.bind(data->rwUDP) ) {
    data->commOK = True;
  }

  return True;
}


Boolean udpConnect( obj inst ) {
  iOBiDiBData data = Data(inst);
  return False;
}


void udpDisconnect( obj inst ) {
  iOBiDiBData data = Data(inst);
  data->commOK = False;
}


int udpRead ( obj inst, unsigned char *msg ) {
  iOBiDiBData data = Data(inst);
  char buffer[256];
  int size = SocketOp.recvfrom( data->rwUDP, buffer, 256, NULL, NULL );;
  if( size > 0 ) {
    MemOp.copy( msg, buffer, size );
    size = bidibDeEscapeMessage(msg, size);
    TraceOp.dump ( "bidibRead", TRCLEVEL_BYTE, (char*)msg, size );
    byte crc = bidibCheckSum(msg, size );
    if( crc != 0 ) {
      TraceOp.trc( "bidibserial", TRCLEVEL_EXCEPTION, __LINE__, 9999, "invalid checksum" );
      return 0;
    }
    return size;
  }
  return 0;
}


Boolean udpWrite( obj inst, unsigned char *path, unsigned char code, unsigned char* pdata, int datalen, int seq ) {
  iOBiDiBData data = Data(inst);
  int   size = 0;
  byte  msg[256];

  int msgidx  = 0;
  int dataidx = 0;

  /*                  pathidx: 0  1  2  3
   * Addr can be 4 bytes long. XX XX XX XX */
  for( msgidx = 0; msgidx < 4; msgidx++ ) {
    msg[1+msgidx] = path[msgidx]; // address
    if( msg[1+msgidx] == 0 )
      break;
  }

  msgidx += 2; // point to sequence offset

  msg[msgidx] = seq;
  msgidx++;

  msg[msgidx] = code;
  msgidx++;

  if( pdata != NULL ) {
    for( dataidx = 0; dataidx < datalen; dataidx++ )
      msg[msgidx+dataidx] = pdata[dataidx]; // address
  }

  size = msgidx+dataidx;
  msg[0] = size;

  TraceOp.dump ( "preWrite", TRCLEVEL_BYTE, (char*)msg, size );
  size = bidibMakeMessage(msg, size);

  data->commOK = SocketOp.sendto( data->rwUDP, msg, size, NULL, 0 );
  return data->commOK;
}


Boolean udpAvailable( obj inst ) {
  iOBiDiBData data = Data(inst);
  int packetSize = SocketOp.recvfrom( data->rwUDP, NULL, 256, NULL, NULL );
  return packetSize > 0 ? True:False;
}

