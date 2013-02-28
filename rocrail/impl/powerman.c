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

#include "rocrail/impl/powerman_impl.h"

#include "rocrail/public/app.h"
#include "rocrail/public/model.h"

#include "rocrail/wrapper/public/BoosterList.h"
#include "rocrail/wrapper/public/Booster.h"

#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/PwrCmd.h"
#include "rocrail/wrapper/public/PwrEvent.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Module.h"
#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/ActionCtrl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/thread.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOPowerManData data = Data(inst);
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
  return NULL;
}

static const char* __id( void* inst ) {
  return NULL;
}

static void __checkAction( iOPowerMan inst, iONode props, const char* state ) {
  iOPowerManData data   = Data(inst);
  iOModel     model  = AppOp.getModel();
  iONode      action = wBooster.getactionctrl( props );

  /* loop over all actions */
  while( action != NULL ) {
    int counter = atoi(wActionCtrl.getstate( action ));

    if( StrOp.len(wActionCtrl.getstate( action )) == 0 ||
        StrOp.equals(state, wActionCtrl.getstate( action )) )
    {

      iOAction Action = ModelOp.getAction(model, wActionCtrl.getid( action ));
      if( Action != NULL ) {
        wActionCtrl.setload(action, wBooster.getload(props));
        wActionCtrl.setvolt(action, wBooster.getvolt(props));
        wActionCtrl.settemp(action, wBooster.gettemp(props));
        ActionOp.exec(Action, action);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action state does not match: [%s-%s]",
          wActionCtrl.getstate( action ), state );
    }

    action = wBooster.nextactionctrl( props, action );
  }
}


static void __sysEvent( obj inst ,const char* cmd ) {
  iOPowerManData data = Data(inst);
  iONode node = NodeOp.inst( wPwrCmd.name(), NULL, ELEMENT_NODE );
  if( StrOp.equals( wSysCmd.go, cmd ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "system power on event" );
    wPwrCmd.setcmd( node, wPwrCmd.on );
  }
  else if( StrOp.equals( wSysCmd.stop, cmd ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "system power off event" );
    wPwrCmd.setcmd( node, wPwrCmd.off );
  }
  PowerManOp.cmd((iOPowerMan)inst, node);

}


static void __informClientOfShortcut(obj inst, iONode booster, Boolean cleared ) {
  iOPowerManData data = Data(inst);
  const char* blockids = wBooster.getblockids(booster);
  const char* modids   = wBooster.getmodids(booster);

  if( blockids != NULL && StrOp.len( blockids ) > 0 ) {
    iOStrTok tok = StrTokOp.inst( blockids, ',' );

    /* iterate all blockid's */
    while( StrTokOp.hasMoreTokens(tok) ) {
      const char* blockid = StrTokOp.nextToken( tok );
      iIBlockBase block = ModelOp.getBlock( AppOp.getModel(), blockid );
      if( block != NULL ) {
        iONode nodeD = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
        wBlock.setid( nodeD, blockid );
        wBlock.setlocid( nodeD, block->getLoc(block) );
        wBlock.setstate( nodeD, cleared?block->getState(block):wBlock.shortcut );
        AppOp.broadcastEvent( nodeD );
      }
    };
    StrTokOp.base.del(tok);

  }

  if( modids != NULL && StrOp.len( modids ) > 0 ) {
    iOStrTok tok = StrTokOp.inst( modids, ',' );

    /* iterate all modid's */
    while( StrTokOp.hasMoreTokens(tok) ) {
      const char* modid = StrTokOp.nextToken( tok );
      iONode nodeD = NodeOp.inst( wModule.name(), NULL, ELEMENT_NODE );
      wModule.setid( nodeD, modid );
      wModule.setcmd( nodeD, wModule.cmd_state );
      wModule.setstate( nodeD, cleared?wModule.state_normal:wModule.state_shortcut );
      AppOp.broadcastEvent( nodeD );
    };
    StrTokOp.base.del(tok);

  }
}


