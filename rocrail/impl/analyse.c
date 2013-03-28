/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis <r.j.versluis@rocrail.net>,
                         Jean-Michel Fischer <jmf@polygonpunkt.de>,
                         Lothar Roth <lothar.roth@lothar-roth.de>

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

/*
=Track Plan Analyser=

==Preface==
Goal of the analyser is to find routes between blocks by checking all surrounded objects.

==Preparation==
create object map with xyz key's

==Processing==
Iterate the block list and try to find routes to other blocks by scanning the surrounded objects by xyz key's
and take in account the object orientations.

==Example==

--[block A]--+--[block B]--
--[block C]--/

Routes are A-B and B-C.
xyz = 0,0,0 in the left upper corner of the track plan.

**object map:**
 x,y,z object type     orientation  
 0,0,0 track  straight W
 1,0,0 track  straight W
11,0,0 track  straight W
12,0,0 track  straight W
13,0,0 switch left     W
14,0,0 track  straight W
15,0,0 track  straight W
25,0,0 track  straight W
25,0,0 track  straight W
 0,1,0 track  straight W
 1,1,0 track  straight W
11,1,0 track  straight W
12,1,0 track  straight W
13,1,0 track  curve    S

**block list:**
 2,0,0 A W
16,0,0 B W
 2,1,0 C W

N = north
E = east
S = south
W = west

For an example on how objects orientations work out see Rocrail/doc/analyse.xml

Starting with block A:
- block raster length for this example is 9
- only at the west and east side could be connected objects: search in map for 1,0,0 and 11,0,0
- following the west side will end up to a dead end.
- the east side will lead to block B following the tracks and switch

For the Analyzer to work the Plan has to fullfill:
- all items must be connected without space
- only one item at one position
 */

#include "rocrail/impl/analyse_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/BlockList.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Track.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/SwitchCmd.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/ModPlan.h"
#include "rocrail/wrapper/public/Module.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/RouteList.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Signal.h"
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

#include "rocrail/public/app.h"
#include "rocrail/public/model.h"
#include "rocrail/public/track.h"
#include "rocrail/public/switch.h"
#include "rocrail/public/signal.h"
#include "rocrail/public/fback.h"
#include "rocrail/public/route.h"


static int instCnt = 0;

#define MIN_CONNECTOR_COUNTERPART_NR 10

/* some forward declaration */
static Boolean _checkPlanHealth(iOAnalyse inst);
static Boolean __analyseItem(iOAnalyse inst, iONode item, iOList route, int travel,
    int turnoutstate, int depth, Boolean toPreRTlist);
static int _cleanupAutogenRouteids( iONode tracklist );
static Boolean connectorCheck( iOAnalyse inst, Boolean repair );
static Boolean blockCheck( iOAnalyse inst, Boolean repair );
static Boolean routeCheck( iOAnalyse inst, Boolean repair );
static Boolean isValidInterfaceID( iOAnalyse inst, const char *iid );
static int invalidBlockidCheck(  iOAnalyse inst, iONode tracklist, Boolean repair );
static int invalidRouteidsCheck( iOAnalyse inst, iONode tracklist, Boolean repair );

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  iOAnalyseData data = Data(inst);

  if( inst != NULL ) {
    iOAnalyseData data = Data(inst);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cleaning up the ANALYSER...");

    iOMap delMap = MapOp.inst();
    char delkey[32];

    iOList plist = (iOList) ListOp.first(data->preRTlist);
    while (plist != NULL) {
      iONode item = (iONode) ListOp.first(plist);
      while (item != NULL) {
        if( item != NULL ) {
          StrOp.fmtb(delkey, "0x%08X", (void*)item);
          if(!MapOp.haskey(delMap, delkey)) {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "delete %s[0x%08X]", ((obj) item)->name(), (void*)item);
            MapOp.put(delMap, delkey, (obj)delkey);
            NodeOp.base.del(item);
          }
        }
        item = (iONode) ListOp.next(plist);
      }
      StrOp.fmtb(delkey, "0x%08X", (void*)item);
      if(!MapOp.haskey(delMap, delkey)) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "delete %s[0x%08X]", ((obj) plist)->name(), (void*)plist);
        MapOp.put(delMap, delkey, (obj)delkey);
        ListOp.base.del(plist);
      }
      plist = (iOList) ListOp.next(data->preRTlist);
    }

    iONode item = (iONode) ListOp.first(data->bklist);
    while (item != NULL) {
      StrOp.fmtb(delkey, "0x%08X", (void*)item);
      if(!MapOp.haskey(delMap, delkey)) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "delete %s[0x%08X]", ((obj) item)->name(), (void*)item);
        MapOp.put(delMap, delkey, (obj)delkey);
        NodeOp.base.del(item);
      }
      item = (iONode) ListOp.next(data->bklist);
    }

    iOList nlist = (iOList) ListOp.first(data->notRTlist);
    while (nlist) {
      iONode item = (iONode) ListOp.first(nlist);
      while (item) {
        StrOp.fmtb(delkey, "0x%08X", (void*)item);
        if(!MapOp.haskey(delMap, delkey)) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "delete %s[0x%08X]", ((obj) item)->name(), (void*)item);
          MapOp.put(delMap, delkey, (obj)delkey);
          NodeOp.base.del(item);
        }
        item = (iONode) ListOp.next(nlist);
      }
      StrOp.fmtb(delkey, "0x%08X", (void*)item);
      if(!MapOp.haskey(delMap, delkey)) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "delete %s[0x%08X]", ((obj) nlist)->name(), (void*)nlist);
        MapOp.put(delMap, delkey, (obj)delkey);
        ListOp.base.del(nlist);
      }
      nlist = (iOList) ListOp.next(data->notRTlist);
    }

    MapOp.base.del(data->objectmap);
    ListOp.base.del(data->bklist);
    ListOp.base.del(data->preRTlist);
    ListOp.base.del(data->notRTlist);

    MapOp.base.del(delMap);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ANALYSER is cleaned up");

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

/** ----- OAnalyse ----- */
#define oriWest  0
#define oriNorth 1
#define oriEast  2
#define oriSouth 3

#define oriWest2  4
#define oriNorth2 5
#define oriEast2  6
#define oriSouth2 7


/* returns 0 for west, 1 for north, 2 for east and 3 for south */
static int __getOri(iONode item ) {
  const char* ori = wItem.getori(item);
  if( ori == NULL ) return oriWest;
  if( StrOp.equals( wItem.west, ori ) ) return oriWest;
  if( StrOp.equals( wItem.north, ori ) ) return oriNorth;
  if( StrOp.equals( wItem.east, ori ) ) return oriEast;
  if( StrOp.equals( wItem.south, ori ) ) return oriSouth;
  return oriWest;
}

static char* __createKey( char* key, iONode node, int xoffset, int yoffset, int zoffset) {

  int itemx = 0;
  int itemy = 0;
  int itemz = 0;
  if( node != NULL) {
    itemx = wItem.getx(node);
    itemy = wItem.gety(node);
    itemz = wItem.getz(node);
  }
  return StrOp.fmtb( key, "%d-%d-%d", itemx+xoffset, itemy+yoffset, itemz );
}

/* clean a single route (only the route not the items in the route) */
static void deleteSingleRoute( iOList route ) {
  if(route != NULL) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "deleteSingleRoute: cleaning up a single route...");
    ListOp.base.del(route);
  }
  return;
}

static Boolean isSingleTrackRRCrossing( iONode node ) {
  if( StrOp.equals( NodeOp.getName(node), wSwitch.name() ) &&
      StrOp.equals( wItem.gettype(node), wSwitch.accessory ) &&
      ( wSwitch.getaccnr(node) == 10 || /* 1 track, no gate */
        wSwitch.getaccnr(node) == 11 || /* 1 track, 1 gate  */
        wSwitch.getaccnr(node) == 12    /* 1 track, 2 gates */
      )
    ) {
    return( True );
  }
  return( False );
}

static Boolean isDoubleTrackRRCrossing( iONode node ) {
  if( StrOp.equals( NodeOp.getName(node), wSwitch.name() ) &&
      StrOp.equals( wItem.gettype(node), wSwitch.accessory ) &&
      wSwitch.getaccnr(node) == 1 
    ) {
    return( True );
  }
  return( False );
}


static Boolean isStageBlockById( iOModel model, const char* blockid  ) {
  return( NULL != ModelOp.getStage(model, blockid ));
}


/* check if state is a valid loco identifier */
static Boolean isLocoIdentifier( iOAnalyse inst, const char* state ) {
  iOAnalyseData data = Data(inst);
  iONode list = wPlan.getlclist(data->plan);
  int listSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isLocoIdentifier: Checking [%08.8X]", list );
  if( list != NULL ) {
    listSize = NodeOp.getChildCnt( list );
  }

  if( listSize > 0 ) {
    iONode node;
    const char* listType = NodeOp.getName( NodeOp.getChild(list, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isLocoIdentifier: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      node = NodeOp.getChild(list, i);
      if( node ) {
        thisNodeChanged = False;
        int               addr = wLoc.getaddr( node );
        const char* identifier = wLoc.getidentifier( node );
        const char*         id = wLoc.getid( node);
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkLcAction: lclist[%d], id[%s] addr[%d], identifier[%s]", i, id, addr, identifier );
        if( StrOp.equals( state, identifier ) || ( atoi(state) == addr ) ) {
          return True;
        }
      }
    }
  }
  return False;
}

/* condState(switch) == [straight, turnout, left, right] */
static Boolean checkActionCondSwitch( const char* state ) {
  if( StrOp.equals( state, wSwitch.straight ) ||
      StrOp.equals( state, wSwitch.turnout  ) ||
      StrOp.equals( state, wSwitch.left     ) ||
      StrOp.equals( state, wSwitch.right    )
    )
    return True;

  return False;
}

/* condState(signal) == [red, yellow, green, white] multiple(CSV) */
static Boolean checkActionCondSignal( const char* state ) {
  if( StrOp.len( state ) == 0 )
    return False;

  Boolean rc = True;

  iOStrTok tok = StrTokOp.inst( state, ',');
  while( StrTokOp.hasMoreTokens(tok) ) {
    const char* token = StrTokOp.nextToken( tok );
    if( ! StrOp.equals( token, wSignal.red    ) &&
        ! StrOp.equals( token, wSignal.yellow ) &&
        ! StrOp.equals( token, wSignal.green  ) &&
        ! StrOp.equals( token, wSignal.white  )
      ) {
      rc = False;
    }
  }
  StrTokOp.base.del(tok);

  return rc;
}

/* condState(output) == [on, off, active] */
static Boolean checkActionCondOutput( const char* state ) {
  if( StrOp.equals( state, wOutput.on     ) ||
      StrOp.equals( state, wOutput.off    ) ||
      StrOp.equals( state, wOutput.active )
    )
    return True;

  return False;
}

/* condState(feedback) == [true, false, Lok-Kennung|*[forwards, reverse]] */
static Boolean checkActionCondFeedback( iOAnalyse inst, const char* state ) {
  const char* stateOnly = NULL;
  const char* direction = NULL;
  Boolean rc = True;

  iOStrTok tok = StrTokOp.inst( state, ',');
  if(StrTokOp.hasMoreTokens(tok))
    stateOnly = StrTokOp.nextToken(tok);
  if(StrTokOp.hasMoreTokens(tok))   
    direction = StrTokOp.nextToken(tok);
  if(StrTokOp.hasMoreTokens(tok)) {
    /* Uuups 3rd token not allowed */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "checkActionCondFeedback: too many parameters in state [%s]", state );
    rc = False;
  }

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkActionCondFeedback: 1st[%s] 2nd[%s]", stateOnly, direction);

  if( stateOnly ) {
    if( StrOp.equals( stateOnly, "true"  ) ||
        StrOp.equals( stateOnly, "false" )
    ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkActionCondFeedback: state[%s] is valid", stateOnly );
    } else if( StrOp.len( stateOnly ) > 0 ) {
      /* check if state is a valid loco identifier */
      if( ! isLocoIdentifier( inst, stateOnly ) ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "checkActionCondFeedback: 1st param not true/false or loco identifier[%s] not found", stateOnly );
        rc = False;
      } else {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkActionCondFeedback: loco identifier[%s] found", stateOnly );
      }
    } else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "checkActionCondFeedback: no state[%s] found", stateOnly );
      rc = False;
    }
  } else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "checkActionCondFeedback: no state found" );
    rc = False;
  }

  if( direction ) {
    if( ! StrOp.equals( direction, "forwards" ) &&
        ! StrOp.equals( direction, "reverse"  )
      ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "checkActionCondFeedback: direction[%s] invalid", direction );
      rc = False;
    }
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkActionCondFeedback: 1st[%s] 2nd[%s] rc[%d]", stateOnly, direction, rc );

  StrTokOp.base.del(tok);

  return rc;
}

/* single addr > 0 _OR_ (range) addr > 0 && addr <= hAddr )*/
static Boolean locoRangeChecks( const char* state ) {
  if( state == NULL )
    return False;

  if( ! StrOp.startsWith( state, "#" ) &&
      ! StrOp.startsWith( state, "x" )
    )
    return False;

  Boolean rc = True;

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "locoRangeChecks: is address/range [%s] valid", state );

  iOStrTok tok = StrTokOp.inst(state, ',');
  while( StrTokOp.hasMoreTokens(tok) ) {
    const char* sAddr = StrTokOp.nextToken(tok);
    char* sHAddr = StrOp.find( sAddr, "-" );
    if( sHAddr != NULL ) {
      int addr = 0;
      int hAddr = atoi(sHAddr+1);
      *sHAddr = '\0';
      addr = atoi(sAddr+1);
      if( addr <= 0 || addr > hAddr ) {
        rc = False;
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "locoRangeChecks: address range [%d-%d] is invalid", addr, hAddr );
      } else {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "locoRangeChecks: address range [%d-%d] is valid", addr, hAddr );
      }
    }
    else {
      int addr = atoi(sAddr+1);
      if( addr <= 0 ) {
        rc = False;
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "locoRangeChecks: address [%d] is invalid", addr );
      } else {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "locoRangeChecks: address [%d] is valid", addr );
      }
    }
  };
  StrTokOp.base.del(tok);

  return rc ;
}

/* [fon|foff],[0-28] */
static Boolean locoFnChecks( const char* state ) {
  if( state == NULL )
    return False;

  const char* fcmd = NULL;
  const char* fidx = NULL;
  Boolean rc = True;

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "locoFnChecks: [%s]", state );

  iOStrTok tok = StrTokOp.inst( state, ',');
  if(StrTokOp.hasMoreTokens(tok))
    fcmd = StrTokOp.nextToken(tok);
  if(StrTokOp.hasMoreTokens(tok))   
    fidx = StrTokOp.nextToken(tok);
  if(StrTokOp.hasMoreTokens(tok)) {
    /* Uuups 3rd token not allowed */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "locoFnChecks: too many parameters in state [%s]", state );
    rc = False;
  }

  if( fcmd ) {
    if( ! StrOp.equals( fcmd, "fon"  ) &&
        ! StrOp.equals( fcmd, "foff" )
      ) {
      rc = False;
    }
  }else {
    /* empty string */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "locoFnChecks: fcmd empty [%s]", state );
    rc = False;
  }

  if( fidx ) {
    /* fidx [0..28] (0 is light/F0) */
    int idx = atoi(fidx);
    if( idx < 0 || idx > 28 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "locoFnChecks: function number [%d] out of range in [%s] ", idx, state );
      rc = False;
    } else {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "locoFnChecks: function number [%d] is OK", idx );
    }
  }else {
    /* fon/foff without number */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "locoFnChecks: function number missing in [%s] ", state );
    rc = False;
  }

  StrTokOp.base.del(tok);

  return rc ;
}

/* conID == loco condState(loco) [min, mid, cruise, max, consist]  ??? [+|-] */
static Boolean checkActionCondLoco( const char* lcid, const char* state ) {
  if( state == NULL || StrOp.len( state ) == 0 )
    return True;

  if( StrOp.startsWith( state, "#" ) ||
      StrOp.startsWith( state, "x" )
    )
    return locoRangeChecks( state );

  if( StrOp.startsWith( state, "fon"  ) ||
      StrOp.startsWith( state, "foff" )
    )
    return locoFnChecks( state );

  Boolean rc = True;

  iOStrTok tok = StrTokOp.inst( state, ',');
  while( StrTokOp.hasMoreTokens(tok) ) {
    const char* token = StrTokOp.nextToken( tok );
    if( ! StrOp.equals( token, wLoc.min     ) &&
        ! StrOp.equals( token, wLoc.mid     ) &&
        ! StrOp.equals( token, wLoc.cruise  ) &&
        ! StrOp.equals( token, wLoc.max     ) &&
        ! StrOp.equals( token, "+"          ) &&
        ! StrOp.equals( token, "-"          )
      ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "checkActionCondLoco: lc[%s] unsupported state[%s] will always be treated as valid. You should use empty state.",
          lcid, token );
      rc = False;
    }
  }

  StrTokOp.base.del(tok);

  return rc;
}

/* conID == "*"  condState(loco_wildcard) == [diesel, steam, electric][+|-][#addr[,#addr]][#addr-addr] */
static Boolean checkActionCondLocoWc( iOAnalyse inst, const char* acLcid, const char* state ) {
  if( state == NULL || StrOp.len( state ) == 0 )
    return False;

  if( StrOp.startsWith( state, "#" ) ||
      StrOp.startsWith( state, "x" )
    )
    return locoRangeChecks( state );

  if( StrOp.startsWith( state, "fon"  ) ||
      StrOp.startsWith( state, "foff" )
    )
    return locoFnChecks( state );

  iOAnalyseData data = Data(inst);
  iOLoc lc = ModelOp.getLoc( data->model, acLcid, NULL, False);
  Boolean rc = True;

  iOStrTok tok = StrTokOp.inst( state, ',');
  while( StrTokOp.hasMoreTokens(tok) ) {
    const char* token = StrTokOp.nextToken( tok );
    if( ! StrOp.equals( token, "diesel"   ) &&
        ! StrOp.equals( token, "steam"    ) &&
        ! StrOp.equals( token, "electric" ) &&
        ! StrOp.equals( token, "+"        ) &&
        ! StrOp.equals( token, "-"        )
      ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "checkActionCondLocoWc: lc[%s] unsupported state[%s] will always be treated as valid. You should use empty state.",
          acLcid, token );
      rc = False;
    }
  }

  StrTokOp.base.del(tok);

  return rc;
}

/* condState(block) == [free, !free, occupied, open, closed] */
static Boolean checkActionCondBlock( const char* state ) {
  if( StrOp.equals( state, "free"        ) ||
      StrOp.equals( state, "!free"       ) ||
      StrOp.equals( state, "occupied"    ) ||
      StrOp.equals( state, wBlock.open   ) ||
      StrOp.equals( state, wBlock.closed )
    )
    return True;

  return False;
}

/* condState(syscmd) == [go, stop] */
static Boolean checkActionCondSysCmd( const char* state ) {
  if( StrOp.equals( state, wSysCmd.go   ) ||
      StrOp.equals( state, wSysCmd.stop )
    )
    return True;

  return False;
}

/* condState(route) == [locked, unlocked] */
static Boolean checkActionCondRoute( const char* state ) {
  if( StrOp.equals( state, "unlocked" ) ||
      StrOp.equals( state, "locked"   )
    )
    return True;

  return False;
}

/* check actions existance and conditions */
static int checkAction( iOAnalyse inst, int acIdx, iONode action, Boolean repair, int* checkedTotal ) {
  if( action == NULL )
    return 0;
  iOAnalyseData data = Data(inst);
  int modifications = 0;
  int numModifiedActions = 0;

  (*checkedTotal)++;

  const char* acId    = wActionCtrl.getid(    action );
  const char* acState = wActionCtrl.getstate( action );
  const char* acLcid  = wActionCtrl.getlcid(  action );
  Boolean isReset     = wActionCtrl.isreset(  action );
  Boolean isAuto      = wActionCtrl.isauto(   action );
  Boolean isManual    = wActionCtrl.ismanual( action );

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkAction: action id[%s] state[%s] loco[%s] reset[%d] auto[%d] manual[%d]",
      acId, acState, acLcid, isReset, isAuto, isManual);
  iOAction iOAc = ModelOp.getAction( data->model, acId );
  if( iOAc == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: action[%s] not found in plan", acId );
    modifications++;
    numModifiedActions++;
  }

  /* loop over all conditions of action) */
  iONode actioncond = wActionCtrl.getactioncond( action );
  int condIdx = 0;
  while( actioncond != NULL ) {
    condIdx++;
    (*checkedTotal)++;
    const char* condId    = wActionCond.getid(    actioncond );
    const char* condState = wActionCond.getstate( actioncond );
    const char* condType  = wActionCond.gettype(  actioncond );
    char*       ptr       = NULL;
    Boolean condUnsuportedType = False;
    Boolean condOK = False;

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkAction: actioncond[%d] type[%s] id[%s] state[%s]",
        condIdx, condType, condId, condState );
    if( StrOp.equals( condType, wSwitch.name() ) ) {
      ptr = (char *) ModelOp.getSwitch( data->model, condId );
      if( ptr && checkActionCondSwitch( condState ) )
        condOK = True;
    }else if ( StrOp.equals( condType, wSignal.name() ) ) {
      ptr = (char *) ModelOp.getSignal( data->model, condId );
      if( ptr && checkActionCondSignal( condState ) )
        condOK = True;
    }else if ( StrOp.equals( condType, wOutput.name() ) ) {
      ptr = (char *) ModelOp.getOutput( data->model, condId );
      if( ptr && checkActionCondOutput( condState ) )
        condOK = True;
    }else if ( StrOp.equals( condType, wFeedback.name() ) ) {
      ptr = (char *) ModelOp.getFBack( data->model, condId );
      if( ptr && checkActionCondFeedback( inst, condState ) )
        condOK = True;
    }else if ( StrOp.equals( condType, wLoc.name() ) ) {
      ptr = (char *) ModelOp.getLoc( data->model, condId, NULL, False );
      if( ptr && checkActionCondLoco( condId, condState ) ) {
        condOK = True;
      }else if( StrOp.equals( condId, "*" ) )
        /* "*" is always a valid loco */
        ptr = (char *) ~0 ;
        if( checkActionCondLocoWc( inst, acLcid, condState ) ) {
          condOK = True;
        }
    }else if ( StrOp.equals( condType, wBlock.name() ) ) {
      ptr = (char *) ModelOp.getBlock( data->model, condId );
      if( ptr && checkActionCondBlock( condState ) ) {
        condOK = True;
      }
    }else if ( StrOp.equals( condType, wSysCmd.name() ) ) {
      /* sys is always valid -> set pointer to something != NULL */
      ptr = (char *) ~0 ;
      if( ptr && checkActionCondSysCmd( condState ) )
        condOK = True;
    }else if ( StrOp.equals( condType, wRoute.name() ) ) {
      ptr = (char *) ModelOp.getRoute( data->model, condId );
      if( ptr && checkActionCondRoute( condState ) )
        condOK = True;
    }else {
      condUnsuportedType = True;
    }
    if( condOK ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkAction: action[%d][%s] condition[%d] valid id[%s][%s] @0x[%08.8X] state[%s]",
          acIdx, acId, condIdx, condId, condType, ptr, condState );
    }else {
      if( condUnsuportedType && StrOp.len( condType ) == 0 ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: action[%d][%s] condition[%d] missing type for ID[%s] (state[%s])",
            acIdx, acId, condIdx, condId, condState );
        modifications++;
      }else if( condUnsuportedType ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: action[%d][%s] condition[%d] unsupported condition type[%s] (condId[%s], state[%s])",
            acIdx, acId, condIdx, condType, condId, condState );
        modifications++;
      }else if( ptr == NULL ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: action[%d][%s] condition[%d][%s](type[%s]) not found in plan (state[%s] unchecked)",
            acIdx, acId, condIdx, condId, condType, condState );
        modifications++;
      }else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: action[%d][%s] condition[%d][%s](type[%s]): verify of state[%s] failed",
            acIdx, acId, condIdx, condId, condType, condState );
        modifications++;
      }
    }
    actioncond = wActionCtrl.nextactioncond( action, actioncond );
  }

  return modifications;
}

/* check action list */
static int checkAcList( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode aclist = wPlan.getaclist(data->plan);
  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;

  if( aclist != NULL ) {
    listSize = NodeOp.getChildCnt( aclist );
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkAcList: Checking [%08.8X] size[%d] repair[%d]",
      aclist, listSize, repair );

  if( listSize > 0 ) {
    iONode action;
    const char* listType = NodeOp.getName( NodeOp.getChild(aclist, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkAcList: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      action = NodeOp.getChild(aclist, i);
      if( action ) {
        thisNodeChanged = False;
        const char* id    = wAction.getid( action );
        const char* type  = wAction.gettype( action );
        const char* oid   = wAction.getoid( action );
        const char* cmd   = wAction.getcmd( action );
        const char* param = wAction.getparam( action );
        const char* desc  = wAction.getdesc( action );
        const char* state = wSwitch.getstate( action );
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checkAcList: action[%08.8X] id[%s] type[%s] oid[%s] cmd[%s] param[%s] desc[%s] state[%s]",
            action, id, type, oid, cmd, param, desc, state );
        /* TODO/MISSING: check action */
        if( thisNodeChanged == True ) {
          numModifiedItems++ ;
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in actions of switch[%s] (see previous lines)",
              id );
        }
      }
    }
    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}

static int checkSwAction( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode list = wPlan.getswlist(data->plan);
  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkSwAction: Checking [%08.8X]", list );
  if( list != NULL ) {
    listSize = NodeOp.getChildCnt( list );
  }

  if( listSize > 0 ) {
    iONode node;
    const char* listType = NodeOp.getName( NodeOp.getChild(list, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkSwAction: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      node = NodeOp.getChild(list, i);
      if( node ) {
        thisNodeChanged = False;
        int acIdx = 0;
        iONode action = wSwitch.getactionctrl( node );
        const char* id = wSwitch.getid( node);
        const char* state = wSwitch.getstate( node );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkSwAction: id[%s] state[%s]",
            id, state );

        /* loop over all actions */
        while( action != NULL ) {
          acIdx++;
          int changes = checkAction( inst, acIdx, action, repair, &checkedTotal );
          if( changes > 0 ) {
            modifications += changes;
            thisNodeChanged = True;
          }
          action = wSwitch.nextactionctrl( node, action );
        }
        if( thisNodeChanged == True ) {
          numModifiedItems++ ;
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in actions of switch[%s] (see previous lines)",
              id );
        }
      }
    }
    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}

static int checkSgAction( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode list = wPlan.getsglist(data->plan);
  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkSgAction: Checking [%08.8X]", list );
  if( list != NULL ) {
    listSize = NodeOp.getChildCnt( list );
  }

  if( listSize > 0 ) {
    iONode node;
    const char* listType = NodeOp.getName( NodeOp.getChild(list, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkSgAction: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      node = NodeOp.getChild(list, i);
      if( node ) {
        thisNodeChanged = False;
        int acIdx = 0;
        iONode action = wSignal.getactionctrl( node );
        const char* id = wSignal.getid( node);
        const char* state = wSignal.getstate( node );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkSgAction: id[%s] state[%s]",
            id, state );

        /* loop over all actions */
        while( action != NULL ) {
          acIdx++;
          int changes = checkAction( inst, acIdx, action, repair, &checkedTotal );
          if( changes > 0 ) {
            modifications += changes;
            thisNodeChanged = True;
          }
          action = wSignal.nextactionctrl( node, action );
        }
        if( thisNodeChanged == True ) {
          numModifiedItems++ ;
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in actions of signal[%s] (see previous lines)",
              id );
        }
      }
    }
    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}

static int checkCoAction( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode list = wPlan.getcolist(data->plan);
  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkCoAction: Checking [%08.8X]", list );
  if( list != NULL ) {
    listSize = NodeOp.getChildCnt( list );
  }

  if( listSize > 0 ) {
    iONode conode;
    const char* listType = NodeOp.getName( NodeOp.getChild(list, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkCoAction: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      conode = NodeOp.getChild(list, i);
      if( conode ) {
        thisNodeChanged = False;
        int acIdx = 0;
        iONode action = wOutput.getactionctrl( conode );
        const char* id = wOutput.getid( conode);
        const char* state = wOutput.getstate( conode );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkCoAction: id[%s] state[%s]",
            id, state );

        /* loop over all actions */
        while( action != NULL ) {
          acIdx++;
          int changes = checkAction( inst, acIdx, action, repair, &checkedTotal );
          if( changes > 0 ) {
            modifications += changes;
            thisNodeChanged = True;
          }
          action = wOutput.nextactionctrl( conode, action );
        }
        if( thisNodeChanged == True ) {
          numModifiedItems++ ;
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in actions of output[%s] (see previous lines)",
              id );
        }
      }
    }
    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}

static int checkFbAction( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode list = wPlan.getfblist(data->plan);
  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkFbAction: Checking [%08.8X]", list );
  if( list != NULL ) {
    listSize = NodeOp.getChildCnt( list );
  }

  if( listSize > 0 ) {
    iONode node;
    const char* listType = NodeOp.getName( NodeOp.getChild(list, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkFbAction: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      node = NodeOp.getChild(list, i);
      if( node ) {
        thisNodeChanged = False;
        int acIdx = 0;
        iONode action = wFeedback.getactionctrl( node );
        const char* id = wFeedback.getid( node);
        Boolean state = wFeedback.isstate( node );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkFbAction: id[%s] state[%d][%s]",
            id, state, state?"on/true":"off/false" );

        /* loop over all actions */
        while( action != NULL ) {
          acIdx++;
          int changes = checkAction( inst, acIdx, action, repair, &checkedTotal );
          if( changes > 0 ) {
            modifications += changes;
            thisNodeChanged = True;
          }
          action = wFeedback.nextactionctrl( node, action );
        }
        if( thisNodeChanged == True ) {
          numModifiedItems++ ;
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in actions of feedback[%s] (see previous lines)",
              id );
        }
      }
    }
    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}


static int checkBkAction( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode list = wPlan.getbklist(data->plan);
  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkBkAction: Checking [%08.8X]", list );
  if( list != NULL ) {
    listSize = NodeOp.getChildCnt( list );
  }

  if( listSize > 0 ) {
    iONode node;
    const char* listType = NodeOp.getName( NodeOp.getChild(list, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkBkAction: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      node = NodeOp.getChild(list, i);
      if( node ) {
        thisNodeChanged = False;
        int acIdx = 0;
        iONode action = wBlock.getactionctrl( node );
        const char* id = wBlock.getid( node);
        const char* state = wBlock.getstate( node );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkBkAction: id[%s] state[%s]",
            id, state );

        /* loop over all actions */
        while( action != NULL ) {
          acIdx++;
          int changes = checkAction( inst, acIdx, action, repair, &checkedTotal );
          if( changes > 0 ) {
            modifications += changes;
            thisNodeChanged = True;
          }
          action = wBlock.nextactionctrl( node, action );
        }
        if( thisNodeChanged == True ) {
          numModifiedItems++ ;
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in actions of block[%s] (see previous lines)",
              id );
        }
      }
    }
    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}

static int checkSbAction( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode list = wPlan.getsblist(data->plan);
  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkSbAction: Checking [%08.8X]", list );
  if( list != NULL ) {
    listSize = NodeOp.getChildCnt( list );
  }

  if( listSize > 0 ) {
    iONode node;
    const char* listType = NodeOp.getName( NodeOp.getChild(list, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkSbAction: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      node = NodeOp.getChild(list, i);
      if( node ) {
        thisNodeChanged = False;
        int acIdx = 0;
        iONode action = wStage.getactionctrl( node );
        const char* id = wStage.getid( node);
        const char* state = wStage.getstate( node );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkSbAction: id[%s] state[%s]",
            id, state );

        /* loop over all actions */
        while( action != NULL ) {
          acIdx++;
          int changes = checkAction( inst, acIdx, action, repair, &checkedTotal );
          if( changes > 0 ) {
            modifications += changes;
            thisNodeChanged = True;
          }
          action = wStage.nextactionctrl( node, action );
        }
        if( thisNodeChanged == True ) {
          numModifiedItems++ ;
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in actions of stageblock[%s] (see previous lines)",
              id );
        }
      }
    }
    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}

static int checkTtAction( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode list = wPlan.getttlist(data->plan);
  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkTtAction: Checking [%08.8X]", list );
  if( list != NULL ) {
    listSize = NodeOp.getChildCnt( list );
  }

  if( listSize > 0 ) {
    iONode node;
    const char* listType = NodeOp.getName( NodeOp.getChild(list, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkTtAction: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      node = NodeOp.getChild(list, i);
      if( node ) {
        thisNodeChanged = False;
        int acIdx = 0;
        iONode action = wTurntable.getactionctrl( node );
        const char* id = wTurntable.getid( node);
        const char* state = wTurntable.getstate( node );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkTtAction: id[%s] state[%s]",
            id, state );

        /* loop over all actions */
        while( action != NULL ) {
          acIdx++;
          int changes = checkAction( inst, acIdx, action, repair, &checkedTotal );
          if( changes > 0 ) {
            modifications += changes;
            thisNodeChanged = True;
          }
          action = wTurntable.nextactionctrl( node, action );
        }
        if( thisNodeChanged == True ) {
          numModifiedItems++ ;
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in actions of turntable[%s] (see previous lines)",
              id );
        }
      }
    }
    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}

static int checkStAction( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode list = wPlan.getstlist(data->plan);
  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkStAction: Checking [%08.8X]", list );
  if( list != NULL ) {
    listSize = NodeOp.getChildCnt( list );
  }

  if( listSize > 0 ) {
    iONode node;
    const char* listType = NodeOp.getName( NodeOp.getChild(list, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkStAction: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      node = NodeOp.getChild(list, i);
      if( node ) {
        thisNodeChanged = False;
        int acIdx = 0;
        iONode action = wRoute.getactionctrl( node );
        const char* id = wRoute.getid( node);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkStAction: id[%s]",
            id );

        /* loop over all actions */
        while( action != NULL ) {
          acIdx++;
          int changes = checkAction( inst, acIdx, action, repair, &checkedTotal );
          if( changes > 0 ) {
            modifications += changes;
            thisNodeChanged = True;
          }
          action = wRoute.nextactionctrl( node, action );
        }
        if( thisNodeChanged == True ) {
          numModifiedItems++ ;
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in actions of route[%s] (see previous lines)",
              id );
        }
      }
    }
    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}

