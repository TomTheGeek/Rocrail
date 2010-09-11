/*
 Rocs - OS independent C library

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/node.h"
#include "rocs/public/system.h"
#include "rocs/impl/doc_impl.h"

static int instCnt = 0;
static Boolean unicodeescapes = False;
static Boolean htmlescapes = False;
static Boolean ignorecase = True;
static Boolean xmlprolog = True;
static const char* docencoding = "UTF-8";
static Boolean utf8encoding = True;
static Boolean utf2latin = False;

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/*
 ***** OBase operations.
 */
static const char* __name(void) {
  return name;
}

static iODoc _parse( const char* xml );
static void __deserialize(void* inst, unsigned char* a) {
  iODoc doc = _parse( (char*)a );
  iODocData newdata = Data(doc);
  iODocData data    = Data(inst);

  /* Cleanup */
  data->doc->base.del(data->doc);
  data->root->base.del(data->root);

  /* Set new data */
  data->doc  = newdata->doc;
  data->root = newdata->root;

  freeIDMem( newdata, RocsDocID );
  freeIDMem( doc, RocsDocID );
}

static char* __toStr( iONode n, int level, Boolean escaped );
static char* __toString(void* inst) {
  return __toStr( Data(inst)->doc, 0, False );
}

static unsigned char* __serialize(void* inst, long* size) {
  const char* s = __toString( inst );
  *size = StrOp.len( s );
  return (unsigned char*)s;
}

static void __del(void* inst) {
  iODocData data = Data(inst);
  data->doc->base.del(data->doc);
  /* o->root will notbe deleted. */
  freeIDMem( data, RocsDocID );
  freeIDMem( inst, RocsDocID );
  instCnt--;
}
static int __count(void) {
  return instCnt;
}
static void* __properties(void* inst) {
  iODocData data = Data(inst);
  return data->doc;
}
static struct OBase* __clone( void* inst ) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}


/**
 *
 */
static char* _node2String( iONode node, Boolean escaped ) {
  if( node == NULL )
    return "";
  return __toStr( node, 0, escaped );
}

static char* __toStr( iONode n, int level, Boolean escaped ) {
  int      i = 0;
  int  ident = 0;
  char*  str = NULL;
  char* str2 = NULL;
  char*  fmt = NULL;

  if( level == 0 && escaped && xmlprolog ) {
    char* p = DocOp.getEncodingProperty();
    str = StrOp.catID( str, p, RocsDocID );
    str = StrOp.catID( str, "\n", RocsDocID );
    StrOp.free( p );
  }

  for( ident = 0; ident < level; ident++ )
    str = StrOp.catID( str, "  ", RocsDocID );

  fmt = StrOp.fmtID( RocsDocID, "<%s", NodeOp.getName(n) );
  str = StrOp.catID( str, fmt, RocsDocID );
  StrOp.freeID( fmt, RocsDocID );
  for( i = 0; i < NodeOp.getAttrCnt(n); i++ ) {
    iOAttr a = NodeOp.getAttr(n,i);
    long len;
    fmt = escaped ? (char*)AttrOp.base.serialize(a,&len) : AttrOp.base.toString(a);
    /*fmt = StrOp.fmtID( RocsDocID, " %s=\"%s\"", AttrOp.getName(a), escaped ? AttrOp.getEscVal(a):AttrOp.getVal(a) );*/
    str = StrOp.catID( str, " ", RocsDocID );
    str = StrOp.catID( str, fmt, RocsDocID );
    StrOp.free( fmt );
  }
  if( NodeOp.getChildCnt(n) == 0 ) {
    str = StrOp.catID( str, "/>\n", RocsDocID );
    if( level == 0 ) {
      str2 = StrOp.dup( str );
      StrOp.freeID( str, RocsDocID );
      return str2;
    }
    else
      return str;
  }
  else
    str = StrOp.catID( str, ">\n", RocsDocID );

  for( i = 0; i < NodeOp.getChildCnt(n); i++ ) {
    iONode child = NodeOp.getChild(n,i);
    fmt = __toStr( child, level + 1, escaped );
    str = StrOp.catID( str, fmt, RocsDocID );
    StrOp.freeID( fmt, RocsDocID );
  }

  for( ident = 0; ident < level; ident++ )
    str = StrOp.catID( str, "  ", RocsDocID );

  fmt = StrOp.fmtID( RocsDocID, "</%s>\n", NodeOp.getName(n) );
  str = StrOp.catID( str, fmt, RocsDocID );
  StrOp.freeID( fmt, RocsDocID );

  if( level == 0 ) {
    str2 = StrOp.dup( str );
    StrOp.freeID( str, RocsDocID );
    return str2;
  }
  else
    return str;
}


