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

#include "rocs/public/rocsdirent.h"
#include "rocs/impl/dir_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"

/*
 ***** __Private functions.
 */
DIR * opendir( char *pchfile )
{
  DIR * p = NULL;

  p = (DIR *)allocIDMem( sizeof(DIR), RocsDirID );
  if(p)
  {
     strcpy( p->Dirent.d_name, pchfile );
     strcat( p->Dirent.d_name, "\\*.*" );
     p->openflag = 0;
  }
  return p;
}

struct dirent *readdir(DIR *dir)
{
  int done = 0;

  do {
    if (dir->openflag == 0) {
      dir->openflag = 1;
      dir->handle = _findfirst( dir->Dirent.d_name, &(dir->DirFindBlk) );
      if (dir->handle == -1L)
        done = 1;  /* nothing found */
    }
    else {
      done = _findnext( dir->handle,  &(dir->DirFindBlk) );
    }
  }
  while (dir->DirFindBlk.attrib & _A_SUBDIR && ! done);

  if (done)
    return NULL;

  strcpy( dir->Dirent.d_name, dir->DirFindBlk.name );
  return &(dir->Dirent);
}

int closedir(DIR *dir)
{
  _findclose( dir->handle );
  freeIDMem( dir, RocsDirID );
  return 0;
}

int rocs_scanDir(const char *dirname, const char* extension, iDirEntry** namelist)
{
  struct _finddata_t c_file;
  long hFile;
  int found = 0;
  int done = 0;

  char* search = StrOp.fmt( "%s%c*%s", dirname, SystemOp.getFileSeparator(), extension!=NULL?extension:".*" );

  hFile = _findfirst( search, &c_file );
  StrOp.free( search );

  if( hFile == -1L )
    return found;

  do {
    if( namelist != NULL ) {
      iDirEntry dir = allocIDMem( sizeof( struct DirEntry ), RocsDirID );
      *namelist = reallocMem( *namelist, (found+1)*sizeof(iDirEntry) );
      dir->name = StrOp.dupID( c_file.name, RocsDirID );
      dir->mtime = c_file.time_write;
      (*namelist)[found] = dir;
    }
    found++;

    done = _findnext( hFile,  &c_file );
  }
  while( !done );

  _findclose(hFile);

  return found;
}

#endif
