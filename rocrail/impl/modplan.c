/** ------------------------------------------------------------
  * Module: RocRail
  * Object: ModPlan
  * ------------------------------------------------------------
  */

#include "rocrail/impl/modplan_impl.h"
#include "rocrail/public/clntcon.h"
#include "rocrail/public/app.h"
#include "rocrail/public/module.h"

#include "rocs/public/mem.h"
#include "rocs/public/doc.h"

#include "rocrail/wrapper/public/Global.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Module.h"
#include "rocrail/wrapper/public/ModuleConnection.h"
#include "rocrail/wrapper/public/ModPlan.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/RouteList.h"

#include "rocrail/wrapper/public/BlockList.h"
#include "rocrail/wrapper/public/TrackList.h"
#include "rocrail/wrapper/public/LocList.h"
#include "rocrail/wrapper/public/CarList.h"
#include "rocrail/wrapper/public/TurntableList.h"
#include "rocrail/wrapper/public/SelTabList.h"
#include "rocrail/wrapper/public/SwitchList.h"
#include "rocrail/wrapper/public/OutputList.h"
#include "rocrail/wrapper/public/SignalList.h"
#include "rocrail/wrapper/public/FeedbackList.h"
#include "rocrail/wrapper/public/TextList.h"
#include "rocrail/wrapper/public/LocationList.h"
#include "rocrail/wrapper/public/ScheduleList.h"
#include "rocrail/wrapper/public/LinkList.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/ActionList.h"
#include "rocrail/wrapper/public/WaybillList.h"
#include "rocrail/wrapper/public/BoosterList.h"
#include "rocrail/wrapper/public/OperatorList.h"

#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Track.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Text.h"
#include "rocrail/wrapper/public/Item.h"

#include "rocrail/wrapper/public/ZLevel.h"
#include "rocrail/wrapper/public/ModelCmd.h"


static int instCnt = 0;
static iOModPlan __modplan = NULL;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOModPlanData data = Data(inst);
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
  iOModPlanData data = Data(inst);
  return NodeOp.base.toString( data->modplan );
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

/** ----- OModPlan ----- */
static iONode _parsePlan( const char* filename ) {

  iOFile moduleFile = FileOp.inst( filename, True );

  if(moduleFile == NULL) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "file [%s] not found, try to open it in the working directory...", filename );
    moduleFile = FileOp.inst( FileOp.ripPath(filename), True );
  }

  if( moduleFile != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "parse file: %s", filename );
    {
      iODoc planDoc = NULL;
      char* planXml = allocMem( FileOp.size( moduleFile ) + 1 );
      FileOp.read( moduleFile, planXml, FileOp.size( moduleFile ) );
      FileOp.close( moduleFile );
      FileOp.base.del( moduleFile );
      planDoc = DocOp.parse( planXml );
      freeMem( planXml );
      if( planDoc != NULL ) {
        iONode root = DocOp.getRootNode( planDoc );
        DocOp.base.del( planDoc );
        if( root != NULL ) {
          /**/
          return root;
        }
      }
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unable to open file: %s", filename );
  }

  return NULL;
}


/**
 * Resolve module routes to complete routes.
 *
 */
