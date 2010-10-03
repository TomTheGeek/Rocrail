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

/* sensitive code don't touch! */

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
  if( StrOp.equals( wItem.west , ori ) ) return oriWest;
  if( StrOp.equals( wItem.north, ori ) ) return oriNorth;
  if( StrOp.equals( wItem.east , ori ) ) return oriEast;
  if( StrOp.equals( wItem.south, ori ) ) return oriSouth;
}

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

      const char * ori = wItem.getori(node);
      if( ori == NULL ) {
        ori = "west";
      }

      const char* type = wItem.gettype(node);
      __createKey( key, node, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
          key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );
      MapOp.put( data->objectmap, key, (obj)node);

      /* put keys for all covered fields */
      if( StrOp.equals( NodeOp.getName(node), "sw" ) ) {
        if( StrOp.equals( wItem.gettype(node), "crossing" ) ||
            StrOp.equals( wItem.gettype(node), "ccrossing" )) {

          if( StrOp.equals( ori, "east" ) || StrOp.equals( ori, "west" ) ) {
            __createKey( key, node, 1, 0, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding EXTRA key %s for %s type: %s ori: %s name: %s",
                      key, NodeOp.getName(node), type==NULL?"":type, wItem.getori(node), ori );
            MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, "north" ) || StrOp.equals( ori, "south" ) ) {
            __createKey( key, node, 0, 1, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding EXTRA key %s for %s type: %s ori: %s name: %s",
                      key, NodeOp.getName(node), type==NULL?"":type, wItem.getori(node), ori );
            MapOp.put( data->objectmap, key, (obj)node);
          }
        }
      }

      if( StrOp.equals( NodeOp.getName(node), "bk" ) ) {

        int fields = 4;
        if( wBlock.issmallsymbol(node) ) {
          fields = 2;
        }

        int i;
        for (i=1;i<fields;i++) {

          if( StrOp.equals( ori, "east" ) || StrOp.equals( ori, "west" ) ) {
            __createKey( key, node, i, 0, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding EXTRA key %s for %s type: %s ori: %s name: %s",
                      key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );
            MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, "north" ) || StrOp.equals( ori, "south" ) ) {
            __createKey( key, node, 0, i, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding EXTRA key %s for %s type: %s ori: %s name: %s",
                      key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );
            MapOp.put( data->objectmap, key, (obj)node);
          }
        }
      }


    }
    node = (iONode)ListOp.next( list );
  };
  return bklist;
}


static const int typeTrackStraight  = 0;
static const int typeTrackCurve  = 1;
static const int typeBlock  = 2;
static const int typeSwitch  = 3;
static const int itemNotInDirection = 1000;

