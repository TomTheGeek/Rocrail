/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis <r.j.versluis@rocrail.net>, Jean-Michel Fischer <jmf@polygonpunkt.de>

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
#include "rocrail/wrapper/public/Location.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/StageList.h"
#include "rocrail/wrapper/public/StageSection.h"
#include "rocrail/wrapper/public/Exception.h"
#include "rocrail/wrapper/public/AnaOpt.h"
#include "rocrail/wrapper/public/State.h"

#include "rocrail/public/app.h"
#include "rocrail/public/model.h"
#include "rocrail/public/track.h"
#include "rocrail/public/switch.h"
#include "rocrail/public/signal.h"
#include "rocrail/public/fback.h"
#include "rocrail/public/route.h"


static int instCnt = 0;

/* some forward declaration */
static Boolean _checkPlanHealth(iOAnalyse inst);
static Boolean __analyseItem(iOAnalyse inst, iONode item, iOList route, int travel,
    int turnoutstate, int depth, Boolean toPreRTlist);

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


static Boolean isStageBlockById( iOModel model, const char* blockid  ) {
  return( NULL != ModelOp.getStage(model, blockid ));
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
        /* fb is already used in a block */
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
        /* fb is already used in a stagingblock */
        return True;
      }
      if( ( fbidocc != NULL ) && StrOp.equals( fbid, id ) ) {
        /* fb is already used in a stagingblock as occ fb */
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
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: bl[%s] from[%s] byroute[%s] found multiple usage of (%s %s %s %s)",
              bkid, from, byroute, wFeedbackEvent.enter_event, wFeedbackEvent.enter2pre_event, wFeedbackEvent.enter2shortin_event, wFeedbackEvent.enter2in_event );
          numProblems++;
        }
        if( foundInMulti ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block feedback action check: bl[%s] from[%s] byroute[%s] found multiple usage of (%s %s)",
              bkid, from, byroute, wFeedbackEvent.in_event, wFeedbackEvent.enter2in_event );
          numProblems++;
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
 
 
static void __prepare(iOAnalyse inst, iOList list, int modx, int mody) {
  iOAnalyseData data = Data(inst);
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
      MapOp.put( data->objectmap, key, (obj)node);

      wItem.setx( node, wItem.getx(node)+modx);
      wItem.sety( node, wItem.gety(node)+mody);

      /* put keys for all covered fields */
      if( StrOp.equals( NodeOp.getName(node), wSwitch.name() ) ) {
        if( StrOp.equals( wItem.gettype(node), wSwitch.crossing ) ||
            StrOp.equals( wItem.gettype(node), wSwitch.dcrossing ) ||
            StrOp.equals( wItem.gettype(node), wSwitch.ccrossing )) {

          if( StrOp.equals( ori, wItem.east ) || StrOp.equals( ori, wItem.west ) ) {
            __createKey( key, node, 1, 0, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, wItem.getori(node), ori );

            MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
            __createKey( key, node, 0, 1, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, wItem.getori(node), ori );

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

            MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
            __createKey( key, node, 0, i, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );

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

            MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
            __createKey( key, node, 0, i, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );

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

            MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, wItem.north ) || StrOp.equals( ori, wItem.south ) ) {
            __createKey( key, node, 0, i, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );

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
    /* switch */
    else if( __getType(item) == typeSwitch) {

      /* coming from the points */
      if(        travel == 0 &&  StrOp.equals( itemori, wItem.east )
              && StrOp.equals( wItem.gettype(item), wSwitch.right ) ) {
        if(turnoutstate == 1)
          return oriNorth+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 0 &&  StrOp.equals( itemori, wItem.west )
              && StrOp.equals( wItem.gettype(item), wSwitch.left ) ) {
        if(turnoutstate == 1)
          return oriSouth+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.north )
              && StrOp.equals( wItem.gettype(item), wSwitch.right ) ) {
        if(turnoutstate == 1)
          return oriEast+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.south )
              && StrOp.equals( wItem.gettype(item), wSwitch.left ) ) {
        if(turnoutstate == 1)
          return oriWest+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.west )
              && StrOp.equals( wItem.gettype(item), wSwitch.right ) ) {
        if(turnoutstate == 1)
          return oriSouth+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.east )
              && StrOp.equals( wItem.gettype(item), wSwitch.left ) ) {
        if(turnoutstate == 1)
          return oriNorth+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.south )
              && StrOp.equals( wItem.gettype(item), wSwitch.right ) ) {
        if(turnoutstate == 1)
          return oriWest+twoWayTurnout;
        return travel+twoWayTurnout;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.north )
              && StrOp.equals( wItem.gettype(item), wSwitch.left ) ) {
        if(turnoutstate == 1)
          return oriEast+twoWayTurnout;
        return travel+twoWayTurnout;
      }

      /* coming from the frog -> straight line */
      else if( travel == 0 &&  StrOp.equals( itemori, wItem.west )
              && StrOp.equals( wItem.gettype(item), wSwitch.right ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 0 &&  StrOp.equals( itemori, wItem.east )
              && StrOp.equals( wItem.gettype(item), wSwitch.left ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.south )
              && StrOp.equals( wItem.gettype(item), wSwitch.right ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.north )
              && StrOp.equals( wItem.gettype(item), wSwitch.left ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.east )
             && StrOp.equals( wItem.gettype(item), wSwitch.right ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.west )
             && StrOp.equals( wItem.gettype(item), wSwitch.left ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.north )
            && StrOp.equals( wItem.gettype(item), wSwitch.right ) ) {
        *turnoutstate_out = 0;
        return travel;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.south )
            && StrOp.equals( wItem.gettype(item), wSwitch.left ) ) {
        *turnoutstate_out = 0;
        return travel;
      }

      /* coming from the frog -> diverging line */
      else if( travel == 0 &&  StrOp.equals( itemori, wItem.north )
              && StrOp.equals( wItem.gettype(item), wSwitch.right ) ) {
        *turnoutstate_out = 1;
        return oriSouth;
      } else if( travel == 0 &&  StrOp.equals( itemori, wItem.north )
              && StrOp.equals( wItem.gettype(item), wSwitch.left ) ) {
        *turnoutstate_out = 1;
        return oriNorth;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.west )
              && StrOp.equals( wItem.gettype(item), wSwitch.right ) ) {
        *turnoutstate_out = 1;
        return oriWest;
      } else if( travel == 1 &&  StrOp.equals( itemori, wItem.west )
              && StrOp.equals( wItem.gettype(item), wSwitch.left ) ) {
        *turnoutstate_out = 1;
        return oriEast;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.south )
             && StrOp.equals( wItem.gettype(item), wSwitch.right ) ) {
        *turnoutstate_out = 1;
        return oriNorth;
      } else if( travel == 2 &&  StrOp.equals( itemori, wItem.south )
             && StrOp.equals( wItem.gettype(item), wSwitch.left ) ) {
        *turnoutstate_out = 1;
        return oriSouth;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.east )
            && StrOp.equals( wItem.gettype(item), wSwitch.right ) ) {
        *turnoutstate_out = 1;
        return oriEast;
      } else if( travel == 3 &&  StrOp.equals( itemori, wItem.east )
            && StrOp.equals( wItem.gettype(item), wSwitch.left ) ) {
        *turnoutstate_out = 1;
        return oriWest;
      }

      /* crossing */
      else if( StrOp.equals( wItem.gettype(item), wSwitch.crossing ) && wSwitch.getaddr1(item) == 0 && wSwitch.getport1(item) == 0 ) {
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
      else if( StrOp.equals( wItem.gettype(item), wSwitch.ccrossing ) ) {
        /* something was wrong with ccrossing :) */
        char mkey[32] = {'\0'};
        __createKey( mkey, item, 0, 0, 0);

        iONode itemA = (iONode)MapOp.get( data->objectmap, key);
        iONode itemB = (iONode)MapOp.get( data->objectmap, mkey);

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__travel: ccross switch[%s] type: [%s] key[%s] mkey[%s] %08.8X %08.8X, travel[%d]",
            wItem.getid(item), wItem.gettype(item), key, mkey, itemA, itemB, travel);

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
      else if( StrOp.equals( wItem.gettype(item), wSwitch.dcrossing ) ||
               StrOp.equals( wItem.gettype(item), wSwitch.crossing ) && (wSwitch.getaddr1(item) != 0 || wSwitch.getport1(item) != 0 ) )
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
      else if( StrOp.equals( wItem.gettype(item), wSwitch.threeway ) ) {

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
      else if( StrOp.equals( wItem.gettype(item), wSwitch.twoway ) ) {

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
          StrOp.equals(wItem.gettype(tracknode),  wTrack.connector ) &&
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

  if( StrOp.equals(NodeOp.getName(item), wTrack.name() ) && StrOp.equals(wItem.gettype(item), wTrack.connector )) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "__aBC: [%s][%s] [%s] is a connector. cpid[%s] tknr[%d]",
        NodeOp.getName(item), wItem.gettype(item), wItem.getid(item), wTrack.getcounterpartid(item), wTrack.gettknr(item));
  } else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "__aBC: [%s][%s] [%s] not a connector",
        NodeOp.getName(item), wItem.gettype(item), wItem.getid(item));
  }

  if( wTrack.gettknr(item) >= 50 ) {
    /* trknr [50..99] */
    Boolean found = False;

    /* search for counterpart somewhere else (may be on same level but not necessarly in same direction) !
     * - during search don't care for direction 
     * - use travel from counterpart when continuing search
     */
    iONode nextitem = __findConnectorCounterpart( inst, item );

    if( nextitem ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "found counterpart: [%s]", wItem.getid(nextitem));
      if( StrOp.equals(NodeOp.getName(nextitem), wTrack.name() ) && StrOp.equals(wItem.gettype(nextitem), wTrack.connector )) {
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
            NodeOp.getName(item), wItem.gettype(item), wItem.getid(item));
      }
    }
    return False;
  }

  int i = 0;
  for ( i = 0; i <= data->maxConnectorDistance; i++) {
    /* search a maximum distance of maxConnectorDistance items for the counterpart (on same level) */
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

    /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "next key: %s", key); */
    iONode nextitem = (iONode)MapOp.get( data->objectmap, key);

    if( nextitem != NULL ) {
      if( StrOp.equals(NodeOp.getName(nextitem), wTrack.name() ) && StrOp.equals(wItem.gettype(nextitem), wTrack.connector )) {
        const char* nextitemori = wItem.getori( nextitem);
        if( nextitemori == NULL )
          nextitemori = wItem.west;
        Boolean found = False;
        if( StrOp.equals( nextitemori, wItem.west ) && travel == 0){
          found = True;
        } else if( StrOp.equals( nextitemori, wItem.north ) && travel == 3){
          found = True;
        } else if( StrOp.equals( nextitemori, wItem.east ) && travel == 2){
          found = True;
        } else if( StrOp.equals( nextitemori, wItem.south ) && travel == 1){
          found = True;
        }

        if( found ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "found counterpart: [%s]", wItem.getid(nextitem));

          __analyseItem(inst, nextitem, route, travel, 0, depth, toPreRTlist);
        }
        return found;
      }
    }
  }
  return False;
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

  if( StrOp.equals(NodeOp.getName(item), wTrack.name() )
      && StrOp.equals(wItem.gettype(item), wTrack.connector )) {

    Boolean found = False;
    if( StrOp.equals( itemori, wItem.west ) && travel == 2){
      found = True;
    } else if( StrOp.equals( itemori, wItem.north ) && travel == 1){
      found = True;
    } else if( StrOp.equals( itemori, wItem.east ) && travel == 0){
      found = True;
    } else if( StrOp.equals( itemori, wItem.south ) && travel == 3){
      found = True;
    }

    if( found ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "found connector: [%s] travel: [%d] ori: [%s]",
              wItem.getid(item), travel, itemori);

      if( wTrack.getcounterpartid(item) != NULL && !StrOp.equals( wTrack.getcounterpartid(item), "") ){
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "found counterpart for: [%s] counterpart: [%s]",
            wItem.getid(item), wTrack.getcounterpartid(item) );

        iOTrack track = ModelOp.getTrack( data->model, wTrack.getcounterpartid(item) );

        /* go on at the connector */
        if( track != NULL ) {
          iONode nextitem = TrackOp.base.properties(track);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "continue at counterpart: [%s]", wItem.getid(nextitem) );
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

  if( StrOp.equals( NodeOp.getName(item), wTrack.name() ) && StrOp.equals( wItem.gettype(item), wTrack.dir ) ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "wTrack.dir=%s travel=%d", wItem.getid(item), travel );
  }

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
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "end   analyzing item [%-20s] travel: [%d] name=%s type=%s",
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
          /* DIR is not exactly in opposite direction -> add route fragmnet to notRTlist and leave */
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
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, " -> stop: nextNext [%s][%s] not fitting -> end of search",
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
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " next is a block: [%s]", wItem.getid(nextitem));
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  -> LIST: block [%s] travel: [%d] depth: [%d] tos: [%d]",
                wItem.getid(nextitem), travel, depth, turnoutstate);

      const char* nextitemori = wItem.getori(nextitem);
      if( nextitemori == NULL) {
        nextitemori = wItem.west;
      }
      const char* state = "-";
      if( StrOp.equals( nextitemori, wItem.west ) && travel == 2){
        state = "+";
      } else if( StrOp.equals( nextitemori, wItem.north ) && travel == 1){
        state = "+";
      } else if( StrOp.equals( nextitemori, wItem.east ) && travel == 0){
        state = "+";
      } else if( StrOp.equals( nextitemori, wItem.south ) && travel == 3){
        state = "+";
      }

      /* add nextitem to route */
      iONode itemA = (iONode)NodeOp.base.clone( nextitem);
      wItem.setstate(itemA, state);
      ListOp.add( route, (obj)itemA );

      /* do we have a preRoute or a notRoute */
      iONode firstitem = (iONode)ListOp.first( route );
      if( StrOp.equals(NodeOp.getName(firstitem), wStage.name() ) && StrOp.equals( wItem.getstate(firstitem), "+" ) ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Route Source [sb] %s%s -> notRTlist", wItem.getid(firstitem), wItem.getstate(firstitem) );
        /* route starting at enter side of staging block */
        ListOp.add( data->notRTlist, (obj)route);
      }
      else if( StrOp.equals(NodeOp.getName(nextitem), wStage.name() ) && StrOp.equals( state, "-" ) ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Route Destination [sb] %s%s -> notRTlist", wItem.getid(nextitem), state );
        /* route ending at exit side of staging block */
        ListOp.add( data->notRTlist, (obj)route);
      }
      else if( StrOp.equals( wItem.getid(firstitem), wItem.getid(nextitem) ) ) {
        /* route ending at start -> loop route detected */
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Loop route (Source == Destination) %s%s to %s%s -> notRTlist", wItem.getid(firstitem), wItem.getstate(firstitem), wItem.getid(nextitem), state );
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Loop route (Source == Destination) %s%s to %s%s. Check your plan.", wItem.getid(firstitem), wItem.getstate(firstitem), wItem.getid(nextitem), state );
        ListOp.add( data->notRTlist, (obj)route);
      }
      else {
        /* if end of route is a selection table that is not in manager mode or not in sharedfb mode then do not create a valid route to it */
        if( StrOp.equals( NodeOp.getName(nextitem), wSelTab.name() ) && ( ! wSelTab.ismanager(nextitem) || ! wSelTab.issharedfb(nextitem) ) ) {
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
    } else if ( StrOp.equals(NodeOp.getName(nextitem), wSwitch.name() ) ) {

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
    } /* if bk || sw */

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
    else if( StrOp.equals( typ, wSwitch.name() ) ) {
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
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "anaAll item loop1: checkitem  [%s] for [%s][%s] state=%s", bka, NodeOp.getName(item), wItem.getid(item), bka, wItem.getstate(item) );

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
            wTrack.setblockid(node, bka);
            modifications++;
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s]", bka, NodeOp.getName(item), wItem.getid(item) );
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
          wFeedback.setblockid(node, bka);
          modifications++;
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s]", bka, NodeOp.getName(item), wItem.getid(item) );
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
        node = FBackOp.base.properties(signal);
        const char* blockid = wItem.getblockid(node);
        /* only set blockid if not already set */
        if( ( blockid == NULL ) || ( StrOp.len( blockid ) == 0 ) ) {
          wSignal.setblockid(node, bka);
          modifications++;
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "anaAll item loop2: setblockid [%s] for [%s][%s]", bka, NodeOp.getName(item), wItem.getid(item) );
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
  for( i = 0; i <childcnt; i++) {
    child = NodeOp.getChild( stlist, i);

    if( StrOp.startsWith( wItem.getid( child), "autogen-" )) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "removed autogen route: [%s]", wItem.getid( child));
      ListOp.add( delList, (obj)child );
    }
  }

  childcnt = ListOp.size(delList);
  for( i = 0; i < childcnt; i++) {
    NodeOp.removeChild( stlist, (iONode)ListOp.get(delList, i) );
  }
  ListOp.base.del(delList);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " ");
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "the analyzer found the routes:");

  int fbcount = 0;

  iOList routelist = (iOList)ListOp.first( data->preRTlist );
  while(routelist) {

    Boolean addToList = True;
    Boolean addRtId = True;

    /* create new route element and set some defaults */
    iONode newRoute = NodeOp.inst( wRoute.name(), NULL, ELEMENT_NODE );
    wRoute.setshow( newRoute, False );
    wRoute.setx( newRoute, 0 );
    wRoute.sety( newRoute, 0 );

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
        wSwitch.setcmd( swcmd, "track");
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

      if( StrOp.equals( NodeOp.getName(item), wSwitch.name()) ) {
        iONode swcmd = NodeOp.inst( wSwitchCmd.name(), NULL, ELEMENT_NODE );
        wItem.setid( swcmd, wItem.getid(item));
        wSwitch.setcmd( swcmd, wItem.getstate(item));
        NodeOp.addChild( newRoute, swcmd );
      }

      if( StrOp.equals( NodeOp.getName(item), wTrack.name()) ||
          StrOp.equals( NodeOp.getName(item), wFeedback.name()) ||
          StrOp.equals( NodeOp.getName(item), wSignal.name()) ) {

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

        /* set routeids for tk|fb|sg */
        if( data->setRouteId ) {
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

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  it contains %d blocks", ListOp.size(data->bklist) );

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
  iOMap xyzMap = MapOp.inst();
  iOMap sensorMap = MapOp.inst();
  iOMap switchMap = MapOp.inst();
  int dbs = NodeOp.getChildCnt(data->plan);
  int i = 0;

  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "checking plan health..." );

  if( !wCtrl.isuseblockside( wRocRail.getctrl( AppOp.getIni() ) ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: Block side routing is not enabled; The classic method is deprecated." );
    healthy = False;
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

      /* check the basic addressing */
      if( StrOp.equals( wLoc.name(), NodeOp.getName(item) ) ) {
        if( wLoc.getaddr(item) == 0 && !StrOp.equals(wLoc.getprot(item), wLoc.prot_A) ) {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: loco %s has no address set", wItem.getid(item) );
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
      }

      if( StrOp.equals( wSwitch.name(), NodeOp.getName(item) ) ) {
        if( wSwitch.getaddr1(item) == 0 && wSwitch.getport1(item) == 0 ) {
          if( StrOp.equals( wSwitch.gettype(item), wSwitch.crossing ) || StrOp.equals( wSwitch.gettype(item), wSwitch.ccrossing ) ) {
            /* crossing and centered crossing do not need an address */
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "INFO: crossing \"%s\" has no address -> cross", wItem.getid(item) );
          }
          else {
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: switch %s has no address set", wItem.getid(item) );
            healthy = False;
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

      /* checking overlapping */
      if( wItem.isshow(item) ) {
        if( MapOp.haskey(xyzMap, key) ) {
          iONode firstItem = (iONode)MapOp.get(xyzMap, key);
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: object [%s] with id [%s] at [%d,%d,%d] overlaps object [%s] with id [%s]",
              NodeOp.getName(item), wItem.getid(item),
              wItem.getx(item), wItem.gety(item), wItem.getz(item),
              NodeOp.getName(firstItem), wItem.getid(firstItem));
          healthy = False;
        }

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
          else
            MapOp.put(xyzMap, key, (obj)item );
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

  /* check for very lonely objects */
  if( MapOp.size(xyzMap) > 0 ) {
    int items = MapOp.size(xyzMap);
    int maxDist = 0;
    iONode lonelyItem = NULL;
    iONode item = (iONode)MapOp.first(xyzMap);
    while( item != NULL ) {
      if( maxDist < wItem.getx(item) + wItem.gety(item) ) {
        /* use x + y coordinates as approach to sqrt( x^2 + y^2 ) */
        maxDist = wItem.getx(item) + wItem.gety(item) ;
        lonelyItem = item;
      }
      if( data->maxZlevel < wItem.getz(item) )
        data->maxZlevel = wItem.getz(item);
      item = (iONode)MapOp.next(xyzMap);
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

  MapOp.base.del(xyzMap);
  MapOp.base.del(sensorMap);
  MapOp.base.del(switchMap);

  if( healthy ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Plan is healthy" );
  } else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Plan is NOT healthty. See ERROR lines above." );
  }

  return healthy;
}


/* check for problems (do not change/repair/clean anything) */
static Boolean _checkExtended(iOAnalyse inst) {
  if( inst == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "AnalyseOp.checkExtended() called without a valid instance" );
    return False;
  }
  iOAnalyseData data = Data(inst);
  Boolean res;
    
  TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Extended checks are work in progress. Do not rely on them. BEGIN" );
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "ExtChk: bFAC[%d]c[%d] bFV[%d]c[%d] sFV[%d]c[%d]",
      data->blockFeedbackActionCheck, data->blockFeedbackActionCheckClean,
      data->blockRouteFbValidation,   data->blockRouteFbValidationClean,
      data->seltabRouteFbValidation,  data->seltabRouteFbValidationClean );

  /* checks that don't change anything are always allowed */
  if( data->blockFeedbackActionCheck ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Block feedback action check: in progress..." );
    res = blockFeedbackActionCheck( inst, False );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Block feedback action check: %s problems detected", res?"no":"some" );
  }

  /* check for fbevent entries of deleted routes */
  if( data->blockRouteFbValidation ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Block route feedback validation: in progress..." );
    res = blockRouteFbValidation( inst, False );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Block route feedback validation: %s problems detected", res?"no":"some" );
  }

  /* check for fbevent entries of deleted routes */
  if( data->seltabRouteFbValidation ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Selection table route feedback validation: in progress..." );
    res = seltabRouteFbValidation( inst, False );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Selection table route feedback validation: %s problems detected", res?"no":"some" );
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
  Boolean res;
  Boolean automode  = ModelOp.isAuto(data->model);
  Boolean isPowerOn = wState.ispower(ControlOp.getState(AppOp.getControl()));
    
  TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Extended checks are work in progress. Do not rely on them. BEGIN" );
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "ExtChk: bFAC[%d]c[%d] bFV[%d]c[%d] sFV[%d]c[%d]",
      data->blockFeedbackActionCheck, data->blockFeedbackActionCheckClean,
      data->blockRouteFbValidation,   data->blockRouteFbValidationClean,
      data->seltabRouteFbValidation,  data->seltabRouteFbValidationClean );

  /* any clean/repair options set ? */
  if( data->blockFeedbackActionCheckClean ||
      data->blockRouteFbValidationClean   ||
      data->seltabRouteFbValidationClean
    ) {
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

      if( data->blockFeedbackActionCheckClean ) {
        /* clean/repair are "once" options, reset option */
        wAnaOpt.setblockFeedbackActionCheckClean( anaOpt, False ) ;

        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Block feedback action check: Clean/repair in progress..." );
        res = blockFeedbackActionCheck( inst, True );
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Block feedback action check: %s Problems cleaned", res?"no":"some" );
        if( res == False )
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Please restart Rocrail server." );
      }

      if( data->blockRouteFbValidationClean ) {
        /* clean/repair are "once" options, reset option */
        wAnaOpt.setblockRouteFbValidationClean( anaOpt, False ) ;

        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Block route feedback validation: Clean/repair in progress..." );
        res = blockRouteFbValidation( inst, True );
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Block route feedback validation: %s problems cleaned", res?"no":"some" );
        if( res == False )
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Please restart Rocrail server." );
      }

      if( data->seltabRouteFbValidationClean ) {
        /* clean/repair are "once" options, reset option */
        wAnaOpt.setseltabRouteFbValidationClean( anaOpt, False ) ;

        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Selection table route feedback validation: Clean/repair in progress..." );
        res = seltabRouteFbValidation( inst, True );
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Selection table route feedback validation: %s problems cleaned", res?"no":"some" );
        if( res == False )
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Please restart Rocrail server." );
      }

    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "No clean/repair options enabled." );
  }

  TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Extended checks are work in progress. Do not rely on them. END" );
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
          if( StrOp.startsWith( wRoute.getid(stNode), "autogen-" )) {
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

/* remove autogen- routeids in tk|sw|sg|fb-list */
static int _cleanupRouteids( iONode tracklist ) {
  int modifications = 0;
  int numModifiedTracks = 0;
  int trackListSize = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__fCC: Checking [%08.8X]", tracklist );
  if( tracklist != NULL ) {
    trackListSize = NodeOp.getChildCnt( tracklist );
  }

  if( trackListSize > 0 ) {
    iONode tracknode;
    const char* listType = NodeOp.getName( NodeOp.getChild(tracklist, 0));
    int i = 0;
    Boolean thisTrackChanged ;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_cleanupRouteids: Checking %d %s nodes", trackListSize, listType );
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
            /* check if id starts with autogen- */
            if( ( StrOp.len(token) > 0 ) && ( ! StrOp.startsWith( token, "autogen-") ) ) {
              /* not "autogen-" so append to new list*/
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
  int blockListSize = NodeOp.getChildCnt( blocklist );

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
  int blockListSize = NodeOp.getChildCnt( blocklist );

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
    modifications += _cleanupRouteids( wPlan.gettklist(data->plan) );
    modifications += _cleanupRouteids( wPlan.getswlist(data->plan) );
    modifications += _cleanupRouteids( wPlan.getsglist(data->plan) );
    modifications += _cleanupRouteids( wPlan.getfblist(data->plan) );
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
  if( ! anaOpt ) {
    /* no analyzer options in ini -> create a node */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "_inst: ceate node %s", wAnaOpt.name() );
    iONode anaOpt = NodeOp.inst( wAnaOpt.name(), aoIni, ELEMENT_NODE );
    if( ! anaOpt ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "_inst: create node %s failed.", wAnaOpt.name() );
    }
    else {
      NodeOp.addChild( aoIni, anaOpt );

      /* initialize with default options */
      wAnaOpt.setsetRouteId( anaOpt, wAnaOpt.issetRouteId( anaOpt ) ) ;
      wAnaOpt.setsetBlockId( anaOpt, wAnaOpt.issetBlockId( anaOpt ) ) ;
      wAnaOpt.setaddSignalBlockAssignment( anaOpt, wAnaOpt.isaddSignalBlockAssignment( anaOpt ) ) ;
      wAnaOpt.setaddFeedbackBlockAssignment( anaOpt, wAnaOpt.isaddFeedbackBlockAssignment( anaOpt ) ) ;

      wAnaOpt.setcleanRouteId( anaOpt, wAnaOpt.iscleanRouteId( anaOpt ) ) ;
      wAnaOpt.setresetBlockId( anaOpt, wAnaOpt.isresetBlockId( anaOpt ) ) ;
      wAnaOpt.setresetSignalBlockAssignment( anaOpt, wAnaOpt.isresetSignalBlockAssignment( anaOpt ) ) ;
      wAnaOpt.setresetFeedbackBlockAssignment( anaOpt, wAnaOpt.isresetFeedbackBlockAssignment( anaOpt ) ) ;

      /* extended checks */
      wAnaOpt.setblockFeedbackActionCheck(      anaOpt, wAnaOpt.isblockFeedbackActionCheck(      anaOpt ) ) ;
      wAnaOpt.setblockFeedbackActionCheckClean( anaOpt, wAnaOpt.isblockFeedbackActionCheckClean( anaOpt ) ) ;
      wAnaOpt.setblockRouteFbValidation(        anaOpt, wAnaOpt.isblockRouteFbValidation(        anaOpt ) ) ;
      wAnaOpt.setblockRouteFbValidationClean(   anaOpt, wAnaOpt.isblockRouteFbValidationClean(   anaOpt ) ) ;
      wAnaOpt.setseltabRouteFbValidation(       anaOpt, wAnaOpt.isseltabRouteFbValidation(       anaOpt ) ) ;
      wAnaOpt.setseltabRouteFbValidationClean(  anaOpt, wAnaOpt.isseltabRouteFbValidationClean(  anaOpt ) ) ;
    }
  }

  /* get values for analyzer options from ini */
  data->setRouteId                    = wAnaOpt.issetRouteId(                    anaOpt ) ;
  data->setBlockId                    = wAnaOpt.issetBlockId(                    anaOpt ) ;
  data->addSignalBlockAssignment      = wAnaOpt.isaddSignalBlockAssignment(      anaOpt ) ;
  data->addFeedbackBlockAssignment    = wAnaOpt.isaddFeedbackBlockAssignment(    anaOpt ) ;

  data->cleanRouteId                  = wAnaOpt.iscleanRouteId(                  anaOpt ) ;
  data->resetBlockId                  = wAnaOpt.isresetBlockId(                  anaOpt ) ;
  data->resetSignalBlockAssignment    = wAnaOpt.isresetSignalBlockAssignment(    anaOpt ) ;
  data->resetFeedbackBlockAssignment  = wAnaOpt.isresetFeedbackBlockAssignment(  anaOpt ) ;

  /* extended check ooptions */
  data->blockFeedbackActionCheck      = wAnaOpt.isblockFeedbackActionCheck(      anaOpt ) ;
  data->blockFeedbackActionCheckClean = wAnaOpt.isblockFeedbackActionCheckClean( anaOpt ) ;
  data->blockRouteFbValidation        = wAnaOpt.isblockRouteFbValidation(        anaOpt ) ;
  data->blockRouteFbValidationClean   = wAnaOpt.isblockRouteFbValidationClean(   anaOpt ) ;
  data->seltabRouteFbValidation       = wAnaOpt.isseltabRouteFbValidation(       anaOpt ) ;
  data->seltabRouteFbValidationClean  = wAnaOpt.isseltabRouteFbValidationClean(  anaOpt ) ;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Analyse Options: sRtId[%d] sBlId[%d] aSgBl[%d] aFbBl[%d] clRtId[%d] reBlId[%d] reSgBl[%d] reFbBl[%d]",
      data->setRouteId, data->setBlockId, data->addSignalBlockAssignment, data->addFeedbackBlockAssignment, 
      data->cleanRouteId, data->resetBlockId, data->resetSignalBlockAssignment, data->resetFeedbackBlockAssignment );

  instCnt++;
  return __Analyse;
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/analyse.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/

