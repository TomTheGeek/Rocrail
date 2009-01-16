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

#include <stdio.h>
#include <stdlib.h>

#include "rocs/public/rocs.h"
#include "rocs/public/objbase.h"
#include "rocs/public/trace.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/thread.h"
#include "rocs/public/file.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/cmdln.h"
#include "rocs/public/stats.h"
#include "rocs/public/system.h"

#include "rocrail/impl/app_impl.h"
#include "rocrail/public/clntcon.h"
#include "rocrail/public/control.h"
#include "rocrail/public/model.h"
#include "rocrail/public/http.h"

#include "rocrail/wrapper/public/Cmdline.h"
#include "rocrail/wrapper/public/ConCmd.h"
#include "rocrail/wrapper/public/Global.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Tcp.h"
#include "rocrail/wrapper/public/Trace.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Exception.h"
#include "rocrail/wrapper/public/SvnLog.h"
#include "rocrail/wrapper/public/SvnLogEntry.h"

#include "common/version.h"


extern const char svnLog[];

static iOApp __appinst = NULL;

static const char* backtrace[10] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
static iOMutex __exceptionMutex = NULL;
static void __exception( int level, char* msg ) {
  if( __exceptionMutex == NULL ) {
    /* not thread safe */
    return;
  }
  else if( MutexOp.wait( __exceptionMutex ) ) {
    /* Broadcast to clients. Node6 */
    iONode e = NodeOp.inst( wException.name(), NULL, ELEMENT_NODE );
    int i = 0;
    int len = StrOp.len( msg );
    for( i = 0; i < len; i++ ) {
      if( msg[i] == '\"' )
        msg[i] = ' ';
    }
    wException.settext( e, msg );
    wException.setlevel( e, level );
    ClntConOp.broadcastEvent( AppOp.getClntCon(), e );

    StrOp.free((char*)backtrace[0]);
    for( i = 0; i < 9; i++ ) {
      backtrace[i] = backtrace[i+1];
    }
    backtrace[9] = StrOp.dup( msg );

    /* Release the mux. */
    MutexOp.post( __exceptionMutex );
  }
}

static const char** _getBackTrace(void) {
  return backtrace;
}



/*
 ***** OBase functions.
 */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static const char* __name(void) {
  return name;
}
static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}
static void __deserialize(void* inst, unsigned char* a) {
}
static char* __toString(void* inst) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    return NodeOp.toEscString( data->ini );
  }
  else
    return NULL;
}
static void __del(void* inst) {
}
static void* __properties(void* inst) {
  return NULL;
}
static int __count(void) {
  return 1;
}
static struct OBase* __clone( void* inst ) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static const char* _getIniFile(void) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "_getIniFile()" );
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    return data->szIniFile;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return NULL;
  }
}


static const char* _getLibPath( void ) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    return data->szLibPath;
  }
  return NULL;
}

static int _getsvn( void ) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    return data->svn;
  }
  return 0;
}


static iONode _getIni( void ) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    return data->ini;
  }
  return NULL;
}


static Boolean _isCreateModplan( void ) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    if( data->createmodplan )
      return True;
    else
      return wRocRail.iscreatemodplan( data->ini);
  }
  return False;
}


static iONode _getNewIni( void ) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    if( data->newini != NULL )
      return data->newini;
    else
      return data->ini;
  }
  return NULL;
}


static void _setIni( iONode ini ) {
  if( __appinst != NULL ) {
    iOAppData data  = Data(__appinst);
    iONode    trace = wRocRail.gettrace( ini );
    iONode curtrace = wRocRail.gettrace( data->ini );

    if( trace != NULL && curtrace != NULL) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "trace levels changes are activated, other setting at restart..." );
      wTrace.setautomatic( curtrace, wTrace.isautomatic( trace ) );
      wTrace.setbyte( curtrace, wTrace.isbyte( trace ) );
      wTrace.setdebug( curtrace, wTrace.isdebug( trace ) );
    }

    data->newini = (iONode)NodeOp.base.clone( ini );

    AppOp.saveIni();
  }
}

