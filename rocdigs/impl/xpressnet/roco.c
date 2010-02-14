/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocdigs/impl/xpressnet_impl.h"
#include "rocdigs/impl/xpressnet/roco.h"
#include "rocdigs/impl/xpressnet/li101.h"
#include "rocrail/wrapper/public/DigInt.h"

Boolean rocoConnect(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );
  SerialOp.setFlow( data->serial, none );
  SerialOp.setLine( data->serial, wDigInt.getbps( data->ini ), 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
  return SerialOp.open( data->serial );
}

void rocoDisConnect(obj xpressnet) {
  li101DisConnect(xpressnet);
}

Boolean rocoAvail(obj xpressnet) {
  return li101Avail(xpressnet);
}

void rocoInit(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);

  // tree times the confirmation
  byte* outa = allocMem(32);
  outa[0] = 1;
  outa[1] = 0x10;
  ThreadOp.post( data->transactor, (obj)outa );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** send confirmation. until response or 6 times");

  // put off programming track
  byte* outa0 = allocMem(32);
  outa0[0] = 2;
  outa0[1] = 0x40;
  outa0[2] = 0xF0;
  ThreadOp.post( data->transactor, (obj)outa0 );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** put off programming track..." );


  // 00 F3 0A 00 00 F9
  byte* outa1 = allocMem(32);
  outa1[0] = 5;
  outa1[1] = 0x00;
  outa1[2] = 0xF3;
  outa1[3] = 0x0A;
  outa1[4] = 0x00;
  outa1[5] = 0x00;
  ThreadOp.post( data->transactor, (obj)outa1 );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** sending: 00 F3 0A 00 00 F9 ..." );


  // set sensor repeat at rate 1
  byte* outb = allocMem(32);
  outb[0] = 3;
  outb[1] = 0x21;
  outb[2] = 0xf1;
  outb[3] = data->readfb ? 0x01:0x00;
  ThreadOp.post( data->transactor, (obj)outb );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*** set sensor repeat at rate %d", outb[3] );

  // Infobyte FB
  byte* oute = allocMem(32);
  oute[0] = 4;
  oute[1] = 0x23;
  oute[2] = 0xF2;
  oute[3] = 0x00;
  oute[4] = 0x00;
  ThreadOp.post( data->transactor, (obj)oute );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** Setting FB info byte for grp. 0 ..." );

  // Infobyte FB
  byte* outf = allocMem(32);
  outf[0] = 4;
  outf[1] = 0x23;
  outf[2] = 0xF2;
  outf[3] = 0x01;
  outf[4] = 0x10;
  ThreadOp.post( data->transactor, (obj)outf );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** Setting FB info byte for grp. 1 ..." );

  // Global Power ON
  byte* outc = allocMem(32);
  outc[0] = 3;
  outc[1] = 0x00;
  outc[2] = 0x21;
  outc[3] = 0x81;
  ThreadOp.post( data->transactor, (obj)outc );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** Global Power ON ..." );


  //00 F0 F0, gives response 00 02 16 80 90
  byte* outc1 = allocMem(32);
  outc1[0] = 2;
  outc1[1] = 0x00;
  outc1[2] = 0xF0;
  ThreadOp.post( data->transactor, (obj)outc1 );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** sending: 00 F0 F0..." );

  //00 21
  byte* outc2 = allocMem(32);
  outc2[0] = 3;
  outc2[1] = 0x00;
  outc2[2] = 0x21;
  outc2[3] = 0x21;
  ThreadOp.post( data->transactor, (obj)outc2 );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** sending: 00 21 ..." );

  //00 F3 0B 00 00 F8
  byte* outc3 = allocMem(32);
  outc3[0] = 5;
  outc3[1] = 0x00;
  outc3[2] = 0xF3;
  outc3[3] = 0x0B;
  outc3[4] = 0x00;
  outc3[5] = 0x00;
  ThreadOp.post( data->transactor, (obj)outc3 );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** sending: 00 F3 0B 00 00 ..." );

  /*00 E3 00 00 03 E0 not found in rocomotion sniffer trace
  byte* outc4 = allocMem(256);
  outc4[0] = 5;
  outc4[1] = 0x00;
  outc4[2] = 0xE3;
  outc4[3] = 0x0B;
  outc4[4] = 0x00;
  outc4[5] = 0x00;
  ThreadOp.post( data->transactor, (obj)outc4 );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** sending: 00 E3 00 00 03 ..." );
  */

  // Setting no of fb in group 0 to %X
  byte* outd = allocMem(32);
  outd[0] = 4;
  outd[1] = 0x22;
  outd[2] = 0xF2;
  outd[3] = 0x00;
  outd[4] = data->fbmod;
  ThreadOp.post( data->transactor, (obj)outd );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*** Setting no of fb in group 0 to %X ...", outd[4] );

   // Setting no of fb in group 1 to %X //TODO
  byte* outg = allocMem(32);
  outg[0] = 4;
  outg[1] = 0x22;
  outg[2] = 0xF2;
  outg[3] = 0x01;
  outg[4] = 0x00;
  ThreadOp.post( data->transactor, (obj)outg );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** Setting no of fb in group 1 to 0 ..." );
}

int rocoRead(obj xpressnet, byte* buffer) {
  return 0;
}

Boolean rocoWrite(obj xpressnet, byte* outin, int* rspexpected) {
  iOXpressNetData data = Data(xpressnet);

  int len = outin[0]+1;
  int i = 0;
  Boolean rc = False;
  byte bXor = 0;
  unsigned char out[len];

  *rspexpected = 1; /* CS will confirm every command? */


  // remove length header
  for (i = 0; i < len-1; i++)
     out[i] = (unsigned char) outin[i+1];

  // calculate xor
  for ( i = 1; i < len-1; i++ ) {
    bXor ^= out[i];
  }
  out[len-1] = bXor;

  // NO XOR for ok byte
  if ( out[0] == 0x10 )
    len = 1;

  // write out
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "wait for mutex..." );
  if( MutexOp.trywait( data->serialmux, 1000 ) ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "out buffer" );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
    if( !data->dummyio ) {
      rc = SerialOp.write( data->serial, (char*)out, len );
    }
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "packet written" );
    MutexOp.post( data->serialmux );
  }

  return rc;
}
