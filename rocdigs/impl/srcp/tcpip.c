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


#include "rocdigs/impl/srcp_impl.h"

#include "rocdigs/impl/srcp/tcpip.h"
#include "rocdigs/impl/srcp/srcp.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SRCP.h"


Boolean tcpipInit( obj inst ) {
  iOSRCPData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  host      : %s", data->host );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  port      : %d", data->port );
  return False;
}


static int __tcpipConnectInfo( obj inst ) {
  iOSRCPData o = Data(inst);
  if( o->infoSocket == NULL ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Connecting info port %s:%d...", o->host, o->port );
    o->infoSocket = SocketOp.inst( o->host, o->port, False, False, False );
  }

  if( o->infoSocket != NULL && !SocketOp.isConnected( o->infoSocket ) ) {
    if( SocketOp.connect( o->infoSocket ) ) {
      /* Handshake */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "info port connected" );
      return SRCPCONNECT_RECONNECTED;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "unable to connecting info port %s:%d...", o->host, o->port );
      if( o->infoSocket != NULL ) {
        SocketOp.base.del(o->infoSocket);
        o->infoSocket = NULL;
      }
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "error connecting info port" );
      return SRCPCONNECT_ERROR;
    }
  }
  else if( o->infoSocket != NULL && SocketOp.isConnected( o->infoSocket ) ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "info port already connected" );
    return SRCPCONNECT_OK;
  }

  return SRCPCONNECT_ERROR;
}


int tcpipConnect( obj inst, Boolean info ) {
  iOSRCPData o = Data(inst);
  int rc = SRCPCONNECT_OK;
  char inbuf[1024];
  /* Will be enough. spec says, no line longer than 1000 chars. */
  char id[1024], data[1024];
  /* Boolean found = False; */

  if( info ) {
    return __tcpipConnectInfo(inst);
  }

  if ( o->cmdSocket == NULL ) {
    o->cmdSocket = SocketOp.inst( o->host, o->port, False, False, False );
    SocketOp.setSndTimeout( o->cmdSocket, wDigInt.gettimeout(o->ini));
    SocketOp.setRcvTimeout( o->cmdSocket, wDigInt.gettimeout(o->ini));
    rc = SRCPCONNECT_RECONNECTED;
  }

  /* Disconnect if connected */
  if ( o->cmdSocket != NULL && !SocketOp.isConnected( o->cmdSocket ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Connecting to SRCP server %s:%d", o->host, o->port );
    rc = SRCPCONNECT_RECONNECTED;

    if ( !SocketOp.connect( o->cmdSocket ) ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR connecting to SRCP server %s:%d", o->host, o->port );
      return SRCPCONNECT_ERROR;
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Handshaking" );

    if ( !SocketOp.readln( o->cmdSocket, inbuf ) ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking with SRCP server %s:%d", o->host, o->port );
      SocketOp.disConnect( o->cmdSocket );
      return SRCPCONNECT_ERROR;
    }

    /*
     * All words are case-sensitive. Commands and replies of the SRCP are always written in uppercase letters.
     * The following keys MUST be determined during normal welcome:
     * SRCP <version>
     */
    StrOp.replaceAll(inbuf, '\n', ' ');
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Response from server: %s", inbuf );
    if ( StrOp.findi( inbuf, "SRCP 0.8." ) != NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Server response for protocol 0.8 ok." );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking. No supported protocol found!" );
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, inbuf );
      SocketOp.disConnect( o->cmdSocket );
      return SRCPCONNECT_ERROR;
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Handshake completed." );
  }

  return rc;
}


void tcpipDisconnect( obj inst, Boolean info ) {
  iOSRCPData o = Data(inst);
  if( info && o->infoSocket != NULL ) {
    SocketOp.disConnect(o->infoSocket);
    SocketOp.base.del(o->infoSocket);
    o->infoSocket = NULL;
  }
  else if( !info && o->cmdSocket != NULL ) {
    iOSocket socket = o->cmdSocket;
    o->cmdSocket = NULL;
    SocketOp.disConnect( socket );
    SocketOp.base.del(socket);
  }
}


int tcpipRead ( obj inst, char *inbuf, Boolean info ) {
  iOSRCPData o = Data(inst);
  if( info && o->infoSocket != NULL ) {
    if( SocketOp.isBroken(o->infoSocket) )
      return -1;
    return SocketOp.readln( o->infoSocket, inbuf ) != NULL ? StrOp.len(inbuf):0;
  }
  else if( !info && o->cmdSocket != NULL ) {
    if( SocketOp.isBroken(o->cmdSocket) )
      return -1;
    return SocketOp.readln( o->cmdSocket, inbuf ) != NULL ? StrOp.len(inbuf):0;
  }
  return 0;
}


int tcpipWrite( obj inst, const char *szCommand, char* szRetVal, Boolean info ) {
  iOSRCPData o = Data(inst);
  char inbuf[1024] = { 0 };
  char szResponse[1024];
  int  retstate = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "write command: %s", szCommand );

  /* write info port */
  if( info ) {
    if( !SocketOp.write( o->infoSocket, szCommand, StrOp.len(szCommand)) )
      return -1;
    if( szRetVal != NULL ) {
      SocketOp.readln(o->infoSocket,szRetVal);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "read line: %s", szRetVal );
    }
    return retstate;
  }

  /* write command port */
  if (szRetVal != NULL )
    szRetVal[0]= '\0';

  if ((o->cmdSocket == NULL) || (!SocketOp.isConnected( o->cmdSocket ))) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "not connected in SendCommand (socket=0x%08X)", o->cmdSocket);
    if(o->cmdSocket != NULL) {
      SocketOp.base.del(o->cmdSocket);
      o->cmdSocket = NULL;
    }
    return -1;
  }

  if (!SocketOp.write( o->cmdSocket, szCommand, StrOp.len(szCommand))) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not write: %s", szCommand );
    return -1;
  }
  else {
    StrOp.replaceAll((char*)szCommand, '\n', ' ');
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "command written: %s",szCommand);
  }

  /* Read server response: */
  if (! SocketOp.readln(o->cmdSocket,inbuf) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SendCommand: could not read response");
    SocketOp.disConnect(o->cmdSocket);
    SocketOp.base.del(o->cmdSocket);
    o->cmdSocket = NULL;

    if( o->infoSocket != NULL ) {
      /* Trigger the socket to generate an exception. */
      SocketOp.write( o->infoSocket, szCommand, StrOp.len(szCommand));
      SocketOp.disConnect(o->infoSocket);
      SocketOp.base.del(o->infoSocket);
      o->infoSocket = NULL;
    }
    return -1;
  }

  StrOp.replaceAll(inbuf, '\n', ' ');
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "srcp response: %s",inbuf);

  /* Scan for SM return? */
  MemOp.set(szResponse,0,900);
  sscanf(inbuf,"%*s %d %900c",&retstate,szResponse);

  o->state = (SRCP_OK(retstate)?SRCP_STATE_OK:SRCP_STATE_ERROR);

  if( szRetVal != NULL ) {
    StrOp.copy( szRetVal, szResponse );
  }

  return retstate;
}


Boolean tcpipAvailable( obj inst ) {
  iOSRCPData data = Data(inst);
  return True;
}



