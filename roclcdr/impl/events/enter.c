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


void eventEnter( iOLcDriver inst, const char* blockId, Boolean curBlockEvent, Boolean dstBlockEvent ) {
  iOLcDriverData data = Data(inst);

  Boolean newEnterEvent = False;
  if( data->preventer + data->ignevt < SystemOp.getTick() && StrOp.equals( blockId, data->preventerbkid ) ) {
    data->preventer = SystemOp.getTick();
    data->preventerbkid = blockId;
    newEnterEvent = True;
  }
  else if( !StrOp.equals( blockId, data->preventerbkid ) ) {
    data->preventer = SystemOp.getTick();
    data->preventerbkid = blockId;
    newEnterEvent = True;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                   "Ignoring enter_block event from %s; it came within %d ticks!", blockId, data->ignevt );
  }

  /* Train could have contacted both feedbacks. */
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                 "enter_block event for \"%s\" from \"%s\"...",
                 data->loc->getId( data->loc ), blockId );

  if( newEnterEvent ) {/* check if the exitblock or outblock state are able to get active: */
    Boolean dontcare = False;
    if( data->state == LC_GO || data->state == LC_PRE2GO || data->state == LC_CHECKROUTE )
      dontcare = True;

    if( dstBlockEvent && data->state == LC_EXITBLOCK ||
        dstBlockEvent && data->state == LC_OUTBLOCK ||
        dstBlockEvent && dontcare )
    {
      data->state = LC_ENTERBLOCK;
      data->loc->setMode(data->loc, wLoc.mode_auto);
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Setting state for \"%s\" to LC_ENTERBLOCK.",
                     data->loc->getId( data->loc ) );
    }
    else if( dstBlockEvent && data->state != LC_IDLE ) {
      /* Could be dirty wheels or a big gap between wheels. */
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Unexpected enter_block event for [%s] in [%s] with state [%d]...",
                     data->loc->getId( data->loc ), blockId, data->state );
    }
  }
  else {
    /* Exception! */
    /* ToDo: Could also happen when wagon wheels are dirty. */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                   "Unexpected (state=%d) enter_block event for \"%s\" from \"%s\"...",
                   data->state, data->loc->getId( data->loc ), blockId );

    if( curBlockEvent ) {
      /* An unknown loc is comming in!!! (Or train too long??!!)*/
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                     "Unexpected enter_block event for \"%s\" from \"%s\"...",
                     data->loc->getId( data->loc ), blockId );
      if( newEnterEvent ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
                       "Stopping because a new enter_block event came after more than a second! loc=\"%s\" block=\"%s\"...",
                       data->loc->getId( data->loc ), blockId );
        data->model->stop( data->model );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                       "Not Stopping because a new enter_block event came within a second! loc=\"%s\" block=\"%s\"...",
                       data->loc->getId( data->loc ), blockId );
      }
    }
  }

}

