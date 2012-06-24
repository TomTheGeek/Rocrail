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
#ifdef _WIN32 

#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "rocs/impl/event_impl.h"
#include "rocs/public/trace.h"

#ifndef __ROCS_EVENT__
	#pragma message("*** Win32 OEvent is disabled. (define __ROCS_EVENT__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */
Boolean rocs_event_create( iOEventData o ) {
#ifdef __ROCS_EVENT__
  o->handle = CreateEvent( NULL, False, 0, o->name );
  if( o->handle == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_event_create [%s] failed. rc=%d", o->name != NULL ? o->name:"", GetLastError() );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_create OK [%s]", o->name != NULL ? o->name:"" );
#endif
  return True;
}

Boolean rocs_event_close( iOEventData o ) {
#ifdef __ROCS_EVENT__
  int rc = CloseHandle( o->handle );
  if( rc == 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_event_close [%s] failed. rc=%d", o->name != NULL ? o->name:"", GetLastError() );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_close OK [%s]", o->name != NULL ? o->name:"" );
#endif
  return True;
}

Boolean rocs_event_open( iOEventData o ) {
#ifdef __ROCS_EVENT__
  o->handle = OpenEvent( EVENT_ALL_ACCESS, True, o->name );
  if( o->handle == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_event_open [%s] failed. rc=%d", o->name != NULL ? o->name:"", GetLastError() );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_open OK [%s]", o->name != NULL ? o->name:"" );
#endif
  return True;
}

Boolean rocs_event_set( iOEventData o ) {
#ifdef __ROCS_EVENT__
  if( !SetEvent( o->handle ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_event_set [%s] failed. rc=%d", o->name != NULL ? o->name:"", GetLastError() );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_set OK [%s]", o->name != NULL ? o->name:"" );
#endif
  return True;
}

Boolean rocs_event_reset( iOEventData o ) {
#ifdef __ROCS_EVENT__
  if( !ResetEvent( o->handle ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_event_reset [%s] failed. rc=%d", o->name != NULL ? o->name:"", GetLastError() );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_reset OK [%s]", o->name != NULL ? o->name:"" );
#endif
  return True;
}

Boolean rocs_event_wait( iOEventData o, int t ) {
#ifdef __ROCS_EVENT__
  int rc = WaitForSingleObject( o->handle, t );
  if( rc == WAIT_FAILED ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_event_wait [%s] failed. rc=%d", o->name != NULL ? o->name:"", GetLastError() );
    return False;
  }
  else if( rc == WAIT_TIMEOUT ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_wait [%s] timeout. rc=%d", o->name != NULL ? o->name:"", GetLastError() );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_wait OK [%s]", o->name != NULL ? o->name:"" );
#endif
  return True;
}

#endif
