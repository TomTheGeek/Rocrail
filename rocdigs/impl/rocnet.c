 /*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 


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

#include "rocdigs/impl/rocnet_impl.h"
#include "rocdigs/impl/rocnet/rocnet-const.h"
#include "rocdigs/impl/rocnet/rocnet-parser.h"
#include "rocdigs/impl/rocnet/rn-utils.h"
#include "rocdigs/impl/rocnet/rnserial.h"
#include "rocdigs/impl/rocnet/rnudp.h"
#include "rocdigs/impl/rocnet/rntcp.h"

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
#include "rocrail/wrapper/public/RocNet.h"
#include "rocrail/wrapper/public/RocNetNode.h"
#include "rocrail/wrapper/public/BinCmd.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/Item.h"

#include "rocutils/public/addr.h"

#include <time.h>


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOrocNetData data = Data(inst);
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

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}


static byte __getProtocol(iONode loc) {
  byte prot = 0;

  if( StrOp.equals( wLoc.getprot(loc), wLoc.prot_N ) || StrOp.equals( wLoc.getprot(loc), wLoc.prot_L ) || StrOp.equals( wLoc.getprot(loc), wLoc.prot_P ) ) {
    if( wLoc.getspcnt(loc) < 28 )
      prot = RN_MOBILE_PROT_DCC14;
    if( wLoc.getspcnt(loc) > 28 )
      prot = RN_MOBILE_PROT_DCC128;
    else
      prot = RN_MOBILE_PROT_DCC28;
  }
  else if( StrOp.equals( wLoc.getprot(loc), wLoc.prot_M ) ) {
    prot = RN_MOBILE_PROT_MM;
  }

  return prot;
}


static int __getbusByNickname(iOrocNet inst, iONode node) {
  iOrocNetData data = Data(inst);
  if( wItem.getuidname(node) != NULL && StrOp.len(wItem.getuidname(node)) > 0 ) {
    iONode rnnode = wRocNet.getrocnetnode( data->ini );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,"searching UID-Name [%s]", wItem.getuidname(node) );
    while( rnnode != NULL ) {
      if( StrOp.equals(wItem.getuidname(node) , wRocNetNode.getnickname(rnnode)) ) {
        return wRocNetNode.getid(rnnode);
      }
      rnnode = wRocNet.nextrocnetnode( data->ini, rnnode );
    }
  }
  return -1;
}

/** ----- OrocNet ----- */
static iONode __translate( iOrocNet inst, iONode node ) {
  iOrocNetData data = Data(inst);
  byte*  rn  = allocMem(128);
  iONode rsp = NULL;
  int busByNickname = __getbusByNickname(inst, node);

  rn[0] = 0; /* network ID 0=ALL */

  if( busByNickname != -1 ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,"found bus %d for UID-Name [%s]", busByNickname, wItem.getuidname(node) );
    wItem.setbus(node, busByNickname);
  }

  rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );


  /* BinCmd command. */
  if( StrOp.equals( NodeOp.getName( node ), wBinCmd.name() ) ) {
  }

  /* Action command, */
  else if( StrOp.equals( NodeOp.getName( node ), wAction.name() ) ) {
    int bus   = wAction.getbus( node );
    int i = 0;
    const char* soundfile = wAction.getsndfile(node);
    if( StrOp.equals(wAction.sound_play, wAction.getcmd(node)) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,"play sound file [%s] on node=%d", wAction.getsndfile(node), bus );
      rn[RN_PACKET_GROUP] = RN_GROUP_SOUND;
      rnReceipientAddresToPacket( bus, rn, data->seven );
      rn[RN_PACKET_ACTION] = RN_SOUND_PLAY;
      rn[RN_PACKET_LEN] = StrOp.len(soundfile);
      for( i = 0; i < StrOp.len(soundfile); i++ ) {
        rn[RN_PACKET_DATA + i] = soundfile[i];
      }
      ThreadOp.post( data->writer, (obj)rn );
      return rsp;
    }
  }

  /* Clock command. */
  else if( StrOp.equals( NodeOp.getName( node ), wClock.name() ) ) {
    const char* cmd = wClock.getcmd( node );
    long l_time = wClock.gettime(node);
    struct tm* lTime = localtime( &l_time );

    int mins  = lTime->tm_min;
    int hours = lTime->tm_hour;
    int wday  = lTime->tm_wday;
    int mday  = lTime->tm_mday;
    int mon   = lTime->tm_mon;

    rn[RN_PACKET_GROUP] |= RN_GROUP_CLOCK;

    if( StrOp.equals( cmd, wClock.set ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Clock set" );
      rn[RN_PACKET_ACTION] = RN_CLOCK_SET;
      rn[RN_PACKET_LEN] = 8;
      rn[RN_PACKET_DATA + 2] = mon;
      rn[RN_PACKET_DATA + 3] = mday;
      rn[RN_PACKET_DATA + 4] = hours;
      rn[RN_PACKET_DATA + 5] = mins;
      rn[RN_PACKET_DATA + 7] = wClock.getdivider(node);
      ThreadOp.post( data->writer, (obj)rn );
      return rsp;
    }
    else if( StrOp.equals( cmd, wClock.sync ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Clock sync" );
      rn[RN_PACKET_ACTION] = RN_CLOCK_SYNC;
      rn[RN_PACKET_LEN] = 8;
      rn[RN_PACKET_DATA + 2] = mon;
      rn[RN_PACKET_DATA + 3] = mday;
      rn[RN_PACKET_DATA + 4] = hours;
      rn[RN_PACKET_DATA + 5] = mins;
      rn[RN_PACKET_DATA + 7] = wClock.getdivider(node);
      ThreadOp.post( data->writer, (obj)rn );
      return rsp;
    }
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    rn[RN_PACKET_GROUP] |= RN_GROUP_CS;

    if( StrOp.equals( cmd, wSysCmd.stop ) || StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      rn[RN_PACKET_ACTION] = RN_CS_TRACKPOWER;
      rn[RN_PACKET_LEN] = 1;
      rn[RN_PACKET_DATA + 0] = RN_CS_TRACKPOWER_OFF;
      ThreadOp.post( data->writer, (obj)rn );
      return rsp;
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      rn[RN_PACKET_ACTION] = RN_CS_TRACKPOWER;
      rn[RN_PACKET_LEN] = 1;
      rn[RN_PACKET_DATA + 0] = RN_CS_TRACKPOWER_ON;
      ThreadOp.post( data->writer, (obj)rn );
      return rsp;
    }
    else if( StrOp.equals( cmd, wSysCmd.sod ) ) {
      rn[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Start of Day" );
      rn[RN_PACKET_ACTION] = RN_STATIONARY_STARTOFDAY;
      rn[RN_PACKET_LEN] = 0;
      ThreadOp.post( data->writer, (obj)rn );
      return rsp;
    }
    else if( StrOp.equals( cmd, wSysCmd.shutdownnode ) ) {
      rn[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
      rnReceipientAddresToPacket( wSysCmd.getbus(node), rn, data->seven );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Shutdown node %d", wSysCmd.getbus(node) );
      rn[RN_PACKET_ACTION] = RN_STATIONARY_SHUTDOWN;
      rn[RN_PACKET_LEN] = 1;
      rn[RN_PACKET_DATA + 0] = 1;
      ThreadOp.post( data->writer, (obj)rn );
      return rsp;
    }

  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int bus    = wSwitch.getbus( node );
    int addr   = wSwitch.getaddr1( node );
    int single = wSwitch.issinglegate( node );
    byte cmd   = 0;

    if( single ) {
      cmd = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout );
    }
    else {
      cmd = 1;
      addr = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? addr+1 : addr;
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "switch bus=%d addr=%d cmd=%d", bus, addr, cmd );

    rn[RN_PACKET_GROUP] |= RN_GROUP_OUTPUT;
    rnReceipientAddresToPacket( bus, rn, data->seven );
    rn[RN_PACKET_ACTION] = RN_STATIONARY_SINGLE_PORT;
    rn[RN_PACKET_LEN] = 4;
    rn[RN_PACKET_DATA + 0] = cmd;
    rn[RN_PACKET_DATA + 1] = wSwitch.getporttype(node);
    rn[RN_PACKET_DATA + 2] = wSwitch.getdelay(node);
    rn[RN_PACKET_DATA + 3] = addr;
    if( data->watchdog != NULL ) {
      byte*  rnwd  = allocMem(32);
      MemOp.copy(rnwd, rn, 32);
      ThreadOp.post( data->watchdog, (obj)rnwd );
    }
    ThreadOp.post( data->writer, (obj)rn );
    return rsp;
  }

  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    int bus    = wSignal.getbus( node );
    int addr   = wSignal.getaddr(node);
    int aspect = wSignal.getaspect(node) + 1; /* Aspects numbers are zero based. */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "signal bus=%d addr=%d aspect=%d", bus, addr, aspect );

    rn[RN_PACKET_GROUP] |= RN_GROUP_OUTPUT;
    rnReceipientAddresToPacket( bus, rn, data->seven );
    rn[RN_PACKET_ACTION] = RN_STATIONARY_SINGLE_PORT;
    rn[RN_PACKET_LEN] = 5;
    rn[RN_PACKET_DATA + 0] = RN_OUTPUT_ON;
    rn[RN_PACKET_DATA + 1] = wProgram.porttype_macro;
    rn[RN_PACKET_DATA + 2] = 0;
    rn[RN_PACKET_DATA + 3] = aspect;
    rn[RN_PACKET_DATA + 4] = addr; /* (addr - 1) is used as macro port offset */

    if( data->watchdog != NULL ) {
      byte*  rnwd  = allocMem(32);
      MemOp.copy(rnwd, rn, 32);
      ThreadOp.post( data->watchdog, (obj)rnwd );
    }
    ThreadOp.post( data->writer, (obj)rn );
    return rsp;
  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int bus  = wOutput.getbus( node );
    int addr = wOutput.getaddr( node );
    byte cmd   = RN_OUTPUT_ON;

    if( StrOp.equals( wOutput.getcmd( node ), wOutput.off ) ) {
      cmd = RN_OUTPUT_OFF;
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output bus=%d addr=%d cmd=%d", bus, addr, cmd );

    rn[RN_PACKET_GROUP] |= RN_GROUP_OUTPUT;
    rnReceipientAddresToPacket( bus, rn, data->seven );
    rn[RN_PACKET_ACTION] = RN_STATIONARY_SINGLE_PORT;
    rn[RN_PACKET_LEN] = 4;
    rn[RN_PACKET_DATA + 0] = cmd;
    rn[RN_PACKET_DATA + 1] = wOutput.getporttype(node);
    rn[RN_PACKET_DATA + 2] = 0;
    rn[RN_PACKET_DATA + 3] = addr;

    if( data->watchdog != NULL ) {
      byte*  rnwd  = allocMem(32);
      MemOp.copy(rnwd, rn, 32);
      ThreadOp.post( data->watchdog, (obj)rnwd );
    }
    ThreadOp.post( data->writer, (obj)rn );
    return rsp;
  }

  /* Loc dispatch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) && StrOp.equals(wLoc.dispatch, wLoc.getcmd(node)) ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "dispatch loco %s:%d", wLoc.getid(node), wLoc.getaddr(node));
    data->dispatchAddr = wLoc.getaddr(node);
    return rsp;
  }

  /* Loco command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   bus  = wLoc.getbus( node );
    int   addr = wLoc.getaddr( node );
    int  spcnt = wLoc.getspcnt( node );
    int      V = wLoc.getV( node );
    int   mass = wLoc.getmass( node );
    byte    fn = wLoc.isfn( node )  ? RN_MOBILE_LIGHTS_ON:0;
    byte   dir = wLoc.isdir( node ) ? RN_MOBILE_DIR_FORWARDS:0;
    byte  prot = __getProtocol(node);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d:%d V=%d lights=%s dir=%s spcnt=%d",
        bus, addr, V, fn?"on":"off", dir?"forwards":"reverse", spcnt );

    if( bus > 0 ) {
      rn[RN_PACKET_GROUP] |= RN_GROUP_CS;
      rnReceipientAddresToPacket( bus, rn, data->seven );
      rn[RN_PACKET_ACTION] = RN_CS_VELOCITY;
      rn[RN_PACKET_LEN] = 7;
      rn[RN_PACKET_DATA + 0] = addr / 256;
      rn[RN_PACKET_DATA + 1] = addr % 256;
      rn[RN_PACKET_DATA + 2] = V;
      rn[RN_PACKET_DATA + 3] = dir;
      rn[RN_PACKET_DATA + 4] = fn;
      rn[RN_PACKET_DATA + 5] = prot;
      rn[RN_PACKET_DATA + 6] = spcnt;
    }
    else {
      rn[RN_PACKET_GROUP] |= RN_GROUP_MOBILE;
      rnReceipientAddresToPacket( addr, rn, data->seven );
      rn[RN_PACKET_ACTION] = RN_MOBILE_VELOCITY;
      rn[RN_PACKET_LEN] = 5;
      rn[RN_PACKET_DATA + 0] = V;
      rn[RN_PACKET_DATA + 1] = dir;
      rn[RN_PACKET_DATA + 2] = fn;
      rn[RN_PACKET_DATA + 3] = mass;
      rn[RN_PACKET_DATA + 4] = spcnt;

      if( data->watchdog != NULL ) {
        byte*  rnwd  = allocMem(32);
        MemOp.copy(rnwd, rn, 32);
        ThreadOp.post( data->watchdog, (obj)rnwd );
      }
    }
    ThreadOp.post( data->writer, (obj)rn );
    return rsp;
  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   bus  = wFunCmd.getbus( node );
    int   addr = wFunCmd.getaddr( node );
    int lights = (wFunCmd.isf0( node )?0x01:0x00);
    byte  prot = __getProtocol(node);
    int i = 0;
    int fb1 = 0;
    int fb2 = 0;
    int fb3 = 0;
    for( i = 0; i < 8; i++) {
      char key[32];
      StrOp.fmtb(key, "f%d", i+1 );
      if( NodeOp.getBool(node, key, False) )
        fb1 |= (1 << i);
    }
    for( i = 0; i < 8; i++) {
      char key[32];
      StrOp.fmtb(key, "f%d", i+9 );
      if( NodeOp.getBool(node, key, False) )
        fb2 |= (1 << i);
    }
    for( i = 0; i < 8; i++) {
      char key[32];
      StrOp.fmtb(key, "f%d", i+17 );
      if( NodeOp.getBool(node, key, False) )
        fb3 |= (1 << i);
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "lc=%d:%d fb1=0x%02X fb2=0x%02X fb3=0x%02X changed=%d",
        bus, addr, fb1, fb2, fb3, wFunCmd.getfnchanged(node));

    if( bus > 0 ) {
      rn[RN_PACKET_GROUP] |= RN_GROUP_CS;
      rnReceipientAddresToPacket( bus, rn, data->seven );
      rn[RN_PACKET_ACTION] = RN_CS_FUNCTION;
      rn[RN_PACKET_LEN] = 7;
      rn[RN_PACKET_DATA + 0] = addr / 256;
      rn[RN_PACKET_DATA + 1] = addr % 256;
      rn[RN_PACKET_DATA + 2] = fb1;
      rn[RN_PACKET_DATA + 3] = fb2;
      rn[RN_PACKET_DATA + 4] = fb3;
      rn[RN_PACKET_DATA + 5] = prot + (lights << 7);
      rn[RN_PACKET_DATA + 6] = (wFunCmd.getfnchanged(node) & 0x7F) + (lights << 7);
    }
    else {
      rn[RN_PACKET_GROUP] |= RN_GROUP_MOBILE;
      rnReceipientAddresToPacket( addr, rn, data->seven );
      rn[RN_PACKET_ACTION] = RN_MOBILE_FUNCTIONS;
      rn[RN_PACKET_LEN] = 3;
      rn[RN_PACKET_DATA + 0] = fb1;
      rn[RN_PACKET_DATA + 1] = fb2;
      rn[RN_PACKET_DATA + 2] = fb3;
      rn[RN_PACKET_DATA + 3] = wFunCmd.getfnchanged(node) & 0xFF;

      if( data->watchdog != NULL ) {
        byte*  rnwd  = allocMem(32);
        MemOp.copy(rnwd, rn, 32);
        ThreadOp.post( data->watchdog, (obj)rnwd );
      }
    }
    ThreadOp.post( data->writer, (obj)rn );
    return rsp;
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    Boolean direct = wProgram.getmode(node) == wProgram.mode_direct;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "program type %d...", wProgram.getlntype(node) );
    if( wProgram.getlntype(node) == wProgram.lntype_sv && wProgram.getcmd( node ) == wProgram.lncvget &&
        wProgram.getcv(node) == 0 && wProgram.getmodid(node) == 0 && wProgram.getaddr(node) == 0 )
    {
      /* This construct is used to to query all LocoIOs, but is here recycled for query all CAN-GC2s. */
      rn[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
      rnReceipientAddresToPacket( 0, rn, data->seven );
      rn[RN_PACKET_ACTION] = RN_STATIONARY_IDENTIFY;
      rn[RN_PACKET_LEN] = 0;
      ThreadOp.post( data->writer, (obj)rn );
    }
    else if(wProgram.getlntype(node) == wProgram.lntype_rocnet ) {
      if( wProgram.getcmd( node ) == wProgram.nnreq ) {
        int i = 0;
        int rnid = wProgram.getmodid(node);
        int newrnid = wProgram.getvalue(node);
        int location = wProgram.getval1(node);
        int subip = wProgram.getval2(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        rn[RN_PACKET_ACTION] = RN_PROGRAMMING_WRNID;
        rn[RN_PACKET_LEN] = 5 + StrOp.len(wProgram.getstrval1(node));
        rn[RN_PACKET_DATA + 0] = newrnid / 256;
        rn[RN_PACKET_DATA + 1] = newrnid % 256;
        rn[RN_PACKET_DATA + 2] = subip / 256;
        rn[RN_PACKET_DATA + 3] = subip % 256;
        rn[RN_PACKET_DATA + 4] = location;
        StrOp.copy((char*)&rn[RN_PACKET_DATA + 5], wProgram.getstrval1(node));
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.nvget ) {
        int porttype = wProgram.getporttype(node);
        int rnid = wProgram.getmodid(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        if( porttype == wProgram.porttype_servo ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "program read channel porttype=%d", porttype );
          rn[RN_PACKET_ACTION] = RN_PROGRAMMING_RCHANNEL;
          rn[RN_PACKET_LEN] = 2;
          rn[RN_PACKET_DATA + 0] = wProgram.getval1(node);
          rn[RN_PACKET_DATA + 1] = wProgram.getval2(node);
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "program read port porttype=%d", porttype );
          rn[RN_PACKET_ACTION] = RN_PROGRAMMING_RPORT;
          rn[RN_PACKET_LEN] = 2;
          rn[RN_PACKET_DATA + 0] = wProgram.getval1(node);
          rn[RN_PACKET_DATA + 1] = wProgram.getval2(node);
        }
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.evget ) {
        int rnid = wProgram.getmodid(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        rn[RN_PACKET_ACTION] = RN_PROGRAMMING_RPORTEVENT;
        rn[RN_PACKET_LEN] = 2;
        rn[RN_PACKET_DATA + 0] = wProgram.getval1(node);
        rn[RN_PACKET_DATA + 1] = wProgram.getval2(node);
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.setchannel ) {
        int porttype = wProgram.getporttype(node);
        int rnid = wProgram.getmodid(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        rn[RN_PACKET_ACTION] = RN_PROGRAMMING_SETCHANNEL;
        rn[RN_PACKET_LEN] = 4;
        rn[RN_PACKET_DATA + 0] = wProgram.getval1(node);
        rn[RN_PACKET_DATA + 1] = wProgram.getval2(node)/256;
        rn[RN_PACKET_DATA + 2] = wProgram.getval2(node)%256;
        rn[RN_PACKET_DATA + 3] = wProgram.getval3(node);
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.nvset ) {
        char key[32] = {'\0'};
        int i = 0;
        int porttype = wProgram.getporttype(node);
        int rnid = wProgram.getmodid(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        if( porttype == wProgram.porttype_servo ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "program write channel porttype=%d", porttype );
          rn[RN_PACKET_ACTION] = RN_PROGRAMMING_WCHANNEL;
          rn[RN_PACKET_LEN] = 8*8;
          for( i = 0; i < 8; i++ ) {
            /* channel#   offposH  offposL  onposH   onposL   offsteps onsteps  options */
            StrOp.fmtb(key, "val%d", i*6 + 1);
            rn[RN_PACKET_DATA + 0 + i*8] = NodeOp.getInt(node, key, 0);
            StrOp.fmtb(key, "val%d", i*6 + 2);
            rn[RN_PACKET_DATA + 1 + i*8] = NodeOp.getInt(node, key, 0)/256;
            rn[RN_PACKET_DATA + 2 + i*8] = NodeOp.getInt(node, key, 0)%256;
            StrOp.fmtb(key, "val%d", i*6 + 3);
            rn[RN_PACKET_DATA + 3 + i*8] = NodeOp.getInt(node, key, 0)/256;
            rn[RN_PACKET_DATA + 4 + i*8] = NodeOp.getInt(node, key, 0)%256;
            StrOp.fmtb(key, "val%d", i*6 + 4);
            rn[RN_PACKET_DATA + 5 + i*8] = NodeOp.getInt(node, key, 0);
            StrOp.fmtb(key, "val%d", i*6 + 5);
            rn[RN_PACKET_DATA + 6 + i*8] = NodeOp.getInt(node, key, 0);
            StrOp.fmtb(key, "val%d", i*6 + 6);
            rn[RN_PACKET_DATA + 7 + i*8] = NodeOp.getInt(node, key, 0);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "channel=%d offpos=%d onpos=%d",
                rn[RN_PACKET_DATA + 0 + i*8], rn[RN_PACKET_DATA + 1 + i*8]*256 + rn[RN_PACKET_DATA + 2 + i*8], rn[RN_PACKET_DATA + 3 + i*8]*256 + rn[RN_PACKET_DATA + 4 + i*8] );
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "program write port porttype=%d", porttype );
          rn[RN_PACKET_ACTION] = RN_PROGRAMMING_WPORT;
          rn[RN_PACKET_LEN] = 8*4;
          for( i = 0; i < 8; i++ ) {
            StrOp.fmtb(key, "val%d", i*4 + 1);
            rn[RN_PACKET_DATA + 0 + i*4] = NodeOp.getInt(node, key, 0);
            StrOp.fmtb(key, "val%d", i*4 + 2);
            rn[RN_PACKET_DATA + 1 + i*4] = NodeOp.getInt(node, key, 0);
            StrOp.fmtb(key, "val%d", i*4 + 3);
            rn[RN_PACKET_DATA + 2 + i*4] = NodeOp.getInt(node, key, 0);
            StrOp.fmtb(key, "val%d", i*4 + 4);
            rn[RN_PACKET_DATA + 3 + i*4] = NodeOp.getInt(node, key, 0);
          }
        }
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.unlearn ) {
        char key[32] = {'\0'};
        int i = 0;
        int rnid = wProgram.getmodid(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        if( wProgram.getporttype( node ) == wProgram.porttype_servo )
          rn[RN_PACKET_ACTION] = RN_PROGRAMMING_RMCHANNEL;
        else
          rn[RN_PACKET_ACTION] = RN_PROGRAMMING_RMPORT;
        rn[RN_PACKET_LEN] = 8;
        for( i = 0; i < 8; i++ ) {
          StrOp.fmtb(key, "val%d", i + 1);
          rn[RN_PACKET_DATA + 0 + i] = NodeOp.getInt(node, key, 0);
        }
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.evset ) {
        char key[32] = {'\0'};
        int i = 0;
        int rnid = wProgram.getmodid(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        rn[RN_PACKET_ACTION] = RN_PROGRAMMING_WPORTEVENT;
        rn[RN_PACKET_LEN] = 8*4;
        for( i = 0; i < 8; i++ ) {
          StrOp.fmtb(key, "val%d", i*4 + 1);
          rn[RN_PACKET_DATA + 0 + i*4] = NodeOp.getInt(node, key, 0);
          StrOp.fmtb(key, "val%d", i*4 + 2);
          rn[RN_PACKET_DATA + 1 + i*4] = NodeOp.getInt(node, key, 0);
          StrOp.fmtb(key, "val%d", i*4 + 3);
          rn[RN_PACKET_DATA + 2 + i*4] = NodeOp.getInt(node, key, 0);
          StrOp.fmtb(key, "val%d", i*4 + 4);
          rn[RN_PACKET_DATA + 3 + i*4] = NodeOp.getInt(node, key, 0);
        }
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.getoptions ) {
        int i = 0;
        int rnid = wProgram.getmodid(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        rn[RN_PACKET_ACTION] = RN_PROGRAMMING_RDOPT;
        rn[RN_PACKET_LEN] = 0;
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.setoptions ) {
        int i = 0;
        int rnid = wProgram.getmodid(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        rn[RN_PACKET_ACTION] = RN_PROGRAMMING_WROPT;
        rn[RN_PACKET_LEN] = 4;
        rn[RN_PACKET_DATA + 0] = wProgram.getval1(node);
        rn[RN_PACKET_DATA + 1] = wProgram.getval2(node);
        rn[RN_PACKET_DATA + 2] = wProgram.getval3(node);
        rn[RN_PACKET_DATA + 3] = wProgram.getval4(node);
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.show ) {
        int i = 0;
        int rnid = wProgram.getmodid(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        rn[RN_PACKET_ACTION] = RN_STATIONARY_SHOW;
        rn[RN_PACKET_LEN] = 0;
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.update ) {
        int rnid = wProgram.getmodid(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        rn[RN_PACKET_ACTION] = RN_PROGRAMMING_UPDATE;
        rn[RN_PACKET_LEN] = 3;
        rn[RN_PACKET_DATA+0] = wProgram.getvalue(node)/256;
        rn[RN_PACKET_DATA+1] = wProgram.getvalue(node)%256;
        rn[RN_PACKET_DATA+2] = wProgram.getval1(node);
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.query ) {
        rn[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
        rnReceipientAddresToPacket( 0, rn, data->seven );
        rn[RN_PACKET_ACTION] = RN_STATIONARY_IDENTIFY;
        rn[RN_PACKET_LEN] = 0;
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.macro_get ) {
        int i = 0;
        int rnid = wProgram.getmodid(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        rn[RN_PACKET_ACTION] = RN_PROGRAMMING_RMACRO;
        rn[RN_PACKET_LEN] = 1;
        rn[RN_PACKET_DATA+0] = wProgram.getvalue(node);
        ThreadOp.post( data->writer, (obj)rn );
      }
      else if( wProgram.getcmd( node ) == wProgram.macro_set ) {
        char key[32] = {'\0'};
        int i = 0;
        int rnid = wProgram.getmodid(node);
        rn[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
        rnReceipientAddresToPacket( rnid, rn, data->seven );
        rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
        rn[RN_PACKET_ACTION] = RN_PROGRAMMING_WMACRO;
        rn[RN_PACKET_LEN] = 1 + 8*4;
        rn[RN_PACKET_DATA + 0] = wProgram.getvalue(node);
        for( i = 0; i < 8; i++ ) {
          StrOp.fmtb(key, "val%d", i*4 + 1);
          rn[RN_PACKET_DATA + 1 + i*4] = NodeOp.getInt(node, key, 0);
          StrOp.fmtb(key, "val%d", i*4 + 2);
          rn[RN_PACKET_DATA + 2 + i*4] = NodeOp.getInt(node, key, 0);
          StrOp.fmtb(key, "val%d", i*4 + 3);
          rn[RN_PACKET_DATA + 3 + i*4] = NodeOp.getInt(node, key, 0);
          StrOp.fmtb(key, "val%d", i*4 + 4);
          rn[RN_PACKET_DATA + 4 + i*4] = NodeOp.getInt(node, key, 0);
        }
        ThreadOp.post( data->writer, (obj)rn );
      }
    }
    else if(wProgram.ispom(node)) {
      int addr = wProgram.getaddr( node );
      rn[RN_PACKET_GROUP] = RN_GROUP_CS;
      rnReceipientAddresToPacket( 0, rn, data->seven );
      rn[RN_PACKET_ACTION] = RN_CS_POM;
      rn[RN_PACKET_LEN] = 6;
      rn[RN_PACKET_DATA + 0] = addr / 256;
      rn[RN_PACKET_DATA + 1] = addr % 256;
      rn[RN_PACKET_DATA + 2] = wProgram.getcv(node)/256;
      rn[RN_PACKET_DATA + 3] = wProgram.getcv(node)%256;
      rn[RN_PACKET_DATA + 4] = wProgram.getvalue(node);
      rn[RN_PACKET_DATA + 5] = wProgram.getcmd(node) == wProgram.set ?1:0;
      ThreadOp.post( data->writer, (obj)rn );
    }
    return rsp;
  }

  /* unhandled command */
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Unhandled command: [%s][%s]",
      NodeOp.getName(node), NodeOp.getStr(node, "cmd", "?") );
  /* not used; free up */
  freeMem(rn);

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOrocNetData data = Data(inst);
  iONode rsp = __translate( (iOrocNet)inst, cmd );

  if( cmd != NULL ) {
    if(StrOp.equals( NodeOp.getName(cmd), wSysCmd.name() ) ) {
      if( StrOp.equals( wSysCmd.getcmd(cmd), wSysCmd.shutdown ) && wSysCmd.getval(cmd) == 1 ) {
        byte* rn = allocMem(32);
        rn[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
        rnReceipientAddresToPacket( 0, rn, data->seven );
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Shutdown all nodes" );
        rn[RN_PACKET_ACTION] = RN_STATIONARY_SHUTDOWN;
        rn[RN_PACKET_LEN] = 1;
        rn[RN_PACKET_DATA + 0] = 1;
        ThreadOp.post( data->writer, (obj)rn );
        ThreadOp.sleep(500);
      }
    }
  }
  /* Cleanup Node1 */
  cmd->base.del(cmd);

  return rsp;
}


/**  */
static void _halt( obj inst, Boolean poweroff ) {
  iOrocNetData data = Data(inst);

  data->shutdown = True;

  if( poweroff ) {
    byte* rn;
    rn = allocMem(32);
    rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
    rn[RN_PACKET_GROUP] = RN_GROUP_CS;
    rn[RN_PACKET_ACTION] = RN_CS_TRACKPOWER;
    rn[RN_PACKET_LEN] = 1;
    rn[RN_PACKET_DATA + 0] = RN_CS_TRACKPOWER_OFF;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
    ThreadOp.post( data->writer, (obj)rn );
    ThreadOp.sleep(500);

    rn = allocMem(32);
    rnSenderAddresToPacket( wRocNet.getid(data->rnini), rn, data->seven );
    rn[RN_PACKET_GROUP] = RN_GROUP_HOST;
    rn[RN_PACKET_ACTION] = RN_HOST_SHUTDOWN;
    rn[RN_PACKET_LEN] = 0;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Shutdown" );
    ThreadOp.post( data->writer, (obj)rn );
    /* grab some time to process the request */
    ThreadOp.sleep(500);
  }

  data->run = False;
  data->connected = False;
  ThreadOp.sleep(500);
  data->rnDisconnect(inst);
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOrocNetData data = Data(inst);
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

/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  return 0;
}


/* external shortcut event */
static void _shortcut(obj inst) {
  iOrocNetData data = Data( inst );
}



/**  */
static Boolean _supportPT( obj inst ) {
  return True;
}

static byte* __evaluateMobile( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);
  byte* rnReply = NULL;
  char key[64] = {'\0'};
  iOMouse Mouse = NULL;

  rcpt = rnReceipientAddrFromPacket(rn, data->seven);
  sndr = rnSenderAddrFromPacket(rn, data->seven);

  if( actionType != RN_ACTIONTYPE_EVENT ) {
    return NULL;
  }

  switch( action ) {
  case RN_MOBILE_ROCMOUSE:
    StrOp.fmtb(key, "%d-%d", sndr, rn[RN_PACKET_DATA+0]&0x7F );
    Mouse = (iOMouse)MapOp.get(data->mousemap, key);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "node [%d] reports a rocmouse 0x%02X event, dispatchAddr=%d", sndr, rn[RN_PACKET_DATA+0], data->dispatchAddr );

    if( Mouse != NULL && (rn[RN_PACKET_DATA+0]&0x80) ) {
      /* Release */
      iONode nodeSpd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setthrottleid( nodeSpd, key );
      wLoc.setaddr(nodeSpd, Mouse->lcaddr);
      wLoc.setV(nodeSpd, 0);
      wLoc.setdir(nodeSpd, True);
      wLoc.setfn(nodeSpd, False);
      wLoc.setiid( nodeSpd, data->iid );
      wLoc.setcmd(nodeSpd, wLoc.fieldcmd);

      data->listenerFun( data->listenerObj, nodeSpd, TRCLEVEL_INFO );

      MapOp.remove(data->mousemap, key);
      freeMem(Mouse);
      Mouse = NULL;

      break;
    }

    if( Mouse == NULL && data->dispatchAddr > 0 ) {
      byte* rnID = allocMem(32);
      Mouse = allocMem( sizeof(struct mouse));
      MapOp.put(data->mousemap, key, (obj)Mouse);
      Mouse->nodeid = sndr;
      Mouse->baseaddr = rn[RN_PACKET_DATA+0]&0x7F;
      Mouse->lcaddr = data->dispatchAddr;
      data->dispatchAddr = 0;

      rnID[RN_PACKET_GROUP] = RN_GROUP_MOBILE;
      rnReceipientAddresToPacket( sndr, rnID, data->seven );
      rnSenderAddresToPacket( wRocNet.getid(data->rnini), rnID, data->seven );
      rnID[RN_PACKET_ACTION] = RN_MOBILE_ROCMOUSE_BIND;
      rnID[RN_PACKET_LEN] = 3;
      rnID[RN_PACKET_DATA + 0] = Mouse->baseaddr;
      rnID[RN_PACKET_DATA + 1] = Mouse->lcaddr / 256;
      rnID[RN_PACKET_DATA + 2] = Mouse->lcaddr % 256;
      ThreadOp.post( data->writer, (obj)rnID );
    }

    if( Mouse != NULL ) {
      iONode nodeSpd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setthrottleid( nodeSpd, key );
      wLoc.setaddr(nodeSpd, Mouse->lcaddr);
      wLoc.setV_raw(nodeSpd, rn[RN_PACKET_DATA+1]);
      if( rn[RN_PACKET_DATA+8] > 0 )
        wLoc.setV_rawMax(nodeSpd, rn[RN_PACKET_DATA+8]);
      else
        wLoc.setV_rawMax(nodeSpd, 28);
      wLoc.setdir(nodeSpd, (rn[RN_PACKET_DATA+2] & 0x01)?True:False);
      wLoc.setfn(nodeSpd, (rn[RN_PACKET_DATA+2] & 0x02)?True:False);
      wLoc.setiid( nodeSpd, data->iid );
      wLoc.setcmd(nodeSpd, wLoc.fieldcmd);

      data->listenerFun( data->listenerObj, nodeSpd, TRCLEVEL_INFO );

      if( rn[RN_PACKET_DATA+7] > 0 ) {
        int i = 0;
        int changedFn = rn[RN_PACKET_DATA+7];
        iONode nodeFn = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
        wLoc.setthrottleid( nodeFn, key );
        wFunCmd.setaddr(nodeFn, Mouse->lcaddr);
        wFunCmd.setfnchanged(nodeFn, changedFn);
        wFunCmd.setgroup(nodeFn, (changedFn-1)/4);

        wFunCmd.setf0(nodeFn, (rn[RN_PACKET_DATA+2] & 0x02)?True:False);
        wLoc.setiid( nodeFn, data->iid );
        wLoc.setcmd(nodeFn, wLoc.fieldcmd);

        for( i = 0; i < 28; i++ ) {
          Boolean fon = False;
          char fnkey[32] = {'\0'};
          StrOp.fmtb(fnkey, "f%d", i+1);
          fon = rn[RN_PACKET_DATA+3+i/8] & (0x01 << (i%8));
          NodeOp.setBool(nodeFn, fnkey, fon);
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "fnchanged=%d %s=%s", changedFn, fnkey, fon?"ON":"OFF");
        }

        data->listenerFun( data->listenerObj, nodeFn, TRCLEVEL_INFO );
      }

    }
    break;
  }

  return rnReply;
}


static byte* __evaluateStationary( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);
  byte* rnReply = NULL;
  char key[32] = {'\0'};
  char mnemonic[32] = {'\0'};
  int i;
  int subip = 0;


  rcpt = rnReceipientAddrFromPacket(rn, data->seven);
  sndr = rnSenderAddrFromPacket(rn, data->seven);

  switch( action ) {
  case RN_STATIONARY_ERROR:
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
        "node [%d] reports a [%s](%d)error, reason [%s](%d) on address [%d]", sndr,
        rnGetRC(rn[RN_PACKET_DATA+0]), rn[RN_PACKET_DATA+0],
        rnGetRS(rn[RN_PACKET_DATA+1]), rn[RN_PACKET_DATA+1],
        rn[RN_PACKET_DATA+2] * 256 + rn[RN_PACKET_DATA+3] );
    break;

  case RN_STATIONARY_IDENTIFY:
    if( data->shutdown ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ignore identify: shutting down...");
      break;
    }

    if( actionType != RN_ACTIONTYPE_EVENT) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ignore identify command...");
      break;
    }

    subip = rn[RN_PACKET_DATA+5]*256+rn[RN_PACKET_DATA+6];
    rn[RN_PACKET_DATA + 7 + (rn[RN_PACKET_LEN]-7) ] = 0;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "Identified: rocnetid=%d class=%s vid=%d revision=%d nrio=%d subip=%d.%d nickname=[%s]", sndr,
        rnClassString(rn[RN_PACKET_DATA+0], mnemonic), rn[RN_PACKET_DATA+1], rn[RN_PACKET_DATA+2] *256 + rn[RN_PACKET_DATA+3],
        rn[RN_PACKET_DATA+4], rn[RN_PACKET_DATA+5], rn[RN_PACKET_DATA+6], &rn[RN_PACKET_DATA+7] );
    if( sndr == 65535 || sndr == 0 || (sndr == 1 && wRocNet.getid(data->rnini) == 1) ) {
      if( data->highestID == 0 ) {
        /* default address; send a new ID */
        data->highestID = wRocNet.getid(data->rnini);
        iONode rrnode = wRocNet.getrocnetnode(data->ini);
        while( rrnode != NULL ) {
          if( wRocNetNode.getid(rrnode) > data->highestID )
            data->highestID = wRocNetNode.getid(rrnode);
          rrnode = wRocNet.nextrocnetnode(data->ini, rrnode);
        }
      }
      data->highestID++;
      byte* rnID = allocMem(32);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "send %d a new ID: %d", sndr, data->highestID );
      rnID[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
      rnReceipientAddresToPacket( sndr, rnID, data->seven );
      rnSenderAddresToPacket( wRocNet.getid(data->rnini), rnID, data->seven );
      rnID[RN_PACKET_ACTION] = RN_PROGRAMMING_WRNID;
      rnID[RN_PACKET_LEN] = 5;
      rnID[RN_PACKET_DATA + 0] = data->highestID / 256;
      rnID[RN_PACKET_DATA + 1] = data->highestID % 256;
      rnID[RN_PACKET_DATA + 2] = rn[RN_PACKET_DATA+5];
      rnID[RN_PACKET_DATA + 3] = rn[RN_PACKET_DATA+6];
      rnID[RN_PACKET_DATA + 4] = rn[RN_PACKET_NETID];
      ThreadOp.post( data->writer, (obj)rnID );
      break;
    }

    StrOp.fmtb( key, "%d-%d", rn[RN_PACKET_NETID], sndr);
    if( data->run && !MapOp.haskey( data->nodemap, key ) ) {
      iONode rnnode = NodeOp.inst( wRocNetNode.name(), data->ini, ELEMENT_NODE );
      wRocNetNode.setid(rnnode, sndr);
      wRocNetNode.setlocation(rnnode, rn[RN_PACKET_NETID]);
      wRocNetNode.setsubip(rnnode, subip);
      wRocNetNode.setclass(rnnode, rnClassString(rn[RN_PACKET_DATA+0], mnemonic));
      wRocNetNode.setmnemonic(rnnode, mnemonic);
      wRocNetNode.setvendor(rnnode, rn[RN_PACKET_DATA+1]);
      wRocNetNode.setrevision(rnnode, rn[RN_PACKET_DATA+2] *256 + rn[RN_PACKET_DATA+3]);
      wRocNetNode.setnrio(rnnode, rn[RN_PACKET_DATA+4]);
      wRocNetNode.setnickname(rnnode, (char*)&rn[RN_PACKET_DATA+7]);

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "registering node %s", key );
      MapOp.put( data->nodemap, key, (obj)rnnode);
      NodeOp.addChild( data->ini, rnnode );
      if( sndr >= data->highestID )
        data->highestID = sndr + 1;

      /* Inform clients */
      {
        iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
        wProgram.setiid( node, data->iid );
        wProgram.setcmd(node, wProgram.identify);
        wProgram.setvalue( node, 0 );
        wProgram.setlntype(node, wProgram.lntype_rocnet);
        NodeOp.addChild( node, (iONode)NodeOp.base.clone(rnnode) );
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
      }
    }
    else if( MapOp.haskey( data->nodemap, key ) ) {
      iONode rnnode = (iONode)MapOp.get( data->nodemap, key );
      if( wRocNetNode.getsubip(rnnode) != subip ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "node %s is already registered with another subIP: %d", key, wRocNetNode.getsubip(rnnode) );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "node %s is already registered", key );
      }
    }

    rnReply = allocMem(32);
    rnReply[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
    rnReceipientAddresToPacket( sndr, rnReply, data->seven );
    rnSenderAddresToPacket( wRocNet.getid(data->rnini), rnReply, data->seven );
    rnReply[RN_PACKET_ACTION] = RN_STATIONARY_ACK;
    rnReply[RN_PACKET_LEN] = 1;
    rnReply[RN_PACKET_DATA] = RN_STATIONARY_IDENTIFY;
    break;

  case RN_STATIONARY_NOP:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "NOP from %d to %d", sndr, rcpt );
    break;

  case RN_STATIONARY_SHUTDOWN: {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "node %d has been shutdown", sndr );

    StrOp.fmtb( key, "%d-%d", rn[RN_PACKET_NETID], sndr);
    if( MapOp.haskey( data->nodemap, key ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "remove node %s from list", key );
      NodeOp.removeChild( data->ini, (iONode)MapOp.remove( data->nodemap, key ) );
      /* Inform clients */
      {
        iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
        wProgram.setiid( node, data->iid );
        wProgram.setmodid( node, sndr );
        wProgram.setcmd(node, wProgram.identify);
        wProgram.setvalue( node, 1 );
        wProgram.setlntype(node, wProgram.lntype_rocnet);
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "node %s is not registered", key );
    }
    /* STOP */
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
    wState.setiid( node, wDigInt.getiid( data->ini ) );
    wState.setpower( node, False );
    wState.setemergency( node, True );
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
    break;

  case RN_STATIONARY_SHOW: {
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(node, sndr);
    wProgram.setcmd( node, wProgram.show );
    wProgram.setiid( node, data->iid );
    wProgram.setlntype(node, wProgram.lntype_rocnet);
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
    break;

  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d] from %d", action, sndr );
    break;
  }

  return rnReply;
}


