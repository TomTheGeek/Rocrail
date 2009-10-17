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
#include "rocs/public/thread.h"
#include "rocs/impl/trace_impl.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/ebcdic.h"
#include "rocs/public/system.h"
#include "rocs/public/file.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <sys/stat.h>

#if defined __unix__ || defined __linux__ || defined _AIX || defined __hpux
  #include <unistd.h>
#endif


static int instCnt = 0;

static iOTrace traceInst = NULL;

static unsigned long mainThreadId = 0;

#define TRACELEN 4096

/*
 ***** _objbase functions.
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
  return "";
}
static void __del(void* inst) {
  if( inst != NULL ) {
    iOTraceData data = Data(inst);
    if( inst == traceInst ) {
      StrOp.freeID( data->appID, RocsTraceID );
      freeIDMem( data, RocsTraceID );
      freeIDMem( inst, RocsTraceID );
      traceInst = NULL;
      instCnt--;
    }
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


/*
 ***** __Private member functions.
 */
static char* __createNumberedFileName( int nr, const char* filename ) {
  return StrOp.fmt( "%s.%03d.trc", filename, nr );
}


static long __sizeF( FILE* fh ) {
#if defined _WIN32
  struct _stat aStat;
  if( _fstat( fileno(fh), &aStat ) == 0 ) {
#else
  struct stat aStat;
  if( fstat( fileno(fh), &aStat ) == 0 ) {
#endif
    return aStat.st_size;
  }
  else {
    /*printf( "TraceOp.__size errno=%d\n", errno );*/
    return 0;
  }
}

static long __fileTime( const char* filename ) {
#if defined _WIN32
  struct _stat aStat;
  if( _stat( filename, &aStat ) == 0 )
#else
  struct stat aStat;
  if( stat( filename, &aStat ) == 0 )
#endif
    return aStat.st_mtime;
  else {
    /*printf( "TraceOp.__fileTime errno=%d\n", errno );*/
    return 0;
  }
}

static Boolean __exist( const char* filename ) {
  int rc = 0;
  #if defined _WIN32 || defined __OS2__
    rc = access( filename, 0 );
  #else
  /* defined __unix__ || defined __linux__ || defined _AIX */
    rc = access( filename, F_OK );
  #endif
  return rc == 0 ? True:False;
}

static int __nextTraceFile( iOTraceData t ) {
  int i = 0;
  char* newfileName = NULL;
  long filetime = 0;
  int oldest = 0;
  for( i = 0; i < t->nrfiles; i++ ) {
    newfileName = __createNumberedFileName( i, t->file );
    if( __exist( newfileName ) ) {
      if( filetime == 0 ) {
        filetime = __fileTime( newfileName );
        oldest = i;
      }
      else if( __fileTime( newfileName ) < filetime ) {
        filetime = __fileTime( newfileName );
        oldest = i;
      }
    }
    else {
      oldest = i;
      i = t->nrfiles; /* break */
    }
    StrOp.free( newfileName );
  }
  return oldest;
}

static void __checkFilesize( iOTraceData t ) {
  if( t->nrfiles > 1 && t->currentfilename != NULL ) {
    long size = __sizeF( t->trcfile );
    /* Convert size in KB. */
    size = size / 1024;
    if( size >= t->filesize ) {
      char* newfileName = NULL;
      newfileName = __createNumberedFileName( __nextTraceFile(t), t->file );
      fclose( t->trcfile );
      t->trcfile = fopen( newfileName, "wba" );
      StrOp.free( t->currentfilename );
      t->currentfilename = newfileName;
    }
  }
}

static void __writeFile( iOTraceData t, char* msg, Boolean err ) {

  if( MutexOp.wait( t->mux ) ) {

    if( t->trcfile != NULL ) {
      /* Check filesize. */
      __checkFilesize( t );
      fwrite( msg, 1, StrOp.len( msg ), t->trcfile );
      fwrite( "\n", 1, StrOp.len( "\n" ), t->trcfile );
      fflush( t->trcfile );
      /*
      if( FileOp.write( t->trcfile, msg, StrOp.len( msg ) ) )
        FileOp.flush( t->trcfile );
      */
    }

    MutexOp.post( t->mux );
  }

  if( t->toStdErr ) {
    fputs( msg , err?stderr:stdout );
    fputs( "\n", err?stderr:stdout );
  }
}


static void __writeExceptionFile( iOTraceData t, char* msg ) {

  if( MutexOp.wait( t->mux ) ) {
    char* filename = StrOp.fmt( "%s.exc", t->file );
    FILE* excFile = fopen( filename, "r" );
    if( excFile != NULL ) {
      fclose( excFile );
      excFile = fopen( filename, "ab" );
      fwrite( msg, 1, StrOp.len( msg ), excFile );
      fclose( excFile );
      MutexOp.post( t->mux );
    }
    else {
      FILE* excFile = fopen( filename, "ab" );
      if( excFile != NULL ) {
        fwrite( msg, 1, StrOp.len( msg ), excFile );
        fclose( excFile );
      }
      MutexOp.post( t->mux );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Invoking: [%s]...", t->invoke );
      SystemOp.system( t->invoke, t->invokeasync, False );
    }
    StrOp.free( filename );
  }

}


static char __level( int level ) {
  char clevel = '?';

  if( level == TRCLEVEL_EXCEPTION )
    clevel = 'E';
  else if( level == TRCLEVEL_INFO )
    clevel = 'I';
  else if( level == TRCLEVEL_WARNING )
    clevel = 'W';
  else if( level == TRCLEVEL_DEBUG )
    clevel = 'D';
  else if( level == TRCLEVEL_BYTE )
    clevel = 'B';
  else if( level == TRCLEVEL_METHOD )
    clevel = 'T';
  else if( level == TRCLEVEL_MEMORY )
    clevel = 'M';
  else if( level == TRCLEVEL_PARAM )
    clevel = 'A';
  else if( level == TRCLEVEL_PROTOCOL )
    clevel = 'P';
  else if( level == TRCLEVEL_ERROR )
    clevel = 'R';
  else if( level == TRCLEVEL_PARSE )
    clevel = 'S';
  else if( level == TRCLEVEL_WRAPPER )
    clevel = 'Z';
  else if( level == TRCLEVEL_USER1 )
    clevel = 'a';
  else if( level == TRCLEVEL_USER2 )
    clevel = 'b';
  else if( level == TRCLEVEL_MONITOR )
    clevel = 'c';
  else if( level == TRCLEVEL_XMLH )
    clevel = 'd';
  else if( level == TRCLEVEL_CALC )
    clevel = 'v';

  return clevel;
}

static Boolean __isProtocolLevel( iOTraceData t, int level ) {
  int protLevel = TRCLEVEL_EXCEPTION | TRCLEVEL_WARNING | TRCLEVEL_PROTOCOL | TRCLEVEL_ERROR;
  if( level & protLevel )
    return True;
  else
    return False;
}


static char* __stamp( char* s ) {
  char* stampStr = StrOp.createStampID(RocsTraceID);
  strcpy( s, stampStr );
  StrOp.freeID( stampStr, RocsTraceID );
  return s;
}

static Boolean __checkLevel( iOTraceData t, int level ) {
  if( (t->level & level) || __isProtocolLevel( t, level ) )
    return True;
  else
    return False;
}

static Boolean __isExceptionLevel( int level ) {
  int excLevel = TRCLEVEL_EXCEPTION;
  if( level & excLevel )
    return True;
  else
    return False;
}


/*
 ***** _Public member functions.
 */
static void _setLevel( iOTrace inst, tracelevel level ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    data->level = level;
  }
}
static tracelevel _getLevel( iOTrace inst ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    return data->level;
  }
  return 0;
}

