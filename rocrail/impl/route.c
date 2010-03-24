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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rocrail/impl/route_impl.h"
#include "rocrail/public/app.h"
#include "rocrail/public/model.h"
#include "rocrail/public/switch.h"
#include "rocrail/public/signal.h"
#include "rocrail/public/output.h"
#include "rocrail/public/tt.h"
#include "rocrail/public/seltab.h"
#include "rocrail/public/r2rnet.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/thread.h"


#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/SwitchCmd.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/Ctrl.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/PermInclude.h"
#include "rocrail/wrapper/public/PermExclude.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Feedback.h"

static int instCnt = 0;

/*
 ***** OBase functions.
 */
static const char* __id( void* inst ) {
  iORouteData o = Data(inst);
  return wRoute.getid( o->props );
}

static void __ctcAction( void* inst, iONode evt ) {
  iORouteData data = Data(inst);
  Boolean state = wFeedback.isstate(evt);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ctc action %d", wFeedback.getaddr(evt));

  if( wFeedback.getaddr(evt) == wRoute.getctcaddr1(data->props) ) {
    data->ctc1 = state;
  }
  else if( wFeedback.getaddr(evt) == wRoute.getctcaddr2(data->props) ) {
    data->ctc2 = state;
  }
  else if( wFeedback.getaddr(evt) == wRoute.getctcaddr3(data->props) ) {
    data->ctc3 = state;
  }

  if( data->ctc2 && data->ctc3 ) {
    /* clear */
    RouteOp.unLock( (iORoute)inst, wRoute.getid( data->props ), NULL, True );
  }

  if( data->ctc1 && data->ctc2 && !data->ctc3 ) {
    if( RouteOp.lock( (iORoute)inst, wRoute.getid( data->props ), False, True ) ) {
      RouteOp.go((iORoute)inst);
    }
  }
}


static void* __event( void* inst, const void* evt ) {
  if( StrOp.equals( wFeedback.name(), NodeOp.getName( (iONode)evt ) ) ) {
    __ctcAction(inst, (iONode)evt);
  }
  return NULL;
}

static const char* __name(void) {
  return name;
}
static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}
static void __deserialize(void* inst, unsigned char* a) {
}
static char* __toString(void* inst) {
  iORouteData data = Data(inst);
  return (char*)RouteOp.getId( (iORoute)inst );
}
static void __del(void* inst) {
  iORouteData data = Data(inst);
  freeMem( data );
  freeMem( inst );
  instCnt--;
}
static void* __properties(void* inst) {
  iORouteData data = Data(inst);
  return data->props;
}
static struct OBase* __clone( void* inst ) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}
static int __count(void) {
  return instCnt;
}


static void __initCTC(iORoute inst, Boolean remove) {
  iORouteData data  = Data(inst);
  iOModel      model = AppOp.getModel();

  if( wRoute.getctcaddr1(data->props) > 0 ) {
    char* key = FBackOp.createAddrKey(wRoute.getctcbus1(data->props), wRoute.getctcaddr1(data->props), wRoute.getctciid1(data->props));
    if( remove )
      ModelOp.removeFbKey( model, key, (obj)inst );
    else
      ModelOp.addFbKey( model, key, (obj)inst );
    StrOp.free(key);
  }

  if( wRoute.getctcaddr2(data->props) > 0 ) {
    char* key = FBackOp.createAddrKey(wRoute.getctcbus2(data->props), wRoute.getctcaddr2(data->props), wRoute.getctciid2(data->props));
    if( remove )
      ModelOp.removeFbKey( model, key, (obj)inst );
    else
      ModelOp.addFbKey( model, key, (obj)inst );
    StrOp.free(key);
  }

  if( wRoute.getctcaddr3(data->props) > 0 ) {
    char* key = FBackOp.createAddrKey(wRoute.getctcbus3(data->props), wRoute.getctcaddr3(data->props), wRoute.getctciid3(data->props));
    if( remove )
      ModelOp.removeFbKey( model, key, (obj)inst );
    else
      ModelOp.addFbKey( model, key, (obj)inst );
    StrOp.free(key);
  }
}


