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



void eventIn( iOLcDriver inst, const char* blockId, iIBlockBase block, Boolean curBlockEvent, Boolean dstBlockEvent ) {
  iOLcDriverData data = Data(inst);

  Boolean newInEvent = False;
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
      newInEvent && dstBlockEvent && data->state == LC_WAIT4EVENT ) {
    data->state = LC_INBLOCK;
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" to LC_INBLOCK.",
                   data->loc->getId( data->loc ) );
    wLoc.setmode( data->loc->base.properties( data->loc ), wLoc.mode_auto );
    
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
    data->next1Route->unLock( data->next1Route, data->loc->getId( data->loc ) );
    data->next1Route = data->next2Route;
    data->next2Route = data->next3Route;
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for \"%s\" to LC_INBLOCK.",
                   data->loc->getId( data->loc ) );
  }
}
