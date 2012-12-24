/** ------------------------------------------------------------
  * $Source: /home/cvs/xspooler/rocs/impl/os2/odir.c,v $
  * $Author: rob $
  * $Date: 2006-08-16 09:01:41 $
  * $Revision: 1.4 $
  * $Name:  $
  * ------------------------------------------------------------ */
#ifdef __OS2__ 

#include <stdlib.h>
#include <string.h>
#include <os2.h>
#include <dir.h>
#include <dos.h>

#include "rocs/public/rocsdirent.h"
#include "rocs/impl/dir_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"

/*
 ***** __Private functions.
 */
int rocs_scanDir(const char *dirname, const char* extension, iDirEntry** namelist)
{ 
  struct ffblk c_file;
  long hFile;
  int found = 0;
  int done = 0;

  char* search = StrOp.fmt( "%s%c*%s", dirname, SystemOp.getFileSeparator(), extension );
  
  done = findfirst( search, &c_file, 0 );
  StrOp.free( search );

  if( done != 0 )
    return found;

  found++;

  do {
    done = findnext( &c_file );
    if( !done )
      found++;      
  }
  while( c_file.ff_attrib & FA_DIREC && !done );

  return found;
}

#endif