static int checkScAction( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode list = wPlan.getsclist(data->plan);
  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkScAction: Checking [%08.8X]", list );
  if( list != NULL ) {
    listSize = NodeOp.getChildCnt( list );
  }

  if( listSize > 0 ) {
    iONode node;
    const char* listType = NodeOp.getName( NodeOp.getChild(list, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkScAction: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      node = NodeOp.getChild(list, i);
      if( node ) {
        thisNodeChanged = False;
        int acIdx = 0;
        iONode action = wSchedule.getactionctrl( node );
        const char* id = wSchedule.getid( node);
        int scEntries = NodeOp.getChildCnt(node);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkScAction: id[%s] #entries[%d]",
            id, scEntries );

        /* loop over all actions */
        while( action != NULL ) {
          acIdx++;
          int changes = checkAction( inst, acIdx, action, repair, &checkedTotal );
          if( changes > 0 ) {
            modifications += changes;
            thisNodeChanged = True;
          }
          action = wSchedule.nextactionctrl( node, action );
        }
        int j;
        for( j = 0 ; j < scEntries ; j++ ) {
          acIdx = 0;
          iONode entry = NodeOp.getChild( node, j );
          if( entry != NULL ) {
            iONode entryAction = wScheduleEntry.getactionctrl( entry );
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkScAction:  entry[%d] entryId[%s]",
                j, wItem.getid( entry ) );
            while( entryAction != NULL ) {
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkScAction:   entryAction[%s] state[%s]",
                  wActionCtrl.getid( entryAction ), wActionCtrl.getstate( entryAction ) );
              acIdx++;
              int changes = checkAction( inst, acIdx, entryAction, repair, &checkedTotal );
              if( changes > 0 ) {
                modifications += changes;
                thisNodeChanged = True;
              }

              entryAction = wSchedule.nextactionctrl( node, entryAction);
            }
          }
        }

        if( thisNodeChanged == True ) {
          numModifiedItems++ ;
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in actions of schedule[%s] (see previous lines)",
              id );
        }
      }
    }
    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}

static int checkLcAction( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode list = wPlan.getlclist(data->plan);
  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkLcAction: Checking [%08.8X]", list );
  if( list != NULL ) {
    listSize = NodeOp.getChildCnt( list );
  }

  if( listSize > 0 ) {
    iONode node;
    const char* listType = NodeOp.getName( NodeOp.getChild(list, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkLcAction: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      node = NodeOp.getChild(list, i);
      if( node ) {
        thisNodeChanged = False;
        int acIdx = 0;
        iONode action = wLoc.getactionctrl( node );
        const char* id = wLoc.getid( node);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkLcAction: id[%s]",
            id );

        /* loop over all actions */
        while( action != NULL ) {
          acIdx++;
          int changes = checkAction( inst, acIdx, action, repair, &checkedTotal );
          if( changes > 0 ) {
            modifications += changes;
            thisNodeChanged = True;
          }
          action = wLoc.nextactionctrl( node, action );
        }
        if( thisNodeChanged == True ) {
          numModifiedItems++ ;
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in actions of loco[%s] (see previous lines)",
              id );
        }
      }
    }
    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}

static int checkTxAction( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode list = wPlan.gettxlist(data->plan);
  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkTxAction: Checking [%08.8X]", list );
  if( list != NULL ) {
    listSize = NodeOp.getChildCnt( list );
  }

  if( listSize > 0 ) {
    iONode node;
    const char* listType = NodeOp.getName( NodeOp.getChild(list, 0));
    int i = 0;
    Boolean thisNodeChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkTxAction: Checking %d %s nodes", listSize, listType );
    for( i = 0 ; i < listSize ; i++ ) {
      node = NodeOp.getChild(list, i);
      if( node ) {
        thisNodeChanged = False;
        int acIdx = 0;
        iONode action = wText.getactionctrl( node );
        const char* id = wText.getid( node);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "checkTxAction: id[%s]",
            id );

        /* loop over all actions */
        while( action != NULL ) {
          acIdx++;
          int changes = checkAction( inst, acIdx, action, repair, &checkedTotal );
          if( changes > 0 ) {
            modifications += changes;
            thisNodeChanged = True;
          }
          action = wText.nextactionctrl( node, action );
        }
        if( thisNodeChanged == True ) {
          numModifiedItems++ ;
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in actions of text[%s] (see previous lines)",
              id );
        }
      }
    }
    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}

static int checkSyAction( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);

  int checkedTotal = 0;
  int modifications = 0;
  int numModifiedItems = 0;
  int listSize = 0;
  const char* listType = "sy";


  iONode system = wPlan.getsystem(data->plan);
  if( system != NULL ) {
    int acIdx = 0;
    iONode action = wSystemActions.getactionctrl(system);
    /* loop over all actions */
    while( action != NULL ) {
      acIdx++;
      Boolean thisNodeChanged ;
      listSize++;
      const char* state = wActionCtrl.getstate(action);
      int changes = checkAction( inst, acIdx, action, repair, &checkedTotal );
      if( changes > 0 ) {
        modifications += changes;
        thisNodeChanged = True;
      }
      if( thisNodeChanged == True ) {
        numModifiedItems++ ;
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: something is wrong in system actions (see previous lines)" );
      }
      action = wSystemActions.nextactionctrl( system, action );
    }

    /* statistics */
    if( modifications > 0 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s %slist [%5d/%5d] invalid action/condition items in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedItems, listSize );
    }
  }
  return modifications;
}


static Boolean isFbeventDuplicate( iONode blocknode, iONode item, const char* actionNew, const char* fromNew ) {
  const char* blid = wItem.getid( blocknode );
  const char* fbidNew = wItem.getid( item );

  const char* fbidOld   = NULL;
  const char* actionOld = NULL;
  const char* fromOld   = NULL;


  if( blocknode ) {
    iONode fbevt = wBlock.getfbevent( blocknode );
    while( fbevt != NULL ) {
      fbidOld   = wFeedbackEvent.getid( fbevt );
      actionOld = wFeedbackEvent.getaction( fbevt );
      fromOld   = wFeedbackEvent.getfrom( fbevt );

      if( StrOp.equals( actionOld, wFeedbackEvent.enter2pre_event ) ) {
        /* for duplicate testing handle "enter2pre" like "enter" */
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isFbeventDuplicate: handling enter2pre like enter" );
        actionOld = wFeedbackEvent.enter_event ;
      }
      if( StrOp.equals( actionOld, wFeedbackEvent.enter2shortin_event ) ) {
        /* for duplicate testing handle "enter2pre" like "enter" */
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isFbeventDuplicate: handling enter2shortin like enter" );
        actionOld = wFeedbackEvent.enter_event ;
      }

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isFbeventDuplicate: block %s NEW %s %s %s <-> OLD %s %s %s", wItem.getid(blocknode), fromNew, wItem.getid(item), actionNew, fromOld, fbidOld, actionOld );

      if( StrOp.equals( actionOld, wFeedbackEvent.enter2in_event ) ) {
        if( ( StrOp.len(fbidOld) > 0 ) && ( StrOp.len(fromOld) > 0 ) ) {
          /* enter2in is like enter _AND/OR_ in */
          /* if this direction and ("enter" or "in") then return duplicate */
          if( StrOp.equals( fromNew, fromOld ) && ( StrOp.equals( actionNew, wFeedbackEvent.enter_event ) || StrOp.equals( actionNew, wFeedbackEvent.in_event ) ) ) {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isFbeventDuplicate: enter2in this dir (True)" );
            return True;
          }
          if( ! StrOp.equals( fromNew, fromOld ) && ( StrOp.equals( actionNew, wFeedbackEvent.enter_event ) || StrOp.equals( actionNew, wFeedbackEvent.in_event ) ) ) {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isFbeventDuplicate: enter2in other dir (True)" );
            return True;
          }
        }
      }
      else if( StrOp.equals( actionOld, wFeedbackEvent.pre2in_event ) ) {
        /* pre2in is ignored -> no comparison for action */
        /* if current FB is already used for this direction then return duplicate */
        if( ( StrOp.len(fbidOld) > 0 ) && ( StrOp.len(fromOld) > 0 ) ) {
          if( StrOp.equals( fromNew, fromOld ) && StrOp.equals( fbidNew, fbidOld ) ) {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isFbeventDuplicate: pre2in this dir (True)" );
            return True ;
          }
        }
      }
      else if( ( StrOp.len(fbidOld) > 0 ) && ( StrOp.len(fromOld) > 0 ) ) {
        /* if FB is already used or action is already set for this direction then return duplicate */
        if( StrOp.equals( fromNew, fromOld ) && ( StrOp.equals( fbidNew, fbidOld ) || StrOp.equals( actionNew, actionOld ) ) ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isFbeventDuplicate: fb or action in this dir (True)" );
          return True ;
        }
      }
      fbevt = wBlock.nextfbevent( blocknode, fbevt );
    }
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isFbeventDuplicate: no match (False)" );
  return False ;
}


/* is the given node a feedback of a staging block ? */
static Boolean isStageblockFeedback( iONode plan, iONode node ) {
  iONode sblist = wPlan.getsblist(plan);

  if( sblist != NULL ) {
    iONode sb = wStageList.getsb( sblist );
    while( sb != NULL ) {
      iONode section = wStage.getsection(sb);
      while( section) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isStageblockFeedback: sb %s sec %s fb %s ?? %s A", 
            wStage.getid( sb ), wStageSection.getid( section ), wStageSection.getfbid( section), wItem.getid(node) );
        if( StrOp.equals( wStageSection.getfbid( section), wItem.getid(node) ) ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isStageblockFeedback: sb %s sec %s fb %s == %s B (True)", 
              wStage.getid( sb ), wStageSection.getid( section ), wStageSection.getfbid( section), wItem.getid(node) );
          return( True );
        }
        section = wStage.nextsection( sb, section );
      }
      sb =  wStageList.nextsb( sblist, sb );
    }
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isStageblockFeedback: no match (False)" );
  return(False);
}

/* is fbid assigned to given block ? */
static Boolean isFeedbackOfBlock( iOModel model, iONode bl, const char* id ) {

  if( StrOp.equals(NodeOp.getName(bl), wBlock.name() ) ) {
    const char* bkid = wItem.getid(bl);
    iIBlockBase block = ModelOp.getBlock( model, bkid );
    iONode bkNode = BlockOp.base.properties( block );

    iONode fbevt = wBlock.getfbevent( bkNode );
    while( fbevt != NULL ) {
      const char* fbid = wFeedbackEvent.getid( fbevt );
      if( ( StrOp.len(fbid) > 0 ) && StrOp.equals( fbid, id ) ) {
        /* fb is used in given block */
        return True ;
      }
      fbevt = wBlock.nextfbevent( bkNode, fbevt );
    }
  }
  else if( StrOp.equals(NodeOp.getName(bl), wStage.name() ) ) {
    const char* fbenterid = wStage.getfbenterid(bl);
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isFeedbackOfBlock: sb %s fbenter %s",
        wStage.getid(bl), fbenterid );
    if( ( fbenterid != NULL ) && StrOp.equals( fbenterid, id ) ) {
      return True;
    }
    iONode section = wStage.getsection(bl);
    while( section ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isFeedbackOfBlock: sb %s sec %s", 
          wStage.getid(bl), wStageSection.getid( section ) );
      const char* fbid    = wStageSection.getfbid( section );
      const char* fbidocc = wStageSection.getfbidocc( section );
      int idx = wStageSection.getidx( section );
      int nr  = wStageSection.getnr( section );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isFeedbackOfBlock: sb %s sec %s fb %s fbocc %s idx %d nr %d",
          wStage.getid( bl ), wStageSection.getid( section ), fbid, fbidocc, idx, nr );
      if( ( fbid != NULL ) && StrOp.equals( fbid, id ) ) {
        /* fb is used in given stagingblock */
        return True;
      }
      if( ( fbidocc != NULL ) && StrOp.equals( fbid, id ) ) {
        /* fb is used in given stagingblock as occ fb */
        return True;
      }
      section = wStage.nextsection( bl, section );
    }
  }
  return False;
}

/* count number of feedback of a staging block's sections (fbid _and_ fbidocc) */
static int countStageblockSectionFeedback( iONode sb ) {
  int count = 0 ;

  if( sb != NULL ) {
    iONode section = wStage.getsection(sb);
    while( section ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "countStageblockSectionFeedback: sb %s sec %s", 
          wStage.getid( sb ), wStageSection.getid( section ) );
      const char* fbid    = wStageSection.getfbid( section );
      const char* fbidocc = wStageSection.getfbidocc( section );
      int idx = wStageSection.getidx( section );
      int nr  = wStageSection.getnr( section );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "countStageblockSectionFeedback: sb %s sec %s fb %s fbocc %s idx %d nr %d", 
          wStage.getid( sb ), wStageSection.getid( section ), fbid, fbidocc, idx, nr );
      if( ( fbid != NULL ) && ( StrOp.len( fbid ) > 0 ) ) {
        count++;
      }
      /* the occupancy fb may be the same as the "standard" fb (doesn't make sense, but is allowed) */
      if( ( fbidocc != NULL ) && ( StrOp.len( fbidocc ) > 0 ) && ( ! StrOp.equals( fbid, fbidocc ) ) ) {
        count++;
      }
      section = wStage.nextsection( sb, section );
    }
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "countStageblockSectionFeedback: sb %s = %d", 
      wStage.getid( sb ), count );
  return count ;
}

/* to avoid duplicate entries in a (delete-)list we should check if a possible entry is already a member */
static Boolean ismemberoflist( iOList list, obj o) {
  if( ListOp.size(list) > 0 ) {
    iONode node;
    node = (iONode)ListOp.first( list );
    while( node != NULL ) {
      if( (obj)node == o )
        return True;
      node = (iONode)ListOp.next( list );
    }
  }  
  return False;
}

static Boolean markMatchingFbevtForRemoval( iOList delList, iONode bkNode, iONode fbevt, const char* action ) {
  Boolean found = False;

  if( fbevt ) {

    const char* from    = wFeedbackEvent.getfrom( fbevt );
    const char* byroute = wFeedbackEvent.getbyroute( fbevt );
    const char* fbid    = wFeedbackEvent.getid( fbevt );

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "mark fbevt: bl[%s] from[%s] byroute[%s] fbid[%s] action[%s]", wItem.getid(bkNode), from, byroute, fbid, action );

    /* search for occurrences of 'action' for "from"/"byroute" of this block */
    iONode fbevtCheck = wBlock.getfbevent( bkNode );
    while( fbevtCheck != NULL ) {
      const char* checkFrom    = wFeedbackEvent.getfrom( fbevtCheck );
      const char* checkByroute = wFeedbackEvent.getbyroute( fbevtCheck );
      const char* checkFbid    = wFeedbackEvent.getid( fbevtCheck );
      const char* checkAction  = wFeedbackEvent.getaction( fbevtCheck );

      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "mark fbevt: bl(chk)[%s] from[%s] byroute[%s] fbid[%s] action[%s]", wItem.getid(bkNode), checkFrom, checkByroute, checkFbid, checkAction );

      if( StrOp.len(byroute)?StrOp.equals( byroute, checkByroute ):StrOp.equals( from, checkFrom ) ) {
        if( StrOp.equals( checkAction, action ) ) {
          found = True;
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "mark fbevt: set found = True" );
          if( ! ismemberoflist( delList, (obj)fbevtCheck ) ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "mark fbevt: add to delList" );
            ListOp.add( delList, (obj)fbevtCheck );
          }
          else {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "mark fbevt: ALREADY IN LIST" );
          }
        }
      }
    fbevtCheck = wBlock.nextfbevent( bkNode, fbevtCheck );
    }
  }

  return found;
}


static Boolean isThereARouteFromTo( iOAnalyse inst, const char* from, const char* to ) {
  iOAnalyseData data = Data(inst);
  iONode stlist = wPlan.getstlist(data->plan);
  int stcnt = NodeOp.getChildCnt( stlist);

  iONode child = NULL;
  int i;
  for( i = 0; i <stcnt; i++) {
    child = NodeOp.getChild( stlist, i);

    const char* stFrom = wRoute.getbka( child );
    const char* stTo   = wRoute.getbkb( child );

    if( StrOp.equals( from, stFrom ) &&  StrOp.equals( to, stTo ) ) {
      return True;
    }
  }
  return False ;
}


/* do all fbevents of blocks have valid enter and in assignments */
static Boolean blockFeedbackActionCheck( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode bklist = wPlan.getbklist(data->plan);
  int numProblems = 0;

  if( bklist != NULL ) {
    iONode bk = wBlockList.getbk( bklist );
    while( bk != NULL ) {
      const char* bkid = wItem.getid(bk);
      iIBlockBase block = ModelOp.getBlock( data->model, bkid );
      iONode bkNode = BlockOp.base.properties( block );
      iOList delList = ListOp.inst();

      /* 
       * for all "from"
       * does every "from" have "enter2in" or ("enter"/"enter2pre/enter2shortin" and "in")
       * this a double loop over the same fbevent list
       *   (might be ineffective but is the easiest way)
       */

      iONode fbevtFrom = wBlock.getfbevent( bkNode );
      while( fbevtFrom != NULL ) {
        const char* from    = wFeedbackEvent.getfrom( fbevtFrom );
        const char* byroute = wFeedbackEvent.getbyroute( fbevtFrom );
        const char* fbid    = wFeedbackEvent.getid( fbevtFrom );
        const char* action  = wFeedbackEvent.getaction( fbevtFrom );

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block feedback action check: bl(ori)[%s] from[%s] byroute[%s] fbid[%s] action[%s]", bkid, from, byroute, fbid, action );

        Boolean foundEnter  = False;
        Boolean foundIn     = False;
        Boolean foundEnterMulti = False;
        Boolean foundInMulti = False;

        /* search for corresponding occurrences of enter or in for fbevtFrom  in this block */
        iONode fbevtCheck = wBlock.getfbevent( bkNode );
        while( fbevtCheck != NULL ) {
          const char* checkFrom    = wFeedbackEvent.getfrom( fbevtCheck );
          const char* checkByroute = wFeedbackEvent.getbyroute( fbevtCheck );
          const char* checkFbid    = wFeedbackEvent.getid( fbevtCheck );
          const char* checkAction  = wFeedbackEvent.getaction( fbevtCheck );
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block feedback action check: bl(chk)[%s] from[%s] byroute[%s] fbid[%s] action[%s]", bkid, checkFrom, checkByroute, checkFbid, checkAction );

          if( StrOp.len(byroute)?(StrOp.equals( byroute, checkByroute )||StrOp.equals( byroute, checkFrom)):StrOp.equals( from, checkFrom ) ) {
            /* current "from/byroute" is identical", so do the checks */
            /* note: enter2in is checked later separatly because it is valid for both actions */
            if( StrOp.equals( checkAction, wFeedbackEvent.enter_event ) ||
                StrOp.equals( checkAction, wFeedbackEvent.enter2pre_event ) ||
                StrOp.equals( checkAction, wFeedbackEvent.enter2shortin_event ) ) {
              if( ! foundEnter ) {
                foundEnter = True;
                TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block feedback action check: set foundEnter = True" );
              }
              else {
                foundEnterMulti = True;
                TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: bl[%s] from[%s] byroute[%s] fbid[%s] action[%s] after (%s %s %s %s)",
                    bkid, from, byroute, fbid, action, wFeedbackEvent.enter_event, wFeedbackEvent.enter2pre_event, wFeedbackEvent.enter2shortin_event, wFeedbackEvent.enter2in_event );
              }
            }
            if( StrOp.equals( checkAction, wFeedbackEvent.in_event ) ) {
              if( ! foundIn ) {
                foundIn = True;
                TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block feedback action check: set foundIn = True" );
              }
              else {
                foundInMulti = True;
                TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: bl[%s] from[%s] byroute[%s] fbid[%s] action[%s] after (%s %s)",
                    bkid, from, byroute, fbid, action, wFeedbackEvent.in_event, wFeedbackEvent.enter2in_event );
              }
            }
            if( StrOp.equals( checkAction, wFeedbackEvent.enter2in_event ) ) {
              if( ! foundEnter ) {
                foundEnter = True;
                TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block feedback action check: set foundEnter = True" );
              }
              else {
                foundEnterMulti = True;
                TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: bl[%s] from[%s] byroute[%s] fbid[%s] action[%s] after (%s %s %s %s)", 
                    bkid, from, byroute, fbid, action, wFeedbackEvent.enter_event, wFeedbackEvent.enter2pre_event, wFeedbackEvent.enter2shortin_event, wFeedbackEvent.enter2in_event );
              }
              if( ! foundIn ) {
                foundIn = True;
                TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block feedback action check: set foundIn = True" );
              }
              else {
                foundInMulti = True;
                TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: bl[%s] from[%s] byroute[%s] fbid[%s] action[%s] after (%s %s)",
                    bkid, from, byroute, fbid, action, wFeedbackEvent.in_event, wFeedbackEvent.enter2in_event );
              }
            }
          }
        fbevtCheck = wBlock.nextfbevent( bkNode, fbevtCheck );
        }

        if( foundEnterMulti ) {
          if( repair ) {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: bl[%s] from[%s] byroute[%s] found multiple usage of (%s %s %s %s): not repaired",
                bkid, from, byroute, wFeedbackEvent.enter_event, wFeedbackEvent.enter2pre_event, wFeedbackEvent.enter2shortin_event, wFeedbackEvent.enter2in_event );
          } else {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: bl[%s] from[%s] byroute[%s] found multiple usage of (%s %s %s %s)",
                bkid, from, byroute, wFeedbackEvent.enter_event, wFeedbackEvent.enter2pre_event, wFeedbackEvent.enter2shortin_event, wFeedbackEvent.enter2in_event );
            numProblems++;
          }
        }
        if( foundInMulti ) {
          if( repair ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: bl[%s] from[%s] byroute[%s] found multiple usage of (%s %s): not repaired",
              bkid, from, byroute, wFeedbackEvent.in_event, wFeedbackEvent.enter2in_event );
          } else {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: bl[%s] from[%s] byroute[%s] found multiple usage of (%s %s)",
                bkid, from, byroute, wFeedbackEvent.in_event, wFeedbackEvent.enter2in_event );
            numProblems++;
          }
        }
        if( ! foundEnter && foundIn ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: bl[%s] from[%s] byroute[%s] found (%s) but no (%s %s %s)",
              bkid, from, byroute, wFeedbackEvent.in_event, wFeedbackEvent.enter_event, wFeedbackEvent.enter2shortin_event, wFeedbackEvent.enter2pre_event );
          if( repair ) {
            markMatchingFbevtForRemoval( delList, bkNode, fbevtFrom, wFeedbackEvent.in_event ) ;
          }
          numProblems++;
        }
        if( foundEnter && ! foundIn ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: bl[%s] from[%s] byroute[%s] found (%s %s %s) but no (%s)",
              bkid, from, byroute, wFeedbackEvent.enter_event, wFeedbackEvent.enter2pre_event, wFeedbackEvent.enter2shortin_event, wFeedbackEvent.in_event );
          if( repair ) {
            markMatchingFbevtForRemoval( delList, bkNode, fbevtFrom, wFeedbackEvent.enter_event ) ;
            markMatchingFbevtForRemoval( delList, bkNode, fbevtFrom, wFeedbackEvent.enter2pre_event ) ;
            markMatchingFbevtForRemoval( delList, bkNode, fbevtFrom, wFeedbackEvent.enter2shortin_event ) ;
          }
          numProblems++;
        }

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block feedback action check: next" );
        fbevtFrom = wBlock.nextfbevent( bkNode, fbevtFrom );
      }

      if( repair ) {
        /* remove all marked fbevt */
        iONode fbevt ;
        if( ListOp.size(delList) > 0 ) {
          fbevt = (iONode)ListOp.first( delList );
          while( fbevt != NULL ) {
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "block feedback action check: bl[%s] Deleting from %s fbid %s action %s", 
                bkid, wFeedbackEvent.getfrom(fbevt), wFeedbackEvent.getid(fbevt), wFeedbackEvent.getaction(fbevt) );
            NodeOp.removeChild( bkNode, fbevt );
            NodeOp.base.del(fbevt);
            fbevt = (iONode)ListOp.next( delList );
          }
        }
      }
      ListOp.base.del(delList);

      bk =  wBlockList.nextbk( bklist, bk );
    }
  }

  if( numProblems ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: %d problematic entries", numProblems );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block feedback action check: %d problematic enries", numProblems );
  return True;
}


/* do all fbevents of blocks have a valid fb id and valid from/byroute */
static Boolean blockCheck( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode bklist = wPlan.getbklist(data->plan);
  int numProblems = 0;

  if( bklist != NULL ) {
    iONode bk = wBlockList.getbk( bklist );
    while( bk != NULL ) {
      Boolean hasAtLeastOneFbevent = False;
      const char* bkid = wItem.getid(bk);
      iIBlockBase block = ModelOp.getBlock( data->model, bkid );
      iONode bkNode = BlockOp.base.properties( block );
      iOList delList = ListOp.inst();

      iONode fbevt = wBlock.getfbevent( bkNode );
      while( fbevt != NULL ) {
        int numProblemsPre = numProblems ;
        const char* from    = wFeedbackEvent.getfrom( fbevt );
        const char* byroute = wFeedbackEvent.getbyroute( fbevt );
        const char* fbid    = wFeedbackEvent.getid( fbevt );
        const char* action  = wFeedbackEvent.getaction( fbevt );

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block check: bl[%s] from[%s](%d) byroute[%s](%d) fbid[%s](%d) action[%s]",
            bkid, from, StrOp.len(from), byroute, StrOp.len(byroute), fbid, StrOp.len(fbid), action );

        /* try to find fbid in plan */
        if( StrOp.len( fbid ) > 0 ) {
          iOFBack fb = ModelOp.getFBack( data->model, fbid );
          if( fb == NULL ) {
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: block/fbevent:  %s[%s] unknown feedback [%s]",
                 NodeOp.getName(bkNode), wItem.getid(bkNode), fbid );
            if( repair && ! ismemberoflist( delList, (obj)fbevt ) ) {
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block check: add fbevt to delList" );
              ListOp.add( delList, (obj)fbevt );
            }
            numProblems++;
          }
        }

        /* try to find "from" as block */
        if( ( StrOp.len( from ) > 0 ) &&
            ! StrOp.equals( from, wFeedbackEvent.from_all ) &&
            ! StrOp.equals( from, wFeedbackEvent.from_all_reverse )
          ) {
          iIBlockBase bl = ModelOp.getBlock( data->model, from );
          if( bl == NULL ) {
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: block/fbevent:  %s[%s] unknown block [%s]",
                 NodeOp.getName(bkNode), wItem.getid(bkNode), from );
            if( repair && ! ismemberoflist( delList, (obj)fbevt ) ) {
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block check: add fbevt to delList" );
              ListOp.add( delList, (obj)fbevt );
            }
            numProblems++;
          }
        }

        /* try to find "byroute" as route in plan */
        if( ( StrOp.len( byroute ) > 0 ) &&
            ! StrOp.equals( byroute, wFeedbackEvent.from_all ) &&
            ! StrOp.equals( byroute, wFeedbackEvent.from_all_reverse )
          ) {
          iORoute rt = ModelOp.getRoute( data->model, byroute );
          if( rt == NULL ) {
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: block/fbevent:  %s[%s] unknown route [%s]",
                 NodeOp.getName(bkNode), wItem.getid(bkNode), byroute );
            if( repair && ! ismemberoflist( delList, (obj)fbevt ) ) {
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block check: add fbevt to delList" );
              ListOp.add( delList, (obj)fbevt );
            }
            numProblems++;
          }
        }

        if( numProblems == numProblemsPre ) {
          /* no problems with this fbevent detected */
          hasAtLeastOneFbevent = True ;
        }

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block check: next" );
        fbevt = wBlock.nextfbevent( bkNode, fbevt );
      }

      if( ! hasAtLeastOneFbevent ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: block/fbevent:  %s[%s] has no fbevents",
            NodeOp.getName(bkNode), wItem.getid(bkNode));
        if( ! repair )
          numProblems++;
      }

      if( repair ) {
        /* remove all marked fbevt */
        iONode fbevt ;
        if( ListOp.size(delList) > 0 ) {
          fbevt = (iONode)ListOp.first( delList );
          while( fbevt != NULL ) {
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "block check: bl[%s] Deleting from %s fbid %s action %s", 
                bkid, wFeedbackEvent.getfrom(fbevt), wFeedbackEvent.getid(fbevt), wFeedbackEvent.getaction(fbevt) );
            NodeOp.removeChild( bkNode, fbevt );
            NodeOp.base.del(fbevt);
            fbevt = (iONode)ListOp.next( delList );
          }
        }
      }
      ListOp.base.del(delList);

      bk =  wBlockList.nextbk( bklist, bk );
    }
  }

  if( numProblems ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block check: %d problematic entries", numProblems );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block check: %d problematic enries", numProblems );
  return True;
}


/* is the given z level valid inside our plan (no special handling for zlevel 0 !) */
static Boolean isValidZlevel( iOAnalyse inst, int z ) {
  iOAnalyseData data = Data(inst);
  iONode zlevel = wPlan.getzlevel( data->plan );
  while( zlevel != NULL ) {
    if( z == wZLevel.getz( zlevel ) )
      return True;
    zlevel = wPlan.nextzlevel( data->plan, zlevel );
  }
  return False;
}

/* check zlevels and all items on zlevels */
static Boolean zlevelCheck( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode zlevel = wPlan.getzlevel( data->plan );
  iOList delList = ListOp.inst();
  int numWarnings = 0;
  int numProblems = 0;
  int i = 0;

  /* cascaded loop over zlevel title definitions */
  while( zlevel != NULL ) {
    int level = wZLevel.getz( zlevel );
    const char* title = wZLevel.gettitle( zlevel );
    Boolean active = wZLevel.isactive( zlevel );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "zlevel check: level[%d] title[%s] active[%d] name[%s]",
        level, title, active, NodeOp.getName(zlevel) );

    iONode zlevelFollower = wPlan.nextzlevel( data->plan, zlevel );
    while( zlevelFollower != NULL ) {
      int levelFollower = wZLevel.getz( zlevelFollower );
      const char* titleFollower = wZLevel.gettitle( zlevelFollower );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "zlevel cross checking: level[%d] title[%s] <-> level[%d] title[%s]", 
          level, title, levelFollower, titleFollower );
      if( level == levelFollower ) {
        numWarnings++;
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: definition of level[%d] title[%s] is following level[%d] title[%s] (first definition is valid)",
            levelFollower, titleFollower, level, title );
        if( repair && ! ismemberoflist( delList, (obj)zlevelFollower ) ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "zlevel check: add zlevel to delList" );
          ListOp.add( delList, (obj)zlevelFollower );
        }
      }
      
      zlevelFollower = wPlan.nextzlevel( data->plan, zlevelFollower );
    }
    zlevel = wPlan.nextzlevel( data->plan, zlevel );
  }

  if( repair ) {
    /* remove all marked zlevel */
    if( ListOp.size(delList) > 0 ) {
      iONode node = (iONode)ListOp.first( delList );
      while( node != NULL ) {
        int level = wZLevel.getz( node );
        const char* title = wZLevel.gettitle( node );
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "DELETING duplicate definition of level[%d] title[%s]", 
            level, title );
        NodeOp.removeChild( data->plan, node );

        node = (iONode)ListOp.next( delList );
      }
    }
    delList = ListOp.inst();
  }


  /* checking items */
  int dbs = NodeOp.getChildCnt(data->plan);
  int numItemsTotal = 0;
  for( i = 0; i < dbs; i++ ) {
    iOMap idMap = MapOp.inst();
    iONode db = NodeOp.getChild( data->plan, i );
    int items = NodeOp.getChildCnt(db);
    int n = 0;
    int numItems = 0 ;

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checking list [%s](%d)...", NodeOp.getName(db), items );
    for( n = 0; n < items; n++ ) {
      iONode item = NodeOp.getChild( db, n );
      const char* itemName = NodeOp.getName(item) ;
      Boolean show = False;
      Boolean valPlan = False;
      int z = -2;
      numItems++;
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "item[%s]", wItem.getid(item) );

      /* known types without z-levels ... */
      if( StrOp.equals( itemName, wLoc.name() ) ||
          StrOp.equals( itemName, wRoute.name() ) ||
          StrOp.equals( itemName, wAction.name() ) ||
          StrOp.equals( itemName, wActionCtrl.name() ) ||
          StrOp.equals( itemName, wLocation.name() ) ||
          StrOp.equals( itemName, wSchedule.name() ) ||
          StrOp.equals( itemName, wCar.name() ) ||
          StrOp.equals( itemName, wWaybill.name() ) ||
          StrOp.equals( itemName, wOperator.name() ) ||
          StrOp.equals( itemName, wTour.name() ) ||
          StrOp.equals( itemName, wLink.name() ) ||
          StrOp.equals( itemName, wBooster.name() ) ||
          StrOp.equals( itemName, wMVTrack.name() )
        ) {
        /* Ignore */
        continue;
      }

      /* known types with z-levels ... */
      if( StrOp.equals( itemName, wTrack.name()  ) ||
          StrOp.equals( itemName, wFeedback.name() ) ||
          StrOp.equals( itemName, wSwitch.name() ) ||
          StrOp.equals( itemName, wSignal.name() ) ||
          StrOp.equals( itemName, wOutput.name() ) ||
          StrOp.equals( itemName, wBlock.name() ) ||
          StrOp.equals( itemName, wStage.name() ) ||
          StrOp.equals( itemName, wTurntable.name() ) ||
          StrOp.equals( itemName, wSelTab.name() ) ||
          StrOp.equals( itemName, wText.name() )
        ) {
        z = wItem.getz(item) ;
        show = wItem.isshow(item);
        valPlan = isValidZlevel( inst, z );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "zlevelCheck: name[%s] item[%s] z[%d] show[%d] valPlan[%d]", itemName, wItem.getid(item), z, show, valPlan );
        if( ! valPlan ) {
          if( show ) {
            numProblems++;
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: visible item[%s] id[%s] on invalid z level [%d]", itemName, wItem.getid(item), z );
            if( repair ) {
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "DELETING: visible item[%s] id[%s] on invalid z level [%d]", itemName, wItem.getid(item), z );
              /* remove node from current list */
              NodeOp.removeChild( db, item ) ;
              /* adjust positional parameters */
              items--;
              n--;
            }
          }else {
            numWarnings++;
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: invisible item[%s] id[%s] on invalid z level [%d] (show[%d])", itemName, wItem.getid(item), z, show );
          }
        }
      }else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "zlevelCheck: unsupported name[%s] -> skipped", itemName );
      }
    }
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "zlevel check: %d items in [%s]", numItems, NodeOp.getName(db) );
    numItemsTotal += numItems;
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "zlevel check: %d items", numItemsTotal );

  ListOp.base.del(delList);

  if( numWarnings ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "zlevel check: %d warnings", numWarnings );
  }
  if( numProblems ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "zlevel check: %d problems", numProblems );
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "zlevel check: %d problematic and %d warning entries", numProblems, numWarnings );
  return( numProblems == 0 );
}