static void __setUTF8Encoded( iODoc inst, Boolean utf8 ) {
  iODocData data = Data(inst);
  data->utf8 = utf8;
}

static Boolean __isUTF8( iONode prop ) {
  Boolean utf8 = False;
  if( StrOp.equals( "property", NodeOp.getName( prop ) ) ) {
    const char* s = NodeOp.getStr( prop, "text", "?" );
    if( StrOp.equalsn( "xml ", s, 4 ) ) {
      const char* encoding = strstr( s, "encoding" );
      if( encoding != NULL && strstr( s, "UTF-8" ) != NULL )
        utf8 = True;
    }
  }
  return utf8;
}


/**
 * Skip everything which couldbe appear between > and <.
 */
static Boolean __skip( const char* s, int* pIdx ) {
  while( s[*pIdx] != '\0' && s[*pIdx] <= ' ' ) {
    *pIdx+=1;
  }
  if( s[*pIdx] != '\0' )
    return True;
  else {
    return False;
  }
}


/**
 * Skip to next given character.
 */
static Boolean __skipTo( const char* s, int* pIdx, char c, iONode parent ) {
  int i = 0;
  char val [MaxAttrValLen ] = {'\0'};

  TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "__skipTo:1 Now pointing at %d [%c][%-10.10s]", *pIdx, s[*pIdx], &s[*pIdx] );
  while( s[*pIdx] != '\0' && s[*pIdx] != c ) {
    val[i] = s[*pIdx];
    i++;
    val[i] = '\0';
    *pIdx+=1;
  }
  if( s[*pIdx] != '\0' && s[*pIdx] == c ) {
    if( strlen( val ) > 0 && parent != NULL ) {
      /*
      iONode n = NodeOp.inst( "tnode", parent, TEXT_NODE );
      NodeOp.addAttr( n, AttrOp.inst( "val", val ) );
      NodeOp.addChild( parent, n );
      */
    }
    return True;
  }
  else {
    /* Could be eof. */
    /*TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "Parser error at %d: [%c] expected but [0x%02X] found", *pIdx, (int)c, s[*pIdx] );*/
    return False;
  }
}


/**
 * Skip to next given character.
 */
static Boolean __skipToNode( const char* s, int* pIdx, iONode parent ) {
  return __skipTo( s, pIdx, '<', parent );
}


static char* __decode( iODoc doc, const char* val ) {
  Boolean utf8 = DocOp.isUTF8Encoded( doc );
  char* decval = NULL;
  if( utf8 && utf2latin )
    decval = SystemOp.utf2latin( val );
  else
    decval = StrOp.dup( val );
  return decval;
}

