/*
 Rocrail - Model Railroad Control System

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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

/** ------------------------------------------------------------
  * Module: RocRail
  * Object: SelTab
  * Date: Fri Apr 18 13:59:49 2008
  * ------------------------------------------------------------
  */


#include "rocrail/impl/seltab_impl.h"

#include "rocrail/public/app.h"
#include "rocrail/public/model.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"


#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/SelTabPos.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/SysCmd.h"

static void _fbEvent( obj inst ,Boolean puls ,const char* id ,int ident, int val );
static void _sysEvent( obj inst, const char* cmd );

static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  iOSelTabData data = Data(inst);
  return wSelTab.getid(data->props);
}

static void* __event( void* inst, const void* evt ) {
  iOSelTabData data = Data(inst);
  iONode node = (iONode)evt;
  if( node != NULL && StrOp.equals( wFeedback.name(), NodeOp.getName(node) ) ) {
    _fbEvent( inst ,wFeedback.isstate(node), wFeedback.getid(node), wFeedback.getidentifier(node), wFeedback.getval(node) );
  }
  else if( node != NULL && StrOp.equals( wSysCmd.name(), NodeOp.getName(node) ) ) {
    _sysEvent( inst ,wSysCmd.getcmd(node) );
  }
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOSelTabData data = Data(inst);
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
  iOSelTabData data = Data(inst);
  return NULL;
}

static void __deserialize( void* inst,unsigned char* bytestream ) {
  iOSelTabData data = Data(inst);
  return;
}

static char* __toString( void* inst ) {
  iOSelTabData data = Data(inst);
  return NULL;
}

static int __count( void ) {
  return instCnt;
}

static struct OBase* __clone( void* inst ) {
  iOSelTabData data = Data(inst);
  return NULL;
}

static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

static void* __properties( void* inst ) {
  iOSelTabData data = Data(inst);
  return data->props;
}

/** ----- OSelTab ----- */


static void _depart(iIBlockBase inst) {
}

static iIBlockBase __getActiveTrackBlock(iIBlockBase inst, const char* msg ) {
  iOSelTabData data = Data(inst);
  iOModel model = AppOp.getModel();

  iONode pos = wSelTab.getseltabpos( data->props );
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "get active block for track %d (%s)...", data->tablepos, msg );
  while( pos != NULL ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block nr %d...", wSelTabPos.getnr(pos) );
    if( data->tablepos == wSelTabPos.getnr(pos) ) {
      iIBlockBase block = ModelOp.getBlock( model, wSelTabPos.getbkid(pos) );
      return block;
    }
    pos = wSelTab.nextseltabpos( data->props, pos );
  };
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no active block found for track %d.", data->tablepos );
  return NULL;
}




static void __initTrackBlocks(iIBlockBase inst) {
  iOSelTabData data = Data(inst);
  iOModel model = AppOp.getModel();

  iONode pos = wSelTab.getseltabpos( data->props );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init fy blocks" );
  while( pos != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "block nr %d...", wSelTabPos.getnr(pos) );
    iIBlockBase block = ModelOp.getBlock( model, wSelTabPos.getbkid(pos) );
    if( block != NULL )
      BlockOp.setManager(block, inst);
    pos = wSelTab.nextseltabpos( data->props, pos );
  };
}




static int __getTrack4Loc(iIBlockBase inst, const char* locid) {
  iOSelTabData data = Data(inst);
  iOModel model = AppOp.getModel();

  iONode pos = wSelTab.getseltabpos( data->props );
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "get block for locid %s...", locid );
  while( pos != NULL ) {
    iIBlockBase block = ModelOp.getBlock( model, wSelTabPos.getbkid(pos) );
    if( StrOp.equals( locid, block->getLoc(block) ) ) {
      return wSelTabPos.getnr(pos);
    }
    pos = wSelTab.nextseltabpos( data->props, pos );
  };
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no active block found for track %d.", data->tablepos );
  return -1;
}




static iIBlockBase __getBlock4Loc(iIBlockBase inst, const char* locid, Boolean* inBlock) {
  iOSelTabData data = Data(inst);
  iOModel model = AppOp.getModel();

  iONode pos = wSelTab.getseltabpos( data->props );
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "get block for locid %s...", locid );
  while( pos != NULL ) {
    iIBlockBase block = ModelOp.getBlock( model, wSelTabPos.getbkid(pos) );
    if( block != NULL && StrOp.equals( locid, block->getLoc(block) ) ) {
      if( inBlock != NULL && block->getInLoc(block) != NULL) {
        *inBlock = StrOp.equals( locid, block->getInLoc(block) );
      }
      return block;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block=0x%08X does not have set locid to %s...", locid );
    }
    pos = wSelTab.nextseltabpos( data->props, pos );
  };
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "no block found for [%s].", locid );
  return NULL;
}




