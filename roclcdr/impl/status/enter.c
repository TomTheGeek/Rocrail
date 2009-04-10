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



void statusEnter( iILcDriverInt inst, Boolean re_enter ) {
  iOLcDriverData data = Data(inst);

  /* Signal of destination block; wait or search for next destination? (_event) */
  iONode bkprops = (iONode)data->curBlock->base.properties( data->curBlock );
  iONode lcprops = (iONode)data->loc->base.properties( data->loc );

  if( data->next1Block == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
        "Unexpected enter event for \"%s\" state=%d run=%d", data->loc->getId( data->loc ), data->state, data->run );
    data->state = LC_IDLE;
    wLoc.setmode( data->loc->base.properties( data->loc ), wLoc.mode_idle );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" from LC_ENTERBLOCK to LC_IDLE.",
                   data->loc->getId( data->loc ) );
    return;
  }

  /*
  data->curBlock->red( data->curBlock, True, !data->next1RouteFromTo );
  data->curBlock->red( data->curBlock, False, !data->next1RouteFromTo );
  */
  data->next1Block->enterBlock( data->next1Block, data->loc->getId( data->loc ) );
    
  // unLock crossing bloks and switches before new calculation in eventEnter
  // data->next1Route->unLock( data->next1Route, data->loc->getId( data->loc ) );
    
  if( !data->next1Block->wait( data->next1Block, data->loc ) &&
      data->run &&
      !data->reqstop &&
      !data->next1Block->isTerminalStation(data->next1Block) )
  {

    /* Find and lock next destination block and street... */
    if( data->schedule == NULL || StrOp.len( data->schedule ) == 0 ) {
      if( data->next2Block == NULL ) {
        data->next2Block = data->model->findDest( data->model, data->next1Block->base.id( data->next1Block ),
                                                  data->loc, &data->next2Route, data->gotoBlock,
                                                  wLoc.istrysamedir( data->loc->base.properties( data->loc ) ),
                                                  wLoc.istryoppositedir( data->loc->base.properties( data->loc ) ), 
					                                        wLoc.isforcesamedir( data->loc->base.properties( data->loc ) ),
		       			                                  data->next1Route->isSwapPost( data->next1Route ) );
      }
      else {
        /* next2Block already locked */
      }
    }
    else {
      Boolean wait = False;
      /* find destination using schedule */
      if( data->next2Route == NULL ) {
        data->next2Route = data->model->calcRouteFromCurBlock( data->model, (iOList)NULL,
                                                               data->schedule, &data->scheduleIdx,
                                                               data->next1Block->base.id( data->next1Block ),
                                                               data->loc );
      }
      else {
        /* next2Route already locked by second next option; adjust the schedule index... */
        data->scheduleIdx += 1;
      }

      if( wLoc.isusescheduletime( data->loc->base.properties( data->loc ) ) &&
          !checkScheduleTime( inst, data->schedule, data->scheduleIdx ) )
      {
        wait = True;
      }

      if( !wait && data->next2Route != NULL ) {
        /* evaluate direction */
        if( StrOp.equals( data->next2Route->getToBlock( data->next2Route ), data->next1Block->base.id(data->next1Block) ) )
          data->next2Block = data->model->getBlock( data->model, data->next1Route->getFromBlock( data->next2Route ) );
        else
          data->next2Block = data->model->getBlock( data->model, data->next2Route->getToBlock( data->next2Route ) );
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkScheduleEntryActions..." );
        if( checkScheduleEntryActions(inst) ) {
          /* wait in block if we have to swap placing... */
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Wait in block because the schedule entry wants a swap placing..." );
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
          resetNext2( (iOLcDriver)inst, True );
          wait = True;
        }
      }
      else if( wait ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
        resetNext2( (iOLcDriver)inst, True );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "End of schedule: STOP." );
        checkScheduleActions(inst, LC_ENTERBLOCK);
      }
    }

    if( data->next2Block != NULL ) {
      /* fix: if a loc is running, and the new destination is opposite, the loc should reject the new destination and stop. */
      Boolean dir = data->next2Route->getDirection( data->next2Route,
          					                                data->next1Block->base.id(data->next1Block), 
						                                        &data->next2RouteFromTo );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
          "loco direction for [%s] is [%s], route direction [%s]",
                     data->loc->getId( data->loc ), dir?"forwards":"reverse", data->next1RouteFromTo?"fromTo":"toFrom" );

      if( data->loc->getDir( data->loc ) != ( data->next1Route->isSwapPost( data->next1Route ) ? !dir : dir ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Destination is in opposite direction while running: Reject and wait in block." );

        if( data->next2Route != NULL && data->scheduleIdx > 0 ) {
          /* go one move back in the schedule */
          data->scheduleIdx--;
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Set schedule index back to [%d].", data->scheduleIdx );
        }

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block  ***** test with unlock flag *****" );
        resetNext2( (iOLcDriver)inst, True );
      }
      else if( initializeGroup( (iOLcDriver)inst, data->next2Block ) &&
               initializeDestination( (iOLcDriver)inst,
                                      data->next2Block,
                                      data->next2Route,
                                      data->next1Block,
                                      data->next2Route->isSwapPost( data->next2Route ) ? data->next2RouteFromTo : !data->next2RouteFromTo ) &&
               initializeSwap( (iOLcDriver)inst, data->next2Route) )
      {
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "Found destination for \"%s\": \"%s\" (from %s)",
            data->loc->getId( data->loc ), data->next2Block->base.id( data->next2Block ),
            data->next1Block->base.id( data->next1Block ) );
        data->state = LC_WAIT4EVENT;
        data->eventTimeout = 0;
        data->signalReset  = 0;

        wLoc.setmode( data->loc->base.properties( data->loc ), wLoc.mode_wait );
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                       "Setting state for \"%s\" from LC_ENTERBLOCK to LC_WAIT4EVENT.",
                       data->loc->getId( data->loc ) );

        checkRouteFunction(inst, data->next2Route, data->next2Block );
        /*
        if( StrOp.equals( wLoc.getV_hint( cmd), wLoc.min ) ||
            data->next2Route->hasThrownSwitch(data->next1Route) ) {
          data->next1Block->yellow( data->next1Block, True, !dir );
          data->next1Block->yellow( data->next1Block, False, !dir );
        }
        else {
          data->next1Block->green( data->next1Block, True, !dir );
          data->next1Block->green( data->next1Block, False, !dir );
        }
        */

        if( !data->gomanual ) {
          if( wBlock.getincline( bkprops ) == wBlock.incline_up &&
              data->direction == LC_DIR_FORWARDS &&
              !wLoc.isregulated( data->loc->base.properties( data->loc ) ) ) {
            wLoc.setV_hint( cmd, wLoc.climb );
          }
          else {
            wLoc.setV_hint( cmd, getBlockV_hint(inst, data->curBlock, False, data->next1Route ) );
          }
          wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
          data->loc->cmd( data->loc, cmd );
        }

      }
      else {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
        resetNext2( (iOLcDriver)inst, False );
      }
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
    resetNext2( (iOLcDriver)inst, True );
  }

  /* Wait in block or no new destination found. */
  if( data->next2Block == NULL ) {
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    /* set V_mid only if it is lower than the current velocity */
    const char* blockV_hint = getBlockV_hint(inst, data->next1Block, False, NULL );
    if( data->loc->compareVhint( data->loc, blockV_hint) == -1 )
      wLoc.setV_hint( cmd, blockV_hint );

    if( StrOp.equals(blockV_hint, wBlock.cruise) || StrOp.equals(blockV_hint, wBlock.max) )
      wLoc.setV_hint( cmd, wBlock.mid );

    if( data->next1Block != NULL ) {
      /* data->curBlock is set after event out_block:
         to be save we must check the data->next1Block here. */
      iONode destbkprops = (iONode)data->next1Block->base.properties( data->next1Block );

      /*
      data->next1Block->red( data->next1Block, True, !data->next2RouteFromTo );
      data->next1Block->red( data->next1Block, False, !data->next2RouteFromTo );
      */

      if( wBlock.getincline( destbkprops ) == wBlock.incline_up &&
          data->direction == LC_DIR_FORWARDS )
      {
        const char* blockV_hint = getBlockV_hint(inst, data->next1Block, False, NULL );
        if( data->loc->compareVhint( data->loc, blockV_hint) == -1 )
          wLoc.setV_hint( cmd, blockV_hint );
      }
    }
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                   "destBlock for \"%s\" is not set!",
                   data->loc->getId( data->loc ) );

    wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
    if( !data->gomanual ) {
      data->loc->cmd( data->loc, cmd );
    }
    else {
      /* delete un sended node */
      NodeOp.base.del(cmd);
    }
    data->state = LC_WAIT4EVENT;
    data->eventTimeout = 0;
    data->signalReset  = 0;
    wLoc.setmode( data->loc->base.properties( data->loc ), wLoc.mode_wait );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" from LC_ENTERBLOCK to LC_WAIT4EVENT.",
                   data->loc->getId( data->loc ) );
  }


  /* SECOND NEXT BLOCK */
  else if( data->secondnextblock || data->loc->trySecondNextBlock(data->loc) ) {
    /* try to reserve next3Block if the train does not have to wait in the next next2Block */
    reserveSecondNextBlock( (iOLcDriver)inst, data->gotoBlock, data->next2Block, data->next2Route,
                              &data->next3Block, &data->next3Route );

    if( data->next3Route != NULL ) {
      data->next3Route->getDirection( data->next3Route,
          data->next2Block->base.id(data->next2Block), &data->next3RouteFromTo );
    }
    /* if a second next block was found show the right signal aspect */
  }

  if( !re_enter )
    setSignals((iOLcDriver)inst, True);

}
