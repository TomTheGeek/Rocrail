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
#ifdef _WIN32 

#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "rocs/impl/mutex_impl.h"
#include "rocs/public/trace.h"

#ifndef __ROCS_MUTEX__
	#pragma message("*** Win32 OMutex is disabled. (define __ROCS_MUTEX__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */
Boolean rocs_mutex_create( iOMutexData o ) {
#ifdef __ROCS_MUTEX__
  o->handle = CreateMutex( NULL, False, o->name );
  if( o->handle == NULL ) {
    o->rc = GetLastError();
    return False;
  }
#endif
  return True;
}
Boolean rocs_mutex_open( iOMutexData o ) {
#ifdef __ROCS_MUTEX__
  o->handle = OpenMutex( MUTEX_ALL_ACCESS, False, o->name );
  if( o->handle == NULL ) {
    o->rc = GetLastError();
    return False;
  }
#endif
  return True;
}
Boolean rocs_mutex_close( iOMutexData o ) {
#ifdef __ROCS_MUTEX__
  int rc = CloseHandle( o->handle );
  if( rc == 0 ) {
    o->rc = GetLastError();
    return False;
  }
#endif
  return True;
}
Boolean rocs_mutex_release( iOMutexData o ) {
#ifdef __ROCS_MUTEX__
  if( !ReleaseMutex( o->handle ) ) {
    o->rc = GetLastError();
    return False;
  }
#endif
  return True;
}
Boolean rocs_mutex_wait( iOMutexData o, int t ) {
#ifdef __ROCS_MUTEX__
  int rc = WaitForSingleObject( o->handle, t );
  if( rc == WAIT_FAILED || rc == WAIT_TIMEOUT ) {
    o->rc = GetLastError();
    return False;
  }
#endif
  return True;
}

#endif