static void __evaluatePTMobile( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);

  rcpt = rnReceipientAddrFromPacket(rn, data->seven);
  sndr = rnSenderAddrFromPacket(rn, data->seven);

  switch( action ) {
  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
    break;
  }
}


static void __evaluatePTStationary( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);

  rcpt = rnReceipientAddrFromPacket(rn, data->seven);
  sndr = rnSenderAddrFromPacket(rn, data->seven);

  switch( action ) {
  case RN_PROGRAMMING_RPORT:
  case RN_PROGRAMMING_WPORT:
  {
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    int nrports = rn[RN_PACKET_LEN] / 4;
    int i = 0;
    int idx = 1;
    char key[32] = {'\0'};
    wProgram.setmodid(node, sndr);
    wProgram.setcmd( node, wProgram.nvget );
    for( i = 0; i < nrports; i++ ) {
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*4 + 0] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*4 + 1] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*4 + 2] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*4 + 3] );
      idx++;
    }
    wProgram.setiid( node, data->iid );
    wProgram.setlntype(node, wProgram.lntype_rocnet);
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
  break;

  case RN_PROGRAMMING_RCHANNEL:
  case RN_PROGRAMMING_WCHANNEL:
  {
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    int nrports = rn[RN_PACKET_LEN] / 8;
    int i = 0;
    int idx = 1;
    char key[32] = {'\0'};
    wProgram.setmodid(node, sndr);
    wProgram.setcmd( node, wProgram.nvget );
    wProgram.setporttype( node, wProgram.porttype_servo );
    for( i = 0; i < nrports; i++ ) {
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*8 + 0] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*8 + 1] * 256 + rn[RN_PACKET_DATA + i*8 + 2] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*8 + 3] * 256 + rn[RN_PACKET_DATA + i*8 + 4] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*8 + 5] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*8 + 6] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*8 + 7] );
      idx++;
    }
    wProgram.setiid( node, data->iid );
    wProgram.setlntype(node, wProgram.lntype_rocnet);
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
  break;

  case RN_PROGRAMMING_RPORTEVENT:
  case RN_PROGRAMMING_WPORTEVENT:
  {
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    int nrports = rn[RN_PACKET_LEN] / 4;
    int i = 0;
    int idx = 1;
    char key[32] = {'\0'};
    wProgram.setmodid(node, sndr);
    wProgram.setcmd( node, wProgram.evget );
    for( i = 0; i < nrports; i++ ) {
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*4 + 0] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*4 + 1] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*4 + 2] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*4 + 3] );
      idx++;
    }
    wProgram.setiid( node, data->iid );
    wProgram.setlntype(node, wProgram.lntype_rocnet);
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
  break;

  case RN_PROGRAMMING_RMACRO:
  case RN_PROGRAMMING_WMACRO:
  {
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    int nrports = (rn[RN_PACKET_LEN]-1) / 4;
    int i = 0;
    int idx = 1;
    char key[32] = {'\0'};
    wProgram.setmodid(node, sndr);
    wProgram.setcmd( node, wProgram.macro_get );
    wProgram.setvalue( node, rn[RN_PACKET_DATA+0] );
    for( i = 0; i < nrports; i++ ) {
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*4 + 1] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*4 + 2] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*4 + 3] );
      idx++;
      StrOp.fmtb( key, "val%d", idx );
      NodeOp.setInt(node, key, rn[RN_PACKET_DATA + i*4 + 4] );
      idx++;
    }
    wProgram.setiid( node, data->iid );
    wProgram.setlntype(node, wProgram.lntype_rocnet);
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
  break;

  case RN_PROGRAMMING_RDOPT:
  case RN_PROGRAMMING_WROPT:
  {
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setmodid(node, sndr);
    wProgram.setcmd( node, wProgram.getoptions );
    wProgram.setval1( node, rn[RN_PACKET_DATA+0] );
    wProgram.setval2( node, rn[RN_PACKET_DATA+1] );
    wProgram.setval3( node, rn[RN_PACKET_DATA+2] );
    wProgram.setval4( node, rn[RN_PACKET_DATA+3] );
    /* I2C scan of 0x20, 0x30 and 0x40 */
    wProgram.setval5( node, rn[RN_PACKET_DATA+4]*256 + rn[RN_PACKET_DATA+5] );
    wProgram.setval6( node, rn[RN_PACKET_DATA+6]*256 + rn[RN_PACKET_DATA+7] );
    wProgram.setval7( node, rn[RN_PACKET_DATA+8]*256 + rn[RN_PACKET_DATA+9] );
    wProgram.setiid( node, data->iid );
    wProgram.setlntype(node, wProgram.lntype_rocnet);
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
  break;

  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
    break;
  }
}


