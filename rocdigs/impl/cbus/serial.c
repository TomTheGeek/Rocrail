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

#include "rocdigs/impl/cbus/serial.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/thread.h"
#include "rocs/public/serial.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"



static Boolean isCts( obj inst ) {
  iOCBUSData data = Data(inst);
  /* CTS */
  int wait4cts = 0;

  if( !data->cts || data->dummyio ) {
    return True;
  }

  while( wait4cts < data->ctsretry ) {
    int rc = SerialOp.isCTS( data->serial );
    if( rc == -1 ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error; switch to dummy mode" );
      data->dummyio = True;
      return False;
    }
    if( rc > 0 ) {
      return True;
    }
    ThreadOp.sleep( 10 );
    wait4cts++;
  }
  TraceOp.trc(name, TRCLEVEL_WARNING, __LINE__, 9999, "CTS not ready");
  return False;
}



Boolean serialConnect( obj inst ) {
  iOCBUSData data = Data(inst);

  if( StrOp.equals( wDigInt.sublib_usb, wDigInt.getsublib(data->ini) ))
    data->bps = 500000;
  else {
    data->bps = wDigInt.getbps(data->ini);
  }

  data->cts = StrOp.equals( wDigInt.cts, wDigInt.getflow( data->ini ) );
  data->ctsretry = wDigInt.getctsretry( data->ini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device        = %s", wDigInt.getdevice( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps           = %d", data->bps );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "flow          = %s", wDigInt.getflow( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cts retry     = %d * 10ms", data->ctsretry );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout       = %d", wDigInt.gettimeout( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, data->cts ? cts:none );
  SerialOp.setLine( data->serial, data->bps, 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );

  if( SerialOp.open( data->serial ) )
    return True;

  SerialOp.base.del(data->serial);
  data->serial = NULL;
  return False;
}


void serialDisconnect( obj inst ) {
  iOCBUSData data = Data(inst);

  if( data->serial != NULL ) {
    SerialOp.close( data->serial );
    SerialOp.base.del( data->serial );
    data->serial = NULL;
  }
}


Boolean serialRead ( obj inst, unsigned char *frame, int len ) {
  iOCBUSData data = Data(inst);
  return SerialOp.read(data->serial, frame, len);
}


Boolean serialWrite( obj inst, unsigned char *msg, int len ) {
  iOCBUSData data = Data(inst);
  Boolean ok = False;
  if( isCts(inst) ) {
    TraceOp.dump ( "cbusserial", TRCLEVEL_BYTE, (char*)msg, len );
    ok = SerialOp.write( data->serial, (char*)msg, len );
  }
  return ok;
}


Boolean serialAvailable( obj inst ) {
  iOCBUSData data = Data(inst);
  return SerialOp.available(data->serial);
}

