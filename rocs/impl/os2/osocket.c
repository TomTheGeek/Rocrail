/** ------------------------------------------------------------
  * $Source: /home/cvs/xspooler/rocs/impl/os2/osocket.c,v $
  * $Author: rob $
  * $Date: 2005-04-25 09:23:50 $
  * $Revision: 1.36 $
  * $Name:  $
  * ------------------------------------------------------------ */
#if defined __OS2__

#include <stdlib.h>
#include <string.h>

#ifdef __BORLANDC__
  #ifndef _System
    #define _System __syscall
  #endif
  #ifndef __STDC__
    #define __STDC__ 1
  #endif
#endif

#include <errno.h>
#include <nerrno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#ifdef __OPENSSL__
  #include <openssl/crypto.h>
  #include <openssl/x509.h>
  #include <openssl/pem.h>
  #include <openssl/ssl.h>
  #include <openssl/err.h>
#else
    #define SSL_CTX void
    #define SSL void
#endif

#include "rocs/impl/socket_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/thread.h"
#include "rocs/public/mem.h"

static int instCnt = 0;

#ifndef __ROCS_SOCKET__
        #pragma message("*** OS/2 OSocket is disabled. (define __ROCS_SOCKET__ in rocs.h) ***")
#endif

char* rocs_socket_mac( const char* device ) {
#ifdef __ROCS_SOCKET__
  return StrOp.dup( "0000000000000000" );
#else
  return StrOp.dup( "0000000000000000" );
#endif
}

