/** ------------------------------------------------------------
  * $Source: /home/cvs/xspooler/rocs/impl/os2/oevent.c,v $
  * $Author: rob $
  * $Date: 2004-02-27 08:12:47 $
  * $Revision: 1.10 $
  * $Name:  $
  * ------------------------------------------------------------ */
#if defined __OS2__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INCL_DOSSEMAPHORES
#include <os2.h>

#include "rocs/impl/event_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/str.h"

static const char* SEM_PREFIX = "\\\\SEM32\\";

#ifndef __ROCS_EVENT__
	#pragma message("*** OS/2 OEvent is disabled. (define __ROCS_EVENT__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */
Boolean rocs_event_create( iOEventData o ) {
#ifdef __ROCS_EVENT__
  char* sem_name = o->name != NULL ? StrOp.fmt( "%s%s", SEM_PREFIX, o->name):NULL;
  int rc = DosCreateEventSem( sem_name, (unsigned long*)&o->handle, 0, 0 );
  if( rc != 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_event_create [%s] failed. rc=%d", o->name != NULL ? o->name:"", rc );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_create OK [%s]", o->name != NULL ? o->name:"" );
  return True;
#else
  return False;
#endif
}

Boolean rocs_event_close( iOEventData o ) {
#ifdef __ROCS_EVENT__
  unsigned long handle  = (unsigned long)o->handle;
  int rc = DosCloseEventSem( handle );
  if( rc != 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_event_close [%s] failed. rc=%d", o->name != NULL ? o->name:"", rc );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_close OK [%s]", o->name != NULL ? o->name:"" );
  return True;
#else
  return False;
#endif
}

Boolean rocs_event_open( iOEventData o ) {
#ifdef __ROCS_EVENT__
  char* sem_name = o->name != NULL ? strcat( SEM_PREFIX, o->name):NULL;
  int rc = DosOpenEventSem( sem_name, (unsigned long*)&o->handle );
  if( rc != 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_event_open [%s] failed. rc=%d", o->name != NULL ? o->name:"", rc );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_open OK [%s]", o->name != NULL ? o->name:"" );
  return True;
#else
  return False;
#endif
}

Boolean rocs_event_set( iOEventData o ) {
#ifdef __ROCS_EVENT__
  unsigned long handle  = (unsigned long)o->handle;
  int rc = DosPostEventSem( handle );
  if( rc != 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_event_set [%s] failed. rc=%d", o->name != NULL ? o->name:"", rc );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_set OK [%s]", o->name != NULL ? o->name:"" );
  return True;
#else
  return False;
#endif
}

Boolean rocs_event_reset( iOEventData o ) {
#ifdef __ROCS_EVENT__
  unsigned long handle  = (unsigned long)o->handle;
  unsigned long postcnt = 0;
  int rc = DosResetEventSem( handle, &postcnt );
  if( rc != 0 && rc != 300 ) /* ERROR_ALREADY_RESET */ {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_event_reset [%s] failed. rc=%d", o->name != NULL ? o->name:"", rc );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_reset OK [%s]", o->name != NULL ? o->name:"" );
  return True;
#else
  return False;
#endif
}

Boolean rocs_event_wait( iOEventData o, int t ) {
#ifdef __ROCS_EVENT__
  unsigned long handle  = (unsigned long)o->handle;
  int rc = DosWaitEventSem( handle, t );
  if( rc != 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_event_wait [%s] failed. rc=%d", o->name != NULL ? o->name:"", rc );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_event_wait OK [%s]", o->name != NULL ? o->name:"" );
  return True;
#else
  return False;
#endif
}

#endif
