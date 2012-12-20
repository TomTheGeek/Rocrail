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


/*
 Example:
--------------------------------------------------------------------------------
 ->-[b1]-(b1sen)->-(sb1ent)-[[sb1]]-(1)-(2)-(3)-(4)-(5)---[b2]-(b2sen)->-


 Sensors:
--------------------------------------------------------------------------------
 ID     | event   | owner
 -------+---------+-------
 b1sen  = enter2in  b1
 sb1ent = enter     sb1
 1      = section1  sb1[0]
 2      = section2  sb1[1]
 3      = section3  sb1[2]
 4      = section4  sb1[3]
 5      = section5  sb1[4]
 b2sen  = enter2in  b2


XML:
--------------------------------------------------------------------------------
  <sblist>
    <sb id="sb1" x="5" y="2" z="0" desc="" slen="30" gap="5" fbenterid="sb1ent">
      <section id="1" fbid="1" nr="0" idx="0" lcid=""/>
      <section id="2" fbid="2" nr="1" idx="1" lcid=""/>
      <section id="3" fbid="3" nr="2" idx="2" lcid=""/>
      <section id="4" fbid="4" nr="3" idx="3" lcid=""/>
      <section id="5" fbid="5" nr="4" idx="4" lcid=""/>
    </sb>
  </sblist>

  The idx attribute is for internal use and will be overwritten at every initialization.


Sequences:
--------------------------------------------------------------------------------
1) The isFree function checks if there is room for a train.
2) Train locks the stage block and the number of sections needed for the length if the isFree was positive.
3) Train runs in auto mode to the first free section seen from the exit side.
4) The train remains in auto mode if the target section is at the exit, otherwise it will be put in manual mode.
5) After the train at the exit section has left and has unlocked the stage block a move will be triggered.


Moving:
--------------------------------------------------------------------------------
A move can be triggered manually, by unlocking the stage block or after a move event.
If a train reaches the exit section it will be put back into auto mode.
1) The move function searches the first free section seen from the exit side, and the first one which
   is occupied also seen from the exit side.
2) The found train will be set speed to V_min.
3) After the event came in of the target section the speed will be reset to zero and
   section list must be updated regarding the number of sections occupied. (lcid)
4) Another move will be triggered: -> 1)

 */

#include "rocrail/impl/stage_impl.h"
#include "rocrail/public/app.h"
#include "rocrail/public/fback.h"
#include "rocrail/public/route.h"
#include "rocrail/public/control.h"
#include "rocrail/public/model.h"
#include "rocrail/public/loc.h"
#include "rocrail/public/signal.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"

#include "rocrail/wrapper/public/Ctrl.h"
#include "rocrail/wrapper/public/Stage.h"
#include "rocrail/wrapper/public/StageSection.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Signal.h"

#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/ActionCtrl.h"


static int instCnt = 0;


static void __initSensors( iOStage inst );
static Boolean __freeSection(iIBlockBase inst, const char* secid);
static Boolean __occSection(iIBlockBase inst, const char* secid, const char* lcid);
static Boolean __moveStageLocos(iIBlockBase inst);
static Boolean __dumpSections( iOStage inst );
static Boolean __freeSections(iIBlockBase inst, const char* locid);
static Boolean __getLength2Section( iOStage inst, const char* sectionid );


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

static void __checkAction( iOStage inst, const char* state ) {
  iOStageData data   = Data(inst);
  iOModel     model  = AppOp.getModel();
  iONode      action = wStage.getactionctrl( data->props );

  /* loop over all actions */
  while( action != NULL ) {
    int counter = atoi(wActionCtrl.getstate( action ));

    if( StrOp.len(wActionCtrl.getstate( action )) == 0 ||
        StrOp.equals(state, wActionCtrl.getstate( action )) )
    {

      iOAction Action = ModelOp.getAction(model, wActionCtrl.getid( action ));
      if( Action != NULL ) {
        wActionCtrl.setbkid(action, data->id);
        wActionCtrl.setlcid(action, data->locId);
        ActionOp.exec(Action, action);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action state does not match: [%s-%s]",
          wActionCtrl.getstate( action ), state );
    }

    action = wStage.nextactionctrl( data->props, action );
  }
}

static void _depart(iIBlockBase inst) {
  __checkAction((iOStage)inst, "depart");
}



