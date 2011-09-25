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

#include "rocdigs/impl/cbus/cbusdefs.h"
#include "rocdigs/impl/cbus/rocrail.h"
#include "rocdigs/impl/cbus/utils.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"

void cbusMon(byte* frame, int opc) {
  int offset  = (frame[1] == 'S') ? 0:4;
  int addrh   = 0;
  int addrl   = 0;

  switch(opc) {
  case OPC_RLOC:
    addrh   = HEXA2Byte(frame + OFFSET_D1 + offset) & 0x3F;
    addrl   = HEXA2Byte(frame + OFFSET_D2 + offset);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "OPC_RLOC(0x%02X) request loco [%d] assignment", opc, addrl+addrh*256 );
    break;

  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "evaluate OPC=0x%02X", opc );
    break;
  }

}

