/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
**Missing symbol is a z-level connector.** "-()"

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

*/

#include "analyser/impl/analyse_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"

#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Track.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Feedback.h"

static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOAnalyseData data = Data(inst);
    /* Cleanup data->xxx members...*/
    MapOp.base.del(data->objectmap);
    MapOp.base.del(data->branchmap);
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
static const int BlockCX = 4;

static char* __createKey( char* key, iONode node, int xoffset, int yoffset, int zoffset) {
  return StrOp.fmtb( key, "%d-%d-%d", wItem.getx(node)+xoffset, wItem.gety(node)+yoffset, wItem.getz(node)+zoffset );
}

static iOList __prepare(iOAnalyse inst, iOList list) {
  iOAnalyseData data = Data(inst);
  iOList bklist = ListOp.inst();
  char key[32] = {'\0'};
  iONode node = (iONode)ListOp.first( list );
  
  while( node != NULL ) {
    if( StrOp.equals( wBlock.name(), NodeOp.getName(node) ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  block %s with key %s", 
          wBlock.getid(node), __createKey( key, node, 0, 0, 0) );
      ListOp.add( bklist, (obj)node );
    }
    { /*blocks as well in the map!*/
      /* put the object in the map */
      const char* type = wItem.gettype(node);
      __createKey( key, node, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
          key, NodeOp.getName(node), type==NULL?"":type, wItem.getori(node), wItem.getid(node) );
      MapOp.put( data->objectmap, key, (obj)node);

      if( StrOp.equals( NodeOp.getName(node), "sw" ) ) {
        if( StrOp.equals( wItem.gettype(node), "left" )
            || StrOp.equals( wItem.gettype(node), "right" ) ){

            int turnouts = 2;
            MapOp.put( data->branchmap, key, (obj)turnouts );
        }
      }
    }
    node = (iONode)ListOp.next( list );
  };
  return bklist;
}


#define oriWest  0
#define oriNorth 1
#define oriEast  2
#define oriSouth 3
/* returns 0 for west, 1 for north, 2 for east and 3 for south */
static int __getOri(iONode item ) {
  const char* ori = wItem.getori(item);
  if( StrOp.equals( wItem.west , ori ) ) return oriWest;
  if( StrOp.equals( wItem.north, ori ) ) return oriNorth;
  if( StrOp.equals( wItem.east , ori ) ) return oriEast;
  if( StrOp.equals( wItem.south, ori ) ) return oriSouth;
}


static const int typeTrackStraight  = 0;
static const int typeTrackCurve  = 1;
static const int typeBlock  = 2;
static const int typeSwitch  = 3;

static int __getType(iONode item ) {
  const char* type = NodeOp.getName(item);
  const char* subtype = wItem.gettype(item);

  /*
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "      GETTYPE: %s %s",
       type, wItem.gettype(item));
       */

  /*if( StrOp.equals( "tk" , type ) || StrOp.equals( wFeedback.name() , type ) ) {*/
    if(        StrOp.equals( wTrack.curve, subtype ) ) {
      return typeTrackCurve;
    } else if( StrOp.equals( "sw", type ) ) {
      return typeSwitch;
    } else {
      return typeTrackStraight;
    }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "      --- GETTYPE: FAILED should not happen");
  return -1;
}


static const int foundBlock = 100;
static const int twoWayTurnout = 200;

