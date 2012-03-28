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
#include <time.h>

#include "rocrail/impl/action_impl.h"
#include "rocrail/public/route.h"
#include "rocrail/public/app.h"
#include "rocrail/public/model.h"
#include "rocrail/public/control.h"
#include "rocrail/public/output.h"
#include "rocrail/public/text.h"
#include "rocrail/public/loc.h"
#include "rocrail/public/fback.h"

#include "rocs/public/system.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/ActionCond.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/Tour.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/AutoCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/Text.h"
#include "rocrail/wrapper/public/State.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOActionData data = Data(inst);
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
  iOActionData data = Data(inst);
  return data->action;
}

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OAction ----- */


static Boolean __checkConditions(struct OAction* inst, iONode actionctrl) {
  iOActionData data = Data(inst);
  iOModel model = AppOp.getModel();
  Boolean automode = ModelOp.isAuto(model);
  Boolean rc = True;

  if( actionctrl != NULL ) {
    iONode actionCond = wActionCtrl.getactioncond(actionctrl);
    if( automode && wActionCtrl.isauto(actionctrl) || !automode && wActionCtrl.ismanual(actionctrl)) {
      while( actionCond != NULL && rc ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Action condition: [%s-%s:%s] ",
            wActionCond.gettype(actionCond),
            wActionCond.getid(actionCond),
            wActionCond.getstate(actionCond) );

        /* Block */
        if( StrOp.equals( wBlock.name(), wActionCond.gettype(actionCond) ) ) {
          const char* id = wActionCond.getid( actionCond );
          iIBlockBase bk = ModelOp.getBlock( model, id );
          if( bk != NULL ) {
            rc = bk->isState(bk, wActionCond.getstate(actionCond) );
          }
          else
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block not found [%s]", id );
        }

        /* System */
        else if( StrOp.equals( wSysCmd.name(), wActionCond.gettype(actionCond) ) ) {
          iONode state = ControlOp.getState(AppOp.getControl());
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "check system: power=%s", wState.ispower(state)?"go":"stop" );
          if( wState.ispower(state) && !StrOp.equalsi( wSysCmd.go, wActionCond.getstate(actionCond) ) )
            rc = False;
          else if( !wState.ispower(state) && !StrOp.equalsi( wSysCmd.stop, wActionCond.getstate(actionCond) ) )
            rc = False;
          /* clean up */
          NodeOp.base.del(state);
        }

        /* Output */
        else if( StrOp.equals( wOutput.name(), wActionCond.gettype(actionCond) ) ) {
          const char* id = wActionCond.getid( actionCond );
          iOOutput co = ModelOp.getOutput( model, id );
          if( co != NULL ) {
            rc = OutputOp.isState(co, wActionCond.getstate(actionCond) );
          }
          else
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "output not found [%s]", id );
        }

        /* Switch */
        else if( StrOp.equals( wSwitch.name(), wActionCond.gettype(actionCond) ) ) {
          const char* id = wActionCond.getid( actionCond );
          iOSwitch sw = ModelOp.getSwitch( model, id );
          if( sw != NULL ) {
            rc = SwitchOp.isState(sw, wActionCond.getstate(actionCond) );
          }
          else
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "object not found [%s]", id );
        }
        else if( StrOp.equals( wSignal.name(), wActionCond.gettype(actionCond) ) ) {
          const char* id = wActionCond.getid( actionCond );
          iOSignal sg = ModelOp.getSignal( model, id );
          if( sg != NULL ) {
            rc = SignalOp.isState(sg, wActionCond.getstate(actionCond) );
          }
          else
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "object not found [%s]", id );
        }
        else if( StrOp.equals( wFeedback.name(), wActionCond.gettype(actionCond) ) ) {
          const char* id = wActionCond.getid( actionCond );
          iOLoc lc = ModelOp.getLoc(model, wActionCtrl.getlcid(actionctrl));
          iOFBack fb = ModelOp.getFBack( model, id );
          const char* state = "";
          const char* direction = NULL;

          iOStrTok tok = StrTokOp.inst(wActionCond.getstate(actionCond), ',');
          if(StrTokOp.hasMoreTokens(tok))
            state = StrTokOp.nextToken(tok);
          if(StrTokOp.hasMoreTokens(tok))
            direction = StrTokOp.nextToken(tok);
          StrTokOp.base.del(tok);

          if( fb != NULL ) {
            if( StrOp.len(state) > 0 && StrOp.equals(state, FBackOp.getIdentifier(fb)) )
              rc = True;
            else if( StrOp.len(state) == 0 )
              rc = FBackOp.isState(fb, state );

            if( rc && direction != NULL && lc != NULL ) {
              /**/
              Boolean dir = LocOp.getDir(lc);
              if( StrOp.equals( "forwards", direction ) && !dir ) {
                rc = False;
                TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                    "loco %s direction %s does not match [%s]", LocOp.getId(lc), dir?"forwards":"reverse", direction );
              }
              else if( StrOp.equals( "reverse", direction ) && dir ) {
                rc = False;
                TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                    "loco %s direction %s does not match [%s]", LocOp.getId(lc), dir?"forwards":"reverse", direction );
              }
            }
          }
          else
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "object not found [%s]", id );
        }
        else if( StrOp.equals( wLoc.name(), wActionCond.gettype(actionCond) ) ) {
          const char* id = wActionCond.getid( actionCond );
          const char* state = wActionCond.getstate(actionCond);
          iOLoc lc = ModelOp.getLoc(model, wActionCtrl.getlcid(actionctrl));
          rc = False;

          if( lc != NULL && state[0] == '#' ) {
            iOStrTok tok = StrTokOp.inst(state, ',');
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                "check if loco address %d fits in [%s]", LocOp.getAddress(lc), state );
            while( StrTokOp.hasMoreTokens(tok) ) {
              const char* sAddr = StrTokOp.nextToken(tok);
              char* sHAddr = StrOp.find( sAddr, "-" );
              if( sHAddr != NULL ) {
                int addr = 0;
                int hAddr = atoi(sHAddr+1);
                *sHAddr = '\0';
                addr = atoi(sAddr+1);
                if( LocOp.getAddress(lc) >= addr && LocOp.getAddress(lc) <= hAddr ) {
                  rc = True;
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                      "loco address %d fits in range [%d-%d]", LocOp.getAddress(lc), addr, hAddr );
                  break; /* break out the while loop */
                }
              }
              else {
                int addr = atoi(sAddr+1);
                if( addr == LocOp.getAddress(lc) ) {
                  rc = True;
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                      "loco address %d equals [%d]", LocOp.getAddress(lc), addr );
                  break; /* break out the while loop */
                }
              }
            };
            StrTokOp.base.del(tok);
          }
          else if( lc != NULL && state[0] == 'x' ) {
            iOStrTok tok = StrTokOp.inst(state, ',');
            rc = True;
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                "check if loco address %d does not match [%s]", LocOp.getAddress(lc), state );
            while( StrTokOp.hasMoreTokens(tok) ) {
              const char* sAddr = StrTokOp.nextToken(tok);
              char* sHAddr = StrOp.find( sAddr, "-" );
              if( sHAddr != NULL ) {
                int addr = 0;
                int hAddr = atoi(sHAddr+1);
                *sHAddr = '\0';
                addr = atoi(sAddr+1);
                if( LocOp.getAddress(lc) >= addr && LocOp.getAddress(lc) <= hAddr ) {
                  rc = False;
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                      "loco address %d falls in excluded range [%d-%d]", LocOp.getAddress(lc), addr, hAddr );
                  break; /* break out the while loop */
                }
              }
              else {
                int addr = atoi(sAddr+1);
                if( addr == LocOp.getAddress(lc) ) {
                  rc = False;
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                      "loco address %d equals exclusion [%d]", LocOp.getAddress(lc), addr );
                  break; /* break out the while loop */
                }
              }
            };
            StrTokOp.base.del(tok);
          }
          else if( lc != NULL && ( StrOp.equals( state, "diesel" ) ||
                  StrOp.equals( state, "steam" ) || StrOp.equals( state, "electric" ) ) ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "check type of loco id [%s]", LocOp.getId(lc) );
            rc = StrOp.equals( state, LocOp.getEngine(lc) );
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                "loco type [%s] does %smatch condtion [%s]", LocOp.getEngine(lc), rc?"":"not ", state );
          }
          else {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                "check if loco id [%s] equals [%s]", id, wActionCtrl.getlcid(actionctrl) );
            if( StrOp.equals("*", id ) || StrOp.equals( wActionCtrl.getlcid(actionctrl), id ) ) {
              iOLoc lc = ModelOp.getLoc(model, wActionCtrl.getlcid(actionctrl));
              if( lc != NULL ) {
                Boolean dir = LocOp.getDir(lc);
                Boolean enterside = LocOp.getBlockEnterSide(lc);
                rc = True;
                if( StrOp.equals( "forwards", wActionCond.getstate(actionCond) ) && !dir ) {
                  rc = False;
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                      "loco %s direction %s does not match [%s]", LocOp.getId(lc),
                      dir?"forwards":"reverse", wActionCond.getstate(actionCond) );
                }
                else if( StrOp.equals( "reverse", wActionCond.getstate(actionCond) ) && dir ) {
                  rc = False;
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                      "loco %s direction %s does not match [%s]", LocOp.getId(lc),
                      dir?"forwards":"reverse", wActionCond.getstate(actionCond) );
                }
                else if(StrOp.equals( "+", wActionCond.getstate(actionCond) ) && !enterside ) {
                  rc = False;
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                      "loco %s block enter side %s does not match [%s]", LocOp.getId(lc),
                      enterside?"+":"-", wActionCond.getstate(actionCond) );
                }
                else if(StrOp.equals( "-", wActionCond.getstate(actionCond) ) && enterside ) {
                  rc = False;
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                      "loco %s block enter side %s does not match [%s]", LocOp.getId(lc),
                      enterside?"+":"-", wActionCond.getstate(actionCond) );
                }
                else if( StrOp.equals( wActionCond.getstate(actionCond), wLoc.min ) || StrOp.equals( wActionCond.getstate(actionCond), wLoc.mid )
                      || StrOp.equals( wActionCond.getstate(actionCond), wLoc.cruise ) || StrOp.equals( wActionCond.getstate(actionCond), wLoc.max ) ) {
                  if( !StrOp.equals( wActionCond.getstate(actionCond), LocOp.getV_hint(lc) ) ) {
                    rc = False;
                    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                        "loco %s speed %s does not match [%s]", LocOp.getId(lc),
                        LocOp.getV_hint(lc), wActionCond.getstate(actionCond) );
                  } else {
                    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                        "loco %s speed %s matches [%s]", LocOp.getId(lc),
                        LocOp.getV_hint(lc), wActionCond.getstate(actionCond) );
                  }
                }
                else if( StrOp.startsWith( wActionCond.getstate(actionCond), "fon" ) || StrOp.startsWith( wActionCond.getstate(actionCond), "foff" ) ) {
                  iOStrTok tok = StrTokOp.inst(wActionCond.getstate(actionCond), ',');
                  const char* fonoff = NULL;
                  const char* fnumber = NULL;
                  if(StrTokOp.hasMoreTokens(tok))
                    fonoff = StrTokOp.nextToken(tok);
                  if(StrTokOp.hasMoreTokens(tok))
                    fnumber = StrTokOp.nextToken(tok);
                  StrTokOp.base.del(tok);

                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                      "checking function of loco %s", LocOp.getId(lc) );

                  if( fonoff != NULL && fnumber != NULL ) {
                    int nr = atoi(fnumber);
                    int fx = wLoc.getfx(LocOp.base.properties(lc));
                    Boolean lights = wLoc.isfn(LocOp.base.properties(lc));
                    rc = False;
                    if( StrOp.equals( "fon", fonoff ) && nr == 0 )
                      rc = lights;
                    else if( StrOp.equals( "foff", fonoff ) && nr == 0 )
                      rc = !lights;
                    else if( StrOp.equals( "fon", fonoff ) && nr != 0 )
                      rc = ( fx & (1 << (nr-1)) );
                    else if( StrOp.equals( "foff", fonoff ) && nr != 0 )
                      rc = ( !(fx & (1 << (nr-1)) ) );

                    if( !rc ) {
                      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                          "loco %s function %d does not match state [%s]", LocOp.getId(lc), nr, fonoff );
                    }
                    else {
                      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                          "loco %s function %d does match state [%s]", LocOp.getId(lc), nr, fonoff );
                    }

                  }
                  else {
                    rc = False;
                  }
                }
              }
              else {
                TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                    "loco id [%s] not found", wActionCtrl.getlcid(actionctrl) );
                rc = False;
              }
            }
            else
              rc = StrOp.equals(id, wActionCtrl.getlcid(actionctrl) );
          }
        }


        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, rc?"Condition is true.":"Condition is not true; skip action." );


        actionCond = wActionCtrl.nextactioncond(actionctrl, actionCond);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s%s%s mode action, running in %s mode, skip action %s",
          wActionCtrl.isauto(actionctrl)?"Auto":"",
          wActionCtrl.isauto(actionctrl)&&wActionCtrl.ismanual(actionctrl)?"/":"",
          wActionCtrl.ismanual(actionctrl)?"Manual":"",
          ModelOp.isAuto(model)?"auto":"manual",
          wActionCtrl.getid(actionctrl) );
      rc = False;
    }
  }

  return rc;
}