static void __broadcast(iORoute inst) {
  iORouteData o = Data(inst);
  /* Broadcast to clients. */
  iOModel model = AppOp.getModel(  );
  iONode nodeD = NodeOp.inst( wRoute.name(), NULL, ELEMENT_NODE );
  const char* routeId = wRoute.getid(o->props);
  iOList aliases = ModelOp.getRouteAliases( model, routeId );

  wRoute.setlocid(nodeD, o->lockedId );

  if( o->lockedId == NULL || StrOp.len(o->lockedId) == 0 )
    wRoute.setstatus( nodeD, wRoute.status_free);
  else if( o->lockedId != NULL && StrOp.len(o->lockedId) > 0 )
    wRoute.setstatus( nodeD, wRoute.status_locked);
  /* TODO: other status reports */

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "status of route %s is %d, locked by %s",
      wRoute.getid(o->props), wRoute.getstatus( nodeD), o->lockedId == NULL ? "-":o->lockedId );

  if( aliases == NULL ) {
    wRoute.setid( nodeD, routeId );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeD );
  }
  else {
    int i = 0;
    for( i = 0; i < ListOp.size(aliases); i++ ) {
      iONode routeseg = (iONode)ListOp.get( aliases, i );
      wRoute.setid( nodeD, wRoute.getid(routeseg) );
      ClntConOp.broadcastEvent( AppOp.getClntCon(), (iONode)NodeOp.base.clone(nodeD) );
    }
    NodeOp.base.del(nodeD);
  }

}


/*
 ***** _Public functions.
 */
static Boolean _go( iORoute inst ) {
  iORouteData o = Data(inst);
  iOModel model = AppOp.getModel(  );
  iONode sw = wRoute.getswcmd( o->props );
  iONode ac = wRoute.getactionctrl( o->props );
  int error = 0;
  int retry = 0;
  iONode sw_retry = NULL;


  while( ac != NULL ) {
    iOAction action = ModelOp.getAction( model, wActionCtrl.getid(ac) );
    if( action != NULL ) {
      if( wAction.getoid( ac) == NULL || StrOp.len(wAction.getoid( ac)) == 0 )
        wActionCtrl.setlcid( ac, o->lockedId );
      ActionOp.exec(action, ac);
    }
    ac = wRoute.nextactionctrl( o->props, ac );
    ThreadOp.sleep( 10 );
  }

  while( sw != NULL ) {
    const char* swId  = wSwitchCmd.getid( sw );
    const char* swCmd = wSwitchCmd.getcmd( sw );
    Boolean     lock  = wSwitchCmd.islock(sw);
    Boolean   isState = False;

    if( StrOp.equals( wSwitchCmd.cmd_track, swCmd ) ) {
      iOTT isw = ModelOp.getTurntable( model, swId );
      iOSelTab iseltab = ModelOp.getSelectiontable( model, swId );
      if( isw != NULL ) {
        iONode cmd = NodeOp.inst( wTurntable.name(), NULL, ELEMENT_NODE );
        wTurntable.setcmd( cmd,  NodeOp.getStr( sw, "track", "0") );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "go() %s:%s.%d", swId, swCmd, wSwitchCmd.gettrack(sw) );
        if( !TTOp.cmd( (iIBlockBase)isw, cmd ) ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Turntable could not process command." );
          return False;
        }
        else {
          if( !TTOp.getRunDir(isw) ) {
            /*
            iOLoc lc = ModelOp.getLoc(model, o->lockedId );
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "[%s] must swap placing on the turntable.", o->lockedId );
            if( lc != NULL ) {
              LocOp.swapPlacing(lc);
            }
            */
          }
        }
      }
      else if( iseltab != NULL ) {
        iONode cmd = NodeOp.inst( wSelTab.name(), NULL, ELEMENT_NODE );
        wSelTab.setcmd( cmd,  NodeOp.getStr( sw, "track", "0") );
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "go() %s:%s.%d", swId, swCmd, wSwitchCmd.gettrack(sw) );
        if( !SelTabOp.cmd( (iIBlockBase)iseltab, cmd ) ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Seltabtable could not process command." );
          return False;
        }
      }
      else
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "*PANIC* No turntable object found for %s:%s", swId, swCmd );
    }
    else if( StrOp.equals( wSignal.red   , swCmd ) ||
             StrOp.equals( wSignal.green , swCmd ) ||
             StrOp.equals( wSignal.yellow, swCmd ) ||
             StrOp.equals( wSignal.white , swCmd ) )
    {
      iOSignal isg = ModelOp.getSignal( model, swId );

      if( isg != NULL ) {
        if( StrOp.equals( wSignal.red, swCmd ) )
          SignalOp.red(isg);
        else if( StrOp.equals( wSignal.green, swCmd ) )
          SignalOp.green(isg);
        else if( StrOp.equals( wSignal.yellow, swCmd ) )
          SignalOp.yellow(isg);
        else if( StrOp.equals( wSignal.white, swCmd ) )
          SignalOp.white(isg);
      }
    }
    else if( StrOp.equals( wOutput.on   , swCmd ) ||
             StrOp.equals( wOutput.off , swCmd ) )
    {
      iOOutput ico = ModelOp.getOutput( model, swId );

      if( ico != NULL ) {
        if( StrOp.equals( wOutput.on, swCmd ) )
          OutputOp.on(ico);
        else
          OutputOp.off(ico);
      }
    }
    else {
      iOSwitch isw = ModelOp.getSwitch( model, swId );

      if( isw == NULL ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "*PANIC* No switch object found for %s:%s", swId, swCmd );
      }
      else if( SwitchOp.isState( isw, swCmd ) && wCtrl.isskipsetsw( wRocRail.getctrl(AppOp.getIni())) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch[%s] already in position [%s]; skip command", swId, swCmd );
      }
      else {
        iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
        wSwitch.setcmd( cmd, swCmd );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "go() %s:%s", swId, swCmd );

        if( !SwitchOp.cmd( isw, cmd, True, 0, &error, o->lockedId ) && lock ) {
          if( error == CMD_ERROR ) {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unable to switch %s:%s", swId, swCmd );
            return False;
          }
          else if( error == CMD_RETRY ) {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "retry on %s:%s", swId, swCmd );
            ThreadOp.sleep( 10 );

            if( sw == sw_retry ) {
              retry++;
            }
            else {
              sw_retry = sw;
              retry = 0;
            }

            if( retry > 9 ) {
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "retry did not work on %s:%s", swId, swCmd );
              return False;
            }
            else
              continue;
          }
          else {
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Error on switching %s:%s", swId, swCmd );
            return False;
          }
        }
        ThreadOp.sleep( wCtrl.getrouteswtime( wRocRail.getctrl( AppOp.getIni() ) ) );
      }
    }

    sw = wRoute.nextswcmd( o->props, sw );

  };

  /* Inform bka. */
  {
    iOModel model = AppOp.getModel(  );
    iIBlockBase bka = ModelOp.getBlock( model, RouteOp.getFromBlock( inst ) );
    iIBlockBase bkb = ModelOp.getBlock( model, RouteOp.getToBlock( inst ) );
  }

  __broadcast(inst);

  return True;
}