static void __evaluateClock( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);

  rcpt = rnReceipientAddrFromPacket(rn, data->seven);
  sndr = rnSenderAddrFromPacket(rn, data->seven);

  switch( action ) {
  case RN_CLOCK_SET:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "clock set" );
    break;
  case RN_CLOCK_SYNC:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "clock sync" );
    break;
  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
    break;
  }
}


static byte* __evaluateSensor( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          addr       = rn[RN_PACKET_DATA+3];
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);
  byte* rnReply = NULL;

  rcpt = rnReceipientAddrFromPacket(rn, data->seven);
  sndr = rnSenderAddrFromPacket(rn, data->seven);

  switch( action ) {
  case RN_SENSOR_REPORT:
  case RN_SENSORID_REPORT:
  {
    char key[32] = {'\0'};
    StrOp.fmtb( key, "%d-%d", rn[RN_PACKET_NETID], sndr);
    iONode evt = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
    iONode rnnode = (iONode)MapOp.get(data->nodemap, key);

    if( rnnode != NULL )
      wItem.setuidname(evt, wRocNetNode.getnickname(rnnode));
    wFeedback.setbus( evt, sndr );
    wFeedback.setaddr( evt, addr );
    wFeedback.setfbtype( evt, wFeedback.fbtype_sensor );

    char ident[32] = {'\0'};
    if( rn[RN_PACKET_LEN] > 4 ) {
      int len = rn[RN_PACKET_LEN] - 4;
      int i = 0;
      for( i = 0; i < len && i < 31; i++) {
        ident[i] = rn[RN_PACKET_DATA + 4 + i];
        ident[i+1] = '\0';
      }
      if( action == RN_SENSORID_REPORT )
        wFeedback.setid( evt, ident );
      else
        wFeedback.setidentifier( evt, ident );
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sensor report %d:%d %s id(ent)=%s", sndr, addr, rn[RN_PACKET_DATA+2]?"on":"off", ident );

    if( data->iid != NULL )
      wFeedback.setiid( evt, data->iid );

    wFeedback.setstate( evt, rn[RN_PACKET_DATA+2]?True:False );

    data->listenerFun( data->listenerObj, evt, TRCLEVEL_INFO );

    if( data->sack ) {
      rnReply = allocMem(32);
      rnReply[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
      rnReceipientAddresToPacket( sndr, rnReply, data->seven );
      rnSenderAddresToPacket( wRocNet.getid(data->rnini), rnReply, data->seven );
      rnReply[RN_PACKET_ACTION] = RN_STATIONARY_ACK;
      rnReply[RN_PACKET_LEN] = 2;
      rnReply[RN_PACKET_DATA+0] = RN_SENSOR_REPORT;
      rnReply[RN_PACKET_DATA+1] = addr;
    }
    break;
  }
  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
    break;
  }

  return rnReply;
}


static void __evaluateInput( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data       = Data(rocnet);
  int          addr       = 0;
  int          rcpt       = 0;
  int          sndr       = 0;
  Boolean      isThis     = rocnetIsThis( rocnet, rn);
  int          action     = rnActionFromPacket(rn);
  int          actionType = rnActionTypeFromPacket(rn);

  rcpt = rnReceipientAddrFromPacket(rn, data->seven);
  sndr = rnSenderAddrFromPacket(rn, data->seven);

  switch( action ) {
  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
    break;
  }
}


static void __evaluateRN( iOrocNet rocnet, byte* rn ) {
  iOrocNetData data = Data(rocnet);
  int group = rn[RN_PACKET_GROUP];
  int actionType = rnActionTypeFromPacket(rn);
  int sndr = rnSenderAddrFromPacket(rn, data->seven);
  int port = rn[RN_PACKET_DATA + 3];
  byte* rnReply = NULL;

  TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)rn, 8 + rn[RN_PACKET_LEN] );

  switch( group ) {
    case RN_GROUP_CS:
      rnReply = rocnetParseGeneral( rocnet, rn );
      break;

    case RN_GROUP_OUTPUT:
      rnReply = rocnetParseOutput( rocnet, rn );
      if( actionType == RN_ACTIONTYPE_EVENT) {
        char key[32] = {'\0'};
        iONode rnnode = NULL;
        iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
        StrOp.fmtb( key, "%d-%d", rn[RN_PACKET_NETID], sndr);
        rnnode = (iONode)MapOp.get(data->nodemap, key);

        if( rnnode != NULL )
          wItem.setuidname(nodeC, wRocNetNode.getnickname(rnnode));

        wSwitch.setbus( nodeC, sndr );
        wSwitch.setaddr1( nodeC, port );
        wSwitch.setaddr1( nodeC, port );
        wSwitch.setstate( nodeC, (rn[RN_PACKET_DATA + 0] == 0) ?"straight":"turnout" );
        wSwitch.setporttype( nodeC, rn[RN_PACKET_DATA + 1] & 0x0F );
        if( data->iid != NULL )
          wSwitch.setiid( nodeC, data->iid );

        if( data->listenerFun != NULL && data->listenerObj != NULL )
          data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

        if( data->watchdog != NULL ) {
          byte* rnwd = allocMem(8+rn[RN_PACKET_LEN]);
          MemOp.copy( rnwd, rn, 8+rn[RN_PACKET_LEN]);
          ThreadOp.post( data->watchdog, (obj)rnwd );
        }
      }
      break;

    case RN_GROUP_INPUT:
      __evaluateInput( rocnet, rn );
      break;

    case RN_GROUP_MOBILE:
      rnReply = __evaluateMobile( rocnet, rn );
      break;

    case RN_GROUP_STATIONARY:
      rnReply = __evaluateStationary( rocnet, rn );
      break;

    case RN_GROUP_PT_MOBILE:
      __evaluatePTMobile( rocnet, rn );
      break;

    case RN_GROUP_PT_STATIONARY:
      __evaluatePTStationary( rocnet, rn );
      break;

    case RN_GROUP_CLOCK:
      __evaluateClock( rocnet, rn );
      break;

    case RN_GROUP_SENSOR:
      rnReply = __evaluateSensor( rocnet, rn );
      break;

    default:
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unsupported group [%d]", group );
      break;
  }

  if( rnReply != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "posting reply..." );
    ThreadOp.post( data->writer, (obj)rnReply );
  }
}