static void __initSensors( iOSelTab inst ) {
  iOSelTabData data = Data(inst);
  iOModel model = AppOp.getModel();
  iOFBack s = ModelOp.getFBack( model, wSelTab.getpsen(data->props) );
  iOFBack b0 = ModelOp.getFBack( model, wSelTab.getb0sen(data->props) );
  iOFBack b1 = ModelOp.getFBack( model, wSelTab.getb1sen(data->props) );
  iOFBack b2 = ModelOp.getFBack( model, wSelTab.getb2sen(data->props) );
  iOFBack b3 = ModelOp.getFBack( model, wSelTab.getb3sen(data->props) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Init sensor for the Selectiontable..." );

  if( s != NULL ) {
    FBackOp.addListener( s, (obj)inst );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Position sensor for the Selectiontable could not be initialized." );
  }

  if( b0 != NULL && b1 != NULL && b2 != NULL && b3 != NULL ) {
    FBackOp.addListener( b0, (obj)inst );
    FBackOp.addListener( b1, (obj)inst );
    FBackOp.addListener( b2, (obj)inst );
    FBackOp.addListener( b3, (obj)inst );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Position sensors for the Selectiontable could not be initialized." );
  }
}


static void __removeSensors( iOSelTab inst ) {
  iOSelTabData data = Data(inst);
  iOModel model = AppOp.getModel();
  iOFBack s = ModelOp.getFBack( model, wSelTab.getpsen(data->props) );
  iOFBack b0 = ModelOp.getFBack( model, wSelTab.getb0sen(data->props) );
  iOFBack b1 = ModelOp.getFBack( model, wSelTab.getb1sen(data->props) );
  iOFBack b2 = ModelOp.getFBack( model, wSelTab.getb2sen(data->props) );
  iOFBack b3 = ModelOp.getFBack( model, wSelTab.getb3sen(data->props) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Remove sensor for the Selectiontable..." );

  if( s != NULL ) {
    FBackOp.removeListener( s, (obj)inst );
  }

  if( b0 != NULL && b1 != NULL && b2 != NULL && b3 != NULL ) {
    FBackOp.removeListener( b0, (obj)inst );
    FBackOp.removeListener( b1, (obj)inst );
    FBackOp.removeListener( b2, (obj)inst );
    FBackOp.removeListener( b3, (obj)inst );
  }
}


static void __evaluatePosition( obj inst ) {
  iOSelTabData data = Data(inst);
  iOModel model = AppOp.getModel();

  iOFBack b0 = ModelOp.getFBack( model, wSelTab.getb0sen(data->props) );
  iOFBack b1 = ModelOp.getFBack( model, wSelTab.getb1sen(data->props) );
  iOFBack b2 = ModelOp.getFBack( model, wSelTab.getb2sen(data->props) );
  iOFBack b3 = ModelOp.getFBack( model, wSelTab.getb3sen(data->props) );

  int tablepos = data->tablepos;

  if( b0 != NULL && b1 != NULL && b2 != NULL && b3 != NULL ) {
    data->reportedPos  = FBackOp.getState( b0 ) ? 1:0;
    data->reportedPos |= FBackOp.getState( b1 ) ? 2:0;
    data->reportedPos |= FBackOp.getState( b2 ) ? 4:0;
    data->reportedPos |= FBackOp.getState( b3 ) ? 8:0;

    tablepos = data->reportedPos;

  }
  else {
    data->reportedPos = -1;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "Reported selection table position is %d", tablepos );
  {
    iONode event = NodeOp.inst( wSelTab.name(), NULL, ELEMENT_NODE );
    wSelTab.setid( event, wSelTab.getid( data->props ) );
    wSelTab.setpos( event, tablepos );
    wSelTab.setlocid( event, data->lockedId!=NULL?data->lockedId:"" );
    wSelTab.setpending( event, False );
    if( wSelTab.getiid( data->props ) != NULL )
      wSelTab.setiid( event, wSelTab.getiid( data->props ) );
    AppOp.broadcastEvent( event );
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Selectiontable [%s] is locked by [%s].",
      SelTabOp.base.id( inst ), data->lockedId!=NULL?data->lockedId:"-" );


}


static void _sysEvent( obj inst, const char* cmd ) {
  iOSelTabData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sysEvent [%s]...", cmd );
  if( StrOp.equals( wSysCmd.shutdown, cmd ) ) {
    /* goto offpos */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Power Off sysEvent: offpos=%d", wSelTab.getoffpos(data->props) );
    if( wSelTab.getoffpos(data->props) > 0 && !SelTabOp.isLocked((iOSelTab)inst, NULL) ) {
      iONode node = NodeOp.inst( wSelTab.name(), NULL, ELEMENT_NODE );
      wSelTab.setid( node, wSelTab.getid( data->props ) );
      NodeOp.setInt( node, "cmd", wSelTab.getoffpos(data->props) );
      SelTabOp.cmd((iIBlockBase)inst, node);
    }
  }
}


/**  */
static void _fbEvent( obj inst ,Boolean state ,const char* id ,int ident, int val ) {
  iOSelTabData data = Data(inst);
  iOModel model = AppOp.getModel();
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fbid=%s state=%s ident=%d",
                 id, state?"true":"false", ident );
  /* process the event */
  /* when reaching the wanted position the GUI must be informed by setting the SelTabPos sensors to "true" */
  /* reset the pending flag */
  /* Broadcast to clients. */

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Selectiontable [%s] is locked by [%s].",
      SelTabOp.base.id( inst ), data->lockedId!=NULL?data->lockedId:"-" );

  if( StrOp.equals( wSelTab.getpsen(data->props), id) ) {
    iONode event = NodeOp.inst( wSelTab.name(), NULL, ELEMENT_NODE );
    wSelTab.setid( event, wSelTab.getid( data->props ) );
    wSelTab.setpos( event, data->tablepos );
    wSelTab.setlocid( event, data->lockedId!=NULL?data->lockedId:"" );
    wSelTab.setpending( event, !state );
    if( wSelTab.getiid( data->props ) != NULL )
      wSelTab.setiid( event, wSelTab.getiid( data->props ) );
    AppOp.broadcastEvent( event );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "position sensor is %s", state?"high":"low" );

    data->psenState = state;

    if( state && data->pending ) {
      iONode pos = wSelTab.getseltabpos( data->props );
      data->pending = False;
      data->tablepos = data->gotopos;
      wSelTab.setpos( data->props, data->gotopos );

      __evaluatePosition(inst);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "table position %d(%d) reached: **ready**",
          data->tablepos, data->reportedPos );

      {
        /* reset new position flag */
        iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
        const char* iid = wSelTab.getiid( data->props );

        if( iid != NULL )
          wOutput.setiid( cmd, iid );

        /* set the protocol */
        wSwitch.setprot( cmd, wSelTab.getprot(data->props) );
        wSwitch.setaddr1( cmd, wSelTab.getaddr4(data->props) );
        wSwitch.setport1( cmd, wSelTab.getport4(data->props) );
        wOutput.setcmd( cmd, wSelTab.isinvnew(data->props) ? wSwitch.straight:wSwitch.turnout );
        ControlOp.cmd( AppOp.getControl(), cmd, NULL );
      }

      /* inform all position feedbacks */
      while( pos != NULL ) {
        if( data->gotopos == wSelTabPos.getnr(pos) ) {
          iOFBack fb = ModelOp.getFBack( AppOp.getModel(), wSelTabPos.gets1id(pos) );
          if( fb != NULL )
            FBackOp.setState( fb, True );
          fb = ModelOp.getFBack( AppOp.getModel(), wSelTabPos.gets2id(pos) );
          if( fb != NULL )
            FBackOp.setState( fb, True );
        }
        else {
          iOFBack fb = ModelOp.getFBack( AppOp.getModel(), wSelTabPos.gets1id(pos) );
          if( fb != NULL )
            FBackOp.setState( fb, False );
          fb = ModelOp.getFBack( AppOp.getModel(), wSelTabPos.gets2id(pos) );
          if( fb != NULL )
            FBackOp.setState( fb, False );
        }
        pos = wSelTab.nextseltabpos( data->props, pos );
      }
    }
  }
  else if( StrOp.equals( wSelTab.getb0sen(data->props), id) ||
           StrOp.equals( wSelTab.getb1sen(data->props), id) ||
           StrOp.equals( wSelTab.getb2sen(data->props), id) ||
           StrOp.equals( wSelTab.getb3sen(data->props), id) )
  {
    __evaluatePosition(inst);

  }
  else if( wSelTab.issharedfb(data->props) ) {
    /* dispatch event to the active track block */
    iONode pos = wSelTab.getseltabpos( data->props );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "dispatching [%s]", id);

    while( pos != NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "checking table pos %d", wSelTabPos.getnr(pos));
      if( data->tablepos == wSelTabPos.getnr(pos) ) {
        iIBlockBase block = ModelOp.getBlock( model, wSelTabPos.getbkid(pos) );
        if( block != NULL ) {
          /* get the event description */
          char key[256] = {'\0'};
          const char* fromBlockId = block->getFromBlockId(block);
          StrOp.fmtb( key, "%s-%s", id, fromBlockId != NULL ? fromBlockId:"" );
          iONode fbevt = (iONode)MapOp.get( data->fbEvents, key );

          if( fbevt == NULL ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "seltab [%s] no event found for fromBlockId [%s], try to find one for all...",
                wSelTab.getid( data->props ), fromBlockId?fromBlockId:"?" );

            /* TODO: check running direction -> from_all or from_all_reverse */
            if( data->reverse ) {
              StrOp.fmtb( key, "%s-%s", id, wFeedbackEvent.from_all_reverse );
            }
            else {
              StrOp.fmtb( key, "%s-%s", id, wFeedbackEvent.from_all );
            }
            fbevt = (iONode)MapOp.get( data->fbEvents, key );
          }

          /* dispatch */
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "dispatching [%s]", key);
          block->event( block, state, id, ident, val, 0, fbevt );
        }
        break;
      }
      pos = wSelTab.nextseltabpos( data->props, pos );
    };
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Selectiontable [%s] is locked by [%s].",
      SelTabOp.base.id( inst ), data->lockedId!=NULL?data->lockedId:"-" );

  return;
}


