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
#include "rocs/impl/thread_impl.h"
#include "rocs/public/map.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/mutex.h"
#include "rocs/public/trace.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int instCnt = 0;

static iOMap     threadMap = NULL;
static iOMutex threadMutex = NULL;


/* OS dependent */
Boolean rocs_thread_start( iOThread inst );
void rocs_thread_sleep( int ms );
unsigned long rocs_thread_id(void);
void rocs_thread_kill( iOThread inst );
Boolean rocs_thread_join( iOThread inst );
void rocs_thread_high( iOThread inst );


static void __addThread( iOThread o ) {
  if( threadMutex == NULL ) {
    threadMutex = MutexOp.inst( NULL, True );
  }
  if( threadMap == NULL ) {
    threadMap = MapOp.inst();
  }
  if( threadMap == NULL || threadMutex == NULL )
    return;
  MutexOp.wait( threadMutex );
  if( !MapOp.haskey(threadMap, Data(o)->tname) ) {
    MapOp.put( threadMap, Data(o)->tname, (obj)o );
  }
  else {
    TraceOp.println( "A named thread, [%s], already exist!", Data(o)->tname );
  }

  MutexOp.post( threadMutex );
}

static void __removeThread( iOThread inst ) {
  if( threadMap == NULL || threadMutex == NULL )
    return;
  else {
    obj o = NULL;
    if( MutexOp.wait( threadMutex ) ) {
      o = MapOp.remove( threadMap, Data(inst)->tname );
      MutexOp.post( threadMutex );
      if( o == NULL )
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "NOT FOUND!!! __removeThread(%s)", Data(inst)->tname );
    }
  }
}


/*
 ***** OBase functions.
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
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    if( data->tdesc != NULL )
      return StrOp.dup( data->tdesc );
    else if( data->tname != NULL )
      return StrOp.dup( data->tname );
  }
  return NULL;
}
static void __del(void* inst) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    __removeThread( inst );
    data->queue->base.del( data->queue );
    StrOp.freeID( data->tname, RocsThreadID );
    StrOp.freeID( data->tdesc, RocsThreadID );
    freeIDMem( data, RocsThreadID );
    freeIDMem( inst, RocsThreadID );
    instCnt--;
  }
  else
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL !" );
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


/*
 ***** OThread public functions.
 */
static const char* _getName( iOThread inst ) {
  if( inst == NULL )
    return "?";
  else
    return Data(inst)->tname;
}

static void _quit( iOThread inst ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    data->quit = True;
  }
}

static void _pause( iOThread inst, Boolean pause ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    data->pause = pause;
  }
}

static Boolean _isQuit( iOThread inst ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    return data->quit;
  }
  return False;
}

static Boolean _isPause( iOThread inst ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    return data->pause;
  }
  return False;
}

static void _quitAll( void ) {
}

static iOList _getAll( void ) {
  iOList thList = ListOp.inst();
  if( threadMap != NULL && threadMutex != NULL ) {
    obj o = NULL;
    MutexOp.wait( threadMutex );
    o = MapOp.first( threadMap );
    while( o != NULL ) {
      ListOp.add( thList, o );
      o = MapOp.next( threadMap );
    }
    MutexOp.post( threadMutex );
  }
  return thList;
}

static iOThread _find( const char* name ) {
  if( threadMap != NULL && threadMutex != NULL ) {
    obj o = NULL;
    MutexOp.wait( threadMutex );
    o = MapOp.first( threadMap );
    while( o != NULL ) {
      if( StrOp.equals( Data(o)->tname, name ) ) {
        MutexOp.post( threadMutex );
        return (iOThread)o;
      }
      o = MapOp.next( threadMap );
    }
    MutexOp.post( threadMutex );
  }
  return NULL;
}

static Boolean _post( iOThread inst, obj msg ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "queue[%s][%s] items %d", data->tname, data->tdesc, QueueOp.count(data->queue) );
    return QueueOp.post( data->queue, msg, normal );
  }
  return False;
}

static Boolean _prioPost( iOThread inst, obj msg, int prio ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    return QueueOp.post( data->queue, msg, prio );
  }
  return False;
}

static obj _getPost( iOThread inst ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    return QueueOp.get( data->queue );
  }
  return NULL;
}

static obj _waitPost( iOThread inst ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    return QueueOp.waitPost( data->queue );
  }
  return NULL;
}

static iOThread _findById( unsigned long id ) {
  if( threadMap != NULL && threadMutex != NULL ) {
    obj o = NULL;
    MutexOp.wait( threadMutex );
    o = MapOp.first( threadMap );
    while( o != NULL ) {
      if( Data(o)->id == id ) {
        MutexOp.post( threadMutex );
        return (iOThread)o;
      }
      o = MapOp.next( threadMap );
    }
    MutexOp.post( threadMutex );
  }
  return NULL;
}

static void* _getParm( iOThread inst ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    return data->parm;
  }
  return NULL;
}

static void _kill( iOThread inst ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    rocs_thread_kill( inst );
  }
}

static Boolean _join( iOThread inst ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    return rocs_thread_join( inst );
  }
  return False;
}

static Boolean _start( iOThread inst ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    return rocs_thread_start( inst );
  }
  return False;
}

static void _setStacksize( iOThread inst, long size ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    data->stacksize = size;
  }
}

static void _setHigh( iOThread inst ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    rocs_thread_high( inst );
  }
}

static void _setDescription( iOThread inst, const char* desc ) {
  if( inst != NULL ) {
    iOThreadData data = Data(inst);
    data->tdesc = StrOp.dupID( desc, RocsThreadID );
    QueueOp.setDesc(data->queue, desc);
  }
}

static iOThread _inst( const char* tname, thread_run run, void* parm ) {
  iOThread     thread = allocIDMem( sizeof( struct OThread ), RocsThreadID );
  iOThreadData data   = allocIDMem( sizeof( struct OThreadData ), RocsThreadID );

  MemOp.basecpy( thread, &ThreadOp, 0, sizeof( struct OThread ), data );

  data->queue = QueueOp.inst( 1000 );
  data->parm  = parm;

  if( tname == NULL )
    data->tname = StrOp.fmtID( RocsThreadID, "tid0x%08X", thread );
  else
    data->tname = StrOp.dupID( tname, RocsThreadID );

  data->run = run;

  instCnt++;
  __addThread( thread );

  return thread;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/thread.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
