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

void checkRouteFunction( iILcDriverInt inst, iORoute route, iIBlockBase block ) {
  iOLcDriverData data = Data(inst);
  int activationtime = 0;
  const char* deactivationevent = "none";
  int fnaction = route->getFunction(route, &activationtime, &deactivationevent);
  const char* blockid = block->base.id(block);

  /* check for a function command */
  if( fnaction >= 0 ) {
    iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "function [%d] activated, event=[%s] block=[%s] time[%d]",
        fnaction, deactivationevent, blockid, activationtime );
    wFunCmd.setid( cmd, data->loc->getId(data->loc) );
    data->loc->getFunctionStatus(data->loc, cmd);

    wFunCmd.setf0( cmd, fnaction==0?True:wFunCmd.isf0( cmd ) );
    wFunCmd.setf1( cmd, fnaction==1?True:wFunCmd.isf1( cmd ) );
    wFunCmd.setf2( cmd, fnaction==2?True:wFunCmd.isf2( cmd ) );
    wFunCmd.setf3( cmd, fnaction==3?True:wFunCmd.isf3( cmd ) );
    wFunCmd.setf4( cmd, fnaction==4?True:wFunCmd.isf4( cmd ) );
    wFunCmd.setf5( cmd, fnaction==5?True:wFunCmd.isf5( cmd ) );
    wFunCmd.setf6( cmd, fnaction==6?True:wFunCmd.isf6( cmd ) );
    wFunCmd.setf7( cmd, fnaction==7?True:wFunCmd.isf7( cmd ) );
    wFunCmd.setf8( cmd, fnaction==8?True:wFunCmd.isf8( cmd ) );
    wFunCmd.setf9( cmd, fnaction==9?True:wFunCmd.isf9( cmd ) );
    wFunCmd.setf10( cmd, fnaction==10?True:wFunCmd.isf10( cmd ) );
    wFunCmd.setf11( cmd, fnaction==11?True:wFunCmd.isf11( cmd ) );
    wFunCmd.setf12( cmd, fnaction==12?True:wFunCmd.isf12( cmd ) );

    if( !StrOp.equals( "none", deactivationevent) ) {
      activationtime = 0;
    }

    wFunCmd.settimedfn( cmd, fnaction );
    wFunCmd.settimer( cmd, activationtime );
    wFunCmd.setevent( cmd, deactivationevent );
    wFunCmd.seteventblock( cmd, block->base.id(block) );
    data->loc->cmd( data->loc, cmd);
  }


}