static void _setFilename( iOTrace inst, const char* file ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    if( data->trcfile != NULL ) {
      fclose( data->trcfile );
    }

    if( data->file != NULL )
      StrOp.free( data->file );

    if( data->currentfilename != NULL )
      StrOp.free( data->currentfilename );

    data->file            = StrOp.dup( file );
    data->currentfilename = __createNumberedFileName( __nextTraceFile(data), data->file );
    data->trcfile         = fopen( data->currentfilename, "wba" );
  }
}

static const char* _getFilename( iOTrace inst ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    return data->file;
  }
  return NULL;
}

static const FILE* _getF( iOTrace inst ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    return data->trcfile;
  }
  return NULL;
}

static const char* _getCurrentFilename( iOTrace inst ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    return data->currentfilename;
  }
  return NULL;
}

static void _setFileSize( iOTrace inst, int filesize ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    data->filesize = filesize;
  }
}

static void _setNrFiles( iOTrace inst, int nrfiles ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    data->nrfiles = nrfiles;
  }
}

static void _setStdErr( iOTrace inst, Boolean toStdErr ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    data->toStdErr = toStdErr;
  }
}
static void _setEbcdicDump( iOTrace inst, Boolean ebcdicDump ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    data->ebcdicDump = ebcdicDump;
  }
}
static Boolean _isStdErr( iOTrace inst ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    return data->toStdErr;
  }
  return False;
}

