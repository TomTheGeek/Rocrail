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

#include "rocrail/impl/switch_impl.h"
#include "rocrail/public/app.h"
#include "rocrail/public/action.h"

#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Ctrl.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/thread.h"

#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/AutoCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/ModelCmd.h"

static int instCnt = 0;

/*
 ***** OBase functions.
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
static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}
static void __deserialize(void* inst, unsigned char* a) {
}
static char* __toString(void* inst) {
  return (char*)SwitchOp.getId( (iOSwitch)inst );
}
static void __del(void* inst) {
  iOSwitchData data = Data(inst);
  freeMem( data );
  freeMem( inst );
  instCnt--;
}
static void* __properties(void* inst) {
  iOSwitchData data = Data(inst);
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


static void __checkAction( iOSwitch inst ) {

  iOSwitchData data     = Data(inst);
  iOModel      model    = AppOp.getModel();
  iONode       swaction = wSwitch.getactionctrl( data->props );

  while( swaction != NULL) {
    if( ModelOp.isAuto(model) == wActionCtrl.isauto(swaction) ) {

      if( StrOp.len( wActionCtrl.getstate(swaction) ) == 0 ||
          StrOp.equals(wActionCtrl.getstate(swaction), wSwitch.getstate(data->props) ) )
      {
        iOAction action = ModelOp.getAction( AppOp.getModel(), wActionCtrl.getid( swaction ));
        if( action != NULL ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch action: %s", wActionCtrl.getid( swaction ));
          ActionOp.exec(action, swaction);
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action state does not match: [%s-%s]",
            wActionCtrl.getstate( swaction ), wSwitch.getstate(data->props) );
      }
    }
    swaction = wSwitch.nextactionctrl( data->props, swaction );
  } /* end loop */

  swaction = NULL;

}

static void __normalizeAddr( int* addr, int* port ) {
  int l_addr = *addr;
  int l_port = *port;

  if( *addr > 0 && *port == 0 ) {
    /* flat */
    l_addr = (*addr / 8) + 1;
    l_port = (*addr % 8) / 2 + 1;
  }
  else if( *addr == 0 && *port > 0 ) {
    /* port */
    l_addr = (*port-1) / 4 +1;
    l_port = (*port-1) % 4 +1;
  }

  *addr = l_addr;
  *port = l_port;
}


static char* _createAddrKey( int bus, int addr, int port, const char* iid ) {
  iONode node = AppOp.getIniNode( wDigInt.name() );
  const char* def_iid = wDigInt.getiid( node );

  int l_addr = addr;
  int l_port = port;

  if( addr == 0 && port == 0 ) {
    /* undef */
    return NULL;
  }

  __normalizeAddr(&l_addr, &l_port);

  return StrOp.fmt( "%d_%d_%d_%s", bus, l_addr, l_port, (iid != NULL && StrOp.len( iid ) > 0) ? iid:def_iid );
}


static const char* __checkFbState( iOSwitch inst ) {
  iOSwitchData data = Data(inst);
  const char* currentState = "-";
  Boolean  fbG = False;
  Boolean  fbR = False;
  Boolean fb2G = False;
  Boolean fb2R = False;
  if( data->fbG ) {
    fbG = FBackOp.getState( data->fbG );
    if( data->fbGinv ) fbG = !fbG;
  }
  if( data->fbR ) {
    fbR = FBackOp.getState( data->fbR );
    if( data->fbRinv ) fbR = !fbR;
  }
  if( data->fb2G ) {
    fb2G = FBackOp.getState( data->fb2G );
    if( data->fb2Ginv ) fb2G = !fb2G;
  }
  if( data->fb2R ) {
    fb2R = FBackOp.getState( data->fb2R );
    if( data->fb2Rinv ) fb2R = !fb2R;
  }

  if( data->fbR && data->fbG && ( fbR && fbG || !fbR && !fbG ) ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
      "Switch:%s: fbG[%d] && fbR[%d] have the same state!",
      SwitchOp.getId( inst ), fbG, fbR );
  }
  if( data->fb2R && data->fb2G && ( fb2R && fb2G || !fb2R && !fb2G ) ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
      "Switch:%s: fb2G[%d] && fb2R[%d] have the same state!",
      SwitchOp.getId( inst ), fb2G, fb2R );
  }

  if( StrOp.equals( wSwitch.gettype( data->props ), wSwitch.threeway ) ) {
    if( !fbR && fbG && !fb2R && fb2G )
      data->fbstate = SW_STRAIGHT;
    else if( fbR && !fbG && !fb2R && fb2G )
      data->fbstate = SW_LEFT;
    else if( !fbR && fbG && fb2R && !fb2G )
      data->fbstate = SW_RIGHT;
  }
  else if( StrOp.equals( wSwitch.gettype( data->props ), wSwitch.dcrossing ) ) {
    if( !fbR && fbG && !fb2R && fb2G )
      data->fbstate = SW_STRAIGHT;
    else if( fbR && !fbG && !fb2R && fb2G )
      data->fbstate = SW_LEFT;
    else if( !fbR && fbG && fb2R && !fb2G )
      data->fbstate = SW_RIGHT;
    else if( fbR && !fbG && fb2R && !fb2G )
      data->fbstate = SW_TURNOUT;
  }
  else {
    if( !fbR && fbG )
      data->fbstate = SW_STRAIGHT;
    else if( fbR && !fbG )
      data->fbstate = SW_TURNOUT;
  }

  if( data->fbstate == SW_STRAIGHT )
    currentState = wSwitch.straight;
  else if( data->fbstate == SW_TURNOUT )
    currentState = wSwitch.turnout;
  else if( data->fbstate == SW_LEFT )
    currentState = wSwitch.left;
  else if( data->fbstate == SW_RIGHT )
    currentState = wSwitch.right;

  /* report */
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
    "Switch[%s] current state [%s], reported state [%s]",
    SwitchOp.getId( inst ), wSwitch.getstate( data->props), currentState );

  /* Not Reached. */
  return currentState;
}


