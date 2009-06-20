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



#include "rocrail/public/model.h"

#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Link.h"


void resetSignals(iOLcDriver inst ) {
  iOLcDriverData data = Data(inst);
  Boolean reverse = False;

  /* signal current block */
  if( data->curBlock != NULL ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset signals of current block..." );
    data->curBlock->red( data->curBlock, True, !data->next1RouteFromTo );
    data->curBlock->red( data->curBlock, False, !data->next1RouteFromTo );
  }
}



Boolean setSignals(iOLcDriver inst, Boolean onEnter ) {
  iOLcDriverData data = Data(inst);
  Boolean semaphore = False;
  Boolean reverse = False;

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "setting signals..." );
  listBlocks(inst);

  /* set signal current block on enter */
  if( onEnter && data->curBlock != NULL ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "setting signals for curBlock to default aspect" );
    data->curBlock->setDefaultAspect( data->curBlock );
    data->curBlock->setDefaultAspect( data->curBlock );
  }

  /* set signal current block */
  else if( data->curBlock != NULL && data->next1Block != NULL && data->next2Block != NULL &&
      data->curBlock != data->next1Block && data->next1Block != data->next2Block )
  {
    if( data->next1Route != NULL && data->next1Route->hasThrownSwitch(data->next1Route) ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
          "setting signals for curBlock to white: thrown switches in route [%s], reverse[%s]",
          data->next1Route->getId(data->next1Route), data->next1RouteFromTo?"false":"true" );
      semaphore |= data->curBlock->white( data->curBlock, True, !data->next1RouteFromTo );
      semaphore |= data->curBlock->white( data->curBlock, False, !data->next1RouteFromTo );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
          "setting signals for curBlock to green, reverse[%s]", data->next1RouteFromTo?"false":"true");
      semaphore |= data->curBlock->green( data->curBlock, True, !data->next1RouteFromTo );
      semaphore |= data->curBlock->green( data->curBlock, False, !data->next1RouteFromTo );
    }
  }

  /* no second next block available: YELLOW */
  else if( data->curBlock != NULL && data->next1Block != NULL && data->next2Block == NULL &&
      data->curBlock != data->next1Block )
  {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
        "setting signals for curBlock to yellow%s, reverse[%s]",
        data->greenaspect ? " (force green)":"", data->next1RouteFromTo?"false":"true");

    if( data->greenaspect ) {
      semaphore |= data->curBlock->green( data->curBlock, True, !data->next1RouteFromTo );
      semaphore |= data->curBlock->green( data->curBlock, False, !data->next1RouteFromTo );
    }
    else {
      semaphore |= data->curBlock->yellow( data->curBlock, True, !data->next1RouteFromTo );
      semaphore |= data->curBlock->yellow( data->curBlock, False, !data->next1RouteFromTo );
    }
  }

  /* no next block available: RED */
  else if( data->curBlock != NULL )
  {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
        "setting signals for curBlock to red, reverse[%s]", data->next1RouteFromTo?"false":"true");
    semaphore |= data->curBlock->red( data->curBlock, True, !data->next1RouteFromTo );
    semaphore |= data->curBlock->red( data->curBlock, False, !data->next1RouteFromTo );
  }



  /* signal next1Block */
  if( data->next1Block != NULL && data->next2Block != NULL && data->next3Block != NULL &&
      data->next1Block != data->next2Block && data->next2Block != data->next3Block )
  {
    if( data->next2Route != NULL && data->next2Route->hasThrownSwitch(data->next2Route) ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "setting signals for next1Block to white: thrown switches in route [%s]",
                   data->next2Route->getId(data->next2Route) );
      data->next1Block->white( data->next1Block, True, !data->next2RouteFromTo );
      data->next1Block->white( data->next1Block, False, !data->next2RouteFromTo );
    }
    else {
      data->next1Block->green( data->next1Block, True, !data->next2RouteFromTo );
      data->next1Block->green( data->next1Block, False, !data->next2RouteFromTo );
    }
  }
  else if( data->next1Block != NULL && data->next2Block != NULL &&
      data->next1Block != data->next2Block )
  {
    if( data->greenaspect ) {
      data->next1Block->green( data->next1Block, True, !data->next2RouteFromTo );
      data->next1Block->green( data->next1Block, False, !data->next2RouteFromTo );
    }
    else {
      data->next1Block->yellow( data->next1Block, True, !data->next2RouteFromTo );
      data->next1Block->yellow( data->next1Block, False, !data->next2RouteFromTo );
    }
  }
  else if( data->next1Block != NULL )
  {
    data->next1Block->red( data->next1Block, True, !data->next1RouteFromTo );
    data->next1Block->red( data->next1Block, False, !data->next1RouteFromTo );
  }

  /* signal next2Block */
  if( data->next2Block != NULL && data->next3Block != NULL &&
      data->next2Block != data->next3Block )
  {
    if( data->greenaspect ) {
      data->next2Block->green( data->next2Block, True, !data->next3RouteFromTo );
      data->next2Block->green( data->next2Block, False, !data->next3RouteFromTo );
    }
    else {
      data->next2Block->yellow( data->next2Block, True, !data->next3RouteFromTo );
      data->next2Block->yellow( data->next2Block, False, !data->next3RouteFromTo );
    }
  }
  else if( data->next2Block != NULL )
  {
    data->next2Block->red( data->next2Block, True, !data->next2RouteFromTo );
    data->next2Block->red( data->next2Block, False, !data->next2RouteFromTo );
  }

  return semaphore;
}



