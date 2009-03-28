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
#include <time.h>

#include "rocrail/impl/block_impl.h"
#include "rocrail/public/app.h"
#include "rocrail/public/fback.h"
#include "rocrail/public/route.h"
#include "rocrail/public/control.h"
#include "rocrail/public/modplan.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Ctrl.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/BlockInclude.h"
#include "rocrail/wrapper/public/BlockExclude.h"


static int instCnt = 0;

/*
 ***** OBase functions.
 */
static const char* __id( void* inst ) {
  iOBlockData data = Data(inst);
  return data->id;
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
  iOBlockData data = Data(inst);
  return (char*)data->id;
}
static void __del(void* inst) {
  iOBlockData data = Data(inst);
  freeMem( data );
  freeMem( inst );
  instCnt--;
}
static void* __properties(void* inst) {
  iOBlockData data = Data(inst);
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


static void __setDefaultAspect(iIBlockBase inst) {
  iOBlockData data = Data(inst);
  /* set default signal aspect */
  int aspect = 0;
  const char* defaspect = wCtrl.getdefaspect( AppOp.getIniNode( wCtrl.name() ) );
  if( StrOp.equals( wSignal.green, defaspect) )
    aspect = 1;
  else if( StrOp.equals( wSignal.yellow, defaspect) )
    aspect = 2;
  else if( StrOp.equals( wSignal.white, defaspect) )
    aspect = 3;

  switch( aspect ) {
    case 0:
      BlockOp.red( inst, False, False );
      BlockOp.red( inst, True, False );
      BlockOp.red( inst, False, True );
      BlockOp.red( inst, True, True );
      break;
    case 1:
      BlockOp.green( inst, False, False );
      BlockOp.green( inst, True, False );
      BlockOp.green( inst, False, True );
      BlockOp.green( inst, True, True );
      break;
    case 2:
      BlockOp.yellow( inst, False, False );
      BlockOp.yellow( inst, True, False );
      BlockOp.yellow( inst, False, True );
      BlockOp.yellow( inst, True, True );
      break;
    case 3:
      BlockOp.white( inst, False, False );
      BlockOp.white( inst, True, False );
      BlockOp.white( inst, False, True );
      BlockOp.white( inst, True, True );
      break;
  }
}



static Boolean __acceptGhost( obj inst ) {
  iOBlockData data = Data(inst);

  if( ( data->locId == NULL || StrOp.equals( data->locId, "") || StrOp.equals( data->locId, "?") || StrOp.equals( data->locId, "GHOST")) && wBlock.isacceptghost( data->props ) ) {
    data->locId = "GHOST";
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Block:%s accepted a Ghosttrain",
                 data->id );
    {
      iONode node = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
      wBlock.setid( node, data->id );
      wBlock.setlocid( node, data->locId );
      ClntConOp.broadcastEvent( AppOp.getClntCon(), node );
    }
    return True;
  }
  return False;
}


/**
 * translate the event name to an event code
 */
static int _getEventCode( const char* evtname ) {
  if( StrOp.equals( evtname, wFeedbackEvent.enter_event ) )
    return enter_event;
  else if( StrOp.equals( evtname, wFeedbackEvent.enter2in_event ) )
    return enter2in_event;
  else if( StrOp.equals( evtname, wFeedbackEvent.in_event ) )
    return in_event;
  else if( StrOp.equals( evtname, wFeedbackEvent.exit_event ) )
    return exit_event;
  else if( StrOp.equals( evtname, wFeedbackEvent.pre2in_event ) )
    return pre2in_event;
  else if( StrOp.equals( evtname, wFeedbackEvent.occupied_event ) )
    return occupied_event;
  else if( StrOp.equals( evtname, wFeedbackEvent.ident_event ) )
    return ident_event;
  else if( StrOp.equals( evtname, wFeedbackEvent.shortin_event ) )
    return shortin_event;
  else
    return 0;
}


/**
 * event listener callback for turntable events
 */
static void __TurntableEvent( obj inst, const char* event, const char* id ) {
  iOBlockData data = Data(inst);
  iOLoc loc = NULL;
  int evt = _getEventCode( event );

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Block:%s: ttid=%s event=%s",
      data->id, id, event );

  if( data->locId != NULL && StrOp.len( data->locId ) > 0 ) {
    iOModel model = AppOp.getModel();
    loc = ModelOp.getLoc( model, data->locId );
    if( loc != NULL )
      LocOp.event( loc, inst, evt, 0 );
  }

}


static void __measureVelocity( iOBlock inst, int event ) {
  iOBlockData data = Data(inst);

  /*if( !wBlock.ismvactive( data->props ) )*/
  if( wBlock.getmvdistance(data->props) == 0 )
  {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "measure velocity not set...");
    return;
  }
  else {
    time_t     t = time(NULL);
    int        ms = SystemOp.getMillis();

    if( event == enter_event ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "measure velocity enter event...");
      data->mvMillis = ms;
      data->mvTime   = t;
    }
    else if( event == in_event ) {
      int seconds = t - data->mvTime;
      int millis = seconds * 1000 + (ms - data->mvMillis);

      if( millis > 0 ) {
        float distanceMM = wBlock.getmvdistance(data->props) * wBlock.getmvscale(data->props);
        float factor = 3600000 / millis;
        float kmh =  (factor * distanceMM) / (1000*1000);

        if( wBlock.ismvmph( data->props ) ) {
        /* kmh * 0.621371192 = mph */
          TraceOp.trc( name, TRCLEVEL_CALC, __LINE__, 9999,
              "average velocity of [%s] in block [%s] was %.1f MPH",
              data->locId, data->id, kmh * 0.621371192 );
        }
        else {
          TraceOp.trc( name, TRCLEVEL_CALC, __LINE__, 9999,
              "average velocity of [%s] in block [%s] was %.1f KM/H",
              data->locId, data->id, kmh );
        }
      }
    }
  }

}


