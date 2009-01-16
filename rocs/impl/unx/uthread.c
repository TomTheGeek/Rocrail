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
#if defined __linux__ || defined _AIX || defined __unix__

#include "rocs/impl/thread_impl.h"
#include "rocs/public/trace.h"

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <signal.h>
#include <errno.h>

#include <pthread.h>
#include <sched.h>


#ifndef __ROCS_THREAD__
	#pragma message("*** Unix OThread is disabled. (define __ROCS_THREAD__ in rocs.h) ***")
#endif

static void* rocs_thread_wrapper( void* inst ) {
#ifdef __ROCS_THREAD__
  iOThreadData o = Data(inst);
  o->id = pthread_self();
  o->run( inst );
#endif
}

Boolean rocs_thread_start( iOThread inst ) {
#ifdef __ROCS_THREAD__
  iOThreadData o = Data(inst);
  pthread_attr_t attr;
  int rc = 0;
  int stacksize = 64 * 4096;
  memset( &attr, 0, sizeof( pthread_attr_t ) );
  rc = pthread_attr_init( &attr );
  if( rc != 0 )
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "pthread_attr_init rc=%d", rc );
  else {
    /* Set as detached: Cleaningup will be done by system after thread end. */
    /* NOT JOINABLE ! */
    rc = pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );
    if( rc != 0 )
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "pthread_attr_setdetachstate rc=%d", rc );

    /* Set stacksize to 256k or minimal 64k. */
    if( o->stacksize >= 16 * 4096 )
      stacksize = o->stacksize;
    rc = pthread_attr_setstacksize( &attr, stacksize );
    if( rc != 0 )
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "pthread_attr_setstacksize rc=%d", rc );

    rc = pthread_create( (pthread_t*)&o->handle, &attr, rocs_thread_wrapper, (void*)inst);
    if( rc != 0 )
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "pthread_create rc=%d", rc );

  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_thread_start rc=%d", rc );
  return rc == 0 ? True:False;
#endif
}

void rocs_thread_sleep( const int ms ) {
#ifdef __ROCS_THREAD__
  if( ms >= 1000 ) {
    sleep( ms / 1000 ); /* seconds */
    usleep((ms%1000) * 1000); /* microseconds */
  }
  else
    usleep(ms * 1000); /* microseconds */
#endif
}

unsigned long rocs_thread_id(void) {
#ifdef __ROCS_THREAD__
  return pthread_self();
#else
  return 0;
#endif
}

void rocs_thread_kill( iOThread inst ) {
#ifdef __ROCS_THREAD__
  iOThreadData o = Data(inst);
  /*pthread_kill( o->handle, SIGABRT );*/
  pthread_cancel( o->handle );
#endif
}

void rocs_thread_high( iOThread inst ) {
#ifdef __ROCS_THREAD__
/* Linux only: needs super user priv's. */
#if defined __linux__
  iOThreadData o = Data(inst);
  if( o != NULL && o->handle != 0 ) {
    int rc = 0;
    struct sched_param param;
    int policy = 0;
    char* szPolicy = "";

    /* Set to RoundRobbin: the process needs superuser privileges */
    policy = SCHED_RR;
    param.__sched_priority = 10; /* valid range = 1...99*/

    rc = pthread_setschedparam( o->handle, policy, &param );
    if( rc == 0 )
      TraceOp.terrno( name, TRCLEVEL_DEBUG, __LINE__, 9999, rc, "Setted thread priority to %d", param.__sched_priority );
    else {
      TraceOp.terrno( name, TRCLEVEL_WARNING, __LINE__, 9999, rc, "Could not set thread priority to %d (only superuser can do this)", param.__sched_priority );
    }
    rc = pthread_getschedparam( o->handle, &policy, &param);
    if( policy == SCHED_FIFO ) szPolicy = "SCHED_FIFO";
    else if( policy == SCHED_RR ) szPolicy = "SCHED_RR";
    else szPolicy = "SCHED_OTHER";
    TraceOp.terrno( name, TRCLEVEL_INFO, __LINE__, 9999, rc, "Current thread priority is %d policy=%s", param.__sched_priority, szPolicy );

  }
#endif
#endif
}

Boolean rocs_thread_join( iOThread inst ) {
#ifdef __ROCS_THREAD__
  iOThreadData o = Data(inst);
  int rc = 0;
  if( o != NULL && o->handle != 0 )
    rc = pthread_join( o->handle, NULL );
  if( rc == ESRCH )
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "pthread_join rc=%d", rc );
  else if( rc != 0 )
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "pthread_join rc=%d", rc );
  return rc == 0 ? True:False;
#endif
}

#endif
