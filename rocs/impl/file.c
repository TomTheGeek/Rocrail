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
#include "rocs/impl/file_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <utime.h>

#if defined _AIX
#define _POSIX_SOURCE
#include <sys/mode.h>
#endif

#if defined _WIN32 || defined __OS2__
#include <direct.h>
#include <io.h>
#endif

#if defined __unix__ || defined __linux__ || defined _AIX || defined __hpux
  #include <unistd.h>
#endif

static int instCnt = 0;

/*
 ***** OBase operations.
 */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static const char* __name(void) {
  return name;
}
static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}
static void __deserialize(void* inst, unsigned char* a) {
}
static char* __toString(void* inst) {
  iOFileData data = Data(inst);
  return data->path;
}
static void __del(void* inst) {
  if( inst != NULL ) {
    iOFileData data = Data(inst);
    FileOp.close( inst );
    StrOp.freeID( data->path, RocsFileID );
    freeIDMem( data, RocsFileID );
    freeIDMem( inst, RocsFileID );
    instCnt--;
  }
}
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


/** ------------------------------------------------------------
  * Converts all path seperators to the appropriated ones.
  * There is no new string allocated for the corected path, but
  * the 'bad' seperators are overwritten.
  *
  * @param path Path to convert/correct.
  */
static void _convertPath2OSType( const char* path ) {
  if( path != NULL ) {
    char  sepOK = SystemOp.getFileSeparator();
    char sepBad = sepOK;
    char*  pSep = NULL;
    int     len = StrOp.len( path );
    int       i = 0;

    /* Get the bad seperator. */
    if( SystemOp.getOSType() == OSTYPE_DOS )
      sepBad = SystemOp.getPathSeparator4OS( OSTYPE_UNIX );
    else if( SystemOp.getOSType() == OSTYPE_UNIX )
      sepBad = SystemOp.getPathSeparator4OS( OSTYPE_DOS );

    /* Find and replace the bad seperators. */
    pSep = strchr( path, sepBad );
    for( i = 0; i < len && pSep != NULL; i++ ) {
      *pSep = sepOK;
      pSep = strchr( pSep, sepBad );
    }

  }
}


/*
 ***** OFile operations.
 */
static Boolean __openFile( iOFileData data ) {
  if( data->path == NULL )
    return False;
  else {
    char* a = "wba";
    switch( data->openflag ) {
      case OPEN_WRITE:
        a = "wba";
        break;
      case OPEN_READWRITE:
        a = "rb+";
        break;
      case OPEN_APPEND:
        a = "ab";
        break;
      case OPEN_READONLY:
        a = "rb";
        break;
    }

    _convertPath2OSType( data->path );
    data->fh = fopen( data->path, a );
    data->rc = errno;
    if( data->fh == NULL ) {
      TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 500, data->rc, "Error open file [%s] [%s]",
                     data->path, a );
    }
    return data->fh != NULL ? True:False;
  }
}

/*
 ***** OFile public operations.
 */
static Boolean _makeDir( const char* dirname ) {
  int rc = 0;
  iOStrTok st = NULL;
  char* dir = NULL;
  _convertPath2OSType( dirname );
  st = StrTokOp.inst( dirname, SystemOp.getFileSeparator() );

  while( StrTokOp.hasMoreTokens( st ) ) {
    const char* tok = StrTokOp.nextToken( st );
    if( dir == NULL ) {
      dir = StrOp.fmtID( RocsFileID, "%s", tok );
    }
    else {
      char* p = dir;
      dir = StrOp.fmtID( RocsFileID, "%s%c%s", dir, SystemOp.getFileSeparator(), tok );
      StrOp.freeID( p, RocsFileID );
    }
    if( StrOp.len( dir ) == 0 )
      continue;

    #if defined _WIN32 || defined __OS2__
    rc = mkdir( dir );
    #elif defined __hpux
    rc = mkdir( dir, S_IRUSR|S_IWUSR|S_IXUSR );
    #else
    /* defined __linux__ || defined _AIX */
    rc = mkdir( dir, S_IRWXU );
    #endif
    if( rc != 0 && errno == EEXIST )
      rc = 0;
    if( rc != 0 ) {
      TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 511, errno, "Error mkdir() [%s]",
                     dir );
    }

  };
  StrOp.freeID( dir, RocsFileID );
  StrTokOp.base.del( st );
  return rc == 0 ? True:False;
}