/* in blocks check/clean fbevents where "from" has no corresponding route to this block */
static int blockRouteFbValidation( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  int numProblems = 0;
  int bkListSize = 0;
  iONode bklist = wPlan.getbklist(data->plan);

  if( bklist ) {
    bkListSize = NodeOp.getChildCnt( bklist );
  }

  if( bkListSize > 0 ) {
    iONode bknode;
    iOBlock bk;
    const char* listType = NodeOp.getName( NodeOp.getChild(bklist, 0));
    int i = 0;
    Boolean thisBlockChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "blockRouteFbValidation: Checking %d %s nodes", bkListSize, listType );
    for( i = bkListSize - 1 ; i >= 0 ; i-- ) {
      bknode = NodeOp.getChild(bklist, i);
      if( bknode ) {
        const char* bkId = wItem.getid( bknode );
        thisBlockChanged = False;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "blockRouteFbValidation: Checking node %s", wBlock.getid( bknode ) );

        iONode fbevt = wBlock.getfbevent( bknode );
        iOList delList = ListOp.inst();
        while( fbevt != NULL ) {
          const char* fbid = wFeedbackEvent.getid( fbevt );
          const char* from = wFeedbackEvent.getfrom( fbevt );
          const char* byroute = wFeedbackEvent.getbyroute( fbevt );
          const char* action = wFeedbackEvent.getaction( fbevt );
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "blockRouteFbValidation: Checking from %s fbid %s action %s", from, fbid, action );
          /* check if from is "all" or "all-reverse" and action is "enter" or "in" */
          if( StrOp.len(from) > 0 ) {
            if( StrOp.equals( from, wFeedbackEvent.from_all) || StrOp.equals( from, wFeedbackEvent.from_all_reverse) ) {
              /* "all" or "all-reverse" have no dedicated source -> skip */
            }
            else {
              /* check if byroute and from is valid -> is there a route from "from" to "current bknode" */
              if( ( StrOp.len( byroute ) > 0 ) &&
                  ( ModelOp.getRoute( data->model, byroute ) != NULL ) &&
                  ( isThereARouteFromTo( inst, from, bkId ) )
                ) {
                /* byroute and from is valid -> OK, skip */
                TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "blockRouteFbValidation: byroute %s and from %s fbid %s action %s : valid (test 1)", byroute, from, fbid, action );
              }
              /* check if from is valid -> is there a route from "from" to "current bknode" */
              else if( ( StrOp.len( byroute ) == 0 ) && isThereARouteFromTo( inst, from, bkId ) ) {
                /* route exists -> OK, skip */
                TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "blockRouteFbValidation: from %s fbid %s action %s : valid (test 2)", from, fbid, action );
              }
              else {
                TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "blockRouteFbValidation: from %s (byr %s) fbid %s action %s has no route", from, byroute, fbid, action );
                if( repair ) {
                  ListOp.add( delList, (obj)fbevt );
                  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "blockRouteFbValidation: Marking  from %s fbid %s action %s to remove", from, fbid, action );
                }
                numProblems++;
              }
            }
          }
          fbevt = wBlock.nextfbevent( bknode, fbevt );
        }

        if( repair ) {
          /* remove all marked fbevt */
          iONode fbevt ;
          if( ListOp.size(delList) > 0 ) {
            fbevt = (iONode)ListOp.first( delList );
            while( fbevt != NULL ) {
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "blockRouteFbValidation: bk[%s] Deleting from %s (byroute %s)fbid %s action %s", 
                  bkId, wFeedbackEvent.getfrom(fbevt), wFeedbackEvent.getbyroute(fbevt), wFeedbackEvent.getid(fbevt), wFeedbackEvent.getaction(fbevt) );
              NodeOp.removeChild( bknode, fbevt );
              NodeOp.base.del(fbevt);
              fbevt = (iONode)ListOp.next( delList );
            }
          }
        }
        ListOp.base.del(delList);
      }
    }
  }

  if( numProblems ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "blockRouteFbValidation: %d problematic entries", numProblems );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "blockRouteFbValidation: %d problematic enries", numProblems );
  return True;
}


/* in selection tables remove fbevents where "from" has no corresponding route to this seltab */
static int seltabRouteFbValidation( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  int numProblems = 0;
  int seltabListSize = 0;
  iONode seltablist = wPlan.getseltablist(data->plan);

  if( seltablist ) {
    seltabListSize = NodeOp.getChildCnt( seltablist );
  }

  if( seltabListSize > 0 ) {
    iONode seltabnode;
    iOSelTab seltab;
    const char* listType = NodeOp.getName( NodeOp.getChild(seltablist, 0));
    int i = 0;
    Boolean thisSelTabChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "seltabRouteFbValidation: Checking %d %s nodes", seltabListSize, listType );
    for( i = seltabListSize - 1 ; i >= 0 ; i-- ) {
      seltabnode = NodeOp.getChild(seltablist, i);
      if( seltabnode ) {
        const char* seltabId = wSelTab.getid( seltabnode );
        thisSelTabChanged = False;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "seltabRouteFbValidation: Checking node %s", wSelTab.getid( seltabnode ) );
        /* only change things if prerequisites are ok */
        if( wSelTab.issharedfb(seltabnode) && wSelTab.ismanager(seltabnode) ) {

          iONode fbevt = wSelTab.getfbevent( seltabnode );
          iOList delList = ListOp.inst();
          while( fbevt != NULL ) {
            const char* fbid = wFeedbackEvent.getid( fbevt );
            const char* from = wFeedbackEvent.getfrom( fbevt );
            const char* action = wFeedbackEvent.getaction( fbevt );
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "seltabRouteFbValidation: Checking from %s fbid %s action %s", from, fbid, action );
            /* check if from is "all" or "all-reverse" and action is "enter" or "in" */
            if( StrOp.len(from) > 0 ) {
              if( StrOp.equals( from, wFeedbackEvent.from_all) || StrOp.equals( from, wFeedbackEvent.from_all_reverse) ) {
                /* "all" or "all-reverse" have no dedicated source -> skip */
              }
              else {
                /* check if from is valid -> is there a route from "from" to "current seltabnode" */
                if( isThereARouteFromTo( inst, from, seltabId ) ) {
                  /* route exists -> skip */
                }
                else {
                  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "seltabRouteFbValidation: from %s fbid %s action %s has no route", from, fbid, action );
                  if( repair ) {
                    ListOp.add( delList, (obj)fbevt );
                    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "seltabRouteFbValidation: Marking  from %s fbid %s action %s to remove", from, fbid, action );
                  }
                  numProblems++;
                }
              }
            }
            fbevt = wSelTab.nextfbevent( seltabnode, fbevt );
          }

          if( repair ) {
            /* remove all marked fbevt */
            iONode fbevt ;
            if( ListOp.size(delList) > 0 ) {
              fbevt = (iONode)ListOp.first( delList );
              while( fbevt != NULL ) {
                TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "seltabRouteFbValidation: seltab[%s] Deleting from %s fbid %s action %s", 
                    seltabId, wFeedbackEvent.getfrom(fbevt), wFeedbackEvent.getid(fbevt), wFeedbackEvent.getaction(fbevt) );
                NodeOp.removeChild( seltabnode, fbevt );
                NodeOp.base.del(fbevt);
                fbevt = (iONode)ListOp.next( delList );
              }
            }
          }
          ListOp.base.del(delList);
        }
      }
    }
  }

  if( numProblems ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "seltabRouteFbValidation: %d problematic entries", numProblems );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "seltabRouteFbValidation: %d problematic enries", numProblems );
  return True;
}


static Boolean isEnterSignal( iONode sb, const char* sgid ) {
  const char* eSig = wStage.getentersignal(sb) ;
  if( eSig != NULL && StrOp.equals( eSig, sgid ) ) {
    return(True);
  }
  return(False);
}


static Boolean isStageBlockEnterSignal(iOAnalyse inst, const char* sgid ) {
  iOAnalyseData data = Data(inst);
  iONode sblist = wPlan.getsblist(data->plan);

  if( sblist != NULL ) {
    iONode sb = wStageList.getsb( sblist );
    while( sb != NULL ) {
      const char* eSig = wStage.getentersignal(sb) ;
      if( eSig != NULL && StrOp.equals( eSig, sgid ) ) {
        return(True);
      }
      sb =  wStageList.nextsb( sblist, sb );
    }
  }
  return(False);
}


static Boolean isSignalMain( iONode node ) {
  const char* signaltype = wSignal.getsignal( node );
  if( signaltype == NULL) {
    return( True );
  }
  return( StrOp.equals( wSignal.main, signaltype ) );
}

static Boolean isSignalDistant( iONode node ) {
  const char* signaltype = wSignal.getsignal( node );
  if( signaltype == NULL) {
    return( False );
  }
  return( StrOp.equals( wSignal.distant, signaltype ) );
 }
 

static void __notifyOverlapError( iONode node, iOMap map, const char* key ) {
  iONode prevNode = (iONode)MapOp.get( map, key);
  TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: object [%s] with id [%s] at [%s] overlaps object [%s] with id [%s]",
      NodeOp.getName(node), wItem.getid(node), key,
      NodeOp.getName(prevNode), wItem.getid(prevNode));

  return;
}

/* return False if something is overlapping */ 
static Boolean __prepare(iOAnalyse inst, iOList list, int modx, int mody) {
  iOAnalyseData data = Data(inst);
  Boolean healthy = True;
  Boolean isModplan = ( NULL != ModelOp.getModPlan( data->model ) );
  char key[32] = {'\0'};

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "mod x: %d y: %d ", modx, mody );

  iONode node = NULL;
  iONode nodetmp = (iONode)ListOp.first( list );
  /* clone the node */
  if( nodetmp != NULL)
    node = (iONode)NodeOp.base.clone( nodetmp);

  while( node != NULL ) {
    const char* type = wItem.gettype(node);
    const char* ori = wItem.getori(node);
    if( ori == NULL ) {
      ori = wItem.west;
    }

    if( ! wItem.isshow(node) ) {
      /* invisible item */
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "prepare: not adding key(s) for INVISBLE object [%s] type[%s] id[%s] at [%d,%d,%d]",
          NodeOp.getName(node), type==NULL?"":type, wItem.getid(node), wItem.getx(node), wItem.gety(node), wItem.getz(node) );
    }
    else {
      /* visible item */

      if( StrOp.equals( NodeOp.getName(node), wBlock.name()  ) ||
          StrOp.equals( NodeOp.getName(node), wStage.name()  ) ||
          StrOp.equals( NodeOp.getName(node), wSelTab.name() ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " ------>  block %s with key (old) %s",
            wBlock.getid(node), __createKey( key, node, 0+modx, 0+mody, 0) );

        ListOp.add( data->bklist, (obj)node );
      }

      /* blocks as well in the map! */
      /* put the object in the map  */

      if( isModplan ) {
        /* in module plans for analyser all items are on the same level */
        wItem.setz( node, 0);
      }

      __createKey( key, node, 0+modx, 0+mody, 0);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
          key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );
      if( MapOp.haskey( data->objectmap, key) ) {
        healthy = False;
        __notifyOverlapError( node, data->objectmap, key );
      }
      else
        MapOp.put( data->objectmap, key, (obj)node);

      wItem.setx( node, wItem.getx(node)+modx);
      wItem.sety( node, wItem.gety(node)+mody);

      /* put keys for all covered fields */
      if( StrOp.equals( NodeOp.getName(node), wSwitch.name() ) ) {
        if( StrOp.equals( type, wSwitch.crossing ) ||
            StrOp.equals( type, wSwitch.dcrossing ) ||
            StrOp.equals( type, wSwitch.ccrossing )) {

          if( StrOp.equals( ori, wItem.east ) || StrOp.equals( ori, wItem.west ) ) {
            __createKey( key, node, 1, 0, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, wItem.getori(node), ori );

            if( MapOp.haskey( data->objectmap, key) ) {
              healthy = False;
              __notifyOverlapError( node, data->objectmap, key );
            }
            else
              MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
            __createKey( key, node, 0, 1, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, wItem.getori(node), ori );

            if( MapOp.haskey( data->objectmap, key) ) {
              healthy = False;
              __notifyOverlapError( node, data->objectmap, key );
            }
            else
              MapOp.put( data->objectmap, key, (obj)node);
          }
        }
        if( isDoubleTrackRRCrossing( node ) ) {
          if( StrOp.equals( ori, wItem.east ) || StrOp.equals( ori, wItem.west ) ) {
            __createKey( key, node, 0, 1, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, wItem.getori(node), ori );

            if( MapOp.haskey( data->objectmap, key) ) {
              healthy = False;
              __notifyOverlapError( node, data->objectmap, key );
            }
            else
              MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
            __createKey( key, node, 1, 0, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, wItem.getori(node), ori );

            if( MapOp.haskey( data->objectmap, key) ) {
              healthy = False;
              __notifyOverlapError( node, data->objectmap, key );
            }
            else
              MapOp.put( data->objectmap, key, (obj)node);
          }
        }
      } /* switch */
      else if( StrOp.equals( NodeOp.getName(node), wBlock.name() ) ) {
        int fields = 4;
        if( wBlock.issmallsymbol(node) ) {
          fields = 2;
        }

        int i;
        for (i=1;i<fields;i++) {
          if( StrOp.equals( ori, wItem.east ) || StrOp.equals( ori, wItem.west ) ) {
            __createKey( key, node, i, 0, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );

            if( MapOp.haskey( data->objectmap, key) ) {
              healthy = False;
              __notifyOverlapError( node, data->objectmap, key );
            }
            else
              MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
            __createKey( key, node, 0, i, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );

            if( MapOp.haskey( data->objectmap, key) ) {
              healthy = False;
              __notifyOverlapError( node, data->objectmap, key );
            }
            else
              MapOp.put( data->objectmap, key, (obj)node);
          }
        }
      } /* block */
      else if( StrOp.equals( NodeOp.getName(node), wStage.name() ) ) {

        int fields = 4; /* always 4 fields */
        int i;
        for (i=1;i<fields;i++) {

          if( StrOp.equals( ori, wItem.east ) || StrOp.equals( ori, wItem.west ) ) {
            __createKey( key, node, i, 0, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );

            if( MapOp.haskey( data->objectmap, key) ) {
              healthy = False;
              __notifyOverlapError( node, data->objectmap, key );
            }
            else
              MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
            __createKey( key, node, 0, i, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );

            if( MapOp.haskey( data->objectmap, key) ) {
              healthy = False;
              __notifyOverlapError( node, data->objectmap, key );
            }
            else
              MapOp.put( data->objectmap, key, (obj)node);
          }
        }
      } /* stage */
      else if( StrOp.equals( NodeOp.getName(node), wSelTab.name() ) ) {

        int fields = wSelTab.getnrtracks( node);

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  SELTAB %d", fields);

        int i;
        for (i=1;i<fields;i++) {

          if( StrOp.equals( ori, wItem.east ) || StrOp.equals( ori, wItem.west ) ) {
            __createKey( key, node, i, 0, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );

            if( MapOp.haskey( data->objectmap, key) ) {
              healthy = False;
              __notifyOverlapError( node, data->objectmap, key );
            }
            else
              MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
            __createKey( key, node, 0, i, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );

            if( MapOp.haskey( data->objectmap, key) ) {
              healthy = False;
              __notifyOverlapError( node, data->objectmap, key );
            }
            else
              MapOp.put( data->objectmap, key, (obj)node);
          }
        }
      } /* seltab */

    } /* visible item */
    iONode nextnode = (iONode)ListOp.next( list );
    if( nextnode != NULL)
      node = (iONode)NodeOp.base.clone( nextnode);
    else
      node = NULL;
  }
  return healthy;
}


static const int typeTrackStraight  =    0;
static const int typeTrackCurve     =    1;
static const int typeBlock          =    2;
static const int typeSwitch         =    3;
static const int itemNotInDirection = 1000;

static int __getType(iONode item ) {
  const char* type = NodeOp.getName(item);
  const char* subtype = wItem.gettype(item);

  if( ( StrOp.equals( wTrack.name(), type ) ||  StrOp.equals( wFeedback.name(), type ) ) && StrOp.equals( wTrack.curve, subtype ) ) {
    return typeTrackCurve;
  } else if( StrOp.equals( wSwitch.name(), type ) ) {
    return typeSwitch;
  } else {
    return typeTrackStraight;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  --- GETTYPE: FAILED should not happen");
  return -1;
}


static const int foundBlock      =  100;
static const int twoWayTurnout   =  200;
static const int threeWayTurnout =  300;
static const int dcrossing       =  400;
static const int dcrossingAhead  = 2000;

static int __travel( iOAnalyse inst, iONode item, int travel, int turnoutstate, int* turnoutstate_out, int* x, int* y, const char* key) {
  iOAnalyseData data = Data(inst);

  if( item ) {
    const char* type    = NodeOp.getName(item);
    const char* subtype = wItem.gettype(item);
    const char* itemori = wItem.getori(item);

    int mori = __getOri(item);

    *turnoutstate_out = 0;
    *x = 0;
    *y = 0;

    /* missing default values */
    if( itemori == NULL) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "set default orientation for %s", wItem.getid(item) );
      itemori = wItem.west;
    }

    /* curve -> change dir */
    if( __getType(item) == typeTrackCurve  || (StrOp.equals( type, wFeedback.name() ) && wFeedback.iscurve( item))   ) {
      /* algebra of Rocrail directions */
      if(        travel == 0 &&  StrOp.equals( itemori, wItem.north )) {
        return oriSouth;
      } else if( travel == 0 &&  StrOp.equals( itemori, wItem.east )) {
        return oriNorth;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.west )) {
        return oriWest;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.north )) {
        return oriEast;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.west )) {
        return oriSouth;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.south )) {
        return oriNorth;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.east )) {
        return oriEast;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.south )) {
        return oriWest;
      } else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  -- no valid curve for us! [%s]",
            wItem.getid(item) );
        return -1; /*end of the game */
      }
    } else if (StrOp.equals( wTrack.name(), type ) && StrOp.equals( subtype, wTrack.dir ) ) {

      if( travel == 1 && mori == 3 )
        return travel;
      else if( travel == 3 && mori == 1 )
        return travel;
      else if( travel == 0 && mori == 0 )
        return travel;
      else if( travel == 2 && mori == 2 )
        return travel;
      return itemNotInDirection;
    } else if (StrOp.equals( wTrack.name(), type ) && StrOp.equals( subtype, wTrack.buffer ) ) {

      if( travel == 1 && mori == 1 )
        return travel;
      else if( travel == 3 && mori == 3 )
        return travel;
      else if( travel == 0 && mori == 2 )
        return travel;
      else if( travel == 2 && mori == 0 )
        return travel;
      return itemNotInDirection;
    }
    /* block */
    else if( StrOp.equals( NodeOp.getName(item), wBlock.name() )) {

      int step = 3;
      if( wBlock.issmallsymbol( item )) {
        step = 1;
      }

      if( StrOp.equals( itemori, wItem.west ) || StrOp.equals( itemori, wItem.east )) {
        if( (travel == 2) ) {
          *x = step;
          return travel;
        } else if ( travel == 0)
          return travel;
      }
      else if( StrOp.equals( itemori, wItem.north ) || StrOp.equals( itemori, wItem.south )) {
        if( (travel == 3) ) {
          *y = step;
          return travel;
        } else if ( travel == 1)
          return travel;
      }
      return itemNotInDirection;
    }

    /* stage block */
    else if( StrOp.equals( NodeOp.getName(item), wStage.name() )) {

      int step = 3;

      if( StrOp.equals( itemori, wItem.west ) || StrOp.equals( itemori, wItem.east )) {
        if( (travel == 2) ) {
          *x = step;
          return travel;
        } else if ( travel == 0)
          return travel;
      }
      else if( StrOp.equals( itemori, wItem.north ) || StrOp.equals( itemori, wItem.south )) {
        if( (travel == 3) ) {
          *y = step;
          return travel;
        } else if ( travel == 1)
          return travel;
      }
      return itemNotInDirection;
    }

    /* seltab / fiddle yard */
    else if( StrOp.equals( NodeOp.getName(item), wSelTab.name() )) {

      int step = wSelTab.getnrtracks(item) - 1 ;

      if( StrOp.equals( itemori, wItem.west ) || StrOp.equals( itemori, wItem.east )) {
        if( (travel == 2) ) {
          *x = step;
          return travel;
        } else if ( travel == 0)
          return travel;
      }
      else if( StrOp.equals( itemori, wItem.north ) || StrOp.equals( itemori, wItem.south )) {
        if( (travel == 3) ) {
          *y = step;
          return travel;
        } else if ( travel == 1)
          return travel;
      }
      return itemNotInDirection;
    }
    /* decoupler */
    else if( __getType(item) == typeSwitch && StrOp.equals( subtype, wSwitch.decoupler)) {
      return travel;
    }
    /* accessory 1 (double railroad crossing) */
    else if( isDoubleTrackRRCrossing( item ) ) {
      return travel;
    }
    /* accessory 10/11/12 (single railroad crossing) */
    else if( isSingleTrackRRCrossing( item ) ) {
      return travel;
    }
    /* switch */
    else if( __getType(item) == typeSwitch) {

      /* coming from the points */
      if(        travel == 0 &&  StrOp.equals( itemori, wItem.east )
              && StrOp.equals( subtype, wSwitch.right ) ) {
        if(turnoutstate == 1)
          return oriNorth+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 0 &&  StrOp.equals( itemori, wItem.west )
              && StrOp.equals( subtype, wSwitch.left ) ) {
        if(turnoutstate == 1)
          return oriSouth+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.north )
              && StrOp.equals( subtype, wSwitch.right ) ) {
        if(turnoutstate == 1)
          return oriEast+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.south )
              && StrOp.equals( subtype, wSwitch.left ) ) {
        if(turnoutstate == 1)
          return oriWest+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.west )
              && StrOp.equals( subtype, wSwitch.right ) ) {
        if(turnoutstate == 1)
          return oriSouth+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.east )
              && StrOp.equals( subtype, wSwitch.left ) ) {
        if(turnoutstate == 1)
          return oriNorth+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.south )
              && StrOp.equals( subtype, wSwitch.right ) ) {
        if(turnoutstate == 1)
          return oriWest+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.north )
              && StrOp.equals( subtype, wSwitch.left ) ) {
        if(turnoutstate == 1)
          return oriEast+twoWayTurnout;
        return travel+twoWayTurnout;
      }

      /* coming from the frog -> straight line */
      else if( travel == 0 &&  StrOp.equals( itemori, wItem.west )
              && StrOp.equals( subtype, wSwitch.right ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 0 &&  StrOp.equals( itemori, wItem.east )
              && StrOp.equals( subtype, wSwitch.left ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.south )
              && StrOp.equals( subtype, wSwitch.right ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.north )
              && StrOp.equals( subtype, wSwitch.left ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.east )
             && StrOp.equals( subtype, wSwitch.right ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.west )
             && StrOp.equals( subtype, wSwitch.left ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.north )
            && StrOp.equals( subtype, wSwitch.right ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.south )
            && StrOp.equals( subtype, wSwitch.left ) ) {
        *turnoutstate_out = 0;
        return travel;
      }

      /* coming from the frog -> diverging line */
      else if( travel == 0 &&  StrOp.equals( itemori, wItem.north )
              && StrOp.equals( subtype, wSwitch.right ) ) {
        *turnoutstate_out = 1;
        return oriSouth;
      } else if( travel == 0 &&  StrOp.equals( itemori, wItem.north )
              && StrOp.equals( subtype, wSwitch.left ) ) {
        *turnoutstate_out = 1;
        return oriNorth;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.west )
              && StrOp.equals( subtype, wSwitch.right ) ) {
        *turnoutstate_out = 1;
        return oriWest;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.west )
              && StrOp.equals( subtype, wSwitch.left ) ) {
        *turnoutstate_out = 1;
        return oriEast;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.south )
             && StrOp.equals( subtype, wSwitch.right ) ) {
        *turnoutstate_out = 1;
        return oriNorth;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.south )
             && StrOp.equals( subtype, wSwitch.left ) ) {
        *turnoutstate_out = 1;
        return oriSouth;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.east )
            && StrOp.equals( subtype, wSwitch.right ) ) {
        *turnoutstate_out = 1;
        return oriEast;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.east )
            && StrOp.equals( subtype, wSwitch.left ) ) {
        *turnoutstate_out = 1;
        return oriWest;
      }

      /* crossing */
      else if( StrOp.equals( subtype, wSwitch.crossing ) && wSwitch.getaddr1(item) == 0 && wSwitch.getport1(item) == 0 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " crossing %d travel: %d", wSwitch.isdir(item), travel );

        /* rectcrossing */
        if( wSwitch.isrectcrossing(item)){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " rectcrossing");
          return travel;
        }

        if( !wSwitch.isdir(item)  ) { /* left */
          if( StrOp.equals( itemori, wItem.west ) || StrOp.equals( itemori, wItem.east )) {
            if( (travel == 1) || (travel == 2)) {
              *x = 1;
              return travel;
            }
            return travel;
          }
          else if( StrOp.equals( itemori, wItem.north ) || StrOp.equals( itemori, wItem.south )) {
            if ( (travel == 2) || (travel == 3) ) {
              *y = 1;
              return travel;
            }
            return travel;
          }
        } else if( wSwitch.isdir(item) ) { /* right */
          if( StrOp.equals( itemori, wItem.west ) || StrOp.equals( itemori, wItem.east )) {
            if ( (travel == 3) || (travel == 2) ) {
              *x = 1;
              return travel;
            }
            return travel;
          }
          else if( StrOp.equals( itemori, wItem.north ) || StrOp.equals( itemori, wItem.south )) {
            if( (travel == 0) || (travel == 3) ) {
              *y = 1;
              return travel;
            }
            return travel;
          }
        }
        return itemNotInDirection;
      }
      /* ccrossing */
      else if( StrOp.equals( subtype, wSwitch.ccrossing ) ) {
        /* something was wrong with ccrossing :) */
        char mkey[32] = {'\0'};
        __createKey( mkey, item, 0, 0, 0);

        iONode itemA = (iONode)MapOp.get( data->objectmap, key);
        iONode itemB = (iONode)MapOp.get( data->objectmap, mkey);

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross switch[%s] type: [%s] key[%s] mkey[%s] %08.8X %08.8X, travel[%d]",
            wItem.getid(item), subtype, key, mkey, itemA, itemB, travel);

        if( StrOp.equals( itemori, wItem.west )  || StrOp.equals( itemori, wItem.east ) ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross WE ori[%s] travel[%d] tos[%d] tos_o[%d]", itemori, travel, turnoutstate, *turnoutstate_out );
          if( (travel == 1) || (travel == 3) ) {
            static int nextX = 0;

            if(   StrOp.equals( key, mkey ) && ( itemA == itemB ) ) { *x =     1; nextX = 1; TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross WE 1a");}
            if( ! StrOp.equals( key, mkey ) && ( itemA != itemB ) ) { *x = nextX;            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross WE 1b");}
            if( ! StrOp.equals( key, mkey ) && ( itemA == itemB ) ) { *x =    -1; nextX = 0; TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross WE 1c");}
            if(   StrOp.equals( key, mkey ) && ( itemA != itemB ) ) { *x =     0;            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross WE 1d");}

            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross WE ret[%d] x[%d] y[%d]", travel, *x, *y );
            return travel;
          }
        }
        else if( StrOp.equals( itemori, wItem.north )  || StrOp.equals( itemori, wItem.south ) ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross NS ori[%s] travel[%d] tos[%d] tos_o[%d]", itemori, travel, turnoutstate, *turnoutstate_out );
          if( (travel == 0) || (travel == 2)) {
            static int nextY = 0;

            if(   StrOp.equals( key, mkey ) && ( itemA == itemB ) ) { *y =     1; nextY = 1; TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross NS 1a");}
            if( ! StrOp.equals( key, mkey ) && ( itemA != itemB ) ) { *y = nextY;            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross NS 1b");}
            if( ! StrOp.equals( key, mkey ) && ( itemA == itemB ) ) { *y =    -1; nextY = 0; TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross NS 1c");}
            if(   StrOp.equals( key, mkey ) && ( itemA != itemB ) ) { *y =     0;            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross NS 1d");}

            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross NS ret[%d] x[%d] y[%d]", travel, *x, *y );
            return travel;
          }
        }
      }
      /* dcrossing */
      else if( StrOp.equals( subtype, wSwitch.dcrossing ) ||
               StrOp.equals( subtype, wSwitch.crossing ) && (wSwitch.getaddr1(item) != 0 || wSwitch.getport1(item) != 0 ) )
      {
        if( !wSwitch.isdir(item)  ) { /* left */
          if( StrOp.equals( itemori, wItem.west ) ) { /* left west */
            if( (travel == 0) ) {
              if ( turnoutstate == 0) {
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                return oriSouth+dcrossing;
              }
            } else if ( travel == 1 ) {
              if ( turnoutstate == 1) {
                *x = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                *x = 1;
                return oriEast+dcrossing;
              }
            } else if ( travel == 2 ) {
              if ( turnoutstate == 0) {
                *x = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                *x = 1;
                return oriNorth+dcrossing;
              }
            } else if ( travel == 3 ) {
              if ( turnoutstate == 1) {
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                return oriWest+dcrossing;
              }
            }
          }
          if( StrOp.equals( itemori, wItem.east ) ) { /* left east */
            if( (travel == 0) ) {
              if ( turnoutstate == 0) {
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                return oriSouth+dcrossing;
              }
            } else if ( travel == 1 ) {
              if ( turnoutstate == 1) {
                *x = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                *x = 1;
                return oriEast+dcrossing;
              }
            } else if ( travel == 2 ) {
              if ( turnoutstate == 0) {
                *x = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                *x = 1;
                return oriNorth+dcrossing;
              }
            } else if ( travel == 3 ) {
              if ( turnoutstate == 1) {
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                return oriWest+dcrossing;
              }
            }
          }
          else if( StrOp.equals( itemori, wItem.north )) { /* left north */
            if( (travel == 0) ) {
              if ( turnoutstate == 1) {
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                return oriNorth+dcrossing;
              }
            } else if ( travel == 1 ) {
              if ( turnoutstate == 0) {
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                return oriWest+dcrossing;
              }
            } else if ( travel == 2 ) {
              if ( turnoutstate == 1) {
                *y = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                *y = 1;
                return oriSouth+dcrossing;
              }
            } else if ( travel == 3 ) {
              if ( turnoutstate == 0) {
                *y = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                *y = 1;
                return oriEast+dcrossing;
              }
            }
          }
          else if( StrOp.equals( itemori, wItem.south )) { /* left south */
            if( (travel == 0) ) {
              if ( turnoutstate == 1) {
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                return oriNorth+dcrossing;
              }
            } else if ( travel == 1 ) {
              if ( turnoutstate == 0) {
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                return oriWest+dcrossing;
              }
            } else if ( travel == 2 ) {
              if ( turnoutstate == 1) {
                *y = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                *y = 1;
                return oriSouth+dcrossing;
              }
            } else if ( travel == 3 ) {
              if ( turnoutstate == 0) {
                *y = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                *y = 1;
                return oriEast+dcrossing;
              }
            }
          }
        } else if( wSwitch.isdir(item) ) { /* right */
          if( StrOp.equals( itemori, wItem.west )) { /* right west */
            if( (travel == 0) ) {
              if ( turnoutstate == 0) {
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                return oriNorth+dcrossing;
              }
            } else if ( travel == 1 ) {
              if ( turnoutstate == 1) {
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                return oriWest+dcrossing;
              }
            } else if ( travel == 2 ) {
              if ( turnoutstate == 0) {
                *x = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                *x = 1;
                return oriSouth+dcrossing;
              }
            } else if ( travel == 3 ) {
              if ( turnoutstate == 1) {
                *x = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                *x = 1;
                return oriEast+dcrossing;
              }
            }
          }

          if( StrOp.equals( itemori, wItem.east )) { /* right east */
            if( (travel == 0) ) {
              if ( turnoutstate == 0) {
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                return oriNorth+dcrossing;
              }
            } else if ( travel == 1 ) {
              if ( turnoutstate == 1) {
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                return oriWest+dcrossing;
              }
            } else if ( travel == 2 ) {
              if ( turnoutstate == 0) {
                *x = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                *x = 1;
                return oriSouth+dcrossing;
              }
            } else if ( travel == 3 ) {
              if ( turnoutstate == 1) {
                *x = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                *x = 1;
                return oriEast+dcrossing;
              }
            }
          }
          else if( StrOp.equals( itemori, wItem.north )) { /* right north */
            if( (travel == 0) ) {
              if ( turnoutstate == 1) {
                *y = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                *y = 1;
                return oriSouth+dcrossing;
              }
            } else if ( travel == 1 ) {
              if ( turnoutstate == 0) {
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                return oriEast+dcrossing;
              }
            } else if ( travel == 2 ) {
              if ( turnoutstate == 1) {
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                return oriNorth+dcrossing;
              }
            } else if ( travel == 3 ) {
              if ( turnoutstate == 0) {
                *y = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                *y = 1;
                return oriWest+dcrossing;
              }
            }
          }
          else if( StrOp.equals( itemori, wItem.south )) { /* right south */
            if( (travel == 0) ) {
              if ( turnoutstate == 1) {
                *y = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                *y = 1;
                return oriSouth+dcrossing;
              }
            } else if ( travel == 1 ) {
              if ( turnoutstate == 0) {
                return travel+dcrossing;
              } else if( turnoutstate == 3) {
                return oriEast+dcrossing;
              }
            } else if ( travel == 2 ) {
              if ( turnoutstate == 1) {
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                return oriNorth+dcrossing;
              }
            } else if ( travel == 3 ) {
              if ( turnoutstate == 0) {
                *y = 1;
                return travel+dcrossing;
              } else if( turnoutstate == 2) {
                *y = 1;
                return oriWest+dcrossing;
              }
            }
          }
        }
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "  "
            "DCROSSING travel:%d- turnoutstate: %d itemori: %s id: %s dir: %d",
            travel,turnoutstate, itemori,wItem.getid(item),wSwitch.isdir(item));
        return dcrossingAhead + travel;
      }

      /* threeway */
      else if( StrOp.equals( subtype, wSwitch.threeway ) ) {

        if( StrOp.equals( itemori, wItem.west )) {
          if( (travel == 0) ) {
            if( turnoutstate == 0) { /* center */
              return travel+threeWayTurnout;
            } else if (turnoutstate == 1) { /* left */
              return oriSouth+threeWayTurnout;
            } else if (turnoutstate == 2) { /* right */
              return oriNorth+threeWayTurnout;
            }
          }
          else if ( travel == 1 ) {
            *turnoutstate_out = 1;
            return oriEast;
          }
          else if ( travel == 2 ) {
            *turnoutstate_out = 0;
            return travel;
          }
          else if ( travel == 3 ) {
            *turnoutstate_out = 2;
            return oriEast;
          }
        }
        else if( StrOp.equals( itemori, wItem.north )) {
          if( (travel == 0) ) {
            *turnoutstate_out = 1;
            return oriNorth;
          }
          else if ( travel == 1 ) {
            *turnoutstate_out = 0;
            return travel;
          }
          else if ( travel == 2 ) {
            *turnoutstate_out = 2;
            return oriNorth;
          }
          else if ( travel == 3 ) {
            if( turnoutstate == 0) {
              return travel+threeWayTurnout;
            } else if (turnoutstate == 1) {
              return oriEast+threeWayTurnout;
            } else if (turnoutstate == 2) {
              return oriWest+threeWayTurnout;
            }
          }
        }
        else if( StrOp.equals( itemori, wItem.east )) {
          if( (travel == 0) ) {
            *turnoutstate_out = 0;
            return travel;
          }
          else if ( travel == 1 ) {
            *turnoutstate_out = 2;
            return oriWest;
          }
          else if ( travel == 2 ) {
            if( turnoutstate == 0) {
              return travel+threeWayTurnout;
            } else if (turnoutstate == 1) {
              return oriNorth+threeWayTurnout;
            } else if (turnoutstate == 2) {
              return oriSouth+threeWayTurnout;
            }
          }
          else if ( travel == 3 ) {
            *turnoutstate_out = 1;
            return oriWest;
          }
        }
        else if( StrOp.equals( itemori, wItem.south )) {
          if( (travel == 0) ) {
            *turnoutstate_out = 2;
            return oriSouth;
          }
          else if ( travel == 1 ) {
            if( turnoutstate == 0) {
             return travel+threeWayTurnout;
            } else if (turnoutstate == 1) {
              return oriWest+threeWayTurnout;
            } else if (turnoutstate == 2) {
              return oriEast+threeWayTurnout;
            }
          }
          else if ( travel == 2 ) {
            *turnoutstate_out = 1;
            return oriSouth;
          }
          else if ( travel == 3 ) {
            *turnoutstate_out = 0;
            return travel;
          }
        }
      }

      /* twoway */
      else if( StrOp.equals( subtype, wSwitch.twoway ) ) {

        /* coming from the points */
        if( travel == 0 &&  StrOp.equals( itemori, wItem.west ) ) {
          if(turnoutstate == 1)
            return oriSouth+twoWayTurnout;
          return oriNorth+twoWayTurnout;
        } else if( travel == 1 &&  StrOp.equals( itemori, wItem.south ) ) {
          if(turnoutstate == 1)
            return oriWest+twoWayTurnout;
          return oriEast+twoWayTurnout;
        } else if( travel == 2 &&  StrOp.equals( itemori, wItem.east ) ) {
          if(turnoutstate == 1)
            return oriNorth+twoWayTurnout;
          return oriSouth+twoWayTurnout;
        } else if( travel == 3 &&  StrOp.equals( itemori, wItem.north ) ) {
          if(turnoutstate == 1)
            return oriEast+twoWayTurnout;
          return oriWest+twoWayTurnout;
        }

        /* coming from the frog -> diverging right line (straight)  */
        else if( travel == 0 &&  StrOp.equals( itemori, wItem.south ) ) {
          *turnoutstate_out = 0;
          return oriSouth;
        } else if( travel == 1 &&  StrOp.equals( itemori, wItem.east ) ) {
          *turnoutstate_out = 0;
          return oriWest;
        } else if( travel == 2 &&  StrOp.equals( itemori, wItem.north ) ) {
          *turnoutstate_out = 0;
          return oriNorth;
        } else if( travel == 3 &&  StrOp.equals( itemori, wItem.west ) ) {
          *turnoutstate_out = 0;
          return oriEast;
        }

        /* coming from the frog -> diverging left line (turnout) */
        else if( travel == 0 &&  StrOp.equals( itemori, wItem.north ) ) {
          *turnoutstate_out = 1;
          return oriNorth;
        } else if( travel == 1 &&  StrOp.equals( itemori, wItem.west ) ) {
          *turnoutstate_out = 1;
          return oriEast;
        } else if( travel == 2 &&  StrOp.equals( itemori, wItem.south ) ) {
          *turnoutstate_out = 1;
          return oriSouth;
        } else if( travel == 3 &&  StrOp.equals( itemori, wItem.east ) ) {
          *turnoutstate_out = 1;
          return oriWest;
        }
      }

      /* turnout in wrong direction*/
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  ---------- no valid turnout for us! [%s]", wItem.getid(item) );
        return -1; /* end of the game */
      }

    } else { /* elements which do not change travel direction */
      /* is the item in our direction? */
      if( !(((StrOp.equals( itemori, wItem.north ) || StrOp.equals( itemori, wItem.south ))
                  && (travel == 1 || travel == 3)) ||
          ((StrOp.equals( itemori, wItem.east ) || StrOp.equals( itemori, wItem.west ))
                  && (travel == 0 || travel == 2))) ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
            "    Item [%s] is not in our travel direction giving up",
                  wItem.getid(item) );
        return itemNotInDirection; /* puh */
      } else {
        return travel;
      }
    }
  } else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ITEM==NULL");
  }

  return itemNotInDirection;
}


