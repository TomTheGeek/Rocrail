/** ------------------------------------------------------------
  * $Source: /home/cvs/xspooler/rocs/impl/os2/omutex.c,v $
  * $Author: rob $
  * $Date: 2004-02-27 08:12:47 $
  * $Revision: 1.7 $
  * $Name:  $
  * ------------------------------------------------------------ */
#if defined __OS2__

#include <stdlib.h>
#include <string.h>

#define INCL_DOSSEMAPHORES
#include <os2.h>

#include "rocs/impl/mutex_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"

static const char* SEM_PREFIX = "\\\\SEM32\\";

#ifndef __ROCS_MUTEX__
	#pragma message("*** OS/2 OMutex is disabled. (define __ROCS_MUTEX__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */
Boolean rocs_mutex_create( iOMutexData o ) {
#ifdef __ROCS_MUTEX__
  char* sem_name = o->name != NULL ? StrOp.cat( SEM_PREFIX, o->name):NULL;
  int rc = DosCreateMutexSem( sem_name, (unsigned long*)&o->handle, 0, False );
  if( o->handle == NULL ) {
    o->rc = rc;
    return False;
  }
  return True;
#else
  return False;
#endif
}
Boolean rocs_mutex_open( iOMutexData o ) {
#ifdef __ROCS_MUTEX__
  char* sem_name = o->name != NULL ? StrOp.cat( SEM_PREFIX, o->name):NULL;
  int rc = DosOpenMutexSem( sem_name, (unsigned long*)&o->handle );
  if( o->handle == NULL ) {
    o->rc = rc;
    return False;
  }
  return True;
#else
  return False;
#endif
}
Boolean rocs_mutex_release( iOMutexData o ) {
#ifdef __ROCS_MUTEX__
  unsigned long handle = (unsigned long)o->handle;
  int rc = 0;
  if( handle == 0 )
    return False;
  rc = DosReleaseMutexSem( handle );
  if( rc ) {
    o->rc = rc;
    return False;
  }
  return True;
#else
  return False;
#endif
}
Boolean rocs_mutex_wait( iOMutexData o, int t ) {
#ifdef __ROCS_MUTEX__
  unsigned long handle = (unsigned long)o->handle;
  int rc = 0;
  if( handle == 0 )
    return False;
  rc = DosRequestMutexSem( handle, t );
  if( rc ) {
    o->rc = rc;
    return False;
  }
  return True;
#else
  return False;
#endif
}
Boolean rocs_mutex_close( iOMutexData o ) {
#ifdef __ROCS_MUTEX__
  unsigned long handle = (unsigned long)o->handle;
  int rc = 0;
  if( handle == 0 )
    return False;
  rc = DosCloseMutexSem( handle );
  if( rc ) {
    o->rc = rc;
    return False;
  }
  return True;
#else
  return False;
#endif
}

#endif
