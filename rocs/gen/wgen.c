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

static iOMap nodeMap = NULL;
static iOMap descMap = NULL;

static const char* lang = NULL;

static void __fillDescMap( const char* descfile, const char* lang );
static int __gConstHdr( iONode node, const char* fileName, Boolean docAll, const char* lang );

static int comp( obj* o1, obj* o2 ) {
  iONode node1 = (iONode)*o1;
  iONode node2 = (iONode)*o2;
  return strcmp( NodeOp.getStr( node1, "name", "" ), NodeOp.getStr( node2, "name", "" ) );
}
static int compNodeName( obj* o1, obj* o2 ) {
  iONode node1 = (iONode)*o1;
  iONode node2 = (iONode)*o2;
  return strcmp( NodeOp.getName( node1 ), NodeOp.getName( node2 ) );
}

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

  iOTrace trc = TraceOp.inst( TRCLEVEL_INFO, "wgen", True );
  TraceOp.setAppID( trc, "g" );

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
  
  lang = CmdLnOp.getStrDef( arg, "-lang", "en" );
  descfile = CmdLnOp.getStrDef( arg, "-desc", NULL );
  descMap = MapOp.inst();
  
  if( descfile != NULL ) {
    __fillDescMap( descfile, lang );
  }

  docAll = CmdLnOp.hasKey( arg, "-all" );

  /* Read const.xml */
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
      doc = DocOp.parse( xmlStr );
      freeMem( xmlStr );
      root = DocOp.getRootNode( doc );

      /*{
        char* str = root->base.toString( root );
        printf( "%s", str );
        StrOp.free( str );
      }*/

      {
        char*       wrapper = StrOp.dup( "./wrapper");
        char*    wrapperdoc = StrOp.dup( "./wrapper/doc");
        char* wrapperpublic = StrOp.dup( "./wrapper/public");
        char*   wrapperimpl = StrOp.dup( "./wrapper/impl");
        char*    wrapperbin = StrOp.dup( "./wrapper/bin");

        if( !FileOp.exist( wrapper ) ) {
          TraceOp.println( "creating: \"./wrapper\"..." );
          FileOp.mkdir( wrapper );
        }
        if( !FileOp.exist( wrapperdoc ) ) {
          TraceOp.println( "creating: \"./wrapper/doc\"..." );
          FileOp.mkdir( wrapperdoc );
        }
        if( !FileOp.exist( wrapperimpl ) ) {
          TraceOp.println( "creating: \"./wrapper/impl\"..." );
          FileOp.mkdir( wrapperimpl );
        }
        if( !FileOp.exist( wrapperpublic ) ) {
          TraceOp.println( "creating: \"./wrapper/public\"..." );
          FileOp.mkdir( wrapperpublic );
        }
        if( !FileOp.exist( wrapperbin ) ) {
          TraceOp.println( "creating: \"./wrapper/bin\"..." );
          FileOp.mkdir( wrapperbin );
        }
      }

      __gConstHdr( root, argv[1], docAll, lang );
    }

  }
  else {
    TraceOp.println( "Usage: gconst const.xml" );
  }

  return rc;
}


static const char* __getRemark( iONode node ) {
  if( StrOp.equalsi( "var", NodeOp.getName( node ) ) || StrOp.equalsi( "const", NodeOp.getName( node ) ) ) {
    iONode parent = NodeOp.getParent( node );
    char* id = NULL;
    const char* desc = NULL;
    
    if( parent == NULL )
      return NodeOp.getStr( node, "remark", "" );
    id = StrOp.fmt( "%s.%s", NodeOp.getName( parent ), NodeOp.getStr( node, "name", "" ) );  
    desc = (const char*)MapOp.get( descMap, StrOp.strlwr( id ) );
    StrOp.free( id );
    return desc == NULL ? NodeOp.getStr( node, "remark", "" ):desc;
  }
  else {
    char* nodename = StrOp.dup( NodeOp.getName( node ) );
    const char* desc = (const char*)MapOp.get( descMap, StrOp.strlwr( nodename ) );
    StrOp.free( nodename );
    if( desc != NULL )
      return desc;
    return NodeOp.getStr( node, "remark", "" );
  }
}

static void __fillDescMap( const char* descfile, const char* lang ) {

  iOFile descXml = FileOp.inst( descfile, OPEN_READONLY );
  char* xmlStr = allocMem( FileOp.size( descXml ) + 1 );
  iODoc doc = NULL;
  iONode root = NULL;

  TraceOp.println( "Reading %s...", descfile );
  FileOp.read( descXml, xmlStr, FileOp.size( descXml ) );
  FileOp.close( descXml );
  FileOp.base.del( descXml );

  TraceOp.println( "Parsing %s...", descfile );
  doc = DocOp.parse( xmlStr );
  freeMem( xmlStr );
  if( doc == NULL )
    return;
  root = DocOp.getRootNode( doc );
  
  if( root == NULL )
    return;
    
  {
    int childCnt = NodeOp.getChildCnt( root );
    int i = 0;
    TraceOp.println( "Processing %d childs.", childCnt );
    for( i = 0; i < childCnt; i++ ) {
      iONode child = NodeOp.getChild( root, i );
      if( NodeOp.getType( child ) == ELEMENT_NODE && StrOp.equalsi( NodeOp.getName(child), "description") ) {
        iONode langnode = NodeOp.findNode( child, lang );
        if( langnode != NULL ) {
          const char* id = NodeOp.getStr( child, "id", NULL );
          const char* text = NodeOp.getStr( langnode, "text", NULL );
          if( id != NULL && text != NULL ) {
            MapOp.put( descMap, StrOp.strlwr( id ), (obj)StrOp.dup(text) ); 
            TraceOp.println( "descMap: %s, [%-20.20s...] added.", id, text );
          }
        }
      }
    }
  }
  
}