static iONode __findConnectorCounterpart(iOAnalyse inst, iONode item ) {
  iOAnalyseData data = Data(inst);
  iONode tracklist = wPlan.gettklist(data->plan);
  int trackListSize = 0;
  int tknr = wTrack.gettknr(item);

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__fCC: " );
  if( tracklist != NULL ) {
    trackListSize = NodeOp.getChildCnt( tracklist );
  }

  if( trackListSize > 0 ) {
    iONode tracknode;
    const char* listType = NodeOp.getName( NodeOp.getChild(tracklist, 0));
    int i = 0;
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__fCC: Checking %d %s nodes", trackListSize, listType );
    for( i = trackListSize - 1 ; i >= 0 ; i-- ) {
      tracknode = NodeOp.getChild(tracklist, i);
      if( tracknode && 
          StrOp.equals(NodeOp.getName(tracknode), wTrack.name() ) &&
          ( StrOp.equals(wItem.gettype(tracknode),  wTrack.connector ) || 
            StrOp.equals(wItem.gettype(tracknode),  wTrack.concurveleft ) || 
            StrOp.equals(wItem.gettype(tracknode),  wTrack.concurveright )
          ) &&
          ! StrOp.equals(wItem.getid(tracknode),  wItem.getid(item) ) &&
          ( wTrack.gettknr(tracknode) == tknr )
        ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__fCC: [%s][%s] [%s] is a connector.   cpid[%s] tknr[%d]",
            NodeOp.getName(item), wItem.gettype(item), wItem.getid(item), wTrack.getcounterpartid(item), wTrack.gettknr(item));
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__fCC: [%s][%s] [%s] is a counterpart. cpid[%s] tknr[%d]",
            NodeOp.getName(tracknode), wItem.gettype(tracknode), wItem.getid(tracknode), wTrack.getcounterpartid(tracknode), wTrack.gettknr(tracknode));
        return tracknode;
      }
    }
  }

  return NULL;
}

static Boolean __analyseBehindConnector(iOAnalyse inst, iONode item, iOList route, int travel,
    int turnoutstate, int depth, Boolean toPreRTlist) {
  iOAnalyseData data = Data(inst);
  int xoffset = 0;
  int yoffset = 0;
  char key[32] = {'\0'};

  if( StrOp.equals(NodeOp.getName(item), wTrack.name() ) && 
      ( StrOp.equals(wItem.gettype(item), wTrack.connector ) ||
        StrOp.equals(wItem.gettype(item), wTrack.concurveleft ) ||
        StrOp.equals(wItem.gettype(item), wTrack.concurveright )
      )
    ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "__aBC: [%s][%s] [%s] is a connector. cpid[%s] tknr[%d] travel[%d]",
        NodeOp.getName(item), wItem.gettype(item), wItem.getid(item), wTrack.getcounterpartid(item), wTrack.gettknr(item), travel);
  } else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "__aBC: [%s][%s] [%s] not a connector",
        NodeOp.getName(item), wItem.gettype(item), wItem.getid(item));
    return False;
  }

  if( wTrack.gettknr(item) >= MIN_CONNECTOR_COUNTERPART_NR ) {
    /* trknr [10..99] */
    Boolean found = False;

    /* search for counterpart somewhere else (may be on same level but not necessarly in same direction) !
     * - during search don't care for direction 
     * - use travel from counterpart when continuing search
     */
    iONode nextitem = __findConnectorCounterpart( inst, item );

    if( nextitem ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "found counterpart: [%s]", wItem.getid(nextitem));
      if( StrOp.equals(NodeOp.getName(nextitem), wTrack.name() ) && 
          ( StrOp.equals(wItem.gettype(nextitem), wTrack.connector ) ||
            StrOp.equals(wItem.gettype(nextitem), wTrack.concurveleft ) ||
            StrOp.equals(wItem.gettype(nextitem), wTrack.concurveright )
          )
        ) {
        const char* nextitemori = wItem.getori( nextitem );
        if( nextitemori == NULL )
          nextitemori = wItem.west;
        Boolean found = False;
        if( StrOp.equals( nextitemori, wItem.west ) ) {
          travel = 0;
        } else if( StrOp.equals( nextitemori, wItem.north ) ) {
          travel = 3;
        } else if( StrOp.equals( nextitemori, wItem.east ) ) {
          travel = 2;
        } else if( StrOp.equals( nextitemori, wItem.south ) ) {
          travel = 1;
        }
        __analyseItem(inst, nextitem, route, travel, 0, depth, toPreRTlist);
        return True;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "__aBC: nextItem [%s][%s] [%s] not a connector",
            NodeOp.getName(nextitem), wItem.gettype(nextitem), wItem.getid(nextitem));
      }
    }
    return False;
  }

  int i = 0;
  for ( i = 0; i <= data->maxConnectorDistance; i++) {
    /* search a maximum distance of maxConnectorDistance items for the counterpart (on same level) */
    if( StrOp.equals(wItem.gettype(item), wTrack.connector ) ||
        StrOp.equals(wItem.gettype(item), wTrack.concurveleft ) ||
        StrOp.equals(wItem.gettype(item), wTrack.concurveright )
      ) {
      switch(travel) {
        case oriWest:
          xoffset--;
          __createKey( key, item, xoffset, yoffset, 0);
          break;
        case oriNorth:
          yoffset--;
          __createKey( key, item, xoffset, yoffset, 0);
          break;
        case oriEast:
          xoffset++;
          __createKey( key, item, xoffset, yoffset, 0);
          break;
        case oriSouth:
          yoffset++;
          __createKey( key, item, xoffset, yoffset, 0);
          break;
      }
    }

    /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "next key: %s", key); */
    iONode nextitem = (iONode)MapOp.get( data->objectmap, key);

    if( nextitem != NULL ) {
      Boolean found = False;

      if( StrOp.equals(NodeOp.getName(nextitem), wTrack.name() ) &&
          StrOp.equals(wItem.gettype(nextitem), wTrack.connector ) &&
          ( wTrack.gettknr(nextitem) < MIN_CONNECTOR_COUNTERPART_NR )
        ) {

        const char* nextitemori = wItem.getori( nextitem );
        if( nextitemori == NULL )
          nextitemori = wItem.west;

        if( StrOp.equals( nextitemori, wItem.west ) && travel == 0){
          found = True;
        } else if( StrOp.equals( nextitemori, wItem.north ) && travel == 3){
          found = True;
        } else if( StrOp.equals( nextitemori, wItem.east ) && travel == 2){
          found = True;
        } else if( StrOp.equals( nextitemori, wItem.south ) && travel == 1){
          found = True;
        }
      }

      if( StrOp.equals(NodeOp.getName(nextitem), wTrack.name() ) &&
          StrOp.equals(wItem.gettype(nextitem), wTrack.concurveleft ) &&
          ( wTrack.gettknr(nextitem) < MIN_CONNECTOR_COUNTERPART_NR )
        ) {

        const char* nextitemori = wItem.getori( nextitem );
        if( nextitemori == NULL )
          nextitemori = wItem.west;

        if( StrOp.equals( nextitemori, wItem.west ) && travel == 3){
          travel = 0;
          found = True;
        } else if( StrOp.equals( nextitemori, wItem.north ) && travel == 2){
          travel = 3;
          found = True;
        } else if( StrOp.equals( nextitemori, wItem.east ) && travel == 1){
          travel = 2;
          found = True;
        } else if( StrOp.equals( nextitemori, wItem.south ) && travel == 0){
          travel = 1;
          found = True;
        }
      }

      if( StrOp.equals(NodeOp.getName(nextitem), wTrack.name() ) &&
          StrOp.equals(wItem.gettype(nextitem), wTrack.concurveright ) &&
          ( wTrack.gettknr(nextitem) < MIN_CONNECTOR_COUNTERPART_NR )
        ) {

        const char* nextitemori = wItem.getori( nextitem );
        if( nextitemori == NULL )
          nextitemori = wItem.west;

        if( StrOp.equals( nextitemori, wItem.west ) && travel == 1){
          travel = 0;
          found = True;
        } else if( StrOp.equals( nextitemori, wItem.north ) && travel == 0){
          travel = 3;
          found = True;
        } else if( StrOp.equals( nextitemori, wItem.east ) && travel == 3){
          travel = 2;
          found = True;
        } else if( StrOp.equals( nextitemori, wItem.south ) && travel == 2){
          travel = 1;
          found = True;
        }
      }

      if( found ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "found counterpart: [%s]", wItem.getid(nextitem));
        __analyseItem(inst, nextitem, route, travel, 0, depth, toPreRTlist);
        return True;
      }
    }
  }
  return False;
}

/* */
static Boolean finalizeRouteWithItemAsDestinationblock( iOAnalyse inst, iOList route, iONode item, int travel, Boolean toPreRTlist ) {
  iOAnalyseData data = Data(inst);
  const char* ori = wItem.getori(item);

  if( ori == NULL)
    ori = wItem.west;

  /* determine block enter side */
  const char* state = "-";
  if( StrOp.equals( ori, wItem.west ) && travel == 2) {
    state = "+";
  }else if( StrOp.equals( ori, wItem.north ) && travel == 1) {
    state = "+";
  }else if( StrOp.equals( ori, wItem.east  ) && travel == 0) {
    state = "+";
  }else if( StrOp.equals( ori, wItem.south ) && travel == 3) {
    state = "+";
  }

  /* add item to route */
  iONode itemA = (iONode)NodeOp.base.clone( item );
  wItem.setstate( itemA, state );
  ListOp.add( route, (obj)itemA );

  /* do we have a preRoute or a notRoute */
  iONode startBlock = (iONode)ListOp.first( route );
  if( StrOp.equals( NodeOp.getName(startBlock), wStage.name() ) && StrOp.equals( wItem.getstate(startBlock), "+" ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Route Source [sb] %s%s -> notRTlist", wItem.getid(startBlock), wItem.getstate(startBlock) );
    /* route starting at enter side of staging block */
    ListOp.add( data->notRTlist, (obj)route);
  }
  else if( StrOp.equals(NodeOp.getName(item), wStage.name() ) && StrOp.equals( state, "-" ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Route Destination [sb] %s%s -> notRTlist", wItem.getid(item), state );
    /* route ending at exit side of staging block */
    ListOp.add( data->notRTlist, (obj)route);
  }
  else if( StrOp.equals( wItem.getid(startBlock), wItem.getid(item) ) ) {
    /* route ending at start -> loop route detected */
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Loop route (Source == Destination) %s%s to %s%s -> notRTlist", wItem.getid(startBlock), wItem.getstate(startBlock), wItem.getid(item), state );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Loop route (Source == Destination) %s%s to %s%s. Check your plan.", wItem.getid(startBlock), wItem.getstate(startBlock), wItem.getid(item), state );
    ListOp.add( data->notRTlist, (obj)route);
  }
  else {
    /* if end of route is a selection table that is not in manager mode or not in sharedfb mode then do not create a valid route to it */
    if( StrOp.equals( NodeOp.getName(item), wSelTab.name() ) && ( ! wSelTab.ismanager(item) || ! wSelTab.issharedfb(item) ) ) {
      toPreRTlist = False;
    }
    if( toPreRTlist ) {
      /* add route to routelist */
      ListOp.add( data->preRTlist, (obj)route);
    }
    else {
      /* add route fragment to notRTlist (for analyse) */
      ListOp.add( data->notRTlist, (obj)route);
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "");

  return True;
}

static Boolean __analyseItem(iOAnalyse inst, iONode item, iOList route, int travel, int turnoutstate, int depth, Boolean toPreRTlist) {
  iOAnalyseData data = Data(inst);
  char key[32] = {'\0'};
  iONode nextitem = NULL;
  Boolean theEnd = False;

  const char* itemori = wItem.getori(item);
  if( itemori == NULL) {
   itemori = wItem.west;
  }

  const char* state = " ";
  if( StrOp.equals( NodeOp.getName(item) , wSwitch.name() ) ) {
    if ( StrOp.equals(wItem.gettype(item), wSwitch.right  ) ||
         StrOp.equals(wItem.gettype(item), wSwitch.left   ) ||
         StrOp.equals(wItem.gettype(item), wSwitch.twoway ) ) {
      state = turnoutstate?wSwitch.turnout:wSwitch.straight;
    } else if ( StrOp.equals(wItem.gettype(item), wSwitch.dcrossing )  ) {
      if( turnoutstate == 0) state = wSwitch.straight;
      if( turnoutstate == 1) state = wSwitch.turnout;
      if( turnoutstate == 2) state = wSwitch.left;
      if( turnoutstate == 3) state = wSwitch.right;
    } else if ( StrOp.equals(wItem.gettype(item), wSwitch.crossing ) && (wSwitch.getaddr1(item) != 0 || wSwitch.getport1(item) != 0 )  ) {
      if( turnoutstate == 0) state = wSwitch.straight;
      if( turnoutstate == 1) state = wSwitch.straight;
      if( turnoutstate == 2) state = wSwitch.turnout;
      if( turnoutstate == 3) state = wSwitch.turnout;
    } else if ( StrOp.equals(wItem.gettype(item), wSwitch.threeway )  ) {
      if( turnoutstate == 0) state = wSwitch.straight;
      if( turnoutstate == 1) state = wSwitch.left;
      if( turnoutstate == 2) state = wSwitch.right;
    }
  } else if( StrOp.equals( NodeOp.getName(item), wBlock.name() ) || 
             StrOp.equals( NodeOp.getName(item), wStage.name() ) || 
             StrOp.equals( NodeOp.getName(item), wSelTab.name() )) {
    state = "-";
    if( StrOp.equals( itemori, wItem.west ) && travel == 0){
      state = "+";
    } else if( StrOp.equals( itemori, wItem.north ) && travel == 3){
      state = "+";
    } else if( StrOp.equals( itemori, wItem.east ) && travel == 2){
      state = "+";
    } else if( StrOp.equals( itemori, wItem.south ) && travel == 1){
      state = "+";
    }
  } else if( StrOp.equals(NodeOp.getName(item), wSignal.name() ) ) {
    /* is the signal in our direction ? */
    if( StrOp.equals( itemori, wItem.west ) && travel == 0){
      state = "yes";
    } else if( StrOp.equals( itemori, wItem.north ) && travel == 3){
      state = "yes";
    } else if( StrOp.equals( itemori, wItem.east ) && travel == 2){
      state = "yes";
    } else if( StrOp.equals( itemori, wItem.south ) && travel == 1){
      state = "yes";
    }
  }

  /* is item already in the list ? */
  iONode listitem = (iONode)ListOp.first( route );
  while(listitem) {

    if( StrOp.equals( wItem.getid(item),    wItem.getid(listitem) ) &&
        StrOp.equals( NodeOp.getName(item), NodeOp.getName(listitem) ) &&
        StrOp.equals( NodeOp.getName(item), wSwitch.name() )
      ) {
      /* an already listed switch detected */
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__analyseItem: double switch [%s] [%s] currState[%s] listState[%s]",
          wItem.getid(item), NodeOp.getName(item), state, wItem.getstate(listitem) );
    }

    if( StrOp.equals( wItem.getid(item),    wItem.getid(listitem) ) &&
        StrOp.equals( NodeOp.getName(item), NodeOp.getName(listitem) ) &&
        StrOp.equals( NodeOp.getName(item), wSwitch.name() ) &&
        StrOp.equals( state, wItem.getstate(listitem) )
      ) {
      /* an already listed switch found
       *  a) with same state it is ok -> no action, ignore
       *  b) different state is not ok -> handled below as evil loop
       */
    }else if( StrOp.equals( wItem.getid(item),    wItem.getid(listitem) ) &&
        StrOp.equals( NodeOp.getName(item), NodeOp.getName(listitem) ) &&
        !StrOp.equals(NodeOp.getName(item), wBlock.name() ) &&
        !StrOp.equals(NodeOp.getName(item), wStage.name() ) &&
        !StrOp.equals(NodeOp.getName(item), wSelTab.name() )
        ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "evil loop on [%s] [%s] (depth=%d)",
          wItem.getid(item), NodeOp.getName(item), depth);
      theEnd = True;
      break;
    }
    listitem = (iONode)ListOp.next( route );
  }

  if( theEnd ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "ANALYSER: leaving with theEnd=%s (depth=%d)", theEnd?"True":"False", depth);
    deleteSingleRoute( route );
    return False;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "start analyzing item [%-20s] travel: [%d] name=%s type=%s",
      wItem.getid(item), travel, NodeOp.getName(item), wItem.gettype(item) );

  if( ( ! StrOp.equals(NodeOp.getName(item), wBlock.name() ) && 
        ! StrOp.equals(NodeOp.getName(item), wStage.name() ) && 
        ! StrOp.equals(NodeOp.getName(item), wSelTab.name() ) 
      ) ||
      ( depth == 0 && 
        ( StrOp.equals(NodeOp.getName(item), wBlock.name() ) || 
          StrOp.equals(NodeOp.getName(item), wStage.name() ) || 
          StrOp.equals(NodeOp.getName(item), wSelTab.name() )
        ) 
      ) 
    ) {
    /* add item to route */
    iONode itemA = (iONode)NodeOp.base.clone( item);
    wItem.setstate(itemA, state);
    ListOp.add( route, (obj)itemA );
  }

  if( StrOp.equals(NodeOp.getName(item), wTrack.name() ) &&
      ( StrOp.equals(wItem.gettype(item), wTrack.connector ) ||
        StrOp.equals(wItem.gettype(item), wTrack.concurveleft ) ||
        StrOp.equals(wItem.gettype(item), wTrack.concurveright )
      )
    ) {

    Boolean found = False;

    if( StrOp.equals(wItem.gettype(item), wTrack.connector ) ) {
      if( StrOp.equals( itemori, wItem.west ) && travel == 2){
        found = True;
      } else if( StrOp.equals( itemori, wItem.north ) && travel == 1){
        found = True;
      } else if( StrOp.equals( itemori, wItem.east ) && travel == 0){
        found = True;
      } else if( StrOp.equals( itemori, wItem.south ) && travel == 3){
        found = True;
      }
    }

    if( StrOp.equals(wItem.gettype(item), wTrack.concurveleft ) ) {
      if( StrOp.equals( itemori, wItem.west ) && travel == 2){
        travel = 1;
        found = True;
      } else if( StrOp.equals( itemori, wItem.north ) && travel == 1){
        travel = 0;
        found = True;
      } else if( StrOp.equals( itemori, wItem.east ) && travel == 0){
        travel = 3;
        found = True;
      } else if( StrOp.equals( itemori, wItem.south ) && travel == 3){
        travel = 2;
        found = True;
      }
    }

    if( StrOp.equals(wItem.gettype(item), wTrack.concurveright ) ) {
      if( StrOp.equals( itemori, wItem.west ) && travel == 2){
        travel = 3;
        found = True;
      } else if( StrOp.equals( itemori, wItem.north ) && travel == 1){
        travel = 2;
        found = True;
      } else if( StrOp.equals( itemori, wItem.east ) && travel == 0){
        travel = 1;
        found = True;
      } else if( StrOp.equals( itemori, wItem.south ) && travel == 3){
        travel = 0;
        found = True;
      }
    }

    if( found ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "found connector: [%s] travel: [%d] ori: [%s]",
              wItem.getid(item), travel, itemori);

      if( wTrack.getcounterpartid(item) != NULL && !StrOp.equals( wTrack.getcounterpartid(item), "") ){
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "found counterpart for: [%s] counterpart: [%s]",
            wItem.getid(item), wTrack.getcounterpartid(item) );
        iOTrack track = ModelOp.getTrack( data->model, wTrack.getcounterpartid(item) );

        /* go on at the connector */
        if( track != NULL ) {
          iONode nextitem = TrackOp.base.properties(track);
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "continue at counterpart: [%s]", wItem.getid(nextitem) );
          depth++;
          __analyseItem(inst, nextitem, route, travel, turnoutstate, depth, toPreRTlist);
        }

      } else {
        Boolean found = __analyseBehindConnector(inst, item, route, travel, 0, depth, toPreRTlist);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__analyseBehindConnector %s", found?"True":"False" );
        if( found == False ) {
          /* save the route fragment */
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " -> stop: no counterpart for connector %s %s found. travel %d", 
              NodeOp.getName(item), wItem.getid(item), travel );
          ListOp.add( data->notRTlist, (obj)route);
        }
        return found;
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "depth: [%d]", depth );
  
  if( depth > 100 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ANALYSER: max. recursion depth (%d > 100) reached", depth);
    return False;
  }

  int x = 0;
  int y = 0;

  int xoffset = 0;
  int yoffset = 0;

  int turnoutstate_out;

  /* get next item */
  travel = __travel( inst, item, travel, turnoutstate, &turnoutstate_out, &x, &y, "");
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "TRAVEL NEXT travel[%d] tos[%d] tos_o[%d] x[%d] y[%d]", travel,  turnoutstate, turnoutstate_out, x, y );

  if( travel >= 200 && travel < 300) {
    travel -= twoWayTurnout;
  }
  if( travel >= 300 && travel < 400) {
    travel -= threeWayTurnout;
  }
  if( travel >= 400 && travel < 500) {
    travel -= dcrossing;
  }

  xoffset += x;
  yoffset += y;

  switch(travel) {
    case oriWest:
      xoffset--;
      __createKey( key, item, xoffset, yoffset, 0);
      break;
    case oriNorth:
      yoffset--;
      __createKey( key, item, xoffset, yoffset, 0);
      break;
    case oriEast:
      xoffset++;
      __createKey( key, item, xoffset, yoffset, 0);
      break;
    case oriSouth:
      yoffset++;
      __createKey( key, item, xoffset, yoffset, 0);
      break;
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "next key: %s", key);
  nextitem = (iONode)MapOp.get( data->objectmap, key);

  if( nextitem != NULL) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "next item: %s tos: [%d]",
        NodeOp.getName(nextitem), turnoutstate_out );

    /* check if direction of nextitem suits */
    int travelp = __travel( inst, nextitem, travel, 0, &turnoutstate_out, &x, &y, key);
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "TRAVEL NEXT travelp[%d] x[%d] y[%d]", travelp, x, y );

    if( (travelp == itemNotInDirection || travelp == -1) && travelp != dcrossingAhead) {
      if( StrOp.equals(NodeOp.getName(nextitem), wTrack.name() ) && StrOp.equals( wItem.gettype(nextitem), wTrack.dir ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "start analyzing item [%-20s] travel: [%d] name=%s type=%s",
            wItem.getid(nextitem), travel, NodeOp.getName(nextitem), wItem.gettype(nextitem) );

        /* continue at dir against direction... to get some info for feedbacks, signal, blockid etc. */
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, " dir [%s] against travel detected. travel=%d travelp=%d", wItem.getid(nextitem), travel, travelp );
        /* if ori of wTrack.dir is opposite of "travel" (dir is exacltly opposite direction)
         * then save to list, jump over it in direction "travel" and check if that nextNextItem suits
         */
        if( ! ( ( ( travel == oriWest  ) && ( __getOri( nextitem ) == oriEast  ) ) ||
                ( ( travel == oriEast  ) && ( __getOri( nextitem ) == oriWest  ) ) ||
                ( ( travel == oriNorth ) && ( __getOri( nextitem ) == oriNorth ) ) ||
                ( ( travel == oriSouth ) && ( __getOri( nextitem ) == oriSouth ) ) 
              )
          ) {
          /* DIR is not exactly in opposite direction -> add route fragment to notRTlist and leave */
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " -> stop: next item %s %s not in direction. travel %d travelp %d turnoutstate %d", 
              NodeOp.getName(nextitem), wItem.getid(nextitem), travel, travelp, turnoutstate );
          ListOp.add( data->notRTlist, (obj)route);
          return False;
        }
        /* DIR is exactly in opposite direction. continue to be able to set blockid and/or find feedback (enter) sensors for starting block */

        /* set mark so this will never get a real route */
        toPreRTlist = False;

        /* add nextitem to route fragment */
        depth++;
        iONode itemA = (iONode)NodeOp.base.clone( nextitem);
        wItem.setstate(itemA, state);
        ListOp.add( route, (obj)itemA );

        /* create key for the item following nextitem */
        __createKey( key, nextitem, xoffset, yoffset, 0);

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "xoffset %d yoffset %d nextNext key: %s", xoffset, yoffset, key);

        /* get item after nextitem. name it nextNextItem */
        iONode nextNextItem = (iONode)MapOp.get( data->objectmap, key);

        if( nextNextItem != NULL ) {
          if( StrOp.equals(NodeOp.getName(nextNextItem), wBlock.name() ) ||
              StrOp.equals(NodeOp.getName(nextNextItem), wStage.name() ) ||
              StrOp.equals(NodeOp.getName(nextNextItem), wSelTab.name() ) ) {
            /* nextNext is a block -> save route fragment in notRTlist and end search */
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, " -> stop: nextNext is an ENDBLOCK" );
            ListOp.add( data->notRTlist, (obj)route);
            return False;
          }
          else {
            /* check if direction of nextNextItem suits */
            int travelp2 = __travel( inst, nextNextItem, travel, 0, &turnoutstate_out, &x, &y, key);
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "TRAVEL NEXTNEXT travelp2[%d] x[%d] y[%d]", travelp2, x, y );
            if( (travelp2 == itemNotInDirection || travelp2 == -1) && travelp2 != dcrossingAhead) {
              /* nextNextItem is also not fitting -> give up -> save route fragment in notRTlist and end this search */
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, " -> stop: nextNext [%s][%s] not fitting -> end tr[%d] trp[%d] trp2[%d] oI[%d] oNI[%d] oNI2[%d]",
                  NodeOp.getName(nextNextItem), wItem.getid(nextNextItem), travel, travelp, travelp2, __getOri( item ), __getOri( nextitem ), __getOri( nextNextItem ) );

              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "end   analyzing item [%-20s] travel: [%d] name=%s type=%s",
                  wItem.getid(nextNextItem), travel, NodeOp.getName(nextNextItem), wItem.gettype(nextNextItem) );
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " -> stop: [%s][%s] 2nd consecutive dir against direction -> end of search",
                  NodeOp.getName(nextNextItem), wItem.getid(nextNextItem) );
              ListOp.add( data->notRTlist, (obj)route);
              return False;
            }
            else {
               /* continue at nextNextItem */
              __analyseItem(inst, nextNextItem, route, travel, turnoutstate, depth, toPreRTlist);
              return False;
            }
          }
        }
        return False;
      }
      else {
        /* a possible route(fragment) that does not end in a valid item (valid are: bl sb seltab buffer connector(?) ) -> leave */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " -> stop: next item %s %s not in direction. travel %d travelp %d turnoutstate %d", 
            NodeOp.getName(nextitem), wItem.getid(nextitem), travel, travelp, turnoutstate );
        ListOp.add( data->notRTlist, (obj)route);
        return False;
      }
    }

    if( StrOp.equals( NodeOp.getName(nextitem), wTrack.name() ) && StrOp.equals( wItem.gettype(nextitem), wTrack.buffer ) ) {
      /* suiting travel direction was already checked above with __travel */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "end   analyzing item [%-20s] travel: [%d] name=%s type=%s",
          wItem.getid(nextitem), travel, NodeOp.getName(nextitem), wItem.gettype(nextitem) );

      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, " buffer detected add route to notRTlist list" );
      /* add buffer to route, then route to notRTlist list */
      depth++;
      iONode itemA = (iONode)NodeOp.base.clone( nextitem);
      wItem.setstate(itemA, state);
      ListOp.add( route, (obj)itemA );
      ListOp.add( data->notRTlist, (obj)route);
      return False;
    } /* if buffer */

    if( StrOp.equals( NodeOp.getName(nextitem), wBlock.name() ) ||
        StrOp.equals( NodeOp.getName(nextitem), wStage.name() ) ||
        StrOp.equals( NodeOp.getName(nextitem), wSelTab.name() )) {
      /* we reached the end block (bkb) */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "end   analyzing item [%-20s] travel: [%d] name=%s type=%s",
          wItem.getid(nextitem), travel, NodeOp.getName(nextitem), wItem.gettype(nextitem) );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " next is a block: [%s]", wItem.getid(nextitem));
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  -> LIST: block [%s] travel: [%d] depth: [%d] tos: [%d]",
          wItem.getid(nextitem), travel, depth, turnoutstate);

      finalizeRouteWithItemAsDestinationblock( inst, route, nextitem, travel, toPreRTlist );

      return True;
    }
    else if( StrOp.equals(NodeOp.getName(nextitem), wSwitch.name() ) &&
             ! StrOp.equals( wItem.gettype(nextitem), wSwitch.decoupler ) && 
             ! StrOp.equals( wItem.gettype(nextitem), wSwitch.accessory )
           ) {
      /* real switch */
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "[%s] next is a switch: [%s] turnoutstate_out: [%d] travelp: [%d]",
          wItem.getid(item), wItem.getid(nextitem), turnoutstate_out, travelp);

      turnoutstate = turnoutstate_out;

      /* item is a turnout -> coming from the points: dive into branches */
      if( travelp >= 200 && travelp < 300) {
        travelp -= twoWayTurnout;
        depth++;

        /* clone partial route before recursion! */
        iOList listA = (iOList)ListOp.base.clone( route);

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into STRAIGHT branch [%s]", wItem.getid(nextitem));
        __analyseItem(inst, nextitem, route, travel, 0, depth, toPreRTlist);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into TURNOUT branch [%s]", wItem.getid(nextitem));
        __analyseItem(inst, nextitem, listA, travel, 1, depth, toPreRTlist);

        return True;
      } else if( travelp >= 300 && travelp < 400) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "three way turnout");
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "     " );
        travelp -= threeWayTurnout;
        depth++;

        /* clone partial route before recursion! */
        iOList listA = (iOList)ListOp.base.clone( route);
        iOList listB = (iOList)ListOp.base.clone( route);

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into STRAIGHT branch [%s]", wItem.getid(nextitem));
        __analyseItem(inst, nextitem, route, travel, 0, depth, toPreRTlist);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into LEFT branch [%s]", wItem.getid(nextitem));
        __analyseItem(inst, nextitem, listA, travel, 1, depth, toPreRTlist);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into RIGHT branch [%s]", wItem.getid(nextitem));
        __analyseItem(inst, nextitem, listB, travel, 2, depth, toPreRTlist);
        return True;
      } else if( (travelp >= 400 && travelp < 500) || travelp >= dcrossingAhead) {

        if( travelp >= dcrossingAhead)
          travelp -= dcrossingAhead;
        else
          travelp -= dcrossing;

        depth++;

        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "dcrossing travel: %d travelp: %d", travel, travelp );

        /* rows: travel  */
        /* cols: itemori */

        const int left[16][2] = {{0,2},{1,2},{0,3},{1,3},
                                 {1,2},{0,3},{1,3},{0,2},
                                 {0,3},{1,3},{0,2},{1,2},
                                 {1,3},{0,2},{1,2},{0,3}};

        const int right[16][2] = {{0,3},{1,2},{0,2},{1,3},
                                  {1,2},{0,2},{1,3},{0,3},
                                  {0,2},{1,3},{0,3},{1,2},
                                  {1,3},{0,3},{1,2},{0,2}};

        int state1 = 0;
        int state2 = 0;

        if( !wSwitch.isdir(nextitem) ) {
          /* left */
          state1 = left[__getOri(nextitem)*4+travelp][0];
          state2 = left[__getOri(nextitem)*4+travelp][1];
        } else if( wSwitch.isdir(nextitem) ) {
          /* right */
          state1 = right[__getOri(nextitem)*4+travelp][0];
          state2 = right[__getOri(nextitem)*4+travelp][1];
        }

        /* clone partial route before recursion! */
        iOList listA = (iOList)ListOp.base.clone( route);

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into %d branch [%s]", state1, wItem.getid(nextitem));
        __analyseItem(inst, nextitem, route, travelp, state1, depth, toPreRTlist);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into %d branch [%s]", state2, wItem.getid(nextitem));
        __analyseItem(inst, nextitem, listA, travelp, state2, depth, toPreRTlist);
        return True;
      } else {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "unhandled after curritem[%s] next is a sw[%s] type[%s] tostate[%d] travelp[%d]",
            wItem.getid(item), wItem.getid(nextitem), wItem.gettype(nextitem), turnoutstate, travelp);
      }
    }
    else if( isDoubleTrackRRCrossing( nextitem ) ) {
      /* double railroad crossing */
      int baseX = 0;
      int baseY = 0;
      int nextitemOri = __getOri( nextitem );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "railroad crossing: [%s](%d-%d-%d) travel[%d] , rrx[%s](%d-%d-%d) travelp[%d] ori[%d]",
          wItem.getid(item), wItem.getx(item), wItem.gety(item), wItem.getz(item), travel,
          wItem.getid(nextitem), wItem.getx(nextitem), wItem.gety(nextitem), wItem.getz(nextitem), travelp, nextitemOri );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "start analyzing item [%-20s] travel: [%d] name=%s type=%s",
          wItem.getid(nextitem), travel, NodeOp.getName(nextitem), wItem.gettype(nextitem) );

      /* add nextitem to route fragment */
      depth++;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "depth: [%d]", depth );
      iONode itemA = (iONode)NodeOp.base.clone( nextitem);
      wItem.setstate(itemA, state);
      ListOp.add( route, (obj)itemA );

      if( nextitemOri == oriWest || nextitemOri == oriEast ) {
        baseX = wItem.getx(nextitem) ;
        baseY = wItem.gety(item) ;
      }else if( nextitemOri == oriNorth || nextitemOri == oriSouth ) {
        baseX = wItem.getx(item) ;
        baseY = wItem.gety(nextitem) ;
      }

      /* rows: travel  */
      /* cols: itemori */
      /*                        itemori W  N  E  S     travel */
      const int xoffsetArray[4][4] = {{-1, 0,-1, 0}, /* west  */
                                      { 0, 0, 0, 0}, /* north */
                                      { 1, 0, 1, 0}, /* east  */
                                      { 0, 0, 0, 0}};/* south */

      const int yoffsetArray[4][4] = {{ 0, 0, 0, 0}, /* west  */
                                      { 0,-1, 0,-1}, /* north */
                                      { 0, 0, 0, 0}, /* east  */
                                      { 0, 1, 0, 1}};/* south */

      xoffset = xoffsetArray[travel][nextitemOri] ;
      yoffset = yoffsetArray[travel][nextitemOri] ;
      StrOp.fmtb( key, "%d-%d-%d", baseX+xoffset, baseY+yoffset, wItem.getz(nextitem) );

      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "railroad crossing: baseX %d baseY %d xoffset %d yoffset %d nextNext key: %s", baseX, baseY, xoffset, yoffset, key);

      /* get item after nextitem. name it nextNextItem */
      iONode nextNextItem = (iONode)MapOp.get( data->objectmap, key);

      if( nextNextItem != NULL ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "railroad crossing: [%s](%d-%d-%d) travel[%d] , rrx[%s](%d-%d-%d) travelp[%d] , [%s](%d-%d-%d)",
            wItem.getid(item), wItem.getx(item), wItem.gety(item), wItem.getz(item), travel,
            wItem.getid(nextitem), wItem.getx(nextitem), wItem.gety(nextitem), wItem.getz(nextitem), travelp,
            wItem.getid(nextNextItem), wItem.getx(nextNextItem), wItem.gety(nextNextItem), wItem.getz(nextNextItem) );

        /* check if direction of nextNextItem suits */
        int travelp2 = __travel( inst, nextNextItem, travel, 0, &turnoutstate_out, &x, &y, key);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "TRAVEL NEXTNEXT travelp2[%d] x[%d] y[%d]", travelp2, x, y );
        if( (travelp2 == itemNotInDirection || travelp2 == -1) && travelp2 != dcrossingAhead) {
          /* nextNextItem is also not fitting -> give up -> save route fragment in notRTlist and end this search */
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, " -> stop: nextNext [%s][%s] not fitting -> end tr[%d] trp[%d] trp2[%d] oI[%d] oNI[%d] oNI2[%d]",
              NodeOp.getName(nextNextItem), wItem.getid(nextNextItem), travel, travelp, travelp2, __getOri( item ), __getOri( nextitem ), __getOri( nextNextItem ) );
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, " -> stop: nextNext [%s][%s] not fitting -> end of search",
              NodeOp.getName(nextNextItem), wItem.getid(nextNextItem) );
          ListOp.add( data->notRTlist, (obj)route);
          return False;
        }
        else {
          if( StrOp.equals(NodeOp.getName(nextNextItem), wBlock.name() ) ||
              StrOp.equals(NodeOp.getName(nextNextItem), wStage.name() ) ||
              StrOp.equals(NodeOp.getName(nextNextItem), wSelTab.name() ) ) {
            /* nextNext is a block -> save route and end search */
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "end   analyzing item [%-20s] travel: [%d] name=%s type=%s",
                wItem.getid(nextNextItem), travel, NodeOp.getName(nextNextItem), wItem.gettype(nextNextItem) );
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " next is a block: [%s]", wItem.getid(nextNextItem));
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  -> LIST: block [%s] travel: [%d] depth: [%d] tos: [%d]",
                wItem.getid(nextNextItem), travel, depth, turnoutstate);

            finalizeRouteWithItemAsDestinationblock( inst, route, nextNextItem, travel, toPreRTlist );

            return True;
          }else {
            depth++;
            /* continue at nextNextItem */
            __analyseItem(inst, nextNextItem, route, travel, turnoutstate, depth, toPreRTlist);
            return False;
          }
        }
      }
      return False;
    }/* if bk || sw */

    depth++;
    __analyseItem(inst, nextitem, route, travel, turnoutstate, depth, toPreRTlist);

  } else { /* nextitem==NULL*/
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "return (nextitem==NULL)");

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " -> stop: no next item after %s %s. travel %d turnoutstate %d", 
        NodeOp.getName(item), wItem.getid(item), travel, turnoutstate );
    ListOp.add( data->notRTlist, (obj)route);
    /* working on a module plan ? perhaps zhe actual item is a connection to the next module... */

    return False;
  } /* item?NULL */

  return True;
}

