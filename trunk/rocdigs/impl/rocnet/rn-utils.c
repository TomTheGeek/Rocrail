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



#include "rocdigs/impl/rocnet/rn-utils.h"
#include "rocdigs/impl/rocnet/rocnet-const.h"
#include "rocs/public/trace.h"

static const char* name = "rnutils";

int rnCheckPacket(unsigned char* rn, int* extended, int* event) {
  int len = 8 + rn[RN_PACKET_LEN];
  int i;
  if( ! (rn[0] & RN_PACKET_START ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "No start bit at begin of rocNet packet!" );
    return False;
  }
  for( i = 1; i < len; i++ ) {
    if( rn[i] & RN_PACKET_START ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "High bit 7 detected in data of rocNet packet!" );
      return False;
    }
  }
  *extended = rn[0] & RN_PACKET_EXTENDED;
  *event    = rn[0] & RN_PACKET_EVENT;
  return True;
}


int rnActionFromPacket(unsigned char* rn) {
  return rn[RN_PACKET_ACTION] & RN_ACTION_MASK;
}
int rnActionTypeFromPacket(unsigned char* rn) {
  return (rn[RN_PACKET_ACTION] >> 5 ) & 3;
}


int rnSenderAddrFromPacket(unsigned char* rn) {
  return rn[RN_PACKET_EXT_SNDRL] + rn[RN_PACKET_EXT_SNDRH] * 128;
}

int rnReceipientAddrFromPacket(unsigned char* rn) {
  return rn[RN_PACKET_EXT_RCPTL] + rn[RN_PACKET_EXT_RCPTH] * 128;
}

int rnAddrFromPacket(unsigned char* rn) {
  return rn[RN_PACKET_ADDRL] + rn[RN_PACKET_ADDRH] * 128;
}

void rnSenderAddresToPacket( int addr, unsigned char* rn ) {
  rn[RN_PACKET_EXT_SNDRL] = addr % 128;
  rn[RN_PACKET_EXT_SNDRH] = addr / 128;
}

void rnReceipientAddresToPacket( int addr, unsigned char* rn ) {
  rn[RN_PACKET_EXT_RCPTL] = addr % 128;
  rn[RN_PACKET_EXT_RCPTH] = addr / 128;
}

void rnAddresToPacket( int addr, unsigned char* rn ) {
  rn[RN_PACKET_ADDRL] = addr % 128;
  rn[RN_PACKET_ADDRH] = addr / 128;
}

const char* rnActionTypeString(unsigned char* rn) {
  int actionType = (rn[RN_PACKET_ACTION] >> 5 ) & 3;
  switch(actionType) {
    case RN_ACTIONTYPE_REQUEST:
      return "request";
    case RN_ACTIONTYPE_EVENT:
      return "event";
    case RN_ACTIONTYPE_REPLY:
      return "reply";
    case RN_ACTIONTYPE_REPLYEXP:
      return "request, reply expected";
    default:
      return "unsupported";
  }
}





