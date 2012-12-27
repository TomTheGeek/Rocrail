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

#include "rocs/impl/attr_impl.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"
#include "rocs/public/doc.h"
#include "rocs/public/system.h"


static int instCnt = 0;

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/* ------------------------------------------------------------
 * base.name()
 */
static const char* __name(void) {
  /* The objectname. */
  /* Conflict */
  return name;
}


/* ------------------------------------------------------------
 * base.serialize()
 */
static unsigned char* __serialize(void* inst, long* size) {
  iOAttrData data = Data(inst);
  Boolean utf8 = ( DocOp.isUTF8Encoding() && DocOp.isUTF2Latin() );
  char* val = utf8 ? SystemOp.latin2utf(data->val):StrOp.dup( data->val );
  char* s = NULL;
  s = StrOp.cat( s, data->name);
  s = StrOp.cat( s, "=\"");
  s = StrOp.cat( s, val);
  s = StrOp.cat( s, "\"");
  *size = StrOp.len( s );
  StrOp.free( val );
  return (unsigned char*)s;
}

/* ------------------------------------------------------------
 * base.deserialize()
 */
static void __deserialize(void* inst, unsigned char* a) {
  char* name = (char*)a;
  char* val  = strchr( name, '=' );

  if( a == NULL )
    return;

  if( val != NULL ) {
    char* tmp;
    *val = '\0'; /* end of name */
    val = val + 1; /* skip the \" */

    tmp = strchr( val, '\"' );
    if( tmp != NULL )
      *tmp = '\0'; /* end of value */
    else
      val = "";
  }
  else
    val = "";

  AttrOp.setName( inst, name );
  AttrOp.setVal( inst, val );
}


/* ------------------------------------------------------------
 * base.toString()
 */
static char* __toString(void* inst) {
  iOAttrData data = Data(inst);
  char* str = allocIDMem( StrOp.len(data->name) + StrOp.len(data->val) + 4, RocsStrID );
  str = StrOp.fmtb( str, "%s=\"%s\"", data->name, data->val );
  return str;
}


/* ------------------------------------------------------------
 * base.del()
 */
static void __del(void* inst) {
  iOAttr     attr = inst;
  iOAttrData data = Data(inst);
  StrOp.freeID( data->name, RocsAttrID );
  StrOp.freeID( data->val, RocsAttrID );
  freeIDMem( data->origval, RocsAttrID );
  freeIDMem( data, RocsAttrID );
  freeIDMem( attr, RocsAttrID );
  instCnt--;
}
static int __count(void) {
  return instCnt;
}
static void* __properties(void* inst) {
  return NULL;
}
static struct OBase* __clone( void* inst ) {
  iOAttr attr = inst;
  iOAttrData data = Data(inst);
  return (obj)AttrOp.inst( AttrOp.getName( attr ), AttrOp.getEscVal( attr ) );
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}


/*
< 	&lt;
> 	&gt;
& 	&amp;
" 	&quot;
' 	&apos;
*/
static int __getUniLatin15( const char* str, char* escapeChar );

