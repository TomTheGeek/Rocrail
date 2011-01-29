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

#include "rocs/impl/node_impl.h"
#include "rocs/public/doc.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"


static int instCnt = 0;

/*
 ***** OBase operations.
 */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static const char* __name(void) {
  return name;
}
static void __delData(void* inst) {
  iONodeData data = Data(inst);
  int i;
  int attrCnt  = data->attrCnt;
  int childCnt = data->childCnt;

  /* delete all attributes */
  for( i = 0; i < attrCnt; i++ ) {
    obj attr = (obj)data->attrs[i];
    attr->del(attr);
  }
  /* delete all childs */
  for( i = 0; i < childCnt; i++ ) {
    obj child = (obj)data->childs[i];
    /*printf( "$$Deleting child[%d of %d]\n", i, childCnt );*/
    child->del( child );
  }

  if( data->parent != NULL ) {
    /*NodeOp.removeChild( data->parent, inst );*/
  }

  MapOp.base.del( data->attrmap );
  StrOp.freeID( data->name, RocsNodeID );
  freeIDMem( data->attrs, RocsNodeID );
  freeIDMem( data->childs, RocsNodeID );
  freeIDMem( data, RocsNodeID );
}
static unsigned char* __serialize(void* inst, long* size) {
  char* s = DocOp.node2String( (iONode)inst, True );
  *size = StrOp.len( s );
  return (unsigned char*)s;
}
static void __deserialize(void* inst, unsigned char* a) {
  iONode node = (iONode)inst;
  iODoc doc = DocOp.parse( (char*)a );
  __delData( inst );
  node->base.data = DocOp.getRootNode( doc )->base.data;
  doc->base.del( doc );
}
static char* __toString(void* inst) {
  return DocOp.node2String( (iONode)inst, False );
}
static void __del(void* inst) {
  if( inst != NULL ) {
    iONodeData data = Data(inst);
    /*
    char* str = __toString( inst );
    printf( "DELETING childs=%d:\n%s\n", data->childCnt, str );
    freeMem( str );
    */
    __delData( inst );
    freeIDMem( inst, RocsNodeID );
    instCnt--;
  }
}
static int __count(void) {
  return instCnt;
}
static iOBase __clone_original( void* inst ) {
  iONode node = inst;
  char* str = (char*)NodeOp.toEscString( node );
  iODoc doc = DocOp.parse( str );
  iONode clone = NULL;
  if( doc == NULL )
    return NULL;
  clone = DocOp.getRootNode( doc );
  doc->base.del(doc);
  StrOp.free( str );
  return (iOBase)clone;
}

static iOBase __clone( void* inst ) {
  iONode node  = inst;
  iONode clone = NodeOp.inst( NodeOp.getName( node ), NULL, ELEMENT_NODE );
  int attrcnt  = NodeOp.getAttrCnt( node );
  int childcnt = NodeOp.getChildCnt( node );
  int i = 0;
  for( i = 0; i < attrcnt; i++ ) {
    iOAttr a = NodeOp.getAttr( node, i );
    NodeOp.addAttr( clone, (iOAttr)a->base.clone( a ) );
  }
  for( i = 0; i < childcnt; i++ ) {
    iONode n = NodeOp.getChild( node, i );
    NodeOp.addChild( clone,(iONode) n->base.clone( n ) );
  }
  return (iOBase)clone;
}


static void* __properties(void* inst) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

/*
 ***** ONode operations.
 */
static char* _toEscString( iONode inst ) {
  return DocOp.node2String( inst, True );
}

static void _setParent( iONode inst, iONode parent ) {
  iONodeData data = Data(inst);
  data->parent = parent;
}

static void _addChild( iONode inst, iONode child ) {
  iONodeData data = Data(inst);
  if( child == NULL )
    return;
  if( data->childs == NULL )
    data->childs = allocIDMem( (data->childCnt+1) * sizeof( iONode ), RocsNodeID );
  else
    data->childs = reallocMem( data->childs, (data->childCnt+1) * sizeof( iONode ) );
  data->childs[ data->childCnt ] = child;
  data->childCnt++;
}

