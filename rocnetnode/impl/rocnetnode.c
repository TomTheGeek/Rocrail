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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "rocs/public/rocs.h"
#include "rocs/public/objbase.h"
#include "rocs/public/trace.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/thread.h"
#include "rocs/public/file.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/cmdln.h"
#include "rocs/public/stats.h"
#include "rocs/public/system.h"
#include "rocs/public/lib.h"

#include "rocrail/wrapper/public/Cmdline.h"
#include "rocrail/wrapper/public/RocNet.h"
#include "rocrail/wrapper/public/PortSetup.h"
#include "rocrail/wrapper/public/Trace.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Program.h"

#include "rocnetnode/impl/rocnetnode_impl.h"

#include "rocint/public/digint.h"

#include "rocdigs/impl/rocnet/rocnet-const.h"
#include "rocdigs/impl/rocnet/rn-utils.h"

#include "rocnetnode/public/io.h"

#define ROCNETNODEINI "rocnetnode.ini"

static int instCnt = 0;

static int versionH = 1;
static int versionL = 0;

typedef iIDigInt (* LPFNROCGETDIGINT)( const iONode ,const iOTrace );

static void __sendRN( iORocNetNode rocnetnode, byte* rn );
static iONode __findPort(iORocNetNode inst, int port);
static void __initPorts(iORocNetNode inst);

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iORocNetNodeData data = Data(inst);
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

/** ----- ORocNetNode ----- */

static Boolean bShutdown = False;
static iORocNetNode __RocNetNode = NULL;

static Boolean __isThis( iORocNetNode rocnetnode, byte* rn ) {
  iORocNetNodeData data = Data(rocnetnode);
  return (rnSenderAddrFromPacket(rn, 0) == data->id);
}

static byte* __handleClock( iORocNetNode rocnetnode, byte* rn ) {
  iORocNetNodeData data       = Data(rocnetnode);
  int rcpt       = 0;
  int sndr       = 0;
  int action     = rnActionFromPacket(rn);
  int actionType = rnActionTypeFromPacket(rn);
  byte* msg = NULL;

  rcpt = rnReceipientAddrFromPacket(rn, 0);
  sndr = rnSenderAddrFromPacket(rn, 0);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Clock request %d from %d to %d", action, sndr, rcpt );

  switch( action ) {
    case RN_CLOCK_SET:
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "Clock set to %02d:%02d divider=%d", rn[RN_PACKET_DATA + 4], rn[RN_PACKET_DATA + 5], rn[RN_PACKET_DATA + 7] );
      break;
    case RN_CLOCK_SYNC:
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "Clock sync to %02d:%02d divider=%d", rn[RN_PACKET_DATA + 4], rn[RN_PACKET_DATA + 5], rn[RN_PACKET_DATA + 7] );
      break;
  }

  return msg;
}


