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
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/RouteList.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/SignalList.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/SelTabList.h"
#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/ActionCond.h"
#include "rocrail/wrapper/public/Location.h"
#include "rocrail/wrapper/public/LocationList.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Stage.h"
#include "rocrail/wrapper/public/StageList.h"
#include "rocrail/wrapper/public/StageSection.h"
#include "rocrail/wrapper/public/AnaOpt.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleList.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"
#include "rocrail/wrapper/public/Car.h"
#include "rocrail/wrapper/public/Tour.h"
#include "rocrail/wrapper/public/Link.h"
#include "rocrail/wrapper/public/Text.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/ActionList.h"
#include "rocrail/wrapper/public/TurntableList.h"
#include "rocrail/wrapper/public/TextList.h"


#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"

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

static void __renameBlockId4Schedules(iONode model, const char* id, const char* previd) {
  /* Iterate schedules. */
  iONode list = wPlan.getsclist(model);
  if( list != NULL ) {
    iONode item = wScheduleList.getsc(list);
    while( item != NULL ) {
      Boolean modified = False;
      iONode entry = wSchedule.getscentry(item);
      while( entry != NULL ) {
        if( wScheduleEntry.getblock(entry) != NULL && StrOp.equals(wScheduleEntry.getblock(entry), previd) ) {
          wScheduleEntry.setblock(entry, id);
          modified = True;
        }
        entry = wSchedule.nextscentry(item, entry);
      }
      if(modified) {
        __broadcastModifiedItem(item);
      }
      item = wScheduleList.nextsc(list, item);
    }
  }

}

/**
 * Iterate all items which could have a reference to the renamed block.
 * Called by _renameItemDependencies().
 */
