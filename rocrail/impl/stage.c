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

#include "rocrail/impl/stage_impl.h"
#include "rocrail/public/app.h"
#include "rocrail/public/fback.h"
#include "rocrail/public/route.h"
#include "rocrail/public/control.h"
#include "rocrail/public/model.h"
#include "rocrail/public/loc.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"

#include "rocrail/wrapper/public/Stage.h"
#include "rocrail/wrapper/public/StageSection.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/Feedback.h"



static int instCnt = 0;


static void __initSensors( iOStage inst );


/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOStageData data = Data(inst);
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
  iOStageData data = Data(inst);
  return data->props;
}

static const char* __id( void* inst ) {
  iOStageData data = Data(inst);
  return data->id;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OStage ----- */

static void _depart(iIBlockBase inst) {
}


/**  */
static Boolean _cmd( iIBlockBase inst ,iONode cmd ) {
  iOStageData data = Data(inst);
  iOModel model = AppOp.getModel(  );

  /* Cmds: lcid="" state="" */
  const char* state = wStage.getstate( cmd );

  if( state != NULL ) {
    if( StrOp.equals( wBlock.closed, state ) ) {
      if( MapOp.size(data->lcMap) > 0 ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
            "close block request; already reserved by [%d] locos", MapOp.size(data->lcMap) );
        NodeOp.base.del(cmd);
        data->closereq = True;
        return False;
      }
    }

    if( data->closereq ) {
      state = wBlock.closed;
      wStage.setstate( cmd, state );
      data->closereq = False;
    }
    wStage.setstate( data->props, state );
    /*ModelOp.setBlockOccupation( AppOp.getModel(), data->id, locid, StrOp.equals( wBlock.closed, state ), 0 );*/
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s state=%s", NodeOp.getStr( data->props, "id", "" ), state );
  }

  StageOp.init( inst );

  /* Broadcast to clients. */
  AppOp.broadcastEvent( cmd );

  return True;
}


/**  */
static void _enterBlock( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  /*
  wBlock.setlocid( data->props, id );
  if( id != NULL ) {
    iONode nodeD = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
    wBlock.setid( nodeD, data->id );
    wBlock.setentering( nodeD, True );
    wBlock.setlocid( nodeD, id );
    AppOp.broadcastEvent( nodeD );
  }
  */
}


/**  */
static void _event( iIBlockBase inst ,Boolean puls ,const char* id ,long ident ,int val, int wheelcount ,iONode evtDescr ) {
  iOStageData data = Data(inst);
  iONode section = (iONode)MapOp.get( data->fbMap, id );

  if( section != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensors [%s] %s event for stage [%s] section [%s][%d] of [%d]",
        id, puls?"on":"off", data->id, wStageSection.getid(section), wStageSection.getnr(section), data->sectionCount );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unknown sensors [%s] for stage [%s]...", id, data->id );
  }

  return;
}

static void _fbEvent( obj inst, Boolean puls, const char* id, int ident, int val, int wheelcount ) {
  _event( (iIBlockBase)inst, puls, id, ident, val, wheelcount, NULL );
}


/**  */
static const char* _getFromBlockId( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return data->fromBlockId;
}


/**  */
static const char* _getInLoc( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return NULL;
}


/**  */
static const char* _getLoc( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return NULL;
}


/**  */
static iIBlockBase _getManager( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return NULL;
}


/**  */
static int _getOccTime( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return 0;
}

static int _getWheelCount( iIBlockBase inst ) {
  return 0;
}

/**  */
static const char* _getState( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return wStage.getstate(data->props);
}


/**  */
static int _getTDaddress( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return 0;
}


/**  */
static const char* _getTDiid( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return 0;
}


/**  */
static int _getTDport( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return 0;
}


/**  */
static const char* _getVelocity( iIBlockBase inst ,int* percent ,Boolean onexit ,Boolean reverse, Boolean onstop ) {
  iOStageData data = Data(inst);
  return wBlock.min;
}


/**  */
static int _getVisitCnt( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  return 0;
}


/**  */
static int _getWait( iIBlockBase inst ,iOLoc loc ,Boolean reverse ) {
  iOStageData data = Data(inst);
  return 1000;
}


/**  */
static Boolean _green( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  iOStageData data = Data(inst);
  return False;
}


/**  */
static Boolean _hasEnter2Route( iIBlockBase inst ,const char* fromBlockId ) {
  iOStageData data = Data(inst);
  return False;
}


