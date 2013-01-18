/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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
#include <string.h>
#include <stdlib.h>

#include "rocrail/impl/text_impl.h"

#include "rocrail/public/app.h"
#include "rocrail/public/loc.h"
#include "rocrail/public/block.h"

#include "rocs/public/mem.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/Text.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"

static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}


/**
 * TODO: Move to string object.
 * All %varnames% are replaced with the values of the environment variables.
 * SystemOp.getProperty() is used for getting the value.
 */
static char* _replaceAllSubstitutions( const char* str, iOMap map ) {
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
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "try to resolve [%s]", startV+1);
      if( map != NULL && MapOp.haskey(map, startV+1) )
        resolvedStr = StrOp.cat( resolvedStr, (const char*)MapOp.get(map, startV+1) );
      else if( SystemOp.getProperty(startV+1) != NULL )
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


static void __checkAction( iOText inst, const char* msg ) {
  iOTextData data   = Data(inst);
  iOModel    model  = AppOp.getModel();
  iONode     action = wText.getactionctrl( data->props );

  /* loop over all actions */
  while( action != NULL ) {
    int counter = atoi(wActionCtrl.getstate( action ));

    {
      iOAction Action = ModelOp.getAction(model, wActionCtrl.getid( action ));
      if( Action != NULL ) {
        wActionCtrl.setparam(action, msg);
        ActionOp.exec(Action, action);
      }
    }

    action = wText.nextactionctrl( data->props, action );
  }
}


static void __evaluateSchedule(iONode schedule, int scidx, iOMap map, char* hour, char* min ) {
  if( schedule != NULL ) {
    int idx = 0;

    iONode entry = wSchedule.getscentry( schedule );
    iONode preventry = NULL;
    while( entry != NULL ) {
      if( idx == scidx ) {
        const char* block = wScheduleEntry.getblock( entry );
        const char* location = wScheduleEntry.getlocation( entry );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "entry!= NULL location[%s] block[%s] idx[%d]", location, block, idx );
        if( StrOp.len( block ) > 0 ) {
          MapOp.put(map, "lcscbk", (obj)block );
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "lcscbk [%s]", block );
        }
        if( StrOp.len( location ) > 0 ) {
          MapOp.put(map, "lcscbkloc", (obj)location );
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "lcscbkloc [%s]", location );
        }

        StrOp.fmtb(hour, "%d", wScheduleEntry.gethour( entry ));
        MapOp.put(map, "lcschour", (obj)hour);
        StrOp.fmtb(min, "%d", wScheduleEntry.getminute( entry ));
        MapOp.put(map, "lcscmin", (obj)min);

        entry = wSchedule.nextscentry( schedule, entry );
        if( entry!= NULL ) {
          const char* block = wScheduleEntry.getblock( entry );
          const char* location = wScheduleEntry.getlocation( entry );
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "(next)entry!= NULL location[%s] block[%s]", location, block );
          if( StrOp.len( block ) > 0 ) {
            MapOp.put(map, "lcscnextbk", (obj)block );
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "lcscnextbk [%s]", block );
          }
          if( StrOp.len( location ) > 0 ) {
            MapOp.put(map, "lcscnextbkloc", (obj)location );
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "lcscnextbkloc [%s]", location );
          }
        }

        if( preventry != NULL ) {
          const char* block = wScheduleEntry.getblock( preventry );
          const char* location = wScheduleEntry.getlocation( preventry );
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "preventry!= NULL location[%s] block[%s]", location, block );
          if( StrOp.len( block ) > 0 ) {
            MapOp.put(map, "lcscprevbk", (obj)block );
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "lcscprevbk [%s]", block );
          }
          if( StrOp.len( location ) > 0 ) {
            MapOp.put(map, "lcscprevbkloc", (obj)location );
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "lcscprevbkloc [%s]", location );
          }
        }

        break;
      }
      preventry = entry;
      idx++;
      entry = wSchedule.nextscentry( schedule, entry );
    }
  }
}


