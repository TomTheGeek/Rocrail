/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2010 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocdigs/impl/loconet/lbtcp.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/DigInt.h"


static Boolean lbTCPReConnect( iOLocoNet inst ) {
  iOLocoNetData data = Data(inst);
  iOSocket s = data->rwTCP;

  if( data->rwTCP != NULL ) {
    TraceOp.trc( "lbtcp", TRCLEVEL_WARNING, __LINE__, 9999, "reconnecting to %s:%d...", wDigInt.gethost( data->ini ), wDigInt.getport( data->ini )  );

    data->comm = False;
    ThreadOp.sleep(100);
    data->rwTCP = NULL;
    SocketOp.base.del( s );
  }
  LocoNetOp.stateChanged(inst);

  if( data->rwTCP == NULL ) {
    while( data->run ) {
      TraceOp.trc( "lbtcp", TRCLEVEL_WARNING, __LINE__, 9999, "trying to connect to %s:%d...", wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ) );
      data->rwTCP = SocketOp.inst( wDigInt.gethost( data->ini ), wDigInt.getport( data->ini ), False, False, False );
      if( data->rwTCP != NULL ) {
        SocketOp.setNodelay(data->rwTCP, True);
        if ( SocketOp.connect( data->rwTCP ) ) {
          data->comm = True;
          TraceOp.trc( "lbtcp", TRCLEVEL_INFO, __LINE__, 9999, "connected to %s:%d", wDigInt.gethost( data->ini ), wDigInt.getport( data->ini )  );
          LocoNetOp.stateChanged(inst);
          return True;
        }
      }
      if( data->rwTCP != NULL ) {
        SocketOp.base.del( data->rwTCP );
        data->rwTCP = NULL;
      }
      ThreadOp.sleep(1000);
    }
  }

	return False;
}


static void __writer( void* threadinst ) {
  iOThread      th      = (iOThread)threadinst;
  iOLocoNet     loconet = (iOLocoNet)ThreadOp.getParm( th );
  iOLocoNetData data    = Data(loconet);

  TraceOp.trc( "lbtcpw", TRCLEVEL_INFO, __LINE__, 9999, "LocoNet TCP writer started." );

  while( data->run ) {
    Boolean ok = False;
    if( data->rwTCP != NULL && data->comm ) {
      byte* post = (byte*)ThreadOp.getPost( th );
      if (post != NULL) {
        byte out[128];
        int outlen = post[0];
        MemOp.copy(out, post+1, outlen);
        freeMem( post);

        TraceOp.dump ( "lbtcpw", TRCLEVEL_BYTE, (char*)out, outlen );
        ok = SocketOp.write( data->rwTCP, out, outlen );
      }
    }
    ThreadOp.sleep(10);
  }

  TraceOp.trc( "lbtcpw", TRCLEVEL_INFO, __LINE__, 9999, "LocoNet TCP writer stopped." );
}


