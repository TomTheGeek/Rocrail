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



void statusPre2In( iILcDriverInt inst ) {
  iOLcDriverData data = Data(inst);

  /* set velocitiy to V_min if the train has to wait */
  if( data->next2Block == NULL ) {
    if( !data->gomanual ) {
      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setV_hint( cmd, wLoc.min );
      wLoc.setdir( cmd, wLoc.isdir( data->loc->base.properties( data->loc ) ) );
      data->loc->cmd( data->loc, cmd );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Setting velocity for \"%s\" to V_Min",
                     data->loc->getId( data->loc ) );
    }
  }
  data->state = LC_WAIT4EVENT;
  data->eventTimeout = 0;
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                 "Setting state for \"%s\" from LC_PRE2INBLOCK to LC_WAIT4EVENT.",
                 data->loc->getId( data->loc ) );
}
