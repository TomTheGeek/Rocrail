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
#include "rocrail/wrapper/public/ModPlan.h"
#include "rocrail/wrapper/public/Module.h"

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
    MapOp.base.del(data->prelist);
    MapOp.base.del(data->bklist);
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
  return StrOp.fmtb( key, "%d-%d-%d", wItem.getx(node)+xoffset, wItem.gety(node)+yoffset, 0 );
  //return StrOp.fmtb( key, "%d-%d-%d", wItem.getx(node)+xoffset, wItem.gety(node)+yoffset, wItem.getz(node)+zoffset );
}

static void __prepare(iOAnalyse inst, iOList list, int modx, int mody) {
  iOAnalyseData data = Data(inst);
  //iOList bklist = ListOp.inst();
  char key[32] = {'\0'};

  /* clone the node */
  iONode node = (iONode)NodeOp.base.clone( (iONode)ListOp.first( list ));

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "mod x: %d y: %d ",
            modx, mody );

  while( node != NULL ) {
    if( StrOp.equals( wBlock.name(), NodeOp.getName(node) ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  block %s with key (old) %s",
          wBlock.getid(node), __createKey( key, node, 0+modx, 0+mody, 0) );

      ListOp.add( data->bklist, (obj)node );
    }
    { /*blocks as well in the map!*/
      /* put the object in the map */

      const char * ori = wItem.getori(node);
      if( ori == NULL ) {
        ori = "west";
      }

      const char* type = wItem.gettype(node);
      __createKey( key, node, 0+modx, 0+mody, 0);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
          key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );
      MapOp.put( data->objectmap, key, (obj)node);

      wItem.setx( node, wItem.getx(node)+modx);
      wItem.sety( node, wItem.gety(node)+mody);

      /* put keys for all covered fields */
      if( StrOp.equals( NodeOp.getName(node), "sw" ) ) {
        if( StrOp.equals( wItem.gettype(node), "crossing" ) ||
            StrOp.equals( wItem.gettype(node), "dcrossing" ) ||
            StrOp.equals( wItem.gettype(node), "ccrossing" )) {

          if( StrOp.equals( ori, "east" ) || StrOp.equals( ori, "west" ) ) {
            __createKey( key, node, 1, 0, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                      key, NodeOp.getName(node), type==NULL?"":type, wItem.getori(node), ori );

            MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, "north" ) || StrOp.equals( ori, "south" ) ) {
            __createKey( key, node, 0, 1, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
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
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                      key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );

            MapOp.put( data->objectmap, key, (obj)node);
          }
          if( StrOp.equals( ori, "north" ) || StrOp.equals( ori, "south" ) ) {
            __createKey( key, node, 0, i, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s type: %s ori: %s name: %s",
                      key, NodeOp.getName(node), type==NULL?"":type, ori, wItem.getid(node) );

            MapOp.put( data->objectmap, key, (obj)node);
          }
        }
      }


    }

    iONode nextnode = (iONode)ListOp.next( list );
    if( nextnode != NULL)
      node = (iONode)NodeOp.base.clone( nextnode);
    else
      node = NULL;
  };
  //return bklist;
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
static const int dcrossing = 400;

