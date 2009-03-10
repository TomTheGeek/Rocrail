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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rocdigs/impl/srcp_impl.h"

#include "rocdigs/public/srcp07.h"
#include "rocdigs/public/srcp08.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"


#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SRCP.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Response.h"


static int instCnt = 0;

#define SRCP_OK(x) ( x < 400 )
#define SRCP_ERROR(x) ( x >= 400 )

static Boolean __srcpConnect( iOSRCPData o );

/* ***** OBase functions. */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static const char * __name( void )
{
  return name;
}

static unsigned char * __serialize( void * inst, long * size )
{
  return NULL;
}

static void __deserialize( void * inst, unsigned char * a )
{
}

static char * __toString( void * inst )
{
  iOSRCPData data = Data( inst );
  return ( char * ) data->iid;
}

static void __del( void * inst )
{
  iOSRCPData data = Data( inst );
  freeMem( data );
  freeMem( inst );
  instCnt--;
}

static void * __properties( void * inst )
{
  return NULL;
}



static struct OBase * __clone( void * inst )
{
  return NULL;
}
static Boolean __equals



( void * inst1, void * inst2 )
{
  return False;
}

static int __count( void )
{
  return instCnt;
}

/* ***** Public functions. */
static Boolean _setListener( obj inst, obj listenerObj, const digint_listener listenerFun )
{
  iOSRCPData data = Data( inst );
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


static iONode _cmd( obj inst, const iONode nodeA )
{
  iOSRCPData o = Data( inst );

  if ( o->srcpversion == SRCP_07 )
  {
    return SRCP07Op.cmd( o->srcpx, nodeA );
  }
  else if ( o->srcpversion == SRCP_08 )
  {
    return SRCP08Op.cmd( o->srcpx, nodeA );
  }
  return NULL;
}

static void _halt( obj inst )
{
  iOSRCPData o = Data( inst );
  o->run = False;

  if ( o->srcpversion == SRCP_07 )
    SRCP07Op.halt( o->srcpx );
  else if ( o->srcpversion == SRCP_08 )
    SRCP08Op.halt( o->srcpx );
}

static Boolean _supportPT( obj inst ) {
  iOSRCPData data = Data(inst);

  if ( data->srcpversion == SRCP_08 )
    return SRCP08Op.supportPT( data->srcpx );
  else if ( data->srcpversion == SRCP_07 )
    return SRCP07Op.supportPT( data->srcpx );

  return False;
}



static void __feedbackReader( void * threadinst )
{
  iOThread th = ( iOThread )threadinst;
  iOSRCP07 srcp = ( iOSRCP07 )ThreadOp.getParm( th );
  iOSRCPData o = Data( srcp );
  Boolean handshakeerror = False;
  Boolean exception = False;
  Boolean srcp08 = False;
  Boolean evalfirst = False;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Connecting FB port %s:%d...",
               o->ddlHost, o->fbackPort );
  o->fbackSocket = SocketOp.inst( o->ddlHost, o->fbackPort, False, False );

  if ( SocketOp.connect( o->fbackSocket ) )
  {
    char inbuf[1024] = { 0 };
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "FB Connected" );

    if ( SocketOp.readln( o->fbackSocket, inbuf ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, inbuf );
      if( StrOp.findi( inbuf, "SRCP 0.8" ) ) {
        const char* protStr = "SET PROTOCOL SRCP 0.8.2\n";
        const char* connStr = "SET CONNECTIONMODE SRCP INFO\n";
        const char* goStr = "GO\n";
        srcp08 = True;

        if( !handshakeerror ) {
          SocketOp.write( o->fbackSocket, protStr, StrOp.len(protStr) );
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "%s",protStr);

          /*"OK PROTOCOL SRCP"*/
          SocketOp.readln( o->fbackSocket, inbuf );
          if( !StrOp.find( inbuf, "201" ) ) {
            /* error */
            handshakeerror = True;
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: expecting 201, received [%s]",inbuf);
          }
        }

        if( !handshakeerror ) {
          SocketOp.write( o->fbackSocket, connStr, StrOp.len(connStr) );
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "%s",connStr);

          /*"OK CONNECTION MODE"*/
          SocketOp.readln( o->fbackSocket, inbuf );
          if( !StrOp.find( inbuf, "202" ) ) {
            /* error */
            handshakeerror = True;
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: expecting 202, received [%s]",inbuf);
          }
        }

        if( !handshakeerror ) {
          SocketOp.write( o->fbackSocket, goStr, StrOp.len(goStr) );
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "%s",goStr);

          /*"OK GO"*/
          SocketOp.readln( o->fbackSocket, inbuf );
          if( !StrOp.find( inbuf, "200" ) ) {
            /* error */
            handshakeerror = True;
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: expecting 200, received [%s]",inbuf);
          }
        }


      }
      else {
      	/* it's the first INFO message */
      	evalfirst = True;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "SRCP 0.7 FB Connection" );
      }
    }

    while( o->run && !handshakeerror )
    {
      Boolean readok = False;

      if( evalfirst ) {
        readok = evalfirst;
        evalfirst = False;
      }
      else if( SocketOp.readln( o->fbackSocket, inbuf ) != NULL )
        readok =  True;

      if ( readok ) {
        char* fbAddrStr       = NULL;
        iOStrTok tok            = NULL;
        int infotype            = 0; /* 0=FB, 1=GA */
        char  tracestr[ 1024 ]  = { 0 };

        strncpy( tracestr, inbuf, ( strlen( inbuf ) - 1 ));
        tracestr[ strlen( inbuf ) ] = '0';
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fbAddrStr = [%s]", tracestr );

        if( StrOp.find( inbuf, "INFO ") ) {
          if( StrOp.find( inbuf, "FB ") )
            infotype = 0;
          if( StrOp.find( inbuf, "GA ") )
            infotype = 1;

          if( srcp08 ) {
            if( !StrOp.find( inbuf, "FB POWER") )
              fbAddrStr = StrOp.find( inbuf, infotype==0?"FB ":"GA ");
          }
          else {
            /* srcp 0.7 */
            fbAddrStr = StrOp.find( inbuf, "S88 ");
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCP 0.7 inbuf=[%s] fbAddrStr = [%s]", inbuf,
            fbAddrStr );
          }
        }

        if( !fbAddrStr ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "no FB/GA info..." );
          ThreadOp.sleep( 10 );
          continue;
        }

        tok = StrTokOp.inst( fbAddrStr, ' ' );

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%s addresses: [%s]", (infotype==0)?"sensor":"turnout", fbAddrStr );

        TraceOp.dump( NULL, TRCLEVEL_BYTE, inbuf, StrOp.len( inbuf ) );

        if( StrTokOp.hasMoreTokens( tok ) ) {
          const char* leadinStr = StrTokOp.nextToken( tok );
        }

        while ( StrTokOp.hasMoreTokens( tok ) )
        {
          const char* addrStr = StrTokOp.nextToken( tok );
          if ( StrTokOp.hasMoreTokens( tok ) )
          {
            const char* valStr = NULL;
            iONode nodeC = NULL;
            int addr = atoi( addrStr );
            int port = 0;
            int val  = 0;

            if( infotype == 1 ) {
              /* GA */
              valStr = StrTokOp.nextToken( tok );
              port = atoi( valStr );
              if( StrTokOp.hasMoreTokens( tok ) ) {
                valStr = StrTokOp.nextToken( tok );
                val = atoi( valStr );
              }
              else {
                TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "unexpected end of GA %d, %d info", addr, port );
                break;
              }

              if( val != 1 ) {
                TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "not interrested in GA %d OFF state", addr );
                break;
              }
              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "switch %d port %d = %d", addr, port, val );
            }
            else {
              /* FB */
              valStr = StrTokOp.nextToken( tok );
              val = atoi( valStr );
              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sensor %d = %d", addr, val );
            }

            if( infotype == 1 ) {
              nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setaddr1( nodeC, (addr-1)/4 + 1 );
              wSwitch.setport1( nodeC, (addr-1)%4 + 1 );
              wSwitch.setstate( nodeC, port ? wSwitch.straight : wSwitch.turnout );
              if ( o->iid != NULL )
                wSwitch.setiid( nodeC, o->iid );
            }
            else {
              nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
              wFeedback.setaddr( nodeC, addr );
              wFeedback.setstate( nodeC, val ? True : False );
              if ( o->iid != NULL )
                wFeedback.setiid( nodeC, o->iid );
            }

            if ( o->listenerFun != NULL && o->listenerObj != NULL )
              o->listenerFun( o->listenerObj, nodeC, TRCLEVEL_INFO );
          }
          else {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "not an INFO line:" );
            TraceOp.dump( NULL, TRCLEVEL_DEBUG, inbuf, StrOp.len( inbuf ) );
          }

        };
        /* end while */

      }
      /* end if */
      else {
        exception = True;
      }

      if( exception ) {
        exception = False;
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Try to reconnect..." );
        SocketOp.disConnect( o->fbackSocket );
        ThreadOp.sleep( 1000 );
        SocketOp.connect( o->fbackSocket );
      }
      else
        ThreadOp.sleep( 10 );

    };

  }
  else
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR connecting to FB port %s:%d rc=%d",
        o->ddlHost, o->fbackPort, SocketOp.getRc( o->fbackSocket ) );
  }

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "FB reader ended" );

}


