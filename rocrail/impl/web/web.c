/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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

#include "rocs/public/str.h"
#include "rocs/public/trace.h"
#include "rocs/public/socket.h"


Boolean webHeader( iOSocket s ) {
  Boolean ok = True;
  if(ok) ok=SocketOp.fmt( s, "HTTP/1.0 200 OK\r\n" );
  if(ok) ok=SocketOp.fmt( s, "Content-type: text/html\r\n\r\n" );
  if(ok) ok=SocketOp.fmt( s, "<html><head><title>RocRail WebClient</title>\n" );
  if(ok) ok=SocketOp.fmt( s, "<link href=\"rocrail.gif\" rel=\"shortcut icon\">\n" );
  
  if(ok) ok=SocketOp.fmt( s, "<STYLE type=\"text/css\">\n" );
  if(ok) ok=SocketOp.fmt( s, "<!--\n" );
  if(ok) ok=SocketOp.fmt( s, "A { text-decoration:none }\n" );
  if(ok) ok=SocketOp.fmt( s, "a:link { text-decoration:none; color:#000000; }\n" );
  if(ok) ok=SocketOp.fmt( s, "a:visited { text-decoration:none; color:#000000; }\n" );
  /*SocketOp.fmt( s, "td { text-align:center; vertical-align:middle; }\n" );*/
  if(ok) ok=SocketOp.fmt( s, "-->\n" );
  if(ok) ok=SocketOp.fmt( s, "</STYLE>\n" );
  if(ok) ok=SocketOp.fmt( s, "</head><body>\n" );
  return  ok;
}

Boolean webFooter( iOSocket s ) {
  Boolean ok = True;
  if(ok) ok=SocketOp.fmt( s, "</body></html>\n" );
  return  ok;
}


