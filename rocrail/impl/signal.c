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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rocrail/impl/signal_impl.h"
#include "rocrail/public/app.h"

#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Ctrl.h"

#include "rocint/public/blockbase.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/thread.h"

#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Accessory.h"

static int instCnt = 0;

/*
 ***** OBase functions.
 */
static const char* __id( void* inst ) {
  iOSignalData data = Data(inst);
  return wSignal.getid(data->props);
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
  return (char*)SignalOp.getId( (iOSignal)inst );
}
static void __del(void* inst) {
  iOSignalData data = Data(inst);
  freeMem( data );
  freeMem( inst );
  instCnt--;
}
static void* __properties(void* inst) {
  iOSignalData data = Data(inst);
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
static const char* _getId( iOSignal inst ) {
  iOSignalData data = Data(inst);
  return wSignal.getid( data->props );
}

static const char* _getState( iOSignal inst ) {
  iOSignalData data = Data(inst);
  return wSignal.getstate( data->props );
}

static void* _getProperties( void* inst ) {
  iOSignalData data = Data((iOSignal)inst);
  return data->props;
}

static const char* __getPatternState(iOSignal inst, int pattern, int gate) {
  iOSignalData o = Data(inst);
  int pattern1, pattern2;
  const char* state = wSignal.getstate(o->props);
  int pattern1green = wSignal.getgreen( o->props ) & 0x0F;
  int pattern2green = (wSignal.getgreen( o->props ) & 0xF0) >> 4;
  int pattern1red = wSignal.getred( o->props ) & 0x0F;
  int pattern2red = (wSignal.getred( o->props ) & 0xF0) >> 4;
  int pattern1yellow = wSignal.getyellow( o->props ) & 0x0F;
  int pattern2yellow = (wSignal.getyellow( o->props ) & 0xF0) >> 4;
  int pattern1white = wSignal.getwhite( o->props ) & 0x0F;
  int pattern2white = (wSignal.getwhite( o->props ) & 0xF0) >> 4;
  int pattern1blank = wSignal.getblank( o->props ) & 0x0F;
  int pattern2blank = (wSignal.getblank( o->props ) & 0xF0) >> 4;

  if( StrOp.equals( state, wSignal.green ) ) {
    pattern1 = pattern1green;
    pattern2 = pattern2green;
  }
  else if( StrOp.equals( state, wSignal.red ) ) {
    pattern1 = pattern1red;
    pattern2 = pattern2red;
  }
  else if( StrOp.equals( state, wSignal.yellow ) ) {
    pattern1 = pattern1yellow;
    pattern2 = pattern2yellow;
  }
  else if( StrOp.equals( state, wSignal.white ) ) {
    pattern1 = pattern1white;
    pattern2 = pattern2white;
  }
  else if( StrOp.equals( state, wSignal.blank ) ) {
    pattern1 = pattern1blank;
    pattern2 = pattern2blank;
  }

  if( pattern == 1 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "event for signal %s, pattern1 ", wSignal.getid( o->props ), gate);
    pattern1 = gate;
  }
  else if( pattern == 2 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "event for signal %s, pattern2 ", wSignal.getid( o->props ), gate);
    pattern2 = gate;
  }

  /*
    OSignal  0167 p=2, pg1=0 pg2=0, pr1=1 pr2=0, py1=0 py2=1, pw1=0 pw2=0, pb1=0 pb2=0, state=green gate=1, p1=0 p2=1
    OSignal  0307 set sg3p to aspect green
   */

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "p=%d, pg1=%d pg2=%d, pr1=%d pr2=%d, py1=%d py2=%d, pw1=%d pw2=%d, pb1=%d pb2=%d, state=%s gate=%d, p1=%d p2=%d",
      pattern, pattern1green, pattern2green, pattern1red, pattern2red, pattern1yellow, pattern2yellow,
      pattern1white, pattern2white, pattern1blank, pattern2blank, state, gate, pattern1, pattern2);

  if( (pattern1 == pattern1green || pattern1green == 2) && (pattern2 == pattern2green || pattern2green == 2) ) {
    return wSignal.green;
  }
  else if( (pattern1 == pattern1red || pattern1red == 2) && (pattern2 == pattern2red || pattern2red == 2) ) {
    return wSignal.red;
  }
  else if( (pattern1 == pattern1yellow || pattern1yellow == 2) && (pattern2 == pattern2yellow || pattern2yellow == 2) ) {
    return wSignal.yellow;
  }
  else if( (pattern1 == pattern1white || pattern1white == 2) && (pattern2 == pattern2white || pattern2white == 2) ) {
    return wSignal.white;
  }
  else if( (pattern1 == pattern1blank || pattern1blank == 2) && (pattern2 == pattern2blank || pattern2blank == 2) ) {
    return wSignal.blank;
  }

  /* Default red. */
  return wSignal.red;
}


