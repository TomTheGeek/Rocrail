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
#include "rocrail/public/model.h"
#include "rocrail/public/clntcon.h"
#include "rocrail/public/loc.h"
#include "rocrail/public/app.h"
#include "rocrail/impl/hclient_impl.h"
#include "rocrail/wrapper/public/Global.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Trace.h"

#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"
#include "rocs/public/system.h"
#include "rocs/public/file.h"
#include "rocs/public/thread.h"
#include "rocs/public/map.h"
#include "rocs/public/strtok.h"
#include "rocs/public/dir.h"

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
    iOHClientData data = Data(inst);
    /* Cleanup data->xxx members...*/

    if( data->socket != NULL ) {
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "delete client socket %s...", data->cid );
      SocketOp.base.del( data->socket );
    }
    StrOp.free( data->cid );

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

/** ----- OHClient ----- */


/** HClient ID. */
static const char* _getId( struct OHClient* inst ) {
  iOHClientData data = Data(inst);
  return data->cid;
}


unsigned char rocrail_gif[] = {
  0x47, 0x49, 0x46, 0x38, 0x39, 0x61, 0x10, 0x00, 0x10, 0x00, 0x84, 0x15, 0x00, 0x00, 0x00, 0x00,
  0x39, 0x00, 0x00, 0x21, 0x20, 0x21, 0x7b, 0x00, 0x00, 0x31, 0x30, 0x31, 0x39, 0x38, 0x39, 0x42,
  0x41, 0x42, 0x7b, 0x38, 0x39, 0x52, 0x51, 0x52, 0x5a, 0x59, 0x5a, 0x63, 0x61, 0x63, 0x73, 0x71,
  0x73, 0x7b, 0x79, 0x7b, 0x84, 0x86, 0x84, 0x94, 0x96, 0x94, 0x9c, 0x9e, 0x9c, 0xa5, 0xa6, 0xa5,
  0xb5, 0xb6, 0xb5, 0xbd, 0xbe, 0xbd, 0xc6, 0xc7, 0xc6, 0xde, 0xdf, 0xde, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x21, 0xfe, 0x07,
  0x52, 0x6f, 0x63, 0x52, 0x61, 0x69, 0x6c, 0x00, 0x21, 0xf9, 0x04, 0x01, 0x0a, 0x00, 0x1f, 0x00,
  0x2c, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 0x10, 0x00, 0x00, 0x05, 0x80, 0x60, 0x25, 0x8e, 0x64,
  0x39, 0x32, 0xd2, 0x63, 0x96, 0x8c, 0xe1, 0x44, 0x90, 0xe3, 0x34, 0x0a, 0x43, 0x95, 0x0a, 0xd2,
  0x2c, 0x8d, 0xa1, 0x40, 0x10, 0x45, 0x23, 0x51, 0x92, 0x18, 0x45, 0x0c, 0xd4, 0xa3, 0x25, 0x30,
  0x49, 0x92, 0x92, 0x12, 0x81, 0x41, 0x62, 0x28, 0xae, 0x0a, 0xc0, 0x94, 0x50, 0xa9, 0x8d, 0x04,
  0x02, 0xc2, 0xb5, 0x20, 0x46, 0xe4, 0x14, 0x84, 0x69, 0x25, 0x7d, 0x75, 0x5c, 0x27, 0x10, 0xf8,
  0xe4, 0x5a, 0xb1, 0x62, 0x65, 0x62, 0x83, 0x1e, 0x21, 0xae, 0x48, 0xc4, 0x04, 0x00, 0x00, 0x68,
  0x83, 0x81, 0x59, 0x05, 0x12, 0x05, 0x0a, 0x13, 0x04, 0x03, 0x03, 0x04, 0x01, 0x8f, 0x91, 0x82,
  0x05, 0x05, 0x7e, 0x49, 0x07, 0x03, 0x0c, 0x99, 0x9b, 0x03, 0x4f, 0x51, 0x5a, 0x81, 0x69, 0xa1,
  0xa4, 0xa3, 0x0d, 0xa7, 0xa8, 0xa9, 0xaa, 0x0d, 0x2b, 0xad, 0x15, 0x21, 0x00, 0x3b };

