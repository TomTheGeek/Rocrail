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
#include "rocrail/wrapper/public/Tour.h"



void statusIdle( iILcDriverInt inst, Boolean reverse ) {
  iOLcDriverData data = Data(inst);

  /* Waiting for run-flag. */
  if( data->run && !data->reqstop && data->loc->getCurBlock( data->loc ) != NULL &&
      data->curBlock->getWait(data->curBlock, data->loc, reverse ) != -1 &&
      data->curBlock->isDepartureAllowed(data->curBlock, data->loc->getId(data->loc)) )
  {

    data->state = LC_FINDDEST;
    data->loc->setMode(data->loc, wLoc.mode_auto);

    /* Check if we are on a tour: */
    if( data->tour != NULL && (data->scheduleended || data->tourIdx == 0) ) {
      iONode tour = data->model->getTour(data->model, data->tour);
      data->scheduleended = False;
      if( tour != NULL ) {
        iOStrTok tok = StrTokOp.inst(wTour.getschedules(tour), ',');
        int cnt = StrTokOp.countTokens(tok);
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,"tour entries: cnt=%d tourIdx=%d", cnt, data->tourIdx);
        if( cnt > data->tourIdx ) {
          const char* scid = NULL;
          int scidx = 0;
          while( StrTokOp.hasMoreTokens(tok) ) {
            if( scidx == data->tourIdx ) {
              scid = StrTokOp.nextToken(tok);
              break;
            }
            else {
              /* skip */
              StrTokOp.nextToken(tok);
            }
            scidx++;
          }
          if( scid != NULL ) {
            iONode schedule = data->model->getSchedule( data->model, scid );
            if( schedule != NULL ) {
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                  "tour [%s] entry [%d][%s]", wTour.getid(tour), data->tourIdx, scid );
              LcDriverOp.useschedule( inst, wSchedule.getid(schedule) );
              data->tourIdx++;
            }
          }
          else {
            /* tour end */
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                "tour [%s] has unexpectedly ended", wTour.getid(tour));
            data->tour = NULL;
            data->tourIdx = 0;
            data->run = False;
            data->state = LC_IDLE;
          }
        }
        else {
          /* tour end */
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,"tour [%s] has ended", wTour.getid(tour));
          data->tour = NULL;
          data->tourIdx = 0;
          data->run = False;
          data->state = LC_IDLE;
        }
        StrTokOp.base.del(tok);
      }
    }

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" from LC_IDLE to LC_FINDDEST.",
                   data->loc->getId( data->loc ) );

    /* reset blocks and routes */
    data->next1Block = NULL;
    data->next1Route = NULL;
    data->next2Block = NULL;
    data->next2Route = NULL;
    data->next3Block = NULL;
    data->next3Route = NULL;

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
        "Finding destination for \"%s\", current block \"%s\"...",
        data->loc->getId( data->loc ), data->loc->getCurBlock( data->loc ) );
  }
  else if( data->run && data->reqstop && data->curBlock != NULL ) {
    if( !data->curBlock->isDepartureAllowed(data->curBlock, data->loc->getId(data->loc)) ) {
      data->state = LC_WAITBLOCK;
      data->prevState = LC_IDLE;
      data->loc->setMode(data->loc, wLoc.mode_wait);
    }
    if( data->reqstop ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,"stop requested");
      data->reqstop = False;
      data->run = False;
      data->warningnodestfound = False;
      data->loc->setMode(data->loc, wLoc.mode_idle);
    }

  }
  else {
    /* DEBUG */
    /*
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
        "idle: run=%d block=%s",
        data->run,
        data->loc->getCurBlock( data->loc )==NULL?"":data->loc->getCurBlock( data->loc ));
    ThreadOp.sleep(1000);
    */
  }
}
