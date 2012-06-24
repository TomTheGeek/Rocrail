/*
 Rocs - OS independent C library

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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

#include "rocs/impl/list_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"


static int instCnt = 0;

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
  iOListData data = Data(inst);
  freeIDMem( data->objList, RocsListID );
  freeIDMem( data, RocsListID );
  freeIDMem( inst, RocsListID );
  instCnt--;
}
static int __count(void) {
  return instCnt;
}
static void* __properties(void* inst) {
  return NULL;
}

static struct OBase* __clone( void* inst ) {
  iOList list = ListOp.inst();
  int i = 0;
  for( i = 0; i < ListOp.size(inst); i++) {
    ListOp.add(list, ListOp.get(inst, i));
  }
   return list;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void __resizeList( iOListData data ) {
  if( data->size + 1 > data->allocsize ) {
    int newsize = data->size + LIST_MINSIZE;
    data->objList = reallocMem( data->objList, newsize * sizeof( obj ) );
    data->allocsize = newsize;
  }
  else if( data->size < data->allocsize - LIST_MINSIZE && data->size >= 2*LIST_MINSIZE ) {
    int newsize = data->allocsize - LIST_MINSIZE;
    data->objList = reallocMem( data->objList, newsize * sizeof( obj ) );
    data->allocsize = newsize;
  }
}

/*
 ***** OList operations.
 */
static void _add( iOList inst, obj o ) {
  iOListData data = Data(inst);
  data->size++;
  __resizeList(data);
  data->objList[ data->size - 1 ] = o;
}

static void _insert( iOList inst, int pos, obj o ) {
  iOListData data = Data(inst);
  if( pos > data->size || pos < 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "insert list out of range: %d > %d", pos, data->size );
    return;
  }
  else if( pos == data->size ) {
    _add( inst, o );
    return;
  }
  else {
    int i = 0;
    data->size++;
    __resizeList(data);
    for( i = data->size; i >= pos; i-- )
      data->objList[i] = data->objList[i-1];
    data->objList[ pos ] = o;
  }
}

static obj _remove( iOList inst, int pos ) {
  iOListData data = Data(inst);
  if( pos > data->size || pos < 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "remove list out of range: %d > %d", pos, data->size );
    return NULL;
  }
  else {
    obj ro = data->objList[pos];
    int i = 0;
    for( i = pos; i < data->size; i++ )
      data->objList[i] = data->objList[i+1];
    data->size--;
    __resizeList(data);
    return ro;
  }
}


static obj _removeObj( iOList inst, obj object ) {
  iOListData data = Data(inst);
  obj ro = NULL;
  int i = 0;
  for( i = 0; i < data->size; i++ ) {
    if( data->objList[i] == object ) {
      ro = data->objList[i];
      ListOp.remove( inst, i );
      break;
    }
  }
  return ro;
}


static void _clear( iOList inst ) {
  while( ListOp.size( inst ) > 0 ) {
    ListOp.remove( inst, 0 );
  };
}


static obj _get( iOList inst, int pos ) {
  iOListData data = Data(inst);
  if( pos > data->size-1 || pos < 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "get list out of range: %d > %d", pos, data->size );
    return NULL;
  }
  else {
    return data->objList[pos];
  }
}

static obj _first( iOList inst ) {
  iOListData data = Data(inst);
  if( data->size == 0 ) {
    return NULL;
  }
  else {
    data->ix = 0;
    return data->objList[data->ix];
  }
}

static obj _next( iOList inst ) {
  iOListData data = Data(inst);
  if( data->size == 0 ) {
    return NULL;
  }
  else if( data->ix + 1 < data->size ) {
    data->ix += 1;
    return data->objList[data->ix];
  }
  else {
    return NULL;
  }
}

static int _getIndex( iOList inst ) {
  iOListData data = Data(inst);
  return data->ix;
}

static int _size( iOList inst ) {
  iOListData data = Data(inst);
  return data->size;
}

static void _replace( iOList inst, int pos, obj o ) {
  iOListData data = Data(inst);
  if( pos >= data->size || pos < 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "replace list out of range: %d >= %d", pos, data->size );
    return;
  }
  else {
    data->objList[ pos ] = o;
  }
}

typedef int (*qsort_compar)(const void *, const void *);

static void _sort( iOList inst, comparator comp ) {
  if( inst != NULL ) {
    iOListData data = Data(inst);
    int size = ListOp.size( inst );
    int i = 0;
    int moved = 0;
    void** objlist;
    
    if( size < 2 )
      return;
    
    /* init index: */  
    objlist = allocIDMem( size * sizeof(void*), RocsListID );
    for( i = 0; i < size; i++ ) {
      objlist[i] = (void*)ListOp.get( inst, i );
    }
  
    qsort( (void *)objlist, (size_t)size, sizeof(void*), (qsort_compar)comp );
    
    /* copy objects in the new list */
    ListOp.clear( inst );
    for( i = 0; i < size; i++ )
      ListOp.add( inst, objlist[i] ); 
    
    /* cleanup index */
    freeIDMem( objlist, RocsListID );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "inst == NULL" );
  }
}

static iOList _inst(void) {
  iOList     list = allocIDMem( sizeof( struct OList ), RocsListID );
  iOListData data = allocIDMem( sizeof( struct OListData ), RocsListID );

  MemOp.basecpy( list, &ListOp, 0, sizeof( struct OList ), data );

  data->objList     = allocIDMem( sizeof( obj ) * LIST_MINSIZE, RocsListID );
  data->allocsize   = LIST_MINSIZE;

  instCnt++;

  return list;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/list.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
