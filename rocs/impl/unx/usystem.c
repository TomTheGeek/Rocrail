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

#include "rocs/impl/system_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#if defined __APPLE__
  /*#include </Developer/SDKs/MacOSX10.5.sdk/System/Library/Frameworks/Kernel.framework/Versions/A/Headers/architecture/i386/io.h>*/
#else
  #include <sys/io.h>
#endif
#include <unistd.h>

#ifndef __ROCS_SYSTEM__
	#pragma message("*** Unix OSystem is disabled. (define __ROCS_SYSTEM__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */
const char* rocs_system_getWSName( iOSystemData o ) {
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "rocs_system_* NOT IMPLEMENTED" );
  return "";
}

const char* rocs_system_getUserName( iOSystemData o ) {
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "rocs_system_* NOT IMPLEMENTED" );
  return "";
}

int rocs_system_getMillis( void ) {
#ifdef __ROCS_SYSTEM__
  struct timeval tp;
  gettimeofday( &tp, NULL );
  return tp.tv_usec / 1000;
#else
  return 0;
#endif
}

Boolean rocs_system_uBusyWait( int us) {
#ifdef __ROCS_SYSTEM__
//   struct   timeval start_tv, stop_tv;
//   struct   timezone start_tz, stop_tz;
//   int   slept;

//   gettimeofday(&start_tv, &start_tz);
//   do {
//     gettimeofday(&stop_tv, &stop_tz);
//     slept = ((stop_tv.tv_sec - start_tv.tv_sec) * 1000000 +
//			  (stop_tv.tv_usec - start_tv.tv_usec));
//   } while (slept < us);
struct timespec rqtp = { 0, us * 1000 };
while (nanosleep(&rqtp, &rqtp)==-1);
return True;
#endif
}

int rocs_system_getTime( int* hours, int* minutes, int* seconds ) {
#ifdef __ROCS_SYSTEM__
  struct timeval tp;
  struct tm* t;
  time_t tval;
  gettimeofday( &tp, NULL );
  tval = tp.tv_sec;
  t = localtime( &tval );
  if( hours   != NULL ) *hours = t->tm_hour;
  if( minutes != NULL ) *minutes = t->tm_min;
  if( seconds != NULL ) *seconds = t->tm_sec;
  return tp.tv_usec / 1000;
#else
  return 0;
#endif
}

char* rocs_system_getURL( const char* szFile ) {
  /* ToDo!!! */
  return StrOp.dup( szFile );
}

Boolean rocs_system_setadmin() {
  setuid(0);
  return (getuid()==0)?True:False;
}

Boolean rocs_system_accessPort( int from, int num ) {
#if defined __APPLE__
  return False;
#else
	return ioperm(from, num, 1) == 0 ? True:False;
#endif
}

Boolean rocs_system_releasePort( int from, int num ) {
#if defined __APPLE__
  return False;
#else
  return ioperm(from, num, 0) == 0 ? True:False;
#endif
}

void rocs_system_writePort( int port, byte val ) {
#if defined __APPLE__
  return;
#else
  outb(val,port);
#endif
}

byte rocs_system_readPort( int port ) {
#if defined __APPLE__
  return 0;
#else
  return inb(port);
#endif
}

Boolean rocs_system_accessDev( const char* device, Boolean readonly ) {
  return access( device, R_OK|(readonly?0:W_OK) ) == 0 ? True:False;
}


long rocs_system_openDevice( const char* devname ) {
  return 0;
}

Boolean rocs_system_writeDevice( long h, char* buffer, int size ) {
  return False;
}

Boolean rocs_system_readDevice( long h, char* buffer, int size ) {
  return False;
}

int rocs_system_availDevice( long h ) {
  return 0;
}



#endif
