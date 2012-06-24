/*
 Rocs - OS independent C library

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "rocs/public/str.h"
#include "rocs/public/mem.h"
#include "rocs/public/system.h"

/* What's in a name? */
#ifdef _WIN32
  #define strcasecmp stricmp
  #define strncasecmp strnicmp
#elif defined __OS2__
  #define strcasecmp stricmp
  #define strncasecmp strnicmp
#endif

/*
 ***** _Public functions.
 */

static char* __cat( char* s1, const char* s2, RocsMemID id ) {
  char* s = NULL;
  if( s1 != NULL && s2 != NULL ) {
    s = allocIDMem( strlen( s1 ) + strlen( s2 ) + 1, id );
    s[0] = '\0';
    strcpy( s, s1 );
    strcat( s, s2 );
    freeIDMem( s1, id );
  }
  else if( s1 == NULL && s2 != NULL ) {
    s = allocIDMem( strlen( s2 ) + 1, id );
    s[0] = '\0';
    strcpy( s, s2 );
  }
  return s;
}
static char* _cat( char* s1, const char* s2 ) {
  return __cat( s1, s2, RocsStrID );
}
static char* _catID( char* s1, const char* s2, RocsMemID id ) {
  return __cat( s1, s2, id );
}


static int _len( const char* s ) {
  return s != NULL ? strlen( s ):0;
}

static char* _copy( char* dst, const char* src ) {
  return strcpy( dst, src );
}


static char* _findc( const char* s, char c ) {
  if( s != NULL )
    return strchr( s, c );
  else
    return NULL;
}

static char* _find( const char* s1, const char* s2 ) {
  if( s1 != NULL && s2 != NULL )
    return strstr( s1, s2 );
  else
    return NULL;
}

static char* _findi( const char* s1, const char* s2 ) {
  if( s1 != NULL && s2 != NULL ) {
    char* s11 = StrOp.dup( s1 );
    char* s22 = StrOp.dup( s2 );
    char* result = NULL;
    s11 = (char*)StrOp.strlwr( s11 );
    s22 = (char*)StrOp.strlwr( s22 );
    result = strstr( s11, s22 );
    if( result != NULL ) {
      /* return the real pointer! */
      result = (result - s11) + (char*)s1;
    }
    StrOp.free( s11 );
    StrOp.free( s22 );
    return result;
  }
  else
    return NULL;
}

static const char* __strupr( const char *str ) {
  char* uprStr = (char*)str;
  if( str != NULL ) {
    while( *uprStr ) {
      *uprStr = toupper(*uprStr);
      uprStr++;
    };
  }
  return str;
}

static const char* __strlwr( const char *str ) {
  char* lwrStr = (char*)str;
  if( str != NULL ) {
    while( *lwrStr ) {
      *lwrStr = tolower(*lwrStr);
      lwrStr++;
    };
  }
  return str;
}

static char* __dup( const char* org, RocsMemID id ) {
  char* s = NULL;
  if( org != NULL ) {
    s = allocIDMem( strlen( org ) + 1, id );
    strcpy( s, org );
  }
  return s;
}
static char* _dup( const char* org ) {
  return __dup( org, RocsStrID );
}
static char* _dupID( const char* org, RocsMemID id ) {
  return __dup( org, id );
}


static int _copynz( char* dest, int len, const char* str ) {
  if( str != NULL ) {
    int strLen = StrOp.len( str );
    if( strLen < len )
      len = strLen;
    memcpy( dest, str, len );
    return len;
  }
  return 0;
}

static char* _fmt( const char* fmt, ... ) {
  va_list args;
  char s[4096] = {'\0'};

  va_start(args, fmt);
  vsprintf(s, fmt, args);
  va_end(args);

  return _dup( s );
}
static char* _fmtID( RocsMemID id, const char* fmt, ... ) {
  va_list args;
  char s[4096] = {'\0'};

  va_start(args, fmt);
  vsprintf(s, fmt, args);
  va_end(args);

  return _dupID( s, id );
}


static char* _fmtb( char* buffer, const char* fmt, ... ) {
  va_list args;

  va_start(args, fmt);
  vsprintf(buffer, fmt, args);
  va_end(args);

  return buffer;
}

static Boolean _equals( const char* s1, const char* s2 ) {
  if( s1 == NULL || s2 == NULL )
    return False;
  else {
    int rc = strcmp( s1, s2 );
    return rc == 0 ? True:False;
  }
}

static Boolean _equalsn( const char* s1, const char* s2, int len ) {
  if( s1 == NULL || s2 == NULL )
    return False;
  else {
    int rc = strncmp( s1, s2, len );
    return rc == 0 ? True:False;
  }
}