/**  */
static void __executeAction( struct OAction* inst, iONode actionctrl ) {
  iOActionData data = Data(inst);
  iOModel model = AppOp.getModel();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Action execution %s [%s-%s:%s] ",
      wAction.getid(data->action),
      wAction.gettype(data->action),
      wAction.getoid(data->action),
      wAction.getcmd(data->action) );

  /* output action */
  if( StrOp.equals( wOutput.name(), wAction.gettype( data->action ) ) ) {
    const char* id = wAction.getoid( data->action );
    iOOutput co = ModelOp.getOutput( model, id );
    if( co != NULL ) {
      iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
      const char* cmdStr = wAction.getcmd( data->action );
      wOutput.setcmd( cmd, cmdStr );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting output [%s] to [%s]", id, cmdStr );
      OutputOp.cmd( co, cmd, True );
    }
  }

  /* sensor action */
  else if( StrOp.equals( wFeedback.name(), wAction.gettype( data->action ) ) ) {
    const char* id = wAction.getoid( data->action );
    iOFBack fb = ModelOp.getFBack( model, id );
    if( fb != NULL ) {
      iONode cmd = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
      const char* cmdStr = wAction.getcmd( data->action );
      int error = 0;
      wFeedback.setid( cmd, id );
      wFeedback.setstate( cmd, StrOp.equals("on", cmdStr) ? True:False);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting sensor [%s] to [%s]", id, cmdStr );
      FBackOp.event( fb, cmd );
    }
  }

  /* text action */
  else if( StrOp.equals( wText.name(), wAction.gettype( data->action ) ) ) {
    const char* id = wAction.getoid( data->action );
    iOText tx = ModelOp.getText( model, id );
    if( tx != NULL ) {
      iONode cmd = NodeOp.inst( wText.name(), NULL, ELEMENT_NODE );
      const char* cmdStr = wAction.getcmd( data->action );
      wText.setcmd( cmd, cmdStr );
      wText.setformat(cmd, wAction.getparam(data->action));
      wText.setreflcid(cmd, wActionCtrl.getlcid(actionctrl));
      wText.setrefbkid(cmd, wActionCtrl.getbkid(actionctrl));
      wText.setcounter(cmd, wActionCtrl.getcounter(actionctrl));
      wText.setcarcount(cmd, wActionCtrl.getcarcount(actionctrl));
      wText.setcountedcars(cmd, wActionCtrl.getcountedcars(actionctrl));
      wText.setwheelcount(cmd, wActionCtrl.getwheelcount(actionctrl));
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting text [%s] to [%s]", id, cmdStr );
      TextOp.base.event( tx, cmd );
    }
  }

  /* ext action */
  else if( StrOp.equals( "ext", wAction.gettype( data->action ) ) ) {
    /* check for a external action */
    const char* extaction = wAction.getcmd( data->action );
    const char* extparam  = wAction.getparam( data->action );
    if( extaction != NULL && StrOp.len(extaction) > 0 ) {
      if( wActionCtrl.getparam(actionctrl) != NULL && StrOp.len(wActionCtrl.getparam(actionctrl)) > 0 ) {
        char* s = NULL;
        if( extparam != NULL && StrOp.len(extparam) > 0 )
          s = StrOp.fmt("\"%s %s\" \"%s\"", extaction, extparam, wActionCtrl.getparam(actionctrl) );
        else
          s = StrOp.fmt("\"%s\" \"%s\"", extaction, wActionCtrl.getparam(actionctrl) );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "executing [%s]", s );
        SystemOp.system( s, True, False );
        StrOp.free(s);
      }
      else {
        char* s = NULL;
        if( extparam != NULL && StrOp.len(extparam) > 0 )
          s = StrOp.fmt("\"%s %s\"", extaction, extparam );
        else
          s = StrOp.fmt("\"%s\"", extaction );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "executing [%s]", s );
        SystemOp.system( s, True, False );
        StrOp.free(s);
      }
    }
  }

  /* switch action */
  else if( StrOp.equals( wSwitch.name(), wAction.gettype( data->action ) ) ) {
    iOSwitch sw = ModelOp.getSwitch( model, wAction.getoid( data->action ) );
    int error = 0;
    if( sw != NULL ) {
      iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
      if( StrOp.equals( wAction.getcmd( data->action ), wAction.switch_flip )) {
        if( SwitchOp.isState(sw, wSwitch.turnout )) {
          wSwitch.setcmd( cmd, wSwitch.straight );
        } else {
          wSwitch.setcmd( cmd, wSwitch.turnout );
        }
        SwitchOp.cmd( sw, cmd, True, 0, &error, NULL );
      } else {
        wSwitch.setcmd( cmd, wAction.getcmd( data->action ) );
        SwitchOp.cmd( sw, cmd, True, 0, &error, NULL );
      }
    }
  }

  /* signal action */
  else if( StrOp.equals( wSignal.name(), wAction.gettype( data->action ) ) ) {
    iOSignal sg = ModelOp.getSignal( model, wAction.getoid( data->action ) );
    if( sg != NULL ) {
      int error = 0;
      iONode cmd = NodeOp.inst( wSignal.name(), NULL, ELEMENT_NODE );
      wSignal.setcmd( cmd, wAction.getcmd( data->action ) );
      SignalOp.cmd( sg, cmd, True );
    }
  }

  /* block action */
  else if( StrOp.equals( wBlock.name(), wAction.gettype( data->action ) ) ) {
    iIBlockBase bl = ModelOp.getBlock( model, wAction.getoid( data->action ) );
    if( bl != NULL ) {
      if( StrOp.equals( "unlock", wAction.getcmd( data->action ) ) ) {
        bl->unLock(bl, wAction.getparam( data->action ));
      }
      else if( StrOp.equals( wAction.block_event, wAction.getcmd( data->action ) ) ) {
        bl->event(bl, True, wAction.getparam( data->action ), 0, 0, 0, NULL);
        bl->event(bl, False, wAction.getparam( data->action ), 0, 0, 0, NULL);
      }
      else if( StrOp.equals( wBlock.closed, wAction.getcmd( data->action ) ) ) {
        iONode cmd = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
        wBlock.setstate( cmd, wBlock.closed );
        bl->cmd(bl, cmd);
      }
      else if( StrOp.equals( wBlock.open, wAction.getcmd( data->action ) ) ) {
        iONode cmd = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
        wBlock.setstate( cmd, wBlock.open );
        bl->cmd(bl, cmd);
      }
      else if( StrOp.equals( wSignal.white, wAction.getcmd( data->action ) ) ) {
        bl->white(bl, False, False);
      }
      else if( StrOp.equals( wSignal.yellow, wAction.getcmd( data->action ) ) ) {
        bl->yellow(bl, False, False);
      }
      else if( StrOp.equals( wSignal.green, wAction.getcmd( data->action ) ) ) {
        bl->green(bl, False, False);
      }
      else if( StrOp.equals( wSignal.red, wAction.getcmd( data->action ) ) ) {
        bl->red(bl, False, False);
      }
      else if( StrOp.equals( "schedule", wAction.getcmd( data->action ) ) ) {
        bl->setLocSchedule(bl, wAction.getparam( data->action ));
      }
      else if( StrOp.equals( wTour.name(), wAction.getcmd( data->action ) ) ) {
        bl->setLocTour(bl, wAction.getparam( data->action ));
      }
      else if( StrOp.equals( wAction.block_acceptident, wAction.getcmd( data->action ) ) ) {
        bl->acceptIdent(bl, True);
      }
      else if( StrOp.equals( wAction.block_setloc, wAction.getcmd( data->action ) ) ) {
        iOLoc lc = ModelOp.getLoc( model, wAction.getparam( data->action ));
        if( lc != NULL ) {
          iONode cmd = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
          wBlock.setlocid(cmd, "");
          bl->cmd(bl, cmd);
          cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
          wLoc.setid( cmd, wAction.getparam( data->action ) );
          wLoc.setcmd( cmd, wLoc.block );
          wLoc.setblockid( cmd, wAction.getoid( data->action ) );
          LocOp.cmd(lc, cmd);
        }
      }
      else if( StrOp.equals( wAction.loco_go, wAction.getcmd( data->action ) ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action: loco_go %s ",
            bl->getLoc(bl));

        iOLoc lc = ModelOp.getLoc( model, bl->getLoc(bl));
        if( lc != NULL ) {
          iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
          wLoc.setid( cmd, bl->getLoc(bl) );
          wLoc.setcmd( cmd, wLoc.go );
          LocOp.cmd(lc, cmd);
        }

      }
    }
  }

  /* route action */
  else if( StrOp.equals( wRoute.name(), wAction.gettype( data->action ) ) ) {
    iORoute st = ModelOp.getRoute( model, wAction.getoid( data->action ) );
    if( st != NULL ) {
      if( StrOp.equals( wAction.route_set, wAction.getcmd( data->action ) ) ) {
        RouteOp.go( st );
      }
      else if( StrOp.equals( wAction.route_lockset, wAction.getcmd( data->action ) ) ) {
        const char* lockid = wAction.getparam(data->action);
        if( lockid == NULL || StrOp.len( lockid ) == 0 )
          lockid = wAction.getid( data->action );
        if( RouteOp.lock(st, lockid, False, True) )
          RouteOp.go( st );
      }
      else if( StrOp.equals( wAction.route_lock, wAction.getcmd( data->action ) ) ) {
        const char* lockid = wAction.getparam(data->action);
        if( lockid == NULL || StrOp.len( lockid ) == 0 )
          lockid = wAction.getid( data->action );
        RouteOp.lock(st, lockid, False, True);
      }
      else if( StrOp.equals( wAction.route_unlock, wAction.getcmd( data->action ) ) ) {
        const char* lockid = wAction.getparam(data->action);
        if( lockid == NULL || StrOp.len( lockid ) == 0 )
          lockid = wAction.getid( data->action );
        RouteOp.unLock(st, lockid, NULL, True, False);
      }
      else {
        RouteOp.go( st );
      }
    }
  }

  /* turntable action */
  else if( StrOp.equals( wTurntable.name(), wAction.gettype( data->action ) ) ) {
    iOTT tt = ModelOp.getTurntable( model, wAction.getoid( data->action ) );
    if( tt != NULL ) {
      iONode cmd = NodeOp.inst( wTurntable.name(), NULL, ELEMENT_NODE );
      if( StrOp.equals( wAction.tt_goto, wAction.getcmd( data->action ) ) )
        wTurntable.setcmd( cmd, wAction.getparam(data->action) );
      else
        wTurntable.setcmd( cmd, wAction.getcmd( data->action ) );
      TTOp.cmd( (iIBlockBase)tt, cmd );
    }
  }

  /* fiddle yard action */
  else if( StrOp.equals( wSelTab.name(), wAction.gettype( data->action ) ) ) {
    iOSelTab fy = ModelOp.getSelectiontable( model, wAction.getoid( data->action ) );
    if( fy != NULL ) {
      iONode cmd = NodeOp.inst( wSelTab.name(), NULL, ELEMENT_NODE );
      wSelTab.setcmd( cmd, wAction.getparam(data->action) );
      SelTabOp.cmd( (iIBlockBase)fy, cmd );
    }
  }

  /* system action */
  else if( StrOp.equals( wSysCmd.name(), wAction.gettype( data->action ) ) ) {
    int error = 0;
    if( StrOp.equals( wSysCmd.stoplocs, wAction.getcmd( data->action ) ) ) {
      /* re-map syscmd stoplocs to autocmd stop: */
      clntcon_callback pfun = ControlOp.getCallback(AppOp.getControl());
      iONode cmd = NodeOp.inst( wAutoCmd.name(), NULL, ELEMENT_NODE );
      wAutoCmd.setcmd( cmd, wAutoCmd.stop );
      pfun( (obj)AppOp.getControl(), cmd );
    }
    else {
      iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
      wSysCmd.setcmd( cmd, wAction.getcmd( data->action ) );
      ControlOp.cmd( AppOp.getControl(), cmd, &error );
    }
  }

  /* check for a locomotive action */
  else if( StrOp.equals( wLoc.name(), wAction.gettype( data->action ) ) ) {
    iOLoc lc = ModelOp.getLoc( model, wAction.getoid( data->action ));
    if( lc == NULL && wActionCtrl.getlcid(actionctrl) != NULL) {
      lc = ModelOp.getLoc( model, wActionCtrl.getlcid(actionctrl) );
    }
    if( lc != NULL ) {
      if( StrOp.equals(wLoc.go, wAction.getcmd(data->action) ) ) {
        if( wAction.getparam(data->action) != NULL && StrOp.len( wAction.getparam(data->action) ) > 0 )
          LocOp.useSchedule( lc, wAction.getparam(data->action));
        LocOp.go(lc);
      }
      else if( StrOp.equals(wAction.loco_carcount, wAction.getcmd(data->action) ) ) {
        LocOp.setCarCount(lc, atoi(wAction.getparam(data->action)));
      }
      else if( StrOp.equals(wLoc.stop, wAction.getcmd(data->action) ) ) {
        LocOp.stop(lc, False);
      }
      else if( StrOp.equals(wLoc.velocity, wAction.getcmd(data->action) ) ) {
        int v = atoi(wAction.getparam(data->action));
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE);
        if( v<0 ) {
            wLoc.setdir( cmd, !LocOp.getDir( lc ) );
            v = abs( v );
        }
        wLoc.setV( cmd, v );
        LocOp.cmd(lc, cmd);
      }
      else if( StrOp.equals(wAction.loco_percent, wAction.getcmd(data->action) ) ) {
        int v = atoi(wAction.getparam(data->action));
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE);
        if( v<0 ) {
            wLoc.setdir( cmd, !LocOp.getDir( lc ) );
            v = abs( v );
        }
        v = v * LocOp.getV( lc ) /100;
        wLoc.setV( cmd, v );
        LocOp.cmd(lc, cmd);
      }
      else if( StrOp.equals(wLoc.min, wAction.getcmd(data->action) )    || StrOp.equals(wLoc.mid, wAction.getcmd(data->action) ) ||
               StrOp.equals(wLoc.cruise, wAction.getcmd(data->action) ) || StrOp.equals(wLoc.max, wAction.getcmd(data->action) )) {
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE);
        wLoc.setV_hint( cmd, wAction.getcmd(data->action) );
        wLoc.setdir( cmd, LocOp.getDir(lc) );
        LocOp.cmd(lc, cmd);
      }
      else if( StrOp.equals(wLoc.dispatch, wAction.getcmd(data->action) ) ) {
        LocOp.dispatch(lc);
      }
      else if( StrOp.equals(wLoc.swap, wAction.getcmd(data->action) ) ) {
        LocOp.swapPlacing(lc, NULL, False);
      }
    }
  }

  /* check for a function command */
  else if( StrOp.equals( wFunCmd.name(), wAction.gettype( data->action ) ) ) {
    iOLoc lc = ModelOp.getLoc( model, wAction.getoid( data->action ));
    if( lc == NULL && wActionCtrl.getlcid(actionctrl) != NULL) {
      lc = ModelOp.getLoc( model, wActionCtrl.getlcid(actionctrl) );
    }
    if( lc != NULL ) {
      iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
      int fnaction = atoi(wAction.getparam(data->action));
      Boolean fon = StrOp.equals( "on", wAction.getcmd( data->action ) );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "function [%d] activated", fnaction );
      wFunCmd.setid( cmd, wAction.getid( data->action ) );
      LocOp.getFunctionStatus(lc, cmd);

      wFunCmd.setf0 ( cmd, fnaction== 0?fon:wFunCmd.isf0 ( cmd ) );
      wFunCmd.setf1 ( cmd, fnaction== 1?fon:wFunCmd.isf1 ( cmd ) );
      wFunCmd.setf2 ( cmd, fnaction== 2?fon:wFunCmd.isf2 ( cmd ) );
      wFunCmd.setf3 ( cmd, fnaction== 3?fon:wFunCmd.isf3 ( cmd ) );
      wFunCmd.setf4 ( cmd, fnaction== 4?fon:wFunCmd.isf4 ( cmd ) );
      wFunCmd.setf5 ( cmd, fnaction== 5?fon:wFunCmd.isf5 ( cmd ) );
      wFunCmd.setf6 ( cmd, fnaction== 6?fon:wFunCmd.isf6 ( cmd ) );
      wFunCmd.setf7 ( cmd, fnaction== 7?fon:wFunCmd.isf7 ( cmd ) );
      wFunCmd.setf8 ( cmd, fnaction== 8?fon:wFunCmd.isf8 ( cmd ) );
      wFunCmd.setf9 ( cmd, fnaction== 9?fon:wFunCmd.isf9 ( cmd ) );
      wFunCmd.setf10( cmd, fnaction==10?fon:wFunCmd.isf10( cmd ) );
      wFunCmd.setf11( cmd, fnaction==11?fon:wFunCmd.isf11( cmd ) );
      wFunCmd.setf12( cmd, fnaction==12?fon:wFunCmd.isf12( cmd ) );
      wFunCmd.setf13( cmd, fnaction==13?fon:wFunCmd.isf13( cmd ) );
      wFunCmd.setf14( cmd, fnaction==14?fon:wFunCmd.isf14( cmd ) );
      wFunCmd.setf15( cmd, fnaction==15?fon:wFunCmd.isf15( cmd ) );
      wFunCmd.setf16( cmd, fnaction==16?fon:wFunCmd.isf16( cmd ) );
      wFunCmd.setf17( cmd, fnaction==17?fon:wFunCmd.isf17( cmd ) );
      wFunCmd.setf18( cmd, fnaction==18?fon:wFunCmd.isf18( cmd ) );
      wFunCmd.setf19( cmd, fnaction==19?fon:wFunCmd.isf19( cmd ) );
      wFunCmd.setf20( cmd, fnaction==20?fon:wFunCmd.isf20( cmd ) );
      wFunCmd.setf21( cmd, fnaction==21?fon:wFunCmd.isf21( cmd ) );
      wFunCmd.setf22( cmd, fnaction==22?fon:wFunCmd.isf22( cmd ) );
      wFunCmd.setf23( cmd, fnaction==23?fon:wFunCmd.isf23( cmd ) );
      wFunCmd.setf24( cmd, fnaction==24?fon:wFunCmd.isf24( cmd ) );
      wFunCmd.setf25( cmd, fnaction==25?fon:wFunCmd.isf25( cmd ) );
      wFunCmd.setf26( cmd, fnaction==26?fon:wFunCmd.isf26( cmd ) );
      wFunCmd.setf27( cmd, fnaction==27?fon:wFunCmd.isf27( cmd ) );
      wFunCmd.setf28( cmd, fnaction==28?fon:wFunCmd.isf28( cmd ) );
      wFunCmd.setfnchanged( cmd, fnaction );
      wFunCmd.settimedfn( cmd, fon?fnaction:-1 );
      wFunCmd.setgroup( cmd, fnaction/4 + ((fnaction%4 > 0) ? 1:0) );
      wLoc.setfn( cmd, wFunCmd.isf0 ( cmd) );

      if( fon )
        wFunCmd.settimer( cmd, wAction.getactiontime(data->action) );
      LocOp.cmd( lc, cmd);
    }
  }

}


