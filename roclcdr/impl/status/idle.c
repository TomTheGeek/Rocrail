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



void statusIdle( iILcDriverInt inst, Boolean reverse ) {
  iOLcDriverData data = Data(inst);

  /* Waiting for run-flag. */
  if( data->run && !data->reqstop && data->loc->getCurBlock( data->loc ) != NULL &&
      data->curBlock->getWait(data->curBlock, data->loc, reverse ) != -1 )
  {

    data->state = LC_FINDDEST;
    data->loc->setMode(data->loc, wLoc.mode_auto);
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
  else if( data->run && data->reqstop ) {
    if( data->reqstop ) {
      data->reqstop = False;
      data->run = False;
      data->warningnodestfound = False;
      data->loc->setMode(data->loc, wLoc.mode_idle);
    }

  }
}