static void __createRoute( iOModPlanData data, iONode model, iOList routeList, iONode toRoute, const char* modid ) {
  /* create the new route, merge all crossing blocks and use the properties of the last route. */
  iONode fromRoute = (iONode)ListOp.get( routeList, 0 );

  iONode newRoute = (iONode)NodeOp.base.clone(toRoute);
  int r = 0;
  char* routeID = NULL;
  char* bkc = NULL;

  NodeOp.setStr( toRoute, "tomodid", modid );
  wRoute.setbka( newRoute, wRoute.getbka( fromRoute ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "creating the new route from[%s] to[%s]",
      wRoute.getbka( newRoute ), wRoute.getbkb( newRoute ) );

  routeID = StrOp.fmt( "%s-%s", wRoute.getbka(newRoute), wRoute.getbkb(newRoute) );
  wRoute.setid( newRoute, routeID );
  StrOp.free(routeID);

  /* merge all crossing blocks: */
  /* merge all commands */
  for( r = 0; r < ListOp.size(routeList); r++ ) {
    iONode routeseg = (iONode)ListOp.get( routeList, r );

    /* add to map to lookup the new route ID with the segment route ID */
    MapOp.put( data->mod2routeIdMap, wRoute.getid(routeseg), (obj)wRoute.getid( newRoute ) );

    if( wRoute.getbkc(routeseg) != NULL &&  StrOp.len( wRoute.getbkc(routeseg) ) > 0 ) {
      if( bkc == NULL )
        bkc = StrOp.fmt( "%s", wRoute.getbkc(routeseg) );
      else
        bkc = StrOp.fmt( "%s,%s", bkc, wRoute.getbkc(routeseg) );
    }
    if( routeseg != toRoute ) {
      iONode swcmd  = wRoute.getswcmd(routeseg);
      iONode acctrl = wRoute.getactionctrl(routeseg);
      while(swcmd != NULL) {
        NodeOp.addChild( newRoute, (iONode)NodeOp.base.clone(swcmd) );
        swcmd = wRoute.nextswcmd(routeseg, swcmd);
      }
      while(acctrl != NULL) {
        NodeOp.addChild( newRoute, (iONode)NodeOp.base.clone(acctrl) );
        acctrl = wRoute.nextactionctrl(routeseg, acctrl);
      }
    }
  }
  if( bkc != NULL )
    wRoute.setbkc( newRoute, bkc );

  /* the route is generated so remove the module ID */
  wRoute.setmodid( newRoute, wRoute.modid_auto_gen );


  /* add to the list: */
  {
    iONode modellist = NodeOp.findNode( model, wRouteList.name() );
    NodeOp.addChild( modellist, newRoute );
  }

}


static iOList __cloneRouteList( iOList routeList ) {
  iOList list = ListOp.inst();
  int i = 0;
  for(i = 0; i< ListOp.size(routeList); i++) {
    ListOp.add( list, ListOp.get(routeList,i) );
  }
  return list;
}

static iONode __findConnection( iOModPlanData data, iONode model, iOList routeList, const char* to, const char* modid, int* routecnt );

/**
 *
 *
 */
static const char* Left_points [] = { "point-nw", "point-en", "point-se", "point-ws" };
static const char* Right_points[] = { "point-ne", "point-es", "point-sw", "point-wn" };

static Boolean __checkPointMatch( const char* point_a, const char* point_b ) {
  int i = 0;
  for( i = 0; i < 4; i++ ) {
    if( StrOp.equals( Left_points[i], point_a ) ) {
      int n = 0;
      for( n = 0; n < 4; n++ ) {
        if( StrOp.equals( Right_points[n], point_b ) ) {
          return True;
        }
      }
    }
    else if( StrOp.equals( Right_points[i], point_a ) ) {
      int n = 0;
      for( n = 0; n < 4; n++ ) {
        if( StrOp.equals( Left_points[n], point_b ) ) {
          return True;
        }
      }
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "[%s] and [%s] do not match", point_a, point_b );
  return False;
}

static iONode __findRouteFromPoint( iOModPlanData data, iONode model, iOList routeList, const char* modid, const char* point, const char* to, int* routecnt ) {
  int routesCnt = ListOp.size( data->unresolvedRouteList );
  int i = 0;
  iONode toRoute = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "find route from point=[%s] modid=[%s] to=[%s]", point, modid, to );

  for( i = 0; i < routesCnt; i++ ) {
    iONode route = (iONode)ListOp.get( data->unresolvedRouteList, i );
    const char* blockFrom = wRoute.getbka(route);
    if( StrOp.equals( modid, wRoute.getmodid(route) ) && StrOp.startsWith( blockFrom, point ) ) {
      /* match; */

      if( StrOp.len(blockFrom) == 8 && StrOp.len(to) == 8 ) {
        if( !__checkPointMatch( to, blockFrom ) ) {
          continue;
        }
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "double track connection from=[%s] to=[%s]", to, blockFrom );
      }

      *routecnt += 1;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MATCHING route[%d] from[%s]-to[%s]",
          *routecnt, wRoute.getbka(route), wRoute.getbkb(route) );

      if( !StrOp.startsWith( wRoute.getbkb(route), "point-" ) ) {
        /* assume end of route is found */
        __createRoute(data, model, routeList, route, modid );
      }
      else {
        iOList clonedRouteList = __cloneRouteList(routeList);
        ListOp.add( clonedRouteList, (obj)route );
        /* find recursive the next match; clone routeList? */
        __findConnection( data, model, clonedRouteList, wRoute.getbkb( route ), wRoute.getmodid( route ), routecnt);
        ListOp.base.del(clonedRouteList);
      }


      if( toRoute == NULL ) {
        /* return only the first found */
        toRoute = route;
      }
    }
  }
  return toRoute;
}

static iONode __findConnection( iOModPlanData data, iONode model, iOList routeList, const char* to, const char* modid, int* routecnt ) {
  int routesCnt = ListOp.size( data->unresolvedRouteList );
  int modconCnt = ListOp.size( data->modconList );
  int i = 0;
  int n = 0;
  iONode routeTo = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "find a connection to=[%s] modid=[%s] in %d connections", to, modid, modconCnt );


  for( i = 0; i < modconCnt; i++ ) {
    iONode modcon = (iONode)ListOp.get( data->modconList, i );
    if( StrOp.equals( modid, wModuleConnection.getid(modcon) ) &&
        StrOp.startsWith( to, wModuleConnection.getside(modcon) ) )
    {
      const char* modToid = wModuleConnection.getmodid(modcon);

      /* find a module connection from modToid which connects to modid */
      for( n = 0; n < modconCnt; n++ ) {
        iONode modTocon = (iONode)ListOp.get( data->modconList, n );
        if( StrOp.equals( modToid, wModuleConnection.getid(modTocon) ) ) {
          if( StrOp.equals( modid, wModuleConnection.getmodid(modTocon) ) ) {
            /* modid is connected to the north side of modToid */
            /* look up a route from modToid which starts with "point-*" */
            /* TODO: regard second character of the compass point */
            routeTo = __findRouteFromPoint( data, model, routeList, modToid, wModuleConnection.getside(modTocon), to, routecnt );
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "connection found: from[%s](%s) - to[%s](%s)",
                wRoute.getbka(routeTo), modid, wRoute.getbkb(routeTo), modToid );
            return routeTo;
          }
        }
      }

    }
  }

  return routeTo;

}


static void __resolveRoutes4Connections( iOModPlanData data, iONode model ) {
  int routesCnt = ListOp.size( data->unresolvedRouteList );
  int modconCnt = ListOp.size( data->modconList );
  int i = 0;
  int n = 0;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "Resolving [%d] routes with [%d] module connections...", routesCnt, modconCnt );

  /* resolve */

  for( i = 0; i < routesCnt; i++ ) {
    iONode route = (iONode)ListOp.get( data->unresolvedRouteList, i );
    if( !StrOp.startsWith( wRoute.getbka( route ), "point-" ) ) {
      /* real starting point */
      const char* to = wRoute.getbkb( route );
      const char* modid = wRoute.getmodid( route );
      int routecnt = 0;
      iOList routeList = ListOp.inst();
      ListOp.add( routeList, (obj)route );

      /* search a connection */
      iONode toRoute = __findConnection( data, model, routeList, to, modid, &routecnt );

      if( toRoute != NULL ){
        char key[64];
        char* val;
        iOList list = NULL;

        /* event list for first part of routes */
        StrOp.fmtb( key, "%s-%s", modid, to );
        list = (iOList)MapOp.get(data->fbeventMap, key);
        if( list == NULL ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "creating list for [%s]", key );
          list = ListOp.inst();
          MapOp.put(data->fbeventMap, key, (obj)list);
        }
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "Adding [%s] to fbeventMapKey [%s]", wRoute.getbkb(toRoute), key );
        ListOp.add(list, (obj)wRoute.getbkb(toRoute));

        /* TODO: event list for last part of routes */
        StrOp.fmtb( key, "%s-%s", NodeOp.getStr(toRoute, "tomodid", "?"), wRoute.getbka(toRoute) );
        list = (iOList)MapOp.get(data->fbeventMap, key);
        if( list == NULL ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "creating list for [%s]", key );
          list = ListOp.inst();
          MapOp.put(data->fbeventMap, key, (obj)list);
        }
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "Adding [%s] to fbeventMapKey [%s]", wRoute.getbka(route), key );
        ListOp.add(list, (obj)wRoute.getbka(route));



        /*
         * make a mapping for route activation
         * key = "bka-bkb(toRoute)"
         * val = "bka-point-x"
         */
        StrOp.fmtb( key, "%s-%s", wRoute.getbka( route ), wRoute.getbkb(toRoute) );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "add %d aliases for route [%s] to routeIdMap", ListOp.size(routeList), key );
        MapOp.put(data->routeIdMap, key, (obj)routeList);

      }
      else {
        ListOp.base.del(routeList);
      }

    }
  }

}