static int __travel( iONode block, iONode item, int travel, int turnoutstate) {
  if( item ) {

    const char * itemori = wItem.getori(item);

    /* missing default values */
    if( itemori == NULL) {
      itemori = "west";
      //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set default");
    }

/*
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "block: [%s] item: [%s] id: [%s] ori: [%s] type: [%d]",
        wBlock.getid(block), NodeOp.getName(item), wItem.getid(item),
            itemori, __getType(item) );
*/


    //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "   we travel -> %d",
    //    travel );

      /* curve -> change dir */
      if( __getType(item) == typeTrackCurve) {
        /* algebra of Rocrail directions */
        if(        travel == 0 &&  StrOp.equals( itemori, "north" )) {
          return oriSouth;
        } else if( travel == 0 &&  StrOp.equals( itemori, "east" )) {
          return oriNorth;
        } else if( travel == 1 &&  StrOp.equals( itemori, "west" )) {
          return oriWest;
        } else if( travel == 1 &&  StrOp.equals( itemori, "north" )) {
          return oriEast;
        } else if( travel == 2 &&  StrOp.equals( itemori, "west" )) {
          return oriSouth;
        } else if( travel == 2 &&  StrOp.equals( itemori, "south" )) {
          return oriNorth;
        } else if( travel == 3 &&  StrOp.equals( itemori, "east" )) {
          return oriEast;
        } else if( travel == 3 &&  StrOp.equals( itemori, "south" )) {
          return oriWest;
        } else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  -- no valid curve for us! [%s]",
              wItem.getid(item) );
          return -1; /*end of the game */
        }
      } else if( __getType(item) == typeSwitch) {


        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SWITCH" );
        /*if( turnoutstate == 0)
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  STATE: %d -> going straight", turnoutstate);
        else if( turnoutstate == 1)
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  STATE: %d -> going turnout", turnoutstate);
        */

        /* coming from the points */
        if(        travel == 0 &&  StrOp.equals( itemori, "east" )
                && StrOp.equals( wItem.gettype(item), "right" ) ) {

          if(turnoutstate == 1)
            return oriNorth+twoWayTurnout;

          return travel+twoWayTurnout;
        } else if( travel == 0 &&  StrOp.equals( itemori, "west" )
                && StrOp.equals( wItem.gettype(item), "left" ) ) {

          if(turnoutstate == 1)
            return oriSouth+twoWayTurnout;

          return travel+twoWayTurnout;
        } else if( travel == 1 &&  StrOp.equals( itemori, "north" )
                && StrOp.equals( wItem.gettype(item), "right" ) ) {

          if(turnoutstate == 1)
            return oriWest+twoWayTurnout;

          return travel+twoWayTurnout;
        } else if( travel == 1 &&  StrOp.equals( itemori, "south" )
                && StrOp.equals( wItem.gettype(item), "left" ) ) {

          if(turnoutstate == 1)
            return oriEast+twoWayTurnout;

          return travel+twoWayTurnout;
        } else if( travel == 2 &&  StrOp.equals( itemori, "west" )
                && StrOp.equals( wItem.gettype(item), "right" ) ) {

          if(turnoutstate == 1)
            return oriSouth+twoWayTurnout;

          return travel+twoWayTurnout;
        } else if( travel == 2 &&  StrOp.equals( itemori, "east" )
                && StrOp.equals( wItem.gettype(item), "left" ) ) {

          if(turnoutstate == 1)
            return oriNorth+twoWayTurnout;

          return travel+twoWayTurnout;
        } else if( travel == 3 &&  StrOp.equals( itemori, "south" )
                && StrOp.equals( wItem.gettype(item), "right" ) ) {

          if(turnoutstate == 1)
            return oriEast+twoWayTurnout;

          return travel+twoWayTurnout;
        } else if( travel == 3 &&  StrOp.equals( itemori, "north" )
                && StrOp.equals( wItem.gettype(item), "left" ) ) {

          if(turnoutstate == 1)
            return oriWest+twoWayTurnout;

          return travel+twoWayTurnout;
        }

        /* coming from the frog -> straight line */
        else if( travel == 0 &&  StrOp.equals( itemori, "west" )
                && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          return travel;
        } else if( travel == 0 &&  StrOp.equals( itemori, "east" )
                && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          return travel;
        } else if( travel == 1 &&  StrOp.equals( itemori, "south" )
                && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          return travel;
        } else if( travel == 1 &&  StrOp.equals( itemori, "north" )
                && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          return travel;
        } else if( travel == 2 &&  StrOp.equals( itemori, "east" )
               && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          return travel;
        } else if( travel == 2 &&  StrOp.equals( itemori, "west" )
               && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          return travel;
        } else if( travel == 3 &&  StrOp.equals( itemori, "north" )
              && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          return travel;
        } else if( travel == 3 &&  StrOp.equals( itemori, "south" )
              && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          return travel;
        }

        /* coming from the frog -> diverging line */
        else if( travel == 0 &&  StrOp.equals( itemori, "north" )
                && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          return oriSouth;
        } else if( travel == 0 &&  StrOp.equals( itemori, "north" )
                && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          return oriNorth;
        } else if( travel == 1 &&  StrOp.equals( itemori, "west" )
                && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          return oriWest;
        } else if( travel == 1 &&  StrOp.equals( itemori, "west" )
                && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          return oriEast;
        } else if( travel == 2 &&  StrOp.equals( itemori, "south" )
               && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          return oriNorth;
        } else if( travel == 2 &&  StrOp.equals( itemori, "south" )
               && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          return oriSouth;
        } else if( travel == 3 &&  StrOp.equals( itemori, "east" )
              && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          return oriEast;
        } else if( travel == 3 &&  StrOp.equals( itemori, "east" )
              && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          return oriWest;
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  -- no valid turnout for us! [%s]",
                        wItem.getid(item) );
          return -1; /*end of the game*/
        }

      } else { /* elements which do not change travel direction */
        /* is the item in our direction? */
        if( !(((StrOp.equals( itemori, "north" ) || StrOp.equals( itemori, "south" ))
                    && (travel == 1 || travel == 3)) ||
            ((StrOp.equals( itemori, "east" ) || StrOp.equals( itemori, "west" ))
                    && (travel == 0 || travel == 2))) ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
              "    Item [%s] is not in our travel direction giving up",
                    wItem.getid(item) );
          return -1; /*puh*/
        } else {
          return travel;
        }
      }
  } else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ITEM==NULL");
  }

  return -1;
}