static void __processEvent( obj inst ,Boolean pulse ,const char* id ,const char* ident, int val ) {
  iOPowerManData data = Data(inst);
  iONode scbooster = (iONode)MapOp.get( data->scmap, id );
  iONode pwbooster = (iONode)MapOp.get( data->pwmap, id );
  iONode pwrevent = NodeOp.inst( wPwrEvent.name(), NULL, ELEMENT_NODE );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "booster sensors [%s] event", id );

  if( scbooster != NULL ) {
    TraceOp.trc( name, pulse?TRCLEVEL_WARNING:TRCLEVEL_INFO, __LINE__, 9999,
        "shortcut %s[%s] event for booster [%s][%s]", pulse?"":"cleared ", id,
            wBooster.getid(scbooster), wBooster.getdistrict(scbooster) );
    wPwrEvent.setid( pwrevent, wBooster.getid(scbooster) );
    wPwrEvent.setshortcut( pwrevent, pulse );

    wBooster.setshortcut( scbooster, pulse );

    __informClientOfShortcut(inst, scbooster, !pulse);

    if( wBooster.isscopt_repoweron(scbooster) || pulse ) {
      iONode pwrcmd = NodeOp.inst( wPwrCmd.name(), NULL, ELEMENT_NODE );
      wPwrCmd.setid( pwrcmd, wBooster.getid(scbooster) );
      wPwrCmd.setcmd( pwrcmd, pulse?wPwrCmd.off:wPwrCmd.on );
      PowerManOp.cmd((iOPowerMan)inst, pwrcmd);
    }

    if( wBooster.isscopt_poweroffall(scbooster) && pulse ) {
      iONode pwrcmd = NodeOp.inst( wPwrCmd.name(), NULL, ELEMENT_NODE );
      wPwrCmd.setcmd( pwrcmd, wPwrCmd.off );
      PowerManOp.cmd((iOPowerMan)inst, pwrcmd);
    }


  }
  if( pwbooster != NULL ) {
    TraceOp.trc( name, pulse?TRCLEVEL_INFO:TRCLEVEL_WARNING, __LINE__, 9999,
        "power %s [%s] event for booster [%s][%s]", pulse?"on":"off", id,
            wBooster.getid(pwbooster), wBooster.getdistrict(scbooster) );
    wPwrEvent.setid( pwrevent, wBooster.getid(pwbooster) );
    wPwrEvent.setpower( pwrevent, pulse );
    if( !pulse ) {
      iONode pwrcmd = NodeOp.inst( wPwrCmd.name(), NULL, ELEMENT_NODE );
      wPwrCmd.setid( pwrcmd, wBooster.getid(pwbooster) );
      wPwrCmd.setcmd( pwrcmd, wPwrCmd.off );
      PowerManOp.cmd((iOPowerMan)inst, pwrcmd);
    }
  }

  /* Broadcast to clients. */
  AppOp.broadcastEvent( pwrevent );

}


static void __stateEvent( obj inst, iONode event ) {
  iOPowerManData data = Data(inst);
  iONode booster = (iONode)MapOp.first( data->boostermap );
  /* command for all */
  TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "State event from %d", wState.getuid(event));
  while( booster != NULL ) {
    if( wBooster.getuid(booster) == wState.getuid(event) ) {
      Boolean shortcut = wState.isshortcut(event);
      wBooster.setpower(booster, wState.ispower(event));

      wBooster.setload( booster, wBooster.getload(event) );
      wBooster.setvolt( booster, wBooster.getvolt(event) );
      wBooster.settemp( booster, wBooster.gettemp(event) );

      TraceOp.trc(name, TRCLEVEL_DEBUG, __LINE__, 9999, "booster %s(%08X) power is %s, diagnostics: %dmA %dmV %dC",
          wBooster.getid(booster), wBooster.getuid(booster), wState.ispower(event) ? "ON":"OFF",
              wBooster.getload(booster), wBooster.getvolt(booster), wBooster.gettemp(booster) );


      if( shortcut != wBooster.isshortcut(booster) ) {
        wBooster.setshortcut( booster, shortcut );
        __informClientOfShortcut(inst, booster, !shortcut);

        if( wBooster.isscopt_repoweron(booster) || shortcut ) {
          iONode pwrcmd = NodeOp.inst( wPwrCmd.name(), NULL, ELEMENT_NODE );
          wPwrCmd.setid( pwrcmd, wBooster.getid(booster) );
          wPwrCmd.setcmd( pwrcmd, shortcut?wPwrCmd.off:wPwrCmd.on );
          PowerManOp.cmd((iOPowerMan)inst, pwrcmd);
        }

        if( wBooster.isscopt_poweroffall(booster) && shortcut ) {
          iONode pwrcmd = NodeOp.inst( wPwrCmd.name(), NULL, ELEMENT_NODE );
          wPwrCmd.setcmd( pwrcmd, wPwrCmd.off );
          PowerManOp.cmd((iOPowerMan)inst, pwrcmd);
        }

      }

      if( wBooster.isshortcut(booster) )
        __checkAction((iOPowerMan)inst, booster, "shortcut");
      else
        __checkAction((iOPowerMan)inst, booster, "load");


      AppOp.broadcastEvent( (iONode)NodeOp.base.clone(booster) );

    }
    booster = (iONode)MapOp.next( data->boostermap );
  }
}



static void* __event( void* inst, const void* evt ) {
  iOPowerManData data = Data(inst);
  iONode node = (iONode)evt;
  if( node != NULL && StrOp.equals( wFeedback.name(), NodeOp.getName(node) ) ) {
    __processEvent( inst ,wFeedback.isstate(node), wFeedback.getid(node), wFeedback.getidentifier(node), wFeedback.getval(node) );
  }
  else if( node != NULL && StrOp.equals( wSysCmd.name(), NodeOp.getName(node) ) ) {
    __sysEvent( inst ,wSysCmd.getcmd(node) );
  }
  else if( node != NULL && StrOp.equals( wState.name(), NodeOp.getName(node) ) ) {
    __stateEvent( inst, node );
  }

  return NULL;
}