/**
 * Return the unresolved routeID by generated routeID.
 */
static iOList _getModuleRouteIDs(iOModPlan inst, const char* routeid) {
  iOModPlanData data = Data(inst);
  return (iOList)MapOp.get(data->routeIdMap, routeid);
}


/**
 * Return the resolved routeID by module routeID.
 */
static const char* _getResolvedRouteID(iOModPlan inst, const char* routeid) {
  iOModPlanData data = Data(inst);
  return (const char*)MapOp.get(data->mod2routeIdMap, routeid);
}


static iOList _getBlocks4Point(iOModPlan inst, const char* modid, const char* point) {
  iOModPlanData data = Data(inst);
  char key[64];
  StrOp.fmtb( key, "%s-%s", modid, point );
  return (iOList)MapOp.get(data->fbeventMap, key);
}


static iONode _getEvent4Block(iOModPlan inst, const char* modid, iONode block, const char* comingfrom, const char* eventid) {
  if( inst != NULL || __modplan != NULL ) {
    iOModPlanData data = Data(inst!=NULL?inst:__modplan);
    char key[64];

    if( modid == NULL ) {
      modid = (const char*)MapOp.get( data->blockMap, wBlock.getid(block));
    }

    if( modid == NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "no map entry found for block [%s]", wBlock.getid(block) );
      return NULL;
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "searching event[%s] from block[%s] to block[%s-%s]...", eventid, comingfrom, modid, wBlock.getid(block) );

    /* iterate all fbevents */
    /* <fbevent id="1f2" action="enter" from="point-e" endpuls="false" ghostdetection="false"/> */
    iONode fbevent = wBlock.getfbevent(block);
    while( fbevent != NULL ) {
      const char* from = wFeedbackEvent.getfrom(fbevent);
      const char* fbid = wFeedbackEvent.getid(fbevent);
      iOList blockids = NULL;

      StrOp.fmtb( key, "%s-%s", modid, from );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get list for [%s]...", key );
      blockids = (iOList)MapOp.get(data->fbeventMap, key);

      if( blockids != NULL && ListOp.size(blockids) > 0 ) {
        int i = 0;
        for( i = 0; i < ListOp.size(blockids); i++ ) {
          const char* blockid = (const char*)ListOp.get( blockids, i );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
              "eventmap[%s]=[%s], fbid=%s, comingfrom=%s, eventid=%s", key, blockid, fbid, comingfrom, eventid );
          if( StrOp.equals(blockid, comingfrom) && StrOp.equals(fbid, eventid) ) {
            return fbevent;
          }
        }
      }
      fbevent = wBlock.nextfbevent(block, fbevent);
    };
  }
  return NULL;
}




static void __resolveRoutes( iOModPlanData data, iONode model, iONode module, iONode moduleRoot, int level ) {
  iONode connection = NULL;
  iONode routes     = wPlan.getstlist( moduleRoot );
  iONode route      = NULL;
  int    routesCnt  = 0;

  if( routes == NULL || wRouteList.getst( routes ) == NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Module [%s] has no routes defined.",
        wModule.gettitle( module ) );
    return;
  }

  routesCnt = NodeOp.getChildCnt( routes );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Module [%s] has [%d] routes defined.",
      wModule.gettitle( module ), routesCnt );

  route = wRouteList.getst( routes );
  while( route != NULL ) {
    wRoute.setmodid( route, wModule.getid(module) );
    wRoute.setz( route, level );
    if( StrOp.startsWith( wRoute.getbka(route), "point-" ) || StrOp.startsWith( wRoute.getbkb(route), "point-" ) ) {
      /* unresolved internal route found; add to the unresolved route list */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Unresolved internal route from [%s] to [%s]",
          wRoute.getbka(route), wRoute.getbkb(route) );
      ListOp.add( data->unresolvedRouteList, NodeOp.base.clone(route) );
    }
    else {
      /* complete internal route; add to the normal route list */
      iONode stlist = wPlan.getstlist( model );
      NodeOp.addChild( stlist, (iONode)NodeOp.base.clone(route) );
    }

    route = wRouteList.nextst( routes, route );
  };

  connection = wModule.getconnection( module );
  while( connection != NULL ) {
    wModuleConnection.setid( connection, wModule.getid(module) );
    ListOp.add( data->modconList, NodeOp.base.clone(connection) );
    connection = wModule.nextconnection( module, connection );
  };

}



