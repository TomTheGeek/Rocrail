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




Boolean serialConnect( obj inst ) {
  iOBiDiBData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = %s", wDigInt.getdevice( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps     = %d", wDigInt.getbps( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "line    = 8N1 (fix)" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout = %d", wDigInt.gettimeout( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
  SerialOp.setFlow( data->serial, cts );
  SerialOp.setLine( data->serial, wDigInt.getbps( data->ini ), 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );

  return SerialOp.open( data->serial );
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

  int  msglen = 0;
  int  index  = 0;
  byte c;
  Boolean ok = False;

  do {
    if( serialAvailable(inst) <= 0 )
      break;

    ok = SerialOp.read(data->serial, &c, 1);
    if(ok) {
      msg[index] = c;
      index++;
      TraceOp.dump ( "bidibserial", TRCLEVEL_DEBUG, (char*)msg, index );
    }
  } while (data->commOK && ok && data->run);

  if( index > 0 ) {
     TraceOp.dump ( "bidibserial", TRCLEVEL_BYTE, (char*)msg, index );
  }

  return index;
}


Boolean serialWrite( obj inst, unsigned char *msg, int len ) {
  iOBiDiBData data = Data(inst);

  TraceOp.dump ( "bidibserial", TRCLEVEL_BYTE, (char*)msg, len );
  Boolean ok = SerialOp.write( data->serial, (char*)msg, len );

  return ok;
}


int serialAvailable( obj inst ) {
  iOBiDiBData data = Data(inst);

  if( data->commOK ) {
    int rc = SerialOp.available(data->serial);
    if( rc == -1 ) {
      data->commOK = False;
      //BiDiBOp.stateChanged((iOBiDiB)inst);
    }
    return rc;
  }
  return 0;
}