static void __infoReader( void * threadinst )
{
  iOThread th = ( iOThread )threadinst;
  iOSRCP07 srcp = ( iOSRCP07 )ThreadOp.getParm( th );
  iOSRCPData o = Data( srcp );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Connecting INFO port %s:%d...",
               o->ddlHost, o->infoPort );
  o->infoSocket = SocketOp.inst( o->ddlHost, o->infoPort, False, False );
  if ( SocketOp.connect( o->infoSocket ) )
  {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Connected" );
    do
    {
      char inbuf[ 1024 ]    = { 0 };
      char tracestr[ 1024 ] = { 0 };

      if ( SocketOp.readln( o->infoSocket, inbuf ) && !SocketOp.isBroken( o->infoSocket )) {

          /* Call callback: */

        if ( StrOp.len( inbuf ) > 0 ) {

          strncpy( tracestr, inbuf, ( strlen( inbuf ) - 1 ));
          tracestr[ strlen( inbuf ) ] = '0';
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "infoReader: %s", tracestr );

        } else {
          ThreadOp.sleep( 1000 );
        }
      } else {
        SocketOp.disConnect( o->infoSocket );
        ThreadOp.sleep( 1000 );
        SocketOp.connect( o->infoSocket );
      }
      ThreadOp.sleep( 100 );
    }
    while( o->run );
  }
  else
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR connecting to INFO port %s:%d",
                 o->ddlHost, o->infoPort );
  }
}