static iONode _getIniNode( const char* name ) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    if( data->ini != NULL ) {
      return NodeOp.findNode( data->ini, name );
    }
    else
      return NULL;
  }
  return NULL;
}

static iOModel _getModel( void ) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    return data->model;
  }
  return NULL;
}

static iOControl _getControl( void ) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    return data->control;
  }
  return NULL;
}

static iOClntCon _getClntCon( void ) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    return data->clntCon;
  }
  return NULL;
}

static int __logo( void ) {
  int svn = 0;
  /* Logo. */
  TraceOp.println( "--------------------------------------------------" );
  TraceOp.println( "                                            lll   " );
  TraceOp.println( "                                        iii lll   " );
  TraceOp.println( "                                            lll   " );
  TraceOp.println( " rrrrrr   ooooo   ccccc rrrrrr   aaaaaa iii lll   " );
  TraceOp.println( " rrr rrr ooo ooo ccc    rrr rrr aaa aaa iii lll   " );
  TraceOp.println( " rrr     ooo ooo ccc    rrr     aaa aaa iii lll   " );
  TraceOp.println( " rrr      ooooo   ccccc rrr      aaaaaa iii  lll  " );
  TraceOp.println( "--------------------------------------------------" );
  TraceOp.println( " Copyright (c) 2002-2008 Rob Versluis"              );
  TraceOp.println( "   r.j.versluis@rocrail.net"                        );
  TraceOp.println( "   http://www.rocrail.net"                          );
  TraceOp.println( " License: GNU GPL 2"                                );
  TraceOp.println( "   http://www.gnu.org/licenses/gpl.txt"             );
  TraceOp.println( "--------------------------------------------------" );
  TraceOp.println( " %s %d.%d.%d \"%s\" %s",
                   wGlobal.productname,
                   wGlobal.vmajor,
                   wGlobal.vminor,
                   wGlobal.patch,
                   wGlobal.releasename, wGlobal.releasesuffix );
  TraceOp.println( " build %s %s",
                   wGlobal.buildDate,
                   wGlobal.buildTime );

  if( bzr > 0 ){
    TraceOp.println( " bzr %d", bzr );
  }
  else {
    iODoc doc = DocOp.parse(svnLog);
    if( doc != NULL ) {
      iONode log = DocOp.getRootNode(doc);
      DocOp.base.del(doc);
      if( log != NULL ) {
        svn = wSvnLogEntry.getrevision( wSvnLog.getlogentry(log) );
        TraceOp.println( " svn %d", svn );
        NodeOp.base.del(log);
      }
    }
  }
  TraceOp.println( " %s", SystemOp.getBuild() );
  TraceOp.println( " processid = %d", SystemOp.getpid() );
  TraceOp.println( "       mac = %s", SocketOp.getMAC( NULL ) );
  TraceOp.println( "--------------------------------------------------" );
  /*TraceOp.printHeader();*/
  return svn;
}

static __help( void ) {
  /* Help. */
  TraceOp.println( "----------------------------------------------------------------------"  );
  TraceOp.println( "Rocrail commandline options:"  );
  TraceOp.println( "-------------------------+--------------------------------------------"  );
  TraceOp.println( "--help or -?             | Displays this help and quit." );
  TraceOp.println( "-version                 | Displays version info and quit." );
  TraceOp.println( "-doc                     | Writes documentation in %s.html.", wGlobal.productname );
  TraceOp.println( "-------------------------+--------------------------------------------"  );
  TraceOp.println( "-debug                   | Switch on debug tracelevel." );
  TraceOp.println( "-byte                    | Switch on byte tracelevel." );
  TraceOp.println( "-parse                   | Switch on xml parse tracelevel." );
  TraceOp.println( "-monitor                 | Switch on controller monitor tracelevel." );
  TraceOp.println( "-info                    | Switch on info tracelevel." );
  TraceOp.println( "-------------------------+--------------------------------------------"  );
  TraceOp.println( "-console                 | Read console input." );
  TraceOp.println( "-nocom                   | Switch off communication." );
  TraceOp.println( "-w [workdir]             | Change the programs working directory." );
  TraceOp.println( "-l [libdir]              | Library directory." );
  TraceOp.println( "-p [portnr]              | Service port for clients. [%d]", wTcp.getport(NULL) );
  TraceOp.println( "-i [inifile]             | Ini file. [%s].", wRocRail.getfile(NULL) );
  TraceOp.println( "-t [tracefile]           | Ini file. [%s].", wTrace.getrfile(NULL) );
  TraceOp.println( "-x [planfile]            | Ini file. [%s].", wRocRail.getplanfile(NULL) );
  TraceOp.println( "-f                       | Init field." );
  TraceOp.println( "-------------------------+--------------------------------------------"  );
  TraceOp.println( "-installservice          | Install Rocrail as Windows service." );
  TraceOp.println( "-deleteservice           | Uninstall Rocrail as Windows service." );
  TraceOp.println( "-------------------------+--------------------------------------------"  );
}