static void __renameBlockDeps(iONode model, const char* id, const char* previd, iONode props, Boolean doTracks) {
  iONode list = NULL;

  if( doTracks ) {
    /* Iterate tracks. */
    list = wPlan.gettklist(model);
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

  /* Iterate routes. */
  list = wPlan.getstlist(model);
  if( list != NULL ) {
    iONode item = wRouteList.getst(list);
    while( item != NULL ) {
      Boolean modified = False;
      const char* bka = wRoute.getbka(item);
      const char* bkb = wRoute.getbkb(item);
      const char* bkc = wRoute.getbkc(item);
      if( bka != NULL && StrOp.equals( bka, previd) ) {
        wRoute.setbka(item, id);
        modified = True;
      }
      if( bkb != NULL && StrOp.equals( bkb, previd) ) {
        wRoute.setbkb(item, id);
        modified = True;
      }


      if( StrOp.find(bkc, previd) ) {
        char* ids = StrTokOp.replaceAll(bkc, ',', previd, id);
        if( ids != NULL ) {
          wRoute.setbkc(item, ids);
          StrOp.free(ids);
          modified = True;
        }
      }

      if(modified) {
        __broadcastModifiedItem(item);
      }
      item = wRouteList.nextst(list, item);
    }
  }


  /* Iterate schedules. */
  __renameBlockId4Schedules(model, id, previd);


  /* Iterate actions. */
  list = wPlan.getaclist(model);
  if( list != NULL ) {
    iONode item = wActionList.getac(list);
    while( item != NULL ) {
      if( StrOp.equals( wAction.gettype(item), wBlock.name() ) && StrOp.equals( wAction.getoid(item), previd ) ) {
        wAction.setoid(item, id );
        __broadcastModifiedItem(item);
      }
      item = wActionList.nextac(list, item);
    }
  }

  /* Iterate locations. */
  list = wPlan.getlocationlist(model);
  if( list != NULL ) {
    iONode item = wLocationList.getlocation(list);
    while( item != NULL ) {
      const char* blocks = wLocation.getblocks(item);
      if( StrOp.find(blocks, previd) ) {
        char* ids = StrTokOp.replaceAll(blocks, ',', previd, id);
        if( ids != NULL ) {
          wLocation.setblocks(item, ids);
          StrOp.free(ids);
          __broadcastModifiedItem(item);
        }
      }
      item = wLocationList.nextlocation(list, item);
    }
  }


}


/**
 * Helper function for __renameRouteDeps().
 */
static void __renameRouteId(iONode item, const char* id, const char* previd) {
  char* routeids = StrTokOp.replaceAll(wItem.getrouteids(item), ',', previd, id);
  if( routeids != NULL ) {
    wItem.setrouteids(item, routeids);
    StrOp.free(routeids);
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

  /* Iterate seltab events. */
  list = wPlan.getseltablist(model);
  if( list != NULL ) {
    iONode item = wSelTabList.getseltab(list);
    while( item != NULL ) {
      Boolean modified = False;
      iONode event = wSelTab.getfbevent(item);
      while( event != NULL ) {
        const char* byroute = wFeedbackEvent.getbyroute(event);
        if( byroute != NULL && StrOp.len(byroute) > 0 && StrOp.equals(byroute, previd) ) {
          wFeedbackEvent.setbyroute(event, id);
          modified = True;
        }
        event = wSelTab.nextfbevent(item, event);
      }
      if( modified ) {
        __broadcastModifiedItem(item);
      }
      item = wSelTabList.nextseltab(list, item);
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


static void __renameSwitchDeps(iONode model, const char* id, const char* previd, iONode props) {
  /* Iterate route events. */
  iONode list = wPlan.getstlist(model);
  if( list != NULL ) {
    iONode item = wRouteList.getst(list);
    while( item != NULL ) {
      Boolean modified = False;
      iONode swcmd = wRoute.getswcmd(item);
      while( swcmd != NULL ) {
        const char* swid = wSwitchCmd.getid(swcmd);
        if( swid != NULL && StrOp.len(swid) > 0 && StrOp.equals(swid, previd) ) {
          wSwitchCmd.setid(swcmd, id);
          modified = True;
        }
        swcmd = wRoute.nextswcmd(item, swcmd);
      }
      if( modified ) {
        __broadcastModifiedItem(item);
      }
      item = wRouteList.nextst(list, item);
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

  /* Iterate tracks. */
  list = wPlan.gettklist(model);
  if( list != NULL ) {
    iONode item = wTrackList.gettk(list);
    while( item != NULL ) {
      __renameBlockId(item, id, previd);
      item = wTrackList.nexttk(list, item);
    }
  }


  /* Iterate stages. */
  list = wPlan.getsblist(model);
  if( list != NULL ) {
    iONode item = wStageList.getsb(list);
    while( item != NULL ) {
      Boolean modified = False;
      iONode section = wStage.getsection(item);
      while( section != NULL ) {
        if( wStageSection.getfbid(section) != NULL && StrOp.equals(wStageSection.getfbid(section), previd) ) {
          wStageSection.setfbid(section, id);
          modified = True;
        }
        if( wStageSection.getfbidocc(section) != NULL && StrOp.equals(wStageSection.getfbidocc(section), previd) ) {
          wStageSection.setfbidocc(section, id);
          modified = True;
        }
        section = wStage.nextsection(item, section);
      }
      if( wStage.getfbenterid(item) != NULL && StrOp.equals(wStage.getfbenterid(item), previd) ) {
        wStage.setfbenterid(item, id);
        modified = True;
      }
      if( modified ) {
        __broadcastModifiedItem(item);
      }
      item = wStageList.nextsb(list, item);
    }
  }

  /* Iterate fiddleyards. */
  list = wPlan.getseltablist(model);
  if( list != NULL ) {
    iONode item = wSelTabList.getseltab(list);
    while( item != NULL ) {
      Boolean modified = False;
      iONode event = wSelTab.getfbevent(item);
      while( event != NULL ) {
        const char* fbid = wFeedbackEvent.getid(event);
        if( fbid != NULL && StrOp.len(fbid) > 0 && StrOp.equals(fbid, previd) ) {
          wFeedbackEvent.setid(event, id);
          modified = True;
        }
        event = wSelTab.nextfbevent(item, event);
      }
      if( wSelTab.getb0sen(item) != NULL && StrOp.equals(wSelTab.getb0sen(item), previd) ) {
        wSelTab.setb0sen(item, id);
        modified = True;
      }
      if( wSelTab.getb1sen(item) != NULL && StrOp.equals(wSelTab.getb1sen(item), previd) ) {
        wSelTab.setb1sen(item, id);
        modified = True;
      }
      if( wSelTab.getb2sen(item) != NULL && StrOp.equals(wSelTab.getb2sen(item), previd) ) {
        wSelTab.setb2sen(item, id);
        modified = True;
      }
      if( wSelTab.getb3sen(item) != NULL && StrOp.equals(wSelTab.getb3sen(item), previd) ) {
        wSelTab.setb3sen(item, id);
        modified = True;
      }
      if( wSelTab.getb4sen(item) != NULL && StrOp.equals(wSelTab.getb4sen(item), previd) ) {
        wSelTab.setb4sen(item, id);
        modified = True;
      }
      if( wSelTab.getb5sen(item) != NULL && StrOp.equals(wSelTab.getb5sen(item), previd) ) {
        wSelTab.setb5sen(item, id);
        modified = True;
      }
      if( wSelTab.getb6sen(item) != NULL && StrOp.equals(wSelTab.getb6sen(item), previd) ) {
        wSelTab.setb6sen(item, id);
        modified = True;
      }
      if( modified ) {
        __broadcastModifiedItem(item);
      }
      item = wSelTabList.nextseltab(list, item);
    }
  }

  /* Iterate tts... */

}


/**
 *
 */
static Boolean _renameItemDependencies( iONode model ,const char* id ,const char* previd ,iONode props ) {

  if( StrOp.equals( wBlock.name(), NodeOp.getName(props) )) {
    __renameBlockDeps(model, id, previd, props, True);
    return True;
  }

  if( StrOp.equals( wStage.name(), NodeOp.getName(props) )) {
    __renameBlockDeps(model, id, previd, props, True);
    return True;
  }

  if( StrOp.equals( wSelTab.name(), NodeOp.getName(props) )) {
    __renameBlockDeps(model, id, previd, props, True);
    return True;
  }

  if( StrOp.equals( wTurntable.name(), NodeOp.getName(props) )) {
    __renameBlockDeps(model, id, previd, props, False);
    return True;
  }

  if( StrOp.equals( wLocation.name(), NodeOp.getName(props) )) {
    __renameBlockId4Schedules(model, id, previd);
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

  if( StrOp.equals( wSwitch.name(), NodeOp.getName(props) )) {
    __renameSwitchDeps(model, id, previd, props);
    return True;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "auto rename of dependencies for object %s [%s] is not supported",
      NodeOp.getName(props), wItem.getid(props) );

  return False;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/modelutils.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/

