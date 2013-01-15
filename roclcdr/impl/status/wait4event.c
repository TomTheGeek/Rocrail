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

#include "roclcdr/impl/lcdriver_impl.h"

#include "roclcdr/impl/tools/tools.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"



#include "rocrail/public/model.h"

#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Link.h"



void statusWait4Event( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);
  Boolean oppwait = True;

  if( data->next1Block != NULL ) {
    if( data->next2Block == NULL ) {
      if( data->loc->isCheck2In( data->loc ) &&
          !data->next1Block->wait( data->next1Block, data->loc, !data->next1RouteFromTo, &oppwait ) &&
          data->run && !data->reqstop )
      {
        /* set step back to ENTER? may be a possible destination block did come free... */
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
            "Setting state for [%s] from LC_WAIT4EVENT to LC_RE_ENTERBLOCK. (check for free block)",
            data->loc->getId( data->loc ) );
        data->state = LC_RE_ENTERBLOCK;
        data->reentertimer = wLoc.getpriority( data->loc->base.properties( data->loc ) );
      }
    }
    else {
      if( !data->next2Route->isSet(data->next2Route) ) {
        if( !data->gomanual && !data->slowdown4route ) {
          /* set velocity to v_mid */
          iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
          if( data->loc->compareVhint( data->loc, wLoc.mid) == -1 )
            wLoc.setV_hint( cmd, wLoc.mid );
          wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
          data->loc->cmd( data->loc, cmd );
          data->slowdown4route = True;
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
              "Slow down for **not set** route running %s",
              data->loc->getId( data->loc ) );
        }
      }
      else if(data->slowdown4route) {
        if( !data->gomanual ) {
          /* set the velocity back */
          iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
          int maxkmh = 0;
          wLoc.setV_hint( cmd, getBlockV_hint(inst, data->curBlock, False, data->next1Route, !data->next1RouteFromTo, &maxkmh ) );
          wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
          wLoc.setV_maxkmh(cmd, maxkmh);
          data->loc->cmd( data->loc, cmd );
          data->slowdown4route = False;
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
              "Restore normale velocity running %s",
              data->loc->getId( data->loc ) );
        }
      }
    }
  }
}