/**  */
static Boolean _hasExtStop( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return False;
}


/**  */
static obj _hasManualSignal( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  iOStageData data = Data(inst);
  return NULL;
}


/**  */
static Boolean _hasPre2In( iIBlockBase inst ,const char* fromBlockId ) {
  iOStageData data = Data(inst);
  return False;
}


/**  */
static void _inBlock( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  return;
}


/**  */
static void _init( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return;
}

/**
 * calculate if the train will fit in the free sections
 */
static Boolean __willLocoFit(iIBlockBase inst ,const char* locid) {
  iOStageData data = Data(inst);
  iOModel model = AppOp.getModel();
  iOLoc loco = ModelOp.getLoc( model, locid );
  Boolean fit = False;

  if( data->freeSections > 1 ) {

    int lclen   = LocOp.getLen(loco);
    int freeLen = data->freeSections * data->sectionLength;

    if( lclen == 0 ) {
      fit = False;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "train length of [%s] not set for stage [%s]",
          locid, data->id );
      return fit;
    }
    else if(lclen < freeLen ) {
      fit = True;
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "train length of [%s][%d] does %sfit in stage [%s][%d]",
        locid, lclen, fit?"":"NOT ", data->id, freeLen );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no sections available on stage [%s]",
        data->id );
  }

  return fit;
}

/**
 * Check the train length if it will fit in the available section(s).
 */
static Boolean _isFree( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  return __willLocoFit(inst, locid);
}


/**  */
static Boolean _isLinked( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return False;
}


/**  */
static Boolean _isReady( iIBlockBase inst ) {
  return True;
}


/**  */
static int _isSuited( iIBlockBase inst ,iOLoc loc ) {
  return __willLocoFit(inst, LocOp.getId(loc)) ? suits_ok:suits_not;
}


/**  */
static Boolean _isTerminalStation( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return False;
}


/**  */
static Boolean _link( iIBlockBase inst ,iIBlockBase linkto ) {
  iOStageData data = NULL;
  return False;
}


/**  */
static Boolean _lock( iIBlockBase inst ,const char* locid ,const char* blockid ,const char* routeid ,Boolean crossing ,Boolean reset ,Boolean reverse ,int indelay ) {
  iOStageData data = Data(inst);
  if( !__willLocoFit(inst, locid) ) {
    return False;
  }

  return 0;
}


/**  */
static Boolean _lockForGroup( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  return False;
}


/**  */
static Boolean _red( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  iOStageData data = Data(inst);
  return 0;
}


/**  */
static void _reset( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
             "reset stageblock [%s]", data->id );
  /*Unlock the occupying must be done manual */
  StageOp.resetTrigs(inst);
}


/**  */
static void _resetTrigs( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  data->trig_enter = False;
  data->trig_in    = False;
  data->trig_exit  = False;
  data->trig_out   = False;

  data->trig_enter_mid   = False;
  data->trig_exit_mid   = False;
  data->trig_renter_mid = False;
  data->trig_rexit_mid  = False;

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                 "StageBlock [%s] resetTrigs.", data->id );
}


/**  */
static void _setAnalog( iIBlockBase inst ,Boolean analog ) {
  iOStageData data = Data(inst);
  return;
}


/**  */
static void _setDefaultAspect( iIBlockBase inst ,Boolean signalpair ) {
  iOStageData data = Data(inst);
  return;
}


/**  */
static void _setGroup( iIBlockBase inst ,const char* group ) {
  iOStageData data = Data(inst);
  return;
}


/**  */
static Boolean _setLocSchedule( iIBlockBase inst ,const char* scid ) {
  iOStageData data = Data(inst);
  return False;
}


/**  */
static void _setManager( iIBlockBase inst ,iIBlockBase manager ) {
  iOStageData data = Data(inst);
  return;
}


/**  */
static Boolean _unLink( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return False;
}



/**
 * move all locos to fill up the space freed up
 */
