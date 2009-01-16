/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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