static byte* __handleCS( iORocNetNode rocnetnode, byte* rn ) {
  iORocNetNodeData data       = Data(rocnetnode);
  int rcpt       = 0;
  int sndr       = 0;
  int action     = rnActionFromPacket(rn);
  int actionType = rnActionTypeFromPacket(rn);
  byte* msg = NULL;
  int addr = 0;
  int V = 0;
  int dir = 0;
  int lights = 0;
  int i = 0;

  rcpt = rnReceipientAddrFromPacket(rn, 0);
  sndr = rnSenderAddrFromPacket(rn, 0);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CS request %d from %d to %d", action, sndr, rcpt );

  switch( action ) {
    case RN_CS_TRACKPOWER:
      if(data->pDI != NULL) {
        iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE);
        wSysCmd.setcmd(cmd, rn[RN_PACKET_DATA + 0] & 0x01 ? wSysCmd.go:wSysCmd.stop);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CS track power %s", rn[RN_PACKET_DATA + 0] & 0x01 ? "ON":"OFF" );
        data->pDI->cmd( (obj)data->pDI, cmd );
      }
      data->power = rn[RN_PACKET_DATA + 0] & 0x01 ? True:False;
      msg = allocMem(32);
      msg[RN_PACKET_GROUP] = RN_GROUP_CS;
      rnSenderAddresToPacket( data->id, msg, 0 );
      msg[RN_PACKET_ACTION] = RN_CS_TRACKPOWER;
      msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
      msg[RN_PACKET_LEN] = 1;
      msg[RN_PACKET_DATA + 0] = data->power;
      break;

    case RN_CS_POM:
      if(data->pDI != NULL) {
        iONode cmd = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE);
        int cv = rn[RN_PACKET_DATA + 2] * 256 + rn[RN_PACKET_DATA + 3];
        addr = rn[RN_PACKET_DATA + 0] * 256 + rn[RN_PACKET_DATA + 1];
        wProgram.setaddr(cmd, addr);
        wProgram.setlongaddr(cmd, addr > 127 ? True:False);
        wProgram.setcv(cmd, cv );
        wProgram.setvalue(cmd, rn[RN_PACKET_DATA + 4]);
        wProgram.setpom(cmd, True);
        wProgram.setcmd(cmd, rn[RN_PACKET_DATA + 5] == 0 ? wProgram.get:wProgram.set );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "CS POM %s loco=%d cv=%d value=%d", rn[RN_PACKET_DATA + 5] == 0 ? "read":"write" ,addr, cv, rn[RN_PACKET_DATA + 4] );
        data->pDI->cmd( (obj)data->pDI, cmd );
      }
      break;

    case RN_CS_VELOCITY:
      addr = rn[RN_PACKET_DATA + 0] * 256 + rn[RN_PACKET_DATA + 1];
      V = rn[RN_PACKET_DATA + 2];
      dir = rn[RN_PACKET_DATA + 3];
      lights = rn[RN_PACKET_DATA + 4];
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco addr=%d V=%d dir=%d lights=%d", addr, V, dir, lights );
      if(data->pDI != NULL) {
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE);
        wLoc.setaddr(cmd, addr);
        wLoc.setV(cmd, V);
        wLoc.setdir(cmd, dir);
        wLoc.setfn(cmd, lights);
        data->pDI->cmd( (obj)data->pDI, cmd );
      }
      break;

    case RN_CS_FUNCTION:
      addr = rn[RN_PACKET_DATA + 0] * 256 + rn[RN_PACKET_DATA + 1];
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco function addr=%d", addr );
      if(data->pDI != NULL) {
        iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE);
        wFunCmd.setaddr(cmd, addr);
        for( i = 0; i < 8; i++ ) {
          char key[32];
          StrOp.fmtb(key, "f%d", i+1);
          NodeOp.setBool(cmd, key, (rn[RN_PACKET_DATA + 2] & (1 << i)) ? True:False);
        }
        for( i = 0; i < 8; i++ ) {
          char key[32];
          StrOp.fmtb(key, "f%d", i+9);
          NodeOp.setBool(cmd, key, (rn[RN_PACKET_DATA + 3] & (1 << (i+8))) ? True:False);
        }
        for( i = 0; i < 8; i++ ) {
          char key[32];
          StrOp.fmtb(key, "f%d", i+17);
          NodeOp.setBool(cmd, key, (rn[RN_PACKET_DATA + 3] & (1 << (i+16))) ? True:False);
        }
        data->pDI->cmd( (obj)data->pDI, cmd );
      }
      break;
  }

  return msg;
}

static void __saveIni(iORocNetNode rocnetnode) {
  iORocNetNodeData data = Data(rocnetnode);
  iOFile iniFile = FileOp.inst( ROCNETNODEINI, OPEN_WRITE );

  if( iniFile != NULL ) {
    char* iniStr = NodeOp.base.toString( data->ini );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "saving ini %s...", ROCNETNODEINI );
    FileOp.write( iniFile, iniStr, StrOp.len( iniStr ) );
    FileOp.close( iniFile );
    StrOp.free(iniStr);
  }
}


