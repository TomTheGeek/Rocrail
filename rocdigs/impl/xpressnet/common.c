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
#include "rocdigs/impl/xpressnet/common.h"
#include "rocs/public/trace.h"

Boolean isChecksumOK(byte* in) {
  byte bXor = 0;
  int i = 0;
  int datalen = (in[0] & 0x0f) + 1;

  for( i = 0; i < datalen; i++ ) {
    bXor ^= in[ i ];
  }

  if( bXor != in[datalen]) {
    TraceOp.trc( "xnxor", TRCLEVEL_EXCEPTION, __LINE__, 9999,
        "XOR error: datalength=%d calculated=0x%02X received=0x%02X", datalen, bXor, in[datalen] );
    return False;
  }
  return True;
}


int makeChecksum(byte* out) {
  int len = out[0] & 0x0f;
  int i = 0;
  byte bXor = 0;
  
  len++; /* header */

  if( out[0] == 0x00 ) {
    return 0;
  }

  for ( i = 0; i < len; i++ ) {
    bXor ^= out[i];
  }
  out[i] = bXor;
  len++; /* checksum */

  return len;
}



