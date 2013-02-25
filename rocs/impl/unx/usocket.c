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
#if defined __linux__ || defined _AIX || defined __unix__ || defined __APPLE__

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
#include "rocs/public/system.h"
#include "rocs/public/str.h"

#include <stdlib.h>
#include <stdio.h>

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/utsname.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>

/*#define __USE_MISC*/ /* Why must this be manualy set? (Linux) */
#include <net/if.h>
/*#include <net/if_arp.h>*/

#ifdef _AIX
#include <sys/ndd_var.h>
#include <sys/kinfo.h>
#endif


#ifndef __ROCS_SOCKET__
        #pragma message("*** Unix OSocket is disabled. (define __ROCS_SOCKET__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */

/* OS dependent */
/* initializes the windows socket implementation */
Boolean rocs_socket_init( iOSocketData o ) {
#ifdef __ROCS_SOCKET__
  if( o->hostaddr == NULL ) {
    o->hostaddr = allocIDMem( sizeof( struct in_addr ), RocsSocketID );
  }
  return True;
#else
  return False;
#endif
}

/* OS dependent */
static Boolean __resolveHost( iOSocketData o, const char* hostname ) {
#ifdef __ROCS_SOCKET__
  struct hostent* host = NULL;
  struct in_addr* addr = o->hostaddr;

  if( o->hostaddr == NULL ) {
    rocs_socket_init( o );
    addr = o->hostaddr;
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__resolveHost: inet_addr(%s)", o->host );
  addr->s_addr = inet_addr( hostname );

  if( addr->s_addr == INADDR_NONE ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__resolveHost: gethostbyname()" );
    host = gethostbyname( hostname );
    if( host == NULL ) {
      o->rc = errno;
      TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 8005, o->rc, "gethostbyname(%s) failed [%d]", o->host );
      return False;
    }
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__resolveHost: memcpy()" );
    memcpy (o->hostaddr, host->h_addr, host->h_length);
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "HostAddr: %ld", addr->s_addr );
  return True;
#else
  return False;
#endif
}

Boolean rocs_socket_resolveHost( iOSocketData o ) {
#ifdef __ROCS_SOCKET__
  return __resolveHost(o, o->host);
#else
  return False;
#endif
}

/* OS dependent */
char* rocs_socket_mac( const char* device ) {
#if defined __ROCS_SOCKET__ && defined __linux__
  int fd;
  struct ifreq  ifrq;
  int rc = 0;
  char* mac = NULL;

  #if defined __s390__
    const char *ifname = device==NULL?"ctc0":device;
  #else
    const char *ifname = device==NULL?"eth0":device;
  #endif

  #if defined __APPLE__

  #else

  fd = socket( AF_INET, SOCK_DGRAM, 0 );
  strncpy (ifrq.ifr_name, ifname, IFNAMSIZ);
  rc = ioctl (fd, SIOCGIFHWADDR, &ifrq);
  close( fd );

#endif
  /*IFHWADDRLEN*/
  if( rc == 0 ) {
    mac = StrOp.fmt( "%02X%02X%02X%02X%02X%02X%02X%02X",
                        (unsigned char)ifrq.ifr_addr.sa_data[0],
                        (unsigned char)ifrq.ifr_addr.sa_data[1],
                        (unsigned char)ifrq.ifr_addr.sa_data[2],
                        (unsigned char)ifrq.ifr_addr.sa_data[3],
                        (unsigned char)ifrq.ifr_addr.sa_data[4],
                        (unsigned char)ifrq.ifr_addr.sa_data[5],
                        (unsigned char)ifrq.ifr_addr.sa_data[6],
                        (unsigned char)ifrq.ifr_addr.sa_data[7]
                    );
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                 "rocs_socket_mac( %s ): [%s]", ifname, mac );
  }
  else {
    mac = StrOp.dup( "0000000000000000" );
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                 "rocs_socket_mac( %s ): rc = %d, errno = %d", ifname, rc, errno );
  }

  return mac;