static int __getLatin15( const char* str, char* escapeChar ) {
  if( str[0] != '&' )
    return 0;

  if( str[1] == '#' )
    return __getUniLatin15( str, escapeChar );

  if( StrOp.equalsn( str, "&lt;"   , 4 ) ) { *escapeChar = '<' ; return 4 ; }
  if( StrOp.equalsn( str, "&gt;"   , 4 ) ) { *escapeChar = '>' ; return 4 ; }
  if( StrOp.equalsn( str, "&amp;"  , 5 ) ) { *escapeChar = '&' ; return 5 ; }
  if( StrOp.equalsn( str, "&quot;" , 6 ) ) { *escapeChar = '\"'; return 6 ; }
  if( StrOp.equalsn( str, "&apos;" , 6 ) ) { *escapeChar = '\''; return 6 ; }

  /* ISO-8859-15 */
  switch( str[1] ) {
    case 'A':
      if( StrOp.equalsn( str, "&Agrave;", 8 ) ) { *escapeChar = 0xC0 ; return 8 ; }
      if( StrOp.equalsn( str, "&Aacute;", 8 ) ) { *escapeChar = 0xC1 ; return 8 ; }
      if( StrOp.equalsn( str, "&Acirc;" , 7 ) ) { *escapeChar = 0xC2 ; return 7 ; }
      if( StrOp.equalsn( str, "&Atilde;", 8 ) ) { *escapeChar = 0xC3 ; return 8 ; }
      if( StrOp.equalsn( str, "&Auml;"  , 6 ) ) { *escapeChar = 0xC4 ; return 6 ; }
      if( StrOp.equalsn( str, "&Aring;" , 7 ) ) { *escapeChar = 0xC5 ; return 7 ; }
      if( StrOp.equalsn( str, "&AElig;" , 7 ) ) { *escapeChar = 0xC6 ; return 7 ; }
      break;

    case 'a':
      if( StrOp.equalsn( str, "&agrave;", 8 ) ) { *escapeChar = 0xE0 ; return 8 ; }
      if( StrOp.equalsn( str, "&aacute;", 8 ) ) { *escapeChar = 0xE1 ; return 8 ; }
      if( StrOp.equalsn( str, "&acirc;" , 7 ) ) { *escapeChar = 0xE2 ; return 7 ; }
      if( StrOp.equalsn( str, "&atilde;", 8 ) ) { *escapeChar = 0xE3 ; return 8 ; }
      if( StrOp.equalsn( str, "&auml;"  , 6 ) ) { *escapeChar = 0xE4 ; return 6 ; }
      if( StrOp.equalsn( str, "&aring;" , 7 ) ) { *escapeChar = 0xE5 ; return 7 ; }
      if( StrOp.equalsn( str, "&aelig;" , 7 ) ) { *escapeChar = 0xE6 ; return 7 ; }
      break;

    case 'C':
      if( StrOp.equalsn( str, "&Ccedil;", 8 ) ) { *escapeChar = 0xC7 ; return 8 ; }
      break;

    case 'c':
      if( StrOp.equalsn( str, "&ccedil;", 8 ) ) { *escapeChar = 0xE7 ; return 8 ; }
      if( StrOp.equalsn( str, "&cent;"  , 6 ) ) { *escapeChar = 0xA2 ; return 6 ; }
      if( StrOp.equalsn( str, "&copy;"  , 6 ) ) { *escapeChar = 0xA9 ; return 6 ; }
      break;

    case 'd':
      if( StrOp.equalsn( str, "&deg;"   , 5 ) ) { *escapeChar = 0xB0 ; return 5 ; }
      if( StrOp.equalsn( str, "&divide;", 8 ) ) { *escapeChar = 0xF7 ; return 8 ; }
      break;

    case 'E':
      if( StrOp.equalsn( str, "&Egrave;", 8 ) ) { *escapeChar = 0xC8 ; return 8 ; }
      if( StrOp.equalsn( str, "&Eacute;", 8 ) ) { *escapeChar = 0xC9 ; return 8 ; }
      if( StrOp.equalsn( str, "&Ecirc;" , 7 ) ) { *escapeChar = 0xCA ; return 7 ; }
      if( StrOp.equalsn( str, "&ETH;"   , 5 ) ) { *escapeChar = 0xD0 ; return 5 ; }
      if( StrOp.equalsn( str, "&Euml;"  , 6 ) ) { *escapeChar = 0xCB ; return 6 ; }
      break;

    case 'e':
      if( StrOp.equalsn( str, "&euro;"  , 6 ) ) { *escapeChar = 0xA4 ; return 6 ; }
      if( StrOp.equalsn( str, "&egrave;", 8 ) ) { *escapeChar = 0xE8 ; return 8 ; }
      if( StrOp.equalsn( str, "&eacute;", 8 ) ) { *escapeChar = 0xE9 ; return 8 ; }
      if( StrOp.equalsn( str, "&ecirc;" , 7 ) ) { *escapeChar = 0xEA ; return 7 ; }
      if( StrOp.equalsn( str, "&euml;"  , 6 ) ) { *escapeChar = 0xEB ; return 6 ; }
      if( StrOp.equalsn( str, "&eth;"   , 5 ) ) { *escapeChar = 0xF0 ; return 5 ; }
      break;

    case 'I':
      if( StrOp.equalsn( str, "&Igrave;", 8 ) ) { *escapeChar = 0xCC ; return 8 ; }
      if( StrOp.equalsn( str, "&Iacute;", 8 ) ) { *escapeChar = 0xCD ; return 8 ; }
      if( StrOp.equalsn( str, "&Icirc;" , 7 ) ) { *escapeChar = 0xCE ; return 7 ; }
      if( StrOp.equalsn( str, "&Iuml;"  , 6 ) ) { *escapeChar = 0xCF ; return 6 ; }
      break;

    case 'i':
      if( StrOp.equalsn( str, "&iexcl;" , 7 ) ) { *escapeChar = 0xA1 ; return 7 ; }
      if( StrOp.equalsn( str, "&iquest;", 8 ) ) { *escapeChar = 0xBF ; return 8 ; }
      if( StrOp.equalsn( str, "&igrave;", 8 ) ) { *escapeChar = 0xEC ; return 8 ; }
      if( StrOp.equalsn( str, "&iacute;", 8 ) ) { *escapeChar = 0xED ; return 8 ; }
      if( StrOp.equalsn( str, "&icirc;" , 7 ) ) { *escapeChar = 0xEE ; return 7 ; }
      if( StrOp.equalsn( str, "&iuml;"  , 6 ) ) { *escapeChar = 0xEF ; return 6 ; }
      break;

    case 'l':
      if( StrOp.equalsn( str, "&laquo;" , 7 ) ) { *escapeChar = 0xAB ; return 7 ; }
      break;

    case 'm':
      if( StrOp.equalsn( str, "&macr;"  , 6 ) ) { *escapeChar = 0xAF ; return 6 ; }
      if( StrOp.equalsn( str, "&micro;" , 7 ) ) { *escapeChar = 0xB5 ; return 7 ; }
      break;

    case 'N':
      if( StrOp.equalsn( str, "&Ntilde;", 8 ) ) { *escapeChar = 0xD1 ; return 8 ; }
      break;

    case 'n':
      if( StrOp.equalsn( str, "&not;"   , 5 ) ) { *escapeChar = 0xAC ; return 5 ; }
      if( StrOp.equalsn( str, "&ntilde;", 8 ) ) { *escapeChar = 0xF1 ; return 8 ; }
      break;

    case 'O':
      if( StrOp.equalsn( str, "&Ograve;", 8 ) ) { *escapeChar = 0xD2 ; return 8 ; }
      if( StrOp.equalsn( str, "&Oacute;", 8 ) ) { *escapeChar = 0xD3 ; return 8 ; }
      if( StrOp.equalsn( str, "&Ocirc;" , 7 ) ) { *escapeChar = 0xD4 ; return 7 ; }
      if( StrOp.equalsn( str, "&Otilde;", 8 ) ) { *escapeChar = 0xD5 ; return 8 ; }
      if( StrOp.equalsn( str, "&Ouml;"  , 6 ) ) { *escapeChar = 0xD6 ; return 6 ; }
      if( StrOp.equalsn( str, "&Oslash;", 8 ) ) { *escapeChar = 0xD8 ; return 8 ; }
      break;

    case 'o':
      if( StrOp.equalsn( str, "&ograve;", 8 ) ) { *escapeChar = 0xF2 ; return 8 ; }
      if( StrOp.equalsn( str, "&oacute;", 8 ) ) { *escapeChar = 0xF3 ; return 8 ; }
      if( StrOp.equalsn( str, "&ocirc;" , 7 ) ) { *escapeChar = 0xF4 ; return 7 ; }
      if( StrOp.equalsn( str, "&otilde;", 8 ) ) { *escapeChar = 0xF5 ; return 8 ; }
      if( StrOp.equalsn( str, "&ouml;"  , 6 ) ) { *escapeChar = 0xF6 ; return 6 ; }
      if( StrOp.equalsn( str, "&ordf;"  , 6 ) ) { *escapeChar = 0xAA ; return 6 ; }
      if( StrOp.equalsn( str, "&ordm;"  , 6 ) ) { *escapeChar = 0xBA ; return 6 ; }
      if( StrOp.equalsn( str, "&oslash;", 8 ) ) { *escapeChar = 0xF8 ; return 8 ; }
      break;

    case 'p':
      if( StrOp.equalsn( str, "&para;"  , 6 ) ) { *escapeChar = 0xB6 ; return 6 ; }
      if( StrOp.equalsn( str, "&plusmn;", 8 ) ) { *escapeChar = 0xB1 ; return 8 ; }
      if( StrOp.equalsn( str, "&pound;" , 7 ) ) { *escapeChar = 0xA3 ; return 7 ; }
      break;

    case 'r':
      if( StrOp.equalsn( str, "&raquo;" , 7 ) ) { *escapeChar = 0xBB ; return 7 ; }
      if( StrOp.equalsn( str, "&reg;"   , 5 ) ) { *escapeChar = 0xAE ; return 5 ; }
      break;

    case 's':
      if( StrOp.equalsn( str, "&sect;"  , 6 ) ) { *escapeChar = 0xA7 ; return 6 ; }
      if( StrOp.equalsn( str, "&sup1;"  , 6 ) ) { *escapeChar = 0xB9 ; return 6 ; }
      if( StrOp.equalsn( str, "&sup2;"  , 6 ) ) { *escapeChar = 0xB2 ; return 6 ; }
      if( StrOp.equalsn( str, "&sup3;"  , 6 ) ) { *escapeChar = 0xB3 ; return 6 ; }
      if( StrOp.equalsn( str, "&szlig;" , 7 ) ) { *escapeChar = 0xDF ; return 7 ; }
      break;

    case 'T':
      if( StrOp.equalsn( str, "&THORN;" , 7 ) ) { *escapeChar = 0xDE ; return 7 ; }
      break;

    case 't':
      if( StrOp.equalsn( str, "&thorn;" , 7 ) ) { *escapeChar = 0xFE ; return 7 ; }
      if( StrOp.equalsn( str, "&times;" , 7 ) ) { *escapeChar = 0xD7 ; return 7 ; }
      break;

    case 'U':
      if( StrOp.equalsn( str, "&Ugrave;", 8 ) ) { *escapeChar = 0xD9 ; return 8 ; }
      if( StrOp.equalsn( str, "&Uacute;", 8 ) ) { *escapeChar = 0xDA ; return 8 ; }
      if( StrOp.equalsn( str, "&Ucirc;" , 7 ) ) { *escapeChar = 0xDB ; return 7 ; }
      if( StrOp.equalsn( str, "&Uuml;"  , 6 ) ) { *escapeChar = 0xDC ; return 6 ; }
      break;

    case 'u':
      if( StrOp.equalsn( str, "&ugrave;", 8 ) ) { *escapeChar = 0xF9 ; return 8 ; }
      if( StrOp.equalsn( str, "&uacute;", 8 ) ) { *escapeChar = 0xFA ; return 8 ; }
      if( StrOp.equalsn( str, "&ucirc;" , 7 ) ) { *escapeChar = 0xFB ; return 7 ; }
      if( StrOp.equalsn( str, "&uuml;"  , 6 ) ) { *escapeChar = 0xFC ; return 6 ; }
      break;

    case 'Y':
      if( StrOp.equalsn( str, "&Yacute;", 8 ) ) { *escapeChar = 0xDD ; return 8 ; }
      break;

    case 'y':
      if( StrOp.equalsn( str, "&yen;"   , 5 ) ) { *escapeChar = 0xA5 ; return 5 ; }
      if( StrOp.equalsn( str, "&yacute;", 8 ) ) { *escapeChar = 0xFD ; return 8 ; }
      if( StrOp.equalsn( str, "&yuml;"  , 6 ) ) { *escapeChar = 0xFF ; return 6 ; }
      break;
  }

  return 0;
}

