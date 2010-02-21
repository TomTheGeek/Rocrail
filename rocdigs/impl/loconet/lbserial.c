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

#include "rocdigs/impl/loconet/lbserial.h"
#include "rocdigs/impl/loconet/lnmon.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/thread.h"
#include "rocs/public/serial.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"


Boolean lbserialConnect( obj inst ) {
  iOLocoNetData data = Data(inst);

  Boolean native = StrOp.equals( wDigInt.sublib_native, wDigInt.getsublib( data->ini ) );

  data->cts      = StrOp.equals( wDigInt.cts, wDigInt.getflow( data->ini ) );
  data->ctsretry = wDigInt.getctsretry( data->ini );
  data->bps      = wDigInt.getbps( data->ini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  =%s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps     =%d", data->bps );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "flow    =%s", data->cts ? "cts":"none" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ctsretry=%d", data->ctsretry );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );


  data->serial = SerialOp.inst( data->device );
  if( native ) {
    /* MS100 bps=16457 */
    SerialOp.setFlow( data->serial, none );
    if( SystemOp.isWindows() ) {
      SerialOp.setLine( data->serial, 16457, 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
    }
    else {
      SerialOp.setLine( data->serial, 57600, 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
      SerialOp.setDivisor( data->serial, 7 );
    }
    // set RTS high, DTR low to power the MS100
    SerialOp.setRTS(data->serial, True);    // not connected in some serial ports and adapters
    SerialOp.setDTR(data->serial, False);   // pin 1 in DIN8; on main connector, this is DTR
  }
  else {
    SerialOp.setFlow( data->serial, data->cts? cts:none );
    SerialOp.setLine( data->serial, data->bps, 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
  }
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );

  if( SerialOp.open( data->serial ) )
    return True;
  else {
    SerialOp.base.del( data->serial );
    return False;
  }
}

void lbserialDisconnect( obj inst ) {
  iOLocoNetData data = Data(inst);

  if( data->serial != NULL ) {
    SerialOp.close( data->serial );
    SerialOp.base.del( data->serial );
    data->serial = NULL;
  }
}

Boolean lbserialAvailable ( obj inst ) {
  iOLocoNetData data = Data(inst);
  return SerialOp.available(data->serial);
}


int lbserialRead ( obj inst, unsigned char *msg ) {
  iOLocoNetData data = Data(inst);
  int  msglen = 0;
  int   index = 0;
  int garbage = 0;
  byte bucket[32];
  byte c;
  Boolean  ok = False;

  do {
    if( !SerialOp.available(data->serial) )
      return 0;

    ok = SerialOp.read(data->serial, &c, 1);
    if(c < 0x80) {
      ThreadOp.sleep(10);
      bucket[garbage] = c;
      garbage++;
    }
  } while (ok && data->run && c < 0x80 && garbage < 10);

  if( garbage > 0 ) {
     TraceOp.trc( "lbserial", TRCLEVEL_INFO, __LINE__, 9999, "garbage=%d", garbage );
     TraceOp.dump ( "lbserial", TRCLEVEL_BYTE, (char*)bucket, garbage );
  }

  if( !data->run || !ok ) {
    if( data->comm ) {
      data->comm = False;
      LocoNetOp.stateChanged((iOLocoNet)inst);
    }
    return -1;
  }

  if( !data->comm ) {
    data->comm = True;
    LocoNetOp.stateChanged((iOLocoNet)inst);
  }

  msg[0] = c;

  switch (c & 0xe0) {
  case 0x80:
      msglen = 2;
      index = 1;
      break;
  case 0xa0:
      msglen = 4;
      index = 1;
      break;
  case 0xc0:
      msglen = 6;
      index = 1;
      break;
  case 0xe0:
      SerialOp.read(data->serial, &c, 1);
      msg[1] = c;
      index = 2;
      msglen = c;
      break;
  default:
    TraceOp.trc( "lbserial", TRCLEVEL_WARNING, __LINE__, 9999, "unknown message 0x%02X length=%d", msg[0], msglen );
    return 0;
  }
  TraceOp.trc( "lbserial", TRCLEVEL_DEBUG, __LINE__, 9999, "message 0x%02X length=%d", msg[0], msglen );

  ok = SerialOp.read(data->serial, &msg[index], msglen - index);

  if( ok ) {
    return msglen;
  }
  else {
    TraceOp.trc( "lbserial", TRCLEVEL_WARNING, __LINE__, 9999, "could not read!" );
    return -1;
  }

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
  TraceOp.trc( "lbserial", TRCLEVEL_WARNING, __LINE__, 9999, "CTS has timed out: please check the wiring." );
  return False;
}


Boolean lbserialWrite( obj inst, unsigned char *msg, int len ) {
  iOLocoNetData data = Data(inst);
  // The Intellibox cannot handle messges over 4 bytes without
  // stopping the sender via CTS/RTS hardware handshake
  // While this should work already by using the normal hardware
  // handshake - it doesn't seem to so we need to check/send/flush
  // each byte to make sure we don't overflow the IB input buffer
  Boolean ok = True;

  int i = 0;

  if( !__isCTS( data->serial, data->ctsretry, data->cts ) ) {

    if( data->comm ) {
      data->comm = False;
      LocoNetOp.stateChanged((iOLocoNet)inst);
    }
    TraceOp.trc( "lbserial", TRCLEVEL_WARNING, __LINE__, 9999, "CTS has timed out: please check the wiring." );
    return False;
  }

  for( i = 0; i < len && __isCTS( data->serial, data->ctsretry, data->cts ); i++ ) {
    ok = SerialOp.write( data->serial, (char*)&msg[i], 1 );
  }

  if( i < len ) {
    return False;
    TraceOp.trc( "lbserial", TRCLEVEL_WARNING, __LINE__, 9999, "CTS has timed out: please check the wiring." );
  }

  return ok;
}


