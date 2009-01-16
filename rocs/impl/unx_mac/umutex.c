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
#if defined __linux__ || defined _AIX || defined __unix__ || defined __APPLE__

#include "rocs/impl/mutex_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/thread.h"
#include "rocs/public/map.h"
#include "rocs/public/objbase.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>


#ifndef __ROCS_MUTEX__
        #pragma message("*** Unix OMutex is disabled. (define __ROCS_MUTEX__ in rocs.h) ***")
#endif

/*
 ***** __Private functions.
 */
Boolean rocs_mutex_create( iOMutexData o ) {
#ifdef __ROCS_MUTEX__
  int rc = 0;

  o->mh = allocIDMem( sizeof( pthread_mutex_t ), RocsMutexID );
  o->rc = pthread_mutex_init( (pthread_mutex_t*)o->mh, NULL );
  if( o->rc != 0 ) {
    return False;
  }

  o->handle = o;

#endif
  return True;
}

Boolean rocs_mutex_open( iOMutexData o ) {
#ifdef __ROCS_MUTEX__
  printf( "umutex.c: rocs_mutex_open NOT SUPPORTED\n" );
  return False;
#else
  return False;
#endif
}

Boolean rocs_mutex_release( iOMutexData o ) {
#ifdef __ROCS_MUTEX__
  iOMutexData data = o->handle;
  o->rc = pthread_mutex_unlock( (pthread_mutex_t*)data->mh );
  if( o->rc != 0 ) {
    return False;
  }
#endif
  return True;
}

Boolean rocs_mutex_close( iOMutexData o ) {
#ifdef __ROCS_MUTEX__
  iOMutexData data = o->handle;
  o->rc = pthread_mutex_destroy( (pthread_mutex_t*)data->mh );
  freeIDMem( data->mh, RocsMutexID );
  data->mh = NULL;
  if( o->rc != 0 ) {
    return False;
  }
#endif
  return True;
}

Boolean rocs_mutex_wait( iOMutexData o, int t ) {
#ifdef __ROCS_MUTEX__
  iOMutexData data = o->handle;
  int rc = 0;
  if( t == -1 ) {
    rc = pthread_mutex_lock( (pthread_mutex_t*)data->mh );
  }
  else if( rc = pthread_mutex_trylock( (pthread_mutex_t*)data->mh ) == EBUSY ) {
    int try = t / 10 + 1;
    do {
      ThreadOp.sleep( 10 );
      rc = pthread_mutex_trylock( (pthread_mutex_t*)data->mh );
      t--;
    } while( t > 0 && rc == EBUSY );
  }
  if( rc != 0 ) {
    o->rc = rc;
    return False;
  }
#endif
  return True;
}

#endif