static iONode _removeChild( iONode inst, iONode child ) {
  iONodeData data = Data(inst);
  int i = 0;
  int cnt = data->childCnt;
  for( i = 0; i < cnt; i++ ) {
    if( data->childs[i] == child ) {
      data->childs[i] = 0;
      memcpy( &data->childs[i], &data->childs[i+1], ( data->childCnt - (i + 1) )* sizeof( iONode ) );
      data->childCnt--;
      data->childs = reallocMem( data->childs, (data->childCnt+1) * sizeof( iONode ) );
      return child;
    }
  }
  return NULL;
}

static void _addAttr( iONode inst, iOAttr attr ) {
  iONodeData data = Data(inst);
  if( data->attrs == NULL )
    data->attrs = allocIDMem( (data->attrCnt+1) * sizeof( iOAttr ), RocsNodeID );
  else
    data->attrs = reallocMem( data->attrs, (data->attrCnt+1) * sizeof( iOAttr ) );
  data->attrs[ data->attrCnt ] = attr;
  data->attrCnt++;
  MapOp.put( data->attrmap, AttrOp.getName( attr ), (obj)attr );
}

static void _removeAttr( iONode inst, iOAttr attr ) {
  iONodeData data = Data(inst);
  int i;
  if( attr == NULL )
    return;

  for( i = 0; i < data->attrCnt; i++ ) {
    if( data->attrs[i] == attr ) {
      MapOp.remove( data->attrmap, AttrOp.getName( attr ) );
      data->attrs[i] = 0;
      /* should this be done here? */
      attr->base.del( attr );
      memcpy( &data->attrs[i], &data->attrs[i+1], (data->attrCnt - (i + 1)) * sizeof( iOAttr ) );
      data->attrCnt--;
      data->attrs = reallocMem( data->attrs, (data->attrCnt+1) * sizeof( iOAttr ) );
      break;
    }
  }
}

static iOAttr _findAttr( iONode inst, const char* aname ) {
  iONodeData data = Data(inst);
  int i;
  if( data != NULL ) {
    if( !DocOp.isIgnoreCase() ) {
      iOAttr attr = (iOAttr)MapOp.get( data->attrmap, aname );
      if( attr != NULL )
        return attr;
    }
    else {
      /* Can't use the attrMap here because this parser is in case insensitive mode. */
      for( i = 0; i < data->attrCnt; i++ ) {
        iOAttr attr = NodeOp.getAttr(inst,i);
        if( attr != NULL && StrOp.equalsi( AttrOp.getName(attr), aname ) )
          return attr;
      }
    }
    TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999,
                   "Attribute [%s] not found in node [%s].", aname, data->name );
  }
  return NULL;
}

static void _removeAttrByName( iONode inst, const char* name ) {
  iONodeData data = Data(inst);
  iOAttr attr = NodeOp.findAttr( inst, name );
  if( attr != NULL )
    NodeOp.removeAttr(inst, attr);
}

static iONode _findNode( iONode inst, const char* nname ) {
  iONodeData data = Data(inst);
  int i;
  if( data != NULL ) {
    for( i = 0; i < data->childCnt; i++ ) {
      iONode child = NodeOp.getChild(inst,i);
      if( StrOp.equalsi( NodeOp.getName(child), nname ) )
        return child;
    }
    TraceOp.trc( name, TRCLEVEL_PARSE, __LINE__, 9999,
                   "Child node [%s] not found in node [%s].", nname, data->name );
  }
  return NULL;
}

static iONode _getNode( iONode inst, const char* nname ) {
  iONodeData data = Data(inst);
  iONode child = _findNode( inst, nname );
  if( child == NULL ) {
    child = NodeOp.inst( nname, inst, ELEMENT_NODE );
    NodeOp.addChild( inst, child );
  }
  return child;
}

