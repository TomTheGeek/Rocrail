/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
#include "rocdigs/impl/loconet_impl.h"

#include "rocdigs/impl/loconet/lbudp.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/DigInt.h"


static void __reader( void* threadinst ) {
  iOThread      th      = (iOThread)threadinst;
  iOLocoNet     loconet = (iOLocoNet)ThreadOp.getParm( th );
  iOLocoNetData data    = Data(loconet);
  char ln[0x7F];

  TraceOp.trc( "lbudp", TRCLEVEL_INFO, __LINE__, 9999, "LocoNet UDP reader started." );

  do {
    if( !data->udpPacketAvailable ) {
      data->udpPacketSize = SocketOp.recvfrom( data->readUDP, data->udpPacket, 0x7F );
      data->udpPacketAvailable = True;
      TraceOp.dump ( "lbudp", TRCLEVEL_BYTE, (char*)data->udpPacket, data->udpPacketSize );
    }
    else {
      ThreadOp.sleep(10);
    }

  } while( data->run );

  TraceOp.trc( "lbudp", TRCLEVEL_INFO, __LINE__, 9999, "LocoNet UDP reader stopped." );
}




Boolean lbUDPConnect( obj inst ) {
  iOLocoNetData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "multicast address [%s]", wDigInt.gethost( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "multicast port    [%d]", wDigInt.getport( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->udpPacketAvailable = False;

  data->readUDP = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getport(data->ini), False, True, True );
  SocketOp.bind(data->readUDP);
  data->writeUDP = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getport(data->ini), False, True, True );

  data->udpReader = ThreadOp.inst( "lnudpreader", &__reader, inst );
  ThreadOp.start( data->udpReader );

  return True;
}

void  lbUDPDisconnect( obj inst ) {
  iOLocoNetData data = Data(inst);
}

int lbUDPRead ( obj inst, unsigned char *msg ) {
  iOLocoNetData data = Data(inst);
  if( data->udpPacketAvailable ) {
    MemOp.copy( msg, data->udpPacket, data->udpPacketSize );
    data->udpPacketAvailable = False;
    return data->udpPacketSize;
  }
  return 0;
}

Boolean lbUDPWrite( obj inst, unsigned char *msg, int len ) {
  iOLocoNetData data = Data(inst);
  return SocketOp.sendto( data->writeUDP, msg, len );
}

Boolean lbUDPAvailable( obj inst ) {
  iOLocoNetData data = Data(inst);
  return data->udpPacketAvailable;
}
