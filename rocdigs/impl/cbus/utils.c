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

#include "rocdigs/impl/cbus/utils.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"



int makeFrame(obj inst, byte* frame, int prio, byte* cmd, int datalen ) {
  iOCBUSData data = Data(inst);
  int i = 0;
  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "makeFrame for OPC=0x%02X", cmd[0] );

  StrOp.fmtb( frame+1, ":S%02X%02XN%02X;", (0x80 + (prio << 5) + (data->cid >> 3)) &0xFF, (data->cid << 5) & 0xFF, cmd[0] );

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

