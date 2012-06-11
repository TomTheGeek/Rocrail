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
static Boolean __freeSection(iIBlockBase inst, const char* secid);
static Boolean __occSection(iIBlockBase inst, const char* secid, const char* lcid);
static Boolean __moveStageLocos(iIBlockBase inst);


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
  const char* command = wStage.getcmd( cmd );
  const char* state   = wStage.getstate( cmd );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stage command: %s", command );

  if( StrOp.equals( wBlock.loc, command ) ) {
    const char* lcid  = wStage.getlocid(cmd);
    const char* secid = wStage.getsecid(cmd);
    if( secid != NULL && StrOp.len(secid) > 0 ) {
      if( lcid != NULL && StrOp.len(lcid) > 0 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set section %s occ to %s", secid, lcid );
        __occSection(inst, secid, lcid);
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "free section %s", secid );
        __freeSection(inst, secid);
      }
      AppOp.broadcastEvent( (iONode)NodeOp.base.clone(data->props) );
    }
  }

  else if( StrOp.equals( wStage.compress, command ) ) {
    __moveStageLocos(inst);
  }

  else if( state != NULL ) {
    if( StrOp.equals( wBlock.closed, state ) ) {
      data->closereq = True;
    }

    if( data->closereq ) {
      state = wBlock.closed;
      wStage.setstate( cmd, state );
      data->closereq = False;
    }
    wStage.setstate( data->props, state );
    ModelOp.setBlockOccupancy( AppOp.getModel(), data->id, NULL, StrOp.equals( wBlock.closed, state ), 0, 0, NULL );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s state=%s", NodeOp.getStr( data->props, "id", "" ), state );

    StageOp.init( inst );
    /* Broadcast to clients. */
    AppOp.broadcastEvent( cmd );
  }


  return True;
}


/**  */
static void _enterBlock( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "entered by loco [%s]", locid );

  if( locid != NULL ) {
    iONode nodeD = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
    wBlock.setid( nodeD, data->id );
    wBlock.setentering( nodeD, True );
    wBlock.setlocid( nodeD, locid );
    AppOp.broadcastEvent( nodeD );
  }
}

static void _exitBlock( iIBlockBase inst, const char* id, Boolean unexpected ) {
}


static Boolean __isEndSection(iIBlockBase inst, iONode section ) {
  iOStageData data = Data(inst);
  if( section == NULL )
    return False;
  int cnt = ListOp.size(data->sectionList);
  if( cnt > 0 && (iONode)ListOp.get(data->sectionList, cnt - 1) == section ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "section %s [%d] is last section", wStageSection.getid(section), cnt-1 );
    return True;
  }
  return False;
}