/**
 * event listener callback for all fbevents
 */
static void _event( iIBlockBase inst, Boolean puls, const char* id, int ident, int val, iONode fbevt ) {
  iOBlockData data = Data(inst);
  iOLoc        loc = NULL;
  obj      manager = (obj)(data->manager == NULL ? inst:data->manager);
  char    key[256] = {'\0'};

  StrOp.fmtb( key, "%s-%s", id, data->fromBlockId != NULL ? data->fromBlockId:"" );

  if( fbevt == NULL ) {
    /* event without description; look up in map */
    fbevt = (iONode)MapOp.get( data->fbEvents, key );
  }


  if( fbevt == NULL ) {
    /* event without description; look up in map */
    fbevt = ModPlanOp.getEvent4Block( NULL, NULL , data->props, data->fromBlockId, id);
  }

  if( fbevt == NULL ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Block [%s] no event found for fromBlockId [%s], try to find one for all...",
        data->id, data->fromBlockId?data->fromBlockId:"?" );

    /* TODO: check running direction -> from_all or from_all_reverse */
/*
    if ( ( data->reverse && !data->next1Route->isSwapPost( data->next1Route ) )
      || ( !data->reverse && data->next1Route->isSwapPost( data->next1Route ) ) ) {
*/
    if ( data->reverse ) {
      StrOp.fmtb( key, "%s-%s", id, wFeedbackEvent.from_all_reverse );
    }
    else {
      StrOp.fmtb( key, "%s-%s", id, wFeedbackEvent.from_all );
    }
    fbevt = (iONode)MapOp.get( data->fbEvents, key );
  }

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Block:%s: fbid=%s state=%s ident=%d fbfrom=%s from=%s",
      data->id, key, puls?"true":"false", ident,
                 wFeedbackEvent.getfrom(fbevt), data->fromBlockId?data->fromBlockId:"?" );

  if( data->crossing ) {
    /* ignore all events */
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "ignore events for crossing block %s", data->id );
    return;
  }

  if( data->locId != NULL && StrOp.len( data->locId ) > 0 ) {
    iOModel model = AppOp.getModel(  );
    loc = ModelOp.getLoc( model, data->locId );
  }

  /* handle a dedicated ident event */
  if( fbevt != NULL && _getEventCode( wFeedbackEvent.getaction( fbevt ) ) == ident_event ) {
    /* display ident code */
    char identString[32];
    StrOp.fmtb( identString, "%04d", ident );
    iONode nodeD = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
    wBlock.setid( nodeD, data->id );
    wBlock.setlocid( nodeD, puls ? identString:data->locId );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeD );
  }

  if( fbevt != NULL && puls == !wFeedbackEvent.isendpuls( fbevt ) && loc != NULL ) {
    int evt = _getEventCode( wFeedbackEvent.getaction( fbevt ) );
    int locident = wLoc.getidentifier( LocOp.base.properties(loc) );

    __measureVelocity( (iOBlock)inst, evt );

    if( ident > 0 && locident > 0 && ident != locident || ident > 0 && locident == 0 ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Loc identifier does not match! block=%s loc=%d ident=%d",
          data->id, locident, ident );
    }
    else if( ident > 0 && locident > 0 && ident == locident ) {
      TraceOp.trc( name, TRCLEVEL_CALC, __LINE__, 9999, "ident matched: block=%s loc=%d ident=%d",
          data->id, locident, ident );
    }

    if( evt == enter2in_event ) {
      LocOp.event( loc, manager, enter_event, 0 );
      LocOp.event( loc, manager, in_event, data->timer > 0 ? data->timer:1 );
    }
    else
      LocOp.event( loc, manager, evt, 0 );

    if( evt == enter2in_event || evt == in_event || evt == shortin_event ) {
      /* TODO: check if the shortin_event does not ruin the auto mode */
      data->fromBlockId = data->id;
    }
  }
  else if( data->fromBlockId == NULL && puls && loc == NULL ) {
    /* ghost train! */
    if( !__acceptGhost((obj)inst) ) {
      int tl = TRCLEVEL_USER1;
      if( ModelOp.isAuto( AppOp.getModel() ) ) {
        tl = TRCLEVEL_EXCEPTION;
        /* power off */
        AppOp.stop();
      }
      /* TODO: broadcast ghost state */
      TraceOp.trc( name, tl, __LINE__, 9999, "Ghost train in block %s, fbid=%s, ident=%d",
          data->id, key, ident );
      {
        iONode nodeD = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
        wBlock.setid( nodeD, data->id );
        wBlock.setstate( nodeD, wBlock.ghost );
        wBlock.setlocid( nodeD, data->locId );
        ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeD );
      }

    }
  }
  else if( fbevt == NULL && puls && loc != NULL ) {
    /* ghost train! */
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Maybe ghost train in block %s, fbid=%s, ident=%d",
        data->id, key, ident );

    fbevt = (iONode)ListOp.first( data->fbGhostEvents );
    while( fbevt != NULL ) {
      /* notify loc */
      int evt = _getEventCode( wFeedbackEvent.getaction( fbevt ) );
      if( evt == enter2in_event )
        LocOp.event( loc, manager, enter_event, 0 );
      else
        LocOp.event( loc, manager, evt, 0 );

      fbevt = (iONode)ListOp.next( data->fbGhostEvents );
    }
  }
  else if( fbevt == NULL && data->fromBlockId != NULL ) {
    /* undefined event! */
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Sensor %s in block %s is undefined! ident=%d",
                   key, data->id, ident );
  }
}