static void __initFeedbackEvents( iOSelTab inst ) {
  iOSelTabData data = Data(inst);
  iOModel model = AppOp.getModel();
  char key[256] = {'\0'};
  iONode fbevt = wSelTab.getfbevent( data->props );

  MapOp.clear( data->fbEvents );

  while( fbevt != NULL ) {
    const char* fbid = wFeedbackEvent.getid( fbevt );
    iOFBack fb = ModelOp.getFBack( model, fbid );

    if( StrOp.len( fbid ) > 0 && fb != NULL ) {
      iOStrTok tok = StrTokOp.inst( wFeedbackEvent.getfrom( fbevt ), ',' );

      /* put all blockid's in the map */
      while( StrTokOp.hasMoreTokens(tok) ) {
        const char* fromblockid = StrTokOp.nextToken( tok );
        StrOp.fmtb( key, "%s-%s", fbid, fromblockid );
        MapOp.put( data->fbEvents, key, (obj)fbevt );
      };
      StrTokOp.base.del(tok);

      FBackOp.addListener( fb, (obj)inst );
    }
    fbevt = wSelTab.nextfbevent( data->props, fbevt );
  };

}


static void __processCmd( struct OSelTab* inst ,iONode nodeA ) {
  iOSelTabData  data = Data(inst);
  iOControl  control = AppOp.getControl();
  const char* cmdStr = wSelTab.getcmd( nodeA );
  int         cmdPos = wSelTab.getpos( nodeA );
  Boolean        dir = True;
  int        gotopos = -1;

  if( StrOp.equals( wSwitch.unlock, cmdStr ) ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "unlock seltab [%s]",
                 SelTabOp.base.id( inst ) );
    SelTabOp.unLock((iIBlockBase)inst, data->lockedId );
    return;
  }


  if( StrOp.equals( wSelTab.next, cmdStr ) ) {
    dir = False;
    gotopos = data->tablepos + 1;
  }
  else if( StrOp.equals( wSelTab.prev, cmdStr ) ) {
    dir = True;
    gotopos = data->tablepos - 1;
  }
  else {
    /* Tracknumber */
    int tracknr = atoi( cmdStr );
    if( tracknr == 0 && cmdPos > 0 )
      tracknr = cmdPos;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "Goto track %d, current pos=%d", tracknr, data->tablepos );
    gotopos = tracknr;
  }

  __evaluatePosition((obj)inst);

  if( data->reportedPos == gotopos && data->psenState ) {
    data->pending = False;
    data->gotopos = gotopos;
    data->tablepos = data->gotopos;
    wSelTab.setpos( data->props, data->gotopos );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "table at track [%d]: **ready**", gotopos );
  }
  else if( gotopos >= 0 && gotopos <= wSelTab.getnrtracks(data->props) ) {
    iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );

    const char* iid = wSelTab.getiid( data->props );

    data->gotopos = gotopos;
    data->pending = True;

    if( iid != NULL )
      wOutput.setiid( cmd, iid );

    if( StrOp.equals( wSelTab.prot_MP, wSelTab.getprot(data->props) ) ) {
      /* rename node to program */
      NodeOp.setName( cmd, wProgram.name() );
      /* set type to multiport */
      wProgram.setlntype( cmd, wProgram.lntype_mp );
      wProgram.setcmd( cmd, wProgram.lncvset );
      wProgram.setaddr( cmd, wSelTab.getaddr0(data->props) );
      wProgram.setmodid( cmd, wSelTab.getaddr1(data->props) );
      wProgram.setcv( cmd, 0x000F ); /* mask */
      wProgram.setvalue( cmd, gotopos ); /* value */
      ControlOp.cmd( control, cmd, NULL );

    }
    else {
      iONode lcmd = NULL;
      Boolean inv    = wSelTab.isinv(data->props);
      Boolean invnew = wSelTab.isinvnew(data->props);

      NodeOp.setName( cmd, wSwitch.name() );

      /* set the protocol */
      wSwitch.setprot( cmd, wSelTab.getprot(data->props) );

      /* signal new position will be set: */
      wSwitch.setaddr1( cmd, wSelTab.getaddr4(data->props) );
      wSwitch.setport1( cmd, wSelTab.getport4(data->props) );
      wOutput.setcmd( cmd, invnew ? wSwitch.straight:wSwitch.turnout );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      wSwitch.setaddr1( cmd, wSelTab.getaddr0(data->props) );
      wSwitch.setport1( cmd, wSelTab.getport0(data->props) );
      if( inv )
        wSwitch.setcmd( cmd, gotopos & 0x01 ? wSwitch.straight:wSwitch.turnout );
      else
        wSwitch.setcmd( cmd, gotopos & 0x01 ? wSwitch.turnout:wSwitch.straight );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      wSwitch.setaddr1( cmd, wSelTab.getaddr1(data->props) );
      wSwitch.setport1( cmd, wSelTab.getport1(data->props) );
      if( inv )
        wSwitch.setcmd( cmd, gotopos & 0x02 ? wSwitch.straight:wSwitch.turnout );
      else
        wSwitch.setcmd( cmd, gotopos & 0x02 ? wSwitch.turnout:wSwitch.straight );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      wSwitch.setaddr1( cmd, wSelTab.getaddr2(data->props) );
      wSwitch.setport1( cmd, wSelTab.getport2(data->props) );
      if( inv )
        wSwitch.setcmd( cmd, gotopos & 0x04 ? wSwitch.straight:wSwitch.turnout );
      else
        wSwitch.setcmd( cmd, gotopos & 0x04 ? wSwitch.turnout:wSwitch.straight );
      lcmd = (iONode)NodeOp.base.clone(cmd);    data->gotopos = gotopos;

      ControlOp.cmd( control, lcmd, NULL );

      wSwitch.setaddr1( cmd, wSelTab.getaddr3(data->props) );
      wSwitch.setport1( cmd, wSelTab.getport3(data->props) );
      if( inv )
        wSwitch.setcmd( cmd, gotopos & 0x08 ? wSwitch.straight:wSwitch.turnout );
      else
        wSwitch.setcmd( cmd, gotopos & 0x08 ? wSwitch.turnout:wSwitch.straight );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      /* signal new position is set: */
      ThreadOp.sleep(10);
      wSwitch.setaddr1( cmd, wSelTab.getaddr4(data->props) );
      wSwitch.setport1( cmd, wSelTab.getport4(data->props) );
      wOutput.setcmd( cmd, invnew ? wSwitch.turnout:wSwitch.straight );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

    }

  }

}