static int __getUniLatin15( const char* str, char* escapeChar ) {
  if( str[0] != '&' && str[1] != '#' )
    return 0;

  if( StrOp.equalsn( str, "&#60;"  , 5 ) ) { *escapeChar = '<' ; return 5 ; }
  if( StrOp.equalsn( str, "&#62;"  , 5 ) ) { *escapeChar = '>' ; return 5 ; }
  if( StrOp.equalsn( str, "&#38;"  , 5 ) ) { *escapeChar = '&' ; return 5 ; }
  if( StrOp.equalsn( str, "&#34;"  , 5 ) ) { *escapeChar = '\"'; return 5 ; }
  if( StrOp.equalsn( str, "&#39;"  , 5 ) ) { *escapeChar = '\''; return 5 ; }

  /* ISO-8859-15 */
  switch( str[2] ) {
    case '1':
      switch( str[3] ) {
        case '6':
        if( StrOp.equalsn( str, "&#161;"  , 6 ) ) { *escapeChar = 0xA1 ; return 6 ; }
        if( StrOp.equalsn( str, "&#162;"  , 6 ) ) { *escapeChar = 0xA2 ; return 6 ; }
        if( StrOp.equalsn( str, "&#163;"  , 6 ) ) { *escapeChar = 0xA3 ; return 6 ; }
        if( StrOp.equalsn( str, "&#165;"  , 6 ) ) { *escapeChar = 0xA5 ; return 6 ; }
        if( StrOp.equalsn( str, "&#167;"  , 6 ) ) { *escapeChar = 0xA7 ; return 6 ; }
        if( StrOp.equalsn( str, "&#169;"  , 6 ) ) { *escapeChar = 0xA9 ; return 6 ; }
        break;
        case '7':
        if( StrOp.equalsn( str, "&#170;"  , 6 ) ) { *escapeChar = 0xAA ; return 6 ; }
        if( StrOp.equalsn( str, "&#171;"  , 6 ) ) { *escapeChar = 0xAB ; return 6 ; }
        if( StrOp.equalsn( str, "&#172;"  , 6 ) ) { *escapeChar = 0xAC ; return 6 ; }
        if( StrOp.equalsn( str, "&#174;"  , 6 ) ) { *escapeChar = 0xAE ; return 6 ; }
        if( StrOp.equalsn( str, "&#175;"  , 6 ) ) { *escapeChar = 0xAF ; return 6 ; }
        if( StrOp.equalsn( str, "&#176;"  , 6 ) ) { *escapeChar = 0xB0 ; return 6 ; }
        if( StrOp.equalsn( str, "&#177;"  , 6 ) ) { *escapeChar = 0xB1 ; return 6 ; }
        if( StrOp.equalsn( str, "&#178;"  , 6 ) ) { *escapeChar = 0xB2 ; return 6 ; }
        if( StrOp.equalsn( str, "&#179;"  , 6 ) ) { *escapeChar = 0xB3 ; return 6 ; }
        break;
        case '8':
        if( StrOp.equalsn( str, "&#181;"  , 6 ) ) { *escapeChar = 0xB5 ; return 6 ; }
        if( StrOp.equalsn( str, "&#182;"  , 6 ) ) { *escapeChar = 0xB6 ; return 6 ; }
        if( StrOp.equalsn( str, "&#185;"  , 6 ) ) { *escapeChar = 0xB9 ; return 6 ; }
        if( StrOp.equalsn( str, "&#186;"  , 6 ) ) { *escapeChar = 0xBA ; return 6 ; }
        if( StrOp.equalsn( str, "&#187;"  , 6 ) ) { *escapeChar = 0xBB ; return 6 ; }
        break;
        case '9':
        if( StrOp.equalsn( str, "&#191;"  , 6 ) ) { *escapeChar = 0xBF ; return 6 ; }
        if( StrOp.equalsn( str, "&#192;"  , 6 ) ) { *escapeChar = 0xC0 ; return 6 ; }
        if( StrOp.equalsn( str, "&#193;"  , 6 ) ) { *escapeChar = 0xC1 ; return 6 ; }
        if( StrOp.equalsn( str, "&#194;"  , 6 ) ) { *escapeChar = 0xC2 ; return 6 ; }
        if( StrOp.equalsn( str, "&#195;"  , 6 ) ) { *escapeChar = 0xC3 ; return 6 ; }
        if( StrOp.equalsn( str, "&#196;"  , 6 ) ) { *escapeChar = 0xC4 ; return 6 ; }
        if( StrOp.equalsn( str, "&#196;"  , 6 ) ) { *escapeChar = 0xC5 ; return 6 ; }
        if( StrOp.equalsn( str, "&#198;"  , 6 ) ) { *escapeChar = 0xC6 ; return 6 ; }
        if( StrOp.equalsn( str, "&#199;"  , 6 ) ) { *escapeChar = 0xC7 ; return 6 ; }
        break;
      }
    break;

    case '2':
      switch( str[3] ) {
        case '0':
        if( StrOp.equalsn( str, "&#200;"  , 6 ) ) { *escapeChar = 0xC8 ; return 6 ; }
        if( StrOp.equalsn( str, "&#201;"  , 6 ) ) { *escapeChar = 0xC9 ; return 6 ; }
        if( StrOp.equalsn( str, "&#202;"  , 6 ) ) { *escapeChar = 0xCA ; return 6 ; }
        if( StrOp.equalsn( str, "&#208;"  , 6 ) ) { *escapeChar = 0xD0 ; return 6 ; }
        if( StrOp.equalsn( str, "&#203;"  , 6 ) ) { *escapeChar = 0xCB ; return 6 ; }
        if( StrOp.equalsn( str, "&#204;"  , 6 ) ) { *escapeChar = 0xCC ; return 6 ; }
        if( StrOp.equalsn( str, "&#205;"  , 6 ) ) { *escapeChar = 0xCD ; return 6 ; }
        if( StrOp.equalsn( str, "&#206;"  , 6 ) ) { *escapeChar = 0xCE ; return 6 ; }
        if( StrOp.equalsn( str, "&#207;"  , 6 ) ) { *escapeChar = 0xCF ; return 6 ; }
        if( StrOp.equalsn( str, "&#209;"  , 6 ) ) { *escapeChar = 0xD1 ; return 6 ; }
        break;
        case '1':
        if( StrOp.equalsn( str, "&#210;"  , 6 ) ) { *escapeChar = 0xD2 ; return 6 ; }
        if( StrOp.equalsn( str, "&#211;"  , 6 ) ) { *escapeChar = 0xD3 ; return 6 ; }
        if( StrOp.equalsn( str, "&#212;"  , 6 ) ) { *escapeChar = 0xD4 ; return 6 ; }
        if( StrOp.equalsn( str, "&#213;"  , 6 ) ) { *escapeChar = 0xD5 ; return 6 ; }
        if( StrOp.equalsn( str, "&#214;"  , 6 ) ) { *escapeChar = 0xD6 ; return 6 ; }
        if( StrOp.equalsn( str, "&#215;"  , 6 ) ) { *escapeChar = 0xD7 ; return 6 ; }
        if( StrOp.equalsn( str, "&#216;"  , 6 ) ) { *escapeChar = 0xD8 ; return 6 ; }
        if( StrOp.equalsn( str, "&#217;"  , 6 ) ) { *escapeChar = 0xD9 ; return 6 ; }
        if( StrOp.equalsn( str, "&#218;"  , 6 ) ) { *escapeChar = 0xDA ; return 6 ; }
        if( StrOp.equalsn( str, "&#219;"  , 6 ) ) { *escapeChar = 0xDB ; return 6 ; }
        break;
        case '2':
        if( StrOp.equalsn( str, "&#220;"  , 6 ) ) { *escapeChar = 0xDC ; return 6 ; }
        if( StrOp.equalsn( str, "&#221;"  , 6 ) ) { *escapeChar = 0xDD ; return 6 ; }
        if( StrOp.equalsn( str, "&#222;"  , 6 ) ) { *escapeChar = 0xDE ; return 6 ; }
        if( StrOp.equalsn( str, "&#223;"  , 6 ) ) { *escapeChar = 0xDF ; return 6 ; }
        if( StrOp.equalsn( str, "&#224;"  , 6 ) ) { *escapeChar = 0xE0 ; return 6 ; }
        if( StrOp.equalsn( str, "&#225;"  , 6 ) ) { *escapeChar = 0xE1 ; return 6 ; }
        if( StrOp.equalsn( str, "&#226;"  , 6 ) ) { *escapeChar = 0xE2 ; return 6 ; }
        if( StrOp.equalsn( str, "&#227;"  , 6 ) ) { *escapeChar = 0xE3 ; return 6 ; }
        if( StrOp.equalsn( str, "&#228;"  , 6 ) ) { *escapeChar = 0xE4 ; return 6 ; }
        if( StrOp.equalsn( str, "&#229;"  , 6 ) ) { *escapeChar = 0xE5 ; return 6 ; }
        break;
        case '3':
        if( StrOp.equalsn( str, "&#230;"  , 6 ) ) { *escapeChar = 0xE6 ; return 6 ; }
        if( StrOp.equalsn( str, "&#231;"  , 6 ) ) { *escapeChar = 0xE7 ; return 6 ; }
        if( StrOp.equalsn( str, "&#232;"  , 6 ) ) { *escapeChar = 0xE8 ; return 6 ; }
        if( StrOp.equalsn( str, "&#233;"  , 6 ) ) { *escapeChar = 0xE9 ; return 6 ; }
        if( StrOp.equalsn( str, "&#234;"  , 6 ) ) { *escapeChar = 0xEA ; return 6 ; }
        if( StrOp.equalsn( str, "&#235;"  , 6 ) ) { *escapeChar = 0xEB ; return 6 ; }
        if( StrOp.equalsn( str, "&#236;"  , 6 ) ) { *escapeChar = 0xEC ; return 6 ; }
        if( StrOp.equalsn( str, "&#237;"  , 6 ) ) { *escapeChar = 0xED ; return 6 ; }
        if( StrOp.equalsn( str, "&#238;"  , 6 ) ) { *escapeChar = 0xEE ; return 6 ; }
        if( StrOp.equalsn( str, "&#239;"  , 6 ) ) { *escapeChar = 0xEF ; return 6 ; }
        break;
        case '4':
        if( StrOp.equalsn( str, "&#240;"  , 6 ) ) { *escapeChar = 0xF0 ; return 6 ; }
        if( StrOp.equalsn( str, "&#241;"  , 6 ) ) { *escapeChar = 0xF1 ; return 6 ; }
        if( StrOp.equalsn( str, "&#242;"  , 6 ) ) { *escapeChar = 0xF2 ; return 6 ; }
        if( StrOp.equalsn( str, "&#243;"  , 6 ) ) { *escapeChar = 0xF3 ; return 6 ; }
        if( StrOp.equalsn( str, "&#244;"  , 6 ) ) { *escapeChar = 0xF4 ; return 6 ; }
        if( StrOp.equalsn( str, "&#245;"  , 6 ) ) { *escapeChar = 0xF5 ; return 6 ; }
        if( StrOp.equalsn( str, "&#246;"  , 6 ) ) { *escapeChar = 0xF6 ; return 6 ; }
        if( StrOp.equalsn( str, "&#247;"  , 6 ) ) { *escapeChar = 0xF7 ; return 6 ; }
        if( StrOp.equalsn( str, "&#248;"  , 6 ) ) { *escapeChar = 0xF8 ; return 6 ; }
        if( StrOp.equalsn( str, "&#249;"  , 6 ) ) { *escapeChar = 0xF9 ; return 6 ; }
        break;
        case '5':
        if( StrOp.equalsn( str, "&#250;"  , 6 ) ) { *escapeChar = 0xFA ; return 6 ; }
        if( StrOp.equalsn( str, "&#251;"  , 6 ) ) { *escapeChar = 0xFB ; return 6 ; }
        if( StrOp.equalsn( str, "&#252;"  , 6 ) ) { *escapeChar = 0xFC ; return 6 ; }
        if( StrOp.equalsn( str, "&#253;"  , 6 ) ) { *escapeChar = 0xFD ; return 6 ; }
        if( StrOp.equalsn( str, "&#254;"  , 6 ) ) { *escapeChar = 0xFE ; return 6 ; }
        if( StrOp.equalsn( str, "&#255;"  , 6 ) ) { *escapeChar = 0xFF ; return 6 ; }
        break;
      }
      break;

    case '8':
      if( StrOp.equalsn( str, "&#8364;" , 7 ) ) { *escapeChar = 0xA4 ; return 7 ; }
      break;

  }

  return 0;
}

