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
#if defined __linux__ || defined _AIX || defined __unix__ || defined __APPLE__

#include "rocs/impl/event_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/thread.h"
#include "rocs/public/map.h"
#include "rocs/public/objbase.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#ifndef __ROCS_EVENT__
	#pragma message("*** Unix OEvent is disabled. (define __ROCS_EVENT__ in rocs.h) ***")
#endif

static iOMap __eventMap = NULL;
/*
 ***** __Private functions.
 */
Boolean rocs_event_create( iOEventData o ) {
#ifdef __ROCS_EVENT__
  obj event = NULL;

  if( __eventMap == NULL ) {
    __eventMap = MapOp.inst();
  }

  if( o->name != NULL )
    event = MapOp.get( __eventMap, o->name );
  if( event != NULL )
    return False;
  else {
    if( o->name != NULL )
      MapOp.put( __eventMap, o->name, (obj)o );
    o->handle = o;
    return True;
  }
#else
  return False;
#endif
}

Boolean rocs_event_close( iOEventData o ) {
#ifdef __ROCS_EVENT__
  if( __eventMap != NULL ) {
    MapOp.remove( __eventMap, o->name );
  }
#else
  return False;
#endif
}

Boolean rocs_event_open( iOEventData o ) {
#ifdef __ROCS_EVENT__
  obj event = NULL;
  if( __eventMap == NULL )
    return False;
  event = MapOp.get( __eventMap, o->name );
  if( event == NULL )
    return False;
  else {
    o->handle = event;
    return True;
  }
#else
  return False;
#endif
}

Boolean rocs_event_set( iOEventData o ) {
#ifdef __ROCS_EVENT__
  if( o->handle == NULL )
    return False;
  else {
    iOEventData event = (iOEventData)o->handle;
    event->posted = True;
    return True;
  }
#else
  return False;
#endif
}

Boolean rocs_event_reset( iOEventData o ) {
#ifdef __ROCS_EVENT__
  if( o->handle == NULL )
    return False;
  else {
    iOEventData event = (iOEventData)o->handle;
    event->posted = False;
    return True;
  }
#else
  return False;
#endif
}

/**
 * Dirty implementation of a eventsem.
 */
Boolean rocs_event_wait( iOEventData o, int t ) {
#ifdef __ROCS_EVENT__
  if( o->handle == NULL )
    return False;
  else {
    iOEventData event = (iOEventData)o->handle;
    if( event->posted ) {
      return True;
    }
    else if( t == -1 ) {
      /* Infinite... */
      while( !event->posted ) {
        ThreadOp.sleep(10);
      }
      return True;
    }
    else {
      int slept = 0;
      while( !event->posted ) {
        if( slept >= t )
          return False;
        ThreadOp.sleep(10);
        slept += 10;
      }
      return True;
    }
  }
#else
  return False;
#endif
}

#endif
