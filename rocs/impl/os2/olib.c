/** ------------------------------------------------------------
  * $Source: /home/cvs/xspooler/rocs/impl/os2/olib.c,v $
  * $Author: rob $
  * $Date: 2004-02-27 08:12:47 $
  * $Revision: 1.9 $
  * $Name:  $
  * ------------------------------------------------------------ */
#if defined __OS2__

#include <stdlib.h>
#include <string.h>

#define INCL_DOSMODULEMGR
#include <os2.h>

#include "rocs/impl/lib_impl.h"
#include "rocs/public/trace.h"

#ifndef __ROCS_LIB__
	#pragma message("*** OS/2 OLib is disabled. (define __ROCS_LIB__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */
Boolean rocs_lib_load( iOLibData o ) {
#ifdef __ROCS_LIB__
  int rc = 0;
  char szError[256] = {'\0'};

  if( strstr( o->name, ".dll" ) == NULL )
    strcat( o->name, ".dll" );

  rc = DosLoadModule( szError, sizeof szError, o->name, &o->lh );
  if( rc ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_lib_load [%s] failed. rc=%d %s", o->name != NULL ? o->name:"", rc, szError );
    return False;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocs_lib_load OK [%s]", o->name != NULL ? o->name:"" );
  return True;
#else
  return False;
#endif
}

const void* rocs_lib_getProc( iOLib inst, const char* procname ) {
#ifdef __ROCS_LIB__
  iOLibData o = Data(inst);
  void* proc = NULL;
  unsigned long handle = (unsigned long)o->lh;
  int rc = DosQueryProcAddr( handle, 0, procname, &proc );
  if( rc ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_lib_getProc [%s.%s] failed. rc=%d", o->name, procname, rc );
    return NULL;
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocs_lib_getProc OK [%s.%s]", o->name, procname );
  return proc;
#else
  return NULL;
#endif
}

#endif
