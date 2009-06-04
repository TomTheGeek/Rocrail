/*
 Rocrail - Model Railroad Software

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
#include "rocdigs/impl/ddx_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/system.h"
#include "rocs/public/trace.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/DDX.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"

#include "rocdigs/impl/ddx/s88.h"
#include "rocdigs/impl/ddx/nmra.h"

#include "rocdigs/impl/common/fada.h"

/**
 * ddx attributes:
 */
static int instCnt = 0;
//static iODDX __inst = NULL;

void rocrail_ddxStateChanged(obj inst);

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iODDXData data = Data(inst);
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
  iODDXData data = Data(inst);
  return NULL;
}

/** ----- ODDX ----- */
void thr_delayedAccCmd(void *threadinst) {
  iOThread th = (iOThread)threadinst;
  obj v = ThreadOp.getParm( th );
  tDelayedAccCmd *cmd = (tDelayedAccCmd *)v;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
     "Delayed AccCmd received. delay: %d, protocol: %c\n",cmd->delay, cmd->acc_p);

  ThreadOp.sleep(cmd->delay);

  switch (cmd->acc_p) {
  case 'M':
    comp_maerklin_ms(cmd->acc_addr, cmd->acc_port, cmd->acc_gate, cmd->action);
    break;
  case 'N':
    comp_nmra_accessory(cmd->acc_addr, cmd->acc_port, cmd->acc_gate, cmd->action);
    break;
  default:
    return;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Delayed AccCmd sent.\n");
  freeMem(cmd);
  return;
}

static int __accessory(obj inst, int addr, int port, int gate, int action, const char* prot) {
  iODDXData data = Data((iODDX)inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "addr=%d", addr );
  if( StrOp.equals( wSignal.prot_M, prot ) && data->mma )
    return comp_maerklin_ms( addr, port, gate, action );
  if( StrOp.equals( wSignal.prot_N, prot  ) && data->dcc )
    return comp_nmra_accessory( addr, port, gate, action );
  return 0;
}

