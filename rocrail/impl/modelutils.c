/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

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

#include "rocrail/impl/modelutils_impl.h"

#include "rocrail/public/app.h"

#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/BlockList.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Track.h"
#include "rocrail/wrapper/public/TrackList.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/SwitchList.h"
#include "rocrail/wrapper/public/SwitchCmd.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/ModPlan.h"
#include "rocrail/wrapper/public/Module.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/RouteList.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/SignalList.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/Stage.h"
#include "rocrail/wrapper/public/Ctrl.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/ActionCond.h"
#include "rocrail/wrapper/public/Location.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/StageList.h"
#include "rocrail/wrapper/public/StageSection.h"
#include "rocrail/wrapper/public/Exception.h"
#include "rocrail/wrapper/public/AnaOpt.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/TTTrack.h"
#include "rocrail/wrapper/public/ZLevel.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"
#include "rocrail/wrapper/public/Car.h"
#include "rocrail/wrapper/public/Waybill.h"
#include "rocrail/wrapper/public/Operator.h"
#include "rocrail/wrapper/public/Tour.h"
#include "rocrail/wrapper/public/Link.h"
#include "rocrail/wrapper/public/Booster.h"
#include "rocrail/wrapper/public/MVTrack.h"
#include "rocrail/wrapper/public/Text.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SwitchCmd.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/SystemActions.h"
#include "rocrail/wrapper/public/ActionList.h"
#include "rocrail/wrapper/public/TurntableList.h"
#include "rocrail/wrapper/public/TextList.h"


#include "rocs/public/mem.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOModelUtilsData data = Data(inst);
    /* Cleanup data->xxx members...*/
    
    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  return;
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  return;
}

static char* __toString( void* inst ) {
  return NULL;
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  return NULL;
}

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OModelUtils ----- */
/*
Route:
-> id( routeids( tk( tklist ) ) )
-> id( routeids( sw( swlist ) ) )
-> id( routeids( sg( sglist ) ) )
-> byroute( fbevent( bk( bklist ) ) )
... (Actions !)

Feedback: =>
-> id( fbevent( bk( bklist ) ) )
-> id( fbevent( st( stlist ) ) )
... (Actions !)


Block:
-> id( blockids( tk( tklist ) ) )
-> id( blockids( sg( sglist ) ) )
-> bka( st( stlist ) )
-> bkb( st( stlist ) )
-> bk( bkc( st( stlist ) ) )
-> block( scentry( sc( sclist ) ) ) )
... (Actions !)
 */

/**  */
static struct OModelUtils* _inst( iONode model ) {
  iOModelUtils __ModelUtils = allocMem( sizeof( struct OModelUtils ) );
  iOModelUtilsData data = allocMem( sizeof( struct OModelUtilsData ) );
  MemOp.basecpy( __ModelUtils, &ModelUtilsOp, 0, sizeof( struct OModelUtils ), data );

  /* Initialize data->xxx members... */
  data->model = model;

  instCnt++;
  return __ModelUtils;
}


static void _setModel( struct OModelUtils* inst ,iONode model ) {
  iOModelUtilsData data = Data(inst);
  data->model = model;
}


/**
 * Broadcast an item from which one or more dependencies have been renamed.
 * Called by __renameBlockDeps().
 */
static void __broadcastModifiedItem(iONode props) {
  iONode clone = (iONode)props->base.clone( props );
  NodeOp.setStr(clone, "cmd", wModelCmd.modify );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "modified dependencies for [%s]", wItem.getid(props) );
  AppOp.broadcastEvent( clone );
}


/**
 * Helper function for __renameBlockDeps().
 */
static void __renameBlockId(iONode item, const char* id, const char* previd) {
  if( wItem.getblockid(item) != NULL && StrOp.equals(wItem.getblockid(item), previd) ) {
    wItem.setblockid(item, id);
    __broadcastModifiedItem(item);
  }
}

/**
 * Iterate all items which could have a reference to the renamed block.
 * Called by _renameItemDependencies().
 */
static void __renameBlockDeps(iONode model, const char* id, const char* previd, iONode props) {
  /* Iterate tracks. */
  iONode list = wPlan.gettklist(model);
  if( list != NULL ) {
    iONode item = wTrackList.gettk(list);
    while( item != NULL ) {
      __renameBlockId(item, id, previd);
      item = wTrackList.nexttk(list, item);
    }
  }

  /* Iterate signals. */
  list = wPlan.getsglist(model);
  if( list != NULL ) {
    iONode item = wSignalList.getsg(list);
    while( item != NULL ) {
      __renameBlockId(item, id, previd);
      item = wSignalList.nextsg(list, item);
    }
  }

  /* Iterate switches. */
  list = wPlan.getswlist(model);
  if( list != NULL ) {
    iONode item = wSwitchList.getsw(list);
    while( item != NULL ) {
      __renameBlockId(item, id, previd);
      item = wSwitchList.nextsw(list, item);
    }
  }

}