static Boolean _cmd( iORoute inst, iONode nodeA ) {
  iORouteData o = Data(inst);
  Boolean ok = False;
  const char* cmdStr = wRoute.getcmd( nodeA );

  if( StrOp.equals( wRoute.go, cmdStr ) ) {
    ok = _go( inst );
  }
  else if( StrOp.equals( wRoute.test, cmdStr ) ) {
    if( RouteOp.isFree(inst, "__manualcommand__") )
      ok = _go( inst );
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Route %s is locked and cannot be switched by hand.", RouteOp.getId(inst) );
  }

  /* Cleanup Node1 */
  nodeA->base.del(nodeA);
  return ok;
}

static const char* _getId( iORoute inst ) {
  iORouteData o = Data(inst);
  return wRoute.getid( o->props );
}

static void* _getProperties( void* inst ) {
  iORouteData o = Data((iORoute)inst);
  return o->props;
}

static const char* _getFromBlock( iORoute inst ) {
  iORouteData o = Data(inst);
  return wRoute.getbka( o->props );
}

static const char* _getToBlock( iORoute inst ) {
  iORouteData o = Data(inst);
  return wRoute.getbkb( o->props );
}

static Boolean _getDirection( iORoute inst, const char* blockid, Boolean* fromto ) {
  iORouteData o = Data(inst);
  Boolean lcdir = wRoute.islcdir( o->props );

  /* in case of a managed block of a fiddle yard the manager ID is needed */
  blockid = ModelOp.getManagedID( AppOp.getModel(), blockid );

  if( R2RnetOp.compare( blockid, wRoute.getbka( o->props ) ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "blockid [%s] in route [%s] is --from--", blockid, RouteOp.getId(inst) );
    *fromto = True;
    return lcdir;
  }
  else if( R2RnetOp.compare( blockid, wRoute.getbkb( o->props ) ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "blockid [%s] in route [%s] is --to--", blockid, RouteOp.getId(inst) );
    *fromto = False;
    return !lcdir;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "unknown blockid %s", blockid );
    return False;
  }
}