/* initializes the windows socket implementation */
Boolean rocs_socket_init( iOSocketData o ) {
#ifdef __ROCS_SOCKET__
  if( o->hostaddr == NULL ) {
    o->hostaddr = allocIDMem( sizeof( struct in_addr ), RocsSocketID );
  }
  sock_init();
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_resolveHost( iOSocketData o ) {
#ifdef __ROCS_SOCKET__
  struct hostent* host = NULL;
  struct in_addr* addr = o->hostaddr;

  addr->s_addr = inet_addr( o->host );

  if( addr->s_addr == (unsigned long)-1 ) {
    host = gethostbyname( o->host );
    if( host == NULL ) {
      o->rc = sock_errno();
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "%s (rc=%d)", "Error looking up host.", sock_errno() );
      return False;
    }
    memcpy (o->hostaddr, host->h_addr_list[0], host->h_length);
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "HostAddr: %d", addr->s_addr );
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_create( const iOSocketData o ) {
#ifdef __ROCS_SOCKET__
  o->sh = socket( AF_INET, SOCK_STREAM, 0 );
  if( o->sh < 0 ) {
    o->rc = sock_errno();
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "socket() failed [%d]", sock_errno() );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "socket created." );
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_close( iOSocketData o ) {
#ifdef __ROCS_SOCKET__
  int rc = 0;
  if( o->sh == 0 )
    return True;
  rc = soclose( o->sh );
  o->connected = False;
  if (rc != 0)   /* free all allocated resources */
  {
    o->rc = sock_errno();
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "closesocket() failed [%d]", sock_errno() );
    return False;
  }
  o->sh = 0;
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "socket closed." );
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_setBlocking( iOSocket inst, Boolean blocking ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  int args = blocking?0:1;
  int rc   = 0;
  if( o->sh == 0 )
    return False;
  /* set/clear non-blocking i/o */
  rc = ioctl( o->sh, FIONBIO, &args, sizeof( args ) );
  if( rc == -1 ) {
    o->rc = sock_errno();
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ioctl() failed [%d]", sock_errno() );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "socket ioctl setted. OK" );
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_setSndTimeout( iOSocket inst, int timeout ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  int rc   = 0;
  struct timeval tv;
  int size = sizeof tv;
  tv.tv_sec  = timeout;
  tv.tv_usec = 0;
  rc = setsockopt( o->sh, SOL_SOCKET, SO_SNDTIMEO, (void*)&tv, size );

  if( rc != 0 ) {
    o->rc = sock_errno();
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_socket_setSndTimeout() failed [%d]", sock_errno() );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_socket_setSndTimeout(%d) OK.", timeout );
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_setRcvTimeout( iOSocket inst, int timeout ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  int rc   = 0;
  struct timeval tv;
  int size = sizeof tv;
  tv.tv_sec  = timeout;
  tv.tv_usec = 0;
  rc = setsockopt( o->sh, SOL_SOCKET, SO_RCVTIMEO, (void*)&tv, size );

  if( rc != 0 ) {
    o->rc = sock_errno();
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_socket_setRcvTimeout() failed [%d]", sock_errno() );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_socket_setRcvTimeout(%d) OK.", timeout );
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_connect( iOSocket inst ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  struct sockaddr_in srvaddr;
  struct in_addr* addr = o->hostaddr;
  int rc = 0;

  if( o->sh == 0 )
    rocs_socket_create( o );

  if( o->sh == 0 )
    return False;

  if( !rocs_socket_resolveHost( o ) )
    return False;

  memset( &srvaddr,0, sizeof( struct sockaddr_in ) );
  srvaddr.sin_family = AF_INET;
  srvaddr.sin_port   = htons( (u_short)o->port );
  srvaddr.sin_addr   = *addr;

  rc = connect( o->sh, (struct sockaddr *)&srvaddr, sizeof( struct sockaddr_in ) );

  if( rc == -1 ) {
    o->rc = sock_errno();
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "connect(%s:%d) failed [%d]",
                 o->host, o->port ,sock_errno() );
    o->connected = False;
    return False;
  }
  o->connected = True;
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "socket connected." );
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_bind( iOSocketData o ) {
#ifdef __ROCS_SOCKET__
  struct sockaddr_in srvaddr;
  struct in_addr* addr = o->hostaddr;
  int rc = 0;

  if( o->sh == 0 )
    return False;

  if( o->binded )
    return True;

  memset( &srvaddr,0, sizeof( struct sockaddr_in ) );
  srvaddr.sin_family = AF_INET;
  srvaddr.sin_port   = htons( (u_short)o->port );
  srvaddr.sin_addr   = *addr;

  rc = bind( o->sh, (struct sockaddr *)&srvaddr, sizeof( struct sockaddr_in ) );

  if( rc == -1 ) {
    o->rc = sock_errno();
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "bind() failed [%d]", sock_errno() );
    o->binded = False;
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "socket binded." );
  o->binded = True;
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_listen( iOSocketData o ) {
#ifdef __ROCS_SOCKET__
  int rc = 0;

  if( o->sh == 0 )
    return False;

  if( o->listening )
    return True;

  rc = listen( o->sh, SOMAXCONN );

  if( rc != 0 ) {
    o->rc = sock_errno();
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "listen() failed [%d]", sock_errno() );
    o->listening = False;
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "socket listening." );
  o->listening = True;
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_write( iOSocket inst, char* buf, int size ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  int written  = 0;
  int twritten = 0;
  int flags    = 0;

  if( o->sh == 0 )
    return False;

  o->written = 0;

  while( twritten < size ) {

    written = send( o->sh, buf + twritten, size - twritten, flags );
    if( written <= 0 ) {
      if( sock_errno() == EPIPE ) {
        rocs_socket_close( o );
      }
      o->rc = sock_errno();
      rocs_socket_close(o);
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "send() = %d failed [%d]", written, sock_errno() );
      if( o->rc == EPIPE )
        o->broken = True;
      return False;
    }
    twritten += written;
  }
  o->written = twritten;
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%d bytes written to socket.", twritten );
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_readpeek( iOSocket inst, char* buf, int size, Boolean peek ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  int readed   = 0;
  int treaded  = 0;
  int flags    = peek ? (MSG_PEEK|MSG_DONTWAIT):0;

  if( o->sh == 0 )
    return False;

  o->readed = 0;

  while( treaded < size ) {

    readed = recv( o->sh, buf + treaded, size - treaded, flags );

    /* Has otherside closed the connection? */
    if( readed == 0 ) {
      o->rc = errno;
      o->broken = True;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Other side has closed connection." );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "errno=%d, readed=%d", errno, readed );
      return False;
    }

    if( peek ) {
      o->peeked = readed;
      if( readed == -1 && o->rc != 0 && o->rc != EAGAIN && o->rc != EINTR )
        o->rc = sock_errno();
        o->broken = True;
      return (readed >= size) ? True:False;
    }


    if( readed < 0 )
    {
      o->rc = sock_errno();
      /* For none blocking...
      if( !o->blocking && o->rc == EWOULDBLOCK ) {
        ThreadOp.sleep(10);
        continue;
      }
      */
      if( o->rc == EPIPE || o->rc == ESHUTDOWN || o->rc == EPIPE || o->rc == ENOTSOCK || o->rc == ETIMEDOUT || o->rc == EWOULDBLOCK )
        rocs_socket_close(o);

      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "recv() = %d failed [%d]", readed, sock_errno() );
      return False;
    }
    treaded += readed;
  }
  o->readed = treaded;
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%d bytes readed from socket.", treaded );
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_read( iOSocket inst, char* buf, int size ) {
  return rocs_socket_readpeek( inst, buf, size, False );
}
Boolean rocs_socket_peek( iOSocket inst, char* buf, int size ) {
  return rocs_socket_readpeek( inst, buf, size, True );
}



int rocs_socket_accept( iOSocket inst ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  struct sockaddr_in clntaddr;
  int len = sizeof( struct sockaddr_in );
  int sh = 0;

  rocs_socket_bind( o );
  rocs_socket_listen( o );

  if( !o->binded || !o->listening ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "server socket not ready." );
    return -1;
  }

  sh = accept( o->sh, (struct sockaddr *)&clntaddr, &len );

  if( sh < 0 ) {
    o->rc = sock_errno();
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "accept() failed [%d]", sock_errno() );
  }

  return sh;
#else
  return 0;
#endif
}

char* rocs_socket_getPeername(iOSocket inst) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  struct sockaddr_in sin;
  struct hostent* hp;
  char* lp;
  int len = sizeof( sin );
  int rc = getpeername( o->sh, (struct sockaddr *)&sin, & len );
  if( rc < 0 ) {
    o->rc = sock_errno();
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "getsockpeer() failed [%d]", sock_errno() );
    return "";
  }
  if( hp = gethostbyaddr( (char *)&sin.sin_addr.s_addr, sizeof(sin.sin_addr.s_addr), AF_INET) )
    lp = hp->h_name;
  else
    lp = inet_ntoa( sin.sin_addr );
  TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "getsockpeer() :%s", lp );
  return lp;
#else
  return NULL;
#endif
}

Boolean rocs_socket_setKeepalive( iOSocket inst, Boolean alive ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  int rc   = 0;
  int size = sizeof( alive );
  rc = setsockopt( o->sh, SOL_SOCKET, SO_KEEPALIVE, (void*)&alive, size );

  if( rc != 0 ) {
    o->rc = sock_errno();
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "rocs_socket_setKeepalive() failed [%d]", sock_errno() );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_socket_setKeepalive(%s) OK.", alive ? "True":"False" );
  return True;
#else
  return False;
#endif
}

/* Load the certification files, ie the public and private keys. */
Boolean rocs_socket_LoadCerts( iOSocket inst, const char *cFile, const char *kFile ) {
  return False;
}

#endif
