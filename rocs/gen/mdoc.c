/** ------------------------------------------------------------
  * $Author: rob $
  * $Date: 2007/01/19 13:58:16 $
  * $Revision: 1.3 $
  * $Source: /home/cvs/xspooler/rocs/gen/mdoc.c,v $
  */


/** ------------------------------------------------------------
  * Example of a status message file: (MUST BE ENCODED IN UTF-8)
  * ------------------------------------------------------------ 
<?xml version="1.0" encoding="UTF-8"?>
<Messages docname="xSpooler StatusMessages">
  <Labels>
    <de sys="Systemaktion" usr="Benutzeraktion" desc="Erklärung"/>
    <en sys="System action" usr="User action" desc="Description"/>
  </Labels> 
  
  <Group from="1000" name="MainTask" to="1999">
    <en txt="MainTask Messages"/>
    <de txt="Hauptprozess Meldungen"/>
  </Group>

  <Msg id="1005" level="info">
    <en txt="Ended at %s [rc=%d]" tip="Shutdown message."/>
    <de txt="Beendet am %s [rc=%d]" tip="Ende Meldung."/>
    <param nr="1">
      <en desc="timestamp"/>
      <de desc="Zeitstempel"/>
    </param>
    <param nr="2">
      <en desc="return code"/>
      <en desc="Rückgabewert"/>
    </param>
  </Msg>

</Messages>
*/


/* ------------------------------------------------------------
 * libc interfaces.
 */
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <time.h>

/* ------------------------------------------------------------
 * platform dependent interfaces.
 */
#if defined __linux__ || defined __aix__
#include <sys/resource.h>
#endif

/* ------------------------------------------------------------
 * rocs interfaces.
 */
#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/file.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/system.h"
#include "rocs/public/str.h"
#include "rocs/public/list.h"
#include "rocs/public/map.h"
#include "rocs/public/cmdln.h"

static const char* lang = NULL;

static int __go( iONode node, const char* fileName, const char* lang );

/** ------------------------------------------------------------
  * public main()
  *
  * @param  argc Number of commanline arguments.
  * @param  argv Commanline arguments.
  * @return      Applications exit code.
  */
int main( int argc, const char* argv[] ) {
  int rc = 0;
  Boolean docAll = False;
  iOCmdLn arg = NULL;
  const char* descfile = NULL;

  /* trace object: */
  iOTrace trc = TraceOp.inst( TRCLEVEL_INFO, "mdoc", True );
  TraceOp.setAppID( trc, "m" );

  /* Enable coredump for Linux platforms: */
  #if defined __linux__
  {
    struct rlimit rl;
    getrlimit( RLIMIT_CORE, &rl );
    /* Default = 0 */
    rl.rlim_cur = 10240 * 10240;
    setrlimit( RLIMIT_CORE, &rl );
  }
  #endif

  /* Resets memory statistics: */
  MemOp.resetDump();

  /* Check commandline arguments: */
  arg = CmdLnOp.inst( argc, argv );
  
  /* default language is en: */
  lang = CmdLnOp.getStrDef( arg, "-lang", "en" );

  /* Read the xml file: */
  if( argc > 1 && FileOp.exist(argv[1]) ) {
    TraceOp.println( "Processing %s...", argv[1] );
    {
      iOFile constXml = FileOp.inst( argv[1], OPEN_READONLY );
      char* xmlStr = allocMem( FileOp.size( constXml ) + 1 );
      iODoc doc = NULL;
      iONode root = NULL;

      TraceOp.println( "Reading %s...", argv[1] );
      FileOp.read( constXml, xmlStr, FileOp.size( constXml ) );
      FileOp.close( constXml );
      FileOp.base.del( constXml );

      TraceOp.println( "Parsing %s...", argv[1] );
      DocOp.setUTF2Latin( False );
      doc = DocOp.parse( xmlStr );
      freeMem( xmlStr );
      root = DocOp.getRootNode( doc );

      /* create the output directories: */
      {
        char*       wrapper = StrOp.dup( "./messages");
        char*    wrapperdoc = StrOp.dup( "./messages/doc");

        if( !FileOp.exist( wrapper ) ) {
          TraceOp.println( "creating: \"./wrapper\"..." );
          FileOp.mkdir( wrapper );
        }
        if( !FileOp.exist( wrapperdoc ) ) {
          TraceOp.println( "creating: \"./wrapper/doc\"..." );
          FileOp.mkdir( wrapperdoc );
        }
      }

      /* create a html document from the messages file: */
      __go( root, argv[1], lang );
    }

  }
  else {
    TraceOp.println( "Usage: mgen message.xml" );
  }

  return rc;
}