static void __timerThread( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOAction action = (iOAction)ThreadOp.getParm( th );
  iOActionData data = Data(action);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action timer started %s [%dms] ",
      wAction.getid(data->action), wAction.gettimer(data->action) );

  ThreadOp.sleep(wAction.gettimer(data->action));
  __executeAction( action, data->actionctrl );
  data->timerthread = NULL;
  ThreadOp.base.del(th);
}


static Boolean _cmd( iOAction inst, iONode cmd ) {
  iOActionData data = Data(inst);
  Boolean ok = False;
  const char* cmdStr = wAction.getcmd( cmd );

  if( StrOp.equals( wAction.test, cmdStr ) ) {
    iONode actionctrl = NodeOp.inst(wActionCtrl.name(), NULL, ELEMENT_NODE);
    __executeAction( inst, actionctrl );
    NodeOp.base.del(actionctrl);
 }

  /* Cleanup Node1 */
  NodeOp.base.del(cmd);
  return ok;
}


static void _exec( struct OAction* inst, iONode actionctrl ) {
  iOActionData data = Data(inst);
  iOModel model = AppOp.getModel();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Action %s [%s-%s:%s] ",
      wAction.getid(data->action),
      wAction.gettype(data->action),
      wAction.getoid(data->action),
      wAction.getcmd(data->action) );

  if( !__checkConditions(inst, actionctrl) ) {
    return;
  }

  if( wAction.gettimer(data->action) == 0 ) {
    __executeAction( inst, actionctrl );
  }
  else {
    if( data->timerthread == NULL ) {
      /* start the timer thread */
      data->actionctrl = actionctrl;
      data->timerthread = ThreadOp.inst( NULL, &__timerThread, inst );
      ThreadOp.start( data->timerthread );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "action timer for [%s] is already active.", wAction.getid(data->action) );
    }
  }
}