int rocrail_gif_len = sizeof(rocrail_gif);

static void __httpHeader( iOSocket s ) {
  SocketOp.fmt( s, "HTTP/1.0 200 OK\r\n" );
  SocketOp.fmt( s, "Content-type: text/html\r\n\r\n" );
}


static void __header( iOSocket s, int refresh ) {
  SocketOp.fmt( s, "HTTP/1.0 200 OK\r\n" );
  SocketOp.fmt( s, "Content-type: text/html\r\n\r\n" );
  SocketOp.fmt( s, "<html><head><title>Rocrail HTTP Service </title>\n" );
  SocketOp.fmt( s, "<link href=\"rocrail.gif\" rel=\"shortcut icon\">\n" );
  if( refresh > 0 )
    SocketOp.fmt( s, "<META CONTENT=\"%d\" HTTP-EQUIV=\"refresh\">\n", refresh );
  SocketOp.fmt( s, "</head><body>\n" );
}

static void __footer( iOSocket s ) {
  SocketOp.fmt( s, "<br><a href=\"/\">Rocrail</a><br>\n" );
  SocketOp.fmt( s, "<a href=\"mailto:support@rocrail.net\">support@rocrail.net</a>\n" );
  SocketOp.fmt( s, "</body></html>\n" );
}

/** ------------------------------------------------------------
  * __scan4Html()
  * Scans for external HTML documents.
  *
  * @param inst     HClient instance.
  * @return
  */
static void __scan4Html( iOHClient inst ) {
  iOHClientData   data = Data(inst);
  iODir            dir = NULL;
  const char* fileName = NULL;

  dir = DirOp.inst( data->path );
  if( dir != NULL ) {
    /* Get the first directory entry. */
    fileName = DirOp.read( dir );

    SocketOp.fmt( data->socket, "<table cellpadding=\"4\" cellspacing=\"0\">" );

    /* Iterate all directory entries. */
    while( fileName != NULL ) {
      if( StrOp.endsWithi( fileName, ".html" ) || StrOp.endsWithi( fileName, ".htm" ) ) {
        char* path = StrOp.fmt( "%s%c%s", data->path, SystemOp.getFileSeparator(), fileName );
        long size  = FileOp.fileSize( path );
        long ftime = FileOp.fileTime( path );
        StrOp.replaceAll( path, '\\', '/' );
        SocketOp.fmt( data->socket, "<tr><td><a href=\"%s\">%s</a></td><td align=\"right\">%ld</td><td align=\"right\">%s</td></tr>\n",
                      path, path, size, ctime(&ftime) );
        StrOp.free(path);
      }
      /* Get the next directory entry. */
      fileName = DirOp.read( dir );
    };

    SocketOp.fmt( data->socket, "</table>\n" );
    /* Close and cleanup. */
    DirOp.close( dir );
    dir->base.del( dir );
  }
}


/** ------------------------------------------------------------
  * __scan4Trc()
  * Scans for trace files.
  *
  * @param inst     HClient instance.
  * @return
  */