static void __analyseBlock(iOAnalyse inst, iONode block, const char* inittravel) {
  iOAnalyseData data = Data(inst);
  char key[32] = {'\0'};
  iONode item = NULL;
  int travel;

  if( StrOp.equals( wItem.west , inittravel ) ) travel = oriWest;
  if( StrOp.equals( wItem.north, inittravel ) ) travel = oriNorth;
  if( StrOp.equals( wItem.east , inittravel ) ) travel = oriEast;
  if( StrOp.equals( wItem.south, inittravel ) ) travel = oriSouth;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "--------------------------------------------------");
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "start analyzing block [%s] in [%s][%d] direction ",
      wBlock.getid(block), inittravel, travel);

    int xoffset = 0;
    int yoffset = 0;

    int blocklenghth = 4;
    if( wBlock.issmallsymbol( block ))
      blocklenghth = 2;

    if( StrOp.equals( inittravel, wItem.east ) ) {
      xoffset = blocklenghth-1;
    }

    if( StrOp.equals( inittravel, wItem.south ) ) {
      yoffset = blocklenghth-1;
    }


    iOList route = ListOp.inst();

    /* start the recursion */
    int ret = __analyseItem(inst, block, route, travel, 0, 0, True);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "end analyzing block [%s] in [%s] direction returned: %d",
        wBlock.getid(block), inittravel, ret);
}

static int setBlockidForListItems( iOAnalyse inst, iOList routeFrag, int first, int last ) {
  iOAnalyseData data = Data(inst);
  int modifications = 0;
  int pos = 0 ;
  
  iONode node = (iONode)ListOp.get( routeFrag, last );
  const char* blid = wItem.getid( node );

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sBidFLI: blid[%s] first[%d] last[%d]", blid, first, last );

  for( pos = first ; pos <= last ; pos++ ) {
    iONode item = (iONode)ListOp.get( routeFrag, pos );
    const char* typ = NodeOp.getName(item);

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sBidFLI: blid[%s] first[%d] last[%d] pos[%d] item[%s][%s]",
        blid, first, last, pos, typ, wItem.getid(item) );

    if( StrOp.equals( typ, wTrack.name() ) ) {
      iOTrack track = ModelOp.getTrack( data->model, wItem.getid(item) );
      node = TrackOp.base.properties(track);
      const char* blockid = wItem.getblockid(node);
      /* only set blockid if not already set */
      if( ( blockid == NULL ) || ( StrOp.len( blockid ) == 0 ) ) {
        wTrack.setblockid(node, blid);
        modifications++;
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "sBidFLI: setblockid [%s] for [%s][%s]", blid, NodeOp.getName(item), wItem.getid(item) );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sBidFLI: setblockid [%s] for [%s][%s] skipped because already %s",
            blid, NodeOp.getName(item), wItem.getid(item), blockid );
        if( ! StrOp.equals( blid, blockid ) ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sBidFLI: setblockid [%s] for [%s][%s] skipped AND NOT EQUAL to current entry %s",
            blid, NodeOp.getName(item), wItem.getid(item), blockid );
        }
      }
    }
    else if( StrOp.equals( typ, wFeedback.name() ) ) {
      iOFBack fback = ModelOp.getFBack( data->model, wItem.getid(item) );
      node = FBackOp.base.properties(fback);
      const char* blockid = wItem.getblockid(node);
      /* only set blockid if not already set */
      if( ( blockid == NULL ) || ( StrOp.len( blockid ) == 0 ) ) {
        wFeedback.setblockid(node, blid);
        modifications++;
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "sBidFLI: setblockid [%s] for [%s][%s]",
            blid, NodeOp.getName(item), wItem.getid(item) );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sBidFLI: setblockid [%s] for [%s][%s] skipped because already %s",
            blid, NodeOp.getName(item), wItem.getid(item), blockid );
        if( ! StrOp.equals( blid, blockid ) ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setblockid [%s] for [%s][%s] skipped. It is not equal to current entry %s",
              blid, NodeOp.getName(item), wItem.getid(item), blockid );
        }
      }
    }
    else if( StrOp.equals( typ, wSignal.name() ) ) {
      iOSignal signal = ModelOp.getSignal( data->model, wItem.getid(item) );
      node = FBackOp.base.properties(signal);
      const char* blockid = wItem.getblockid(node);
      /* only set blockid if not already set */
      if( ( blockid == NULL ) || ( StrOp.len( blockid ) == 0 ) ) {
        wSignal.setblockid(node, blid);
        modifications++;
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "sBidFLI: setblockid [%s] for [%s][%s]",
            blid, NodeOp.getName(item), wItem.getid(item) );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sBidFLI: setblockid [%s] for [%s][%s] skipped because already %s",
            blid, NodeOp.getName(item), wItem.getid(item), blockid );
        if( ! StrOp.equals( blid, blockid ) ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setblockid: [%s] for [%s][%s] skipped. It is not equal to current entry %s",
              blid, NodeOp.getName(item), wItem.getid(item), blockid );
        }
      }
    }
    else if( StrOp.equals( typ, wBlock.name() ) ) {
      /* blocks have no blockid */
    }
    else if( StrOp.equals( typ, wStage.name() ) ) {
      /* staging blocks have no blockid */
    }
    else if( StrOp.equals( typ, wSelTab.name() ) ) {
      /* selection tables have no blockid */
    }
    else if( StrOp.equals( typ, wSwitch.name() ) && 
             ! StrOp.equals( wItem.gettype(item), wSwitch.decoupler ) &&
             ! isSingleTrackRRCrossing( item ) 
           ) {
      /* switch has a blockid, but we do not set it */
    }
    else if( StrOp.equals( typ, wOutput.name() ) ) {
      /* output has a blockid, but we do not set it */
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "sBidFLI: setblockid [%s] for [%s][%s] UNKNOWN/UNEXPECTED",
          blid, NodeOp.getName(item), wItem.getid(item) );
    }
  }
  return modifications;
}

/* set block id inside sections of a staging block */
static int setBlockIDinsideSBsections( iOAnalyse inst, iOList routeFrag ) {
  iOAnalyseData data = Data(inst);
  const char* sb        = NULL;
  const char* sbside    = NULL;
  const char* sbtyp     = NULL;
  iONode      sbnode    = NULL;
  int         fbcount   = 0;
  int	      savedIdx  = ListOp.getIndex( routeFrag );
  Boolean     rt_setBl  = True;
  int         firstItem = -1;
  int         lastItem  = -1;

  sbnode = (iONode)ListOp.first( routeFrag );
  sb = wItem.getid(sbnode);
  sbside = wItem.getstate(sbnode);
  sbtyp = NodeOp.getName(sbnode);
  fbcount = countStageblockSectionFeedback( sbnode );

  int modifications = 0;

  if( fbcount == 0 ) {
    /* a staging block without any feedbacks for sections !  */
  }
  else {
    iONode item = (iONode)ListOp.next( routeFrag );
    firstItem = ListOp.getIndex( routeFrag );
    while( rt_setBl && item ) { /* loop1 */
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sBIDiS: checkitem  [%s] for [%s][%s] state=%s",
          sb, NodeOp.getName(item), wItem.getid(item), sb, wItem.getstate(item) );

      if( StrOp.equals( NodeOp.getName(item), wBlock.name()) ||
          StrOp.equals( NodeOp.getName(item), wStage.name()) ||
          StrOp.equals( NodeOp.getName(item), wSelTab.name())
        ) {
        /* reached an end block while searching feedbacks of staging block */
        rt_setBl = False;
      } /* bk || sb || seltab */
      else if( StrOp.equals( NodeOp.getName(item), wSwitch.name()) && 
               ( StrOp.equals( wItem.gettype(item), wSwitch.decoupler ) ||
                 isSingleTrackRRCrossing(item) ||
                 isDoubleTrackRRCrossing(item)
               )
             ) {
        /* inside staging block no special handling of decoupler and railroad crossings */
      } /* sw */
      else if( StrOp.equals( NodeOp.getName(item), wSwitch.name()) ) {
        /* reached a switch while searching feedbacks of staging block */
        rt_setBl = False;
      } /* sw */
      else if( StrOp.equals( NodeOp.getName(item), wTrack.name()) ) {
        /* inside staging block no special handling of tracks */
      } /* tk */
      else if( StrOp.equals( NodeOp.getName(item), wSignal.name()) ) {
        /* inside staging block no special handling of signal */
      } /* sg */
      else if( StrOp.equals( NodeOp.getName(item), wOutput.name()) ) {
        /* no special handling for output */
      } /* sg */
      else if( StrOp.equals( NodeOp.getName(item), wFeedback.name()) ) {
        /* is fb assigned to startblock ? */
        if( isFeedbackOfBlock( data->model, sbnode, wItem.getid(item) ) ) {
          lastItem = ListOp.getIndex( routeFrag );
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "sBIDiS: [%s][%s] firstIdx[%d] lastIdx[%d]",
              NodeOp.getName(item), wItem.getid(item), firstItem, lastItem );
          int num = setBlockidForListItems( inst, routeFrag, firstItem, lastItem );
          modifications += num;
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sBIDiS: [%s][%s] firstIdx[%d] lastIdx[%d] num[%d] modifications[%d]",
              NodeOp.getName(item), wItem.getid(item), firstItem, lastItem, num, modifications );
          /* set firstItem to item after current feedback */
          firstItem = lastItem + 1;
          /* decrease number of stage block section feedbacks to search for */
          fbcount--;
          if( fbcount == 0 ) {
            /* no more feedbacks left -> stop searching/assigning */
            rt_setBl = False;
          }
        } else {
          /* a feedback that does not belong to sb -> stop searching */
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "sBIDiS: [%s][%s] SET lastItem := %d a feedback that does not belong to sb[%s]",
              NodeOp.getName(item), wItem.getid(item), lastItem, sb );
          rt_setBl = False;
        }
      } /* fb */

      item = (iONode)ListOp.next( routeFrag );
    } /* loop every item in route frag */

  } /* fbcount */


  /* restore list index */
  int foundIdx = ListOp.getIndex( routeFrag );
  ListOp.first( routeFrag );
  int i;
  for( i=0 ; i<savedIdx ; i++ ) {
    ListOp.next( routeFrag );
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sBIDiS: savedIdx[%d] foundIdx[%d] restoredidx[%d]",
      savedIdx, foundIdx, ListOp.getIndex( routeFrag ) ) ;

  return modifications;
}


static int __analyseAllLists(iOAnalyse inst) {
  iOAnalyseData data = Data(inst);
  /* Do our work according to the variables:
   *   setBlockId
   *   addSignalBlockAssignment
   *   addFeedbackBlockAssignment
   */
  int modifications = 0;

  /* merge all members of preRTlist and notRTlist to allRTlist -> just 1 list to work on */
  iOList allRTlist = ListOp.inst();

  /* if TRCLEVEL_DEBUG then list all members (routes and route fragments) while merging */

  int size_preRTlist = ListOp.size( data->preRTlist );
  int size_notRTlist = ListOp.size( data->notRTlist );

  iOList currlist = (iOList)ListOp.first( data->preRTlist );
  while( currlist ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll NEW LIST(pre):" );

    iONode item = (iONode)ListOp.first( currlist );
    while( item ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll  item  [%s] [%s]", NodeOp.getName(item), wItem.getid(item) );
      item = (iONode)ListOp.next( currlist );
    }
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll" );
    /* add current member to allRTlist */
    ListOp.add( allRTlist, (obj)currlist );

    currlist = (iOList)ListOp.next( data->preRTlist );
  }

  currlist = (iOList)ListOp.first( data->notRTlist );
  while( currlist ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll NEW LIST(not):" );
    
    iONode item = (iONode)ListOp.first( currlist );
    while( item ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll  item  [%s] [%s]", NodeOp.getName(item), wItem.getid(item) );
      item = (iONode)ListOp.next( currlist );
    }
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll" );
    /* add current member to allRTlist */
    ListOp.add( allRTlist, (obj)currlist );

    currlist = (iOList)ListOp.next( data->notRTlist );
  }

  int size_allRTlist = ListOp.size( allRTlist );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "anaAll preRTlist(%d)+notRTlist(%d)=allRTlist(%d)", size_preRTlist, size_notRTlist, size_allRTlist);

  /* now we can start our jobs...
   * first:
   *  setfb[all|all-reverse](block)
   *  set[main|dist]sg(block)
   * last (because SG/FB assignments to block has to be finished to determine end of blockID setting ):
   *  setBlockId
   */

  /* assign fb/sg to blocks */
  iOList routeFrag = (iOList)ListOp.first( allRTlist );
  while( ( data->addSignalBlockAssignment || data->addFeedbackBlockAssignment ) && routeFrag ) {
    const char* bka     = NULL;
    const char* bkaside = NULL;
    const char* bkatyp  = NULL;
    iONode      bkanode = NULL;
    Boolean     bkaManaged = False;
    const char* bkb     = NULL;
    const char* bkbside = NULL;
    const char* bkbtyp  = NULL;
    iONode      bkbnode = NULL;

    int         fbcount = 0; /* # of FB to skip when starting from a SB */

    /* local variables for current route frag */
    Boolean rt_addSg = data->addSignalBlockAssignment;
    Boolean rt_addFb = data->addFeedbackBlockAssignment;


    /* save node and id of first item (block|stageblock|seltab) for settings/assignments */
    iONode item = (iONode)ListOp.first( routeFrag );
    bka = wItem.getid(item);
    bkatyp = NodeOp.getName(item);
    if( StrOp.equals( bkatyp, wBlock.name()) ) {
      bkaside = wItem.getstate(item);
      iIBlockBase blocka = ModelOp.getBlock( data->model, wItem.getid(item) );
      bkanode = BlockOp.base.properties( blocka );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "bka [%s] %s is a block", bkatyp, bka );

      /* is this block managed by a selection table then do not assign any feedback sensor to it */
      iIBlockBase managerA = BlockOp.getManager(blocka);
      if( managerA != NULL ) {
        iONode managerNode = SelTabOp.base.properties( managerA );
        const char* manager = wItem.getid( managerNode );
        if( StrOp.len( manager ) > 0 ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "block [%s] %s is managed by %s", bkatyp, bka, manager );
          bkaManaged = True;
          rt_addFb = False;
        }
      }
    }else if( StrOp.equals( bkatyp, wStage.name())) {
      bkaside = wItem.getstate(item);
      iOStage blocka = ModelOp.getStage( data->model, wItem.getid(item) );
      bkanode = StageOp.base.properties( blocka );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "bka [%s] %s is a staging block", bkatyp, bka );
    }else if( StrOp.equals( bkatyp, wSelTab.name())) {
      bkaside = wItem.getstate(item);
      iOSelTab blocka = ModelOp.getSelectiontable( data->model, wItem.getid(item) );
      bkanode = SelTabOp.base.properties( blocka );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "bka [%s] %s is a selection table", bkatyp, bka );
      Boolean bkaSTmngr = wSelTab.ismanager(bkanode);
      Boolean bkaSTshFB = wSelTab.issharedfb(bkanode);
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "bka [%s] %s ismanager[%s] sharedfb[%s]", bkatyp, bka, bkaSTmngr?"True":"False", bkaSTshFB?"True":"False" );
      /* if selection table is not in shared feedback mode and not in manager mode then do not assign any feedback sensors */
      if( ! wSelTab.issharedfb(bkanode) || ! wSelTab.ismanager(bkanode)) {
        rt_addFb = False;
      }
    }else {
      /* this should never happen because already checked while generating lists */
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: bka [%s] %s is NOT A BLOCK", bkatyp, bka );
      bka = NULL;
      bkatyp = NULL;
    }
    if( StrOp.equals( bkatyp, wStage.name()) ) {
      /* we may have to skip/ignore the stageblock fbs before starting work */
      fbcount = countStageblockSectionFeedback( item );
    }

    /* save node and id of last item (maybe a block|stageblock|seltab) for settings/assignments */
    item = (iONode)ListOp.get( routeFrag, ListOp.size(routeFrag)-1 );
    bkb = wItem.getid(item);
    bkbtyp = NodeOp.getName(item);
    if( StrOp.equals( bkbtyp, wBlock.name()) ) {
      bkbside = wItem.getstate(item);
      iIBlockBase blockb = ModelOp.getBlock( data->model, wItem.getid(item) );
      bkbnode = BlockOp.base.properties( blockb );
    } else if( StrOp.equals( bkbtyp, wStage.name()) ) {
      bkbside = wItem.getstate(item);
      iOStage blockb = ModelOp.getStage( data->model, wItem.getid(item) );
      bkbnode = StageOp.base.properties( blockb );
    } else if( StrOp.equals( bkbtyp, wSelTab.name()) ) {
      bkbside = wItem.getstate(item);
      iOSelTab blockb = ModelOp.getSelectiontable( data->model, wItem.getid(item) );
      bkbnode = SelTabOp.base.properties( blockb );
    } else {
      /* end of a former notRTlist member (routeFrag) may be a track (buffer, dir,...) ... */
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "bkb [%s] %s is NOT A BLOCK", bkbtyp, bkb );
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "anaAll [%s%s] -> [%s%s]", bka, bkaside, bkb, bkbside?bkbside:"");

    item = (iONode)ListOp.first( routeFrag );
    if( item ) {
      /* skip first item (starting block) */
      item = (iONode)ListOp.next( routeFrag );
    }

    while( ( rt_addSg || rt_addFb ) && item ) {
      if( StrOp.equals( NodeOp.getName(item), wBlock.name()) ||
          StrOp.equals( NodeOp.getName(item), wStage.name()) ||
          StrOp.equals( NodeOp.getName(item), wSelTab.name())
        ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll: REACHED END BLOCK [%s][%s]", NodeOp.getName(item), wItem.getid(item) );
      }
      else if( StrOp.equals( NodeOp.getName(item), wSwitch.name()) ) {
        if( ( wSwitch.getaddr1(item) == 0 ) &&
            ( wSwitch.getport1(item) == 0 ) && 
            ( StrOp.equals( wSwitch.gettype(item), wSwitch.crossing ) || 
              StrOp.equals( wSwitch.gettype(item), wSwitch.ccrossing ) ) ) {
          /* crossing and centered crossing are not relevant for SG/FB */
        } else if( StrOp.equals( wItem.gettype(item), wSwitch.decoupler ) ) {
          /* decoupler is not relevant for SG/FB */
        } else if( isSingleTrackRRCrossing(item) || isDoubleTrackRRCrossing(item) ) {
          /* railroad crossing is not relevant for SG/FB */
        } else {
          /* a regular switch is always the end of assignments to the starting block */
          rt_addSg = False;
          rt_addFb = False;
        }
      } 
      else if( StrOp.equals( NodeOp.getName(item), wFeedback.name()) ) {
        if( fbcount ) {
          /* skip FBs of a starting SB */
          fbcount--;
        } else {
          if( isStageblockFeedback( data->plan, item ) ) {
            /* we reached a FB of a staging block at end of the route, stop searching/assigning FB/SG to start block */
            rt_addSg = False;
            rt_addFb = False;
          } else if( rt_addFb == False ) {
            /* feedback assign is turned off -> nothing to do */
          } else {
            /* a regular fb */
            const char* bkaAction = wFeedbackEvent.in_event;
            const char* bkaFrom = StrOp.equals( bkaside, "-" )?wFeedbackEvent.from_all:wFeedbackEvent.from_all_reverse;
            if( isFbeventDuplicate( bkanode, item, bkaAction, bkaFrom ) ) {
              /* skip */
            } else {
              /* if bka is a "normal" block */
              iONode fbevent = NodeOp.inst( wFeedbackEvent.name(), NULL, ELEMENT_NODE );
              wItem.setid( fbevent, wItem.getid(item) );
              wFeedbackEvent.setaction( fbevent, bkaAction );
              wFeedbackEvent.setfrom( fbevent, bkaFrom );

              NodeOp.addChild( bkanode, fbevent );
              modifications++;
            }

            bkaAction = wFeedbackEvent.enter_event;
            bkaFrom = StrOp.equals( bkaside, "+" )?wFeedbackEvent.from_all:wFeedbackEvent.from_all_reverse;
            if( isFbeventDuplicate( bkanode, item, bkaAction, bkaFrom ) ) {
              /* skip */
            } else {
              iONode fbevent = NodeOp.inst( wFeedbackEvent.name(), NULL, ELEMENT_NODE );
              wItem.setid( fbevent, wItem.getid(item) );
              wFeedbackEvent.setaction( fbevent, bkaAction );
              wFeedbackEvent.setfrom( fbevent, bkaFrom );

              NodeOp.addChild( bkanode, fbevent );
              modifications++;
            }
          }
        }
      } else if( StrOp.equals( NodeOp.getName(item), wSignal.name()) ) {
        /* add signals only for "normal" blocks (do not touch staging blocks) */

        if( rt_addSg && StrOp.equals( bkatyp, wBlock.name()) ) {

          if( StrOp.equals( wItem.getstate(item), "yes" ) ) {
            /* a valid signal detected that is in our direction */
            const char* signaltype = wSignal.getsignal( item);
            if( signaltype == NULL)
              signaltype = wSignal.main;

            if( StrOp.equals( bkaside, "-") ) {
              if( StrOp.equals( wSignal.getsignal(item), wSignal.main) ) {
                const char* prevSigF = wBlock.getsignal(bkanode);
                /* only add signal, if not already set */
                if( StrOp.len( prevSigF ) == 0 ) {
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll: block %s setsignal [%s][%s] (prevSigF=[%08.8X]%s)",
                      bka, NodeOp.getName(item), wItem.getid(item), prevSigF, prevSigF );
                  wBlock.setsignal(bkanode, wItem.getid(item));
                  modifications++;
                }
                else if( ! StrOp.equals( prevSigF, wItem.getid(item) )) {
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll: block %s setsignal [%s][%s] differs from already set %s",
                      bka, NodeOp.getName(item), wItem.getid(item), prevSigF ) ;
                }
                else if( StrOp.equals( prevSigF, wItem.getid(item) ) ) {
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll: block %s setsignal [%s][%s] == already set %s",
                      bka, NodeOp.getName(item), wItem.getid(item), prevSigF ) ;
                }
              } else if (StrOp.equals( wSignal.getsignal(item), wSignal.distant) ) {
                const char* prevSigFw = wBlock.getwsignal(bkanode);
                if( StrOp.len( prevSigFw ) == 0 ) {
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll: setwsignal [%s][%s] (prevSigFw=%s)",
                      NodeOp.getName(item), wItem.getid(item), prevSigFw );
                  wBlock.setwsignal(bkanode, wItem.getid(item));
                  modifications++;
                }
              }
            } else if( StrOp.equals( bkaside, "+") ) {
              if( StrOp.equals( wSignal.getsignal(item), wSignal.main) ) {
                const char* prevSigR = wBlock.getsignalR(bkanode);
                if( StrOp.len( prevSigR ) == 0 ) {
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll: setsignalR [%s][%s] (prevSigR=%s)",
                      NodeOp.getName(item), wItem.getid(item), prevSigR );
                  wBlock.setsignalR(bkanode, wItem.getid(item));
                  modifications++;
                }
              } else if (StrOp.equals( wSignal.getsignal(item), wSignal.distant) ) {
                const char* prevSigRw = wBlock.getwsignalR(bkanode);
                if( StrOp.len( prevSigRw ) == 0 ) {
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll: setwsignalR [%s][%s] (prevSigRw=%s)",
                      NodeOp.getName(item), wItem.getid(item), prevSigRw );
                  wBlock.setwsignalR(bkanode, wItem.getid(item));
                  modifications++;
                }
              }
            }
          } else {
            /* we reach a signal that doesn't fit in our direction */
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                "anaAll: signal [%s][%s] is against our direction -> stop searching signal for block [%s][%s]",
                NodeOp.getName(item), wItem.getid(item), bka, bkaside);

            /* main signal in wrong direction -> stop searching for signals for current block */
            if( isSignalMain( item ) ) {
              rt_addSg = False;
            }
            /* non main in wrong direction */
            else {
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                  "anaAll: signal [%s][%s] is against our direction (NON MAIN) a[%s][%s] b[%s][%s]",
                  NodeOp.getName(item), wItem.getid(item), bka, bkaside, bkb, bkbside );
            }
            /* may be a distant signal to set for starting block */
            if( rt_addSg && isSignalDistant( item ) ) {
              /* add distant signal to starting block */
              iIBlockBase block = ModelOp.getBlock( data->model, bka );
              iONode blocknode = BlockOp.base.properties(block);

              /* add signals only for "normal" blocks (do not touch staging blocks) */
              if( StrOp.equals( NodeOp.getName(bkanode), wBlock.name() ) ) {
                if( StrOp.equals( bkaside, "+") ) {
                  const char* prevSigFw = wBlock.getwsignal(bkanode);
                  if( StrOp.len( prevSigFw ) == 0 ) {
                    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll: setwsignal [%s][%s] (prevSigFw=%s)",
                        NodeOp.getName(item), wItem.getid(item), prevSigFw );
                    wBlock.setwsignal(bkanode, wItem.getid(item));
                    modifications++;
                  }
                } else if( ! StrOp.equals( bkaside, "+") ) {
                  const char* prevSigRw = wBlock.getwsignalR(bkanode);
                  if( StrOp.len( prevSigRw ) == 0 ) {
                    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll: setwsignalR [%s][%s] (prevSigRw=%s)",
                        NodeOp.getName(item), wItem.getid(item), prevSigRw );
                    wBlock.setwsignalR(bkanode, wItem.getid(item));
                    modifications++;
                  }
                }
              }
            }
          }
        }
      } else if( StrOp.equals( NodeOp.getName(item), wTrack.name()) ) {
        if( StrOp.equals( wItem.gettype(item), wTrack.dir ) ) {
          /* not relevant for SG/FB */
        } else {
          /* not relevant for SG/FB */
        }
      }
      item = (iONode)ListOp.next( routeFrag );
    }
    routeFrag = (iOList)ListOp.next( allRTlist );
  }


  /* SET BLOCKIDs */
  /* for every route/frag
   *   loop1: determine last item to set blockID
   *   loop2: set blockID
   */
  
  routeFrag = (iOList)ListOp.first( allRTlist );
  while( data->setBlockId && routeFrag) { /* loop every route/-frag */

    const char* bka       = NULL;
    const char* bkaside   = NULL;
    const char* bkatyp    = NULL;
    iONode      bkablock  = NULL;
    Boolean     bkaManaged = False;
    Boolean     bkaIsAsb  = False;
    int         lastItem  = 0;
    Boolean     rt_setBl  = data->setBlockId;

    /* bka (block or stageblock) is first item in list */
    bkablock = (iONode)ListOp.first( routeFrag );
    bka = wItem.getid(bkablock);
    bkaside = wItem.getstate(bkablock);
    bkatyp = NodeOp.getName(bkablock);
    bkaIsAsb = StrOp.equals(NodeOp.getName(bkablock), wStage.name() );

    /* if bka is a stageblock in "right direction" -> special handling of sections needed ? */
    if( bkaIsAsb && StrOp.equals( bkaside, "-" ) ) {
      int mod = setBlockIDinsideSBsections( inst, routeFrag );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll: setblockid for sb [%s] = %d modifications", wItem.getid(bkablock), mod );
      modifications += mod;
      rt_setBl = False;
    }

    if( StrOp.equals( bkatyp, wBlock.name()) ) {
      bkaside = wItem.getstate(bkablock);
      iIBlockBase blocka = ModelOp.getBlock( data->model, wItem.getid(bkablock) );
      iIBlockBase managerA = BlockOp.getManager(blocka);
      if( managerA != NULL ) {
        iONode managerNode = SelTabOp.base.properties( managerA );
        const char* manager = wItem.getid( managerNode );
        if( StrOp.len( manager ) > 0 ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "block [%s] %s is managed by %s", bkatyp, bka, manager );
          bkaManaged = True;
        }
      }
    }

    /* starting block is a "normal" block or a staging block in wrong direction (a frag from notRTlist) !!! */
    /* skip first item in this loop analyse */
    iONode item = (iONode)ListOp.next( routeFrag );
    while( rt_setBl && item ) { /* loop1 */
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop1: checkitem  [%s] for [%s][%s] state=%s", bka, NodeOp.getName(item), wItem.getid(item), bka, wItem.getstate(item) );

      if( StrOp.equals( NodeOp.getName(item), wBlock.name()) ||
          StrOp.equals( NodeOp.getName(item), wStage.name()) ||
          StrOp.equals( NodeOp.getName(item), wSelTab.name())
        ) {
        /* reached end block -> don't update lastItem */
      } /* bk || sb */
      else if( StrOp.equals( NodeOp.getName(item), wTrack.name()) ) {
        const char* subtype = wItem.gettype(item);
        /* if bka is a managed block accept all */
        if( bkaManaged ) {
          lastItem = ListOp.getIndex( routeFrag );
        }
        /* buffer: -> possible last item
         * dir not in direction: -> possible last item
         * dir in direction: -> possible last item
         */
        else if( StrOp.equals( subtype, wTrack.dir ) || StrOp.equals( subtype, wTrack.buffer ) ) {
          lastItem = ListOp.getIndex( routeFrag );
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop1: [%s][%s] SET lastItem := %d", NodeOp.getName(item), wItem.getid(item), lastItem );
        }
        else {
          /* normal track -> don't update lastItem */
        }
      } /* tk */
      else if( StrOp.equals( NodeOp.getName(item), wFeedback.name()) ) {
        /* if bka is a managed block accept all */
        if( bkaManaged ) {
          lastItem = ListOp.getIndex( routeFrag );
        }
        /* is fb assigned to startblock ? */
        else if( isFeedbackOfBlock( data->model, bkablock, wItem.getid(item) ) ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll item loop1: [%s][%s] is member of [%s]", NodeOp.getName(item), wItem.getid(item), bka );
          lastItem = ListOp.getIndex( routeFrag );
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop1: [%s][%s] SET lastItem := %d", NodeOp.getName(item), wItem.getid(item), lastItem );
        } else {
          /* a feedback that does not belong to bka -> don't save postion and stop searching */
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop1: [%s][%s] SET lastItem := %d a feedback that does not belong to bka=%s", NodeOp.getName(item), wItem.getid(item), lastItem, bka );
          rt_setBl = False;
        }
      } /* fb */
      else if( StrOp.equals( NodeOp.getName(item), wSignal.name()) ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop1: [%s][%s] state[%s] [%s]", NodeOp.getName(item), wItem.getid(item), wItem.getstate(item), bka );
        /* if bka is a managed block accept all */
        if( bkaManaged ) {
          lastItem = ListOp.getIndex( routeFrag );
        }
        /* if we start at SB (in wrong direction!) this might be the enter signal */
        else if( bkaIsAsb && isEnterSignal( bkablock, wItem.getid(item) ) ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll item loop1: [%s] [%s] isEnterSignal( %s )", NodeOp.getName(item), wItem.getid(item), bka );
          lastItem = ListOp.getIndex( routeFrag );
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop1: [%s][%s] SET lastItem := %d", NodeOp.getName(item), wItem.getid(item), lastItem );
        }
        else if( isSignalMain(item) && StrOp.equals( wItem.getstate(item), "yes" ) ) {
          /* a main signal is in right direction -> possible last item */
          lastItem = ListOp.getIndex( routeFrag );
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop1: [%s][%s] SET lastItem := %d", NodeOp.getName(item), wItem.getid(item), lastItem );
        }
        else if( isSignalMain(item) && ! StrOp.equals( wItem.getstate(item), "yes" ) ) {
          /* a main signal is in wrong direction -> stop searching last item */
          rt_setBl = False;
        }
      } /* sg */
      else if( StrOp.equals( NodeOp.getName(item), wSwitch.name()) ) {
        /* a real switch is always the end of the blockid settings */
        if( ( wSwitch.getaddr1(item) == 0 ) &&
            ( wSwitch.getport1(item) == 0 ) && 
            ( StrOp.equals( wSwitch.gettype(item), wSwitch.crossing ) || 
              StrOp.equals( wSwitch.gettype(item), wSwitch.ccrossing ) ) ) {
          /* crossing and centered crossing without address are not relevant for end of blockID setting */
        } else if( StrOp.equals( wItem.gettype(item), wSwitch.decoupler ) ) {
          /* decoupler is not relevant for end of blockID setting */
        } else if( isSingleTrackRRCrossing(item) || isDoubleTrackRRCrossing(item) ) {
          /* railroad crossing is not relevant for end of blockID setting */
        } else {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll item loop1: [%s] [%s] SWITCH DETECTED ( %s )", NodeOp.getName(item), wItem.getid(item), bka );

          lastItem = ListOp.getIndex( routeFrag );
          rt_setBl = False;
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop1: [%s][%s] SET lastItem := %d", NodeOp.getName(item), wItem.getid(item), lastItem );
        }
      } /* sw */

      item = (iONode)ListOp.next( routeFrag );
    } /* loop1 */


    /* now setblockid between second item of routeFrag until we reach lastItem */
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: size=%d lastItem=%d", ListOp.size( routeFrag ), lastItem );
    item = (iONode)ListOp.first( routeFrag );
    item = (iONode)ListOp.next( routeFrag );
    while( lastItem && item ) { /* loop2 */
      Boolean isLastItem = (ListOp.getIndex( routeFrag ) == lastItem)?True:False;
      const char* typ = NodeOp.getName(item);
      iONode node = NULL;

      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: isLastItem( [%s][%s] ) = %s",
          NodeOp.getName(item), wItem.getid(item), isLastItem?"True":"False" );

      if( StrOp.equals( typ, wTrack.name() ) ) {
        const char* subtype = wItem.gettype(item);
        if( ( StrOp.equals( subtype, wTrack.dir ) && isLastItem ) ) {
          /* track "dir" is last Item -> don't set blockid */
        }
        else {
          iOTrack track = ModelOp.getTrack( data->model, wItem.getid(item) );
          node = TrackOp.base.properties(track);
          const char* blockid = wItem.getblockid(node);
          /* only set blockid if not already set */
          if( ( blockid == NULL ) || ( StrOp.len( blockid ) == 0 ) ) {
            if( bkaIsAsb ) {
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s] skipped because bkaIsAsb",
                  bka, NodeOp.getName(item), wItem.getid(item) );
            }
            else {
              wTrack.setblockid(node, bka);
              modifications++;
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s]",
                  bka, NodeOp.getName(item), wItem.getid(item) );
            }
          }
          else {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s] skipped because already %s",
                bka, NodeOp.getName(item), wItem.getid(item), blockid );
            if( ! StrOp.equals( bka, blockid ) ) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s] skipped AND NOT EQUAL to current entry %s",
                  bka, NodeOp.getName(item), wItem.getid(item), blockid );
            }
          }
        }
      }
      else if( StrOp.equals( typ, wFeedback.name() ) ) {
        iOFBack fback = ModelOp.getFBack( data->model, wItem.getid(item) );
        node = FBackOp.base.properties(fback);
        const char* blockid = wItem.getblockid(node);
        /* only set blockid if not already set */
        if( ( blockid == NULL ) || ( StrOp.len( blockid ) == 0 ) ) {
          if( bkaIsAsb ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s] skipped because bkaIsAsb",
                bka, NodeOp.getName(item), wItem.getid(item) );
          }
          else {
            wFeedback.setblockid(node, bka);
            modifications++;
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s]",
                bka, NodeOp.getName(item), wItem.getid(item) );
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s] skipped because already %s",
              bka, NodeOp.getName(item), wItem.getid(item), blockid );
          if( ! StrOp.equals( bka, blockid ) ) {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "setblockid [%s] for [%s][%s] skipped. It is not equal to current entry %s",
                bka, NodeOp.getName(item), wItem.getid(item), blockid );
          }
        }
      }
      else if( StrOp.equals( typ, wSignal.name() ) ) {
        iOSignal signal = ModelOp.getSignal( data->model, wItem.getid(item) );
        node = SignalOp.base.properties(signal);
        const char* blockid = wItem.getblockid(node);
        /* only set blockid if not already set */
        if( ( blockid == NULL ) || ( StrOp.len( blockid ) == 0 ) ) {
          if( bkaIsAsb ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s] skipped because bkaIsAsb",
                bka, NodeOp.getName(item), wItem.getid(item) );
          }
          else {
            wSignal.setblockid(node, bka);
            modifications++;
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s]",
                bka, NodeOp.getName(item), wItem.getid(item) );
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s] skipped because already %s",
              bka, NodeOp.getName(item), wItem.getid(item), blockid );
          if( ! StrOp.equals( bka, blockid ) ) {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "setblockid: [%s] for [%s][%s] skipped. It is not equal to current entry %s",
                bka, NodeOp.getName(item), wItem.getid(item), blockid );
          }
        }
      }
      else if( StrOp.equals( typ, wSwitch.name() ) && 
               StrOp.equals( wItem.gettype(item), wSwitch.decoupler )
             ) {
        /* decouplers have a blockid */
        iOSwitch decoupler = ModelOp.getSwitch( data->model, wItem.getid(item) );
        node = SwitchOp.base.properties(decoupler);
        const char* blockid = wItem.getblockid(node);
        /* only set blockid if not already set */
        if( ( blockid == NULL ) || ( StrOp.len( blockid ) == 0 ) ) {
          if( bkaIsAsb ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s][%s] skipped because bkaIsAsb",
                bka, NodeOp.getName(item), wItem.gettype(item), wItem.getid(item) );
          }
          else {
            wSwitch.setblockid(node, bka);
            modifications++;
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s][%s]",
                bka, NodeOp.getName(item), wItem.gettype(item), wItem.getid(item) );
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s][%s] skipped because already %s",
              bka, NodeOp.getName(item), wItem.gettype(item), wItem.getid(item), blockid );
          if( ! StrOp.equals( bka, blockid ) ) {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "setblockid: [%s] for [%s][%s][%s] skipped. It is not equal to current entry %s",
                bka, NodeOp.getName(item), wItem.gettype(item), wItem.getid(item), blockid );
          }
        }
      }
      else if( isSingleTrackRRCrossing(item) ) {
        /* single railroad crossings have a blockid */
        iOSwitch rrCrossS = ModelOp.getSwitch( data->model, wItem.getid(item) );
        node = SwitchOp.base.properties(rrCrossS);
        const char* blockid = wItem.getblockid(node);
        /* only set blockid if not already set */
        if( ( blockid == NULL ) || ( StrOp.len( blockid ) == 0 ) ) {
          if( bkaIsAsb ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s][%s] skipped because bkaIsAsb",
                bka, NodeOp.getName(item), wItem.gettype(item), wItem.getid(item) );
          }
          else {
            wSwitch.setblockid(node, bka);
            modifications++;
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s][%s]",
                bka, NodeOp.getName(item), wItem.gettype(item), wItem.getid(item) );
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s][%s] skipped because already %s",
              bka, NodeOp.getName(item), wItem.gettype(item), wItem.getid(item), blockid );
          if( ! StrOp.equals( bka, blockid ) ) {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "setblockid: [%s] for [%s][%s][%s] skipped. It is not equal to current entry %s",
                bka, NodeOp.getName(item), wItem.gettype(item), wItem.getid(item), blockid );
          }
        }
      }
      else if( StrOp.equals( typ, wBlock.name() ) ) {
        /* blocks have no blockid */
      }
      else if( StrOp.equals( typ, wStage.name() ) ) {
        /* staging blocks have no blockid */
      }
      else if( StrOp.equals( typ, wSelTab.name() ) ) {
        /* selection tables have no blockid */
      }
      else if( StrOp.equals( typ, wSwitch.name() ) ) {
        /* switches have no blockid */
      }
      else if( StrOp.equals( typ, wOutput.name() ) ) {
        /* output has a blockid, but we do not set it */
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s] UNKNOWN/UNEXPECTED", bka, NodeOp.getName(item), wItem.getid(item) );
      }

      if( isLastItem ) {
        /* set stop condition for loop2 */
        item = NULL;
      } else {
        item = (iONode)ListOp.next( routeFrag );
      }
    } /* loop2 */

    routeFrag = (iOList)ListOp.next( allRTlist );
  } /* loop every route/-frag */
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll after all loops size( list ) = %d", ListOp.size( allRTlist ) );

  /* Clear the temporarylist (all items are still in preRTlist and notRTlist */
  ListOp.clear( allRTlist );

  return modifications;
}


