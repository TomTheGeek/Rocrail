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



void statusPre2Go( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);

  if( data->next1Block == NULL || data->curBlock == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "blocks are reseted while running!" );
    return;
  }

  /* if the destination block hast an enter2route event set state to LC_ENTER */
  if( data->next1Block->hasEnter2Route( data->next1Block, data->curBlock->base.id(data->curBlock) ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "**enter2route** event for block [%s]",
                 data->next1Block->base.id(data->next1Block) );
    data->state = LC_ENTERBLOCK;
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Setting state for [%s] from LC_GO to LC_ENTERBLOCK.",
                   data->loc->getId( data->loc ) );
  }
  else {
    data->state = LC_GO;
  }
}


void statusGo( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);

}