#elif defined _AIX

  int size;
  struct kinfo_ndd *nddp;
  char* mac = NULL;

  size = getkerninfo(KINFO_NDD, 0, 0, 0);
  if (size <= 0) {
    return NULL;
  }
  nddp = (struct kinfo_ndd *)allocMem(size);

  if (!nddp) {
    return NULL;
  }
  if (getkerninfo(KINFO_NDD, nddp, &size, 0) < 0) {
    freeMem(nddp);
    return NULL;
  }
  mac = StrOp.fmt( "%02X%02X%02X%02X%02X%02X%02X%02X",
                      (unsigned char)nddp->ndd_addr[0],
                      (unsigned char)nddp->ndd_addr[1],
                      (unsigned char)nddp->ndd_addr[2],
                      (unsigned char)nddp->ndd_addr[3],
                      (unsigned char)nddp->ndd_addr[4],
                      (unsigned char)nddp->ndd_addr[5],
                      0,
                      0
                  );
  freeMem(nddp);
  return mac;
#else

  return StrOp.dup( "0000000000000000" );
#endif
}

/* OS dependent */
Boolean rocs_socket_create( iOSocketData o ) {
#ifdef __ROCS_SOCKET__

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "creating socket udp=%d", o->udp );

  o->sh = socket( AF_INET, o->udp ? SOCK_DGRAM:SOCK_STREAM, o->udp ? IPPROTO_UDP:IPPROTO_TCP );
  if( o->sh < 0 ) {
    o->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 8015, o->rc, "socket() failed" );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "socket created." );
  return True;
#else
  return False;
#endif
}

void rocs_socket_localip( iOSocketData o, const char* ip ) {
#ifdef __ROCS_SOCKET__
  struct in_addr localInterface;
  /* IP_MULTICAST_IF:  Sets the interface over which outgoing multicast datagrams are sent. */
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Set the interface over which outgoing multicast datagrams are sent..." );
  localInterface.s_addr = inet_addr(ip);
  if(setsockopt(o->sh, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
  {
    o->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 8015, o->rc, "setsockopt() failed" );
  }
#endif
}


/* OS dependent */
Boolean rocs_socket_close( iOSocketData o ) {
#ifdef __ROCS_SOCKET__
  int rc = 0;

  if( o->ssl ) {
    #ifdef __OPENSSL__
    if( o->ssl_sh )
      /* send SSL/TLS close_notify */
      SSL_shutdown( o->ssl_sh );
    #endif
  }

  /* remove Multicast Socket from group */
  if (o->udp && o->multicast ) {
    struct ip_mreq command;
    command.imr_multiaddr.s_addr = inet_addr (o->host);
    command.imr_interface.s_addr = htonl (INADDR_ANY);
    setsockopt( o->sh, IPPROTO_IP, IP_DROP_MEMBERSHIP,  &command, sizeof (command));
  }


  rc = close( o->sh );
  if (rc != 0)   /* free all allocated resources */
  {
    o->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 8036, o->rc, "close() failed" );
    return False;
  }
  o->connected = False;
  o->sh = 0;

  if( o->ssl ) {
    #ifdef __OPENSSL__
    /* Clean up. */
    if( o->ssl_sh ) {
      SSL_free( o->ssl_sh );
      o->ssl_sh = NULL;
    }
    if( o->ssl_ctx ) {
      SSL_CTX_free( o->ssl_ctx );
      o->ssl_ctx = NULL;
    }
    #endif
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "socket closed." );
  return True;
#else
  return False;
#endif
}


/*
 ***** _Public functions.
 */
/* OS dependent */
Boolean rocs_socket_setBlocking( iOSocket inst, Boolean blocking ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  int rc    = 0;
  int flags = 0;

  /* Get the flags. */
  flags = fcntl(o->sh, F_GETFL, 0);

  /* Set/clear non-blocking in flags. */
  if( !blocking )
    flags |= O_NONBLOCK;
  else
    flags &= ~O_NONBLOCK;

  /* Set the modified flags. */
  rc = fcntl(o->sh, F_SETFL, flags);

  return rc < 0 ? False:True;
#else
  return False;
#endif
}