static void _setDumpsize( iOTrace inst, int size ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    data->dumpsize = size;
  }
}
static int _getDumpsize( iOTrace inst ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    return data->dumpsize;
  }
  return 0;
}

static void _setAppID( iOTrace inst, const char* appID ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    data->appID = StrOp.dupID( appID, RocsTraceID );
  }
}

static void _setExceptionListener( iOTrace inst, ExceptionListener listener, Boolean timestamp, Boolean all ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    data->excListener = listener;
    data->excTimestamp = timestamp;
    data->excAll = all;
  }
}

static iOTrace _get (void) {
  return traceInst;
}
static void _set (iOTrace trace) {
  traceInst = trace;
}

static const char* _getOS(void) {
  static char* os = NULL;
  if( os == NULL ) {
    #if defined _AIX
    os = "aix";
    #elif defined __s390__
    os = "linux-s390";
    #elif defined __linux__
    os = "linux";
    #elif defined _WIN32
    os = "win32";
    #elif defined __OS2__
    os = "os/2";
    #elif defined __sun
    os = "Sun Solaris";
    #elif defined __hpux
    os = "HP_UX";
    #else
    os = "UNIX";
    #endif
  }
  return os;
}


static char* __getThreadName( void ) {
  char* nameStr;
  unsigned long ti     = ThreadOp.id();
  iOThread      thread = ThreadOp.findById( ti );
  const char*   tname  = ThreadOp.getName( thread );

  if( thread != NULL )
    nameStr = StrOp.fmtID( RocsTraceID, "%s", tname );
  else if( ti == mainThreadId )
    nameStr = StrOp.fmtID( RocsTraceID, "%s", "main" );
  else
    nameStr = StrOp.fmtID( RocsTraceID, "%08X"  , ti );

  return nameStr;
}


static void _trc( const char* objectname, tracelevel level, int line, int id, const char* fmt, ... ) {
  iOTrace l_trc = traceInst;
  iOTraceData t = NULL;
  char stmp[40];
  va_list args;

  if( l_trc == NULL )
    return;
  t = Data(l_trc);

  if( __checkLevel( t, level ) ) {
    char msg[TRACELEN] = {'\0'};
    char* tname  = __getThreadName();
    char* fmtMsg = NULL;

    va_start(args, fmt);
#if defined vsnprintf
    vsnprintf(msg, TRACELEN, fmt, args);
#else
    vsprintf(msg, fmt, args);
#endif
    va_end(args);
/*
    if( t->level & TRCLEVEL_DEBUG && objectname != NULL )
*/
    fmtMsg = StrOp.fmtID( RocsTraceID, "%s %-1.1s%04d%c %-8.8s %-8.8s %04d %s",
                          __stamp( stmp ), t->appID, id, __level( level ),
                          tname, objectname, line, msg );

    __writeFile( t, fmtMsg, __isExceptionLevel( level ) );

    if( __isExceptionLevel( level ) && t->exceptionfile ) {
      __writeExceptionFile( t, fmtMsg );
    }

    if( t->excListener != NULL ) {
      if( t->excAll || level == TRCLEVEL_EXCEPTION || level == TRCLEVEL_WARNING || level == TRCLEVEL_MONITOR || level == TRCLEVEL_CALC )
        t->excListener(level, t->excTimestamp?fmtMsg:msg);
    }
    StrOp.freeID( tname, RocsTraceID );
    StrOp.freeID( fmtMsg, RocsTraceID );


  }
}