static int __generateRoutes(iOAnalyse inst) {
  iOAnalyseData data = Data(inst);
  iONode stlist = wPlan.getstlist(data->plan);
  int modifications = 0;

  const char* bka = NULL;
  const char* bkb = NULL;
  const char* bkaside = NULL;
  const char* bkbside = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " ");

  /* remove all "autogen-"-routes (in case there was no cleanup) */
  iONode child = NULL;
  iOList delList = ListOp.inst();
  int i;
  int childcnt = NodeOp.getChildCnt( stlist);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Searching %d old routes for autogen routes", childcnt );  
  for( i = 0; i <childcnt; i++) {
    child = NodeOp.getChild( stlist, i);

    if( StrOp.startsWith( wItem.getid( child), "autogen-" ) && ! wItem.isgenerated(child) ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "remove autogen route: [%s]", wItem.getid( child));
      ListOp.add( delList, (obj)child );
    }
  }

  childcnt = ListOp.size(delList);
  if( childcnt > 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Removing %d autogen routes", childcnt );  
  }
  for( i = 0; i < childcnt; i++) {
    NodeOp.removeChild( stlist, (iONode)ListOp.get(delList, i) );
  }
  ListOp.base.del(delList);


  /* if option to set "autogen-"routeids is active then 
       we cleanup the old entries before starting reassigning new routeids */
  if( data->setRouteId ) {
    int removedIDs = 0;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Cleaning autogen-routeids in plan");
    removedIDs += _cleanupAutogenRouteids( wPlan.gettklist(data->plan) );
    removedIDs += _cleanupAutogenRouteids( wPlan.getswlist(data->plan) );
    removedIDs += _cleanupAutogenRouteids( wPlan.getsglist(data->plan) );
    removedIDs += _cleanupAutogenRouteids( wPlan.getfblist(data->plan) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Removed %d autogen-routeids in plan", removedIDs );
  }


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " ");
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "the analyzer found the routes:");

  int fbcount = 0;

  iOList routelist = (iOList)ListOp.first( data->preRTlist );
  while(routelist) {

    Boolean addToList = True;
    Boolean addRtId = True;

    /* create new route element */
    iONode newRoute = NodeOp.inst( wRoute.name(), NULL, ELEMENT_NODE );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "route:");

    /* bka is first item in a routelist */
    iONode item = (iONode)ListOp.first( routelist );
    bka = wItem.getid(item);
    bkaside = wItem.getstate(item);

    /* if bka is a stageblock start setting routeid after last feedback that belongs to the stageblock itself */
    if( StrOp.equals(NodeOp.getName(item), wStage.name() ) ) {
      /* number of FBs of bka is: every section must have 1 sensor FB and may have 1 optional occupancy FB */
      fbcount = countStageblockSectionFeedback( item );
      addRtId = False;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "__generateRoutes: sb [%s] has %d FBs", wItem.getid(item), fbcount);
    }

    /* bkb is last item in routelist */
    /* check if it is really some kind of block */
    item = (iONode)ListOp.get( routelist, ListOp.size( routelist ) - 1 );
    if( ( item != NULL ) &&
        ( ( StrOp.equals(NodeOp.getName(item), wBlock.name() ) ||
            StrOp.equals(NodeOp.getName(item), wStage.name() ) ||
            StrOp.equals(NodeOp.getName(item), wSelTab.name() ) ) ) ) {
      bkb = wItem.getid(item);
      bkbside = wItem.getstate(item);
    } else {
      /* this should never happen because already checked when creating preRTlist */
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Last item of a route starting at [%s%s] is NOT A BLOCK|STAGEBLOCK|SELTAB: [%s][%s]",
          bka, bkaside, wItem.getid( item), NodeOp.getName(item) );
      addToList = False;
      addRtId = False;
    }

    char* autogenID = StrOp.fmt( "autogen-[%s%s]-[%s%s]", bka, bkaside, bkb, bkbside );
    wRoute.setid( newRoute, autogenID );
    StrOp.free(autogenID);
    wRoute.setbka( newRoute, bka);
    wRoute.setbkb( newRoute, bkb);
    wRoute.setbkaside( newRoute, StrOp.equals( bkaside, "+" )?True:False );
    wRoute.setbkbside( newRoute, StrOp.equals( bkbside, "+" )?True:False );

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "__generateRoutes: bka=%s bkaside=%s bkb=%s bkbside=%s ", bka, bkaside, bkb, bkbside);

    if( isStageBlockById( data->model, bka ) && StrOp.equals( bkaside, "+" ) ) {
      /* skip routes starting at enter side of staging block */
      /* this should never happen because already checked when creating preRTlist */
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SHOULD NEVER HAPPEN __generateRoutes Route Source bka=%s bkaside %s skipped", bka, bkaside );
      addToList = False;
    }
    if( isStageBlockById( data->model, bkb ) && StrOp.equals( bkbside, "-" ) ) {
      /* skip routes ending at exit side of staging block */
      /* this should never happen because already checked when creating preRTlist */
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SHOULD NEVER HAPPEN __generateRoutes Route Destination bkb %s bkbside %s skipped", bkb, bkbside );
      addToList = False;
    }

    for( i = 0; i < NodeOp.getChildCnt( stlist); i++) {
      child = NodeOp.getChild( stlist, i);

      if( StrOp.equals( wRoute.getbka( child), wRoute.getbka( newRoute)) &&
          StrOp.equals( wRoute.getbkb( child), wRoute.getbkb( newRoute)) &&
              wRoute.isbkaside( child) ==  wRoute.isbkaside( newRoute) &&
              wRoute.isbkbside( child) ==  wRoute.isbkbside( newRoute) ) {

        if( !StrOp.equals( wRoute.getid( child), wRoute.getid( newRoute)) ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "found an edited route: [%s] from [%s] to [%s] skip", 
              wItem.getid( child), wRoute.getbka( child), wRoute.getbkb( child));
          addToList = False;
          break;
        } else {
          /* second route between two identical blocks found, make unique ID */
          char* extID = StrOp.fmt( "%s-%d", wRoute.getid( newRoute ), i );
          wRoute.setid( newRoute, extID );
          StrOp.free( extID );
        }
      }
    }

    /* second loop over all items of a possible route */
    Boolean reachedEndblock = False;
    item = (iONode)ListOp.first( routelist );
    while(item) {

      /* check if generator was correct... */
      if( reachedEndblock ) {
        /* another item after reached end block */
        /* this should never happen because already checked when creating preRTlist */
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SHOULD NEVER HAPPEN __generateRoutes: item after reachedEndblock newRoute=%s item=%s", wRoute.getid( newRoute ), wItem.getid(item) );
      }

      const char* itemori = wItem.getori(item);
      if( itemori == NULL) {
        itemori = wItem.west;
      }

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " [%s][%s][%s]", NodeOp.getName(item), wItem.getid(item), wItem.getstate(item) );

      if( StrOp.equals( NodeOp.getName(item), wSelTab.name()) ) {
        iONode swcmd = NodeOp.inst( wSwitchCmd.name(), NULL, ELEMENT_NODE );
        wItem.setid( swcmd, wItem.getid(item));
        wSwitch.setcmd( swcmd, wSwitchCmd.cmd_track);
        NodeOp.addChild( newRoute, swcmd );
      }

      if( ( StrOp.equals( NodeOp.getName(item), wBlock.name() ) || 
            StrOp.equals( NodeOp.getName(item), wStage.name() ) || 
            StrOp.equals( NodeOp.getName(item), wSelTab.name() ) 
          ) && 
          StrOp.equals(wItem.getid(item), bkb) ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "REACHED ENDBLOCK");
        reachedEndblock = True;
      }

      if( StrOp.equals( NodeOp.getName(item), wSwitch.name()) &&
          ! StrOp.equals( wItem.gettype(item), wSwitch.decoupler ) &&
          ! StrOp.equals( wItem.gettype(item), wSwitch.accessory )
        ) {
        iONode swcmd = NodeOp.inst( wSwitchCmd.name(), NULL, ELEMENT_NODE );
        wItem.setid( swcmd, wItem.getid(item));
        wSwitch.setcmd( swcmd, wItem.getstate(item));
        NodeOp.addChild( newRoute, swcmd );
      }

      if( StrOp.equals( NodeOp.getName(item), wTrack.name()) ||
          StrOp.equals( NodeOp.getName(item), wFeedback.name()) ||
          StrOp.equals( NodeOp.getName(item), wSignal.name()) ||
          isSingleTrackRRCrossing(item)
        ) {

        iONode tracknode = NULL;

        if( StrOp.equals( NodeOp.getName(item), wTrack.name()) ) {
          iOTrack track = ModelOp.getTrack( data->model, wItem.getid(item) );
          tracknode = TrackOp.base.properties(track);
        }

        if( StrOp.equals( NodeOp.getName(item), wFeedback.name()) ) {
          /* skip fb if we start at a stageblock */
          if( fbcount ) {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__generateRoutes: %s [fb] skip addRtId because it belongs to a stageblock. fbcount=%d", wItem.getid(item), fbcount);
            fbcount--;
            if( fbcount == 0 ) {
              /* we travelled through all FB of a SB, now we may set routeIDs*/
              addRtId = True;
            }
          }
          else {
            iOFBack track = ModelOp.getFBack( data->model, wItem.getid(item) );
            tracknode = FBackOp.base.properties(track);
          }
        }

        if( StrOp.equals( NodeOp.getName(item), wSignal.name()) ) {
          iOSignal track = ModelOp.getSignal( data->model, wItem.getid(item) );
          tracknode = SignalOp.base.properties(track);
        }

        if( isSingleTrackRRCrossing(item) ) {
          iOSwitch track = ModelOp.getSwitch( data->model, wItem.getid(item) );
          tracknode = SwitchOp.base.properties(track);
        }

        /* set routeids for tk|fb|sg */
        if( addToList && data->setRouteId ) {
          char* prevrouteids = StrOp.dup( wItem.getrouteids(tracknode) );
          if( prevrouteids != NULL ) {
            iOStrTok tok = StrTokOp.inst( prevrouteids, ',' );
            /* check if id is already in the list */
            Boolean isInList = False;
            while ( StrTokOp.hasMoreTokens( tok )) {
              const char* token = StrTokOp.nextToken( tok );
              if( StrOp.equals( token, wRoute.getid( newRoute))) {
                isInList = True;
              }
            }

            if( !isInList ) {
                if( StrOp.len(prevrouteids)>0 ) {
                  prevrouteids = StrOp.cat( (char*)prevrouteids, ",");
                }
                prevrouteids = StrOp.cat( (char*)prevrouteids, wRoute.getid( newRoute) );
                wItem.setrouteids(tracknode, prevrouteids );
                modifications++;
            }

            StrTokOp.base.del(tok);
          }
          else { /* empty attribute */
            wItem.setrouteids(tracknode, wRoute.getid( newRoute) );
            modifications++;
          }
          StrOp.free(prevrouteids);
        }
      } /* tk || fb || sg */

      item = (iONode)ListOp.next( routelist );
    }

    /* merge into stlist */
    if( addToList) {

      if ( !(StrOp.equals( wRoute.getbka(newRoute), wRoute.getbkb(newRoute))) ) {
        /* set some useful defaults... */
        wRoute.setshow( newRoute, False );
        wRoute.setx( newRoute, 0 );
        wRoute.sety( newRoute, 0 );
        /* ...then add to the list */
        NodeOp.addChild( stlist, newRoute );
        modifications++;
      } else {
        /* this should never happen because already checked when creating preRTlist */
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "found loop route: %s -> check your plan!", wRoute.getid(newRoute));
      }
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " ");
    routelist = (iOList)ListOp.next( data->preRTlist );
  }
  return modifications;
}

static const char* __readableOri( int ori) {

  if (ori == 0)
    return wItem.west;
  else  if (ori == 1)
    return wItem.north;
  else  if (ori == 2)
    return wItem.east;
  else  if (ori == 3)
    return wItem.south;

  return "";
}


static int _analyse(iOAnalyse inst) {
  if( inst == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "AnalyseOp.analyse() called without a valid instance" );
    return 0;
  }
  iOAnalyseData data = Data(inst);
  iONode block = NULL;
  int cx, cy;
  int zlevel = 0;
  int modifications = 0;
  Boolean res = True;
  Boolean resCT;

  /* do some extended tests on current layout */

  /* check if connectors and counterparts are ok */
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Connector test: in progress..." );
  resCT = connectorCheck( inst, False );
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Connector test: %s errors detected", resCT?"no":"some" );
  res &= resCT;

  if( ! res ) {
    /* errors in one of the tests above */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Analyser skipped because plan has ERRORs" );
    return modifications;
  }

  MapOp.clear(data->objectmap);
  ListOp.clear(data->preRTlist);
  ListOp.clear(data->bklist);
  ListOp.clear(data->notRTlist);

  iONode modplan = ModelOp.getModPlan( data->model );
  if( modplan == NULL) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "not a modplan" );

    int i;
    for( i = 0; i <= data->maxZlevel ; i++) {
      iOList list = ModelOp.getLevelItems( data->model, i, &cx, &cy, True);

      if( ListOp.size(list) > 0) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                "Trackplan: %d objects at level %d and sizes %d x %d", ListOp.size(list), i, cx, cy );
        __prepare(inst, list, 0,0);
      }
    }

  } 
  else {
    iONode mod = wModPlan.getmodule( modplan );
    while( mod != NULL ) {

      iOList list = ModelOp.getLevelItems( data->model, zlevel, &cx, &cy, True);

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
           "preparing module: %s", wModule.gettitle( mod) );
      __prepare(inst, list, wModule.getx(mod), wModule.gety(mod));

      zlevel++;
      mod = wModPlan.nextmodule( modplan, mod );
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " plan contains %d blocks", ListOp.size(data->bklist) );

  block = (iONode)ListOp.first(data->bklist);
  while(block) {

    const char* blockori = wItem.getori(block);

    if(  blockori == NULL) {
      blockori = wItem.west;
    }

    if( StrOp.equals( blockori, wItem.west ) || StrOp.equals( blockori, wItem.east ) ) {
      __analyseBlock(inst, block, wItem.west);
      __analyseBlock(inst, block, wItem.east);
    } else if( StrOp.equals( blockori, wItem.north ) || StrOp.equals( blockori, wItem.south ) ) {
      __analyseBlock(inst, block, wItem.north);
      __analyseBlock(inst, block, wItem.south);
    }

    block = (iONode)ListOp.next(data->bklist);
  }

  int chgRoutes = __generateRoutes(inst);
  modifications += chgRoutes;
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "generateRoutes:%6d modifications",  chgRoutes );

  int chgLists = __analyseAllLists(inst);
  modifications += chgLists;
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "analyseAllLists:%5d modifications", chgLists );

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__generateRoutes[%d] + __analyseAllLists[%d] = %d modifications",
      chgRoutes, chgLists, modifications );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Analyser finished with %d modifications", modifications );

  if( modifications > 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Please restart Rocrail server." );
    /* force disconnect/reconnect ? */
    /* AppOp.shutdown(); -> sometimes crashes */
    /* ==> need something like AppOp.reinit(); */
  }
  return modifications ;
}

