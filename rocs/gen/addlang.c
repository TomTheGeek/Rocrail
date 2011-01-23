/** ------------------------------------------------------------
  * $Author: rob $
  * $Date: 2006/12/22 06:44:08 $
  * $Revision: 1.1 $
  * $Source $
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

static iOMap nodeMap = NULL;
static iOMap descMap = NULL;

static const char* lang = NULL;

static void __addlang( iONode xml, const char* newlang, Boolean empty, Boolean remove );

/** ------------------------------------------------------------
  * public main()
  * usage: addlang -i <inputfile> -l <lang>
  *
  * @param  argc Number of commanline arguments.
  * @param  argv Commanline arguments.
  * @return      Applications exit code.
  */
int main( int argc, const char* argv[] ) {
  int rc = 0;
  iOCmdLn arg = NULL;
  const char* infile = NULL;
  const char* newlang = NULL;
  Boolean empty = False;
  Boolean remove = False;

  iOTrace trc = TraceOp.inst( TRCLEVEL_INFO, "addlang", True );
  TraceOp.setAppID( trc, "a" );

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
  empty = CmdLnOp.hasKey( arg, "-empty" );
  remove = CmdLnOp.hasKey( arg, "-remove" );
  
  /* Read const.xml */
  if( infile != NULL && FileOp.exist(infile) && newlang != NULL ) {
    TraceOp.println( "Processing %s...", infile );
    {
      iOFile constXml = FileOp.inst( infile, OPEN_READONLY );
      char* xmlStr = allocMem( FileOp.size( constXml ) + 1 );
      iODoc doc = NULL;
      iONode root = NULL;

      TraceOp.println( "Reading %s...", infile );
      FileOp.read( constXml, xmlStr, FileOp.size( constXml ) );
      FileOp.close( constXml );
      FileOp.base.del( constXml );

      TraceOp.println( "Parsing %s...", infile );
      doc = DocOp.parse( xmlStr );
      freeMem( xmlStr );
      root = DocOp.getRootNode( doc );

      __addlang( root, newlang, empty, remove );
      {
        char* backupname = StrOp.fmt( "%s.bak", infile );
        TraceOp.println( "Renaming %s to %s", infile, backupname );
        FileOp.rename( infile, backupname );
        StrOp.free( backupname );
      }
      TraceOp.println( "Writing %s...", infile );
      constXml = FileOp.inst( infile, OPEN_WRITE );
      xmlStr = NodeOp.base.toString( root );
      FileOp.write( constXml, xmlStr, StrOp.len( xmlStr ) );
      FileOp.close( constXml );
      FileOp.base.del( constXml );
      
    }

  }
  else {
    TraceOp.println( "Usage: addlang -i <inputfile> -l <lang> [-empty]" );
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

static void __addlang( iONode xml, const char* newlang, Boolean empty, Boolean remove ) {
  int cnt = 0;
  iONode msg = NodeOp.findNode( xml, "Msg" );
  
  while( msg != NULL ) {
    iONode all  = NodeOp.findNode( msg, "all" );
    iONode en   = NodeOp.findNode( msg, "en" );
    iONode lang = NodeOp.findNode( msg, newlang );
    
    if( remove && lang != NULL ) {
      NodeOp.removeChild( msg, lang );
    }
    else if( all == NULL ) {
      /* not adding a line in case a global text is wanted */
    
      if( lang == NULL && en == NULL || lang == NULL && empty) {
        lang = NodeOp.inst( newlang, msg, ELEMENT_NODE );
        NodeOp.setStr( lang, "txt", "" );
        NodeOp.addChild( msg, lang );
        cnt++;
      }
      else if( lang == NULL && en != NULL) {
        lang = (iONode)NodeOp.base.clone(en);
        NodeOp.setName( lang, newlang );
        NodeOp.addChild( msg, lang );
        cnt++;
      }
      else {
        TraceOp.println( "Skipping [%s], lang [%s] already defined", NodeOp.getStr(msg, "id", ""), newlang );
      }
    }
    else {
      TraceOp.println( "Skipping [%s], global [all] found", NodeOp.getStr(msg, "id", "") );
    }
    
    msg = NodeOp.findNextNode( xml, msg );
  }; 
  TraceOp.println( "Added [%d] new lines", cnt );
  
}

