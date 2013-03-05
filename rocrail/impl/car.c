/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include "rocrail/impl/car_impl.h"

#include "rocrail/public/app.h"

#include "rocs/public/mem.h"

#include "rocrail/wrapper/public/Car.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/FunDef.h"
#include "rocrail/wrapper/public/Loc.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOCarData data = Data(inst);
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
  iOCarData data = Data((iOCar)inst);
  return data->props;
}

static const char* __id( void* inst ) {
  iOCarData data = Data((iOCar)inst);
  return wCar.getid(data->props);
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OCar ----- */


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
static struct OCar* _inst( iONode ini ) {
  iOCar __Car = allocMem( sizeof( struct OCar ) );
  iOCarData data = allocMem( sizeof( struct OCarData ) );
  MemOp.basecpy( __Car, &CarOp, 0, sizeof( struct OCar ), data );

  /* Initialize data->xxx members... */
  data->props = ini;
  if( StrOp.equals("freight", wCar.gettype(data->props)) ) {
    /* type value changed from freight to goods */
    wCar.settype(data->props, wCar.cartype_freight);
  }

  instCnt++;
  return __Car;
}

static int __getFnAddr( iOCar inst, int function, int* mappedfn) {
  iOCarData    data = Data(inst);

  iONode fundef = wCar.getfundef( data->props );

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "looking up function %d...", function );


  while( fundef != NULL ) {
    if( wFunDef.getfn(fundef) == function ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "function address for %d = %d:%d", function, wFunDef.getaddr(fundef), wFunDef.getmappedfn(fundef) );
      if( mappedfn != NULL ) {
        if( wFunDef.getmappedfn(fundef) > 0 ) {
          *mappedfn = wFunDef.getmappedfn(fundef);
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "mapped function is %d", *mappedfn );
        }
        else
          *mappedfn = function;
      }

      return wFunDef.getaddr(fundef);
    }
    fundef = wCar.nextfundef( data->props, fundef );
  };
  if( mappedfn != NULL ) {
    *mappedfn = function;
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "function %d not defined", *mappedfn );
  }
  return 0;
}


static Boolean _cmd( iOCar inst, iONode nodeA ) {
  iOCarData data = Data(inst);
  iOControl control = AppOp.getControl();

  const char* nodename = NodeOp.getName( nodeA );
  const char* cmd      = wCar.getcmd( nodeA );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command %s:%s for car %s",
      nodename, (cmd==NULL?"-":cmd), wCar.getid( data->props ) );

  if( wCar.getaddr(data->props) > 0 ) {
    if( StrOp.equals(wLoc.name(), nodename ) ) {
      Boolean dir = wCar.isinvdir(data->props) ? !wLoc.isdir(nodeA):wLoc.isdir(nodeA);
      Boolean lights = wLoc.isfn(nodeA);

      if( wCar.isusedir(data->props) ) {
        wLoc.setdir( nodeA, dir );
        if( wCar.getiid(data->props) != NULL )
          wCar.setiid( nodeA, wCar.getiid(data->props) );
        wCar.setaddr( nodeA, wCar.getaddr(data->props) );
        wCar.setprot( nodeA, wCar.getprot( data->props ) );
        wCar.setprotver( nodeA, wCar.getprotver( data->props ) );
        ControlOp.cmd( control, (iONode)NodeOp.base.clone(nodeA), NULL );
      }

      if( wCar.isuselights(data->props) ) {
        char fattr[32] = {'\0'};
        StrOp.fmtb(fattr, "f%d", wCar.getfnlights(data->props));
        NodeOp.setName(nodeA, wFunCmd.name());

        /* use mapped function */
        wFunCmd.setfnchanged(nodeA, wCar.getfnlights(data->props));
        NodeOp.setBool(nodeA, fattr, lights);

        wFunCmd.setf0(nodeA, lights); /**/
        wLoc.setdir( nodeA, dir );
        if( wCar.getiid(data->props) != NULL )
          wCar.setiid( nodeA, wCar.getiid(data->props) );
        wCar.setaddr( nodeA, wCar.getaddr(data->props) );
        wCar.setprot( nodeA, wCar.getprot( data->props ) );
        wCar.setprotver( nodeA, wCar.getprotver( data->props ) );
        ControlOp.cmd( control, (iONode)NodeOp.base.clone(nodeA), NULL );
      }

      NodeOp.base.del(nodeA);
    }
    else if( StrOp.equals(wFunCmd.name(), nodename) ) {
      int mappedfn = 0;
      int decaddr = __getFnAddr(inst, wFunCmd.getfnchanged(nodeA), &mappedfn );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "function %d address=%d:%d", wFunCmd.getfnchanged(nodeA), decaddr, mappedfn );

      if(mappedfn != wFunCmd.getfnchanged(nodeA)) {
        char fattr[32] = {'\0'};
        Boolean fon = False;
        StrOp.fmtb(fattr, "f%d", wFunCmd.getfnchanged(nodeA));
        fon = NodeOp.getBool(nodeA, fattr, False);
        NodeOp.setBool(nodeA, fattr, False);
        StrOp.fmtb(fattr, "f%d", mappedfn);
        NodeOp.setBool(nodeA, fattr, fon);
        wFunCmd.setfnchanged( nodeA, mappedfn );
      }

      if( wCar.getiid(data->props) != NULL )
        wCar.setiid( nodeA, wCar.getiid(data->props) );
      wCar.setaddr( nodeA, wCar.getaddr(data->props) );
      wCar.setprot( nodeA, wCar.getprot( data->props ) );
      wCar.setprotver( nodeA, wCar.getprotver( data->props ) );
      ControlOp.cmd( control, nodeA, NULL );
    }
    else {
      NodeOp.base.del(nodeA);
    }
  }

  return True;
}