static void _fbEvent( obj inst, Boolean puls, const char* id, int ident, int val ) {
  _event( (iIBlockBase)inst, puls, id, ident, val, NULL );
}


/**
 * map all fbevent's and set the listener to the common __fbEvent
 */
static void __initFeedbackEvents( iOBlock inst ) {
  iOBlockData data = Data(inst);
  iOModel model = AppOp.getModel();
  char key[256] = {'\0'};
  iONode fbevt = wBlock.getfbevent( data->props );

  ListOp.clear( data->fbGhostEvents );
  MapOp.clear( data->fbEvents );

  while( fbevt != NULL ) {
    const char* fbid = wFeedbackEvent.getid( fbevt );
    iOFBack fb = ModelOp.getFBack( model, fbid );

    if( StrOp.len( fbid ) > 0 && fb != NULL ) {
      iOStrTok tok = StrTokOp.inst( wFeedbackEvent.getfrom( fbevt ), ',' );
      if( wFeedbackEvent.isghostdetection( fbevt ) )
        ListOp.add( data->fbGhostEvents, (obj)fbevt );

      /* put all blockid's in the map */
      while( StrTokOp.hasMoreTokens(tok) ) {
        const char* fromblockid = StrTokOp.nextToken( tok );
        StrOp.fmtb( key, "%s-%s", fbid, fromblockid );
        MapOp.put( data->fbEvents, key, (obj)fbevt );
      };

      FBackOp.setListener( fb, (obj)inst, &_fbEvent );
    }
    fbevt = wBlock.nextfbevent( data->props, fbevt );
  };

  if( wBlock.getttid(data->props) != NULL && StrOp.len(wBlock.getttid(data->props)) > 0 ) {
    iOTT tt = ModelOp.getTurntable( model, wBlock.getttid(data->props) );
    if( tt != NULL )
      TTOp.setListener( tt, (obj)inst, &__TurntableEvent );
  }

}


/*
 ***** _Public functions.
 */
static Boolean _setListener( iOBlock inst, obj listenerObj, const block_listener listenerFun ) {
  iOBlockData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}

static const char* _getFromBlockId( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  return data->fromBlockId;
}


static const char* _getTDiid( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  return wBlock.getiid( data->props );
}


static int _getTDaddress( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  return wBlock.getaddr( data->props );
}


static int _getTDport( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  return wBlock.getport( data->props );
}


static void _setAnalog( iIBlockBase inst, Boolean analog ) {
  iOBlockData data = Data(inst);
  iOControl control = AppOp.getControl();
  /* setAlalog */
  iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
  wSysCmd.setcmd( cmd, analog?wSysCmd.analog:wSysCmd.dcc );
  wSysCmd.setiid( cmd, wBlock.getiid( data->props ) );
  wSysCmd.setaddr( cmd, wBlock.getaddr( data->props ) );
  wSysCmd.setport( cmd, wBlock.getport( data->props ) );
  ControlOp.cmd( control, cmd, NULL );
}


static Boolean _setManual( iOBlock inst, Boolean manual ) {
  iOBlockData data = Data(inst);
  iOControl control = AppOp.getControl();
  /* setManual TODO: check if block is already used for auto-mode... */
  wBlock.setmanual( data->props, manual );
  return True;
}


static void _setManager( iIBlockBase inst, iIBlockBase manager ) {
  iOBlockData data = Data(inst);
  data->manager = manager;
}


static iIBlockBase _getManager( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  return data->manager;
}


static Boolean _isReady( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  return True;
}


static Boolean _isFree( iIBlockBase inst, const char* locId ) {
  iOBlockData data = Data(inst);

  const char* state = wBlock.getstate( data->props );

  if( StrOp.equals( state, wBlock.closed ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Block \"%s\" is closed!.",
                   data->id );
    return False;
  }

  if( locId != NULL && StrOp.equals( locId, data->locId ) )
    return True;

/* check all sensors... */

  {
    iONode fbevt = wBlock.getfbevent( data->props );

    while( fbevt != NULL ) {
      iOFBack fb = ModelOp.getFBack( AppOp.getModel(), wFeedbackEvent.getid(fbevt));
      if( fb != NULL && FBackOp.getState(fb) && _getEventCode( wFeedbackEvent.getaction( fbevt ) ) != ident_event ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                       "Block \"%s\" is electrically occupied.",
                       data->id );

        if( data->locId == NULL || StrOp.len( data->locId ) == 0 ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                         "Block \"%s\" is electrically occupied without locId set!",
                         data->id );
        }

        return False;

      }
      fbevt = wBlock.nextfbevent( data->props, fbevt );
    };
  }


  if( data->locId == NULL || StrOp.len( data->locId ) == 0 || StrOp.equals( "(null)", data->locId ) )
    return True;
  else if( data->locId != NULL )
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Block [%s] is reserved by [%s]",
                   data->id, data->locId );

  return False;
}


static const char* _getVelocity( iIBlockBase inst, int* percent, Boolean onexit ) {
  iOBlockData data = Data(inst);
  const char* V_hint = onexit? wBlock.getexitspeed(data->props):wBlock.getspeed(data->props);
  *percent = wBlock.getspeedpercent(data->props);
  return V_hint;
}