static Boolean _rmDir( const char* dirname ) {
  int rc = 0;
  _convertPath2OSType( dirname );
  rc = rmdir( dirname );
  if( rc != 0 ) {
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 511, errno, "Error rmdir() [%s]",
                   dirname );
  }
  return rc == 0 ? True:False;
}

static Boolean _cd( const char* dirname ) {
  _convertPath2OSType( dirname );
  return chdir( dirname ) == 0 ? True:False;
}

static Boolean _cp( const char* src, const char* dst ) {
  Boolean     ok = False;
  int buffersize = 1024 * 1024;
  byte*   buffer = NULL;

  /* Correct file separators. */
  _convertPath2OSType( src );
  _convertPath2OSType( dst );

  /* Allocate buffer for copy. */
  buffer = allocIDMem( buffersize, RocsFileID );

  if( buffer != NULL && FileOp.exist( src ) ) {
    iOFile srcFile = FileOp.inst( src, OPEN_READONLY );
    iOFile dstFile = FileOp.inst( dst, OPEN_WRITE );
    if( srcFile != NULL && dstFile != NULL ) {
      long    readed = 0;
      long   written = 0;
      long     fsize = FileOp.size( srcFile );
      long readtotal = 0;

      do {
        long toread = buffersize;

        if( readtotal + buffersize > fsize )
          toread = fsize - readtotal;

        ok = FileOp.read( srcFile, (char*)buffer, toread );
        readed = FileOp.getReaded( srcFile );
        readtotal += readed;

        if( ok && readed > 0 ) {
          ok = FileOp.write( dstFile, (char*)buffer, readed );
          written += FileOp.getWritten( dstFile );
        }

      } while( ok && fsize > written );

    }
    FileOp.base.del( srcFile );
    FileOp.base.del( dstFile );

  }

  freeIDMem( buffer, RocsFileID );

  return ok;
}

static char* _pwd( void ) {
  char wd[1024] = {0};
  #if defined _WIN32
  _getcwd( wd, 1024 );
  #elif defined __OS2__
  getcwd( wd, MAXPATH );
  #else
  /* defined __unix__ || defined __linux__ || defined _AIX */
  getcwd( wd, 1024 );
  #endif
  return StrOp.dup( wd );
}

static Boolean _remove( const char* filename ) {
  int rc = 0;
  _convertPath2OSType( filename );
  rc = remove( filename );
  if( rc != 0 )
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 3020, errno, "Error remove file [%s]", filename );
  return rc == 0 ? True:False;
}


/* ------------------------------------------------------------
 * ripPath()
 */
static const char* _ripPath( const char* filepath ) {
  const char* p   = filepath;
  const char* tmp = p;

  _convertPath2OSType( filepath );
  while( tmp ) {
    tmp = StrOp.findc( p, SystemOp.getFileSeparator() );
    if( tmp )
      p = tmp + 1;
  }
  return p;
}


/* ------------------------------------------------------------
 * getPath()
 */
static char* _getPath( const char* p_filepath ) {
  char* filepath = NULL;
  char*        p = NULL;
  char*      tmp = NULL;
  char*      sep = NULL;

  _convertPath2OSType( filepath );

  if( StrOp.findc( p_filepath, SystemOp.getFileSeparator() ) == NULL ) {
    filepath = StrOp.dup( "." );
    return filepath;
  }
  else {
    filepath = StrOp.dup( p_filepath );
    p = filepath;
    tmp = p;
  }

  while( tmp ) {
    tmp = StrOp.findc( p, SystemOp.getFileSeparator() );
    if( tmp ) {
      sep = tmp;
      p = tmp + 1;
    }
  }
  if( sep != NULL )
    *sep = '\0';

  return filepath;
}


static Boolean _accessCheck( const char* filename ) {
  int rc = 0;
  _convertPath2OSType( filename );
  #if defined _WIN32 || defined __OS2__
    rc = access( filename, 4 );
  #else
  /* defined __unix__ || defined __linux__ || defined _AIX */
    rc = access( filename, R_OK );
  #endif
  if( rc != 0 )
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 510, errno, "Error get status information of [%s]",
                   filename );
  return rc == 0 ? True:False;
}

