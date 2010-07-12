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
    byte packet[0x7F];

    int packetSize = SocketOp.recvfrom( data->readUDP, packet, 0x7F );

    if( MutexOp.wait( data->udpmux ) ) {
      byte* p = allocMem(0x7F+1);
      p[0] = packetSize;
      MemOp.copy( p+1, packet, 0x7F);
      QueueOp.post( data->udpQueue, (obj)p, normal);
      MutexOp.post( data->udpmux );
      TraceOp.dump ( "lbudp", TRCLEVEL_BYTE, (char*)packet, packetSize );
      ThreadOp.sleep(0);
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

  data->udpmux = MutexOp.inst(NULL, True);
  data->udpQueue = QueueOp.inst(1000);

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
  if( !QueueOp.isEmpty(data->udpQueue) && MutexOp.trywait( data->udpmux, 100 ) ) {
    byte* p = (byte*)QueueOp.get(data->udpQueue);
    int size = p[0];
    MemOp.copy( msg, &p[1], size );
    freeMem(p);
    MutexOp.post( data->udpmux );
    return size;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "could not read queue %d", QueueOp.count(data->udpQueue)  );
  }
  return 0;
}

Boolean lbUDPWrite( obj inst, unsigned char *msg, int len ) {
  iOLocoNetData data = Data(inst);
  return SocketOp.sendto( data->writeUDP, msg, len );
}

Boolean lbUDPAvailable( obj inst ) {
  iOLocoNetData data = Data(inst);
  return !QueueOp.isEmpty(data->udpQueue);
}
