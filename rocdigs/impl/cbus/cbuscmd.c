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

#include "rocdigs/impl/cbus/cbuscmd.h"
#include "rocdigs/impl/cbus/utils.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"

#include "rocdigs/impl/cbus/cbusdefs.h"

void slotServer(obj cbus, int opc, byte* frame) {
  iOCBUSData data = Data(cbus);
  int offset = (frame[1] == 'S') ? 0:4;
  byte cmd[32];

  switch(opc) {
  case OPC_RLOC:
    {
      int addrh = HEXA2Byte(frame + OFFSET_D1 + offset) & 0x3F;
      int addrl = HEXA2Byte(frame + OFFSET_D2 + offset);
      byte* frame = allocMem(32);

      cmd[0] = OPC_PLOC;
      cmd[1] = 1;   // session
      cmd[2] = addrh;
      cmd[3] = addrl;
      cmd[4] = 0;   // speed
      cmd[5] = 0;   // f0
      cmd[6] = 0;   // f1
      cmd[7] = 0;   // f2

      makeFrame(frame, PRIORITY_NORMAL, cmd, 7, data->cid, False );
      TraceOp.trc( "slotserv", TRCLEVEL_MONITOR, __LINE__, 9999, "OPC_PLOC response" );
      ThreadOp.post(data->writer, (obj)frame);
    }
    break;

  }
}