static void __fbEvent( obj inst, Boolean puls, const char* id, int ident, int val ) {
  iOSwitchData data = Data(inst);
  const char* strState = __checkFbState( (iOSwitch)inst );
  Boolean isSet = True;

  if( !StrOp.equals( strState, wSwitch.getstate( data->props) ) ) {
    isSet = False;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "Switch[%s] current state [%s], reported state [%s]",
      SwitchOp.getId( (iOSwitch)inst ), wSwitch.getstate( data->props), strState );
  }

  {
    iONode nodeF = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
    wSwitch.setid( nodeF, SwitchOp.getId( (iOSwitch)inst ) );
    wSwitch.setset( nodeF, isSet );
    wSwitch.setstate( nodeF, strState );
    wSwitch.setswitched( nodeF, wSwitch.getswitched( data->props ) );
    if( wSwitch.getiid( data->props ) != NULL )
      wSwitch.setiid( nodeF, wSwitch.getiid( data->props ) );
    if( data->lockedId != NULL )
      wSwitch.setlocid( nodeF, data->lockedId );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeF );
  }
}


static Boolean __initCallback( iOSwitch inst ) {
  iOSwitchData data = Data(inst);
  Boolean hasFb = False;
  iOModel model = AppOp.getModel(  );
  iOFBack  fbR = ModelOp.getFBack( model, wSwitch.getfbR( data->props ) );
  iOFBack  fbG = ModelOp.getFBack( model, wSwitch.getfbG( data->props ) );
  iOFBack fb2R = ModelOp.getFBack( model, wSwitch.getfb2R( data->props ) );
  iOFBack fb2G = ModelOp.getFBack( model, wSwitch.getfb2G( data->props ) );
  if( fbR != NULL ) {
    FBackOp.setListener( fbR, (obj)inst, &__fbEvent );
    data->hasFbSignal = True;
    data->fbR = fbR;
    data->fbRinv = wSwitch.isfbRinv(data->props);
    hasFb = True;
  }
  if( fbG != NULL ) {
    FBackOp.setListener( fbG, (obj)inst, &__fbEvent );
    data->hasFbSignal = True;
    data->fbG = fbG;
    data->fbGinv = wSwitch.isfbGinv(data->props);
    hasFb = True;
  }
  if( fb2R != NULL ) {
    FBackOp.setListener( fb2R, (obj)inst, &__fbEvent );
    data->hasFbSignal = True;
    data->fb2R = fb2R;
    data->fb2Rinv = wSwitch.isfb2Rinv(data->props);
    hasFb = True;
  }
  if( fb2G != NULL ) {
    FBackOp.setListener( fb2G, (obj)inst, &__fbEvent );
    data->hasFbSignal = True;
    data->fb2G = fb2G;
    data->fb2Ginv = wSwitch.isfb2Ginv(data->props);
    hasFb = True;
  }

  return hasFb;
}

/*
 ***** _Public functions.
 */
static const char* _getId( iOSwitch inst ) {
  iOSwitchData data = Data(inst);
  return wSwitch.getid( data->props );
}

static void* _getProperties( void* inst ) {
  iOSwitchData data = Data((iOSwitch)inst);
  return data->props;
}