static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOrocNet rocnet = (iOrocNet)ThreadOp.getParm( th );
  iOrocNetData data = Data(rocnet);
  byte rn[0x7F];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocNet reader started" );

  /* Connect with the sublib. */
  while( !data->connected && data->run ) {
    data->connected = data->rnConnect((obj)rocnet);
    ThreadOp.sleep(2500);
  };


  while( data->connected && data->run ) {
    int extended = False;
    int event    = False;
    int insize   = 0;

    if( data->rnAvailable((obj)rocnet) ) {
      insize = data->rnRead( (obj)rocnet, rn );

      if( rnCheckPacket(rn, &extended, &event) ) {
        Boolean isThis = rocnetIsThis( rocnet, rn);
        int rcpt = rnReceipientAddrFromPacket(rn, 0);
        int sndr = rnSenderAddrFromPacket(rn, 0);
        if( isThis ) {
          char* str = StrOp.byteToStr(rn, 8 + rn[RN_PACKET_LEN]);
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "ignore %s [%s] from %d(self) to %d", rnActionTypeString(rn), str, sndr, rcpt );
          StrOp.free(str);
        }
        else {
          char* str = StrOp.byteToStr(rn, 8 + rn[RN_PACKET_LEN]);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "evaluate %s [%s] from %d to %d", rnActionTypeString(rn), str, sndr, rcpt );
          StrOp.free(str);
          __evaluateRN( rocnet, rn );
        }
      }
      else
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "reject invalid packet" );
    }
    else {
      ThreadOp.sleep(10);
    }

  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocNet reader stopped." );
}


