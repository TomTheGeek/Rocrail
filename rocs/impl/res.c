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
#include "rocs/impl/res_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/trace.h"

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
    iOResData data = Data(inst);
    /* Cleanup data->xxx members...*/

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
  iOResData data = Data(inst);
  return data->msgNode;
}

/** ----- ORes ----- */


/** Get a message by key. */
static void _addTranslation( struct ORes* inst, const char* xmlStr ) {
  iOResData data = Data(inst);
  iODoc doc = DocOp.parse( xmlStr );
  if( doc != NULL ) {
    data->msgNode = DocOp.getRootNode( doc );
    if( data->msgNode != NULL ) {
      int cnt = NodeOp.getChildCnt( data->msgNode );
      int i = 0;
      if( data->msgMap == NULL )
        data->msgMap = MapOp.inst();
      for( i = 0; i < cnt; i++ ) {
        iONode msg = NodeOp.getChild( data->msgNode, i );
        const char* msgid = NodeOp.getStr( msg, "id", NULL );
        if( msgid != NULL ) {
          if( MapOp.haskey( data->msgMap, msgid ) )
            MapOp.remove( data->msgMap, msgid );
          MapOp.put( data->msgMap, msgid, (obj)msg );
        }
      }
    }
  }
}


/** Creates a resource object. */
static struct ORes* _inst( const char* xml ,const char* lang ) {
  iORes __Res = allocMem( sizeof( struct ORes ) );
  iOResData data = allocMem( sizeof( struct OResData ) );
  MemOp.basecpy( __Res, &ResOp, 0, sizeof( struct ORes ), data );

  /* Initialize data->xxx members... */
  data->xmlStr = xml;
  data->language = lang;
  _addTranslation( __Res, data->xmlStr );

  instCnt++;
  return __Res;
}


/** Get a message by key. */
static const char* _getMsg( struct ORes* inst ,const char* key ) {
  iOResData data = Data(inst);
  if( data->msgMap != NULL ) {
    iONode msg = (iONode)MapOp.get( data->msgMap, key );
    if( msg != NULL ) {
      iONode lang = NodeOp.findNode( msg, data->language );
      if( lang != NULL ) {
        const char* alttxt = NodeOp.getStr( lang, "alttxt", NULL );
        if( alttxt != NULL )
          return alttxt;
        return NodeOp.getStr( lang, "txt", key );
      }
      lang = NodeOp.findNode( msg, "all" );
      if( lang != NULL ) {
        return NodeOp.getStr( lang, "txt", key );
      }
      lang = NodeOp.findNode( msg, "en" );
      if( lang != NULL ) {
        return NodeOp.getStr( lang, "txt", key );
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Resource [%s_%s] not found.", data->language, key );
  return key;
}


/** Get a message by key. */
static const char* _getMenu( struct ORes* inst ,const char* key, Boolean useAccel ) {
  iOResData data = Data(inst);
  if( data->msgMap != NULL ) {
    iONode msg = (iONode)MapOp.get( data->msgMap, key );
    if( msg != NULL ) {
      const char* shortcutkey = NodeOp.getStr( msg, "key", NULL );
      iONode lang = NodeOp.findNode( msg, data->language );
      if( lang == NULL )
        lang = NodeOp.findNode( msg, "all" );
      if( lang == NULL )
        lang = NodeOp.findNode( msg, "en" );
      if( lang != NULL ) {
        Boolean dialog = NodeOp.getBool( msg, "dialog", False );
        int accel = useAccel ? NodeOp.getInt( lang, "accel", -1 ) : -1;
        const char* txt = NodeOp.getStr( lang, "alttxt", NULL );
        const char* menu = NodeOp.getStr( lang, "menu", NULL );
        if( txt == NULL || StrOp.len(txt) == 0 )
          txt = NodeOp.getStr( lang, "txt", key );
        if( menu == NULL ) {
          char* menustr = StrOp.fmt( "%s%s%s%s", txt, dialog?"...":"", shortcutkey!=NULL?"\t":"", shortcutkey!=NULL?shortcutkey:"" );
          if( accel != -1 ) {
            int strlen = StrOp.len( menustr );
            char* amenustr = allocIDMem( strlen + 2, RocsStrID );
            int i = 0;
            int idx = 0;
            for( i = 0; i < strlen; i++ ) {
              if( i == accel ) {
                amenustr[idx] = '&';
                idx++;
              }
              amenustr[idx] = menustr[i];
              idx++;
              amenustr[idx] = '\0';
            }
            StrOp.free( menustr );
            menustr = amenustr;
          }
          NodeOp.setStr( lang, "menu", menustr );
          StrOp.free( menustr );
          return NodeOp.getStr( lang, "menu", NULL );
        }
        else
          return menu;
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Resource [%s_%s] not found.", data->language, key );
  return key;
}


/** Get a tooltip by key. */
static const char* _getTip( struct ORes* inst ,const char* key ) {
  iOResData data = Data(inst);
  if( data->msgMap != NULL ) {
    iONode msg = (iONode)MapOp.get( data->msgMap, key );
    if( msg != NULL ) {
      iONode lang = NodeOp.findNode( msg, data->language );
      if( lang != NULL ) {
        return NodeOp.getStr( lang, "tip", key );
      }
      lang = NodeOp.findNode( msg, "all" );
      if( lang != NULL ) {
        return NodeOp.getStr( lang, "tip", key );
      }
      lang = NodeOp.findNode( msg, "en" );
      if( lang != NULL ) {
        return NodeOp.getStr( lang, "tip", key );
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Resource [%s_%s] not found.", data->language, key );
  return key;
}


/** Test if the given key is in the XML. */
static Boolean _hasKey( struct ORes* inst ,const char* key ) {
  iOResData data = Data(inst);
  return MapOp.haskey( data->msgMap, key );
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/res.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