static void _event( iOSignal inst, iONode nodeC ) {
  iOSignalData data = Data(inst);
  Boolean update = False;
  Boolean acc = wAccessory.isaccevent(nodeC);
  int val = wAccessory.getval1( nodeC );
  const char* state = wSwitch.getstate(nodeC);
  const char* id = wSignal.getid( data->props );

  if( wSwitch.getgatevalue(nodeC) == 0 ) {
    /* Ignore off events */
    return;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "event for signal %s acc=%d state=%s active=%d...", id, val, state, wSwitch.getgatevalue(nodeC));

  if( TraceOp.getLevel(NULL) & TRCLEVEL_DEBUG ) {
    char* strNode = (char*)NodeOp.base.toString( nodeC );
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "signal event: %s", strNode );
    StrOp.free( strNode );
  }

  if( wSignal.getaspects(data->props) == 2 && wSignal.isasswitch(data->props) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"%s two aspect as switch", id);
    if( wSignal.isinv(data->props) )
      wSignal.setstate( data->props, StrOp.equals( state, wSwitch.turnout ) ? wSignal.red:wSignal.green );
    else
      wSignal.setstate( data->props, StrOp.equals( state, wSwitch.turnout ) ? wSignal.green:wSignal.red );
    update = True;
  }
  else {
    int bus = wSwitch.getbus( nodeC );
    int addr = wSwitch.getaddr1( nodeC );
    int port = wSwitch.getport1( nodeC );
    int gate = StrOp.equals( state, wSwitch.turnout ) ? 0:1;
    int pada, fada;

    int matchaddr1 = wSignal.getaddr(data->props);
    int matchport1 = wSignal.getport1(data->props);
    int matchaddr2 = wSignal.getaddr2(data->props);
    int matchport2 = wSignal.getport2(data->props);
    int matchaddr3 = wSignal.getaddr3(data->props);
    int matchport3 = wSignal.getport3(data->props);
    int matchaddr4 = wSignal.getaddr4(data->props);
    int matchport4 = wSignal.getport4(data->props);

    if( addr > 0 && port == 0 ) {
      /* flat */
      fada = matchaddr1;
      matchaddr1 = fada / 8 + 1;
      matchport1 = (fada % 8) /2 + 1;
      fada = matchaddr2;
      matchaddr2 = fada / 8 + 1;
      matchport2 = (fada % 8) /2 + 1;
      fada = matchaddr3;
      matchaddr3 = fada / 8 + 1;
      matchport3 = (fada % 8) /2 + 1;
      fada = matchaddr4;
      matchaddr4 = fada / 8 + 1;
      matchport4 = (fada % 8) /2 + 1;
    }
    else if( matchaddr1 == 0 && matchport1 > 0 ) {
      pada = matchport1;
      matchaddr1 = (pada - 1) / 4 + 1;
      matchport1 = (pada - 1) % 4 + 1;
      pada = matchport2;
      matchaddr2 = (pada - 1) / 4 + 1;
      matchport2 = (pada - 1) % 4 + 1;
      pada = matchport3;
      matchaddr3 = (pada - 1) / 4 + 1;
      matchport3 = (pada - 1) % 4 + 1;
      pada = matchport4;
      matchaddr4 = (pada - 1) / 4 + 1;
      matchport4 = (pada - 1) % 4 + 1;
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "addr=%d port=%d gate=%d, ma1=%d mp1=%d, ma2=%d mp2=%d, ma3=%d mp3=%d, ma4=%d mp4=%d",
        addr, port, gate, matchaddr1, matchport1, matchaddr2, matchport2, matchaddr3, matchport3, matchaddr4, matchport4);

    if( wSignal.getusepatterns(data->props)  == 0 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"%s multi aspect", id);
      if( matchaddr1 == addr && matchport1 == port && wSignal.getgate1(data->props) == gate ) {
        wSignal.setstate( data->props, wSignal.red );
        update = True;
      }
      else if( matchaddr2 == addr && matchport2 == port && wSignal.getgate2(data->props) == gate ) {
        if( wSignal.getaspects(data->props) == 2 )
          wSignal.setstate( data->props, wSignal.green );
        else
          wSignal.setstate( data->props, wSignal.yellow );
        update = True;
      }
      else if( matchaddr3 == addr && matchport3 == port && wSignal.getgate3(data->props) == gate ) {
        wSignal.setstate( data->props, wSignal.green );
        update = True;
      }
      else if( matchaddr4 == addr && matchport4 == port && wSignal.getgate4(data->props) == gate ) {
        wSignal.setstate( data->props, wSignal.white );
        update = True;
      }
      if( update )
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"set %s to aspect %s", id, wSignal.getstate(data->props) );
    }

    /* Patterns */
    else if( wSignal.getusepatterns(data->props)  == 1 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"%s multi aspect with patterns", id);
      if( matchaddr1 == addr && matchport1 == port ) {
        wSignal.setstate( data->props, __getPatternState( inst, 1, gate ^ 1 ) );
        update = True;
      }
      else if( matchaddr2 == addr && matchport2 == port ) {
        wSignal.setstate( data->props, __getPatternState( inst, 2, gate ^ 1 ) );
        update = True;
      }
      if( update )
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,"set %s to aspect %s", id, wSignal.getstate(data->props) );
    }


  }

  /* Broadcast to clients. */
  if( update ) {
    iONode nodeF = NodeOp.inst( wSignal.name(), NULL, ELEMENT_NODE );
    wSignal.setid( nodeF, wSignal.getid( data->props ) );
    wSignal.setstate( nodeF, wSignal.getstate( data->props ) );
    wSignal.setmanual( nodeF, wSignal.ismanual( data->props ) );
    if( wSignal.getiid( data->props ) != NULL )
      wSignal.setiid( nodeF, wSignal.getiid( data->props ) );
    AppOp.broadcastEvent( nodeF );
  }


  /* Cleanup Node3 */
  if( nodeC != NULL )
    NodeOp.base.del(nodeC);
}


