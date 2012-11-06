
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

#include "rocdigs/impl/virtual_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"

#include "rocutils/public/addr.h"

static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOVirtualData data = Data(inst);
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
  return NULL;
}

/** ----- OVirtual ----- */

static void __inform( iOVirtual inst ) {
  iOVirtualData data = Data(inst);
  iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
  wState.setiid( node, wDigInt.getiid( data->ini ) );
  wState.setpower( node, data->power );
  wState.settrackbus( node, True );
  data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
}

/**  */
static void _halt( obj inst, Boolean poweroff ) {
  iOVirtualData data = Data(inst);
  data->run = False;
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOVirtualData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


static Boolean _setRawListener(obj inst, obj listenerObj, const digint_rawlistener listenerFun ) {
  return True;
}

static byte* _cmdRaw( obj inst, const byte* cmd ) {
  return NULL;
}

/**  */
static Boolean _supportPT( obj inst ) {
  /* ProgrammingTrack (ServiceMode) */
  return True;
}


static Boolean __sendRequest( iOVirtual virtual, byte* outin ) {
  return True;
}

/** */
static iONode __translate( iOVirtual virtual, iONode node ) {
  iOVirtualData data = Data(virtual);
  //iONode rsp = (iONode)NodeOp.base.clone(node);
  iONode rsp = NULL;

  /*
  char* cmdStr = NodeOp.base.toString(node);
  TraceOp.println( cmdStr );
  StrOp.free(cmdStr);
  */
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "translate: %s", NodeOp.getName( node ));


  /* Switch command. */
  if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {

    int bus  = wSwitch.getbus( node );
    int addr = wSwitch.getaddr1( node );
    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );
    int delay = wSwitch.getdelay(node);
    int fada = 0;
    int pada = 0;
    int dir  = 1;
    int action = 1;

    if( port == 0 ) {
      fada = addr;
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    }
    else if( addr == 0 && port > 0 ) {
      pada = port;
      AddrOp.fromPADA( port, &addr, &port );
    }

    if( fada == 0 )
      fada = AddrOp.toFADA( addr, port, gate );
    if( pada == 0 )
      pada = AddrOp.toPADA( addr, port );

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      dir = 0; /* thrown */

    if( wSwitch.issinglegate( node ) ) {
      dir = gate;
      if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.straight ) )
        action = 0;
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "turnout %04d %d %-10.10s fada=%04d pada=%04d bus=%d addr=%d port=%d gate=%d dir=%d action=%d delay=%d",
        addr, port, wSwitch.getcmd( node ), fada, pada, bus, addr, port, gate, dir, action, delay );

    {
      iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );

      if( port == 0 && addr > 0 )
        AddrOp.fromFADA( addr, &addr, &port, &gate );
      else if( addr == 0 && port > 0 )
        AddrOp.fromPADA( port, &addr, &port );
      wSwitch.setaddr1( nodeC, addr );
      wSwitch.setport1( nodeC, port );

      if( wSwitch.getiid(node) != NULL )
        wSwitch.setiid( nodeC, wSwitch.getiid(node) );

      wSwitch.setstate( nodeC, wSwitch.getcmd( node ) );

      data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
    }




  }
  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {

    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    int fada = 0;
    int pada = 0;

    if( port == 0 ) {
      fada = addr;
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    }
    else if( addr == 0 && port > 0 ) {
      pada = port;
      AddrOp.fromPADA( port, &addr, &port );
    }

    if( fada == 0 )
      fada = AddrOp.toFADA( addr, port, gate );
    if( pada == 0 )
      pada = AddrOp.toPADA( addr, port );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d %d %d %s fada=%d pada=%d",
        addr, port, gate, wOutput.getcmd(node)!=NULL?wOutput.getcmd(node):"-", fada, pada );
  }
  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    if( StrOp.equals( wSignal.aspect, wSignal.getcmd(node) ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "setting signal aspect to %d", wSignal.getaspect(node) );
    }
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    if( wFeedback.isactivelow(node) )
      wFeedback.setstate( node, !state);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );

    if( wDigInt.isfbreset( data->ini ) && state ) {
      iQCmd cmd = allocMem(sizeof(struct QCmd));
      cmd->time  = SystemOp.getTick();
      cmd->delay = wDigInt.getpsleep( data->ini ) / 10; /* ms -> ticks */
      cmd->node  = (iONode)NodeOp.base.clone(node);
      wFeedback.setstate( cmd->node, !state );
      ThreadOp.post( data->transactor, (obj)cmd );
    }

  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    Boolean fn = wLoc.isfn( node );
    int    dir = wLoc.isdir( node );
    int  spcnt = wLoc.getspcnt( node );

    if( StrOp.equals( wLoc.shortid, wLoc.getcmd(node) ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "shortID=[%s]", wLoc.getshortid(node) );
    }
    else {
      if( wLoc.getV( node ) != -1 ) {
        if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
          speed = (wLoc.getV( node ) * spcnt) / 100;
        else if( wLoc.getV_max( node ) > 0 )
          speed = (wLoc.getV( node ) * spcnt) / wLoc.getV_max( node );
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "speed=%d", speed );
      }
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "addr=%d V=%d(%d) dir=%s lights=%s throttleid=%s",
          addr, speed, wLoc.getV( node ), wLoc.isdir( node )?"fwd":"rev", fn?"on":"off", wLoc.getthrottleid(node) );
    }


  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );
    Boolean f0 = wFunCmd.isf0( node );
    Boolean f1 = wFunCmd.isf1( node );
    Boolean f2 = wFunCmd.isf2( node );
    Boolean f3 = wFunCmd.isf3( node );
    Boolean f4 = wFunCmd.isf4( node );
    Boolean f5 = wFunCmd.isf5( node );
    Boolean f6 = wFunCmd.isf6( node );
    Boolean f7 = wFunCmd.isf7( node );
    Boolean f8 = wFunCmd.isf8( node );
    Boolean f9  = wFunCmd.isf9( node );
    Boolean f10 = wFunCmd.isf10( node );
    Boolean f11 = wFunCmd.isf11( node );
    Boolean f12 = wFunCmd.isf12( node );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
            "decoder %d fc=%d f0=%s f1=%s f2=%s f3=%s f4=%s f5=%s f6=%s f7=%s f8=%s f9=%s f10=%s f11=%s f12=%s",
            addr, wFunCmd.getfnchanged(node) , f0?"on":"off", f1?"on":"off", f2?"on":"off", f3?"on":"off", f4?"on":"off",
            f5?"on":"off", f6?"on":"off", f7?"on":"off", f8?"on":"off",
            f9?"on":"off", f10?"on":"off", f11?"on":"off", f12?"on":"off" );
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      data->power = False;
      __inform(virtual);
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      data->power = True;
      __inform(virtual);
    }
    else if( StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Emergency break" );
    }
    else if( StrOp.equals( cmd, wSysCmd.txshortids ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "send short IDs to the throttle" );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "syscmd=%s", cmd );
    }

  }
  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "program cmd=%d mode=%d", wProgram.getcmd( node ), wProgram.getmode( node ) );

    if( wProgram.getcmd( node ) == wProgram.get ) {
      int cv = wProgram.getcv( node );
      if( cv < 1024 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get CV%d...", cv );
        rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
        wProgram.setcv( rsp, cv );
        wProgram.setvalue( rsp, data->cvval[cv] );
        wProgram.setcmd( rsp, wProgram.datarsp );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "out of range CV%d...", cv );
      }
    }
    else if( wProgram.getcmd( node ) == wProgram.set ) {
      int cv = wProgram.getcv( node );
      if( cv < 1024 ) {
        int value = wProgram.getvalue( node );
        int decaddr = wProgram.getdecaddr( node );
        data->cvval[cv] = value;

        // POM ?
        if( wProgram.ispom(node) ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "POM: set CV%d of loc %d to %d...", cv, decaddr, value );

          if ( cv == 0 )
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "POM does not allow writing of adress!");

        } else {

          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set CV%d to %d...", cv, value );
          rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
          wProgram.setcv( rsp, cv );
          wProgram.setvalue( rsp, value );
          wProgram.setcmd( rsp, wProgram.statusrsp );


        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "out of range CV%d...", cv );
      }
    }
    else if(  wProgram.getcmd( node ) == wProgram.pton ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "PT ON");
      rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      wProgram.setcmd( rsp, wProgram.statusrsp );
    }  // PT off, send: All ON"
    else if( wProgram.getcmd( node ) == wProgram.ptoff ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "PT OFF");
      rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      wProgram.setcmd( rsp, wProgram.statusrsp );
    }

    else if( wProgram.getcmd( node ) == wProgram.lncvset ) {
      int cv = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int addr = wProgram.getaddr( node );

      if( wProgram.getlntype(node) == wProgram.lntype_mp ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MultiPort addr=%d mask=0x%04X value=%d", addr, cv, value);
      }
    }

    else if( wProgram.getcmd( node ) == wProgram.lncvget ) {
      int cv = wProgram.getcv( node );
      int addr = wProgram.getaddr( node );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lncvget addr=%d cv=%d", addr, cv);
    }


  }
  return rsp;
}


