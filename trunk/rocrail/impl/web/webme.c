/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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
#include "rocrail/impl/pclient_impl.h"

#include "rocrail/impl/web/webme.h"
#include "rocrail/impl/web/web.h"


#include "rocs/public/str.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/socket.h"






Boolean rocWebME( iOPClient inst, const char* str ) {
  TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "work for rocWebME [%s]", str );
  
  if( inst != NULL ) {
    iOPClientData data = Data(inst);
    
    webHeader( data->socket );
    
    SocketOp.fmt( data->socket, "\n<!-- rocWebME -->\n" );
    SocketOp.fmt( data->socket, "<table width='220px' border='0' cellpadding='5' cellspacing='0'>\n" );
    SocketOp.fmt( data->socket, "<tr><td align='center' colspan='3'><img border='0' src='images/logo.png'>\n" );
    
    SocketOp.fmt( data->socket, "<tr><td align='center'><a href='system.html'><img border='0' src='images/power.png'></a>\n" );
    SocketOp.fmt( data->socket, "<td align='center'><a href='auto.html'><img border='0' src='images/automode.png'></a>\n" );
    SocketOp.fmt( data->socket, "<td align='center'><a href='auto.html'><img border='0' src='images/stopall.png'></a>\n" );
    
    SocketOp.fmt( data->socket, "<tr><td align='center'><a href='loco.html'><img border='0' src='images/locctrl.png'></a>\n" );
    SocketOp.fmt( data->socket, "<td align='center'><a href='switch.html'><img border='0' src='images/swctrl.png'></a>\n" );
    SocketOp.fmt( data->socket, "<td align='center'><a href='routes.html'><img border='0' src='images/routes.png'></a>\n" );
    
    SocketOp.fmt( data->socket, "<tr><td align='center'><a href='loco.html'><img border='0' src='images/schedules.png'></a>\n" );
    SocketOp.fmt( data->socket, "<td align='center'><a href='switch.html'><img border='0' src='images/server.png'></a>\n" );
    SocketOp.fmt( data->socket, "<td align='center'><a href='routes.html'><img border='0' src='images/properties.png'></a>\n" );
    
    SocketOp.fmt( data->socket, "</table>\n" );
    
    
    webFooter( data->socket );
  }
  
  
  return True;
}