static int _getWait( iIBlockBase inst, iOLoc loc ) {
  iOBlockData data = Data(inst);
  if( StrOp.equals( wLoc.cargo_cleaning, wLoc.getcargo( (iONode)loc->base.properties( loc ) ) ) ){
    return 0;
  }
  else if( StrOp.equals( wBlock.wait_random, wBlock.getwaitmode( data->props ) ) ) {
    /* Random between 1 and 30. */
    int min = wBlock.getminwaittime( data->props );
    int max = wBlock.getmaxwaittime( data->props );
    float fmax = max;
    int rwait = 0;
    if( max < min ) {
      fmax = min;
      min = max;
    }
    rwait = min + (int) (fmax*rand()/(RAND_MAX+1.0));
    return rwait;
  }
  else if( StrOp.equals( wBlock.wait_loc, wBlock.getwaitmode( data->props ) ) ) {
    return wLoc.getblockwaittime( (iONode)loc->base.properties( loc ) );
  }
  else {
    return wBlock.getwaittime( data->props ) ;
  }
}

/* cross checking block and train types */
static block_suits __crossCheckType(iOBlock block, iOLoc loc, Boolean* wait) {
  iOBlockData data = Data(block);
  const char* traintype = wLoc.getcargo( LocOp.base.properties(loc) );
  const char* blocktype = wBlock.gettype( BlockOp.base.properties(block) );
  Boolean     blockwait = wBlock.iswait(BlockOp.base.properties(block) );

  const char* ttId = wBlock.getttid( BlockOp.base.properties(block) );

  /* always wait in block if it contains a turntable */
  if( ttId != NULL && StrOp.len( ttId ) > 0 )
    blockwait = True;

  if( wait != NULL )
    *wait = blockwait;

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                 "wait in block \"%s\" is %s",
                 data->id, blockwait?"true":"false" );

  /* undefined block type */
  if( StrOp.equals( wBlock.type_none, blocktype ) ) {
    if( data->prevLocId != NULL ) {
      const char* locid = LocOp.getId( loc );
      if( StrOp.equals( data->prevLocId, locid ) ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                       "Block \"%s\" is last visited by \"%s\"",
                       data->id, locid );
        return suits_ok;
      }
      else
        return suits_well;
    }
  }

  if( StrOp.equals( wBlock.type_goods, blocktype ) && StrOp.equals( wLoc.cargo_goods, traintype ) )
    return suits_well;

  if( StrOp.equals( wBlock.type_ice, blocktype ) && StrOp.equals( wLoc.cargo_ice, traintype ) )
    return suits_well;

  if( StrOp.equals( wBlock.type_local, blocktype ) && StrOp.equals( wLoc.cargo_person, traintype ) )
    return suits_well;

  if( StrOp.equals( wBlock.type_local, blocktype ) && StrOp.equals( wLoc.cargo_mixed, traintype ) )
    return suits_well;

  if( StrOp.equals( wBlock.type_goods, blocktype ) && StrOp.equals( wLoc.cargo_mixed, traintype ) )
    return suits_well;

  if( StrOp.equals( wBlock.type_shunting, blocktype ) && StrOp.equals( wLoc.cargo_none, traintype ) )
    return suits_well;

  if( wait != NULL )
    *wait = False;

  return suits_ok;
}


static int _isSuited( iIBlockBase inst, iOLoc loc ) {
  iOBlockData data = Data(inst);
  /* ToDo: Check if loc can run in this block. */

  int bklen = wBlock.getlen( data->props );
  int lclen = LocOp.getLen( loc );
  const char* id = LocOp.getId( loc );

  /* Permissions */
  iONode incl = wBlock.getincl( data->props );
  iONode excl = wBlock.getexcl( data->props );

  /* test if the id is included: */
  if( incl != NULL ) {
    Boolean included = False;
    while( incl != NULL ) {
      if( StrOp.equals( id, wBlockInclude.getid(incl) )) {
        included = True;
        break;
      }
      incl = wBlock.nextincl( data->props, incl );
    };
    if( !included ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Loc [%s] has no permission to enter block [%s]",
                     id, data->id );
      return suits_not;
    }
  }

  /* test if the id is excluded: */
  if( excl != NULL ) {
    Boolean excluded = False;
    while( excl != NULL ) {
      if( StrOp.equals( id, wBlockInclude.getid(excl) )) {
        excluded = True;
        break;
      }
      excl = wBlock.nextexcl( data->props, excl );
    };
    if( excluded ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Loc [%s] has no permission to enter block [%s]",
                     id, data->id );
      return suits_not;
    }
  }


  if( wBlock.ismanual( data->props ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Block \"%s\" is in manual-mode: not chooseable for auto-mode!",
                   data->id );
    return suits_not;
  }

  if( wLoc.getaddr( loc->base.properties( loc ) ) == 0 && !wBlock.istd( data->props ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                   "Block \"%s\" does not support analog locs like \"%s\". (%d, %d)",
                   data->id, LocOp.getId( loc ), bklen, lclen );
    return suits_not;
  }

  if( bklen > 0 && lclen > 0 ) {
    if( lclen + data->minbklc > bklen ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Block \"%s\" is too short for \"%s\". (%d, %d)",
                     data->id, LocOp.getId( loc ), bklen, lclen );
      return suits_not;
    }
  }

  if( StrOp.equals( "no", wBlock.getcommuter( data->props ) ) &&
      wLoc.iscommuter( loc->base.properties( loc ) ) ) {
    return suits_not;
  }
  else if( StrOp.equals( "only", wBlock.getcommuter( data->props ) ) &&
      !wLoc.iscommuter( loc->base.properties( loc ) ) ) {
    return suits_not;
  }

  /* TODO: check all other loc properties if it realy suits. */
  if( StrOp.equals( wLoc.engine_electric, wLoc.getengine( loc->base.properties( loc ) ) ) &&
      !wBlock.iselectrified( data->props ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                   "Block \"%s\" is not electrified so locs \"%s\" can not enter.",
                   data->id, LocOp.getId( loc ) );
    return suits_not;
  }


  /* this only returns suits_ok or suits_well */
  return __crossCheckType((iOBlock)inst, loc, NULL);
}

