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



void statusInitDest( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);
  /* Lock the block and the needed street. */
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Init destination for [%s]...",
                 data->loc->getId( data->loc ) );
  {
    /*
     * The getDirection function returns the loco direction and puts the route direction in the third parameter.
     * The route direction flag is True if the train will travel "from" -> "to".
     */
    Boolean dir = data->next1Route->getDirection( data->next1Route,
                              data->loc->getCurBlock( data->loc ),
                              &data->next1RouteFromTo );
    Boolean swapDir = False;
    if( data->next1RouteFromTo )
      swapDir = data->next1Route->isSwapPost( data->next1Route ) ? data->next1RouteFromTo : !data->next1RouteFromTo;
    else
      swapDir = data->next1Route->isSwapPost( data->next1Route ) ? !data->next1RouteFromTo : data->next1RouteFromTo;
    swapDir = dir ? swapDir:!swapDir;
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
        "loco direction for [%s] is [%s], route direction [%s], swappost[%s]",
                   data->loc->getId( data->loc ), dir?"forwards":"reverse",
                   data->next1RouteFromTo?"fromTo":"toFrom",
                   data->next1Route->isSwapPost( data->next1Route )?"true":"false");
    /*
     * The initializeDestination functions last parameter is for inverting the route direction.
     * If the route direction is true there is no invert in traveling, so the default value should be the
     * opposite of the route direction flag.
     */
    if( initializeDestination( (iOLcDriver)inst,
                data->next1Block,
                data->next1Route,
                data->curBlock,
                swapDir, data->indelay ) &&
        initializeSwap( (iOLcDriver)inst, data->next1Route ) )
    {

      if( !data->gomanual ) {
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );

        /* Send the first command to the loc with the direction: */
        wLoc.setdir( cmd, dir );
        wLoc.setV( cmd, 0 );
        data->loc->cmd( data->loc, cmd );
      }


      if( !data->next1Block->isLinked( data->next1Block ) ) {
        data->next1Block->link( data->next1Block, data->curBlock );
      }
      if( data->next1Route != NULL ) {
        data->next1Route->link(data->next1Route, data->curBlock->getTDport(data->curBlock));
      }

      if( data->secondnextblock || data->loc->trySecondNextBlock(data->loc) ) {
        reserveSecondNextBlock( (iOLcDriver)inst, data->gotoBlock, data->next1Block, data->next1Route,
                                  &data->next2Block, &data->next2Route, !data->next1RouteFromTo, False );
        if( data->next2Route != NULL ) {
          /* TODO: make sure the running direction does not change */
          data->next2Route->getDirection( data->next2Route,
              data->next1Block->base.id(data->next1Block), &data->next2RouteFromTo );
        }
        /* TODO: if a second next block was found and initialized show the right signal aspect */
      }

      if( !data->gomanual ) {
        /* pause between the turnout commands and the new loco command: */
        if( wLoc.getdirpause( data->loc->base.properties( data->loc ) ) > 0 ) {
          int dirpause = wLoc.getdirpause( data->loc->base.properties( data->loc ) );
          /*
          if( dirpause > 100 )
            dirpause = 100;
          */
           ThreadOp.sleep( dirpause );
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                       "Waiting for user to start loc \"%s\"",
                       data->loc->getId( data->loc ) );
      }

      data->state = LC_CHECKROUTE;
      data->loc->setMode(data->loc, wLoc.mode_auto);
      /*data->run = False;*/
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Setting state for \"%s\" from LC_INITDEST to LC_CHECKROUTE.",
                     data->loc->getId( data->loc ) );
    }
    else {
      Boolean oppwait = True;
      int ioppwait = 0;
      /* Error! */
      /* if running a schedule the schedule index should be decreased by one to match the current block */
      /* TODO: wait and getWait reverse signal flag */
      if( data->curBlock->wait(data->curBlock, data->loc, False, &oppwait ) ) {
        data->pause = data->curBlock->getWait(data->curBlock, data->loc, False, &ioppwait );
        if( data->pause != -1 )
          data->pause = data->pause * wLoc.getpriority( data->loc->base.properties( data->loc ) );
      } else
        data->pause = wLoc.getpriority( data->loc->base.properties( data->loc ) );

      if( data->schedule != NULL ) {
        data->scheduleIdx--;
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "set schedule index back to %d to match the current entry", data->scheduleIdx );
      }

      data->state = data->run ? LC_PAUSE:LC_IDLE;
      data->loc->setMode(data->loc, wLoc.mode_wait);
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Setting state for [%s] pause=%d from LC_INITDEST to %s.",
                     data->loc->getId( data->loc ), data->pause, data->run ? "LC_PAUSE":"LC_IDLE" );
    }
  }
}