static const char* __getHTMLEscape( int code, int* len ) {

  /* ToDo! */

  if( code == '<'  ) { *len = 4; return "&lt;";   }
  if( code == '>'  ) { *len = 4; return "&gt;";   }
  if( code == '&'  ) { *len = 5; return "&amp;";  }
  if( code == '\"' ) { *len = 6; return "&quot;"; }
  if( code == '\'' ) { *len = 6; return "&apos;"; }

  if( !( code & 0x80 ) || !DocOp.isHTMLEscapes() ) {
    *len = 0;
    return NULL;
  }

  /* ISO-8859-15 */
  code = code & 0xFF;
  switch( code & 0xF0 ) {
    case 0xA0:
      if( code == 0xA0 ) { *len = 6; return "&nbsp;"  ; }
      if( code == 0xA1 ) { *len = 7; return "&iexcl;" ; }
      if( code == 0xA2 ) { *len = 6; return "&cent;"  ; }
      if( code == 0xA3 ) { *len = 7; return "&pound;" ; }
      if( code == 0xA4 ) { *len = 6; return "&euro;"  ; }
      if( code == 0xA5 ) { *len = 5; return "&yen;"   ; }
      if( code == 0xA7 ) { *len = 6; return "&sect;"  ; }
      if( code == 0xA9 ) { *len = 6; return "&copy;"  ; }
      if( code == 0xAA ) { *len = 6; return "&ordf;"  ; }
      if( code == 0xAB ) { *len = 7; return "&laquo;" ; }
      if( code == 0xAC ) { *len = 5; return "&not;"   ; }
      if( code == 0xAE ) { *len = 5; return "&reg;"   ; }
      if( code == 0xAF ) { *len = 6; return "&macr;"  ; }
      break;

    case 0xB0:
      if( code == 0xB0 ) { *len = 5; return "&deg;"   ; }
      if( code == 0xB1 ) { *len = 8; return "&plusmn;"; }
      if( code == 0xB2 ) { *len = 6; return "&sup2;"  ; }
      if( code == 0xB3 ) { *len = 6; return "&sup3;"  ; }
      if( code == 0xB5 ) { *len = 7; return "&micro;" ; }
      if( code == 0xB6 ) { *len = 6; return "&para;"  ; }
      if( code == 0xB7 ) { *len = 8; return "&middot;"; }
      if( code == 0xB9 ) { *len = 6; return "&sup1;"  ; }
      if( code == 0xBA ) { *len = 6; return "&ordm;"  ; }
      if( code == 0xBB ) { *len = 7; return "&raquo;" ; }
      if( code == 0xBF ) { *len = 8; return "&iquest;"; }
      break;

    case 0xC0:
      if( code == 0xC0 ) { *len = 8; return "&Agrave;"; }
      if( code == 0xC1 ) { *len = 8; return "&Aacute;"; }
      if( code == 0xC2 ) { *len = 7; return "&Acirc;" ; }
      if( code == 0xC3 ) { *len = 8; return "&Atilde;"; }
      if( code == 0xC4 ) { *len = 6; return "&Auml;"  ; }
      if( code == 0xC5 ) { *len = 7; return "&Aring;" ; }
      if( code == 0xC6 ) { *len = 7; return "&AElig;" ; }
      if( code == 0xC7 ) { *len = 8; return "&Ccedil;"; }
      if( code == 0xC8 ) { *len = 8; return "&Egrave;"; }
      if( code == 0xC9 ) { *len = 8; return "&Eacute;"; }
      if( code == 0xCA ) { *len = 7; return "&Ecirc;" ; }
      if( code == 0xCB ) { *len = 6; return "&Euml;"  ; }
      if( code == 0xCC ) { *len = 8; return "&Igrave;"; }
      if( code == 0xCD ) { *len = 8; return "&Iacute;"; }
      if( code == 0xCE ) { *len = 7; return "&Icirc;" ; }
      if( code == 0xCF ) { *len = 6; return "&Iuml;"  ; }
      break;

    case 0xD0:
      if( code == 0xD0 ) { *len = 5; return "&ETH;"   ; }
      if( code == 0xD1 ) { *len = 8; return "&Ntilde;"; }
      if( code == 0xD2 ) { *len = 8; return "&Ograve;"; }
      if( code == 0xD3 ) { *len = 8; return "&Oacute;"; }
      if( code == 0xD4 ) { *len = 7; return "&Ocirc;" ; }
      if( code == 0xD5 ) { *len = 8; return "&Otilde;"; }
      if( code == 0xD6 ) { *len = 6; return "&Ouml;"  ; }
      if( code == 0xD7 ) { *len = 7; return "&times;" ; }
      if( code == 0xD8 ) { *len = 8; return "&Oslash;"; }
      if( code == 0xD9 ) { *len = 8; return "&Ugrave;"; }
      if( code == 0xDA ) { *len = 8; return "&Uacute;"; }
      if( code == 0xDB ) { *len = 7; return "&Ucirc;" ; }
      if( code == 0xDC ) { *len = 6; return "&Uuml;"  ; }
      if( code == 0xDD ) { *len = 8; return "&Yacute;"; }
      if( code == 0xDE ) { *len = 7; return "&THORN;" ; }
      if( code == 0xDF ) { *len = 7; return "&szlig;" ; }
      break;

    case 0xE0:
      if( code == 0xE0 ) { *len = 8; return "&agrave;"; }
      if( code == 0xE1 ) { *len = 8; return "&aacute;"; }
      if( code == 0xE2 ) { *len = 7; return "&acirc;" ; }
      if( code == 0xE3 ) { *len = 8; return "&atilde;"; }
      if( code == 0xE4 ) { *len = 6; return "&auml;"  ; }
      if( code == 0xE5 ) { *len = 7; return "&aring;" ; }
      if( code == 0xE6 ) { *len = 7; return "&aelig;" ; }
      if( code == 0xE7 ) { *len = 8; return "&ccedil;"; }
      if( code == 0xE8 ) { *len = 8; return "&egrave;"; }
      if( code == 0xE9 ) { *len = 8; return "&eacute;"; }
      if( code == 0xEA ) { *len = 7; return "&ecirc;" ; }
      if( code == 0xEB ) { *len = 6; return "&euml;"  ; }
      if( code == 0xEC ) { *len = 8; return "&igrave;"; }
      if( code == 0xED ) { *len = 8; return "&iacute;"; }
      if( code == 0xEE ) { *len = 7; return "&icirc;" ; }
      if( code == 0xEF ) { *len = 6; return "&iuml;"  ; }
      break;

    case 0xF0:
      if( code == 0xF0 ) { *len = 5; return "&eth;"   ; }
      if( code == 0xF1 ) { *len = 8; return "&ntilde;"; }
      if( code == 0xF2 ) { *len = 8; return "&ograve;"; }
      if( code == 0xF3 ) { *len = 8; return "&oacute;"; }
      if( code == 0xF4 ) { *len = 7; return "&ocirc;" ; }
      if( code == 0xF5 ) { *len = 8; return "&otilde;"; }
      if( code == 0xF6 ) { *len = 6; return "&ouml;"  ; }
      if( code == 0xF7 ) { *len = 8; return "&divide;"; }
      if( code == 0xF8 ) { *len = 8; return "&oslash;"; }
      if( code == 0xF9 ) { *len = 8; return "&ugrave;"; }
      if( code == 0xFA ) { *len = 8; return "&uacute;"; }
      if( code == 0xFB ) { *len = 7; return "&ucirc;" ; }
      if( code == 0xFC ) { *len = 6; return "&uuml;"  ; }
      if( code == 0xFD ) { *len = 8; return "&yacute;"; }
      if( code == 0xFE ) { *len = 7; return "&thorn;" ; }
      if( code == 0xFF ) { *len = 6; return "&yuml;"  ; }
      break;

  }
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Could not escape code=0x%02X.", code );

  return NULL;
}