static Boolean _checkPlanHealth(iOAnalyse inst) {
  if( inst == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "AnalyseOp.checkPlanHealth() called without a valid instance" );
    return False;
  }
  iOAnalyseData data = Data(inst);
  char key[64] = {'\0'};
  Boolean healthy = True;
  iOMap sensorMap = MapOp.inst();
  iOMap switchMap = MapOp.inst();
  int dbs = NodeOp.getChildCnt(data->plan);
  int i = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "checking plan health..." );

  if( !wCtrl.isuseblockside( wRocRail.getctrl( AppOp.getIni() ) ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: Block side routing is not enabled; The classic method is deprecated." );
  }

  /* checking ID's */
  for( i = 0; i < dbs; i++ ) {
    iOMap idMap = MapOp.inst();
    iONode db = NodeOp.getChild( data->plan, i );
    int items = NodeOp.getChildCnt(db);
    int n = 0;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "checking list [%s]...", NodeOp.getName(db) );
    for( n = 0; n < items; n++ ) {
      iONode item = NodeOp.getChild( db, n );

      if( StrOp.equals( wActionCtrl.name(), NodeOp.getName(item) ) ||
          StrOp.equals( wAction.name(), NodeOp.getName(item) ) ||
          StrOp.equals( wLocation.name(), NodeOp.getName(item) ))
      {
        /* Ignore */
        continue;
      }

      /* set maxZlevel */
      if( data->maxZlevel < wItem.getz(item) )
        data->maxZlevel = wItem.getz(item);

      /* check the basic addressing */
      if( StrOp.equals( wLoc.name(), NodeOp.getName(item) ) ) {
        if( wLoc.getaddr(item) == 0 && !StrOp.equals(wLoc.getprot(item), wLoc.prot_A) ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: loco %s has no address set", wItem.getid(item) );
          healthy = False;
        }
        if( ! isValidInterfaceID( inst, wLoc.getiid(item) ) ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: loco %s configured for non existent interface id [%s]",
              wItem.getid(item), wLoc.getiid(item) );
          healthy = False;
        }
      }

      if( StrOp.equals( wFeedback.name(), NodeOp.getName(item) ) ) {
        if( wFeedback.getaddr(item) == 0 ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: sensor %s has no address set", wItem.getid(item) );
          healthy = False;
        }
        else {
          char* key = FBackOp.createAddrKey( wFeedback.getbus(item), wFeedback.getaddr(item), wFeedback.getiid(item) );
          if( MapOp.haskey(sensorMap, key ) ) {
            iONode sensorItem = (iONode)MapOp.get( sensorMap, key );
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: sensor %s has an already used address %d by %s (%s)",
                wItem.getid(item), wFeedback.getaddr(item), wItem.getid(sensorItem), key );
            healthy = False;
          }
          else {
            MapOp.put( sensorMap, key, (obj)item );
          }
          StrOp.free( key );
        }
        if( ! isValidInterfaceID( inst, wFeedback.getiid(item) ) ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: sensor %s configured for non existent interface id [%s]",
              wItem.getid(item), wFeedback.getiid(item) );
          healthy = False;
        }
      }

      if( StrOp.equals( wSwitch.name(), NodeOp.getName(item) ) ) {
        if( wSwitch.getaddr1(item) == 0 && wSwitch.getport1(item) == 0 ) {
          if( StrOp.equals( wSwitch.gettype(item), wSwitch.crossing ) || StrOp.equals( wSwitch.gettype(item), wSwitch.ccrossing ) ) {
            /* crossing and centered crossing do not need an address */
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "INFO: crossing \"%s\" has no address -> cross", wItem.getid(item) );
          }
          else {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: switch %s has no address set", wItem.getid(item) );
          }
        }
        else {
          char key[32];
          StrOp.fmtb( key, "%d-%d-%d-%s", wSwitch.getaddr1(item), wSwitch.getport1(item), wSwitch.getgate1(item), wItem.getiid(item) );
          if( MapOp.haskey(switchMap, key ) ) {
            iONode switchItem = (iONode)MapOp.get( switchMap, key );
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: switch %s has an already used address %d-%d by %s (%s)",
                wItem.getid(item), wSwitch.getaddr1(item), wSwitch.getport1(item), wItem.getid(switchItem), key );
          }
          else {
            MapOp.put( switchMap, key, (obj)item );
          }

          if( ( StrOp.equals( wSwitch.gettype(item), wSwitch.dcrossing ) || StrOp.equals( wSwitch.gettype(item), wSwitch.threeway ) ) && ( wSwitch.getaddr2(item) > 0 || wSwitch.getport2(item) > 0 ) ) {
            StrOp.fmtb( key, "%d-%d-%d-%s", wSwitch.getaddr2(item), wSwitch.getport2(item), wSwitch.getgate2(item), wItem.getiid(item) );
            if( MapOp.haskey(switchMap, key ) ) {
              iONode switchItem = (iONode)MapOp.get( switchMap, key );
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: switch %s has an already used second address %d-%d by %s (%s)",
                  wItem.getid(item), wSwitch.getaddr2(item), wSwitch.getport2(item), wItem.getid(switchItem), key );
            }
            else {
              MapOp.put( switchMap, key, (obj)item );
            }
          }
        }
        if( ! isValidInterfaceID( inst, wSwitch.getiid(item) ) ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: switch %s configured for non existent interface id [%s]",
              wItem.getid(item), wSwitch.getiid(item) );
          healthy = False;
        }
      }

      if( StrOp.equals( wOutput.name(), NodeOp.getName(item) ) ) {
        if( wOutput.getaddr(item) > 0 || wOutput.getport(item) > 0 ) {
          char key[32];
          StrOp.fmtb( key, "%d-%d-%s", wOutput.getaddr(item), wOutput.getport(item), wItem.getiid(item) );
          if( MapOp.haskey(switchMap, key ) ) {
            iONode switchItem = (iONode)MapOp.get( switchMap, key );
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: output %s has an already used address %d-%d by %s (%s)",
                wItem.getid(item), wOutput.getaddr(item), wOutput.getport(item), wItem.getid(switchItem), key );
          }
          else {
            MapOp.put( switchMap, key, (obj)item );
          }
        }
        if( ! isValidInterfaceID( inst, wOutput.getiid(item) ) ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: output %s configured for non existent interface id [%s]",
              wItem.getid(item), wOutput.getiid(item) );
          healthy = False;
        }
      }

      if( StrOp.equals( wSignal.name(), NodeOp.getName(item) ) ) {
        if( wSignal.getaddr(item) == 0 && wSignal.getport1(item) == 0 ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: signal %s has no address set", wItem.getid(item) );
          healthy = False;
        }
        else {
          char key[32];
          StrOp.fmtb( key, "%d-%d-%d-%s", wSignal.getaddr(item), wSignal.getport1(item), wSignal.getgate1(item), wItem.getiid(item) );
          if( MapOp.haskey(switchMap, key ) ) {
            iONode switchItem = (iONode)MapOp.get( switchMap, key );
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: signal %s has an already used first address %s by %s (%s)",
                wItem.getid(item), key, wItem.getid(switchItem), key );
          }
          else {
            MapOp.put( switchMap, key, (obj)item );
          }
        }

        if( wSignal.getaddr2(item) > 0 || wSignal.getport2(item) > 0 ) {
          char key[32];
          StrOp.fmtb( key, "%d-%d-%d-%s", wSignal.getaddr2(item), wSignal.getport2(item), wSignal.getgate2(item), wItem.getiid(item) );
          if( MapOp.haskey(switchMap, key ) ) {
            iONode switchItem = (iONode)MapOp.get( switchMap, key );
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: signal %s has an already used second address %s by %s (%s)",
                wItem.getid(item), key, wItem.getid(switchItem), key );
          }
          else {
            MapOp.put( switchMap, key, (obj)item );
          }
        }

        if( wSignal.getaspects(item) >= 3 && ( wSignal.getaddr3(item) > 0 || wSignal.getport3(item) > 0 ) ) {
          char key[32];
          StrOp.fmtb( key, "%d-%d-%d-%s", wSignal.getaddr3(item), wSignal.getport3(item), wSignal.getgate3(item), wItem.getiid(item) );
          if( MapOp.haskey(switchMap, key ) ) {
            iONode switchItem = (iONode)MapOp.get( switchMap, key );
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: signal %s has an already used third address %s by %s (%s)",
                wItem.getid(item), key, wItem.getid(switchItem), key );
          }
          else {
            MapOp.put( switchMap, key, (obj)item );
          }
        }

        if( wSignal.getaspects(item) >= 4 && ( wSignal.getaddr4(item) > 0 || wSignal.getport4(item) > 0 ) ) {
          char key[32];
          StrOp.fmtb( key, "%d-%d-%d-%s", wSignal.getaddr4(item), wSignal.getport4(item), wSignal.getgate4(item), wItem.getiid(item) );
          if( MapOp.haskey(switchMap, key ) ) {
            iONode switchItem = (iONode)MapOp.get( switchMap, key );
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: signal %s has an already used fourth address %s by %s (%s)",
                wItem.getid(item), key, wItem.getid(switchItem), key );
          }
          else {
            MapOp.put( switchMap, key, (obj)item );
          }
        }
        if( ! isValidInterfaceID( inst, wSignal.getiid(item) ) ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: signal %s configured for non existent interface id [%s]",
              wItem.getid(item), wSignal.getiid(item) );
          healthy = False;
        }
      }



      StrOp.fmtb( key, "%d-%d-%d", wItem.getx(item), wItem.gety(item), wItem.getz(item) );

      if( MapOp.haskey(idMap, wItem.getid(item)) ) {
        iONode firstItem = (iONode)MapOp.get(idMap, wItem.getid(item));
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: object [%s] with id [%s] at [%d,%d,%d] already exist at [%d,%d,%d]",
            NodeOp.getName(item), wItem.getid(item),
            wItem.getx(item), wItem.gety(item), wItem.getz(item),
            wItem.getx(firstItem), wItem.gety(firstItem), wItem.getz(firstItem));
        healthy = False;
      }
      else {
        MapOp.put(idMap, wItem.getid(item), (obj)item );
      }

      /* check visible items for valid coordinates */
      if( wItem.isshow(item) ) {
        if( wItem.getx(item) != -1 && wItem.gety(item) != -1 ) {
          if( wItem.getx(item) < -1 || wItem.gety(item) < -1 ) {
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: object [%s] with id [%s] has invalid coordinates [%d,%d,%d]",
                NodeOp.getName(item), wItem.getid(item), wItem.getx(item), wItem.gety(item), wItem.getz(item));
            if( wItem.getx(item) < -1 )
              wItem.setx(item, 0);
            if( wItem.gety(item) < -1 )
              wItem.sety(item, 0);
            healthy = False;
          }
        }

        if( wItem.getx(item) > 256 || wItem.gety(item) > 256 ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: object [%s] with id [%s] has invalid coordinates [%d,%d,%d]",
              NodeOp.getName(item), wItem.getid(item), wItem.getx(item), wItem.gety(item), wItem.getz(item));
          if( wItem.getx(item) > 256 )
            wItem.setx(item, 0);
          if( wItem.gety(item) > 256 )
            wItem.sety(item, 0);
          healthy = False;
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "INFO: invisible object [%s] with id [%s] and coordinates [%d,%d,%d]",
            NodeOp.getName(item), wItem.getid(item), wItem.getx(item), wItem.gety(item), wItem.getz(item));
      }

    }
    MapOp.base.del(idMap);
  }

  /* check overlapping by using __prepare */
  int cx, cy;
  int zlevel = 0;
  MapOp.clear(data->objectmap);

  iONode modplan = ModelOp.getModPlan( data->model );
  if( modplan == NULL) {
    int i;
    for( i = 0; i <= data->maxZlevel ; i++) {
      iOList list = ModelOp.getLevelItems( data->model, i, &cx, &cy, True);
      if( ListOp.size(list) > 0) {
        if( ! __prepare(inst, list, 0,0) )
          healthy = False;
      }
    }
  } 
  else {
    iONode mod = wModPlan.getmodule( modplan );
    while( mod != NULL ) {
      iOList list = ModelOp.getLevelItems( data->model, zlevel, &cx, &cy, True);
      if( ! __prepare(inst, list, wModule.getx(mod), wModule.gety(mod)) )
        healthy = False;
      zlevel++;
      mod = wModPlan.nextmodule( modplan, mod );
    }
  }
  /* 
   * visible routes/streets are not tested within __prepare, 
   * so check them here against the objectmap created by __prepare
   */
  iONode stList = wPlan.getstlist(data->plan);
  int stListSize = 0;
  if( stList != NULL ) {
    stListSize = NodeOp.getChildCnt( stList );

    if( stListSize > 0 ) {
      iONode node;
      const char* stListType = NodeOp.getName( NodeOp.getChild(stList, 0));
      int i = 0;
      Boolean thisNodeChanged ;

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_checkPlanHealth: Checking %d %s nodes", stListSize, stListType );
      for( i = 0 ; i < stListSize ; i++ ) {
        node = NodeOp.getChild(stList, i);
        if( node && wItem.isshow(node) && ( wItem.getx(node) > -1 ) && ( wItem.gety(node) > -1 ) ) {
          __createKey( key, node, 0, 0, 0);
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_checkPlanHealth: Checking node %s", wItem.getid(node) );
          if( MapOp.haskey( data->objectmap, key) ) {
            healthy = False;
            __notifyOverlapError( node, data->objectmap, key );
          }
          else
            MapOp.put( data->objectmap, key, (obj)node);
        }
      }
    }
  }


  /* check zlevels and all items on zlevels */
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Starting zlevel check" );
  if( ! zlevelCheck( inst, False ) ) {
    healthy = False;
  }

  /* check for very lonely objects */
  if( MapOp.size(data->objectmap) > 0 ) {
    int items = MapOp.size(data->objectmap);
    int maxDist = 0;
    iONode lonelyItem = NULL;
    iONode item = (iONode)MapOp.first(data->objectmap);
    while( item != NULL ) {
      if( maxDist < wItem.getx(item) + wItem.gety(item) ) {
        /* use x + y coordinates as approach to sqrt( x^2 + y^2 ) */
        maxDist = wItem.getx(item) + wItem.gety(item) ;
        lonelyItem = item;
      }
      item = (iONode)MapOp.next(data->objectmap);
    }

    if( lonelyItem != NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "INFO: object [%s] with id [%s] at [%d,%d,%d] is the most far away object in the plan",
          NodeOp.getName(lonelyItem), wItem.getid(lonelyItem), wItem.getx(lonelyItem), wItem.gety(lonelyItem), wItem.getz(lonelyItem));

      /* set maxConnectorDistance */
      if( wItem.getx(lonelyItem) >= wItem.gety(lonelyItem) )
        data->maxConnectorDistance = wItem.getx(lonelyItem) ;
      else
        data->maxConnectorDistance = wItem.gety(lonelyItem) ;
      if( data->maxConnectorDistance < AnalyseOp.MINIMAL_MAX_CONNECTOR_DISTANCE )
        data->maxConnectorDistance = AnalyseOp.MINIMAL_MAX_CONNECTOR_DISTANCE;

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "INFO: analyzer: max connector distance is %d (def/min=%d)", data->maxConnectorDistance, AnalyseOp.MINIMAL_MAX_CONNECTOR_DISTANCE );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "INFO: analyzer: max z-level is %d", data->maxZlevel );
    }
  }
  else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "objectmap is empty" );
  }

  MapOp.clear(data->objectmap);

  MapOp.base.del(sensorMap);
  MapOp.base.del(switchMap);


  if( healthy ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Plan is healthy" );
  } else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Plan is NOT healthty. See ERROR lines above." );
  }

  return healthy;
}

/* check if all text elements have valid settings */
static Boolean textCheck( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode txlist = wPlan.gettxlist(data->plan);
  Boolean retVal = True;
  int numProblems = 0;

  if( txlist != NULL ) {
    int txSize = NodeOp.getChildCnt( txlist );
    if( txSize > 0 ) {
      int i = 0;
      iONode tx ;

      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "textCheck: Checking %d tx nodes", txSize );
      for( i = 0 ; i < txSize ; i++ ) {
        tx = NodeOp.getChild(txlist, i);
        if( tx ) {
          /*
          text id x y z cx cy pointsize 
          */
          const char* txId = wText.getid( tx ) ;
          const char* txText = wText.gettext( tx ) ;
          int txX = wText.getx( tx ) ;
          int txY = wText.gety( tx ) ;
          int txZ = wText.getz( tx ) ;
          int txCX = wText.getcx( tx ) ;
          int txCY = wText.getcy( tx ) ;
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "textCheck: text[%s][%s]: xyz[%d-%d-%d] cx[%d] cy[%d]",
              txId, txText, txX, txY, txZ, txCX, txCY );
          if( txCX <= 0 ||
              txCY <= 0 ||
              ! isValidZlevel( inst, txZ )
            ) {
            if( ! isValidZlevel( inst, txZ ) )
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: object [tx] with id[%s] text[%s] at [%d,%d,%d]: level[%d] is invalid",
                  txId, txText, txX, txY, txZ, txZ );
            if( txCX <= 0 )
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: object [tx] with id[%s] text[%s] at [%d,%d,%d]: width cx[%d] is invalid -> element invisible",
                  txId, txText, txX, txY, txZ, txCX );
            if( txCY <= 0 )
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: object [tx] with id[%s] text[%s] at [%d,%d,%d]: height cy[%d] is invalid -> element is invisible",
                  txId, txText, txX, txY, txZ, txCY );
            numProblems++;
            retVal = False;
          
            if( repair ) {
              /* remove text */
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "DELETING: object [tx] with id[%s] text[%s] at [%d,%d,%d]",
                  txId, txText, txX, txY, txZ );
              /* -- -- */
              NodeOp.removeChild( txlist, tx ) ;
              txSize--;
              i--;
            }
          }
        } else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "text check: tx[%d] not found", i );
        }
      }
    }
  }

  if( numProblems ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "text check: %d problematic entries", numProblems );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "text check: %d problematic enries", numProblems );
  return retVal;
}


/* check if every connector with a tknr of 10 and above has a counterpart */
static Boolean connectorCheck( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode tklist = wPlan.gettklist(data->plan);
  Boolean retVal = True;

  if( tklist != NULL ) {
    int i ;
    int cnt = NodeOp.getChildCnt( tklist );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "connectorCheck: #of %s=[%d]", wTrack.name(), cnt );
    for( i = 0; i < cnt; i++ ) {
      iONode tk = NodeOp.getChild( tklist, i );
      const char* tkid = wItem.getid( tk );
      const char* type = wTrack.gettype( tk );
      if( tk && 
          StrOp.equals(NodeOp.getName(tk), wTrack.name() ) &&
          ( StrOp.equals(wItem.gettype(tk), wTrack.connector ) ||
            StrOp.equals(wItem.gettype(tk), wTrack.concurveleft ) ||
            StrOp.equals(wItem.gettype(tk), wTrack.concurveright )
          )
        ) {
        int tknr = wTrack.gettknr(tk);

        if( tknr >= MIN_CONNECTOR_COUNTERPART_NR ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "connectorCheck: tknr[%d] [%s]",
              wTrack.gettknr(tk), wItem.getid(tk) );
          int j ;
          int numConnectors = 0 ;
          for( j = 0; j < cnt; j++ ) {
            iONode tkJ = NodeOp.getChild( tklist, j );
            if( tkJ &&
                StrOp.equals(NodeOp.getName(tk), wTrack.name() ) &&
                ( StrOp.equals(wItem.gettype(tk),  wTrack.connector )  ||
                  StrOp.equals(wItem.gettype(tk), wTrack.concurveleft ) ||
                  StrOp.equals(wItem.gettype(tk), wTrack.concurveright )
                ) &&
                ( wTrack.gettknr(tkJ) == tknr )
              ) {
              numConnectors++ ;
            }
          }
          if( numConnectors == 0 ) {
            /* should never happen !!! */
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "connectorCheck: tknr[%d] [%s] : NONE found.",
                wTrack.gettknr(tk), wItem.getid(tk) );
          } else if( numConnectors == 1 ) {
            /* only one... */
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: connector [%s] with tknr[%d] : no counterpart found.",
                tkid, wTrack.gettknr(tk) );
          } else if( numConnectors == 2 ) {
            /* everything OK */
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "connectorCheck: tknr[%d] [%s] : 2 found.",
                wTrack.gettknr(tk), wItem.getid(tk) );
          } else if( numConnectors > 2 ) {
            /* Ooops, too many connectors */
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: connector [%s] tknr[%d] : Too many counterparts (total %d)",
                tkid, wTrack.gettknr(tk), numConnectors );
            retVal = False;
          }
        }
      }
    }
  }
  return retVal;
}


/* check if all routes have usage "from-to" and direction "forward" (-> are compatble to block sides) 
   and if all swcmd use a valid id for sw/sg/co */
static Boolean routeCheck( iOAnalyse inst, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode stlist = wPlan.getstlist(data->plan);
  Boolean retVal = True;
  int numProblems = 0;

  if( stlist != NULL ) {
    int stSize = NodeOp.getChildCnt( stlist );
    if( stSize > 0 ) {
      int i = 0;
      iONode stNode ;

      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "routeCheck: Checking %d stRoutes", stSize );
      for( i = 0 ; i < stSize ; i++ ) {
        stNode = NodeOp.getChild(stlist, i);
        if( stNode ) {
          const char* bka = wRoute.getbka( stNode );
          const char* bkb = wRoute.getbkb( stNode );
          const char* bkc = wRoute.getbkc( stNode );
          Boolean dir = wRoute.isdir(stNode);
          Boolean lcdir = wRoute.islcdir(stNode);

          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "routeCheck: st[%d] [%s] bka[%s] bkb[%s] bkc[%s] dir[%d] lcdir[%d]",
              i, wRoute.getid(stNode), bka, bkb, bkc, dir, lcdir );

          iOList delList = ListOp.inst();
          iONode swCmd = wRoute.getswcmd( stNode );
          while( swCmd != NULL ) {
            const char* swid = wSwitchCmd.getid( swCmd );
            const char* swcmd = wSwitchCmd.getcmd( swCmd );
            Boolean swlock = wSwitchCmd.islock( swCmd );
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "routeCheck: st[%d] [%s] swid[%s] swcmd[%s] swlock[%d]",
                i, wRoute.getid(stNode), swid, swcmd, swlock );
            /* validate swid */
            iOSwitch sw = ModelOp.getSwitch( data->model, swid );
            iOSignal sg = ModelOp.getSignal( data->model, swid );
            iOOutput co = ModelOp.getOutput( data->model, swid );
            iOSelTab st = ModelOp.getSelectiontable( data->model, swid );
            iOTT     tt = ModelOp.getTurntable( data->model, swid );
            if( sw == NULL && sg == NULL && co == NULL && st == NULL && tt == NULL ) {
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: routeCheck: route[%d] [%s] id[%s] in command does not exist",
                  i, wRoute.getid(stNode), swid );
              numProblems++;
              retVal = False;
              if( repair && ! ismemberoflist( delList, (obj)swCmd ) ) {
                ListOp.add( delList, (obj)swCmd );
              }
            }

            swCmd = wRoute.nextswcmd( stNode, swCmd );
          }

          if( repair ) {
            /* remove all marked swcmd */
            iONode swcmd ;
            if( ListOp.size(delList) > 0 ) {
              swcmd = (iONode)ListOp.first( delList );
              while( swcmd != NULL ) {
                TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "route check: route[%s]: Deleting swcmd id[%s] cmd[%s] lock[%d]", 
                    wRoute.getid(stNode), wSwitchCmd.getid( swcmd ), wSwitchCmd.getcmd( swcmd ), wSwitchCmd.islock( swcmd ) );
                NodeOp.removeChild( stNode, swcmd );
                NodeOp.base.del(swcmd);
                swcmd = (iONode)ListOp.next( delList );
              }
            }
          }
          ListOp.base.del(delList);

          if( ! repair ) {
            /* checks that don't have a repair part are skipped in repair mode */

            iIBlockBase blockA = ModelOp.getBlock( data->model, bka );
            iIBlockBase blockB = ModelOp.getBlock( data->model, bkb );

            /* check starting block */
            if( blockA == NULL ) {
              if( StrOp.len( bka ) == 0 ) {
                TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: routeCheck: [%s] start block not defined",
                    wRoute.getid(stNode) );
              }else {
                TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: routeCheck: [%s] start block [%s] not found in plan",
                    wRoute.getid(stNode), bka );
              }
              numProblems++;
            }

            /* check destination block */
            if( blockB == NULL ) {
              if( StrOp.len( bkb ) == 0 ) {
                TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: routeCheck: [%s] destination block not defined",
                    wRoute.getid(stNode) );
              }else {
                TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: routeCheck: [%s] destination block [%s] not found in plan",
                    wRoute.getid(stNode), bkb );
              }
              numProblems++;
            }

            /* check crossing blocks */
            if( bkc != NULL && StrOp.len( bkc ) > 0 ) {
              iOStrTok tok = StrTokOp.inst( bkc, ',' );
              while( StrTokOp.hasMoreTokens(tok) ) {
                const char* bk = StrTokOp.nextToken( tok );
                iIBlockBase blockC = ModelOp.getBlock( data->model, bk );
                if( blockC != NULL ) {
                    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "routeCheck: [%s] crossing block [%s]",
                        wRoute.getid(stNode), bk );
                }else {
                  TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: routeCheck: [%s] crossing blocks[%s] invalid/non existant block [%s]",
                      wRoute.getid(stNode), bkc, bk);
                  numProblems++;
                }
              }
              StrTokOp.base.del(tok);
            }

            /* "dir" is "from-to" or "both directions" */
            if( ! dir ) {
              /* routes with a turntable block as destination may use "both directions" in block side mode! */
              /* bkb is the destination block */
              /* find destination blocks in model/plan */
              /* check if it is a turntable block */
              Boolean isTTBlock = False;
              if( blockB != NULL ) {
                isTTBlock = BlockOp.isTTBlock( blockB );
              }

              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "route check: route [%s]: bkb[%s] isTTBlock[%d]",
                wRoute.getid(stNode), bkb, isTTBlock );

              if( ! isTTBlock ) {
                retVal = False;
                TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: route [%s] uses incompatible Usage: both directions",
                  wRoute.getid(stNode) );
                numProblems++;
              }
            }
            if( ! lcdir ) {
              retVal = False;
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: route [%s] uses incompatible Run direction: reverse",
                wRoute.getid(stNode) );
              numProblems++;
            }
          }

        } else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "route check: stNode[%d] not found", i );
        }
      }
    }
  }

  if( numProblems ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "route check: %d problematic entries", numProblems );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "route check: %d problematic enries", numProblems );
  return retVal;
}

/* is the given iid a valid iid of a command station */
static Boolean isValidInterfaceID( iOAnalyse inst, const char *iid )
{
  /* empty interface id is always OK -> first CS */
  if( iid == NULL || StrOp.len( iid ) == 0 )
    return True;

  iOAnalyseData data = Data(inst);
  iONode ini    = AppOp.getIni();
  iONode plan   = ModelOp.getModel( data->model );
  iONode modeldigint = plan?wPlan.getdigint( plan ):NULL;
  iONode digint = wRocRail.getdigint( ini );
  Boolean bModelDigints = modeldigint == NULL ? False:True;

  while( digint != NULL ) {
    const char* digintIid = wDigInt.getiid( digint );
    /*
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "digint iid=\"%s\" bModelDigints[%d]",
        digintIid!=NULL ? digintIid:"?", bModelDigints );
    */
    if( StrOp.equals( digintIid, iid ) ) {
      return True;
    }

    if( bModelDigints )
      digint = wPlan.nextdigint( ModelOp.getModel( data->model ), digint );
    else
      digint = wRocRail.nextdigint( ini, digint );
  }
  return False;
}


/* check for problems (do not change/repair/clean anything) */
static Boolean _checkExtended(iOAnalyse inst) {
  if( inst == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "AnalyseOp.checkExtended() called without a valid instance" );
    return False;
  }
  iOAnalyseData data = Data(inst);
  int modifications = 0;
  Boolean res;

  TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Extended checks are work in progress. Do not rely on them. BEGIN" );
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "ExtChk: basic[%d]c[%d] block[%d]c[%d] route[%d]c[%d] action[%d]c[%d]",
      data->basicCheck,  data->basicClean,
      data->blockCheck,  data->blockClean,
      data->routeCheck,  data->routeClean,
      data->actionCheck, data->actionClean );

  /* checks that don't change anything are always allowed */

  /* BASIC ELEMENT CHECKS */
  if( data->basicCheck ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Basic checks starting..." );

    /* check zlevels and all items on zlevels */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " zlevel test: in progress..." );
    res = zlevelCheck( inst, False );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " zlevel test: %s problems detected", res?"no":"some" );

    /* check if all text elements have valid settings */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Text test: in progress..." );
    res = textCheck( inst, False );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Text test: %s problems detected", res?"no":"some" );

    /* check if every connector with a tknr of 10 and above has a counterpart */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Connector test: in progress..." );
    res = connectorCheck( inst, False );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Connector test: %s problems detected", res?"no":"some" );

    /* check if every track element uses only valid blockid */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid blockid test: in progress..." );
    modifications  = 0;
    modifications += invalidBlockidCheck( inst, wPlan.gettklist(data->plan), False );
    modifications += invalidBlockidCheck( inst, wPlan.getswlist(data->plan), False );
    modifications += invalidBlockidCheck( inst, wPlan.getsglist(data->plan), False );
    modifications += invalidBlockidCheck( inst, wPlan.getfblist(data->plan), False );
    if( modifications > 0 )
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid blockid test: %d invalid entries detected", modifications );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid blockid test: %s problems detected", (modifications == 0)?"no":"some" );

    /* check if every track element uses only valid routeids */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid routeid test: in progress..." );
    modifications  = 0;
    modifications += invalidRouteidsCheck( inst, wPlan.gettklist(data->plan), False );
    modifications += invalidRouteidsCheck( inst, wPlan.getswlist(data->plan), False );
    modifications += invalidRouteidsCheck( inst, wPlan.getsglist(data->plan), False );
    modifications += invalidRouteidsCheck( inst, wPlan.getfblist(data->plan), False );
    if( modifications > 0 )
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid routeid test: %d invalid entries detected", modifications );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid routeid test: %s problems detected", (modifications == 0)?"no":"some" );

    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Basic checks finished" );
  }


  /* BLOCK CHECKS */
  if( data->blockCheck ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Block checks starting..." );

    /* do all fbevents of blocks have a valid fb id and valid from/byroute */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Block test: in progress..." );
    res = blockCheck( inst, False );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Block test: %s problems detected", res?"no":"some" );

    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Block feedback/action check: in progress..." );
    res = blockFeedbackActionCheck( inst, False );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Block feedback/action check: %s problems detected", res?"no":"some" );

    /* check for fbevent entries of deleted routes */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Block route/feedback validation: in progress..." );
    res = blockRouteFbValidation( inst, False );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Block route/feedback validation: %s problems detected", res?"no":"some" );

    /* check for fbevent entries of deleted routes */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Selection table route/feedback validation: in progress..." );
    res = seltabRouteFbValidation( inst, False );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Selection table route/feedback validation: %s problems detected", res?"no":"some" );

    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Block checks finished" );
  }

  /* ROUTE CHECKS */
  if( data->routeCheck ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Route checks starting..." );
    /* check if all routes have usage "from-to" and direction "forward" (-> are compatble to block sides) 
       and if all swcmd use a valid id for sw/sg/co */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Route test: in progress..." );
    res = routeCheck( inst, False );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Route test: %s problems detected", res?"no":"some" );

    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Route checks finished" );
  }


  if( data->actionCheck ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Action checks starting..." ); 
    /* check actions in all elements that may use actions */
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Action test: in progress..." );
    modifications  = 0;
    modifications += checkAcList( inst, False );
    modifications += checkSwAction( inst, False );
    modifications += checkSgAction( inst, False );
    modifications += checkCoAction( inst, False );
    modifications += checkFbAction( inst, False );
    modifications += checkBkAction( inst, False );
    modifications += checkSbAction( inst, False );
    modifications += checkTtAction( inst, False );
    modifications += checkStAction( inst, False );
    modifications += checkScAction( inst, False );
    modifications += checkLcAction( inst, False );
    modifications += checkTxAction( inst, False );
    modifications += checkSyAction( inst, False );
    if( modifications > 0 )
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Action test: %d invalid entries detected", modifications );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Action test: %s problems detected", (modifications == 0)?"no":"some" );

    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Action checks finished" );
  }

  TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Extended checks are work in progress. Do not rely on them. END" );
  return False;
}


/* clean problems according to variables */
static Boolean _cleanExtended(iOAnalyse inst) {
  if( inst == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "AnalyseOp.cleanExtended() called without a valid instance" );
    return False;
  }
  iOAnalyseData data = Data(inst);
  iONode aoIni = AppOp.getIni() ;
  iONode anaOpt = wRocRail.getanaopt( aoIni ) ;
  Boolean requirements = True; /* modfying plan allowed ? */
  Boolean planChanged = False;
  Boolean res;
  int modifications = 0;
  Boolean automode  = ModelOp.isAuto(data->model);
  Boolean isPowerOn = wState.ispower(ControlOp.getState(AppOp.getControl()));
  iONode modplan    = ModelOp.getModPlan( data->model );

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "ExtChk: basic[%d]c[%d] block[%d]c[%d] route[%d]c[%d] action[%d]c[%d]",
      data->basicCheck,  data->basicClean,
      data->blockCheck,  data->blockClean,
      data->routeCheck,  data->routeClean,
      data->actionCheck, data->actionClean );

  /* clean/repair are only allowed if power _and_ auto mode are off */
  /* requirements should be checked by calling function, but to be sure... */
  if( automode || isPowerOn ) {
    requirements = False;
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Cleaning/repairing skipped because...");
    /* explain why */
    if( automode )
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "... automode is on. Switch off to use cleaning/repairing.");
    if( isPowerOn )
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "... track power is on. Switch off to use cleaning/repairing.");
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Extended checks are work in progress. Do not rely on them. BEGIN" );


    if( data->basicClean ) {
      /* clean/repair are "once" options, reset option */
      wAnaOpt.setbasicClean( anaOpt, False ) ;
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Basic cleanup starting..." );

      /* check zlevels and all items on zlevels */
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " zlevel: Clean/repair in progress..." );
      res = zlevelCheck( inst, True );
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " zlevel: %s problems cleaned", res?"no":"some" );
      if( res == False )
        planChanged = True;

      /* delete all text elements with invalid settings */
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Text clean: in progress..." );
      res = textCheck( inst, True );
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Text clean: %s items deleted", res?"no":"some" );
      if( res == False )
        planChanged = True;

      /* clean invalid blockid/routeids */
      /* ...only if we do NOT have a modular layout (not yet supported) */
      if( modplan != NULL ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid blockid/routeids clean: skipped for modular layouts" );
      }
      else {
        /* no module plan */

        /* clean invalid blockids */
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid blockid clean: in progress..." );
        modifications  = 0;
        modifications += invalidBlockidCheck( inst, wPlan.gettklist(data->plan), True );
        modifications += invalidBlockidCheck( inst, wPlan.getswlist(data->plan), True );
        modifications += invalidBlockidCheck( inst, wPlan.getsglist(data->plan), True );
        modifications += invalidBlockidCheck( inst, wPlan.getfblist(data->plan), True );
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid blockid clean: %d invalid entries cleaned", modifications );
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid blockid clean: %s entries modified", (modifications == 0)?"no":"some" );

        /* clean invalid routeids */
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid routeid clean: in progress..." );
        modifications  = 0;
        modifications += invalidRouteidsCheck( inst, wPlan.gettklist(data->plan), True );
        modifications += invalidRouteidsCheck( inst, wPlan.getswlist(data->plan), True );
        modifications += invalidRouteidsCheck( inst, wPlan.getsglist(data->plan), True );
        modifications += invalidRouteidsCheck( inst, wPlan.getfblist(data->plan), True );

        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid routeid clean: %d invalid entries cleaned", modifications );
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Invalid routeid clean: %s entries modified", (modifications == 0)?"no":"some" );

        if( modifications > 0 )
          planChanged = True;
      }

      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Basic cleanup finished" );
    }


    if( data->blockClean ) {
      /* clean/repair are "once" options, reset option */
      wAnaOpt.setblockClean( anaOpt, False ) ;
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Block cleanup starting..." );

      /* check blocks */
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Block clean/repair in progress..." );
      res = blockCheck( inst, True );
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Block %s problems cleaned", res?"no":"some" );
      if( res == False )
        planChanged = True;

      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Block feedback action check: Clean/repair in progress..." );
      res = blockFeedbackActionCheck( inst, True );
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Block feedback action check: %s Problems cleaned", res?"no":"some" );
      if( res == False )
        planChanged = True;

      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Block route feedback validation: Clean/repair in progress..." );
      res = blockRouteFbValidation( inst, True );
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Block route feedback validation: %s problems cleaned", res?"no":"some" );
      if( res == False )
        planChanged = True;

      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Selection table route feedback validation: Clean/repair in progress..." );
      res = seltabRouteFbValidation( inst, True );
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Selection table route feedback validation: %s problems cleaned", res?"no":"some" );
      if( res == False )
        planChanged = True;

      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Block cleanup finished" );
    }

    if( data->routeClean ) {
      /* clean/repair are "once" options, reset option */
      wAnaOpt.setrouteClean( anaOpt, False ) ;
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Route cleanup starting..." );

      /* clean swcmd in routes where switch is is invalid */
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Route clean/repair in progress..." );
      res = routeCheck( inst, True );
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Route %s problems cleaned", res?"no":"some" );
      if( res == False )
        planChanged = True;

      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Route cleanup finished" );
    }

    if( data->actionClean ) {
      /* clean/repair are "once" options, reset option */
      wAnaOpt.setactionClean( anaOpt, False ) ;
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Action cleanup not yet implemented/available" );
    }

    if( planChanged )
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Please restart Rocrail server." );

    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Extended checks are work in progress. Do not rely on them. END" );
  }

  return False;
}

static int _cleanupRoutes(iOAnalyse inst) {
  iOAnalyseData data = Data(inst);
  iONode stlist = wPlan.getstlist(data->plan);
  int stSizeBefore = 0;
  int stSizeAfter  = 0;
  int modifications = 0 ;

  if( stlist != NULL ) {
    /* cleanup stlist (remove only "autogen-"-entries) */
    stSizeBefore = NodeOp.getChildCnt( stlist );
    if( stSizeBefore > 0 ) {
      int i = 0;
      iONode stNode ;

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupRoutes: Checking %d stRoutes", stSizeBefore );
      for( i = stSizeBefore - 1 ; i >= 0 ; i-- ) {
        stNode = NodeOp.getChild(stlist, i);
        if( stNode ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupRoutes: stNode[%d] = %s : %s", i, NodeOp.getName( stNode ), wRoute.getid( stNode) ) ;
          if( StrOp.startsWith( wRoute.getid(stNode), "autogen-" ) && ! wItem.isgenerated(stNode) ) {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupRoutes: remove stRoute %s", wRoute.getid(stNode) );
            NodeOp.removeChild( stlist, stNode );
            modifications++ ;
          } else {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupRoutes: keep   stRoute %s", wRoute.getid(stNode) );
          }
        } else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stNode[%d] not found", i );
        }
      }
    }

    stSizeAfter = NodeOp.getChildCnt( stlist );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cleanup %s [%4d/%4d nodes]%5d modifications in%5d nodes (routes   with \"autogen-\")", 
        NodeOp.getName(stlist), stSizeBefore, stSizeAfter, stSizeBefore-stSizeAfter, modifications );
  }
  return( modifications );
}