static byte* __handlePTStationary( iORocNetNode rocnetnode, byte* rn ) {
  iORocNetNodeData data       = Data(rocnetnode);
  int port       = rn[RN_PACKET_DATA + 3];
  int rcpt       = 0;
  int sndr       = 0;
  int action     = rnActionFromPacket(rn);
  int actionType = rnActionTypeFromPacket(rn);
  Boolean isThis = __isThis( rocnetnode, rn);
  byte* msg = NULL;
  int i = 0;
  int from = 0;
  int to = 0;
  int idx = 0;

  rcpt = rnReceipientAddrFromPacket(rn, 0);
  sndr = rnSenderAddrFromPacket(rn, 0);

  switch( action ) {
  case RN_PROGRAMMING_WRNID:
    msg = allocMem(32);
    msg[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
    rnReceipientAddresToPacket( 0, msg, 0 );
    rnSenderAddresToPacket( data->id, msg, 0 );
    msg[RN_PACKET_ACTION] = RN_STATIONARY_SHUTDOWN;
    msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
    msg[RN_PACKET_LEN] = 0;

    data->id = rn[RN_PACKET_DATA + 0] * 256 + rn[RN_PACKET_DATA + 1];
    data->identack = False;
    /* Save the rocnetnode.ini to persistent the new ID. */
    {
      iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
      wRocNet.setid(rocnet, data->id);
      __saveIni(rocnetnode);
    }

    msg = allocMem(128);
    MemOp.copy(msg, rn, rn[RN_PACKET_LEN] + 8 );
    rnReceipientAddresToPacket( sndr, msg, 0 );
    rnSenderAddresToPacket( data->id, msg, 0 );
    msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);

    break;

  case RN_PROGRAMMING_WPORT:
  {
    iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
    int i = 0;

    if( rocnet == NULL ) {
      rocnet = NodeOp.inst( wRocNet.name(), data->ini, ELEMENT_NODE);
      NodeOp.addChild( data->ini, rocnet );
    }

    for( i = 0; i < 8; i++ ) {
      int port  = rn[RN_PACKET_DATA+0+i*4];
      int ionr  = rn[RN_PACKET_DATA+1+i*4];
      int type  = rn[RN_PACKET_DATA+2+i*4];
      int delay = rn[RN_PACKET_DATA+3+i*4];
      iONode portsetup = __findPort(rocnetnode, port);
      if( portsetup == NULL ) {
        portsetup = NodeOp.inst( wPortSetup.name(), rocnet, ELEMENT_NODE);
        wPortSetup.setport( portsetup, port);
        NodeOp.addChild( rocnet, portsetup );
      }
      wPortSetup.setionr( portsetup, ionr);
      wPortSetup.settype( portsetup, type);
      wPortSetup.setdelay( portsetup, delay);
    }
    __saveIni(rocnetnode);
    __initPorts(rocnetnode);
  }
  break;

  case RN_PROGRAMMING_RPORT:
    from = rn[RN_PACKET_DATA+0];
    to   = rn[RN_PACKET_DATA+1];
    if( from > to ) {
      from = rn[RN_PACKET_DATA+1];
      to   = rn[RN_PACKET_DATA+0];
    }
    if( to - from > 7 ) {
      to = from + 7;
    }

    msg = allocMem(128);
    msg[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
    rnReceipientAddresToPacket( sndr, msg, 0 );
    rnSenderAddresToPacket( data->id, msg, 0 );
    msg[RN_PACKET_ACTION] = RN_PROGRAMMING_RPORT;
    msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
    msg[RN_PACKET_LEN] = ((to-from)+1)*4;
    for( i = from; i <= to; i++ ) {
      if( data->ports[i] != NULL ) {
        msg[RN_PACKET_DATA + 0 + idx * 4] = i;
        msg[RN_PACKET_DATA + 1 + idx * 4] = data->ports[i]->ionr;
        msg[RN_PACKET_DATA + 2 + idx * 4] = data->ports[i]->type;
        msg[RN_PACKET_DATA + 3 + idx * 4] = data->ports[i]->delay;
      }
      idx++;
    }
    break;
  }

  return msg;
}


static byte* __handleStationary( iORocNetNode rocnetnode, byte* rn ) {
  iORocNetNodeData data       = Data(rocnetnode);
  int port       = rn[RN_PACKET_DATA + 3];
  int rcpt       = 0;
  int sndr       = 0;
  int action     = rnActionFromPacket(rn);
  int actionType = rnActionTypeFromPacket(rn);
  Boolean isThis = __isThis( rocnetnode, rn);
  byte* msg = NULL;

  rcpt = rnReceipientAddrFromPacket(rn, 0);
  sndr = rnSenderAddrFromPacket(rn, 0);

  switch( action ) {
  case RN_STATIONARY_ACK:
    if( rn[RN_PACKET_DATA + 0] == RN_STATIONARY_QUERYIDS ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "queryids acknowleged from %d to %d", sndr, rcpt );
      data->identack = True;
    }
    else if( rn[RN_PACKET_DATA + 0] == RN_SENSOR_REPORT ) {
      int port = rn[RN_PACKET_DATA + 1];
      if( port < 32 && data->ports[port] != NULL ) {
        data->ports[port]->acktimer = 0;
        data->ports[port]->ackretry = 0;
        data->ports[port]->ackpending = False;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensor %d acknowleged from %d to %d", port, sndr, rcpt );
      }
    }
    break;

  case RN_STATIONARY_QUERYIDS:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "queryids request from %d to %d", sndr, rcpt );
    msg = allocMem(32);
    msg[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
    rnReceipientAddresToPacket( sndr, msg, 0 );
    rnSenderAddresToPacket( data->id, msg, 0 );
    msg[RN_PACKET_ACTION] = RN_STATIONARY_QUERYIDS;
    msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
    msg[RN_PACKET_LEN] = 4;
    msg[RN_PACKET_DATA+0] = RN_CLASS_RASPI_IO;
    msg[RN_PACKET_DATA+1] = 70;
    msg[RN_PACKET_DATA+2] = versionH;
    msg[RN_PACKET_DATA+3] = versionL;
    break;
  }
  return msg;
}

