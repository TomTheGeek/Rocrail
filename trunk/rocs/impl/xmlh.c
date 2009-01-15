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
#include "rocs/impl/xmlh_impl.h"

/* ------------------------------------------------------------
 * rocs interfaces.
 */
#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"

static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__del( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOXmlhData data = Data(inst);

    freeMem( data->buffer );
    ListOp.base.del( data->xmlList );
    ListOp.base.del( data->binList );
    NodeOp.base.del( data->xmlh );

    StrOp.free( data->header_tagname );
    StrOp.free( data->xml_tagname );
    StrOp.free( data->bin_tagname );
    StrOp.free( data->xmlh_begin );
    StrOp.free( data->xmlh_end );

    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return;
  }
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__serialize( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOXmlhData data = Data(inst);
    if( data->xmlh != NULL ) {
      char* s = NodeOp.toEscString( data->xmlh );
      int len = StrOp.len( s );
      int i = len - 1;
      /* Strip of all nonsense. */
      while( s[i] != '>' && i >= 0 ) {
        s[i] = '\0';
        i--;
      };
      if( size != NULL )
        *size = StrOp.len( s );
      return (unsigned char*)s;
    }
    else
      return NULL;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return NULL;
  }
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__deserialize( inst=0x%08X, a=0x%08X )", inst, bytestream );
  if( inst != NULL ) {
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return;
  }
}

static char* __toString( void* inst ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__toString( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOXmlhData data = Data(inst);
    if( data->xmlh != NULL ) {
      char* s = NodeOp.base.toString( data->xmlh );
      int len = StrOp.len( s );
      int i = len - 1;
      /* Strip of all nonsense. */
      while( s[i] != '>' && i >= 0 ) {
        s[i] = '\0';
        i--;
      };
      return s;
    }
    else
      return NULL;

  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return NULL;
  }
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "__properties( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOXmlhData data = Data(inst);
    return data->xmlh;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return NULL;
  }
}

/**----------------------------------------------------------------------
 * private __checkBufferSize()
 * ----------------------------------------------------------------------
 */
static void __checkBufferSize( iOXmlh inst, int size ) {
  iOXmlhData data = Data(inst);
  if( data->bufferIdx + size >= XmlhOp.initAllocSize ) {
    int n = size / XmlhOp.initAllocSize;
    n++;
    data->bufferSize += XmlhOp.initAllocSize * n;
    data->buffer = reallocMem( data->buffer, data->bufferSize );
  }
}


static void __fillBinList( iOXmlh inst ) {
  iOXmlhData data = Data(inst);
  iONode binNode = NodeOp.findNode( data->xmlh, data->bin_tagname );
  while( binNode != NULL ) {
    ListOp.add( data->binList, (obj)binNode );
    binNode = NodeOp.findNextNode( data->xmlh, binNode );
  }
}


/**----------------------------------------------------------------------
 * private __parseHdr()
 * ----------------------------------------------------------------------
 */
static Boolean __parseHdr( iOXmlh inst ) {
  iOXmlhData data = Data(inst);
  iODoc doc = DocOp.parse( (char*)data->buffer );
  if( doc != NULL ) {
    char* str = NULL;

    data->xmlh = DocOp.getRootNode( doc );
    doc->base.del( doc );

    if( data->xmlh != NULL ) {
      str = NodeOp.toEscString( data->xmlh );
      TraceOp.trc( name, TRCLEVEL_XMLH, __LINE__, 9999, str );
      /* Cleanup. */
      StrOp.free( str );

      __fillBinList( inst );
      return True;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Invalid Xmlh! (NULL after parse.)" );
      return False;
    }

  }
  return False;
}


/** ----- OXmlh ----- */


/** Add. */
static void _addNode( struct OXmlh* inst ,iONode node ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999,
               "_addNode( inst=0x%08X, node=0x%08X )", inst, node );
  if( inst != NULL && node != NULL ) {
    iOXmlhData data = Data(inst);
    NodeOp.addChild( data->xmlh, node );
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL || node == NULL!" );
    return;
  }
}


/** Get filename for bin block by index. */
static const char* _getBinName( struct OXmlh* inst ,int idx ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999,
               "_getBinName( inst=0x%08X, dataIdx=%d )", inst, idx );
  if( inst != NULL ) {
    iOXmlhData data = Data(inst);
    if( data->binList != NULL ) {
      iONode binNode = (iONode)ListOp.get( data->binList, idx );
      if( binNode != NULL ) {
        const char* l_name = NodeOp.getStr( binNode, "filename", NULL );
        if( l_name != NULL )
          return l_name;
        return NodeOp.getStr( binNode, "name", NULL );
      }
    }
    return NULL;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL || buffer == NULL!" );
    return NULL;
  }
}


/** Get size of bin block by index. */
static long _getBinSize( struct OXmlh* inst ,int idx ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999,
               "_getBinSize( inst=0x%08X, dataIdx=%d )", inst, idx );
  if( inst ) {
    long binsize = 0;
    iOXmlhData data = Data(inst);

    if( data->binList ) {
      iONode binNode = (iONode)ListOp.get( data->binList, idx );
      if( binNode )
        binsize = NodeOp.getInt( binNode, "size", 0 );
    }
    return binsize;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL" );
    return 0;
  }

}


/** Get size of block by name. */
static int _getSizeByTagName( struct OXmlh* inst ,const char* name ,int idx ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "_getSizeByTagName( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOXmlhData data = Data(inst);
    if( data->xmlh != NULL ) {
      iONode node = NodeOp.findNode( data->xmlh, name );
      if( node != NULL )
        return NodeOp.getInt( node, "size", 0 );
    }
    return 0;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL || buffer == NULL!" );
    return 0;
  }
}