/* comparators for sorting the lists: */
static int comp( obj* o1, obj* o2 ) {
  iONode node1 = (iONode)*o1;
  iONode node2 = (iONode)*o2;
  return strcmp( NodeOp.getStr( node1, "name", "" ), NodeOp.getStr( node2, "name", "" ) );
}
static int compid( obj* o1, obj* o2 ) {
  iONode node1 = (iONode)*o1;
  iONode node2 = (iONode)*o2;
  return strcmp( NodeOp.getStr( node1, "id", "" ), NodeOp.getStr( node2, "id", "" ) );
}
static int compfrom( obj* o1, obj* o2 ) {
  iONode node1 = (iONode)*o1;
  iONode node2 = (iONode)*o2;
  return strcmp( NodeOp.getStr( node1, "from", "" ), NodeOp.getStr( node2, "from", "" ) );
}
static int compNodeName( obj* o1, obj* o2 ) {
  iONode node1 = (iONode)*o1;
  iONode node2 = (iONode)*o2;
  return strcmp( NodeOp.getName( node1 ), NodeOp.getName( node2 ) );
}


/* getting the language specific texts: */
static const char* __getTxt(iONode msg, const char** tip, const char** sysaction, const char** usraction) {
  iONode langmsg = NodeOp.findNode(msg, lang);
  if( langmsg == NULL )
    return "??";
  *tip = NodeOp.getStr( langmsg, "tip", "?" );
  *sysaction = NodeOp.getStr( langmsg, "sysaction", "-" );
  *usraction = NodeOp.getStr( langmsg, "usraction", "-" );
  return NodeOp.getStr( langmsg, "txt", "?" );
}


/* create a table of all documented parameters: */
static void __listParam( iOFile f, iONode msg ) {
  iONode param = NodeOp.findNode( msg, "param" );
  if( param == NULL ) 
    return;
  FileOp.fmt( f, "<table>\n" );
  while( param != NULL ) {
    const char* desc = "";
    iONode langdesc = NodeOp.findNode( param, lang );
    if( langdesc != NULL ) {
      desc = NodeOp.getStr( langdesc, "desc", "?" );
    } 
    FileOp.fmt( f, "<tr><td>%d</td><td><i>%s</i></td></tr>\n", 
                NodeOp.getInt( param, "nr", 0 ), desc );
    param = NodeOp.findNextNode( msg, param );
  }
  FileOp.fmt( f, "</table>\n" );
}


/* List all messages fitting in the group range from/to: */
static void __processGroup( int idx, iONode grp, iOList msgList, iOFile f, int level, const char* fileName, iONode labels ) {
  int msgcnt = ListOp.size( msgList );
  int i = 0;
  const char* from = NodeOp.getStr( grp, "from", "" );
  const char* to = NodeOp.getStr( grp, "to", "" );
  const char* grpdesc = NodeOp.getStr( grp, "name", "" );
  const char* labelDescription = "Description";
  const char* labelSysAction = "Systemaction";
  const char* labelUsrAction = "Useraction";
  
  /* language specific group description: */
  iONode langmsg = NodeOp.findNode( grp, lang );
  if( langmsg != NULL ) {
    grpdesc = NodeOp.getStr( langmsg, "txt", grpdesc );
  }
  
  /* language specific labels: */
  if( labels != NULL ) {
    labelDescription = NodeOp.getStr( labels, "desc", labelDescription );
    labelSysAction = NodeOp.getStr( labels, "sys", labelSysAction );
    labelUsrAction = NodeOp.getStr( labels, "usr", labelUsrAction );
  }
  
  /* group header: */
  FileOp.fmt( f, "<h3>%d %s (%s-%s)</h3>\n", idx, grpdesc, from, to );
  
  /* list all fitting messages: */
  for( i = 0; i < msgcnt; i++ ) {
    iONode msg = (iONode)ListOp.get( msgList, i );
    Boolean listed = NodeOp.getBool( msg, "listed", False );
    const char* id = NodeOp.getStr( msg, "id", "" );
    
    if(listed)
      continue;
    
    if( StrOp.len(id) == 0 ) {
      /* warning: */
      TraceOp.println( "skipping empty id!" );
      continue;
    }
      
    /* check if the message fits: */
    if( strcmp( from, id  ) <= 0 &&  strcmp( to, id  ) >= 0 ) {
      const char* tip = "-";
      const char* sysaction = "-";
      const char* usraction = "-";
      FileOp.fmt( f, "<br><tt><b>%s %s</b></tt><br>\n", id, NodeOp.getStr( msg, "level", "info" ) );
      FileOp.fmt( f, "<table><tr><td width=\"10\"></td><td><b>%s</b></td></table>\n", __getTxt(msg, &tip, &sysaction, &usraction) );
      FileOp.fmt( f, "<table><tr><td width=\"30\"></td><td><u>%s:</u></td></tr>\n", labelDescription );
      FileOp.fmt( f, "<tr><td></td><td>%s</td></tr>\n", tip );
      FileOp.fmt( f, "<tr><td></td><td>\n" );
      __listParam( f, msg );
      FileOp.fmt( f, "</td></tr></table>\n" );
      FileOp.fmt( f, "<table><tr><td width=\"30\"></td><td><u>%s:</u></td></tr>\n", labelSysAction );
      FileOp.fmt( f, "<tr><td></td><td>%s</td></tr></table>\n", sysaction );
      FileOp.fmt( f, "<table><tr><td width=\"30\"></td><td><u>%s:</u></td></tr>\n", labelUsrAction );
      FileOp.fmt( f, "<tr><td></td><td>%s</td></tr></table>\n", usraction );
      /* mark as listed: */
      NodeOp.setBool( msg, "listed", True );
    }
  }
  
}


