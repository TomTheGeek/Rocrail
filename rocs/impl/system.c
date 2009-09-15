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
#include "rocs/impl/system_impl.h"
#include "rocs/impl/errstr.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/thread.h"
#include "rocs/public/str.h"
#include "rocs/public/socket.h"
#include "rocs/public/mutex.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#if defined _WIN32 || defined __OS2__
#include <process.h>
#elif defined __unix__
#include <unistd.h>
#endif


static int instCnt = 0;
static iOSystem __system = NULL;

/* OS dependent: windows(wsocket.c) (os/2)osocket.c (unix)usocket.o */
const char* rocs_system_getWSName( iOSystemData o );
const char* rocs_system_getUserName( iOSystemData o );
int rocs_system_getMillis( void );
Boolean rocs_system_uBusyWait( int us );
char* rocs_system_getURL( const char* filepath );
Boolean rocs_system_setadmin( void );
int rocs_system_getTime( int* hours, int* minutes, int* seconds );
Boolean rocs_system_accessPort( int from, int num );
Boolean rocs_system_releasePort( int from, int num );
void rocs_system_writePort( int port, byte val );
byte rocs_system_readPort( int port );
Boolean rocs_system_accessDev( const char* device, Boolean readonly );

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
  iOSystemData data = Data(inst);
  return "";
}
static void __del(void* inst) {
  iOSystemData data = Data(inst);
  freeIDMem( data, RocsSystemID );
  freeIDMem( inst, RocsSystemID );
  instCnt--;
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


static void __ticker( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOSystem inst = (iOSystem)ThreadOp.getParm( th );
  iOSystemData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Ticker thread has started." );
  do {
    ThreadOp.sleep( 10 );
    data->tick++;
  } while( True );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Ticker thread has stopped." );
}

static unsigned long _getTick( void ) {
  if( __system == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "System not instantiated! Tick not available!" );
    return 0;
  }
  else {
    iOSystemData data = Data(__system);
    return data->tick;
  }
}

/*
 ***** _Public functions.
 */
static iOSystem _inst(void) {
  if( __system == NULL ) {
    iOSystem     system = allocIDMem( sizeof( struct OSystem ), RocsSystemID );
    iOSystemData data   = allocIDMem( sizeof( struct OSystemData ), RocsSystemID );
    char* tickername = NULL;

    MemOp.basecpy( system, &SystemOp, 0, sizeof( struct OSystem ), data );

    tickername = StrOp.fmt( "tick%08X", system );
    data->ticker = ThreadOp.inst( tickername, &__ticker, system );
    ThreadOp.start( data->ticker );
    StrOp.free( tickername );

    __system = system;

    instCnt++;
  }

  return __system;
}


// Translates CP1252 into ISO-8859-15 Euro.
// Not all chars in the range of 0x80 and 0x9F are mapable into Latin 15!
static char* _cp1252toISO885915( const char* s ) {
  int len = StrOp.len( s );
  char* iso = (char*)allocMem( len +1 );
  int i = 0;
  for( i = 0; i < len; i++ ) {
    unsigned char* c = (unsigned char*)&s[i];
    switch( *c ) {
      case 0x80: iso[i] = 0xA4; break; // Euro
      case 0x8A: iso[i] = 0xA6; break;
      case 0x9A: iso[i] = 0xA8; break;
      case 0x8E: iso[i] = 0xB4; break;
      case 0x9E: iso[i] = 0xB8; break;
      case 0x8C: iso[i] = 0xBC; break;
      case 0x9C: iso[i] = 0xBD; break;
      case 0x9F: iso[i] = 0xBE; break;
      default:   iso[i] = s[i]; break;
    }
  }
  return iso;
}

/**
 * Creates a 32 char GUID: Globally Unique ID.
 * 12 chars of hwaddress, 17 chars timestamp, 3 chars counter.
 */
