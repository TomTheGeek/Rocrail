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

#include "rocs/impl/lib_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/str.h"

#ifndef __ROCS_LIB__
	#pragma message("*** Win32 OLib is disabled. (define __ROCS_LIB__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */
Boolean rocs_lib_load( iOLibData o ) {
#ifdef __ROCS_LIB__
  if( !StrOp.endsWith( o->name, ".dll" ) )
    o->name = StrOp.catID( o->name, ".dll", RocsLibID );

  o->lh = LoadLibrary( o->name );
  if( (unsigned int)o->lh <= 32 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_lib_load [%s] failed. rc=%d", o->name != NULL ? o->name:"", GetLastError() );
    return False;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocs_lib_load OK [%s]", o->name != NULL ? o->name:"" );
#endif
  return True;
}

const void* rocs_lib_getProc( iOLib inst, const char* procname ) {
  void* proc = NULL;
#ifdef __ROCS_LIB__
  iOLibData o = Data(inst);
  proc = GetProcAddress( o->lh, procname );
  if( proc == NULL || IsBadCodePtr(proc) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_lib_getProc [%s:%s] failed. rc=%d", o->name, procname, GetLastError() );
    return NULL;
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocs_lib_getProc OK [%s:%s]", o->name, procname );
#endif
  return proc;
}

#endif