static void __transactor( void* threadinst ) {
  iOThread      th   = (iOThread)threadinst;
  iOVirtual     vcs  = (iOVirtual)ThreadOp.getParm(th);
  iOVirtualData data = Data(vcs);
  iOList list = ListOp.inst();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Transactor is started.");

  do {
    iQCmd cmd = (iQCmd)ThreadOp.getPost( th );
    if (cmd != NULL) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "new timed command time=%d delay=%d tick=%d", cmd->time, cmd->delay, SystemOp.getTick() );
      ListOp.add(list, (obj)cmd);
    }

    int i = 0;
    for( i = 0; i < ListOp.size(list); i++ ) {
      iQCmd cmd = (iQCmd)ListOp.get(list, i);
      if( (cmd->time + cmd->delay) <= SystemOp.getTick() ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "timed command" );

        /* Sensor command. */
        if( StrOp.equals( NodeOp.getName( cmd->node ), wFeedback.name() ) ) {
          int addr = wFeedback.getaddr( cmd->node );
          Boolean state = wFeedback.isstate( cmd->node );

          if( wFeedback.isactivelow(cmd->node) )
            wFeedback.setstate( cmd->node, !state);

          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
          data->listenerFun( data->listenerObj, (iONode)NodeOp.base.clone( cmd->node ), TRCLEVEL_INFO );
        }


        NodeOp.base.del(cmd->node);
        ListOp.removeObj(list, (obj)cmd);
        freeMem(cmd);
        break;
      }
    }

    // Give up timeslize:
    ThreadOp.sleep( 10 );

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Transactor has stopped.");
}