/**  */
static Boolean _cmd( iIBlockBase inst ,iONode cmd ) {
  iOStageData data = Data(inst);
  iOModel model = AppOp.getModel(  );

  /* Cmds: lcid="" state="" */
  const char* command   = wStage.getcmd( cmd );
  const char* state     = NULL;
  const char* exitstate = NULL;

  if( NodeOp.findAttr( cmd, "state" ) )
    state = wStage.getstate( cmd );
  if( NodeOp.findAttr( cmd, "exitstate" ) )
    exitstate = wStage.getexitstate( cmd );

  if( command != NULL )
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
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "free all section" );
      __freeSections(inst, NULL);
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
      __checkAction((iOStage)inst, "closed");
    }
    wStage.setstate( data->props, state );
    ModelOp.setBlockOccupancy( AppOp.getModel(), data->id, NULL, StrOp.equals( wBlock.closed, state ), 0, 0, NULL );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s state=%s", NodeOp.getStr( data->props, "id", "" ), state );

    StageOp.init( inst );
    /* Broadcast to clients. */
    AppOp.broadcastEvent( (iONode)NodeOp.base.clone(data->props) );
  }

  if( exitstate != NULL ) {
    wStage.setexitstate( data->props, exitstate );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s exitstate=%s", data->id, exitstate );
    /* Broadcast to clients. */
    AppOp.broadcastEvent( (iONode)NodeOp.base.clone(data->props) );
  }

  NodeOp.base.del(cmd);

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
    __checkAction((iOStage)inst, "enter");
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


/**
 * Update the section list after a train has been moved.
 */
static Boolean __updateList4Move( iIBlockBase inst, const char* locId, int targetSection ) {
  iOStageData data = Data(inst);
  iOModel model = AppOp.getModel();
  int sections = ListOp.size( data->sectionList );
  int i = 0;
  int nrSections = 0;
  iOLoc loco = ModelOp.getLoc( model, locId );

  if( loco != NULL ) {
    int lclen = LocOp.getLen(loco);
    int freelen = 0;
    int freeupsection = targetSection;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "move loco=%s to target section %d(of %d)", data->locId, targetSection, sections );

    for( i = targetSection; i >= 0 ; i-- ) {
      iONode section = (iONode)ListOp.get( data->sectionList, i);
      if( wStageSection.getlen(section) > 0 ) {
        freelen += wStageSection.getlen(section);
      }
      else {
        freelen += data->sectionLength;
      }
      freeupsection = i;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "freelen=%d trainlen=%d", freelen, data->trainGap + lclen );
      if( freelen >= (data->trainGap + lclen) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "freelen=%d loco=%s freeupsection=%d", freelen, data->locId, freeupsection );
        break;
      }
    }

    /* check the sections */
    for( i = 0; i < freeupsection; i++ ) {
      iONode section = (iONode)ListOp.get( data->sectionList, i);
      if( StrOp.equals( locId, wStageSection.getlcid(section) ) ) {
        nrSections++;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "free section idx=%d", wStageSection.getidx(section ) );
        wStageSection.setlcid(section, NULL );
      }
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "moving %d sections of loco %s to section head %d", nrSections, data->locId, targetSection );

    if( nrSections >  0 ) {
      for( i = 0; i < nrSections; i++ ) {
        iONode section = (iONode)ListOp.get( data->sectionList, targetSection - i);
        wStageSection.setlcid(section, locId );
      }

      __dumpSections((iOStage)inst);

      return True;
    }
  }

  return False;
}


