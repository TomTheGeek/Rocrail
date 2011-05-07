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
#include "rocrail/wrapper/public/LocoNet.h"
#include "rocdigs/impl/loconet/lnconst.h"

static void __reader( void* threadinst ) {
  iOThread      th      = (iOThread)threadinst;
  iOLocoNet     loconet = (iOLocoNet)ThreadOp.getParm( th );
  iOLocoNetData data    = Data(loconet);
  Boolean       seqStarted = False;
  char ln[0x7F];


  TraceOp.trc( "lbudp", TRCLEVEL_INFO, __LINE__, 9999, "LocoNet UDP reader started." );

  do {
    byte packet[0x7F];
    MemOp.set( packet, 0, 0x7F);

    int packetSize = SocketOp.recvfrom( data->readUDP, packet, 0x7F, NULL, NULL );

    if( packetSize > 0 ) {
      if( data->usedouble && MemOp.cmp( data->prevPacket, packet, packetSize ) ) {
        /* reject double packet */
        MemOp.set(data->prevPacket, 0, 0x7F );
        data->expectdouble = False;
      }
      else {
        byte* p = allocMem(0x7F+1);
        MemOp.copy( data->prevPacket, packet, packetSize );
        if( data->usedouble && data->expectdouble ) {
          data->packetloss++;
          TraceOp.trc( "lbudp", TRCLEVEL_WARNING, __LINE__, 9999, "packet loss [0x%02X] of %d total losses", data->prevPacket[0], data->packetloss );
        }
        data->expectdouble = True;

        if( data->useseq ) {
          byte inseq = packet[0];

          if( seqStarted ) {
            if( data->inseq + 1 != packet[0] ) {
              /* packet lost! */
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
                  "packet loss detected: expected seq=%d, received seq=%d", data->inseq + 1, packet[0] );
              {
                byte* bcmd = allocMem(32);
                bcmd[0] = OPC_GPOFF;
                bcmd[1] = LocoNetOp.checksum( bcmd, 1 );
                lbUDPWrite((obj)loconet, bcmd, 2);
              }
            }
          }
          else {
            seqStarted = True;
          }

          data->inseq = packet[0];
          p[0] = packetSize - 1;
          MemOp.copy( p+1, packet+1, packetSize - 1);
        }
        else {
          p[0] = packetSize;
          MemOp.copy( p+1, packet, packetSize);
        }

        QueueOp.post( data->udpQueue, (obj)p, normal);
        TraceOp.dump ( "lbudp", TRCLEVEL_BYTE, (char*)packet, packetSize );
      }
    }
    else {
      TraceOp.trc( "lbudp", TRCLEVEL_WARNING, __LINE__, 9999, "unexpected packet size %d received" );
      ThreadOp.sleep(10);
    }

  } while( data->run );

  TraceOp.trc( "lbudp", TRCLEVEL_INFO, __LINE__, 9999, "LocoNet UDP reader stopped." );
}




Boolean lbUDPConnect( obj inst ) {
  iOLocoNetData data = Data(inst);
  iONode loconet = wDigInt.getloconet( data->ini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "multicast address [%s]", wDigInt.gethost( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "multicast port    [%d]", wDigInt.getport( data->ini )  );
  if( wDigInt.getlocalip( data->ini ) != NULL && StrOp.len(wDigInt.getlocalip( data->ini )) > 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "local interface address [%s]", wDigInt.getlocalip( data->ini )  );
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->udpQueue  = QueueOp.inst(1000);
  data->useseq    = loconet != NULL ? wLocoNet.isuseseq(loconet):False;
  data->usedouble = loconet != NULL ? wLocoNet.isusedouble(loconet):False;

  data->readUDP = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getport(data->ini), False, True, True );
  if( wDigInt.getlocalip( data->ini ) != NULL && StrOp.len(wDigInt.getlocalip( data->ini )) > 0 ) {
    SocketOp.setLocalIP(data->readUDP, wDigInt.getlocalip( data->ini ));
  }
  SocketOp.bind(data->readUDP);
  data->writeUDP = SocketOp.inst( wDigInt.gethost(data->ini), wDigInt.getport(data->ini), False, True, True );
  if( wDigInt.getlocalip( data->ini ) != NULL && StrOp.len(wDigInt.getlocalip( data->ini )) > 0 ) {
    SocketOp.setLocalIP(data->writeUDP, wDigInt.getlocalip( data->ini ));
  }

  data->udpReader = ThreadOp.inst( "lnudpreader", &__reader, inst );
  ThreadOp.start( data->udpReader );

  return True;
}

void  lbUDPDisconnect( obj inst ) {
  iOLocoNetData data = Data(inst);
}

int lbUDPRead ( obj inst, unsigned char *msg ) {
  iOLocoNetData data = Data(inst);
  if( !QueueOp.isEmpty(data->udpQueue) ) {
    byte* p = (byte*)QueueOp.get(data->udpQueue);
    int size = p[0];
    MemOp.copy( msg, &p[1], size );
    freeMem(p);
    return size;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "could not read queue %d", QueueOp.count(data->udpQueue)  );
  }
  return 0;
}

Boolean lbUDPWrite( obj inst, unsigned char *msg, int len ) {
  iOLocoNetData data = Data(inst);
  byte out[256];
  if( data->useseq ) {
    out[0] = data->outseq;
    data->outseq++;
    MemOp.copy( out+1, msg, len);
    if( data->usedouble ) {
      Boolean rc = SocketOp.sendto( data->writeUDP, msg, len+1, NULL, 0 );
      ThreadOp.sleep(1);
    }
    return SocketOp.sendto( data->writeUDP, out, len+1, NULL, 0 );
  }
  else {
    if( data->usedouble ) {
      Boolean rc = SocketOp.sendto( data->writeUDP, msg, len, NULL, 0 );
      ThreadOp.sleep(1);
    }
    return SocketOp.sendto( data->writeUDP, msg, len, NULL, 0 );
  }
}

Boolean lbUDPAvailable( obj inst ) {
  iOLocoNetData data = Data(inst);
  return !QueueOp.isEmpty(data->udpQueue);
}
