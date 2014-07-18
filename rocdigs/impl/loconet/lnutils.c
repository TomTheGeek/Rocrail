/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

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
#include "rocdigs/impl/loconet/lnconst.h"
#include "rocdigs/impl/loconet/lnutils.h"

unsigned char checksumLN(const unsigned char* cmd, int len) {
  unsigned char chksum = 0xff;
  int i;
  for (i = 0; i < len; i++) {
    chksum ^= cmd[i];
  }
  return chksum;
}

int binaryStateLN(unsigned char* packet, int addr, int nr, int val) {
  int longaddr = (addr > 127 ? 1:0);

  packet[0] = OPC_IMM_PACKET;
  packet[1] = 0x0B;
  packet[2] = 0x7F;
  packet[3] = (longaddr ? 0x54:0x44); // REPS
  packet[4] = 0x20; // DHI
  if( longaddr ) {
    packet[5]  = 0xC0 | (addr >> 8); // IM1 = (lange) Lokadresse 1234 HIGH BYTE
    packet[6] = addr & 0xFF;  // IM2 = (lange) Lokadresse 1234 LOW BYTE
    packet[7] = 0xC0; // IM2 = Binary state Control - long form
    packet[8] = (nr&0x7F) + (val?0x80:0x00); // IM3 = (nr | val)
    packet[9] = (nr >>7); // IM5
  }
  else {
    packet[5]  = addr; // IM1 = (kurze) Lokadresse 3
    packet[6] = 0xC0; // IM2 = Binary state Control - long form
    packet[7] = (nr&0x7F) | (val?0x80:0x00); // IM3 = (nr | val)
    packet[8] = (nr >>7); // IM4
    packet[9] = 0; // IM5
  }
  packet[10] =0;

  // in DHI gehören die MSBs von IM1 bis IM5 gepackt
  if (packet[5] & 0x80) packet[4] |= 0x01;  // DHI.0 = IM1.7
  if (packet[6] & 0x80) packet[4] |= 0x02;  // DHI.1 = IM2.7
  if (packet[7] & 0x80) packet[4] |= 0x04;  // DHI.2 = IM3.7
  if (packet[8] & 0x80) packet[4] |= 0x08;  // DHI.3 = IM4.7
  if (packet[9] & 0x80) packet[4] |= 0x10;  // DHI.4 = IM5.7

  // nun die MSBs von IM1 bis IM5 löschen
  packet[5]  &= 0x7F; // IM1
  packet[6] &= 0x7F; // IM2
  packet[7] &= 0x7F; // IM3
  packet[8] &= 0x7F; // IM4
  packet[9] &= 0x7F; // IM5

  packet[10] = checksumLN(packet, 10);

  return 11;
}


const char* CONSIST_STAT(int s) {   // encode consisting status as a string
	   return ((s & CONSIST_MASK) == CONSIST_MID) ? "mid cons." :
      		( ((s & CONSIST_MASK) == CONSIST_TOP) ? "top cons." :
      		( ((s & CONSIST_MASK) == CONSIST_SUB) ? "sub cons." :
						"Not Consisted"));
	   }

const char* LOCO_STAT(int s)   { // encode loco status as a string
	   return ((s & LOCOSTAT_MASK) == LOCO_IN_USE) ? "in use" :
			      ( ((s & LOCOSTAT_MASK) == LOCO_IDLE)   ? "idle" :
            ( ((s & LOCOSTAT_MASK) == LOCO_COMMON) ? "common":"free"));
       }

const char* DEC_MODE(int s) { // encode decoder type as a string
       return ((s & DEC_MODE_MASK) == DEC_MODE_128A)  ? "128(adv.cons.)" :
            ( ((s & DEC_MODE_MASK) == DEC_MODE_28A)   ? "28(adv.cons.)" :
            ( ((s & DEC_MODE_MASK) == DEC_MODE_128)   ? "128" :
            ( ((s & DEC_MODE_MASK) == DEC_MODE_14)    ? "14" :
            ( ((s & DEC_MODE_MASK) == DEC_MODE_28TRI) ? "28(motorola)":"28"))));
       }

