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

#include "rocrail/impl/control_impl.h"
#include "rocrail/public/app.h"
#include "rocrail/public/model.h"
#include "rocrail/public/block.h"
#include "rocrail/public/throttle.h"

#include "rocint/public/digint.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/thread.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/map.h"
#include "rocs/public/lib.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/Global.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Link.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/AutoCmd.h"
#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/DataReq.h"
#include "rocrail/wrapper/public/ActionList.h"
#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/ThrottleCmd.h"

typedef iIDigInt (* LPFNROCGETDIGINT)( const iONode ,const iOTrace );
/* proto types */
static void __informDigInts( iOControl inst );
static void __listener( obj inst, iONode nodeC, int level );

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
  return "";
}
static void __del(void* inst) {
  iOControlData data = Data(inst);
  freeMem( data );
  freeMem( inst );
  instCnt--;
}
static void* __properties(void* inst) {
  if( inst != NULL ) {
    iOControlData data = Data(inst);
    return NULL;
  }
  else
    return NULL;
}
static int __count(void) {
  return instCnt;
}
static struct OBase* __clone( void* inst ) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}


static Boolean __informDigInt( iOControl inst, iIDigInt pDi, iONode node, int* error ) {
  iOControlData data  = Data(inst);
  Boolean rc          = True;

  /* inform digitalInterface */
  if( pDi != NULL ) {
    iONode rsp = pDi->cmd( (obj)pDi, node );
    if( rsp != NULL ) {

      if( StrOp.equals( NodeOp.getName( rsp ), wProgram.name() ) ) {
        /* Decoder programming response: */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Program event...value=%d", wProgram.getvalue( rsp ) );
        ClntConOp.broadcastEvent( AppOp.getClntCon(  ), rsp );
      }
      else if( StrOp.equals( NodeOp.getName( rsp ), wFeedback.name() ) ) {
        /* sensor simulation response: */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Sensor event...addr=%d", wFeedback.getaddr( rsp ) );
        __listener( (obj)inst, rsp, TRCLEVEL_INFO );
      }
      else if( wResponse.iserror( rsp ) ) {
        char* str = NodeOp.base.toString( rsp );
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "%s", str );
        StrOp.free( str );
        rc = False;
        if( error != NULL )
          *error = CMD_ERROR;
      }
      else if( wResponse.isretry( rsp ) ) {
        if( error != NULL )
          *error = CMD_RETRY;
        rc = False;
      }


      /*
      const char* threadName = NodeOp.getStr( node, "server", "?" );
      iOThread sthread = ThreadOp.find( threadName );
      if( sthread != NULL )
        ThreadOp.post( sthread, (obj)rsp );
      else
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "cmd() thread [%s] unknown.", threadName );
      */
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Interface ID is not set!" );
  }

  return rc;
}


