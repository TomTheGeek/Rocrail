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

#include "rocdigs/impl/cbus/flim.h"
#include "rocdigs/impl/cbus/utils.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/Program.h"

#include "rocdigs/impl/cbus/cbusdefs.h"

/*
 *  +----------+----+---------------------------+--------------------+---------------+
 *  | Module   | ID | NV                        | EV1                | EV2           |
 *  +----------+----+---------------------------+--------------------+---------------+
 *  | CANACC8  | 3  | -                         | output selection   | polarity 0=on |
 *  | CANACE8C | 5  | 1 = bit mask ON/OFF event | 0=report separated | -             |
 */


/*
 * The frame has the incoming FLiM message.
 * A Program node is created if the client should be informed.
 * The extraMsg is a pointer to an empty byte array which can be used
 * to request more info of the node before creating a Program response
 * for clients.
 */
iONode processFLiM(obj inst, int opc, byte *frame, byte **extraMsg) {
  iOCBUSData data = Data(inst);
  byte cmd[32];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "FLiM: 0x%02X", opc );

  switch(opc) {
  case OPC_NNACK:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "FLiM: request node parameters" );
    *extraMsg = allocMem(32);
    cmd[0] = OPC_RQNP;
    makeFrame(inst, *extraMsg, PRIORITY_NORMAL, cmd, 0 );
    break;

  case OPC_PARAMS:
    {
      iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      /* :SB020NEFA5520320020000;
        Para 1 Manufacturer number as allocated by the NMRA
        Para 2 Module version number or code
        Para 3 Module identifier
        Para 4 Number of events allowed
        Para 5 Number of event variables per event
        Para 6 Number of node variables
        Para 7 Not yet allocated.
      */
      int offset = (frame[1] == 'S') ? 0:4;
      int para1  = HEXA2Byte(frame + OFFSET_D1 + offset);
      int para2  = HEXA2Byte(frame + OFFSET_D2 + offset);
      int para3  = HEXA2Byte(frame + OFFSET_D3 + offset);
      int para4  = HEXA2Byte(frame + OFFSET_D4 + offset);
      int para5  = HEXA2Byte(frame + OFFSET_D5 + offset);
      int para6  = HEXA2Byte(frame + OFFSET_D6 + offset);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "FLiM: node parameters received" );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "manuID=%d, version=%d, moduleID=%d", para1, para2, para3 );

      wProgram.setcmd( node, wProgram.nnreq );
      wProgram.setmodid( node, para3 );
      wProgram.setval1( node, para1 );
      wProgram.setval2( node, para2 );
      wProgram.setval3( node, para3 );
      wProgram.setval4( node, para4 );
      wProgram.setval5( node, para5 );
      wProgram.setval6( node, para6 );
      return node;
    }

  }

  return NULL;
}



