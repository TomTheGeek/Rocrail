/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

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


#include "rocdigs/impl/hue_impl.h"

#include "rocs/public/mem.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/HUE.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Output.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOHUEData data = Data(inst);
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

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OHUE ----- */
/*
https://www.meethue.com/api/nupnp

GET /api/nupnp HTTP/1.1
Host: www.meethue.com
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:33.0) Gecko/20100101 Firefox/33.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,* / *;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
Cache-Control: max-age=0

HTTP/1.1 200 OK
access-control-allow-credentials: true
access-control-allow-headers: Origin, X-Requested-With, Content-Type, Accept
access-control-allow-methods: GET, POST, PUT, DELETE
access-control-allow-origin: *
Cache-Control: no-cache
Content-Encoding: gzip
Content-Length: 22
Content-Type: application/json
Date: Tue, 02 Dec 2014 11:47:39 GMT
Expires: Thu, 01 Jan 1970 00:00:00 GMT
Server: Google Frontend
Set-Cookie: PLAY_FLASH=;Path=/;Expires=Thu, 01 Jan 1970 00:00:00 GMT
Set-Cookie: PLAY_ERRORS=;Path=/;Expires=Thu, 01 Jan 1970 00:00:00 GMT
Set-Cookie: PLAY_SESSION=;Path=/;Expires=Thu, 01 Jan 1970 00:00:00 GMT
Vary: Accept-Encoding
X-Firefox-Spdy: 3.1
----------------------------------------------------------
<html><head><link rel="alternate stylesheet" type="text/css" href="resource://gre-resources/plaintext.css" title="Wrap Long Lines"></head><body><pre>[]</pre></body></html>
 */


static char* __httpRequest( iOHUE inst, const char* request ) {
  iOHUEData data = Data(inst);
  char* reply = NULL;
  Boolean OK = True;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "trying to connected to %s:80", wHUE.getbridge(data->hueini) );
  iOSocket sh = SocketOp.inst( wHUE.getbridge(data->hueini), 80, False, False, False );
  if( SocketOp.connect( sh ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Connected to %s", wHUE.getbridge(data->hueini) );

    char* httpReq = StrOp.fmt("%s HTTP/1.1\nHost: %s\n\n", request, wHUE.getbridge(data->hueini) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "length=%d\n%s", StrOp.len(httpReq), httpReq );
    SocketOp.write( sh, httpReq, StrOp.len(httpReq) );
    StrOp.free(httpReq);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Read response..." );
    char str[1024] = {'\0'};
    SocketOp.setRcvTimeout( sh, 1000 );
    /* Read first HTTP header line: */
    OK = False;

    if( SocketOp.readln( sh, str ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, str );
      if( StrOp.find( str, "200 OK" ) ) {
        OK = True;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "OK" );
      }

      /* Reading rest of HTTP header: */
      int contlen = 0;
      while( SocketOp.readln( sh, str ) && !SocketOp.isBroken( sh ) ) {
        if( str[0] == '\r' || str[0] == '\n' ) {
          break;
        }
        if( StrOp.find( str, "Content-Length:" ) ) {
          char* p = StrOp.find( str, ": " ) + 2;
          contlen = atoi( p );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "contlen = %d", contlen );
        }

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, str );
      };

      if( OK && contlen > 0 ) {
        char* reply = (char*)allocMem(contlen+1);
        SocketOp.read( sh, reply, contlen );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reply = %.200s", reply );
      }
    }

    SocketOp.disConnect(sh);
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not connected to %s", wHUE.getbridge(data->hueini) );
  }
  SocketOp.base.del(sh);

  return reply;

}



/*
 * Content-Type: application/json
 */
static iONode __hueGET(iOHUE inst, const char* json) {
  iOHUEData data = Data(inst);
  iONode rsp = NULL;
  __httpRequest(inst, "GET /");
  return rsp;
}


static iONode __huePUT(iOHUE inst, const char* json) {
  iOHUEData data = Data(inst);
  iONode rsp = NULL;
  return rsp;
}


static iONode __translate( iOHUE inst, iONode node ) {
  iOHUEData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cmd=%s", NodeOp.getName( node ) );

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "system STOP" );
    }
    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "system GO" );
    }
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int addr = wOutput.getaddr( node );
    int gate = wOutput.getgate( node );
    Boolean active = StrOp.equals( wOutput.getcmd( node ), wOutput.on );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output addr=%d", addr );
  }

  return NULL;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOHUEData data = Data(inst);

  if( cmd != NULL ) {
    __translate( (iOHUE)inst, cmd );

    /* Cleanup */
    NodeOp.base.del(cmd);
  }

  return NULL;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOHUEData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _setRawListener( obj inst ,obj listenerObj ,const digint_rawlistener listenerRawFun ) {
  return 0;
}


/** external shortcut event */
static void _shortcut( obj inst ) {
  return;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  return 0;
}


/**  */
static Boolean _supportPT( obj inst ) {
  return 0;
}


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 1;
static int _version( obj inst ) {
  return vmajor*10000 + vminor*100 + patch;
}


/**  */
static struct OHUE* _inst( const iONode ini ,const iOTrace trc ) {
  iOHUE __HUE = allocMem( sizeof( struct OHUE ) );
  iOHUEData data = allocMem( sizeof( struct OHUEData ) );
  MemOp.basecpy( __HUE, &HUEOp, 0, sizeof( struct OHUE ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini     = ini;
  data->hueini  = wDigInt.gethue(ini);
  data->iid     = StrOp.dup( wDigInt.getiid( ini ) );

  if( data->hueini == NULL ) {
    data->hueini = NodeOp.inst(wHUE.name(), ini, ELEMENT_NODE);
    NodeOp.addChild(ini, data->hueini);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Philips HUE %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  iid   : [%s]", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  bridge: [%s]", wHUE.getbridge(data->hueini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  user  : [%s]", wHUE.getuser(data->hueini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  __hueGET(__HUE, NULL);

  instCnt++;
  return __HUE;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/hue.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
