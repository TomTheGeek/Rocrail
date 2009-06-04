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

#include "rocrail/impl/tt_impl.h"
#include "rocrail/public/app.h"
#include "rocrail/public/model.h"
#include "rocrail/public/switch.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"


#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/TTTrack.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/ModelCmd.h"

static int instCnt = 0;

static int __getMappedTrack( iOTT inst, int tracknr );
static void __polarize(obj inst, int pos, Boolean polarization);


/*
 ***** OBase functions.
 */
static const char* __id( void* inst ) {
  iOTTData data = Data(inst);
  return wTurntable.getid( data->props );
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
  iOTTData data = Data(inst);
  iOTT     tt   = (iOTT)inst;

  return (char*)tt->base.id(tt);
}
static void __del(void* inst) {
  iOTTData data = Data(inst);
  MutexOp.base.del(data->muxLock);
  freeMem( data );
  freeMem( inst );
  instCnt--;
}
static void* __properties(void* inst) {
  iOTTData data = Data(inst);
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


/*
 ***** _Public functions.
 */
/**
 * Checks for property changes.
 * todo: Range checking?
 */
static void _modify( iOTT inst, iONode props ) {
  iOTTData o = Data(inst);
  Boolean move = StrOp.equals( wModelCmd.getcmd( props ), wModelCmd.move );
  int cnt = NodeOp.getAttrCnt( props );
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );
    NodeOp.setStr( o->props, name, value );
  }

  if( !move ) {
    cnt = NodeOp.getChildCnt( o->props );
    while( cnt > 0 ) {
      iONode child = NodeOp.getChild( o->props, 0 );
      NodeOp.removeChild( o->props, child );
      cnt = NodeOp.getChildCnt( o->props );
    }
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


static iONode __getTrackNode( iOTT inst, int tracknr ) {
  iOTTData data = Data(inst);

  iONode track = wTurntable.gettrack( data->props );
  while( track != NULL ) {
    if( tracknr == wTTTrack.getnr( track ) ) {
      if( wTTTrack.getnr( track ) == tracknr ) {
        return track;
      }
    }
    track = wTurntable.nexttrack( data->props, track );
  }

  return NULL;
}



/*
 * calculate direction
 *
 */
static const Boolean CCW = True;
static const Boolean CW  = False;
static Boolean __bridgeDir( iOTT inst, int destpos, Boolean* ttdir ) {
  iOTTData data = Data(inst);
  Boolean swap = wTurntable.isswaprotation(data->props);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "from track[%d] to track[%d]", data->tablepos, destpos );

  if( data->tablepos == destpos ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "wanted destination position[%d] equals the table position[%d]", destpos, data->tablepos );
    return False;
  }

  if( destpos > data->tablepos && destpos - data->tablepos <= 24 )
    *ttdir = swap?CW:CCW;
  else if( destpos < data->tablepos && data->tablepos - destpos >= 24 )
    *ttdir = swap?CW:CCW;
  else
    *ttdir = swap?CCW:CW;

  return True;
}


/**
 * function offset  red      green
 * -------- ------- -------- --------
 * lights   0       off      on
 * step     1       right    left
 * turn     2       right    left
 * dir      3       CW       CCW
 * pos      4       #1       #2
 * pos      5       #3       #4
 * pos      6       #5       #6
 * pos      7       #7       #8
 * pos      8       #9       #10
 * pos      9       #11      #12
 * pos      10      #13      #14
 * pos      11      #15      #16
 * pos      12      #17      #18
 * pos      13      #19      #20
 * pos      14      #21      #22
 * pos      15      #23      #24
 */
static const int DIGITALBAHN_LIGHT = 0;
static const int DIGITALBAHN_STEP  = 1;
static const int DIGITALBAHN_TURN  = 2;
static const int DIGITALBAHN_DIR   = 3;
static const int DIGITALBAHN_POS   = 4;

static const char* DIGITALBAHN_DIR_CW  = "turnout";
static const char* DIGITALBAHN_DIR_CCW = "straight";