static Boolean _lock( iOSwitch inst, const char* id, iORoute route ) {
  iOSwitchData data = Data(inst);
  Boolean ok = False;

  if( data->activated && data->fbstate != SW_UNKNOWN && !SwitchOp.isSet(inst) ) {
    /* pending operation */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Switch [%s] pending operation...",
                   SwitchOp.getId( inst ) );
    return False;
  }

  /* wait only 10ms for getting the mutex: */
  if( !MutexOp.trywait( data->muxLock, 10 ) ) {
    return False;
  }

  if( data->lockedId == NULL || StrOp.equals( id, data->lockedId ) ) {
    data->lockedId = id;
    data->route = route;
    /* Broadcast to clients. Node6 */
    {
      iONode nodeF = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
      wSwitch.setid( nodeF, SwitchOp.getId( inst ) );
      wSwitch.setstate( nodeF, wSwitch.getstate( data->props ) );
      wSwitch.setswitched( nodeF, wSwitch.getswitched( data->props ) );
      if( data->lockedId != NULL )
        wSwitch.setlocid( nodeF, data->lockedId );
      ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeF );
    }
    ok = True;
  }

  /* Unlock the semaphore: */
  MutexOp.post( data->muxLock );

  return ok;
}

static Boolean _unLock( iOSwitch inst, const char* id ) {
  iOSwitchData data = Data(inst);
  if( StrOp.equals( id, data->lockedId ) ) {
    data->lockedId = NULL;
    data->savepostimer = wCtrl.getsavepostime( wRocRail.getctrl( AppOp.getIni(  ) ) ) * 10;
    /* Broadcast to clients. Node6 */
    {
      iONode nodeF = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
      wSwitch.setid( nodeF, SwitchOp.getId( inst ) );
      wSwitch.setstate( nodeF, wSwitch.getstate( data->props ) );
      wSwitch.setswitched( nodeF, wSwitch.getswitched( data->props ) );
      wSwitch.setlocid( nodeF, wSwitch.unlocked );
      ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeF );
    }
    return True;
  }
  return False;
}

static Boolean _isLocked( iOSwitch inst, const char* id ) {
  iOSwitchData data = Data(inst);
  if( data->lockedId != NULL && id == NULL ) {
    return True;
  }
  if( data->lockedId != NULL && !StrOp.equals( id, data->lockedId ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Switch \"%s\" is locked by \"%s\".",
                   SwitchOp.getId( inst ), data->lockedId );
    return True;
  }
  return False;
}

static Boolean _isLeft( iOSwitch inst ) {
  iOSwitchData data = Data(inst);
  return StrOp.equals( wSwitch.left, wSwitch.gettype( data->props ) );
}

static Boolean _isRight( iOSwitch inst ) {
  iOSwitchData data = Data(inst);
  return StrOp.equals( wSwitch.right, wSwitch.gettype( data->props ) );
}

static void _green( iOSwitch inst ) {
  iOSwitchData data = Data(inst);
  int error = 0;
  iONode node = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
  wSwitch.setcmd( node, wSwitch.straight );
  wSwitch.setid( node, SwitchOp.getId( inst ) );
  SwitchOp.cmd( inst, node, True, 0, &error );
}

static void _red( iOSwitch inst ) {
  iOSwitchData data = Data(inst);
  int error = 0;
  iONode node = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
  wSwitch.setcmd( node, wSwitch.turnout );
  wSwitch.setid( node, SwitchOp.getId( inst ) );
  SwitchOp.cmd( inst, node, True, 0, &error );
}


static Boolean _isSet( iOSwitch inst ) {
  iOSwitchData data  = Data(inst);
  Boolean      isSet = True;

  if( data->hasFbSignal && ModelOp.isEnableSwFb(AppOp.getModel()) ) {
    sw_state stateCmd = SW_STRAIGHT;

    if( StrOp.equals( wSwitch.straight, wSwitch.getstate( data->props ) ) )
      stateCmd = SW_STRAIGHT;
    else if( StrOp.equals( wSwitch.turnout, wSwitch.getstate( data->props ) ) )
      stateCmd = SW_TURNOUT;
    else if( StrOp.equals( wSwitch.left, wSwitch.getstate( data->props ) ) )
      stateCmd = SW_LEFT;
    else if( StrOp.equals( wSwitch.right, wSwitch.getstate( data->props ) ) )
      stateCmd = SW_RIGHT;

    __checkFbState( inst );
    if( stateCmd != data->fbstate )
      isSet = False;
  }

  return isSet;
}