static void __scan4Trc( iOHClient inst ) {
  iOHClientData   data = Data(inst);
  iODir            dir = NULL;
  const char* fileName = NULL;
  char* tracepath = FileOp.getPath( wTrace.getrfile( wRocRail.gettrace( AppOp.getIni() ) ) );
  const char* tracefile = FileOp.ripPath( wTrace.getrfile( wRocRail.gettrace( AppOp.getIni() ) ) );
  Boolean      absolute = False;

  if( tracepath == NULL )
    tracepath = ".";

  absolute = FileOp.isAbsolute( tracepath );

  dir = DirOp.inst( tracepath );
  if( dir != NULL ) {
    /* Get the first directory entry. */
    fileName = DirOp.read( dir );

    SocketOp.fmt( data->socket, "<table cellpadding=\"4\" cellspacing=\"0\">" );

    /* Iterate all directory entries. */
    while( fileName != NULL ) {
      if( StrOp.find( fileName, tracefile ) && StrOp.find( fileName, ".trc" ) ) {
        char sep[10] = {SystemOp.getFileSeparator(),'.',SystemOp.getFileSeparator(),0};
        char* realpath = StrOp.fmt( "%s%c%s", tracepath, SystemOp.getFileSeparator(), fileName );
        char* path = StrOp.fmt( "%s%s%c%s", absolute?sep:"", tracepath, SystemOp.getFileSeparator(), fileName );
        long size  = FileOp.fileSize( realpath );
        long ftime = FileOp.fileTime( realpath );
        StrOp.replaceAll( path, '\\', '/' );
        SocketOp.fmt( data->socket, "<tr><td><a type=\"text/plain\" href=\"%s\">%s</a></td><td align=\"right\">%ld</td><td align=\"right\">%s</td></tr>\n",
                      path, path, size, ctime(&ftime) );
        StrOp.free( path );
        StrOp.free( realpath );
      }
      /* Get the next directory entry. */
      fileName = DirOp.read( dir );
    };

    SocketOp.fmt( data->socket, "</table>" );
    /* Close and cleanup. */
    DirOp.close( dir );
    dir->base.del( dir );
  }
  StrOp.free(tracepath);
}


