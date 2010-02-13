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
#include "rocdigs/impl/xpressnet/liusb.h"
#include "rocdigs/impl/xpressnet/li101.h"
#include "rocrail/wrapper/public/DigInt.h"


Boolean liusbConnect(obj xpressnet) {
  iOXpressNetData data = Data(xpressnet);
  SerialOp.setLine( data->serial, 57600, 8, 1, 0, wDigInt.isrtsdisabled( data->ini ) );
  return SerialOp.open( data->serial );
}

void liusbInit(obj xpressnet) {
  li101Init(xpressnet);
}

int liusbRead(obj xpressnet, byte* buffer) {
  return 0;
}

Boolean liusbWrite(obj xpressnet, byte* buffer, int* rspexpected) {
  return False;
}

