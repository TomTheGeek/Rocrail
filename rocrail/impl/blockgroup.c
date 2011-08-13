/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2010 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocrail/impl/blockgroup_impl.h"
#include "rocrail/public/app.h"
#include "rocrail/public/block.h"

#include "rocs/public/mem.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/Link.h"
#include "rocrail/wrapper/public/LinkCond.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOBlockGroupData data = Data(inst);
    /* Cleanup data->xxx members...*/
    MapOp.base.del(data->lockmap);
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
  iOBlockGroupData data = Data(inst);
  return data->props;
}

static const char* __id( void* inst ) {
  iOBlockGroupData data = Data(inst);
  return wLink.getid(data->props);
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OBlockGroup ----- */


/**  */
static Boolean _cmd( struct OBlockGroup* inst ,iONode cmd ) {
  return 0;
}


/**  */
static struct OBlockGroup* _inst( iONode ini ) {
  iOBlockGroup __BlockGroup = allocMem( sizeof( struct OBlockGroup ) );
  iOBlockGroupData data = allocMem( sizeof( struct OBlockGroupData ) );
  MemOp.basecpy( __BlockGroup, &BlockGroupOp, 0, sizeof( struct OBlockGroup ), data );

  /* Initialize data->xxx members... */
  data->props = ini;
  data->lockmap = MapOp.inst();
  data->allowfollowup = wLink.isallowfollowup(ini);
  data->maxfollowup = wLink.getmaxfollowup(ini);
  data->followupend = False;
  instCnt++;
  return __BlockGroup;
}


/**  */
static Boolean _lock( struct OBlockGroup* inst ,const char* BlockId ,const char* LocoId ) {
  iOBlockGroupData data = Data(inst);
  iOModel     model  = AppOp.getModel();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco %s request to lock blockgroup %s with block %s",
      LocoId, wLink.getid(data->props), BlockId);

  if( MapOp.get(data->lockmap, LocoId) != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco %s already locked blockgroup %s", LocoId, wLink.getid(data->props));
    return True;
  }

  if( MapOp.size(data->lockmap) == 0 ) {
    iOStrTok tok = StrTokOp.inst( wLink.getdst(data->props), ',' );
    Boolean grouplocked = True;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco %s pending to lock blockgroup %s", LocoId, wLink.getid(data->props));

    while( StrTokOp.hasMoreTokens(tok) && grouplocked ) {
      const char* id = StrTokOp.nextToken( tok );
      iIBlockBase gblock = ModelOp.getBlock( model, id );
      if( gblock != NULL ) {
        grouplocked = gblock->lockForGroup( gblock, LocoId );
      }
    };
    StrTokOp.base.del(tok);

    MapOp.put( data->lockmap, LocoId, (obj)LocoId);

    if( !grouplocked ) {
      /* rewind */
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
          "unable to lock blockgroup %s for loco %s", wLink.getid(data->props), LocoId);
      BlockGroupOp.unlock(inst, LocoId);
      data->firstBlock = NULL;
      data->firstLoco  = NULL;
    }
    else {
      /* check the conditions... */
      Boolean condOK = True;
      iONode cond = wLink.getlinkcond(data->props);
      while( cond != NULL && condOK ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "checking blockgroup %s condition first=%s...",
            wLink.getid(data->props), wLinkCond.getfirst(cond) );
        if( StrOp.equals( wLinkCond.getfirst(cond), BlockId )) {
          Boolean freeBlock = False;
          iOStrTok tok = StrTokOp.inst(wLinkCond.getfree(cond), ',');
          while( StrTokOp.hasMoreTokens(tok) ) {
            const char* id = StrTokOp.nextToken( tok );
            iIBlockBase block = ModelOp.getBlock( model, id );
            if( block != NULL && block->isFree(block, LocoId) ) {
              freeBlock = True;
              break;
            }
          };
          StrTokOp.base.del(tok);
          if( freeBlock ) {
            break;
          }
          else {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                "loco %s cannot lock blockgroup %s with block %s because there is no block free. [%s]",
                LocoId, wLink.getid(data->props), BlockId, wLinkCond.getfree(cond));
            condOK = False;
            grouplocked = False;
            data->firstBlock = NULL;
            data->firstLoco  = NULL;
          }
        }
        cond = wLink.nextlinkcond(data->props, cond);
      }
      if( condOK ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco %s locked blockgroup %s with block %s",
            LocoId, wLink.getid(data->props), BlockId);
        data->firstBlock = BlockId;
        data->firstLoco  = LocoId;
      }
    }

    return grouplocked;
  }
  else if( MapOp.size(data->lockmap) > 0 && data->allowfollowup && data->firstBlock != NULL ) {
    if( StrOp.equals( BlockId, data->firstBlock) && MapOp.get(data->lockmap, LocoId) == NULL && !data->followupend ) {
      MapOp.put( data->lockmap, LocoId, (obj)LocoId);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "loco %s locked blockgroup %s for followup starting in block %s",
          LocoId, wLink.getid(data->props), BlockId);
      if( data->maxfollowup > 0 && data->maxfollowup >= MapOp.size( data->lockmap ) ) {
        data->followupend = True;
      }
      return True;
    }
    else if( MapOp.get(data->lockmap, LocoId) == NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "loco %s not allowed to followup in blockgroup %s starting in block %s (max=%d/%d)",
          LocoId, wLink.getid(data->props), BlockId, MapOp.size( data->lockmap ), data->maxfollowup );
    }

  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "loco %s cannot lock(%d) blockgroup %s for block %s, starting block is %s, followup is %s",
        LocoId, MapOp.size(data->lockmap), wLink.getid(data->props), BlockId,
        data->firstBlock!=NULL?data->firstBlock:"-", data->allowfollowup?"allowed":"not allowed");
  }

  return False;
}


