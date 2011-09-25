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
  int session = 0;
  int speed   = 0;
  int flags   = 0;
  int steps   = 128;
  int f0      = 0;
  int f1      = 0;
  int f2      = 0;

  switch(opc) {
  case OPC_RESTP:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "OPC_RESTP(0x%02X) request emergency stop all", opc );
    break;

  case OPC_RLOC:
    addrh   = HEXA2Byte(frame + OFFSET_D1 + offset) & 0x3F;
    addrl   = HEXA2Byte(frame + OFFSET_D2 + offset);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "OPC_RLOC(0x%02X) request loco [%d] assignment", opc, addrl+addrh*256 );
    break;

  case OPC_PLOC:
    session = HEXA2Byte(frame + OFFSET_D1 + offset);
    addrh   = HEXA2Byte(frame + OFFSET_D2 + offset) & 0x3F;
    addrl   = HEXA2Byte(frame + OFFSET_D3 + offset);
    speed   = HEXA2Byte(frame + OFFSET_D4 + offset);
    f0      = HEXA2Byte(frame + OFFSET_D5 + offset);
    f1      = HEXA2Byte(frame + OFFSET_D6 + offset);
    f2      = HEXA2Byte(frame + OFFSET_D7 + offset);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "OPC_PLOC(0x%02X) loco [%d] report: session=%d speed=%d dir=%s f0-4=0x%02X f5-8=0x%02X f9-12=0x%02X",
        opc, addrl+addrh*256, session, speed&0x7F, (speed&0x80)?"fwd":"rev", f0, f1, f2 );
    break;

  case OPC_DSPD:
    session = HEXA2Byte(frame + OFFSET_D1 + offset);
    speed   = HEXA2Byte(frame + OFFSET_D2 + offset);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "OPC_DSPD(0x%02X) loco speed/dir: session=%d speed=%d dir=%s",
        opc, session, speed&0x7F, (speed&0x80)?"fwd":"rev" );
    break;

  case OPC_DFLG:
    session = HEXA2Byte(frame + OFFSET_D1 + offset);
    flags   = HEXA2Byte(frame + OFFSET_D2 + offset);
    if( flags & 0x02 ) steps = 28;
    else if( flags & 0x01 ) steps = 14;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "OPC_DFLG(0x%02X) loco flags: session=%d flags=0x%02X steps=%d lights=%s state=%d",
        opc, session, flags, steps, (flags&0x04)?"on":"off", (flags&0x30)>>4 );
    break;

  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "evaluate OPC=0x%02X", opc );
    break;
  }

}