static Boolean __cmd_digitalbahn( iOTT inst, iONode nodeA ) {
  iOTTData data = Data(inst);
  Boolean ok = True;
  iOControl control = AppOp.getControl();
  const char* cmdStr = wTurntable.getcmd( nodeA );
  Boolean ttdir = True;
  Boolean doDirCmd = False;
  int port = 0;
  const char* cmdstr = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "TT command = [%s]", cmdStr );

  if( StrOp.equals( wTurntable.next, cmdStr ) ) {
    port = DIGITALBAHN_STEP;
    cmdstr = wSwitch.straight;
    data->pending = True;
  }
  else if( StrOp.equals( wTurntable.prev, cmdStr ) ) {
    port = DIGITALBAHN_STEP;
    cmdstr = wSwitch.turnout;
    data->pending = True;
  }
  else if( StrOp.equals( wTurntable.turn180, cmdStr ) ) {
    port = DIGITALBAHN_TURN;
    cmdstr = wSwitch.straight;
    data->pending = True;
  }
  else if( StrOp.equals( wTurntable.lighton, cmdStr ) ) {
    port = DIGITALBAHN_LIGHT;
    data->light = !data->light;
    cmdstr = data->light?wSwitch.straight:wSwitch.turnout;
  }
  else if( StrOp.equals( wTurntable.lightoff, cmdStr ) ) {
    port = DIGITALBAHN_LIGHT;
    cmdstr = wSwitch.turnout;
  }
  else {
    /* Tracknumber */
    int tracknr = atoi( cmdStr );
    /* DA Save tracknumber for 180 degrees turn */
    int orig_tracknr = tracknr;

    Boolean move = __bridgeDir(inst, tracknr, &ttdir );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "Goto track %d, current pos=%d", tracknr, data->tablepos );

    if( move ) {
      /* check for a mapping of the track number */
      data->gotopos = tracknr;
      tracknr = __getMappedTrack( inst, tracknr );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Goto decoder track [%d] in direction [%s]", tracknr, ttdir ? "CCW":"CW" );
      data->pending = True;

      /* Broadcast to clients. */
      {
        iONode event = NodeOp.inst( wTurntable.name(), NULL, ELEMENT_NODE );
        wTurntable.setid( event, wTurntable.getid( data->props ) );
        wTurntable.setbridgepos( event, tracknr );
        if( wTurntable.getiid( data->props ) != NULL )
          wTurntable.setiid( event, wTurntable.getiid( data->props ) );
        ClntConOp.broadcastEvent( AppOp.getClntCon(), event );
      }

      doDirCmd = True;

      port = DIGITALBAHN_POS + ((tracknr-1)/2);
      if( (tracknr-1) % 2 == 0 )
        cmdstr = wSwitch.turnout;
      else
        cmdstr = wSwitch.straight;

      /* DA check whether 180 degrees turn is required */
      if( (data->tablepos-orig_tracknr) == 24)
      {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "Turn 180 ---> Goto track [%d], current pos=[%d]", tracknr, data->tablepos );
        port   = DIGITALBAHN_TURN;
        cmdstr = wSwitch.straight;
      }
      else if( (data->tablepos-orig_tracknr) == -24)
      {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "Turn 180 ---> Goto track [%d], current pos=[%d]", tracknr, data->tablepos );
        port   = DIGITALBAHN_TURN;
        cmdstr = wSwitch.straight;
      }
      /* DA check whether 180 degrees turn is required */

    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bridge already at track %d", tracknr );
      __polarize((obj)inst, tracknr, False);
    }
  }


  if( cmdstr != NULL && control != NULL )
  {
    /* Direction */
    if( doDirCmd ) {
      iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );

      int addr = wTurntable.getaddr( data->props );
      const char* iid = wTurntable.getiid( data->props );
      if( iid != NULL )
        wSwitch.setiid( cmd, iid );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set direction address=%d, port=%d, gate=%s",
          addr, DIGITALBAHN_DIR, ttdir ? DIGITALBAHN_DIR_CCW:DIGITALBAHN_DIR_CW );

      int addrCmd = ( addr + DIGITALBAHN_DIR) / 4 + 1;
      int portCmd = ( addr + DIGITALBAHN_DIR) % 4 + 1;
      wSwitch.setaddr1( cmd, addrCmd );
      wSwitch.setport1( cmd, portCmd );
      wSwitch.setcmd  ( cmd, ttdir ? DIGITALBAHN_DIR_CCW:DIGITALBAHN_DIR_CW );
      wSwitch.setprot( cmd, wTurntable.getprot( data->props ) );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sending switch command [%d,%d,%s]...", 
                   addrCmd, portCmd, ttdir ? DIGITALBAHN_DIR_CCW:DIGITALBAHN_DIR_CW);

      ControlOp.cmd( control, cmd, NULL );
      /* give the decoder some time to think... */
      ThreadOp.sleep( wTurntable.getpause( data->props ) * 1000 );
    }


    /* Command */
    iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );

    int addr = wTurntable.getaddr( data->props );
    const char* iid = wTurntable.getiid( data->props );
    if( iid != NULL )
      wSwitch.setiid( cmd, iid );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set position address=%d, port=%d, gate=%s",
        addr, port, cmdstr );

    int addrCmd = (addr + port) / 4 + 1;
    int portCmd = (addr + port) % 4 + 1;
    wSwitch.setaddr1( cmd, addrCmd );
    wSwitch.setport1( cmd, portCmd );
    wSwitch.setcmd  ( cmd, cmdstr );
    wSwitch.setprot( cmd, wTurntable.getprot( data->props ) );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sending switch command [%d,%d,%s]...", 
                 addrCmd, portCmd, cmdstr );

    ControlOp.cmd( control, cmd, NULL );
    /* give the decoder some time to think... */
    ThreadOp.sleep(100);

    /* set tablepos optimistic predicted, if fb is not defined for destination track */
    iONode track = wTurntable.gettrack( data->props );
    while ( track != NULL ) {
      if ( ( wTTTrack.getnr( track ) == data->gotopos) 
        && ( ( wTTTrack.getposfb( track) == NULL) || StrOp.equals( wTTTrack.getposfb( track), "\0"))) {

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set optimistic tablepos %d", data->tablepos );
        
        data->tablepos = data->gotopos;
        wTurntable.setbridgepos( data->props, data->tablepos );
      }
      track = wTurntable.nexttrack( data->props, track );
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no command sended" );
  }

  /* Cleanup Node1 */
  nodeA->base.del(nodeA);
  return ok;
}