static iONode __parseNodeName( const char* s, int* pIdx, iONode parent, iODoc doc ) {
  char nodeName[MaxNodeNameLen];
  int i = 0;
  int proplen = 0;
  const char* prop = NULL;
  iONode newnode = NULL;
  Boolean isRemark   = False;
  Boolean isProperty = False;
  Boolean isVar      = False;

  TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "__parseNodeName:1 Now pointing at %d [%c][%-10.10s]", *pIdx, s[*pIdx], &s[*pIdx] );
  if( s[*pIdx] != '<' ) {
    TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "__parseNodeName: s[*pIdx] != '<' s[*pIdx] = [%c]", s[*pIdx] );
    if( !__skipToNode( s, pIdx, parent ) )
      return NULL;
  }

  TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "__parseNodeName:2 Now pointing at %d [%c][%-10.10s]", *pIdx, s[*pIdx], &s[*pIdx] );
  if( StrOp.equalsni( &s[*pIdx], remToken, strlen( remToken ) ) ) {
    char* pEndToken = strstr( &s[*pIdx], remEndToken );
    if( pEndToken != NULL ) {
      int delta = pEndToken - &s[*pIdx];
      *pIdx += delta;
    }
    else {
      /* Error */
      *pIdx += 4;
    }
    TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "Remark found." );
    isRemark = True;
  }
  else if( StrOp.equalsni( &s[*pIdx], propToken, strlen( propToken ) ) ) {
    char* pEndToken = strstr( &s[*pIdx], propEndToken );
    if( pEndToken != NULL ) {
      int delta = pEndToken - &s[*pIdx];
      prop = &s[*pIdx] + strlen( propToken );
      proplen = delta;
      *pIdx += delta;
    }
    else {
      /* Error */
      *pIdx += 2;
    }
    TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "Property found." );
    isProperty = True;
  }
  else if( StrOp.equalsni( &s[*pIdx], varToken, strlen( varToken ) ) ) {
    char* pEndToken = strstr( &s[*pIdx], varEndToken );
    if( pEndToken != NULL ) {
      int delta = pEndToken - &s[*pIdx];
      *pIdx += delta;
    }
    else {
      /* Error */
      *pIdx += 2;
    }
    isVar = True;
    TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "Variable found." );
  }
  else {
    *pIdx += 1;
  }

  if( isRemark && s[*pIdx] == '-' ) {
    if( StrOp.equalsni( &s[*pIdx], remEndToken, strlen( remEndToken ) ) ) {
      iONode n = NodeOp.inst( "remark", parent, REMARK_NODE );
      nodeName[i] = '\0';
      NodeOp.addAttr( n, AttrOp.inst( "text", nodeName ) );
      *pIdx+=strlen( remEndToken );
      TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "REMARK_NODE created." );
      return n;
    }
  }
  else if( isProperty && s[*pIdx] == '?' ) {
    if( StrOp.equalsni( &s[*pIdx], propEndToken, strlen( propEndToken ) ) ) {
      iONode n = NodeOp.inst( "property", parent, PROPERTY_NODE );
      if( proplen > 1 ) {
        strncpy( nodeName, prop, proplen );
        nodeName[proplen-2] = '\0';
      }
      else
        nodeName[i] = '\0';
      NodeOp.addAttr( n, AttrOp.inst( "text", nodeName ) );
      if( __isUTF8( n ) )
        __setUTF8Encoded( doc, True );
      *pIdx+=strlen( propEndToken );
      return n;
    }
  }
  else if( isVar ) {
    if( StrOp.equalsni( &s[*pIdx], varEndToken, strlen( varEndToken ) ) ) {
      iONode n = NodeOp.inst( "variable", parent, VARIABLE_NODE );
      nodeName[i] = '\0';
      NodeOp.addAttr( n, AttrOp.inst( "text", nodeName ) );
      *pIdx+=strlen( varEndToken );
      return n;
    }
  }

  TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "__parseNodeName:3 Now pointing at %d [%c][%-10.10s]", *pIdx, s[*pIdx], &s[*pIdx] );
  while( s[*pIdx] != 0 && s[*pIdx] != '>' && s[*pIdx] != '/' && i < MaxNodeNameLen-1 ) {

    if( s[*pIdx] <= ' ' )
      break;
    nodeName[i] = s[*pIdx];
    i++;
    *pIdx += 1;
  }

  TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "__parseNodeName:4 Now pointing at %d [%c][%-10.10s]", *pIdx, s[*pIdx], &s[*pIdx] );
  if( s[*pIdx] == '\0' )
    return NULL;

  nodeName[i] = '\0';
  TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "__parseNodeName = [%s]", nodeName );


  newnode = NodeOp.inst( nodeName, parent, ELEMENT_NODE );
  TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "ELEMENT_NODE created." );

  return newnode;
}