/**  */
static void _modify( struct OBlockGroup* inst ,iONode props ) {
  iOBlockGroupData data = Data(inst);

  int cnt = NodeOp.getAttrCnt( props );

  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );

    if( StrOp.equals("id", name) && StrOp.equals( value, wLink.getid(data->props) ) )
      continue; /* skip to avoid making invalid pointers */

    NodeOp.setStr( data->props, name, value );
  }

  data->allowfollowup = wLink.isallowfollowup(props);
  data->maxfollowup = wLink.getmaxfollowup(props);
  return;
}


/**  */
static void _reset( struct OBlockGroup* inst ) {
  iOBlockGroupData data = Data(inst);
  data->firstBlock = NULL;
  data->followupend = False;
  MapOp.clear(data->lockmap);
  return;
}


static Boolean _isLockedForLoco( struct OBlockGroup* inst ,const char* LocoId ) {
  iOBlockGroupData data = Data(inst);

  return MapOp.haskey( data->lockmap, LocoId);
}


/**  */
static Boolean _unlock( struct OBlockGroup* inst ,const char* LocoId ) {
  iOBlockGroupData data = Data(inst);
  iOModel     model  = AppOp.getModel();

  if( MapOp.remove( data->lockmap, LocoId) == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "blockgroup %s is not locked by %s", wLink.getid(data->props), LocoId );
    return False;
  }


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "loco %s removed from blockgroup %s",
      LocoId, wLink.getid(data->props));


  if( MapOp.size(data->lockmap) == 0 && data->firstLoco != NULL ) {
    iOStrTok tok = StrTokOp.inst( wLink.getdst(data->props), ',' );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "releasing blockgroup %s from initial owner %s",
        wLink.getid(data->props), data->firstLoco);

    data->followupend = False;

    while( StrTokOp.hasMoreTokens(tok) ) {
      const char* id = StrTokOp.nextToken( tok );
      iIBlockBase gblock = ModelOp.getBlock( model, id );
      if( gblock != NULL ) {
        gblock->unLockForGroup( gblock, data->firstLoco );
      }
    };
    StrTokOp.base.del(tok);

    data->firstLoco = NULL;
  }



  return True;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/blockgroup.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