static Boolean _wait( iIBlockBase inst, iOLoc loc ) {
  iOBlockData data = Data(inst);
  Boolean wait = False;
  if( StrOp.equals( wLoc.cargo_cleaning, wLoc.getcargo( (iONode)loc->base.properties( loc ) ) ) ){
    return False;
  }
  __crossCheckType( (iOBlock)inst, loc, &wait);
  return wait;
}

static void _enterBlock( iIBlockBase inst, const char* id ) {
  iOBlockData data = Data(inst);
  wBlock.setlocid( data->props, id );
  if( id != NULL ) {
    iONode nodeD = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
    wBlock.setid( nodeD, data->id );
    wBlock.setentering( nodeD, True );
    wBlock.setlocid( nodeD, id );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeD );
  }
}


static void _inBlock( iIBlockBase inst, const char* id ) {
  iOBlockData data = Data(inst);
  wBlock.setlocid( data->props, id );
  if( id != NULL ) {
    iONode nodeD = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
    wBlock.setid( nodeD, data->id );
    wBlock.setreserved( nodeD, False );
    wBlock.setlocid( nodeD, id );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeD );
  }
}


static int _getVisitCnt( iIBlockBase inst, const char* id ) {
  iOBlockData data = Data(inst);
  if( data->prevLocId != NULL && StrOp.equals( id, data->prevLocId ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Block \"%s\" is %d visited by \"%s\"",
                   data->id, data->prevLocIdCnt, id );
    return data->prevLocIdCnt;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "Block \"%s\" is not visited by \"%s\"",
                   data->id, id );
    return 0;
  }
}


static int _getOccTime( iIBlockBase inst ) {
  iOBlockData data  = Data(inst);
  iOModel     model = AppOp.getModel();
  /* check if the loco is in auto mode */
  if( data->locId != NULL && StrOp.len( data->locId ) > 0 ) {
    iOLoc loc = ModelOp.getLoc( model, data->locId );
    if( loc != NULL && LocOp.isAutomode(loc)) {
      return data->occtime;
    }
  }
  return 0;
}


static Boolean _link( iIBlockBase inst, iIBlockBase linkto ) {
  iOBlockData data = NULL;

  if( inst == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "BlockOp._link( NULL, ... )" );
    return False;
  }

  data = Data(inst);

  if( linkto != NULL ) {
    iOControl control = AppOp.getControl();
    iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
    wSysCmd.setcmd( cmd, wSysCmd.link );
    wSysCmd.setvalA( cmd, inst->getTDport( inst ) );
    wSysCmd.setvalB( cmd, linkto->getTDport( linkto ) );
    if( ControlOp.cmd( control, cmd, NULL ) ) {
      data->linkto = linkto;
      return True;
    }
  }
  return False;
}

/**
 * Ignore all events wenn the crossing flag is set.
 */
static Boolean _lock( iIBlockBase inst, const char* id, const char* blockid, Boolean crossing, Boolean reset, Boolean reverse ) {
  iOBlockData data = NULL;
  Boolean ok = False;

  if( inst == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "BlockOp.lock( NULL, %s )", id );
    return False;
  }

  data = Data(inst);

  /* wait only 10ms for getting the mutex: */
  if( !MutexOp.trywait( data->muxLock, 10 ) ) {
    return False;
  }

  if( !BlockOp.isFree( inst, id ) ) {
    MutexOp.post( data->muxLock );
    return False;
  }

  /* check group lock */
  if( data->locIdGroup != NULL && !StrOp.equals( data->locIdGroup, id ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block already has a group lock by [%s]", data->locIdGroup );
    MutexOp.post( data->muxLock );
    return False;
  }

  if( data->locId == NULL || StrOp.len( data->locId ) == 0 || StrOp.equals( "(null)", data->locId) ) {
    data->locId = id;
    data->crossing = crossing;
    ok = True;
    ModelOp.setBlockOccupation( AppOp.getModel(), data->id, data->locId, False, 0 );
  }
  else if( StrOp.equals( id, data->locId ) ) {
    data->crossing = crossing;
    ok = True;
  }

  if( ok ) {
    if( data->prevLocId != NULL && StrOp.equals( id, data->prevLocId ) ) {
      data->prevLocIdCnt++;
    }
    else {
      data->prevLocId = id;
      data->prevLocIdCnt = 1;
    }
  }

  /* Broadcast to clients. */
  if( ok ) {
    iONode nodeD = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
    wBlock.setid( nodeD, data->id );
    wBlock.setreserved( nodeD, True );
    wBlock.setlocid( nodeD, id );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeD );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                 "Block \"%s\" already locked by \"%s\".",
                 data->id, data->locId );
  }

  if( ok ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
        "block %s locked for [%s][%s][%s] in [%s] direction", data->id, id, data->locId, blockid, reverse?"reverse":"normal" );
    data->reverse = reverse;
    data->fromBlockId = blockid;
    if( reset )
      BlockOp.resetTrigs( inst );
  }

  if( ok ) {
    /* reset occupation ticker */
    data->occtime = SystemOp.getTick();
  }

  /* Unlock the semaphore: */
  MutexOp.post( data->muxLock );

  return ok;
}


