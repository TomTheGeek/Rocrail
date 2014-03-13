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
#include "rocdigs/impl/zimocan_impl.h"
#include "rocdigs/impl/zimocan/udp.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/thread.h"
#include "rocs/public/serial.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"



Boolean UDPConnect( obj inst ) {
  iOZimoCANData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "host    = %s", wDigInt.gethost( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "port    = %d", wDigInt.getport( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  data->socket = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getport( data->ini ), False, True, False );
  if( SocketOp.bind(data->socket) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "socket binded" );
    data->connOK = True;
  }
  return True;
}


void UDPDisconnect( obj inst ) {
  iOZimoCANData data = Data(inst);
}


int UDPRead ( obj inst, unsigned char *msg ) {
  iOZimoCANData data = Data(inst);
  char buffer[256];
  int size = SocketOp.recvfrom( data->socket, buffer, 256, NULL, NULL );;
  if( size > 0 ) {
    MemOp.copy( msg, buffer, size );
    TraceOp.dump ( "zcanRead", TRCLEVEL_BYTE, (char*)msg, size );
    return size;
  }
  return 0;
}


Boolean UDPWrite( obj inst, unsigned char *msg, int len ) {
  iOZimoCANData data = Data(inst);
  TraceOp.dump ( "zcanWrite", TRCLEVEL_BYTE, (char*)msg, len );
  data->connOK = SocketOp.sendto( data->socket, (char*)msg, len, NULL, 0 );
  return data->connOK;
}


Boolean UDPAvailable( obj inst ) {
  iOZimoCANData data = Data(inst);
  int packetSize = SocketOp.recvfrom( data->socket, NULL, 256, NULL, NULL );
  return packetSize > 0 ? True:False;
}

