/** ------------------------------------------------------------
  * $Author: rob $
  * $Date: 2007-06-28 08:49:41 $
  * $Revision: 1.3 $
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

static const char* package   = "PACKAGE";
static const char* language  = "LANGUAGE";
static const char* copyright = "YEAR THE PACKAGE'S COPYRIGHT HOLDER";


static char* __lang2pot( iONode xml );
static char* __lang2po( iONode xml, const char* lang );

/** ------------------------------------------------------------
  * public main()
  * usage: lang2pot -i <xml-file> -o <po-file> [-template] [-l <lang>]
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
  Boolean template = False;
  const char* lang = NULL;


  iOTrace trc = TraceOp.inst( TRCLEVEL_INFO, "lang2po", True );
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

  infile   = CmdLnOp.getStrDef( arg, "-i", NULL );
  outfile  = CmdLnOp.getStrDef( arg, "-o", NULL );
  template = CmdLnOp.hasKey( arg, "-template" );
  lang     = CmdLnOp.getStrDef( arg, "-l", NULL );
  package  = CmdLnOp.getStrDef( arg, "-p", NULL );

  /* Read const.xml */
  if( infile != NULL && FileOp.exist(infile) ) {
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

      {
        char* po = NULL;
        if( template )
          po = __lang2pot( root );
        else
          po = __lang2po( root, lang );
        iOFile pofile = FileOp.inst( outfile, OPEN_WRITE );
        TraceOp.println( "Writing %s...", outfile );
        FileOp.write( pofile, po, StrOp.len( po ) );
        FileOp.close( pofile );
        FileOp.base.del( pofile );
      }

    }

  }
  else {
    TraceOp.println( "usage: lang2pot -i <xml-file> -o <po-file> [-template] [-l <lang>]" );
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

static char* __lang2pot( iONode xml ) {
  int cnt = 0;
  char* template = NULL;
  iONode msg = NodeOp.findNode( xml, "Msg" );


  template = StrOp.fmt( "# %s.\n", NodeOp.getStr(xml, "docname", "?") );
  template = StrOp.cat( template, "# Copyright (C) YEAR THE PACKAGE'S COPYRIGHT HOLDER\n");
  template = StrOp.cat( template, "# This file is distributed under the same license as the PACKAGE package.\n" );
  template = StrOp.cat( template, "# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.\n" );
  template = StrOp.cat( template, "#\n" );
  template = StrOp.cat( template, "#, fuzzy\n\n" );
  template = StrOp.cat( template, "msgid \"\"\n" );
  template = StrOp.cat( template, "msgstr \"\"\n" );
  template = StrOp.cat( template, "\"Project-Id-Version: PACKAGE VERSION\\n\"\n" );
  template = StrOp.cat( template, "\"Report-Msgid-Bugs-To: \\n\"\n" );
  template = StrOp.cat( template, "\"POT-Creation-Date: 2009-01-26 07:51+0100\\n\"\n" );
  template = StrOp.cat( template, "\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n" );
  template = StrOp.cat( template, "\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n\"\n" );
  template = StrOp.cat( template, "\"Language-Team: LANGUAGE <LL@li.org>\\n\"\n" );
  template = StrOp.cat( template, "\"MIME-Version: 1.0\\n\"\n" );
  template = StrOp.cat( template, "\"Content-Type: text/plain; charset=UTF-8\\n\"\n" );
  template = StrOp.cat( template, "\"Content-Transfer-Encoding: 8bit\\n\"\n\n" );

  while( msg != NULL ) {
    iONode en   = NodeOp.findNode( msg, "en" );

    if( en != NULL ) {
      /*
      # id="myname"
      #, c-format
      msgid "My name is %s.\n"
      msgstr ""
      */
      char* msgid = StrOp.fmt( "# id=\"%s\"\n#, c-format\nmsgid \"%s\"\nmsgstr \"\"\n\n",
          NodeOp.getStr(msg, "id", "?"), NodeOp.getStr(en, "txt", "?") );
      template = StrOp.cat( template, msgid );
      StrOp.free(msgid);
      cnt++;
    }

    msg = NodeOp.findNextNode( xml, msg );
  };
  TraceOp.println( "Added [%d] msgid's", cnt );

  return template;
}

static char* __lang2po( iONode xml, const char* lang ) {
  int cnt = 0;
  char* template = NULL;
  iONode msg = NodeOp.findNode( xml, "Msg" );


  template = StrOp.fmt( "# %s.\n", NodeOp.getStr(xml, "docname", "?") );
  template = StrOp.cat( template, "# Copyright (C) YEAR THE PACKAGE'S COPYRIGHT HOLDER\n");
  template = StrOp.cat( template, "# This file is distributed under the same license as the PACKAGE package.\n" );
  template = StrOp.cat( template, "# FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.\n" );
  template = StrOp.cat( template, "#\n" );
  template = StrOp.cat( template, "#, fuzzy\n\n" );
  template = StrOp.cat( template, "msgid \"\"\n" );
  template = StrOp.cat( template, "msgstr \"\"\n" );
  template = StrOp.cat( template, "\"Project-Id-Version: PACKAGE VERSION\\n\"\n" );
  template = StrOp.cat( template, "\"Report-Msgid-Bugs-To: \\n\"\n" );
  template = StrOp.cat( template, "\"POT-Creation-Date: 2009-01-26 07:51+0100\\n\"\n" );
  template = StrOp.cat( template, "\"PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\\n\"\n" );
  template = StrOp.cat( template, "\"Last-Translator: FULL NAME <EMAIL@ADDRESS>\\n\"\n" );
  template = StrOp.cat( template, "\"Language-Team: LANGUAGE <LL@li.org>\\n\"\n" );
  template = StrOp.cat( template, "\"MIME-Version: 1.0\\n\"\n" );
  template = StrOp.cat( template, "\"Content-Type: text/plain; charset=UTF-8\\n\"\n" );
  template = StrOp.cat( template, "\"Content-Transfer-Encoding: 8bit\\n\"\n\n" );

  while( msg != NULL ) {
    iONode en     = NodeOp.findNode( msg, "en" );
    iONode polang = NodeOp.findNode( msg, lang );

    if( en != NULL && polang != NULL ) {
      /*
      # id="myname"
      #, c-format
      msgid "My name is %s.\n"
      msgstr ""
      */
      char* msgid = StrOp.fmt( "# id=\"%s\"\n#, c-format\nmsgid \"%s\"\nmsgstr \"%s\"\n\n",
          NodeOp.getStr(msg, "id", "?"), NodeOp.getStr(en, "txt", "?"), NodeOp.getStr(polang, "txt", "?") );
      template = StrOp.cat( template, msgid );
      StrOp.free(msgid);
      cnt++;
    }

    msg = NodeOp.findNextNode( xml, msg );
  };
  TraceOp.println( "Added [%d] msgid's", cnt );

  return template;
}