static char* _getGUID( const char* macdev ) {
  static unsigned long loopCnt = 0;
  static char* mac = NULL;
  static iOMutex mux = NULL;

  /* Onetime initializing. */
  if( mux == NULL ) {
    mux = MutexOp.inst( NULL, True );
  }

  if( mac == NULL ) {
    mac = SocketOp.getMAC( macdev );
    if( mac == NULL )
      mac = StrOp.fmt( "%012u", SystemOp.getpid() );
  }

  /* Get the semaphore. */
  if( MutexOp.wait( mux ) ) {
    char* stamp = StrOp.createStampNoDots();
    char* guid = StrOp.fmt( "%-12.12s%-17.17s%03ld", mac, stamp, loopCnt++ );
    StrOp.free( stamp );

    /* Sleep 0.01 second to get unique timestamps. */
    ThreadOp.sleep( 10 );
    /* Free the semaphore. */
    MutexOp.post( mux );

    return guid;
  }
  else {
    return NULL;
  }

}



static const char* _getWSName(void) {
  iOSystemData data = Data(_inst());
  return strlen( data->WSName ) > 0 ? data->WSName:rocs_system_getWSName( data );
}

static const char* _getUserName(void) {
  iOSystemData data = Data(_inst());
  return strlen( data->UserName ) > 0 ? data->UserName:rocs_system_getUserName( data );
}

static const char* _getProperty(const char* name) {
  return getenv( name );
}

static const char* _getPrgExt(void) {
#ifdef _WIN32
  return ".exe";
#elif defined __OS2__
  return ".exe";
#elif defined __unix__ || defined __linux__ || defined _AIX
  return "";
#endif
}

static char _getFileSeparator(void) {
#ifdef _WIN32
  return '\\';
#elif defined __OS2__
  return '\\';
#elif defined __unix__ || defined __linux__ || defined _AIX
  return '/';
#endif
}

static char _getPathSeparator(void) {
#ifdef _WIN32
  return ';';
#elif defined __OS2__
  return ';';
#elif defined __unix__ || defined __linux__ || defined _AIX
  return ':';
#endif
}

static ostype _getOSType(void) {
#ifdef _WIN32
  return OSTYPE_DOS;
#elif defined __OS2__
  return OSTYPE_DOS;
#elif defined __unix__ || defined __linux__ || defined _AIX
  return OSTYPE_UNIX;
#endif
}

static Boolean _isWindows(void) {
#ifdef _WIN32
  return True;
#else
  return False;
#endif
}

static Boolean _isUnix(void) {
#if defined __unix__ || defined __linux__ || defined _AIX
  return True;
#else
  return False;
#endif
}

static char _getPathSeparator4OS( ostype type ) {
  switch( type ) {
    case OSTYPE_DOS:
      return '\\';
    case OSTYPE_UNIX:
      return '/';
    default:
      return '/';
  }
}


static const char* _getErrStr( int error ) {
  if( error == -1 )
    return "Error";
  if( error < -1 || error > ERRSTR_LAST )
    return "Unknown errno";
  else
    return errStr[error];
}

static const char* _getSigStr( int sig ) {
  if( sig < 0 || sig > SIGSTR_LAST )
    return "Unknown signal";
  else
    return sigStr[sig];
}

static void _sysbeep( void ) {
}

static void __execRunner( void* inst ) {
  iOThread th = (iOThread)inst;
  const char* cmdStr = (const char*)ThreadOp.getParm(th);
  system( cmdStr );
  th->base.del( th );
}

static int _systemExec( const char* cmdStr, Boolean async ) {
  if( async ) {
    iOThread th = ThreadOp.inst( name, __execRunner, (obj)cmdStr );
    ThreadOp.start( th );
    return 0;
  }
  else {
    return system( cmdStr );
  }
}

static int _systemgetpid(void) {
  int pid = 0;
#if defined _WIN32
  pid = _getpid();
#elif defined __OS2__
  pid = getpid();
#elif defined __unix__
  pid = getpid();
#endif
  return pid;
}