/**
 * Helper function for __renameRouteDeps().
 */
static void __renameRouteId(iONode item, const char* id, const char* previd) {
  if( StrOp.find(wItem.getrouteids(item), previd) ) {
    char* ids = StrOp.replaceAllSub(wItem.getrouteids(item), previd, id);
    wItem.setrouteids(item, ids);
    StrOp.free(ids);
    __broadcastModifiedItem(item);
  }
}

/**
 * Iterate all items which could have a reference to the renamed route.
 * Called by _renameItemDependencies().
 */
static void __renameRouteDeps(iONode model, const char* id, const char* previd, iONode props) {

  /* Iterate tracks. */
  iONode list = wPlan.gettklist(model);
  if( list != NULL ) {
    iONode item = wTrackList.gettk(list);
    while( item != NULL ) {
      __renameRouteId(item, id, previd);
      item = wTrackList.nexttk(list, item);
    }
  }

  /* Iterate signals. */
  list = wPlan.getsglist(model);
  if( list != NULL ) {
    iONode item = wSignalList.getsg(list);
    while( item != NULL ) {
      __renameRouteId(item, id, previd);
      item = wSignalList.nextsg(list, item);
    }
  }

  /* Iterate switches. */
  list = wPlan.getswlist(model);
  if( list != NULL ) {
    iONode item = wSwitchList.getsw(list);
    while( item != NULL ) {
      __renameRouteId(item, id, previd);
      item = wSwitchList.nextsw(list, item);
    }
  }

  /* Iterate block events. */
  list = wPlan.getbklist(model);
  if( list != NULL ) {
    iONode item = wBlockList.getbk(list);
    while( item != NULL ) {
      Boolean modified = False;
      iONode event = wBlock.getfbevent(item);
      while( event != NULL ) {
        const char* byroute = wFeedbackEvent.getbyroute(event);
        if( byroute != NULL && StrOp.len(byroute) > 0 && StrOp.equals(byroute, previd) ) {
          wFeedbackEvent.setbyroute(event, id);
          modified = True;
        }
        event = wBlock.nextfbevent(item, event);
      }
      if( modified ) {
        __broadcastModifiedItem(item);
      }
      item = wBlockList.nextbk(list, item);
    }
  }

  /* Iterate actions. */
  list = wPlan.getaclist(model);
  if( list != NULL ) {
    iONode item = wActionList.getac(list);
    while( item != NULL ) {
      if( StrOp.equals( wAction.gettype(item), wRoute.name() ) && StrOp.equals( wAction.getoid(item), previd ) ) {
        wAction.setoid(item, id );
        __broadcastModifiedItem(item);
      }
      item = wActionList.nextac(list, item);
    }
  }

}


static void __renameSensorDeps(iONode model, const char* id, const char* previd, iONode props) {
  /* Iterate block events. */
  iONode list = wPlan.getbklist(model);
  if( list != NULL ) {
    iONode item = wBlockList.getbk(list);
    while( item != NULL ) {
      Boolean modified = False;
      iONode event = wBlock.getfbevent(item);
      while( event != NULL ) {
        const char* fbid = wFeedbackEvent.getid(event);
        if( fbid != NULL && StrOp.len(fbid) > 0 && StrOp.equals(fbid, previd) ) {
          wFeedbackEvent.setid(event, id);
          modified = True;
        }
        event = wBlock.nextfbevent(item, event);
      }
      if( modified ) {
        __broadcastModifiedItem(item);
      }
      item = wBlockList.nextbk(list, item);
    }
  }

  /* Iterate route events. */
  list = wPlan.getstlist(model);
  if( list != NULL ) {
    iONode item = wRouteList.getst(list);
    while( item != NULL ) {
      Boolean modified = False;
      iONode event = wRoute.getfbevent(item);
      while( event != NULL ) {
        const char* fbid = wFeedbackEvent.getid(event);
        if( fbid != NULL && StrOp.len(fbid) > 0 && StrOp.equals(fbid, previd) ) {
          wFeedbackEvent.setid(event, id);
          modified = True;
        }
        event = wRoute.nextfbevent(item, event);
      }
      if( modified ) {
        __broadcastModifiedItem(item);
      }
      item = wRouteList.nextst(list, item);
    }
  }
}


/**
 *
 */
static Boolean _renameItemDependencies( iONode model ,const char* id ,const char* previd ,iONode props ) {

  if( StrOp.equals( wBlock.name(), NodeOp.getName(props) )) {
    __renameBlockDeps(model, id, previd, props);
    return True;
  }

  if( StrOp.equals( wRoute.name(), NodeOp.getName(props) )) {
    __renameRouteDeps(model, id, previd, props);
    return True;
  }

  if( StrOp.equals( wFeedback.name(), NodeOp.getName(props) )) {
    __renameSensorDeps(model, id, previd, props);
    return True;
  }

  return False;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/modelutils.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/

