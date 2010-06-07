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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

#ifdef _WIN32
#include <winsock.h>
#define ETIMEDOUT WSAETIMEDOUT
#elif defined __OS2__
#include <nerrno.h>
#endif

#ifdef __OPENSSL__
#include <openssl/ssl.h>
#include <openssl/err.h>
#else
#define SSL_CTX void
#define SSL void
#endif

#include "rocs/impl/socket_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"

static int instCnt = 0;


/*
 ***** __Private functions.
 */
/* OS dependent: windows(wsocket.c) (os/2)osocket.c (unix)usocket.o */
Boolean rocs_socket_init( iOSocketData o );
Boolean rocs_socket_resolveHost( iOSocketData o );
Boolean rocs_socket_bind( iOSocketData o );
Boolean rocs_socket_close( iOSocketData o );
Boolean rocs_socket_setBlocking( iOSocket inst, Boolean blocking );
Boolean rocs_socket_connect( iOSocket inst );
Boolean rocs_socket_create( iOSocketData o );
Boolean rocs_socket_listen( iOSocketData o );
Boolean rocs_socket_write( iOSocket inst, const char* buf, int size );
Boolean rocs_socket_read( iOSocket inst, char* buf, int size );
int rocs_socket_recvfrom( iOSocket inst, char* buf, int size );
Boolean rocs_socket_sendto( iOSocket inst, char* buf, int size );
int rocs_socket_accept( iOSocket inst );
Boolean rocs_socket_setSndTimeout( iOSocket inst, int timeout );
Boolean rocs_socket_setRcvTimeout( iOSocket inst, int timeout );
const char* rocs_socket_getPeername(iOSocket inst);
Boolean rocs_socket_setKeepalive(iOSocket,Boolean);
Boolean rocs_socket_peek( iOSocket inst, char* buf, int size );
char* rocs_socket_mac( const char* device );
Boolean rocs_socket_LoadCerts( iOSocket inst, const char *cFile, const char *kFile );
const char* rocs_socket_gethostaddr( void );

#ifdef __OPENSSL__
Boolean rocs_socket_CreateCTX( iOSocket inst );
Boolean rocs_socket_LoadCerts( iOSocket inst, const char *cFile, const char *kFile );
#endif

/*
 ***** OBase functions.
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
  iOSocketData data = Data(inst);
  return data->host;
}
Boolean __closeSocket( iOSocketData o );
static void __del(void* inst) {
  iOSocketData data = Data(inst);

  if( data->sh > 0 )
    rocs_socket_close( data );

  if( data->hostaddr != NULL )
    freeIDMem( data->hostaddr, RocsSocketID );

  StrOp.freeID( data->host, RocsSocketID );
  freeIDMem( data, RocsSocketID );
  freeIDMem( inst, RocsSocketID );
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




/*
 ***** OSocket public functions.
 */
static iOSocket _acceptSocket( iOSocket inst ) {
  iOSocketData o = Data(inst);
  int sh = 0;

  sh = rocs_socket_accept( inst );

  if( sh >= 0 ) {
    iOSocket     socket = allocIDMem( sizeof( struct OSocket ), RocsSocketID );
    iOSocketData data   = allocIDMem( sizeof( struct OSocketData ), RocsSocketID );

    MemOp.basecpy( socket, &SocketOp, 0, sizeof( struct OSocket ), data );

    data->port = o->port;
    data->sh   = sh;
    instCnt++;

#ifdef __OPENSSL__
    if( o->ssl ) {
      data->ssl = True;
      data->ssl_sh = SSL_new( o->ssl_ctx );
      SSL_set_fd( data->ssl_sh, sh );
      if( SSL_accept( data->ssl_sh ) == -1 ) {
        ERR_print_errors_fp( (FILE*)TraceOp.getF(NULL) );
      }
    }
#endif

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "client accepted." );
    return socket;
  }
  else {
    return NULL;
  }
}

static Boolean __setBlocking( iOSocket inst, Boolean blocking ) {
  iOSocketData data = Data(inst);
  if( rocs_socket_setBlocking( inst, blocking ) ) {
    data->blocking = blocking;
    return True;
  }
  return False;
}

static Boolean _writecSocket( iOSocket inst, char c ) {
  return rocs_socket_write( inst, &c, 1 );
}


static char _readcSocket( iOSocket inst ) {
  char buf[1] = { '\0' };
  rocs_socket_read( inst, buf, 1 );
  return buf[0];
}