/**  */
static void _event( iIBlockBase inst ,Boolean puls ,const char* id ,const char* ident ,int val, int wheelcount ,iONode evtDescr ) {
  iOStageData data = Data(inst);
  iONode section = (iONode)MapOp.get( data->fbMap, id );
  Boolean endSection = __isEndSection(inst, section);

  if( StrOp.equals( wStage.getfbenterid(data->props), id ) ) {
    if( data->locId != NULL && StrOp.len(data->locId) > 0 ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "expecting loco %s: ENTER", data->locId );
      if( data->locId != NULL ) {
        iOLoc loc = ModelOp.getLoc(AppOp.getModel(), data->locId);
        iONode nodeD = NodeOp.inst( wStage.name(), NULL, ELEMENT_NODE );
        wStage.setid( nodeD, data->id );
        wStage.setentering( nodeD, True );
        wStage.setlocid( nodeD, data->locId );
        AppOp.broadcastEvent( nodeD );

        if( loc != NULL ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "informing loco %s of ENTER event", data->locId );
          LocOp.event( loc, (obj)inst, enter_event, 0, True, NULL );
        }

      }
    }
  }

  else if( section != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensors [%s] %s event for stage [%s] section [%s][%d] of [%d]",
        id, puls?"on":"off", data->id, wStageSection.getid(section), wStageSection.getnr(section), data->sectionCount );

    if( wStageSection.getidx(section) == data->targetSection ) {
      iOLoc loc = ModelOp.getLoc(AppOp.getModel(), data->locId);
      iONode nodeD = (iONode)NodeOp.base.clone(data->props);
      wStage.setid( nodeD, data->id );
      wStage.setlocid( nodeD, "" );
      AppOp.broadcastEvent( nodeD );

      if( loc != NULL ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "informing loco %s of IN event (endsection=%d)", data->locId, endSection );
        LocOp.event( loc, (obj)inst, in_event, 0, True, NULL );
        if( !endSection ) {
          /* stop loco */
          if( LocOp.isAutomode(loc) ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stop loco %s because its not in the end section", data->locId );
            LocOp.stop(loc, False);
          }
          else {
            iONode cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set loco %s speed to zero", data->locId );
            wLoc.setcmd(cmd, wLoc.velocity);
            wLoc.setV(cmd, 0);
            LocOp.cmd(loc, cmd);
          }

          if( data->pendingFree && data->pendingSection != -1 ) {
            iONode s = (iONode)ListOp.get(data->sectionList, data->pendingSection );
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "pendingFree for section [%d]", data->pendingSection );
            if( s != NULL )
              wStageSection.setlcid(s, NULL);
            data->pendingFree = False;
            data->pendingSection = -1;
          }

        }
        else {
          if( !LocOp.isAutomode(loc) ) {
            iONode cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set loco %s speed in auto mode", LocOp.getId(loc) );
            LocOp.setCurBlock(loc, data->id);
            wLoc.setcmd(cmd, wLoc.velocity);
            wLoc.setV(cmd, 0);
            LocOp.cmd(loc, cmd);
            LocOp.go(loc);
            if( !data->pendingFree ) {
              iONode s = (iONode)ListOp.get(data->sectionList, data->pendingSection );
              if( s != NULL )
                wStageSection.setlcid(s, NULL);
              data->pendingFree = True;
            }
          }
        }
        ModelOp.setBlockOccupancy( AppOp.getModel(), data->id, LocOp.getId(loc), False, 0, 0, wStageSection.getid(section) );
        data->locId = NULL;
        wStage.setlocid( data->props, "" );
      }
    }
  }

  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unknown sensors [%s] for stage [%s]...", id, data->id );
  }

  return;
}

static void _fbEvent( obj inst, Boolean puls, const char* id, const char* ident, int val, int wheelcount ) {
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


static int _getDepartDelay( iIBlockBase inst ) {
  return 0;
}

static float _getmvspeed( iIBlockBase inst ) {
  return 0;
}

static int _getMaxKmh( iIBlockBase inst ) {
  return 0;
}


/**  */
static int _getWait( iIBlockBase inst ,iOLoc loc ,Boolean reverse ) {
  iOStageData data = Data(inst);
  return 1;
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
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "loco [%s] is in", locid );
  if( locid != NULL ) {
    iONode nodeD = (iONode)NodeOp.base.clone(data->props);
    wStage.setid( nodeD, data->id );
    wStage.setreserved( nodeD, False );
    wStage.setlocid( nodeD, "" );
    AppOp.broadcastEvent( nodeD );
  }
  return;
}


/**  */
static void _init( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init stageblock [%s]", data->id );
  return;
}

/**
 * calculate if the train will fit in the free sections
 */