static byte* __handleOutput( iORocNetNode rocnetnode, byte* rn ) {
  iORocNetNodeData data       = Data(rocnetnode);
  int port       = rn[RN_PACKET_DATA + 3];
  int rcpt       = 0;
  int sndr       = 0;
  int action     = rnActionFromPacket(rn);
  int actionType = rnActionTypeFromPacket(rn);
  Boolean isThis = __isThis( rocnetnode, rn);
  byte* msg = NULL;

  rcpt = rnReceipientAddrFromPacket(rn, 0);
  sndr = rnSenderAddrFromPacket(rn, 0);

  switch( action ) {
  case RN_OUTPUT_SWITCH:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "output SWITCH(%s) port=%d %s action for %d%s from %d, %d data bytes",
        rnActionTypeString(rn), port, rn[RN_PACKET_DATA + 0] & RN_OUTPUT_ON ? "on":"off",
        rcpt, isThis?"(this)":"", sndr, rn[RN_PACKET_LEN] );
    if( port < 32 && data->ports[port] != NULL && rn[RN_PACKET_DATA + 0] & RN_OUTPUT_ON ) {
      data->ports[port]->offtimer = SystemOp.getTick();
      data->ports[port]->state = True;
    }
    if(data->ports[port] != NULL) {
      raspiWrite(data->ports[port]->ionr, rn[RN_PACKET_DATA + 0] & RN_OUTPUT_ON ? 1:0);
      msg = allocMem(32);
      msg[RN_PACKET_GROUP] = RN_GROUP_OUTPUT;
      rnReceipientAddresToPacket( 0, msg, 0 );
      rnSenderAddresToPacket( data->id, msg, 0 );
      msg[RN_PACKET_ACTION] = RN_OUTPUT_SWITCH;
      msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
      msg[RN_PACKET_LEN] = 4;
      msg[RN_PACKET_DATA + 0] = rn[RN_PACKET_DATA + 0] & RN_OUTPUT_ON ? 1:0;
      msg[RN_PACKET_DATA + 1] = 0;
      msg[RN_PACKET_DATA + 2] = 0;
      msg[RN_PACKET_DATA + 3] = port;
    }
  break;

  default:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unsupported action [%d]", action );
    break;
  }

  return msg;
}


static void __sendRN( iORocNetNode rocnetnode, byte* rn ) {
  iORocNetNodeData data = Data(rocnetnode);
  int rcpt = rnReceipientAddrFromPacket(rn, 0);
  char* str = StrOp.byteToStr(rn, 8 + rn[RN_PACKET_LEN]);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "write %s [%s] to %d", rnActionTypeString(rn), str, rcpt );
  StrOp.free(str);
  SocketOp.sendto( data->writeUDP, rn, 8 + rn[RN_PACKET_LEN], NULL, 0 );

}