static iOAttr __parseAttribute( const char* s, int* pIdx, iODoc doc ) {
  char  attrName[MaxAttrNameLen];
  int   valAlloc = 0;
  char* val      = NULL;
  int   i        = 0;

  /* get attributename */
  while( s[*pIdx] != 0 && s[*pIdx] != '>' && s[*pIdx] != '/' && s[*pIdx] > ' ' && s[*pIdx] != '=' && s[*pIdx] != '\"' && i < MaxAttrNameLen-1 ) {
    attrName[i] = s[*pIdx];
    i++;
    *pIdx += 1;
  }
  if( i == 0 ) {
    return NULL;
  }
  attrName[i] = 0;
  /*TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "attrName = [%s]", attrName );*/

  __skip( s, pIdx );

  /* check for value */
  
  val = allocIDMem( MaxAttrValLen, RocsDocID );
  valAlloc = MaxAttrValLen;
  
  i = 0;
  val[i] = 0;
  if( s[*pIdx] == '=' ) {
    *pIdx+=1;
    if( s[*pIdx] == '\"' ) {
      *pIdx+=1;
      while( s[*pIdx] != 0 && s[*pIdx] != '\"' && i < valAlloc-1 ) {
        val[i] = s[*pIdx];
        i++;
        *pIdx+=1;
        if(i >= valAlloc-1) {
          /* increase the value size to max. 100KB */
          if( valAlloc < (MaxAttrValLen * 100) ) {
            valAlloc += MaxAttrValLen;
            val = reallocMem( val, valAlloc );
          }
        }
      }
      if( !(s[*pIdx] == '\"') ) {
        /* (AS, 19.8.2003: adding more exception info) */
        /* The while loop finished, but not because of the quotation-mark-condition. */
        /* Either we ran into a string-terminating zero or we are over the MaxAttrValLen: */
        if (i >= valAlloc-1)
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__,9999, "Parser error at %d: attribut value exceeds the maximum length of %d", *pIdx, valAlloc );
        else if (s[*pIdx] == 0)
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__,9999, "Parser error at %d: encountered string-termination-symbol while reading an attribut value.", *pIdx );
        else
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__,9999, "Parser error at %d: [%c] expected but [%c] found", *pIdx, '\"', s[*pIdx] );
        /* Problem: this trace will be cut if (s[*pIdx] == 0) */
        /* solution: catch this case earlier */
        /* in any case: */
        freeIDMem(val, RocsDocID);
        return NULL;
      }
      *pIdx+=1;
      val[i] = 0;
      TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "val = [%s]", val );
    }
  }

  { /* Decode the attribute value into ISO-8859-15: */
    char* decval = __decode( doc, val );
    iOAttr a = AttrOp.inst( attrName, decval );
    StrOp.free( decval );
    freeIDMem(val, RocsDocID);
    return a;
  }
}


/**
 *
 */