static Boolean _exist( const char* filename ) {
  int rc = 0;
  _convertPath2OSType( filename );
  #if defined _WIN32 || defined __OS2__
    rc = access( filename, 0 );
  #else
  /* defined __unix__ || defined __linux__ || defined _AIX */
    rc = access( filename, F_OK );
  #endif
  if( rc != 0 )
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 510, "Error get status information of [%s][%d]",
                   filename, errno );
  return rc == 0 ? True:False;
}

static Boolean __renamef( const char* filename, const char* newName, Boolean force ) {
  int rc = 0;
  _convertPath2OSType( filename );
  _convertPath2OSType( newName );
  if( force && FileOp.exist( newName ) ) {
    FileOp.remove( newName );
  }
  rc = rename( filename, newName );
  if( rc != 0 )
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 3015, errno, "Error rename file [%s] to [%s]",
                   filename, newName );
  return rc == 0 ? True:False;
}


static Boolean _rename( const char* filename, const char* newName ) {
  return __renamef( filename, newName, False );
}


static Boolean _forcerename( const char* filename, const char* newName ) {
  return __renamef( filename, newName, True );
}


static long _fileSize( const char* filename ) {
#if defined _WIN32
  struct _stat aStat;
  _convertPath2OSType( filename );
  if( _stat( filename, &aStat ) == 0 )
#else
  struct stat aStat;
  _convertPath2OSType( filename );
  if( stat( filename, &aStat ) == 0 )
#endif
    return aStat.st_size;
  else
    return 0;
}


static Boolean _isRegularFile( const char* filename ) {
#if defined _WIN32
  struct _stat aStat;
  _convertPath2OSType( filename );
  if( _stat( filename, &aStat ) == 0 )
    return ((aStat.st_mode & S_IFREG) == S_IFREG) ? True:False;
#elif defined _AIX || defined __hpux
  struct stat aStat;
  _convertPath2OSType( filename );
  if( stat( filename, &aStat ) == 0 )
    return ((aStat.st_mode & _S_IFREG) == _S_IFREG) ? True:False;/*_S_IFREG*/
#elif defined __OS2__ || defined __sun
  struct stat aStat;
  _convertPath2OSType( filename );
  if( stat( filename, &aStat ) == 0 )
    return ((aStat.st_mode & S_IFREG) == S_IFREG) ? True:False;/*S_IFREG*/
#else
  struct stat aStat;
  _convertPath2OSType( filename );
  if( stat( filename, &aStat ) == 0 )
    return ((aStat.st_mode & S_IFREG) == S_IFREG) ? True:False;/*__S_IFREG*/
#endif
  else
    return False;
}


static Boolean _isDirectory( const char* filename ) {
#if defined _WIN32
  struct _stat aStat;
  _convertPath2OSType( filename );
  if( _stat( filename, &aStat ) == 0 )
    return ((aStat.st_mode & S_IFDIR) == S_IFDIR) ? True:False;
#elif defined _AIX || defined __hpux
  struct stat aStat;
  _convertPath2OSType( filename );
  if( stat( filename, &aStat ) == 0 )
    return ((aStat.st_mode & S_IFDIR) == S_IFDIR) ? True:False;/*_S_IFREG*/
#elif defined __OS2__ || defined __sun
  struct stat aStat;
  _convertPath2OSType( filename );
  if( stat( filename, &aStat ) == 0 )
    return ((aStat.st_mode & S_IFDIR) == S_IFDIR) ? True:False;/*S_IFREG*/
#else
  struct stat aStat;
  _convertPath2OSType( filename );
  if( stat( filename, &aStat ) == 0 )
    return ((aStat.st_mode & S_IFDIR) == S_IFDIR) ? True:False;/*__S_IFREG*/
#endif
  else
    return False;
}


static Boolean _isAbsolute( const char* path ) {
#if defined _WIN32 || defined __OS2__
  if( path[1] == ':' )
    return True;
#else
  if( path[0] == '/'  )
    return True;
#endif
  return False;
}