static int __travel( iONode item, int travel, int turnoutstate, int * turnoutstate_out, int * x, int * y, const char * key) {
  if( item ) {
    const char * itemori = wItem.getori(item);

    *turnoutstate_out = 0;
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
      }
      /* block */
      else if( StrOp.equals( NodeOp.getName(item) , "bk" )) {

        int step = 3;
        if( wBlock.issmallsymbol( item )) {
          step = 1;
        }

        if( StrOp.equals( itemori, "west" ) || StrOp.equals( itemori, "east" )) {
          if( (travel == 2) ) {
            *x = step;
            return travel;
          } else if ( travel == 0)
            return travel;
        }
        else if( StrOp.equals( itemori, "north" ) || StrOp.equals( itemori, "south" )) {
          if( (travel == 3) ) {
            *y = step;
            return travel;
          } else if ( travel == 1)
            return travel;
        }
        return itemNotInDirection;
      }

      /* switch */
      else if( __getType(item) == typeSwitch) {
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
          *turnoutstate_out = 0;
          return travel;
        } else if( travel == 0 &&  StrOp.equals( itemori, "east" )
                && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          *turnoutstate_out = 0;
          return travel;
        } else if( travel == 1 &&  StrOp.equals( itemori, "south" )
                && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          *turnoutstate_out = 0;
          return travel;
        } else if( travel == 1 &&  StrOp.equals( itemori, "north" )
                && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          *turnoutstate_out = 0;
          return travel;
        } else if( travel == 2 &&  StrOp.equals( itemori, "east" )
               && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          *turnoutstate_out = 0;
          return travel;
        } else if( travel == 2 &&  StrOp.equals( itemori, "west" )
               && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          *turnoutstate_out = 0;
          return travel;
        } else if( travel == 3 &&  StrOp.equals( itemori, "north" )
              && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          *turnoutstate_out = 0;
          return travel;
        } else if( travel == 3 &&  StrOp.equals( itemori, "south" )
              && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming STRAIGHT" );
          *turnoutstate_out = 0;
          return travel;
        }

        /* coming from the frog -> diverging line */
        else if( travel == 0 &&  StrOp.equals( itemori, "north" )
                && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          *turnoutstate_out = 1;
          return oriSouth;
        } else if( travel == 0 &&  StrOp.equals( itemori, "north" )
                && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          *turnoutstate_out = 1;
          return oriNorth;
        } else if( travel == 1 &&  StrOp.equals( itemori, "west" )
                && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          *turnoutstate_out = 1;
          return oriWest;
        } else if( travel == 1 &&  StrOp.equals( itemori, "west" )
                && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          *turnoutstate_out = 1;
          return oriEast;
        } else if( travel == 2 &&  StrOp.equals( itemori, "south" )
               && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          *turnoutstate_out = 1;
          return oriNorth;
        } else if( travel == 2 &&  StrOp.equals( itemori, "south" )
               && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          *turnoutstate_out = 1;
          return oriSouth;
        } else if( travel == 3 &&  StrOp.equals( itemori, "east" )
              && StrOp.equals( wItem.gettype(item), "right" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          *turnoutstate_out = 1;
          return oriEast;
        } else if( travel == 3 &&  StrOp.equals( itemori, "east" )
              && StrOp.equals( wItem.gettype(item), "left" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " coming DIVERGE" );
          *turnoutstate_out = 1;
          return oriWest;
        }

        /* crossing */
        else if( StrOp.equals( wItem.gettype(item), "crossing" ) ) {
          //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " crossing %d", wSwitch.isdir(item) );

          if( !wSwitch.isdir(item)  ) { // left

            if( StrOp.equals( itemori, "west" ) || StrOp.equals( itemori, "east" )) {
              if( (travel == 1) || (travel == 2)) {
                *x = 1;
                return travel;
              } else if ( travel == 3 ) {
                *x = -1;
                return travel;
              }
              return travel;
            }
            else if( StrOp.equals( itemori, "north" ) || StrOp.equals( itemori, "south" )) {
              if( (travel == 0) ) {
                *y = -1;
                return travel;
              } else if ( (travel == 2) || (travel == 3) ) {
                *y = 1;
                return travel;
              }
              return travel;
            }
          } else if( wSwitch.isdir(item) ) { // right
            if( StrOp.equals( itemori, "west" ) || StrOp.equals( itemori, "east" )) {
              if( (travel == 1) ) {
                *x = -1;
                return travel;
              } else if ( (travel == 3) || (travel == 2) ) {
                *x = 1;
                return travel;
              }
              return travel;
            }
            else if( StrOp.equals( itemori, "north" ) || StrOp.equals( itemori, "south" )) {
              if( (travel == 0) || (travel == 3) ) {
                *y = 1;
                return travel;
              } else if ( travel == 2 ) {
                *y = -1;
                return travel;
              }
              return travel;
            }
          }
          return itemNotInDirection;
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
        /* dcrossing */
        else if( StrOp.equals( wItem.gettype(item), "dcrossing" ) ) {
          /*TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " +++++++%d+itemori:[%s]+++++++ dcrossing travel: %d turnoutstate: %d",
              wSwitch.isdir(item),itemori,travel, turnoutstate );*/

          if( !wSwitch.isdir(item)  ) { // left
            if( StrOp.equals( itemori, "west" ) || StrOp.equals( itemori, "east" )) {
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
            else if( StrOp.equals( itemori, "north" ) || StrOp.equals( itemori, "south" )) {
              if( (travel == 0) ) {

              } else if ( travel == 2 ) {

              }
            }
          } else if( wSwitch.isdir(item) ) { // right
            if( StrOp.equals( itemori, "west" ) || StrOp.equals( itemori, "east" )) {
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
            else if( StrOp.equals( itemori, "north" ) || StrOp.equals( itemori, "south" )) {
              if( (travel == 0) ) {

              } else if ( travel == 2 ) {

              }
            }
          }

          return travel+dcrossing;
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

  return itemNotInDirection;
}


static void __analyseItem(iOAnalyse inst, iONode item, iOList route, int travel, int turnoutstate, int depth) {
  iOAnalyseData data = Data(inst);
  char key[32] = {'\0'};
  iONode nextitem = NULL;
  int blockleftcounter = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "start analyzing item [%s] travel: [%d] depth: [%d] tos: [%d]",
      wItem.getid(item), travel, depth, turnoutstate);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " -> LIST: item [%s] travel: [%d] depth: [%d] tos: [%d]",
        wItem.getid(item), travel, depth, turnoutstate);


  const char * state = "-";
  if( StrOp.equals( NodeOp.getName(item) , "sw" ) ) {
    if ( StrOp.equals(wItem.gettype(item), "right" ) || StrOp.equals(wItem.gettype(item), "left" )  ) {
    state = turnoutstate?"straight":"turnout";
    }
  } else if( StrOp.equals( NodeOp.getName(item) , "bk" )) {
    state = "start";
  }


  /* LIST */
  iONode itemA = (iONode)NodeOp.base.clone( item);
  wItem.setstate(itemA, state);
  ListOp.add( route, (obj)itemA );

  /*security*/
  if ( depth > 100)
    return;

  int x = 0;
  int y = 0;

  int xoffset = 0;
  int yoffset = 0;

  int turnoutstate_out;

  /* get next item */
  travel = __travel(item, travel, turnoutstate, &turnoutstate_out, &x, &y, "");

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

    //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "next key: %s", key);
    nextitem = (iONode)MapOp.get( data->objectmap, key);

    if( nextitem != NULL) {

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "NEXT ITEM: %s TOS: [%d]",
          NodeOp.getName(nextitem), turnoutstate_out );

      int travelp = __travel(nextitem, travel, turnoutstate, &turnoutstate_out, &x, &y, "");
      if( travelp == itemNotInDirection || travelp == -1) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "STOP NOT IN DIR" );
        return;
      }


      if( StrOp.equals(NodeOp.getName(nextitem) , "bk" ) ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "next is a block: [%s]", wItem.getid(nextitem));

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " -> LIST: item [%s] travel: [%d] depth: [%d] tos: [%d]",
                wItem.getid(nextitem), travel, depth, turnoutstate);

        /* LIST */
        iONode itemA = (iONode)NodeOp.base.clone( nextitem);
        wItem.setstate(itemA, "stop");
        ListOp.add( route, (obj)itemA );

        /* add route to routelist */
        ListOp.add( data->prelist, (obj)route);

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "     ");


        //TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "route: [[%s][%s]]", wItem.getid(item), wItem.getid(nextitem));
        return;
      } else if( StrOp.equals(NodeOp.getName(nextitem) , "sw" ) ) {

         /*int travelp = __travel(nextitem, travel, turnoutstate, &turnoutstate_out, &x, &y, ""); */
       TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "next is a switch: [%s] state: [%d] travelp: [%d]",
            wItem.getid(nextitem), turnoutstate_out, travelp);

       turnoutstate = turnoutstate_out;

        /* item is a turnout -> coming from the points: dive into branches */
        if( travelp >= 200 && travelp < 300) {
          travelp -= twoWayTurnout;
          depth++;

          /* clone the route */
          iOList routecloneA = (iOList)ListOp.base.clone( route);

          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "going into STRAIGHT branch [%s]", wItem.getid(nextitem));
          __analyseItem(inst, nextitem, route, travel, 0, depth);
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "going into TURNOUT branch [%s]", wItem.getid(nextitem));
          __analyseItem(inst, nextitem, routecloneA, travel, 1, depth);

          return;
        } else if( travelp >= 300 && travelp < 400) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "THREE WAY");
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "     " );
          travelp -= threeWayTurnout;
          depth++;
          __analyseItem(inst, nextitem, route, travelp, 0, depth);
          __analyseItem(inst, nextitem, route, travelp, 1, depth);
          __analyseItem(inst, nextitem, route, travelp, 2, depth);
          return;
        } else if( travelp >= 400 && travelp < 500) {

          travelp -= dcrossing;
          depth++;

          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "DCROSSING travel: %d travelp: %d", travel, travelp );

          if( travelp == 0) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into 0 branch [%s]", wItem.getid(nextitem));
            __analyseItem(inst, nextitem, route, travelp, 0, depth);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into 2 branch [%s]", wItem.getid(nextitem));
            __analyseItem(inst, nextitem, route, travelp, 2, depth);
            return;
          } else if ( travelp == 1) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into 1 branch [%s]", wItem.getid(nextitem));
            __analyseItem(inst, nextitem, route, travelp, 1, depth);
            int branch = 2;
            if( wSwitch.isdir(nextitem) ) // right
              branch = 3;
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into %d branch [%s]", branch, wItem.getid(nextitem));
            __analyseItem(inst, nextitem, route, travelp, branch, depth);
            return;
          } else if ( travelp == 2) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into 0 branch [%s]", wItem.getid(nextitem));
            __analyseItem(inst, nextitem, route, travelp, 0, depth);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into 3 branch [%s]", wItem.getid(nextitem));
            __analyseItem(inst, nextitem, route, travelp, 3, depth);
            return;
          } else if ( travelp == 3) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into 1 branch [%s]", wItem.getid(nextitem));
            __analyseItem(inst, nextitem, route, travelp, 1, 0);
            int branch = 3;
            if( wSwitch.isdir(nextitem)  ) // right
              branch = 2;
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "-- > going into %d branch [%s]", branch, wItem.getid(nextitem));
            __analyseItem(inst, nextitem, route, travelp, branch, depth);
            return;
          }

        }

      } // if bk || sw


      depth++;
      __analyseItem(inst, nextitem, route, travel, turnoutstate, depth);


    } else { /*item==NULL*/

      /* TODO: delete route*/

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "return");
      return;
    } /*item?NULL*/

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
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "--------------------------------------------------");
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

    iOList route = ListOp.inst();

    /* start the recursion */
    __analyseItem(inst, block, route, travel, 0, 0);

}

