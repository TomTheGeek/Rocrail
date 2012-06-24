/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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
#include "rocdigs/impl/rocnet_impl.h"
#include "rocdigs/impl/rocnet/rnserial.h"
#include "rocrail/wrapper/public/DigInt.h"


Boolean rnSerialConnect( obj inst ) {
  iOrocNetData data = Data(inst);

  data->cts = StrOp.equals( wDigInt.cts, wDigInt.getflow( data->ini ) );
  data->ctsretry = wDigInt.getctsretry( data->ini );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  =%s", wDigInt.getdevice( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps     =%d", wDigInt.getbps( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "flow    =%s", data->cts ? "cts":"none" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ctsretry=%d", data->ctsretry );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout =%d", wDigInt.gettimeout( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serialCon = SerialOp.inst( wDigInt.getdevice( data->ini ) );
  SerialOp.setFlow( data->serialCon, data->cts ? cts:none );
  SerialOp.setLine( data->serialCon, wDigInt.getbps( data->ini ), 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
  SerialOp.setTimeout( data->serialCon, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );

  if( SerialOp.open( data->serialCon ) ) {
    return True;
  }
  else {
    SerialOp.base.del( data->serialCon );
    return False;
  }
}


void rnSerialDisconnect( obj inst ) {
  iOrocNetData data = Data(inst);
  if( data->serialCon != NULL ) {
    SerialOp.close( data->serialCon );
    SerialOp.base.del( data->serialCon );
    data->serialCon = NULL;
  }
}


int rnSerialRead ( obj inst, unsigned char *msg ) {
  iOrocNetData data = Data(inst);

  int garbage = 0;
  byte bucket[128];
  byte c;
  Boolean  ok = False;

  do {
    if( !SerialOp.available(data->serialCon) )
      return 0;

    ok = SerialOp.read(data->serialCon, &c, 1);
    if(c < 0x80) {
      ThreadOp.sleep(10);
      bucket[garbage] = c;
      garbage++;
    }
  } while (ok && data->run && c < 0x80 && garbage < 128);

  if( garbage > 0 ) {
     TraceOp.trc( "rnserial", TRCLEVEL_INFO, __LINE__, 9999, "garbage=%d", garbage );
     TraceOp.dump ( "rnserial", TRCLEVEL_BYTE, (char*)bucket, garbage );
  }

  if( ok && ( c & 0x80 ) ) {
    int dataLen = 0;
    msg[0] = c;
    ok = SerialOp.read(data->serialCon, msg+1, 7);
    if( !ok ) {
      return 0;
    }
    dataLen = msg[7];
    if( ok && dataLen > 0 ) {
      ok = SerialOp.read(data->serialCon, msg+8, dataLen);
    }
    if( ok && data->crc ) {
      byte crc = 0;
      ok = SerialOp.read(data->serialCon, &crc, 1);
      if( ok && crc != rnChecksum(msg, 8 + dataLen) ) {
        /* checksum error */
        TraceOp.trc( "rnserial", TRCLEVEL_EXCEPTION, __LINE__, 9999, "checksum error; 0x%02X expected, got 0x%02X", rnChecksum(msg, 8 + dataLen), crc );
        TraceOp.dump ( "rnserial", TRCLEVEL_BYTE, msg, 8 + dataLen );
        return 0;
      }
      msg[8 + dataLen] = crc;
    }
    if(ok) {
      TraceOp.dump ( "rnserial", TRCLEVEL_BYTE, msg, 8 + dataLen + (data->crc ? 1:0));
      return 8 + dataLen;
    }
  }

  return 0;
}


static Boolean __isCTS( iOSerial ser, int retries, Boolean handshake ) {
  /* CTS */
  int wait4cts = 0;

  if( !handshake ) {
    return True;
  }

  while( wait4cts < retries ) {
    if( SerialOp.isCTS( ser ) ) {
      return True;
    }
    ThreadOp.sleep( 10 );
    wait4cts++;
  }
  TraceOp.trc( "rnserial", TRCLEVEL_WARNING, __LINE__, 9999, "CTS has timed out: please check the wiring." );
  return False;
}


Boolean rnSerialWrite( obj inst, unsigned char *msg, int len ) {
  iOrocNetData data = Data(inst);
  Boolean ok = True;

  int i = 0;

  if( !__isCTS( data->serialCon, data->ctsretry, data->cts ) ) {
    return False;
  }

  /* set 7 bit flag */
  msg[0] |= 0x80;
  if( data->crc ) {
    msg[len] = rnChecksum(msg, len);
    len++;
  }
  TraceOp.dump ( "rnserial", TRCLEVEL_BYTE, msg, len );
  ok = SerialOp.write( data->serialCon, msg, len );

  return ok;
}


Boolean rnSerialAvailable( obj inst ) {
  iOrocNetData data = Data(inst);
  if( data->serialCon != NULL )
    return SerialOp.available(data->serialCon);
  return 0;
}