static Boolean _cmd( iOSwitch inst, iONode nodeA, Boolean update, int extra, int* error ) {
  iOSwitchData o = Data(inst);
  iOControl control = AppOp.getControl(  );

  const char* state     = wSwitch.getcmd( nodeA );
  const char* prevstate = wSwitch.getcmd( o->props );
  Boolean inv1 = wSwitch.isinv( o->props );
  Boolean inv2 = wSwitch.isinv2( o->props );
  const char* iid = wSwitch.getiid( o->props );

  o->savepostimer = wCtrl.getsavepostime( wRocRail.getctrl( AppOp.getIni(  ) ) ) * 10;

  if( StrOp.equals( wSwitch.unlock, wSwitch.getcmd( nodeA ) ) ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "unlock switch [%s]",
                 SwitchOp.getId( inst ) );
    SwitchOp.unLock( inst, o->lockedId );
    return True;
  }


  if( wSwitch.getaddr1( o->props ) == 0 && wSwitch.getport1( o->props ) == 0 ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Switch [%s] has no address.",
                   SwitchOp.getId( inst ) );
    return True;
  }


  if( !MutexOp.trywait( o->muxCmd, 100 ) ) {
    return False;
  }

  if( StrOp.equals( wSwitch.flip, wSwitch.getcmd( nodeA ) ) ) {
    const char* savedState = wSwitch.getstate( o->props );

    if( StrOp.equals( wSwitch.gettype( o->props ), wSwitch.threeway ) ) {
      if( StrOp.equals( wSwitch.left, savedState ) )
        state = wSwitch.straight;
      else if( StrOp.equals( wSwitch.straight, savedState ) )
        state = wSwitch.right;
      else if( StrOp.equals( wSwitch.right, savedState ) )
        state = wSwitch.left;
      else {
        state = wSwitch.straight;
        wSwitch.setstate( o->props, wSwitch.left );
      }
    }
    else if( StrOp.equals( wSwitch.gettype( o->props ), wSwitch.dcrossing ) && ( wSwitch.getaddr2( o->props ) > 0 || wSwitch.getport2( o->props ) > 0 ) ) {
      if( StrOp.equals( wSwitch.left, savedState ) )
        state = wSwitch.straight;
      else if( StrOp.equals( wSwitch.straight, savedState ) )
        state = wSwitch.right;
      else if( StrOp.equals( wSwitch.right, savedState ) )
        state = wSwitch.turnout;
      else if( StrOp.equals( wSwitch.turnout, savedState ) )
        state = wSwitch.left;
      else {
        state = wSwitch.straight;
        wSwitch.setstate( o->props, wSwitch.left );
      }
    }
    else {
      if( StrOp.equals( wSwitch.straight, savedState ) )
        state = wSwitch.turnout;
      else if( StrOp.equals( wSwitch.turnout, savedState ) )
        state = wSwitch.straight;
      else {
        state = wSwitch.straight;
        wSwitch.setstate( o->props, wSwitch.turnout );
      }
    }
  }

  wSwitch.setstate( o->props, state );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Switch [%s] will be set to [%s,%d,%d]",
                 SwitchOp.getId( inst ), state, wSwitch.issinglegate( o->props ), wSwitch.getgate1( o->props ) );

  if( iid != NULL )
    wSwitch.setiid( nodeA, iid );

  wSwitch.setprot( nodeA, wSwitch.getprot( o->props ) );

  if( StrOp.equals( wSwitch.gettype( o->props ), wSwitch.threeway ) ) {
    const char* state1 = NULL;
    const char* state2 = NULL;
    /* NodeA is cleaned up by the digint so we need a copy for the second unit. */
    iONode nodeA2 = (iONode)nodeA->base.clone( nodeA );

    if( StrOp.equals( state, wSwitch.left ) ) {
      state1 = inv1?wSwitch.straight:wSwitch.turnout;
      state2 = inv2?wSwitch.turnout:wSwitch.straight;

      wSwitch.setaddr1( nodeA, wSwitch.getaddr2( o->props ) );
      wSwitch.setport1( nodeA, wSwitch.getport2( o->props ) );
      wSwitch.setgate1( nodeA, wSwitch.getgate2( o->props ) );
      wSwitch.setcmd( nodeA, state2 );

      wSwitch.setaddr1( nodeA2, wSwitch.getaddr1( o->props ) );
      wSwitch.setport1( nodeA2, wSwitch.getport1( o->props ) );
      wSwitch.setgate1( nodeA2, wSwitch.getgate1( o->props ) );
      wSwitch.setcmd( nodeA2, state1 );
    }
    else if( StrOp.equals( state, wSwitch.right ) ) {
      state1 = inv1?wSwitch.turnout:wSwitch.straight;
      state2 = inv2?wSwitch.straight:wSwitch.turnout;

      wSwitch.setaddr1( nodeA, wSwitch.getaddr1( o->props ) );
      wSwitch.setport1( nodeA, wSwitch.getport1( o->props ) );
      wSwitch.setgate1( nodeA, wSwitch.getgate1( o->props ) );
      wSwitch.setcmd( nodeA, state1 );

      wSwitch.setaddr1( nodeA2, wSwitch.getaddr2( o->props ) );
      wSwitch.setport1( nodeA2, wSwitch.getport2( o->props ) );
      wSwitch.setgate1( nodeA2, wSwitch.getgate1( o->props ) );
      wSwitch.setcmd( nodeA2, state2 );
    }
    else {
      state1 = inv1?wSwitch.turnout:wSwitch.straight;
      state2 = inv2?wSwitch.turnout:wSwitch.straight;

      wSwitch.setaddr1( nodeA, wSwitch.getaddr1( o->props ) );
      wSwitch.setport1( nodeA, wSwitch.getport1( o->props ) );
      wSwitch.setgate1( nodeA, wSwitch.getgate1( o->props ) );
      wSwitch.setcmd( nodeA, state1 );

      wSwitch.setaddr1( nodeA2, wSwitch.getaddr2( o->props ) );
      wSwitch.setport1( nodeA2, wSwitch.getport2( o->props ) );
      wSwitch.setgate1( nodeA2, wSwitch.getgate2( o->props ) );
      wSwitch.setcmd( nodeA2, state2 );
    }

    wSwitch.setdelay( nodeA, wSwitch.getdelay( o->props ) );
    wSwitch.setactdelay( nodeA, wSwitch.isactdelay( o->props ) );
    wSwitch.setsinglegate( nodeA, wSwitch.issinglegate( o->props ) );
    if( !ControlOp.cmd( control, nodeA, error ) ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Switch \"%s\" could not be switched!",
                     SwitchOp.getId( inst ) );
      MutexOp.post( o->muxCmd );
      return False;
    }

    wSwitch.setdelay( nodeA2, wSwitch.getdelay( o->props ) );
    wSwitch.setactdelay( nodeA2, wSwitch.isactdelay( o->props ) );
    wSwitch.setsinglegate( nodeA2, wSwitch.issinglegate( o->props ) );
    if( !ControlOp.cmd( control, nodeA2, error ) ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Switch \"%s\" could not be switched!",
                     SwitchOp.getId( inst ) );
      MutexOp.post( o->muxCmd );
      return False;
    }
  }
  else if( StrOp.equals( wSwitch.gettype( o->props ), wSwitch.dcrossing ) ) {
    const char* state1 = NULL;
    const char* state2 = NULL;
    Boolean has2Units = ( wSwitch.getaddr2( o->props ) > 0 || wSwitch.getport2( o->props ) > 0 )  ? True:False;
    iONode nodeA2 = NULL;

    /* NodeA is cleaned up by the digint so we need a copy for the second unit. */
    if( has2Units )
      nodeA2 = (iONode)nodeA->base.clone( nodeA );

    if( StrOp.equals( state, wSwitch.left ) ) {
      state1 = inv1?wSwitch.straight:wSwitch.turnout;
      state2 = inv2?wSwitch.turnout:wSwitch.straight;
    }
    else if( StrOp.equals( state, wSwitch.right ) ) {
      state1 = inv1?wSwitch.turnout:wSwitch.straight;
      state2 = inv2?wSwitch.straight:wSwitch.turnout;
    }
    else if( StrOp.equals( state, wSwitch.turnout ) ) {
      state1 = inv1?wSwitch.straight:wSwitch.turnout;
      state2 = inv2?wSwitch.straight:wSwitch.turnout;
    }
    else if( StrOp.equals( state, wSwitch.straight ) ) {
      state1 = inv1?wSwitch.turnout:wSwitch.straight;
      state2 = inv2?wSwitch.turnout:wSwitch.straight;
    }

    wSwitch.setaddr1( nodeA, wSwitch.getaddr1( o->props ) );
    wSwitch.setport1( nodeA, wSwitch.getport1( o->props ) );
    wSwitch.setgate1( nodeA, wSwitch.getgate1( o->props ) );
    wSwitch.setdelay( nodeA, wSwitch.getdelay( o->props ) );
    wSwitch.setactdelay( nodeA, wSwitch.isactdelay( o->props ) );
    wSwitch.setsinglegate( nodeA, wSwitch.issinglegate( o->props ) );
    wSwitch.setcmd( nodeA, state1 );
    if( !ControlOp.cmd( control, nodeA, error ) ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Switch \"%s\" could not be switched!",
                     SwitchOp.getId( inst ) );
      MutexOp.post( o->muxCmd );
      return False;
    }

    if( has2Units ) {
      wSwitch.setaddr1( nodeA2, wSwitch.getaddr2( o->props ) );
      wSwitch.setport1( nodeA2, wSwitch.getport2( o->props ) );
      wSwitch.setgate1( nodeA2, wSwitch.getgate2( o->props ) );
      wSwitch.setdelay( nodeA2, wSwitch.getdelay( o->props ) );
      wSwitch.setactdelay( nodeA2, wSwitch.isactdelay( o->props ) );
      wSwitch.setsinglegate( nodeA2, wSwitch.issinglegate( o->props ) );
      wSwitch.setcmd( nodeA2, state2 );
      if( !ControlOp.cmd( control, nodeA2, error ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Switch \"%s\" could not be switched!",
                       SwitchOp.getId( inst ) );
        MutexOp.post( o->muxCmd );
        return False;
      }
    }

  }
  else {

    if( inv1 && StrOp.equals( wSwitch.straight, state ) )
      state = wSwitch.turnout;
    else if( inv1 && StrOp.equals( wSwitch.turnout, state ) )
      state = wSwitch.straight;
    else if( inv1 && StrOp.equals( wSwitch.left, state ) )
      state = wSwitch.right;
    else if( inv1 && StrOp.equals( wSwitch.right, state ) )
      state = wSwitch.left;

    wSwitch.setaddr1( nodeA, wSwitch.getaddr1( o->props ) );
    wSwitch.setport1( nodeA, wSwitch.getport1( o->props ) );
    wSwitch.setgate1( nodeA, wSwitch.getgate1( o->props ) );
    wSwitch.setdelay( nodeA, wSwitch.getdelay( o->props ) );
    wSwitch.setactdelay( nodeA, wSwitch.isactdelay( o->props ) );
    wSwitch.setsinglegate( nodeA, wSwitch.issinglegate( o->props ) );
    wSwitch.setcmd( nodeA, state );
    if( !ControlOp.cmd( control, nodeA, error ) ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Switch \"%s\" could not be switched!",
                     SwitchOp.getId( inst ) );
      MutexOp.post( o->muxCmd );
      return False;
    }
  }

  __checkAction( inst );

  /* both strings can be compared by pointer because they both are of qualifier const */
  if( prevstate != state ) {
    /* increase the switch counter */
    int switched = wSwitch.getswitched(o->props);
    switched++;
    wSwitch.setswitched(o->props, switched);
  }


  /* Broadcast to clients. Node6 */

  if( update ) {
    iONode nodeF = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
    wSwitch.setid( nodeF, SwitchOp.getId( inst ) );
    wSwitch.setstate( nodeF, wSwitch.getstate( o->props ) );
    wSwitch.setswitched( nodeF, wSwitch.getswitched( o->props ) );

    if( o->hasFbSignal && ModelOp.isEnableSwFb(AppOp.getModel()) )
      wSwitch.setset( nodeF, SwitchOp.isSet(inst) );

    if( wSwitch.getiid( o->props ) != NULL )
      wSwitch.setiid( nodeF, wSwitch.getiid( o->props ) );
    if( o->lockedId != NULL )
      wSwitch.setlocid( nodeF, o->lockedId );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "broadcasting switch state [%s]", wSwitch.getstate( o->props ) );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeF );
  }

  o->activated = True;

  MutexOp.post( o->muxCmd );
  return True;
}

