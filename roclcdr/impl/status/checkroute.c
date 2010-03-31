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



void statusCheckRoute( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);

  if( !data->next1Route->isSet(data->next1Route) ) {
    /* not all switches are set; wait */
    ThreadOp.sleep(10);
    return;
  }
  else {
    /* Start engine and roll. */
    Boolean semaphore = False;
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    Boolean dir = data->next1Route->getDirection( data->next1Route,
        data->loc->getCurBlock( data->loc ), &data->next1RouteFromTo );

    semaphore = setSignals((iOLcDriver)inst, False);


    if( !data->gomanual ) {
      /* Send the second command to the loc: */
      wLoc.setdir( cmd, dir );
      wLoc.setV_hint( cmd, getBlockV_hint(inst, data->curBlock, True, data->next1Route, !data->next1RouteFromTo ) );

      if( !StrOp.equals( wLoc.getV_hint( cmd), wLoc.min ) && data->next1Route->hasThrownSwitch(data->next1Route) ) {
        if( data->loc->compareVhint( data->loc, wLoc.mid) == -1 || data->loc->getV( data->loc ) == 0 )
          wLoc.setV_hint( cmd, wLoc.mid );
      }

      if(semaphore) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "give the semaphore some time to get in position..." );
        /* give the semaphore some time to get in position... */
        ThreadOp.sleep(data->semaphoreWait);
      }
      else if(data->signalWait > 0){
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "give the signal some time to set another aspect..." );
        ThreadOp.sleep(data->signalWait);
      }

      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Setting direction for [%s] to [%s] at velocity [%s].",
                     data->loc->getId( data->loc ), dir?"forwards":"reverse",
                     wLoc.getV_hint(cmd) );

      /* Send the command to the loc: */
      data->loc->cmd( data->loc, cmd );
    }

    data->state = LC_PRE2GO;
    data->eventTimeout = 0;
    data->signalReset  = 0;
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" from LC_CHECKROUTE to LC_PRE2GO.",
                   data->loc->getId( data->loc ) );
  }
}