static void __analyseList(iOAnalyse inst) {
  iOAnalyseData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " ");
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "the analyzer found the routes:");

  iOList routelist = (iOList)ListOp.first( data->prelist );
  while(routelist) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "route:");
    iONode item = (iONode)ListOp.first( routelist );
    while(item) {

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        " [%s][%s][%s]", NodeOp.getName(item),
        wItem.getid(item), wItem.getstate(item) );


      item = (iONode)ListOp.next( routelist );
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " ");
    routelist = (iOList)ListOp.next( data->prelist );
  }


  /*
  iONode item = (iONode)ListOp.first( data->prelist );

    while(item) {

      if( StrOp.equals(NodeOp.getName(item) , "bk" ) ) {

        if ( StrOp.equals(wSwitch.getstate(item) , "start" ) ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, " ");
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Route:");
        }

      } /*else if ( StrOp.equals(NodeOp.getName(item) , "sw" )) {
        if ( !StrOp.equals(wSwitch.getstate(item) , "-" ) ) {
                  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "     subroute: [%s] is [%s]", wItem.getid(item), wSwitch.getstate(item));
                }
      }*/

  /*
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          " [%s][%s][%s]", NodeOp.getName(item),
          wItem.getid(item), wItem.getstate(item) );


      prevItemId = wItem.getid(item);
      item = (iONode)ListOp.next( data->prelist );
    }
    */
}