static iONode __translate( obj inst, const iONode node ) {
  iODDXData data = Data((iODDX)inst);
  iONode rsp = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "translating: %s", NodeOp.getName( node ) );

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "translating: cmd=%s", cmdstr );

    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      stop_voltage(inst);
    }
    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      start_voltage(inst);
    }
  }

  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;
    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );

    if( port == 0 )
      fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      fromPADA( port, &addr, &port );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "co addr=%d port=%d gate=%d action=%d", addr, port, gate, action );
    __accessory(inst, addr, port, gate, action, wOutput.getprot( node ));
  }


  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );
  }


  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int error = 0;
    tDelayedAccCmd *delayedCmd = NULL;
    int delay  = data->swtime;
    int port   = wSwitch.getport1( node );
    int addr   = wSwitch.getaddr1( node );
    int action = 1;
    int gate   = 0;

    if( port == 0 ) {
      int l_addr = addr;
      fromFADA( addr, &addr, &port, &gate );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
          "sw FADA addr=%d converted to addr=%d port=%d gate=%d", l_addr, addr, port, gate );
    }
    else if( addr == 0 && port > 0 ) {
      int l_port = port;
      fromPADA( port, &addr, &port );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
          "sw PADA port=%d converted to addr=%d port=%d gate=%d", l_port, addr, port, gate );
    }
    else {
      int l_addr = toFADA( addr, port, gate );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
          "sw FADA would be addr=%d or addr=%d", l_addr, l_addr+1 );
    }

    gate = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 0x00:0x01;

    /* to get two decouples at one address: */
    if( StrOp.equals( wSwitch.gettype(node), wSwitch.decoupler ) )
      gate = wSwitch.getgate1( node );

    if( wSwitch.issinglegate( node ) ) {
      gate = wSwitch.getgate1( node );
      delay = 0;
      if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.straight ) )
        action = 0;
    }

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sw addr=%d port=%d gate=%d", addr, port, gate );

    if( wSwitch.isactdelay( node ) )
      delay = wSwitch.getdelay( node );

    __accessory(inst, addr, port, gate, action, wSwitch.getprot( node ));

    if( delay > 0 ) {
      delayedCmd = (tDelayedAccCmd*)allocMem(sizeof(tDelayedAccCmd));
      if (delayedCmd) {
        delayedCmd->acc_p=wSwitch.getprot( node )[0];
        delayedCmd->acc_addr=addr;
        delayedCmd->acc_port=port;
        delayedCmd->acc_gate=gate;
        delayedCmd->action=0;
        delayedCmd->delay=delay;
        delayedCmd->busnr=-1;
        delayedCmd->devicegroup=-1;
        data->swReset = ThreadOp.inst( "swreset", &thr_delayedAccCmd, delayedCmd );
        ThreadOp.start( data->swReset );
      }
    }
  }


  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() )||
          (( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) &&
          ( StrOp.equals( wLoc.getprot( node ), wLoc.prot_M ) && data->mm) )
         ) {
    int   addr = wLoc.getaddr( node );
    int    dir = wLoc.isdir( node );
    int  spcnt = wLoc.getspcnt( node );
    int  speed = 0;
    int   info = 0;
    Boolean sw = wLoc.issw( node );

    Boolean fn0 = wFunCmd.isf0(node);
    Boolean fn1 = wFunCmd.isf1(node);
    Boolean fn2 = wFunCmd.isf2(node);
    Boolean fn3 = wFunCmd.isf3(node);
    Boolean fn4 = wFunCmd.isf4(node);

    int f[28];
    MemOp.set(f, 0, 28);

    f[0] = fn0;
    f[1] = fn1;
    f[2] = fn2;
    f[3] = fn3;
    f[4] = fn4;

    if( spcnt > 127 ) spcnt = 127;

    if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
      speed = (wLoc.getV( node ) * spcnt) / 100;
    else if( wLoc.getV_max( node ) > 0 )
      speed = (wLoc.getV( node ) * spcnt) / wLoc.getV_max( node );

    if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_P ) ) {
      if( data->dcc )
        wLoc.setprot( node, wLoc.prot_N );
      else
        wLoc.setprot( node, wLoc.prot_M );
    }


    if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_M ) && data->mm) {
      int protver = wLoc.getprotver( node );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "lc=%d prot=MM%d spd=%d dir=%s lights=%s f1=%s f2=%s f3=%s f4=%s",
          addr, protver, speed, dir?"forward":"reverse", fn0?"on":"off", fn1?"on":"off", fn2?"on":"off", fn3?"on":"off", fn4?"on":"off" );
      switch( protver ) {
        case 1:
          comp_maerklin_1(addr, dir, speed, fn0);
          break;
        case 2:
          comp_maerklin_2(addr, dir, speed, fn0, fn1, fn2, fn3, fn4);
          break;
        case 3:
          comp_maerklin_3(addr, dir, speed, fn0, fn1, fn2, fn3, fn4);
          break;
        case 4:
          comp_maerklin_4(addr, dir, speed, fn0, fn1, fn2, fn3, fn4);
          break;
        case 5:
          comp_maerklin_5(addr, dir, speed, fn0, fn1, fn2, fn3, fn4);
          break;
        default:
          comp_maerklin_1(addr, dir, speed, fn0);
          break;
      }
      update_MaerklinPacketPool_Loco_Data(addr, dir, speed, fn0, fn1, fn2, fn3, fn4);
    }
    else if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_N ) && data->dcc ) {
      int rc = 0;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "lc=%d prot=DCCS spd=%d dir=%s lights=%s f1=%s f2=%s f3=%s f4=%s",
          addr, speed, dir?"forward":"reverse", fn0?"on":"off", fn1?"on":"off", fn2?"on":"off", fn3?"on":"off", fn4?"on":"off" );
      if( spcnt >= 127 )
        rc = comp_nmra_f4b7s128( addr, dir, speed, f);
      else
        rc = comp_nmra_f4b7s28( addr, dir, speed, f);
      if( rc != 0 ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "error sending DCC packet" );
      }
    }
    else if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_L ) && data->dcc ) {
      int rc = 0;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "lc=%d prot=DCCL spd=%d dir=%s lights=%s f1=%s f2=%s f3=%s f4=%s",
          addr, speed, dir?"forward":"reverse", fn0?"on":"off", fn1?"on":"off", fn2?"on":"off", fn3?"on":"off", fn4?"on":"off" );
      if( spcnt >= 127 )
        rc = comp_nmra_f4b14s128( addr, dir, speed, f);
      else
        rc = comp_nmra_f4b14s28( addr, dir, speed, f);
      if( rc != 0 ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "error sending DCC packet" );
      }
    }
    else {
      /* default */
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unsupported protocol [%s]", wLoc.getprot( node ));
    }

  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );
    int   info = 0;
    Boolean sw = wLoc.issw( node );
    int fgroup = wFunCmd.getgroup(node);
    int f[28];

    f[ 0] = wFunCmd.isf0(node);
    f[ 1] = wFunCmd.isf1(node);
    f[ 2] = wFunCmd.isf2(node);
    f[ 3] = wFunCmd.isf3(node);
    f[ 4] = wFunCmd.isf4(node);
    f[ 5] = wFunCmd.isf5(node);
    f[ 6] = wFunCmd.isf6(node);
    f[ 7] = wFunCmd.isf7(node);
    f[ 8] = wFunCmd.isf8(node);
    f[ 9] = wFunCmd.isf9(node);
    f[10] = wFunCmd.isf10(node);
    f[11] = wFunCmd.isf11(node);
    f[12] = wFunCmd.isf12(node);
    f[13] = wFunCmd.isf13(node);
    f[14] = wFunCmd.isf14(node);
    f[15] = wFunCmd.isf15(node);
    f[16] = wFunCmd.isf16(node);
    f[17] = wFunCmd.isf17(node);
    f[18] = wFunCmd.isf18(node);
    f[19] = wFunCmd.isf19(node);
    f[20] = wFunCmd.isf20(node);
    f[21] = wFunCmd.isf21(node);
    f[22] = wFunCmd.isf22(node);
    f[23] = wFunCmd.isf23(node);
    f[24] = wFunCmd.isf24(node);
    f[25] = wFunCmd.isf25(node);
    f[26] = wFunCmd.isf26(node);
    f[27] = wFunCmd.isf27(node);
    f[28] = wFunCmd.isf28(node);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "lc=%d lights=%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
        addr,
        f[0]  ? "on":"off",
        f[1]  ? "01":"--", f[2]  ? "02":"--", f[3]  ? "03":"--", f[4]  ? "04":"--",
        f[5]  ? "05":"--", f[6]  ? "06":"--", f[7]  ? "07":"--", f[8]  ? "08":"--",
        f[9]  ? "09":"--", f[10] ? "10":"--", f[11] ? "11":"--", f[12] ? "12":"--",
        f[13] ? "13":"--", f[14] ? "14":"--", f[15] ? "15":"--", f[16] ? "16":"--",
        f[17] ? "17":"--", f[18] ? "18":"--", f[19] ? "19":"--", f[20] ? "20":"--",
        f[21] ? "21":"--", f[22] ? "22":"--", f[23] ? "23":"--", f[24] ? "24":"--",
        f[25] ? "25":"--", f[26] ? "26":"--", f[27] ? "27":"--", f[28] ? "28":"--"
    );

    if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_P ) ) {
      if( data->dcc )
        wLoc.setprot( node, wLoc.prot_N );
      else
        wLoc.setprot( node, wLoc.prot_M );
    }

    if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_N ) ) {
      comp_nmra_fb7( addr, fgroup, f);
    }
    else if( StrOp.equals( wLoc.getprot( node ), wLoc.prot_L ) ) {
      comp_nmra_fb14( addr, fgroup, f);
    }
    else {
      /* default */
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unsupported protocol [%s]", wLoc.getprot( node ));
    }

  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    Boolean pom = wProgram.ispom( node );
    rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wProgram.setiid( rsp, data->iid );
    wProgram.setcmd( rsp, wProgram.datarsp );
    wProgram.setcv( rsp, wProgram.getcv( node ) );

    if(  wProgram.getcmd( node ) == wProgram.ptstat ) {
    }
    else if(  wProgram.getcmd( node ) == wProgram.pton ) {
      /* stop s88 and set D7 high: */
      setPT(inst, 1);
      data->ptflag = 1;
      rocrail_ddxStateChanged(inst);
	  if (data->powerflag) {
          stop_voltage(inst);
          ThreadOp.sleep(100);
	  }
    }
    else if( wProgram.getcmd( node ) == wProgram.ptoff ) {
      /* set D7 low and start s88:  */
      setPT(inst, 0);
      data->ptflag = 0;
      rocrail_ddxStateChanged(inst);
    }
    else if( wProgram.getcmd( node ) == wProgram.get && isPT(inst) ) {
      int cv = wProgram.getcv( node );
      int idx = 0;
      int ack = 0;
      int value = -1;
      iONode ddx_ini = wDigInt.getddx( data->ini );
	  if (data->powerflag) {
          stop_voltage(inst);
          ThreadOp.sleep(100);
	  }

      value = nmragetcvbyte(inst, cv-1);
      wProgram.setvalue( rsp, value );

    }
    else if( wProgram.getcmd( node ) == wProgram.set && (pom?True:isPT(inst)) ) {
      int cv  = wProgram.getcv( node );
      int val = wProgram.getvalue( node );
      int ack = 0;

      if( !pom && data->powerflag ){
          stop_voltage(inst);
          ThreadOp.sleep(100);
	  }

      ack = protocol_nmra_sm_write_cvbyte(inst, cv-1, val, pom);

      wProgram.setvalue( rsp, val );
      if( ack != 1 && !pom )
        wProgram.setvalue( rsp, -1 );
    }

  }

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iODDXData data = Data((iODDX)inst);
  iONode rsp = __translate( inst, cmd );

  /* Cleanup Node1 */
  NodeOp.base.del(cmd);

  return rsp;
}