/**  */
static void _event( iIBlockBase inst ,Boolean puls ,const char* id ,const char* ident ,int val, int wheelcount ,iONode evtDescr ) {
  iOStageData data = Data(inst);
  iONode section = (iONode)MapOp.get( data->fbMap, id );
  Boolean endSection = __isEndSection(inst, section);

  if( StrOp.equals( wStage.getfbenterid(data->props), id ) ) {
    if( data->locId != NULL && StrOp.len(data->locId) > 0 && data->wait4enter ) {
      if( puls ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "expecting loco %s: ENTER", data->locId );
        if( data->locId != NULL ) {
          iOLoc loc = ModelOp.getLoc(AppOp.getModel(), data->locId);
          iONode nodeD = NodeOp.inst( wStage.name(), NULL, ELEMENT_NODE );
          wStage.setid( nodeD, data->id );
          wStage.setentering( nodeD, True );
          wStage.setlocid( nodeD, data->locId );
          AppOp.broadcastEvent( nodeD );
          __checkAction((iOStage)inst, "enter");

          if( loc != NULL ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "informing loco %s of ENTER event", data->locId );
            LocOp.event( loc, (obj)inst, enter_event, 0, True, NULL );
          }
          data->wait4enter = False;

          if( wStage.getentersignal( data->props ) != NULL && StrOp.len( wStage.getentersignal( data->props ) ) > 0 ) {
            iOModel model = AppOp.getModel();
            iOSignal sg = ModelOp.getSignal( model, wStage.getentersignal( data->props ) );
            if( sg != NULL ) {
              SignalOp.red( sg );
            }
          }
        }
      }
      else {
        /* Ignore off event. */
      }
    }
    else if( data->locId == NULL && puls ) {
      if( wCtrl.ispoweroffatghost( AppOp.getIniNode( wCtrl.name() ) ) ) {
        /* power off */
        AppOp.stop();
      }
      /* broadcast ghost state */
      {
        iONode nodeD = (iONode)NodeOp.base.clone(data->props);
        wStage.setstate( nodeD, wBlock.ghost );
        AppOp.broadcastEvent( nodeD );
        __checkAction((iOStage)inst, "ghost");
      }

      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Ghost train in staging block %s, fbid=%s, ident=%s, wait4enter=%d",
          data->id, id, ident, data->wait4enter );
    }
  }

  else if( section != NULL && !data->wait4enter ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensors [%s] %s event for stage [%s] section [%s][%d] of [%d]",
        id, puls?"on":"off", data->id, wStageSection.getid(section), wStageSection.getnr(section), data->sectionCount );

    __dumpSections((iOStage)inst);


    if( puls && wStageSection.getidx(section) == data->targetSection ) {
      iOLoc loc = ModelOp.getLoc(AppOp.getModel(), data->locId);
      iONode nodeD = (iONode)NodeOp.base.clone(data->props);
      wStage.setid( nodeD, data->id );
      wStage.setlocid( nodeD, "" );
      AppOp.broadcastEvent( nodeD );

      if( loc != NULL ) {
        Boolean execMove = False;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "informing loco %s of IN event (endsection=%d)", data->locId, endSection );
        if( !data->early2in )
          LocOp.event( loc, (obj)inst, in_event, 0, True, NULL );
        else
          data->early2in = False;

        if( inst->hasExtStop(inst) ) {
          iONode cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE);
          wLoc.setcmd(cmd, wLoc.velocity);
          if( StrOp.equals( wBlock.percent, wStage.getstopspeed(data->props) ) ) {
            char percent[32];
            StrOp.fmtb( percent, "%d", wStage.getspeedpercent(data->props) );
            wLoc.setV_hint(cmd, percent );
          }
          else
            wLoc.setV_hint(cmd, wStage.getstopspeed(data->props));

          LocOp.cmd(loc, cmd);
        }


        if( data->pendingMove ) {
          data->pendingMove = False;
          /* Move the train in the section list. */
          if( __updateList4Move( inst, data->locId, data->targetSection ) ) {
            iONode nodeD = (iONode)NodeOp.base.clone(data->props);
            wStage.setid( nodeD, data->id );
            wStage.setlocid( nodeD, "" );
            AppOp.broadcastEvent( nodeD );
            execMove = True;
          }
        }

        if( !endSection ) {
          /* stop loco */
          if( LocOp.isAutomode(loc) ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stop loco %s because its not in the end section", data->locId );
            LocOp.stop(loc, False);
          }

          if( inst->hasExtStop(inst) ) {
            iONode cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set loco %s speed to zero", data->locId );
            wLoc.setcmd(cmd, wLoc.velocity);
            wLoc.setV(cmd, 0);
            LocOp.cmd(loc, cmd);

          }

          if( !data->pendingFree && data->pendingSection != -1 ) {
            iONode s = (iONode)ListOp.get(data->sectionList, data->pendingSection );
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "pendingFree for section [%d]", data->pendingSection );
            /*
            if( s != NULL )
              wStageSection.setlcid(s, NULL);
            */
            data->pendingFree = True;
            data->pendingSection = -1;
          }

        }
        else {
          if( !LocOp.isAutomode(loc) ) {
            iONode cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set loco %s speed to zero", LocOp.getId(loc) );
            LocOp.setCurBlock(loc, data->id);
            wLoc.setcmd(cmd, wLoc.velocity);
            wLoc.setV(cmd, 0);
            LocOp.cmd(loc, cmd);
            if( !data->closereq && ModelOp.isAuto( AppOp.getModel() ) ) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set loco %s in auto mode", LocOp.getId(loc) );
              LocOp.go(loc);
              __checkAction((iOStage)inst, "exit");
            }
            else if( data->closereq ) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set loco %s not in auto mode; block is closed.", LocOp.getId(loc) );
            }

            if( !data->pendingFree ) {
              /*
              iONode s = (iONode)ListOp.get(data->sectionList, data->pendingSection );
              if( s != NULL )
                wStageSection.setlcid(s, NULL);
              */
              data->pendingFree = True;
            }
          }
        }
        ModelOp.setBlockOccupancy( AppOp.getModel(), data->id, LocOp.getId(loc), False, 0, 0, wStageSection.getid(section) );
        data->locId = NULL;
        wStage.setlocid( data->props, "" );

        if( execMove ) {
          __moveStageLocos(inst);
        }

      }
    }
    else if(puls) {
      iOLoc loc = ModelOp.getLoc(AppOp.getModel(), data->locId);
      if( loc != NULL && wStage.isinatlen(data->props) && !data->early2in ) {
        /* Check if train length does already fit inside for generating the **in** event:  */
        int sectionlen = __getLength2Section((iOStage)inst, wStageSection.getid(section));
        if( sectionlen >= LocOp.getLen(loc) ) {
          /* ToDo: Test.
           * The in_event will set the loco speed to zero in another thread...
           */
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "informing loco %s of IN event", data->locId );
          data->early2in = True;
          LocOp.stop(loc, False);
          LocOp.event( loc, (obj)inst, in_event, 0, True, NULL );
        }
      }
    }
    else if(!puls) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensors [%s] for stage [%s] came free; move locos...", id, data->id );
      __moveStageLocos(inst);
    }
  }

  else {
    if( puls ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
          "unexpected sensor [%s] event for stage [%s]: wait4enter=%s section=%s",
          id, data->id, data->wait4enter?"true":"false", section!=NULL?wStageSection.getid(section):"?" );
    }
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
  if( onexit )
    return wStage.getexitspeed(data->props);
  else {
    *percent = wStage.getspeedpercent(data->props);
    return wStage.getstopspeed(data->props);
  }
}