static iONode _findNextNode( iONode inst, iONode offset ) {
  iONodeData data = Data(inst);
  int i;
  Boolean find = False;

  for( i = 0; i < data->childCnt; i++ ) {
    iONode child = data->childs[i];
    if( child == offset ) {
      find = True;
      continue;
    }
    if( find && child != NULL && StrOp.equalsi( NodeOp.getName(child), NodeOp.getName(offset) ) )
      return child;
  }
  return NULL;
}

static nodetype _getType( iONode inst ) {
  iONodeData data = Data(inst);
  return data != NULL ? data->ntype:0;
}

static void _setType( iONode inst, nodetype ntype ) {
  iONodeData data = Data(inst);
  data->ntype = ntype;
}

static const char* _getName( iONode inst ) {
  iONodeData data = Data(inst);
  return data != NULL ? data->name:"";
}

static void _setName( iONode inst, const char* nname ) {
  iONodeData data = Data(inst);
  char* cpName = StrOp.dupID( nname, RocsNodeID );
  if( data->name != NULL )
    StrOp.freeID( data->name, RocsNodeID );
  data->name = cpName;
}

static int _getAttrCnt( iONode inst ) {
  iONodeData data = Data(inst);
  return data != NULL ? data->attrCnt:0;
}

static iOAttr _getAttr( iONode inst, int idx ) {
  iONodeData data = Data(inst);
  return data->attrs[idx];
}

static int _getChildCnt( iONode inst ) {
  iONodeData data = Data(inst);
  return data != NULL ? data->childCnt:0;
}

static iONode _getChild( iONode inst, int idx ) {
  iONodeData data = Data(inst);
  if( idx < data->childCnt && idx >= 0 )
    return data->childs[idx];
  else
    return NULL;
}


static const char* rocs_node_getStr(iONode node,const char* attrName, const char* defaultVal) {
  if( node == NULL )
    return defaultVal;
  else {
    iOAttr attr = _findAttr( node, attrName );
    if( attr == NULL )
      return defaultVal;
    else
      return AttrOp.getVal( attr );
  }
}

static void rocs_node_setStr(iONode node,const char* aname, const char* val) {
  iONodeData data = Data(node);
  iOAttr attr = _findAttr( node, aname );
  if( attr == NULL && val != NULL ) {
    attr = AttrOp.inst( aname, val );
    NodeOp.addAttr( node, attr );
  }
  else if( attr != NULL && val != NULL ) {
    AttrOp.setVal( attr, val );
  }
  else if( attr != NULL && val == NULL ) {
    NodeOp.removeAttr( node, attr );
  }
}

static int rocs_node_getInt(iONode node,const char* attrName, int defaultVal) {
  if( node == NULL )
    return defaultVal;
  else {
    iOAttr attr = _findAttr( node, attrName );
    if( attr == NULL )
      return defaultVal;
    else
      return AttrOp.getInt( attr );
  }
}

static void rocs_node_setInt(iONode node,const char* aname, int ival) {
  iONodeData data = Data(node);
  iOAttr attr = _findAttr( node, aname );
  if( attr == NULL ) {
    attr = AttrOp.instInt( aname, ival );
    NodeOp.addAttr( node, attr );
  }
  else
    AttrOp.setInt( attr, ival );
}

static long rocs_node_getLong(iONode node,const char* attrName, long defaultVal) {
  if( node == NULL )
    return defaultVal;
  else {
    iOAttr attr = _findAttr( node, attrName );
    if( attr == NULL )
      return defaultVal;
    else
      return AttrOp.getLong( attr );
  }
}

static void rocs_node_setLong(iONode node,const char* aname, long lval) {
  iONodeData data = Data(node);
  char val[256];
  iOAttr attr = _findAttr( node, aname );
  if( attr == NULL ) {
    sprintf( val, "%ld", lval );
    attr = AttrOp.inst( aname, val );
    NodeOp.addAttr( node, attr );
  }
  else
    AttrOp.setLong( attr, lval );
}