static const char* _getVelocity( iORoute inst, int* percent ) {
  iORouteData data = Data(inst);
  const char* V_hint = wRoute.getspeed(data->props);
  *percent = wRoute.getspeedpercent(data->props);
  return V_hint;
}


static Boolean _getDir( iORoute inst ) {
  iORouteData o = Data(inst);
  return wRoute.isdir( o->props );
}

static Boolean _isSwap( iORoute inst ) {
  iORouteData o = Data(inst);
  return wRoute.isswap( o->props );
}

static Boolean _isSwapPost( iORoute inst ) {
  iORouteData o = Data(inst);
  return wRoute.isswappost( o->props );
}

static Boolean _hasThrownSwitch( iORoute inst ) {
  iORouteData o = Data(inst);
  iOModel    model = AppOp.getModel(  );
  iONode        sw = wRoute.getswcmd( o->props );
  Boolean noreduce = wCtrl.isdisablerouteVreduce( wRocRail.getctrl( AppOp.getIni() ) );

  if( noreduce )
    return False;

  if( wRoute.isreduceV(o->props) ) {
    while( sw != NULL ) {
      if( !StrOp.equals( wSwitch.straight, wSwitchCmd.getcmd(sw) ) ) {
        return True;
      }
      sw = wRoute.nextswcmd( o->props, sw );
    };
  }

  return False;
}

static Boolean __checkSwitches( iORoute inst, const char* id ) {
  iORouteData o = Data(inst);
  iOModel  model = AppOp.getModel(  );
  iONode      sw = wRoute.getswcmd( o->props );
  while( sw != NULL ) {
    const char* swId  = wSwitchCmd.getid( sw );
    Boolean tt = StrOp.equals( wSwitchCmd.cmd_track, wSwitchCmd.getcmd(sw) );
    Boolean lock = wSwitchCmd.islock(sw);

    if( StrOp.equals( wSignal.red, wSwitchCmd.getcmd(sw) ) || StrOp.equals( wSignal.green, wSwitchCmd.getcmd(sw) ) ||
        StrOp.equals( wSignal.yellow, wSwitchCmd.getcmd(sw) ) || StrOp.equals( wSignal.white, wSwitchCmd.getcmd(sw) ) )
    {
      iOSignal isw = ModelOp.getSignal( model, swId );
      if( isw == NULL ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*PANIC* No signal object found for %s", swId );
        return False;
      }
    }
    else if( tt ) {
      iOSelTab iseltab = ModelOp.getSelectiontable( model, swId );
      iOTT itt = ModelOp.getTurntable( model, swId );
      if( itt != NULL ) {
        if( TTOp.isLocked( itt, id ) )
          return False;
      }
      else if( iseltab != NULL ) {
        if( SelTabOp.isLocked( iseltab, id ) )
          return False;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*PANIC* No turntable object found for %s", swId );
        return False;
      }
    }
    else {
      iOSwitch isw = ModelOp.getSwitch( model, swId );
      if( isw != NULL ) {
        if( lock && SwitchOp.isLocked( isw, id ) )
          return False;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*PANIC* No switch object found for %s", swId );
        return False;
      }
    }

    sw = wRoute.nextswcmd( o->props, sw );
  };

  return True;
}

static Boolean __checkCrossingBlocks( iORoute inst, const char* id ) {
  iORouteData o = Data(inst);
  iOModel  model = AppOp.getModel(  );
  const char* bkc = wRoute.getbkc( o->props );
  if( bkc != NULL && StrOp.len( bkc ) > 0 ) {
    iOStrTok tok = StrTokOp.inst( bkc, ',' );

    while( StrTokOp.hasMoreTokens(tok) ) {
      const char* bk = StrTokOp.nextToken( tok );
      iIBlockBase block = ModelOp.getBlock( model, bk );
      if( block != NULL ) {
        if( !block->isFree( block, id ) ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "crossing block [%s] is not free or closed.", id );
          StrTokOp.base.del(tok);
          return False;
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*PANIC* No block object found for %s", bkc );
        StrTokOp.base.del(tok);
        return False;
      }
    };
    StrTokOp.base.del(tok);


  }

  return True;
}