static Boolean _cmd( iOControl inst, iONode node, int* error ) {
  iOControlData data  = Data(inst);
  Boolean rc          = True;

  if( error != NULL )
    *error = CMD_OK;

  {
    const char* iid = wCommand.getiid( node );
    iIDigInt pDi    = data->pDi;

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "NodeOp.getName=%s", NodeOp.getName( node ));

    /* check for program commands which must be send to other than the default */
    if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
      const char* ptiid = wRocRail.getptiid( AppOp.getIni() );
      const char* sviid = wRocRail.getsviid( AppOp.getIni() );
      if( ( wProgram.getlntype(node) == wProgram.lntype_sv || wProgram.getlntype(node) == wProgram.lntype_cv ) &&
          sviid != NULL && StrOp.len( sviid ) > 0 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                     "setting iid=[%s] for LN-SV programming", sviid );
        wCommand.setiid( node, sviid );
        iid = sviid;
      }
      else if( ptiid != NULL && StrOp.len( ptiid ) > 0 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                     "setting iid=[%s] for programming", ptiid );
        wCommand.setiid( node, ptiid );
        iid = ptiid;
      }
    }

    /* check for locomotive commands which must be send to other than the default */
    if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
      if( iid == NULL || StrOp.len(iid) == 0 ) {
        const char* lciid = wRocRail.getlciid( AppOp.getIni() );
        const char* dpiid = wRocRail.getdpiid( AppOp.getIni() );
        if( StrOp.equals( wLoc.getcmd(node), wLoc.dispatch ) && dpiid != NULL ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                       "setting iid=[%s] for dispatch locomotive %s", dpiid, wLoc.getid(node) );
          wLoc.setiid( node, dpiid );
          iid = dpiid;
        }
        else if( lciid != NULL && StrOp.len( lciid ) > 0 ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                       "setting iid=[%s] for locomotive %s", lciid, wLoc.getid(node) );
          wLoc.setiid( node, lciid );
          iid = lciid;
        }
      }
    }


    if( StrOp.equals( wSysCmd.name(), NodeOp.getName(node) ) ) {
      /* inform model */
      ModelOp.cmd( AppOp.getModel(), (iONode)NodeOp.base.clone(node));
    }

    if( StrOp.equals( wSysCmd.name(), NodeOp.getName(node) ) && wSysCmd.isinformall(node) ||
        StrOp.equals( wClock.name(), NodeOp.getName(node) ) )
    {
      /* inform all */
      pDi = (iIDigInt)MapOp.first( data->diMap );
      while( pDi != NULL ) {
        rc = __informDigInt(inst, pDi, (iONode)NodeOp.base.clone(node), error);
        pDi = (iIDigInt)MapOp.next( data->diMap );
      }
      /* clean up original command node */
      NodeOp.base.del(node);
    }
    else if( iid != NULL && StrOp.len( iid ) > 0 ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "address of diMap=%d", data->diMap );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "iid=%s", iid );
      /* inform a specific one */
      pDi = (iIDigInt)MapOp.get( data->diMap, iid );
      rc = __informDigInt(inst, pDi, node, error);
    }
    else {
      /* inform the default */
      rc = __informDigInt(inst, data->pDi, node, error);
    }

  }

  return rc;
}


static void  __grouplink( obj inst, iONode link ) {
  iOControlData data = Data(inst);
  iOModel model = AppOp.getModel();

  if( StrOp.equals( wLink.name(), NodeOp.getName( link ) ) ) {
    const char* src = wLink.getsrc( link );
    iIBlockBase blockA = ModelOp.getBlock( model, src );
    iIBlockBase blockB = NULL;
    iOStrTok tok = StrTokOp.inst( wLink.getdst( link ), ',' );

    while( StrTokOp.hasMoreTokens( tok ) )  {
      const char* id = StrTokOp.nextToken( tok );
      if( StrOp.len( id ) > 0 ) {
        blockB = ModelOp.getBlock( model, id );
        if( blockA != NULL && blockB != NULL ) {
          if( wLink.isactive( link ) )
            BlockOp.link( blockB, blockA );
          else
            BlockOp.unLink( blockB );
          blockA = blockB;
          blockB = NULL;
        }
      }
    }
    StrTokOp.base.del(tok);
  }
}


