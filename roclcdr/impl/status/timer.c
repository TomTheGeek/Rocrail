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



void statusTimer( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);

  if( data->timer == -1 ) {
    /* handle manual operated signal */
    if( !data->curBlock->wait(data->curBlock, data->loc ) ) {
      data->timer = 0;
    }
  }

  if( data->timer == 0 || !data->run || data->reqstop ) {

    /* swap the loc placing to run backwards in the default direction (to use in terminal stations)*/
    if( data->curBlock->isTerminalStation( data->curBlock ) ) {
      /* only swap after the IN block event! */
      /* BUG: 367452
      if( data->swapBlock != data->curBlock ) {
        data->loc->swapPlacing( data->loc );
        data->swapBlock = data->curBlock;
      }
      */
    }

    if( data->reqstop ) {
      data->reqstop = False;
      data->run = False;
      data->warningnodestfound = False;
    }

    data->state = LC_IDLE;
    data->loc->setMode(data->loc, wLoc.mode_idle);
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" from LC_TIMER to LC_IDLE.",
                   data->loc->getId( data->loc ) );

    if( data->next1Block != NULL )
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "next1Block for [%s] is [%s]",
                     data->loc->getId( data->loc ), data->next1Block->base.id(data->next1Block) );
    if( data->next2Block != NULL )
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "next2Block for [%s] is [%s]",
                     data->loc->getId( data->loc ), data->next2Block->base.id(data->next2Block) );
    if( data->next3Block != NULL )
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "next3Block for [%s] is [%s]",
                     data->loc->getId( data->loc ), data->next3Block->base.id(data->next3Block) );

    /* Correct the direction for helping the P50 interface. */
    /*
    if( !data->loc->getDir( data->loc ) ) {
      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setdir( cmd, True );
      wLoc.setV( cmd, 0 );
      data->loc->cmd( data->loc, cmd );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting direction for \"%s\" back to default.",
                   data->loc->getId( data->loc ) );
    }
    */
  }
  else {
    if( data->timer > 0 )
      data->timer--;
  }
}
