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

#include "rocs/impl/queue_impl.h"
#include "rocs/public/mem.h"


static int instCnt = 0;

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
  iOQueueData data = Data(inst);
  return "";
}
static void __del(void* inst) {
  iOQueueData data = Data(inst);
  data->evt->base.del( data->evt );
  data->mux->base.del( data->mux );
  freeIDMem( data, RocsQueueID );
  freeIDMem( inst, RocsQueueID );
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

static qMsg __newQMsg( obj po, q_prio prio ) {
  qMsg m = allocIDMem( sizeof( struct SqMsg ), RocsQueueID );
  m->o = po;
  m->prio = prio;
  return m;
}

static Boolean __addMsg( iOQueueData o, qMsg msg ) {
  if( o->first == NULL ) {
    /* first element in queue */
    msg->next = NULL;
    o->first = msg;
    o->last[ msg->prio ] = msg;
  }
  else {
    qMsg last = o->last[msg->prio];
    if( last != NULL ) {
      msg->next = last->next;
      last->next = msg;
      o->last[msg->prio] = msg;
    }
    else {
      /* Prio pointer == NULL; First (and last) element of this prio. */
      o->last[msg->prio] = msg;

      if( msg->prio == high ) {
        /* insert at start of queue */
        qMsg first = o->first;
        o->first = msg;
        msg->next = first;
      }
      else if( msg->prio == normal ) {
        if( o->last[high] != NULL ) {
          msg->next = o->last[high]->next;
          o->last[high]->next = msg;
          o->last[normal] = msg;
        }
      }
      else if( msg->prio == low ) {
        if( o->last[normal] != NULL ) {
          msg->next = o->last[normal]->next;
          o->last[normal]->next = msg;
          o->last[low] = msg;
        }
        else if( o->last[high] != NULL ) {
          msg->next = o->last[high]->next;
          o->last[high]->next = msg;
          o->last[low] = msg;
        }
      }
    }
  }
  o->count++;
  return True;
}

/*
 ***** _Public functions.
 */
static Boolean _post( iOQueue inst, obj po, q_prio prio ) {
  iOQueueData data = Data(inst);
  Boolean rc = False;

  if( data->count < data->size ) {
    MutexOp.wait( data->mux );
    rc = __addMsg( data, __newQMsg( po, prio ) );
    MutexOp.post( data->mux );
    EventOp.set( data->evt );
  }
  else {
    printf( "##### QueueOp.post: count(%d) is getting bigger than size(%d)!\n", data->count, data->size );
  }
  return rc;
}

static obj _get( iOQueue inst ) {
  iOQueueData data = Data(inst);
  obj qo = NULL;

  MutexOp.wait( data->mux );
  if( data->first != NULL ) {
    qMsg qm = data->first;
    qo = qm->o;
    data->first = qm->next;
    if( data->last[qm->prio] == qm ) {
      /* last element of this prio */
      data->last[qm->prio] = NULL;
    }
    freeIDMem( qm, RocsQueueID );
    data->count--;
  }
  MutexOp.post( data->mux );
  return qo;
}

static obj _waitPost( iOQueue inst ) {
  iOQueueData data = Data(inst);
  obj qo = NULL;
  int tries = 0;

  while( data->first == NULL && tries < 2 ) {
    EventOp.wait( data->evt );
    EventOp.reset( data->evt );
    tries++;
    /* data->first is not NULL before event is set... */
    if( data->first == NULL )
      printf( "##### QueueOp.waitPost has nothing after %d events?!\n", tries );
  }

  qo = _get( inst );
  /* Reset event in case of data->first is not NULL before event is set... */
  EventOp.reset( data->evt );

  return qo;
}

static Boolean _isEmpty( iOQueue inst ) {
  iOQueueData data = Data(inst);
  return data->count == 0 ? True:False;
}

static int _msgCount( iOQueue inst ) {
  iOQueueData data = Data(inst);
  return data->count;
}

static iOQueue _inst( int size ) {
  iOQueue     queue = allocIDMem( sizeof( struct OQueue ), RocsQueueID );
  iOQueueData data  = allocIDMem( sizeof( struct OQueueData ), RocsQueueID );

  MemOp.basecpy( queue, &QueueOp, 0, sizeof( struct OQueue ), data );

  data->mux = MutexOp.inst( NULL, True );
  data->evt = EventOp.inst( NULL, True );
  EventOp.reset( data->evt );
  data->size = size;

  instCnt++;

  return queue;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/queue.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