static void __callback( obj inst, iONode nodeA ) {
  iOControlData data    = Data(inst);
  iOModel model         = AppOp.getModel(  );
  const char* nodeName  = NodeOp.getName( nodeA );
  int error             = 0;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__callback..." );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "nodeName=%s", nodeName );

  if( StrOp.equals( wClock.name(), nodeName ) ) {
    if( StrOp.equals( wClock.getcmd(nodeA), wClock.freeze ) ) {
      if( data->devider > 1 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "freeze clock" );
        data->clockrun = False;
        /* broadcast to clients */
        {
          iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
          wClock.setcmd( tick, wClock.freeze );
          wClock.setdivider( tick, data->devider );
          wClock.settime( tick, data->time );
          ClntConOp.broadcastEvent( AppOp.getClntCon(), tick );
        }
        {
          iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
          wClock.setdivider( tick, data->devider );
          wClock.settime( tick, data->time );
          wClock.setcmd( tick, wClock.freeze );
          /* inform all digints */
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "inform all digints..." );
          ControlOp.cmd( (iOControl)inst, tick, NULL );
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "freeze/resume clock not possible with real time" );
      }
    }
    else if( StrOp.equals( wClock.getcmd(nodeA), wClock.go ) ) {
      if( data->devider > 1 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "resume clock" );
        data->clockrun = True;
        /* broadcast to clients */
        {
          iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
          wClock.setcmd( tick, wClock.go );
          wClock.setdivider( tick, data->devider );
          wClock.settime( tick, data->time );
          ClntConOp.broadcastEvent( AppOp.getClntCon(), tick );
        }
        {
          iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
          wClock.setdivider( tick, data->devider );
          wClock.settime( tick, data->time );
          wClock.setcmd( tick, wClock.go );
          /* inform all digints */
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "inform all digints..." );
          ControlOp.cmd( (iOControl)inst, tick, NULL );
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "freeze/resume clock not possible with real time" );
      }
    }
    else {
      iONode clockini = wRocRail.getclock( AppOp.getIni() );
      if( clockini != NULL ) {
        wClock.setdivider( clockini, wClock.getdivider(nodeA) );
      }
      data->devider = wClock.getdivider(nodeA);
      data->time = wClock.gettime(nodeA);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting time with devider %d", data->devider );
      data->timeset = True;
      {
        iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
        wClock.setdivider( tick, data->devider );
        wClock.settime( tick, data->time );
        wClock.setcmd( tick, wClock.set );
        /* inform all digints */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "inform all digints..." );
        ControlOp.cmd( (iOControl)inst, tick, NULL );
      }
    }
    return;
  }
  else if( StrOp.equals( wSwitch.name(), nodeName ) ) {
    iOSwitch sw = ModelOp.getSwitch( model, wSwitch.getid( nodeA ) );
    if( sw != NULL ) {
      SwitchOp.cmd( sw, nodeA, True, 0, &error );
      return;
    }
  }
  else if( StrOp.equals( wSignal.name(), nodeName ) ) {
    iOSignal sg = ModelOp.getSignal( model, wSignal.getid( nodeA ) );
    if( sg != NULL ) {
      SignalOp.cmd( sg, nodeA, True );
      return;
    }
  }
  else if( StrOp.equals( wOutput.name(), nodeName ) ) {
    iOOutput co = ModelOp.getOutput( model, wOutput.getid( nodeA ) );
    if( co != NULL ) {
      OutputOp.cmd( co, nodeA, True );
      return;
    }
  }
  else if( StrOp.equals( wFeedback.name(), nodeName ) ) {
    iOFBack fb = ModelOp.getFBack( model, wFeedback.getid( nodeA ) );
    if( fb != NULL ) {
      FBackOp.cmd( fb, nodeA, True );
      return;
    }
  }
  else if( StrOp.equals( wRoute.name(), nodeName ) ) {
    iORoute route = ModelOp.getRoute( model, wRoute.getid( nodeA ) );
    if( route != NULL ) {
      RouteOp.cmd( route, nodeA );
      return;
    }
  }
  else if( StrOp.equals( wLoc.name(), nodeName ) ) {
    iOLoc loc = ModelOp.getLoc( model, wLoc.getid( nodeA ) );
    if( loc != NULL ) {
      LocOp.cmd( loc, nodeA );
      return;
    }
  }
  else if( StrOp.equals( wDataReq.name(), nodeName ) ) {
    if( wDataReq.getcmd(nodeA) == wDataReq.get ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DataReq filename=[%s]", wDataReq.getfilename(nodeA) );
      if( wDataReq.gettype(nodeA) == wDataReq.image ) {
        iOFile f = NULL;
        char* filename = StrOp.fmt( "%s%c%s", AppOp.getImgPath(),
            SystemOp.getFileSeparator(), FileOp.ripPath(wDataReq.getfilename(nodeA)) );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Server filename=[%s]", filename );
        f = FileOp.inst( filename, OPEN_READONLY);
        if( f != NULL ) {
          int   size    = FileOp.size(f);
          if( size > 0 && size < (50*1024) ) {
            byte* buffer  = allocMem( size );
            char* byteStr = NULL;

            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "reading %d bytes...", size );
            FileOp.read( f, buffer, size );
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "convert %d bytes to string...", size );
            byteStr = StrOp.byteToStr( buffer, size );
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "free buffer..." );
            freeMem(buffer);
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "setdata..." );
            wDataReq.setdata( nodeA, byteStr );
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "free byteStr..." );
            StrOp.free( byteStr );
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "post event..." );
            ClntConOp.postEvent( AppOp.getClntCon(), nodeA, wCommand.getserver( nodeA ) );
            return;
          }
          else {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "image file size out of range! [%d]", size );
          }
        }
      }
    }
  }
  else if( StrOp.equals( wFunCmd.name(), nodeName ) ) {
    iOLoc loc = ModelOp.getLoc( model, wFunCmd.getid( nodeA ) );
    if( loc != NULL ) {
      LocOp.cmd( loc, nodeA );
      return;
    }
  }
  else if( StrOp.equals( wBlock.name(), nodeName ) ) {
    iIBlockBase block = ModelOp.getBlock( model, wBlock.getid( nodeA ) );
    if( block != NULL ) {
      block->cmd( block, nodeA );
      return;
    }
  }
  else if( StrOp.equals( wTurntable.name(), nodeName ) ) {
    iOTT tt = ModelOp.getTurntable( model, wTurntable.getid( nodeA ) );
    if( tt != NULL ) {
      TTOp.cmd( (iIBlockBase)tt, nodeA );
      return;
    }
  }
  else if( StrOp.equals( wSelTab.name(), nodeName ) ) {
    iOSelTab seltab = ModelOp.getSelectiontable( model, wSelTab.getid( nodeA ) );
    if( seltab != NULL ) {
      SelTabOp.cmd( (iIBlockBase)seltab, nodeA );
      return;
    }
  }
  else if( StrOp.equals( wModelCmd.name(), nodeName ) ) {
    if( wModelCmd.getcmdfrom( nodeA ) != NULL )
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command from [%s]", wModelCmd.getcmdfrom( nodeA ) );
    ModelOp.cmd( model, nodeA );
    return;
  }
  else if( StrOp.equals( wSysCmd.name(), nodeName ) ) {
    if( StrOp.equals( wSysCmd.shutdown, wSysCmd.getcmd( nodeA ) ) ) {
      /* TODO: Cleanup... */
      AppOp.shutdown(  );
      return;
    }
    else if( StrOp.equals( wSysCmd.getini, wSysCmd.getcmd( nodeA ) ) ) {
      iONode ini = (iONode)NodeOp.base.clone( AppOp.getNewIni() );
      ClntConOp.postEvent( AppOp.getClntCon(), ini, wCommand.getserver( nodeA ) );
      NodeOp.base.del( nodeA );
      return;
    }
    else if( StrOp.equals( wSysCmd.getmodplan, wSysCmd.getcmd( nodeA ) ) ) {
      iONode ini = (iONode)NodeOp.base.clone( ModelOp.getModPlan(model) );
      ClntConOp.postEvent( AppOp.getClntCon(), ini, wCommand.getserver( nodeA ) );
      NodeOp.base.del( nodeA );
      return;
    }
    else if( StrOp.equals( wSysCmd.getmodule, wSysCmd.getcmd( nodeA ) ) ) {
      iONode ini = (iONode)NodeOp.base.clone( ModelOp.getModule(model, wSysCmd.getid(nodeA)) );
      ClntConOp.postEvent( AppOp.getClntCon(), ini, wCommand.getserver( nodeA ) );
      NodeOp.base.del( nodeA );
      return;
    }
    else if( StrOp.equals( wSysCmd.setini, wSysCmd.getcmd( nodeA ) ) ) {
      iONode ini = NodeOp.getChild( nodeA, 0 );
      AppOp.setIni( ini );
      NodeOp.base.del( nodeA );
      /* inform the controller: could be an option store rerquest... */
      __informDigInts((iOControl)inst);
      return;
    }
    else if( StrOp.equals( wSysCmd.setmodplan, wSysCmd.getcmd( nodeA ) ) ) {
      iONode ini = NodeOp.getChild( nodeA, 0 );
      ModelOp.setModPlan( model, ini );
      NodeOp.base.del( nodeA );
      return;
    }
    else if( StrOp.equals( wSysCmd.setmodule, wSysCmd.getcmd( nodeA ) ) ) {
      iONode ini = NodeOp.getChild( nodeA, 0 );
      ModelOp.setModule( model, ini );
      NodeOp.base.del( nodeA );
      return;
    }
    else if( StrOp.equals( wSysCmd.grouplink, wSysCmd.getcmd( nodeA ) ) ) {
      iONode link = NodeOp.getChild( nodeA, 0 );
      if( link != NULL ) {
        /* TODO: but what? */
        __grouplink( inst, link );
      }
      NodeOp.base.del( nodeA );
      return;
    }
  }
  else if( StrOp.equals( wAutoCmd.name(), nodeName ) ) {
    ModelOp.cmd( model, nodeA );
    return;
  }
  else if( StrOp.equals( wPlan.name(), nodeName ) ) {
    /* Serialize plan. */
    const char* fname = wPlan.getname( nodeA );
    char* xml = NodeOp.base.toString( nodeA );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serialized Plan=%d", StrOp.len( xml ) );
    if( StrOp.len( xml ) > 0 )
    {
      iOFile planFile;

      planFile = FileOp.inst( fname, False );
      if( planFile != NULL ) {
        FileOp.write( planFile, xml, StrOp.len( xml ) );
        FileOp.close( planFile );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "plan [%s] saved [%ld bytes].", fname, FileOp.getWritten( planFile ) );
        /* Cleanup. */
        planFile->base.del(planFile);
      }
    }
    StrOp.free( xml );
    NodeOp.base.del( nodeA );
    return;
  }
  else if( StrOp.equals( wProgram.name(), nodeName ) ) {
    if( wProgram.getcmd(nodeA) == wProgram.save ) {
      iOLoc loc = ModelOp.getLocByAddress( model, wProgram.getaddr(nodeA) );
      if( loc != NULL ) {
        LocOp.setCV( loc, wProgram.getcv(nodeA), wProgram.getvalue(nodeA) );
      }
      ClntConOp.postEvent( AppOp.getClntCon(), (iONode)NodeOp.base.clone(nodeA), wCommand.getserver( nodeA ) );
      NodeOp.base.del( nodeA );
      return;
    }
    else if( wProgram.getcmd(nodeA) == wProgram.load ) {
      iOLoc loc = ModelOp.getLocByAddress( model, wProgram.getaddr(nodeA) );
      if( loc != NULL ) {
        int value = LocOp.getCV( loc, wProgram.getcv(nodeA) );
        wProgram.setvalue(nodeA, value);
      }
      ClntConOp.postEvent( AppOp.getClntCon(), (iONode)NodeOp.base.clone(nodeA), wCommand.getserver( nodeA ) );
      NodeOp.base.del( nodeA );
      return;
    }
    else {
      ControlOp.cmd( (iOControl)inst, nodeA, &error );
    }
    return;
  }

  ControlOp.cmd( (iOControl)inst, nodeA, &error );
}