static Boolean __moveStageLocos(iIBlockBase inst) {
  iOStageData data = Data(inst);
  Boolean locoMoved = False;

  iONode firstFreeSection = NULL;
  iONode firstOccupiedSection = NULL;
  int i = 0;
  for( i = 0; i < ListOp.size(data->sectionList); i++) {
    iONode section = (iONode)ListOp.get(data->sectionList, i);
    if( firstFreeSection == NULL && (wStageSection.getlcid(section) == NULL || StrOp.len(wStageSection.getlcid(section)) == 0) ) {
      firstFreeSection = section;
    }
    if( firstOccupiedSection == NULL && (wStageSection.getlcid(section) != NULL && StrOp.len(wStageSection.getlcid(section)) > 0) ) {
      firstOccupiedSection = section;
      break;
    }
  }
  if( firstFreeSection != NULL && firstOccupiedSection != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "moving loco %s from %s to %s in stage %s",
        wStageSection.getlcid(firstOccupiedSection), wStageSection.getid(firstOccupiedSection),
        wStageSection.getid(firstFreeSection), data->id );

    /* TODO: V_min and wait for event of sensor firstFreeSection */
    locoMoved = True;
  }

  return locoMoved;
}


static Boolean __freeSections(iIBlockBase inst, const char* locid) {
  iOStageData data = Data(inst);
  int i = 0;
  iOList list = (iOList)MapOp.remove( data->lcMap, locid );

  if( list == NULL )
    return False;

  for( i = 0; i < ListOp.size(list); i++) {
    iONode section = (iONode)ListOp.get(list, i);
    wStageSection.setlcid(section, NULL);
  }

  __moveStageLocos(inst);

  /* clean up list */
  ListOp.base.del(list);

  return True;
}


/**  */
static Boolean _unLock( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  Boolean unlocked = False;
  if( MapOp.get( data->lcMap, locid) != NULL ) {
    unlocked = __freeSections(inst, locid);
  }
  return unlocked;
}


/**  */
static Boolean _unLockForGroup( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  return 0;
}


/**  */
static Boolean _wait( iIBlockBase inst ,iOLoc loc ,Boolean reverse ) {
  iOStageData data = Data(inst);
  return True;
}


/**  */
static Boolean _white( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  iOStageData data = Data(inst);
  return 0;
}


/**  */
static Boolean _yellow( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  iOStageData data = Data(inst);
  return 0;
}


static void _modify( iOStage inst, iONode props ) {
  iOStageData data = Data(inst);
  int cnt = NodeOp.getAttrCnt( props );
  Boolean move = StrOp.equals( wModelCmd.getcmd( props ), wModelCmd.move );

  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );
    NodeOp.setStr( data->props, name, value );
  }
  data->id = wStage.getid( data->props );

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
    __initSensors( inst );

  }
  else {
    NodeOp.removeAttrByName(data->props, "cmd");
  }


  /* Broadcast to clients. */
  {
    iONode clone = (iONode)props->base.clone( props );
    AppOp.broadcastEvent( clone );
  }

  props->base.del(props);
}


/**
 * map all fb's and set the listener to the common _event
 */
static void __initSensors( iOStage inst ) {
  iOStageData data = Data(inst);
  iOModel model = AppOp.getModel();
  int sectionNr = 0;
  iONode section = wStage.getsection( data->props );

  MapOp.clear( data->fbMap );
  ListOp.clear( data->sectionList );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init sensors for stage %s...", data->id );

  while( section != NULL ) {
    const char* fbid = wStageSection.getfbid( section );
    wStageSection.setnr(section, sectionNr);

    if( StrOp.len( fbid ) > 0 ) {
    iOFBack fb = ModelOp.getFBack( model, fbid );

      if( fb != NULL ) {
        FBackOp.setListener( fb, (obj)inst, &_fbEvent );
        MapOp.put( data->fbMap, fbid, (obj)section);
      }
    }
    ListOp.add( data->sectionList, (obj)section );
    sectionNr++;
    section = wStage.nextsection( data->props, section );
  };
  data->sectionCount = sectionNr;

}


static void _setCarCount( iIBlockBase inst, int count ) {
}

static void _acceptIdent( iIBlockBase inst, Boolean accept ) {
}

static Boolean _isDepartureAllowed( iIBlockBase inst, const char* id ) {
  return True;
}


/**  */
static struct OStage* _inst( iONode props ) {
  iOStage __Stage = allocMem( sizeof( struct OStage ) );
  iOStageData data = allocMem( sizeof( struct OStageData ) );
  MemOp.basecpy( __Stage, &StageOp, 0, sizeof( struct OStage ), data );

  /* Initialize data->xxx members... */
  data->props       = props;
  data->id          = wStage.getid( props );
  data->fbMap       = MapOp.inst();
  data->lcMap       = MapOp.inst();
  data->sectionList = ListOp.inst();
  data->sectionLength = wStage.getslen(props);

  __initSensors(__Stage);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stage %s created", data->id );

  instCnt++;
  return __Stage;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/stage.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
