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
    else {
      /* put the object in the map */
      const char* type = wItem.gettype(node);
      __createKey( key, node, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  adding key %s for %s %s %s", 
          key, NodeOp.getName(node), type==NULL?"":type, wItem.getori(node) );
      MapOp.put( data->objectmap, key, (obj)node );
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

static int __getType(iONode item ) {
  const char* type = NodeOp.getName(item);
  const char* subtype = wItem.gettype(item);
  if( StrOp.equals( wTrack.name() , type ) ) {
    if( StrOp.equals( wTrack.straight, subtype ) ) return typeTrackStraight;
    if( StrOp.equals( wTrack.curve   , subtype ) ) return typeTrackCurve;
  }
  return -1;
}


static const int foundBlock = 100;

static int __travelWest( iONode block, iONode item ) {
  if( item ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "west of block %s is %s %s %s", 
        wBlock.getid(block), NodeOp.getName(item), wItem.gettype(item), wItem.getori(item) );
    if( __getType(item) == typeTrackStraight  && (__getOri(block) == oriWest || __getOri(block) == oriEast) ) {
      /* traveling west */
      return oriWest;
    }
    else if( __getType(item) == typeTrackCurve && __getOri(block) == oriNorth ) {
      /* traveling south */
      return oriSouth;
    }
    else if( __getType(item) == typeTrackCurve && __getOri(block) == oriEast ) {
      /* traveling north */
      return oriNorth;
    }
    else if( __getType(item) == typeBlock && (__getOri(block) == oriWest || __getOri(block) == oriEast) ) {
      /* found a block */
      return foundBlock;
    }
  }
      
  return -1;
}


static void __analyseBlock(iOAnalyse inst, iONode block ) {
  iOAnalyseData data = Data(inst);
  char key[32] = {'\0'};
  iONode item = NULL;
  int travel = -1;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "start analysing block %s", 
      wBlock.getid(block) );
  
  if( __getOri(block) == oriWest ) {
    int xoffset = 0;
    travel = oriWest;
    /* creeping west */
    do {
      xoffset--;
      __createKey( key, block, xoffset, 0, 0);
      item = (iONode)MapOp.get( data->objectmap, key);
      
      if( item ) {
      
        switch(travel) {
        case oriWest:
          travel = __travelWest(block, item);
          break;
        }
      
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "end of west direction" ); 
        break;
      }
    } while(item && travel != foundBlock );

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
    __analyseBlock(inst,block);
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