static void __listener( obj inst, iONode nodeC, int level ) {
  iOControlData data = Data(inst);
  /* event from digitalInterface */
  /* inform model */
  iOModel model = AppOp.getModel(  );

  if( level == TRCLEVEL_EXCEPTION ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, NodeOp.getStr( nodeC, "msg", "" ) );
    /*AppOp.stop(  );*/
  }

  if( nodeC == NULL )
    return;

  if( StrOp.equals( wResponse.name(), NodeOp.getName( nodeC ) ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, NodeOp.getStr( nodeC, "msg", "--empty message--" ) );
  }

  if( StrOp.equals( wCommand.name(), NodeOp.getName( nodeC ) ) ) {
    /* command for another controller: */
    if( NodeOp.getChildCnt( nodeC ) > 0 ) {
      iONode cmd = NodeOp.getChild( nodeC, 0 );
      /* release the child node and delete the parent: */
      NodeOp.removeChild( nodeC, cmd );
      NodeOp.base.del(nodeC);
      ControlOp.cmd( (iOControl)inst, cmd, NULL );
    }
  }
  else if( StrOp.equals( wThrottleCmd.name(), NodeOp.getName( nodeC ) ) ) {
    /* Dispatch to the throttle object: */
    if( data->throttle != NULL )
      ThrottleOp.cmd( data->throttle, nodeC );
  }
  else if( StrOp.equals( wDigInt.name(), NodeOp.getName( nodeC ) ) ) {
    /* Broadcast to clients. Node3 */
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeC );
  }
  else if( StrOp.equals( wProgram.name(), NodeOp.getName( nodeC ) ) ) {
    /* check if it is a multiport event */
    if( wProgram.getlntype(nodeC) == wProgram.lntype_mp ) {
      /* TODO: inform mp listeners */
    }

    /* Broadcast to clients. Node3 */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Program event...value=%d", wProgram.getvalue( nodeC ) );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeC );
  }
  else if( StrOp.equals( wState.name(), NodeOp.getName( nodeC ) ) ) {
    /* Broadcast to clients. Node3 */
    wState.setconsolemode( nodeC, AppOp.isConsoleMode() );
    if( data->power && !wState.ispower( nodeC ) ) {
      /* freeze clock */
      control_callback cb = ControlOp.getCallback((iOControl)inst);
      iONode clockcmd = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
      wClock.setcmd(clockcmd, wClock.freeze );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power off, freeze clock" );
      cb(inst,clockcmd);
    }
    data->power        = wState.ispower( nodeC );
    data->programming  = wState.isprogramming( nodeC );
    data->trackbus     = wState.istrackbus( nodeC );
    data->sensorbus    = wState.issensorbus( nodeC );
    data->accessorybus = wState.isaccessorybus( nodeC );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "State event from=%s", wState.getiid( nodeC )==NULL?"":wState.getiid( nodeC ) );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeC );
  }
  else
    ModelOp.event( model, nodeC );
}