/**
 * Checks for property changes.
 * todo: Range checking?
 */
static void _modify( iOSwitch inst, iONode props ) {
  iOSwitchData o = Data(inst);
  iOModel model = AppOp.getModel(  );
  Boolean move = StrOp.equals( wModelCmd.getcmd( props ), wModelCmd.move );

  int cnt = NodeOp.getAttrCnt( props );
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );
    NodeOp.setStr( o->props, name, value );
  }

  if(!move) {
    Boolean has2Units = ( wSwitch.getaddr2( o->props ) > 0 || wSwitch.getport2( o->props ) > 0 )  ? True:False;

    /* initCallback */
    if( __initCallback( inst ) ) {

    }

    if( o->addrKey != NULL )
      ModelOp.removeSwKey( model, o->addrKey );
    o->addrKey = _createAddrKey(
      wSwitch.getbus( o->props ),
      wSwitch.getaddr1( o->props ),
      wSwitch.getport1( o->props ),
      wSwitch.getiid( o->props )
      );
    if( o->addrKey != NULL )
      ModelOp.addSwKey( model, o->addrKey, inst );


    if( has2Units ) {
      if( o->addrKey2 != NULL )
        ModelOp.removeSwKey( model, o->addrKey2 );
      o->addrKey2 = _createAddrKey(
        wSwitch.getbus( o->props ),
        wSwitch.getaddr2( o->props ),
        wSwitch.getport2( o->props ),
        wSwitch.getiid( o->props )
        );
      if( o->addrKey2 != NULL )
        ModelOp.addSwKey( model, o->addrKey2, inst );
    }

    /* delete all childs to make 'room' for the new ones: */
    cnt = NodeOp.getChildCnt( o->props );
    while( cnt > 0 ) {
      iONode child = NodeOp.getChild( o->props, 0 );
      NodeOp.removeChild( o->props, child );
      cnt = NodeOp.getChildCnt( o->props );
    }

    /* add the new or modified childs: */
    cnt = NodeOp.getChildCnt( props );
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( props, i );
      NodeOp.addChild( o->props, (iONode)NodeOp.base.clone(child) );
    }

  }

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( o->props );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), clone );
  }
  props->base.del(props);
}