static Boolean __willLocoFit(iIBlockBase inst ,const char* locid, Boolean lock) {
  iOStageData data = Data(inst);
  iOModel model = AppOp.getModel();
  iOLoc loco = ModelOp.getLoc( model, locid );
  Boolean fit = False;

  int sections = ListOp.size( data->sectionList );
  int i = 0;
  int targetSection = -1;
  int nrSections    = 0;

  /* check the sections */
  data->freeSections = 0;
  for( i = 0; i < sections; i++ ) {
    iONode section = (iONode)ListOp.get( data->sectionList, i);
    if( wStageSection.getlcid(section) == NULL || StrOp.len(wStageSection.getlcid(section)) == 0 ) {
      /* free section */
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "section[%d] is free", i );
      data->freeSections++;
      targetSection = i;
    }
    else {
      break;
    }
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "%d free sections, target section is %d", data->freeSections, targetSection );


  if( data->freeSections >= 1 ) {

    int lclen   = LocOp.getLen(loco);
    int freeLen = data->freeSections * data->sectionLength;

    if( lclen == 0 ) {
      fit = False;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "train length of [%s] not set for stage [%s]",
          locid, data->id );
      return fit;
    }
    else if( (lclen + data->trainGap) < freeLen ) {
      fit = True;
      nrSections = (lclen + data->trainGap) / data->sectionLength;
      nrSections += ((lclen + data->trainGap) % data->sectionLength) > 0 ? 1:0;

      if( lock ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "locking section(s) for [%s]", locid);
        data->locId = locid;
        data->targetSection = targetSection;
        for( i = targetSection - (nrSections-1); i < targetSection + 1; i++ ) {
          iONode section = (iONode)ListOp.get( data->sectionList, i);
          wStageSection.setlcid( section, locid );
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
              "section [%d] locked for [%s]", wStageSection.getidx(section), locid);
        }
      }
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "train length of [%s][%d] does %sfit in stage [%s][%d] on [%d] sections",
        locid, lclen, fit?"":"NOT ", data->id, freeLen, nrSections );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no sections available on stage [%s]",
        data->id );
  }

  return fit;
}

static Boolean _isState( iIBlockBase inst, const char* state ) {
  return False;
}

/**
 * Check the train length if it will fit in the available section(s).
 */
static Boolean _isFree( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  return __willLocoFit(inst, locid, False);
}


/**  */
static Boolean _isLinked( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return False;
}

static Boolean _isTTBlock( iIBlockBase inst ) {
  return False;
}

/**  */
static Boolean _isReady( iIBlockBase inst ) {
  return True;
}


/**  */
static int _isSuited( iIBlockBase inst ,iOLoc loc ) {
  return __willLocoFit(inst, LocOp.getId(loc), False) ? suits_ok:suits_not;
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
  if( !__willLocoFit(inst, locid, True) ) {
    return False;
  }

  if( !data->pendingFree ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "loco %s is pending", data->locId!=NULL?data->locId:"?" );
    return False;
  }

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "loco %s locks stageblock", locid );
  {
    iONode nodeD = (iONode)NodeOp.base.clone(data->props);
    wStage.setid( nodeD, data->id );
    wStage.setreserved( nodeD, True );
    wStage.setlocid( nodeD, locid );
    AppOp.broadcastEvent( nodeD );
  }
  return True;
}


/**  */
static Boolean _lockForGroup( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "stageblock not groupable" );
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
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "stageblock not groupable" );
  return;
}


/**  */
static Boolean _setLocSchedule( iIBlockBase inst ,const char* scid ) {
  iOStageData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "loco schedule %s", scid );
  return False;
}


static Boolean _setLocTour( iIBlockBase inst ,const char* tourid ) {
  iOStageData data = Data(inst);
  return False;
}


