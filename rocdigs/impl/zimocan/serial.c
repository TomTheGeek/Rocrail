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
#include "rocdigs/impl/zimocan/serial.h"
#include "rocrail/wrapper/public/DigInt.h"


Boolean SerialConnect( obj inst ) {
  iOZimoCANData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  =%s", wDigInt.getdevice( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps     =%d", wDigInt.getbps( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
  SerialOp.setFlow( data->serial, cts );
  SerialOp.setLine( data->serial, wDigInt.getbps( data->ini ), 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );

  if( SerialOp.open( data->serial ) ) {
    return True;
  }
  else {
    SerialOp.base.del( data->serial );
    return False;
  }
}


void SerialDisconnect( obj inst ) {
  iOZimoCANData data = Data(inst);
  if( data->serial != NULL ) {
    SerialOp.close( data->serial );
    SerialOp.base.del( data->serial );
    data->serial = NULL;
  }
}


int SerialRead ( obj inst, unsigned char *msg ) {
  iOZimoCANData data = Data(inst);


  return 0;
}


Boolean SerialWrite( obj inst, unsigned char *msg, int len ) {
  iOZimoCANData data = Data(inst);
  Boolean ok = True;


  return ok;
}


Boolean SerialAvailable( obj inst ) {
  iOZimoCANData data = Data(inst);
  if( data->serial != NULL )
    return SerialOp.available(data->serial);
  return 0;
}

