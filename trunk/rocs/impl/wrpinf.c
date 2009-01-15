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
#include "rocs/impl/wrpinf_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/doc.h"
#include "rocs/public/list.h"

static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOWrpInfData data = Data(inst);
    /* Cleanup data->xxx members...*/
    MapOp.base.del( data->wrpMap );

    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  return;
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void* inst, long* size ) {
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  return;
}

static char* __toString( void* inst ) {
  return NULL;
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
  return NULL;
}

/** ----- OWrpInf ----- */

static void __scanWrapper( iONode wrpNode, iOMap map ) {
  int cnt = NodeOp.getChildCnt( wrpNode );
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iONode wrp = NodeOp.getChild( wrpNode, i );
    const char* wrpname = NodeOp.getName( wrp );
    if( !StrOp.equals( "var", wrpname ) && !StrOp.equals( "const", wrpname ) ) {
      MapOp.put( map, wrpname, (obj)wrp );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Wrapper info \"%s\" added to map.", wrpname );
      __scanWrapper( wrp, map );
    }
  }
}


/** Get a message by key. */
static void __initMap( struct OWrpInf* inst ) {
  iOWrpInfData data = Data(inst);
  int i = 0;
  data->wrpMap = MapOp.inst();
  for( i = 0; i < data->cnt; i++ ) {
    iODoc doc = DocOp.parse( data->xmlStrs[i] );
    if( doc != NULL ) {
      iONode root = DocOp.getRootNode( doc );
      if( root != NULL ) {
        __scanWrapper( root, data->wrpMap );
        root->base.del( root );
      }
      doc->base.del( doc );
    }
  }
}


/** Creates a resource object. */
static struct OWrpInf* _inst( const char** xmls, int cnt ) {
  iOWrpInf __WrpInf = allocMem( sizeof( struct OWrpInf ) );
  iOWrpInfData data = allocMem( sizeof( struct OWrpInfData ) );
  MemOp.basecpy( __WrpInf, &WrpInfOp, 0, sizeof( struct OWrpInf ), data );

  /* Initialize data->xxx members... */
  data->xmlStrs = xmls;
  data->cnt = cnt;
  __initMap( __WrpInf );


  instCnt++;
  return __WrpInf;
}


/** Get a wrapper by key. */
static iONode _getWrapper( struct OWrpInf* inst ,const char* wrpname ) {
  iOWrpInfData data = Data(inst);
  iONode wrp = (iONode)MapOp.get( data->wrpMap, wrpname );
  if( wrp != NULL ) {
    return wrp;
  }
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Wrapper Info \"%s\" not found.", wrpname );
  return NULL;
}


static iOList _getVars( iONode wrp ) {
  if( wrp != NULL ) {
    iOList list = ListOp.inst();
    int cnt = NodeOp.getChildCnt( wrp );
    int i = 0;
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( wrp, i );
      if( StrOp.equals( "var", NodeOp.getName( child ) ) )
        ListOp.add( list, (obj)child );
    }
    return list;
  }
  return NULL;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/wrpinf.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