/**  */
static void _setManager( iIBlockBase inst ,iIBlockBase manager ) {
  iOStageData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "stageblock not managable" );
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
  int cnt = 0;

  iONode nextFreeSection = NULL;
  iONode firstOccupiedSection = NULL;
  iONode lastSection = NULL;

  if( (data->locId != NULL && StrOp.len(data->locId) > 0) || !data->pendingFree ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "can not move a loco because %s is pending", data->locId!=NULL?data->locId:"?");
    return locoMoved;
  }

  int i = 0;
  for( i = 0; i < ListOp.size(data->sectionList); i++) {
    iONode section = (iONode)ListOp.get(data->sectionList, i);
    if( firstOccupiedSection == NULL && (wStageSection.getlcid(section) != NULL && StrOp.len(wStageSection.getlcid(section)) > 0) ) {
      firstOccupiedSection = section;
    }
    if( firstOccupiedSection != NULL && nextFreeSection == NULL && (wStageSection.getlcid(section) == NULL || StrOp.len(wStageSection.getlcid(section)) == 0) ) {
      nextFreeSection = section;
      break;
    }
  }
  if( nextFreeSection != NULL && firstOccupiedSection != NULL ) {
    iOLoc lc = ModelOp.getLoc( AppOp.getModel(), wStageSection.getlcid(firstOccupiedSection) );
    if( lc != NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "moving loco %s from %s to %s in stage %s",
          wStageSection.getlcid(firstOccupiedSection), wStageSection.getid(firstOccupiedSection),
          wStageSection.getid(nextFreeSection), data->id );

      data->targetSection = wStageSection.getidx(nextFreeSection);
      data->pendingSection =  wStageSection.getidx(firstOccupiedSection);
      data->pendingFree = False;
      data->locId = LocOp.getId(lc);

      wStageSection.setlcid(nextFreeSection, wStageSection.getlcid(firstOccupiedSection) );
      iONode cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE);
      wLoc.setcmd(cmd, wLoc.velocity);
      wLoc.setV_hint(cmd, wLoc.min);
      LocOp.cmd(lc, cmd);
      /* TODO: V_min and wait for event of sensor firstFreeSection */
      locoMoved = True;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "loco %s not found", wStageSection.getlcid(firstOccupiedSection) );
    }
  }

  if( data->pendingFree ) {
    cnt = ListOp.size(data->sectionList);
    lastSection = (iONode)ListOp.get(data->sectionList, cnt - 1);
    if( lastSection != NULL ) {
      iOLoc lc = ModelOp.getLoc( AppOp.getModel(), wStageSection.getlcid(lastSection) );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"start loco %s in the last section %s", wStageSection.getlcid(lastSection), wStageSection.getid(lastSection));
      if( lc != NULL && !LocOp.isAutomode(lc) ) {
        iONode cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE);
        LocOp.setCurBlock(lc, data->id);
        wLoc.setcmd(cmd, wLoc.go);
        LocOp.cmd(lc, cmd);
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"loco %s is in the last section %s and in auto mode", wStageSection.getlcid(lastSection), wStageSection.getid(lastSection));
      }

    }
  }

  return locoMoved;
}


static Boolean __freeSections(iIBlockBase inst, const char* locid) {
  iOStageData data = Data(inst);
  Boolean unlocked = False;
  int i = 0;
  int sections = ListOp.size( data->sectionList );

  for( i = 0; i < sections; i++ ) {
    iONode section = (iONode)ListOp.get( data->sectionList, i);
    if( wStageSection.getlcid(section) != NULL && StrOp.equals(wStageSection.getlcid(section), locid) ) {
      /* free section */
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "unlock section[%d] from %s", i, locid );
      wStageSection.setlcid(section, NULL);
      unlocked = True;
    }
  }

  if( unlocked ) {
    if( StrOp.equals( data->locId, locid ) ) {
      data->locId = NULL;
      wStage.setlocid( data->props, "" );
    }
    data->pendingFree = True;
    __moveStageLocos(inst);
  }

  /* Broadcast to clients. */
  AppOp.broadcastEvent( (iONode)NodeOp.base.clone(data->props) );

  return unlocked;
}