/**  */
static void _addWaybill( struct OCar* inst ,iONode waybill ) {
  iOCarData data = Data(inst);
}


/**  */
static const char* _getIdent( struct OCar* inst ) {
  iOCarData data = Data(inst);
  return wCar.getident(data->props);
}


/**  */
static int _getLen( struct OCar* inst ) {
  iOCarData data = Data(inst);
  return wCar.getlen(data->props);
}


/**  */
static void _setLocality( struct OCar* inst, const char* id ) {
  iOCarData data = Data(inst);
  wCar.setlocation(data->props, id);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "car [%s] arrived in block [%s]", CarOp.base.id(inst), id );
}


static int _getFnNrByDesc( iOCar inst, const char* desc) {
  iOCarData    data = Data(inst);

  iONode fundef = wCar.getfundef( data->props );
  while( fundef != NULL ) {
    if( wFunDef.gettext(fundef) != NULL && StrOp.equals(wFunDef.gettext(fundef), desc) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "function number for [%s] = %d", desc, wFunDef.getfn(fundef) );
      return wFunDef.getfn(fundef);
    }
    fundef = wCar.nextfundef( data->props, fundef );
  };

  if( StrOp.len(desc) > 0 && isdigit(desc[0]) )
    return atoi(desc);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "car function [%s] not defined", desc );
  return -1;
}


/**  */
static void _modify( struct OCar* inst ,iONode props ) {
  iOCarData data = Data(inst);
  int cnt = NodeOp.getAttrCnt( props );
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );

    if( StrOp.equals("id", name) && StrOp.equals( value, wCar.getid(data->props) ) )
      continue; /* skip to avoid making invalid pointers */

    NodeOp.setStr( data->props, name, value );
  }

  /* Leave the childs if no new are comming */
  if( NodeOp.getChildCnt( props ) > 0 ) {
    cnt = NodeOp.getChildCnt( data->props );
    while( cnt > 0 ) {
      iONode child = NodeOp.getChild( data->props, 0 );
      NodeOp.removeChild( data->props, child );
      cnt = NodeOp.getChildCnt( data->props );
    }
    cnt = NodeOp.getChildCnt( props );
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( props, i );
      NodeOp.addChild( data->props, (iONode)NodeOp.base.clone(child) );
    }
  }


  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( data->props );
    AppOp.broadcastEvent( clone );
  }
  props->base.del(props);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/car.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