static iONode _getNodeByTagName( struct OXmlh* inst ,const char* name ,int idx ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "_getSizeByTagName( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOXmlhData data = Data(inst);
    if( data->xmlh != NULL ) {
      iONode node = NodeOp.findNode( data->xmlh, name );
      return node;
    }
    return NULL;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL || buffer == NULL!" );
    return NULL;
  }
}


/** Get node for xml block by index. */
static const char* _getXmlName( struct OXmlh* inst ,int dataIdx ) {
  return 0;
}


/** Get size of xml block by index. */
static int _getXmlSize( struct OXmlh* inst ,int idx ) {
  return 0;
}


/** Get number of bin nodes. */
static int _hasBin( struct OXmlh* inst ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999,
               "_hasBin( inst=0x%08X )", inst );
  if( inst != NULL ) {
    iOXmlhData data = Data(inst);
    if( data->binList != NULL ) {
      return ListOp.size( data->binList );
    }
    return 0;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL || buffer == NULL!" );
    return 0;
  }
}


/** Get number of xml nodes. */
static int _hasXml( struct OXmlh* inst ) {
  return 0;
}


/** Object creator. */
static struct OXmlh* _inst( Boolean create ,const char* header_tagname ,const char* bin_tagname ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "_inst( void )" );
  {
    iOXmlh __Xmlh = allocMem( sizeof( struct OXmlh ) );
    iOXmlhData data = allocMem( sizeof( struct OXmlhData ) );

    MemOp.basecpy( __Xmlh, &XmlhOp, 0, sizeof( struct OXmlh ), data );

    data->buffer     = allocMem( XmlhOp.initAllocSize );
    data->bufferIdx  = 0;
    data->bufferSize = XmlhOp.initAllocSize;
    data->xmlList   = ListOp.inst();
    data->binList   = ListOp.inst();

    if( header_tagname != NULL )
      data->header_tagname = StrOp.dup( header_tagname );
    else
      data->header_tagname = StrOp.dup( XmlhOp.header_tagname );

    if( bin_tagname != NULL )
      data->bin_tagname = StrOp.dup( bin_tagname );
    else
      data->bin_tagname = StrOp.dup( XmlhOp.bin_tagname );

    data->xmlh_begin = StrOp.fmt( "<%s", data->header_tagname );
    data->xmlh_end = StrOp.fmt( "</%s>", data->header_tagname );

    if( create ) {
      data->xmlh = NodeOp.inst( data->header_tagname, NULL, ELEMENT_NODE );
    }

    instCnt++;
    return __Xmlh;
  }
}


/** Get error state of Xmlh. */
static Boolean _isError( struct OXmlh* inst ) {
  if( inst != NULL ) {
    iOXmlhData data = Data(inst);
    return data->error;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
    return False;
  }
  return False;
}


static void _reset( struct OXmlh* inst ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999, "_reset( inst=0x%08X )", inst );

  if( inst != NULL ) {
    iOXmlhData data = Data(inst);

    freeMem( data->buffer );
    data->buffer     = allocMem( XmlhOp.initAllocSize );
    data->bufferIdx  = 0;
    data->bufferSize = XmlhOp.initAllocSize;

    data->error = False;
    data->beginHdr = False;
    data->endHdr = False;
    ListOp.clear( data->binList );

    if( data->xmlh != NULL ) {
      NodeOp.base.del( data->xmlh );
      data->xmlh = NULL;
    }
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL!" );
  }
}


/** Reads the Xmlh from buffer. True if complete header is read. */
static Boolean _read( struct OXmlh* inst ,const byte* buffer ,int size ) {
  TraceOp.trc( name, TRCLEVEL_METHOD, __LINE__, 9999,
               "_read( inst=0x%08X, buffer=0x%08X )", inst, buffer );
  if( inst != NULL && buffer != NULL ) {
    iOXmlhData data = Data(inst);

    __checkBufferSize( inst, size );
    MemOp.copy( data->buffer + data->bufferIdx, buffer, size );
    data->bufferIdx += size;

    if( data->beginHdr &! data->endHdr ) {
      if( StrOp.find( (char*)data->buffer, data->xmlh_end ) != NULL ) {
        TraceOp.trc( name, TRCLEVEL_XMLH, __LINE__, 9999, "End tag of xmlh detected." );
        data->endHdr = True;
        if( !__parseHdr( inst ) ) {
          /* Error in xmlh. */
          data->error = True;
          return False;
        }
      }
    }
    else {
      char* l_Start = MemOp.chr( data->buffer, '<', data->bufferIdx );
      while( l_Start != NULL && !data->beginHdr ) {
        data->beginHdr = MemOp.cmp( l_Start, data->xmlh_begin, StrOp.len( data->xmlh_begin ) );
        if( data->beginHdr )
          TraceOp.trc( name, TRCLEVEL_XMLH, __LINE__, 9999, "Begin tag of xmlh detected." );
        else {
          long dif = l_Start - (char*)data->buffer;
          if( dif + 1 < data->bufferIdx )
            l_Start = MemOp.chr( l_Start+1, '<', data->bufferIdx - dif );
          else
            l_Start = NULL;
        }
      }
    }


    return data->endHdr;
  }
  else { /* NULL */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "inst == NULL || buffer == NULL!" );
    return False;
  }
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/xmlh.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