static void __getHome( iOHClient inst ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__getHome( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOHClientData data = Data(inst);
    __header( data->socket, data->refresh );

    SocketOp.fmt( data->socket, "<h2><a href=\"http://www.rocrail.net\">%s</a> %d.%d.%d-%d %s (%s)</h2>",
             wGlobal.productname,
             wGlobal.vmajor,
             wGlobal.vminor,
             wGlobal.patch,
             AppOp.getrevno(),
             wGlobal.releasename,
             TraceOp.getOS() );

    SocketOp.fmt( data->socket, "<table cellpadding=\"4\">\n" );
    SocketOp.fmt( data->socket, "<tr><td>\n" );

    SocketOp.fmt( data->socket, "<table border=\"1\" cellpadding=\"4\" cellspacing= \"0\">\n" );
    SocketOp.fmt( data->socket, "<tr><td>process id       </td><td>%d    </td></tr>\n", SystemOp.getpid() );
    {
      long t = AppOp.getStartTime();
      SocketOp.fmt( data->socket, "<tr><td>started at</td><td>%s</td></tr>\n", ctime( &t ) );
    }

    char* pwd = FileOp.pwd();
    SocketOp.fmt( data->socket, "<tr><td>console mode     </td><td>%s    </td></tr>\n", AppOp.isConsoleMode()?"yes":"no" );
    SocketOp.fmt( data->socket, "<tr><td>working directory</td><td>%s    </td></tr>\n", pwd );
    SocketOp.fmt( data->socket, "<tr><td>allocated memory </td><td>%ld KB</td></tr>\n", MemOp.getAllocSize() / 1024 );
    SocketOp.fmt( data->socket, "<tr><td>clients          </td><td>%d    </td></tr>\n", ClntConOp.getClientCount( AppOp.getClntCon() ) );
    SocketOp.fmt( data->socket, "<tr><td>connections      </td><td>%d    </td></tr>\n", ClntConOp.getConCount( AppOp.getClntCon() ) );
    SocketOp.fmt( data->socket, "<tr><td>locos            </td><td>%d    </td></tr>\n", LocOp.base.count() );
    StrOp.free(pwd);
    {
      iOList thList = ThreadOp.getAll();
      int i = 0;
      int cnt = ListOp.size( thList );
      SocketOp.fmt( data->socket, "<tr><td valign=\"top\">%d threads</td><td>\n", cnt );
      SocketOp.fmt( data->socket, "<table>\n", cnt );
      for( i = 0; i < cnt; i++ ) {
        char* bgcolor = i%2==0 ? "bgcolor=\"#DDFFDD\"":"";
        iOThread th = (iOThread)ListOp.get( thList, i );
        const char* tname = ThreadOp.getName( th );
        char* tdesc = ThreadOp.base.toString( th );
        SocketOp.fmt( data->socket, "<tr %s><td>%s</td><td><small>%s</small></td></tr>\n", bgcolor, tname, tdesc );
        StrOp.free( tdesc );
      }
      SocketOp.fmt( data->socket, "</table>\n", cnt );
      SocketOp.fmt( data->socket, "</td></tr>\n", cnt );
      /* Cleanup. */
      thList->base.del( thList );
    }

    SocketOp.fmt( data->socket, "</table><br>\n" );

    SocketOp.fmt( data->socket, "</td><td valign=\"top\">\n" );


    SocketOp.fmt( data->socket, "<form action=\"trace\">\n" );
    SocketOp.fmt( data->socket, "<h3>TraceLevel:</h3>\n" );
    {
      Boolean debug = TraceOp.getLevel( NULL ) & TRCLEVEL_USER2 ? True:False;
      Boolean dbyte = TraceOp.getLevel( NULL ) & TRCLEVEL_BYTE ? True:False;
      SocketOp.fmt( data->socket, "Debug<input type=\"checkbox\" name=\"debug\" value=\"%s\" %s>\n",
               debug?"false":"true",
               debug?"checked":"" );
      SocketOp.fmt( data->socket, "Byte<input type=\"checkbox\" name=\"byte\" value=\"%s\" %s>\n",
               dbyte?"false":"true",
               dbyte?"checked":"" );
    }
    SocketOp.fmt( data->socket, "<input type=\"submit\" value=\"Submit\"><br>\n" );
    SocketOp.fmt( data->socket, "</form>\n" );

    SocketOp.fmt( data->socket, "<h3>Commands:</h3>\n" );
    SocketOp.fmt( data->socket, "<ul>\n" );
    SocketOp.fmt( data->socket, "<li><a href=\"shutdown\">Shutdown</a></li>\n" );
    SocketOp.fmt( data->socket, "</ul>\n" );

    SocketOp.fmt( data->socket, "<h3>Listings:</h3>\n" );
    SocketOp.fmt( data->socket, "<ul>\n" );
    SocketOp.fmt( data->socket, "<li><a href=\"locs\"    >Locs    </a></li>\n" );
    SocketOp.fmt( data->socket, "<li><a href=\"streets\" >Streets </a></li>\n" );
    /*SocketOp.fmt( data->socket, "<li><a href=\"blocks\"  >Block   </a></li>\n" );*/
    SocketOp.fmt( data->socket, "<li><a href=\"fbacks\"  >FBacks  </a></li>\n" );
    SocketOp.fmt( data->socket, "<li><a href=\"switches\">Switches</a></li>\n" );
    SocketOp.fmt( data->socket, "</ul>\n" );

    SocketOp.fmt( data->socket, "<h3>Documentation:</h3>\n" );
    SocketOp.fmt( data->socket, "<ul>\n" );
    SocketOp.fmt( data->socket, "<li><a title=\"Shows parsed ini as XML string.\" href=\"ini\">%s</a></li>\n",
                                AppOp.getIniFile() );
    SocketOp.fmt( data->socket, "<li><a href=\"rocrail_doc\">Rocrail</a></li>\n" );
    SocketOp.fmt( data->socket, "</ul>\n" );

    SocketOp.fmt( data->socket, "</td></tr>\n" );
    SocketOp.fmt( data->socket, "<tr><td colspan=\"2\">\n" );

    SocketOp.fmt( data->socket, "<h3>Trace files:</h3>\n" );
    __scan4Trc( inst );

    SocketOp.fmt( data->socket, "<h3>External documents:</h3>\n" );
    __scan4Html( inst );

    SocketOp.fmt( data->socket, "</td></tr>\n" );
    SocketOp.fmt( data->socket, "<tr><td colspan=\"2\">\n" );

    {
      int i = 0;
      const char** ex = AppOp.getBackTrace();
      SocketOp.fmt( data->socket, "<h3>BackTrace:</h3>(last 10 exceptions and warnings)\n" );
      SocketOp.fmt( data->socket, "<table cellspacing= \"0\">\n" );
      for( i = 9; i >= 0; i-- ) {
        char* bgcolor = i%2==1 ? "bgcolor=\"#DDFFDD\"":"";
        if( ex[i] != NULL ) {
          SocketOp.fmt( data->socket, "<tr %s><td><pre width=\"132\" style=\"display: inline\">%s</pre></td></tr>\n", bgcolor, ex[i] );
        }
      }
      SocketOp.fmt( data->socket, "</table>\n" );
    }

    SocketOp.fmt( data->socket, "</td></tr>\n" );
    SocketOp.fmt( data->socket, "</table>\n" );

    __footer( data->socket );


  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return;
  }
}


