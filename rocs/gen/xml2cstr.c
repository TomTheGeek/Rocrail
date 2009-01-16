/** ------------------------------------------------------------
  * $Author: robvrs $
  * $Date: 2005/12/31 13:24:29 $
  * $Revision: 1.7 $
  * $Source: /cvsroot/rojav/rocs/gen/xml2cstr.c,v $
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

#define BUFFERSIZE 1024

/** ------------------------------------------------------------
  * public main()
  *
  * @param  argc Number of commanline arguments.
  * @param  argv Commanline arguments.
  * @return      Applications exit code.
  */
int main( int argc, const char* argv[] ) {
  int rc = 0;

  iOTrace trc = TraceOp.inst( TRCLEVEL_INFO, "xml2cstr", True );
  TraceOp.setAppID( trc, "x" );

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
  if( argc >= 4 && FileOp.exist(argv[1]) ) {
    TraceOp.println( "Converting [%s] into a \"C\" strting [%s], named as [%s]", argv[1], argv[2], argv[3] );
    {
      iOFile msgXml = FileOp.inst( argv[1], OPEN_READONLY );
      iOFile msgStr = FileOp.inst( argv[2], OPEN_WRITE );
      if( msgXml == NULL || msgStr == NULL )
        return -1;
      else {
        unsigned char* inBuffer  = allocMem( 1 * BUFFERSIZE );
        unsigned char* outBuffer = allocMem( 2 * BUFFERSIZE );
        long infileLen = FileOp.size( msgXml );

        long readed = 0;


        FileOp.fmt( msgStr, "#ifdef __cplusplus\n  extern \"C\" {\n#endif\n\n" );
        FileOp.fmt( msgStr, "const char %s[] = {\n", argv[3] );

        while( readed != infileLen ) {
          long toRead = BUFFERSIZE <= infileLen - readed ? BUFFERSIZE:(infileLen - readed);
          if( FileOp.read( msgXml, inBuffer, toRead ) ) {
            int i = 0;
            int len = 0;
            readed += toRead;
            for( i = 0; i < toRead; i++ ) {
              if( inBuffer[i] == '\r' )
                continue;
              else {
                FileOp.fmt( msgStr, "%d,", inBuffer[i] );
                if (inBuffer[i] > 99)
                  len += 3;
                else if (inBuffer[i] > 9)
                  len += 2;
                else
                  len++;
              }
              if( len > 50 ) {
                len = 0;
                FileOp.write( msgStr, "\n", 1 );
              }
            }
          }
          else {
            TraceOp.println( "Error reading [%]! readed=%ld infileLen=%ld", argv[1], readed, infileLen );
            break;
          }
        }

        FileOp.fmt( msgStr, "%d\n};\n", 0 );
        FileOp.fmt( msgStr, "#ifdef __cplusplus\n  }\n#endif\n\n" );


        FileOp.close( msgXml );
        FileOp.base.del( msgXml );
        FileOp.close( msgStr );
        FileOp.base.del( msgStr );

        freeMem( inBuffer );
        freeMem( outBuffer );
      }
    }

  }
  else {
    TraceOp.println( "Usage: xml2cstr input.xml output.c strname" );
  }

  return rc;
}