static iONode __parse( const char* s, int* pIdx, int level, iONode parent, int* pErr, iODoc doc ) {
  iONode thisNode = NULL;
  int  idx      = 0;
  int idxLoop   = 0;

  if( pIdx != NULL )
    idx = *pIdx;

  TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "ParseLevel = %d", level );

  thisNode = __parseNodeName( s, &idx, parent, doc );
  if( thisNode == NULL )
    return NULL;
  if( NodeOp.getType( thisNode ) == REMARK_NODE   ||
      NodeOp.getType( thisNode ) == PROPERTY_NODE ||
      NodeOp.getType( thisNode ) == VARIABLE_NODE )
  {
    if( pIdx != NULL )
      *pIdx = idx;
    return thisNode;
  }

  /* get attributes and childnodes */
  do {
    iOAttr a = NULL;
    idxLoop = idx;
    /* skip whitespace */
    if( !__skip( s, &idx ) ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Parser error at %d: unexpected eof!", idx );
      *pErr = 1;
      return NULL;
    }
    a = __parseAttribute( s, &idx, doc );
    if( a != NULL )
      NodeOp.addAttr( thisNode, a );

    if( s[idx] == '>' ) {
      int idxTest = -1;
      /* Looking for childnodes */
      idx++;
      TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "_parse:0 Now pointing at %d [%c][%-10.10s]", idx, s[idx], &s[idx] );
      /*__skip( s, &idx );*/
      __skipToNode( s, &idx, thisNode );
      while( s[idx] == '<' && s[idx+1] != '/') {
        iONode child = NULL;
        TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "Looking for childnodes..." );
        if( idxTest == idx ) {
          TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "idxTest(%d) == idx(%d)", idxTest, idx );
          return NULL;
        }
        child = __parse( s, &idx, level + 1, thisNode, pErr, doc );
        if( child == NULL || *pErr != 0 )
          return NULL;
        NodeOp.addChild( thisNode, child );
        idxTest = idx;
        if( !__skipToNode( s, &idx, thisNode ) ) {
          return NULL;
        }
        /* New node start is found; reset idxTest. */
        idxTest = -1;
        TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "_parse:1 Now pointing at %d [%c][%-10.10s]", idx, s[idx], &s[idx] );
      };
      TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "_parse:2 Now pointing at %d [%c][%-10.10s]", idx, s[idx], &s[idx] );
    }
    else if( s[idx] == '/' && s[idx+1] == '>' ) {
      /* End of node */
      TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "End (inline) of node [%s].", NodeOp.getName( thisNode ) );
      idx+=2;
      if( pIdx != NULL )
        *pIdx = idx;
      return thisNode;
    }
    else if( NodeOp.getType( thisNode ) == PROPERTY_NODE && s[idx] == '?' && s[idx+1] == '>' ) {
      /* End of node */
      TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "End (inline) of node [%s].", NodeOp.getName( thisNode ) );
      idx+=2;
      if( pIdx != NULL )
        *pIdx = idx;
      return thisNode;
    }

    /* skip whitespace */
    if( !__skip( s, &idx ) ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Parser error at %d: unexpected eof!", idx );
      *pErr = 1;
      return NULL;
    }

    TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "_parse:3 Now pointing at %d [%c][%-10.10s]\n", idx, s[idx], &s[idx] );
    if( s[idx] == '<' && s[idx+1] == '/' && StrOp.equalsni( &s[idx+2], NodeOp.getName( thisNode ), strlen( NodeOp.getName( thisNode ) ) ) ) {
      /* End of node */
      idx+=2;
      idx+=strlen( NodeOp.getName( thisNode ) );
      TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "End of node [%s].", NodeOp.getName( thisNode ) );
      if( pIdx != NULL )
        *pIdx = idx;
      return thisNode;
    }

  } while( s[idx] != '\0' && idx != idxLoop );

  /* Error? */
  TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Parser error at position [%d]!", idx );
  *pErr = 1;
  if( pIdx != NULL )
    *pIdx = idx;
  return NULL;
}


/**
 *
 */
static iONode _getDocNode( iODoc inst ) {
  return Data(inst)->doc;
}

static iONode _getRootNode( iODoc inst ) {
  return Data(inst)->root;
}

static const char* _getStr(iODoc inst,const char* nodeName,const char* attrName, const char* defaultVal) {
  iODocData data = Data(inst);
  if( data->root == NULL )
    return defaultVal;
  else {
    iONode node = NodeOp.findNode( data->root, nodeName );
    if( node == NULL )
      return defaultVal;
    else {
      return NodeOp.getStr( node, attrName, defaultVal );
    }
  }
}