static void __getList( iOHClient inst, int list ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__getList( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOHClientData data = Data(inst);
    FILE* f = fopen( "list.html", "w+" );
    if( f == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unable to open outputfile." );
      return;
    }
    SocketOp.fmt( data->socket, "HTTP/1.0 200 OK\r\n" );
    SocketOp.fmt( data->socket, "Content-type: text/html\r\n\r\n" );


    if( list == 1 ) {
      ModelOp.printLocs( AppOp.getModel(), f );
    }
    else if( list == 2 ) {
      ModelOp.printRoutes( AppOp.getModel(), f );
    }
    else if( list == 3 ) {
      ModelOp.printBlocks( AppOp.getModel(), f );
    }
    else if( list == 4 ) {
      ModelOp.printFBacks( AppOp.getModel(), f );
    }
    else if( list == 5 ) {
      ModelOp.printSwitches( AppOp.getModel(), f );
    }

    fclose( f );

    if(FileOp.exist( "list.html" ) ) {
      iOFile f = FileOp.inst( "list.html", OPEN_READONLY );
      long len = FileOp.size( f );
      char* buff = allocMem( len );
      FileOp.read( f, buff, len );
      SocketOp.write( data->socket, buff, len );
      FileOp.base.del( f );
      freeMem( buff );
      FileOp.remove( "list.html" );
    }


  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return;
  }
}


static void __getIni( iOHClient inst ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__getIni( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOHClientData data = Data(inst);
    __header( data->socket, data->refresh );

    {
      char* str = AppOp.base.toString( NULL );
      int i = 0;
      SocketOp.fmt( data->socket, "<pre>\n" );
      for( i = 0; i < StrOp.len( str ); i++ ) {
        if( str[i] == '<' )
          SocketOp.fmt( data->socket, "&lt;" );
        else if( str[i] == '>' )
          SocketOp.fmt( data->socket, "&gt;" );
        else
          SocketOp.fmt( data->socket, "%c", str[i] );
      }
      SocketOp.fmt( data->socket, "</pre>\n" );
      StrOp.free( str );
    }


    __footer( data->socket );

  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return;
  }
}


/** ------------------------------------------------------------
  * __getTracefile()
  * Send the xSpooler ini as xml string to the client.
  *
  * @param inst     HClient instance.
  * @return
  */
