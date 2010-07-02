/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "rocrail/impl/text_impl.h"

#include "rocrail/public/app.h"

#include "rocs/public/mem.h"

#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/Text.h"

static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  iOTextData data = Data(inst);
  iONode node = (iONode)evt;
  if( node != NULL && StrOp.equals( wText.name(), NodeOp.getName(node))) {
    iOLoc lc = ModelOp.getLoc(AppOp.getModel(), wText.getrefid(node));

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "text event [%s][%s]", wText.getrefid(node), wText.getformat(node) );


    /* Broadcast to clients. */
    {
      iONode clone = (iONode)NodeOp.base.clone( data->props );
      ClntConOp.broadcastEvent( AppOp.getClntCon(), clone );
    }
  }

  if( node != NULL )
    NodeOp.base.del(node);

  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOTextData data = Data(inst);
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
  iOTextData data = Data((iOText)inst);
  return data->props;
}

/** ----- OText ----- */


/**  */
static char* _getForm( void* object ) {
  return 0;
}


/**  */
static char* _postForm( void* object ,const char* data ) {
  return 0;
}


/**  */
static const char* _tableHdr( void ) {
  return 0;
}


/**  */
static char* _toHtml( void* object ) {
  return 0;
}


/**  */
static const char* _getId( struct OText* inst ) {
  return 0;
}


/**  */
static struct OText* _inst( iONode ini ) {
  iOText __Text = allocMem( sizeof( struct OText ) );
  iOTextData data = allocMem( sizeof( struct OTextData ) );
  MemOp.basecpy( __Text, &TextOp, 0, sizeof( struct OText ), data );

  /* Initialize data->xxx members... */
  data->props = ini;

  instCnt++;
  return __Text;
}


/**  */
static void _modify( struct OText* inst ,iONode props ) {
  iOTextData o = Data(inst);
  int cnt = NodeOp.getAttrCnt( props );
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );
    NodeOp.setStr( o->props, name, value );
  }

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( o->props );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), clone );
  }
  props->base.del(props);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/text.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