/* OS dependent */
Boolean rocs_socket_setSndTimeout( iOSocket inst, int timeout ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  int rc   = 0;
  struct timeval tv;
  int size = sizeof tv;
  tv.tv_sec  = timeout;
  tv.tv_usec = 0;
  rc = setsockopt( o->sh, SOL_SOCKET, SO_SNDTIMEO, (void*)&tv, size );
  o->rc = rc;
  if( rc != 0 ) {
    o->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, __LINE__, o->rc, "setsockopt() failed" );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_socket_setSndTimeout() OK." );
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
  o->rc = rc;

  if( rc != 0 ) {
    o->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, o->rc, "setsockopt() failed" );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_socket_setRcvTimeout() OK." );
  return True;
#else
  return False;
#endif
}


/* OS dependent */
Boolean rocs_socket_connect( iOSocket inst ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  struct sockaddr_in srvaddr;
  struct in_addr* addr = o->hostaddr;
  int rc = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_socket_connect: BEGIN" );

  if( o->sh == 0 )
    rocs_socket_create( o );

  if( o->sh == 0 )
    return False;

  if( !rocs_socket_resolveHost( o ) )
    return False;

  addr = o->hostaddr;

  memset( &srvaddr,0, sizeof( struct sockaddr_in ) );
  srvaddr.sin_family = AF_INET;
  srvaddr.sin_port   = htons( (unsigned short)o->port );
  srvaddr.sin_addr   = *addr;

  rc = connect( o->sh, (struct sockaddr *)&srvaddr, sizeof( struct sockaddr_in ) );

  if( rc == -1 ) {
    o->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 8020, o->rc,
                    "connect(%s:%d) failed", o->host, o->port );
    o->connected = False;
    return False;
  }
  o->connected = True;
  o->broken = False;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "socket connected." );
  if( o->ssl ) {
    #ifdef __OPENSSL__
    SSL_METHOD* meth;
    X509*       server_cert;

    o->openssl_support = True;

    SSLeay_add_ssl_algorithms();
    meth = SSLv3_client_method();
    SSL_load_error_strings();
    o->ssl_ctx = SSL_CTX_new (meth);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "OpenSSL support.(v%d)", meth->version );

    o->ssl_sh = SSL_new( o->ssl_ctx );
    if( o->ssl_sh != NULL ) {
      SSL_set_fd( o->ssl_sh, o->sh );
      rc = SSL_connect( o->ssl_sh );
      if( rc <= 0 ) {
        char err[256] = {'\0'};
        rc = ERR_peek_error();
        ERR_error_string_n( (unsigned long) rc, err, sizeof(err) );

        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SSL_connect() failed [%d] %s", rc, err );
        o->connected = False;
        o->openssl_support = False;
        return False;
      }

      /* Trace some info. */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SSL connection using %s", SSL_get_cipher( o->ssl_sh ) );

      server_cert = SSL_get_peer_certificate( o->ssl_sh );
      if( server_cert != NULL ) {
        char* sub = NULL;
        char* iss = NULL;

        sub = X509_NAME_oneline( X509_get_subject_name( server_cert ), 0, 0 );
        iss = X509_NAME_oneline( X509_get_issuer_name ( server_cert ), 0, 0 );

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "SSL Server certificate subject = %s, issuer = %s", sub, iss );
        free( sub );
        free( iss );
      }

    }
    #else
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SSL requested but not supported! Compile with __OPENSSL__ defined." );
    return False;
    #endif
  }

  return True;
#else
  return False;
#endif
}