static Boolean __cmd_multiport( iOTT inst, iONode nodeA ) {
  iOTTData data = Data(inst);
  Boolean ok = True;
  iOControl control = AppOp.getControl();
  const char* cmdStr = wTurntable.getcmd( nodeA );
  Boolean ttdir = True;
  iONode cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s", cmdStr );

  if( StrOp.equals( wTurntable.next, cmdStr ) ) {
    /*cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );*/
    ttdir = False;
    data->gotopos = -1;
    data->skippos = -1;
  }
  else if( StrOp.equals( wTurntable.prev, cmdStr ) ) {
    /*cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );*/
    ttdir = True;
    data->gotopos = -1;
    data->skippos = -1;
  }
  else if( StrOp.equals( wTurntable.turn180, cmdStr ) ) {
    /*cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );*/
    data->gotopos = data->tablepos;
    data->skippos = data->tablepos;
  }
  else {
    /* Tracknumber */
    int tracknr = atoi( cmdStr );
    Boolean move = __bridgeDir(inst, tracknr, &ttdir );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "Goto track %d, current pos=%d", tracknr, data->tablepos );

    if( move ) {
      cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    }

  }

  if( cmd != NULL && control != NULL )
  {
    const char* iid = wTurntable.getiid( data->props );
    if( iid != NULL )
      wTurntable.setiid( cmd, iid );

    /* pending move operation */
    data->pending = True;

    if( StrOp.equals( wTurntable.prot_MP, wTurntable.getprot(data->props) ) ) {
      /* rename node to program */
      NodeOp.setName( cmd, wProgram.name() );
      /* set type to multiport */
      wProgram.setlntype( cmd, wProgram.lntype_mp );
      wProgram.setcmd( cmd, wProgram.lncvset );
      wProgram.setaddr( cmd, wTurntable.getaddr0(data->props) );
      wProgram.setmodid( cmd, wTurntable.getaddr1(data->props) );
      wProgram.setcv( cmd, 0x000F ); /* mask */
      wProgram.setvalue( cmd, data->gotopos ); /* value */
      ControlOp.cmd( control, cmd, NULL );

    }
    else if( wTurntable.isswcmd(data->props) ){
      iONode lcmd = NULL;

      NodeOp.setName( cmd, wSwitch.name() );

      /* set the protocol */
      wSwitch.setprot( cmd, wTurntable.getprot(data->props) );

      /* signal new position will be set: */
      wSwitch.setaddr1( cmd, wTurntable.getaddr4(data->props) );
      wSwitch.setport1( cmd, wTurntable.getport4(data->props) );
      wOutput.setcmd( cmd, wSwitch.straight );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      wSwitch.setaddr1( cmd, wTurntable.getaddr0(data->props) );
      wSwitch.setport1( cmd, wTurntable.getport0(data->props) );
      wSwitch.setcmd( cmd, data->gotopos & 0x01 ? wSwitch.straight:wSwitch.turnout );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      wSwitch.setaddr1( cmd, wTurntable.getaddr1(data->props) );
      wSwitch.setport1( cmd, wTurntable.getport1(data->props) );
      wSwitch.setcmd( cmd, data->gotopos & 0x02 ? wSwitch.straight:wSwitch.turnout );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      wSwitch.setaddr1( cmd, wTurntable.getaddr2(data->props) );
      wSwitch.setport1( cmd, wTurntable.getport2(data->props) );
      wSwitch.setcmd( cmd, data->gotopos & 0x04 ? wSwitch.straight:wSwitch.turnout );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      wSwitch.setaddr1( cmd, wTurntable.getaddr3(data->props) );
      wSwitch.setport1( cmd, wTurntable.getport3(data->props) );
      wSwitch.setcmd( cmd, data->gotopos & 0x08 ? wSwitch.straight:wSwitch.turnout );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      /* signal new position is set: */
      wSwitch.setaddr1( cmd, wTurntable.getaddr4(data->props) );
      wSwitch.setport1( cmd, wTurntable.getport4(data->props) );
      wOutput.setcmd( cmd, wSwitch.turnout );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

    }
    else {
      iONode lcmd = NULL;
      /* set the protocol */
      wOutput.setprot( cmd, wTurntable.getprot(data->props) );

      /* signal new position will be set: */
      wOutput.setaddr( cmd, wTurntable.getaddr4(data->props) );
      wOutput.setport( cmd, wTurntable.getport4(data->props) );
      wOutput.setgate( cmd, wTurntable.getgate4(data->props) );
      wOutput.setcmd( cmd, wOutput.on );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      wOutput.setaddr( cmd, wTurntable.getaddr0(data->props) );
      wOutput.setport( cmd, wTurntable.getport0(data->props) );
      wOutput.setgate( cmd, wTurntable.getgate0(data->props) );
      wOutput.setcmd( cmd, data->gotopos & 0x01 ? wOutput.on:wOutput.off );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      wOutput.setaddr( cmd, wTurntable.getaddr1(data->props) );
      wOutput.setport( cmd, wTurntable.getport1(data->props) );
      wOutput.setgate( cmd, wTurntable.getgate1(data->props) );
      wOutput.setcmd( cmd, data->gotopos & 0x02 ? wOutput.on:wOutput.off );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      wOutput.setaddr( cmd, wTurntable.getaddr2(data->props) );
      wOutput.setport( cmd, wTurntable.getport2(data->props) );
      wOutput.setgate( cmd, wTurntable.getgate2(data->props) );
      wOutput.setcmd( cmd, data->gotopos & 0x04 ? wOutput.on:wOutput.off );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      wOutput.setaddr( cmd, wTurntable.getaddr3(data->props) );
      wOutput.setport( cmd, wTurntable.getport3(data->props) );
      wOutput.setgate( cmd, wTurntable.getgate3(data->props) );
      wOutput.setcmd( cmd, data->gotopos & 0x08 ? wOutput.on:wOutput.off );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );

      /* signal new position is set: */
      wOutput.setaddr( cmd, wTurntable.getaddr4(data->props) );
      wOutput.setport( cmd, wTurntable.getport4(data->props) );
      wOutput.setgate( cmd, wTurntable.getgate4(data->props) );
      wOutput.setcmd( cmd, wOutput.off );
      lcmd = (iONode)NodeOp.base.clone(cmd);
      ControlOp.cmd( control, lcmd, NULL );
    }

    data->dir = ttdir;

    ControlOp.cmd( control, cmd, NULL );
  }



  /* Cleanup Node1 */
  nodeA->base.del(nodeA);


  return ok;
}