static char* _getUNC( const char* filepath ) {
  return rocs_system_getURL( filepath );
}

static const char* _getBuild(void) {
  static char* stamp = NULL;
  if( stamp == NULL ) {
    stamp = StrOp.fmtID( RocsSystemID, "librocs %d.%d-%d build %s %s",
                         SystemOp.vmajor, SystemOp.vminor, SystemOp.patch,
                         SystemOp.builddate, SystemOp.buildtime );
  }
  return stamp;
}


static char* _latin2utf( const char* latinstr ) {
  int len = StrOp.len( latinstr );
  char* utfstr = allocMem( len * 3 + 1 ); /* One Euro sign needs 3 chars in UTF-8 */

  int i = 0;
  int idx = 0;
  for( i = 0; i < len; i++ ) {
    unsigned short val = latinstr[i];
    val &= 0xFF;
    if( val < 0x80 ) {
      utfstr[idx] = (char)(val & 0x7F);
      idx++;
      utfstr[idx] = '\0';
    }
    else if( val == 0xA4 ) {
      /* Euro */
      unsigned short unicode = 0x20AC;
      utfstr[idx] = 0xE0 | ((unicode & 0xF000) >> 12);
      idx++;
      utfstr[idx] = 0x80 | ((unicode & 0x0FC0) >> 6);
      idx++;
      utfstr[idx] = 0x80 | (unicode & 0x003F);
      idx++;
      utfstr[idx] = '\0';
    }
    else {
      unsigned short unicode = 0x0000 | val;
      utfstr[idx] = 0xC0 | ((unicode & 0x00C0) >> 6);
      idx++;
      utfstr[idx] = 0x80 | (unicode & 0x003F);
      idx++;
      utfstr[idx] = '\0';
    }

  }

  { /* Free up the un unsed bytes: */
    char* tmp = utfstr;
    utfstr = StrOp.dup( utfstr );
    freeMem( tmp );
    return utfstr;
  }
}


static char* _utf2latin( const char* utfstr ) {
  /* */
  int len = StrOp.len( utfstr );
  char* latinstr = allocIDMem( len + 1, RocsStrID );
  int i = 0;
  int idx = 0;
  for( i = 0; i < len; i++ ) {
    unsigned short val = utfstr[i];
    val &= 0xFF;
    if( val > 0x007F ) {
      if( (val & 0x00E0) == 0x00C0 ) {
        if( i + 1 < len ) {
          unsigned short unicode = utfstr[i+1] & 0x3F;
          i += 1;
          unicode |= (val & 0x1F) << 6;
          if( unicode < 0x0100 ) {
            latinstr[idx] = (char)unicode & 0xFF;
            idx++;
          }
        }
        else {
          /* UTF-8 error! */
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "UTF-8 error: %d + 1 => %d", i, len );
        }
      }
      else if( (val & 0x00F0) == 0x00E0 ) {
        if( i + 2 < len ) {
          unsigned short unicode = utfstr[i+1] & 0x3F;
          unicode = unicode << 6;
          unicode |= utfstr[i+2] & 0x3F;
          i += 2;
          unicode |= (val & 0x0F) << 12;
          if( unicode == 0x20AC ) { /* Euro */
            latinstr[idx] = 0xA4;
            idx++;
          }
          else if( unicode == 0x20A4 ) { /* Pound */
            latinstr[idx] = 0xA3;
            idx++;
          }
          else {
            /* Unsupported unicode! */
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Unsupported unicode=0x%04X!", unicode );
          }
        }
        else {
          /* UTF-8 error! */
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "UTF-8 error: %d + 2 >= %d", i, len );
        }
      }
      else {
        /* Unsupported UTF-8 sequence. */
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Unsupported UTF-8 sequence." );
      }

    }
    else {
      latinstr[idx] = utfstr[i];
      idx++;
    }
  }
  return latinstr;
}





/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/system.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