/**  */
static Boolean _cmd( iIBlockBase inst ,iONode cmd ) {
  iOSelTabData    data = Data(inst);
  iOControl    control = AppOp.getControl();
  const char*    state = wSelTab.getstate( cmd );
  const char*  command = wSelTab.getcmd(cmd);
  iONode broadcastNode = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cmd=[%s] state=[%s]", wSelTab.getcmd(cmd), state );

  if( command == NULL && state != NULL ) {
    if( StrOp.equals( wBlock.closed, state ) ) {
      if( data->lockedId != NULL && StrOp.len( data->lockedId ) > 0 ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "close request; already reserved by [%s]", data->lockedId );
        NodeOp.base.del(cmd);
        data->closereq = True;
        return False;
      }
    }

    if( data->closereq ) {
      state = wBlock.closed;
      wBlock.setstate( cmd, state );
      data->closereq = False;
    }
    broadcastNode = cmd;
    wSelTab.setstate( data->props, state );
    ModelOp.setBlockOccupancy( AppOp.getModel(), wSelTab.getid(data->props), data->lockedId, StrOp.equals( wBlock.closed, state ), 0, 0 );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s state=%s", NodeOp.getStr( data->props, "id", "" ), state );
  }
  else {
    /* process the command */
    /* set the pending flag in case of a move */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "processing command...");
    if( wSelTab.ismanager(data->props) ) {
      /* find the block belonging to the locking locomotive */
      if( data->lockedId != NULL && StrOp.len(data->lockedId) > 0 ) {
        int trackpos = __getTrack4Loc(inst, data->lockedId);
        if( trackpos > 0 )
          wSelTab.setpos(cmd, trackpos);
      }

    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s pos[%d]", wSelTab.getcmd(cmd), wSelTab.getpos(cmd) );
    __processCmd( (iOSelTab)inst, cmd );

  }


  if( broadcastNode != NULL ) {
    /* Broadcast to clients. */
    AppOp.broadcastEvent( broadcastNode );
  }

  return True;
}