static void __mergeList( const char* listname, iONode model, iONode module, int level, int r, int cx, int cy, Boolean broadcast ) {
  iONode modellist = NodeOp.findNode( model, listname );
  iONode list = NodeOp.findNode( module, listname );

  if( modellist == NULL ) {
    modellist = NodeOp.inst( listname, NULL, ELEMENT_NODE );
    NodeOp.addChild(model, modellist);
  }

  if( list != NULL ) {
    int i = 0;
    int cnt = NodeOp.getChildCnt( list );
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( list, i );
      if( level != -1 )
        wItem.setz( child, level );

      wItem.setprev_x( child, wItem.getx(child) );
      wItem.setprev_y( child, wItem.gety(child) );
      if( wItem.getori(child) != NULL )
        wItem.setprev_ori( child, wItem.getori(child) );

      if( r == 180 && cx > 0 && cy > 0 )
        ModuleOp.rotate180( child, cx,cy );
      else if( r == 90 && cx > 0 && cy > 0 )
        ModuleOp.rotate90( child, cx,cy );
      else if( r == 270 && cx > 0 && cy > 0 )
        ModuleOp.rotate270( child, cx,cy );

      NodeOp.addChild( modellist, (iONode)NodeOp.base.clone(child) );

      if( broadcast ) {
        iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
        wModelCmd.setcmd( cmd, wModelCmd.merge );
        iONode item = (iONode)NodeOp.base.clone( child );
        NodeOp.addChild( cmd, item );
        ClntConOp.broadcastEvent( AppOp.getClntCon(), cmd );
      }
    }
  }
}

/* *
  all "z" attributes will be set to this number before merge
  lists: tklist, bklist, swlist, sglist, colist, fblist, ttlist, txlist
*/
static iONode __mergeModule( iOModPlanData data, iONode model, iONode module, int level, Boolean informClients ) {
  iONode moduleRoot = _parsePlan( wModule.getfilename(module) );
  if( moduleRoot != NULL ) {
    int cx = 0;
    int cy = 0;
    int r = 0;
    /* add zlevel */
    iONode zlevel = NodeOp.inst( wZLevel.name(), model, ELEMENT_NODE );
    wZLevel.settitle(zlevel, wModule.gettitle( module ) );
    wZLevel.setmodid(zlevel, wModule.getid( module ) );
    wZLevel.setz(zlevel, level );
    wZLevel.setmodviewx( zlevel, wModule.getx(module));
    wZLevel.setmodviewy( zlevel, wModule.gety(module));
    NodeOp.addChild( model, zlevel );

    r  = wModule.getrotation(module);
    cx = wModule.getcx(module);
    cy = wModule.getcy(module);
    if( cx == 0 || cy == 0 ) {
      ModuleOp.getRotationArea( moduleRoot, &cx, &cy );
      /* getRotationArea returns mins and maxs; adjust cx and cy */
      cx++;
      cy++;
      wModule.setcx(module, cx);
      wModule.setcy(module, cy);
    }

    if( r == 0 || r == 180 ) {
      wZLevel.setmodviewcx( zlevel, cx);
      wZLevel.setmodviewcy( zlevel, cy);
    }
    else {
      wZLevel.setmodviewcx( zlevel, cy);
      wZLevel.setmodviewcy( zlevel, cx);
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Module Plan [%s] parsed for level [%d][%s]",
        wModule.gettitle( module ), level, wModule.getid( module ) );

    /* fill the blockmap */
    {
      iONode list = NodeOp.findNode( moduleRoot, wBlockList.name() );
      if( list != NULL ) {
        const char* modid = wModule.getid( module );
        iONode block = wBlockList.getbk(list);
        while(block != NULL) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "put [%s]-[%s] in the blockmap",
              wBlock.getid(block), modid );
          MapOp.put( data->blockMap, wBlock.getid(block), (obj)modid);
          block = wBlockList.nextbk(list, block);
        }
      }
    }

    __mergeList(wTrackList.name()    , model, moduleRoot, level, r, cx, cy, informClients);
    __mergeList(wBlockList.name()    , model, moduleRoot, level, r, cx, cy, informClients);
    __mergeList(wSwitchList.name()   , model, moduleRoot, level, r, cx, cy, informClients);
    __mergeList(wSignalList.name()   , model, moduleRoot, level, r, cx, cy, informClients);
    __mergeList(wOutputList.name()   , model, moduleRoot, level, r, cx, cy, informClients);
    __mergeList(wFeedbackList.name() , model, moduleRoot, level, r, cx, cy, informClients);
    __mergeList(wTextList.name()     , model, moduleRoot, level, r, cx, cy, informClients);
    __mergeList(wTurntableList.name(), model, moduleRoot, level, r, cx, cy, informClients);
    __mergeList(wSelTabList.name()   , model, moduleRoot, level, r, cx, cy, informClients);
    __mergeList(wActionList.name()   , model, moduleRoot, level, r, cx, cy, informClients);
    __mergeList(wLocationList.name() , model, moduleRoot, level, r, cx, cy, informClients);
    __mergeList(wBoosterList.name()  , model, moduleRoot, level, r, cx, cy, informClients);

    __resolveRoutes( data, model, module, moduleRoot, level );

    NodeOp.base.del( moduleRoot );

    return zlevel;

  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Unable to parse Module Plan [%s]", wModule.getfilename( module ) );
    return NULL;
  }

}

/*
  this writes down the logic module connections which is dependent
  of the arrangement
  lists: stlist, linklist, locationlist

  optional schedules which are also dependent of the arrangement
  lists: sclist
*/
static Boolean __mergeRoutes( iOModPlanData data, iONode model, const char* fname, iONode modplan ) {
  iONode root = ModPlanOp.parsePlan( fname );
  if( root != NULL ) {

    __mergeList(wRouteList.name()   , model, root, -1, 0,0,0, False);
    __mergeList(wLinkList.name()    , model, root, -1, 0,0,0, False);
    __mergeList(wLocationList.name(), model, root, -1, 0,0,0, False);
    __mergeList(wScheduleList.name(), model, root, -1, 0,0,0, False);
    __mergeList(wWaybillList.name() , model, root, -1, 0,0,0, False);

    NodeOp.base.del( root );

  }

  if( wModPlan.ismodroutes(modplan) ) {
    __resolveRoutes4Connections(data, model);
  }
  else {
    int routesCnt = ListOp.size( data->unresolvedRouteList );
    int modconCnt = ListOp.size( data->modconList );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "**NOT** Resolving [%d] routes with [%d] module connections...", routesCnt, modconCnt );
  }

  return True;
}