static iONode _getState( iOControl inst ) {
  iOControlData data = Data(inst);
  iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
  wState.setpower( node, data->power );
  wState.setprogramming( node, data->programming );
  wState.settrackbus( node, data->trackbus );
  wState.setsensorbus( node, data->sensorbus );
  wState.setaccessorybus( node, data->accessorybus );
  return node;
}


static void __informDigInts( iOControl inst ) {
  iOControlData o = Data(inst);
  iONode rsp = NULL;
  iONode ini    = AppOp.getNewIni();
  iONode digint = wRocRail.getdigint( ini );
  while( digint != NULL ) {
    const char*  iid = wDigInt.getiid( digint );
    iIDigInt pDi = NULL;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "informDigInt [%s]...", iid!=NULL ? iid:"?" );

    if( iid != NULL ) {
      pDi = (iIDigInt)MapOp.get( o->diMap, iid );
      if( pDi != NULL ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "informing fitting DigInt [%s]...", iid );
        pDi->cmd( (obj)pDi, (iONode)NodeOp.base.clone(digint) );
      }
    }

    digint = wRocRail.nextdigint( ini, digint );
  };
}


static Boolean __initDigInts( iOControl inst ) {
  iOControlData o = Data(inst);
  iOModel model = AppOp.getModel();
  iONode ini    = AppOp.getIni();
  iONode plan   = ModelOp.getModel( model );
  iONode modeldigint = plan?wPlan.getdigint( plan ):NULL;
  iONode digint = wRocRail.getdigint( ini );
  Boolean bModelDigints = modeldigint == NULL ? False:True;

  if( bModelDigints ) {
    digint = modeldigint;
  }

  if( digint == NULL ) {
    /* no digint specified */
    iONode virtual = NodeOp.inst( wDigInt.name(), ini, ELEMENT_NODE );
    wDigInt.setlib( virtual, wDigInt.vcs );
    wDigInt.setiid( virtual, "vcs-1" );
    NodeOp.addChild( ini, virtual );
    digint = virtual;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no digint defined; using default [virtual]" );
  }

  while( digint != NULL ) {
    /* TODO! Create Map with multiple digints; one is the default,
             others should be addressed with their iid.
             <system iid="p50_1" cmd="go"/>
    */
    const char*  lib = wDigInt.getlib( digint );
    const char*  iid = wDigInt.getiid( digint );
    iIDigInt pDi = NULL;
    iOLib    pLib = NULL;
    LPFNROCGETDIGINT pInitFun = (void *) NULL;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "initDigInts lib=\"%s\" idd=\"%s\"", lib, iid!=NULL ? iid:"?" );

    wDigInt.setstress( digint, AppOp.isStress());

    wDigInt.setlibpath( digint, AppOp.getLibPath() );
    {
      char* libpath = StrOp.fmt( "%s%c%s", AppOp.getLibPath(), SystemOp.getFileSeparator(), lib );
      pLib = LibOp.inst( libpath );
      StrOp.free( libpath );
    }


    if (pLib == NULL)
      return False;
    pInitFun = (LPFNROCGETDIGINT)LibOp.getProc(pLib,"rocGetDigInt");
    if (pInitFun == NULL)
      return False;
    pDi = pInitFun(digint,TraceOp.get());

    if (pDi == NULL) {
      return False;
    }
    else {
      /* vmajor*10000 + vminor*100 */
      int libVersion = pDi->version((obj)pDi);
      int vmajor = libVersion/10000;
      int vminor = (libVersion%10000)/100;
      if( vmajor != wGlobal.vmajor || vminor != wGlobal.vminor ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "version mismatch for library [%s]; reports:[%d.%d] wanted:[%d.%d]", lib, vmajor, vminor, wGlobal.vmajor, wGlobal.vminor );
        return False;
      }
    }

    pDi->setListener( (obj)pDi, (obj)inst, &__listener );

    if( iid != NULL )
      MapOp.put( o->diMap, iid, (obj)pDi );

    /* First digint is default! */
    if( o->pDi == NULL ) {
      o->pDi = pDi;
      o->iid = iid != NULL ? iid:"default";
    }

    if( bModelDigints )
      digint = wPlan.nextdigint( ModelOp.getModel( model ), digint );
    else
      digint = wRocRail.nextdigint( ini, digint );
  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "initDigInts OK" );
  return True;
}