static Boolean __hasLockedRouteID(iOList list, const char* routeid) {
  int cnt = ListOp.size(list);
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    const char* id = (const char*)ListOp.get( list, i);
    if( StrOp.equals(routeid, id))
      return True;
  }
  return False;
}


static Boolean __removeLockedRouteID(iOList list, const char* routeid) {
  int cnt = ListOp.size(list);
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    const char* id = (const char*)ListOp.get( list, i);
    if( StrOp.equals(routeid, id)) {
      ListOp.remove(list, i);
      return True;
    }
  }
  return False;
}


/**  */
static struct OSelTab* _inst( iONode ini ) {
  iOSelTab __SelTab = allocMem( sizeof( struct OSelTab ) );
  iOSelTabData data = allocMem( sizeof( struct OSelTabData ) );
  MemOp.basecpy( __SelTab, &SelTabOp, 0, sizeof( struct OSelTab ), data );

  /* Initialize data->xxx members... */
  data->props = ini;

  data->muxLock = MutexOp.inst( NULL, True );
  data->fbEvents = MapOp.inst();
  data->lockedId = wSelTab.getlocid(ini);
  data->lockedRouteList = ListOp.inst();

  NodeOp.removeAttrByName(data->props, "cmd");

  __initSensors( __SelTab );
  __initFeedbackEvents( __SelTab );
  data->tablepos = -1;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Selectiontable [%s] initialized.", wSelTab.getid(ini) );

  ModelOp.addSysEventListener( AppOp.getModel(), (obj)__SelTab );

  instCnt++;
  return __SelTab;
}


static int __getOccTrackBlocks(iIBlockBase inst, long* oldestOccTick) {
  iOSelTabData data = Data(inst);
  /* check for a free track block */
  iOModel model = AppOp.getModel();
  int occBlocks = 0;
  iONode pos = wSelTab.getseltabpos( data->props );

  *oldestOccTick = 0;

  while( pos != NULL ) {
    iIBlockBase block = ModelOp.getBlock( model, wSelTabPos.getbkid(pos) );
    if( block != NULL && !block->isFree(block, NULL) ) {
      long blockOccTime = block->getOccTime(block);
      if( blockOccTime > 0 && blockOccTime < *oldestOccTick || blockOccTime > 0 && *oldestOccTick == 0 )
        *oldestOccTick = block->getOccTime(block);
      occBlocks++;
    }
    pos = wSelTab.nextseltabpos( data->props, pos );
  };
  return occBlocks;
}




/**  */
static Boolean _isLocked( struct OSelTab* inst ,const char* locid ) {
  iOSelTabData data = Data(inst);

  if( locid != NULL ) {
    Boolean inBlock = False;
    iIBlockBase block = __getBlock4Loc((iIBlockBase)inst, locid, &inBlock);
    if( block != NULL ) {
      /* loc is on the FY */
      long oldestOccTick = 0;
      long locOccTick    = block->getOccTime(block);
      int  occBlocks     = __getOccTrackBlocks((iIBlockBase)inst, &oldestOccTick);
      int  minOcc        = wSelTab.getminocc( data->props );

      if( inBlock && minOcc > 0 && occBlocks < minOcc ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "FY[%s] contains only [%d] trains; minimum is [%d]...",
                       inst->base.id( inst ), occBlocks, minOcc );
        return True;
      }

      if( wSelTab.isfifo(data->props) ) {
        if( inBlock && locOccTick > oldestOccTick && oldestOccTick > 0 ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "FY[%s]: loco[%s] is not at turn. (loco[%d], oldest[%d])",
                         inst->base.id( inst ), locid, locOccTick, oldestOccTick );
          return True;
        }
      }

    }
  }

  if( wSelTab.getmovedelay(data->props) > 0 ) {
    long ticks = wSelTab.getmovedelay(data->props) / 10;
    long elapsed = SystemOp.getTick() - data->unlockTick;
    if( elapsed < ticks ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "FY[%s] movedelay is [%ld] ticks, elapsed[%ld]",
                     inst->base.id( inst ), ticks, elapsed );
      return True;
    }
  }

  if( data->lockedId != NULL && StrOp.len(data->lockedId) > 0 && !StrOp.equals( locid, data->lockedId ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Selectiontable [%s] is locked by [%s], request ID[%s].",
                   inst->base.id( inst ), data->lockedId, locid );
    return True;
  }
  /* TODO: add a timer starting at unlock to prevent moving the FY before the loco had stopped completely. */
  return False;
}