static void __watchdog( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOrocNet rocnet = (iOrocNet)ThreadOp.getParm( th );
  iOrocNetData data = Data(rocnet);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocNet watchdog started." );

  /* give the sublib time to connect */
  ThreadOp.sleep(1000);
  while( data->run ) {
    int i = 0;
    int size = ListOp.size( data->AckList );
    byte* rn = (byte*)ThreadOp.getPost( th );

    if (rn != NULL) {
      int group = rn[RN_PACKET_GROUP];
      int actionType = rnActionTypeFromPacket(rn);
      int sndr = rnSenderAddrFromPacket(rn, data->seven);

      if( actionType == RN_ACTIONTYPE_REQUEST ) {
        Boolean newReq = True;
        for( i = 0; i < size; i++ ) {
          iORNreq req = (iORNreq)ListOp.get( data->AckList, i );
          if( rn[RN_PACKET_LEN] == req->req[RN_PACKET_LEN] && MemOp.cmp(req->req, rn, 8 + rn[RN_PACKET_LEN]) ) {
            newReq = False;
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "same request is allready in the list %d", i );
            break;
          }
        }

        if( newReq ) {
          iORNreq req = allocMem(sizeof(struct rnreq));
          req->req = rn;
          req->timer = SystemOp.getTick();
          req->ack = False;
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "add request to the list" );
          ListOp.add( data->AckList, (obj)req );
        }

      }
      else if( actionType == RN_ACTIONTYPE_EVENT ) {
        rn[RN_PACKET_ACTION] &= RN_ACTION_CODE_MASK;
        for( i = 0; i < size; i++ ) {
          iORNreq req = (iORNreq)ListOp.get( data->AckList, i );
          if( sndr == rnReceipientAddrFromPacket(req->req, data->seven) && rn[RN_PACKET_LEN] == req->req[RN_PACKET_LEN] &&
              MemOp.cmp(rn+RN_PACKET_GROUP, req->req+RN_PACKET_GROUP, 2 + rn[RN_PACKET_LEN]) )
          {
            req->ack = True;
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "request %d is acknowledged", i );
          }
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unhandled request" );
        freeMem(rn);
      }
    }

    Boolean ack = False;
    do {
      ack = False;
      size = ListOp.size( data->AckList );
      for( i = 0; i < size; i++ ) {
        iORNreq req = (iORNreq)ListOp.get( data->AckList, i );
        if( req->ack ) {
          iORNreq req = (iORNreq)ListOp.remove(data->AckList, i);
          freeMem(req->req);
          freeMem(req);
          ack = True;
          break;
        }
      }
    } while(ack);

    size = ListOp.size( data->AckList );
    for( i = 0; i < size; i++ ) {
      iORNreq req = (iORNreq)ListOp.get( data->AckList, i );
      if( req->timer + 50 <= SystemOp.getTick() ) {
        byte* rncopy = allocMem(8+req->req[RN_PACKET_LEN]);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "request %d of %d timeout: resend", i, size );
        req->timer = SystemOp.getTick();
        MemOp.copy(rncopy, req->req, 8+req->req[RN_PACKET_LEN]);
        ThreadOp.post( data->writer, (obj)rncopy );
      }
    }


    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocNet watchdog stopped." );
}


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOrocNet rocnet = (iOrocNet)ThreadOp.getParm( th );
  iOrocNetData data = Data(rocnet);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocNet writer started." );

  /* give the sublib time to connect */
  ThreadOp.sleep(1000);


  do {
    if( data->connected ) {
      byte* rnRequest = (byte*)ThreadOp.getPost( th );
      if (rnRequest != NULL) {
        int extended = False;
        int event    = False;
        int plen     = 0;
        Boolean ok   = False;

        plen = 8 + rnRequest[RN_PACKET_LEN];

        if( rnCheckPacket(rnRequest, &extended, &event) ) {
          int rcpt = rnReceipientAddrFromPacket(rnRequest, 0);
          char* str = StrOp.byteToStr(rnRequest, 8 + rnRequest[RN_PACKET_LEN]);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "write %s [%s] to %d", rnActionTypeString(rnRequest), str, rcpt );
          StrOp.free(str);
          ok = data->rnWrite( (obj)rocnet, rnRequest, plen );

        }
        freeMem( rnRequest);
      }
    }

    ThreadOp.sleep(10);
  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocNet writer stopped." );
}