static double rocs_node_getFloat(iONode node,const char* attrName, double defaultVal) {
  if( node == NULL )
    return defaultVal;
  else {
    iOAttr attr = _findAttr( node, attrName );
    if( attr == NULL )
      return defaultVal;
    else
      return AttrOp.getFloat( attr );
  }
}

static void rocs_node_setFloat(iONode node,const char* aname, double dval) {
  iONodeData data = Data(node);
  char val[256];
  iOAttr attr = _findAttr( node, aname );
  if( attr == NULL ) {
    sprintf( val, "%f", dval );
    attr = AttrOp.inst( aname, val );
    NodeOp.addAttr( node, attr );
  }
  else
    AttrOp.setFloat( attr, dval );
}

static Boolean rocs_node_getBool(iONode node,const char* attrName, Boolean defaultVal) {
  if( node == NULL )
    return defaultVal;
  else {
    iOAttr attr = _findAttr( node, attrName );
    if( attr == NULL )
      return defaultVal;
    else
      return AttrOp.getBoolean( attr );
  }
}

static void rocs_node_setBool(iONode node,const char* aname, Boolean val) {
  rocs_node_setStr( node, aname, val?"true":"false" );
}

/** ------------------------------------------------------------
  * _mergeNode()
  *
  * Merged all unknown attributes from B into A.
  */
static iONode _mergeNode( iONode nodeA, iONode nodeB, Boolean overwrite, Boolean recursive, Boolean keepid ) {
  int cnt = NodeOp.getAttrCnt( nodeB );
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( nodeB, i );
    if( NodeOp.findAttr( nodeA, AttrOp.getName( attr ) ) == NULL ) {
      NodeOp.setStr( nodeA, AttrOp.getName( attr ), AttrOp.getVal( attr ) );
    }
    else if( overwrite ) {
      if( keepid && StrOp.equals( "id", AttrOp.getName( attr ) ) ) {
        /* do not overwrite the id */
      }
      else {
        NodeOp.setStr( nodeA, AttrOp.getName( attr ), AttrOp.getVal( attr ) );
      }
    }
  }

  if( recursive ) {
    cnt = NodeOp.getChildCnt( nodeB );
    for( i = 0; i < cnt; i++ ) {
      iONode node = NodeOp.getChild( nodeB, i );
      if( NodeOp.findNode( nodeA, NodeOp.getName( node ) ) == NULL ) {
        NodeOp.addChild( nodeA, (iONode)node->base.clone( node ) );
      }
      /* Merge recursive */
      NodeOp.mergeNode( NodeOp.findNode( nodeA, NodeOp.getName( node ) ), node, overwrite, recursive, False );
    }
  }

  return nodeA;
}


/*
static iONode _clone( iONode node ) {
  char* str = NodeOp.toString( node );
  iODoc doc = DocOp.parse( str );
  iONode clone = DocOp.getRootNode( doc );
  doc->base.del(doc);
  freeMem( str );
  return clone;
}
*/
static iONode _getParent( iONode inst ) {
  return Data(inst)->parent;
}

static iONode _inst( const char* nname, iONode parent, nodetype ntype ) {
  iONode     node = allocIDMem( sizeof( struct ONode ), RocsNodeID );
  iONodeData data = allocIDMem( sizeof( struct ONodeData ), RocsNodeID );

  MemOp.basecpy( node, &NodeOp, 0, sizeof( struct ONode ), data );

  data->name     = StrOp.dupID( nname, RocsNodeID );
  data->parent   = parent;
  data->ntype    = ntype;
  data->attrs    = NULL;
  data->childs   = NULL;
  data->attrCnt  = 0;
  data->childCnt = 0;
  data->attrmap  = MapOp.inst();

  instCnt++;

  return node;
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/node.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