/** ------------------------------------------------------------
  * private __conhelp()
  * See wrapper.xml: ConCmd.
  */
static void __conhelp() {
    TraceOp.println( "-C-o-n-s-o-l-e---C-o-m-m-a-n-d-s-----------" );
    TraceOp.println( " ? - Help" );
    TraceOp.println( " q - Shutdown" );
    TraceOp.println( " p - Power ON" );
    TraceOp.println( " y - Power OFF" );
    TraceOp.println( " x - Read all slots" );
    TraceOp.println( " t - List all active threads" );
    TraceOp.println( " z - Analyse track plan" );

    TraceOp.println( " i - Initfield" );

    TraceOp.println( " m - Shows memory(object) use" );
    TraceOp.println( " d - Toggle debug tracelevel" );
    TraceOp.println( " b - Toggle byte tracelevel" );
    TraceOp.println( " w - Toggle wrapper tracelevel" );
    TraceOp.println( " a - Toggle automat tracelevel" );
    TraceOp.println( " h - Toggle http tracelevel" );
    TraceOp.println( " o - Toggle monitor tracelevel" );
}


static void __syscmd( const char* command ) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE);
    wSysCmd.setcmd( cmd, command );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SysCommand: %s", command );
    ControlOp.cmd( data->control, (iONode)NodeOp.base.clone(cmd), NULL );
    ClntConOp.broadcastEvent( AppOp.getClntCon(), cmd );
  }
}


static __checkConsole( iOAppData data ) {
  /* Check for command. */
  int c = getchar();

  if( c == wConCmd.debug ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Toggle debug tracelevel." );
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) ^ TRCLEVEL_DEBUG );
  }
  else if( c == wConCmd.byte ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Toggle byte tracelevel." );
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) ^ TRCLEVEL_BYTE );
  }
  else if( c == wConCmd.automat ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Toggle auto tracelevel." );
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) ^ TRCLEVEL_USER1 );
  }
  else if( c == wConCmd.http ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Toggle http tracelevel." );
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) ^ TRCLEVEL_USER2 );
  }
  else if( c == wConCmd.monitor ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Toggle monitor tracelevel." );
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) ^ TRCLEVEL_MONITOR );
  }
  else if( c == wConCmd.quit ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutdown requested." );
    data->consoleMode = False;
    AppOp.shutdown();
  }
  else if( c == wConCmd.initfield ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Initfield requested." );
    ModelOp.initField( data->model );
  }
  else if( c == wConCmd.threads ) {
    AppOp.listThreads();
  }
  else if( c == wConCmd.poweron ) {
    AppOp.go();
  }
  else if( c == wConCmd.poweroff ) {
    AppOp.stop();
  }
  else if( c == wConCmd.slots ) {
    __syscmd( wSysCmd.slots );
  }
  else if( c == wConCmd.config ) {
    __syscmd( wSysCmd.config );
  }
  else if( c == wConCmd.analyse ) {
    ModelOp.analyse( data->model );
  }
  else if( c == wConCmd.memory )
    rocsStatistics( True );
  else if( c == wConCmd.help )
    __conhelp();
}



static Boolean bShutdown = False;

