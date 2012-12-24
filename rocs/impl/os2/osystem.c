/** ------------------------------------------------------------
  * $Source: /home/cvs/xspooler/rocs/impl/os2/osystem.c,v $
  * $Author: rob $
  * $Date: 2007-02-13 14:20:52 $
  * $Revision: 1.13 $
  * $Name:  $
  * ------------------------------------------------------------ */
#if defined __OS2__

#include <stdlib.h>
#include <string.h>

#define PURE_32
#include <neterr.h>
#include <netcons.h>
#include <wksta.h>

#define INCL_DOSDATETIME
#include <os2.h>

#include "rocs/impl/system_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"

#ifndef __ROCS_SYSTEM__
	#pragma message("*** OS/2 OSystem is disabled. (define __ROCS_SYSTEM__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */
const char* rocs_system_getWSName( iOSystemData o ) {
#ifdef __ROCS_SYSTEM__
  struct wksta_info_10 *pWkstaInfo10;
  unsigned long rc = 0;
  unsigned long size = sizeof (struct wksta_info_10) + CNLEN + UNLEN + (6 * DNLEN) + 8 + 10240;

  pWkstaInfo10 = allocIDMem( size, RocsSystemID );
  rc = Net32WkstaGetInfo( NULL, 10, (PBYTE) pWkstaInfo10, size, &size );

  if( rc != NERR_Success ) {
    freeIDMem( pWkstaInfo10, RocsSystemID );
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_system_getWSName failed. rc=%d", rc );
    return NULL;
  }
  strcpy( o->WSName,   pWkstaInfo10->wki10_computername );
  strcpy( o->UserName, pWkstaInfo10->wki10_username );
  freeIDMem( pWkstaInfo10, RocsSystemID );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocs_system_getWSName OK [%s,%s]", o->WSName, o->UserName );
  return o->WSName;
#else
  return NULL;
#endif
}

const char* rocs_system_getUserName( iOSystemData o ) {
#ifdef __ROCS_SYSTEM__
  const char* wsName = rocs_system_getWSName( o );
  if( wsName == NULL )
    return NULL;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocs_system_getUserName OK [%s]", o->UserName );
  return o->UserName;
#else
  return NULL;
#endif
}

unsigned long rocs_system_getMillis( void ) {
#ifdef __ROCS_SYSTEM__
  DATETIME dt;
  DosGetDateTime( &dt );
  return dt.hundredths * 10;
#else
  return 0;
#endif
}

char* rocs_system_getURL( const char* szFile ) {
  /* ToDo!!! */
  return StrOp.dup( szFile );
}

Boolean rocs_system_setadmin() {
  /* TODO: set to admin. */
  return True;
}

int rocs_system_getTime( int* hours, int* minutes, int* seconds ) {
#ifdef __ROCS_SYSTEM__
  /* TODO: set to admin. */
  return 0;
#endif
}


#endif