static void* __event( void* inst, const void* evt ) {
  iOTextData data = Data(inst);
  iONode node = (iONode)evt;
  if( node != NULL && StrOp.equals( wText.name(), NodeOp.getName(node))) {
    iOLoc       lc = ModelOp.getLoc(AppOp.getModel(), wText.getreflcid(node), NULL);
    iIBlockBase bk = ModelOp.getBlock(AppOp.getModel(), wText.getrefbkid(node));

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "text event [%s-%s][%s]",
        wText.getreflcid(node), wText.getrefbkid(node), wText.getformat(node) );

    if( lc != NULL && bk != NULL ) {
      iONode lcprops = LocOp.base.properties(lc);
      iONode bkprops = bk->base.properties(bk);
      iIBlockBase frombk = ModelOp.getBlock(AppOp.getModel(), bk->getFromBlockId(bk));
      char* msg = NULL;
      iOMap map = MapOp.inst();
      int scidx = 0;
      const char* scid = LocOp.getSchedule(lc, &scidx);

      iONode sc = ModelOp.getSchedule(AppOp.getModel(), scid);
      char* scidxStr = StrOp.fmt("%d", scidx);
      char* speedStr = StrOp.fmt("%.1f", bk->getmvspeed(bk));

      char hour[8];
      char min[8];

      __evaluateSchedule(sc, scidx, map, hour, min);

      MapOp.put(map, "counter", (obj)NodeOp.getStr(node, "counter", "0") );
      MapOp.put(map, "carcount", (obj)NodeOp.getStr(node, "carcount", "0") );
      MapOp.put(map, "countedcars", (obj)NodeOp.getStr(node, "countedcars", "0") );
      MapOp.put(map, "wheelcount", (obj)NodeOp.getStr(node, "wheelcount", "0") );
      MapOp.put(map, "lcid", (obj)LocOp.getId(lc));
      MapOp.put(map, "lcident", (obj)wLoc.getidentifier(lcprops));
      MapOp.put(map, "lcdest", (obj)LocOp.getDestination(lc));
      MapOp.put(map, "lcscid", (obj)scid);
      MapOp.put(map, "lcscidx", (obj)scidxStr);
      MapOp.put(map, "lcnr", (obj)wLoc.getnumber(lcprops));
      MapOp.put(map, "lcdesc", (obj)wLoc.getdesc(lcprops));
      MapOp.put(map, "lcimg", (obj)wLoc.getimage(lcprops));
      MapOp.put(map, "bkid", (obj)bk->base.id(bk));
      MapOp.put(map, "bkdesc", (obj)wBlock.getdesc(bkprops));
      MapOp.put(map, "bkmvspeed", (obj)speedStr); 
      MapOp.put(map, "frombkid", (obj)bk->getFromBlockId(bk));
      MapOp.put(map, "lcdir", (obj)(LocOp.getDir(lc)?"fwd":"rev" ) );
      MapOp.put(map, "lcplacing", (obj)(wLoc.isplacing(lcprops)?"norm":"swap" ) );

      if( frombk != NULL ) {
        iONode frombkprops = frombk->base.properties(frombk);
        MapOp.put(map, "frombkdesc", (obj)wBlock.getdesc(frombkprops));
      }

      if( ModelOp.getBlockLocation(AppOp.getModel(), bk->base.id(bk)) != NULL )
        MapOp.put(map, "bkloc", (obj)ModelOp.getBlockLocation(AppOp.getModel(), bk->base.id(bk)));
      if( ModelOp.getBlockLocation(AppOp.getModel(), bk->getFromBlockId(bk)) != NULL )
        MapOp.put(map, "frombkloc", (obj)ModelOp.getBlockLocation(AppOp.getModel(), bk->getFromBlockId(bk)));

      msg = _replaceAllSubstitutions(wText.getformat(node), map);
      wText.settext(data->props, msg );
      wText.setblock(data->props, bk->base.id(bk) );
      if( MapOp.haskey(map, "frombkloc") ) {
        iOLocation location = (iOLocation)MapOp.get(map, "frombkloc");
        wText.setlocation(data->props, location->base.id(location) );
      }
      else
        wText.setlocation(data->props, "" );

      MapOp.base.del(map);

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "new text [%s]", msg);
      __checkAction(inst, msg);

      StrOp.free(speedStr);
      StrOp.free(scidxStr);
      StrOp.free(msg);
    }
    else if( bk != NULL ) {
      char* msg = NULL;
      char* speedStr = StrOp.fmt("%.1f", bk->getmvspeed(bk));
      iOMap map = MapOp.inst();
      MapOp.put(map, "bkid", (obj)bk->base.id(bk));
      if(ModelOp.getBlockLocation(AppOp.getModel(), bk->base.id(bk)) != NULL)
        MapOp.put(map, "bkloc", (obj)ModelOp.getBlockLocation(AppOp.getModel(), bk->base.id(bk)));
      MapOp.put(map, "bkmvspeed", (obj)speedStr); 
      MapOp.put(map, "counter", (obj)NodeOp.getStr(node, "counter", "0") );
      MapOp.put(map, "carcount", (obj)NodeOp.getStr(node, "carcount", "0") );
      MapOp.put(map, "countedcars", (obj)NodeOp.getStr(node, "countedcars", "0") );
      MapOp.put(map, "wheelcount", (obj)NodeOp.getStr(node, "wheelcount", "0") );

      msg = _replaceAllSubstitutions(wText.getformat(node), map);
      wText.setblock(data->props, bk->base.id(bk) );
      if( MapOp.haskey(map, "frombkloc") )
        wText.setlocation(data->props, (const char*)MapOp.get(map, "frombkloc") );
      else
        wText.setlocation(data->props, "" );
      wText.settext(data->props, msg);
      MapOp.base.del(map);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "new text [%s]", msg);
      __checkAction(inst, msg);
      StrOp.free(msg);
      StrOp.free(speedStr);
    }
    else {
      char* msg = NULL;
      iOMap map = MapOp.inst();

      MapOp.put(map, "counter", (obj)NodeOp.getStr(node, "counter", "0") );
      MapOp.put(map, "carcount", (obj)NodeOp.getStr(node, "carcount", "0") );
      MapOp.put(map, "countedcars", (obj)NodeOp.getStr(node, "countedcars", "0") );
      MapOp.put(map, "wheelcount", (obj)NodeOp.getStr(node, "wheelcount", "0") );

      msg = _replaceAllSubstitutions(wText.getformat(node), map);
      MapOp.base.del(map);
      wText.settext(data->props, msg);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "new text [%s]", msg);
      __checkAction(inst, msg);
      StrOp.free(msg);
    }


    if( wText.getaddr(data->props) > 0 ) {
      iONode node = NodeOp.inst( wText.name(), NULL, ELEMENT_NODE );
      wText.setiid( node, wText.getiid( data->props ) );
      wText.setaddr( node, wText.getaddr( data->props ) );
      wText.setdisplay( node, wText.getdisplay( data->props ) );
      wText.setid( node, wText.getid( data->props ) );
      wText.setblock( node, wText.getblock( data->props ) );
      wText.setlocation( node, wText.getlocation( data->props ) );
      wText.settext( node, wText.gettext( data->props ) );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "send text [%s]", wText.gettext( data->props ));
      ControlOp.cmd( AppOp.getControl(), node, NULL );
    }

    /* Broadcast to clients. */
    {
      iONode node = NodeOp.inst( wText.name(), NULL, ELEMENT_NODE );
      wText.setid( node, wText.getid( data->props ) );
      wText.setblock( node, wText.getblock( data->props ) );
      wText.setlocation( node, wText.getlocation( data->props ) );
      wText.settext( node, wText.gettext( data->props ) );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "broadcast text [%s]", wText.gettext( data->props ));
      AppOp.broadcastEvent( node );
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

  /* Leave the childs if no new are comming */
  if( NodeOp.getChildCnt( o->props ) > 0 ) {
    cnt = NodeOp.getChildCnt( o->props );
    while( cnt > 0 ) {
      iONode child = NodeOp.getChild( o->props, 0 );
      NodeOp.removeChild( o->props, child );
      cnt = NodeOp.getChildCnt( o->props );
    }
  }

  if( NodeOp.getChildCnt( props ) > 0 ) {
    cnt = NodeOp.getChildCnt( props );
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( props, i );
      NodeOp.addChild( o->props, (iONode)NodeOp.base.clone(child) );
    }
  }

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( o->props );
    AppOp.broadcastEvent( clone );
  }
  props->base.del(props);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/text.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