static long _fileTime( const char* filename ) {
#if defined _WIN32
  struct _stat aStat;
  _convertPath2OSType( filename );
  if( _stat( filename, &aStat ) == 0 )
#else
  struct stat aStat;
  _convertPath2OSType( filename );
  if( stat( filename, &aStat ) == 0 )
#endif
    return aStat.st_mtime;
  else {
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, errno, "Error stat file [%s]",
                   filename );
    return 0;
  }
}


static Boolean _setfileTime( const char* filename, long filetime ) {
#if defined _WIN32
  struct _utimbuf aTimeBuf;
  aTimeBuf.actime  = filetime;
  aTimeBuf.modtime = filetime;
  _convertPath2OSType( filename );
  if( _utime( filename, &aTimeBuf ) == 0)
#else
  struct utimbuf aTimeBuf;
  aTimeBuf.actime  = filetime;
  aTimeBuf.modtime = filetime;
  _convertPath2OSType( filename );
  if( utime( filename, &aTimeBuf ) == 0 )
#endif
    return True;
  else {
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, errno, "Error utime file [%s]",
                   filename );
    return False;
  }
}


static long _size( iOFile inst ) {
  iOFileData data = Data(inst);
#if defined _WIN32
  struct _stat aStat;
  if( _fstat( fileno(data->fh), &aStat ) == 0 ) {
#else
  struct stat aStat;
  if( fstat( fileno(data->fh), &aStat ) == 0 ) {
#endif
    data->rc = errno;
    data->size = aStat.st_size;
    return aStat.st_size;
  }
  else {
    data->rc = errno;
    return 0;
  }
}


static Boolean _closeFile( iOFile inst ) {
  iOFileData data = Data(inst);
  if( data->fh == NULL )
    return False;
  else {
    int   rc = fclose( data->fh );
    data->rc = errno;
    data->fh = NULL;

    if( rc != 0 ) {
      TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 505, data->rc, "Error close file [%s]",
                     data->path );
    }

    return rc == 0 ? True:False;
  }
}

static Boolean _flushFile( iOFile inst ) {
  iOFileData data = Data(inst);
  if( data->fh == NULL )
    return False;
  else {
    int rc = fflush( data->fh );
    data->rc = errno;
    return rc == 0 ? True:False;
  }
}

static Boolean _readFile( iOFile inst, char* buffer, long size ) {
  iOFileData data = Data(inst);
  data->readed = 0;
  if( data->fh == NULL )
    return False;
  else {
    data->readed = fread( buffer, 1, size, data->fh );
    data->rc = errno;
    if( data->readed != size && data->rc!= 0 ) {
      TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 501, data->rc, "Error read file [%s]",
                     data->path );
    }
    return data->readed == size ? True:False;
  }
}


static Boolean _append( iOFile inst, const char* buffer, long size ) {
  iOFileData data = Data(inst);
  int rc = fseek( data->fh, 0, SEEK_END );
  return FileOp.write( inst, buffer, size );
}

static Boolean _writeFile( iOFile inst, const char* buffer, long size ) {
  iOFileData data = Data(inst);
  data->written = 0;
  if( data->fh == NULL ) {
    /* printf( "##### file.c _writeFile() data->fh == NULL\n" ); */
    return False;
  }
  else {
    data->written = fwrite( buffer, 1, size, data->fh );
    data->rc = errno;
    if( data->written != size ) {
      TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 502, data->rc, "Error write file [%s]",
                     data->path );
    }
    return data->written == size ? True:False;
  }
}

static Boolean _writeStr( iOFile inst, const char* buffer ) {
  iOFileData data = Data(inst);
  long s = StrOp.len( buffer );
  return _writeFile( inst, buffer, s );
}

static Boolean _fmtFile( iOFile inst, const char* fmt, ... ) {
  iOFileData data = Data(inst);
  int rc = 0;
  va_list args;

  if( data->fh == NULL )
    return False;

  va_start(args, fmt);
  rc = vfprintf(data->fh, fmt, args);
  va_end(args);

  if( rc < 0 ) {
    data->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 502, data->rc, "Error write file [%s]",
                   data->path );
  }
  return data->rc == 0 ? True:False;
}

static void _setFilename( iOFile inst, const char* path ) {
  iOFileData data = Data(inst);
  _closeFile( inst );
  StrOp.freeID( data->path, RocsFileID );
  data->path = StrOp.dupID( path, RocsFileID );
  __openFile(data);
}