static Boolean __unlockCrossingBlocks( iORoute inst, const char* id, const char** resblocks );
static Boolean __lockCrossingBlocks( iORoute inst, const char* id ) {
  iORouteData o = Data(inst);
  iOModel  model = AppOp.getModel(  );
  const char* bkc = wRoute.getbkc( o->props );
  if( bkc != NULL && StrOp.len( bkc ) > 0 ) {
    iOStrTok tok = StrTokOp.inst( bkc, ',' );

    while( StrTokOp.hasMoreTokens(tok) ) {
      const char* bk = StrTokOp.nextToken( tok );
      iIBlockBase block = ModelOp.getBlock( model, bk );
      if( block != NULL ) {
        if( !block->lock( block, id, "", "", True, False, False, 0 ) ) {
          StrTokOp.base.del(tok);
          return False;
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*PANIC* No block object found for %s", bkc );
        StrTokOp.base.del(tok);
        return False;
      }
    };
    StrTokOp.base.del(tok);

  }

  return True;
}


static Boolean __isReservedBlock(const char* id, const char** resblocks) {
  int i = 0;
  if( resblocks != NULL ) {
    while( resblocks[i] != NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "compare crossing block[%s] with reserved block[%s]", id, resblocks[i] );
      if( StrOp.equals(id, resblocks[i]) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "crossing block[%s] equals reserved block[%s]", id, resblocks[i] );
        return True;
      }
      i++;
    }
  }
  return False;
}

static Boolean __unlockCrossingBlocks( iORoute inst, const char* id, const char** resblocks ) {
  iORouteData  o = Data(inst);
  Boolean      ok = True;
  iOModel   model = AppOp.getModel(  );
  const char* bkc = wRoute.getbkc( o->props );

  if( bkc != NULL && StrOp.len( bkc ) > 0 ) {
    iOStrTok tok = StrTokOp.inst( bkc, ',' );

    while( StrTokOp.hasMoreTokens(tok) ) {
      const char* bk = StrTokOp.nextToken( tok );
      if( !__isReservedBlock(bk, resblocks) ) {
        iIBlockBase block = ModelOp.getBlock( model, bk );
        if( block != NULL ) {
          if( !block->unLock( block, id ) )
            ok = False;
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*PANIC* No block object found for %s", bkc );
          StrTokOp.base.del(tok);
          ok = False;
        }
      }
    };
    StrTokOp.base.del(tok);

  }

  return ok;
}




static Boolean __unlockSwitches( iORoute inst, const char* locId );
static Boolean __lockSwitches( iORoute inst, const char* locId ) {
  iORouteData o = Data(inst);
  iOModel  model = AppOp.getModel(  );
  iONode      sw = wRoute.getswcmd( o->props );

  while( sw != NULL ) {
    const char* swId  = wSwitchCmd.getid( sw );
    const char* cmd  = wSwitchCmd.getcmd( sw );
    if( StrOp.equals( wSwitchCmd.cmd_track, cmd ) ) {
      iOSelTab iseltab = ModelOp.getSelectiontable( model, swId );
      iOTT itt = ModelOp.getTurntable( model, swId );
      if( itt != NULL ) {
        if( !TTOp.lock( (iIBlockBase)itt,
            locId,
            NULL,
      NULL,
            False,
            False,
            wRoute.isswappost( o->props ) ? !o->reverse : o->reverse, 0 ) ) {
          /* Rewind. */
          __unlockSwitches( inst, locId );
          return False;
        }
      }
      else if( iseltab != NULL ) {
        StrOp.free(o->routeLockId);
        o->routeLockId = StrOp.fmt( "%s%s%s", wRoute.routelock, wRoute.getid(o->props), locId );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"lock FY for route [%s]", o->routeLockId );
        if( !SelTabOp.lock( (iIBlockBase)iseltab,
                locId,
                SelTabOp.isManager( iseltab)?o->routeLockId:locId,
            NULL,
                False,
                False,
                wRoute.isswappost( o->props ) ? !o->reverse : o->reverse, 0 ) ) {
          /* Rewind. */
          __unlockSwitches( inst, locId );
          return False;
        }
      }
    }
    else {
      iOSwitch isw = ModelOp.getSwitch( model, swId );
      if( isw != NULL ) {
        if( wSwitchCmd.islock( sw ) ) {
          if( !SwitchOp.lock( isw, locId, inst ) ) {
            /* Rewind. */
            __unlockSwitches( inst, locId );
            return False;
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
              "***** no locking wanted for switch [%s] for route [%s]", SwitchOp.getId(isw) , o->routeLockId );
        }
      }
    }

    sw = wRoute.nextswcmd( o->props, sw );
  };

  return True;
}