static int _Main( iOApp inst, int argc, char** argv ) {
  iOAppData data = Data(inst);
  iOTrace trc = NULL;

  /* check commandline arguments */
  iOCmdLn     arg     = CmdLnOp.inst( argc, (const char**)argv );
  tracelevel  debug   = CmdLnOp.hasKey( arg, wCmdline.debug  ) ? TRCLEVEL_DEBUG:0;
  tracelevel  dump    = CmdLnOp.hasKey( arg, wCmdline.byte   ) ? TRCLEVEL_BYTE:0;
  tracelevel  parse   = CmdLnOp.hasKey( arg, wCmdline.parse  ) ? TRCLEVEL_PARSE:0;
  tracelevel  monitor = CmdLnOp.hasKey( arg, wCmdline.monitor) ? TRCLEVEL_MONITOR:0;
  tracelevel  info    = CmdLnOp.hasKey( arg, wCmdline.info   ) ? TRCLEVEL_INFO:0;

  const char* wd      = CmdLnOp.getStr( arg, wCmdline.workdir );
  const char* tf      = CmdLnOp.getStr( arg, wCmdline.trcfile );
  const char* pf      = CmdLnOp.getStr( arg, wCmdline.planfile );
  const char* port    = CmdLnOp.getStr( arg, wCmdline.port );
  const char* nf      = CmdLnOp.getStr( arg, wCmdline.inifile );

  Boolean initfield   = CmdLnOp.hasKey( arg, wCmdline.initfield );
  Boolean      help   = CmdLnOp.hasKey( arg, wCmdline.help ) | CmdLnOp.hasKey( arg, "-?" );
  Boolean     nocom   = CmdLnOp.hasKey( arg, wCmdline.nocom );
  Boolean   console   = CmdLnOp.hasKey( arg, wCmdline.console );
  Boolean       doc   = CmdLnOp.hasKey( arg, wCmdline.doc );
  Boolean   version   = CmdLnOp.hasKey( arg, wCmdline.version );
  Boolean   service   = CmdLnOp.hasKey( arg, wCmdline.service );
  Boolean       lcd   = CmdLnOp.hasKey( arg, wCmdline.lcd );


  data->stress        = CmdLnOp.hasKey( arg, wCmdline.stress );
  data->createmodplan = CmdLnOp.hasKey( arg, wCmdline.modplan );
  data->szLibPath     = CmdLnOp.getStr( arg, wCmdline.libpath );

  /* change the programs working directory */
  if( wd != NULL ) {
    FileOp.cd( wd );
  }

  trc = TraceOp.inst( debug | dump | monitor | parse | TRCLEVEL_INFO | TRCLEVEL_WARNING | TRCLEVEL_CALC, tf, True );
  TraceOp.setAppID( trc, "r" );

  data->consoleMode = console;

  if( service ) {
    /* block reading console */
    console = False;
    nocom   = False;
  }
  else {
    if( help ) {
      data->svn = __logo();
      __help();
      return 0;
    }
    else if( doc ) {
      /* Write the embeded documentation into local HTML files: */
      extern const char rocrail_doc[]; /* xspooler.ini doc */

      char* fname = StrOp.fmt( "%s.html", wGlobal.productname );
      iOFile f = FileOp.inst( fname, OPEN_WRITE );
      __logo();
      if( f != NULL ) {
        Boolean ok = FileOp.write( f, rocrail_doc, StrOp.len( rocrail_doc ) );
        FileOp.base.del( f );
        TraceOp.println( "%s %s", fname, ok?"successfully written.":"not written."  );
      }
      StrOp.free( fname );
      return 0;
    }
    else if( version ) {
      __logo();
      return 0;
    }
  }


  /* Read the Inifile: */
  {
    char* iniXml = NULL;
    iODoc iniDoc = NULL;
    iOFile iniFile = FileOp.inst( nf?nf:wRocRail.getfile(NULL), True );
    data->szIniFile = nf?nf:wRocRail.getfile(NULL);
    if( iniFile != NULL ) {
      iniXml = allocMem( FileOp.size( iniFile ) + 1 );
      FileOp.read( iniFile, iniXml, FileOp.size( iniFile ) );
      if( StrOp.len( iniXml ) == 0 )
        iniXml = StrOp.fmt( "<%s/>", wRocRail.name());
      FileOp.close( iniFile );
    }
    else {
      iniXml = StrOp.fmt( "<%s/>", wRocRail.name());
    }

    /* Parse the Inifile: */
    iniDoc = DocOp.parse( iniXml );
    if( iniDoc != NULL )
      data->ini = DocOp.getRootNode( iniDoc );
    else {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Invalid ini file! [%s]", nf?nf:wRocRail.getfile(NULL) );
      return -1;
    }
  }

  if( wRocRail.isrunasroot( data->ini ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Try to run rocrail as root..." );
      if( !SystemOp.setAdmin() ) {
         TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Only user 'root' can start rocrail! Abort!" );
         return -1;
      }
  }

  if( data->szLibPath == NULL ) {
    data->szLibPath = wRocRail.getlibpath( data->ini );
  }

  if( info != TRCLEVEL_INFO ){ /* Check info tracelevel. */
    int         level = TraceOp.getLevel( trc );
    Boolean infoParam = wTrace.isinfo( wRocRail.gettrace( data->ini ) );
    Boolean   infoCmd = (info & TRCLEVEL_INFO) ? True:False;
    if( infoCmd != infoParam ) {
      level &= 0xffff ^ TRCLEVEL_INFO;
      level |= infoParam ? TRCLEVEL_INFO:0;
      TraceOp.setLevel( trc, level );
    }
  }

  /* Tracefile and listener */
  if( wRocRail.gettrace( data->ini ) == NULL ) {
    iONode trace = NodeOp.inst( wTrace.name(), data->ini, ELEMENT_NODE );
    NodeOp.addChild( data->ini, trace );
  }

  if( wTrace.isdebug( wRocRail.gettrace( data->ini ) ) || debug )
    TraceOp.setLevel( trc, TraceOp.getLevel( trc ) | TRCLEVEL_DEBUG );
  if( wTrace.isautomatic( wRocRail.gettrace( data->ini ) ) )
    TraceOp.setLevel( trc, TraceOp.getLevel( trc ) | TRCLEVEL_USER1 );
  if( wTrace.ismonitor( wRocRail.gettrace( data->ini ) ) || monitor )
    TraceOp.setLevel( trc, TraceOp.getLevel( trc ) | TRCLEVEL_MONITOR );
  if( wTrace.isbyte( wRocRail.gettrace( data->ini ) ) || dump )
    TraceOp.setLevel( trc, TraceOp.getLevel( trc ) | TRCLEVEL_BYTE );
  if( wTrace.isparse( wRocRail.gettrace( data->ini ) ) || parse )
    TraceOp.setLevel( trc, TraceOp.getLevel( trc ) | TRCLEVEL_PARSE );
  if( wTrace.iscalc( wRocRail.gettrace( data->ini ) ) )
    TraceOp.setLevel( trc, TraceOp.getLevel( trc ) | TRCLEVEL_CALC );


  /* Tracefile and listener */
  {
    iONode tini = wRocRail.gettrace( data->ini );
    char*    tracefilename = NULL;
    const char*   protpath = wTrace.getprotpath( tini );
    Boolean        unique  = wTrace.isunique( tini );
    const char*        tf2 = wTrace.getrfile( tini );
    int               size = wTrace.getsize( tini );
    int                 nr = wTrace.getnr( tini );
    Boolean  exceptionfile = wTrace.isexceptionfile( tini );
    const char*     invoke = wTrace.getinvoke( tini );
    Boolean    invokeasync = wTrace.isinvokeasync( tini );
    int           dumpsize = wTrace.getdumpsize( tini );

    TraceOp.setFileSize( trc, size );
    TraceOp.setNrFiles( trc, nr );
    TraceOp.setExceptionFile( trc, exceptionfile );
    TraceOp.setInvoke( trc, invoke, invokeasync );
    TraceOp.setDumpsize( trc, dumpsize );

    if( tf == NULL )
      tf = tf2;
    else {
      char* p = FileOp.getPath( tf );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "ProtPath set to [%s]. (tf=\"%s\")", p, tf );
      wTrace.setprotpath( tini, p );
      StrOp.free( p );
      protpath = wTrace.getprotpath( tini );
      wTrace.setrfile( tini, FileOp.ripPath( tf ) );
    }

    /* Check protpath. */
    if( protpath != NULL ) {
      if( !FileOp.access( protpath ) ) {
        if( FileOp.mkdir( protpath ) )
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "ProtPath [%s] created.", protpath );
        else {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 1002,
              "Protocol path [%s] invalid.(Using current folder.", protpath );
          protpath = NULL;
        }
      }
      else
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "ProtPath [%s] OK.", protpath );
    }

    if( protpath != NULL && !FileOp.isAbsolute( tf ) ) {
      char* stamp = StrOp.createStamp();

      if( !FileOp.isAbsolute( protpath ) ) {
        char* wd = FileOp.pwd();
        tracefilename = StrOp.fmt( "%s%c%s%c%s%s",
                                   wd,
                                   SystemOp.getFileSeparator(),
                                   protpath,
                                   SystemOp.getFileSeparator(),
                                   tf,
                                   unique ? stamp:"" );
        StrOp.free( wd );
      }
      else {
        tracefilename = StrOp.fmt( "%s%c%s%s",
                                   protpath,
                                   SystemOp.getFileSeparator(),
                                   tf,
                                   unique ? stamp:"" );
      }

      StrOp.free( stamp );
    }
    else {
      char* stamp = StrOp.createStamp();
      tracefilename = StrOp.fmt( "%s%s", tf, unique ? stamp:"" );
      StrOp.free( stamp );
    }

    TraceOp.setFilename( trc, tracefilename );
    TraceOp.setExceptionListener( trc, __exception, False );

    StrOp.free( tracefilename );
  }



  /* Logo. */
  data->svn = __logo();

  /* planDoc */
  pf = pf?pf:wRocRail.getplanfile(data->ini);
  data->model = ModelOp.inst( pf );
  ModelOp.init( data->model );

  MemOp.setDebug( False );


  /* Control */
  data->control = ControlOp.inst( nocom );
  if( lcd ) {
    data->display = DisplayOp.inst( data->ini );
  }

  /* Client connection */
  {
    iONode tcp = wRocRail.gettcp(data->ini);
    int iPort = 0;
    if( tcp == NULL ) {
      tcp = NodeOp.inst( wTcp.name(), data->ini, ELEMENT_NODE );
      NodeOp.addChild( data->ini, tcp );
    }

    iPort = port?atoi(port):wTcp.getport(tcp);
    data->clntCon = ClntConOp.inst( tcp, iPort, ControlOp.getCallback( data->control), (obj)data->control );
  }

  /* Http (Optional)*/
  {
    iONode http = wRocRail.gethttp( data->ini );
    if( http != NULL )
      data->http = HttpOp.inst( http );
  }

  if( initfield )
    ModelOp.initField( data->model );

  /* update the feedback arrays */
  ModelOp.updateFB( data->model );


  /* Memory watcher */
  while( !bShutdown ) {
    static int cnt1 = 0;
    int cnt2 = MemOp.getAllocCount();
    if( cnt1 != cnt2 ) {
      cnt1 = cnt2;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MemOp.getAllocCount() = %u", cnt2 );
    }
    ThreadOp.sleep( 1000 );

    /* Check for command. */
    if( data->consoleMode )
      __checkConsole( data );
  };

  return 0;
}