static Boolean _lockForGroup( iIBlockBase inst, const char* id ) {
  iOBlockData data = NULL;
  Boolean ok = False;
  Boolean broadcast = False;

  if( inst == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "lockForGroup( NULL, %s )", id );
    return False;
  }

  /* should we use a mutex here? */

  data = Data(inst);

  /* wait only 10ms for getting the mutex: */
  if( !MutexOp.trywait( data->muxLock, 10 ) ) {
    return False;
  }

  if( !BlockOp.isFree( inst, id ) ) {
    MutexOp.post( data->muxLock );
    return False;
  }

  if( data->locIdGroup == NULL || StrOp.len( data->locIdGroup ) == 0 || StrOp.equals( "(null)", data->locIdGroup ) ) {
    if( data->locId == NULL || StrOp.len( data->locId ) == 0 || StrOp.equals( id, data->locId ) ) {
      data->locIdGroup = id;
      ok = True;
      broadcast = True;
    }
  }
  else if( StrOp.equals( id, data->locIdGroup ) ) {
    ok = True;
    broadcast = False;
  }


  /* Broadcast to clients. */
  if( ok && broadcast ) {
    iONode nodeD = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
    wBlock.setid( nodeD, data->id );
    if( data->locId != NULL && StrOp.equals( id, data->locId ) )
      wBlock.setreserved( nodeD, False );
    else
      wBlock.setreserved( nodeD, True );
    wBlock.setlocid( nodeD, id );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeD );
  }

  if( !ok ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                 "block \"%s\" already group locked by \"%s\".",
                 data->id, data->locIdGroup );
  }

  if( ok ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
        "block [%s] group locked for [%s]", data->id, id );
  }

  /* Unlock the semaphore: */
  MutexOp.post( data->muxLock );

  return ok;
}


static Boolean _isLinked( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  return data->linkto != NULL ? True:False;
}


static const char* _getGroup( iOBlock inst ) {
  iOBlockData data = Data(inst);
  return data->group;
}

static const char* _getLoc( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  return data->locId;
}

static const char* _getInLoc( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  return wBlock.getlocid( data->props );
}

static void _setGroup( iIBlockBase inst, const char* group ) {
  iOBlockData data = Data(inst);
  data->group = group;
}


static void _resetTrigs( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  data->trig_enter = False;
  data->trig_in    = False;
  data->trig_exit  = False;
  data->trig_out   = False;

  data->trig_enter_mid   = False;
  data->trig_exit_mid   = False;
  data->trig_renter_mid = False;
  data->trig_rexit_mid  = False;

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                 "Block \"%s\" resetTrigs.",
                 data->id );
}


static Boolean _unLink( iIBlockBase inst ) {
  if( inst != NULL ) {
    iOBlockData data = Data(inst);

    if( data->linkto != NULL ) {
      iOControl control = AppOp.getControl();
      iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
      wSysCmd.setcmd( cmd, wSysCmd.ulink );
      wSysCmd.setvalA( cmd, BlockOp.getTDport( inst ) );
      if( ControlOp.cmd( control, cmd, NULL ) ) {
        data->linkto = NULL;
        return True;
      }
    }
  }
  return False;
}


static Boolean _unLock( iIBlockBase inst, const char* id ) {
  if( inst != NULL && id != NULL ) {
    iOBlockData data = Data(inst);
    Boolean ok = False;

    /* wait only 10ms for getting the mutex: */
    if( !MutexOp.trywait( data->muxLock, 10 ) ) {
      return False;
    }

    if( data->locId == NULL || StrOp.len(data->locId) == 0 || StrOp.equals( id, data->locId ) ) {
      data->locId = NULL;
      BlockOp.resetTrigs( inst );
      wBlock.setlocid(data->props, "");
      data->crossing = False;

      if( data->closereq ) {
        wBlock.setstate( data->props, wBlock.closed );
        data->closereq = False;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set block to close: requested" );
      }

      ModelOp.setBlockOccupation( AppOp.getModel(), data->id, "", False, 0 );

      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                 "Block \"%s\" unlock %s, group lock=[%s]",
                 data->id, id, data->locIdGroup!=NULL?data->locIdGroup:"" );

      /* Broadcast to clients. */
      {
        iONode nodeD = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
        wBlock.setid( nodeD, data->id );
        wBlock.setstate( nodeD, wBlock.getstate(data->props) );

        if( data->locIdGroup == NULL ) {
          wBlock.setlocid( nodeD, "" );
        }
        else {
          wBlock.setlocid( nodeD, data->locIdGroup );
          wBlock.setreserved( nodeD, True );
        }
        ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeD );
      }
      /* Set signal. */

      __setDefaultAspect(inst);

      ok = True;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                     "\"%s\" tried to unlock block \"%s\" but \"%s\" owns it!",
                     id, data->id, data->locId==NULL?"?":data->locId );
    }

    data->occtime = 0;

    /* Unlock the semaphore: */
    MutexOp.post( data->muxLock );
  }

  return False;
}


