/** ------------------------------------------------------------
  * $Author: rob $
  * $Date: 2006/08/16 09:01:43 $
  * $Revision: 1.28 $
  * $Source: /home/cvs/xspooler/rocs/gen/ogen.c,v $
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
#include "rocs/public/strtok.h"
#include "rocs/public/list.h"
#include "rocs/public/map.h"
#include "rocs/public/thread.h"

static iOMap nodeMap = NULL;
static int __gConstHdr( iONode node, const char* fileName );
static Boolean forceSkeleton = False;
static const char* mountpoint = "./";

/** ------------------------------------------------------------
  * public main()
  *
  * @param  argc Number of commanline arguments.
  * @param  argv Commanline arguments.
  * @return      Applications exit code.
  */
int main( int argc, const char* argv[] ) {
  int rc = 0;

  iOTrace trc = TraceOp.inst( TRCLEVEL_INFO, "ogen", True );
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
        char*    objectdoc = StrOp.dup( "./doc");
        char* objectpublic = StrOp.dup( "./public");
        char*   objectimpl = StrOp.dup( "./impl");

        if( !FileOp.exist( objectdoc ) ) {
          TraceOp.println( "creating \"./doc\"..." );
          FileOp.mkdir( objectdoc );
        }
        if( !FileOp.exist( objectimpl ) ) {
          TraceOp.println( "creating \"./impl\"..." );
          FileOp.mkdir( objectimpl );
        }
        if( !FileOp.exist( objectpublic ) ) {
          TraceOp.println( "creating \"./public\"..." );
          FileOp.mkdir( objectpublic );
        }
      }

      if( argc > 2 ) {
        mountpoint = argv[2];
      }
      if( argc > 3 ) {
        forceSkeleton = StrOp.equalsi( "--force", argv[3] );
      }
      __gConstHdr( root, argv[1] );

    }

  }
  else {
    TraceOp.println( "Usage: ogen objects.xml --force" );
    TraceOp.println( "  \"--force\" Overwrites existing skeleton!" );
  }

  return rc;
}