static void __evaluateRN( iORocNetNode rocnetnode, byte* rn ) {
  iORocNetNodeData data = Data(rocnetnode);
  int group = rn[RN_PACKET_GROUP];
  byte* rnReply = NULL;
  int action = rnActionFromPacket(rn);
  int actionType = rnActionTypeFromPacket(rn);
  Boolean isThis = __isThis( rocnetnode, rn);
  byte* msg = NULL;

  int rcpt = rnReceipientAddrFromPacket(rn, 0);
  int sndr = rnSenderAddrFromPacket(rn, 0);

  if( isThis ) {
    char* str = StrOp.byteToStr(rn, 8 + rn[RN_PACKET_LEN]);
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "ignore %s [%s] from %d(self) to %d", rnActionTypeString(rn), str, sndr, rcpt );
    StrOp.free(str);
    return;
  }
  else if(rcpt != data->id && rcpt != 0) {
    char* str = StrOp.byteToStr(rn, 8 + rn[RN_PACKET_LEN]);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ignore %s [%s] from %d to %d; address does not match", rnActionTypeString(rn), str, sndr, rcpt );
    StrOp.free(str);
    return;
  }
  else {
    char* str = StrOp.byteToStr(rn, 8 + rn[RN_PACKET_LEN]);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "evaluate %s [%s] from %d to %d", rnActionTypeString(rn), str, sndr, rcpt );
    StrOp.free(str);
  }

  TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)rn, 8 + rn[RN_PACKET_LEN] );

  switch( group ) {
    case RN_GROUP_HOST:
      if( action == RN_HOST_SHUTDOWN ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "host shutdown from %d to %d", sndr, rcpt );
        data->identack = False;
      }
      break;

    case RN_GROUP_STATIONARY:
      rnReply = __handleStationary( rocnetnode, rn );
      break;

    case RN_GROUP_PT_STATIONARY:
      rnReply = __handlePTStationary( rocnetnode, rn );
      break;

    case RN_GROUP_OUTPUT:
      rnReply = __handleOutput( rocnetnode, rn );
      break;

    case RN_GROUP_INPUT:
      break;

    case RN_GROUP_CS:
      rnReply = __handleCS( rocnetnode, rn );
      break;

    case RN_GROUP_CLOCK:
      rnReply = __handleClock( rocnetnode, rn );
      break;

    default:
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "unsupported group [%d]", group );
      break;
  }

  if( rnReply != NULL ) {
    __sendRN(rocnetnode, rnReply);
    freeMem(rnReply);
  }
}


