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
#include <time.h>

#include "roclcdr/impl/lcdriver_impl.h"

#include "roclcdr/impl/tools/tools.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"



#include "rocrail/public/model.h"
#include "rocrail/public/action.h"

#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Link.h"
#include "rocrail/wrapper/public/ActionCtrl.h"



static Boolean isHourlyInRange(iILcDriverInt inst, iONode schedule) {
  iOLcDriverData data = Data(inst);
  Boolean inRange  = True;
  int     fromhour = wSchedule.getfromhour(schedule);
  int     tohour   = wSchedule.gettohour(schedule);
  int     hours    = 0;
  int     mins     = 0;

  long modeltime = data->model->getTime( data->model );
  struct tm* ltm;

  ltm = localtime( &modeltime );
  hours = ltm->tm_hour;
  mins  = ltm->tm_min;

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "modeltime %02d:%02d (%ld)",
      hours, mins, modeltime );

  if( hours < fromhour || tohour < hours || tohour == hours ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
        "current hour, %d, is no longer in the hourly range from %d to %d",
        hours, fromhour, tohour );
    inRange = False;
  }


  return inRange;
}



void checkScheduleActions( iILcDriverInt inst, int state) {
  iOLcDriverData data = Data(inst);

  /* reset schedule index */
  data->scheduleIdx = 0;
  data->prewaitScheduleIdx = -1;
  data->scheduleCycle++;

  if( data->schedule != NULL ) {
    iONode sc = data->model->getSchedule( data->model, data->schedule );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "check schedule actions [%s]", data->schedule );

    if( sc != NULL ) {
      const char* scaction = wSchedule.getscaction(sc);
      int           cycles = wSchedule.getcycles(sc);
      iONode    actionctrl = wSchedule.getactionctrl(sc);

      while( actionctrl != NULL ) {
        iOAction action = data->model->getAction(data->model, wActionCtrl.getid(actionctrl) );
        if( action != NULL ) {
          wActionCtrl.setlcid( actionctrl, data->loc->getId( data->loc ) );
          action->exec(action, actionctrl );
        }
        actionctrl = wSchedule.nextactionctrl(sc, actionctrl);
      };

      if( wSchedule.gettimeprocessing(sc) == wSchedule.time_hourly && isHourlyInRange(inst, sc) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "hourly schedule is recycled", scaction );
        /* set the schedule start time: */
        data->scheduletime = data->model->getTime( data->model );
        data->scheduleCycle = 0;

        if( state == LC_FINDDEST )
          data->next1Block = NULL;

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
        resetNext2( (iOLcDriver)inst, True );
      }
      else if( cycles > 0 && data->scheduleCycle < cycles ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "next schedule cycle[%d] of %d activated", data->scheduleCycle, cycles );
        if( state == LC_FINDDEST )
          data->next1Block = NULL;

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
        resetNext2( (iOLcDriver)inst, True );
      }
      /* check for a next action */
      else if( scaction != NULL && StrOp.len(scaction) > 0  ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "next schedule [%s] activated", scaction );
        /* set the schedule start time: */
        data->scheduletime = data->model->getTime( data->model );
        data->scheduleCycle = 0;
        data->schedule = scaction;
        if( state == LC_FINDDEST )
          data->next1Block = NULL;

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
        resetNext2( (iOLcDriver)inst, True );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no new schedule" );
        data->schedule = NULL;
        data->scheduleCycle = 0;
        if( state == LC_FINDDEST )
          data->next1Block = NULL;
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
        resetNext2( (iOLcDriver)inst, True );
        if( data->tour == NULL ) {
          data->run = False;
        }
      }

    }
    else {
      data->schedule = NULL;
      data->scheduleCycle = 0;
      if( state == LC_FINDDEST )
        data->next1Block = NULL;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
      resetNext2( (iOLcDriver)inst, True );
      data->run = False;
    }
  }
}