/**  */
static void _halt( obj inst ) {
  iODDXData data = Data((iODDX)inst);
  stop_voltage(inst);
  close_comport(inst);
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iODDXData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}

void rocrail_ddxStateChanged( obj inst ) {
    iODDXData data = Data(inst);
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
    wState.setiid( node, wDigInt.getiid( data->ini ) );
    wState.setpower( node, data->powerflag?True:False );
    wState.setprogramming( node, data->ptflag?True:False );
    wState.settrackbus( node, data->communicationflag?True:False );
    wState.setsensorbus( node, data->s88flag?True:False );
    wState.setaccessorybus( node, data->communicationflag?True:False );
    if( data->listenerFun != NULL ) {
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
}

void rocrail_ddxFbListener( obj inst, int addr, int state ) {
	iODDXData data = Data(inst);
	if( data->listenerObj != NULL && data->listenerFun != NULL ) {
	  iONode node = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
	  wFeedback.setaddr( node, addr );
	  wFeedback.setstate( node, state?True:False );
	  if( data->iid != NULL )
	    wFeedback.setiid( node, data->iid );

	  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Sensor %d=%d", addr, state );
	  if( data->listenerFun != NULL ) {
	    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
      }
    }
}


/**  */
static Boolean _supportPT( obj inst ) {
  iODDXData data = Data((iODDX)inst);
  return True;
}


/* Status */
static int _state( obj inst ) {
  iODDXData data = Data((iODDX)inst);
  int state = 0;
  state |= data->powerflag << 0;
  state |= data->ptflag << 1;
  state |= data->communicationflag << 2;
  return state;
}


/* external shortcut event */
static void _shortcut(obj inst) {
  iODDXData data = Data( inst );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "external shortcut event; power off." );
  stop_voltage(inst);
}