/**  */
static int _getVisitCnt( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  return 0;
}


static int _getDepartDelay( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return wStage.getdepartdelay(data->props) ;
}

static float _getmvspeed( iIBlockBase inst ) {
  return 0;
}

static int _getMaxKmh( iIBlockBase inst ) {
  return 0;
}


/**  */
static int _getWait( iIBlockBase inst ,iOLoc loc ,Boolean reverse, int* oppwait ) {
  iOStageData data = Data(inst);
  int blockwait = 1;
  *oppwait = 1;

  if( StrOp.equals( wLoc.cargo_cleaning, wLoc.getcargo( (iONode)loc->base.properties( loc ) ) ) ||
      StrOp.equals( wBlock.wait_none, wBlock.getwaitmode( data->props ) ) )
  {
    blockwait = 1;
  }
  else if( StrOp.equals( wBlock.wait_random, wBlock.getwaitmode( data->props ) ) ) {
    /* Random between 1 and 30. */
    int min = wStage.getminwaittime( data->props );
    int max = wStage.getmaxwaittime( data->props );
    float fmax = max;

    if( min == 0 )
      min = 1;
    if( max == 0 ) {
      max = 1;
      fmax = max;
    }

    if( max < min ) {
      fmax = min;
      min = max;
    }
    blockwait = min + (int) (fmax*rand()/(RAND_MAX+1.0));
  }
  else if( StrOp.equals( wBlock.wait_loc, wStage.getwaitmode( data->props ) ) && wLoc.getblockwaittime( (iONode)loc->base.properties( loc ) ) > 0) {
    blockwait = wLoc.getblockwaittime( (iONode)loc->base.properties( loc ) );
  }
  else if( wLoc.isuseownwaittime( (iONode)loc->base.properties( loc ) ) && wLoc.getblockwaittime( (iONode)loc->base.properties( loc ) ) > 0) {
    blockwait = wLoc.getblockwaittime( (iONode)loc->base.properties( loc ) );
  }
  else if( StrOp.equals( wBlock.wait_fixed, wStage.getwaitmode( data->props ) ) && wStage.getwaittime( data->props ) > 0 ) {
    blockwait = wStage.getwaittime( data->props );
  }

  *oppwait = blockwait;
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "wait for %d seconds", blockwait );
  return blockwait;
}


/**  */
static Boolean _green( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  iOStageData data = Data(inst);
  Boolean semaphore = False;
  const char* sgId = NULL;

  if( reverse || distant )
    return False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set green %s signal", distant?"enter":"exit" );

  sgId = distant ? wStage.getentersignal( data->props ):wStage.getexitsignal( data->props );

  if( sgId != NULL && StrOp.len( sgId ) > 0 ) {
    iOModel model = AppOp.getModel();
    iOSignal sg = ModelOp.getSignal( model, sgId );
    if( sg != NULL ) {
      SignalOp.green( sg );
      semaphore = StrOp.equals( wSignal.semaphore, wSignal.gettype(sg->base.properties(sg)) );
    }
  }
  return semaphore;
}


/**  */
static Boolean _hasEnter2Route( iIBlockBase inst ,const char* fromBlockId ) {
  iOStageData data = Data(inst);
  return False;
}


/**  */
static Boolean _hasExtStop( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return wStage.isinatlen(data->props);
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
    __checkAction((iOStage)inst, "occupied");
  }
  return;
}


