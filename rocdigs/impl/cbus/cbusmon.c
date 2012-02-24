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
  int frange  = 0;
  int fdat    = 0;

  int hh   = HEXA2Byte(frame + 2);
  int hl   = HEXA2Byte(frame + 4);

  int canid = ((hh&0x0F) << 3) + ((hl&0xE0) >> 5);

  if( StrOp.startsWith(frame, ":X00080005N") || StrOp.startsWith(frame, ":X00080004N") ) {
    /* boot mode */
    return;
  }

  switch(opc) {
  case OPC_HLT:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%03d] OPC_HLT(0x%02X) bus halt", canid, opc );
    break;

  case OPC_BON:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%03d] OPC_BON(0x%02X) bus on", canid, opc );
    break;

  case OPC_RESTP:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%03d] OPC_RESTP(0x%02X) request emergency stop all", canid, opc );
    break;

  case OPC_ESTOP:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%03d] OPC_ESTOP(0x%02X) all locos have emergency stopped", canid, opc );
    break;

  case OPC_RTOF:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%03d] OPC_RTOF(0x%02X) request track off", canid, opc );
    break;

  case OPC_RTON:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%03d] OPC_RTON(0x%02X) request track on", canid, opc );
    break;

  case OPC_QNN:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%03d] OPC_QNN(0x%02X) query node numbers", canid, opc );
    break;

  case OPC_RLOC:
    addrh   = HEXA2Byte(frame + OFFSET_D1 + offset) & 0x3F;
    addrl   = HEXA2Byte(frame + OFFSET_D2 + offset);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%03d] OPC_RLOC(0x%02X) request loco %d assignment", canid, opc, addrl+addrh*256 );
    break;

  case OPC_KLOC:
    session = HEXA2Byte(frame + OFFSET_D1 + offset);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%03d] OPC_KLOC(0x%02X) release loco: session=%d", canid, opc, session );
    break;

  case OPC_QLOC:
    session = HEXA2Byte(frame + OFFSET_D1 + offset);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%03d] OPC_QLOC(0x%02X) query loco: session=%d", canid, opc, session );
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
        "[%03d] OPC_PLOC(0x%02X) loco %d report: session=%d speed=%d dir=%s f0-4=0x%02X f5-8=0x%02X f9-12=0x%02X",
        canid, opc, addrl+addrh*256, session, speed&0x7F, (speed&0x80)?"fwd":"rev", f0, f1, f2 );
    break;

  case OPC_STMOD:
    session = HEXA2Byte(frame + OFFSET_D1 + offset);
    flags   = HEXA2Byte(frame + OFFSET_D2 + offset);
    if( flags & 0x02 ) steps = 28;
    else if( flags & 0x01 ) steps = 14;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "[%03d] OPC_STMOD(0x%02X) loco flags: session=%d flags=0x%02X steps=%d service=%s soundctrl=%s",
        canid, opc, session, flags, steps, flags&0x04?"on":"off", flags&0x08?"on":"off" );
    break;

  case OPC_DSPD:
    session = HEXA2Byte(frame + OFFSET_D1 + offset);
    speed   = HEXA2Byte(frame + OFFSET_D2 + offset);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "[%03d] OPC_DSPD(0x%02X) loco speed/dir: session=%d speed=%d dir=%s",
        canid, opc, session, speed&0x7F, (speed&0x80)?"fwd":"rev" );
    break;

  case OPC_DFLG:
    session = HEXA2Byte(frame + OFFSET_D1 + offset);
    flags   = HEXA2Byte(frame + OFFSET_D2 + offset);
    if( flags & 0x02 ) steps = 28;
    else if( flags & 0x01 ) steps = 14;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "[%03d] OPC_DFLG(0x%02X) loco flags: session=%d flags=0x%02X steps=%d lights=%s state=%d",
        canid, opc, session, flags, steps, (flags&0x04)?"on":"off", (flags&0x30)>>4 );
    break;

  case OPC_DFUN:
    session = HEXA2Byte(frame + OFFSET_D1 + offset);
    frange  = HEXA2Byte(frame + OFFSET_D2 + offset);
    fdat    = HEXA2Byte(frame + OFFSET_D3 + offset);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "[%03d] OPC_DFUN(0x%02X) loco functions: session=%d range=0x%02X functions=0x%02X",
        canid, opc, session, frange, fdat );
    break;

  case OPC_DKEEP:
    session = HEXA2Byte(frame + OFFSET_D1 + offset);
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
        "[%03d] OPC_DKEEP(0x%02X) keep alive for cab: session=%d",
        canid, opc, session );
    break;

  case OPC_ACDAT:
  {
    char char0  = '\0';
    char char1  = '\0';
    char char2  = '\0';
    char char3  = '\0';
    int display = 0;
    addrh   = HEXA2Byte(frame + OFFSET_D1 + offset);
    addrl   = HEXA2Byte(frame + OFFSET_D2 + offset);
    display = HEXA2Byte(frame + OFFSET_D3 + offset);
    char0   = HEXA2Byte(frame + OFFSET_D4 + offset);
    char1   = HEXA2Byte(frame + OFFSET_D5 + offset);
    char2   = HEXA2Byte(frame + OFFSET_D6 + offset);
    char3   = HEXA2Byte(frame + OFFSET_D7 + offset);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "OPC_ACDAT(0x%02X) address=%d display=%d line=%d nr=%d text=[%c%c%c%c]",
        opc, addrh*256+addrl, display&0x03, ((display&0x04) >> 2), ((display&0xF0) >> 4),
        char0=='\0'?' ':char0, char1=='\0'?' ':char1, char2=='\0'?' ':char2, char3=='\0'?' ':char3 );
  }
  break;

  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "[%03d] evaluate OPC=0x%02X", canid, opc );
    break;
  }

}