static Boolean __unlockSwitches( iORoute inst, const char* locId ) {
  iORouteData o = Data(inst);
  iOModel  model = AppOp.getModel(  );
  iONode      sw = wRoute.getswcmd( o->props );
  Boolean ok = True;
  while( sw != NULL ) {
    const char* swId  = wSwitchCmd.getid( sw );
    const char* cmd  = wSwitchCmd.getcmd( sw );
    if( StrOp.equals( wSwitchCmd.cmd_track, cmd ) ) {
      iOSelTab iseltab = ModelOp.getSelectiontable( model, swId );
      iOTT itt = ModelOp.getTurntable( model, swId );
      if( itt != NULL ) {
        if( !TTOp.unLock( (iIBlockBase)itt, locId ) ) {
          ok = False;
        }
      }
      else if( iseltab != NULL ) {
        StrOp.free(o->routeLockId);
        o->routeLockId = StrOp.fmt( "%s%s%s", wRoute.routelock, wRoute.getid(o->props), locId );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"unlock FY for route [%s]", o->routeLockId );
        if( !SelTabOp.unLock( (iIBlockBase)iseltab, SelTabOp.isManager( iseltab)?o->routeLockId:locId ) ) {
          ok = False;
        }
      }
    }
    else {
      iOSwitch isw = ModelOp.getSwitch( model, swId );
      if( isw != NULL ) {
        if( !SwitchOp.unLock( isw, locId, inst ) )
          ok = False;
      }
    }
    sw = wRoute.nextswcmd( o->props, sw );
  };

  return ok;
}


static Boolean __checkSensors( iORoute inst ) {
  iORouteData data = Data(inst);
  /* check all sensors... */

  iONode fbevt = wRoute.getfbevent( data->props );

  while( fbevt != NULL ) {
    iOFBack fb = ModelOp.getFBack( AppOp.getModel(), wFeedbackEvent.getid(fbevt));
    if( fb != NULL && FBackOp.getState(fb) ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                     "Route [%s] is electrically occupied!",
                     RouteOp.getId( inst ) );

      return False;

    }
    fbevt = wRoute.nextfbevent( data->props, fbevt );
  };

  return True;
}


static Boolean _hasPermission( iORoute inst, iOLoc loc ) {
  iORouteData data = Data(inst);

  const char* id = LocOp.getId( loc );

  /* Permissions */
  iONode incl = wRoute.getincl( data->props );
  iONode excl = wRoute.getexcl( data->props );

  /* test if the id is included: */
  if( incl != NULL ) {
    Boolean included = False;
    while( incl != NULL ) {
      if( StrOp.equals( id, wPermInclude.getid(incl) )) {
        included = True;
        break;
      }
      incl = wRoute.nextincl( data->props, incl );
    };
    if( !included ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Loc [%s] has no permission to use route [%s]",
                     id, wRoute.getid(data->props) );
      return False;
    }
  }

  /* test if the id is excluded: */
  if( excl != NULL ) {
    Boolean excluded = False;
    while( excl != NULL ) {
      if( StrOp.equals( id, wPermExclude.getid(excl) )) {
        excluded = True;
        break;
      }
      excl = wRoute.nextexcl( data->props, excl );
    };
    if( excluded ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Loc [%s] has no permission to use route [%s]",
                     id, wRoute.getid(data->props) );
      return False;
    }
  }

  /* test if the cargo type is permitted */
  {
    const char* permtype = wRoute.gettypeperm(data->props);
    if( permtype != NULL && StrOp.len(permtype) > 0 && !StrOp.equals( permtype, wLoc.cargo_all) ) {
      iONode lc = LocOp.base.properties(loc);
      const char* cargo    = wLoc.getcargo(lc);
      if( !StrOp.equals( permtype, cargo)) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                       "Loc [%s] has no permission to use route [%s]; cargo does not fit. (%s!=%s)",
                       id, wRoute.getid(data->props), permtype, cargo );
        return suits_not;
      }
    }
  }

  /* test if the train length does not exceed the maxlen */
  {
    int maxlen = wRoute.getmaxlen(data->props);
    if( maxlen > 0 ) {
      iONode lc = LocOp.base.properties(loc);
      if( wLoc.getlen(lc) > maxlen ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                       "Loc [%s] has no permission to use route [%s]; train length exceeds the max. allowed length. (%d>%d)",
                       id, wRoute.getid(data->props), wLoc.getlen(lc), maxlen );
        return suits_not;
      }
      }

  }


  return True;

}