static void __setLocDecFn( iONode cmd, int fn, Boolean state ) {
  char fStr[32];
  StrOp.fmtb( fStr, "f%d", fn );
  NodeOp.setBool( cmd, fStr, state );
}


static Boolean __cmd_locdec( iOTT inst, iONode nodeA ) {
  iOTTData data = Data(inst);
  Boolean ok = True;
  iOControl control = AppOp.getControl();
  const char* cmdStr = wTurntable.getcmd( nodeA );
  Boolean ttdir = True;
  iONode vcmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s", cmdStr );

  if( StrOp.equals( wTurntable.next, cmdStr ) ) {
    vcmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    ttdir = False;
    data->gotopos = -1;
    data->skippos = -1;
  }
  else if( StrOp.equals( wTurntable.prev, cmdStr ) ) {
    vcmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    ttdir = True;
    data->gotopos = -1;
    data->skippos = -1;
  }
  else if( StrOp.equals( wTurntable.turn180, cmdStr ) ) {
    vcmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    data->gotopos = data->tablepos;
    data->skippos = data->tablepos;
  }
  else {
    /* Tracknumber */
    int tracknr = atoi( cmdStr );
    Boolean move = __bridgeDir(inst, tracknr, &ttdir );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "Goto track %d, current pos=%d", tracknr, data->tablepos );

    if( move ) {
      data->gotopos = tracknr;
      vcmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bridge already at track %d", tracknr );
      __polarize((obj)inst, tracknr, False);
    }


  }

  if( vcmd != NULL && control != NULL )
  {
    const char* iid = wTurntable.getiid( data->props );
    if( iid != NULL )
      wTurntable.setiid( vcmd, iid );

    /* pending move operation */
    data->pending = True;

    wLoc.setaddr( vcmd, wTurntable.getaddr( data->props ) );

    /* Using the Loc wrapper for the other parameters: */
    wLoc.setV_mode( vcmd, wLoc.V_mode_percent );
    wLoc.setV( vcmd, wTurntable.getV( data->props ) );
    wLoc.setprot( vcmd, wTurntable.getprot( data->props ) );
    wLoc.setfn( vcmd, wTurntable.getactfn( data->props ) == 0 ? True:False );
    wLoc.setdir( vcmd, ttdir );
    data->dir = ttdir;

    ControlOp.cmd( control, vcmd, NULL );

    if(wTurntable.getactfn( data->props ) > 0) {
      iONode fcmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
      if( iid != NULL )
        wTurntable.setiid( fcmd, iid );
      wFunCmd.setaddr( fcmd, wTurntable.getaddr( data->props ) );
      wFunCmd.setfnchanged( fcmd, wTurntable.getactfn( data->props ));
      __setLocDecFn( fcmd, wTurntable.getactfn( data->props ), True );

      wLoc.setV_mode( fcmd, wLoc.V_mode_percent );
      wLoc.setV( fcmd, wTurntable.getV( data->props ) );
      wLoc.setprot( fcmd, wTurntable.getprot( data->props ) );
      wLoc.setfn( fcmd, wTurntable.getactfn( data->props ) == 0 ? True:False );
      wLoc.setdir( fcmd, ttdir );

      ControlOp.cmd( control, fcmd, NULL );
    }

  }



  /* Cleanup Node1 */
  nodeA->base.del(nodeA);
  return ok;
}


static Boolean _setListener( iOTT inst, obj listenerObj, const tt_listener listenerFun ) {
  iOTTData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%s listener set for %s",
    inst->base.id( inst ),
    listenerObj->toString(listenerObj) );
  return True;
}

static Boolean _cmd( iIBlockBase inst, iONode nodeA ) {
  iOTTData data = Data(inst);
  if( StrOp.equals( wTurntable.getcmd(nodeA), wSwitch.unlock ) ) {
    TTOp.unLock( inst, data->lockedId );
  }
  else if( StrOp.equals( wTurntable.gettype( data->props ), wTurntable.locdec ) )
    return __cmd_locdec( (iOTT)inst, nodeA );
  else if( StrOp.equals( wTurntable.gettype( data->props ), wTurntable.digitalbahn ) )
    return __cmd_digitalbahn( (iOTT)inst, nodeA );
  else if( StrOp.equals( wTurntable.gettype( data->props ), wTurntable.multiport ) )
    return __cmd_multiport( (iOTT)inst, nodeA );
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                 "Unknown turntable type [%s] for [%s]",
                 wTurntable.gettype( data->props ),
                 wTurntable.getid( data->props ) );
  }
  return False;
}