static int __processPrefix( iOFile fDoc, iOFile fIndex, const char* modulename,
                            const char* title, const char* docname,
                            const char* revision, const char* source ) {
  int rc = 0;
  time_t clock ;
  time(&clock);




  TraceOp.println( "Processing Prefix D" );
  FileOp.fmt( fDoc, "<html>\n" );
  FileOp.fmt( fDoc, "<head>\n  <title>%s</title>\n", title );
  FileOp.fmt( fDoc, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n" );
  FileOp.fmt( fDoc, "</head>\n" );
  FileOp.fmt( fDoc, "<body>\n" );
  FileOp.fmt( fDoc, "<h2>%s</h2>\n", title );
  FileOp.fmt( fDoc, "XML: %s<br>\n", source );
  FileOp.fmt( fDoc, "XML: %s<br>\n", revision );
  FileOp.fmt( fDoc, "Module: %s<br>\n", modulename );
  FileOp.fmt( fDoc, "Auto generated at: %s<br>\n", ctime( &clock ) );
  FileOp.fmt( fDoc, "Index: <a href=\"%s-index.html\">%s-index.html</a><br>\n", docname, docname );

  FileOp.flush( fDoc );

  TraceOp.println( "Processing Prefix E" );
  FileOp.fmt( fIndex, "<html>\n" );
  FileOp.fmt( fIndex, "<head>\n  <title>Index for %s</title>\n</head>\n", title );
  FileOp.fmt( fIndex, "<body>\n" );
  FileOp.fmt( fIndex, "<h2>%s</h2>\n", title );
  FileOp.fmt( fIndex, "XML: %s<br>\n", source );
  FileOp.fmt( fIndex, "XML: %s<br>\n", revision );
  FileOp.fmt( fIndex, "Module: %s<br>\n", modulename );
  FileOp.fmt( fIndex, "Auto generated at: %s<br>\n", ctime( &clock ) );
  FileOp.fmt( fIndex, "<hr><h1>Index:</h1>\n", title );
  FileOp.fmt( fIndex, "<ol>\n" );

  FileOp.flush( fDoc );

  return rc;
}

static int __processSuffix( iOFile fDoc, iOFile fIndex ) {
  int rc = 0;
  TraceOp.println( "Processing Suffix" );
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



static void __wrpPrefix( Boolean isInterface, iOFile fPublH, iOFile fImplC, iOFile fImplM, iOFile fImplH,
                         const char* wrpName, const char* modulename, const char* use,
                         const char* incl, const char* impl_incl,
                         const char* revision, const char* source, Boolean toLower, Boolean noBase, const char* wrpFileName ) {
  time_t clock ;
  time(&clock);

  FileOp.fmt( fPublH, "/** ------------------------------------------------------------\n" );
  FileOp.fmt( fPublH, "  * A U T O   G E N E R A T E D\n" );
  FileOp.fmt( fPublH, "  * Generator: Rocs ogen (build %s %s)\n", __DATE__, __TIME__ );
  FileOp.fmt( fPublH, "  * Module: %s\n", modulename );
  FileOp.fmt( fPublH, "  * XML: %s\n", source );
  FileOp.fmt( fPublH, "  * XML: %s\n", revision );
  FileOp.fmt( fPublH, "  * Date: %s", ctime( &clock ) );
  FileOp.fmt( fPublH, "  */\n\n" );
  FileOp.fmt( fPublH, "#ifndef __object_%s_H\n", wrpName );
  FileOp.fmt( fPublH, "#define __object_%s_H\n\n", wrpName );
  FileOp.fmt( fPublH, "#include \"rocs/public/rocs.h\"\n" );
  FileOp.fmt( fPublH, "#include \"rocs/public/objbase.h\"\n" );

  if( use ) {
    iOStrTok strTok = StrTokOp.inst( use, ',' );
    FileOp.fmt( fPublH, "\n/* Rocs includes: */\n" );
    while( StrTokOp.hasMoreTokens( strTok ) ) {
      const char* tok = StrTokOp.nextToken( strTok );
      if( tok[0] == '#' )
        FileOp.fmt( fPublH, "#include \"%s/public/%s.h\"\n", modulename, tok+1 );
      else if( tok[0] == '$' )
        FileOp.fmt( fPublH, "#include \"%s.h\"\n", tok+1 );
      else if( tok[0] == '&' )
        FileOp.fmt( fPublH, "#include <%s.h>\n", tok+1 );
      else
        FileOp.fmt( fPublH, "#include \"rocs/public/%s.h\"\n", tok );
    };
  }

  if( incl ) {
    iOStrTok strTok = StrTokOp.inst( incl, ',' );
    FileOp.fmt( fPublH, "\n/* %s and system includes: */\n", modulename );
    while( StrTokOp.hasMoreTokens( strTok ) ) {
      const char* tok = StrTokOp.nextToken( strTok );
      if( tok[0] == '#' ) {
        FileOp.fmt( fPublH, "#include <%s.h>\n", tok+1 );
      }
      else if( tok[0] == '$' ) {
        FileOp.fmt( fPublH, "#include \"%s.h\"\n", tok+1 );
      }
      else {
        char* inclstr = StrOp.fmt( "%s/public/%s.h", modulename, tok );
        FileOp.fmt( fPublH, "#include \"%s\"\n", toLower?StrOp.strlwr(inclstr):inclstr );
        StrOp.free(inclstr);
      }
    };
  }

  FileOp.fmt( fPublH, "\n#ifdef __cplusplus\n" );
  FileOp.fmt( fPublH, "  extern \"C\" {\n" );
  FileOp.fmt( fPublH, "#endif\n" );
  FileOp.fmt( fPublH, "\n\n" );

  /* Insert typedefs here... */



  if(!isInterface) {

    if( fImplH ) {
      char* inclstr = NULL;
      FileOp.fmt( fImplH, "/** ------------------------------------------------------------\n" );
      FileOp.fmt( fImplH, "  * A U T O   G E N E R A T E D\n" );
      FileOp.fmt( fImplH, "  * Generator: Rocs ogen (build %s %s)\n", __DATE__, __TIME__ );
      FileOp.fmt( fImplH, "  * Module: %s\n", modulename );
      FileOp.fmt( fImplH, "  * XML: %s\n", source );
      FileOp.fmt( fImplH, "  * XML: %s\n", revision );
      FileOp.fmt( fImplH, "  * Date: %s", ctime( &clock ) );
      FileOp.fmt( fImplH, "  */\n\n" );
      inclstr = StrOp.fmt( "%s/public/%s.h", modulename, wrpFileName );
      FileOp.fmt( fImplH, "#include \"%s\"\n\n", toLower?StrOp.strlwr(inclstr):inclstr );
      StrOp.free(inclstr);

      if( impl_incl ) {
        iOStrTok strTok = StrTokOp.inst( impl_incl, ',' );
        FileOp.fmt( fImplH, "/* %s and system includes: */\n", modulename );
        while( StrTokOp.hasMoreTokens( strTok ) ) {
          const char* tok = StrTokOp.nextToken( strTok );
          if( tok[0] == '#' ) {
            FileOp.fmt( fImplH, "#include <%s.h>\n", tok+1 );
          }
          else if( tok[0] == '$' ) {
            FileOp.fmt( fImplH, "#include \"%s.h\"\n", tok+1 );
          }
          else {
            char* inclstr = StrOp.fmt( "%s/public/%s.h", modulename, tok );
            FileOp.fmt( fImplH, "#include \"%s\"\n", toLower?StrOp.strlwr(inclstr):inclstr );
            StrOp.free(inclstr);
          }
        };
      }

      FileOp.fmt( fImplH, "\nstatic const char* name = \"O%s\";\n\n", wrpName );
    }
    FileOp.fmt( fImplM, "/** ------------------------------------------------------------\n" );
    FileOp.fmt( fImplM, "  * A U T O   G E N E R A T E D\n" );
    FileOp.fmt( fImplM, "  * Generator: Rocs ogen (build %s %s)\n", __DATE__, __TIME__ );
    FileOp.fmt( fImplM, "  * Module: %s\n", modulename );
    FileOp.fmt( fImplM, "  * XML: %s\n", source );
    FileOp.fmt( fImplM, "  * XML: %s\n", revision );
    FileOp.fmt( fImplM, "  * Date: %s", ctime( &clock ) );
    FileOp.fmt( fImplM, "  */\n\n" );
    FileOp.fmt( fImplM, "struct O%s %sOp = {\n", wrpName, wrpName );

    if( !noBase ) {
      FileOp.fmt( fImplM, "  {\n" );
      FileOp.fmt( fImplM, "    NULL,\n" );
      FileOp.fmt( fImplM, "    __del,\n" );
      FileOp.fmt( fImplM, "    __name,\n" );
      FileOp.fmt( fImplM, "    __serialize,\n" );
      FileOp.fmt( fImplM, "    __deserialize,\n" );
      FileOp.fmt( fImplM, "    __toString,\n" );
      FileOp.fmt( fImplM, "    __count,\n" );
      FileOp.fmt( fImplM, "    __clone,\n" );
      FileOp.fmt( fImplM, "    __equals,\n" );
      FileOp.fmt( fImplM, "    __properties,\n" );
      FileOp.fmt( fImplM, "    __id,\n" );
      FileOp.fmt( fImplM, "    __event,\n" );
      FileOp.fmt( fImplM, "  },\n" );
    }

    if( fImplC ) {
      char* inclstr = NULL;
      FileOp.fmt( fImplC, "/** ------------------------------------------------------------\n" );
      FileOp.fmt( fImplC, "  * A U T O   G E N E R A T E D  (First time only!)\n" );
      FileOp.fmt( fImplC, "  * Generator: Rocs ogen (build %s %s)\n", __DATE__, __TIME__ );
      FileOp.fmt( fImplC, "  * Module: %s\n", modulename );
      FileOp.fmt( fImplC, "  * XML: %s\n", source );
      FileOp.fmt( fImplC, "  * XML: %s\n", revision );
      FileOp.fmt( fImplC, "  * Object: %s\n", wrpName );
      FileOp.fmt( fImplC, "  * Date: %s", ctime( &clock ) );
      FileOp.fmt( fImplC, "  * ------------------------------------------------------------\n" );
      FileOp.fmt( fImplC, "  * %cSource%c\n", '$', '$' );
      FileOp.fmt( fImplC, "  * %cAuthor%c\n", '$', '$' );
      FileOp.fmt( fImplC, "  * %cDate%c\n", '$', '$' );
      FileOp.fmt( fImplC, "  * %cRevision%c\n", '$', '$' );
      FileOp.fmt( fImplC, "  * %cName%c\n", '$', '$' );
      FileOp.fmt( fImplC, "  */\n\n" );
      inclstr = StrOp.fmt( "%s/impl/%s_impl.h", modulename, wrpName );
      FileOp.fmt( fImplC, "#include \"%s\"\n\n", toLower?StrOp.strlwr(inclstr):inclstr );
      StrOp.free( inclstr );
      FileOp.fmt( fImplC, "#include \"rocs/public/mem.h\"\n\n" );

      FileOp.fmt( fImplC, "static int instCnt = 0;\n\n" );

      if( !noBase ) {
        FileOp.fmt( fImplC, "/** ----- OBase ----- */\n" );
        FileOp.fmt( fImplC, "static void __del( void* inst ) {\n" );
        FileOp.fmt( fImplC, "  if( inst != NULL ) {\n" );
        FileOp.fmt( fImplC, "    iO%sData data = Data(inst);\n", wrpName );
        FileOp.fmt( fImplC, "    /* Cleanup data->xxx members...*/\n" );
        FileOp.fmt( fImplC, "    \n" );
        FileOp.fmt( fImplC, "    freeMem( data );\n" );
        FileOp.fmt( fImplC, "    freeMem( inst );\n" );
        FileOp.fmt( fImplC, "    instCnt--;\n" );
        FileOp.fmt( fImplC, "  }\n" );
        FileOp.fmt( fImplC, "  return;\n" );
        FileOp.fmt( fImplC, "}\n\n" );
        FileOp.fmt( fImplC, "static const char* __name( void ) {\n" );
        FileOp.fmt( fImplC, "  return name;\n" );
        FileOp.fmt( fImplC, "}\n\n" );
        FileOp.fmt( fImplC, "static unsigned char* __serialize( void* inst, long* size ) {\n" );
        FileOp.fmt( fImplC, "  return NULL;\n" );
        FileOp.fmt( fImplC, "}\n\n" );
        FileOp.fmt( fImplC, "static void __deserialize( void* inst,unsigned char* bytestream ) {\n" );
        FileOp.fmt( fImplC, "  return;\n" );
        FileOp.fmt( fImplC, "}\n\n" );
        FileOp.fmt( fImplC, "static char* __toString( void* inst ) {\n" );
        FileOp.fmt( fImplC, "  return NULL;\n" );
        FileOp.fmt( fImplC, "}\n\n" );
        FileOp.fmt( fImplC, "static int __count( void ) {\n" );
        FileOp.fmt( fImplC, "  return instCnt;\n" );
        FileOp.fmt( fImplC, "}\n\n" );
        FileOp.fmt( fImplC, "static struct OBase* __clone( void* inst ) {\n" );
        FileOp.fmt( fImplC, "  return NULL;\n" );
        FileOp.fmt( fImplC, "}\n\n" );
        FileOp.fmt( fImplC, "static Boolean __equals( void* inst1, void* inst2 ) {\n" );
        FileOp.fmt( fImplC, "  return False;\n" );
        FileOp.fmt( fImplC, "}\n\n" );
        FileOp.fmt( fImplC, "static void* __properties( void* inst ) {\n" );
        FileOp.fmt( fImplC, "  return NULL;\n" );
        FileOp.fmt( fImplC, "}\n\n" );
        FileOp.fmt( fImplC, "static const char* __id( void* inst ) {\n" );
        FileOp.fmt( fImplC, "  return NULL;\n" );
        FileOp.fmt( fImplC, "}\n\n" );
        FileOp.fmt( fImplC, "static void* __event( void* inst, const void* evt ) {\n" );
        FileOp.fmt( fImplC, "  return NULL;\n" );
        FileOp.fmt( fImplC, "}\n\n" );
      }
      FileOp.fmt( fImplC, "/** ----- O%s ----- */\n", wrpName );
    }
  }

}


static void __wrpSuffix( Boolean isInterface, iOFile fPublH, iOFile fImplC, iOFile fImplM,
                         iOFile fImplH, const char* wrpName, const char* modulename, Boolean toLower ) {
  FileOp.fmt( fPublH, "} *i%c%s;\n", isInterface?'I':'O', wrpName );
  FileOp.fmt( fPublH, "\n" );
  if( !isInterface )
    FileOp.fmt( fPublH, "extern struct O%s %sOp;\n", wrpName, wrpName );
  FileOp.fmt( fPublH, "\n" );
  FileOp.fmt( fPublH, "#ifdef __cplusplus\n" );
  FileOp.fmt( fPublH, "  }\n" );
  FileOp.fmt( fPublH, "#endif\n" );
  FileOp.fmt( fPublH, "\n\n#endif\n", wrpName );

  if(!isInterface) {
    FileOp.fmt( fImplM, "};\n" );
    if( fImplC ) {
      char* inclstr = StrOp.fmt( "%s/impl/%s.fm", modulename, wrpName );
      FileOp.fmt( fImplC, "\n\n/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/\n" );
      FileOp.fmt( fImplC, "#include \"%s\"\n", toLower?StrOp.strlwr(inclstr):inclstr );
      FileOp.fmt( fImplC, "/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/\n" );
      StrOp.free( inclstr );
    }
  }
}


static void __addData( Boolean isInterface, iONode data, iOFile fPublH, iOFile fImplC, iOFile fImplM, iOFile fImplH, const char* wrpName, const char* modulename ) {
  int i = 0;
  int cnt = NodeOp.getChildCnt( data );
  int varCnt = 0;

  if(isInterface || fImplH == NULL )
    return;

  FileOp.fmt( fImplH, "typedef struct O%sData {\n\n", wrpName );

  for( i = 0; i < cnt; i++ ) {
    iONode var = NodeOp.getChild( data, i );
    if( StrOp.equalsi( "var", NodeOp.getName( var ) ) ) {
      FileOp.fmt( fImplH, "    /** %s */\n", NodeOp.getStr( var, "remark", "" ) );
      FileOp.fmt( fImplH, "  %s %s;\n",
                  NodeOp.getStr( var, "vt", "?" ),
                  NodeOp.getStr( var, "name", "?" )
      );
    }
  } /* End for(i) */

  FileOp.fmt( fImplH, "\n} *iO%sData;\n\n", wrpName );
  FileOp.fmt( fImplH, "static iO%sData Data( void* p ) { return (iO%sData)((iO%s)p)->base.data; }\n\n", wrpName, wrpName, wrpName );
}


static void __addDef( Boolean isInterface, iONode data, iOFile fPublH, iOFile fImplC, iOFile fImplM, iOFile fImplH, const char* wrpName, const char* modulename ) {
  if(isInterface || fImplH == NULL )
    return;

  FileOp.fmt( fImplH, "/* %s */\n", NodeOp.getStr( data, "remark", "" ) );
  if( StrOp.equals( "string", NodeOp.getStr( data, "vt", "int" ) ) )
    FileOp.fmt( fImplH, "#define %s \"%s\"\n",
                NodeOp.getStr( data, "name", "?" ),
                NodeOp.getStr( data, "val", "?" )
                );
  else
    FileOp.fmt( fImplH, "#define %s %s\n",
                NodeOp.getStr( data, "name", "?" ),
                NodeOp.getStr( data, "val", "?" )
                );
}


static void __addStruct( Boolean isInterface, iONode data, iOFile fPublH, iOFile fImplC, iOFile fImplM, iOFile fImplH, const char* wrpName, const char* modulename ) {
  iOFile hdr = fImplH;

  if(isInterface || fImplH == NULL || fPublH == NULL )
    return;

  if( NodeOp.getBool( data, "public", False ) ) {
    hdr = fPublH;
  }

  if( StrOp.equalsi( "struct", NodeOp.getName( data ) ) ) {
    int memberCnt = NodeOp.getChildCnt( data );
    int n = 0;
    FileOp.fmt( hdr, "\n/** %s */\n", NodeOp.getStr( data, "remark", "" ) );
    FileOp.fmt( hdr, "%s struct %s {\n", NodeOp.getStr( data, "typedef", NULL )!=NULL?"typedef":"", NodeOp.getStr( data, "name", "" ) );
    for( n = 0; n < memberCnt; n++ ) {
      iONode mem = NodeOp.getChild( data, n );
      if( StrOp.equalsi( "var", NodeOp.getName( mem ) ) ) {
        FileOp.fmt( hdr, "    /** %s */\n", NodeOp.getStr( mem, "remark", "" ) );
        FileOp.fmt( hdr, "  %s %s;\n",
                    NodeOp.getStr( mem, "vt", "?" ),
                    NodeOp.getStr( mem, "name", "?" )
        );
      }
    } /* End for(n) */
    FileOp.fmt( hdr, "} %s;\n", NodeOp.getStr( data, "typedef", "" ) );

  }

}


static void __addFun( Boolean isInterface, iONode fun, iOFile fPublH, iOFile fImplC, iOFile fImplM, iOFile fImplH,
                      const char* wrpName, const char* modulename, const char* interfacename ) {

  Boolean isMain = StrOp.equals( "main", NodeOp.getStr( fun, "name", "?" ) );
  Boolean isInst = StrOp.equals( "inst", NodeOp.getStr( fun, "name", "?" ) );

  char* vt = StrOp.dup( NodeOp.getStr( fun, "vt", "void" ) );
  const char* implname = NodeOp.getStr( fun, "implname", NULL );

  if( StrOp.equalsi( "this", vt ) && isInterface )
    vt = StrOp.fmt( "struct I%s*",  wrpName );
  else if( StrOp.equalsi( "this", vt ) && interfacename == NULL )
    vt = StrOp.fmt( "struct O%s*",  wrpName );
  else if( StrOp.equalsi( "this", vt ) && interfacename != NULL )
    vt = StrOp.fmt( "iI%s", interfacename );

  FileOp.fmt( fPublH, "  /** %s */\n", NodeOp.getStr( fun, "remark", "" ) );
  FileOp.fmt( fPublH, "  %s (*%s)(", vt, NodeOp.getStr( fun, "name", "?" ) );

  if( fImplC && !isInterface ) {
    FileOp.fmt( fImplC, "\n\n/** %s */\n", NodeOp.getStr( fun, "remark", "" ) );
    if( isMain )
      FileOp.fmt( fImplC, "%s %s(", vt, NodeOp.getStr( fun, "name", "?" ) );
    else
      FileOp.fmt( fImplC, "static %s _%s(", vt, NodeOp.getStr( fun, "name", "?" ) );
  }


  if( !isInterface ) {
    if( isMain )
      FileOp.fmt( fImplM, "  %s,\n", NodeOp.getStr( fun, "name", "?" ) );
    else {
      if( implname != NULL )
        FileOp.fmt( fImplM, "  %s,\n", implname );
      else
        FileOp.fmt( fImplM, "  _%s,\n", NodeOp.getStr( fun, "name", "?" ) );
    }
  }

  {
    int i = 0;
    int cnt = NodeOp.getChildCnt( fun );
    int funCnt = 0;
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( fun, i );
      char* l_vt = StrOp.dup( NodeOp.getStr( child, "vt", "void" ) );

      if( NodeOp.getType( child ) != ELEMENT_NODE )
        continue;

      if( StrOp.equalsi( "this", l_vt ) && isInterface )
        l_vt = StrOp.fmt( "struct I%s*",  wrpName );
      else if( StrOp.equalsi( "this", l_vt ) && interfacename == NULL )
        l_vt = StrOp.fmt( "struct O%s*",  wrpName );
      else if( StrOp.equalsi( "this", l_vt ) && interfacename != NULL )
        l_vt = StrOp.fmt( "iI%s",  interfacename );

      if( StrOp.equalsi( "param", NodeOp.getName( child ) ) ) {
        FileOp.fmt( fPublH, "%s%s %s ", i==0?" ":",",
                    l_vt,
                    NodeOp.getStr( child, "name", "?" )
        );
      if( fImplC && !isInterface )
        FileOp.fmt( fImplC, "%s%s %s ", i==0?" ":",",
                    l_vt,
                    NodeOp.getStr( child, "name", "?" )
        );

        funCnt++;
      }
      StrOp.free( l_vt );
    }
    if( funCnt == 0 ) {
      FileOp.fmt( fPublH, " void " );
      if( fImplC && !isInterface )
        FileOp.fmt( fImplC, " void " );
    }
  }
  FileOp.fmt( fPublH, ");\n" );

  if( fImplC && isInst && !isInterface ) {
    FileOp.fmt( fImplC, ") {\n" );
    FileOp.fmt( fImplC, "  iO%s __%s = allocMem( sizeof( struct O%s ) );\n", wrpName, wrpName, wrpName );
    FileOp.fmt( fImplC, "  iO%sData data = allocMem( sizeof( struct O%sData ) );\n", wrpName, wrpName );
    FileOp.fmt( fImplC, "  MemOp.basecpy( __%s, &%sOp, 0, sizeof( struct O%s ), data );\n\n", wrpName, wrpName, wrpName );

    FileOp.fmt( fImplC, "  /* Initialize data->xxx members... */\n\n" );

    FileOp.fmt( fImplC, "  instCnt++;\n" );
    FileOp.fmt( fImplC, "  return __%s;\n", wrpName );
    FileOp.fmt( fImplC, "}\n" );
  }
  else if( fImplC && !isInterface ) {
    FileOp.fmt( fImplC, ") {\n" );
    if( StrOp.equalsi( "void", vt ) )
      FileOp.fmt( fImplC, "  return;\n" );
    else
      FileOp.fmt( fImplC, "  return 0;\n" );
    FileOp.fmt( fImplC, "}\n" );
  }

  StrOp.free(vt);
}


static void __addConst( Boolean isInterface, iONode var, iOFile fPublH, iOFile fImplC, iOFile fImplM, iOFile fImplH, const char* wrpName, const char* modulename ) {
  char* vt = "const char*";
  char* val = NULL;
  const char* attrVal  = NodeOp.getStr( var, "val", "" );
  const char* attrName = NodeOp.getStr( var, "name", "?" );
  if( StrOp.equalsi( "int", NodeOp.getStr( var, "vt", "string" ) ) ) {
    vt = "int";
    val = StrOp.dup(attrVal);
  }
  else if( StrOp.equalsi( "long", NodeOp.getStr( var, "vt", "string" ) ) ) {
    vt = "long";
    val = StrOp.dup(attrVal);
  }
  else if( StrOp.equalsi( "float", NodeOp.getStr( var, "vt", "string" ) ) ) {
    vt = "double";
    val = StrOp.dup(attrVal);
  }
  else if( StrOp.equalsi( "bool", NodeOp.getStr( var, "vt", "string" ) ) ) {
    vt = "Boolean";
    val = StrOp.dup( StrOp.equalsi( "true", attrVal ) ? "True":"False" );
  }
  else {
    if( attrVal[0] == '$' ) {
      val = StrOp.fmt( "%s", attrVal+1 );
    }
    else
      val = StrOp.fmt( "\"%s\"", attrVal );
  }

  FileOp.fmt( fPublH, "  %s %s;\n", vt, attrName );
  FileOp.fmt( fImplM, "  %s,\n", val );
}


static iONode __getExternalInterface( const char* path, const char* iname, const char* mapname, iOMap iMap ) {
  iOFile constXml = FileOp.inst( path, OPEN_READONLY );
  iONode interface = NULL;

  if( constXml != NULL ) {
    char* xmlStr = allocMem( FileOp.size( constXml ) + 1 );
    iODoc doc = NULL;
    iONode root = NULL;

    TraceOp.println( "Reading %s...", path );
    FileOp.read( constXml, xmlStr, FileOp.size( constXml ) );
    FileOp.close( constXml );
    FileOp.base.del( constXml );

    TraceOp.println( "Parsing %s...", path );
    doc = DocOp.parse( xmlStr );
    freeMem( xmlStr );
    root = DocOp.getRootNode( doc );
    if( root != NULL ) {
      interface = NodeOp.findNode( root, "interface" );
      while( interface != NULL ) {
        const char* name = NodeOp.getStr( interface, "name", "" );
        if( StrOp.equalsi( name, iname ) ) {
          interface = (iONode)interface->base.clone( interface );
          /* ToDo: put in map! */
          MapOp.put( iMap, mapname, (obj)interface );
          break;
        }
        interface = NodeOp.findNextNode( root, interface );
      };
    }
    DocOp.base.del( doc );
    NodeOp.base.del( root );
  }

  return interface;
}


static int comp( obj* o1, obj* o2 ) {
  iONode node1 = (iONode)*o1;
  iONode node2 = (iONode)*o2;
  return strcmp( NodeOp.getStr( node1, "name", "" ), NodeOp.getStr( node2, "name", "" ) );
}

static void __wrpCreate( iONode node, const char* modulename, const char* revision,
                         const char* source, Boolean toLower ) {
  const char*   wrpName = NodeOp.getStr( node, "name"  , NodeOp.getName( node ) );
  const char*   wrpFileName = NodeOp.getStr( node, "filename"  , wrpName );
  const char*   interfaceName = NodeOp.getStr( node, "interface"  , NULL );
  const char* nodeName = NodeOp.getName( node );
  Boolean isInterface = StrOp.equalsi( "interface", nodeName );
  Boolean createSkeleton = False;
  Boolean noBase = NodeOp.getBool( node, "nobase", False );

  char* wrpPublH = StrOp.fmt( "public/%s.h", wrpFileName );
  char* wrpImplC = StrOp.fmt( "impl/%s.c", wrpFileName );
  char* wrpImplM = StrOp.fmt( "impl/%s.fm", wrpFileName );
  char* wrpImplH = StrOp.fmt( "impl/%s_impl.h", wrpFileName );

  iOFile fPublH = FileOp.inst( toLower?StrOp.strlwr(wrpPublH):wrpPublH, OPEN_WRITE );
  iOFile fImplC = NULL;
  iOFile fImplM = isInterface?NULL:FileOp.inst( toLower?StrOp.strlwr(wrpImplM):wrpImplM, OPEN_WRITE );
  iOFile fImplH = isInterface?NULL:FileOp.inst( toLower?StrOp.strlwr(wrpImplH):wrpImplH, OPEN_WRITE );

  iOList funList = ListOp.inst();
  iONode data = NULL;

  static iOMap iMap = NULL;

  if( iMap == NULL ) iMap = MapOp.inst();

  if( isInterface ) {
    MapOp.put( iMap, wrpName, (obj)node );
  }


  if( !FileOp.exist( toLower?StrOp.strlwr(wrpImplC):wrpImplC ) || forceSkeleton ) {
    TraceOp.println( "generating skeleton for \"%s\" as \"%s\"", wrpName, wrpImplC );
    if( !isInterface ) {
      createSkeleton = True;
      fImplC = FileOp.inst( toLower?StrOp.strlwr(wrpImplC):wrpImplC, OPEN_WRITE );
    }
  }
  else
    TraceOp.println( "skeleton for \"%s\" already generated as \"%s\"", wrpName, wrpImplC );

  {
    int childCnt = NodeOp.getChildCnt( node );
    int i = 0;
    int cnt = NodeOp.getChildCnt( node );
    iONode dataNode = NodeOp.findNode( node, "data" );
    const char* impl_include = dataNode != NULL ? NodeOp.getStr( dataNode, "include", NULL ):NULL;

    __wrpPrefix( isInterface, fPublH, fImplC, fImplM, fImplH, wrpName, modulename,
                 NodeOp.getStr( node, "use", NULL ), NodeOp.getStr( node, "include", NULL ),
                 impl_include,
                 revision, source, toLower, noBase, wrpFileName );

    /* Process typedefs first. */
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( node, i );
      if( StrOp.equalsi( "typedef", NodeOp.getName( child ) ) ) {
        const char* def = NodeOp.getStr( child, "def", "" );
        Boolean implH = NodeOp.getBool( child, "implh", False );
        FileOp.fmt( implH?fImplH:fPublH, "typedef %s;\n", def );
      }
    }

    /* Process macro's. */
    FileOp.fmt( fPublH, "\n" );
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( node, i );
      if( StrOp.equalsi( "macro", NodeOp.getName( child ) ) ) {
        const char* def = NodeOp.getStr( child, "def", "" );
        FileOp.fmt( fPublH, "#define %s\n", def );
      }
    }

    /* Process public struct's. */
    FileOp.fmt( fPublH, "\n" );
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( node, i );
      if( StrOp.equalsi( "struct", NodeOp.getName( child ) ) &&
          NodeOp.getBool( child, "public", False ) ) {
        __addStruct( isInterface, child, fPublH, fImplC, fImplM, fImplH, wrpName, modulename );
      }
    }

    /* Start the object structure... */
    FileOp.fmt( fPublH, "\ntypedef struct %c%s {\n", isInterface?'I':'O', wrpName );
    if( !noBase ) {
      FileOp.fmt( fPublH, "  /***** Base *****/\n" );
      FileOp.fmt( fPublH, "  struct OBase  base;\n" );
    }


    if( interfaceName ) {
      iONode interface = NULL;
      if( interfaceName[0] == '$' ) {
        /* External interface: $package/xmlfile:interfacename */
        interface = (iONode)MapOp.get( iMap, interfaceName );
        if( interface == NULL ) {
          char* path  = StrOp.fmt( "%s%s", mountpoint, interfaceName+1 );
          char* iname = StrOp.findc( path, ':' );
          if( iname != NULL ) {
            *iname = '\0';
            iname++;
            interface = __getExternalInterface( path, iname, interfaceName, iMap );
          }
          else {
            TraceOp.println( "*** No valid interface name: \"%s\"", interfaceName );
          }
        }
        if( interface != NULL )
          interfaceName = NodeOp.getStr( interface, "name", NULL );
      }
      else
        interface = (iONode)MapOp.get( iMap, interfaceName );

      if( interface ) {
        int i = 0;
        int cnt = NodeOp.getChildCnt( interface );
        TraceOp.println( "Using interface: \"%s\"", interfaceName );
        FileOp.fmt( fPublH, "\n  /***** Interface: %s *****/\n", interfaceName );
        for( i = 0; i < cnt; i++ ) {
          iONode child = NodeOp.getChild( interface, i );
          if( StrOp.equalsi( "fun", NodeOp.getName( child ) ) ) {
            ListOp.add( funList, (obj)child );
            /*__addFun( isInterface, child, fPublH, fImplC, fImplM, fImplH, wrpName, modulename, interfaceName );*/
          }
        }
        ListOp.sort( funList, comp );
        for( i = 0; i < ListOp.size( funList ); i++ ) {
          iONode fun = (iONode)ListOp.get( funList, i );
          __addFun( isInterface, fun, fPublH, fImplC, fImplM, fImplH, wrpName, modulename, interfaceName );
        }
        ListOp.base.del(funList);
        funList = ListOp.inst();
      }
    }


    FileOp.fmt( fPublH, "\n  /***** Object: %s *****/\n", wrpName );
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( node, i );
      if( StrOp.equalsi( "fun", NodeOp.getName( child ) ) ) {
        ListOp.add( funList, (obj)child );
        /*__addFun( isInterface, child, fPublH, fImplC, fImplM, fImplH, wrpName, modulename, NULL );*/
      }
      else if( StrOp.equalsi( "data", NodeOp.getName( child ) ) && data == NULL )
        __addData( isInterface, child, fPublH, fImplC, fImplM, fImplH, wrpName, modulename );
      else if( StrOp.equalsi( "def", NodeOp.getName( child ) ) && data == NULL )
        __addDef( isInterface, child, fPublH, fImplC, fImplM, fImplH, wrpName, modulename );
      else if( StrOp.equalsi( "struct", NodeOp.getName( child ) ) && data == NULL && !NodeOp.getBool( child, "public", False ) )
        __addStruct( isInterface, child, fPublH, fImplC, fImplM, fImplH, wrpName, modulename );
      else if( StrOp.equalsi( "const", NodeOp.getName( child ) ) && data == NULL )
        __addConst( isInterface, child, fPublH, fImplC, fImplM, fImplH, wrpName, modulename );
    }
    ListOp.sort( funList, comp );
    for( i = 0; i < ListOp.size( funList ); i++ ) {
      iONode child = (iONode)ListOp.get( funList, i );
      __addFun( isInterface, child, fPublH, fImplC, fImplM, fImplH, wrpName, modulename, NULL );
    }


    __wrpSuffix( isInterface, fPublH, fImplC, fImplM, fImplH, wrpName, modulename, toLower );
  }

  funList->base.del( funList );

  FileOp.base.del( fPublH );

  if( !isInterface ) {
    FileOp.base.del( fImplC );
    FileOp.base.del( fImplM );
    FileOp.base.del( fImplH );
  }

}