static const char* __getHTMLUniEscape( int code, int* len ) {

  /* ToDo! */

  if( code == '<'  ) { *len = 5; return "&#60;"; }
  if( code == '>'  ) { *len = 5; return "&#62;"; }
  if( code == '&'  ) { *len = 5; return "&#38;"; }
  if( code == '\"' ) { *len = 5; return "&#34;"; }
  if( code == '\'' ) { *len = 5; return "&#39;"; }

  if( !( code & 0x80 ) || !DocOp.isUniCodeEscapes() ) {
    *len = 0;
    return NULL;
  }

  /* ISO-8859-15 */
  code = code & 0xFF;
  switch( code & 0xF0 ) {
    case 0xA0:
      if( code == 0xA0 ) { *len = 6; return "&#160;"  ; }
      if( code == 0xA1 ) { *len = 6; return "&#161;"  ; }
      if( code == 0xA2 ) { *len = 6; return "&#162;"  ; }
      if( code == 0xA3 ) { *len = 6; return "&#163;"  ; }
      if( code == 0xA4 ) { *len = 7; return "&#8364;" ; }
      if( code == 0xA5 ) { *len = 6; return "&#165;"  ; }
      if( code == 0xA7 ) { *len = 6; return "&#167;"  ; }
      if( code == 0xA9 ) { *len = 6; return "&#169;"  ; }
      if( code == 0xAA ) { *len = 6; return "&#170;"  ; }
      if( code == 0xAB ) { *len = 6; return "&#171;"  ; }
      if( code == 0xAC ) { *len = 6; return "&#172;"  ; }
      if( code == 0xAE ) { *len = 6; return "&#174;"  ; }
      if( code == 0xAF ) { *len = 6; return "&#175;"  ; }
      break;

    case 0xB0:
      if( code == 0xB0 ) { *len = 6; return "&#176;"  ; }
      if( code == 0xB1 ) { *len = 6; return "&#177;"  ; }
      if( code == 0xB2 ) { *len = 6; return "&#178;"  ; }
      if( code == 0xB3 ) { *len = 6; return "&#179;"  ; }
      if( code == 0xB5 ) { *len = 6; return "&#181;"  ; }
      if( code == 0xB6 ) { *len = 6; return "&#182;"  ; }
      if( code == 0xB7 ) { *len = 6; return "&#183;"  ; }
      if( code == 0xB9 ) { *len = 6; return "&#185;"  ; }
      if( code == 0xBA ) { *len = 6; return "&#186;"  ; }
      if( code == 0xBB ) { *len = 6; return "&#187;"  ; }
      if( code == 0xBF ) { *len = 6; return "&#191;"  ; }
      break;

    case 0xC0:
      if( code == 0xC0 ) { *len = 6; return "&#192;"  ; }
      if( code == 0xC1 ) { *len = 6; return "&#193;"  ; }
      if( code == 0xC2 ) { *len = 6; return "&#194;"  ; }
      if( code == 0xC3 ) { *len = 6; return "&#195;"  ; }
      if( code == 0xC4 ) { *len = 6; return "&#196;"  ; }
      if( code == 0xC5 ) { *len = 6; return "&#197;"  ; }
      if( code == 0xC6 ) { *len = 6; return "&#198;"  ; }
      if( code == 0xC7 ) { *len = 6; return "&#199;"  ; }
      if( code == 0xC8 ) { *len = 6; return "&#200;"  ; }
      if( code == 0xC9 ) { *len = 6; return "&#201;"  ; }
      if( code == 0xCA ) { *len = 6; return "&#202;"  ; }
      if( code == 0xCB ) { *len = 6; return "&#203;"  ; }
      if( code == 0xCC ) { *len = 6; return "&#204;"  ; }
      if( code == 0xCD ) { *len = 6; return "&#205;"  ; }
      if( code == 0xCE ) { *len = 6; return "&#206;"  ; }
      if( code == 0xCF ) { *len = 6; return "&#207;"  ; }
      break;

    case 0xD0:
      if( code == 0xD0 ) { *len = 6; return "&#208;"  ; }
      if( code == 0xD1 ) { *len = 6; return "&#209;"  ; }
      if( code == 0xD2 ) { *len = 6; return "&#210;"  ; }
      if( code == 0xD3 ) { *len = 6; return "&#211;"  ; }
      if( code == 0xD4 ) { *len = 6; return "&#212;"  ; }
      if( code == 0xD5 ) { *len = 6; return "&#213;"  ; }
      if( code == 0xD6 ) { *len = 6; return "&#214;"  ; }
      if( code == 0xD7 ) { *len = 6; return "&#215;"  ; }
      if( code == 0xD8 ) { *len = 6; return "&#216;"  ; }
      if( code == 0xD9 ) { *len = 6; return "&#217;"  ; }
      if( code == 0xDA ) { *len = 6; return "&#218;"  ; }
      if( code == 0xDB ) { *len = 6; return "&#219;"  ; }
      if( code == 0xDC ) { *len = 6; return "&#220;"  ; }
      if( code == 0xDD ) { *len = 6; return "&#221;"  ; }
      if( code == 0xDE ) { *len = 6; return "&#222;"  ; }
      if( code == 0xDF ) { *len = 6; return "&#223;"  ; }
      break;

    case 0xE0:
      if( code == 0xE0 ) { *len = 6; return "&#224;"  ; }
      if( code == 0xE1 ) { *len = 6; return "&#225;"  ; }
      if( code == 0xE2 ) { *len = 6; return "&#226;"  ; }
      if( code == 0xE3 ) { *len = 6; return "&#227;"  ; }
      if( code == 0xE4 ) { *len = 6; return "&#228;"  ; }
      if( code == 0xE5 ) { *len = 6; return "&#229;"  ; }
      if( code == 0xE6 ) { *len = 6; return "&#230;"  ; }
      if( code == 0xE7 ) { *len = 6; return "&#231;"  ; }
      if( code == 0xE8 ) { *len = 6; return "&#232;"  ; }
      if( code == 0xE9 ) { *len = 6; return "&#233;"  ; }
      if( code == 0xEA ) { *len = 6; return "&#234;"  ; }
      if( code == 0xEB ) { *len = 6; return "&#235;"  ; }
      if( code == 0xEC ) { *len = 6; return "&#236;"  ; }
      if( code == 0xED ) { *len = 6; return "&#237;"  ; }
      if( code == 0xEE ) { *len = 6; return "&#238;"  ; }
      if( code == 0xEF ) { *len = 6; return "&#239;"  ; }
      break;

    case 0xF0:
      if( code == 0xF0 ) { *len = 6; return "&#240;"  ; }
      if( code == 0xF1 ) { *len = 6; return "&#241;"  ; }
      if( code == 0xF2 ) { *len = 6; return "&#242;"  ; }
      if( code == 0xF3 ) { *len = 6; return "&#243;"  ; }
      if( code == 0xF4 ) { *len = 6; return "&#244;"  ; }
      if( code == 0xF5 ) { *len = 6; return "&#245;"  ; }
      if( code == 0xF6 ) { *len = 6; return "&#246;"  ; }
      if( code == 0xF7 ) { *len = 6; return "&#247;"  ; }
      if( code == 0xF8 ) { *len = 6; return "&#248;"  ; }
      if( code == 0xF9 ) { *len = 6; return "&#249;"  ; }
      if( code == 0xFA ) { *len = 6; return "&#250;"  ; }
      if( code == 0xFB ) { *len = 6; return "&#251;"  ; }
      if( code == 0xFC ) { *len = 6; return "&#252;"  ; }
      if( code == 0xFD ) { *len = 6; return "&#253;"  ; }
      if( code == 0xFE ) { *len = 6; return "&#254;"  ; }
      if( code == 0xFF ) { *len = 6; return "&#255;"  ; }
      break;

  }
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Could not escape code=0x%02X.", code );

  return NULL;
}