static int __getType(iONode item ) {
  const char* type = NodeOp.getName(item);
  const char* subtype = wItem.gettype(item);

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
static const int threeWayTurnout = 300;

static int __travel( iONode block, iONode item, int travel, int turnoutstate, int * x, int * y, const char * key) {
  if( item ) {
    const char * itemori = wItem.getori(item);

    *x = 0;
    *y = 0;

    /* missing default values */
    if( itemori == NULL) {
      itemori = "west";
      //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set default");
    }

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

        /* crossing */
        else if( StrOp.equals( wItem.gettype(item), "crossing" ) ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " crossing %d", wSwitch.isdir(item) );

          if( !wSwitch.isdir(item)  ) { // left

            if( StrOp.equals( itemori, "west" ) || StrOp.equals( itemori, "east" )) {
              if( (travel == 1) ) {
                *x = 1;
              } else if ( travel == 3 ) {
                *x = -1;
              }
            }
            else if( StrOp.equals( itemori, "north" ) || StrOp.equals( itemori, "south" )) {
              if( (travel == 0) ) {
                *y = -1;
              } else if ( travel == 2 ) {
                *y = 1;
              }
            }
          } else if( wSwitch.isdir(item) ) { // right
            if( StrOp.equals( itemori, "west" ) || StrOp.equals( itemori, "east" )) {
              if( (travel == 1) ) {
                *x = -1;
              } else if ( travel == 3 ) {
                *x = 1;
              }
            }
            else if( StrOp.equals( itemori, "north" ) || StrOp.equals( itemori, "south" )) {
              if( (travel == 0) ) {
                *y = 1;
              } else if ( travel == 2 ) {
                *y = -1;
              }
            }
          }
          return travel;
        }
        /* ccrossing */
        else if( StrOp.equals( wItem.gettype(item), "ccrossing" ) ) {

          char mkey[32] = {'\0'};
          __createKey( mkey, item, 0, 0, 0);

          int sign = 1;
          if( !StrOp.equals( key, mkey ) ) {
            sign = -1;
          }

          if( StrOp.equals( itemori, "west" ) || StrOp.equals( itemori, "east" )) {
            if( (travel == 1) ) {
              *x = sign;
              return travel;
            } else if ( travel == 3 ) {
              *x = sign;
              return travel;
            }
          }
          else if( StrOp.equals( itemori, "north" ) || StrOp.equals( itemori, "south" )) {
            if( (travel == 0) ) {
              *y = sign;
              return travel;
            } else if ( travel == 2 ) {
              *y = sign;
              return travel;
            }
          }
        }

        /* threeway */
        else if( StrOp.equals( wItem.gettype(item), "threeway" ) ) {

          if( StrOp.equals( itemori, "west" )) {
            if( (travel == 0) ) {
              if( turnoutstate == 0) { // center
                return travel+threeWayTurnout;
              } else if (turnoutstate == 1) { // left
                return oriSouth+threeWayTurnout;
              } else if (turnoutstate == 2) { // right
                return oriNorth+threeWayTurnout;
              }
            }
            else if ( travel == 1 ) {
              return oriEast;
            }
            else if ( travel == 2 ) {
              return travel;
            }
            else if ( travel == 3 ) {
              return oriEast;
            }
          }
          else if( StrOp.equals( itemori, "north" )) {
            if( (travel == 0) ) {
              return oriNorth;
            }
            else if ( travel == 1 ) {
              return travel;
            }
            else if ( travel == 2 ) {
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
          else if( StrOp.equals( itemori, "east" )) {
            if( (travel == 0) ) {
              return travel;
            }
            else if ( travel == 1 ) {
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
              return oriWest;
            }
          }
          else if( StrOp.equals( itemori, "south" )) {
            if( (travel == 0) ) {
              return oriSouth;
            }
            else if ( travel == 1 ) {
              return travel;
            }
            else if ( travel == 2 ) {
              return oriSouth;
            }
            else if ( travel == 3 ) {
              if( turnoutstate == 0) {
                return travel+threeWayTurnout;
              } else if (turnoutstate == 1) {
                return oriWest+threeWayTurnout;
              } else if (turnoutstate == 2) {
                return oriEast+threeWayTurnout;
              }
            }
          }
        }

        /* turnout in wrong direction*/
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  ---------- no valid turnout for us! [%s]",
                        wItem.getid(item) );
          return -1; /*end of the game*/
        }

      } else { /* elements which do not change travel direction */
        /* is the item in our direction? */
        if( !(((StrOp.equals( itemori, "north" ) || StrOp.equals( itemori, "south" ))
                    && (travel == 1 || travel == 3)) ||
            ((StrOp.equals( itemori, "east" ) || StrOp.equals( itemori, "west" ))
                    && (travel == 0 || travel == 2))) ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
              "    Item [%s] is not in our travel direction giving up",
                    wItem.getid(item) );
          return itemNotInDirection; /*puh*/
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
  int blockleftcounter = 0;
  int prevtravel = -1;
  Boolean creep = False;

  const char * prevItemId = " ";

  int x = 0;
  int y = 0;

  const char * deep = " - ";
    int i;
    for( i = 0; i<depth; i++) {
      deep = StrOp.fmt( "%s - ", deep );
    }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sstart analyzing item [%s] travel: %d for state: %d", deep,
        wSwitch.getid(turnout), travel, turnoutstate);

  /* start again at the currend turnout */
  travel = __travel(turnout, turnout, travel, turnoutstate, &x, &y, "");
  if( travel >= 200 && travel < 300) {
    travel -= twoWayTurnout;
  }
  if( travel >= 300 && travel < 400) {
    travel -= threeWayTurnout;
  }


  int xoffset = 0;
  int yoffset = 0;

  do {
    if( travel >= 0) {

      xoffset += x;
      yoffset += y;

      switch(travel) {
      case oriWest:
        xoffset--;
        __createKey( key, turnout, xoffset, yoffset, 0);
        //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sgoing west",deep );
         break;
      case oriNorth:
        yoffset--;
        __createKey( key, turnout, xoffset, yoffset, 0);
        //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sgoing north",deep );
         break;
      case oriEast:
        xoffset++;
        __createKey( key, turnout, xoffset, yoffset, 0);
        //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sgoing east",deep );
         break;
      case oriSouth:
        yoffset++;
        __createKey( key, turnout, xoffset, yoffset, 0);
        //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sgoing south", deep );
         break;
      }

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "key: %s", key);
      item = (iONode)MapOp.get( data->objectmap, key);


      if( item != NULL) {



        if( StrOp.equals( wItem.getid(item), prevItemId )) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "same item move on!");
        } else {

          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sitem: [%s] id: [%s] travel: [%d]", deep,
                                            NodeOp.getName(item), wItem.getid(item), travel);
        }


        if( StrOp.equals(NodeOp.getName(item) , "bk" ) ) {

          if( StrOp.equals( wItem.getid(turnout), wItem.getid(item) )) {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "same block move on!");
          } else {

            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%sWe found a Block! [%s]", deep, wItem.getid(item) );

            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sfound route: [[%s]->[%s]]", deep,
                wItem.getid(turnout), wItem.getid(item) );
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "     " );

            travel = -1;
          break; /*DONE*/
          }

        } else {
          prevtravel = travel;
          travel = __travel(turnout, item, travel, turnoutstate, &x, &y, key);
        }

        /* found Item is not in our direction. done. */
        if( travel == itemNotInDirection) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
              "%sitem [%s] is not in our travel direction. this branch ends here.", deep,
                    wItem.getid(item) );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " ");
          break;
        }

        prevItemId = wItem.getid(item);

        /* item is a turnout -> coming from the points: dive into branches */
        if( travel >= 200 && travel < 300) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "     " );
          travel -= twoWayTurnout;
          depth++;
          __analyseTurnout(inst, item, prevtravel, 0, depth);
          __analyseTurnout(inst, item, prevtravel, 1, depth);
        } else if( travel >= 300 && travel < 400) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "     " );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  THREE  " );
          travel -= threeWayTurnout;
          depth++;
          __analyseTurnout(inst, item, prevtravel, 0, depth);
          __analyseTurnout(inst, item, prevtravel, 1, depth);
          __analyseTurnout(inst, item, prevtravel, 2, depth);
        }
        /*TODO: 3-way, DKW ...*/


      } else { /*item==NULL*/

         TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%sthis branch ends here.",deep);
         TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " ");
         break;
      }

    } else { /*travel*/
     TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "XXXXXXXXXXXXXXXX PANIC this line should not appear!" );
     break;
    }
  } while(foundBlock );



}