static int __processChild( char* indexStr, iONode child, iOFile fDoc, iOFile fIndex, int level,
                           const char* parent, const char* modulename, const char* docname,
                           const char* revision, const char* source, Boolean toLower ) {
  int rc = 0;
  const char* nodeName = NodeOp.getName( child );
  const char* objectName = NodeOp.getStr( child, "name", nodeName );
  Boolean isInterface = StrOp.equals( "interface", nodeName );
  TraceOp.println( "generating child \"%s\"", nodeName );

  if( NodeOp.getBool( child, "createobject", True ) )
    __wrpCreate( child, modulename, revision, source, toLower );

  if( level == 0 ) {
    if( isInterface ) {
      FileOp.fmt( fDoc, "<p><a name=\"%s\"/><h3>%s <i>%s %s</i></h3>\n", objectName, indexStr, nodeName, objectName );
      FileOp.fmt( fDoc, "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\" width=\"100%%\">\n" );
      FileOp.fmt( fIndex, "<li><a href=\"%s.html#%s\"><big><b><i>%s</i></b></big></a>\n", docname, objectName, objectName );
    }
    else {
      FileOp.fmt( fDoc, "<p><a name=\"%s\"/><h3>%s %s %s</h3>\n", objectName, indexStr, nodeName, objectName );
      FileOp.fmt( fDoc, "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\" width=\"100%%\">\n" );
      FileOp.fmt( fIndex, "<li><a href=\"%s.html#%s\"><big><b>%s</b></big></a>\n", docname, objectName, objectName );
    }
    if( NodeOp.getStr( child, "implements", NULL ) )
      FileOp.fmt( fDoc, "<b>implements: %s</b><br>\n", NodeOp.getStr( child, "implements", "" ) );
    if( NodeOp.getStr( child, "remark", NULL ) )
      FileOp.fmt( fDoc, "%s<br>\n", NodeOp.getStr( child, "remark", "" ) );
  }
  else
    FileOp.fmt( fIndex, "<li><a href=\"%s.html#%s\">%s</a>\n", docname, objectName, objectName );

  { /* Processing attributes. */
    int childCnt = NodeOp.getChildCnt( child );
    int i = 0;
    int fun = 0;
    int childnr = 1;
    iOList funList = ListOp.inst();
    const char* implements = NodeOp.getStr( child, "interface", NULL );

    FileOp.fmt( fIndex, "<ol>\n" );
    TraceOp.println( "Processing %d childs.", childCnt );
    FileOp.fmt( fDoc, "<tr bgcolor=\"#e8e8e8\"><th colspan=\"2\" align=\"left\"><a name=\"%s\"/><tt>Summary of %s %s%s<a href=\"#%s\">%s</a></tt></th></tr>\n",
                objectName, isInterface?"interface":"object",
                objectName,
                implements?", implementing ":"", implements?implements:"", implements?implements:""
                 );

    for( i = 0; i < childCnt; i++ ) {
      iONode var = NodeOp.getChild( child, i );
      if( StrOp.equals( "typedef", NodeOp.getName( var ) ) ) {
        FileOp.fmt( fDoc, "<tr><td valign=\"top\"><tt><i>typedef</i></tt></td><td><tt><b>%s</b></tt><br>%s</td></tr>\n",
                    NodeOp.getStr( var, "def", "" ), NodeOp.getStr( var, "remark", "" ) );
      }
      else if( StrOp.equals( "macro", NodeOp.getName( var ) ) ) {
        FileOp.fmt( fDoc, "<tr><td valign=\"top\"><tt><i>macro</i></tt></td><td><tt><b>%s</b></tt><br>%s</td></tr>\n",
                    NodeOp.getStr( var, "def", "" ), NodeOp.getStr( var, "remark", "" ) );
      }
      else if( StrOp.equals( "const", NodeOp.getName( var ) ) ) {
        char* l_vt = StrOp.dup( NodeOp.getStr( var, "vt", "void" ) );
        if( StrOp.equals( "this", l_vt ) )
          l_vt = StrOp.fmt( "iO%s", objectName );
        FileOp.fmt( fDoc, "<tr><td valign=\"top\" align=\"right\"><sub><i>const</i></sub><tt> %s</tt></td><td valign=\"top\"><tt><i><b>%s</b></i> = %s</tt><br>%s</td></tr>\n",
                    l_vt,
                    NodeOp.getStr( var, "name", "?" ),
                    NodeOp.getStr( var, "val", "?" ),
                    NodeOp.getStr( var, "remark", "" )
                   );
        StrOp.free( l_vt );
      }
      else if( StrOp.equals( "fun", NodeOp.getName( var ) ) ) {
        ListOp.add( funList, (obj)var );
      }
    }

    ListOp.sort( funList, comp );

    for( i = 0; i < ListOp.size( funList ); i++ ) {
      iONode var = (iONode)ListOp.get( funList, i );
      if( StrOp.equals( "fun", NodeOp.getName( var ) )  && !NodeOp.getBool( var, "internal", False ) ) {
        iOList paramList = ListOp.inst();
        const char* funName = NodeOp.getStr( var, "name", "?" );
        char* vt = StrOp.dup( NodeOp.getStr( var, "vt", "void" ) );
        if( StrOp.equals( "this", vt ) )
          vt = StrOp.fmt( "iO%s", objectName );

        FileOp.fmt( fIndex, "<li><a href=\"%s.html#%s_%s\">%s</a>\n", docname, objectName, funName, funName );

        FileOp.fmt( fDoc, "<tr><td valign=\"top\" align=\"right\"><tt>%s</tt></td><td valign=\"top\"><a name=\"%s_%s\"/><tt><b>%s(",
                    vt,
                    objectName,funName,funName );

        StrOp.free( vt );
        {
          int subChildCnt = NodeOp.getChildCnt( var );
          int s = 0;

          for( s = 0; s < subChildCnt; s++ ) {
            iONode subchild = NodeOp.getChild( var, s );
            if( StrOp.equals( "param", NodeOp.getName( subchild ) ) ) {
              char* l_vt = StrOp.dup( NodeOp.getStr( subchild, "vt", "void" ) );
              if( StrOp.equals( "this", l_vt ) )
                l_vt = StrOp.fmt( "iO%s", objectName );

              if( NodeOp.getStr( subchild, "remark", NULL ) )
                ListOp.add( paramList, (obj)subchild );
              FileOp.fmt( fDoc, "%s%s <i>%s</i>",
                          s==0?" ":", ",
                          l_vt,
                          NodeOp.getStr( subchild, "name", "?" )
                         );
              StrOp.free( l_vt );
            }
          }
          if( subChildCnt == 0 ) {
            FileOp.fmt( fDoc, " void" );
          }
        }

        /**/
        if( ListOp.size( paramList ) == 0 )
          FileOp.fmt( fDoc, " );</b></tt><br>%s</td></tr>\n", NodeOp.getStr( var, "remark", "" ) );
        else {
          int paramCnt = ListOp.size( paramList );
          int p = 0;
          FileOp.fmt( fDoc, " );</b></tt><br>%s\n", NodeOp.getStr( var, "remark", "" ) );
          FileOp.fmt( fDoc, "<table border=\"1\" cellpadding=\"4\" cellspacing=\"0\" width=\"100%%\">\n" );
          for( p = 0; p < paramCnt; p++ ) {
            iONode param = (iONode)ListOp.get( paramList, p );
            FileOp.fmt( fDoc, "<tr><td><tt><i>%s</i></tt></td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
                        NodeOp.getStr( param, "name", "" ), NodeOp.getStr( param, "remark", "" ),
                        NodeOp.getStr( param, "range", "*" ), NodeOp.getStr( param, "unit", "" ) );
          }
          FileOp.fmt( fDoc, "</table>\n" );
          FileOp.fmt( fDoc, "</td></tr>\n" );
        }
        paramList->base.del( paramList );

      }

    }
    ListOp.base.del( funList );

    FileOp.fmt( fIndex, "</ol>\n" );
  }

  if( level == 0 ) {
    FileOp.fmt( fDoc, "</table></p>\n", nodeName );
    FileOp.fmt( fIndex, "</li>\n", nodeName );
  }

  return rc;
}