/* generate the html header: */
static void __header( iOFile f, const char* title ) {
  FileOp.fmt( f, "<html>\n" );
  FileOp.fmt( f, "<head>\n" );
  FileOp.fmt( f, "  <title>%s</title>\n", title );
  FileOp.fmt( f, "  <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n" );
  FileOp.fmt( f, "</head>\n" );
  FileOp.fmt( f, "<body>\n" );
  FileOp.fmt( f, "<h2>%s</h2>\n", title );
  FileOp.fmt( f, "Auto generated at: %s<br><hr><br>", StrOp.createStamp() );
}


/* generate the html footer: */
static void __footer( iOFile f ) {
  FileOp.fmt( f, "</body>\n" );
  FileOp.fmt( f, "</html>\n" );
}
    

/* starting point: */
static int __go( iONode node, const char* fileName, const char* lang ) {
  int rc = 0;
  const char* modulename = NodeOp.getStr( node, "modulename", "?" );
  const char* docname = NodeOp.getStr( node, "docname", fileName );
  iONode labels = NodeOp.findNode( node, "Labels" );
  iOFile fDoc = NULL;
  char* docFileName = StrOp.fmt( "messages/doc/%s-%s.html", docname, lang );
  StrOp.replaceAll(docFileName, ' ', '_');

  /* try to open the output file: */
  fDoc = FileOp.inst( docFileName, OPEN_WRITE );
  if( fDoc == NULL ) {
    /* the file object will write out a trace in case something is wrong */
    return -1;
  }

  /* the language specific labels: */
  if( labels != NULL ) {
    labels = NodeOp.findNode( labels, lang );
  }

  /* start the html document: */
  __header( fDoc, docname );

  /* fill the group and message lists: */
  {
    iOList grpList = ListOp.inst();
    iOList msgList = ListOp.inst();
    
    int childCnt = NodeOp.getChildCnt( node );
    int childIdx = 1;
    int i = 0;
    TraceOp.println( "Processing %d childs.", childCnt );
    for( i = 0; i < childCnt; i++ ) {
      iONode child = NodeOp.getChild( node, i );
      if( NodeOp.getType( child ) == ELEMENT_NODE ) {
        if( StrOp.equalsi( "group", NodeOp.getName(child) ) )
          ListOp.add( grpList, (obj)child );
        else if( StrOp.equalsi( "msg", NodeOp.getName(child) ) )
          ListOp.add( msgList, (obj)child );
      }
    }
    
    /* sort the lists: */
    ListOp.sort( grpList, compfrom );
    ListOp.sort( msgList, compid );
    
    for( i = 0; i < ListOp.size( grpList ); i++ ) {
      iONode child = (iONode)ListOp.get( grpList, i );
      if( NodeOp.getType( child ) == ELEMENT_NODE ) {
        __processGroup( childIdx, child, msgList, fDoc, 0, fileName, labels );
        childIdx++;
      }
    }
  }

  /* close the html document: */
  __footer( fDoc );

  /* clean up: */
  FileOp.base.del( fDoc );
  
  return rc;
}