static Boolean _isManager( struct OSelTab* inst ) {
  iOSelTabData data = Data(inst);
  return wSelTab.ismanager(data->props);
}


/** true if the sensor state matches the wanted state */
static Boolean _isSet( struct OSelTab* inst ) {
  iOSelTabData data = Data(inst);
  return data->pending ? False:True;
}


static Boolean _isReady( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  return True;
}


static Boolean _hasExtStop( iIBlockBase inst ) {
  return False;
}


static void _setManager( iIBlockBase inst, iIBlockBase manager ) {
  iOSelTabData data = Data(inst);
}


static iIBlockBase _getManager( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  return NULL;
}



static iIBlockBase __getFreeTrackBlock(iIBlockBase inst, const char* locId, int* trackpos) {
  iOSelTabData data = Data(inst);
  /* check for a free track block */
  iOModel model = AppOp.getModel();

  iONode pos = wSelTab.getseltabpos( data->props );

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                 "FY[%s]: find free track block...", wSelTab.getid( data->props ) );

  while( pos != NULL ) {
    iIBlockBase block = ModelOp.getBlock( model, wSelTabPos.getbkid(pos) );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "FY[%s]: checking block [%s]...", wSelTab.getid( data->props ), wSelTabPos.getbkid(pos) );
    if( block != NULL && block->isFree(block, locId) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                     "FY[%s]: block [%s] is free", wSelTab.getid( data->props ), block->base.id(block) );
      *trackpos = wSelTabPos.getnr(pos);
      return block;
    }

    pos = wSelTab.nextseltabpos( data->props, pos );
  };
  return NULL;
}

static Boolean _isState( iIBlockBase inst, const char* state ) {
  return False;
}

static Boolean _isFree( iIBlockBase inst, const char* locId ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = NULL;
  int           pos = 0;
  const char* state = wSelTab.getstate( data->props );

  if( StrOp.equals( state, wBlock.closed ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                   "FY [%s] is closed!.", wSelTab.getid( data->props ) );

    return False;
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Selectiontable [%s] is locked by [%s].",
      inst->base.id( inst ), data->lockedId!=NULL?data->lockedId:"-" );

  /* check for a free track block */
  block = __getFreeTrackBlock(inst, locId, &pos);

  return block != NULL ? True:False;
}

static int _isSuited( iIBlockBase inst, iOLoc loc ) {
  iOSelTabData data = Data(inst);
  return suits_ok;
}

static int _getVisitCnt( iIBlockBase inst, const char* id ) {
  iOSelTabData data = Data(inst);
  return 0;
}

static int _getOccTime( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  return 0;
}

/**  */
static Boolean _lock( iIBlockBase inst, const char* id, const char* blockid, const char* routeid, Boolean crossing, Boolean reset, Boolean reverse, int indelay ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = NULL;
  int           pos = 0;
  Boolean        ok = True;
  Boolean   manager = False;

  /* wait only 10ms for getting the mutex: */
  if( !MutexOp.trywait( data->muxLock, 10 ) ) {
    return False;
  }

  if( !StrOp.startsWith(blockid, wRoute.routelock) && wSelTab.ismanager(data->props) ) {
    manager = True;
    block = __getFreeTrackBlock( inst, id, &pos );
    if( block == NULL || !block->lock(block, id, blockid, NULL, crossing, reset, reverse, 0 ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "could not lock block [%s] for [%s]", block->base.id(block), id);
      ok = False;
    }
    BlockOp.setManager( block, inst);
  }

  if( ok ) {

    if( data->lockedId != NULL && StrOp.equals(data->lockedId, id) ) {
      if( routeid != NULL && StrOp.len(routeid) > 0 && !__hasLockedRouteID(data->lockedRouteList, routeid) )
        ListOp.add( data->lockedRouteList, (obj)routeid );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Selectiontable [%s] is locked by [%s] with %d routes, new is [%s].",
          inst->base.id( inst ), data->lockedId, ListOp.size(data->lockedRouteList), routeid );
    }
    else if( data->lockedId == NULL || StrOp.len(data->lockedId) == 0 ) {
      data->lockedId = id;
      ListOp.clear( data->lockedRouteList );

      if( routeid != NULL && StrOp.len(routeid) > 0 && !__hasLockedRouteID(data->lockedRouteList, routeid) )
        ListOp.add( data->lockedRouteList, (obj)routeid );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Selectiontable [%s] is locked by [%s] with route [%s].",
          inst->base.id( inst ), data->lockedId, routeid );

      /* goto position */
      if( manager ) {
        /*
        data->reverse = reverse;
        iONode nodeCmd = NodeOp.inst( wSelTab.name(), NULL, ELEMENT_NODE );
        wSelTab.setid( nodeCmd, inst->base.id( inst ) );
        NodeOp.setInt(nodeCmd, "cmd", pos );
        __processCmd( (iOSelTab)inst ,nodeCmd );
        */
      }
      /* Broadcast to clients. Node6 */
      else {
        iONode nodeF = NodeOp.inst( wSelTab.name(), NULL, ELEMENT_NODE );
        wSelTab.setid( nodeF, inst->base.id( inst ) );
        wSelTab.setpos( nodeF, wSelTab.getpos( data->props) );
        if( data->lockedId != NULL )
          wSelTab.setlocid( nodeF, data->lockedId );
        if( wSelTab.getiid( data->props ) != NULL )
          wSelTab.setiid( nodeF, wSelTab.getiid( data->props ) );
        AppOp.broadcastEvent( nodeF );
      }
      ok = True;
    }
    else{
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Selectiontable [%s] is already locked by [%s].",
          inst->base.id( inst ), data->lockedId );
      ok = False;
      if( block != NULL ) {
        block->unLock(block, id);
      }
    }
  }

  /* Unlock the semaphore: */
  MutexOp.post( data->muxLock );

  return ok;
}


