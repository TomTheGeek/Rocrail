/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "rocrail/public/hclient.h"
#include "rocrail/public/pclient.h"
#include "rocrail/impl/http_impl.h"
#include "rocrail/wrapper/public/HttpService.h"
#include "rocrail/wrapper/public/WebClient.h"

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/map.h"
#include "rocs/public/strtok.h"


static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOHttpData data = Data(inst);
    /* Cleanup data->xxx members...*/

    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  return;
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  return;
}

static char* __toString( void* inst ) {
  return NULL;
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  return NULL;
}

/** ----- OHttp ----- */


/**----------------------------------------------------------------------
 * PMonOp __portserver()
 * ----------------------------------------------------------------------
 * @param  inst Thread instance.
 */
static void __portserver( void* threadinst ) {
  iOThread     th = (iOThread)threadinst;
  iOHttp     http = (iOHttp)ThreadOp.getParm(th);
  iOHttpData data = Data( http );

  char* desc = StrOp.fmt( "HttpService on port %d", data->port  );
  ThreadOp.setDescription( th, desc );
  StrOp.free( desc );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "HttpService started on %d.", data->port );

  do {
    iOHClient client = (iOHClient)MapOp.first( data->clientMap );
    /* Iterate the list of connected clients. */
    while( client != NULL ) {

      /* Let the client do the work... */
      Boolean remove = HClientOp.work( client );

      if( remove ) {
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "Removing HClient [%s].", HClientOp.getId( client ) );
        MapOp.remove( data->clientMap, HClientOp.getId( client ) );

        /* Cleanup. */
        HClientOp.base.del( client );

        break; /* We better do not call MapOp.next() after a remove. */
      }

      ThreadOp.sleep( 5 );
      client = (iOHClient)MapOp.next( data->clientMap );
    };

    ThreadOp.sleep( 5 );
  } while( !ThreadOp.isQuit( th ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "HttpService ended on %d.", data->port );
  ThreadOp.base.del( th );
  data->portserver = NULL;
}


/**----------------------------------------------------------------------
 * PMonOp __portmanager()
 * ----------------------------------------------------------------------
 * @param  inst Thread instance.
 */
static void __portmanager( void* threadinst ) {
  iOThread           th = (iOThread)threadinst;
  iOHttp           http = (iOHttp)ThreadOp.getParm(th);
  iOHttpData       data = Data( http );

  char* desc = StrOp.fmt( "HTTPManager on port %d", data->port  );
  ThreadOp.setDescription( th, desc );
  StrOp.free( desc );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "HTTPManager started on %d.", data->port );
  do {
    /* Wait for a connection. */
    iOSocket clientSocket = SocketOp.accept( data->srvrsocket );

    if( clientSocket ) {
      iOHClient client = HClientOp.inst( clientSocket, wHttpService.getpath( data->ini ), wHttpService.getrefresh( data->ini )  );

      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "HTTPManager accept for %s:%d. (id=%s)",
                     SocketOp.getPeername( clientSocket ), data->port, HClientOp.getId( client ) );

      MapOp.put( data->clientMap, HClientOp.getId( client ), (obj)client );

    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Accept broken." );
      if( data->portserver )
        ThreadOp.requestQuit( data->portserver );
      break;
    }

    ThreadOp.sleep( 10 );
  } while( !ThreadOp.isQuit( th ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Manager ended for %d.", data->port );
  ThreadOp.base.del( th );
  data->portmanager = NULL;
}


/**----------------------------------------------------------------------
 * PMonOp __portserver()
 * ----------------------------------------------------------------------
 * @param  inst Thread instance.
 */
static void __pportserver( void* threadinst ) {
  iOThread     th = (iOThread)threadinst;
  iOHttp     http = (iOHttp)ThreadOp.getParm(th);
  iOHttpData data = Data( http );

  char* desc = StrOp.fmt( "WebClient Service on port %d", data->pport  );
  ThreadOp.setDescription( th, desc );
  StrOp.free( desc );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "WebClient Service started on %d.", data->pport );

  do {
    iOPClient client = NULL;
    if( MutexOp.wait( data->pclientmux ) ) {
      client = (iOPClient)MapOp.first( data->pclientMap );
      MutexOp.post( data->pclientmux );
    }
    /* Iterate the list of connected clients. */
    while( client != NULL ) {

      /* Let the client do the work... */
      Boolean remove = PClientOp.work( client );

      if( remove ) {
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "Removing WebClient [%s].", PClientOp.getId( client ) );
        if( MutexOp.wait( data->pclientmux ) ) {
          MapOp.remove( data->pclientMap, PClientOp.getId( client ) );
          MutexOp.post( data->pclientmux );
        }

        /* Cleanup. */
        PClientOp.base.del( client );

        break; /* We better do not call MapOp.next() after a remove. */
      }

      ThreadOp.sleep( 5 );
      if( MutexOp.wait( data->pclientmux ) ) {
        client = (iOPClient)MapOp.next( data->pclientMap );
        MutexOp.post( data->pclientmux );
      }
    };

    ThreadOp.sleep( 5 );
  } while( !ThreadOp.isQuit( th ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "WebClient Service ended on %d.", data->port );
  ThreadOp.base.del( th );
  data->pportserver = NULL;
}


/**----------------------------------------------------------------------
 * PMonOp __portmanager()
 * ----------------------------------------------------------------------
 * @param  inst Thread instance.
 */