static void _tick( iOAction inst ) {
  iOActionData data = Data(inst);
  iOControl control = AppOp.getControl();
  long l_time = ControlOp.getTime(control);
  struct tm* lTime = localtime( &l_time );

  data->ticker++; /* scale minutes */

  if( wAction.istimed(data->action) ) {

    if( wAction.israndom(data->action) ) {
      if( data->randommins == data->ticker ) {
        _exec( inst, NULL );

        int mins = wAction.gethour(data->action) * 60 + wAction.getmin(data->action);
        if( mins < 1 ) mins = 1;
        data->randommins = rand() % mins;
        if( data->randommins < 1 )
          data->randommins = 1;
        data->ticker = 0;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action timer [%s] random scale minutes=%d.",
            wAction.getid(data->action), data->randommins );
      }
    }

    else if( lTime->tm_hour == wAction.gethour(data->action) && lTime->tm_min == wAction.getmin(data->action) ) {
      _exec( inst, NULL );
    }
  }

}



static void _modify( iOAction inst, iONode props ) {
  iOActionData data = Data(inst);
  NodeOp.mergeNode( data->action, props, True, True, True );
}


/**  */
static struct OAction* _inst( iONode ini ) {
  iOAction __Action = allocMem( sizeof( struct OAction ) );
  iOActionData data = allocMem( sizeof( struct OActionData ) );
  MemOp.basecpy( __Action, &ActionOp, 0, sizeof( struct OAction ), data );

  /* Initialize data->xxx members... */
  data->action = ini;

  if( wAction.israndom(data->action) ) {
    int mins = wAction.gethour(data->action) * 60 + wAction.getmin(data->action);
    if( mins < 1 ) mins = 1;
    data->randommins = rand() % mins;
    if( data->randommins < 1 )
      data->randommins = 1;
    data->ticker = 0;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action timer [%s] random scale minutes=%d.",
        wAction.getid(data->action), data->randommins );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action timer [%s] %02d:%02d.",
        wAction.getid(data->action), wAction.gethour(data->action), wAction.getmin(data->action) );
  }

  instCnt++;
  return __Action;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/action.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