static const char* __escapeStr( iOAttr inst, const char* str ) {
  iOAttrData data = Data(inst);

  /* Reset the original string value: */
  if( data->origval != NULL ) {
    freeIDMem( data->origval, RocsAttrID );
    data->origval = NULL;
  }

  data->escaped = False;

  /* Escape the string: */
  if( str != NULL ) {
    int len = StrOp.len( str );
    int i = 0;
    int idx = 0;
    char* buffer = allocIDMem( 1 + len * 10, RocsAttrID ); /* Worst case. */

    for( i = 0; i < len; i++ ) {
      if( str[i] == '&' && str[i+1] == '#' ) {
        char esc;
        int esclen = __getUniLatin15( str+i, &esc );
        if( esclen > 0 ) {
          buffer[idx] = str[i];
          idx++;
          data->escaped = True;
        }
      }
      else if( str[i] == '&' ) {
        char esc;
        int esclen = __getLatin15( str+i, &esc );
        if( esclen > 0 ) {
          buffer[idx] = str[i];
          idx++;
        }
        else {
          buffer[idx+0] = '&'; buffer[idx+1] = 'a'; buffer[idx+2] = 'm'; buffer[idx+3] = 'p';
          buffer[idx+4] = ';';
          idx += 5;
        }
        data->escaped = True;
      }
      else {
        int len = 0;
        const char* esc = NULL;

        if( DocOp.isUniCodeEscapes() )
          esc = __getHTMLUniEscape( str[i], &len );
        else
          esc = __getHTMLEscape( str[i], &len );

        if( esc != NULL ) {
          int n = 0;
          for( n = 0; n < len; n++ ) {
            buffer[idx+n] = esc[n];
          }
          idx += len;
          data->escaped = True;
        }
        else {
          buffer[idx] = str[i];
          idx++;
        }
      }

    }
    buffer[idx] = 0;
    if( data->val != NULL )
      StrOp.freeID( data->val, RocsAttrID );
    data->val = StrOp.dupID( buffer, RocsAttrID );
    freeIDMem( buffer, RocsAttrID );
  }
  return data->val;
}