static int __evaluatePos( iOTT inst, Boolean puls, const char* id, Boolean* polarization ) {
  iOTTData data = Data(inst);

  iONode track = wTurntable.gettrack( data->props );
  while( track != NULL ) {
    if( StrOp.equals( id, wTTTrack.getposfb( track ) ) ) {
      data->tablepos = wTTTrack.getnr( track );
      *polarization = wTTTrack.ispolarization(track);
      return wTTTrack.getnr( track );
    }
    track = wTurntable.nexttrack( data->props, track );
  }

  return -1;
}


static int __getMappedTrack( iOTT inst, int tracknr ) {
  iOTTData data = Data(inst);

  iONode track = wTurntable.gettrack( data->props );
  while( track != NULL ) {
    if( tracknr == wTTTrack.getnr( track ) ) {
      if( wTTTrack.getdecnr( track ) != -1 ) {
        return wTTTrack.getdecnr( track );
      }
    }
    track = wTurntable.nexttrack( data->props, track );
  }

  return tracknr;
}


static void __fbPositionEvent( obj inst, Boolean puls, const char* id, int ident, int val ) {
  iOTTData data = Data(inst);
  iOControl control = AppOp.getControl();
  /* TODO: evaluate position event */

  if( puls ) {

    if( data->gotopos != -1 ) {
      data->tablepos = data->gotopos;
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fbPositionEvent for Turntable [%s] fb=[%s]",
        inst->id(inst), id, data->tablepos );

    /* Broadcast to clients. */
    {
      iONode event = NodeOp.inst( wTurntable.name(), NULL, ELEMENT_NODE );
      wTurntable.setid( event, wTurntable.getid( data->props ) );
      wTurntable.setbridgepos( event, data->tablepos );
      wTurntable.setstate1( event, wTurntable.isstate1(data->props) );
      wTurntable.setstate2( event, wTurntable.isstate2(data->props) );
      if( wTurntable.getiid( data->props ) != NULL )
        wTurntable.setiid( event, wTurntable.getiid( data->props ) );
      ClntConOp.broadcastEvent( AppOp.getClntCon(), event );
    }


    /* bridge is in position */
    data->pending = False;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "bridge at position pos=%d ", data->tablepos );
    wTurntable.setbridgepos( data->props, data->tablepos );

    if( wTurntable.getdelay( data->props) > 0 ) {
      data->delaytick = SystemOp.getTick();
    }
    else {
      data->delaytick = 0;
    }



    if( StrOp.equals( wTurntable.gettype( data->props ), wTurntable.locdec ) ) {
      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      const char* iid = wTurntable.getiid( data->props );
      if( iid != NULL )
        wTurntable.setiid( cmd, iid );

      wLoc.setaddr( cmd, wTurntable.getaddr( data->props ) );

      /* Using the Loc wrapper for the other parameters: */
      wLoc.setV_mode( cmd, wLoc.V_mode_percent );
      wLoc.setV( cmd, 0 );
      wLoc.setfn( cmd, False );
      wLoc.setdir( cmd, data->dir );

      if(wTurntable.getactfn( data->props ) > 0) {
        iONode fcmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
        if( iid != NULL )
          wTurntable.setiid( fcmd, iid );
        wFunCmd.setaddr( fcmd, wTurntable.getaddr( data->props ) );
        wFunCmd.setfnchanged( fcmd, wTurntable.getactfn( data->props ));
        __setLocDecFn( fcmd, wTurntable.getactfn( data->props ), False );

        wLoc.setV_mode( fcmd, wLoc.V_mode_percent );
        wLoc.setV( fcmd, wTurntable.getV( data->props ) );
        wLoc.setprot( fcmd, wTurntable.getprot( data->props ) );
        wLoc.setfn( fcmd, wTurntable.getactfn( data->props ) == 0 ? True:False );
        wLoc.setdir( fcmd, data->dir );

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "turn function %d off...", wTurntable.getactfn( data->props ) );
        ControlOp.cmd( control, fcmd, NULL );
      }

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "turn motor off..." );
      ControlOp.cmd( control, cmd, NULL );


    }

  }

}