static void _resetSocket( iOSocket inst ) {
  iOSocketData data = Data(inst);
  data->binded = False;
  data->listening = False;
}


static Boolean _isConnected( iOSocket inst ) {
  iOSocketData data = Data(inst);
  return data->connected;
}

static Boolean _fmt( iOSocket inst, const char* fmt, ... ) {
  va_list args;
  char s[4096] = {'\0'};

  va_start(args, fmt);
  vsprintf(s, fmt, args);
  va_end(args);

  return rocs_socket_write( inst, s, strlen(s) );
}

static char* _readln( iOSocket inst, char* buf ) {
  iOSocketData data = Data(inst);
  Boolean ok = False;
  char inc[2] = { '\0', '\0' };
  buf[0] = '\0';
  do {
    ok = rocs_socket_read( inst, inc, 1 );
    inc[1] = '\0';
    if( ok )
      strcat( buf, inc );
    else {
      return NULL;
    }
  } while( inc[0] != '\0' && inc[0] != '\n' && !data->broken );
  return (data->broken?NULL:buf);
}

static char* _readStr( iOSocket inst, char* buf ) {
  iOSocketData data = Data(inst);
  Boolean ok = False;
  char inc[2] = { '\0', '\0' };
  buf[0] = '\0';
  do {
    ok = rocs_socket_read( inst, inc, 1 );
    inc[1] = '\0';
    if( ok )
      strcat( buf, inc );
    else {
      return NULL;
    }
  } while( inc[0] != '\0' && !data->broken  );
  return (data->broken?NULL:buf);
}

static void _disConnect( iOSocket inst ) {
  iOSocketData data = Data(inst);
  rocs_socket_close( data );
}

static int _getRc( iOSocket inst ) {
  iOSocketData data = Data(inst);
  return data->rc;
}

static Boolean _isTimedOut( iOSocket inst ) {
  iOSocketData data = Data(inst);
  return data->rc == ETIMEDOUT ? True:False;
}

static long _getReceived( iOSocket inst ) {
  iOSocketData data = Data(inst);
  return data->readed;
}

static long _getPeeked( iOSocket inst ) {
  iOSocketData data = Data(inst);
  return data->peeked;
}

static Boolean _isBroken( iOSocket inst ) {
  iOSocketData data = Data(inst);
  return data->broken;
}

static FILE * _getStream( iOSocket inst ) {
  iOSocketData data = Data(inst);
  FILE* f = fdopen( data->sh, "w+" );
  if( f == NULL )
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, errno, "Error getting stream for socket." );
  return f;
}

static long _getSended( iOSocket inst ) {
  iOSocketData data = Data(inst);
  return data->written;
}

static Boolean _bind( iOSocket inst ) {
  iOSocketData data = Data(inst);
  return rocs_socket_bind( data );
}



static char __hostname[256];
static const char* _gethostname( void ) {
#if defined __linux__ || defined __hpux || defined _AIX
  gethostname( __hostname, sizeof( __hostname ) );
  return __hostname;
#else
  return "localhost";
#endif
}

static const char* _gethostaddr( void ) {
  return rocs_socket_gethostaddr();
}

static char* _getMAC( const char* device ) {
  return rocs_socket_mac( device );
}

static iOSocket _inst( const char* host, int port, Boolean ssl, Boolean udp, Boolean multicast ) {
  iOSocket     socket = allocIDMem( sizeof( struct OSocket ), RocsSocketID );
  iOSocketData data   = allocIDMem( sizeof( struct OSocketData ), RocsSocketID );

  MemOp.basecpy( socket, &SocketOp, 0, sizeof( struct OSocket ), data );

  data->host = StrOp.dupID( host, RocsSocketID );
  data->port = port;

  data->ssl  = ssl;
  data->udp  = udp;
  data->multicast = multicast;

  if( rocs_socket_init(data) ) {
    rocs_socket_create(data);
  }

  instCnt++;

  return socket;
}

static iOSocket _instSSLserver( int port, const char* certFile, const char* keyFile ) {
  iOSocket s = _inst( "localhost", port, True, False, False );
  #ifdef __OPENSSL__
  rocs_socket_CreateCTX( s );
  rocs_socket_LoadCerts( s, certFile, keyFile );
  #endif

  return s;
}


static Boolean _isOpenSSL(void) {
  #ifdef __OPENSSL__
    return True;
  #endif
  return False;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/socket.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