static void __reader( void* threadinst ) {
  iOThread      th      = (iOThread)threadinst;
  iOLocoNet     loconet = (iOLocoNet)ThreadOp.getParm( th );
  iOLocoNetData data    = Data(loconet);
  char ln[0x7F];

  TraceOp.trc( "lbtcpr", TRCLEVEL_INFO, __LINE__, 9999, "LocoNet TCP reader started." );
  
  data->comm = lbTCPReConnect(loconet);

  while( data->run  && data->rwTCP != NULL && data->comm ) {
    byte msg[0x7F];

    int  msglen = 0;
    int   index = 0;
    int garbage = 0;
    byte bucket[128];
    byte c;
    Boolean  ok = False;


    do {

      ok = SocketOp.read( data->rwTCP, &c, 1 );
      if(ok && c < 0x80) {
        ThreadOp.sleep(10);
        bucket[garbage] = c;
        garbage++;
      }
    } while (ok && data->run && c < 0x80 && garbage < 128);

    if( garbage > 0 ) {
       TraceOp.trc( "lbtcpr", TRCLEVEL_INFO, __LINE__, 9999, "garbage=%d", garbage );
       TraceOp.dump ( "lbtcpr", TRCLEVEL_BYTE, (char*)bucket, garbage );
    }

    if( !ok && SocketOp.isBroken(data->rwTCP) ) {
      /* recover */
      data->comm = lbTCPReConnect(loconet);
      ThreadOp.sleep(data->comm?10:1000);
      continue;
    }

    if( ok ) {

      msg[0] = c;

      switch (c & 0xf0) {
      case 0x80:
          msglen = 2;
          index = 1;
          break;
      case 0xa0:
      case 0xb0:
          msglen = 4;
          index = 1;
          break;
      case 0xc0:
          msglen = 6;
          index = 1;
          break;
      case 0xe0:
        SocketOp.read( data->rwTCP, &c, 1);
          msg[1] = c;
          index = 2;
          msglen = c;
          break;
      default:
        TraceOp.trc( "lbtcpr", TRCLEVEL_WARNING, __LINE__, 9999, "undocumented message: start=0x%02X", msg[0] );
        msglen = 0;
        ThreadOp.sleep(10);
        continue;
      }

      TraceOp.trc( "lbtcpr", TRCLEVEL_DEBUG, __LINE__, 9999, "message 0x%02X length=%d", msg[0], msglen );

      ok = SocketOp.read( data->rwTCP, &msg[index], msglen - index);

      if( ok ) {
        if( MutexOp.trywait( data->udpmux, 10 ) ) {
          byte* p = allocMem(msglen+1);
          p[0] = msglen;
          MemOp.copy( p+1, msg, msglen);
          QueueOp.post( data->udpQueue, (obj)p, normal);
          MutexOp.post( data->udpmux );
          TraceOp.dump ( "lbtcp", TRCLEVEL_BYTE, (char*)msg, msglen );
        }
      }
      else {
        TraceOp.trc( "lbtcpr", TRCLEVEL_WARNING, __LINE__, 9999, "could not read rest of packet" );
        ThreadOp.sleep(10);
      }

    }

    ThreadOp.sleep(0);
  }

  TraceOp.trc( "lbtcpr", TRCLEVEL_INFO, __LINE__, 9999, "LocoNet TCP reader stopped." );
}




Boolean lbTCPConnect( obj inst ) {
  iOLocoNetData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "IP address [%s]", wDigInt.gethost( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "IP port    [%d]", wDigInt.getport( data->ini )  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->udpmux = MutexOp.inst(NULL, True);
  data->udpQueue = QueueOp.inst(1000);

  data->subReader = ThreadOp.inst( "lntcpr", &__reader, inst );
  ThreadOp.start( data->subReader );
  data->subWriter = ThreadOp.inst( "lntcpw", &__writer, inst );
  ThreadOp.start( data->subWriter );

  return True;
}

void  lbTCPDisconnect( obj inst ) {
  iOLocoNetData data = Data(inst);
  if( data->comm && data->rwTCP != NULL ) {
    TraceOp.trc( "lbtcp", TRCLEVEL_INFO, __LINE__, 9999, "disconnecting..." );
    data->run = False;
    data->comm = False;
    ThreadOp.sleep(100);
    SocketOp.disConnect( data->rwTCP );
    ThreadOp.sleep(100);
    SocketOp.base.del( data->rwTCP );
    data->rwTCP = NULL;
  }
}

int lbTCPRead ( obj inst, unsigned char *msg ) {
  iOLocoNetData data = Data(inst);
  if( !QueueOp.isEmpty(data->udpQueue) && MutexOp.trywait( data->udpmux, 10 ) ) {
    byte* p = (byte*)QueueOp.get(data->udpQueue);
    int size = p[0];
    MemOp.copy( msg, &p[1], size );
    freeMem(p);
    MutexOp.post( data->udpmux );
    return size;
  }
  else {
    TraceOp.trc( "lbtcp", TRCLEVEL_DEBUG, __LINE__, 9999, "could not read queue %d", QueueOp.count(data->udpQueue)  );
  }
  return 0;
}

Boolean lbTCPWrite( obj inst, unsigned char *msg, int len ) {
  iOLocoNetData data = Data(inst);

  /* put packet in the write queue */
  if( data->subWriter != NULL ) {
    byte* p = allocMem(len+1);
    p[0] = len;
    MemOp.copy( p+1, msg, len);
    return ThreadOp.post( data->subWriter, (obj)p);
  }

  return False;
}

Boolean lbTCPAvailable( obj inst ) {
  iOLocoNetData data = Data(inst);
  return !QueueOp.isEmpty(data->udpQueue);
}