/**  */
static void _init( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  int i = 0;
  int sections = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init stageblock [%s]", data->id );
  StageOp.red( inst, False, False );
  StageOp.red( inst, True, False );

  sections = ListOp.size( data->sectionList );

  for( i = 0; i < sections; i++ ) {
    iONode section = (iONode)ListOp.get( data->sectionList, i);
    if( wStageSection.getlcid(section) != NULL && StrOp.len( wStageSection.getlcid(section) ) > 0 ) {
      iOLoc loc = ModelOp.getLoc( AppOp.getModel(), wStageSection.getlcid(section) );
      if( loc != NULL ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "set current block for loco %s to %s", wStageSection.getlcid(section), data->id );
        LocOp.setCurBlock( loc, data->id );
      }
    }

  }

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
  int lenSections   = 0;
  int len[256];

  /* check the sections */
  data->freeSections = 0;
  for( i = 0; i < sections; i++ ) {
    iONode section = (iONode)ListOp.get( data->sectionList, i);
    iOFBack fb    = ModelOp.getFBack( AppOp.getModel(), wStageSection.getfbid(section) );
    iOFBack fbocc = ModelOp.getFBack( AppOp.getModel(), wStageSection.getfbidocc(section) );
    Boolean occ = False;
    if( fb != NULL )
      occ = FBackOp.isState(fb, "true");
    if( fbocc != NULL )
      occ |= FBackOp.isState(fbocc, "true");

    if( (!occ && wStageSection.getlcid(section) == NULL) || (!occ && StrOp.len(wStageSection.getlcid(section)) == 0) ) {
      /* free section */
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "section[%d] is free", i );

      len[i] = data->sectionLength;
      if( wStageSection.getlen(section) > 0 ) {
        lenSections += wStageSection.getlen(section);
        len[i] = wStageSection.getlen(section);
      }
      else
        lenSections += data->sectionLength;

      data->freeSections++;
      targetSection = i;
    }
    else {
      if( (occ && wStageSection.getlcid(section) == NULL) || ( occ && StrOp.len(wStageSection.getlcid(section)) == 0) ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "section[%d] is electrically occupied", i );
      }
      break;
    }
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "%d free sections, target section is %d", data->freeSections, targetSection );


  if( data->freeSections >= 1 ) {

    int lclen   = LocOp.getLen(loco);

    int freeLen = data->freeSections * data->sectionLength;
    if( lenSections > 0 )
      freeLen = lenSections;

    if( lclen == 0 ) {
      fit = False;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "train length of [%s] not set for stage [%s]",
          locid, data->id );
      return fit;
    }
    else if( (lclen + data->trainGap) <= freeLen ) {
      fit = True;
      int lentmp = 0;
      nrSections = 0;
      for( i = targetSection; i >= 0; i-- ) {
        lentmp += len[i];
        nrSections++;
        if( (lclen + data->trainGap) <= lentmp ) {
          break;
        }
      }
      /*
      nrSections = (lclen + data->trainGap) / data->sectionLength;
      nrSections += ((lclen + data->trainGap) % data->sectionLength) > 0 ? 1:0;
      */

      if( lock ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "locking section(s) for [%s]", locid);
        data->locId = locid;
        data->targetSection = targetSection;
        data->wait4enter = True;
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
  Boolean locoFit = False;
  iOFBack fb = NULL;

  fb = ModelOp.getFBack( AppOp.getModel(), wStage.getfbenterid(data->props) );
  if( fb != NULL && FBackOp.getState(fb) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "enter sensor [%s] is occupied", FBackOp.getId(fb));
    return False;
  }

  if( data->closereq || StrOp.equals( wStage.getstate(data->props), wBlock.closed ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stagingblock %s is closed", data->id );
    return False;
  }

  if( data->early2in ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stagingblock %s has a pending IN event", data->id );
    return False;
  }

  locoFit = __willLocoFit(inst, locid, False);
  if( !locoFit ) {
    __moveStageLocos(inst);
  }
  return locoFit;
}


/**  */
static Boolean _isLinked( iIBlockBase inst ) {
  iOStageData data = Data(inst);
  return False;
}

static Boolean _isTTBlock( iIBlockBase inst ) {
  return False;
}

static Boolean _isTD( iIBlockBase inst ) {
  return False;
}

static void _resetTD( iIBlockBase inst ) {
}


/**  */
static Boolean _isReady( iIBlockBase inst ) {
  return True;
}


/**  */
static int _isSuited( iIBlockBase inst ,iOLoc loc, int* restlen, Boolean checkprev ) {
  iOStageData data = Data(inst);
  if( __willLocoFit(inst, LocOp.getId(loc), False) ) {
    return wStage.issuitswell(data->props)?suits_well:suits_ok;
  }
  return suits_not;
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

  if( !StageOp.isFree(inst, locid) ) {
    return False;
  }

  if( !data->pendingFree ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "loco %s is pending", data->locId!=NULL?data->locId:"?" );
    return False;
  }

  if( data->early2in ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stagingblock %s has a pending IN event", data->id );
    return False;
  }

  if( !__willLocoFit(inst, locid, True) ) {
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
  __checkAction((iOStage)inst, "reserved");


  if( wStage.getentersignal( data->props ) != NULL && StrOp.len( wStage.getentersignal( data->props ) ) > 0 ) {
    iOModel model = AppOp.getModel();
    iOSignal sg = ModelOp.getSignal( model, wStage.getentersignal( data->props ) );
    if( sg != NULL ) {
      SignalOp.yellow( sg );
    }
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
  Boolean semaphore = False;
  const char* sgId = NULL;

  if( reverse )
    return False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set red %s signal", distant?"enter":"exit" );

  sgId = distant ? wStage.getentersignal( data->props ):wStage.getexitsignal( data->props );

  if( sgId != NULL && StrOp.len( sgId ) > 0 ) {
    iOModel model = AppOp.getModel();
    iOSignal sg = ModelOp.getSignal( model, sgId );
    if( sg != NULL ) {
      SignalOp.red( sg );
      semaphore = StrOp.equals( wSignal.semaphore, wSignal.gettype(sg->base.properties(sg)) );
    }
  }
  return semaphore;
}


/**  */
static void _reset( iIBlockBase inst, Boolean saveCurBlock ) {
  iOStageData data = Data(inst);
  int sections = ListOp.size( data->sectionList );
  int i = 0;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
             "reset stageblock [%s][%d]", data->id, sections );

  StageOp.resetTrigs(inst);
  data->wait4enter = False;


  /* Unlock the occupyied sections */
  for( i = 0; i < sections; i++ ) {
    iONode section = (iONode)ListOp.get( data->sectionList, i);
    const char* locid = wStageSection.getlcid(section);
    if( locid != NULL && StrOp.len( locid ) > 0 ) {
      if( ! saveCurBlock ) {
        /* free section */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unlock section[%d] from %s", i, locid );
        wStageSection.setlcid(section, NULL);
      }
      else {
        /* soft reset: just inform about occupation */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "section[%d] is occupied by %s", i, locid );
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "section[%d] is not occupied", i );
    }
  }

  /* enter and exit signal to red */
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset enter and exit signals [%s] [%s]", __name(), __id( inst ) );
  StageOp.red( inst, True,  False );
  StageOp.red( inst, False, False );

  /* Broadcast to clients. */
  AppOp.broadcastEvent( (iONode)NodeOp.base.clone(data->props) );


}