static const char* _tableHdr(void) {
  return "<tr bgcolor=\"#CCCCCC\"><th>SwID</th><th>Unit</th><th>Pin</th><th>Invert</th><th>type</th><th>x</th><th>y</th><th>z</th></tr>\n";
}

static char* _toHtml( void* inst ) {
  iOSwitchData data = Data((iOSwitch)inst);
  return StrOp.fmt( "<tr><td>%s</td><td align=\"right\">%d</td><td align=\"right\">%d</td><td>%d</td><td>%s</td><td align=\"right\">%d</td><td align=\"right\">%d</td><td align=\"right\">%d</td></tr>\n",
      NodeOp.getStr( data->props, "id", "?" ),
      NodeOp.getInt( data->props, "addr1", 0 ),
      NodeOp.getInt( data->props, "port1", 0 ),
      NodeOp.getBool( data->props, "inv", False ),
      NodeOp.getStr( data->props, "type", "?" ),
      NodeOp.getInt( data->props, "x", 0 ),
      NodeOp.getInt( data->props, "y", 0 ),
      NodeOp.getInt( data->props, "z", 0 )
      );
}

static char* _getForm( void* inst ) {
  iOSwitchData data = Data((iOSwitch)inst);
  return NULL;
}


static char* _postForm( void* inst, const char* postdata ) {
  iOSwitchData data = Data((iOSwitch)inst);
  char* reply = StrOp.fmt( "Thanks!<br>" );
  return reply;
}