static Boolean _isManual( iORoute inst, Boolean* isset ) {
  iORouteData data = Data(inst);
  if( isset != NULL && wRoute.ismanual(data->props) && StrOp.equals( data->lockedId, wRoute.getid(data->props) ) )
    *isset = True;
  else if( isset != NULL )
    *isset = False;

  return wRoute.ismanual(data->props);
}

static Boolean _isFree( iORoute inst, const char* id ) {
  iORouteData data = Data(inst);
  Boolean isset = False;

  if( data->lockedId != NULL && StrOp.equals(data->lockedId, id ) ) {
    /* it is free for itself */
    return True;
  }

  if( data->lockedId != NULL && StrOp.equals(data->lockedId, RouteOp.getId(inst) ) && RouteOp.isManual(inst, &isset) ) {
    /* it is free for itself */
    return isset ? True:False;
  }

  if( data->lockedId == NULL || StrOp.len( data->lockedId ) == 0 ) {
    /* Check all switches: */
    if( !__checkSensors( inst ) )
      return False;

    if( !__checkSwitches( inst, id ) )
      return False;

    if( !__checkCrossingBlocks( inst, id ) )
      return False;

    return True;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Street already locked by %s", data->lockedId );
    return False;
  }
}


static Boolean _lock( iORoute inst, const char* id, Boolean reverse, Boolean lockswitches ) {
  iORouteData o = Data(inst);
  Boolean isset = False;
  o->reverse = reverse;

  if( o->lockedId != NULL && StrOp.equals(o->lockedId, RouteOp.getId(inst) ) && RouteOp.isManual(inst, &isset) ) {
    if( isset ) {
      o->lockedId = NULL;
    }
  }


  if( o->lockedId == NULL || StrOp.len( o->lockedId ) == 0 ) {
    /* Check all switches: */
    if( !__checkSensors( inst ) )
      return False;

    if( lockswitches && !__checkSwitches( inst, id ) )
      return False;

    if( !__checkCrossingBlocks( inst, id ) )
      return False;

    if( lockswitches && !__lockSwitches( inst, id ) )
      return False;

    if( !__lockCrossingBlocks( inst, id ) ) {
      if( lockswitches )
        __unlockSwitches( inst, id );
      return False;
    }

    o->lockedId = id;

    __broadcast(inst);

    return True;
  }
  else if( StrOp.equals( id, o->lockedId ) ) {
    return True;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Route already locked by %s", o->lockedId );
    return False;
  }
}

static Boolean _unLock( iORoute inst, const char* id, const char** resblocks, Boolean unlockswitches ) {
  iORouteData o = Data(inst);
  if( StrOp.equals( id, o->lockedId ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unlocking route %s by %s", RouteOp.getId(inst), id );
    if( unlockswitches )
      __unlockSwitches( inst, id );
    __unlockCrossingBlocks( inst, id, resblocks );
    o->lockedId = NULL;
    __broadcast(inst);
    return True;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "cannot unlock with %s, route %s already locked by %s",
        id, RouteOp.getId(inst), o->lockedId );
  }
  return False;
}

/**
 * Checks for property changes.
 * todo: Range checking?
 */
static void _modify( iORoute inst, iONode props ) {
  iORouteData data = Data(inst);
  int cnt = NodeOp.getAttrCnt( props );
  int i = 0;

  __initCTC(inst, True);

  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );
    NodeOp.setStr( data->props, name, value );
  }

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

  StrOp.free(data->routeLockId);
  data->routeLockId = StrOp.fmt( "%s%s", wRoute.routelock, wRoute.getid(props) );

  __initCTC(inst, False);

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( data->props );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), clone );
  }
  props->base.del(props);
}

static const char* _tableHdr(void) {
  return "<tr bgcolor=\"#CCCCCC\"><th>StreetID</th><th>bka</th><th>bkb</th><th>switches</th></tr>\n";
}