/** ----- OPowerMan ----- */


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

static void __initBoosters( iOPowerMan inst ) {
  iOPowerManData data = Data(inst);
  iOModel model = AppOp.getModel();

  iONode booster = wBoosterList.getbooster(data->props);
  MapOp.clear(data->boostermap);

  while( booster != NULL ) {
    iOFBack scfb = ModelOp.getFBack( model, wBooster.getscfb( booster ) );
    iOFBack pwfb = ModelOp.getFBack( model, wBooster.getpowerfb( booster ) );

    wBooster.setload( booster, 0 );
    wBooster.setvolt( booster, 0 );
    wBooster.settemp( booster, 0 );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "adding booster [%s]...", wBooster.getid(booster) );

    MapOp.put( data->boostermap, wBooster.getid(booster), (obj)booster );

    if( scfb != NULL && pwfb != NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "Init sensors for booster [%s]...", wBooster.getid(booster) );
      FBackOp.addListener( scfb, (obj)inst );
      FBackOp.addListener( pwfb, (obj)inst );
      MapOp.put( data->scmap, wBooster.getscfb( booster ), (obj)booster );
      MapOp.put( data->pwmap, wBooster.getpowerfb( booster ), (obj)booster );
    }

    booster = wBoosterList.nextbooster(data->props, booster);
  }

}

static Boolean _cmd(iOPowerMan inst, iONode cmd) {
  iOPowerManData data = Data(inst);
  iOModel model = AppOp.getModel();
  iOControl control = AppOp.getControl();
  const char* boosterid = wPwrCmd.getid(cmd);

  if( !StrOp.equals( wPwrCmd.name(), NodeOp.getName(cmd))) {
    /* Ignore */
    return False;
  }

  TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "%s=%s", NodeOp.getName(cmd), wPwrCmd.getcmd(cmd));

  if( boosterid == NULL || StrOp.len( boosterid ) == 0 ) {
    iONode booster = (iONode)MapOp.first( data->boostermap );
    /* command for all */
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Power command [%s] for all boosters.", wPwrCmd.getcmd(cmd));
    while( booster != NULL ) {
      iOOutput output = ModelOp.getOutput( model, wBooster.getpowersw(booster) );
      boosterid = wBooster.getid(booster);
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Power command [%s] for booster [%s].", wPwrCmd.getcmd(cmd), boosterid);
      if( output != NULL ) {
        if( StrOp.equals( wPwrCmd.on, wPwrCmd.getcmd(cmd) ) ) {
          ThreadOp.sleep(50);
          OutputOp.on(output);
        }
        else if( StrOp.equals( wPwrCmd.off, wPwrCmd.getcmd(cmd) ) )
          OutputOp.off(output);
      }
      booster = (iONode)MapOp.next( data->boostermap );
    }
  }
  else {
    iONode booster = (iONode)MapOp.get( data->boostermap, boosterid );
    if( booster != NULL ) {
      iOOutput output = ModelOp.getOutput( model, wBooster.getpowersw(booster) );
      int uid = wBooster.getuid(booster);
      /* single booster command */
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Power command for booster [%s].", boosterid);
      if( output != NULL ) {
        if( StrOp.equals( wPwrCmd.on, wPwrCmd.getcmd(cmd) ) )
          OutputOp.on(output);
        else
          OutputOp.off(output);
      }
      else if( uid > 0 ) {
        /* */
        iONode nodeA = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
        wSysCmd.setbus(nodeA, uid);
        wSysCmd.setiid(nodeA, wBooster.getiid(booster));
        if( StrOp.equals( wPwrCmd.on, wPwrCmd.getcmd(cmd) ) )
          wSysCmd.setcmd(nodeA, wSysCmd.go);
        else
          wSysCmd.setcmd(nodeA, wSysCmd.stop);
        ControlOp.cmd( control, nodeA, NULL );
      }
    }
    else {
      TraceOp.trc(name, TRCLEVEL_WARNING, __LINE__, 9999, "Undefined booster [%s].", boosterid);
    }
  }

  cmd->base.del(cmd);

  return True;
}


/**  */
static struct OPowerMan* _inst( iONode ini ) {
  iOPowerMan __PowerMan = allocMem( sizeof( struct OPowerMan ) );
  iOPowerManData data = allocMem( sizeof( struct OPowerManData ) );
  MemOp.basecpy( __PowerMan, &PowerManOp, 0, sizeof( struct OPowerMan ), data );

  /* Initialize data->xxx members... */
  data->props = ini;
  data->boostermap = MapOp.inst();
  data->scmap = MapOp.inst();
  data->pwmap = MapOp.inst();

  __initBoosters(__PowerMan);

  ModelOp.addSysEventListener( AppOp.getModel(), (obj)__PowerMan );

  TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Power Manager instantiated.");

  instCnt++;
  return __PowerMan;
}


/**  */
static void _modify( struct OPowerMan* inst ,iONode mod ) {
  __initBoosters(inst);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/powerman.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