static const char* __unescapeStr( iOAttr inst ) {
  iOAttrData data = Data(inst);

  if( data->escaped && data->val != NULL ) {
    Boolean hasEscapes = False;
    int len = StrOp.len( data->val );
    int i = 0;
    int idx = 0;
    int esclen = 0;
    data->origval = allocIDMem( len, RocsAttrID );
    for( i = 0; i < len; i++ ) {
      char esc = '?';
      esclen = __getLatin15( data->val+i, &esc );
      if( esclen == 0 ) {
        data->origval[idx] = data->val[i];
        idx++;
      }
      else {
        hasEscapes = True;
        data->origval[idx] = esc;
        idx++;
        i += esclen - 1; /* Add escape length, but substract 1 for the loop incrementer. */
      }
    }

    if( !hasEscapes ) {
      freeIDMem( data->origval, RocsAttrID );
      data->origval = NULL;
    }
    else {
      /*printf( "##### escaped = [%s]\n", data->val );*/
    }
  }


  return data->origval == NULL ? data->val:data->origval;
}


/* ------------------------------------------------------------
 * AttrOp.getName()
 */
static const char* _getName( iOAttr inst ) {
  if( inst == NULL )
    return NULL;
  return Data(inst)->name;
}


/* ------------------------------------------------------------
 * AttrOp.setName()
 */