/* OS dependent */
Boolean rocs_socket_bind( iOSocketData o ) {
#ifdef __ROCS_SOCKET__
  struct sockaddr_in srvaddr;
  struct in_addr* addr = o->hostaddr;
  int rc = 0;

  if( o->binded ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "already binded" );
    return True;
  }

  memset( &srvaddr,0, sizeof( struct sockaddr_in ) );

  srvaddr.sin_family = AF_INET;
  srvaddr.sin_port   = htons( (unsigned short)o->port );
  if( o->udp )
    srvaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  else
    srvaddr.sin_addr   = *addr;

  if( o->udp && o->multicast ) {
    int loop = 1;
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "allow all processes to use this port..." );
    setsockopt ( o->sh, SOL_SOCKET, SO_REUSEADDR, &loop, sizeof (loop));
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "bind..." );
  rc = bind( o->sh, (struct sockaddr *)&srvaddr, sizeof( struct sockaddr_in ) );

  if( rc != -1 && o->udp && o->multicast ) {
    struct ip_mreq command;
    int loop = 1;
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Allow broadcasting..." );
    rc = setsockopt ( o->sh, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof (loop));
    if( rc == -1 ) {
      o->rc = errno;
      TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, o->rc, "setsockopt() failed" );
      o->binded = False;
      return False;
    }

    /* Join the broadcast group: */
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Join the broadcast group..." );
    command.imr_multiaddr.s_addr = inet_addr (o->host);
    command.imr_interface.s_addr = htonl (INADDR_ANY);

    if (command.imr_multiaddr.s_addr == -1) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "%s id no multicast address!", o->host );
      o->binded = False;
      return False;
    }

    rc = setsockopt ( o->sh, IPPROTO_IP, IP_ADD_MEMBERSHIP, &command, sizeof (command));
    if( rc == -1 ) {
      o->rc = errno;
      TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, o->rc, "setsockopt() failed" );
      o->binded = False;
      return False;
    }
  }

  if( rc == -1 ) {
    o->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, o->rc, "bind() failed" );
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

/* OS dependent */
Boolean rocs_socket_listen( iOSocketData o ) {
#ifdef __ROCS_SOCKET__
  int rc = 0;

  if( o->listening )
    return True;

  rc = listen( o->sh, SOMAXCONN );

  if( rc != 0 ) {
    o->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, o->rc, "listen() failed" );
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

/* OS dependent */
Boolean rocs_socket_write( iOSocket inst, char* buf, int size ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  int written  = 0;
  int twritten = 0;
  int flags    = 0;
  int blockretry = 0;

  o->written = 0;

  while( size > 0 && twritten < size && !o->broken && blockretry < 100 ) {

    /*if( o->ssl && o->openssl_support ) {*/
    if( o->ssl ) {
      #ifdef __OPENSSL__
      if( o->ssl_sh ) {
        written = SSL_write( o->ssl_sh, buf + twritten, size - twritten );
      }
      #endif
    }
    else{
      if (o->sh){
        errno = 0;
        written = send( o->sh, buf + twritten, size - twritten, flags );
      }
    }

    if( written == 0 ) {
      /**/
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "cannot write to socket sh=%d errno=%d...", o->sh, errno );
      rocs_socket_close(o);
      o->broken = True;
      return False;
    }
    else if( written < 0 ) {
      if( errno == EWOULDBLOCK ) {
        blockretry++;
        ThreadOp.sleep(10);
        if( blockretry >= 100 )
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "socket blocks sh=%d errno=%d...", o->sh, errno );
        continue;
      }
      o->rc = errno;

      rocs_socket_close(o);

      if( o->ssl ) {
      #ifdef __OPENSSL__
        ERR_print_errors_fp( (FILE*)TraceOp.getF(NULL) );
        fflush( (FILE*)TraceOp.getF(NULL) );
      #endif
      }
      else
        TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 8030, o->rc, "send() failed" );

      if( o->rc == EPIPE || o->rc == ECONNRESET ) {
        o->broken = True;
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Connection broken!" );
      }
      return False;
    }
    twritten += written;
  }
  o->written = twritten;
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%d bytes written to socket.", twritten );
  return twritten == size ? True:False;
#else
  return False;
#endif
}