/*
 ***** _Public functions.
 */
static control_callback _getCallback( iOControl inst ) {
  iOControlData data = Data(inst);
  return &__callback;
}

static void _halt( iOControl inst ) {
  if( inst != NULL ) {
    iOControlData data = Data(inst);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down..." );

    ThreadOp.requestQuitAll();

    /* Inform DigInts. */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Informing interfaces..." );
    {
      iIDigInt di = (iIDigInt)MapOp.first( data->diMap );
      while( di != NULL ) {
        di->halt((obj)di );
        di = (iIDigInt)MapOp.next( data->diMap );
      }
    }
  }
}

static const char* _getIid( iOControl inst ) {
  iOControlData data = Data(inst);
  return data->iid;
}

static long _getTime( iOControl inst ) {
  iOControlData data = Data(inst);
  return data->time;
}


static void __checkActions( iOControl control ) {
  iOModel model = AppOp.getModel();
  iOControlData data = Data(control);

  if( model != NULL ) {
    iONode plan   = ModelOp.getModel( model );
    if( plan != NULL ) {
      iONode aclist = wPlan.getaclist(plan);
      if( aclist != NULL ) {
        iONode action = wActionList.getac( aclist );
        while( action != NULL ) {
          iOAction act = ModelOp.getAction(model, wAction.getid(action));
          if( act != NULL )
            ActionOp.tick(act);
          action = wActionList.nextac( aclist, action );
        };
      }
    }
  }

}

