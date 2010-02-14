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
#include "rocdigs/impl/xpressnet/elite.h"
#include "rocdigs/impl/xpressnet/li101.h"

Boolean eliteConnect(obj xpressnet) {
  return li101Connect(xpressnet);
}

void eliteDisConnect(obj xpressnet) {
  li101DisConnect(xpressnet);
}

Boolean eliteAvail(obj xpressnet) {
  return li101Avail(xpressnet);
}

void eliteInit(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);

  /* Asking for CS version */
  byte* out = allocMem(32);
  out[0] = 0x21;
  out[1] = 0x21;
  out[2] = 0x00;
  ThreadOp.post( data->transactor, (obj)out );



  if( data->startpwstate) {
    /* ALL ON */
    out = allocMem(32);
    out[0] = 0x21;
    out[1] = 0x81;
    out[2] = 0xA0;
    ThreadOp.post( data->transactor, (obj)out );
  } else {
    /* ALL OFF*/
    out = allocMem(32);
    out[0] = 0x21;
    out[1] = 0x80;
    out[2] = 0xA1;
    ThreadOp.post( data->transactor, (obj)out );
  }
}

int eliteRead(obj xpressnet, byte* buffer) {
  return li101Read(xpressnet, buffer);
}

Boolean eliteWrite(obj xpressnet, byte* buffer, int* rspexpected) {
  return li101Write(xpressnet, buffer, rspexpected);
}