static void _reset( iOSwitch inst ) {
  iOSwitchData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
             "reset switch [%s]", SwitchOp.getId( inst ) );
  SwitchOp.unLock( inst, data->lockedId );
}


static void _event( iOSwitch inst, iONode nodeC ) {
  iOSwitchData data = Data(inst);
  Boolean has2Units = ( wSwitch.getaddr2( data->props ) > 0 || wSwitch.getport2( data->props ) > 0 )  ? True:False;

  /* if( !data->hasFbSignal ) */ {
    Boolean inv  = wSwitch.isinv( data->props );
    Boolean inv2 = wSwitch.isinv2( data->props );
    const char* state = wSwitch.getstate( nodeC );

    if( TraceOp.getLevel(NULL) & TRCLEVEL_DEBUG ) {
      char* strNode = (char*)NodeOp.base.toString( nodeC );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "event %s", strNode );
      StrOp.free( strNode );
    }

    if( has2Units ) {
      /* dual motor */
      int addr = wSwitch.getaddr1( nodeC );
      int port = wSwitch.getport1( nodeC );

      int addr1 = wSwitch.getaddr1( data->props );
      int port1 = wSwitch.getport1( data->props );
      int addr2 = wSwitch.getaddr2( data->props );
      int port2 = wSwitch.getport2( data->props );
      __normalizeAddr( &addr1, &port1 );
      __normalizeAddr( &addr2, &port2 );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch [%s] addr=%d port=%d", SwitchOp.getId(inst), addr, port );
      if( addr == addr1 && port == port1 ) {
        if( inv )
          data->fieldState1 = StrOp.equals( state, wSwitch.turnout ) ? 0:1;
        else
          data->fieldState1 = StrOp.equals( state, wSwitch.turnout ) ? 1:0;

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch [%s] fieldState1=%d", SwitchOp.getId(inst), data->fieldState1 );
      }
      if( addr == addr2 && port == port2 ) {
        if( inv2 )
          data->fieldState2 = StrOp.equals( state, wSwitch.turnout ) ? 0:1;
        else
          data->fieldState2 = StrOp.equals( state, wSwitch.turnout ) ? 1:0;

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch [%s] fieldState2=%d", SwitchOp.getId(inst), data->fieldState2 );
      }

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch [%s] fieldState1=%d, fieldState2=%d", SwitchOp.getId(inst), data->fieldState1, data->fieldState2 );

      if( StrOp.equals( wSwitch.gettype( data->props ), wSwitch.threeway ) ) {
        if( data->fieldState1 == 0 && data->fieldState2 == 0)
          wSwitch.setstate( data->props, wSwitch.straight );
        else if( data->fieldState1 == 1 && data->fieldState2 == 0)
          wSwitch.setstate( data->props, wSwitch.left );
        else if( data->fieldState1 == 0 && data->fieldState2 == 1)
          wSwitch.setstate( data->props, wSwitch.right );
      }
      else {
        /* double crossing */
        if( data->fieldState1 == 0 && data->fieldState2 == 0)
          wSwitch.setstate( data->props, wSwitch.straight );
        else if( data->fieldState1 == 1 && data->fieldState2 == 0)
          wSwitch.setstate( data->props, wSwitch.left );
        else if( data->fieldState1 == 0 && data->fieldState2 == 1)
          wSwitch.setstate( data->props, wSwitch.right );
        else if( data->fieldState1 == 1 && data->fieldState2 == 1)
          wSwitch.setstate( data->props, wSwitch.turnout );
      }
    }
    else {
      /* single motor */
      if( !inv )
        wSwitch.setstate( data->props, state );
      else {
        if( StrOp.equals( state, wSwitch.turnout ) )
          wSwitch.setstate( data->props, wSwitch.straight );
        else if( StrOp.equals( state, wSwitch.straight ) )
          wSwitch.setstate( data->props, wSwitch.turnout );
      }
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch [%s] field state=%s", SwitchOp.getId(inst), wSwitch.getstate( data->props) );

    __checkAction( inst );

    /* Broadcast to clients. Node4 */
    {
      iONode nodeD = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
      wSwitch.setid( nodeD, SwitchOp.getId( inst ) );
      wSwitch.setstate( nodeD, wSwitch.getstate( data->props) );
      wSwitch.setswitched( nodeD, wSwitch.getswitched( data->props ) );

      if( data->hasFbSignal && ModelOp.isEnableSwFb(AppOp.getModel()) )
        wSwitch.setset( nodeD, SwitchOp.isSet(inst) );

      wSwitch.setaddr1( nodeD, wSwitch.getaddr1( data->props ) );
      wSwitch.setport1( nodeD, wSwitch.getport1( data->props ) );
      if( has2Units ) {
        wSwitch.setaddr2( nodeD, wSwitch.getaddr2( data->props ) );
        wSwitch.setport2( nodeD, wSwitch.getport2( data->props ) );
      }
      ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeD );
    }
  }

  /* Cleanup Node3 */
  nodeC->base.del(nodeC);

}