/** ------------------------------------------------------------
  * AppOp.listThreads()
  * Traced out all activated rocs::OThread objects.
  */
static void _listThreads(void) {
  iOList thList = ThreadOp.getAll();
  int cnt = ListOp.size( thList );
  int i = 0;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%d active threads.", cnt );

  for( i = 0; i < cnt; i++ ) {
    iOThread th = (iOThread)ListOp.get( thList, i );
    const char* tname = ThreadOp.getName( th );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Thread[%d] [%s]", i, tname );
  }
  /* Cleanup. */
  thList->base.del( thList );
}


static void _saveIni( void ) {
  if( __appinst != NULL ) {

    iOAppData data = Data(__appinst);

    /* backup existing ini: */

    if( FileOp.exist(data->szIniFile) ) {
      char* backupfile = StrOp.fmt( "%s.bak", data->szIniFile );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "backing up %s to %s...",
          data->szIniFile, backupfile );
      FileOp.remove( backupfile );
      FileOp.rename( data->szIniFile, backupfile );
      StrOp.free( backupfile );
    }

    /* Write the Inifile: */
    {
      iOFile iniFile = FileOp.inst( data->szIniFile, OPEN_WRITE );

      if( iniFile != NULL ) {
        char* iniStr = NULL;
        if(data->newini != NULL)
          iniStr = NodeOp.base.toString( data->newini );
        else
          iniStr = NodeOp.base.toString( data->ini );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "saving ini %s%s...", data->szIniFile, data->newini==NULL?"":"(new)" );
        FileOp.write( iniFile, iniStr, StrOp.len( iniStr ) );
        FileOp.close( iniFile );
        StrOp.free(iniStr);
      }
    }
  }
}