/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOrocNetData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}



/**  */
static struct OrocNet* _inst( const iONode ini ,const iOTrace trc ) {
  iOrocNet __rocNet = allocMem( sizeof( struct OrocNet ) );
  iOrocNetData data = allocMem( sizeof( struct OrocNetData ) );
  MemOp.basecpy( __rocNet, &rocNetOp, 0, sizeof( struct OrocNet ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini    = ini;
  data->rnini = wDigInt.getrocnet(ini);
  data->AckList = ListOp.inst();
  data->nodemap  = MapOp.inst();
  data->mousemap = MapOp.inst();

  if( wDigInt.getiid(ini) != NULL )
    data->iid = StrOp.dup(wDigInt.getiid( ini ));

  if( data->rnini == NULL ) {
    data->rnini = NodeOp.inst( wRocNet.name(), ini, ELEMENT_NODE );
    NodeOp.addChild( ini, data->rnini );
  }
  data->crc = wRocNet.iscrc(data->rnini);
  data->sack = wRocNet.issack(data->rnini);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocNET %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid        = %s", wDigInt.getiid( ini ) != NULL ? wDigInt.getiid( ini ):"" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib     = %s", wDigInt.getsublib( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "crc        = %s", data->crc ? "on":"off" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "secure ack = %s", data->sack ? "on":"off" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );


  /* choose interface: */
  if( StrOp.equals( wDigInt.sublib_serial, wDigInt.getsublib( ini ) ) ) {
    /* rnserial */
    data->rnConnect    = rnSerialConnect;
    data->rnDisconnect = rnSerialDisconnect;
    data->rnRead       = rnSerialRead;
    data->rnWrite      = rnSerialWrite;
    data->rnAvailable  = rnSerialAvailable;
    data->seven = True;
    data->run = True;
  }
  else if( StrOp.equals( wDigInt.sublib_tcp, wDigInt.getsublib( ini ) ) ) {
    /* rnserial */
    data->rnConnect    = rnTcpConnect;
    data->rnDisconnect = rnTcpDisconnect;
    data->rnRead       = rnTcpRead;
    data->rnWrite      = rnTcpWrite;
    data->rnAvailable  = rnTcpAvailable;
    data->seven = True;
    data->run = True;
  }
  else if( StrOp.equals( wDigInt.sublib_udp, wDigInt.getsublib( ini ) ) ||
           StrOp.equals( wDigInt.sublib_default, wDigInt.getsublib( ini ) ) ) {
    /* rnudp */
    data->rnConnect    = rnUDPConnect;
    data->rnDisconnect = rnUDPDisconnect;
    data->rnRead       = rnUDPRead;
    data->rnWrite      = rnUDPWrite;
    data->rnAvailable  = rnUDPAvailable;
    data->run = True;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib [%s] is not supported", wDigInt.getsublib( ini ) );
  }

  /* Remove all nodes from previous session: */
  {
    iONode rnnode = wRocNet.getrocnetnode(data->ini);
    while( rnnode != NULL ) {
      NodeOp.removeChild(data->ini, rnnode);
      rnnode = wRocNet.getrocnetnode(data->ini);
    }
  }

  if( data->run == True ) {
    data->reader = ThreadOp.inst( "rnreader", &__reader, __rocNet );
    ThreadOp.start( data->reader );

    data->writer = ThreadOp.inst( "rnwriter", &__writer, __rocNet );
    ThreadOp.start( data->writer );

    if( wRocNet.iswd(data->rnini) ) {
      data->watchdog = ThreadOp.inst( "rnwatchdog", &__watchdog, __rocNet );
      ThreadOp.start( data->watchdog );
    }
  }

  instCnt++;
  return __rocNet;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/rocnet.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