static void _trace( const void* cargo, tracelevel level, int id, const char* fmt, ... ) {
  iOTrace l_trc = traceInst;
  iOTraceData t = NULL;
  char stmp[40];
  va_list args;
  const char* objectname = NULL;

  if( l_trc == NULL )
    return;
  t = Data(l_trc);

  /* Check cargo parameter. */
  if( cargo != NULL && cargo != traceInst ) {
    /* Asuming cargo is the object name or phase identifier. */
    objectname = cargo;
  }

  if( __checkLevel( t, level ) ) {
    char msg[TRACELEN] = {'\0'};
    char* tname  = __getThreadName();
    char* fmtMsg = NULL;

    va_start(args, fmt);
#if defined vsnprintf
    vsnprintf(msg, TRACELEN, fmt, args);
#else
    vsprintf(msg, fmt, args);
#endif
    va_end(args);
/*
    if( t->level & TRCLEVEL_DEBUG && objectname != NULL )
*/
    if( objectname != NULL )
      fmtMsg = StrOp.fmtID( RocsTraceID, "%s %-1.1s%04d%c %-8.8s %-8.8s %s",
                            __stamp( stmp ), t->appID, id, __level( level ), tname, objectname, msg );
    else
      fmtMsg = StrOp.fmtID( RocsTraceID, "%s %-1.1s%04d%c %-8.8s %s",
                            __stamp( stmp ), t->appID, id, __level( level ), tname, msg );

    __writeFile( t, fmtMsg, __isExceptionLevel( level ) );

    if( __isExceptionLevel( level ) && t->exceptionfile ) {
      __writeExceptionFile( t, fmtMsg );
    }

    if( t->excListener != NULL ) {
      if( level == TRCLEVEL_EXCEPTION || level == TRCLEVEL_WARNING )
        t->excListener(level, t->excTimestamp?fmtMsg:msg);
    }
    StrOp.freeID( tname, RocsTraceID );
    StrOp.freeID( fmtMsg, RocsTraceID );


  }
}

static void _printHeader( void ) {
  iOTrace l_trc = traceInst;
  iOTraceData t = NULL;
  char* fmtMsg = NULL;

  if( l_trc == NULL )
    return;
  t = Data(l_trc);

  __writeFile( t, "\n-------------------+------+--------+--------+----+---------- - - -", False );
  fmtMsg = StrOp.fmtID( RocsTraceID, "%s %-1.1s%-4.4s%c %-8.8s %-13.13s %s",
                        "yyyyMMDD.HHMMSS.mmm", "a", " ID ", 'l',
                        "Thread  ", "Object   Line", "Message" );
  __writeFile( t, fmtMsg, False );
  StrOp.freeID( fmtMsg, RocsTraceID );
  __writeFile( t, "-------------------+------+--------+--------+----+---------- - - -", False );
}

static void _terrno( const char* objectname, tracelevel level, int line, int id, int error, const char* fmt, ... ) {
  iOTrace l_trc = traceInst;
  iOTraceData t = NULL;
  char stmp[40];
  va_list args;

  if( l_trc == NULL )
    return;
  t = Data(l_trc);


  if( __checkLevel( t, level ) ) {
    char msg[TRACELEN] = {'\0'};
    char* tname  = __getThreadName();
    char* fmtMsg = NULL;

    va_start(args, fmt);
#if defined vsnprintf
    vsnprintf(msg, TRACELEN, fmt, args);
#else
    vsprintf(msg, fmt, args);
#endif
    va_end(args);

/*    if( t->level & TRCLEVEL_DEBUG && objectname != NULL )*/
    fmtMsg = StrOp.fmtID( RocsTraceID, "%s %-1.1s%04d%c %-8.8s %-8.8s %04d %s [%d] [%s]",
                          __stamp( stmp ), t->appID, id, __level( level ),
                          tname, objectname, line, msg, error, SystemOp.getErrStr( error ) );

    if( t->excListener != NULL ) {
      if( level == TRCLEVEL_EXCEPTION || level == TRCLEVEL_WARNING )
        t->excListener(level, t->excTimestamp?fmtMsg:msg);
    }
    __writeFile( t, fmtMsg, __isExceptionLevel( level ) );
    StrOp.freeID( tname, RocsTraceID );
    StrOp.freeID( fmtMsg, RocsTraceID );


  }
}


static void _println( const char* fmt, ... ) {
  iOTrace l_trc = traceInst;
  iOTraceData t = NULL;
  va_list args;
  const char* objectname = NULL;

  if( l_trc == NULL )
    return;
  else {
    char msg[TRACELEN] = {'\0'};
    t = Data(l_trc);

    va_start(args, fmt);
#if defined vsnprintf
    vsnprintf(msg, TRACELEN, fmt, args);
#else
    vsprintf(msg, fmt, args);
#endif
    va_end(args);

    __writeFile( t, msg, False );
    /*__writeFile( t, "\n" );*/

  }

}


static const int iCharLines = 16;

static void _dump( const void* cargo, tracelevel level, const char* buf, int size ) {
  TraceOp.dmp( cargo, level, 0, buf, size );
}

