/*
 * compile for 64 file size support with following options:
 *
 * gcc -D__USE_LARGEFILE64 -D__USE_FILE_OFFSET64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 what.c -o what
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char** argv ) {

  int i = 0;
  printf("\n----------------------------------------\n");
  for( i = 0; i < argc; i++ ) {
    printf( "argv[%d]=[%s]\n", i, argv[i] );
  }

  printf("----------------------------------------\n");
  printf("sizeof(void*)   = %d\n", sizeof(void *));
  printf("sizeof(char)    = %d\n", sizeof(char));
  printf("sizeof(short)   = %d\n", sizeof(short));
  printf("sizeof(int)     = %d\n", sizeof(int));
  printf("sizeof(long)    = %d\n", sizeof(long));
  printf("sizeof(float)   = %d\n", sizeof(float));
  printf("sizeof(double)  = %d\n", sizeof(double));
  printf("sizeof(off_t)   = %d\n", sizeof(off_t));

  printf("----------------------------------------\n");
#if defined _AIX
  printf( "_AIX\n" );
#endif

#if defined __unix__
  printf( "__unix__\n" );
#endif

#if defined __linux__
  printf( "__linux__\n" );
#endif

#if defined __OS2__
  printf( "__OS2__\n" );
#endif

#if defined _WIN32
  printf( "_WIN32\n" );
#endif

#if defined _WIN64
  printf( "_WIN64\n" );
#endif

#if defined __hpux
  printf( "__hpux\n" );
#endif

#if defined sun
  printf( "sun\n" );
#endif

#if defined __APPLE__
  printf( "__APPLE__\n" );
#endif

#if defined __MVS__
  printf( "__MVS__\n" );
#endif
  printf("----------------------------------------\n");

  return 0;
}