/*
  all locomotives
  lists: lclist
*/
static Boolean __mergeLocs( iONode model, const char* fname ) {
  iONode root = ModPlanOp.parsePlan( fname );
  if( root != NULL ) {

    __mergeList(wLocList.name()  , model, root, -1, 0,0,0, False);
    __mergeList(wCarList.name()  , model, root, -1, 0,0,0, False);
    __mergeList(wOperatorList.name()  , model, root, -1, 0,0,0, False);

    NodeOp.base.del( root );

  }

  return True;
}



/**
 * merging all parts together into one master plan
 */
static iONode __parseModPlan( iOModPlan inst ) {
  iOModPlanData data = Data(inst);

  const char* locs   = wModPlan.getlocs  ( data->modplan );
  const char* routes = wModPlan.getroutes( data->modplan );
  iONode model = NodeOp.inst( wPlan.name(), NULL, ELEMENT_NODE );
  wPlan.setmodplan( model, True );
  iONode mod = wModPlan.getmodule( data->modplan );

  wPlan.settitle( model, wModPlan.gettitle( data->modplan ) );

  /* add all lists: */
  const char*  dbkey = wBlockList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wTurntableList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wSelTabList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wTrackList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wFeedbackList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wLocList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wCarList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wRouteList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wSwitchList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wSignalList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wOutputList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wTextList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wActionList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wLocationList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );
  dbkey = wBoosterList.name();
  NodeOp.addChild( model, NodeOp.inst( dbkey, model, ELEMENT_NODE ) );


  int level = 0;
  while( mod != NULL ) {
    if( __mergeModule( data, model, mod, level, False ) != NULL ) {
      /* add module to the list for modify */
      ListOp.add( data->modList, (obj)mod );
      level++;
    }
    mod = wModPlan.nextmodule( data->modplan, mod );
  };

  __mergeRoutes( data, model, routes, data->modplan );
  __mergeLocs  ( model, locs );

  return model;
}






static iONode _getModPlan( iOModPlan inst ) {
  iOModPlanData data = Data(inst);
  return data->modplan;
}


static iONode _parse( iOModPlan inst ) {
  iOModPlanData data = Data(inst);
  data->model = __parseModPlan( inst );
  return data->model;
}


static void _resolveRoutes( iOModPlan inst, iONode model, iONode module, iONode moduleRoot, int level ) {
  iOModPlanData data = Data(inst);

  iONode connection = NULL;
  iONode routes     = wPlan.getstlist( moduleRoot );
  iONode route      = NULL;
  int    routesCnt  = 0;

  if( routes == NULL || wRouteList.getst( routes ) == NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Module [%s] has no routes defined.",
        wModule.gettitle( module ) );
    return;
  }

  routesCnt = NodeOp.getChildCnt( routes );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Module [%s] has [%d] routes defined.",
      wModule.gettitle( module ), routesCnt );

  route = wRouteList.getst( routes );
  while( route != NULL ) {
    wRoute.setmodid( route, wModule.getid(module) );
    wRoute.setz( route, level );
    if( StrOp.startsWith( wRoute.getbka(route), "point-" ) || StrOp.startsWith( wRoute.getbkb(route), "point-" ) ) {
      /* unresolved internal route found; add to the unresolved route list */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Unresolved internal route from [%s] to [%s]",
          wRoute.getbka(route), wRoute.getbkb(route) );
      ListOp.add( data->unresolvedRouteList, NodeOp.base.clone(route) );
    }
    else {
      /* complete internal route; add to the normal route list */
      iONode stlist = wPlan.getstlist( model );
      NodeOp.addChild( stlist, (iONode)NodeOp.base.clone(route) );
    }

    route = wRouteList.nextst( routes, route );
  };

  connection = wModule.getconnection( module );
  while( connection != NULL ) {
    wModuleConnection.setid( connection, wModule.getid(module) );
    ListOp.add( data->modconList, NodeOp.base.clone(connection) );
    connection = wModule.nextconnection( module, connection );
  };

}


static iONode _getModule( iOModPlan inst, const char* modid ) {
  iOModPlanData data = Data(inst);
  iONode module = (iONode)ListOp.first(data->modList);
  while( module != NULL ) {
    if( StrOp.equals( modid, wModule.getid(module) ) ) {
      return module;
    }
    module = (iONode)ListOp.next(data->modList);
  };
  return NULL;
}