/* OS dependent */
Boolean rocs_socket_readpeek( iOSocket inst, char* buf, int size, Boolean peek ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  int readed   = 0;
  int treaded  = 0;
  #ifdef MSG_DONTWAIT
  int flags    = peek ? (MSG_PEEK|MSG_DONTWAIT):0;
  #else
  int flags    = peek ? (MSG_PEEK):0;
  #endif

  o->readed = 0;

  while( treaded < size ) {

    /*if( o->ssl && o->openssl_support ) {*/
    if( o->ssl && !peek ) {
      #ifdef __OPENSSL__
      if( o->ssl_sh ) {
        readed = SSL_read( o->ssl_sh, buf + treaded, size - treaded );
      }
      #endif
    }
    else
      readed = recv( o->sh, buf + treaded, size - treaded, flags );

    /* Has otherside closed the connection? */
    if( readed == 0 ) {
      o->rc = errno;
      o->broken = True;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Other side has closed connection." );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "errno=%d, read=%d", errno, readed );
      return False;
    }

    if( peek ) {
      o->peeked = readed;
      if( readed == -1 && errno != 0 && errno != EAGAIN && errno != EINTR ) {
        o->rc = errno;
        o->broken = True;
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Socket 0x%08X error %d", o->sh, o->rc );
      }
      return (readed >= size) ? True:False;
    }

    if( readed < 0 )
    {
      o->rc = errno;
      /* For none blocking...
      if( !o->blocking && o->rc == EWOULDBLOCK ) {
        ThreadOp.sleep(10);
        continue;
      }
      */
      if( o->rc != EAGAIN )
        if( o->rc == EWOULDBLOCK || o->rc == ESHUTDOWN || o->rc == EPIPE || o->rc == ENOTSOCK || o->rc == ETIMEDOUT || o->rc == ECONNRESET ) {
          TraceOp.terrno( name, TRCLEVEL_WARNING, __LINE__, 8035, o->rc, "closing socket..." );
          if( o->rc == ECONNRESET )
            o->broken = True;
          rocs_socket_close(o);
        }

      if( o->ssl ) {
      #ifdef __OPENSSL__
        ERR_print_errors_fp( (FILE*)TraceOp.getF(NULL) );
        fflush( (FILE*)TraceOp.getF(NULL) );
      #endif
      }
      else
        TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 8035, o->rc, "recv() failed" );
      return False;
    }
    treaded += readed;
  }
  o->readed = treaded;
  if( treaded > 1 )
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%d bytes read from socket.", treaded );
  return True;
#else
  return False;
#endif
}


int rocs_socket_recvfrom( iOSocket inst, char* buf, int size, char* client, int* port ) {
  iOSocketData o = Data(inst);
  struct sockaddr_in sin;
  int sin_len = sizeof(sin);
  int rc = 0;
  if( buf == NULL ) {
    char l_buf[256];
    size = 256;
    rc = recvfrom ( o->sh, l_buf, size, MSG_PEEK, (struct sockaddr *) &sin, &sin_len);
  }
  else
    rc = recvfrom ( o->sh, buf, size, 0, (struct sockaddr *) &sin, &sin_len);
  o->rc = errno;
  if( rc < 0 ) {
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, o->rc, "recvfrom() failed" );
    return 0;
  }
  if( client != NULL && port != NULL ) {
		StrOp.copy( client, inet_ntoa(sin.sin_addr));
		*port = ntohs(sin.sin_port);
		TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%d bytes readed from %s:%d", rc, client, *port );
  }
  return rc;
}



Boolean rocs_socket_sendto( iOSocket inst, char* buf, int size, char* client, int port ) {
  iOSocketData o = Data(inst);
  struct sockaddr_in address;
  int rc = 0;
  struct in_addr* addr = o->hostaddr;

  if( !__resolveHost(o, client == NULL ? o->host:client) ) {
    return False;
  }
  addr = o->hostaddr;

  memset (&address, 0, sizeof (address));
  address.sin_family = AF_INET;
  /*address.sin_addr.s_addr = inet_addr (client == NULL ? o->host:client);*/
  address.sin_addr = *addr;
  address.sin_port = htons (port > 0 ? port:o->port);

  rc = sendto ( o->sh, buf, size, 0, (struct sockaddr *) &address, sizeof(address));
  o->rc = errno;
  if( rc < 0 ) {
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, o->rc, "sendto() failed" );
    return False;
  }
  return True;
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

  sh = accept( o->sh, (struct sockaddr *)&clntaddr, (socklen_t*)&len );

  if( sh < 0 ) {
    o->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, o->rc, "accept() failed" );
  }
  else
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "accept serversh=%d sh=%d errno=%d", o->sh, sh, errno );

  return sh;
#else
  return 0;
#endif
}

char* rocs_socket_getPeername(iOSocket inst) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  struct sockaddr_in sin;
  /*struct hostent* hp;*/
  char* lp;
  int len = sizeof( sin );
  int rc = getpeername( o->sh, (struct sockaddr *)&sin, (socklen_t*)&len );
  if( rc < 0 ) {
    o->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, o->rc, "getsockpeer() failed" );
    return "";
  }