static Boolean _unLockForGroup( iIBlockBase inst, const char* id ) {
  Boolean ok = False;
  if( inst != NULL && id != NULL ) {
    iOBlockData data = Data(inst);

    /* wait only 10ms for getting the mutex: */
    if( !MutexOp.trywait( data->muxLock, 10 ) ) {
      return False;
    }

    if( StrOp.equals( id, data->locIdGroup ) ) {
      data->locIdGroup = NULL;

      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                 "Block [%s] unlockForGroup [%s].",
                 data->id, id );


      ok = True;
      /* Broadcast to clients only if the block is not locked. */
      if( data->locId == NULL || StrOp.len(data->locId) == 0 ) {
        iONode nodeD = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
        wBlock.setid( nodeD, data->id );
        wBlock.setlocid( nodeD, "" );
        ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeD );
      }

    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                 "unmatching lockID: block [%s] unlockForGroup [%s] by [%s].",
                 data->id, data->locIdGroup, id );
    }

    /* Unlock the semaphore: */
    MutexOp.post( data->muxLock );

  }
  return ok;
}


static void _init( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  iOModel model = AppOp.getModel(  );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init block %s", data->id );
  /* this string pointer is not persistent! */
  data->locId = wBlock.getlocid( data->props );

  if( data->locId != NULL && StrOp.len( data->locId ) > 0 ) {
    iOLoc loc = ModelOp.getLoc( model, data->locId );
    if( loc != NULL ) {
      LocOp.setCurBlock( loc, data->id );
      /* overwrite data->locId with the static id from the loc object: */
      data->locId = LocOp.getId( loc );
      data->occtime = SystemOp.getTick();

      BlockOp.red( inst, False, False );
      BlockOp.red( inst, True, False );
      BlockOp.red( inst, False, True );
      BlockOp.red( inst, True, True );
    }
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "init() unknown locId: %s", data->locId );
  }
  else {
    __setDefaultAspect(inst);
  }
}

static Boolean _cmd( iIBlockBase inst, iONode nodeA ) {
  iOBlockData data = Data(inst);
  iOModel model = AppOp.getModel(  );

  /* Cmds: lcid="" state="" */
  const char* locid = wBlock.getlocid( nodeA );
  const char* state = wBlock.getstate( nodeA );

  if( locid != NULL ) {
    if( StrOp.len(locid) == 0 && data->locId != NULL && StrOp.len(data->locId) > 0 ) {
      /* inform loc */
      iOLoc loc = ModelOp.getLoc( model, data->locId );
      if( loc != NULL ) {
        LocOp.setCurBlock( loc, NULL );
        data->occtime = SystemOp.getTick();
      }
    }
    wBlock.setlocid( data->props, locid );
    ModelOp.setBlockOccupation( AppOp.getModel(), data->id, locid, False, 0 );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "%s locid=%s", NodeOp.getStr( data->props, "id", "" ), locid );
  }

  if( state != NULL ) {
    if( StrOp.equals( wBlock.closed, state ) ) {
      if( data->locId != NULL && StrOp.len( data->locId ) > 0 ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "close block request; already reserved by [%s]", data->locId );
        NodeOp.base.del(nodeA);
        data->closereq = True;
        return False;
      }
    }

    if( data->closereq ) {
      state = wBlock.closed;
      wBlock.setstate( nodeA, state );
      data->closereq = False;
    }
    wBlock.setstate( data->props, state );
    ModelOp.setBlockOccupation( AppOp.getModel(), data->id, locid, StrOp.equals( wBlock.closed, state ), 0 );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s state=%s", NodeOp.getStr( data->props, "id", "" ), state );
  }

  _init( inst );

  /* Broadcast to clients. */
  ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeA );

  return True;
}


static Boolean _green( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOBlockData data = Data(inst);
  Boolean semaphore = False;
  const char* sgId = NULL;

  if( distant )
    sgId = reverse ? wBlock.getwsignalR( data->props ):wBlock.getwsignal( data->props );
  else
    sgId = reverse ? wBlock.getsignalR( data->props ):wBlock.getsignal( data->props );

  if( sgId != NULL && StrOp.len( sgId ) > 0 ) {
    iOModel model = AppOp.getModel(  );
    iOSignal sg = ModelOp.getSignal( model, sgId );
    if( sg != NULL ) {
      SignalOp.green( sg );
      semaphore = StrOp.equals( wSignal.semaphore, wSignal.gettype(sg->base.properties(sg)) );
    }
  }

  return semaphore;
}

static Boolean _yellow( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOBlockData data = Data(inst);
  Boolean semaphore = False;
  const char* sgId = NULL;

  if( distant )
    sgId = reverse ? wBlock.getwsignalR( data->props ):wBlock.getwsignal( data->props );
  else
    sgId = reverse ? wBlock.getsignalR( data->props ):wBlock.getsignal( data->props );

  if( sgId != NULL && StrOp.len( sgId ) > 0 ) {
    iOModel model = AppOp.getModel(  );
    iOSignal sg = ModelOp.getSignal( model, sgId );
    if( sg != NULL ) {
      SignalOp.yellow( sg );
      semaphore = StrOp.equals( wSignal.semaphore, wSignal.gettype(sg->base.properties(sg)) );
    }
  }
  return semaphore;
}

static Boolean _white( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOBlockData data = Data(inst);
  Boolean semaphore = False;
  const char* sgId = NULL;

  if( distant )
    sgId = reverse ? wBlock.getwsignalR( data->props ):wBlock.getwsignal( data->props );
  else
    sgId = reverse ? wBlock.getsignalR( data->props ):wBlock.getsignal( data->props );

  if( sgId != NULL && StrOp.len( sgId ) > 0 ) {
    iOModel model = AppOp.getModel(  );
    iOSignal sg = ModelOp.getSignal( model, sgId );
    if( sg != NULL ) {
      SignalOp.white( sg );
      semaphore = StrOp.equals( wSignal.semaphore, wSignal.gettype(sg->base.properties(sg)) );
    }
  }
  return semaphore;
}