static char* _toHtml( void* inst ) {
  iORouteData data = Data((iORoute)inst);
  char* str1 = StrOp.fmt( "<tr><td>%s</td><td>%s</td><td>%s</td>\n",
      NodeOp.getStr( data->props, "id", "?" ),
      NodeOp.getStr( data->props, "bka", "?" ),
      NodeOp.getStr( data->props, "bkb", "?" )
      );
  str1 = StrOp.cat( str1, "<td><table border=\"1\" cellpadding=\"4\" cellspacing=\"0\">\n" );
  str1 = StrOp.cat( str1, "<tr bgcolor=\"#CCCCCC\"><th>SwID</th><th>Cmd</th></tr>\n" );
  {
    iONode cmd = NodeOp.findNode( data->props, "swcmd" );
    while( cmd != NULL ) {
      char* str2 = StrOp.fmt( "<tr><td>%s</td><td>%s</td></tr>\n",
        NodeOp.getStr( cmd, "id", "?" ),
        NodeOp.getStr( cmd, "cmd", "?" )
      );
      str1 = StrOp.cat( str1, str2 );
      StrOp.free( str2 );
      cmd = NodeOp.findNextNode( data->props, cmd );
    };
  }
  str1 = StrOp.cat( str1, "</table></td>\n</tr>\n" );

  return str1;
}

static char* _getForm( void* inst ) {
  iORouteData data = Data((iORoute)inst);
  return NULL;
}


static char* _postForm( void* inst, const char* postdata ) {
  iORouteData data = Data((iORoute)inst);
  char* reply = StrOp.fmt( "Thanks!<br>" );
  return reply;
}


static void _reset( iORoute inst ) {
  iORouteData o = Data(inst);
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
             "reset route [%s]", RouteOp.getId( inst ) );
  RouteOp.unLock( inst, o->lockedId, NULL, True );
}


static Boolean _isSet( iORoute inst ) {
  iORouteData data  = Data(inst);
  Boolean      isSet = True;

  /* check if all switches are set */
  iOModel model = AppOp.getModel(  );
  iONode  sw    = wRoute.getswcmd( data->props );

  iIBlockBase bka = ModelOp.getBlock( model, wRoute.getbka( data->props ) );
  iIBlockBase bkb = ModelOp.getBlock( model, wRoute.getbkb( data->props ) );

  if( bka != NULL && !bka->isReady(bka) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
               "block [%s] for route [%s] is not ready",
               bka->base.id(bka),
               RouteOp.getId( inst ) );
    return False;
  }

  if( bkb != NULL && !bkb->isReady(bkb) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
               "block [%s] for route [%s] is not ready",
               bkb->base.id(bkb),
               RouteOp.getId( inst ) );
    return False;
  }

  while( sw != NULL ) {
    const char* swId  = wSwitchCmd.getid( sw );
    Boolean tt = StrOp.equals( wSwitchCmd.cmd_track, wSwitchCmd.getcmd(sw) );

    if( tt ) {
      iOSelTab iseltab = ModelOp.getSelectiontable( model, swId );
      iOTT itt = ModelOp.getTurntable( model, swId );
      if( itt != NULL && !TTOp.isSet(itt) ) {
        isSet = False;
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "turntable [%s] for route [%s] is not set",
                   swId,
                   RouteOp.getId( inst ) );
      }
      else if( iseltab != NULL && !SelTabOp.isSet(iseltab) ) {
        isSet = False;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                   "selection table [%s] for route [%s] is not set",
                   swId,
                   RouteOp.getId( inst ) );
      }
    }
    else {
      iOSwitch isw = ModelOp.getSwitch( model, swId );

      if( isw != NULL && !SwitchOp.isSet(isw) ) {
        isSet = False;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                   "switch [%s] for route [%s] is not set",
                   swId,
                   RouteOp.getId( inst ) );
      }
    }

    sw = wRoute.nextswcmd( data->props, sw );
  };

  return isSet;
}


static Boolean _isSetCrossingblockSignals( iORoute inst ) {
  iORouteData data = Data(inst);
  return wRoute.iscrossingblocksignals( data->props );
}


static iORoute _inst( iONode props ) {
  iORoute     route = allocMem( sizeof( struct ORoute ) );
  iORouteData data  = allocMem( sizeof( struct ORouteData ) );

  /* OBase operations */
  MemOp.basecpy( route, &RouteOp, 0, sizeof( struct ORoute ), data );

  data->props = props;
  data->routeLockId = StrOp.fmt( "%s%s", wRoute.routelock, wRoute.getid(props) );

  __initCTC(route, False);

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "inst for %s", _getId(route) );

  instCnt++;

  return route;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/route.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