static int __gConstHdr( iONode node, const char* fileName ) {
  int rc = 0;
  const char* modulename = NodeOp.getStr( node, "name", "?" );
  const char* docname = NodeOp.getStr( node, "docname", "object" );

  const char* revision = NodeOp.getStr( node, "revision", "" );
  const char*   source = NodeOp.getStr( node, "source", "" );

  Boolean toLower = NodeOp.getBool( node, "tolower", True );

  char* docFileName = StrOp.fmt( "doc/%s.html", docname );
  char* idxFileName = StrOp.fmt( "doc/%s-index.html", docname );


  iOFile fDoc = FileOp.inst( toLower?StrOp.strlwr(docFileName):docFileName, OPEN_WRITE );
  iOFile fIndex = FileOp.inst( toLower?StrOp.strlwr(idxFileName):idxFileName, OPEN_WRITE );

  nodeMap = MapOp.inst();

  __processPrefix( fDoc, fIndex, modulename, NodeOp.getStr( node, "title", "object" ),
                   docname, revision, source );

  {
    iOList childList = ListOp.inst();
    iOList interfaceList = ListOp.inst();
    int childCnt = NodeOp.getChildCnt( node );
    int interfaceCnt = 0;
    int i = 0;
    TraceOp.println( "Processing %d childs.", childCnt );

    for( i = 0; i < childCnt; i++ ) {
      iONode child = NodeOp.getChild( node, i );
      if( NodeOp.getType( child ) == ELEMENT_NODE ) {
        if( StrOp.equals( "interface", NodeOp.getName( child ) ) )
          ListOp.add( interfaceList, (obj)child );
        else
          ListOp.add( childList, (obj)child );
      }
    }

    ListOp.sort( interfaceList, comp );
    ListOp.sort( childList, comp );

    for( i = 0; i < ListOp.size( interfaceList ); i++ ) {
      iONode child = (iONode)ListOp.get( interfaceList, i );
      if( NodeOp.getType( child ) == ELEMENT_NODE ) {
        char* indexStr = StrOp.fmt( "%d.", i+1 );
        __processChild( indexStr, child, fDoc, fIndex, 0, fileName, modulename, docname, revision, source, toLower );
        StrOp.free( indexStr );
        interfaceCnt++;
      }
    }

    for( i = 0; i < ListOp.size( childList ); i++ ) {
      iONode child = (iONode)ListOp.get( childList, i );
      if( NodeOp.getType( child ) == ELEMENT_NODE ) {
        char* indexStr = StrOp.fmt( "%d.", i+1+interfaceCnt );
        __processChild( indexStr, child, fDoc, fIndex, 0, fileName, modulename, docname, revision, source, toLower );
        StrOp.free( indexStr );
      }
    }

    ListOp.base.del( interfaceList );
    ListOp.base.del( childList );
  }

  __processSuffix( fDoc, fIndex );


  FileOp.base.del( fDoc );
  FileOp.base.del( fIndex );

  StrOp.free(docFileName);
  StrOp.free(idxFileName);

  return rc;
}