static Boolean _equalsi( const char* s1, const char* s2 ) {
  if( s1 == NULL || s2 == NULL )
    return False;
  else {
    int rc = strcasecmp( s1, s2 );
    return rc == 0 ? True:False;
  }
}

static Boolean _equalsni( const char* s1, const char* s2, int len ) {
  if( s1 == NULL || s2 == NULL )
    return False;
  else {
    int rc = strncasecmp( s1, s2, len );
    return rc == 0 ? True:False;
  }
}

static char* _byteToStr( unsigned char* data, int size ) {
  static char cHex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  int i = 0;
  char* s = allocIDMem( size*2 + 1, RocsStrID );
  for( i = 0; i < size; i++ ) {
    int b = data[i];
    s[i*2]   = cHex[(b&0xF0)>>4 ];
    s[i*2+1] = cHex[ b&0x0F     ];
  }
  s[size*2] = '\0';
  return s;
}

static unsigned char* _strToByte( const char* s ) {
  int i = 0;
  int len = StrOp.len(s);
  unsigned char* b = allocMem( len/2 + 1);
  for( i = 0; i < len; i+=2 ) {
    char val[3] = {0};
    val[0] = s[i];
    val[1] = s[i+1];
    val[2] = '\0';
    b[i/2] = (unsigned char)(strtol( val, NULL, 16)&0xFF);
  }
  return b;
}

/* ------------------------------------------------------------
 * StrOp.trim()
 */
static char* __trim( char* s, RocsMemID id ) {
  int   i   = 0;
  int   len = StrOp.len( s );
  char* tmp = s;

  /* Leading blanks. */
  for( i = 0; i < len; i++ ) {
    if( s[i] == ' ' )
      tmp = &s[i+1];
    else
      break;
  }

  /* Trailing blanks. */
  for( i = len-1; i >= 0; i-- ) {
    if( s[i] == ' ' )
      s[i] = '\0';
    else
      break;
  }

  if( StrOp.len( tmp ) < len ) {
    tmp = StrOp.dup( tmp );
    freeIDMem( s, id );
  }
  return tmp;
}
static char* _trim( char* s ) {
  return __trim( s, RocsStrID );
}
static char* _trimID( char* s, RocsMemID id ) {
  return __trim( s, id );
}


static Boolean _endsWith( const char* s1, const char* s2 ) {
  int len    = StrOp.len( s1 );
  int endLen = StrOp.len( s2 );
  if( len >= endLen ) {
    const char* s11 =  &s1[len - endLen];
    return StrOp.equals( s11, s2 );
  }
  return False;
}


static Boolean _endsWithi( const char* s1, const char* s2 ) {
  int len    = StrOp.len( s1 );
  int endLen = StrOp.len( s2 );
  if( len >= endLen ) {
    const char* s11 =  &s1[len - endLen];
    return StrOp.equalsi( s11, s2 );
  }
  return False;
}


static Boolean _startsWith( const char* s1, const char* s2 ) {
  int len    = StrOp.len( s1 );
  int startLen = StrOp.len( s2 );
  if( len >= startLen ) {
    return StrOp.equalsn( s1, s2, startLen );
  }
  return False;
}


static Boolean _startsWithi( const char* s1, const char* s2 ) {
  int len    = StrOp.len( s1 );
  int startLen = StrOp.len( s2 );
  if( len >= startLen ) {
    return StrOp.equalsni( s1, s2, startLen );
  }
  return False;
}


static int _replaceAll( char* str, char charA, char charB ) {
  int len    = StrOp.len( str );
  int i = 0;
  int replaced = 0;
  for( i = 0; i < len; i++ ) {
    if( str[i] == charA ) {
      str[i] = charB;
      replaced++;
    }
  }
  return replaced;
}


static char* _replaceAllSub( const char* inputString, const char* substring, const char* replacement  ) {
  char* newStr = StrOp.dup(inputString);
  char* pSub   = StrOp.find( newStr, substring );
  int   sublen = StrOp.len(substring);
  int   replen = StrOp.len(replacement);

  while( pSub != NULL ) {
    char* tmp = newStr;
    int offset = pSub - newStr;
    *pSub = '\0';
    newStr = StrOp.fmt("%s%s%s", newStr, replacement, pSub+sublen);
    StrOp.free(tmp);
    if( StrOp.len( newStr + offset) > sublen )
      pSub = StrOp.find( newStr + offset + replen , substring );
    else
      pSub = NULL;
  };

  return newStr;
}