static void _green( iOSignal inst ) {
  if( inst != NULL && !SignalOp.isManualOperated(inst) ) {
    iOSignalData data = Data(inst);
    iONode node = NodeOp.inst( wSignal.name(), NULL, ELEMENT_NODE );
    wSignal.setcmd( node, wSignal.green );
    wSignal.setid( node, SignalOp.getId( inst ) );
    SignalOp.cmd( inst, node, True );
  }
}

static void _yellow( iOSignal inst ) {
  if( inst != NULL && !SignalOp.isManualOperated(inst) ) {
    iOSignalData data = Data(inst);
    iONode node = NodeOp.inst( wSignal.name(), NULL, ELEMENT_NODE );
    if( wSignal.getaspects( data->props ) == 2 )
      wSignal.setcmd( node, wSignal.green );
    else
      wSignal.setcmd( node, wSignal.yellow );
    wSignal.setid( node, SignalOp.getId( inst ) );
    SignalOp.cmd( inst, node, True );
  }
}

static void _red( iOSignal inst ) {
  if( inst != NULL ) {
    iOSignalData data = Data(inst);
    iONode node = NodeOp.inst( wSignal.name(), NULL, ELEMENT_NODE );
    wSignal.setcmd( node, wSignal.red );
    wSignal.setid( node, SignalOp.getId( inst ) );
    SignalOp.cmd( inst, node, True );
  }
}

static void _white( iOSignal inst ) {
  if( inst != NULL && !SignalOp.isManualOperated(inst) ) {
    iOSignalData data = Data(inst);
    iONode node = NodeOp.inst( wSignal.name(), NULL, ELEMENT_NODE );
    if( wSignal.getaspects( data->props ) == 2 )
      wSignal.setcmd( node, wSignal.green );
    else if( wSignal.getaspects( data->props ) == 3 )
      wSignal.setcmd( node, wSignal.yellow );
    else
      wSignal.setcmd( node, wSignal.white );
    wSignal.setid( node, SignalOp.getId( inst ) );
    SignalOp.cmd( inst, node, True );
  }
}

static void _blank( iOSignal inst ) {
  if( inst != NULL && !SignalOp.isManualOperated(inst) ) {
    iOSignalData data = Data(inst);
    iONode node = NodeOp.inst( wSignal.name(), NULL, ELEMENT_NODE );
    wSignal.setcmd( node, wSignal.blank );
    wSignal.setid( node, SignalOp.getId( inst ) );
    SignalOp.cmd( inst, node, True );
  }
}


static Boolean __processPairCmd( iOSignal inst, const char* state, Boolean invert ) {
  iOSignalData o = Data(inst);
  iOControl control = AppOp.getControl(  );

  const char* iid = wSignal.getiid( o->props );

  iONode swcmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "Pair processing for signal [%s][%s]...", wSignal.getid( o->props ), state );

  if( iid != NULL )
    wSwitch.setiid( swcmd, iid );

  wSwitch.setbus( swcmd, wSignal.getbus( o->props ) );
  wSwitch.setprot( swcmd, wSignal.getprot( o->props ) );
  wSwitch.setactdelay( swcmd, False );

  /* reset */
  wSwitch.setaddr1( swcmd, wSignal.getaddr( o->props ) );
  wSwitch.setport1( swcmd, wSignal.getport1( o->props ) );
  wSwitch.setcmd( swcmd, invert?wSwitch.straight:wSwitch.turnout );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(swcmd), NULL );
	ThreadOp.sleep(wSignal.getcmdtime( o->props ));

  if( wSignal.getaddr2( o->props ) > 0 || wSignal.getport2( o->props ) > 0 ) {
    if( wSignal.getaddr2( o->props ) > 0 )
      wSwitch.setaddr1( swcmd, wSignal.getaddr2( o->props ) );
    wSwitch.setport1( swcmd, wSignal.getport2( o->props ) );
    ControlOp.cmd( control, (iONode)NodeOp.base.clone(swcmd), NULL );
  	ThreadOp.sleep(wSignal.getcmdtime( o->props ));
  }

  if( wSignal.getaddr3( o->props ) > 0 || wSignal.getport3( o->props ) > 0 ) {
    if( wSignal.getaddr3( o->props ) > 0 )
      wSwitch.setaddr1( swcmd, wSignal.getaddr3( o->props ) );
    wSwitch.setport1( swcmd, wSignal.getport3( o->props ) );
    ControlOp.cmd( control, (iONode)NodeOp.base.clone(swcmd), NULL );
	  ThreadOp.sleep(wSignal.getcmdtime( o->props ));
  }

  if( wSignal.getaddr4( o->props ) > 0 || wSignal.getport4( o->props ) > 0 ) {
    if( wSignal.getaddr4( o->props ) > 0 )
      wSwitch.setaddr1( swcmd, wSignal.getaddr4( o->props ) );
    wSwitch.setport1( swcmd, wSignal.getport4( o->props ) );
    ControlOp.cmd( control, (iONode)NodeOp.base.clone(swcmd), NULL );
  	ThreadOp.sleep(wSignal.getcmdtime( o->props ));
  }

  wSwitch.setcmd( swcmd, invert?wSwitch.turnout:wSwitch.straight );
  wSwitch.setaddr1( swcmd, wSignal.getaddr( o->props ) );

  if( StrOp.equals( state, wSignal.red ) ) {
    wSwitch.setport1( swcmd, wSignal.getport1( o->props ) );
  }
  else if( StrOp.equals( state, wSignal.yellow ) && wSignal.getaspects(o->props) > 2) {
    if( wSignal.getaddr2( o->props ) > 0 )
      wSwitch.setaddr1( swcmd, wSignal.getaddr2( o->props ) );
    wSwitch.setport1( swcmd, wSignal.getport2( o->props ) );
  }
  else if( StrOp.equals( state, wSignal.white ) && wSignal.getaspects(o->props) > 3 ) {
    if( wSignal.getaddr4( o->props ) > 0 )
      wSwitch.setaddr1( swcmd, wSignal.getaddr4( o->props ) );
    wSwitch.setport1( swcmd, wSignal.getport4( o->props ) );
  }
  else {
    if( wSignal.getaddr3( o->props ) > 0 )
      wSwitch.setaddr1( swcmd, wSignal.getaddr3( o->props ) );
    wSwitch.setport1( swcmd, wSignal.getport3( o->props ) );
  }
  return ControlOp.cmd( control, swcmd, NULL );

}