static void _setName( iOAttr inst, const char* name ) {
  iOAttrData data = Data(inst);
  if( data->name != NULL )
    StrOp.freeID( data->name, RocsAttrID );
  data->name = StrOp.dupID( name, RocsAttrID );
}


/* ------------------------------------------------------------
 * AttrOp.getVal()
 */
static const char* _getVal( iOAttr inst ) {
  iOAttrData data = Data(inst);
  if( data == NULL )
    return NULL;
  return __unescapeStr( inst );
}


/* ------------------------------------------------------------
 * AttrOp.getEscVal()
 */
static const char* _getEscVal( iOAttr inst ) {
  iOAttrData data = Data(inst);
  if( data == NULL )
    return NULL;
  return data->val;
}


/* ------------------------------------------------------------
 * AttrOp.setVal()
 */
static void _setVal( iOAttr inst, const char* val ) {
  iOAttrData data = Data(inst);
  __escapeStr( inst, val );
}


/* ------------------------------------------------------------
 * AttrOp.getInt()
 */
static int _getInt( iOAttr inst ) {
  iOAttrData data = Data(inst);
  if( data == NULL )
    return 0;
  return atoi( _getVal( inst ) );
}


/* ------------------------------------------------------------
 * AttrOp.setInt()
 */
static void _setInt( iOAttr inst, int val ) {
  iOAttrData data = Data(inst);
  char ival[256];
  sprintf( ival, "%d", val );
  if( data->val != NULL )
    StrOp.freeID( data->val, RocsAttrID );
  data->val = StrOp.dupID( ival, RocsAttrID );
}


/* ------------------------------------------------------------
 * AttrOp.getLong()
 */
static long _getLong( iOAttr inst ) {
  iOAttrData data = Data(inst);
  if( data == NULL )
    return 0;
  return atol( _getVal( inst ) );
}


/* ------------------------------------------------------------
 * AttrOp.setLong()
 */
static void _setLong( iOAttr inst, long val ) {
  iOAttrData data = Data(inst);
  char ival[256];
  sprintf( ival, "%ld", val );
  if( data->val != NULL )
    StrOp.freeID( data->val, RocsAttrID );
  data->val = StrOp.dupID( ival, RocsAttrID );
}


/* ------------------------------------------------------------
 * AttrOp.getBoolean()
 */
static Boolean _getBoolean( iOAttr inst ) {
  iOAttrData data = Data(inst);
  if( data == NULL )
    return False;
  if( StrOp.equalsi( data->val, "true" ) )
    return True;
  else if( StrOp.equalsi( data->val, "false" ) )
    return False;
  else {
    /* Attribute value not boolean. */
    return False;
  }
}


/* ------------------------------------------------------------
 * AttrOp.setBoolean()
 */
static void _setBoolean( iOAttr inst, Boolean val ) {
  iOAttrData data = Data(inst);
  char* bval = val==True ? "true":"false";
  if( data->val != NULL )
    StrOp.freeID( data->val, RocsAttrID );
  data->val = StrOp.dupID( bval, RocsAttrID );
}


/* ------------------------------------------------------------
 * AttrOp.getFloat()
 */
static double _getFloat( iOAttr inst ) {
  iOAttrData data = Data(inst);
  if( data == NULL )
    return 0;
  return atof( _getVal( inst ) );
}


/* ------------------------------------------------------------
 * AttrOp.setFloat()
 */
static void _setFloat( iOAttr inst, double val ) {
  iOAttrData data = Data(inst);
  char ival[256];
  sprintf( ival, "%f", val );
  if( data->val != NULL )
    StrOp.freeID( data->val, RocsAttrID );
  data->val = StrOp.dupID( ival, RocsAttrID );
}


/* ------------------------------------------------------------
 * AttrOp.instInt()
 */
static iOAttr _instInt( const char* name, int val ) {

  iOAttr attr = AttrOp.inst( name, "0" );
  AttrOp.setInt( attr, val );

  return attr;
}


/* ------------------------------------------------------------
 * AttrOp.inst()
 */
static iOAttr _inst( const char* name, const char* val ) {
  iOAttr     obj  = allocIDMem( sizeof( struct OAttr     ), RocsAttrID );
  iOAttrData data = allocIDMem( sizeof( struct OAttrData ), RocsAttrID );

  /* OBase operations */
  MemOp.basecpy( obj, &AttrOp, 0, sizeof( struct OAttr ), data );

  /* OAttrData */
  data->name = StrOp.dupID( name, RocsAttrID );
  if( val != NULL )
    __escapeStr( obj, val );

  instCnt++;

  return obj;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/attr.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
