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


void checkScheduleActions( iILcDriverInt inst, int state) {
  iOLcDriverData data = Data(inst);

  /* reset schedule index */
  data->scheduleIdx = 0;

  if( data->schedule != NULL ) {
    iONode sc = data->model->getSchedule( data->model, data->schedule );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "check schedule actions [%s]", data->schedule );

    if( sc != NULL ) {
      const char* scaction = wSchedule.getscaction(sc);
      iONode actionctrl = wSchedule.getactionctrl(sc);

      while( actionctrl != NULL ) {
        iOAction action = data->model->getAction(data->model, wActionCtrl.getid(actionctrl) );
        if( action != NULL ) {
          wActionCtrl.setlcid( actionctrl, data->loc->getId( data->loc ) );
          action->exec(action, actionctrl );
        }
        actionctrl = wSchedule.nextactionctrl(sc, actionctrl);
      };

      /* check for a next action */
      if( scaction != NULL && StrOp.len(scaction) > 0  ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "next schedule [%s] activated", scaction );
        /* set the schedule start time: */
        data->scheduletime = data->model->getTime( data->model );
        data->schedule = scaction;
        if( state == LC_FINDDEST )
          data->next1Block = NULL;

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
        resetNext2( (iOLcDriver)inst, True );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no new schedule" );
        data->schedule = NULL;
        if( state == LC_FINDDEST )
          data->next1Block = NULL;
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
        resetNext2( (iOLcDriver)inst, True );
        data->run = False;
      }

    }
    else {
      data->schedule = NULL;
      if( state == LC_FINDDEST )
        data->next1Block = NULL;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset next2Block" );
      resetNext2( (iOLcDriver)inst, True );
      data->run = False;
    }
  }
}

Boolean checkScheduleEntryActions( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);

  if( data->schedule != NULL ) {
    iONode sc = data->model->getSchedule( data->model, data->schedule );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "check schedule entry actions [%s:%d]",
        data->schedule, data->scheduleIdx );

    if( sc != NULL ) {
      iONode entry = NodeOp.getChild(sc,data->scheduleIdx-1);
      iONode actionctrl = wScheduleEntry.getactionctrl(entry);

      while( actionctrl != NULL ) {
        iOAction action = data->model->getAction(data->model, wActionCtrl.getid(actionctrl) );
        if( action != NULL ) {
          action->exec(action, actionctrl);
        }
        actionctrl = wSchedule.nextactionctrl(entry, actionctrl);
      };

      return wScheduleEntry.isswap(entry);
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