/**  */
static void _resetTrigs( iIBlockBase inst ) {
  iOStageData data = Data(inst);

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
      StageOp.red( inst, False, signalpair );
      StageOp.red( inst, True, signalpair );
      break;
    case 1:
      StageOp.green( inst, False, signalpair );
      StageOp.green( inst, True, signalpair );
      break;
    case 2:
      StageOp.yellow( inst, False, signalpair );
      StageOp.yellow( inst, True, signalpair );
      break;
    case 3:
      StageOp.white( inst, False, signalpair );
      StageOp.white( inst, True, signalpair );
      break;
  }
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


static Boolean __isSectionElectricallyFree(iIBlockBase inst, iONode section) {
  iOStageData data = Data(inst);

  iOFBack fb    = ModelOp.getFBack( AppOp.getModel(), wStageSection.getfbid(section) );
  iOFBack fbocc = ModelOp.getFBack( AppOp.getModel(), wStageSection.getfbidocc(section) );

  Boolean occ = False;
  if( fbocc != NULL )
    occ = FBackOp.isState(fbocc, "true");

  if( fb != NULL && FBackOp.isState(fb, "false") && !occ ) {
    return True;
  }

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

  /* wait only 100ms for getting the mutex: */
  if( !MutexOp.trywait( data->moveMux, 100 ) ) {
    return False;
  }


  if( (data->locId != NULL && StrOp.len(data->locId) > 0) || !data->pendingFree || data->pendingMove ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "can not move a loco because %s is pending(free=%d, move=%d)",
        data->locId!=NULL?data->locId:"-", data->pendingFree, data->pendingMove);
    MutexOp.post( data->moveMux );
    return locoMoved;
  }


  int i = 0;
  for( i = ListOp.size(data->sectionList)-1; i >= 0; i--) {
    iONode section = (iONode)ListOp.get(data->sectionList, i);

    if( nextFreeSection == NULL && (wStageSection.getlcid(section) == NULL || StrOp.len(wStageSection.getlcid(section)) == 0) ) {
      if( __isSectionElectricallyFree(inst, section ) ) {
        /* Last free section in the list */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "section %d [%d] is free (%s)",
            i, wStageSection.getidx(section), wStageSection.getlcid(section) == NULL ? "-":wStageSection.getlcid(section) );
        nextFreeSection = section;
        continue;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "section [%d] is electrically occupied", wStageSection.getidx(section) );
      }
    }

    if( nextFreeSection != NULL && firstOccupiedSection == NULL && (wStageSection.getlcid(section) != NULL && StrOp.len(wStageSection.getlcid(section)) > 0) ) {
      /* Last occpupied section in the list */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "section [%d] is occupied by %s",
          wStageSection.getidx(nextFreeSection), wStageSection.getlcid(nextFreeSection) );
      firstOccupiedSection = section;
      break;
    }
  }




  if( nextFreeSection != NULL && firstOccupiedSection != NULL ) {
    if( wStageSection.getidx(nextFreeSection) > wStageSection.getidx(firstOccupiedSection) )
    {
    iOLoc lc = ModelOp.getLoc( AppOp.getModel(), wStageSection.getlcid(firstOccupiedSection) );
    Boolean eOcc = __dumpSections((iOStage)inst);

      if( lc != NULL && !eOcc ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "moving loco %s from %s[%d] to %s[%d] in stage %s",
            wStageSection.getlcid(firstOccupiedSection), wStageSection.getid(firstOccupiedSection), wStageSection.getidx(firstOccupiedSection),
            wStageSection.getid(nextFreeSection), wStageSection.getidx(nextFreeSection), data->id );

        data->targetSection = wStageSection.getidx(nextFreeSection);
        data->pendingSection =  wStageSection.getidx(firstOccupiedSection);
        data->pendingFree = False;
        data->locId = LocOp.getId(lc);

        __dumpSections((iOStage)inst);

        wStageSection.setlcid(nextFreeSection, wStageSection.getlcid(firstOccupiedSection) );
        iONode cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE);
        wLoc.setcmd(cmd, wLoc.velocity);
        if( StrOp.equals( wBlock.percent, wStage.getstopspeed(data->props) ) ) {
          char percent[32];
          StrOp.fmtb( percent, "%d", wStage.getspeedpercent(data->props) );
          wLoc.setV_hint(cmd, percent );
        }
        else
          wLoc.setV_hint(cmd, wStage.getstopspeed(data->props));

        LocOp.cmd(lc, cmd);
        data->pendingMove = True;
        locoMoved = True;
      }
      else if(eOcc) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "cannot move locos because of electrically occupied section(s)" );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "loco %s not found", wStageSection.getlcid(firstOccupiedSection) );
      }
    }
    else {
      /* free section is before the one last occupied section... */
    }
  }

  if( data->pendingFree ) {
    cnt = ListOp.size(data->sectionList);
    lastSection = (iONode)ListOp.get(data->sectionList, cnt - 1);
    if( lastSection != NULL ) {
      iOLoc lc = ModelOp.getLoc( AppOp.getModel(), wStageSection.getlcid(lastSection) );
      if( lc != NULL && !LocOp.isAutomode(lc) ) {
        iONode cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set loco %s speed to zero", data->locId );
        wLoc.setcmd(cmd, wLoc.velocity);
        wLoc.setV(cmd, 0);
        LocOp.cmd(lc, cmd);

        LocOp.setCurBlock(lc, data->id);

        if( !StrOp.equals( wStage.getexitstate(data->props), wBlock.closed ) ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"start loco %s in the last section %s", wStageSection.getlcid(lastSection), wStageSection.getid(lastSection));
          cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE);
          wLoc.setcmd(cmd, wLoc.go);
          LocOp.cmd(lc, cmd);
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"exit of stage %s is closed", data->id );
        }
      }
      else if( lc != NULL && LocOp.isAutomode(lc) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"loco %s is in the last section %s and in auto mode", wStageSection.getlcid(lastSection), wStageSection.getid(lastSection));
        __dumpSections((iOStage)inst);
      }

    }
  }

  MutexOp.post( data->moveMux );
  return locoMoved;
}


