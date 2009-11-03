/*
 * keygen.c
 *
 *  Created on: Nov 2, 2009
 *      Author: rob
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
#include "rocs/public/system.h"
#include "rocs/public/str.h"
#include "rocs/public/cmdln.h"


/**Function to encode a string (vigenere-encoding)
 * @param the string to encode
 * @param key
 * @return the encoded byte[]
 */
byte* encode(const char* b, const char* key) {
  byte* encodedArray = allocMem(StrOp.len(b));
  int keyLength = StrOp.len(key);
  int x = 0;
  int i = 0;
  int newChar = 0;
  for(i = 0; i < StrOp.len(b); i++){
    newChar = b[i] + key[x];
    if(newChar>127)
      newChar -= 256;
    encodedArray[i] = (byte)newChar;
    x++;
    if(x == keyLength)
      x=0;
  }
  return encodedArray;
}

int main( int argc, const char* argv[] ) {
  int rc = 0;
  iOCmdLn arg = NULL;
  const char* keyfile = NULL;
  const char* email = NULL;
  const char* expdate = NULL;
  Boolean enc = False;
  Boolean dec = False;

  iOTrace trc = TraceOp.inst( TRCLEVEL_INFO, "keygen", True );
  TraceOp.setAppID( trc, "k" );

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

  enc     = CmdLnOp.hasKey( arg, "-enc" );
  dec     = CmdLnOp.hasKey( arg, "-dec" );
  keyfile = CmdLnOp.getStrDef( arg, "-file", NULL );
  email   = CmdLnOp.getStrDef( arg, "-email", NULL );
  expdate = CmdLnOp.getStrDef( arg, "-date", NULL );

  if( !enc && !dec )
    enc = True;

  if( keyfile == NULL )
    keyfile = "lic.dat";

  if( email == NULL ) {
    rc = -1;
    TraceOp.println( "-email <address> is missing" );
  }

  if( enc && expdate == NULL ) {
    rc = -1;
    TraceOp.println( "-date <DD-MM-YYYY> is missing" );
  }

  if( rc == 0 && enc ) {
    char*  s = StrOp.fmt("%s%s%s", SystemOp.getEyecatcher(), expdate, email );
    byte*  b = encode(s, email);
    iOFile f = FileOp.inst( keyfile, OPEN_WRITE );
    char* bs = StrOp.byteToStr(b, StrOp.len(s));
    FileOp.write( f, email, StrOp.len(email) );
    FileOp.write( f, ";", 1 );
    FileOp.write( f, bs, StrOp.len(bs) );
    FileOp.base.del(f);
    StrOp.free(s);
  }
  else if( rc == 0 && dec ) {
    iOFile f = FileOp.inst( keyfile, OPEN_READONLY );
    int len = FileOp.size(f);
    byte* b;
    char* s;
    char* bs = allocMem(len+1);
    MemOp.set( bs, 0, len+1 );
    FileOp.read( f, bs, len );
    FileOp.base.del(f);
    b = StrOp.strToByte(bs);
    s = SystemOp.decode(b, len/2, email);
    TraceOp.println( s );

    if( !StrOp.startsWith( s, SystemOp.getEyecatcher() ) ) {
      TraceOp.println( "invalid key" );
      rc = -1;
    }

    if( rc == 0 ) {
      Boolean expired = SystemOp.isExpired(s);
      TraceOp.println( "license is %s", expired ? "expired":"valid" );
    }

  }

  return rc;
}
