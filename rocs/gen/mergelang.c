/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
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
#include "rocs/public/thread.h"
#include "rocs/public/cmdln.h"

static void __mergelang( iONode xmlIn, iONode xmlOut, const char* newlang );

/** ------------------------------------------------------------
  * public main()
  * usage: addlang -i <inputfile> -o <outputfile> -l <lang>
  *
  * @param  argc Number of commanline arguments.
  * @param  argv Commanline arguments.
  * @return      Applications exit code.
  */
int main( int argc, const char* argv[] ) {
  int rc = 0;
  iOCmdLn arg = NULL;
  const char* infile = NULL;
  const char* outfile = NULL;
  const char* newlang = NULL;

  iOTrace trc = TraceOp.inst( TRCLEVEL_INFO, "mergelang", True );
  TraceOp.setAppID( trc, "m" );

  /* Enable coredump for Linux platforms. */
  #if defined __linux__
  {
    struct rlimit rl;
    getrlimit( RLIMIT_CORE, &rl );
    /* Default = 0 */
    rl.rlim_cur = 10240 * 10240;
    setrlimit( RLIMIT_CORE, &rl );
  }
  #endif

  /* Resets memory statistics. */
  MemOp.resetDump();

  /* Check commandline arguments. */
  arg = CmdLnOp.inst( argc, argv );
  
  newlang = CmdLnOp.getStrDef( arg, "-l", NULL );
  infile = CmdLnOp.getStrDef( arg, "-i", NULL );
  outfile = CmdLnOp.getStrDef( arg, "-o", NULL );
  
  /* Read const.xml */
  if( infile != NULL && outfile != NULL && FileOp.exist(infile) && newlang != NULL ) {
    TraceOp.println( "Merging %s...", infile );
    {
      iOFile fi = FileOp.inst( infile, OPEN_READONLY );
      iOFile fo = FileOp.inst( outfile, OPEN_READONLY );
      char* xmlIn = allocMem( FileOp.size( fi ) + 1 );
      char* xmlOut = allocMem( FileOp.size( fo ) + 1 );
      iODoc idoc = NULL;
      iONode iroot = NULL;
      iODoc odoc = NULL;
      iONode oroot = NULL;
      long serializeSize = 0;

      TraceOp.println( "Reading %s...", infile );
      FileOp.read( fi, xmlIn, FileOp.size( fi ) );
      FileOp.close( fi );
      FileOp.base.del( fi );

      TraceOp.println( "Reading %s...", outfile );
      FileOp.read( fo, xmlOut, FileOp.size( fo ) );
      FileOp.close( fo );
      FileOp.base.del( fo );
      
      DocOp.setUTF2Latin( False );

      TraceOp.println( "Parsing %s...", infile );
      idoc = DocOp.parse( xmlIn );
      freeMem( xmlIn );
      iroot = DocOp.getRootNode( idoc );

      TraceOp.println( "Parsing %s...", outfile );
      odoc = DocOp.parse( xmlOut );
      freeMem( xmlOut );
      oroot = DocOp.getRootNode( odoc );

      __mergelang( iroot, oroot, newlang );

      TraceOp.println( "Writing %s...", outfile );
      fo = FileOp.inst( outfile, OPEN_WRITE );
      xmlOut = NodeOp.base.serialize( oroot, &serializeSize );
      FileOp.write( fo, xmlOut, StrOp.len( xmlOut ) );
      FileOp.close( fo );
      FileOp.base.del( fo );
      
    }

  }
  else {
    TraceOp.println( "Usage: mergelang -i <inputfile> -o <outputfile> -l <lang>" );
  }

  return rc;
}



/* The input file should be formated like this:
<Messages>
  <Msg id="thanks">
    <de txt="Besonderer Dank an:"/>
    <en txt="Special thanks to:"/>
    <nl txt="Met speciale dank aan:"/>
    <sv txt="Special thanks to:"/>
    <fr txt="Remerciement:"/>
  </Msg>
</Messages>
*/
static void __addlang( iONode xmlOut, const char* iid, const char* lang, const char* txt, const char* tip ) {
  iONode msg = NodeOp.findNode( xmlOut, "Msg" );
  
  while( msg != NULL ) {
    const char* oid = NodeOp.getStr( msg, "id", NULL );
    
    if( iid != NULL && oid != NULL && StrOp.equals( iid, oid ) ) {
      Boolean addnode = False;
      iONode node = NodeOp.findNode(msg, lang);
      if( node == NULL ) {
        node = NodeOp.inst( lang, xmlOut, ELEMENT_NODE );
        addnode = True;
      }
      else
        addnode = False;
      
      if( txt != NULL ) NodeOp.setStr( node, "txt", txt );
      if( tip != NULL ) NodeOp.setStr( node, "tip", tip );
      if( addnode )
        NodeOp.addChild( msg, node );
      break;
    }
    
    msg = NodeOp.findNextNode( xmlOut, msg );
  }; 
}

static void __mergelang( iONode xmlIn, iONode xmlOut, const char* newlang ) {
  int cnt = 0;
  iONode msg = NodeOp.findNode( xmlIn, "Msg" );
  
  while( msg != NULL ) {
    iONode lang = NodeOp.findNode( msg, newlang );
    const char* id = NodeOp.getStr( msg, "id", NULL );
    
    if( id != NULL && lang != NULL ) {
      /* not adding a line in case a global text is wanted */
      const char* txt = NodeOp.getStr( lang, "txt", NULL );
      const char* tip = NodeOp.getStr( lang, "tip", NULL );
      __addlang( xmlOut, id, newlang, txt, tip );
      cnt++;
    }
    
    msg = NodeOp.findNextNode( xmlIn, msg );
  }; 
  TraceOp.println( "Added [%d] new lines", cnt );
  
}

