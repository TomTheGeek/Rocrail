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



#include "rocdigs/impl/rocnet/rn-utils.h"
#include "rocdigs/impl/rocnet/rocnet-const.h"
#include "rocs/public/trace.h"

static const char* name = "rnutils";


unsigned char rnChecksum(const unsigned char *b, int len) {
  unsigned char chksum = 0xff;
  int i;
  for (i = 0; i < len; i++) {
    chksum ^= b[i];
  }
  return chksum;
}


int rnCheckPacket(unsigned char* rn, int* extended, int* event) {
  return True;
}


int rnActionFromPacket(unsigned char* rn) {
  return rn[RN_PACKET_ACTION] & RN_ACTION_CODE_MASK;
}
int rnActionTypeFromPacket(unsigned char* rn) {
  return (rn[RN_PACKET_ACTION] >> 5 ) & 3;
}


int rnSenderAddrFromPacket(unsigned char* rn, int seven) {
  return rn[RN_PACKET_SNDRL] + rn[RN_PACKET_SNDRH] * (seven?128:256);
}

int rnReceipientAddrFromPacket(unsigned char* rn, int seven) {
  return rn[RN_PACKET_RCPTL] + rn[RN_PACKET_RCPTH] * (seven?128:256);
}

void rnSenderAddresToPacket( int addr, unsigned char* rn, int seven ) {
  rn[RN_PACKET_SNDRL] = addr % (seven?128:256);
  rn[RN_PACKET_SNDRH] = addr / (seven?128:256);
}

void rnReceipientAddresToPacket( int addr, unsigned char* rn, int seven ) {
  rn[RN_PACKET_RCPTL] = addr % (seven?128:256);
  rn[RN_PACKET_RCPTH] = addr / (seven?128:256);
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
    default:
      return "unsupported";
  }
}