static const char* _getAddrKey( iOSwitch inst ) {
  iOSwitchData data = Data(inst);
  return data->addrKey;
}


static const char* _getAddrKey2( iOSwitch inst ) {
  iOSwitchData data = Data(inst);
  return data->addrKey2;
}


/**
 * check save position and sensors
 * is called every 100ms by the controller
 */
static void _checkSenPos( iOSwitch inst ) {
  iOSwitchData data  = Data(inst);
  iOModel      model = AppOp.getModel();
  Boolean      isSet = True;

  if( data->savepostimer > 0 ) {
    data->savepostimer--;

    if( data->savepostimer == 0 &&
        data->lockedId == NULL &&
        !StrOp.equals( wSwitch.getsavepos(data->props), "none") )
      {
      /* check the savepos */
      if( !StrOp.equals( wSwitch.getsavepos(data->props), wSwitch.getstate( data->props ) ) ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Switch [%s] to save position", wSwitch.name() );

        int error = 0;
        iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
        wSwitch.setcmd( cmd, wSwitch.getsavepos(data->props) );
        SwitchOp.cmd( inst, cmd, True, 0, &error );
      }
    }
  }
}


static iOSwitch _inst( iONode props ) {
  iOSwitch     sw   = allocMem( sizeof( struct OSwitch ) );
  iOSwitchData data = allocMem( sizeof( struct OSwitchData ) );

  /* OBase operations */
  MemOp.basecpy( sw, &SwitchOp, 0, sizeof( struct OSwitch ), data );

  data->props = props;
  data->muxLock = MutexOp.inst( NULL, True );
  data->muxCmd  = MutexOp.inst( NULL, True );
  data->id      = wSwitch.getid( props );

  if( __initCallback( sw ) ) {
    data->fbstate = SW_UNKNOWN;
  }


  data->addrKey = _createAddrKey(
    wSwitch.getbus( props ),
    wSwitch.getaddr1( props ),
    wSwitch.getport1( props ),
    wSwitch.getiid( props )
    );

  data->addrKey2 = _createAddrKey(
    wSwitch.getbus( props ),
    wSwitch.getaddr2( props ),
    wSwitch.getport2( props ),
    wSwitch.getiid( props )
    );

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "inst[%d] for %s, %s",
        instCnt,
        _getId(sw),
        data->addrKey
        );

  instCnt++;

  return sw;
}

static Boolean _isState( iOSwitch inst, const char* state ) {
  iOSwitchData data = Data(inst);
  return StrOp.equals( state, wSwitch.getstate(data->props) );
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/switch.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