/* VERSION: */
static int vmajor = 1;
static int vminor = 3;
static int patch  = 0;
static int _version( obj inst ) {
  iODDXData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

/**  */
static struct ODDX* _inst( const iONode ini ,const iOTrace trc ) {
  iODDX __DDX = allocMem( sizeof( struct ODDX ) );
  iODDXData data = allocMem( sizeof( struct ODDXData ) );
  iONode ddx_ini = NULL;
  MemOp.basecpy( __DDX, &DDXOp, 0, sizeof( struct ODDX ), data );

  TraceOp.set( trc );
  /* Initialize data->xxx members... */

  /* make a clone of the ini node: it could be replaced by rocrailDialog in the rocgui... */
  data->ini = (iONode)NodeOp.base.clone( ini );
  data->swtime = wDigInt.getswtime( ini );

  ddx_ini = wDigInt.getddx( data->ini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "DDX Digital Direct Xtra");
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->boosterport	= wDDX.getport( ddx_ini );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx boosterport=%s", data->boosterport );

  data->portbase = (int)strtol( wDDX.getportbase( ddx_ini ), (char**)NULL, 16 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx portbase addr=0x%X", data->portbase );

  data->shortcutchecking = wDDX.isshortcutchecking( ddx_ini );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx shortcutchecking=%d", data->shortcutchecking );
  data->shortcutdelay = wDDX.getshortcutdelay( ddx_ini );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx shortcutdelay=%d", wDDX.getshortcutdelay( ddx_ini ) );
  data->inversedsr = wDDX.isinversedsr( ddx_ini );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx inversedsr=%d", data->inversedsr );
  data->dcc = wDDX.isdcc( ddx_ini );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx dcc=%d", data->dcc );
  data->mm = wDDX.ismotorolarefresh( ddx_ini );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx mm=%d", data->mm );
  data->mma = wDDX.ismotorola( ddx_ini );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx mma=%d", data->mma );
  data->queuecheck = wDDX.isqueuecheck( ddx_ini );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx queuecheck=%d", data->queuecheck );
  data->fastcvget = wDDX.isfastcvget( ddx_ini );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx fastcvget=%d", data->fastcvget );

  data->s88port = (int)strtol( wDDX.gets88port( ddx_ini ), (char**)NULL, 16 );
  if( data->s88port > 0 ) {
    data->parport = wDDX.gets88port( ddx_ini );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx parport=%s", data->parport );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx base addr=0x%X", data->s88port );
    data->s88buses = wDDX.gets88busses( ddx_ini );
    data->s88b0modcnt = wDDX.gets88b0modcnt( ddx_ini );
    data->s88b1modcnt = wDDX.gets88b1modcnt( ddx_ini );
    data->s88b2modcnt = wDDX.gets88b2modcnt( ddx_ini );
    data->s88b3modcnt = wDDX.gets88b3modcnt( ddx_ini );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx s88buses=%d (%d,%d,%d,%d)",
        data->s88buses, data->s88b0modcnt, data->s88b1modcnt,
        data->s88b2modcnt, data->s88b3modcnt );
    data->s88refresh = wDDX.gets88refresh( ddx_ini );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx s88refresh=%d", data->s88refresh );
    data->s88clockscale = wDDX.gets88clockscale( ddx_ini );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx s88clockscale=%d", data->s88clockscale );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ddx Default turnout switch time=%d", data->swtime );
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------"
);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Starting DDX..." );
  SystemOp.inst();
  data->serial = NULL;
  ddx_entry( __DDX , ddx_ini );

  instCnt++;
//  __inst = __DDX;
  return __DDX;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/ddx.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/