/**
 * A pattern is a gate bit mask where two ports are involved.
 *
 */
static Boolean __processPatternCmd( iOSignal inst, const char* state ) {
  iOSignalData o = Data(inst);
  iOControl control = AppOp.getControl(  );
  const char* iid = wSignal.getiid( o->props );
  int pattern1 = 0;
  int pattern2 = 0;
  int addr1 = 0;
  int port1 = 0;
  int gate1 = 0;
  int addr2 = 0;
  int port2 = 0;
  int gate2 = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "Pattern processing for signal [%s][%s]...", wSignal.getid( o->props ), state );

  if( control == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "control is not initialized" );
    return False;
  }

  if( StrOp.equals( state, wSignal.green ) ) {
    pattern1 = wSignal.getgreen( o->props ) & 0x0F;
    pattern2 = (wSignal.getgreen( o->props ) & 0xF0) >> 4;
  }
  else if( StrOp.equals( state, wSignal.red ) ) {
    pattern1 = wSignal.getred( o->props ) & 0x0F;
    pattern2 = (wSignal.getred( o->props ) & 0xF0) >> 4;
  }
  else if( StrOp.equals( state, wSignal.yellow ) ) {
    pattern1 = wSignal.getyellow( o->props ) & 0x0F;
    pattern2 = (wSignal.getyellow( o->props ) & 0xF0) >> 4;
  }
  else if( StrOp.equals( state, wSignal.white ) ) {
    pattern1 = wSignal.getwhite( o->props ) & 0x0F;
    pattern2 = (wSignal.getwhite( o->props ) & 0xF0) >> 4;
  }
  else if( StrOp.equals( state, wSignal.blank ) ) {
    pattern1 = wSignal.getblank( o->props ) & 0x0F;
    pattern2 = (wSignal.getblank( o->props ) & 0xF0) >> 4;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "pattern [%d][%d]", pattern1, pattern2 );

  switch( pattern1 ) {
    case 0:
      addr1 = wSignal.getaddr(o->props);
      port1 = wSignal.getport1(o->props);
      gate1 = 1;
      break;
    case 1:
      addr1 = wSignal.getaddr(o->props);
      port1 = wSignal.getport1(o->props);
      gate1 = 0;
      break;
    case 2:
      /* don't care; leave it as is */
      break;
  }

  switch( pattern2 ) {
    case 0:
      addr2 = wSignal.getaddr2(o->props);
      port2 = wSignal.getport2(o->props);
      gate2 = 1;
      break;
    case 1:
      addr2 = wSignal.getaddr2(o->props);
      port2 = wSignal.getport2(o->props);
      gate2 = 0;
      break;
    case 2:
      /* don't care; leave it as is */
      break;
  }

  if( addr1 != 0 || port1 != 0 ) {
    iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
    if( iid != NULL )
      wOutput.setiid( cmd, iid );
    wOutput.setprot( cmd, wSignal.getprot( o->props ) );
    wOutput.setaddr( cmd, addr1 );
    wOutput.setport( cmd, port1 );
    wOutput.setgate( cmd, gate1 );
    wOutput.setcmd ( cmd, wOutput.on);

    /* invoke the command by calling the control */
    if(  !ControlOp.cmd( control, cmd, NULL ) ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
          "Signal [%s] could not be set!", wSignal.getid( o->props ) );
      return False;
    }
  	ThreadOp.sleep(wSignal.getcmdtime( o->props ));
  }

  if( addr2 != 0 || port2 != 0 ) {
    iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
    if( iid != NULL )
      wOutput.setiid( cmd, iid );
    wOutput.setprot( cmd, wSignal.getprot( o->props ) );
    wOutput.setaddr( cmd, addr2 );
    wOutput.setport( cmd, port2 );
    wOutput.setgate( cmd, gate2 );
    wOutput.setcmd ( cmd, wOutput.on);

    /* invoke the command by calling the control */
    if( !ControlOp.cmd( control, cmd, NULL ) ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
          "Signal [%s] could not be set!", wSignal.getid( o->props ) );
      return False;
    }
  }

  return True;
}