static Boolean __freeSection(iIBlockBase inst, const char* secid) {
  iOStageData data = Data(inst);
  Boolean unlocked = False;
  int i = 0;
  int sections = ListOp.size( data->sectionList );

  for( i = 0; i < sections; i++ ) {
    iONode section = (iONode)ListOp.get( data->sectionList, i);
    if( StrOp.equals(wStageSection.getid(section), secid) ) {
      /* free section */
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "unlock section[%d] from %s", i, wStageSection.getlcid(section) );
      wStageSection.setlcid(section, NULL);
      ModelOp.setBlockOccupancy( AppOp.getModel(), data->id, "", False, 0, 0, secid );
      unlocked = True;
    }
  }

  return unlocked;
}


static Boolean __occSection(iIBlockBase inst, const char* secid, const char* lcid) {
  iOStageData data = Data(inst);
  Boolean locked = False;
  int i = 0;
  int sections = ListOp.size( data->sectionList );

  for( i = 0; i < sections; i++ ) {
    iONode section = (iONode)ListOp.get( data->sectionList, i);
    if( StrOp.equals(wStageSection.getid(section), secid) ) {
      /* occ section */
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "lock section[%d] by %s", i, lcid );
      wStageSection.setlcid(section, lcid);
      ModelOp.setBlockOccupancy( AppOp.getModel(), data->id, lcid, False, 0, 0, secid );
      locked = True;
    }
  }

  return locked;
}


/**  */
static Boolean _unLock( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "unlock for loco %d", locid );
  return __freeSections(inst, locid);
}


/**  */
static Boolean _unLockForGroup( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "stageblock not groupable" );
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

  data->freeSections = 0;
  MapOp.clear( data->fbMap );
  ListOp.clear( data->sectionList );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init sensors for stage %s...", data->id );

  iOFBack fb = ModelOp.getFBack( model, wStage.getfbenterid(data->props) );
  if( fb != NULL ) {
    FBackOp.setListener( fb, (obj)inst, &_fbEvent );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "enter sensors for stage %s is not defined", data->id );
  }


  while( section != NULL ) {
    const char* fbid = wStageSection.getfbid( section );
    wStageSection.setnr(section, sectionNr);

    if( StrOp.len( fbid ) > 0 ) {
    fb = ModelOp.getFBack( model, fbid );

      if( fb != NULL ) {
        FBackOp.setListener( fb, (obj)inst, &_fbEvent );
        MapOp.put( data->fbMap, fbid, (obj)section);
        if( wStageSection.getlcid(section) == NULL || StrOp.len(wStageSection.getlcid(section)) == 0 ) {
          data->freeSections++;
        }
      }
    }
    wStageSection.setidx(section, sectionNr);
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


static void _setSectionOcc(iOStage inst, const char* sectionid, const char* locoid) {
  iOStageData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set stage %s section %s occ to %s", data->id, sectionid, locoid );

  int sections = ListOp.size( data->sectionList );
  int i = 0;

  /* check the sections */
  for( i = 0; i < sections; i++ ) {
    iONode section = (iONode)ListOp.get( data->sectionList, i);
    if( StrOp.equals( wStageSection.getid( section ), sectionid ) ) {
      wStageSection.setlcid( section, locoid);
      break;
    }
  }
}


/**  */
static struct OStage* _inst( iONode props ) {
  iOStage __Stage = allocMem( sizeof( struct OStage ) );
  iOStageData data = allocMem( sizeof( struct OStageData ) );
  MemOp.basecpy( __Stage, &StageOp, 0, sizeof( struct OStage ), data );

  /* Initialize data->xxx members... */
  data->props          = props;
  data->id             = wStage.getid( props );
  data->fbMap          = MapOp.inst();
  data->sectionList    = ListOp.inst();
  data->sectionLength  = wStage.getslen(props);
  data->trainGap       = wStage.getgap(props);
  data->pendingFree    = True;
  data->pendingSection = -1;

  wStage.setlocid(data->props, NULL);

  __initSensors(__Stage);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stage %s created", data->id );

  instCnt++;
  return __Stage;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/stage.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