/**  */
static void _modify( struct OSelTab* inst ,iONode props ) {
  iOSelTabData data = Data(inst);
  Boolean move = StrOp.equals( wModelCmd.getcmd( props ), wModelCmd.move );

  int cnt = NodeOp.getAttrCnt( props );
  int i = 0;

  __removeSensors(inst);

  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );
    NodeOp.setStr( data->props, name, value );
  }

  if(!move) {
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
    /* re-init callback for all feedbacks: */
    __initFeedbackEvents( inst );
  }
  else {
    NodeOp.removeAttrByName(data->props, "cmd");
  }

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( data->props );
    AppOp.broadcastEvent( clone );
  }
  props->base.del(props);
  return;
}


/**  */
static void _reset( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
             "reset selectiontable [%s]", inst->base.id( inst ) );
  SelTabOp.unLock( inst, data->lockedId );
  ListOp.clear( data->lockedRouteList );

  {
    iONode cmd = NodeOp.inst( wSelTab.name(), NULL, ELEMENT_NODE );
    wSelTab.setcmd( cmd,  "1" );
    __processCmd( (iOSelTab)inst, cmd );
  }

  return;
}


static Boolean _setLocSchedule( iIBlockBase inst, const char* scid ) {
  Boolean ok = False;
  if( inst != NULL && scid != NULL ) {
    iOSelTabData data = Data(inst);
  }
  return ok;
}

/**  */
static Boolean _unLock( iIBlockBase inst ,const char* id ) {
  iOSelTabData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "unlock for [%s]", id!=NULL?id:"?" );

  if( id == NULL ) {
    return False;
  }

  if( !StrOp.startsWith(id, wRoute.routelock) && wSelTab.ismanager(data->props) ) {
    iIBlockBase block = __getBlock4Loc(inst, id, NULL);

      /* dispatch to active tracke block */
    if( block != NULL ) {
      int track = __getTrack4Loc(inst, id);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unlock FY block [%s] for [%s]", block->base.id(block), id );
      block->unLock( block, id );
    }
  }
  else if( StrOp.startsWith(id, wRoute.routelock) && wSelTab.ismanager(data->props) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unlock FY [%s] for [%s]", inst->base.id(inst), id );
    /* TODO: save without checking the route id? */
    if( StrOp.endsWith(id, data->lockedId) )
      id = data->lockedId;
  }

  if( data->lockedId != NULL && StrOp.equals( id, data->lockedId ) ) {
    if( ListOp.size( data->lockedRouteList ) > 0 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "removing one of %d lock for [%s]", ListOp.size( data->lockedRouteList ), id );
      ListOp.remove(data->lockedRouteList, 0);
    }
    if( ListOp.size( data->lockedRouteList ) > 0 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%d locks are still active for [%s]", ListOp.size( data->lockedRouteList ), id );
      return True;
    }
  }

  if( data->lockedId == NULL || StrOp.equals( id, data->lockedId ) ) {
    data->lockedId = NULL;
    wSelTab.setlocid( data->props, "" );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unlocked for [%s]", id );

    if( data->closereq ) {
      wSelTab.setstate( data->props, wBlock.closed );
      data->closereq = False;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set to close: requested" );
    }

    /* Broadcast to clients. Node6 */
    {
      iONode nodeF = NodeOp.inst( wSelTab.name(), NULL, ELEMENT_NODE );
      wSelTab.setid( nodeF, inst->base.id( inst ) );
      wSelTab.setpos( nodeF, wSelTab.getpos( data->props) );
      if( wSelTab.getiid( data->props ) != NULL )
        wSelTab.setiid( nodeF, wSelTab.getiid( data->props ) );
      AppOp.broadcastEvent( nodeF );
    }

    data->unlockTick = SystemOp.getTick();

    return True;
  }
  else if( data->lockedId != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lockedID is [%s], not [%s]", data->lockedId, id );
  }

  return False;
}


static const char* _getState( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  return "";
}



static const char* _getLoc( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "getLoc");
  /* dispatch to active tracke block */
  return block != NULL ? block->getLoc( block ) : "";
}

static const char* _getInLoc( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "getLoc");
  /* dispatch to active tracke block */
  return block != NULL ? block->getInLoc( block ) : "";
}

