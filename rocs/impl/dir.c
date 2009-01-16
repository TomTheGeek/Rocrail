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
#include "rocs/public/str.h"
#include "rocs/public/rocsdirent.h"
#include "rocs/impl/dir_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/list.h"
#include "rocs/public/file.h"
#include "rocs/public/system.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>


static int instCnt = 0;

#ifdef _WIN32
/* OS dependent */
DIR* opendir( char *pchfile );
struct dirent *readdir(DIR *dir);
int closedir(DIR *dir);
int rocs_scanDir(const char *dirname, const char* extension, iDirEntry** namelist);
#endif /* _WIN32 */

#ifdef __OS2__
/* OS dependent */
int rocs_scanDir(const char *dirname, const char* extension);
#endif /* __OS2__ */

#ifdef __sun
int scandir(const char *dir, struct dirent ***namelist,
            int (*select)(const struct dirent *),
            int (*compar)(const struct dirent **, const struct dirent **));
#endif



static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/* ------------------------------------------------------------
 * base.name()
 */
static const char* __name(void) {
  return name;
}


/* ------------------------------------------------------------
 * base.serialize()
 */
static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}


/* ------------------------------------------------------------
 * base.deserialize()
 */
static void __deserialize(void* inst, unsigned char* a) {
}


/* ------------------------------------------------------------
 * base.toString()
 */
static char* __toString(void* inst) {
  iODirData data = Data(inst);
  return data->path;
}


/* ------------------------------------------------------------
 * base.del()
 */
static void __del(void* inst) {
  iODirData data = Data(inst);
  StrOp.freeID( data->path, RocsDirID );
  freeIDMem( data, RocsDirID );
  freeIDMem( inst, RocsDirID );
  instCnt--;
}


/* ------------------------------------------------------------
 * base.count()
 */
static int __count(void) {
  return instCnt;
}
static void* __properties(void* inst) {
  return NULL;
}
static struct OBase* __clone( void* inst ) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}


/* ------------------------------------------------------------
 * DirOp.open()
 */
static Boolean _openDir( iODir inst ) {
  iODirData data = Data(inst);
  if( data->dir != NULL ) {
    /* rewind */
    DirOp.close( inst );
  }
  data->dir = opendir( data->path );
  if( data->dir == NULL ) {
    data->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 550, data->rc, "Error on opendir(%s)",
                   data->path );
    return False;
  }
  return True;
}


/* ------------------------------------------------------------
 * DirOp.getRc()
 */
static int _getRc( iODir inst ) {
  iODirData data = Data(inst);
  return data->rc;
}


/* ------------------------------------------------------------
 * DirOp.read()
 */
static const char* _readDir( iODir inst ) {
  iODirData data = Data(inst);
  struct dirent* direntry = readdir( data->dir );
  return direntry == NULL ? NULL:direntry->d_name;
}


/* ------------------------------------------------------------
 * DirOp.close()
 */
static void _closeDir( iODir inst ) {
  iODirData data = Data(inst);
  closedir( data->dir );
}


/* ------------------------------------------------------------
 * __sort()
 */
static int __sortOldest(obj* _a, obj* _b)
{
    iDirEntry a = (iDirEntry)*_a;
    iDirEntry b = (iDirEntry)*_b;
    if( a->mtime > b->mtime )
      return 1;
    if( a->mtime < b->mtime )
      return -1;
    /* they must be equal */
    return 0;
}


static int __sortNewest(obj* _a, obj* _b)
{
    iDirEntry a = (iDirEntry)*_a;
    iDirEntry b = (iDirEntry)*_b;
    if( a->mtime < b->mtime )
      return 1;
    if( a->mtime > b->mtime )
      return -1;
    /* they must be equal */
    return 0;
}


static int __sortName(obj* _a, obj* _b)
{
    iDirEntry a = (iDirEntry)*_a;
    iDirEntry b = (iDirEntry)*_b;
    return strcmp( a->name, b->name );
}