static void _dmp( const void* cargo, tracelevel level, int code, const char* buf, int size ) {
  iOTrace l_trc = traceInst;
  iOTraceData t = NULL;
  int i     = 0;
  int n     = 0;
  int lines = 0;
  char stmp[40];
  char* msg    = NULL;
  char* tname  = NULL;
  const char* objectname = NULL;

  if( l_trc == NULL )
    return;

  t = Data(l_trc);

  /* Check cargo parameter. */
  if( cargo != NULL && cargo != traceInst ) {
    /* Asuming cargo is the object name. */
    objectname = cargo;
  }

  if( !__checkLevel( t, level ) )
    return;

  if( size > t->dumpsize )
    size = t->dumpsize;

  lines = size / iCharLines + ((size % iCharLines) ? 1:0);

  tname = __getThreadName();

  if( t->ebcdicDump ) {
    msg = StrOp.fmtID( RocsTraceID, "%s %s%04d%c %-8.8s %-8.8s *trace dump( 0x%08X: length=%d )\n    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|EBCDIC..........|\n    --------------------------------------------------------- |----------------|----------------|",
                    __stamp(stmp), t->appID, code, __level( level ), tname, objectname, buf, size );
  }
  else {
    msg = StrOp.fmtID( RocsTraceID, "%s %s%04d%c %-8.8s %-8.8s *trace dump( 0x%08X: length=%d )\n    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |ASCII...........|\n    --------------------------------------------------------- |----------------|",
                    __stamp(stmp), t->appID, code, __level( level ), tname, objectname, buf, size );
  }

  __writeFile( t, msg, __isExceptionLevel( level ) );
  StrOp.freeID( tname, RocsTraceID );
  StrOp.freeID( msg, RocsTraceID );

  for( i = 0; i < lines; i++ ) {
    char l[ 40] = { '\0' };
    char s[132] = { '\0' };
    char a[132] = { '\0' };
    char e[132] = { '\0' };
    for( n = 0; n < iCharLines; n++ ) {
      if( n+i*iCharLines < size ) {
        unsigned char val = buf[n+i*iCharLines];
        sprintf( l, "%02X", val );
        strcat( s, l );
        strcat( s, " " );
        e[n] = (val == 0 ? 0x4B:val);
        if( val >= ' ' && val <= '~' )
          a[n] = val;
        else
          a[n] = '.';
      }
      else {
        strcat( s, "   " );
        a[n] = ' ';
      }
    }
    a[n] = '\0';
    e[n] = '\0';
    if( t->ebcdicDump ) {
      EbcdicOp.Ebcdic2ExtAscii( t->ebcdic, e, StrOp.len( a ) );
      msg = StrOp.fmtID( RocsTraceID, "    %08X: %47.47s |%16.16s|%16.16s|", i*16, s, a, e );
    }
    else {
      msg = StrOp.fmtID( RocsTraceID, "    %08X: %47.47s |%16.16s|", i*16, s, a );
    }
    __writeFile( t, msg, __isExceptionLevel( level ) );
    StrOp.freeID( msg, RocsTraceID );
  }
}

static void _setMainThreadId( unsigned long id ) {
  mainThreadId = id;
}

static void _setInvoke( iOTrace inst, const char* invoke, Boolean async ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    data->invoke = StrOp.dup( invoke );
    data->invokeasync = async;
  }
}

static void _setExceptionFile( iOTrace inst, Boolean exceptionfile ) {
  iOTrace l_trc = inst != NULL ? inst:traceInst;
  if( l_trc != NULL ) {
    iOTraceData data = Data(l_trc);
    data->exceptionfile = exceptionfile;
  }
}

/* Caution! singelton. */
static iOTrace _inst ( tracelevel level, const char* file, Boolean toStdErr) {
  if( traceInst == NULL ) {
    iOTrace     trace = allocIDMem( sizeof( struct OTrace ), RocsTraceID );
    iOTraceData data  = allocIDMem( sizeof( struct OTraceData ), RocsTraceID );

    MemOp.basecpy( trace, &TraceOp, 0, sizeof( struct OTrace ), data );

    data->mux      = MutexOp.inst( NULL, True );
    data->ebcdic   = EbcdicOp.inst( CODEPAGE_1252, NULL );
    data->level    = level;
    data->toStdErr = toStdErr;
    data->dumpsize = TRC_DUMPSIZE;
    data->appID    = StrOp.dupID( "..", RocsTraceID );
    data->filesize = TRC_FILESIZE;
    data->nrfiles  = TRC_NRFILES;

    traceInst = trace;

    instCnt++;

    if( file != NULL )
      _setFilename( trace, file );

    mainThreadId = ThreadOp.id();
    return trace;
  }
  else
    return traceInst;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/trace.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