static void __getTracefile( iOHClient inst, const char* currentTrace ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__getTracefile( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOHClientData data = Data(inst);
    __header( data->socket, data->refresh );

    SocketOp.fmt( data->socket, "<table cellspacing= \"0\">\n" );
    if( FileOp.exist( currentTrace ) ) {
      long size = FileOp.fileSize( currentTrace );
      char* str = allocMem( size + 1 );
      char* p = str;
      char* plf = str;
      int i = 0;
      iOFile f = FileOp.inst( currentTrace, OPEN_READONLY );

      if( f != NULL ) {
        FileOp.read( f, str, size );
        FileOp.base.del( f );
        plf = StrOp.findc( p, '\n' );

        while( plf != NULL ) {
          char* bgcolor = i%2==0 ? "bgcolor=\"#DDFFDD\"":"";
          if( StrOp.len( p ) > 25 ) {
            if( p[25] == 'E' )
              bgcolor = "bgcolor=\"#FFCCCC\"";
            else if( p[25] == 'W' )
              bgcolor = "bgcolor=\"#CCCCFF\"";
          }
          SocketOp.fmt( data->socket, "<tr %s><td><pre width=\"132\" style=\"display: inline\">", bgcolor );
          i++;
          *plf = '\0';
          plf++;
          if( StrOp.findc( p, '<' ) != NULL ) {
            int i = 0;
            for( i = 0; i < StrOp.len( p ); i++ ) {
              if( p[i] == '<' )
                SocketOp.fmt( data->socket, "&lt;" );
              else if( p[i] == '>' )
                SocketOp.fmt( data->socket, "&gt;" );
              else
                SocketOp.fmt( data->socket, "%c", p[i] );
            }
          }
          else
            SocketOp.fmt( data->socket, "%s\n", p );
          p = plf;
          plf = StrOp.findc( p, '\n' );
          SocketOp.fmt( data->socket, "</pre></td></tr>\n" );
        };
        StrOp.free( str );
      }
    }

    SocketOp.fmt( data->socket, "</table>\n" );

    __footer( data->socket );

  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return;
  }
}


static void __formTrace( const char* str ) {
    Boolean debug = StrOp.find( str, "debug=true" ) ? True:False;
    Boolean dbyte = StrOp.find( str, "byte=true" ) ? True:False;
    int tlevel = TraceOp.getLevel( NULL ) & (0xFFFF ^ TRCLEVEL_USER2);
    if( debug )
      tlevel |= TRCLEVEL_USER2;
    TraceOp.setLevel( NULL, tlevel );

    tlevel = TraceOp.getLevel( NULL ) & (0xFFFF ^ TRCLEVEL_BYTE);
    if( dbyte )
      tlevel |= TRCLEVEL_BYTE;
    TraceOp.setLevel( NULL, tlevel );
}


/** ------------------------------------------------------------
  * __getFavicon()
  * Writes the internal si_gif[] array to the client.
  *
  * @param inst     HClient instance.
  * @return
  */
static void __getFavicon( iOHClient inst ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__getFavicon( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOHClientData data = Data(inst);
    SocketOp.write( data->socket, (char*)rocrail_gif, sizeof( rocrail_gif ) );
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return;
  }
}


/** ------------------------------------------------------------
  * __getFile()
  *
  * @param inst     HClient instance.
  * @param str      The GET string.
  * @return
  */
static void __getFile( iOHClient inst, const char* getstr ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__getFile( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOHClientData data = Data(inst);
    char* htmlfile = StrOp.dup( StrOp.find( getstr, "/" ) + 1 ) ;
    char* p = StrOp.find( htmlfile, "HTTP" );

    if( p != NULL ) {
      p--;
      *p = '\0';
      if( !FileOp.isAbsolute( htmlfile ) ) {
        long size = FileOp.fileSize( htmlfile );
        char* str = allocMem( size + 1 );
        iOFile f = FileOp.inst( htmlfile, OPEN_READONLY );
        if( f != NULL ) {
          FileOp.read( f, str, size );
          FileOp.base.del( f );
          SocketOp.write( data->socket, str, size );
        }
        else {
          __header( data->socket, data->refresh );
          SocketOp.fmt( data->socket, "<big>Sorry, but file \"%s\" does not exist on this server!</big><br>\n", htmlfile );
          __footer( data->socket );
        }
        freeMem( str );
      }
      else {
        __header( data->socket, data->refresh );
        SocketOp.fmt( data->socket, "<big>Sorry, no access to file \"%s\" on this server.</big><br>\n", htmlfile );
        __footer( data->socket );
      }

    }

  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return;
  }
}