static iONode _cmd( obj inst ,const iONode nodeA ) {
  iOVirtualData data = Data(inst);
  char out[256];
  iONode nodeB = NULL;

  if( nodeA != NULL ) {
    nodeB = __translate( (iOVirtual)inst, nodeA );
    data->listenerFun( data->listenerObj, nodeB, TRCLEVEL_INFO );
    nodeB = NULL;
    /* Cleanup Node1 */
    nodeA->base.del(nodeA);
  }

  /* return Node2 */
  return nodeB;
}


/* Status */
static int _state( obj inst ) {
  iOVirtualData data = Data(inst);
  int state = 0;
  state |= data->power << 0;
  return state;
}

/* external shortcut event */
static void _shortcut(obj inst) {
  iOVirtualData data = Data( inst );
}


/* Generate a fload of monitor messages to test the clients. */
static void __stressRunner( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOVirtual virt = (iOVirtual)ThreadOp.getParm( th );
  iOVirtualData data = Data(virt);
  int cnt = 0;

  ThreadOp.sleep(5000);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Virtual stress runner started." );

  /* try to get the system status: */
  while( data->run ) {
    cnt++;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "stress with monitor messages, run %d", cnt);
    ThreadOp.sleep(5);
  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Virtual stress runner ended." );
}




/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOVirtualData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

/**  */
static struct OVirtual* _inst( const iONode ini ,const iOTrace trc ) {
  iOVirtual __Virtual = allocMem( sizeof( struct OVirtual ) );
  iOVirtualData data = allocMem( sizeof( struct OVirtualData ) );
  MemOp.basecpy( __Virtual, &VirtualOp, 0, sizeof( struct OVirtual ), data );

  TraceOp.set( trc );
  SystemOp.inst();

  /* Initialize data->xxx members... */
  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->fbmod  = wDigInt.getfbmod( ini );
  data->readfb = wDigInt.isreadfb( ini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "virtual %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->run = True;


  data->transactor = ThreadOp.inst( data->iid, &__transactor, __Virtual );
  ThreadOp.start( data->transactor );

  if( wDigInt.isstress(ini) ) {
    iOThread stressRunner = ThreadOp.inst( "virtstress", &__stressRunner, __Virtual );
    ThreadOp.start( stressRunner );
  }


  instCnt++;
  return __Virtual;
}

/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/virtual.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