/**
 * All %varnames% are replaced with the values of the environment variables.
 * SystemOp.getProperty() is used for getting the value.
 */
static char* _replaceAllSubstitutions( const char* str ) {
  static char delimiter = '%';
  int strLen = StrOp.len(str);
  int i = 0;
  char* tmpStr = StrOp.dup(str);
  char* resolvedStr = NULL;

  char* startV = NULL;
  char* endV = NULL;

  do {

    startV = strchr( tmpStr, delimiter );

    if( startV != NULL ) {
    tmpStr[startV-tmpStr] = '\0';
    endV = strchr( startV + 1, delimiter );
    }
    else {
      resolvedStr = StrOp.cat( resolvedStr, tmpStr );
      break;
    }


    if( startV != NULL && endV != NULL ) {
      /* hit */
      tmpStr[endV-tmpStr] = '\0';
      resolvedStr = StrOp.cat( resolvedStr, tmpStr );
      resolvedStr = StrOp.cat( resolvedStr, SystemOp.getProperty(startV+1) );
      tmpStr = endV + 1;
      startV = strchr( tmpStr, delimiter );

      /* copy part between the endV and the new startV or end of string */
      if( startV == NULL )
        resolvedStr = StrOp.cat( resolvedStr, tmpStr );

    }
    else {
      /* end of loop */
      resolvedStr = StrOp.cat( resolvedStr, tmpStr );
      startV = NULL;
    }
  } while( startV != NULL );
  return resolvedStr;
}


static void _long2snz( char* dest, int destlen, long val ) {
        char szFormat[256];
        char* szVal = allocIDMem( destlen + 1, RocsStrID );

        /* create formatstring with <len> trailing zero's */
        sprintf( szFormat, "%c0%uld", '%', destlen );
        sprintf( szVal, szFormat, val );

  /* copy string without terminating zero */
  StrOp.copynz( dest, destlen, szVal );
  freeIDMem( szVal, RocsStrID );
}


static void _int2snz( char* dest, int destlen, int val ) {
  _long2snz( dest, destlen, (long)val );
}


static char* __snz2sz( const char* snz, int len, RocsMemID id ) {
  char* sz = allocIDMem( len + 1, id );
  memcpy( sz, snz, len );
  sz[len] = '\0';
  return sz;
}
static char* _snz2sz( const char* snz, int len ) {
  return __snz2sz( snz, len, RocsStrID );
}
static char* _snz2szID( const char* snz, int len, RocsMemID id ) {
  return __snz2sz( snz, len, id );
}


static void _freeStr( char* str ) {
  freeIDMem( str, RocsStrID );
}
static void _freeStrID( char* str, RocsMemID id ) {
  freeIDMem( str, id );
}


static char* _getExtension( const char* str ) {
  char* p = NULL;
  p = strrchr( str, '.' );
  if( p != NULL )
    return p + 1;
  return NULL;
}


static char* __createStamp( RocsMemID id ) {
  time_t     tt = time(NULL);
  struct tm* t  = NULL;
  int        ms = SystemOp.getMillis();

  /* Alloc a string for the stamp; caller should freeup! */
  char* s = allocIDMem( 32, id );

  t = localtime( &tt );

  /* Create the stamp. */
  sprintf( s, "%04d%02d%02d.%02d%02d%02d.%03d",
    t->tm_year+1900, t->tm_mon+1, t->tm_mday,
    t->tm_hour, t->tm_min, t->tm_sec, ms
    );

  return s;
}
static char* _createStamp(void) {
  return __createStamp( RocsStrID );
}
static char* _createStampID( RocsMemID id ) {
  return __createStamp( id );
}


static char* __createStampNoDots( RocsMemID id ) {
  time_t     tt = time(NULL);
  struct tm* t  = NULL;
  int        ms = SystemOp.getMillis();

  /* Alloc a string for the stamp; caller should freeup! */
  char* s = allocIDMem( 32, id );

  t = localtime( &tt );

  /* Create the stamp. */
  sprintf( s, "%04d%02d%02d%02d%02d%02d%03d",
    t->tm_year+1900, t->tm_mon+1, t->tm_mday,
    t->tm_hour, t->tm_min, t->tm_sec, ms
    );

  return s;
}
static char* _createStampNoDots(void) {
  return __createStampNoDots( RocsStrID );
}
static char* _createStampNoDotsID( RocsMemID id ) {
  return __createStampNoDots( id );
}