static void __fbBridgeEvent( obj inst, Boolean puls, const char* id, int ident, int val ) {
  iOTTData data = Data(inst);
  const char* event = NULL;
  Boolean state1 = wTurntable.isstate1( data->props );
  Boolean state2 = wTurntable.isstate2( data->props );

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Turntable:%s: fbid=%s state=%s ident=%d",
                 inst->id(inst), id, puls?"true":"false", ident );

  /* send event to the turntable owner block */
  if( StrOp.equals( id, wTurntable.gets1( data->props ) ) ) {
    state1 = puls;
    if( data->triggerS1 && data->triggerS2 ) {
      /* ignore */
    }
    else if( !data->triggerS1 && !data->triggerS2 ) {
      /* enter event */
      data->triggerS1 = True;
      if( wTurntable.gets2( data->props ) == NULL || StrOp.len(wTurntable.gets2( data->props )) == 0 ) {
        data->triggerS2 = True;
        event = wFeedbackEvent.enter2in_event;
      }
      else
        event = wFeedbackEvent.enter_event;
    }
    else if( !data->triggerS1 && data->triggerS2 ) {
      /* in event */
      data->triggerS1 = True;
      event = wFeedbackEvent.in_event;
    }
  }
  else if( StrOp.equals( id, wTurntable.gets2( data->props ) ) ) {
    state2 = puls;
    if( data->triggerS1 && data->triggerS2 ) {
      /* ignore */
    }
    else if( !data->triggerS1 && !data->triggerS2 ) {
      /* enter event */
      data->triggerS2 = True;
      event = wFeedbackEvent.enter_event;
    }
    else if( data->triggerS1 && !data->triggerS2 ) {
      /* in event */
      data->triggerS2 = True;
      event = wFeedbackEvent.in_event;
    }
  }

  if( event != NULL && data->listenerFun != NULL ) {
    /* Call listener. */
    data->listenerFun( data->listenerObj, event, inst->id(inst) );
  }

  wTurntable.setstate1( data->props, state1 );
  wTurntable.setstate2( data->props, state2 );

  /* Broadcast to clients. */
  {
    iONode node = NodeOp.inst( wTurntable.name(), NULL, ELEMENT_NODE );
    wTurntable.setid( node, inst->id(inst) );
    wTurntable.setstate1( node, state1 );
    wTurntable.setstate2( node, state2 );
    wTurntable.setbridgepos( node, wTurntable.getbridgepos( data->props) );
    if( wTurntable.getiid( data->props ) != NULL )
      wTurntable.setiid( node, wTurntable.getiid( data->props ) );
    ClntConOp.broadcastEvent( AppOp.getClntCon(), node );
  }

}


static void __polarize(obj inst, int pos, Boolean polarization) {
  iOTTData data = Data(inst);
  iOControl control = AppOp.getControl();


  if( pos != -1 ) {
    iONode track = wTurntable.gettrack( data->props );
    while( track != NULL ) {
      if( wTTTrack.getnr( track ) == pos ) {
        polarization = wTTTrack.ispolarization(track);
        break;
      }
      track = wTurntable.nexttrack( data->props, track );
    }
  }


  if( wTurntable.getpoladdr( data->props ) > 0 ) {
    iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
    int addr = wTurntable.getpoladdr( data->props );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "setting bridge polarization to %d", polarization );

    const char* iid = wTurntable.getiid( data->props );
    if( iid != NULL )
      wSwitch.setiid( cmd, iid );

    wSwitch.setprot( cmd, wTurntable.getprot( data->props ) );

    if( polarization ) {
      wSwitch.setaddr1( cmd, 0 );
      wSwitch.setport1( cmd, addr );
      wSwitch.setcmd( cmd, wSwitch.turnout );
      ControlOp.cmd( control, cmd, NULL );
    }
    else {
      wSwitch.setaddr1( cmd, 0 );
      wSwitch.setport1( cmd, addr );
      wSwitch.setcmd( cmd, wSwitch.straight );
      ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );
    }

  }

}


static void __fbEvent( obj inst, Boolean puls, const char* id, int identifier, int val ) {
  iOTTData data = Data(inst);
  iOControl control = AppOp.getControl();
  int prevpos = data->tablepos;
  Boolean polarization;
  int pos = __evaluatePos( (iOTT)inst, puls, id, &polarization );
  Boolean stop = False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fbEvent for Turntable [%s] fb=[%s] val=[%d] pos=[%d] polarization=[%d] ",
      inst->id(inst), id, val, pos, polarization );

  if( control == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "controller is not initialized..." );
    return;
  }

  if( !puls ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "OFF events are not used..." );
    return;
  }

  /* Broadcast to clients. */
  {
    iONode event = NodeOp.inst( wTurntable.name(), NULL, ELEMENT_NODE );
    wTurntable.setid( event, wTurntable.getid( data->props ) );
    wTurntable.setbridgepos( event, pos );
    wTurntable.setstate1( event, wTurntable.isstate1(data->props) );
    wTurntable.setstate2( event, wTurntable.isstate2(data->props) );
    if( wTurntable.getiid( data->props ) != NULL )
      wTurntable.setiid( event, wTurntable.getiid( data->props ) );
    ClntConOp.broadcastEvent( AppOp.getClntCon(), event );
  }

  if( data->gotopos != -1 ) {
    if( pos == data->gotopos ) {
      stop = True;
      data->gotopos = -1;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "bridge not at position pos=%d goto=%d", pos, data->gotopos );
    }
  }
  else if( puls )
    stop = True;

  if( stop ) {
    /* bridge is in position */
    data->pending = False;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "bridge at position pos=%d ", pos );
    wTurntable.setbridgepos( data->props, pos );

    if( wTurntable.getdelay( data->props) > 0 ) {
      data->delaytick = SystemOp.getTick();
    }
    else {
      data->delaytick = 0;
    }

  }

  if( stop && StrOp.equals( wTurntable.gettype( data->props ), wTurntable.locdec ) ) {
    iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    const char* iid = wTurntable.getiid( data->props );
    if( iid != NULL )
      wTurntable.setiid( cmd, iid );

    wLoc.setaddr( cmd, wTurntable.getaddr( data->props ) );

    /* Using the Loc wrapper for the other parameters: */
    wLoc.setV_mode( cmd, wLoc.V_mode_percent );
    wLoc.setV( cmd, 0 );
    wLoc.setfn( cmd, False );
    wLoc.setdir( cmd, data->dir );

    if(wTurntable.getactfn( data->props ) > 0) {
      iONode fcmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
      if( iid != NULL )
        wTurntable.setiid( fcmd, iid );
      wFunCmd.setaddr( fcmd, wTurntable.getaddr( data->props ) );
      wFunCmd.setfnchanged( fcmd, wTurntable.getactfn( data->props ));
      __setLocDecFn( fcmd, wTurntable.getactfn( data->props ), False );

      wLoc.setV_mode( fcmd, wLoc.V_mode_percent );
      wLoc.setV( fcmd, wTurntable.getV( data->props ) );
      wLoc.setprot( fcmd, wTurntable.getprot( data->props ) );
      wLoc.setfn( fcmd, wTurntable.getactfn( data->props ) == 0 ? True:False );
      wLoc.setdir( fcmd, data->dir );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "turn function %d off...", wTurntable.getactfn( data->props ) );
      ControlOp.cmd( control, fcmd, NULL );
    }

    /* TODO: wait in a thread to prevent blocking the system*/
    ThreadOp.sleep(10 + wTurntable.getmotoroffdelay( data->props ));

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "turn motor off..." );
    ControlOp.cmd( control, cmd, NULL );

  }

  __polarize(inst, -1, polarization );

}


