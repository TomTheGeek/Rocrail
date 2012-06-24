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

#include "rocdigs/impl/cbus_impl.h"

#include "rocdigs/impl/cbus/utils.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"



int makeFrame(byte* frame, int prio, byte* cmd, int datalen, int cid, Boolean eth ) {
  int i = 0;
  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "makeFrame for OPC=0x%02X", cmd[0] );

  StrOp.fmtb( frame+1, ":%c%02X%02XN%02X;", (eth?'Y':'S'), (0x80 + (prio << 5) + (cid >> 3)) &0xFF, (cid << 5) & 0xFF, cmd[0] );

  if( datalen > 0 ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "datalen=%d", datalen );
    for( i = 0; i < datalen; i++ ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "makeframe: %s", frame+1 );
      StrOp.fmtb( frame+1+9+i*2, "%02X;", cmd[i+1] );
    }
  }

  frame[0] = StrOp.len(frame+1);

  return frame[0];
}


byte HEXA2Byte( const char* s ) {
  char val[3] = {0};
  val[0] = s[0];
  val[1] = s[1];
  val[2] = '\0';
  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "HEXA=[%s]", val );
  return (unsigned char)(strtol( val, NULL, 16)&0xFF);
}

void Byte2HEXA( char* s, byte b ) {
  static char cHex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  int i = 0;
  s[0] = cHex[(b&0xF0)>>4 ];
  s[1] = cHex[ b&0x0F     ];
}