static Boolean __process4AspectsCmd( iOSignal inst, const char* state ) {
  iOSignalData o = Data(inst);
  iOControl control = AppOp.getControl(  );
  const char* iid = wSignal.getiid( o->props );

  iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "4 aspects processing for signal [%s][%s]...", wSignal.getid( o->props ), state );

  /* reset all outputs */
  if( iid != NULL )
    wOutput.setiid( cmd, iid );

  wOutput.setbus( cmd, wSignal.getbus( o->props ) );

  wOutput.setprot( cmd, wSignal.getprot( o->props ) );
  wOutput.setcmd( cmd, wOutput.off );
  wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport1( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate1( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );
	ThreadOp.sleep(wSignal.getcmdtime( o->props ));

  if( wSignal.getaddr2( o->props ) > 0 )
    wOutput.setaddr( cmd, wSignal.getaddr2( o->props ) );
  else
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport2( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate2( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );
	ThreadOp.sleep(wSignal.getcmdtime( o->props ));

  if( wSignal.getaddr3( o->props ) > 0 )
    wOutput.setaddr( cmd, wSignal.getaddr3( o->props ) );
  else
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport3( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate3( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );
	ThreadOp.sleep(wSignal.getcmdtime( o->props ));


  if( wSignal.getaddr4( o->props ) > 0 )
    wOutput.setaddr( cmd, wSignal.getaddr4( o->props ) );
  else
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport4( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate4( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );
	ThreadOp.sleep(wSignal.getcmdtime( o->props ));


  wOutput.setcmd( cmd, wOutput.on );

  if( StrOp.equals( wSignal.white, state ) ) {
    if( wSignal.getaddr4( o->props ) > 0 )
      wOutput.setaddr( cmd, wSignal.getaddr4( o->props ) );
    else
      wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
    wOutput.setport( cmd, wSignal.getport4( o->props ) );
    wOutput.setgate( cmd, wSignal.getgate4( o->props ) );
  }
  else if( StrOp.equals( wSignal.green, state ) ) {
    if( wSignal.getaddr3( o->props ) > 0 )
      wOutput.setaddr( cmd, wSignal.getaddr3( o->props ) );
    else
      wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
    wOutput.setport( cmd, wSignal.getport3( o->props ) );
    wOutput.setgate( cmd, wSignal.getgate3( o->props ) );
  }
  else if( StrOp.equals( wSignal.yellow, state ) ) {
    if( wSignal.getaddr2( o->props ) > 0 )
      wOutput.setaddr( cmd, wSignal.getaddr2( o->props ) );
    else
      wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
    wOutput.setport( cmd, wSignal.getport2( o->props ) );
    wOutput.setgate( cmd, wSignal.getgate2( o->props ) );
  }
  else if( !StrOp.equals( wSignal.blank, state ) ) {
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
    wOutput.setport( cmd, wSignal.getport1( o->props ) );
    wOutput.setgate( cmd, wSignal.getgate1( o->props ) );
  }


  /* invoke the command by calling the control */
  if( !ControlOp.cmd( control, cmd, NULL ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
        "Signal [%s] could not be set!", wSignal.getid( o->props ) );
    return False;
  }

  return True;
}


static Boolean __process3AspectsCmd( iOSignal inst, const char* state ) {
  iOSignalData o = Data(inst);
  iOControl control = AppOp.getControl(  );
  const char* iid = wSignal.getiid( o->props );

  iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "3 aspects processing for signal [%s][%s]...", wSignal.getid( o->props ), state );

  /* reset all outputs */
  if( iid != NULL )
    wOutput.setiid( cmd, iid );

  wOutput.setbus( cmd, wSignal.getbus( o->props ) );

  wOutput.setprot( cmd, wSignal.getprot( o->props ) );
  wOutput.setcmd( cmd, wOutput.off );
  wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport1( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate1( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );
	ThreadOp.sleep(wSignal.getcmdtime( o->props ));

  if( wSignal.getaddr2( o->props ) > 0 )
    wOutput.setaddr( cmd, wSignal.getaddr2( o->props ) );
  else
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport2( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate2( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );
	ThreadOp.sleep(wSignal.getcmdtime( o->props ));

  if( wSignal.getaddr3( o->props ) > 0 )
    wOutput.setaddr( cmd, wSignal.getaddr3( o->props ) );
  else
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport3( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate3( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );
	ThreadOp.sleep(wSignal.getcmdtime( o->props ));


  wOutput.setcmd( cmd, wOutput.on );

  if( StrOp.equals( wSignal.green, state ) ) {
    if( wSignal.getaddr3( o->props ) > 0 )
      wOutput.setaddr( cmd, wSignal.getaddr3( o->props ) );
    else
      wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
    wOutput.setport( cmd, wSignal.getport3( o->props ) );
    wOutput.setgate( cmd, wSignal.getgate3( o->props ) );
  }
  else if( StrOp.equals( wSignal.yellow, state ) ) {
    if( wSignal.getaddr2( o->props ) > 0 )
      wOutput.setaddr( cmd, wSignal.getaddr2( o->props ) );
    else
      wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
    wOutput.setport( cmd, wSignal.getport2( o->props ) );
    wOutput.setgate( cmd, wSignal.getgate2( o->props ) );
  }
  else if( !StrOp.equals( wSignal.blank, state ) ) {
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
    wOutput.setport( cmd, wSignal.getport1( o->props ) );
    wOutput.setgate( cmd, wSignal.getgate1( o->props ) );
  }


  /* invoke the command by calling the control */
  if( !ControlOp.cmd( control, cmd, NULL ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
        "Signal [%s] could not be set!", wSignal.getid( o->props ) );
    return False;
  }

  return True;
}


static Boolean __process2AspectsCmd( iOSignal inst, const char* state ) {
  iOSignalData o = Data(inst);
  iOControl control = AppOp.getControl(  );
  const char* iid = wSignal.getiid( o->props );

  iONode cmd = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "2 aspects processing for signal [%s][%s]...", wSignal.getid( o->props ), state );

  /* reset all outputs */
  if( iid != NULL )
    wOutput.setiid( cmd, iid );

  wOutput.setbus( cmd, wSignal.getbus( o->props ) );

  wOutput.setprot( cmd, wSignal.getprot( o->props ) );
  wOutput.setcmd( cmd, wOutput.off );
  wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport1( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate1( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );
	ThreadOp.sleep(wSignal.getcmdtime( o->props ));

  if( wSignal.getaddr2( o->props ) > 0 )
    wOutput.setaddr( cmd, wSignal.getaddr2( o->props ) );
  else
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport2( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate2( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );
	ThreadOp.sleep(wSignal.getcmdtime( o->props ));

  wOutput.setcmd( cmd, wOutput.on );

  if( StrOp.equals( wSignal.green, state ) ) {
    if( wSignal.getaddr2( o->props ) > 0 )
      wOutput.setaddr( cmd, wSignal.getaddr2( o->props ) );
    else
      wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
    wOutput.setport( cmd, wSignal.getport2( o->props ) );
    wOutput.setgate( cmd, wSignal.getgate2( o->props ) );
  }
  else if( !StrOp.equals( wSignal.blank, state ) ) {
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
    wOutput.setport( cmd, wSignal.getport1( o->props ) );
    wOutput.setgate( cmd, wSignal.getgate1( o->props ) );
  }


  /* invoke the command by calling the control */
  if( !ControlOp.cmd( control, cmd, NULL ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
        "Signal [%s] could not be set!", wSignal.getid( o->props ) );
    return False;
  }

  return True;
}




static Boolean __processAspectNrCmd( iOSignal inst, const char* state ) {
  iOSignalData o = Data(inst);
  iOControl control = AppOp.getControl(  );
  const char* iid = wSignal.getiid( o->props );
  int aspect = 0;

  iONode cmd = NodeOp.inst( wSignal.name(), NULL, ELEMENT_NODE );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "aspect number processing for signal [%s][%s]...", wSignal.getid( o->props ), state );

  if( StrOp.equals( wSignal.green, state ) )
    aspect = wSignal.getgreennr(o->props);
  else if( StrOp.equals( wSignal.red, state ) )
    aspect = wSignal.getrednr(o->props);
  else if( StrOp.equals( wSignal.yellow, state ) )
    aspect = wSignal.getyellownr(o->props);
  else if( StrOp.equals( wSignal.white, state ) )
    aspect = wSignal.getwhitenr(o->props);
  else if( StrOp.equals( wSignal.blank, state ) )
    aspect = wSignal.getblanknr(o->props);

  /* reset all outputs */
  if( iid != NULL )
    wSignal.setiid( cmd, iid );

  wSignal.setbus( cmd, wSignal.getbus( o->props ) );

  wSignal.setprot( cmd, wSignal.getprot( o->props ) );
  wSignal.setcmd( cmd, wSignal.aspect );
  wSignal.setaspect( cmd, aspect );
  wSignal.setaddr( cmd, wSignal.getaddr( o->props ) );
  wSignal.setport1( cmd, wSignal.getport1( o->props ) );
  wSignal.setgate1( cmd, wSignal.getgate1( o->props ) );

  /* invoke the command by calling the control */
  if( !ControlOp.cmd( control, cmd, NULL ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
        "Signal [%s] could not be set!", wSignal.getid( o->props ) );
    return False;
  }

  return True;
}




static Boolean __process2AspectsAsSwitchCmd( iOSignal inst, const char* state ) {
  iOSignalData o = Data(inst);
  iOControl control = AppOp.getControl(  );
  const char* iid = wSignal.getiid( o->props );
  int gate = 0;

  iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "2 aspects processing as switch for signal [%s][%s]...", wSignal.getid( o->props ), state );

  if( iid != NULL )
    wSwitch.setiid( cmd, iid );

  wSwitch.setprot( cmd, wSignal.getprot( o->props ) );
  wSwitch.setcmd( cmd, wOutput.on );
  wSwitch.setbus( cmd, wSignal.getbus( o->props ) );
  wSwitch.setaddr1( cmd, wSignal.getaddr( o->props ) );
  wSwitch.setport1( cmd, wSignal.getport1( o->props ) );

  wSwitch.setcmd( cmd, StrOp.equals( state, wSignal.green ) ? wSwitch.turnout:wSwitch.straight );

  /* invoke the command by calling the control */
  if( !ControlOp.cmd( control, cmd, NULL ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
        "Signal [%s] could not be set!", wSignal.getid( o->props ) );
    return False;
  }

  return True;
}


static void __checkAction( iOSignal inst ) {

  iOSignalData data     = Data(inst);
  iOModel      model    = AppOp.getModel();
  iONode       sgaction = wSignal.getactionctrl( data->props );
  iIBlockBase  bk       = NULL;

  while( sgaction != NULL) {
      if( StrOp.len( wActionCtrl.getstate(sgaction) ) == 0 ||
          StrOp.equals(wActionCtrl.getstate(sgaction), wSignal.getstate(data->props) ) )
      {

        iOAction action = ModelOp.getAction( AppOp.getModel(), wActionCtrl.getid( sgaction ));
        if( action != NULL ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "signal action: %s", wActionCtrl.getid( sgaction ));
          bk = ModelOp.getBlock4Signal(model, SignalOp.getId(inst));

          if( bk != NULL ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "signal in block %s with loco %s", bk->base.id(bk), bk->getLoc(bk) );
            wActionCtrl.setlcid(sgaction, bk->getLoc(bk) );
          }

          ActionOp.exec(action, sgaction);
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action state does not match: [%s-%s]",
            wActionCtrl.getstate( sgaction ), wSignal.getstate(data->props) );
      }
    sgaction = wSignal.nextactionctrl( data->props, sgaction );
  }

}


static Boolean _cmd( iOSignal inst, iONode nodeA, Boolean update ) {
  iOSignalData o = Data(inst);
  iOControl control = AppOp.getControl(  );
  Boolean ok = True;

  const char* state      = wSignal.getcmd( nodeA );
  const char* iid        = wSignal.getiid( o->props );
  const char* savedState = wSignal.getstate( o->props );
  Boolean     inv        = wSignal.isinv( o->props );
  Boolean     chgState   = True;


  if( control == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "control is not initialized" );
    return False;
  }

  /* flip the signal for manual mode */
  if( StrOp.equals( wSignal.flip, state ) ) {
    if( StrOp.equals( savedState, wSignal.green ) ) {
      if( wSignal.getaspects( o->props ) == 4 ) {
        state = wSignal.white;
      }
      else if( wSignal.getaspects( o->props ) == 3 ) {
        state = wSignal.yellow;
      }
      else
        state = wSignal.red;
    }
    else if( StrOp.equals( savedState, wSignal.yellow ) ) {
      state = wSignal.red;
    }
    else if( StrOp.equals( savedState, wSignal.white ) ) {
      state = wSignal.yellow;
    }
    else if( StrOp.equals( savedState, wSignal.blank ) ) {
      state = wSignal.blank;
    }
    else {
      state = wSignal.green;
    }
  }
  else if( StrOp.equals( wSignal.autooperated, state ) ) {
    wSignal.setmanual( o->props, False);
    chgState = False;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting signal %s to auto (%s)",
                 wSignal.getid( o->props ), state );
  }
  else if( StrOp.equals( wSignal.manualoperated, state ) ) {
    wSignal.setmanual( o->props, True);
    chgState = False;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting signal %s to manual (%s)",
                 wSignal.getid( o->props ), state );
  }

  if( !NodeOp.getBool(nodeA, "force", False ) ) {
    if( StrOp.equals(wSignal.getstate( o->props ), state) && wCtrl.isskipsetsg( wRocRail.getctrl(AppOp.getIni())) ) {
      chgState = False;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "skipping signal[%s] command: no change of state[%s].",
          wSignal.getid( o->props ), state);
    }
  }

  /* save the new state of the signal */
  if( chgState ) {
    Boolean hasAddr =  (wSignal.getaddr(o->props) == 0 && wSignal.getport1(o->props) == 0 ) ? False:True;
    if( StrOp.equals(wSignal.red, state ) )
      wSignal.setstate( o->props, wSignal.red );
    else if( StrOp.equals(wSignal.blank, state ) )
      wSignal.setstate( o->props, wSignal.blank );
    else if( StrOp.equals(wSignal.green, state ) )
      wSignal.setstate( o->props, wSignal.green );
    else if( StrOp.equals(wSignal.yellow, state ) )
      wSignal.setstate( o->props, wSignal.yellow );
    else if( StrOp.equals(wSignal.white, state ) )
      wSignal.setstate( o->props, wSignal.white );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting signal %s to %s",
                 wSignal.getid( o->props ), wSignal.getstate( o->props ) );

    /* invert only 2 aspect signals */
    if( inv && wSignal.getaspects( o->props ) == 2 ) {
      if( inv && StrOp.equals( wSignal.red, state ) )
        state = wSignal.green;
      else if( inv && StrOp.equals( wSignal.green, state ) )
        state = wSignal.red;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "inverting signal %s to %s",
                   wSignal.getid( o->props ), state );
    }

    /* check using patterns previous type (backwards compatibility) */
    if( NodeOp.getBool( o->props, "usepatterns", False ) ) {
      wSignal.setusepatterns( o->props, wSignal.use_patterns );
    }

    /* check using patterns */
    if( hasAddr && wSignal.getusepatterns( o->props ) == wSignal.use_patterns ) {
      if( !__processPatternCmd( inst, state ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Signal [%s] could not be set!", wSignal.getid( o->props ) );
        ok = False;
      }
    }
    else if( hasAddr && wSignal.getusepatterns( o->props ) == wSignal.use_aspectnrs ) {
      if( !__processAspectNrCmd( inst, state ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Signal [%s] could not be set!", wSignal.getid( o->props ) );
        ok = False;
      }
    }
    /* pair processing */
    else if( hasAddr && wSignal.ispair( o->props ) ) {
      if( !__processPairCmd( inst, state, inv ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Signal [%s] could not be set!", wSignal.getid( o->props ) );
        ok = False;
      }
    }
    else if( hasAddr && wSignal.getaspects(o->props) == 4 ){
      /* invoke the command by calling the control */
      if( !__process4AspectsCmd( inst, state ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Signal [%s] could not be set!", wSignal.getid( o->props ) );
        ok = False;
      }
    }
    else if( hasAddr && wSignal.getaspects(o->props) == 3 ){
      /* invoke the command by calling the control */
      if( !__process3AspectsCmd( inst, state ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Signal [%s] could not be set!", wSignal.getid( o->props ) );
        ok = False;
      }
    }
    else if( hasAddr && wSignal.isasswitch(o->props) ) {
      /* invoke the command by calling the control */
      if( !__process2AspectsAsSwitchCmd( inst, state ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Signal [%s] could not be set!", wSignal.getid( o->props ) );
        ok = False;
      }
    }
    else if(hasAddr){
      /* invoke the command by calling the control */
      if( !__process2AspectsCmd( inst, state ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Signal [%s] could not be set!", wSignal.getid( o->props ) );
        ok = False;
      }
    }

    if( ok )
      __checkAction( inst );
  }

  /* Broadcast to clients. Node6 */
  if( ok && update ) {
    iONode nodeF = NodeOp.inst( wSignal.name(), NULL, ELEMENT_NODE );
    wSignal.setid( nodeF, wSignal.getid( o->props ) );
    wSignal.setstate( nodeF, wSignal.getstate( o->props ) );
    wSignal.setmanual( nodeF, wSignal.ismanual( o->props ) );
    if( wSignal.getiid( o->props ) != NULL )
      wSignal.setiid( nodeF, wSignal.getiid( o->props ) );
    AppOp.broadcastEvent( nodeF );
  }

  /* remove the signal command */
  NodeOp.base.del(nodeA);
  nodeA = NULL;

  return ok;
}


/**
 * Checks for property changes.
 * todo: Range checking?
 */
static void _modify( iOSignal inst, iONode props ) {
  iOSignalData o = Data(inst);
  int cnt = NodeOp.getAttrCnt( props );
  Boolean move = StrOp.equals( wModelCmd.getcmd( props ), wModelCmd.move );

  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );

    if( StrOp.equals("id", name) && StrOp.equals( value, wSignal.getid(o->props) ) )
      continue; /* skip to avoid making invalid pointers */

    NodeOp.setStr( o->props, name, value );
  }

  if( !move ) {
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
  else {
    NodeOp.removeAttrByName(o->props, "cmd");
  }

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( o->props );
    AppOp.broadcastEvent( clone );
  }
  props->base.del(props);
}

static const char* _tableHdr(void) {
  return "<tr bgcolor=\"#CCCCCC\"><th>SgID</th><th>Unit</th><th>Pin</th><th>type</th><th>x</th><th>y</th><th>z</th></tr>\n";
}

static char* _toHtml( void* inst ) {
  iOSignalData data = Data((iOSignal)inst);
  return StrOp.fmt( "<tr><td>%s</td><td align=\"right\">%d</td><td align=\"right\">%d</td><td>%s</td><td align=\"right\">%d</td><td align=\"right\">%d</td><td align=\"right\">%d</td></tr>\n",
      NodeOp.getStr( data->props, "id", "?" ),
      NodeOp.getInt( data->props, "unit", 0 ),
      NodeOp.getInt( data->props, "pin", 0 ),
      NodeOp.getStr( data->props, "type", "?" ),
      NodeOp.getInt( data->props, "x", 0 ),
      NodeOp.getInt( data->props, "y", 0 ),
      NodeOp.getInt( data->props, "z", 0 )
      );
}

static char* _getForm( void* inst ) {
  iOSignalData data = Data((iOSignal)inst);
  return NULL;
}


static char* _postForm( void* inst, const char* postdata ) {
  iOSignalData data = Data((iOSignal)inst);
  char* reply = StrOp.fmt( "Thanks!<br>" );
  return reply;
}

static iOSignal _inst( iONode props ) {
  iOSignal     sg   = allocMem( sizeof( struct OSignal ) );
  iOSignalData data = allocMem( sizeof( struct OSignalData ) );

  /* OBase operations */
  MemOp.basecpy( sg, &SignalOp, 0, sizeof( struct OSignal ), data );

  data->props = props;
  if( wSignal.getstate( data->props ) == NULL || StrOp.len(wSignal.getstate( data->props )) == 0) {
    wSignal.setstate( data->props, wSignal.red );
  }

  NodeOp.removeAttrByName(data->props, "cmd");

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "inst[%d] for %s", instCnt, _getId(sg) );

  instCnt++;

  return sg;
}


static Boolean _isState( iOSignal inst, const char* state ) {
  iOSignalData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "signal [%s] is %s (%s)",
      _getId(inst), wSignal.getstate(data->props), state );
  return StrOp.equals( state, wSignal.getstate(data->props) );
}


static Boolean _isManualOperated( iOSignal inst ) {
  iOSignalData data = Data(inst);
  return wSignal.ismanual(data->props);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/signal.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
