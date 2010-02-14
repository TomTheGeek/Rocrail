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
#include "rocdigs/impl/xpressnet/opendcc.h"
#include "rocdigs/impl/xpressnet/li101.h"

Boolean opendccConnect(obj xpressnet) {
  return li101Connect(xpressnet);
}

void opendccDisConnect(obj xpressnet) {
  li101DisConnect(xpressnet);
}

Boolean opendccAvail(obj xpressnet) {
  return li101Avail(xpressnet);
}

void opendccInit(obj xpressnet) {
  li101Init(xpressnet);
}
int opendccRead(obj xpressnet, byte* buffer) {
  return li101Read(xpressnet, buffer);
}
Boolean opendccWrite(obj xpressnet, byte* buffer, int* rspexpected) {
  return li101Write(xpressnet, buffer, rspexpected);
}