/* check if "id" is member of stlist or fblist */
static Boolean isValidBlockOrFeedbackId( iONode bklist, iONode fblist, const char* id ) {
  if( id == NULL || StrOp.len( id ) == 0 ) {
    /* emtpy blockid is valid */
    return True;
  }
  if( ( bklist == NULL && fblist == NULL ) ) {
    /* no lists */
    return False;
  }

  int i;
  int childcnt = NodeOp.getChildCnt( bklist );
  for( i = 0 ; i < childcnt ; i++ ) {
    iONode child = NodeOp.getChild( bklist, i );

    if( StrOp.equals( wItem.getid( child ), id )) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isValidBlockOrFeedbackId: [%s] found block", id );
      return True;
    }
  }

  childcnt = NodeOp.getChildCnt( fblist );
  for( i = 0 ; i < childcnt ; i++ ) {
    iONode child = NodeOp.getChild( fblist, i );

    if( StrOp.equals( wItem.getid( child ), id )) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isValidBlockOrFeedbackId: [%s] found feedback", id );
      return True;
    }
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isValidBlockOrFeedbackId: [%s] NO match", id );
  return False;
}

/* check if "id" is member of stlist */
static Boolean isValidRoute( iONode stlist, const char* id ) {
  if( stlist == NULL || id == NULL || StrOp.len( id ) == 0 )
    return False;
  int childcnt = NodeOp.getChildCnt( stlist );
  int i;

  for( i = 0 ; i < childcnt ; i++ ) {
    iONode child = NodeOp.getChild( stlist, i );

    if( StrOp.equals( wItem.getid( child ), id )) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isValidRoute route [%s] found", id );
      return True;
    }
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "isValidRoute route [%s] NOT found", id );
  return False;
}

/* check if id is in idlist */
Boolean isInList( char *idlist, const char *id ) {
  if( idlist != NULL ) {
    iOStrTok tok = StrTokOp.inst( idlist, ',' );
    /* check if id is already in the list */
    while( StrTokOp.hasMoreTokens( tok )) {
      const char* token = StrTokOp.nextToken( tok );
      if( StrOp.equals( token, id ) ) {
        StrTokOp.base.del(tok);
        return True;
      }
    }
    StrTokOp.base.del(tok);
  }
  return False;
}


/* check for invalid blockids in tk|sw|sg|fb-list */
static int invalidBlockidCheck( iOAnalyse inst, iONode tracklist, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode bklist = wPlan.getbklist( data->plan );
  iONode fblist = wPlan.getfblist( data->plan );
  int modifications = 0;
  int numModifiedTracks = 0;
  int trackListSize = 0;
  int checkedTotal = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "invalidBlockidCheck: Checking [%08.8X]", tracklist );
  if( tracklist != NULL ) {
    trackListSize = NodeOp.getChildCnt( tracklist );
  }

  if( trackListSize > 0 ) {
    iONode tracknode;
    const char* listType = NodeOp.getName( NodeOp.getChild(tracklist, 0));
    int i = 0;
    Boolean thisTrackChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "invalidBlockidCheck: Checking %d %s nodes", trackListSize, listType );
    for( i = trackListSize - 1 ; i >= 0 ; i-- ) {
      tracknode = NodeOp.getChild(tracklist, i);
      if( tracknode ) {
        checkedTotal++;
        thisTrackChanged = False;
        char* blockid = StrOp.dup(wItem.getblockid(tracknode));
        if( ( blockid == NULL ) || ( StrOp.len( blockid ) == 0 ) ) {
          /* no blockid */
        } else {
          if( isValidBlockOrFeedbackId( bklist, fblist, blockid ) ) {
            /* OK */
          } else {
            modifications++;
            numModifiedTracks++;
            thisTrackChanged = True;
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "node[%s][%s] has invalid blockid[%s]",
                wItem.getid(tracknode), listType, blockid );
            if( repair ) {
              wItem.setblockid( tracknode, "" );
            }
          }
        }
      }
    }
    if( ( modifications > 0 ) || repair ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "  %s %slist [%5d/%5d] invalid blockids in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedTracks, trackListSize );
    } else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  %s %slist [%5d/%5d] invalid blockids in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedTracks, trackListSize );
    }
  }
  return modifications;
}


/* check for invalid routeids in tk|sw|sg|fb-list */
static int invalidRouteidsCheck( iOAnalyse inst, iONode tracklist, Boolean repair ) {
  iOAnalyseData data = Data(inst);
  iONode stlist = wPlan.getstlist( data->plan );
  int modifications = 0;
  int numModifiedTracks = 0;
  int trackListSize = 0;
  int checkedTotal = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "invalidRouteidsCheck: Checking [%08.8X]", tracklist );
  if( tracklist != NULL ) {
    trackListSize = NodeOp.getChildCnt( tracklist );
  }

  if( trackListSize > 0 ) {
    iONode tracknode;
    const char* listType = NodeOp.getName( NodeOp.getChild(tracklist, 0));
    int i = 0;
    Boolean thisTrackChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "invalidRouteidsCheck: Checking %d %s nodes", trackListSize, listType );
    for( i = trackListSize - 1 ; i >= 0 ; i-- ) {
      tracknode = NodeOp.getChild(tracklist, i);
      if( tracknode ) {
        thisTrackChanged = False;
        char* prevrouteids = StrOp.dup(wItem.getrouteids(tracknode));
        char* userrouteids = StrOp.dup("");

        if( StrOp.len(prevrouteids) > 0 ) {
          iOStrTok tok = StrTokOp.inst( prevrouteids, ',' );
          while( StrTokOp.hasMoreTokens( tok ) ) {
            const char* token = StrTokOp.nextToken( tok );
            if( StrOp.len(token) > 0 ) {
              checkedTotal++;
              /* check if it is valid */
              if( isValidRoute( stlist, token ) ) {
                if( isInList( userrouteids, token ) ) {
                  /* already in list -> duplicate entry -> skip */
                  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "invalidRouteidsCheck: [%s][%s] duplicate entry[%s] in routeids[%s]",
                      wItem.getid( tracknode ), listType, token, prevrouteids );
                  thisTrackChanged = True;
                  modifications++;
                } else {
                  /* valid route so append to new list */
                  if( StrOp.len(userrouteids) > 0 ) {
                    userrouteids = StrOp.cat( userrouteids, ",");
                  }
                  userrouteids = StrOp.cat( userrouteids, token );
                }
              }else {
                /* invalid routeid skipped */
                TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "invalidRouteidsCheck: [%s][%s] invalid entry[%s] in routeids[%s]",
                    wItem.getid( tracknode ), listType, token, prevrouteids );
                thisTrackChanged = True;
                modifications++;
              }
            }
          }
          StrTokOp.base.del(tok);

          if( repair && thisTrackChanged ) {
            /* only set routeIDs if they are really different */
            if( ! StrOp.equals( prevrouteids, userrouteids ) )
              wItem.setrouteids( tracknode, userrouteids );
          }
        }
        StrOp.free(userrouteids);
        StrOp.free(prevrouteids);
        if( thisTrackChanged ) {
          numModifiedTracks++;
        }
      }
    }
    if( ( modifications > 0 ) || repair ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "  %s %slist [%5d/%5d] invalid/duplicate routeids in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedTracks, trackListSize );
    } else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  %s %slist [%5d/%5d] invalid/duplicate routeids in [%4d/%4d] nodes", 
          repair?"cleaned":"checked", listType, modifications, checkedTotal, numModifiedTracks, trackListSize );
    }
  }
  return(modifications);
}

static Boolean isgeneratedRoute( const char* routeId ) {
  Boolean isGenerated = False;

  if( routeId == NULL || StrOp.len( routeId ) == 0 ) {
    return False;
  }else {
    iORoute route = ModelOp.getRoute( AppOp.getModel(), routeId );
    if( route != NULL ) {
      iONode stNode = RouteOp.base.properties( route );
      if( stNode != NULL ) {
        isGenerated = wItem.isgenerated( stNode );
      }
    }
  }
  return isGenerated ; 
}

/* remove autogen- routeids in tk|sw|sg|fb-list */
static int _cleanupAutogenRouteids( iONode tracklist ) {
  int modifications = 0;
  int numModifiedTracks = 0;
  int trackListSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupAutogenRouteids: Checking [%08.8X]", tracklist );
  if( tracklist != NULL ) {
    trackListSize = NodeOp.getChildCnt( tracklist );
  }

  if( trackListSize > 0 ) {
    iONode tracknode;
    const char* listType = NodeOp.getName( NodeOp.getChild(tracklist, 0));
    int i = 0;
    Boolean thisTrackChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupAutogenRouteids: Checking %d %s nodes", trackListSize, listType );
    for( i = trackListSize - 1 ; i >= 0 ; i-- ) {
      tracknode = NodeOp.getChild(tracklist, i);
      if( tracknode ) {
        thisTrackChanged = False;
        /* remove all "autogen-"-routeids */
        char* prevrouteids = StrOp.dup(wItem.getrouteids(tracknode));
        char* userrouteids = StrOp.dup("");

        if( StrOp.len(prevrouteids) > 0 ) {
          iOStrTok tok = StrTokOp.inst( prevrouteids, ',' );
          while ( StrTokOp.hasMoreTokens( tok )) {
            const char* token = StrTokOp.nextToken( tok );
            /* check if routeid does not start with autogen- except it is a generated route */
            if( ( StrOp.len(token) > 0 ) && ( isgeneratedRoute( token ) || ( ! StrOp.startsWith( token, "autogen-") ) ) ) {
              /* generated or not "autogen-" so append to new list*/
              if( StrOp.len(userrouteids)>0 ) {
                userrouteids = StrOp.cat( userrouteids, ",");
              }
              userrouteids = StrOp.cat( userrouteids, token );
            }else {
              thisTrackChanged = True;
              modifications++;
            }
          }
          StrTokOp.base.del(tok);
        }
        wItem.setrouteids(tracknode, userrouteids );
        StrOp.free(userrouteids);
        StrOp.free(prevrouteids);
        if( thisTrackChanged ) {
          numModifiedTracks++;
        }
      }
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cleanup %slist [%4d/%4d nodes]%5d modifications in%5d nodes (routeids with \"autogen-\")", 
        listType, trackListSize, NodeOp.getChildCnt( tracklist ), modifications, numModifiedTracks );
  }
  return(modifications);
}

static int _resetBlockids( iONode tracklist ) {
  int modifications = 0;
  int numModifiedTracks = 0;
  int trackListSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_resetBlockids: Checking [%08.8X]", tracklist );
  if( tracklist != NULL ) {
    trackListSize = NodeOp.getChildCnt( tracklist );
  }

  if( trackListSize > 0 ) {
    iONode tracknode;
    const char* listType = NodeOp.getName( NodeOp.getChild(tracklist, 0));
    int i = 0;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_resetBlockids: Checking %d %s nodes", trackListSize, listType );
    for( i = trackListSize - 1 ; i >= 0 ; i-- ) {
      tracknode = NodeOp.getChild(tracklist, i);
      if( tracknode ) {
        const char* blockid = wItem.getblockid(tracknode);
        if( blockid && StrOp.len( blockid ) ) {
          wItem.setblockid(tracknode, "");
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_resetBlockids: cleaned blockid %s", blockid);
          modifications++;
          numModifiedTracks++;
        }
      }
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cleanup %slist [%4d/%4d nodes]%5d modifications in%5d nodes (all blockids)", 
        listType, trackListSize, NodeOp.getChildCnt( tracklist ), modifications, numModifiedTracks );
  }
  return(modifications);
}

/* check if given action is used in "all" or "all-reverse" */
Boolean blockUsesActionInDefaultDirs( iONode blocknode, const char* action ) {
  if( blocknode ) {
    iONode fbevt = wBlock.getfbevent( blocknode );
    while( fbevt != NULL ) {
      const char* currFbid   = wFeedbackEvent.getid( fbevt );
      const char* currFrom   = wFeedbackEvent.getfrom( fbevt );
      const char* currAction = wFeedbackEvent.getaction( fbevt );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "blockUsesActionInDefaultDirs: Checking from %s fbid %s action %s", currFrom, currFbid, currAction );
      /* check if currFrom is "all" or "all-reverse" and action is "enter" or "in" */
      if( ( StrOp.len(currFrom) > 0 )
          && ( StrOp.equals( currFrom, wFeedbackEvent.from_all) || StrOp.equals( currFrom, wFeedbackEvent.from_all_reverse) ) 
          && ( StrOp.len(currAction) > 0 )
          && StrOp.equals( currAction, action) ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "blockUsesActionInDefaultDirs: return True" );
        return True;
      }
      fbevt = wBlock.nextfbevent( blocknode, fbevt );
    }
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "blockUsesActionInDefaultDirs: return False" );
  return False;
}

/* in blocks remove fbevents if "from" is "all" or "all-reverse" and enter2in is _NOT_ used in "all" or "all-reverse" */
static int _cleanupBlocksFbEvtBasic( iONode blocklist ) {
  int modifications = 0;
  int numModifiedBlocks = 0;
  int blockListSize = NodeOp.getChildCnt( blocklist );

  if( blockListSize > 0 ) {
    iONode blocknode;
    iOBlock block;   
    const char* listType = NodeOp.getName( NodeOp.getChild(blocklist, 0));
    int i = 0;
    Boolean thisBlockChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksFbEvtBasic: Checking %d %s nodes", blockListSize, listType );
    for( i = blockListSize - 1 ; i >= 0 ; i-- ) {
      blocknode = NodeOp.getChild(blocklist, i);
      if( blocknode ) {
        thisBlockChanged = False;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksFbEvtBasic: Checking node %s", wBlock.getid( blocknode ) );

        Boolean usesEnter2In = blockUsesActionInDefaultDirs( blocknode, wFeedbackEvent.enter2in_event );
        if( usesEnter2In ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "[%s][%s] usesEnter2In -> don't remove any enter or in",
              NodeOp.getName(blocknode), wItem.getid(blocknode) );
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksFbEvtBasic: [%s][%s] usesEnter2In -> don't remove any enter or in",
              NodeOp.getName(blocknode), wItem.getid(blocknode) );
        }
        else {
          iONode fbevt = wBlock.getfbevent( blocknode );
          iOList delList = ListOp.inst();
          while( fbevt != NULL ) {
            const char* fbid = wFeedbackEvent.getid( fbevt );
            const char* from = wFeedbackEvent.getfrom( fbevt );
            const char* action = wFeedbackEvent.getaction( fbevt );
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksFbEvtBasic: Checking from %s fbid %s action %s", from, fbid, action );
            /* check if from is "all" or "all-reverse" and action is "enter" or "in" */
            if( ( StrOp.len(from) > 0 ) 
                && ( StrOp.equals( from, wFeedbackEvent.from_all) || StrOp.equals( from, wFeedbackEvent.from_all_reverse) ) 
                && ( StrOp.len(action) > 0 )
                && ( StrOp.equals( action, wFeedbackEvent.enter_event) || StrOp.equals( action, wFeedbackEvent.in_event) ) ) {
              modifications++;
              thisBlockChanged = True;
              ListOp.add( delList, (obj)fbevt );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksFbEvtBasic: Marking  from %s fbid %s action %s to remove", from, fbid, action );
            }
            fbevt = wBlock.nextfbevent( blocknode, fbevt );
          }

          /* remove all marked items */
          iONode fb ;
          if( ListOp.size(delList) > 0 ) {
            fb = (iONode)ListOp.first( delList );
            while( fb != NULL ) {
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksFbEvtBasic: Deleting from %s fbid %s action %s", 
                  wFeedbackEvent.getfrom(fb), wFeedbackEvent.getid(fb), wFeedbackEvent.getaction(fb) );
              NodeOp.removeChild( blocknode, fb );
              NodeOp.base.del(fb);
              fb = (iONode)ListOp.next( delList );
            }
          }  
          ListOp.base.del(delList);

        }
        if( thisBlockChanged ) {
          numModifiedBlocks++;  
        }
      }  
    }    
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cleanup %slist [%4d/%4d nodes]%5d modifications in%5d nodes (fbevents in \"all\" and \"all-reverse\")", 
        listType, blockListSize, NodeOp.getChildCnt( blocklist ), modifications, numModifiedBlocks );
  }
  return(modifications);
}
 
/* check if given action is used in "all" or "all-reverse" */
Boolean seltabUsesActionInDefaultDirs( iONode seltabnode, const char* action ) {
  if( seltabnode ) {
    iONode fbevt = wSelTab.getfbevent( seltabnode );
    while( fbevt != NULL ) {
      const char* currFbid   = wFeedbackEvent.getid( fbevt );
      const char* currFrom   = wFeedbackEvent.getfrom( fbevt );
      const char* currAction = wFeedbackEvent.getaction( fbevt );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "seltabUsesActionInDefaultDirs: Checking from %s fbid %s action %s", currFrom, currFbid, currAction );
      /* check if currFrom is "all" or "all-reverse" and action is "enter" or "in" */
      if( ( StrOp.len(currFrom) > 0 )
          && ( StrOp.equals( currFrom, wFeedbackEvent.from_all) || StrOp.equals( currFrom, wFeedbackEvent.from_all_reverse) ) 
          && ( StrOp.len(currAction) > 0 )
          && StrOp.equals( currAction, action) ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "seltabUsesActionInDefaultDirs: return True" );
        return True;
      }
      fbevt = wBlock.nextfbevent( seltabnode, fbevt );
    }
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "seltabUsesActionInDefaultDirs: return False" );
  return False;
}

/* in selection tabels remove fbevents if "from" is "all" or "all-reverse" and action is "enter" or "in"
 * but only if enter2in is _NOT_ used in "all" or "all-reverse"
 */
static int _cleanupSelTabsFbEvtBasic( iONode seltablist ) {
  int seltabListSize = 0;
  int modifications = 0;
  int numModifiedSelTabs = 0;

  if( seltablist != NULL ) {
    seltabListSize = NodeOp.getChildCnt( seltablist );
  }

  if( seltabListSize > 0 ) {
    iONode seltabnode;
    iOSelTab seltab;   
    const char* listType = NodeOp.getName( NodeOp.getChild(seltablist, 0));
    int i = 0;
    Boolean thisSelTabChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupSelTabsFbEvtBasic: Checking %d %s nodes", seltabListSize, listType );
    for( i = seltabListSize - 1 ; i >= 0 ; i-- ) {
      seltabnode = NodeOp.getChild(seltablist, i);
      if( seltabnode ) {
        thisSelTabChanged = False;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupSelTabsFbEvtBasic: Checking node %s", wSelTab.getid( seltabnode ) );

        Boolean usesEnter2In = seltabUsesActionInDefaultDirs( seltabnode, wFeedbackEvent.enter2in_event );
        if( usesEnter2In ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "[%s][%s] usesEnter2In -> don't remove any enter or in",
              NodeOp.getName(seltabnode), wItem.getid(seltabnode) );
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupSelTabsFbEvtBasic: [%s][%s] usesEnter2In -> don't remove any enter or in",
              NodeOp.getName(seltabnode), wItem.getid(seltabnode) );
        }
        else {
          iONode fbevt = wSelTab.getfbevent( seltabnode );
          iOList delList = ListOp.inst();
          while( fbevt != NULL ) {
            const char* fbid = wFeedbackEvent.getid( fbevt );
            const char* from = wFeedbackEvent.getfrom( fbevt );
            const char* action = wFeedbackEvent.getaction( fbevt );
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupSelTabsFbEvtBasic: Checking from %s fbid %s action %s", from, fbid, action );
            /* check if from is "all" or "all-reverse" and action is "enter" or "in" */
            if( ( StrOp.len(from) > 0 ) 
                && ( StrOp.equals( from, wFeedbackEvent.from_all) || StrOp.equals( from, wFeedbackEvent.from_all_reverse) ) 
                && ( StrOp.len(action) > 0 )
                && ( StrOp.equals( action, wFeedbackEvent.enter_event) || StrOp.equals( action, wFeedbackEvent.in_event) ) ) {
              modifications++;
              thisSelTabChanged = True;
              ListOp.add( delList, (obj)fbevt );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupSelTabsFbEvtBasic: Marking  from %s fbid %s action %s to remove", from, fbid, action );
            }
            fbevt = wSelTab.nextfbevent( seltabnode, fbevt );
          }

          /* remove all marked items */
          iONode fb ;
          if( ListOp.size(delList) > 0 ) {
            fb = (iONode)ListOp.first( delList );
            while( fb != NULL ) {
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupSelTabsFbEvtBasic: Deleting from %s fbid %s action %s", 
                  wFeedbackEvent.getfrom(fb), wFeedbackEvent.getid(fb), wFeedbackEvent.getaction(fb) );
              NodeOp.removeChild( seltabnode, fb );
              NodeOp.base.del(fb);
              fb = (iONode)ListOp.next( delList );
            }
          }  
          ListOp.base.del(delList);

        }
        if( thisSelTabChanged ) {
          numModifiedSelTabs++;  
        }
      }  
    }    
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cleanup %slist [%4d/%4d nodes]%5d modifications in%5d nodes (fbevents in \"all\" and \"all-reverse\")", 
        listType, seltabListSize, NodeOp.getChildCnt( seltablist ), modifications, numModifiedSelTabs );
  }
  return(modifications);
}

/* in blocks remove fbevent if byroute starts with "autogen-" */
static int _cleanupBlocksFbEvtAutogen( iONode blocklist ) {
  int modifications = 0;
  int numModifiedBlocks = 0;
  int blockListSize = 0;

  if( blocklist != NULL ) {
    blockListSize = NodeOp.getChildCnt( blocklist );
  }

  if( blockListSize > 0 ) {
    iONode blocknode;
    iOBlock block;
    const char* listType = NodeOp.getName( NodeOp.getChild(blocklist, 0));
    int i = 0;
    Boolean thisBlockChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksFbEvtAutogen: Checking %d %s nodes", blockListSize, listType );
    for( i = blockListSize - 1 ; i >= 0 ; i-- ) {
      blocknode = NodeOp.getChild(blocklist, i);
      if( blocknode ) {
        thisBlockChanged = False;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksFbEvtAutogen: Checking node %s", wBlock.getid( blocknode ) );

        iONode fbevt = wBlock.getfbevent( blocknode );
        iOList delList = ListOp.inst();
        while( fbevt != NULL ) {
          const char* fbid = wFeedbackEvent.getid( fbevt );
          const char* byroute = wFeedbackEvent.getbyroute( fbevt );
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksFbEvtAutogen: Checking fbid %s byroute %s", fbid, byroute );
          /* check if byroute starts with autogen- */
          if( ( StrOp.len(byroute) > 0 ) && ( StrOp.startsWith( byroute, "autogen-") ) ) {
            thisBlockChanged = True;
            modifications++;
            ListOp.add( delList, (obj)fbevt );
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksFbEvtAutogen: Marking fbid %s byroute %s to remove", fbid, byroute );
          }
          fbevt = wBlock.nextfbevent( blocknode, fbevt );
        }

        /* remove all "autogen-"-routeids */
        iONode fb ;
        if( ListOp.size(delList) > 0 ) {
          fb = (iONode)ListOp.first( delList );
          while( fb != NULL ) {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksFbEvtAutogen: Deleting fbid %s byroute %s", wFeedbackEvent.getid(fb), wFeedbackEvent.getbyroute(fb) );
            NodeOp.removeChild( blocknode, fb );
            NodeOp.base.del(fb);
            fb = (iONode)ListOp.next( delList );
          };
        }
        ListOp.base.del(delList);

        if( thisBlockChanged ) {
          numModifiedBlocks++;
        }
      }
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cleanup %slist [%4d/%4d nodes]%5d modifications in%5d nodes (fbevents with \"autogen-\")", 
        listType, blockListSize, NodeOp.getChildCnt( blocklist ), modifications, numModifiedBlocks );
  }
  return(modifications);
}

/* for blocks remove signal assignments */
static int _cleanupBlocksSignal( iONode blocklist ) {
  int modifications = 0;
  int numModifiedBlocks = 0;
  int blockListSize = 0;

  if( blocklist != NULL ) {
    blockListSize = NodeOp.getChildCnt( blocklist );
  }

  if( blockListSize > 0 ) {
    iONode blocknode;
    iOBlock block;
    const char* listType = NodeOp.getName( NodeOp.getChild(blocklist, 0));
    int i = 0;
    Boolean thisBlockChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksSignal: Checking %d %s nodes", blockListSize, listType );
    for( i = blockListSize - 1 ; i >= 0 ; i-- ) {
      blocknode = NodeOp.getChild(blocklist, i);
      if( blocknode ) {
        thisBlockChanged = False;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupBlocksSignal: Checking node %s", wBlock.getid( blocknode ) );

        if( wBlock.getsignal(blocknode) != NULL && StrOp.len(wBlock.getsignal(blocknode)) ) {
          wBlock.setsignal(blocknode, "");
          thisBlockChanged = True;
          modifications++;
        }
        if( wBlock.getwsignal(blocknode) != NULL && StrOp.len(wBlock.getwsignal(blocknode)) ) {
          wBlock.setwsignal(blocknode, "" );
          thisBlockChanged = True;
          modifications++;
        }
        if( wBlock.getsignalR(blocknode) != NULL && StrOp.len(wBlock.getsignalR(blocknode)) ) {
          wBlock.setsignalR(blocknode, "" );
          thisBlockChanged = True;
          modifications++;
        }
        if( wBlock.getwsignalR(blocknode) != NULL && StrOp.len(wBlock.getwsignalR(blocknode)) ) {
          wBlock.setwsignalR(blocknode, "" ) ;
          thisBlockChanged = True;
          modifications++;
        }

        if( thisBlockChanged ) {
          numModifiedBlocks++;
        }
      }
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cleanup %slist [%4d/%4d nodes]%5d modifications in%5d nodes (signals)", 
        listType, blockListSize, NodeOp.getChildCnt( blocklist ), modifications, numModifiedBlocks );
  }
  return(modifications);
}



/* cleanup called -> call the subroutines according to variables */
static int _cleanup(iOAnalyse inst) {
  if( inst == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "AnalyseOp.cleanup() called without a valid instance" );
    return -1;
  }
  iOAnalyseData data = Data(inst);
  int modifications = 0;

  /* remove all "autogen-"-routes */
  modifications += _cleanupRoutes(inst);

  /* clean old references to "autogen-"-routes in "fbevent" of blocks */
  modifications += _cleanupBlocksFbEvtAutogen( wPlan.getbklist(data->plan) );

  if( data->cleanRouteId ) {
    modifications += _cleanupAutogenRouteids( wPlan.gettklist(data->plan) );
    modifications += _cleanupAutogenRouteids( wPlan.getswlist(data->plan) );
    modifications += _cleanupAutogenRouteids( wPlan.getsglist(data->plan) );
    modifications += _cleanupAutogenRouteids( wPlan.getfblist(data->plan) );
  }

  if( data->resetBlockId ) {
    modifications += _resetBlockids( wPlan.gettklist(data->plan) );
    modifications += _resetBlockids( wPlan.getswlist(data->plan) );
    modifications += _resetBlockids( wPlan.getsglist(data->plan) );
    modifications += _resetBlockids( wPlan.getfblist(data->plan) );
  }


  if( data->resetSignalBlockAssignment ) {
    /* remove signal assignments of blocks */
    modifications += _cleanupBlocksSignal( wPlan.getbklist(data->plan) );
  }

  if( data->resetFeedbackBlockAssignment ) {
    /* remove feedback assignments for "enter" and "in" actions in blocks (except blocks with "enter2in") */
    modifications += _cleanupBlocksFbEvtBasic( wPlan.getbklist(data->plan) );
    /* remove feedback assignments for "enter" and "in" selection table (except blocks with "enter2in") */
    modifications += _cleanupSelTabsFbEvtBasic( wPlan.getseltablist(data->plan) );
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Cleanup finished with %d modifications.", modifications );

  if( modifications > 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Please restart Rocrail server." );
    /* force disconnect/reconnect ? */
    /* AppOp.shutdown(); -> sometimes crashes */
    /* ==> need something like AppOp.reinit(); */
  }

  return( modifications );
}


/**  */
static Boolean didShowCredits = False;
static struct OAnalyse* _inst() {
  iOAnalyse __Analyse = allocMem( sizeof( struct OAnalyse ) );
  iOAnalyseData data = allocMem( sizeof( struct OAnalyseData ) );
  MemOp.basecpy( __Analyse, &AnalyseOp, 0, sizeof( struct OAnalyse ), data );

  data->model     = AppOp.getModel();
  data->plan      = ModelOp.getModel(data->model);
  data->maxZlevel = 0;
  data->maxConnectorDistance = AnalyseOp.MINIMAL_MAX_CONNECTOR_DISTANCE;

  /* Initialize lists members... */
  data->objectmap = MapOp.inst();
  data->bklist    = ListOp.inst();
  data->preRTlist = ListOp.inst();
  data->notRTlist = ListOp.inst();

  iONode aoIni = AppOp.getIni() ;
  iONode anaOpt = wRocRail.getanaopt( aoIni ) ;

  if( !didShowCredits ) {
    didShowCredits = True;
  }

  if( ! anaOpt ) {
    /* no analyzer options in ini -> create a node */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "_inst: create node %s", wAnaOpt.name() );
    iONode anaOpt = NodeOp.inst( wAnaOpt.name(), aoIni, ELEMENT_NODE );
    if( ! anaOpt ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "_inst: create node %s failed.", wAnaOpt.name() );

      /* cleanup memory */
      MapOp.base.del(data->objectmap);
      ListOp.base.del(data->bklist);
      ListOp.base.del(data->preRTlist);
      ListOp.base.del(data->notRTlist);
      freeMem( data );
      freeMem( __Analyse );

      return NULL;
    }
    else {
      NodeOp.addChild( aoIni, anaOpt );
    }
  }

  /* set options to current value or initialize with default (creates non existant entries in rocrail.ini) */
  /* basic analyzer jobs */
  wAnaOpt.setsetRouteId( anaOpt, wAnaOpt.issetRouteId( anaOpt ) ) ;
  wAnaOpt.setsetBlockId( anaOpt, wAnaOpt.issetBlockId( anaOpt ) ) ;
  wAnaOpt.setaddSignalBlockAssignment( anaOpt, wAnaOpt.isaddSignalBlockAssignment( anaOpt ) ) ;
  wAnaOpt.setaddFeedbackBlockAssignment( anaOpt, wAnaOpt.isaddFeedbackBlockAssignment( anaOpt ) ) ;

  wAnaOpt.setcleanRouteId( anaOpt, wAnaOpt.iscleanRouteId( anaOpt ) ) ;
  wAnaOpt.setresetBlockId( anaOpt, wAnaOpt.isresetBlockId( anaOpt ) ) ;
  wAnaOpt.setresetSignalBlockAssignment( anaOpt, wAnaOpt.isresetSignalBlockAssignment( anaOpt ) ) ;
  wAnaOpt.setresetFeedbackBlockAssignment( anaOpt, wAnaOpt.isresetFeedbackBlockAssignment( anaOpt ) ) ;

  /* extended checks */
  wAnaOpt.setbasicCheck(  anaOpt, wAnaOpt.isbasicCheck(  anaOpt ) ) ;
  wAnaOpt.setbasicClean(  anaOpt, wAnaOpt.isbasicClean(  anaOpt ) ) ;
  wAnaOpt.setblockCheck(  anaOpt, wAnaOpt.isblockCheck(  anaOpt ) ) ;
  wAnaOpt.setblockClean(  anaOpt, wAnaOpt.isblockClean(  anaOpt ) ) ;
  wAnaOpt.setrouteCheck(  anaOpt, wAnaOpt.isrouteCheck(  anaOpt ) ) ;
  wAnaOpt.setrouteClean(  anaOpt, wAnaOpt.isrouteClean(  anaOpt ) ) ;
  wAnaOpt.setactionCheck( anaOpt, wAnaOpt.isactionCheck( anaOpt ) ) ;
  wAnaOpt.setactionClean( anaOpt, wAnaOpt.isactionClean( anaOpt ) ) ;

  /* store option values in local instance */
  data->setRouteId                    = wAnaOpt.issetRouteId(                    anaOpt ) ;
  data->setBlockId                    = wAnaOpt.issetBlockId(                    anaOpt ) ;
  data->addSignalBlockAssignment      = wAnaOpt.isaddSignalBlockAssignment(      anaOpt ) ;
  data->addFeedbackBlockAssignment    = wAnaOpt.isaddFeedbackBlockAssignment(    anaOpt ) ;

  data->cleanRouteId                  = wAnaOpt.iscleanRouteId(                  anaOpt ) ;
  data->resetBlockId                  = wAnaOpt.isresetBlockId(                  anaOpt ) ;
  data->resetSignalBlockAssignment    = wAnaOpt.isresetSignalBlockAssignment(    anaOpt ) ;
  data->resetFeedbackBlockAssignment  = wAnaOpt.isresetFeedbackBlockAssignment(  anaOpt ) ;

  /* extended check options */
  data->basicCheck  = wAnaOpt.isbasicCheck(  anaOpt ) ;
  data->basicClean  = wAnaOpt.isbasicClean(  anaOpt ) ;
  data->blockCheck  = wAnaOpt.isblockCheck(  anaOpt ) ;
  data->blockClean  = wAnaOpt.isblockClean(  anaOpt ) ;
  data->routeCheck  = wAnaOpt.isrouteCheck(  anaOpt ) ;
  data->routeClean  = wAnaOpt.isrouteClean(  anaOpt ) ;
  data->actionCheck = wAnaOpt.isactionCheck( anaOpt ) ;
  data->actionClean = wAnaOpt.isactionClean( anaOpt ) ;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ExtChk: basic[%d]c[%d] block[%d]c[%d] route[%d]c[%d] action[%d]c[%d]",
      data->basicCheck,  data->basicClean,
      data->blockCheck,  data->blockClean,
      data->routeCheck,  data->routeClean,
      data->actionCheck, data->actionClean );

  instCnt++;
  return __Analyse;
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/analyse.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/