static const char* _getFilename( iOFile inst ) {
  iOFileData data = Data(inst);
  return data->path;
}

static Boolean _reopen( iOFile inst, Boolean truncate ) {
  iOFileData data = Data(inst);
  if( data->fh != NULL )
    fclose( data->fh );
  data->fh = fopen( data->path, truncate ? "w+":"a+" );
  data->rc = errno;
  if( data->fh == NULL ) {
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 500, data->rc, "Error open file [%s] [%s]",
                   data->path, truncate ? "w+":"a+" );
  }
  return data->fh != NULL ? True:False;
}

static int _getRc( iOFile inst ) {
  iOFileData data = Data(inst);
  return data->rc;
}

static Boolean _rewind( iOFile inst ) {
  iOFileData data = Data(inst);
  int rc = fseek( data->fh, 0, SEEK_SET );
  data->rc = errno;
  return rc == 0 ? True:False;
}

static Boolean _setpos( iOFile inst, long pos ) {
  iOFileData data = Data(inst);
  int rc = fseek( data->fh, pos, SEEK_SET );
  data->rc = errno;
  return rc == 0 ? True:False;
}

static long _getpos( iOFile inst ) {
  iOFileData data = Data(inst);
  long pos = ftell( data->fh );
  data->rc = errno;
  return pos;
}

static long _getReaded( iOFile inst ) {
  iOFileData data = Data(inst);
  return data->readed;
}

static long _getWritten( iOFile inst ) {
  iOFileData data = Data(inst);
  return data->written;
}



static char* ms_fuser = NULL;
static char* ms_fuserusage = NULL;

static Boolean _isAccessed( const char* filename ) {
  if( ms_fuser == NULL )
    ms_fuser = StrOp.dupID( "fuser", RocsFileID );
  if( ms_fuserusage == NULL )
    ms_fuserusage = StrOp.dupID( "R", RocsFileID );

  if( StrOp.equals( "R", ms_fuserusage ) ){
    char* cmd = StrOp.fmtID( RocsFileID, "%s %s", ms_fuser, filename );
    int rc = SystemOp.system( cmd, False );
    StrOp.freeID( cmd, RocsFileID );
    return rc == 0 ? True:False;
  }
  else if( StrOp.equals( "F", ms_fuserusage ) ) {
    char*     cmd = NULL;
    char*       f = NULL;
    Boolean inuse = False;

    f   = StrOp.fmtID( RocsFileID, "%s_fuser.out", FileOp.ripPath( filename ) );
    cmd = StrOp.fmtID( RocsFileID, "%s %s > %s", ms_fuser, filename, f );

    SystemOp.system( cmd, False );

    inuse = FileOp.fileSize( f ) > 1 ? True:False;

    /* Remove the temp file. */
    if( !inuse )
      FileOp.remove( f );

    /* Cleanup */
    StrOp.freeID( f, RocsFileID );
    StrOp.freeID( cmd, RocsFileID );

    return inuse;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                   "Unknown option for <fuserusage> [%s]", ms_fuserusage );
    return False;
  }
}

static void _setFuser( const char* fuser ) {
  if( ms_fuser != NULL )
    StrOp.freeID( ms_fuser, RocsFileID );
  ms_fuser = StrOp.dupID( fuser, RocsFileID );
}

static void _setFuserUsage( const char* fuserusage ) {
  if( ms_fuserusage != NULL )
    StrOp.freeID( ms_fuserusage, RocsFileID );
  ms_fuserusage = StrOp.dupID( fuserusage, RocsFileID );
}


static FILE* _getStream( iOFile inst ) {
  iOFileData data = Data(inst);
  return data->fh;
}


static iOFile _inst( const char* path, int openflag ) {
  iOFile     file = allocIDMem( sizeof( struct OFile ), RocsFileID );
  iOFileData data = allocIDMem( sizeof( struct OFileData ), RocsFileID );


  MemOp.basecpy( file, &FileOp, 0, sizeof( struct OFile ), data );

  data->openflag = openflag;
  data->path     = StrOp.dupID( path, RocsFileID );

  if( !__openFile(data) ) {
    file->base.del( file );
    file = NULL;
  }
  else {
    instCnt++;
  }

  return file;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/file.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