static void __initCallback( iOTT inst ) {
  iOTTData data = Data(inst);
  iOModel model = AppOp.getModel();
  iONode track = wTurntable.gettrack( data->props );

  const char* s1   = wTurntable.gets1( data->props );
  const char* s2   = wTurntable.gets2( data->props );
  const char* psen = wTurntable.getpsen( data->props );


  if( s1 != NULL && StrOp.len( s1 ) > 0 ) {
    iOFBack bridgefb = ModelOp.getFBack( model, s1 );
    if( bridgefb != NULL ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Setting bridge listener [%s]", s1 );
      FBackOp.setListener( bridgefb, (obj)inst, &__fbBridgeEvent );
    }
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "bridge s1 [%s] not found!", s1 );
  }

  if( s2 != NULL && StrOp.len( s2 ) > 0 ) {
    iOFBack bridgefb = ModelOp.getFBack( model, s2 );
    if( bridgefb != NULL ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Setting bridge listener [%s]", s2 );
      FBackOp.setListener( bridgefb, (obj)inst, &__fbBridgeEvent );
    }
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "bridge s2 [%s] not found!", s2 );
  }

  if( psen != NULL && StrOp.len( psen ) > 0 ) {
    iOFBack posfb = ModelOp.getFBack( model, psen );
    if( posfb != NULL ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Setting position listener [%s]", psen );
      FBackOp.setListener( posfb, (obj)inst, &__fbPositionEvent );
    }
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "position sensor [%s] not found!", psen );
  }





  if( track == NULL )
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "No tracks defined for TT [%s]", inst->base.id(inst)  );

  while( track != NULL ) {
    const char* posfbid = wTTTrack.getposfb( track );

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "track [%d]", wTTTrack.getnr( track ) );

    if( posfbid != NULL && StrOp.len( posfbid ) > 0 ) {
      iOFBack posfb = ModelOp.getFBack( model, posfbid );
      if( posfb != NULL ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Setting listener [%s]", posfbid );
        FBackOp.setListener( posfb, (obj)inst, &__fbEvent );
      }
      else
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "posfb [%s] not found!", posfbid );
    }

    track = wTurntable.nexttrack( data->props, track );
  }
}


static Boolean _lock( iIBlockBase inst, const char* id, const char* blockid, Boolean crossing, Boolean reset, Boolean reverse ) {
  iOTTData data = Data(inst);
  Boolean ok = False;

  /* wait only 10ms for getting the mutex: */
  if( !MutexOp.trywait( data->muxLock, 10 ) ) {
    return False;
  }

  if( data->lockedId == NULL || StrOp.equals( id, data->lockedId ) ) {
    data->lockedId = id;
    /* Broadcast to clients. Node6 */
    {
      iONode nodeF = NodeOp.inst( wTurntable.name(), NULL, ELEMENT_NODE );
      wTurntable.setid( nodeF, inst->base.id(inst) );
      wTurntable.setbridgepos( nodeF, wTurntable.getbridgepos( data->props) );
      wTurntable.setstate1( nodeF, wTurntable.isstate1(data->props) );
      wTurntable.setstate2( nodeF, wTurntable.isstate2(data->props) );
      if( data->lockedId != NULL )
        wTurntable.setlocid( nodeF, data->lockedId );
      if( wTurntable.getiid( data->props ) != NULL )
        wTurntable.setiid( nodeF, wTurntable.getiid( data->props ) );
      ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeF );
    }
    ok = True;
  }

  data->triggerS1 = False;
  data->triggerS2 = False;

  /* Unlock the semaphore: */
  MutexOp.post( data->muxLock );

  return ok;
}


static Boolean _setLocSchedule( iIBlockBase inst, const char* scid ) {
  Boolean ok = False;
  if( inst != NULL && scid != NULL ) {
    iOTTData data = Data(inst);
  }
  return ok;
}



