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



void statusIn( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);

  data->loc->setBlockEnterSide(data->loc, data->next1Route->getToBlockSide(data->next1Route));

  /* Signal of destination block. (_event) */
  if( data->next2Block == NULL ) {
    if( !data->gomanual ) {
      if( data->next1Block->hasExtStop(data->next1Block) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "block %s has a stop module; not sending velocity 0 to loco %s",
            data->next1Block->base.id(data->next1Block), data->loc->getId(data->loc));
      }
      else {
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setV( cmd, 0 );
        wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
        data->loc->cmd( data->loc, cmd );
      }
    }

    data->state = LC_WAITBLOCK;
    data->prevState = LC_INBLOCK;

    data->loc->setMode(data->loc, wLoc.mode_wait);
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" from LC_INBLOCK to LC_WAITBLOCK.",
                   data->loc->getId( data->loc ) );

  }
  else if( data->next1Route != NULL && !data->next1Route->isSet(data->next1Route) ) {
    /* stop and go in status checkroute */

    if( !data->gomanual ) {
      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setV( cmd, 0 );
      wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
      data->loc->cmd( data->loc, cmd );
    }

    data->next1Block = data->next2Block;
    data->next2Block = data->next3Block;
    data->next3Block = NULL;
    data->next1Block->link( data->next1Block, data->curBlock );
    data->next1RouteFromTo = data->next2RouteFromTo;
    data->next2RouteFromTo = data->next3RouteFromTo;

    data->state = LC_CHECKROUTE;

    data->loc->setMode(data->loc, wLoc.mode_auto);
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                   "Setting state for [%s] from LC_INBLOCK to LC_CHECKROUTE.",
                   data->loc->getId( data->loc ) );


  }
  else {
    /* set the block departure velocity: */
    if( !data->gomanual ) {
      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setV_hint( cmd, getBlockV_hint(inst, data->next1Block, True, data->next1Route, !data->next1RouteFromTo ) );

      /* check for thrown switches in route */
      if( !StrOp.equals( wLoc.getV_hint( cmd), wLoc.min ) && data->next1Route->hasThrownSwitch(data->next1Route) ) {
        if( data->loc->compareVhint( data->loc, wLoc.mid) == -1 || data->loc->getV( data->loc ) == 0 )
          wLoc.setV_hint( cmd, wLoc.mid );
      }

      wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
      data->loc->cmd( data->loc, cmd );
    }

    data->next1Block = data->next2Block;
    data->next2Block = data->next3Block;
    data->next3Block = NULL;
    data->next1Block->link( data->next1Block, data->curBlock );
    data->next1RouteFromTo = data->next2RouteFromTo;
    data->next2RouteFromTo = data->next3RouteFromTo;

    data->state = LC_PRE2GO;

    data->loc->setMode(data->loc, wLoc.mode_auto);
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" from LC_INBLOCK to LC_PRE2GO.",
                   data->loc->getId( data->loc ) );
  }
}