static void __analyseTurnout(iOAnalyse inst, iONode turnout, int travel, int turnoutstate, int depth) {
  iOAnalyseData data = Data(inst);
  char key[32] = {'\0'};
  iONode item = NULL;

  const char * deep = " - ";
    int i;
    for( i = 0; i<depth; i++) {
      deep = StrOp.fmt( "%s - ", deep );
    }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s##########################################", deep);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sstart analysing turnout %s IN  travel: %d for state: %d", deep,
        wSwitch.getid(turnout), travel, turnoutstate);


  travel = __travel(turnout, turnout, travel, turnoutstate);
  if( travel > 100) {
    travel -= twoWayTurnout;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sstart analysing turnout %s IN travel1: %d for state: %d", deep,
          wSwitch.getid(turnout), travel, turnoutstate);

  if( True) { //__getOri(turnout) == oriWest ) {
    int xoffset = 0;
    int yoffset = 0;
    //travel = __getOri(turnout);

    do {

      //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "     " );
      //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sHERE WE TRAVEL %d at (%d,%d)",deep, travel, xoffset, yoffset );

      if( travel >= 0) {
        switch(travel) {
        case oriWest:
          xoffset--;
          __createKey( key, turnout, xoffset, yoffset, 0);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sgoing west",deep );
           break;
        case oriNorth:
          yoffset--;
          __createKey( key, turnout, xoffset, yoffset, 0);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sgoing north",deep );
           break;
        case oriEast:
          xoffset++;
          __createKey( key, turnout, xoffset, yoffset, 0);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sgoing east",deep );
           break;
        case oriSouth:
          yoffset++;
          __createKey( key, turnout, xoffset, yoffset, 0);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sgoing south", deep );
           break;
        }

        //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sHERE KEY %s", deep, key );
        item = (iONode)MapOp.get( data->objectmap, key);


        if( item != NULL) {
          if( StrOp.equals(NodeOp.getName(item) , "bk" ) ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sWe found a Block! [%s]", deep, wItem.getid(item) );
            travel = -1;
          } /*else if ( StrOp.equals(NodeOp.getName(item) , "sw" ) ) {
           TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "      We found another Turnout! [%s]", wItem.getid(item) );
            travel = __travel(turnout, item, travel, 0);
          }*/ else {


            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s[%s] item: [%s] id: [%s] type: [%d]", deep,
                    wItem.getid(item), NodeOp.getName(item), wItem.getid(item),
                         __getType(item) );

            travel = __travel(turnout, item, travel, turnoutstate);
          }

            if( travel > 100) {
              travel -= twoWayTurnout;

              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> turnout TO: %d", deep, travel );


              __analyseTurnout(inst, item, travel, 0, depth++);
              __analyseTurnout(inst, item, travel, 1, depth++);
            }


        } else {
          travel = -1;
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%send of west direction (dead end)", deep );
        }

      } else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%send of west direction", deep );
        break;
      }

      } while(item && travel != foundBlock );

    }

}