static void __getShutdown( iOHClient inst ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__getShutdown( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOHClientData data = Data(inst);
    const char* consolemode = AppOp.isConsoleMode() ? "disabled":"";
    __header( data->socket, data->refresh );

    SocketOp.fmt( data->socket, "<table border=\"1\" cellspacing=\"0\" cellpadding=\"10\"><tr><td>\n" );
    SocketOp.fmt( data->socket, "<b>Are you sure you want to shutdown Rocrail?</b>\n" );
    SocketOp.fmt( data->socket, "<br><small>(Does not work in console mode!)</small>\n" );
    SocketOp.fmt( data->socket, "</td></tr></table>\n" );

    SocketOp.fmt( data->socket, "<table>\n" );
    SocketOp.fmt( data->socket, "<tr><td><form action=\"shutdown\">\n" );
    SocketOp.fmt( data->socket, "<input type=\"hidden\" name=\"ok\" value=\"true\"><br>\n" );
    SocketOp.fmt( data->socket, "<input type=\"submit\" value=\"Shutdown\" %s><br>\n", consolemode );
    SocketOp.fmt( data->socket, "</form></td>\n" );

    SocketOp.fmt( data->socket, "<td><form action=\"shutdown\">\n" );
    SocketOp.fmt( data->socket, "<input type=\"hidden\" name=\"ok\" value=\"false\"><br>\n" );
    SocketOp.fmt( data->socket, "<input type=\"submit\" value=\"Cancel\"><br>\n" );
    SocketOp.fmt( data->socket, "</form></td></tr>\n" );

    SocketOp.fmt( data->socket, "</table><br>\n" );

    __footer( data->socket );

  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return;
  }
}


static void __getBlank( iOHClient inst ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__getBlank( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOHClientData data = Data(inst);
    __header( data->socket, data->refresh );
    SocketOp.fmt( data->socket, "<a href=\"/\">Rocrail</a><br>\n" );
    __footer( data->socket );
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return;
  }
}


static char* __getID( const char* str ) {
  char* nid = StrOp.dup( StrOp.find( str, "id=" ) + 3 );
  char* p = StrOp.findc( nid, ' ' );
  *p = '\0';
  p = nid;
  while( *p != '\0' ) {
    if( *p == '_' )
      *p = ' ';
    p++;
  };
  return nid;
}


static void __getLoc( iOHClient inst, const char* str ) {
  iOHClientData data = Data(inst);
  char* nid = __getID( str );
  {
    iOLoc loc = ModelOp.getLoc( AppOp.getModel(), nid );
    if( loc != NULL ) {
      iIHtmlInt html = (iIHtmlInt)loc;
      char* form = html->getForm( loc );
      iONode props = LocOp.base.properties( loc );
      char* xml = NodeOp.base.toString( props );
      printf( "%s\n", xml );
      StrOp.free( xml );

      __header( data->socket, data->refresh );
      SocketOp.fmt( data->socket, form );
      __footer( data->socket );
      StrOp.free( form );
    }
  }
  StrOp.free(nid);
}


static void __postLoc( iOHClient inst, const char* postid, const char* postdata ) {
  iOHClientData data = Data(inst);
  iOLoc loc = ModelOp.getLoc( AppOp.getModel(), postid );
  if( loc != NULL ) {
    iIHtmlInt html = (iIHtmlInt)loc;
    char* reply = html->postForm( loc, postdata );

    __header( data->socket, data->refresh );
    SocketOp.fmt( data->socket, reply );
    __footer( data->socket );
    StrOp.free( reply );
  }
}