static void _cleandirlist( iOList list ) {
  int i = 0;
  int size = ListOp.size( list );
  for( i = 0; i < size; i++ ) {
    iDirEntry dir = (iDirEntry)ListOp.get( list, i );
    StrOp.freeID( dir->name, RocsDirID );
    freeIDMem( dir, RocsDirID );
  }
  ListOp.base.del( list );
}

static iOList _listdir( const char* path, const char* extension, sortmode sort ) {
  iOList list = NULL;
#if defined _WIN32 || defined __OS2__
  /* TODO: Windows impl */
  int n = 0;
  iDirEntry *namelist = allocIDMem( sizeof(iDirEntry), RocsDirID );
  n = rocs_scanDir( path, extension, &namelist );
  list = ListOp.inst();

   while( n-- ) {
    iDirEntry dir = namelist[n];
    if( extension == NULL || StrOp.endsWith( dir->name, extension ) ) {
      ListOp.add( list, (obj)dir );
    }
  }
  freeIDMem( namelist, RocsDirID );

  if( sort == SORT_NAME )
    ListOp.sort( list, &__sortName );
  else if( sort == SORT_OLDEST )
    ListOp.sort( list, &__sortOldest );
  else if( sort == SORT_NEWEST )
    ListOp.sort( list, &__sortNewest );

  return list;
#else
  struct dirent **namelist;
  int    n = 0;
  int hits = 0;

  n = scandir( path, &namelist, 0, NULL );

  if( n < 0 ) {
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, errno, "Error on scandir(%s, %s)",
                   path, extension );
    return NULL;
  }
  else {
    list = ListOp.inst();

    while( n-- ) {
      if( extension == NULL || StrOp.endsWith( namelist[n]->d_name, extension ) ) {
        char* filepath = StrOp.fmtID( RocsDirID, "%s%c%s", path, SystemOp.getFileSeparator(), namelist[n]->d_name );
        iDirEntry dir = allocIDMem( sizeof( struct DirEntry ), RocsDirID );
        dir->name = StrOp.dupID( namelist[n]->d_name, RocsDirID );
        dir->mtime = FileOp.fileTime( filepath );

        ListOp.add( list, (obj)dir );
        StrOp.freeID( filepath, RocsDirID );
      }
      free( namelist[n] );
    }
    free( namelist );
  }
  if( list != NULL ) {
    if( sort == SORT_NAME )
      ListOp.sort( list, &__sortName );
    else if( sort == SORT_OLDEST )
      ListOp.sort( list, &__sortOldest );
    else if( sort == SORT_NEWEST )
      ListOp.sort( list, &__sortNewest );
  }
  return list;
#endif
}

/* ------------------------------------------------------------
 * DirOp.scan()
 */
static int _scandir( const char* path, const char* extension ) {
#if defined _WIN32 || defined __OS2__
  return rocs_scanDir( path, extension, NULL );
#else
  struct dirent **namelist;
  int    n = 0;
  int hits = 0;

  n = scandir( path, &namelist, 0, NULL );

  if( n < 0 ) {
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, errno, "Error on scandir(%s, %s)",
                   path, extension );
  }
  else {
    while( n-- ) {
      if( extension == NULL || StrOp.endsWith( namelist[n]->d_name, extension ) )
        hits++;
      free(namelist[n]);
    }
    free( namelist );
  }
  return hits;
#endif
}


/* ------------------------------------------------------------
 * DirOp.inst()
 */
static iODir _inst( const char* path ) {
  iODir     dir  = allocIDMem( sizeof( struct ODir ), RocsDirID );
  iODirData data = allocIDMem( sizeof( struct ODirData ), RocsDirID );


  MemOp.basecpy( dir, &DirOp, 0, sizeof( struct ODir ), data );

  data->path = StrOp.dupID( path, RocsDirID );

  instCnt++;

  if( !DirOp.open( dir ) ) {
    dir->base.del( dir );
    dir = NULL;
  }

  return dir;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/dir.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