static void __clockticker( void* threadinst ) {
  iOThread        th = (iOThread)threadinst;
  iOControl  control = (iOControl)ThreadOp.getParm(th);
  iOControlData data = Data(control);
  iONode         ini = AppOp.getIni();
  iONode    clockini = wRocRail.getclock( ini );
  int        seconds = 0;
  Boolean    timeset = False;

  data->time = time(NULL);
  data->devider = 1;

  if( clockini != NULL ) {
    data->devider = wClock.getdivider( clockini );
    if( wClock.gethour( clockini ) < 24 && wClock.getminute( clockini ) < 60 ) {
      struct tm* ltm = localtime( &data->time );
      ltm->tm_hour = wClock.gethour( clockini );
      ltm->tm_min  = wClock.getminute( clockini );
      data->time = mktime(ltm);
    }
  }

  if( data->devider != 1 && data->devider != 2 && data->devider != 4 && data->devider != 5 && data->devider != 10 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "ClockTicker: unexpected devider value [%d] reset to [1].", data->devider );
    data->devider = 1;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ClockTicker started." );

  while( !ThreadOp.isQuit(th) ) {

    /* 1=1000, 2=500, 4=250, 5=200, 10=100 */
    ThreadOp.sleep( 1000 / data->devider );

    if( data->timeset ) {
      timeset = True;
      data->timeset = False;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ClockTicker time set." );
    }
    else if( data->clockrun ) {
      seconds++;
      if( seconds < 60 )
        continue;
    }
    else {
      continue;
    }

    seconds = 0;

    if( data->devider > 1 || timeset )
      data->time += 60;
    else
      data->time = time(NULL);

    {
      iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
      wClock.setdivider( tick, data->devider );
      wClock.settime( tick, data->time );
      wClock.setcmd( tick, wClock.sync );
      ClntConOp.broadcastEvent( AppOp.getClntCon(), (iONode)NodeOp.base.clone(tick) );
      /* inform all digints */
      ControlOp.cmd( control, tick, NULL );
    }

    __checkActions( control );

  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ClockTicker ended." );
}


static void __checker( void* threadinst ) {
  iOThread        th = (iOThread)threadinst;
  iOControl  control = (iOControl)ThreadOp.getParm(th);
  iOControlData data = Data(control);
  iONode         ini = AppOp.getIni();
  iOModel      model = AppOp.getModel();
  iOMap        swMap = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Checker started." );

  ThreadOp.sleep( 1000 );
  swMap = ModelOp.getSwitchMap(model);
  while( !ThreadOp.isQuit(th) ) {

    /* call the switches every 100 ms */
    ThreadOp.sleep( 100 );

    /* only in automode to make sure there are no edits on the map */
    if( ModelOp.isAuto(model) ) {
      iOSwitch sw = (iOSwitch)MapOp.first( swMap );

      while( sw != NULL && ModelOp.isAuto(model) ) {
        /* call the switch */
        SwitchOp.checkSenPos( sw );
        sw = (iOSwitch)MapOp.next( swMap );
      };
    }

  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Checker ended." );
}

static iOThrottle _getThrottle(iOControl control) {
  iOControlData data = Data(control);
  return data->throttle;
}

static iOControl _inst( Boolean nocom ) {
    iOControl     control = allocMem( sizeof( struct OControl ) );
    iOControlData data    = allocMem( sizeof( struct OControlData ) );
    iONode        ini     = AppOp.getIni();

    /* OBase operations */
    MemOp.basecpy( control, &ControlOp, 0, sizeof( struct OControl ), data );

    data->diMap = MapOp.inst();

    if( !nocom ) {
      __initDigInts( control );
    }

    /* instantiate the throttle object if a throttle node is found in the rocrail.ini */
    if( wRocRail.getthrottle(ini) != NULL ) {
      data->throttle = ThrottleOp.inst(wRocRail.getthrottle(ini));
    }

    data->clockrun = True;
    data->clockticker = ThreadOp.inst( "clockticker", __clockticker, control );
    ThreadOp.start( data->clockticker );

    data->checker = ThreadOp.inst( "checker", __checker, control );
    ThreadOp.start( data->checker );

    instCnt++;

  return control;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/control.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