static void __analyseBlock(iOAnalyse inst, iONode block, Boolean firstrun, const char * turnoutid) {
  iOAnalyseData data = Data(inst);
  char key[32] = {'\0'};
  iONode item = NULL;
  int travel = -1;

  int turnouts = 0;

  //iOMap turnoutmap = MapOp.inst();
  iOList turnoutids;
  if(firstrun) {
  turnoutids = ListOp.inst();
  }

  Boolean weHaveOpenBranches = False;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "##########################################");
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "start analysing block %s ",
      wBlock.getid(block));
  
  if( __getOri(block) == oriWest ) {
    int xoffset = 0;
    int yoffset = 0;
    travel = oriWest;
    /* creeping west */
    do {

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "     " );
      //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "HERE WE TRAVEL %d at (%d,%d)", travel, xoffset, yoffset );

      if( travel >= 0) {
        switch(travel) {
        case oriWest:
          xoffset--;
          __createKey( key, block, xoffset, yoffset, 0);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "going west" );
           break;
        case oriNorth:
          yoffset--;
          __createKey( key, block, xoffset, yoffset, 0);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "going north" );
           break;
        case oriEast:
          xoffset++;
          __createKey( key, block, xoffset, yoffset, 0);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "going east" );
           break;
        case oriSouth:
          yoffset++;
          __createKey( key, block, xoffset, yoffset, 0);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "going south" );
           break;
        }

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "HERE KEY %s", key );
        item = (iONode)MapOp.get( data->objectmap, key);



        if( item != NULL) {
          if( StrOp.equals(NodeOp.getName(item) , "bk" ) ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "We found a Block! [%s]", wItem.getid(item) );
            travel = -1;
          } else if ( StrOp.equals(NodeOp.getName(item) , "sw" ) ) {

            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "We found a Turnout! [%s]", wItem.getid(item) );
            travel = __travel(block, item, travel, 0);

          } else {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "[%s] item: [%s] id: [%s] type: [%d]",
                                wItem.getid(item), NodeOp.getName(item), wItem.getid(item),
                                     __getType(item) );
            travel = __travel(block, item, travel, 0);
          }


            if( travel > 100) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " -> turnout" );

              travel -= twoWayTurnout;
              __analyseTurnout(inst, item, travel, 0, 0);
              __analyseTurnout(inst, item, travel, 1, 0);

              travel = -1;

            }

        } else {
          travel = -1;
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "end of west direction 1 for: [%s]", wBlock.getid(block) );
        }

      } else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "end of west direction 2 for: [%s]", wBlock.getid(block) );
        break;
      }
    } while(item && travel != foundBlock );


    if(turnouts > 0 && firstrun) {

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "there were %d turnouts in our path:", turnouts );

      iONode node = (iONode)ListOp.first(turnoutids);
      while(node) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sw: %s", wItem.getid(node) );

        node = (iONode)ListOp.next(turnoutids);
      }
      node = (iONode)ListOp.first(turnoutids);
      while(node) {

        __analyseBlock(inst,block,False, wItem.getid(node));

        node = (iONode)ListOp.next(turnoutids);
      }

    }


    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DONE travelling west" );

  }

}


static void _analyse(iOAnalyse inst) {
  iOAnalyseData data = Data(inst);
  iOList bklist = NULL;
  iONode block = NULL;
  int cx, cy;
  iOList list = data->model->getLevelItems( data->model, 0, &cx, &cy, False);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, 
      "Trackplan: %d objects at level 0 and sizes %d x %d", ListOp.size(list), cx, cy );
  
  MapOp.clear(data->objectmap);
  bklist = __prepare(inst, list);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "  it contains %d blocks", ListOp.size(bklist) );
  
  block = (iONode)ListOp.first(bklist);
  while(block) {
    __analyseBlock(inst,block,True,NULL);
    block = (iONode)ListOp.next(bklist);
  }
  
  ListOp.base.del(list);
  ListOp.base.del(bklist);
}


/**  */
static struct OAnalyse* _inst( iOModel model, iONode plan ) {
  iOAnalyse __Analyse = allocMem( sizeof( struct OAnalyse ) );
  iOAnalyseData data = allocMem( sizeof( struct OAnalyseData ) );
  MemOp.basecpy( __Analyse, &AnalyseOp, 0, sizeof( struct OAnalyse ), data );

  /* Initialize data->xxx members... */
  data->model = model;
  data->plan  = plan;
  data->objectmap = MapOp.inst();
  data->branchmap = MapOp.inst();
  
  instCnt++;
  return __Analyse;
}

/* Support for dynamic Loading */
iOAnalyse rocGetAnalyserInt( iOModel model, iONode plan )
{
  return (iOAnalyse)_inst( model, plan );
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "analyser/impl/analyse.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/

