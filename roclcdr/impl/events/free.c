/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

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
#include "roclcdr/impl/events/events.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"



#include "rocrail/public/model.h"

#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Block.h"

void eventFree( iOLcDriver inst, const char* blockId, iIBlockBase block, Boolean curBlockEvent, Boolean dstBlockEvent ) {
  iOLcDriverData data = Data(inst);

  if( wLoc.isfreeblockonenter(data->loc->base.properties(data->loc)) && data->next1Block->isFreeBlockOnEnter(data->next1Block) )
  {
    Boolean dontcare = False;
    if( data->state == LC_GO || data->state == LC_PRE2GO )
      dontcare = True;

    if( (dstBlockEvent && data->state == LC_EXITBLOCK) || (dstBlockEvent && data->state == LC_OUTBLOCK) || (dstBlockEvent && dontcare) )
    {
      if( !data->didFree ) {
        TraceOp.trc(name, TRCLEVEL_USER1, __LINE__, 9999,
            "Free previous block on free for [%s] in [%s] with state [%d]", data->loc->getId(data->loc), blockId, data->state);
        freePrevBlock(inst, block);
        data->didFree = True;
      }
    }
  }

}