static iONode _setModule( iOModPlan inst, iONode module ) {
  iOModPlanData data = Data(inst);
  iONode planmodule = NULL;

  /* merge attribute values */
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "merging module [%s] attributes...", wModule.getid(module) );

  planmodule = ModPlanOp.getModule( inst, wModule.getid(module) );
  if( planmodule == NULL ) {
    planmodule = ModPlanOp.getModule( inst, wItem.getprev_id(module) );
    if( planmodule != NULL ) {
      /* ID did change... */
      wModule.setid( planmodule, wModule.getid(module) );
    }
  }

  if( planmodule == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "no module found with id [%s] or [%s]", wModule.getid(module), wItem.getprev_id(module) );
    return;
  }

  wModule.settitle( planmodule, wModule.gettitle(module) );
  wModule.setfilename( planmodule, wModule.getfilename(module) );

  wModule.setcx( planmodule, wModule.getcx(module) );
  wModule.setcy( planmodule, wModule.getcy(module) );

  /* move */
  if( wModule.getx(module) != wModule.getx(planmodule) || wModule.gety(module) != wModule.gety(planmodule) ) {
    wModule.setx( planmodule, wModule.getx(module) );
    wModule.sety( planmodule, wModule.gety(module) );
    wModule.setcmd( module, wModule.cmd_move );
    return (iONode)NodeOp.base.clone(module);
  }

  /* rotate */
  if( wModule.getrotation(planmodule) != wModule.getrotation(module) ) {
    wModule.setrotation( planmodule, wModule.getrotation(module) );
    switch(wModule.getrotation(module)) {
      case 0:
        wModule.setcmd( module, wModule.cmd_north );
        break;
      case 90:
        wModule.setcmd( module, wModule.cmd_east );
        break;
      case 180:
        wModule.setcmd( module, wModule.cmd_south );
        break;
      case 270:
        wModule.setcmd( module, wModule.cmd_west );
        break;
    }
    return (iONode)NodeOp.base.clone(module);
  }

  /* connections */
  {
    iONode conn = NULL;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "merging module [%s] connection...", wModule.getid(module) );

    int cnt = NodeOp.getChildCnt( planmodule );
    while( cnt > 0 ) {
      iONode child = NodeOp.getChild( planmodule, 0 );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "remove connection for side %s...", wModuleConnection.getside(child) );
      NodeOp.removeChild( planmodule, child );
      cnt = NodeOp.getChildCnt( planmodule );
      NodeOp.base.del(child);
    }

    conn = wModule.getconnection(module);
    while( conn != NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "add connection for side %s...", wModuleConnection.getside(conn) );
      NodeOp.addChild(planmodule, (iONode)NodeOp.base.clone(conn));
      conn = wModule.nextconnection(module, conn );
    };

    wModule.setcmd( module, wModule.cmd_connect );
    return (iONode)NodeOp.base.clone(module);
  }

}

static void __copyLevel( iOModPlan inst, iONode model, int level, const char* dbname ) {
  iOModPlanData data = Data(inst);
  iONode list = NodeOp.findNode( data->model, dbname );
  if( list != NULL ) {
    iONode db = NodeOp.inst( dbname, model, ELEMENT_NODE );
    int childs = NodeOp.getChildCnt(list);
    int i = 0;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "Adding %d childs to %s for level %d", childs, dbname, level );
    NodeOp.addChild( model, db );
    for( i = 0; i < childs; i++ ) {
      iONode child = NodeOp.getChild( list, i );
      if( wItem.getz(child) == level ) {
        iONode clone = (iONode)NodeOp.base.clone(child);
        wItem.setx( clone, wItem.getprev_x(clone) );
        wItem.sety( clone, wItem.getprev_y(clone) );
        wItem.setori( clone, wItem.getprev_ori(clone) );
        NodeOp.addChild( db, clone );
      }
    }
  }
}


static void __copyResolvedRoutes( iOModPlan inst, iONode model, const char* modid ) {
  iOModPlanData data = Data(inst);
  iONode list = NodeOp.findNode( data->model, wRouteList.name() );
  if( list != NULL ) {
    iONode db = wPlan.getstlist(model);
    int childs = NodeOp.getChildCnt(list);
    int i = 0;
    if( db == NULL ) {
      db = NodeOp.inst( wRouteList.name(), model, ELEMENT_NODE );
      NodeOp.addChild( model, db );
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "Adding routes to module [%s]", modid );
    for( i = 0; i < childs; i++ ) {
      iONode child = NodeOp.getChild( list, i );
      if( StrOp.equals( wRoute.getmodid(child), modid ) ) {
        iONode clone = (iONode)NodeOp.base.clone(child);
        NodeOp.addChild( db, clone );
      }
    }
  }
}


static void __copyUnresolvedRoutes( iOModPlan inst, iONode model, int level ) {
  iOModPlanData data = Data(inst);
  iOList list = data->unresolvedRouteList;
  if( list != NULL ) {
    iONode db = wPlan.getstlist(model);
    int childs = ListOp.size(list);
    int i = 0;
    if( db == NULL ) {
      db = NodeOp.inst( wRouteList.name(), model, ELEMENT_NODE );
      NodeOp.addChild( model, db );
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "Adding %d childs to %s for level %d", childs, wRouteList.name(), level );
    for( i = 0; i < childs; i++ ) {
      iONode child = (iONode)ListOp.get( list, i );
      if( wItem.getz(child) == level ) {
        iONode clone = (iONode)NodeOp.base.clone(child);
        NodeOp.addChild( db, clone );
      }
    }
  }
}


static void __updateZLevel( iOModPlan inst, iONode module, int level ) {
  iOModPlanData data = Data(inst);
  iONode zlevel = wPlan.getzlevel( data->model );
  while( zlevel != NULL ) {
    if( wZLevel.getz( zlevel ) == level ) {
      int rotation = wModule.getrotation(module);
      wZLevel.setmodviewx( zlevel, wModule.getx( module ) );
      wZLevel.setmodviewy( zlevel, wModule.gety( module ) );
      if( rotation == 0 || rotation == 180 ) {
        wZLevel.setmodviewcx( zlevel, wModule.getcx( module ) );
        wZLevel.setmodviewcy( zlevel, wModule.getcy( module ) );
      }
      else {
        wZLevel.setmodviewcx( zlevel, wModule.getcy( module ) );
        wZLevel.setmodviewcy( zlevel, wModule.getcx( module ) );
      }
      break;
    }
    zlevel = wPlan.nextzlevel( data->model, zlevel );
  };
}

static void __backupSave( const char* fileName, const char* xml ) {
  char* backupfile;
  iOFile planFile;

  backupfile = StrOp.fmt( "%s.bak",fileName );
  /* Make Backup copy! Somtimes rocrail loses the plan and writes an empty plan! */
  if( FileOp.exist(backupfile) )
    FileOp.remove(backupfile);
  if( FileOp.exist(fileName) )
    FileOp.rename(fileName,backupfile);
  StrOp.free(backupfile);


  planFile = FileOp.inst( fileName, False );
  if( planFile != NULL ) {
    FileOp.write( planFile, xml, StrOp.len( xml ) );
    FileOp.close( planFile );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%ld bytes saved in %s.",
        FileOp.getWritten( planFile ), fileName );
    /* Cleanup. */
    planFile->base.del(planFile);
  }
}