static void __scanner( void* threadinst ) {
  iOThread         th         = (iOThread)threadinst;
  iORocNetNode     rocnetnode = (iORocNetNode)ThreadOp.getParm( th );
  iORocNetNodeData data       = Data(rocnetnode);
  int inputVal[32];
  byte msg[256];
  int identwait = 0;


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RocNet scanner started" );

  MemOp.set( inputVal, 32*sizeof(int), 0);

  while( data->run ) {
    int i;
    if( data->iorc == 0 ) {
      for( i = 0; i < 32; i++ ) {
        if( data->ports[i] != NULL && data->ports[i]->type == 0 ) {
          if( data->ports[i]->delay > 0 && data->ports[i]->state ) {
            if( data->ports[i]->offtimer + data->ports[i]->delay <= SystemOp.getTick() ) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "pulse off port %d", i );
              data->ports[i]->state = False;
              raspiWrite(data->ports[i]->ionr, 0);

              msg[RN_PACKET_GROUP] = RN_GROUP_OUTPUT;
              rnSenderAddresToPacket( data->id, msg, 0 );
              msg[RN_PACKET_ACTION] = RN_STATIONARY_SINGLE_PORT;
              msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
              msg[RN_PACKET_LEN] = 4;
              msg[RN_PACKET_DATA + 0] = 0; /* off */
              msg[RN_PACKET_DATA + 1] = 0;
              msg[RN_PACKET_DATA + 2] = 0;
              msg[RN_PACKET_DATA + 3] = i;
              __sendRN(rocnetnode, msg);

            }
          }
        }

        /* Check for pending Ack */
        if( data->ports[i] != NULL && data->ports[i]->type == 1 && data->ports[i]->ackpending) {
          data->ports[i]->acktimer++;
          if( data->ports[i]->acktimer > 50 ) {
            data->ports[i]->ackretry++;
            data->ports[i]->acktimer = 0;
            if( data->ports[i]->ackretry <= 10 ) {
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no ack for port %d; resend", i );
              msg[RN_PACKET_GROUP] = RN_GROUP_SENSOR;
              msg[RN_PACKET_ACTION] = RN_SENSOR_REPORT;
              msg[RN_PACKET_LEN] = 4;
              msg[RN_PACKET_DATA+2] = data->ports[i]->state;
              msg[RN_PACKET_DATA+3] = i;
              rnSenderAddresToPacket( data->id, msg, 0 );
              __sendRN(rocnetnode, msg);
            }
            else {
              /* giving up */
              data->ports[i]->ackpending = False;
              data->ports[i]->ackretry = 0;
              data->ports[i]->acktimer = 0;
              TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "no ack for port %d", i );
            }
          }
        }


        if( data->ports[i] != NULL && data->ports[i]->type == 1 && !data->ports[i]->ackpending ) {
          int val = (raspiRead(data->ports[i]->ionr) ? 0:1);
          Boolean report = inputVal[i] != val;

          if( data->ports[i]->delay > 0 ) {
            report = False;
            if( val > 0 ) {
              data->ports[i]->offtimer = SystemOp.getTick();
              if( !data->ports[i]->state ) {
                data->ports[i]->state = True;
                TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "delayed on port %d", i );
                report = True;
              }
            }
            else if( data->ports[i]->state && data->ports[i]->offtimer + data->ports[i]->delay <= SystemOp.getTick() ) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "delayed off port %d", i );
              data->ports[i]->state = False;
              report = True;
            }
          }

          if( report ) {
            data->ports[i]->ackpending = True;
            data->ports[i]->ackretry = 0;
            data->ports[i]->acktimer = 0;
            inputVal[i] = val;
            msg[RN_PACKET_GROUP] = RN_GROUP_SENSOR;
            msg[RN_PACKET_ACTION] = RN_SENSOR_REPORT;
            msg[RN_PACKET_LEN] = 4;
            msg[RN_PACKET_DATA+2] = val;
            msg[RN_PACKET_DATA+3] = i;
            rnSenderAddresToPacket( data->id, msg, 0 );
            __sendRN(rocnetnode, msg);
            ThreadOp.sleep(raspiDummy()?500:10);
          }
        }
      }
    }

    if( !data->identack ) {
      identwait++;
      if( identwait > 100 ) {
        identwait = 0;
        msg[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
        rnReceipientAddresToPacket( 0, msg, 0 );
        rnSenderAddresToPacket( data->id, msg, 0 );
        msg[RN_PACKET_ACTION] = RN_STATIONARY_QUERYIDS;
        msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
        msg[RN_PACKET_LEN] = 5;
        msg[RN_PACKET_DATA+0] = RN_CLASS_RASPI_IO;
        msg[RN_PACKET_DATA+1] = 70;
        msg[RN_PACKET_DATA+2] = versionH;
        msg[RN_PACKET_DATA+3] = versionL;
        msg[RN_PACKET_DATA+4] = 16;
        __sendRN(rocnetnode, msg);
      }
    }

    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RocNet scanner stopped" );
}


static void __reader( void* threadinst ) {
  iOThread         th         = (iOThread)threadinst;
  iORocNetNode     rocnetnode = (iORocNetNode)ThreadOp.getParm( th );
  iORocNetNodeData data       = Data(rocnetnode);
  byte msg[256];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RocNet reader started" );

  while( data->run ) {
    SocketOp.recvfrom( data->readUDP, msg, 0x7F, NULL, NULL );
    __evaluateRN(rocnetnode, msg);
    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RocNet reader stopped" );
}


static void __listener( obj inst, iONode nodeC, int level ) {
  iORocNetNodeData data = Data(inst);
  if( nodeC != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "listener: %s", NodeOp.getName(nodeC) );
    NodeOp.base.del(nodeC);
  }
}

static Boolean __initDigInt(iORocNetNode inst) {
  iORocNetNodeData data = Data(inst);
  const char*  lib = wDigInt.getlib( data->digintini );
  const char*  iid = wDigInt.getiid( data->digintini );
  iIDigInt pDi = NULL;
  iOLib    pLib = NULL;
  LPFNROCGETDIGINT pInitFun = (void *) NULL;
  char* libpath = StrOp.fmt( "%s%c%s", ".", SystemOp.getFileSeparator(), lib );
  pLib = LibOp.inst( libpath );
  StrOp.free( libpath );
  if (pLib == NULL)
    return False;
  pInitFun = (LPFNROCGETDIGINT)LibOp.getProc(pLib,"rocGetDigInt");
  if (pInitFun == NULL)
    return False;
  data->pDI = pInitFun(data->digintini,TraceOp.get());
  data->pDI->setListener( (obj)data->pDI, (obj)inst, &__listener );

  return True;
}