static Boolean __srcpConnect( iOSRCPData o )
{
  char inbuf[1024];
  /* Will be enough. spec says, no line longer than 1000 chars. */
  char * token;
  char id[1024], data[1024];
  /* Boolean found = False; */

  if ( o->cmdSocket == NULL )
    o->cmdSocket = SocketOp.inst( o->ddlHost, o->cmdPort, False, False );

  /* Disconnect if connected */
  if ( SocketOp.isConnected( o->cmdSocket ) )
    SocketOp.disConnect( o->cmdSocket );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Connecting to SRCP server %s:%d",
               o->ddlHost, o->cmdPort );

  if ( !SocketOp.connect( o->cmdSocket ) )
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR connecting to SRCP server %s:%d",
                 o->ddlHost, o->cmdPort );
    return False;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Handshaking" );

  if ( !SocketOp.readln( o->cmdSocket, inbuf ) )
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking with SRCP server %s:%d",
                 o->ddlHost, o->cmdPort );
    SocketOp.disConnect( o->cmdSocket );
    return False;
  }

  {
    int len = StrOp.len( inbuf );
    if ( inbuf[len - 1] == '\n' )
      inbuf[len - 1] = '\0';
  }

  /*
   * All words are case-sensitive. Commands and replies of the SRCP are always written in uppercase letters.
   * The following keys MUST be determined during normal welcome:
   * SRCP <version>
   */
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Response from server: %s", inbuf );
  if ( StrOp.findi( inbuf, "SRCP 0.7." ) != NULL )
  {
    o->srcpversion = SRCP_07;
    o->srcpx = ( obj )SRCP07Op.inst( o->ini, o->trace, o->cmdSocket );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Server response for protocol 0.7 ok." );
  }
  else if ( StrOp.findi( inbuf, "SRCP 0.8." ) != NULL )
  {
    o->srcpversion = SRCP_08;
    o->srcpx = ( obj )SRCP08Op.inst( o->ini, o->trace, o->cmdSocket );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Server response for protocol 0.8 ok." );
  }
  else
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking. No supported protocol found!" );
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, inbuf );
    SocketOp.disConnect( o->cmdSocket );
    return False;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Handshake completed." );
  return True;
}


/* Status */
static int _state( obj inst ) {
  iOSRCPData data = Data(inst);
  int state = 0;
  return state;
}

/* VERSION: */
static int vmajor = 1;
static int vminor = 3;
static int patch  = 0;
static int _version( obj inst ) {
  iOSRCPData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

static iOSRCP _inst( const iONode settings, const iOTrace trace )
{
  iOSRCP srcp = allocMem( sizeof( struct OSRCP ) );
  iOSRCPData data = allocMem( sizeof( struct OSRCPData ) );
  int i = 0;

  TraceOp.set( trace );

  /* OBase */
  MemOp.basecpy( srcp, & SRCPOp, 0, sizeof( struct OSRCP ), data );

  data->ini   = settings;
  data->trace = trace;

  data->srcpini = wDigInt.getsrcp(data->ini);

  if( data->srcpini == NULL ) {
    data->srcpini = NodeOp.inst( wSRCP.name(), data->ini, ELEMENT_NODE );
    NodeOp.addChild(data->ini, data->srcpini );
  }

  /* Evaluate attributes. */
  data->iid = StrOp.dup( wDigInt.getiid( settings ) );

  data->ddlHost   = wDigInt.gethost( settings );
  data->cmdPort   = wSRCP.getcmdport( data->srcpini );
  data->infoPort  = wSRCP.getinfoport( data->srcpini );
  data->fbackPort = wSRCP.getfbackport( data->srcpini );
  data->run       = True;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "srcp %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  if( __srcpConnect( data ) ) {
    if ( data->fbackPort > 0 )
    {
      char * fbname = StrOp.fmt( "ddlfb%08X", srcp );
      data->fbackReader = ThreadOp.inst( fbname, & __feedbackReader, srcp );
      ThreadOp.start( data->fbackReader );
    }

    if ( data->infoPort > 0 )
    {
      char * infoname = StrOp.fmt( "ddlif%08X", srcp );
      data->infoReader = ThreadOp.inst( infoname, & __infoReader, srcp );
      ThreadOp.start( data->infoReader );
    }
  }

  instCnt++;

  return srcp;
}

/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini, const iOTrace trc )
{
  return ( iIDigInt )_inst( ini, trc );
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! ----- */
#include "rocdigs/impl/srcp.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! ----- */
