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

#include "rocrail/impl/signal_impl.h"
#include "rocrail/public/app.h"

#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Ctrl.h"

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

static void* _getProperties( void* inst ) {
  iOSignalData data = Data((iOSignal)inst);
  return data->props;
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
  if( inst != NULL && !SignalOp.isManualOperated(inst) ) {
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
    wSignal.setcmd( node, wSignal.white );
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

  wSwitch.setprot( swcmd, wSignal.getprot( o->props ) );
  wSwitch.setactdelay( swcmd, False );

  /* reset */
  wSwitch.setaddr1( swcmd, wSignal.getaddr( o->props ) );
  wSwitch.setport1( swcmd, wSignal.getport1( o->props ) );
  wSwitch.setcmd( swcmd, invert?wSwitch.straight:wSwitch.turnout );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(swcmd), NULL );

  if( wSignal.getaddr2( o->props ) > 0 || wSignal.getport2( o->props ) > 0 ) {
    if( wSignal.getaddr2( o->props ) > 0 )
      wSwitch.setaddr1( swcmd, wSignal.getaddr2( o->props ) );
    wSwitch.setport1( swcmd, wSignal.getport2( o->props ) );
    ControlOp.cmd( control, (iONode)NodeOp.base.clone(swcmd), NULL );
  }

  if( wSignal.getaddr3( o->props ) > 0 || wSignal.getport3( o->props ) > 0 ) {
    if( wSignal.getaddr3( o->props ) > 0 )
      wSwitch.setaddr1( swcmd, wSignal.getaddr3( o->props ) );
    wSwitch.setport1( swcmd, wSignal.getport3( o->props ) );
    ControlOp.cmd( control, (iONode)NodeOp.base.clone(swcmd), NULL );
  }

  if( wSignal.getaddr4( o->props ) > 0 || wSignal.getport4( o->props ) > 0 ) {
    if( wSignal.getaddr4( o->props ) > 0 )
      wSwitch.setaddr1( swcmd, wSignal.getaddr4( o->props ) );
    wSwitch.setport1( swcmd, wSignal.getport4( o->props ) );
    ControlOp.cmd( control, (iONode)NodeOp.base.clone(swcmd), NULL );
  }

  wSwitch.setcmd( swcmd, invert?wSwitch.turnout:wSwitch.straight );

  if( StrOp.equals( state, wSignal.red ) ) {
    wSwitch.setport1( swcmd, wSignal.getport1( o->props ) );
  }
  else if( StrOp.equals( state, wSignal.yellow ) ) {
    wSwitch.setport1( swcmd, wSignal.getport2( o->props ) );
  }
  else if( StrOp.equals( state, wSignal.green ) ) {
    wSwitch.setport1( swcmd, wSignal.getport3( o->props ) );
  }
  else if( StrOp.equals( state, wSignal.white ) ) {
    wSwitch.setport1( swcmd, wSignal.getport4( o->props ) );
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

  wOutput.setprot( cmd, wSignal.getprot( o->props ) );
  wOutput.setcmd( cmd, wOutput.off );
  wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport1( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate1( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );

  if( wSignal.getaddr2( o->props ) > 0 )
    wOutput.setaddr( cmd, wSignal.getaddr2( o->props ) );
  else
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport2( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate2( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );

  if( wSignal.getaddr3( o->props ) > 0 )
    wOutput.setaddr( cmd, wSignal.getaddr3( o->props ) );
  else
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport3( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate3( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );


  if( wSignal.getaddr4( o->props ) > 0 )
    wOutput.setaddr( cmd, wSignal.getaddr4( o->props ) );
  else
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport4( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate4( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );


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
  else {
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

  wOutput.setprot( cmd, wSignal.getprot( o->props ) );
  wOutput.setcmd( cmd, wOutput.off );
  wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport1( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate1( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );

  if( wSignal.getaddr2( o->props ) > 0 )
    wOutput.setaddr( cmd, wSignal.getaddr2( o->props ) );
  else
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport2( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate2( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );

  if( wSignal.getaddr3( o->props ) > 0 )
    wOutput.setaddr( cmd, wSignal.getaddr3( o->props ) );
  else
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport3( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate3( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );


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
  else {
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

  wOutput.setprot( cmd, wSignal.getprot( o->props ) );
  wOutput.setcmd( cmd, wOutput.off );
  wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport1( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate1( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );

  if( wSignal.getaddr2( o->props ) > 0 )
    wOutput.setaddr( cmd, wSignal.getaddr2( o->props ) );
  else
    wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
  wOutput.setport( cmd, wSignal.getport2( o->props ) );
  wOutput.setgate( cmd, wSignal.getgate2( o->props ) );
  ControlOp.cmd( control, (iONode)NodeOp.base.clone(cmd), NULL );

  wOutput.setcmd( cmd, wOutput.on );

  if( StrOp.equals( wSignal.green, state ) ) {
    if( wSignal.getaddr2( o->props ) > 0 )
      wOutput.setaddr( cmd, wSignal.getaddr2( o->props ) );
    else
      wOutput.setaddr( cmd, wSignal.getaddr( o->props ) );
    wOutput.setport( cmd, wSignal.getport2( o->props ) );
    wOutput.setgate( cmd, wSignal.getgate2( o->props ) );
  }
  else {
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

  while( sgaction != NULL) {
    if( ModelOp.isAuto(model) == wActionCtrl.isauto(sgaction) ) {

      if( StrOp.len( wActionCtrl.getstate(sgaction) ) == 0 ||
          StrOp.equals(wActionCtrl.getstate(sgaction), wSignal.getstate(data->props) ) )
      {

        iOAction action = ModelOp.getAction( AppOp.getModel(), wActionCtrl.getid( sgaction ));
        if( action != NULL ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "signal action: %s", wActionCtrl.getid( sgaction ));
          ActionOp.exec(action, sgaction);
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action state does not match: [%s-%s]",
            wActionCtrl.getstate( sgaction ), wSignal.getstate(data->props) );
      }

    }
    sgaction = wSignal.nextactionctrl( data->props, sgaction );
  } /* end loop */

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
    else {
      state = wSignal.green;
    }
  }
  else if( StrOp.equals( wSignal.autooperated, state ) ) {
    wSignal.setmanual( o->props, False);
    chgState = False;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting signal %s to auto",
                 wSignal.getid( o->props ) );
  }
  else if( StrOp.equals( wSignal.manualoperated, state ) ) {
    wSignal.setmanual( o->props, True);
    chgState = False;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting signal %s to manual",
                 wSignal.getid( o->props ) );
  }

  /* save the new state of the signal */
  if( chgState ) {
    wSignal.setstate( o->props, state );
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

    /* check using patterns */
    if( wSignal.isusepatterns( o->props ) ) {
      if( !__processPatternCmd( inst, state ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Signal [%s] could not be set!", wSignal.getid( o->props ) );
        ok = False;
      }
    }
    /* pair processing */
    else if( wSignal.ispair( o->props ) ) {
      if( !__processPairCmd( inst, state, inv ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Signal [%s] could not be set!", wSignal.getid( o->props ) );
        ok = False;
      }
    }
    else if( wSignal.getaspects(o->props) == 4 ){
      /* invoke the command by calling the control */
      if( !__process4AspectsCmd( inst, state ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Signal [%s] could not be set!", wSignal.getid( o->props ) );
        ok = False;
      }
    }
    else if( wSignal.getaspects(o->props) == 3 ){
      /* invoke the command by calling the control */
      if( !__process3AspectsCmd( inst, state ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Signal [%s] could not be set!", wSignal.getid( o->props ) );
        ok = False;
      }
    }
    else if( wSignal.isasswitch(o->props) ) {
      /* invoke the command by calling the control */
      if( !__process2AspectsAsSwitchCmd( inst, state ) ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "Signal [%s] could not be set!", wSignal.getid( o->props ) );
        ok = False;
      }
    }
    else {
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
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeF );
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

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( o->props );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), clone );
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