static Boolean __freeSections(iIBlockBase inst, const char* locid) {
  iOStageData data = Data(inst);
  Boolean unlocked = False;
  int i = 0;
  int sections = ListOp.size( data->sectionList );

  for( i = 0; i < sections; i++ ) {
    iONode section = (iONode)ListOp.get( data->sectionList, i);
    if( locid == NULL || ( wStageSection.getlcid(section) != NULL && StrOp.equals(wStageSection.getlcid(section), locid) ) ) {
      /* free section */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unlock section[%d] from %s", i, locid!=NULL?locid:"-" );
      wStageSection.setlcid(section, NULL);
      ModelOp.setBlockOccupancy( AppOp.getModel(), data->id, "", False, 0, 0, wStageSection.getid(section) );
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

  if( secid == NULL || StrOp.len(secid) == 0 )
    return unlocked;

  for( i = 0; i < sections; i++ ) {
    iONode section = (iONode)ListOp.get( data->sectionList, i);
    if( StrOp.equals(wStageSection.getid(section), secid) ) {
      /* free section */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unlock section[%d] from %s", i, wStageSection.getlcid(section) );
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
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lock section[%d] by %s", i, lcid );
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
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unlock for loco %s", locid!=NULL?locid:"?" );
  if( locid != NULL ) {
    if(__freeSections(inst, locid)) {
      __checkAction((iOStage)inst, "free");
      return True;
    }
  }
  return False;
}


/**  */
static Boolean _unLockForGroup( iIBlockBase inst ,const char* locid ) {
  iOStageData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "stageblock not groupable" );
  return 0;
}


/**  */
static Boolean _wait( iIBlockBase inst ,iOLoc loc ,Boolean reverse, Boolean* oppwait ) {
  iOStageData data = Data(inst);
  *oppwait = True;
  return True;
}


/**  */
static Boolean _white( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  iOStageData data = Data(inst);
  Boolean semaphore = False;
  const char* sgId = NULL;

  if( reverse || distant )
    return False;

  sgId = distant ? wStage.getentersignal( data->props ):wStage.getexitsignal( data->props );

  if( sgId != NULL && StrOp.len( sgId ) > 0 ) {
    iOModel model = AppOp.getModel();
    iOSignal sg = ModelOp.getSignal( model, sgId );
    if( sg != NULL ) {
      SignalOp.white( sg );
      semaphore = StrOp.equals( wSignal.semaphore, wSignal.gettype(sg->base.properties(sg)) );
    }
  }
  return semaphore;
}


/**  */
static Boolean _yellow( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  iOStageData data = Data(inst);
  Boolean semaphore = False;
  const char* sgId = NULL;

  if( reverse || distant )
    return False;

  sgId = distant ? wStage.getentersignal( data->props ):wStage.getexitsignal( data->props );

  if( sgId != NULL && StrOp.len( sgId ) > 0 ) {
    iOModel model = AppOp.getModel();
    iOSignal sg = ModelOp.getSignal( model, sgId );
    if( sg != NULL ) {
      SignalOp.yellow( sg );
      semaphore = StrOp.equals( wSignal.semaphore, wSignal.gettype(sg->base.properties(sg)) );
    }
  }
  return semaphore;
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



static Boolean __dumpSections( iOStage inst ) {
  iOStageData data = Data(inst);
  Boolean eOcc = False;
  iONode section = wStage.getsection( data->props );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "---------- dump sections" );
  while( section != NULL ) {
    const char* lcid = wStageSection.getlcid( section );
    iOFBack fb    = ModelOp.getFBack( AppOp.getModel(), wStageSection.getfbid(section) );
    iOFBack fbocc = ModelOp.getFBack( AppOp.getModel(), wStageSection.getfbidocc(section) );
    Boolean occ = False;
    if( fb != NULL )
      occ = FBackOp.isState(fb, "true");
    if( fbocc != NULL )
      occ |= FBackOp.isState(fbocc, "true");

    if( occ && lcid == NULL || occ && StrOp.len(lcid) == 0 ) {
      eOcc = True;
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "section %d, loco %s, occ=%s",
        wStageSection.getnr(section), lcid==NULL?"-":lcid, occ?"true":"false" );
    section = wStage.nextsection( data->props, section );
  }
  return eOcc;
}

static Boolean __getLength2Section( iOStage inst, const char* sectionid ) {
  iOStageData data = Data(inst);
  int len = 0;
  iONode section = wStage.getsection( data->props );
  while( section != NULL ) {
    const char* lcid = wStageSection.getlcid( section );

    if( wStageSection.getlen(section) == 0 )
      len += wStage.getslen(data->props);
    else
      len += wStageSection.getlen(section);

    if( StrOp.equals(wStageSection.getid( section ), sectionid) ) {
      break;
    }
    section = wStage.nextsection( data->props, section );
  }
  return len;
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
  iOStageData data = Data(inst);
  iONode section = NULL;
  int i = 0;

  if( StrOp.equals( wStage.getexitstate(data->props), wBlock.closed ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
        "departure of loco %s from stage %s is not allowed; stageblock exit is closed", id, data->id );
    return False;
  }

  for( i = 0; i < ListOp.size(data->sectionList); i++ ) {
    iONode section = (iONode)ListOp.get( data->sectionList, i);
    if( StrOp.equals( id, wStageSection.getlcid(section) ) ) {
      if( __isEndSection(inst, section) )
        return True;
    }
  }
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
      "departure of loco %s from stage %s is not allowed; its not in the end section", id, data->id );

  return False;
}


static void _setSectionOcc(iOStage inst, const char* sectionid, const char* locoid) {
  iOStageData data = Data(inst);

  int sections = ListOp.size( data->sectionList );
  int i = 0;

  /* check the sections */
  for( i = 0; i < sections; i++ ) {
    iONode section = (iONode)ListOp.get( data->sectionList, i);
    if( StrOp.equals( wStageSection.getid( section ), sectionid ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set stage %s section %s occ to %s", data->id, sectionid, locoid );
      wStageSection.setlcid( section, locoid);
      break;
    }
  }
}


static void __watchdog( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOStage stage = (iOStage)ThreadOp.getParm( th );
  iOStageData data = Data(stage);

  ThreadOp.sleep(2000);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Watchdog thread for staging block \"%s\" started.", data->id );

  do {
    ThreadOp.sleep(1000);

    if( ModelOp.isAuto( AppOp.getModel() ) && data->pendingFree && !data->pendingMove ) {

      int cnt = ListOp.size(data->sectionList);
      if( cnt > 0  ) {
        iONode section = (iONode)ListOp.get(data->sectionList, cnt - 1);
        if( wStageSection.getlcid(section) == NULL || StrOp.len(wStageSection.getlcid(section)) == 0 ) {
          int i = 0;
          for( i = 0; i < cnt - 1; i++ ) {
            section = (iONode)ListOp.get(data->sectionList, i);
            if( wStageSection.getlcid(section) != NULL && StrOp.len(wStageSection.getlcid(section)) > 0 ) {
              __moveStageLocos((iIBlockBase)stage);
              break;
            }
          }
        }
      }

    }

  } while(data->run);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Watchdog thread for staging block \"%s\" ended.", data->id );
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
  data->moveMux        = MutexOp.inst( NULL, True );

  if( wStage.isusewd(props) ) {
    data->run            = True;
    data->watchdog       = ThreadOp.inst( data->id, &__watchdog, __Stage );
    ThreadOp.start( data->watchdog );
  }

  wStage.setlocid(data->props, NULL);

  __initSensors(__Stage);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stage %s created", data->id );

  instCnt++;
  return __Stage;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/stage.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
