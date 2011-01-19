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

#include "rocdigs/impl/rocnet_impl.h"
#include "rocdigs/impl/rocnet/rocnet-parser.h"
#include "rocdigs/impl/rocnet/rn-utils.h"
#include "rocrail/wrapper/public/RocNet.h"

static const char* myname = "rnparser";

const char* rocnetGetProtocolStr(byte prot) {
  const char* protStr = "?";

  switch( prot & 0x0F ) {
    case RN_MOBILE_PROT_256   : protStr = "256" ; break;
    case RN_MOBILE_PROT_DCC28 : protStr = "DCC 28" ; break;
    case RN_MOBILE_PROT_DCC128: protStr = "DCC 128"; break;
    case RN_MOBILE_PROT_DCC14 : protStr = "DCC 14" ; break;
  }

  return protStr;

}



Boolean rocnetIsThis( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          sndr       = rnSenderAddrFromPacket(rn, data->seven);
  return (sndr == wRocNet.getid(data->rnini));
}


byte* rocnetParseMobile( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  byte*        rnReply    = NULL;
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);

  rcpt = rnReceipientAddrFromPacket(rn, data->seven);
  sndr = rnSenderAddrFromPacket(rn, data->seven);

  switch( action ) {
  case RN_MOBILE_VELOCITY:
    TraceOp.trc( myname, TRCLEVEL_INFO, __LINE__, 9999,
        "mobile RN_MOBILE_VELOCITY addr=%d V=%d prot=%s dir=%s lights=%s",
        addr, rn[RN_PACKET_DATA + 0], rocnetGetProtocolStr(rn[RN_PACKET_DATA + 1]),
        (rn[RN_PACKET_DATA + 1] & RN_MOBILE_DIR_FORWARDS) ? "forwards":"reverse",
        (rn[RN_PACKET_DATA + 1] & RN_MOBILE_LIGHTS_ON) ? "on":"off" );
    break;
  case RN_MOBILE_FUNCTIONS:
    TraceOp.trc( myname, TRCLEVEL_INFO, __LINE__, 9999,
        "mobile RN_MOBILE_FUNCTIONS addr=%d prot=%s lights=%s f1=%s f2=%s f3=%s f4=%s f5=%s f6=%s f7=%s f8=%s f9=%s f10=%s f11=%s f12=%s",
        addr, rocnetGetProtocolStr(rn[RN_PACKET_DATA + 2]),
        (rn[RN_PACKET_DATA + 0] & 0x40) ? "on":"off",
        (rn[RN_PACKET_DATA + 0] & 0x01) ? "on":"off",
        (rn[RN_PACKET_DATA + 0] & 0x02) ? "on":"off",
        (rn[RN_PACKET_DATA + 0] & 0x04) ? "on":"off",
        (rn[RN_PACKET_DATA + 0] & 0x08) ? "on":"off",
        (rn[RN_PACKET_DATA + 0] & 0x10) ? "on":"off",
        (rn[RN_PACKET_DATA + 0] & 0x20) ? "on":"off",
        (rn[RN_PACKET_DATA + 1] & 0x01) ? "on":"off",
        (rn[RN_PACKET_DATA + 1] & 0x02) ? "on":"off",
        (rn[RN_PACKET_DATA + 1] & 0x04) ? "on":"off",
        (rn[RN_PACKET_DATA + 1] & 0x08) ? "on":"off",
        (rn[RN_PACKET_DATA + 1] & 0x10) ? "on":"off",
        (rn[RN_PACKET_DATA + 1] & 0x20) ? "on":"off"
    );
    break;
  default:
    TraceOp.trc( myname, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
    break;
  }

  return rnReply;
}


byte* rocnetParseGeneral( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);

  static Boolean selfTest = True;

  rcpt = rnReceipientAddrFromPacket(rn, data->seven);
  sndr = rnSenderAddrFromPacket(rn, data->seven);

  switch( action ) {
    case RN_CS_NOP:
      TraceOp.trc( myname, TRCLEVEL_INFO, __LINE__, 9999,
          "general NOP(%s) action for %d from %d%s, %d data bytes",
          rnActionTypeString(rn), rcpt, sndr, isThis?"(this)":"", rn[RN_PACKET_LEN] );

      if(!isThis || selfTest ) {
        /* reply with the same packet */
        int len = 8 + rn[RN_PACKET_LEN];
        byte* rnReply = allocMem(len);
        MemOp.copy( rnReply, rn, len );
        rnReply[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_REPLY << 5);
        return rnReply;
        selfTest = False;
      }
      break;

    case RN_CS_TRACKPOWER:
      TraceOp.trc( myname, TRCLEVEL_INFO, __LINE__, 9999,
          "general TRACKPOWER(%s) action for %d from %d%s, %d data bytes",
          rnActionTypeString(rn), rcpt, sndr, isThis?"(this)":"", rn[RN_PACKET_LEN] );
      break;

    default:
      TraceOp.trc( myname, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
      break;
  }

  return NULL;
}


byte* rocnetParseOutput( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);

  rcpt = rnReceipientAddrFromPacket(rn, data->seven);
  sndr = rnSenderAddrFromPacket(rn, data->seven);

  switch( action ) {
  case RN_OUTPUT_SWITCH:
    TraceOp.trc( myname, TRCLEVEL_INFO, __LINE__, 9999,
        "output SWITCH(%s) addr=%d %s action for %d from %d%s, %d data bytes",
        rnActionTypeString(rn), addr, rn[RN_PACKET_DATA + 0] & RN_OUTPUT_ON ? "on":"off",
        rcpt, sndr, isThis?"(this)":"", rn[RN_PACKET_LEN] );
    break;
  case RN_OUTPUT_SWITCH_MULTI:
    TraceOp.trc( myname, TRCLEVEL_INFO, __LINE__, 9999,
        "output SWITCH MULTI(%s) addr=%d %02X:%02X action for %d from %d%s, %d data bytes",
        rnActionTypeString(rn), addr, rn[RN_PACKET_DATA + 1], rn[RN_PACKET_DATA + 3],
        rcpt, sndr, isThis?"(this)":"", rn[RN_PACKET_LEN] );
    break;
  default:
    TraceOp.trc( myname, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
    break;
  }

  return NULL;
}