static void __analyseBlock(iOAnalyse inst, iONode block, const char * inittravel) {
  iOAnalyseData data = Data(inst);
  char key[32] = {'\0'};
  iONode item = NULL;
  int travel;
  int blockleftcounter = 0;

  if( StrOp.equals( wItem.west , inittravel ) ) travel = oriWest;
  if( StrOp.equals( wItem.north, inittravel ) ) travel = oriNorth;
  if( StrOp.equals( wItem.east , inittravel ) ) travel = oriEast;
  if( StrOp.equals( wItem.south, inittravel ) ) travel = oriSouth;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "     " );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "start analyzing block [%s] in [%s] direction",
      wBlock.getid(block), inittravel);
  

    int xoffset = 0;
    int yoffset = 0;

    if( StrOp.equals( inittravel, "east" ) ) {
      /* TODO: the real block length */
      xoffset = 3;
    }

    if( StrOp.equals( inittravel, "south" ) ) {
      /* TODO: the real block length */
      yoffset = 3;
    }


    /* start the recursion */
    __analyseTurnout(inst, block, travel, 0, 0);

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

    const char * blockori = wItem.getori(block);
    /*default*/
    if(  blockori == NULL) {
      blockori = "west";
    }

    if( StrOp.equals( blockori, "west" ) || StrOp.equals( blockori, "east" ) ) {
      __analyseBlock(inst,block, "west");
      __analyseBlock(inst,block, "east");
    } else if( StrOp.equals( blockori, "north" ) || StrOp.equals( blockori, "south" ) ) {
      __analyseBlock(inst,block, "north");
      __analyseBlock(inst,block, "south");
    }

    block = (iONode)ListOp.next(bklist);
  }
  
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