static int _getInt(iODoc inst,const char* nodeName,const char* attrName, int defaultVal) {
  iODocData data = Data(inst);
  if( data->root == NULL )
    return defaultVal;
  else {
    iONode node = NodeOp.findNode( data->root, nodeName );
    if( node == NULL )
      return defaultVal;
    else {
      return NodeOp.getInt( node, attrName, defaultVal );
    }
  }
}

static Boolean _getBool(iODoc inst,const char* nodeName,const char* attrName, Boolean defaultVal) {
  iODocData data = Data(inst);
  if( data->root == NULL )
    return defaultVal;
  else {
    iONode node = NodeOp.findNode( data->root, nodeName );
    if( node == NULL )
      return defaultVal;
    else {
      return NodeOp.getBool( node, attrName, defaultVal );
    }
  }
}


/**
 *
 */
static iODoc _parse( const char* xml ) {
  int i = 0;
  iODoc     doc  = allocIDMem( sizeof( struct ODoc ), RocsDocID );
  iODocData data = allocIDMem( sizeof( struct ODocData ), RocsDocID );

  iONode docNode   = NULL;
  iONode childNode = NULL;
  iONode rootNode  = NULL;

  int Err = 0;

  if( StrOp.len( xml ) == 0 )
    return NULL;

  docNode   = NodeOp.inst( "document", NULL, ELEMENT_NODE );
  instCnt++;

  MemOp.basecpy( doc, &DocOp, 0, sizeof( struct ODoc ), data );

  data->doc = docNode;

  TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "Parsing started, input: %-20.20s...", xml );

  do {
    childNode = __parse( xml, &i, 1, NULL, &Err, doc );
    if( childNode != NULL ) {
      if( NodeOp.getType( childNode ) == ELEMENT_NODE && rootNode == NULL ) {
        TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "Found rootnode [%s].",
                     NodeOp.getName(childNode) );
        rootNode = childNode;
        /* root is not a child of doc; you can delete it separately. */
      }
      else {
        TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "Adding [%s][type=%d] to document.",
                     NodeOp.getName(childNode), NodeOp.getType( childNode ) );
        NodeOp.addChild( docNode, childNode );
      }
    }
    else
      TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999, "childNode == NULL" );
  } while( childNode != NULL && Err == 0 );

  if( Err == 0 ) {
    data->root = rootNode;
    return doc;
  }
  else {
    docNode->base.del( docNode );
    if( rootNode != NULL )
      rootNode->base.del( rootNode );
    return NULL;
  }
}

static void _setEncoding( const char* enc ) {
  docencoding = enc;
  utf8encoding = StrOp.equals( DocOp.ENC_UTF8, docencoding );
}
static const char* _getEncoding( void ) {
  return docencoding;
}
static char* _getEncodingProperty( void ) {
  return StrOp.fmt( "<?xml version=\"1.0\" encoding=\"%s\"?>", docencoding );
}
static Boolean _isUTF8Encoding( void ) {
  return utf8encoding;
}
static Boolean _isUTF2Latin( void ) {
  return utf2latin;
}
static void _setUTF2Latin( Boolean decode ) {
  utf2latin = decode;
}
static void _setUniCodeEscapes( Boolean uni ) {
  unicodeescapes = uni;
}
static void _setHTMLEscapes( Boolean html ) {
  htmlescapes = html;
}
static Boolean _isUniCodeEscapes( void ) {
  return unicodeescapes;
}
static Boolean _isHTMLEscapes( void ) {
  return htmlescapes;
}
static void _setIgnoreCase( Boolean ignore ) {
  ignorecase = ignore;
}
static Boolean _isIgnoreCase( void ) {
  return ignorecase;
}
static void _setXMLProlog( Boolean prolog ) {
  xmlprolog = prolog;
}


static Boolean _isUTF8Encoded( iODoc inst ) {
  iODocData data = Data(inst);
  return data->utf8;
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/doc.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