Boolean checkScheduleEntryActions( iILcDriverInt inst, int index ) {
  iOLcDriverData data = Data(inst);
  int scheduleIdx = (index == -1 ? data->scheduleIdx:index);

  if( data->schedule != NULL ) {
    iONode sc = data->model->getSchedule( data->model, data->schedule );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "check schedule entry actions [%s:%d]",
        data->schedule, scheduleIdx );

    if( sc != NULL && scheduleIdx < NodeOp.getChildCnt(sc)) {
      iONode entry = NodeOp.getChild(sc,scheduleIdx);
      if( entry != NULL ) {
        iONode actionctrl = wScheduleEntry.getactionctrl(entry);

        while( actionctrl != NULL ) {
          iOAction action = data->model->getAction(data->model, wActionCtrl.getid(actionctrl) );
          if( action != NULL ) {
            wActionCtrl.setlcid( actionctrl, data->loc->getId( data->loc ) );
            action->exec(action, actionctrl);
          }
          actionctrl = wSchedule.nextactionctrl(entry, actionctrl);
        };

        return wScheduleEntry.isswap(entry);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "schedule index %d is out of bounds for schedule %s",
          scheduleIdx, data->schedule );
    }
  }
  return False;
}


/**
 * check the departure time
 * return True if the train should go
 */
Boolean checkScheduleTime( iILcDriverInt inst, const char* scheduleID, int scheduleIdx ) {
  iOLcDriverData data = Data(inst);
  Boolean go = False;
  iONode schedule = data->model->getSchedule( data->model, scheduleID );

  if( schedule != NULL ) {
    int idx = 0;
    int timeprocessing = wSchedule.gettimeprocessing(schedule);
    int timeframe      = wSchedule.gettimeframe(schedule);
    int fromhour       = wSchedule.getfromhour(schedule);
    int tohour         = wSchedule.gettohour(schedule);

    iONode entry = wSchedule.getscentry( schedule );

    /* check if the schedule index is correct: */
    while( entry != NULL ) {
      if( idx == scheduleIdx ) {
        long modeltime = data->model->getTime( data->model );
        struct tm* ltm;
        int modelminutes    = 0;
        int scheduleminutes = 0;
        int mins  = 0;
        int hours = 0;

        if( timeprocessing == wSchedule.time_relative ) {
          modeltime = modeltime - data->scheduletime;
          modelminutes = modeltime / 60;
          mins  = modelminutes % 60;
          hours = modelminutes / 60;
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "using relative time: modeltime=%d", modeltime );
        }
        else {
          ltm = localtime( &modeltime );
          hours = ltm->tm_hour;
          mins  = ltm->tm_min;
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "using real time: modeltime=%d", modeltime );
        }

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "modeltime %02d:%02d (%ld)",
            hours, mins, modeltime );

        scheduleminutes = wScheduleEntry.gethour(entry) * 60 + wScheduleEntry.getminute(entry);

        if(timeprocessing == wSchedule.time_hourly ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "using hourly timing" );
          /* processing hourly timing */
          modelminutes = mins;
          if( hours < fromhour || tohour < hours ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                "current hour, %d, is not in the hourly range from %d to %d",
                hours, fromhour, tohour );
            scheduleminutes += 60;
          }
          else if( modelminutes > scheduleminutes && modelminutes - scheduleminutes > timeframe ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                "diff between schedule[%d] and model[%d] time is bigger then the allowed frame of %d; force wait for next hour...",
                scheduleminutes, modelminutes, timeframe );
            scheduleminutes += 60;
          }
        }
        else {
          modelminutes = hours * 60 + mins;
        }


        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
            "check departure time schedule=%d model=%d index=%d",
            scheduleminutes, modelminutes, scheduleIdx );

        /* compare clock with departure time */
        if( scheduleminutes <= modelminutes ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
              "train must leave now %d <= %d", scheduleminutes, modelminutes );
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
              "train is delayed by %d minutes", modelminutes - scheduleminutes );
          go = True;
        }
        else {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
              "train must wait %d > %d", scheduleminutes, modelminutes );
        }

        break;
      }
      idx++;
      entry = wSchedule.nextscentry( schedule, entry );
    };

  }
  else
    go = True;

  return go;
}

Boolean isScheduleEnd( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);
  iONode sc = data->model->getSchedule( data->model, data->schedule );
  if( sc != NULL ) {
    int nrEntries = 0;
    iONode scEntry = wSchedule.getscentry(sc);
    while( scEntry != NULL ) {
      nrEntries++;
      scEntry = wSchedule.nextscentry(sc, scEntry);
    };
    if( data->scheduleIdx >= nrEntries ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
          "end of schedule[%s] detected; entries=%d index=%d", data->schedule, nrEntries, data->scheduleIdx);
      data->scheduleended = True; /* flag for signaling the tour for the next schedule */
      return True;
    }
  }
  return False;
}