static void __pportmanager( void* threadinst ) {
  iOThread           th = (iOThread)threadinst;
  iOHttp           http = (iOHttp)ThreadOp.getParm(th);
  iOHttpData       data = Data( http );

  char* desc = StrOp.fmt( "WebClient Manager on port %d", data->port  );
  ThreadOp.setDescription( th, desc );
  StrOp.free( desc );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "WebClient Manager started on %d.", data->pport );
  do {
    /* Wait for a connection. */
    iOSocket clientSocket = SocketOp.accept( data->psrvrsocket );

    if( clientSocket ) {
      iOPClient client = PClientOp.inst( clientSocket, data->webclient  );

      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "WebClient Manager accept for %s:%d. (id=%s)",
                     SocketOp.getPeername( clientSocket ), data->pport, PClientOp.getId( client ) );

      if( MutexOp.wait( data->pclientmux ) ) {
       MapOp.put( data->pclientMap, PClientOp.getId( client ), (obj)client );
       MutexOp.post( data->pclientmux );
      }

    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Accept broken." );
      if( data->pportserver )
        ThreadOp.requestQuit( data->pportserver );
      break;
    }

    ThreadOp.sleep( 10 );
  } while( !ThreadOp.isQuit( th ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "WebClient Manager ended for %d.", data->pport );
  ThreadOp.base.del( th );
  data->pportmanager = NULL;
}


/** Object creator. */
static struct OHttp* _inst( iONode ini ) {
  if( ini != NULL ) {
    iOHttp __Http = allocMem( sizeof( struct OHttp ) );
    iOHttpData data = allocMem( sizeof( struct OHttpData ) );
    MemOp.basecpy( __Http, &HttpOp, 0, sizeof( struct OHttp ), data );

    data->ini = ini;
    data->port = wHttpService.getport( ini );
    data->webclient = wHttpService.getwebclient( ini );

  
    /* Initialize data->xxx members... */
    if( data->port > 0 ) {
      char*  htmName = StrOp.fmt( "htm%08X", __Http );
      char*  htsName = StrOp.fmt( "hts%08X", __Http );

      data->clientMap = MapOp.inst();
  
      data->srvrsocket  = SocketOp.inst( "localhost", data->port, False, False, False );
      data->portmanager = ThreadOp.inst( htmName, __portmanager, __Http );
      data->portserver  = ThreadOp.inst( htsName, __portserver , __Http );
  
      ThreadOp.start( data->portmanager );
      ThreadOp.start( data->portserver );
  
      /* Cleanup */
      StrOp.free( htmName );
      StrOp.free( htsName );
    }
  
    if( data->webclient && wWebClient.getport( data->webclient ) > 0 ) {
      char*  phtmName = StrOp.fmt( "phtm%08X", __Http );
      char*  phtsName = StrOp.fmt( "phts%08X", __Http );
      
      data->pclientMap = MapOp.inst();
      data->pclientmux = MutexOp.inst( NULL, True );
  
      data->pport = wWebClient.getport( data->webclient );
      data->psrvrsocket = SocketOp.inst( "localhost", data->pport, False, False, False );
      data->pportmanager = ThreadOp.inst( phtmName, __pportmanager, __Http );
      data->pportserver  = ThreadOp.inst( phtsName, __pportserver , __Http );
  
      ThreadOp.start( data->pportmanager );
      ThreadOp.start( data->pportserver );
  
      /* Cleanup */
      StrOp.free( phtmName );
      StrOp.free( phtsName );
    }
  
    instCnt++;
    return __Http;
  }
  return NULL;
}




/** Shutdown Http. */
static void _shutdown( struct OHttp* inst ) {
  iOHttpData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down HTTP..." );
  
  if( data->portmanager != NULL )
    ThreadOp.requestQuit( data->portmanager );
  if( data->portserver != NULL )
    ThreadOp.requestQuit( data->portserver );
  if( data->srvrsocket != NULL )
    SocketOp.disConnect( data->srvrsocket );
  
  if( data->webclient != NULL ) {
    if( data->pportmanager != NULL )
      ThreadOp.requestQuit( data->pportmanager );
    if( data->pportserver != NULL )
      ThreadOp.requestQuit( data->pportserver );
    if( data->psrvrsocket != NULL )
      SocketOp.disConnect( data->psrvrsocket );
  }
  return;
}


static iOMap _createPostDataMap( const char* postdata ) {
  iOMap map = MapOp.inst();
  iOStrTok list = StrTokOp.inst( postdata, '&' );
  while( StrTokOp.hasMoreTokens( list ) ) {
    iOStrTok pair = StrTokOp.inst( StrTokOp.nextToken( list ), '=' );
    if( StrTokOp.countTokens( pair ) == 2 ) {
      const char* attrname  = StrTokOp.nextToken( pair );
      char* attrvalue = StrOp.dup( StrTokOp.nextToken( pair ) );
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "pair: %s=%s\n", attrname, attrvalue );
      MapOp.put( map, attrname, (obj)attrvalue );
    }
    StrTokOp.base.del( pair );
  };
  StrTokOp.base.del( list );
  return map;
}

static void _deletePostDataMap( iOMap map ) {
  char* attrvalue = (char*)MapOp.first( map );
  do {
    StrOp.free( attrvalue );
    attrvalue = (char*)MapOp.next( map );
  } while( attrvalue != NULL );
  MapOp.base.del( map );
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/http.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
