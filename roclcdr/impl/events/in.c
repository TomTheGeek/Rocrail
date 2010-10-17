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



void eventIn( iOLcDriver inst, const char* blockId, iIBlockBase block, Boolean curBlockEvent, Boolean dstBlockEvent, Boolean shortIn ) {
  iOLcDriverData data = Data(inst);

  Boolean newInEvent = False;

  if( shortIn && wLoc.isshortin( data->loc->base.properties( data->loc ) ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "using shortin_block event for \"%s\" from \"%s\"...",
                   data->loc->getId( data->loc ), blockId );
  }
  else if( shortIn ) {
    /* ignore */
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "ignoring shortin_block event for \"%s\" from \"%s\"...",
                   data->loc->getId( data->loc ), blockId );
    return;
  }


  if( data->previn + data->ignevt < SystemOp.getTick() && StrOp.equals( blockId, data->previnbkid ) ) {
    data->previn = SystemOp.getTick();
    data->previnbkid = blockId;
    newInEvent = True;
  }
  else if( !StrOp.equals( blockId, data->previnbkid ) ) {
    data->previn = SystemOp.getTick();
    data->previnbkid = blockId;
    newInEvent = True;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                   "Ignoring in_block event from %s; it came within %d ticks!", blockId, data->ignevt );
  }

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                 "in_block event for \"%s\" from \"%s\"...",
                 data->loc->getId( data->loc ), blockId );

  if( data->next1Route == NULL ) {
    /* An unknown loc is comming in!!! Or some feedback noise... */
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Unexpected in_block event for \"%s\" from \"%s\"...",
                   data->loc->getId( data->loc ), blockId );
    /*AppOp.stop(  );*/
    return;
  }

  if( newInEvent && dstBlockEvent && data->state == LC_ENTERBLOCK ||
      newInEvent && dstBlockEvent && data->state == LC_RE_ENTERBLOCK ||
      newInEvent && dstBlockEvent && data->state == LC_WAIT4EVENT ) {
    data->state = LC_INBLOCK;
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" to LC_INBLOCK.",
                   data->loc->getId( data->loc ) );
    data->loc->setMode(data->loc, wLoc.mode_auto);

    /* Check wheel counters */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "wheel count %s[%d], %s[%d] ",
        data->curBlock->base.id( data->curBlock ), data->curBlock->getWheelCount(data->curBlock),
        data->next1Block->base.id( data->next1Block ), data->next1Block->getWheelCount(data->next1Block) );
    if( data->curBlock->getWheelCount(data->curBlock) > 0 && data->next1Block->getWheelCount(data->next1Block) > 0  ) {
      if( data->curBlock->getWheelCount(data->curBlock) == data->next1Block->getWheelCount(data->next1Block) ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "wheel count match %s=%s [%d]",
            data->curBlock->base.id( data->curBlock ), data->next1Block->base.id( data->next1Block ), data->curBlock->getWheelCount(data->curBlock) );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "wheel count does not match %s[%d] != %s[%d] ",
            data->curBlock->base.id( data->curBlock ), data->curBlock->getWheelCount(data->curBlock),
            data->next1Block->base.id( data->next1Block ), data->next1Block->getWheelCount(data->next1Block) );
        data->state = LC_IDLE;
        data->run = False;
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                       "Setting state for \"%s\" to LC_IDLE and stop running auto mode.",
                       data->loc->getId( data->loc ) );
        data->loc->setMode(data->loc, wLoc.mode_idle);
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setV( cmd, 0 );
        wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
        data->loc->cmd( data->loc, cmd );
      }
    }

    /* continue if not set to IDLE state */
    if( data->state != LC_IDLE ) {

      /* unlink-up after inblock event */
      data->next1Block->unLink( data->next1Block );

      if( data->next2Block == NULL || (data->next2Block != NULL && data->next2Block != data->curBlock) ) {
        data->curBlock->unLock( data->curBlock, data->loc->getId( data->loc ) );
      }
      else {
        data->curBlock->resetTrigs( data->curBlock );
      }
      data->curBlock = data->next1Block;
      data->loc->setCurBlock( data->loc, data->curBlock->base.id( data->curBlock ) );

      /**/
      /*
      data->loc->setCurBlock( data->loc, blockId );
      */

      block->inBlock( block, data->loc->getId( data->loc ) );

      /* free the block group from the previous block */
      initializeGroup(inst, NULL, block);

      {
        /*
         * unlock the previous route regarding reserved blocks
         */
        const char* resblocks[4] = {NULL, NULL, NULL, NULL};
        if( data->next1Block != NULL ) {
          resblocks[0] = data->next1Block->base.id(data->next1Block);
          if( data->next2Block != NULL ) {
            resblocks[1] = data->next2Block->base.id(data->next2Block);
            if( data->next3Block != NULL )
              resblocks[2] = data->next3Block->base.id(data->next3Block);
          }
        }
        data->next1Route->unLock( data->next1Route, data->loc->getId( data->loc ), resblocks, True );
      }

      if( data->next1Block != NULL ) {
        if( StrOp.equals(data->next1Block->base.id(data->next1Block), data->next1Route->getToBlock(data->next1Route)) )
          data->loc->setBlockEnterSide(data->loc, data->next1Route->getToBlockSide(data->next1Route), data->next1Route->getToBlock(data->next1Route) );
        else
          data->loc->setBlockEnterSide(data->loc, data->next1Route->getFromBlockSide(data->next1Route), data->next1Route->getFromBlock(data->next1Route) );
      }

      if ( data->next1Route->isSwapPost( data->next1Route ) ) {
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );

        /* swap post route */
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "swap placing post route %s", data->next1Route->getId( data->next1Route ));
        data->loc->swapPlacing( data->loc, NULL, False );

        wLoc.setdir( cmd, !data->loc->getDir( data->loc) );
        data->loc->cmd( data->loc, cmd);
      }

      data->next1Route = data->next2Route;
      data->next2Route = data->next3Route;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Setting state for \"%s\" to LC_INBLOCK.",
                     data->loc->getId( data->loc ) );

      /* swap the loc placing to run backwards in the default direction (to use in terminal stations)*/
      if( data->curBlock->isTerminalStation( data->curBlock ) ) {
        /* only swap after the IN block event! */
        data->loc->swapPlacing( data->loc, NULL, False );

         if( data->stopnonecommuter &&
             !wLoc.iscommuter( data->loc->base.properties(data->loc)) ) {
            /* Switch to manual mode: */
            data->loc->stop( data->loc, False );
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                               "stop in terminal for [%s] (not a commuter train)", data->loc->getId( data->loc ));
         }

      }
    }

  }
  else {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "unexpected IN event for [%s], state=[%d]",
                   data->loc->getId( data->loc ), data->state );
  }
}