/* save locs for modules */
static void __saveLocs( iOModPlan inst, const char* filename ) {
  /* locs must not be a part of an other file! */
  iOModPlanData data = Data(inst);
  iONode model = NodeOp.inst( wPlan.name(), NULL, ELEMENT_NODE );
  char* xml = NULL;
  if( wPlan.getlclist(data->model) != NULL )
    NodeOp.addChild( model, (iONode)NodeOp.base.clone( wPlan.getlclist(data->model) ) );
  if( wPlan.getcarlist(data->model) != NULL )
    NodeOp.addChild( model, (iONode)NodeOp.base.clone( wPlan.getcarlist(data->model) ) );
  if( wPlan.getoperatorlist(data->model) != NULL )
    NodeOp.addChild( model, (iONode)NodeOp.base.clone( wPlan.getoperatorlist(data->model) ) );

  /* Serialize plan. */
  xml = model->base.toString( model );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serialized Loc List=%d", StrOp.len( xml ) );
  if( StrOp.len( xml ) > 0 ) {
    __backupSave( filename, xml );
  }
  /* clean up */
  StrOp.free( xml );

  /* clean up */
  NodeOp.base.del(model);
}



static void __saveModPlan( iOModPlan inst, const char* filename ) {
  /* routes must not be a part of an other file! */
  iOModPlanData data = Data(inst);
  /* Serialize plan. */
  char* xml = ModPlanOp.base.serialize(inst, NULL);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serialized Modplan=%d", StrOp.len( xml ) );
  __backupSave( filename, xml );
  /* clean up */
  StrOp.free( xml );
}




/* save routes and schedules for modules */
static void __saveRoutes( iOModPlan inst, const char* filename ) {
  /* routes must not be a part of an other file! */
  iOModPlanData data = Data(inst);

  iONode model = NodeOp.inst( wPlan.name(), NULL, ELEMENT_NODE );
  char* xml = NULL;
  if( wPlan.getsclist(data->model) != NULL )
    NodeOp.addChild( model, (iONode)NodeOp.base.clone( wPlan.getsclist(data->model) ) );

  if( wPlan.getwaybilllist(data->model) != NULL )
    NodeOp.addChild( model, (iONode)NodeOp.base.clone( wPlan.getwaybilllist(data->model) ) );

  /* only routes without a modid set */
  if( wPlan.getstlist(data->model) != NULL ) {
  iONode routes = wPlan.getstlist(data->model);
  int cnt = NodeOp.getChildCnt(routes);
  int i = 0;
    NodeOp.addChild( model, (iONode)NodeOp.inst( wRouteList.name(), model, ELEMENT_NODE ) );
    for( i = 0; i < cnt; i++ ) {
      iONode route = NodeOp.getChild( routes, i );
      if( wRoute.getmodid(route) != NULL && (StrOp.len(wRoute.getmodid(route)) > 0 || StrOp.equals( wRoute.modid_auto_gen, wRoute.getmodid(route) ) ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Skipping Route %s which belongs to module %s",
          wRoute.getid(route), wRoute.getmodid(route) );
      continue;
      }
      NodeOp.addChild(model, (iONode)NodeOp.base.clone(route));
    }
  }

  /* Serialize plan. */
  xml = model->base.toString( model );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serialized Route List=%d", StrOp.len( xml ) );
  if( StrOp.len( xml ) > 0 ) {
    __backupSave( filename, xml );
  }
  /* clean up */
  StrOp.free( xml );

  /* clean up */
  NodeOp.base.del(model);
}




/* save a module */
static void __saveModule( iOModPlan inst, iONode module, int level ) {
  iOModPlanData data = Data(inst);
  const char* filename = wModule.getfilename(module);
  /* rotate back if rotated? */
  /* reset levels to zero? */
  char* xml = NULL;
  iONode model = NodeOp.inst( wPlan.name(), NULL, ELEMENT_NODE );
  iONode zlevel = NodeOp.inst( wZLevel.name(), model, ELEMENT_NODE );
  char* version = StrOp.fmt( "%d.%d.%d-%d", wGlobal.vmajor, wGlobal.vminor, wGlobal.patch, AppOp.getrevno() );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Saving module [%s][%d]...", filename, level );

  wZLevel.settitle( zlevel, wModule.gettitle(module) );
  wZLevel.setz( zlevel, level );
  NodeOp.addChild( model, zlevel );

  __copyLevel( inst, model, level, wTrackList.name() );
  __copyLevel( inst, model, level, wSwitchList.name() );
  __copyLevel( inst, model, level, wFeedbackList.name() );
  __copyLevel( inst, model, level, wSignalList.name() );
  __copyLevel( inst, model, level, wOutputList.name() );
  __copyLevel( inst, model, level, wBlockList.name() );
  __copyLevel( inst, model, level, wTextList.name() );
  __copyLevel( inst, model, level, wTurntableList.name() );
  __copyLevel( inst, model, level, wSelTabList.name() );
  __copyLevel( inst, model, level, wLocationList.name() );
  __copyLevel( inst, model, level, wScheduleList.name() );
  __copyLevel( inst, model, level, wActionList.name() );
  __copyLevel( inst, model, level, wBoosterList.name() );

  __copyResolvedRoutes( inst, model, wModule.getid(module) );
  __copyUnresolvedRoutes( inst, model, level );

  wPlan.setrocrailversion( model, version );
  /* Serialize plan. */
  xml = model->base.toString( model );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serialized Module=%d", StrOp.len( xml ) );
  if( StrOp.len( xml ) > 0 ) {
    __backupSave( filename, xml );
  }
  /* clean up */
  StrOp.free( xml );
  NodeOp.base.del(model);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Module Saved." );
}




