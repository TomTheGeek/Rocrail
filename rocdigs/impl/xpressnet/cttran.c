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

#include "rocdigs/impl/xpressnet_impl.h"
#include "rocdigs/impl/xpressnet/cttran.h"
#include "rocdigs/impl/xpressnet/li101.h"

Boolean cttranConnect(obj xpressnet) {
  return li101Connect(xpressnet);
}

void cttranDisConnect(obj xpressnet) {
  li101DisConnect(xpressnet);
}

Boolean cttranAvail(obj xpressnet) {
  return li101Avail(xpressnet);
}

void cttranInit(obj xpressnet) {
  li101Init(xpressnet);
}
int cttranRead(obj xpressnet, byte* buffer, Boolean* rspreceived) {
  return li101Read(xpressnet, buffer, rspreceived);
}
Boolean cttranWrite(obj xpressnet, byte* buffer, Boolean* rspexpected) {
  return li101Write(xpressnet, buffer, rspexpected);
}
