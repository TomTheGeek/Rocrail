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
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Output.h"

#include <math.h>

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
 * Example
 * method : PUT /api/<user>/lights/1/state
 * request: {"bri":42}
 */
static char* __httpRequest( iOHUE inst, const char* method, const char* request ) {
  iOHUEData data = Data(inst);
  char* reply = NULL;
  Boolean OK = True;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "trying to connected to %s:80", wDigInt.gethost(data->ini) );
  iOSocket sh = SocketOp.inst( wDigInt.gethost(data->ini), 80, False, False, False );
  if( SocketOp.connect( sh ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Connected to %s", wDigInt.gethost(data->ini) );

    char* httpReq = StrOp.fmt("%s HTTP/1.1\nHost: %s\nContent-Type: application/json\nContent-Length: %d\n\n%s", method, wDigInt.gethost(data->ini), StrOp.len(request), request );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "length=%d\n%s", StrOp.len(httpReq), httpReq );
    SocketOp.write( sh, httpReq, StrOp.len(httpReq) );
    StrOp.free(httpReq);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Read response..." );
    char str[1024] = {'\0'};
    int idx = 0;
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

        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, str );
      };

      if( OK && contlen > 0 ) {
        char* reply = (char*)allocMem(contlen+1);
        SocketOp.read( sh, reply, contlen );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reply = %.200s", reply );
      }
      else if( OK ) {
        while( SocketOp.read( sh, &str[idx], 1 ) && !SocketOp.isBroken( sh ) && idx < 1024) {
          idx++;
          str[idx] = '\0';
        }
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reply: %s", str  );
      }
    }

    SocketOp.disConnect(sh);
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not connected to %s", wDigInt.gethost(data->ini) );
  }
  SocketOp.base.del(sh);

  return reply;

}


void __RGBtoXY(int R, int G, int B, float* x, float* y) {
  float red   = ( (float)R / 255.0 );        //R from 0 to 255
  float green = ( (float)G / 255.0 );        //G from 0 to 255
  float blue  = ( (float)B / 255.0 );        //B from 0 to 255

  if ( red > 0.04045 )
    red = pow( ( red + 0.055 ) / 1.055, 2.4);
  else
    red = red / 12.92;

  if ( green > 0.04045 )
    green = pow( ( green + 0.055 ) / 1.055, 2.4);
  else
    green = green / 12.92;

  if ( blue > 0.04045 )
    blue = pow( ( blue + 0.055 ) / 1.055, 2.4);
  else
    blue = blue / 12.92;

  float X = (float) (red * 0.649926 + green * 0.103455 + blue * 0.197109);
  float Y = (float) (red * 0.234327 + green * 0.743075 + blue * 0.022598);
  float Z = (float) (red * 0.000000 + green * 0.053077 + blue * 1.035763);

  *x = X / ( X + Y + Z );
  *y = Y / ( X + Y + Z );
}



static iONode __translate( iOHUE inst, iONode node ) {
  iOHUEData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cmd=%s", NodeOp.getName( node ) );
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {

  /* System command. */
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
    int val  = wOutput.getvalue( node );
    Boolean active = StrOp.equals( wOutput.getcmd( node ), wOutput.on );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output addr=%d", addr );

    iHueCmd cmd = allocMem(sizeof(struct HueCmd));
    cmd->methode = StrOp.fmt("PUT /api/%s/lights/%d/state", wDigInt.getuserid(data->ini), addr);
    cmd->request = StrOp.fmt("{\"on\":%s, \"bri\":%d}", active?"true":"false", val);
    ThreadOp.post( data->transactor, (obj)cmd );

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
  iOHUEData data = Data(inst);
  data->run = False;
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


static void __transactor( void* threadinst ) {
  iOThread  th   = (iOThread)threadinst;
  iOHUE     hue  = (iOHUE)ThreadOp.getParm(th);
  iOHUEData data = Data(hue);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Transactor is started.");

  do {
    iHueCmd cmd = (iHueCmd)ThreadOp.getPost( th );
    if (cmd != NULL) {
      __httpRequest(hue, cmd->methode, cmd->request);
      StrOp.free(cmd->methode);
      StrOp.free(cmd->request);
      freeMem(cmd);
    }
  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Transactor has stopped.");
}


/**  */
static struct OHUE* _inst( const iONode ini ,const iOTrace trc ) {
  iOHUE __HUE = allocMem( sizeof( struct OHUE ) );
  iOHUEData data = allocMem( sizeof( struct OHUEData ) );
  MemOp.basecpy( __HUE, &HUEOp, 0, sizeof( struct OHUE ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini     = ini;
  data->iid     = StrOp.dup( wDigInt.getiid( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Philips HUE %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  iid   : [%s]", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  bridge: [%s]", wDigInt.gethost(data->ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  user  : [%s]", wDigInt.getuserid(data->ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->run = True;
  data->transactor = ThreadOp.inst( data->iid, &__transactor, __HUE );
  ThreadOp.start( data->transactor );

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
