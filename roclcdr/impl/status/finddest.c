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


void statusFindDest( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);
  int scheduleIdx = data->scheduleIdx;
  Boolean mansignal = (data->curBlock->hasManualSignal(data->curBlock, False, False) != NULL ? True:False);

  /* Find a free destination. */
  if( data->schedule == NULL || StrOp.len( data->schedule ) == 0 ) {
    data->next1Block = data->model->findDest( data->model, data->loc->getCurBlock( data->loc ),
                                        NULL,
                                        data->loc, &data->next1Route, data->gotoBlock,
                                        data->opponly ? False:wLoc.istrysamedir( data->loc->base.properties( data->loc ) ),
                                        data->opponly | wLoc.istryoppositedir( (iONode)data->loc->base.properties( data->loc ) ),
                                        data->opponly ? False:(mansignal|wLoc.isforcesamedir( data->loc->base.properties( data->loc ) )),
                                        False, data->opponly ); /* currently is no prev route running */
  }
  else {
    Boolean wait = False;

    if( scheduleIdx == 0 && !data->model->isScheduleFree(data->model, data->schedule, data->loc->getId(data->loc)) ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,"schedule[%s] is not free2go", data->schedule);
      wait = True;
    }
    /* evaluate departure time */
    else if( wLoc.isusescheduletime( data->loc->base.properties( data->loc ) ) &&
        !checkScheduleTime( inst, data->schedule, data->prewaitScheduleIdx == -1 ? data->scheduleIdx:data->prewaitScheduleIdx ) ){
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
          "Waiting for schedule index[%d] (preWaitIdx=%d)",
          data->prewaitScheduleIdx == -1 ? data->scheduleIdx:data->prewaitScheduleIdx, data->prewaitScheduleIdx);
      wait = True;
    }

    if( !wait ) {
      /* find destination using schedule */
      if( data->prewaitScheduleIdx != -1 ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
            "reset schedule index from %d to %d (preWait)", data->scheduleIdx, data->prewaitScheduleIdx );
        data->scheduleIdx = data->prewaitScheduleIdx;
        data->prewaitScheduleIdx = -1;
      }
      data->next1Route = data->model->calcRouteFromCurBlock( data->model, (iOList)NULL, data->schedule, &data->scheduleIdx,
                                                      data->loc->getCurBlock( data->loc ), NULL, data->loc, False, False, &data->indelay );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reported schedule index=%d", data->scheduleIdx );
    }

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkScheduleEntryActions for entry %d...", scheduleIdx );
    if( checkScheduleEntryActions(inst, scheduleIdx) ) {
      /* wait in block if we have to swap placing... */
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "the schedule entry wants a swap placing" );
      data->loc->swapPlacing( data->loc, NULL, False );
    }

    if( !wait && data->next1Route != NULL ) {
      /* evaluate direction */
      if( StrOp.equals( data->next1Route->getToBlock( data->next1Route ), data->loc->getCurBlock( data->loc ) ) )
        data->next1Block = data->model->getBlock( data->model, data->next1Route->getFromBlock( data->next1Route ) );
      else
        data->next1Block = data->model->getBlock( data->model, data->next1Route->getToBlock( data->next1Route ) );
    }
    else if( wait ) {
      data->next1Block = NULL;
    }
    else {
      if( isScheduleEnd(inst) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "End of schedule: STOP." );
        if( checkScheduleEntryActions(inst, scheduleIdx) ) {
          /* wait in block if we have to swap placing... */
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "the schedule entry wants a swap placing" );
          data->loc->swapPlacing( data->loc, NULL, False );
        }
        checkScheduleActions(inst, LC_FINDDEST);
      }
    }
  }


  data->curBlock  = data->model->getBlock( data->model, data->loc->getCurBlock( data->loc ) );

  if( data->next1Block != NULL && data->curBlock == data->next1Block ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "new destination block equals current block: stop" );
    data->schedule = NULL;
    data->next1Block = NULL;
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
    resetNext2( (iOLcDriver)inst, True );
    data->next3Block = NULL;
    data->run = False;
  }

  if( data->next1Block != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "Found destination for \"%s\": \"%s\".",
        data->loc->getId( data->loc ), data->next1Block->base.id( data->next1Block ) );

    data->loc->informBlock( data->loc, data->next1Block->base.id(data->next1Block),
        data->curBlock->base.id(data->curBlock) );

    data->state = LC_INITDEST;
    data->loc->setMode(data->loc, wLoc.mode_auto);
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" from LC_FINDDEST to LC_INITDEST.",
                   data->loc->getId( data->loc ) );

  }
  else {
    data->state = LC_WAITBLOCK;
    data->prevState = LC_FINDDEST;
    data->loc->setMode(data->loc, wLoc.mode_wait);
    if( !data->warningnodestfound ) {
      data->warningnodestfound = True;
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                   "No destination found for [%s]; waiting...",
                   data->loc->getId( data->loc ) );
    }
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for [%s] from LC_FINDDEST to LC_WAITBLOCK.",
                   data->loc->getId( data->loc ) );
  }

}

