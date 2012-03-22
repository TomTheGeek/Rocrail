/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2011 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocdigs/impl/cbus_impl.h"

#include "rocdigs/impl/cbus/tcp.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/thread.h"
#include "rocs/public/serial.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"





Boolean tcpConnect( obj inst ) {
  iOCBUSData data = Data(inst);


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "IP address [%s]", wDigInt.gethost( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "IP port    [%d]", wDigInt.getport( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  TraceOp.trc( "cbustcp", TRCLEVEL_WARNING, __LINE__, 9999, "trying to connect to %s:%d...", wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );
  data->socket = SocketOp.inst( wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ), False, False, False );
  if( data->socket != NULL ) {
    SocketOp.setNodelay(data->socket, True);
    if ( SocketOp.connect( data->socket ) ) {
      TraceOp.trc( "cbustcp", TRCLEVEL_INFO, __LINE__, 9999, "connected to %s:%d", wDigInt.gethost( data->ini ), wDigInt.getport( data->ini )  );
      return True;
    }
  }
  if( data->socket != NULL ) {
    SocketOp.base.del( data->socket );
    data->socket = NULL;
  }

  return False;
}


void tcpDisconnect( obj inst ) {
  iOCBUSData data = Data(inst);

  if( data->socket != NULL ) {
    TraceOp.trc( "cbustcp", TRCLEVEL_INFO, __LINE__, 9999, "disconnecting..." );
    iOSocket socket = data->socket;
    data->socket = NULL;
    SocketOp.disConnect( socket );
    SocketOp.base.del( socket );
  }
}

Boolean tcpRead ( obj inst, unsigned char *frame, int len ) {
  iOCBUSData data = Data(inst);

  if( data->socket == NULL ) {
    return False;
  }
  
  if( data->socket == NULL ||  SocketOp.isBroken(data->socket) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "problem reading CBUS: Disconnect" );
    tcpDisconnect(inst);
    ThreadOp.sleep(1000);
    return False;
  }

  if( SocketOp.read( data->socket, frame, len ) ) {
    return True;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "problem reading CBUS: Disconnect" );
    tcpDisconnect(inst);
    ThreadOp.sleep(1000);
  }
  return False;
}


Boolean tcpWrite( obj inst, unsigned char *frame, int len ) {
  iOCBUSData data = Data(inst);
  TraceOp.dump ( "cbustcp", TRCLEVEL_BYTE, (char*)frame, len );
  if( data->socket != NULL )
    return SocketOp.write( data->socket, frame, len );
  else
    return False;
}


Boolean tcpAvailable( obj inst ) {
  iOCBUSData data = Data(inst);
  char msgStr[32];
  if( data->socket == NULL || SocketOp.isBroken(data->socket) ) {
    tcpDisconnect(inst);
    tcpConnect(inst);
    return False;
  }
  return SocketOp.peek( data->socket, msgStr, 1 );
}

