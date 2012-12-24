/** ------------------------------------------------------------
  * $Source: /home/cvs/xspooler/rocs/impl/os2/othread.c,v $
  * $Author: rob $
  * $Date: 2006-09-19 08:42:58 $
  * $Revision: 1.15 $
  * $Name:  $
  * ------------------------------------------------------------ */
#if defined __OS2__

#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <stddef.h>

#define INCL_DOSPROCESS
#include <os2.h>

#include "rocs/impl/thread_impl.h"

static const unsigned long stackSize = 64 * 1024;


#ifndef __ROCS_THREAD__
	#pragma message("*** OS/2 OThread is disabled. (define __ROCS_THREAD__ in rocs.h) ***")
#endif

static void rocs_thread_wrapper( void* inst ) {
#ifdef __ROCS_THREAD__
  iOThreadData o = Data(inst);
  o->run( inst );
#endif
}

Boolean rocs_thread_start( iOThread inst ) {
#ifdef __ROCS_THREAD__
  iOThreadData o = Data(inst);
  unsigned long stacksize = stackSize;
  if( o->stacksize > 0 )
    stacksize = o->stacksize;
  o->id = _beginthread( rocs_thread_wrapper, stacksize, (void *)inst );
  return o->id == -1 ? False:True;
#endif
}

void rocs_thread_sleep( const int ms ) {
#ifdef __ROCS_THREAD__
  DosSleep( ms );
#endif
}

unsigned long rocs_thread_id(void) {
#ifdef __ROCS_THREAD__
  return _threadid;
#else
  return 0;
#endif
}

void rocs_thread_kill( iOThread inst ) {
#ifdef __ROCS_THREAD__
  iOThreadData o = Data(inst);
  DosKillThread( o->id );
#endif
}

Boolean rocs_thread_join( iOThread inst ) {
#ifdef __ROCS_THREAD__
  iOThreadData o = Data(inst);
  DosWaitThread( &o->id, DCWW_WAIT );
  return True;
#endif
}

void rocs_thread_high( iOThread inst ) {
#ifdef __ROCS_THREAD__
  iOThreadData o = Data(inst);
  DosSetPriority (PRTYS_THREAD, PRTYC_TIMECRITICAL, 0, o->id);
#endif
}

#endif