static Boolean _red( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOBlockData data = Data(inst);
  Boolean semaphore = False;
  const char* sgId = NULL;

  if( distant )
    sgId = reverse ? wBlock.getwsignalR( data->props ):wBlock.getwsignal( data->props );
  else
    sgId = reverse ? wBlock.getsignalR( data->props ):wBlock.getsignal( data->props );

  if( sgId != NULL && StrOp.len( sgId ) > 0 ) {
    iOModel model = AppOp.getModel(  );
    iOSignal sg = ModelOp.getSignal( model, sgId );
    if( sg != NULL ) {
      SignalOp.red( sg );
      semaphore = StrOp.equals( wSignal.semaphore, wSignal.gettype(sg->base.properties(sg)) );
    }
  }
  return semaphore;

}

/**
 * Checks for property changes.
 * todo: Range checking?
 */
static void _modify( iOBlock inst, iONode props ) {
  iOBlockData data = Data(inst);
  int cnt = NodeOp.getAttrCnt( props );
  Boolean move = StrOp.equals( wModelCmd.getcmd( props ), wModelCmd.move );

  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );
    NodeOp.setStr( data->props, name, value );
  }
  data->id = wBlock.getid( data->props );

  if( !move ) {
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

    /* re-init callback for all feedbacks: */
    __initFeedbackEvents( inst );

    /* re-init timer */
    data->timer = wBlock.getevttimer( data->props );
  }


  /* Broadcast to clients. */
  {
    iONode clone = (iONode)props->base.clone( props );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), clone );
  }

  props->base.del(props);
}

static const char* _tableHdr(void) {
  return "<tr bgcolor=\"#CCCCCC\"><th>BlockID</th><th>fba</th><th>fbb</th><th>x</th><th>y</th><th>z</th></tr>\n";
}

static char* _toHtml( void* inst ) {
  iOBlockData data = Data((iOBlock)inst);
  return StrOp.fmt( "<tr><td>%s</td><td>%s</td><td>%s</td><td align=\"right\">%d</td><td align=\"right\">%d</td><td align=\"right\">%d</td></tr>\n",
      NodeOp.getStr( data->props, "id", "?" ),
      NodeOp.getStr( data->props, "fba", "?" ),
      NodeOp.getStr( data->props, "fbb", "?" ),
      NodeOp.getInt( data->props, "x", 0 ),
      NodeOp.getInt( data->props, "y", 0 ),
      NodeOp.getInt( data->props, "z", 0 )
      );
}


static char* _getForm( void* inst ) {
  iOBlockData data = Data((iOBlock)inst);
  return NULL;
}


static char* _postForm( void* inst, const char* postdata ) {
  iOBlockData data = Data((iOBlock)inst);
  char* reply = StrOp.fmt( "Thanks!<br>" );
  return reply;
}


static Boolean _isTerminalStation( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  return wBlock.isterminalstation( data->props );
}


static Boolean _hasEnter2Route( iIBlockBase inst, const char* fromBlockID ) {
  iOBlockData data = Data(inst);
  iONode fbevt = wBlock.getfbevent( data->props );

  while( fbevt != NULL ) {
    const char* fromid = wFeedbackEvent.getfrom( fbevt );

    if( StrOp.equals( fromid, fromBlockID ) ) {
      if( StrOp.equals( wFeedbackEvent.enter2route_event, wFeedbackEvent.getaction(fbevt) ) )
        return True;
    }
    fbevt = wBlock.nextfbevent( data->props, fbevt );
  };

  return False;
}


static Boolean _hasPre2In( iIBlockBase inst, const char* fromBlockID ) {
  iOBlockData data = Data(inst);
  iONode fbevt = wBlock.getfbevent( data->props );

  while( fbevt != NULL ) {
    const char* fromid = wFeedbackEvent.getfrom( fbevt );

    if( StrOp.equals( fromid, fromBlockID ) ) {
      if( StrOp.equals( wFeedbackEvent.pre2in_event, wFeedbackEvent.getaction(fbevt) ) )
        return True;
    }
    fbevt = wBlock.nextfbevent( data->props, fbevt );
  };

  return False;
}


static void _reset( iIBlockBase inst ) {
  iOBlockData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
             "reset block [%s]", data->id );
  BlockOp.unLockForGroup(inst, data->locIdGroup );
  /*Unlock the occupying must be done manual */
  /*BlockOp.unLock(inst, data->locId);*/
  BlockOp.resetTrigs(inst);
}


static iOBlock _inst( iONode props ) {
  iOBlock     block = allocMem( sizeof( struct OBlock ) );
  iOBlockData data  = allocMem( sizeof( struct OBlockData ) );

  /* OBase operations */
  MemOp.basecpy( block, &BlockOp, 0, sizeof( struct OBlock ), data );

  data->props = props;
  data->locId = NodeOp.getStr( props, "locid", NULL );
  data->minbklc = wCtrl.getminbklc( AppOp.getIniNode( wCtrl.name() ) );
  data->fbEvents = MapOp.inst();
  data->fbGhostEvents = ListOp.inst();
  data->timer = wBlock.getevttimer( props );
  data->id = wBlock.getid( props );

  wBlock.setreserved( data->props, False );
  wBlock.setentering( data->props, False );

  data->muxLock = MutexOp.inst( NULL, True );

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "inst for %s", data->id );

  __initFeedbackEvents(block);

  instCnt++;

  return block;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/block.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