typedef void(*postcall)(iOHClient,const char*,const char*);
/** Work slice. */
static Boolean _work( struct OHClient* inst ) {
  if( inst != NULL ) {
    iOHClientData data = Data(inst);
    char str[1025] = {'\0'};
    char postdata[1025] = {'\0'};
    int contlen = 0;
    Boolean readPost = False;
    char *p = postdata;
    postcall pc = NULL;
    char* postid = NULL;
    const char* tracefile = FileOp.ripPath( wTrace.getrfile( wRocRail.gettrace( AppOp.getIni() ) ) );

    SocketOp.setRcvTimeout( data->socket, 1000 );
    /* Read first HTTP header line: */
    SocketOp.readln( data->socket, str );
    TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, str );

    /* Flag if post data is comming: */
    if( StrOp.find( str, "POST" ) )
      readPost = True;

    if( StrOp.find( str, "GET" ) && StrOp.find( str, " /locs " ) )
      __getList( inst, 1 );
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /loc?" ) ) {
      __getLoc( inst, str );
    }
    else if( StrOp.find( str, "POST" ) && StrOp.find( str, " /loc?id=" ) ) {
      postid = __getID( str );
      pc = &__postLoc;
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /streets " ) )
      __getList( inst, 2 );
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /blocks " ) )
      __getList( inst, 3 );
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /fbacks " ) )
      __getList( inst, 4 );
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /switches " ) )
      __getList( inst, 5 );
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /trace?" ) )
      __formTrace( str );
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /ini " ) )
      __getIni( inst );
    else if( StrOp.find( str, tracefile ) && StrOp.find( str, ".trc" ) ) {
      char* tracefile = StrOp.dup( StrOp.find( str, " /" ) + 2 ) ;
      char* p = StrOp.find( tracefile, "HTTP" );

      if( p != NULL ) {
        p--;
        *p = '\0';
        __getTracefile( inst, tracefile );
      }
      StrOp.free( tracefile );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /tracefile" ) ) {
      /* href to xspooler.ini */
      __getTracefile( inst, TraceOp.getCurrentFilename( NULL ) );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /shutdown " ) ) {
      __getShutdown( inst );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /shutdown?ok=true" ) ) {
      __getBlank( inst );
      SocketOp.disConnect( data->socket );
      AppOp.shutdown();
      return True;
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, " /shutdown?ok=false" ) ) {
      __getHome( inst );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, " / " ) ) {
      __getHome( inst );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/rocrail_doc" ) ) {
      extern const char rocrail_doc[];
      __httpHeader( data->socket );
      SocketOp.write( data->socket, rocrail_doc, StrOp.len( rocrail_doc ) );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/rocrail.gif" ) ) {
      __getFavicon( inst );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/favicon.ico" ) ) {
      __getFavicon( inst );
    }
    else if( StrOp.find( str, "GET" ) && StrOp.find( str, "/" ) ) {
      __getFile( inst, str );
    }

    /* Reading rest of HTTP header: */
    while( SocketOp.readln( data->socket, str ) && !SocketOp.isBroken( data->socket ) ) {
      if( str[0] == '\r' || str[0] == '\n' ) {
        break;
      }
      if( StrOp.find( str, "Content-Length:" ) ) {
        char* p = StrOp.find( str, ": " ) + 2;
        contlen = atoi( p );
        TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "contlen = %d\n", contlen );
      }
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, str );
    };

    if( readPost ) {
      /* Read the post data: */
      while( SocketOp.peek( data->socket, p, 1 ) && !SocketOp.isBroken( data->socket ) ) {
        if( !SocketOp.read( data->socket, p, 1 ) )
          break;
        /*printf( "%c", *p );*/
        p++;
        *p = '\0';
      };
      TraceOp.trc( name, TRCLEVEL_USER2, __LINE__, 9999, "postdata=\"%s\"\n", postdata );
      /* Call the post data handler: */
      if( pc != NULL )
        pc( inst, postid, postdata );
    }

    StrOp.free( postid );
    SocketOp.disConnect( data->socket );
  }
  return True;
}


/** Object creator. */
static struct OHClient* _inst( iOSocket socket, const char* path, int refresh ) {
  iOHClient __HClient = allocMem( sizeof( struct OHClient ) );
  iOHClientData data = allocMem( sizeof( struct OHClientData ) );
  MemOp.basecpy( __HClient, &HClientOp, 0, sizeof( struct OHClient ), data );

  /* Initialize data->xxx members... */
  data->socket        = socket;
  data->cid           = StrOp.fmt( "%08X", __HClient );
  data->path          = path;
  data->refresh       = refresh;

  instCnt++;
  return __HClient;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/hclient.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