static iONode __findPort(iORocNetNode inst, int port) {
  iORocNetNodeData data = Data(inst);
  iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
  iONode portsetup = NULL;

  if( rocnet != NULL ) {
    iONode portsetup = wRocNet.getportsetup(rocnet);
    while( portsetup != NULL ) {
      if( wPortSetup.getport(portsetup) == port ) {
        return portsetup;
      }
      portsetup = wRocNet.nextportsetup(rocnet, portsetup);
    }
  }

  return portsetup;
}

static void __initPorts(iORocNetNode inst) {
  iORocNetNodeData data = Data(inst);
  int iomap = 0;
  iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "read portsetups" );

  if( rocnet != NULL ) {
    iONode portsetup = wRocNet.getportsetup(rocnet);
    while( portsetup != NULL ) {
      int portnr = wPortSetup.getport(portsetup);
      if( portnr < 128 ) {
        iOPort port = allocMem( sizeof( struct Port) );
        port->port = portnr;
        port->ionr = wPortSetup.getionr(portsetup);
        port->delay = wPortSetup.getdelay(portsetup);
        port->type = wPortSetup.gettype(portsetup);
        port->invert = wPortSetup.isinvert(portsetup);
        data->ports[portnr] = port;
        if( wPortSetup.gettype(portsetup) == 1 )
          iomap |= (1 << port->ionr );

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "portsetup: port=%d ionr=%d type=%d delay=%d", port->port, port->ionr, port->type, port->delay );
      }
      portsetup = wRocNet.nextportsetup(rocnet, portsetup);
    }
  }
  /* I/O map: 0=output, 1=input*/
  data->iorc = raspiSetupIO(iomap);
}