static void _setModPlan( iOModPlan inst, iONode modplan ) {
  iOModPlanData data = Data(inst);
  /* merge attribute values */
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "merging modplan attributes..." );
  wModPlan.settitle( data->modplan, wModPlan.gettitle(modplan) );
  wModPlan.setlocs( data->modplan, wModPlan.getlocs(modplan) );
  wModPlan.setroutes( data->modplan, wModPlan.getroutes(modplan) );
  wModPlan.setsavemodplan( data->modplan, wModPlan.issavemodplan(modplan) );
  wModPlan.setsavemodules( data->modplan, wModPlan.issavemodules(modplan) );
  wModPlan.setinitfield( data->modplan, wModPlan.isinitfield(modplan) );
  wModPlan.setmodroutes( data->modplan, wModPlan.ismodroutes(modplan) );
}


static void _save( iOModPlan inst, const char* filename ) {
  iOModPlanData o = Data(inst);
  if( wModPlan.issavemodplan(o->modplan) ){
  /* save the modplan */
  if(wModPlan.issavemodules(o->modplan)) {
    int modcnt = ListOp.size( o->modList );
    int i = 0;
    for( i = 0; i < modcnt; i++ ) {
      iONode module = (iONode)ListOp.get( o->modList, i );
      __saveModule( inst, module, i );
    }
  }
    __saveLocs( inst, wModPlan.getlocs(o->modplan) );
    __saveRoutes( inst, wModPlan.getroutes(o->modplan) );
    __saveModPlan( inst, filename );
  }
}



static Boolean _modify( iOModPlan inst, iONode item ) {
  iOModPlanData data = Data(inst);
  const char* itemname = NodeOp.getName( item );
  const char* id = wItem.getid( item );
  const char* prev_id = wItem.getprev_id( item );

  if( StrOp.equals( wModule.name(), itemname ) ) {
    const char* modid = wModule.getid(item);
    const char* modcmd = wModule.getcmd(item);
    int modcnt = ListOp.size(data->modList);
    int i = 0;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "modify [%s]", modid );
    for( i = 0; i < modcnt; i++ ) {
      iONode module = (iONode)ListOp.get( data->modList, i );
      if( StrOp.equals( modid, wModule.getid(module) ) ) {
        int level = i;
        if( StrOp.equals(wModule.cmd_move, modcmd ) ) {
          wModule.setx( module, wModule.getx(item) );
          wModule.sety( module, wModule.gety(item) );
          __updateZLevel( inst, item, level );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
              "moving module [%s] to %d,%d", modid, wModule.getx(item), wModule.gety(item) );

          /* Notify the clients */
          {
            iONode zlevel = NodeOp.inst( wZLevel.name(), NULL, ELEMENT_NODE );
            wZLevel.settitle(zlevel, wModule.gettitle( module ) );
            wZLevel.setmodid(zlevel, wModule.getid( module ) );
            wZLevel.setz(zlevel, level );
            wZLevel.setmodviewx( zlevel, wModule.getx(module));
            wZLevel.setmodviewy( zlevel, wModule.gety(module));
            ClntConOp.broadcastEvent( AppOp.getClntCon(), zlevel );
          }
        }
        else if( StrOp.equals(wModule.cmd_north, modcmd ) ) {
          ModuleOp.rotateModule( data->model, module, level, 0 );
          __updateZLevel( inst, module, level );
        }
        else if( StrOp.equals(wModule.cmd_east, modcmd ) ) {
          ModuleOp.rotateModule( data->model, module, level, 90 );
          __updateZLevel( inst, module, level );
        }
        else if( StrOp.equals(wModule.cmd_south, modcmd ) ) {
          ModuleOp.rotateModule( data->model, module, level, 180 );
          __updateZLevel( inst, module, level );
        }
        else if( StrOp.equals(wModule.cmd_west, modcmd ) ) {
          ModuleOp.rotateModule( data->model, module, level, 270 );
          __updateZLevel( inst, module, level );
        }
        break;
      }
    }


  }
  else if( StrOp.equals( wZLevel.name(), itemname ) ) {
    /* remove the module: */
    iONode module = wModPlan.getmodule(data->modplan);
    while( module != NULL ) {
      if( StrOp.equals( wZLevel.getmodid(item), wModule.getid(module) ) ) {
        NodeOp.removeChild( data->modplan, module );
        /* TODO: clean up the zlevel and module node? */
        break;
      }
      module = wModPlan.nextmodule(data->modplan, module);
    };
  }
}


static void _addModule(iOModPlan inst, iONode module) {
  iOModPlanData data = Data(inst);
  if( module != NULL ) {
    iONode zlevel = __mergeModule( data, data->model, module, ListOp.size(data->modList), True );
    if( zlevel != NULL ) {
      iONode zlevelClone = (iONode)NodeOp.base.clone( zlevel );
      /* add module to the list for modify */
      iONode moduleClone = (iONode)NodeOp.base.clone(module);
      ListOp.add( data->modList, (obj)moduleClone );
      NodeOp.addChild( data->modplan, moduleClone );


      /* Add all item and broadcast to clients. */
      /* TODO: add all level related items and new routes... */
      ClntConOp.broadcastEvent( AppOp.getClntCon(), zlevelClone );
    }
  }

}

/**  */
static struct OModPlan* _inst( iONode modplan ) {
  iOModPlan __ModPlan = allocMem( sizeof( struct OModPlan ) );
  iOModPlanData data = allocMem( sizeof( struct OModPlanData ) );
  MemOp.basecpy( __ModPlan, &ModPlanOp, 0, sizeof( struct OModPlan ), data );

  /* Initialize data->xxx members... */

  data->modplan             = modplan;
  data->unresolvedRouteList = ListOp.inst();
  data->modList             = ListOp.inst();
  data->modconList          = ListOp.inst();
  data->fbeventMap          = MapOp.inst();
  data->blockMap            = MapOp.inst();
  data->routeIdMap          = MapOp.inst();
  data->mod2routeIdMap      = MapOp.inst();

  instCnt++;
  __modplan = __ModPlan;
  return __ModPlan;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/modplan.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