static void _event( iIBlockBase inst, Boolean puls, const char* id, long ident, int val, int wheelcount, iONode fbevt ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "event");
  /* dispatch to active tracke block */
  if( block != NULL && !data->pending )
    block->event( block, puls, id, ident, val, 0, fbevt );
}


static const char* _getFromBlockId( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "getFromBlockId");
  /* dispatch to active tracke block */
  return block != NULL ? block->getFromBlockId( block ) : "";
}

static const char* _getTDiid( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "getTDiid");
  /* dispatch to active tracke block */
  return block != NULL ? block->getTDiid( block ) : "";
}


static int _getTDaddress( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "getTDaddress");
  /* dispatch to active tracke block */
  return block != NULL ? block->getTDaddress( block ) : 0;
}


static int _getTDport( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "getTDport");
  /* dispatch to active tracke block */
  return block != NULL ? block->getTDport( block ) : 0;
}


static void _setAnalog( iIBlockBase inst, Boolean analog ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "setAnalog");
  /* dispatch to active tracke block */
  if( block != NULL )
    block->setAnalog( block, analog );
}


static Boolean _isLinked( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "isLinked");
  /* dispatch to active tracke block */
  return block != NULL ? block->isLinked( block ) : False;
}

static void _setGroup( iIBlockBase inst, const char* group ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "setGroup");
  /* dispatch to active tracke block */
  if( block != NULL )
    block->setGroup( block, group );
}

static void _enterBlock( iIBlockBase inst, const char* id ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "enterBlock");
  /* dispatch to active tracke block */
  if( block != NULL )
    block->enterBlock( block, id );
}

static const char* _getVelocity( iIBlockBase inst, int* percent, Boolean onexit, Boolean reverse, Boolean onstop ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "getVelocity");
  *percent = 0;
  /* dispatch to active tracke block */
  return block != NULL ? block->getVelocity( block, percent, onexit, reverse, onstop ) : "";
}

static int _getWait( iIBlockBase inst, iOLoc loc, Boolean reverse ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "getWait");
  /* dispatch to active tracke block */
  return block != NULL ? block->getWait( block, loc, reverse ) : 0;
}

static Boolean _green( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "green");
  /* dispatch to active tracke block */
  if( block != NULL )
    return block->green( block, distant, reverse );

  return False;
}

static Boolean _red( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "red");
  /* dispatch to active tracke block */
  if( block != NULL )
    return block->red( block, distant, reverse );

  return False;
}

static Boolean _yellow( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "yellow");
  /* dispatch to active tracke block */
  if( block != NULL )
    return block->yellow( block, distant, reverse );

  return False;
}

static Boolean _white( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "white");
  /* dispatch to active tracke block */
  if( block != NULL )
    return block->white( block, distant, reverse );

  return False;
}

static void _setDefaultAspect(iIBlockBase inst, Boolean signalpair) {
}

static obj _hasManualSignal( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "red");
  /* dispatch to active tracke block */
  if( block != NULL )
    return block->hasManualSignal( block, distant, reverse );

  return NULL;
}

static Boolean _hasEnter2Route( iIBlockBase inst, const char* fromBlockID ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "hasEnter2Route");
  /* dispatch to active tracke block */
  return block != NULL ? block->hasEnter2Route( block, fromBlockID ) : False;
}

static Boolean _hasPre2In( iIBlockBase inst, const char* fromBlockID ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "hasPre2In");
  /* dispatch to active tracke block */
  return block != NULL ? block->hasPre2In( block, fromBlockID ) : False;
}

static void _inBlock( iIBlockBase inst, const char* id ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "inBlock");
  /* dispatch to active tracke block */
  if( block != NULL )
    block->inBlock( block, id );
}

static Boolean _isTerminalStation( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "isTerminalStation");
  /* dispatch to active tracke block */
  return block != NULL ? block->isTerminalStation( block ) : False;
}

static Boolean _link( iIBlockBase inst, iIBlockBase linkto ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "link");
  /* dispatch to active tracke block */
  return block != NULL ? block->link( block, linkto ) : False;
}

static int _getWheelCount( iIBlockBase inst ) {
  return 0;
}


static Boolean _unLink( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "unLink");
  /* dispatch to active tracke block */
  return block != NULL ? block->unLink( block ) : False;
}


static Boolean _lockForGroup( iIBlockBase inst, const char* id ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "lockForGroup");
  /* dispatch to active tracke block */
  return block != NULL ? block->lockForGroup( block, id ) : False;
}

static Boolean _unLockForGroup( iIBlockBase inst, const char* id ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "unLockForGroup");
  /* dispatch to active tracke block */
  return block != NULL ? block->unLockForGroup( block, id ) : False;
}


static void _resetTrigs( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "resetTrigs");
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reset trigs" );
  /* dispatch to active tracke block */
  if( block != NULL )
    block->resetTrigs( block );
}

static Boolean _wait( iIBlockBase inst, iOLoc loc, Boolean reverse ) {
  iOSelTabData data = Data(inst);
  iIBlockBase block = __getActiveTrackBlock(inst, "wait");
  /* dispatch to active tracke block */
  return block != NULL ? block->wait( block, loc, reverse ) : False;
}

static void _setCarCount( iIBlockBase inst, int count ) {
}

static void _acceptIdent( iIBlockBase inst, Boolean accept ) {
}

static Boolean _isDepartureAllowed( iIBlockBase inst, const char* id ) {
  return True;
}



static void _init( iIBlockBase inst ) {
  iOSelTabData data = Data(inst);
  /* init all track blocks */
  __initTrackBlocks( inst );
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/seltab.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