static void _analyse(iOAnalyse inst) {
  iOAnalyseData data = Data(inst);
  //iOList bklist = NULL;
  iONode block = NULL;
  int cx, cy;
  int zlevel = 0;

  MapOp.clear(data->objectmap);
  ListOp.clear(data->prelist);
  ListOp.clear(data->bklist);

  iONode modplan = data->model->getModPlan( data->model);
  if( modplan == NULL) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "not a modplan" );

    iOList list = data->model->getLevelItems( data->model, 0, &cx, &cy, True);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "Trackplan: %d objects at level 0 and sizes %d x %d", ListOp.size(list), cx, cy );

    __prepare(inst, list, 0,0);
  } else {
    iONode mod = wModPlan.getmodule( modplan );
    while( mod != NULL ) {

      iOList list = data->model->getLevelItems( data->model, zlevel, &cx, &cy, True);


      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
           "preparing module: %s", wModule.gettitle( mod) );
      __prepare(inst, list, wModule.getx(mod), wModule.gety(mod));

      zlevel++;
      mod = wModPlan.nextmodule( modplan, mod );
    };
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "  it contains %d blocks", ListOp.size(data->bklist) );
  
  block = (iONode)ListOp.first(data->bklist);
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


    block = (iONode)ListOp.next(data->bklist);
  }

  __analyseList(inst);

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
  data->prelist = ListOp.inst();
  data->bklist = ListOp.inst();
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