static Boolean _unLock( iIBlockBase inst, const char* id ) {
  iOTTData data = Data(inst);
  if( StrOp.equals( id, data->lockedId ) ) {
    data->lockedId = NULL;
    /* Broadcast to clients. Node6 */
    {
      iONode nodeF = NodeOp.inst( wTurntable.name(), NULL, ELEMENT_NODE );
      wTurntable.setid( nodeF, inst->base.id(inst) );
      wTurntable.setbridgepos( nodeF, wTurntable.getbridgepos( data->props) );
      wTurntable.setlocid( nodeF, wTurntable.unlocked );
      wTurntable.setstate1( nodeF, wTurntable.isstate1(data->props) );
      wTurntable.setstate2( nodeF, wTurntable.isstate2(data->props) );
      if( wTurntable.getiid( data->props ) != NULL )
        wTurntable.setiid( nodeF, wTurntable.getiid( data->props ) );
      ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeF );
    }
    data->triggerS1 = False;
    data->triggerS2 = False;
    return True;
  }
  return False;
}

static Boolean _isLocked( iOTT inst, const char* id ) {
  iOTTData data = Data(inst);
  if( data->lockedId != NULL && !StrOp.equals( id, data->lockedId ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Turntable \"%s\" is locked by \"%s\".",
        inst->base.id(inst), data->lockedId );
    return True;
  }
  return False;
}


static Boolean _isSet( iOTT inst ) {
  iOTTData data = Data(inst);

  if( !data->pending && data->delaytick > 0 ) {
    unsigned long delay = wTurntable.getdelay( data->props) * 100;
    if( data->delaytick + delay <= SystemOp.getTick() ) {
      data->delaytick = 0;
      return True;
    }
    else
      return False;
  }

  return data->pending ? False:True;
}


static Boolean _getRunDir( iOTT inst ) {
  iOTTData data = Data(inst);

  return data->lcdir;
}


static void _reset( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
             "reset turntable [%s]", inst->base.id(inst) );
  TTOp.unLock( inst, data->lockedId );
}


static iOTT _inst( iONode props ) {
  iOTT     tt    = allocMem( sizeof( struct OTT ) );
  iOTTData data  = allocMem( sizeof( struct OTTData ) );

  /* OBase operations */
  MemOp.basecpy( tt, &TTOp, 0, sizeof( struct OTT ), data );

  data->props = props;
  data->muxLock = MutexOp.inst( NULL, True );
  __initCallback( tt );
  data->tablepos = wTurntable.getbridgepos(data->props);
  data->gotopos = -1;
  data->lcdir = True;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "turntable [%s] initialized at position [%d]", tt->base.id(tt), data->tablepos );

  instCnt++;

  return tt;
}

static const char* _getLoc( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return "";
}

static const char* _getInLoc( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return "";
}

static void _event( iIBlockBase inst, Boolean puls, const char* id, int ident, int val, iONode fbevt ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
}

static const char* _getFromBlockId( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return "";
}

static const char* _getTDiid( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return "";
}


static int _getTDaddress( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return 0;
}


static int _getTDport( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return 0;
}


static void _setAnalog( iIBlockBase inst, Boolean analog ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
}


static Boolean _isLinked( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}

static void _setGroup( iIBlockBase inst, const char* group ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
}

static void _enterBlock( iIBlockBase inst, const char* id ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
}

static const char* _getVelocity( iIBlockBase inst, int* percent, Boolean onexit ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return "";
}

static int _getWait( iIBlockBase inst, iOLoc loc ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return 0;
}

static Boolean _green( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}

static Boolean _red( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}

static Boolean _yellow( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}

static Boolean _white( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}

static obj _hasManualSignal( iIBlockBase inst, Boolean distant, Boolean reverse ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return NULL;
}


static Boolean _hasEnter2Route( iIBlockBase inst, const char* fromBlockID ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}

static Boolean _hasPre2In( iIBlockBase inst, const char* fromBlockID ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}

static void _inBlock( iIBlockBase inst, const char* id ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
}

static Boolean _isTerminalStation( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}

static Boolean _link( iIBlockBase inst, iIBlockBase linkto ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}

static Boolean _unLink( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}


static Boolean _lockForGroup( iIBlockBase inst, const char* id ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}

static Boolean _unLockForGroup( iIBlockBase inst, const char* id ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}


static void _resetTrigs( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
}

static Boolean _wait( iIBlockBase inst, iOLoc loc ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
  return False;
}


static void _init( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  /* TODO: dispatch to active tracke block */
}



static Boolean _isReady( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  return True;
}


static void _setManager( iIBlockBase inst, iIBlockBase manager ) {
  iOTTData data = Data(inst);
}


static iIBlockBase _getManager( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  return NULL;
}


static Boolean _isFree( iIBlockBase inst, const char* locId ) {
  iOTTData data = Data(inst);
  return False;
}

static int _isSuited( iIBlockBase inst, iOLoc loc ) {
  iOTTData data = Data(inst);
  return 1;
}

static int _getVisitCnt( iIBlockBase inst, const char* id ) {
  iOTTData data = Data(inst);
  return 0;
}

static int _getOccTime( iIBlockBase inst ) {
  iOTTData data = Data(inst);
  return 0;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/tt.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/

