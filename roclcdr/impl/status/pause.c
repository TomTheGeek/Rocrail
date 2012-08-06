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



void statusPause( iILcDriverInt inst, Boolean reverse ) {
  iOLcDriverData data = Data(inst);
  Boolean oppwait = True;

  if( data->pause == -1 ) {
    /* handle manual operated signal */
    if( !data->curBlock->wait(data->curBlock, data->loc, reverse, &oppwait ) ) {
      data->pause = 0;
      data->state = LC_IDLE;
      data->loc->setMode(data->loc, wLoc.mode_idle);
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Setting state for \"%s\" from LC_PAUSE to LC_IDLE for manual signal.",
                     data->loc->getId( data->loc ) );
    }
  }
  else if( data->pause == 0 ) {
    data->state = LC_IDLE;
    data->loc->setMode(data->loc, wLoc.mode_idle);
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" from LC_PAUSE to LC_IDLE.",
                   data->loc->getId( data->loc ) );
  }
  else if( data->pause > 0 )
    data->pause--;
}
