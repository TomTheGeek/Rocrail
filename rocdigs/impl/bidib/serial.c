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

#include "rocdigs/impl/bidib_impl.h"

#include "rocdigs/impl/bidib/serial.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/thread.h"
#include "rocs/public/serial.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"

#include "rocdigs/impl/bidib/bidib.h"



static void __writer( void* threadinst ) {
  iOThread    th    = (iOThread)threadinst;
  iOBiDiB     bidib = (iOBiDiB)ThreadOp.getParm( th );
  iOBiDiBData data  = Data(bidib);
  char msg[256];

  TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "BIDIB sub writer started." );

  do {
    ThreadOp.sleep(10);
  } while( data->run );

  TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "BIDIB sub writer stopped." );
}


static void __reader( void* threadinst ) {
  iOThread    th    = (iOThread)threadinst;
  iOBiDiB     bidib = (iOBiDiB)ThreadOp.getParm( th );
  iOBiDiBData data  = Data(bidib);
  byte msg[256];
  byte c;
  int index = 0;

  TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "BIDIB sub reader started." );

  do {
    int available = SerialOp.available(data->serial);
    if( available > 0 ) {

      if(SerialOp.read(data->serial, &c, 1) ) {
        TraceOp.trc( "bidib", TRCLEVEL_DEBUG, __LINE__, 9999, "byte read: 0x%02X", c );

        if( c == BIDIB_PKT_MAGIC ) {
          if( index > 0 ) {
            byte* p = allocMem(index+1);
            p[0] = index;
            MemOp.copy( p+1, msg, index);
            QueueOp.post( data->subReadQueue, (obj)p, normal);
            TraceOp.dump ( "bidibserial", TRCLEVEL_BYTE, (char*)msg, index );
            index = 0;
          }
        }
        else {
          msg[index] = c;
          index++;
          TraceOp.dump ( "bidibserial", TRCLEVEL_DEBUG, (char*)msg, index );
        }
      }

    }
    else if( available == -1 ) {
      /* device error */
      data->run = False;
      TraceOp.trc( "bidibserial", TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error" );
    }

    ThreadOp.sleep(10);
  } while( data->run );

  TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "BIDIB sub reader stopped." );
}


Boolean serialConnect( obj inst ) {
  iOBiDiBData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = %s", wDigInt.getdevice( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps     = %d", wDigInt.getbps( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "line    = 8N1 (fix)" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "flow    = CTS (fix)" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout = %d", wDigInt.gettimeout( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
  SerialOp.setFlow( data->serial, cts );
  SerialOp.setLine( data->serial, wDigInt.getbps( data->ini ), 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );

  if( SerialOp.open( data->serial ) ) {
    data->subReadQueue  = QueueOp.inst(1000);
    data->subWriteQueue = QueueOp.inst(1000);
    data->subReader = ThreadOp.inst( "bidibsubreader", &__reader, inst );
    ThreadOp.start( data->subReader );
    data->subWriter = ThreadOp.inst( "bidibsubwriter", &__writer, inst );
    ThreadOp.start( data->subWriter );
    return True;
  }

  return False;
}


void serialDisconnect( obj inst ) {
  iOBiDiBData data = Data(inst);

  if( data->serial != NULL ) {
    data->commOK = False;
    SerialOp.close( data->serial );
    SerialOp.base.del( data->serial );
    data->serial = NULL;
  }
}


int serialRead ( obj inst, unsigned char *msg ) {
  iOBiDiBData data = Data(inst);

  if( !QueueOp.isEmpty(data->subReadQueue) ) {
    byte* p = (byte*)QueueOp.get(data->subReadQueue);
    int size = p[0];
    MemOp.copy( msg, &p[1], size );
    freeMem(p);
    return size;
  }
  else {
    TraceOp.trc( "bidibserial", TRCLEVEL_DEBUG, __LINE__, 9999, "could not read queue %d", QueueOp.count(data->subReadQueue)  );
  }
  return 0;
}


Boolean serialWrite( obj inst, unsigned char *msg, int len ) {
  iOBiDiBData data = Data(inst);

  TraceOp.dump ( "bidibserial", TRCLEVEL_BYTE, (char*)msg, len );
  Boolean ok = SerialOp.write( data->serial, (char*)msg, len );

  return ok;
}


Boolean serialAvailable( obj inst ) {
  iOBiDiBData data = Data(inst);
  return !QueueOp.isEmpty(data->subReadQueue);
}