static Boolean _shutdown( void ) {
  if( __appinst != NULL ) {

    iOAppData data = Data(__appinst);

    /* Inform Model. */
    if( data->model != NULL )
      ModelOp.save(data->model);

    if( data->consoleMode ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "For shutdown in console mode use q<enter>" );
      return False;
    }

    if( bShutdown )
      return True;

    /* signal all threads to stop */
    ThreadOp.requestQuitAll();

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down..." );
    {
      iOList thList = ThreadOp.getAll();
      int cnt = ListOp.size( thList );
      int i = 0;
      for( i = 0; i < cnt; i++ ) {
        iOThread th = (iOThread)ListOp.get( thList, i );
        const char* thname = ThreadOp.getName( th );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Thread[%d] <%s> still active.", i, thname );
      };
    }

    AppOp.saveIni();

    if( data->http != NULL )
      HttpOp.shutdown( data->http );

    ControlOp.halt( data->control );

    /* signal main loop */
    bShutdown = True;

    /*
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Exit." );
    exit(0);
    */
    return True;
  }
  return False;
}

static void _stop( void ) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE);
    wSysCmd.setcmd( cmd, wSysCmd.stop );
    wSysCmd.setinformall( cmd, True );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "POWER OFF" );
    if( data->control != NULL )
      ControlOp.cmd( data->control, (iONode)NodeOp.base.clone(cmd), NULL );
    if( AppOp.getClntCon() != NULL )
      ClntConOp.broadcastEvent( AppOp.getClntCon(), cmd );
  }
}


