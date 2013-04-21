/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

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
#include "rocdigs/impl/raspi_impl.h"
#include "rocdigs/impl/raspi/io.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/RasPi.h"

#include "rocs/public/trace.h"
#include "rocs/public/str.h"
#include "rocs/public/thread.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/SysCmd.h"

static void __reportState(obj inst) {
  iORasPiData data = Data(inst);

  if( data->listenerFun != NULL && data->listenerObj != NULL ) {
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );

    if( data->iid != NULL )
      wState.setiid( node, data->iid );
    wState.setpower( node, data->power );
    wState.setsensorbus( node, True );
    wState.setaccessorybus( node, True );

    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}

iONode raspiTranslate(obj inst, iONode node) {
  iORasPiData data = Data(inst);
  iONode rsp = NULL;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cmd=%s", NodeOp.getName( node ) );

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );

    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      data->power = False;
      __reportState(inst);
    }
    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      data->power = True;
      __reportState(inst);
    }
  }
  return rsp;
}