static char* _isoDate( long tt ) {
  struct tm* t  = NULL;

  /* Alloc a string for the stamp; caller should freeup! */
  char* s = allocIDMem( 32, RocsStrID );

  t = localtime( &tt );

  /* Create the stamp. */
  sprintf( s, "%04d-%02d-%02d",
    t->tm_year+1900, t->tm_mon+1, t->tm_mday
    );

  return s;
}

static char* _isoTime( long tt ) {
  struct tm* t  = NULL;

  /* Alloc a string for the stamp; caller should freeup! */
  char* s = allocIDMem( 32, RocsStrID );

  t = localtime( &tt );

  /* Create the stamp. */
  sprintf( s, "%02d:%02d:%02d",
    t->tm_hour, t->tm_min, t->tm_sec
    );

  return s;
}

static int _getCntLinefeeds( const char* str ) {
  const char* pLf = NULL;
  int cnt = 0;
  pLf = strchr( str, '\n' );
  while( pLf != NULL ) {
    cnt++;
    pLf = strchr( pLf+1, '\n' );
  };
  return cnt;
}


static char* __getLine( const char* str, int linenr, RocsMemID id ) {
  const char* p = str;
  char* lineStr = NULL;
  int cnt = 0;

  while( p != NULL && cnt != linenr ) {
    cnt++;
    p = strchr( p, '\n' );
    if( p != NULL ) {
      p++; /* Skip the linefeed. */
      if( p[0] == '\r' )
        p++; /* Skip the cariage return. */
    }
  };

  if( p != NULL ) {
    const char* pLf = strchr( p, '\n' );
    if( pLf != NULL ) {
      int len = pLf - p;
      lineStr = allocIDMem( len + 1, id );
      MemOp.copy( lineStr, p, len );
      lineStr[len] = '\0';
    }
    else if( linenr == 0 ) {
      /* One line without linefeed. */
      lineStr = StrOp.dup( str );
    }
  }

  /* Remove (DOS/Windows/OS2) trailing return. */
  if( lineStr != NULL ) {
    int   i = 0;
    int len = StrOp.len( lineStr );
    /* Trailing returns. */
    for( i = len-1; i >= 0; i-- ) {
      if( lineStr[i] == '\r' )
        lineStr[i] = '\0';
      else
        break;
    }
  }

  return lineStr;
}


static char* _getLine( const char* str, int linenr ) {
  return __getLine( str, linenr, RocsStrID );
}
static char* _getLineID( const char* str, int linenr, RocsMemID id ) {
  return __getLine( str, linenr, id );
}


static char* _getNextLine( const char* str, int* pLen ) {
  const char* p  = str;
  const char* pl = NULL;

  p = strchr( p, '\n' );
  if( p != NULL ) {
    p++; /* Skip the linefeed. */
    if( p[0] == '\r' )
      p++; /* Skip the cariage return. */

    /* Calculate the length. */
    pl = strchr( p, '\n' );
    if( pl != NULL )
      *pLen = pl - p;
    else
      *pLen = StrOp.len( p );
  }

  return (char*)p;
}

static char* _encode4URL( const char* url ) {
  char* id = NULL;
  char* enc = NULL;
  int i = 0;
  int len = StrOp.len(url);
  int idx = 0;
  int c;
  enc = allocMem( len * 3 + 1 ); /* worst case */
  for( i = 0; i < len; i++ ) {
    c = (byte)url[i];
    if( c > 0x7F ) {
      StrOp.fmtb( &enc[idx], "%%%02X", c );
      idx+=3;
    }
    else {
      switch(url[i]) {
        case '%':
        case '?':
        case '/':
        case '=':
        case '"':
        case ' ':
          StrOp.fmtb( &enc[idx], "%%%02X", c );
          idx+=3;
          break;
        default:
          enc[idx] = url[i];
          idx++;
          break;
      }
    }
  }
  id = StrOp.dup(enc);
  freeMem(enc);
  return id;
}


static char* _decode4URL( const char* url ) {
  char* dec = NULL;
  char* id = NULL;
  int i = 0;
  int len = StrOp.len(url);
  int idx = 0;
  dec = allocMem( len + 1 );
  for( i = 0; i < len; i++ ) {
    if( url[i] =='%' ) {
      int code;
      char sCode[5];
      sCode[0] = '0';
      sCode[1] = 'x';
      sCode[2] = url[i+1];
      sCode[3] = url[i+2];
      sCode[4] = 0;
      code = strtol( sCode, NULL, 16 );
      i+=2;
      dec[idx] = (char)code;
      idx++;
    }
    else {
      dec[idx] = url[i];
      idx++;
    }
  }
  id = StrOp.dup(dec);
  freeMem(dec);
  return id;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/str.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