static int _Main( iORocNetNode inst, int argc, char** argv ) {
  iORocNetNodeData data = Data(inst);
  iOTrace trc = NULL;
  Boolean cd = False;
  const char* tf = "rocnetnode";

  /* check commandline arguments */
  iOCmdLn     arg     = CmdLnOp.inst( argc, (const char**)argv );
  tracelevel  debug   = CmdLnOp.hasKey( arg, wCmdline.debug  ) ? TRCLEVEL_DEBUG:0;
  tracelevel  dump    = CmdLnOp.hasKey( arg, wCmdline.byte   ) ? TRCLEVEL_BYTE:0;
  tracelevel  parse   = CmdLnOp.hasKey( arg, wCmdline.parse  ) ? TRCLEVEL_PARSE:0;
  tracelevel  monitor = CmdLnOp.hasKey( arg, wCmdline.monitor) ? TRCLEVEL_MONITOR:0;
  tracelevel  info    = CmdLnOp.hasKey( arg, wCmdline.info   ) ? TRCLEVEL_INFO:0;


  /* Read the Inifile: */
  {
    char* iniXml = NULL;
    iODoc iniDoc = NULL;
    iOFile iniFile = FileOp.inst( ROCNETNODEINI, True );
    if( iniFile != NULL ) {
      iniXml = allocMem( FileOp.size( iniFile ) + 1 );
      FileOp.read( iniFile, iniXml, FileOp.size( iniFile ) );
      if( StrOp.len( iniXml ) == 0 )
        iniXml = StrOp.fmt( "<%s/>", "rocnetnode");
      FileOp.close( iniFile );
    }
    else {
      iniXml = StrOp.fmt( "<%s/>", "rocnetnode");
    }

    /* Parse the Inifile: */
    iniDoc = DocOp.parse( iniXml );
    if( iniDoc != NULL ) {
      data->ini = DocOp.getRootNode( iniDoc );
    }
    else {
      printf( "Invalid ini file! [%s]", ROCNETNODEINI );
      return -1;
    }
  }



  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Up and running the RocNetNode" );
  if( NodeOp.findNode(data->ini, wRocNet.name()) != NULL ) {
    data->id    = wRocNet.getid(NodeOp.findNode(data->ini, wRocNet.name()));
    data->addr  = wRocNet.getaddr(NodeOp.findNode(data->ini, wRocNet.name()));
    data->port  = wRocNet.getport(NodeOp.findNode(data->ini, wRocNet.name()));
    if( NodeOp.findNode(data->ini, wTrace.name()) != NULL ) {
      iONode traceini = NodeOp.findNode(data->ini, wTrace.name());
      tf = wTrace.getrfile(traceini);
      trc = TraceOp.inst( debug | dump | monitor | parse | TRCLEVEL_INFO | TRCLEVEL_WARNING | TRCLEVEL_CALC, tf, True );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "using ini setup" );

      if( wTrace.isdebug( traceini ) || debug )
        TraceOp.setLevel( trc, TraceOp.getLevel( trc ) | TRCLEVEL_DEBUG );
      if( wTrace.ismonitor( traceini ) || monitor )
        TraceOp.setLevel( trc, TraceOp.getLevel( trc ) | TRCLEVEL_MONITOR );
      if( wTrace.isbyte( traceini ) || dump )
        TraceOp.setLevel( trc, TraceOp.getLevel( trc ) | TRCLEVEL_BYTE );
      if( wTrace.isparse( traceini ) || parse )
        TraceOp.setLevel( trc, TraceOp.getLevel( trc ) | TRCLEVEL_PARSE );
      if( wTrace.iscalc( traceini ) )
        TraceOp.setLevel( trc, TraceOp.getLevel( trc ) | TRCLEVEL_CALC );
    }
    data->digintini = NodeOp.findNode(data->ini, wDigInt.name());
  }
  else {
    trc = TraceOp.inst( debug | dump | monitor | parse | TRCLEVEL_INFO | TRCLEVEL_WARNING | TRCLEVEL_CALC, tf, True );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "using default setup" );
    data->id    = 4711;
    data->addr  = "224.0.0.1";
    data->port  = 4321;
  }
  TraceOp.setAppID( trc, "r" );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  ID [%d]", data->id );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast address [%s]", data->addr );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast port    [%d]", data->port );
  data->readUDP = SocketOp.inst( data->addr, data->port, False, True, True );
  SocketOp.bind(data->readUDP);
  data->writeUDP = SocketOp.inst( data->addr, data->port, False, True, True );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  __initPorts(inst);
  __initDigInt(inst);

  data->run = True;
  data->reader = ThreadOp.inst( "rnreader", &__reader, __RocNetNode );
  ThreadOp.start( data->reader );
  data->scanner = ThreadOp.inst( "rnscanner", &__scanner, __RocNetNode );
  ThreadOp.start( data->scanner );

  /* Memory watcher */
  while( !bShutdown ) {
    static int cnt1 = 0;
    int cnt2 = MemOp.getAllocCount();
    if( cnt2 > cnt1 ) {
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "memory allocations old=%u new=%u", cnt1, cnt2 );
    }
    cnt1 = cnt2;
    ThreadOp.sleep( 1000 );
  }

  return 0;
}


static Boolean _shutdown( void ) {
  iORocNetNodeData data = Data(__RocNetNode);
  byte msg[32];
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutdown the RocNetNode" );

  msg[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
  rnReceipientAddresToPacket( 0, msg, 0 );
  rnSenderAddresToPacket( data->id, msg, 0 );
  msg[RN_PACKET_ACTION] = RN_STATIONARY_SHUTDOWN;
  msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
  msg[RN_PACKET_LEN] = 0;
  __sendRN(__RocNetNode, msg);

  data->run = False;
  ThreadOp.sleep(1000);
  bShutdown = True;
  return False;
}


static void _stop( void ) {
}

/**  */
static struct ORocNetNode* _inst( iONode ini ) {
  printf( "Initialize the AccNode......\n" );
  if( __RocNetNode == NULL ) {
    __RocNetNode = allocMem( sizeof( struct ORocNetNode ) );
    iORocNetNodeData data = allocMem( sizeof( struct ORocNetNodeData ) );
    MemOp.basecpy( __RocNetNode, &RocNetNodeOp, 0, sizeof( struct ORocNetNode ), data );

    /* Initialize data->xxx members... */
    SystemOp.inst();

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Instance created of the RocNetNode" );

  }
  return __RocNetNode;
}




/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocnetnode/impl/rocnetnode.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