static int __processPrefix( iOFile fHdr, iOFile fImpl, iOFile fDoc, iOFile fIndex,
                            const char* modulename, const char* title, const char* docname ) {
  int rc = 0;
  time_t clock ;
  time(&clock);



  TraceOp.println( "Processing Prefix A" );
  FileOp.fmt( fHdr, "/** ------------------------------------------------------------\n" );
  FileOp.fmt( fHdr, "  * A U T O   G E N E R A T E D ! ! !\n" );
  FileOp.fmt( fHdr, "  * Generator: Rocs wgen (build %s %s)\n", __DATE__, __TIME__ );
  FileOp.fmt( fHdr, "  * Module: %s\n", modulename );
  FileOp.fmt( fHdr, "  * Date: %s", ctime( &clock ) );
  FileOp.fmt( fHdr, "  */\n" );
  FileOp.fmt( fHdr, "\n" );
  FileOp.fmt( fHdr, "#ifndef __WRAPPER_H\n" );
  FileOp.fmt( fHdr, "#define __WRAPPER_H\n" );
  FileOp.fmt( fHdr, "\n" );
  FileOp.fmt( fHdr, "#include \"rocs/public/rocs.h\"\n" );
  FileOp.fmt( fHdr, "#include \"rocs/public/objbase.h\"\n" );
  FileOp.fmt( fHdr, "#include \"rocs/public/node.h\"\n" );
  FileOp.fmt( fHdr, "#include \"rocs/public/attr.h\"\n" );
  FileOp.fmt( fHdr, "#include \"rocs/public/trace.h\"\n" );
  FileOp.fmt( fHdr, "\n" );

  TraceOp.println( "Processing Prefix B" );
  FileOp.fmt( fHdr, "\n" );
  FileOp.fmt( fHdr, "typedef enum {vt_bool,vt_int,vt_long,vt_float,vt_string} attr_vt;\n" );
  FileOp.fmt( fHdr, "struct __attrdef {\n" );
  FileOp.fmt( fHdr, "  char*   name;\n" );
  FileOp.fmt( fHdr, "  char*   remark;\n" );
  FileOp.fmt( fHdr, "  char*   unit;\n" );
  FileOp.fmt( fHdr, "  char*   vtype;\n" );
  FileOp.fmt( fHdr, "  char*   defval; /* NULL if no default */\n" );
  FileOp.fmt( fHdr, "  char*   range;  /* \"10-1000\" or \"10,20,30\" or \"no,yes,delete\" or \"*\" */\n" );
  FileOp.fmt( fHdr, "  Boolean required;\n" );
  FileOp.fmt( fHdr, "};\n" );
  FileOp.fmt( fHdr, "\n" );
  FileOp.fmt( fHdr, "struct __nodedef {\n" );
  FileOp.fmt( fHdr, "  char*   name;\n" );
  FileOp.fmt( fHdr, "  char*   remark;\n" );
  FileOp.fmt( fHdr, "  Boolean required;\n" );
  FileOp.fmt( fHdr, "  char*   cardinality;\n" );
  FileOp.fmt( fHdr, "};\n" );
  FileOp.fmt( fHdr, "\n" );

  FileOp.fmt( fHdr, "Boolean xBool( struct __attrdef attr); \n" );
  FileOp.fmt( fHdr, "int xInt( struct __attrdef attr); \n" );
  FileOp.fmt( fHdr, "long xLong( struct __attrdef attr); \n" );
  FileOp.fmt( fHdr, "double xFloat( struct __attrdef attr); \n" );
  FileOp.fmt( fHdr, "const char* xStr( struct __attrdef attr); \n" );
  FileOp.fmt( fHdr, "Boolean xNode( struct __nodedef attr, iONode node); \n" );
  FileOp.fmt( fHdr, "Boolean xAttr( struct __attrdef* attr, iONode node); \n" );
  FileOp.fmt( fHdr, "Boolean xAttrTest( struct __attrdef* attr[], iONode node); \n" );
  FileOp.fmt( fHdr, "Boolean xNodeTest( struct __nodedef* nodeList[], iONode node); \n" );
  FileOp.flush( fHdr );

  TraceOp.println( "Processing Prefix C" );
  FileOp.fmt( fImpl, "/** ------------------------------------------------------------\n" );
  FileOp.fmt( fImpl, "  * A U T O   G E N E R A T E D ! ! !\n" );
  FileOp.fmt( fImpl, "  * Generator: Rocs wgen (build %s %s)\n", __DATE__, __TIME__ );
  FileOp.fmt( fImpl, "  * Module: %s\n", modulename );
  FileOp.fmt( fImpl, "  * Date: %s", ctime( &clock ) );
  FileOp.fmt( fImpl, "  */\n" );
  FileOp.fmt( fImpl, "\n" );
  FileOp.fmt( fImpl, "#include \"%s/wrapper/public/wrapper.h\"\n", modulename );
  FileOp.fmt( fImpl, "#include \"rocs/public/str.h\"\n" );
  FileOp.fmt( fImpl, "#include \"rocs/public/wutils.h\"\n" );
  FileOp.fmt( fImpl, "\n" );
  FileOp.fmt( fImpl, "Boolean xBool( struct __attrdef attr) { \n" );
  FileOp.fmt( fImpl, "  if( attr.defval != NULL && StrOp.equalsi( \"true\", attr.defval ) )\n" );
  FileOp.fmt( fImpl, "    return True;\n" );
  FileOp.fmt( fImpl, "  else\n" );
  FileOp.fmt( fImpl, "    return False;\n" );
  FileOp.fmt( fImpl, "} \n" );
  FileOp.fmt( fImpl, "int xInt( struct __attrdef attr) {\n" );
  FileOp.fmt( fImpl, "  if( attr.defval == NULL )\n" );
  FileOp.fmt( fImpl, "    return 0;\n" );
  FileOp.fmt( fImpl, "  return atoi( attr.defval );\n" );
  FileOp.fmt( fImpl, "} \n" );
  FileOp.fmt( fImpl, "long xLong( struct __attrdef attr) {\n" );
  FileOp.fmt( fImpl, "  if( attr.defval == NULL )\n" );
  FileOp.fmt( fImpl, "    return 0;\n" );
  FileOp.fmt( fImpl, "  return atol( attr.defval );\n" );
  FileOp.fmt( fImpl, "} \n" );
  FileOp.fmt( fImpl, "double xFloat( struct __attrdef attr) {\n" );
  FileOp.fmt( fImpl, "  if( attr.defval == NULL )\n" );
  FileOp.fmt( fImpl, "    return 0.0;\n" );
  FileOp.fmt( fImpl, "  return atof( attr.defval );\n" );
  FileOp.fmt( fImpl, "} \n" );
  FileOp.fmt( fImpl, "const char* xStr( struct __attrdef attr) {\n" );
  FileOp.fmt( fImpl, "  if( attr.defval == NULL || StrOp.equalsi( \"NULL\", attr.defval ) )\n" );
  FileOp.fmt( fImpl, "    return NULL;\n" );
  FileOp.fmt( fImpl, "  return attr.defval;\n" );
  FileOp.fmt( fImpl, "} \n" );
  FileOp.fmt( fImpl, "Boolean xNode( struct __nodedef def, iONode node ) {\n" );
  FileOp.fmt( fImpl, "  if( !StrOp.equalsi( def.name, NodeOp.getName( node ) ) ) {\n" );
  FileOp.fmt( fImpl, "    TraceOp.trc( \"wrapper\", TRCLEVEL_WRAPPER, __LINE__, 9999, \"Wrong wrapper? NodeName %cs expected instead of %cs\", def.name, NodeOp.getName( node ) );\n", '%', '%');
  FileOp.fmt( fImpl, "    return False;\n" );
  FileOp.fmt( fImpl, "  } \n" );
  FileOp.fmt( fImpl, "  return True;\n" );
  FileOp.fmt( fImpl, "} \n" );

  FileOp.fmt( fImpl, "Boolean xAttr( struct __attrdef* def, iONode node ) {\n" );
  FileOp.fmt( fImpl, "  Boolean ok = True;\n" );
  FileOp.fmt( fImpl, "  iOAttr attr = NodeOp.findAttr( node, (const char*)def->name );\n" );
  FileOp.fmt( fImpl, "  if( attr == NULL && def->required ) {\n" );
  FileOp.fmt( fImpl, "    TraceOp.trc( \"param\", TRCLEVEL_EXCEPTION, __LINE__, 9999, \">>>>> Required attribute %cs.%cs not found!\", NodeOp.getName( node ), def->name );\n", '%', '%');
  FileOp.fmt( fImpl, "    return False;\n" );
  FileOp.fmt( fImpl, "  } \n" );
  FileOp.fmt( fImpl, "  else if( attr == NULL && !def->required ) {\n" );
  FileOp.fmt( fImpl, "    return True;\n" );
  FileOp.fmt( fImpl, "  } \n" );
  FileOp.fmt( fImpl, "  ok = wUtils.checkAttrRange( NodeOp.getName( node ), def->name, def->vtype, def->range, NodeOp.getStr( node, def->name, def->defval ) );\n" );
  FileOp.fmt( fImpl, "  if( !ok && !def->required ) {\n" );
  FileOp.fmt( fImpl, "    NodeOp.setStr( node, def->name, StrOp.dup( def->defval ) );\n" );
  FileOp.fmt( fImpl, "    ok = True;\n" );
  FileOp.fmt( fImpl, "    TraceOp.trc( \"param\", TRCLEVEL_WARNING, __LINE__, 9999, \"Using default [%cs%cs] for %cs.%cs.\", def->defval, def->unit, NodeOp.getName( node ), def->name );\n", '%', '%', '%', '%');
  FileOp.fmt( fImpl, "  } \n" );
  FileOp.fmt( fImpl, "  return ok;\n" );
  FileOp.fmt( fImpl, "} \n" );

  FileOp.fmt( fImpl, "Boolean xAttrTest( struct __attrdef* def[], iONode node ) {\n" );
  FileOp.fmt( fImpl, "  Boolean ok = True;\n" );
  FileOp.fmt( fImpl, "  int i = 0;\n" );
  FileOp.fmt( fImpl, "  int cnt = NodeOp.getAttrCnt( node );\n" );
  FileOp.fmt( fImpl, "  TraceOp.trc( \"param\", TRCLEVEL_WRAPPER, __LINE__, 9999, \"Testing %cd attributes in node [%cs]\", cnt, NodeOp.getName( node ) );\n", '%', '%' );
  FileOp.fmt( fImpl, "  for( i = 0; i < cnt; i++ ) {\n" );
  FileOp.fmt( fImpl, "    iOAttr attr = NodeOp.getAttr( node, i );\n" );
  FileOp.fmt( fImpl, "    int n = 0;\n" );
  FileOp.fmt( fImpl, "    Boolean attrMatch = False;\n" );
  FileOp.fmt( fImpl, "    struct __attrdef* attrdef = def[n];\n" );
  FileOp.fmt( fImpl, "    TraceOp.trc( \"param\", TRCLEVEL_WRAPPER, __LINE__, 9999, \"Checking attribute [%cs] in node [%cs]\", AttrOp.getName( attr ), NodeOp.getName( node ) );\n", '%', '%' );
  FileOp.fmt( fImpl, "    while( attrdef ) {\n");
  FileOp.fmt( fImpl, "      if( StrOp.equalsi( AttrOp.getName( attr ), attrdef->name ) ) {\n" );
  FileOp.fmt( fImpl, "        attrMatch = True;\n" );
  FileOp.fmt( fImpl, "        break;\n" );
  FileOp.fmt( fImpl, "      };\n" );
  FileOp.fmt( fImpl, "      n++;\n" );
  FileOp.fmt( fImpl, "      attrdef = def[n];\n" );
  FileOp.fmt( fImpl, "    };\n" );
  FileOp.fmt( fImpl, "    if( !attrMatch && n > 0 ) {\n" );
  FileOp.fmt( fImpl, "      ok = False;\n" );
  FileOp.fmt( fImpl, "      TraceOp.trc( \"param\", TRCLEVEL_WARNING, __LINE__, 9999, \"Unknown attribute [%cs] found in node [%cs]\", AttrOp.getName( attr ), NodeOp.getName( node ) );\n", '%', '%' );
  FileOp.fmt( fImpl, "    } \n" );
  FileOp.fmt( fImpl, "  }\n" );
  FileOp.fmt( fImpl, "  return ok;\n" );
  FileOp.fmt( fImpl, "} \n" );

  FileOp.fmt( fImpl, "Boolean xNodeTest( struct __nodedef* def[], iONode node ) {\n" );
  FileOp.fmt( fImpl, "  Boolean ok = True;\n" );
  FileOp.fmt( fImpl, "  int i = 0;\n" );
  FileOp.fmt( fImpl, "  int cnt = NodeOp.getChildCnt( node );\n" );
  FileOp.fmt( fImpl, "  TraceOp.trc( \"param\", TRCLEVEL_WRAPPER, __LINE__, 9999, \"Testing %cd childnodes in node [%cs]\", cnt, NodeOp.getName( node ) );\n", '%', '%' );
  FileOp.fmt( fImpl, "  for( i = 0; i < cnt; i++ ) {\n" );
  FileOp.fmt( fImpl, "    iONode childnode = NodeOp.getChild( node, i );\n" );
  FileOp.fmt( fImpl, "    int n = 0;\n" );
  FileOp.fmt( fImpl, "    Boolean nodeMatch = False;\n" );
  FileOp.fmt( fImpl, "    struct __nodedef* nodedef = def[n];\n" );
  FileOp.fmt( fImpl, "    TraceOp.trc( \"param\", TRCLEVEL_WRAPPER, __LINE__, 9999, \"Checking childnode [%cs] in node [%cs]\", NodeOp.getName( childnode ), NodeOp.getName( node ) );\n", '%', '%' );
  FileOp.fmt( fImpl, "    while( nodedef ) {\n");
  FileOp.fmt( fImpl, "      if( StrOp.equalsi( NodeOp.getName( childnode ), nodedef->name ) || StrOp.equalsi( NodeOp.getName( childnode ), \"remark\" ) ) {\n" );
  FileOp.fmt( fImpl, "        nodeMatch = True;\n" );
  FileOp.fmt( fImpl, "        break;\n" );
  FileOp.fmt( fImpl, "      };\n" );
  FileOp.fmt( fImpl, "      n++;\n" );
  FileOp.fmt( fImpl, "      nodedef = def[n];\n" );
  FileOp.fmt( fImpl, "    };\n" );
  FileOp.fmt( fImpl, "    if( !nodeMatch && n > 0 ) {\n" );
  FileOp.fmt( fImpl, "      ok = False;\n" );
  FileOp.fmt( fImpl, "      TraceOp.trc( \"param\", TRCLEVEL_WARNING, __LINE__, 9999, \"Unknown childnode [%cs] found in node [%cs]\", NodeOp.getName( childnode ), NodeOp.getName( node ) );\n", '%', '%' );
  FileOp.fmt( fImpl, "    } \n" );
  FileOp.fmt( fImpl, "  }\n" );
  FileOp.fmt( fImpl, "  return ok;\n" );
  FileOp.fmt( fImpl, "} \n" );

  FileOp.flush( fImpl );

  TraceOp.println( "Processing Prefix D" );
  FileOp.fmt( fDoc, "<html>\n" );
  FileOp.fmt( fDoc, "<head>\n  <title>%s</title>\n", title );
  FileOp.fmt( fDoc, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n" );
  FileOp.fmt( fDoc, "</head>\n" );
  
  FileOp.fmt( fDoc, "<body>\n" );
  FileOp.fmt( fDoc, "<h2>%s</h2>\n", title );
  FileOp.fmt( fDoc, "Module: %s<br>\n", modulename );
  FileOp.fmt( fDoc, "Auto generated at: %s<br>\n", ctime( &clock ) );
  /*FileOp.fmt( fDoc, "Index: <a href=\"%s-index.html\">wrapper-index.html</a><br>\n", docname, ctime( &clock ) );*/

  FileOp.flush( fDoc );

  TraceOp.println( "Processing Prefix E" );
  FileOp.fmt( fIndex, "<html>\n" );
  FileOp.fmt( fIndex, "<head>\n  <title>Index for %s</title>\n</head>\n", title );
  FileOp.fmt( fIndex, "<body>\n" );
  FileOp.fmt( fIndex, "<h2>%s</h2>\n", title );
  FileOp.fmt( fIndex, "Module: %s<br>\n", modulename );
  FileOp.fmt( fIndex, "Auto generated at: %s<br>\n", ctime( &clock ) );
  FileOp.fmt( fIndex, "<hr><h1>Index:</h1>\n", title );
  FileOp.fmt( fIndex, "<ol>\n" );

  FileOp.flush( fIndex );

  return rc;
}

static int __processSuffix( iOFile fHdr, iOFile fImpl, iOFile fDoc, iOFile fIndex ) {
  int rc = 0;
  TraceOp.println( "Processing Suffix" );
  FileOp.fmt( fHdr, "\n\n#endif\n" );
  FileOp.flush( fHdr );

  FileOp.fmt( fDoc, "</body>\n</html>\n" );
  FileOp.flush( fDoc );

  FileOp.fmt( fIndex, "</ol>\n</body>\n</html>\n" );
  FileOp.flush( fIndex );

  return rc;
}

static void __addIdent( iOFile f, int level ) {
  int i = 0;
  for( i = 0; i < level; i++ ) {
    FileOp.fmt( f, "  " );
  }
}

typedef enum {vt_bool,vt_int,vt_long,vt_float,vt_string} attr_vt;
static int __getVarType( const char* vtype ) {
  if( StrOp.equals( "bool", vtype ) )
    return vt_bool;
  else if( StrOp.equals( "int", vtype ) )
    return vt_int;
  else if( StrOp.equals( "long", vtype ) )
    return vt_long;
  else if( StrOp.equals( "float", vtype ) )
    return vt_float;
  else
    return vt_string;
}



static void __wrpPrefix( iOFile fPublH, iOFile fImplC, const char* wrpName, const char* modulename ) {
  time_t clock ;
  time(&clock);

  FileOp.fmt( fPublH, "/** ------------------------------------------------------------\n" );
  FileOp.fmt( fPublH, "  * A U T O   G E N E R A T E D\n" );
  FileOp.fmt( fPublH, "  * Generator: Rocs wgen (build %s %s)\n", __DATE__, __TIME__ );
  FileOp.fmt( fPublH, "  * Module: %s\n", modulename );
  FileOp.fmt( fPublH, "  * Date: %s", ctime( &clock ) );
  FileOp.fmt( fPublH, "  */\n\n" );
  FileOp.fmt( fPublH, "#ifndef __WRAPPER_%s_H\n", wrpName );
  FileOp.fmt( fPublH, "#define __WRAPPER_%s_H\n\n", wrpName );
  FileOp.fmt( fPublH, "#include \"%s/wrapper/public/wrapper.h\"\n", modulename );
  FileOp.fmt( fPublH, "#include \"rocs/public/rocs.h\"\n" );
  FileOp.fmt( fPublH, "#include \"rocs/public/objbase.h\"\n" );
  FileOp.fmt( fPublH, "#include \"rocs/public/node.h\"\n" );
  FileOp.fmt( fPublH, "#include \"rocs/public/str.h\"\n" );
  FileOp.fmt( fPublH, "#ifdef __cplusplus\n" );
  FileOp.fmt( fPublH, "  extern \"C\" {\n" );
  FileOp.fmt( fPublH, "#endif\n" );
  FileOp.fmt( fPublH, "\n\n" );
  FileOp.fmt( fPublH, "struct __w%s {\n", wrpName );

  FileOp.fmt( fImplC, "/** ------------------------------------------------------------\n" );
  FileOp.fmt( fImplC, "  * A U T O   G E N E R A T E D ! ! !\n" );
  FileOp.fmt( fImplC, "  * Generator: Rocs wgen (build %s %s)\n", __DATE__, __TIME__ );
  FileOp.fmt( fImplC, "  * Module: %s\n", modulename );
  FileOp.fmt( fImplC, "  * Date: %s", ctime( &clock ) );
  FileOp.fmt( fImplC, "  */\n\n" );
  FileOp.fmt( fImplC, "#include \"%s/wrapper/public/%s.h\"\n", modulename, wrpName );
  FileOp.fmt( fImplC, "#include <stdarg.h>\n\n\n" );
  FileOp.fmt( fImplC, "/* ------------------------------------------------------------\n" );
  FileOp.fmt( fImplC, " * WRAPPER: %s.%s\n", modulename, wrpName );
  FileOp.fmt( fImplC, " * ------------------------------------------------------------\n" );
  FileOp.fmt( fImplC, " */\n" );
}

static void __wrpAddConst( iONode var, iOFile fPublH, iOFile fImplC, iOList opList, const char* structName ) {
  char* vt = "const char*";
  char* vtNode = "Str";
  char* val = NULL;
  const char* attrVal  = NodeOp.getStr( var, "val", "" );
  const char* attrName = NodeOp.getStr( var, "name", "?" );
  if( StrOp.equalsi( "int", NodeOp.getStr( var, "vt", "string" ) ) ) {
    vt = "int";
    vtNode = "Int";
    val = StrOp.dup(attrVal);
  }
  else if( StrOp.equalsi( "long", NodeOp.getStr( var, "vt", "string" ) ) ) {
    vt = "long";
    vtNode = "Long";
    val = StrOp.dup(attrVal);
  }
  else if( StrOp.equalsi( "float", NodeOp.getStr( var, "vt", "string" ) ) ) {
    vt = "double";
    vtNode = "Float";
    val = StrOp.dup(attrVal);
  }
  else if( StrOp.equalsi( "bool", NodeOp.getStr( var, "vt", "string" ) ) ) {
    vt = "Boolean";
    vtNode = "Bool";
    val = StrOp.dup( StrOp.equalsi( "true", attrVal ) ? "True":"False" );
  }
  else if( StrOp.equalsi( "char", NodeOp.getStr( var, "vt", "string" ) ) ) {
    vt = "char";
    vtNode = "Char";
    val = StrOp.fmt( "\'%c\'", attrVal[0] );
  }
  else {
    if( attrVal[0] == '$' ) {
      val = StrOp.fmt( "%s", attrVal+1 );
    }
    else
      val = StrOp.fmt( "\"%s\"", attrVal );
  }

  FileOp.fmt( fPublH, "  %s %s;\n", vt, attrName );
  ListOp.add( opList, (obj)val );
}


static Boolean __evalType( iONode var, char** vt, char** vtNode, char** prefix ) {
  if( StrOp.equalsi( "int", NodeOp.getStr( var, "vt", "string" ) ) ) {
    *vt = "int";
    *vtNode = "Int";
  }
  else if( StrOp.equalsi( "long", NodeOp.getStr( var, "vt", "string" ) ) ) {
    *vt = "long";
    *vtNode = "Long";
  }
  else if( StrOp.equalsi( "float", NodeOp.getStr( var, "vt", "string" ) ) ) {
    *vt = "double";
    *vtNode = "Float";
  }
  else if( StrOp.equalsi( "bool", NodeOp.getStr( var, "vt", "string" ) ) ) {
    *vt = "Boolean";
    *vtNode = "Bool";
    *prefix = "is";
  }

  return NodeOp.getBool( var, "readonly", False );;
}

static void __wrpAddVar( iONode var, iOFile fPublH, iOFile fImplC, iOList opList, iOList attrList, const char* structName, Boolean getname  ) {
  char* vt = "const char*";
  char* vtNode = "Str";
  char* prefix = "get";
  const char* attrName = NodeOp.getStr( var, "name", "?" );
  const char* wrapperName = NodeOp.getStr( var, "wrappername", attrName );

  Boolean readonly = __evalType( var, &vt, &vtNode, &prefix );

  FileOp.fmt( fPublH, "  %s (*%s%s)(iONode);\n", vt, prefix, wrapperName );

  {
    char* l_attrName = StrOp.fmt( "__%s", attrName );
    ListOp.add( attrList, (obj)l_attrName );
  }

  FileOp.fmt( fImplC, "\n\n/* -a-t-t-r-i-b-u-t-e------------------------------------------\n" );
  FileOp.fmt( fImplC, " * %s\n", attrName );
  FileOp.fmt( fImplC, " */\n" );
  FileOp.fmt( fImplC, "static struct __attrdef __%s = {\n", attrName );
  FileOp.fmt( fImplC, "  \"%s\",", NodeOp.getStr( var, "name", "?" ) );
  FileOp.fmt( fImplC, "  \"%s\",", __getRemark(var) );
  FileOp.fmt( fImplC, "  \"%s\",", NodeOp.getStr( var, "unit", "" ) );
  FileOp.fmt( fImplC, "  \"%s\",", NodeOp.getStr( var, "vt", "string" ) );
  {
    const char* defval = NodeOp.getStr( var, "defval", "" );
    if( defval[0] == '$' ) {
      FileOp.fmt( fImplC, "  %s,", defval+1 );
    }
    else
      FileOp.fmt( fImplC, "  \"%s\",", defval );
  }
  FileOp.fmt( fImplC, "  \"%s\",", NodeOp.getStr( var, "range", "" ) );
  FileOp.fmt( fImplC, "  %s,\n", NodeOp.getBool( var, "required", False )?"True":"False" );
  FileOp.fmt( fImplC, "};\n" );

  FileOp.fmt( fImplC, "static %s _%s%s(iONode node) {\n", vt, prefix, wrapperName );
  FileOp.fmt( fImplC, "  %s defval = x%s( __%s );\n", vt, vtNode, attrName );
  FileOp.fmt( fImplC, "  \n" );
  FileOp.fmt( fImplC, "  if( node == NULL ) {\n" );
  FileOp.fmt( fImplC, "    return defval;\n" );
  FileOp.fmt( fImplC, "  }\n" );
  FileOp.fmt( fImplC, "  xNode( __%s, node );\n", structName );
  FileOp.fmt( fImplC, "  return NodeOp.get%s( node, \"%s\", defval );\n", vtNode, attrName );
  FileOp.fmt( fImplC, "}\n" );
  {
    char* opName = StrOp.fmt( "_%s%s", prefix, wrapperName );
    ListOp.add( opList, (obj)opName );
  }

  if( !readonly ) {
    char* opName = StrOp.fmt( "_set%s", wrapperName );
    ListOp.add( opList, (obj)opName );
    FileOp.fmt( fPublH, "  void (*set%s)(iONode,%s);\n", wrapperName, vt );
    FileOp.fmt( fImplC, "static void _set%s(iONode node, %s p_%s) {\n", wrapperName, vt, attrName );
    FileOp.fmt( fImplC, "  if( node == NULL ) return;\n" );
    FileOp.fmt( fImplC, "  xNode( __%s, node );\n", structName );
    FileOp.fmt( fImplC, "  NodeOp.set%s( node, \"%s\", p_%s );\n", vtNode, attrName, attrName );
    FileOp.fmt( fImplC, "}\n" );
  }

  if( getname) {
    char* opName = StrOp.fmt( "_name%s", wrapperName );
    ListOp.add( opList, (obj)opName );
    FileOp.fmt( fPublH, "  const char* (*name%s)(void);\n", wrapperName );
    FileOp.fmt( fImplC, "static const char* _name%s(void) {\n", wrapperName );
    FileOp.fmt( fImplC, "  return \"%s\";\n", attrName );
    FileOp.fmt( fImplC, "}\n" );
  }
}

static void __wrpAddNode( iONode node, iOFile fPublH, iOFile fImplC, iOList opList, iOList nodeList, const char* structName ) {
  const char* nodeName = NodeOp.getName( node );
  const char* xmlName = NodeOp.getStr( node, "xmlname", nodeName );
  Boolean readonly = NodeOp.getBool( node, "readonly", False );
  FileOp.fmt( fPublH, "  iONode (*get%s)(iONode);\n", nodeName );

  FileOp.fmt( fImplC, "\n\n/* -c-h-i-l-d-n-o-d-e------------------------------------------\n" );
  FileOp.fmt( fImplC, " * %s\n", nodeName );
  FileOp.fmt( fImplC, " */\n" );

  FileOp.fmt( fImplC, "static struct __nodedef __%s = {\n", nodeName );
  FileOp.fmt( fImplC, "  \"%s\",", nodeName );
  FileOp.fmt( fImplC, "  \"%s\",", __getRemark(node) );
  FileOp.fmt( fImplC, "  %s,", NodeOp.getBool( node, "required", False )?"True":"False" );
  FileOp.fmt( fImplC, "  \"%s\",", NodeOp.getStr( node, "cardinality", "1" ) );
  FileOp.fmt( fImplC, "};\n" );

  {
    char* l_nodeName = StrOp.fmt( "__%s", nodeName );
    ListOp.add( nodeList, (obj)l_nodeName );
  }

  FileOp.fmt( fImplC, "static iONode _get%s(iONode node) {\n", nodeName );
  FileOp.fmt( fImplC, "  xNode( __%s, node );\n", structName );
  FileOp.fmt( fImplC, "  return NodeOp.findNode( node, \"%s\");\n", xmlName );
  FileOp.fmt( fImplC, "}\n" );
  {
    char* opName = StrOp.fmt( "_get%s", nodeName );
    ListOp.add( opList, (obj)opName );
  }
  FileOp.fmt( fPublH, "  iONode (*next%s)(iONode,iONode);\n", nodeName );
  FileOp.fmt( fImplC, "\n\nstatic iONode _next%s(iONode node, iONode child) {\n", nodeName );
  FileOp.fmt( fImplC, "  xNode( __%s, node );\n", structName );
  FileOp.fmt( fImplC, "  return NodeOp.findNextNode( node, child);\n" );
  FileOp.fmt( fImplC, "}\n" );
  {
    char* opName = StrOp.fmt( "_next%s", nodeName );
    ListOp.add( opList, (obj)opName );
  }
  if( !readonly ) {
    char* opName = StrOp.fmt( "_set%s", nodeName );
    ListOp.add( opList, (obj)opName );
    FileOp.fmt( fPublH, "  void (*set%s)(iONode,iONode);\n", nodeName );
    FileOp.fmt( fImplC, "static void _set%s(iONode node, iONode p_%s) {\n", nodeName, nodeName );
    FileOp.fmt( fImplC, "  xNode( __%s, node );\n", structName );
    FileOp.fmt( fImplC, "  TraceOp.println( \"!!!!!TODO!!!!! Wrapper setNode()\" );\n" );
    FileOp.fmt( fImplC, "}\n" );
  }
}

static void __wrpSuffix( iOFile fPublH, iOFile fImplC, const char* wrpName, const char* modulename, iOList opList, iOList attrList ) {
  FileOp.fmt( fPublH, "};\n", wrpName );
  FileOp.fmt( fPublH, "\n" );
  FileOp.fmt( fPublH, "extern struct __w%s w%s;\n", wrpName, wrpName );
  FileOp.fmt( fPublH, "\n" );
  FileOp.fmt( fPublH, "#ifdef __cplusplus\n" );
  FileOp.fmt( fPublH, "  }\n" );
  FileOp.fmt( fPublH, "#endif\n" );
  FileOp.fmt( fPublH, "\n\n#endif\n", wrpName );

  FileOp.fmt( fImplC, "\n\n" );
  FileOp.fmt( fImplC, "struct __w%s w%s = {\n", wrpName, wrpName );
  {
    int n = ListOp.size( opList );
    int i = 0;
    for( i = 0; i < n; i++ ) {
      const char* op = (const char*)ListOp.get( opList, i );
      FileOp.fmt( fImplC, "  %s,\n", op );
    }
  }
  FileOp.fmt( fImplC, "};\n" );
}


static void __wrpCreate( iONode node, const char* modulename ) {
  const char*  wrpName = NodeOp.getStr( node, "wrappername"  , NodeOp.getName( node ) );
  const char* nodeName = NodeOp.getName( node );
  const char*  xmlName = NodeOp.getStr( node, "xmlname", nodeName );

  char* wrpPublH = StrOp.fmt( "wrapper/public/%s.h", wrpName );
  char* wrpImplC = StrOp.fmt( "wrapper/impl/%s.c", wrpName );

  iOFile fPublH = FileOp.inst( wrpPublH, OPEN_WRITE );
  iOFile fImplC = FileOp.inst( wrpImplC, OPEN_WRITE );

  iOList opList = ListOp.inst();
  iOList attrList = ListOp.inst();
  iOList nodeList = ListOp.inst();
  iOList varList = ListOp.inst();
  iOList constList = ListOp.inst();
  iOList subnodeList = ListOp.inst();

  TraceOp.println( "creating wrapper \"%s\"", wrpName );
  
  if( fPublH != NULL && fImplC != NULL ) {
    int childCnt = NodeOp.getChildCnt( node );
    int i = 0;
    __wrpPrefix( fPublH, fImplC, wrpName, modulename );

    FileOp.fmt( fImplC, "static struct __nodedef __%s = {\n", nodeName );
    FileOp.fmt( fImplC, "  \"%s\",", xmlName );
    FileOp.fmt( fImplC, "  \"%s\",", __getRemark(node) );
    FileOp.fmt( fImplC, "  %s,", NodeOp.getBool( node, "required", False )?"True":"False" );
    FileOp.fmt( fImplC, "  \"%s\",\n", NodeOp.getStr( node, "cardinality", "1" ) );
    FileOp.fmt( fImplC, "};\n\n" );

    FileOp.fmt( fPublH, "  const char* (*name)(void);\n" );
    FileOp.fmt( fImplC, "static const char* _node_name(void) {\n" );
    FileOp.fmt( fImplC, "  return __%s.name;\n", nodeName );
    FileOp.fmt( fImplC, "}\n" );
    ListOp.add( opList, (obj)"_node_name" );

    FileOp.fmt( fPublH, "  const char* (*remark)(void);\n" );
    FileOp.fmt( fImplC, "static const char* _node_remark(void) {\n" );
    FileOp.fmt( fImplC, "  return __%s.remark;\n", nodeName );
    FileOp.fmt( fImplC, "}\n" );
    ListOp.add( opList, (obj)"_node_remark" );

    FileOp.fmt( fPublH, "  Boolean (*required)(void);\n" );
    FileOp.fmt( fImplC, "static Boolean _node_required(void) {\n" );
    FileOp.fmt( fImplC, "  return __%s.required;\n", nodeName );
    FileOp.fmt( fImplC, "}\n" );
    ListOp.add( opList, (obj)"_node_required" );

    FileOp.fmt( fPublH, "  const char* (*cardinality)(void);\n" );
    FileOp.fmt( fImplC, "static const char* _node_cardinality(void) {\n" );
    FileOp.fmt( fImplC, "  return __%s.cardinality;\n", nodeName );
    FileOp.fmt( fImplC, "}\n" );
    ListOp.add( opList, (obj)"_node_cardinality" );

    for( i = 0; i < childCnt; i++ ) {
      iONode var = NodeOp.getChild( node, i );
      if( NodeOp.getType( var ) != ELEMENT_NODE )
        continue;
      if( StrOp.equals( "var", NodeOp.getName( var ) ) ) {
        ListOp.add( varList, (obj) var );
        /*__wrpAddVar( var, fPublH, fImplC, opList, attrList, nodeName );*/
      }
      else if( StrOp.equals( "const", NodeOp.getName( var ) ) ) {
        ListOp.add( constList, (obj) var );
        /*__wrpAddConst( var, fPublH, fImplC, opList, nodeName );*/
      }
      else {
        ListOp.add( subnodeList, (obj) var );
        /*__wrpAddNode( var, fPublH, fImplC, opList, nodeList, nodeName );*/
      }
    }

    ListOp.sort( varList, comp );
    ListOp.sort( constList, comp );
    ListOp.sort( subnodeList, compNodeName );

    for( i = 0; i < ListOp.size( constList ); i++ ) {
      iONode var = (iONode)ListOp.get( constList, i );
        __wrpAddConst( var, fPublH, fImplC, opList, nodeName );
    }
    for( i = 0; i < ListOp.size( varList ); i++ ) {
      Boolean getname = NodeOp.getBool( node, "getname", False );
      iONode var = (iONode)ListOp.get( varList, i );
      __wrpAddVar( var, fPublH, fImplC, opList, attrList, nodeName, getname );
    }
    for( i = 0; i < ListOp.size( subnodeList ); i++ ) {
      iONode var = (iONode)ListOp.get( subnodeList, i );
      __wrpAddNode( var, fPublH, fImplC, opList, nodeList, nodeName );
    }


    FileOp.fmt( fPublH, "  Boolean (*dump)(iONode node);\n" );



    FileOp.fmt( fImplC, "\n/* -a-t-t-r-i-b-u-t-e-t-e-s-t----------------------------------\n" );
    FileOp.fmt( fImplC, "\n * dump\n" );
    FileOp.fmt( fImplC, "\n */\n" );
    {
      int n = ListOp.size( attrList );
      FileOp.fmt( fImplC, "static struct __attrdef* attrList[%d] = {NULL};\n", n + 1 );
      n = ListOp.size( nodeList );
      FileOp.fmt( fImplC, "static struct __nodedef* nodeList[%d] = {NULL};\n", n + 1 );
    }
    FileOp.fmt( fImplC, "static Boolean _node_dump(iONode node) {\n" );
    FileOp.fmt( fImplC, "  if( node == NULL && __%s.required ) {\n", nodeName );
    FileOp.fmt( fImplC, "    TraceOp.trc( \"param\", TRCLEVEL_EXCEPTION, __LINE__, 9999, \">>>>> Required node %s not found!\" );\n", nodeName );
    FileOp.fmt( fImplC, "    return False;\n" );
    FileOp.fmt( fImplC, "  }\n" );
    FileOp.fmt( fImplC, "  else if( node == NULL ) {\n" );
    FileOp.fmt( fImplC, "    TraceOp.trc( \"param\", TRCLEVEL_WRAPPER, __LINE__, 9999, \"Node %s not found!\" );\n", nodeName );
    FileOp.fmt( fImplC, "    return True;\n" );
    FileOp.fmt( fImplC, "  }\n" );
    FileOp.fmt( fImplC, "  TraceOp.trc( \"param\", TRCLEVEL_PARAM, __LINE__, 9999, \"\" );\n" );
    {
      int n = ListOp.size( attrList );
      int i = 0;
      for( i = 0; i < n; i++ ) {
        const char* attr = (const char*)ListOp.get( attrList, i );
        FileOp.fmt( fImplC, "  attrList[%d] = &%s;\n", i, attr );
      }
      FileOp.fmt( fImplC, "  attrList[%d] = NULL;\n", i );

      n = ListOp.size( nodeList );
      for( i = 0; i < n; i++ ) {
        const char* l_node = (const char*)ListOp.get( nodeList, i );
        FileOp.fmt( fImplC, "  nodeList[%d] = &%s;\n", i, l_node );
      }
      FileOp.fmt( fImplC, "  nodeList[%d] = NULL;\n", i );
    }
    FileOp.fmt( fImplC, "  {\n" );
    FileOp.fmt( fImplC, "    int i = 0;\n" );
    FileOp.fmt( fImplC, "    Boolean err = False;\n" );
    FileOp.fmt( fImplC, "    xAttrTest( attrList, node );\n" );
    FileOp.fmt( fImplC, "    xNodeTest( nodeList, node );\n" );
    FileOp.fmt( fImplC, "    while( attrList[i] ) {\n" );
    FileOp.fmt( fImplC, "      err |= !xAttr( attrList[i], node );\n" );
    FileOp.fmt( fImplC, "      i++;\n" );
    FileOp.fmt( fImplC, "    };\n" );
    FileOp.fmt( fImplC, "    return !err;\n" );
    FileOp.fmt( fImplC, "  }\n" );
    FileOp.fmt( fImplC, "  \n" );
    FileOp.fmt( fImplC, "}\n" );
    ListOp.add( opList, (obj)"_node_dump" );

    __wrpSuffix( fPublH, fImplC, wrpName, modulename, opList, attrList );
  }

  opList->base.del( opList );
  attrList->base.del( attrList );
  ListOp.base.del( nodeList );
  ListOp.base.del( constList );
  ListOp.base.del( varList );
  ListOp.base.del( subnodeList );

  FileOp.base.del( fPublH );
  FileOp.base.del( fImplC );
}



static Boolean __processChild( char* indexStr, iONode child, iOFile fHdr, iOFile fImpl, iOFile fDoc,
                           iOFile fIndex, int level, const char* parent, const char* modulename,
                           const char* docname, Boolean pubWrapper, Boolean docAll ) {
  const char* nodeName = NodeOp.getName( child );
  const char*  docName = NodeOp.getStr( child, "docname", nodeName );
  const char*  xmlName = NodeOp.getStr( child, "xmlname", docName );
  const char* docTitle = NodeOp.getStr( child, "title", docName );
  Boolean          pub = pubWrapper?NodeOp.getBool( child, "public", pubWrapper ):False;
  Boolean   extWrapper = NodeOp.getBool( child, "extern", False );
  Boolean referenceonly = NodeOp.getBool( child, "referenceonly", False );
  const char*  wrpName = NodeOp.getStr( child, "wrappername"  , nodeName );
  iOList     constList = ListOp.inst();
  iOList       varList = ListOp.inst();
  iOList   subnodeList = ListOp.inst();
  Boolean     existing = False;

  if( docAll )
    pub = True;

  TraceOp.println( "generating child \"%s\"", nodeName );

  if( !extWrapper ) {
    if( referenceonly ) {
      TraceOp.println( "reference to child \"%s\"", nodeName );
      existing = True;
    }
    else if( MapOp.haskey( nodeMap, NodeOp.getName( child ) ) ) {
      child = (iONode)MapOp.get( nodeMap, NodeOp.getName( child ) );
      TraceOp.println( "using existing for child \"%s\"", nodeName );
      existing = True;
    }
    else {
      MapOp.put( nodeMap, NodeOp.getName( child ), (obj)child );
      if( NodeOp.getBool( child, "createwrapper", True ) )
        __wrpCreate( child, modulename );
    }
  }

  {
    int childCnt = NodeOp.getChildCnt( child );
    int i = 0;
    TraceOp.println( "Processing %d childs.", childCnt );
    for( i = 0; i < childCnt; i++ ) {
      iONode var = NodeOp.getChild( child, i );

      if( NodeOp.getType( var ) != ELEMENT_NODE )
        continue;

      if( StrOp.equals( "var", NodeOp.getName( var ) ) ) {
        ListOp.add( varList, (obj)var );
      }
      else if( StrOp.equals( "const", NodeOp.getName( var ) ) ) {
        ListOp.add( constList, (obj)var );
      }
      else {
        /* subnode */
        ListOp.add( subnodeList, (obj)var );
      }
    }

  }

  if( level == 0 && pub ) {
    FileOp.fmt( fDoc, "<p><a name=\"%s\"/><h3>%s %s</h3>\n", docTitle, indexStr, docTitle );
    FileOp.fmt( fDoc, "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\" width=\"100%%\">\n" );
    FileOp.fmt( fDoc, "<tr><th>Node</th><th>Parent node<br>Attribute/Constant</th><th>vType</th><th>Remark</th><th>Unit</th><th>Default</th><th>Range</th><th>Required</th></tr>\n" );
    FileOp.fmt( fIndex, "<li><a href=\"%s-%s.html#%s\"><big><b>%s</b></big> - w%s</a>\n", docname, lang, docTitle, docTitle, wrpName );
  }
  else if( pub )
    FileOp.fmt( fIndex, "<li><a href=\"%s-%s.html#%s\">%s - w%s</a>\n", docname, lang, docTitle, docTitle, wrpName );

  if( pub ) { /* Processing attributes. */
    int i = 0;
    FileOp.fmt( fIndex, "<ol>\n" );
    if( !existing )
      FileOp.fmt( fDoc, "<tr bgcolor=\"#e8e8e8\"><td><a name=\"%s\"/><small>%s%s</small> <b>%s</b> <small>(%s)%s</small></td><td>%s%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
                  docName, indexStr, level == 0?"0":"",
                  xmlName,NodeOp.getStr( child, "cardinality", "1" ),
                  extWrapper ? "(extern)":"",
                  level==0?"":"childnode of ",
                  level==0?"*root*":parent,
                  "",
                  __getRemark(child),
                  "",
                  "",
                  "",
                  NodeOp.getBool( child, "required", False )?"Yes":"No"
                 );
    else
      FileOp.fmt( fDoc, "<tr bgcolor=\"#e8e8e8\"><td><a href=\"#%s\"/><small>%s%s</small> <b>%s</b> <small>(%s)%s</small></td><td>%s%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
                  docName, indexStr, level == 0?"0":"",
                  xmlName,NodeOp.getStr( child, "cardinality", "1" ),
                  extWrapper ? "(extern)":"",
                  level==0?"":"childnode of ",
                  level==0?"*root*":parent,
                  "",
                  __getRemark(child),
                  "",
                  "",
                  "",
                  NodeOp.getBool( child, "required", False )?"Yes":"No"
                 );




    ListOp.sort( constList, comp );
    ListOp.sort( varList, comp );
    ListOp.sort( subnodeList, compNodeName );

    for( i = 0; !existing && i < ListOp.size( constList ); i++ ) {
      iONode var = (iONode)ListOp.get( constList, i );
      FileOp.fmt( fDoc, "<tr><td><i>%s</i></td><td align=\"right\"><i>%s%s%s</i></td><td><i>const %s</i></td><td><i>%s</i></td><td><i>%s</i></td><td><i>%s</i></td><td><i>%s</i></td><td><i>%s</i></td></tr>\n",
                  "",
                  NodeOp.getBool( var, "deprecated", False )?"<s>":"",
                  NodeOp.getStr( var, "name", "?" ),
                  NodeOp.getBool( var, "deprecated", False )?"</s>":"",
                  NodeOp.getStr( var, "vt", "?" ),
                  __getRemark(var),
                  NodeOp.getStr( var, "unit", "-" ),
                  NodeOp.getStr( var, "val", "-" ),
                  "-",
                  "-"
                 );
    }

    for( i = 0; !existing && i < ListOp.size( varList ); i++ ) {
      iONode var = (iONode)ListOp.get( varList, i );
      if( !NodeOp.getBool( var, "hidden", False ) ) {
        const char* sizeAttr = NodeOp.getStr( var, "size", NULL );
        FileOp.fmt( fDoc, "<tr><td>%s</td><td><tt>%s%s%s</tt></td><td>%s%s%s%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
                  "",
                  NodeOp.getBool( var, "deprecated", False )?"<s>":"",
                  NodeOp.getStr( var, "name", "?" ),
                  NodeOp.getBool( var, "deprecated", False )?"</s>":"",
                  NodeOp.getStr( var, "vt", "?" ),
                  sizeAttr ? "[":"",
                  sizeAttr ? sizeAttr:"",
                  sizeAttr ? "]":"",
                  __getRemark(var),
                  NodeOp.getStr( var, "unit", "-" ),
                  NodeOp.getStr( var, "defval", "-" ),
                  NodeOp.getStr( var, "range", "-" ),
                  NodeOp.getBool( var, "required", False )?"Yes":"No"
                 );
      }
    }

  }

  { /* Process childnodes: */
    int childnr = 1;
    int i = 0;
    for( i = 0; !existing && i < ListOp.size( subnodeList ); i++ ) {
      iONode var = (iONode)ListOp.get( subnodeList, i );
      if( NodeOp.getType( var ) == ELEMENT_NODE ) {
        char* l_indexStr = StrOp.fmt( "%s%d.", indexStr, childnr );
        if( __processChild( l_indexStr, var, fHdr, fImpl, fDoc, fIndex, level + 1, docName, modulename, docname, pub, docAll ) )
          childnr++;
        StrOp.free( l_indexStr );
      }
    }
  }

  if( pub )
    FileOp.fmt( fIndex, "</ol>\n" );

  if( level == 0 ) {
    FileOp.fmt( fDoc, "</table></p>\n" );
    FileOp.fmt( fIndex, "</li>\n" );
  }

  FileOp.flush( fHdr );

  /* Cleanup */
  ListOp.base.del( constList );
  ListOp.base.del( varList );
  ListOp.base.del( subnodeList );
  return pub;
}

static int __gConstHdr( iONode node, const char* fileName, Boolean docAll, const char* lang ) {
  int rc = 0;
  const char* modulename = NodeOp.getStr( node, "modulename", "?" );
  const char* docname = NodeOp.getStr( node, "docname", "wrapper" );

  char* docFileName = StrOp.fmt( "wrapper/doc/%s-%s.html", docname, lang );
  char* idxFileName = StrOp.fmt( "wrapper/doc/%s-%s-index.html", docname, lang );


  iOFile fHdr  = FileOp.inst( "wrapper/public/wrapper.h", OPEN_WRITE );
  iOFile fImpl = FileOp.inst( "wrapper/impl/wrapper.c", OPEN_WRITE );
  iOFile fDoc = FileOp.inst( docFileName, OPEN_WRITE );
  iOFile fIndex = FileOp.inst( idxFileName, OPEN_WRITE );

  nodeMap = MapOp.inst();

  TraceOp.println( "Generating wrapper.h" );
  __processPrefix( fHdr, fImpl, fDoc, fIndex, modulename, NodeOp.getStr( node, "title", "xconst" ), docname );

  {
    iOList childList = ListOp.inst();
    int childCnt = NodeOp.getChildCnt( node );
    int childIdx = 1;
    int i = 0;
    TraceOp.println( "Processing %d childs.", childCnt );
    for( i = 0; i < childCnt; i++ ) {
      iONode child = NodeOp.getChild( node, i );
      if( NodeOp.getType( child ) == ELEMENT_NODE )
        ListOp.add( childList, (obj)child );
    }
    ListOp.sort( childList, compNodeName );
    for( i = 0; i < ListOp.size( childList ); i++ ) {
      iONode child = (iONode)ListOp.get( childList, i );
      if( NodeOp.getType( child ) == ELEMENT_NODE ) {
        char* indexStr = StrOp.fmt( "%d.", childIdx );
        if( __processChild( indexStr, child, fHdr, fImpl, fDoc, fIndex, 0, fileName, modulename, docname, True, docAll ) )
          childIdx++;
        StrOp.free( indexStr );
      }
    }
  }

  __processSuffix( fHdr, fImpl, fDoc, fIndex );

  FileOp.base.del( fHdr );
  FileOp.base.del( fImpl );
  FileOp.base.del( fDoc );
  FileOp.base.del( fIndex );

  return rc;
}
