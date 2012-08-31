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
#include "rocdigs/impl/rocnet/rntcp.h"

#include "rocrail/wrapper/public/RocNet.h"

Boolean rnTcpConnect( obj inst ) {
  iOrocNetData data = Data(inst);
}

void rnTcpDisconnect( obj inst ) {
  iOrocNetData data = Data(inst);
}


int rnTcpRead ( obj inst, unsigned char *msg ) {
  iOrocNetData data = Data(inst);
  return 0;
}

Boolean rnTcpWrite( obj inst, unsigned char *msg, int len ) {
  iOrocNetData data = Data(inst);
  return True;
}


Boolean rnTcpAvailable( obj inst ) {
  iOrocNetData data = Data(inst);
  return True;
}

