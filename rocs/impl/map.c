/*
 Rocs - OS independent C library

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rocs/impl/map_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"


static int instCnt = 0;

/* declarations */
static obj __removeMapItem( iOMapData data, const char* key );

/*
 ***** OBase operations.
 */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static const char* __name(void) {
  return name;
}
static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}
static void __deserialize(void* inst, unsigned char* a) {
}
static char* __toString(void* inst) {
  return "";
}
static void __del(void* inst) {
  iOMapData data = Data(inst);
  MapOp.clear( inst );
  freeIDMem( data, RocsMapID );
  freeIDMem( inst, RocsMapID );
  instCnt--;
}
static int __count(void) {
  return instCnt;
}
static void* __properties(void* inst) {
  return NULL;
}
static struct OBase* __clone( void* inst ) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}


static unsigned int __hash( const char* str ) {
  unsigned int    h = 0;
  unsigned char*  p = NULL;

  for( p = (unsigned char*)str; *p != '\0'; p++ )
    h = MULTIPLIER * h + *p;

  return h % NHASH;
}


static obj __findMapItem( iOMapData data, const char* key, Boolean* keyfound ) {
  int hashVal = __hash( key );
  iOList list = data->hashTable[ hashVal ];

  /* hashMap */
  if( list != NULL ) {
    iMapItem item = (iMapItem)ListOp.first( list );
    while( item != NULL ) {
      if( StrOp.equals( item->key, key ) ) {
        if( keyfound != NULL ) *keyfound = True;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "findMapItem(): hashVal = %d, key = %s", hashVal, key );
        return item->o;
      }
      item = (iMapItem)ListOp.next( list );
    }
  }

  return NULL;
}

static void __addMapItem( iOMapData data, const char* k, obj o ) {
  iMapItem item     = NULL;
  Boolean  keyfound = False;
  int      hashVal  = __hash( k );

  /* remove object with the same key if exist */
  __findMapItem(data, k, &keyfound );
  if( keyfound ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "replace existing object with key [%s]", k );
    __removeMapItem(data, k);
  }

  /* hashMap */
  if( data->hashTable[ hashVal ] == NULL ) {
    data->hashTable[ hashVal ] = ListOp.inst();
  }
  else {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "addMapItem(): *MULTIPLE* hashVal = %d, key = %s", hashVal, k );
  }
  item = allocIDMem( sizeof( struct MapItem ), RocsMapID );
  item->key = StrOp.dupID( k, RocsMapID );
  item->o   = o;
  ListOp.add( data->hashTable[ hashVal ], (void*)item );

  data->size++;
}

static obj __removeMapItem( iOMapData data, const char* key ) {
  int hashVal = __hash( key );
  iOList list = data->hashTable[ hashVal ];

  /* hashMap */
  if( list != NULL ) {
    iMapItem item = (iMapItem)ListOp.first( list );
    while( item != NULL ) {
      obj o = NULL;
      if( StrOp.equals( item->key, key ) ) {
        o = item->o;
        ListOp.remove( list, ListOp.getIndex( list ) );
        data->size--;
        StrOp.freeID( item->key, RocsMapID );
        freeIDMem( item, RocsMapID );
        /*return o;*/
      }
      /* Cleanup if list is empty. */
      if( ListOp.size( list ) == 0 ) {
        data->hashTable[ hashVal ] = NULL;
        list->base.del( list );
        item = NULL;
      }
      else
        item = (iMapItem)ListOp.next( list );

      if( o != NULL )
        return o;
    }
  }
  return NULL;
}

/*
 ***** _Public functions.
 */
static void _clear( iOMap inst ) {
  iOMapData data = Data(inst);
  int i = 0;

  for( i = 0; i < NHASH; i++ ) {
    if( data->hashTable[i] != NULL ) {
      int n = 0;
      iOList list = data->hashTable[i];
      for( n = 0; n < ListOp.size( list ); n++ ) {
        iMapItem item = (iMapItem)ListOp.get( list, n );
        StrOp.freeID( item->key, RocsMapID );
        freeIDMem( item, RocsMapID );
      }
      list->base.del( list );
      data->hashTable[i] = NULL;
    }
  }
  memset( data->hashTable, 0, sizeof( data->hashTable ) );

  data->size = 0;
}

static int _getListCount( iOMap inst ) {
  iOMapData data = Data(inst);
  int cnt = 0;
  int   i = 0;

  for( i = 0; i < NHASH; i++ ) {
    if( data->hashTable[i] != NULL ) {
      cnt++;
    }
  }
  return cnt;
}

static void _put( iOMap inst, const char* key, obj o ) {
  iOMapData data = Data(inst);
  if( key != NULL )
    __addMapItem( data, key, o );
}

static obj _remove( iOMap inst, const char* key ) {
  iOMapData data = Data(inst);
  if( key != NULL )
    return __removeMapItem( data, key );
  else
    return NULL;
}

static obj _get( iOMap inst, const char* key ) {
  iOMapData data = Data(inst);
  if( key != NULL && StrOp.len(key) > 0 )
    return __findMapItem( data, key, NULL );
  else
    return NULL;
}

static Boolean _haskey( iOMap inst, const char* key ) {
  iOMapData data = Data(inst);
  Boolean keyfound = False;
  if( key != NULL )
    __findMapItem( data, key, &keyfound );

  return keyfound;
}

static obj _first( iOMap inst ) {
  iOMapData data = Data(inst);
  int i = 0;
  for( i = 0; i < NHASH; i++ ) {
    if( data->hashTable[i] != NULL ) {
      iMapItem item = (iMapItem)ListOp.first( data->hashTable[i] );
      if( item != NULL ) {
        data->index = i;
        return item->o;
      }
    }
  }
  return NULL;
}

static obj _next( iOMap inst ) {
  iOMapData data = Data(inst);
  iMapItem item = (iMapItem)ListOp.next( data->hashTable[data->index] );
  if( item != NULL ) {
    return item->o;
  }
  else {
    int i = 0;
    for( i = data->index + 1; i < NHASH; i++ ) {
      if( data->hashTable[i] != NULL ) {
        item = (iMapItem)ListOp.first( data->hashTable[i] );
        if( item != NULL ) {
          data->index = i;
          return item->o;
        }
      }
    }
  }
  return NULL;
}


static iOList _getList( iOMap inst ) {
  iOMapData data = Data(inst);
  iOList list = ListOp.inst();
  obj o = _first( inst );
  while( o != NULL ) {
    ListOp.add( list, o );
    o = _next( inst );
  };
  return list;
}


static int _size( iOMap inst ) {
  iOMapData data = Data(inst);
  return data->size;
}

static iOMap _inst(void) {
  iOMap     map  = allocIDMem( sizeof( struct OMap ), RocsMapID );
  iOMapData data = allocIDMem( sizeof( struct OMapData ), RocsMapID );

  MemOp.basecpy( map, &MapOp, 0, sizeof( struct OMap ), data );

  instCnt++;

  return map;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/map.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