static void _go( void ) {
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE);
    wSysCmd.setcmd( cmd, wSysCmd.go );
    wSysCmd.setinformall( cmd, True );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "POWER ON" );
    ControlOp.cmd( data->control, cmd, NULL );
  }
}


static long _getStartTime( void ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "_getStartTime()" );
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    return data->appstartTime;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return 0;
  }
}

static Boolean _isConsoleMode( void ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "_isConsoleMode()" );
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    return data->consoleMode;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return False;
  }
}

static Boolean _isStress( void ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "_isStress()" );
  if( __appinst != NULL ) {
    iOAppData data = Data(__appinst);
    return data->stress;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return False;
  }
}

static iOApp _inst(void) {
  if( __appinst == NULL ) {
    iOApp     app  = allocMem( sizeof( struct OApp ) );
    iOAppData data = allocMem( sizeof( struct OAppData ) );

    /* OBase operations */
    MemOp.basecpy( app, &AppOp, 0, sizeof( struct OApp ), data );
    __appinst = app;

    __exceptionMutex = MutexOp.inst( NULL, True );

    data->appstartTime = time(NULL);
    data->szLibPath = NULL;

    SystemOp.inst();
  }

  return __appinst;
}

static iOApp _getApp( void ) {
  return __appinst;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/app.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