/*  if( hp = gethostbyaddr( (char *)&sin.sin_addr.s_addr, sizeof(sin.sin_addr.s_addr), AF_INET) )
    lp = hp->h_name;
  else*/
    lp = inet_ntoa( sin.sin_addr );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "getsockpeer() :%s", lp );
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
    o->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, o->rc, "setsockopt() failed" );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_socket_setKeepalive() OK." );
  return True;
#else
  return False;
#endif
}


Boolean rocs_socket_setNodelay( iOSocket inst, Boolean flag ) {
#ifdef __ROCS_SOCKET__
  iOSocketData o = Data(inst);
  int rc   = 0;
  int size = sizeof( flag );
  rc = setsockopt(o->sh,            /* socket affected */
                   IPPROTO_TCP,     /* set option at TCP level */
                   TCP_NODELAY,     /* name of option */
                   (void*) &flag,   /* the cast is historical cruft */
                   size);           /* length of option value */
  if( rc != 0 ) {
    o->rc = errno;
    TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, o->rc, "setsockopt() failed" );
    return False;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rocs_socket_setNodelay() OK." );
  return True;
#else
 return False;
#endif
}

Boolean rocs_socket_CreateCTX( iOSocket inst ) {
#ifdef __OPENSSL__
  iOSocketData o = Data(inst);
  /* The method describes which SSL protocol we will be using. */
  SSL_METHOD *method;

  /* Load algorithms and error strings. */
  OpenSSL_add_all_algorithms();
  SSL_load_error_strings();

  /* Compatible with SSLv2, SSLv3 and TLSv1 */
  method = SSLv23_server_method();

  /* Create new context from method. */
  o->ssl_ctx = SSL_CTX_new( method );
  if( o->ssl_ctx == NULL ) {
    ERR_print_errors_fp( (FILE*)TraceOp.getF(NULL) );
    fflush( (FILE*)TraceOp.getF(NULL) );
    return False;
  }
  return True;
#else
  return False;
#endif
}

/* Load the certification files, ie the public and private keys. */
Boolean rocs_socket_LoadCerts( iOSocket inst, const char *cFile, const char *kFile ) {
#ifdef __OPENSSL__
  iOSocketData o = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SSL_CTX_use_certificate_chain_file( %s )...", cFile==NULL?"NULL":cFile );
  if ( cFile != NULL ) {
    if ( SSL_CTX_use_certificate_chain_file( o->ssl_ctx, cFile ) <= 0) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SSL_CTX_use_certificate_chain_file(): Examine trace!" );
      ERR_print_errors_fp( (FILE*)TraceOp.getF(NULL) );
      fflush( (FILE*)TraceOp.getF(NULL) );
      return False;
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SSL_CTX_use_PrivateKey_file( %s )...", kFile==NULL?"NULL":kFile );
  if ( kFile != NULL ) {
    if ( SSL_CTX_use_PrivateKey_file( o->ssl_ctx, kFile, SSL_FILETYPE_PEM ) <= 0) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SSL_CTX_use_PrivateKey_file(): Examine trace!" );
      ERR_print_errors_fp( (FILE*)TraceOp.getF(NULL) );
      fflush( (FILE*)TraceOp.getF(NULL) );
      return False;
    }
  }

  /* Verify that the two keys goto together. */
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SSL_CTX_check_private_key()..." );
  if ( !SSL_CTX_check_private_key( o->ssl_ctx ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SSL_CTX_check_private_key(): Private key is invalid!" );
    ERR_print_errors_fp( (FILE*)TraceOp.getF(NULL) );
    fflush( (FILE*)TraceOp.getF(NULL) );
    return False;
  }

  return True;
#else
  return False;
#endif
}


static char __hostname[256];
const char* rocs_socket_gethostaddr( void ) {
  struct hostent *he;
  struct in_addr a;
  int i = 0;

  gethostname( __hostname, sizeof( __hostname ) );
  he = gethostbyname (__hostname);
  while(he->h_addr_list[i] != NULL ) {
    const char* s = inet_ntoa (*(struct in_addr *)he->h_addr_list[i]);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "address %d = %s", i, s );
    i++;
    if( !StrOp.equals( "127.0.1.1", s ) )
      return s;
  }
  return __hostname;
}



#endif
