/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2010 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocrail/impl/location_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/Location.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOLocationData data = Data(inst);
    /* Cleanup data->xxx members...*/
    ListOp.base.del(data->arriveList);
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
  iOLocationData data = Data(inst);
  return data->props;
}

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OLocation ----- */


/**  */
static struct OLocation* _inst( iONode ini ) {
  iOLocation __Location = allocMem( sizeof( struct OLocation ) );
  iOLocationData data = allocMem( sizeof( struct OLocationData ) );
  MemOp.basecpy( __Location, &LocationOp, 0, sizeof( struct OLocation ), data );

  /* Initialize data->xxx members... */
  data->props  = ini;
  data->minocc = wLocation.getminocc(ini);
  data->fifo   = wLocation.isfifo(ini);
  data->arriveList = ListOp.inst();

  instCnt++;
  return __Location;
}


/**  */
static Boolean _isDepartureAllowed( struct OLocation* inst ,const char* LocoId ) {
  iOLocationData data = Data(inst);
  int i = 0;
  if( data->minocc > 0 ) {
    for( i = 0; i < ListOp.size(data->arriveList); i++ ) {
      if( StrOp.equals( LocoId, (const char*)ListOp.get( data->arriveList, i ) ) ) {
        if( ListOp.size(data->arriveList) >= data->minocc ) {
          if( data->fifo && i == 0 ) {
            ListOp.remove( data->arriveList, i);
            return True;
          }
          else if( data->fifo && i > 0 ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "loco %s is not first in the list for FiFo", LocoId );
            return False;
          }
          else {
            ListOp.remove( data->arriveList, i);
            return True;
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
              "loco %s must wait: MinOcc=%d Occ=%d", LocoId, data->minocc, ListOp.size(data->arriveList) );
          return False;
        }
      }
    }
  }
  return True;
}


/**  */
static void _locoDidArrive( struct OLocation* inst ,const char* LocoId ) {
  iOLocationData data = Data(inst);
  int i = 0;
  for( i = 0; i < ListOp.size(data->arriveList); i++ ) {
    if( StrOp.equals( LocoId, (const char*)ListOp.get( data->arriveList, i ) ) ) {
      /* already in the list */
      return;
    }
  }
  ListOp.add( data->arriveList, (obj)LocoId);
}


/**  */
static void _locoDidDepart( struct OLocation* inst ,const char* LocoId ) {
  iOLocationData data = Data(inst);
  int i = 0;
  for( i = 0; i < ListOp.size(data->arriveList); i++ ) {
    if( StrOp.equals( LocoId, (const char*)ListOp.get( data->arriveList, i ) ) ) {
      ListOp.remove( data->arriveList, i);
      return;
    }
  }
}


/**  */
static void _modify( struct OLocation* inst ,iONode mod ) {
  iOLocationData data = Data(inst);
  data->minocc = wLocation.getminocc(mod);
  data->fifo   = wLocation.isfifo(mod);
  NodeOp.mergeNode( data->props, mod, True, True, True );
}


/**  */
static void _reset( struct OLocation* inst ) {
  iOLocationData data = Data(inst);
  ListOp.clear(data->arriveList);
  return;
}



static Boolean _hasBlock( iOLocation inst, const char* blockid ) {
  iOLocationData data = Data(inst);
  /* iterrate location: */
  iOStrTok blocks = StrTokOp.inst( wLocation.getblocks( data->props ), ',' );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "check if block [%s] is in location [%s](%s)", blockid, wLocation.getid(data->props), wLocation.getblocks( data->props ));
  while( StrTokOp.hasMoreTokens( blocks ) ) {
    const char* locationBlock = StrTokOp.nextToken( blocks );
    if( StrOp.equals( blockid, locationBlock ) ) {
      StrTokOp.base.del( blocks );
      return True;
    }
  }
  StrTokOp.base.del( blocks );
  return False;
}





/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/location.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
