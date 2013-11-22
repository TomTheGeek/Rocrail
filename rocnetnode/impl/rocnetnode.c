/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

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
#include "rocrail/wrapper/public/ConCmd.h"
#include "rocrail/wrapper/public/RocNet.h"
#include "rocrail/wrapper/public/PortSetup.h"
#include "rocrail/wrapper/public/ChannelSetup.h"
#include "rocrail/wrapper/public/RocNetNodeOptions.h"
#include "rocrail/wrapper/public/Trace.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Macro.h"
#include "rocrail/wrapper/public/MacroLine.h"

#include "rocnetnode/impl/rocnetnode_impl.h"

#include "rocint/public/digint.h"

#include "rocdigs/impl/rocnet/rocnet-const.h"
#include "rocdigs/impl/rocnet/rn-utils.h"

#include "rocnetnode/public/io.h"
#include "rocnetnode/public/i2c.h"
#include "rocnetnode/public/ip.h"

#include "common/version.h"

#define ROCNETNODEINI "rocnetnode.ini"

static int instCnt = 0;

/*
 * LED1: GPIO 23, normal operation
 * LED2: GPIO 24, alert
 * PB1 : GPIO 25, identify
 */

typedef iIDigInt (* LPFNROCGETDIGINT)( const iONode ,const iOTrace );

static void __sendRN( iORocNetNode rocnetnode, byte* rn );
static iONode __findPort(iORocNetNode inst, int port);
static void __initI2C(iORocNetNode inst, int iotype);
static void __initI2CPWM(iORocNetNode inst);
static void __writePort(iORocNetNode rocnetnode, int port, int value, int iotype);
static void __writeChannel(iORocNetNode rocnetnode, int channel, int value);
static int __readPort(iORocNetNode rocnetnode, int port, int iotype);
static void __saveIni(iORocNetNode rocnetnode);
static void __initControl(iORocNetNode inst);
static void __initIO(iORocNetNode rocnetnode);
static iONode __findMacro(iORocNetNode inst, int nr);
static Boolean __initDigInt(iORocNetNode inst);
static void __unloadDigInt(iORocNetNode inst, int prevcstype);
static void __errorReport( iORocNetNode inst, int rc, int rs, int addr);
static iONode __findChannel(iORocNetNode inst, int channel);


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
  int prot = 0;
  int spcnt = 0;
  int i = 0;

  rcpt = rnReceipientAddrFromPacket(rn, 0);
  sndr = rnSenderAddrFromPacket(rn, 0);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "CS %s %d from %d to %d", (actionType == RN_ACTIONTYPE_EVENT)?"event":"request", action, sndr, rcpt );

  switch( action ) {
    case RN_CS_TRACKPOWER:
      if( actionType == RN_ACTIONTYPE_EVENT ) {
        break;
      }
      if( (rn[RN_PACKET_DATA + 0] & 0x01) == 0x00) {
        /* save at power off to persistent the output state */
        __saveIni(rocnetnode);
      }

      if(data->cstype > 0 && data->pDI != NULL) {
        iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE);
        wSysCmd.setcmd(cmd, rn[RN_PACKET_DATA + 0] & 0x01 ? wSysCmd.go:wSysCmd.stop);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CS track power %s", rn[RN_PACKET_DATA + 0] & 0x01 ? "ON":"OFF" );
        data->pDI->cmd( (obj)data->pDI, cmd );
        data->power = rn[RN_PACKET_DATA + 0] & 0x01 ? True:False;
        msg = allocMem(32);
        msg[RN_PACKET_NETID] = data->location;
        msg[RN_PACKET_GROUP] = RN_GROUP_CS;
        rnSenderAddresToPacket( data->id, msg, 0 );
        msg[RN_PACKET_ACTION] = RN_CS_TRACKPOWER;
        msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
        msg[RN_PACKET_LEN] = 1;
        msg[RN_PACKET_DATA + 0] = data->power;
      }
      break;

    case RN_CS_POM:
      if(data->cstype > 0 && data->pDI != NULL) {
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
      if(data->cstype > 0 && data->pDI != NULL) {
        addr = rn[RN_PACKET_DATA + 0] * 256 + rn[RN_PACKET_DATA + 1];
        V = rn[RN_PACKET_DATA + 2];
        dir = rn[RN_PACKET_DATA + 3];
        lights = rn[RN_PACKET_DATA + 4];
        prot = rn[RN_PACKET_DATA + 5];
        spcnt = rn[RN_PACKET_DATA + 6];
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco addr=%d V=%d dir=%d lights=%d spcnt=%d", addr, V, dir, lights, spcnt );
        if(data->pDI != NULL) {
          iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE);
          wLoc.setaddr(cmd, addr);
          wLoc.setV(cmd, V);
          wLoc.setspcnt(cmd, spcnt);
          wLoc.setdir(cmd, dir);
          wLoc.setfn(cmd, lights);
          data->pDI->cmd( (obj)data->pDI, cmd );
        }
      }
      break;

    case RN_CS_FUNCTION:
      if(data->cstype > 0 && data->pDI != NULL) {
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
      }
      break;
  }

  return msg;
}

static void __saveIni(iORocNetNode rocnetnode) {
  iORocNetNodeData data = Data(rocnetnode);
  iOFile iniFile = FileOp.inst( data->inifile, OPEN_WRITE );
  iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());

  int i = 0;

  if( rocnet == NULL ) {
    rocnet = NodeOp.inst(wRocNet.name(), data->ini, ELEMENT_NODE);
    NodeOp.addChild( data->ini, rocnet );
  }
  wRocNet.setid(rocnet, data->id);
  wRocNet.setnet(rocnet, data->location);

  for( i = 0; i < 129; i++ ) {
    if( data->ports[i] != NULL && (data->ports[i]->type&IO_TYPE) == 0 ) {
      iONode portsetup = __findPort(rocnetnode, i);
      if( portsetup != NULL ) {
        wPortSetup.setstate(portsetup, data->ports[i]->state);
      }
    }
    if( data->channels[i] != NULL ) {
      iONode channelsetup = __findChannel(rocnetnode, i);
      if( channelsetup != NULL ) {
        wChannelSetup.setstate(channelsetup, data->channels[i]->state);
      }
    }
  }

  for( i = 1; i < 129; i++ ) {
    if( data->macro[i] != NULL ) {
      iONode macro = __findMacro(rocnetnode, i);
      if( macro == NULL ) {
        macro = NodeOp.inst(wMacro.name(), data->ini, ELEMENT_NODE);
        wMacro.setnr(macro, i);
        NodeOp.addChild(data->ini, macro);
      }
      if( macro != NULL ) {
        int n = 0;
        for( n = 0; n < 8; n++ ) {
          iONode line = wMacro.getmacroline(macro);
          if( line != NULL )
            NodeOp.removeChild(macro, line);
        }
        for( n = 0; n < 8; n++ ) {
          if( data->macro[i]->line[n].port > 0 ) {
            iONode line = NodeOp.inst(wMacroLine.name(), macro, ELEMENT_NODE);
            NodeOp.addChild(macro, line);
            wMacroLine.setport(line, data->macro[i]->line[n].port);
            wMacroLine.setporttype(line, data->macro[i]->line[n].type);
            wMacroLine.setdelay(line, data->macro[i]->line[n].delay);
            wMacroLine.setstatus(line, data->macro[i]->line[n].value);
            wMacroLine.setblink(line, data->macro[i]->line[n].blink);
          }
        }
      }
    }
  }


  TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) & ~TRCLEVEL_INFO );
  if( data->tl_info )
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) | TRCLEVEL_INFO );

  TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) & ~TRCLEVEL_MONITOR );
  if( data->tl_monitor )
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) | TRCLEVEL_MONITOR );

  if( NodeOp.findNode(data->ini, wTrace.name()) == NULL ) {
    iONode traceini = NodeOp.inst(wTrace.name(), data->ini, ELEMENT_NODE);
    NodeOp.addChild(data->ini, traceini);
  }

  if( NodeOp.findNode(data->ini, wTrace.name()) != NULL ) {
    iONode traceini = NodeOp.findNode(data->ini, wTrace.name());
    wTrace.setinfo(traceini, TraceOp.getLevel( NULL ) & TRCLEVEL_INFO );
    wTrace.setmonitor(traceini, TraceOp.getLevel( NULL ) & TRCLEVEL_MONITOR );
  }

  if( iniFile != NULL ) {
    char* iniStr = NodeOp.base.toString( data->ini );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "saving ini %s...", data->inifile );
    FileOp.write( iniFile, iniStr, StrOp.len( iniStr ) );
    FileOp.flush( iniFile );
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
    if( rcpt != data->id || (rn[RN_PACKET_LEN] > 2 &&  rn[RN_PACKET_DATA+2] != data->ip[data->ipsize-2]) || rn[RN_PACKET_DATA+3] != data->ip[data->ipsize-1] )
    {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "new ID %d not for me", rn[RN_PACKET_DATA + 0] * 256 + rn[RN_PACKET_DATA + 1] );
      break;
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "new ID %d Location %d for me", rn[RN_PACKET_DATA + 0] * 256 + rn[RN_PACKET_DATA + 1], rn[RN_PACKET_DATA + 4] );
    msg = allocMem(128);
    msg[RN_PACKET_NETID] = data->location;
    msg[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
    rnReceipientAddresToPacket( 0, msg, 0 );
    rnSenderAddresToPacket( data->id, msg, 0 );
    msg[RN_PACKET_ACTION] = RN_STATIONARY_SHUTDOWN;
    msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
    msg[RN_PACKET_LEN] = 0;

    data->id = rn[RN_PACKET_DATA + 0] * 256 + rn[RN_PACKET_DATA + 1];
    data->location = rn[RN_PACKET_DATA + 4];
    if( data->id < 2 ) {
      data->id = 65535;
    }

    /* Save the rocnetnode.ini to persistent the new ID. */
    {
      iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
      wRocNet.setid(rocnet, data->id);
      __saveIni(rocnetnode);
    }
    data->identack = False;
    break;

  case RN_PROGRAMMING_WMACRO:
  {
    int i = 0;
    int nr = rn[RN_PACKET_DATA+0];
    iOMacro macro = data->macro[nr];
    if( macro == NULL ) {
      data->macro[nr] = allocMem(sizeof(struct Macro));
      macro = data->macro[nr];
    }

    for( i = 0; i < 8; i++ ) {
      macro->line[i].port  = rn[RN_PACKET_DATA+1+i*4];
      macro->line[i].delay = rn[RN_PACKET_DATA+2+i*4];
      macro->line[i].type  = (rn[RN_PACKET_DATA+3+i*4]&IO_TYPE);
      macro->line[i].blink = ((rn[RN_PACKET_DATA+3+i*4]&IO_BLINK)?True:False);
      macro->line[i].value = rn[RN_PACKET_DATA+4+i*4];
    }
    __saveIni(rocnetnode);
  }
  break;

  case RN_PROGRAMMING_RMACRO:
  {
    int i = 0;
    int nr = rn[RN_PACKET_DATA+0];
    iOMacro macro = data->macro[nr];
    if( macro == NULL ) {
      data->macro[nr] = allocMem(sizeof(struct Macro));
      macro = data->macro[nr];
    }

    msg = allocMem(128);
    msg[RN_PACKET_NETID] = data->location;
    msg[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
    rnReceipientAddresToPacket( sndr, msg, 0 );
    rnSenderAddresToPacket( data->id, msg, 0 );
    msg[RN_PACKET_ACTION] = RN_PROGRAMMING_RMACRO;
    msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
    msg[RN_PACKET_LEN] = 1 + 8*4;
    msg[RN_PACKET_DATA+0] = nr;
    for( i = 0; i < 8; i++ ) {
      msg[RN_PACKET_DATA+1+i*4] = macro->line[i].port ;
      msg[RN_PACKET_DATA+2+i*4] = macro->line[i].delay;
      msg[RN_PACKET_DATA+3+i*4] = macro->line[i].type + (macro->line[i].blink?IO_BLINK:0x00);
      msg[RN_PACKET_DATA+4+i*4] = macro->line[i].value;
    }
  }
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
      int res   = rn[RN_PACKET_DATA+1+i*4];
      int type  = rn[RN_PACKET_DATA+2+i*4];
      int delay = rn[RN_PACKET_DATA+3+i*4];
      iONode portsetup = __findPort(rocnetnode, port);
      if( portsetup == NULL ) {
        portsetup = NodeOp.inst( wPortSetup.name(), rocnet, ELEMENT_NODE);
        wPortSetup.setport( portsetup, port);
        NodeOp.addChild( rocnet, portsetup );
      }
      wPortSetup.settype( portsetup, type);
      wPortSetup.setdelay( portsetup, delay);
    }
    __saveIni(rocnetnode);
    __initIO(rocnetnode);
  }
  break;

  case RN_PROGRAMMING_SETCHANNEL:
  {
    int channel = rn[RN_PACKET_DATA+0];
    int pos     = rn[RN_PACKET_DATA+1]*256 + rn[RN_PACKET_DATA+2];
    int type    = rn[RN_PACKET_DATA+3];
    if( data->channels[channel] != NULL ) {
      data->channels[i]->ready = False;
      data->channels[i]->sleep = False;
      data->channels[i]->idle  = 0;
      if( type == 0 ) {
        data->channels[channel]->offpos = pos;
        data->channels[channel]->state  = 0;
      }
      else {
        data->channels[channel]->onpos = pos;
        data->channels[channel]->state  = 1;
      }
    }
  }
  break;

  case RN_PROGRAMMING_WCHANNEL:
  {
    iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
    int i = 0;

    if( rocnet == NULL ) {
      rocnet = NodeOp.inst( wRocNet.name(), data->ini, ELEMENT_NODE);
      NodeOp.addChild( data->ini, rocnet );
    }

    for( i = 0; i < 8; i++ ) {
      int channel    = rn[RN_PACKET_DATA+0+i*8];
      int offpos   = rn[RN_PACKET_DATA+1+i*8]*256 + rn[RN_PACKET_DATA+2+i*8];
      int onpos    = rn[RN_PACKET_DATA+3+i*8]*256 + rn[RN_PACKET_DATA+4+i*8];
      int offsteps = rn[RN_PACKET_DATA+5+i*8];
      int onsteps  = rn[RN_PACKET_DATA+6+i*8];
      int options  = rn[RN_PACKET_DATA+7+i*8];
      iONode channelsetup = __findChannel(rocnetnode, channel);
      if( channelsetup == NULL ) {
        channelsetup = NodeOp.inst( wChannelSetup.name(), rocnet, ELEMENT_NODE);
        wChannelSetup.setchannel( channelsetup, channel);
        NodeOp.addChild( rocnet, channelsetup );
      }
      wChannelSetup.setoffpos( channelsetup, offpos);
      wChannelSetup.setonpos( channelsetup, onpos);
      wChannelSetup.setoffsteps( channelsetup, offsteps);
      wChannelSetup.setonsteps( channelsetup, onsteps);
      wChannelSetup.setoptions( channelsetup, options);
    }
    __saveIni(rocnetnode);
    __initIO(rocnetnode);
  }
  break;

  case RN_PROGRAMMING_RMPORT:
  {
    iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
    int i = 0;

    if( rocnet == NULL ) {
      rocnet = NodeOp.inst( wRocNet.name(), data->ini, ELEMENT_NODE);
      NodeOp.addChild( data->ini, rocnet );
    }

    for( i = 0; i < 8; i++ ) {
      int port  = rn[RN_PACKET_DATA+0+i];
      iONode portsetup = __findPort(rocnetnode, port);
      if( portsetup != NULL ) {
        iOPort portdef = data->ports[port];
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "removing port %d", port);
        NodeOp.removeChild( rocnet, portsetup );
        NodeOp.base.del(portsetup);

        if( portdef != NULL ) {
          data->ports[port] = NULL;
          freeMem(portdef);
        }
      }
    }
    __saveIni(rocnetnode);
    __initIO(rocnetnode);
  }
  break;

  case RN_PROGRAMMING_RMCHANNEL:
  {
    iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
    int i = 0;

    if( rocnet == NULL ) {
      rocnet = NodeOp.inst( wRocNet.name(), data->ini, ELEMENT_NODE);
      NodeOp.addChild( data->ini, rocnet );
    }

    for( i = 0; i < 8; i++ ) {
      int channel  = rn[RN_PACKET_DATA+0+i];
      iONode channelsetup = __findChannel(rocnetnode, channel);
      if( channelsetup != NULL ) {
        iOChannel channeldef = data->channels[channel];
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "removing channel %d", channel);
        NodeOp.removeChild( rocnet, channelsetup );
        NodeOp.base.del(channelsetup);

        if( channeldef != NULL ) {
          data->channels[channel] = NULL;
          freeMem(channeldef);
        }
      }
    }
    __saveIni(rocnetnode);
    __initIO(rocnetnode);
  }
  break;

  case RN_PROGRAMMING_WPORTEVENT:
  {
    iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
    int i = 0;

    if( rocnet == NULL ) {
      rocnet = NodeOp.inst( wRocNet.name(), data->ini, ELEMENT_NODE);
      NodeOp.addChild( data->ini, rocnet );
    }

    for( i = 0; i < 8; i++ ) {
      int port  = rn[RN_PACKET_DATA+0+i*4];
      int eventid = rn[RN_PACKET_DATA+1+i*4]*256+rn[RN_PACKET_DATA+2+i*4];
      int eventport = rn[RN_PACKET_DATA+3+i*4];
      iONode portsetup = __findPort(rocnetnode, port);
      if( portsetup == NULL ) {
        portsetup = NodeOp.inst( wPortSetup.name(), rocnet, ELEMENT_NODE);
        wPortSetup.setport( portsetup, port);
        NodeOp.addChild( rocnet, portsetup );
      }
      wPortSetup.seteventid( portsetup, eventid);
      wPortSetup.seteventport( portsetup, eventport);

      if( data->ports[port] != NULL ) {
        data->ports[port]->eventid= eventid;
        data->ports[port]->eventport= eventport;
      }

    }
    __saveIni(rocnetnode);
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
    msg[RN_PACKET_NETID] = data->location;
    msg[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
    rnReceipientAddresToPacket( sndr, msg, 0 );
    rnSenderAddresToPacket( data->id, msg, 0 );
    msg[RN_PACKET_ACTION] = RN_PROGRAMMING_RPORT;
    msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
    msg[RN_PACKET_LEN] = ((to-from)+1)*4;
    for( i = from; i <= to; i++ ) {
      if( data->ports[i] != NULL ) {
        msg[RN_PACKET_DATA + 0 + idx * 4] = i;
        msg[RN_PACKET_DATA + 1 + idx * 4] = data->ports[i]->state;
        msg[RN_PACKET_DATA + 2 + idx * 4] = data->ports[i]->type;
        msg[RN_PACKET_DATA + 3 + idx * 4] = data->ports[i]->delay;
      }
      idx++;
    }
    break;

  case RN_PROGRAMMING_RCHANNEL:
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
    msg[RN_PACKET_NETID] = data->location;
    msg[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
    rnReceipientAddresToPacket( sndr, msg, 0 );
    rnSenderAddresToPacket( data->id, msg, 0 );
    msg[RN_PACKET_ACTION] = RN_PROGRAMMING_RCHANNEL;
    msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
    msg[RN_PACKET_LEN] = ((to-from)+1)*8;
    for( i = from; i <= to; i++ ) {
      if( data->channels[i] != NULL ) {
        msg[RN_PACKET_DATA + 0 + idx * 8] = i;
        msg[RN_PACKET_DATA + 1 + idx * 8] = data->channels[i]->offpos/256;
        msg[RN_PACKET_DATA + 2 + idx * 8] = data->channels[i]->offpos%256;
        msg[RN_PACKET_DATA + 3 + idx * 8] = data->channels[i]->onpos/256;
        msg[RN_PACKET_DATA + 4 + idx * 8] = data->channels[i]->onpos%256;
        msg[RN_PACKET_DATA + 5 + idx * 8] = data->channels[i]->offsteps;
        msg[RN_PACKET_DATA + 6 + idx * 8] = data->channels[i]->onsteps;
        msg[RN_PACKET_DATA + 7 + idx * 8] = data->channels[i]->options;
      }
      idx++;
    }
    break;

  case RN_PROGRAMMING_RPORTEVENT:
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
    msg[RN_PACKET_NETID] = data->location;
    msg[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
    rnReceipientAddresToPacket( sndr, msg, 0 );
    rnSenderAddresToPacket( data->id, msg, 0 );
    msg[RN_PACKET_ACTION] = RN_PROGRAMMING_RPORTEVENT;
    msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
    msg[RN_PACKET_LEN] = ((to-from)+1)*4;
    for( i = from; i <= to; i++ ) {
      if( data->ports[i] != NULL ) {
        msg[RN_PACKET_DATA + 0 + idx * 4] = i;
        msg[RN_PACKET_DATA + 1 + idx * 4] = data->ports[i]->eventid/256;
        msg[RN_PACKET_DATA + 2 + idx * 4] = data->ports[i]->eventid%256;
        msg[RN_PACKET_DATA + 3 + idx * 4] = data->ports[i]->eventport;
      }
      idx++;
    }
    break;

  case RN_PROGRAMMING_UPDATE:
    RocNetNodeOp.sysUpdate( rn[RN_PACKET_DATA + 0] * 256 + rn[RN_PACKET_DATA + 1], rn[RN_PACKET_DATA + 2] );
    break;

  case RN_PROGRAMMING_RDOPT:
    msg = allocMem(128);
    msg[RN_PACKET_NETID] = data->location;
    msg[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
    rnReceipientAddresToPacket( sndr, msg, 0 );
    rnSenderAddresToPacket( data->id, msg, 0 );
    msg[RN_PACKET_ACTION] = RN_PROGRAMMING_RDOPT;
    msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
    msg[RN_PACKET_LEN] = 4;
    msg[RN_PACKET_DATA + 0] = data->iotype;
    msg[RN_PACKET_DATA + 1] = (data->sack ? 0x01:0x00) | (data->rfid ? 0x02:0x00) | (data->usepb ? 0x04:0x00);
    msg[RN_PACKET_DATA + 1] |= (data->tl_info ? 0x10:0x00) | (data->tl_monitor ? 0x20:0x00);
    msg[RN_PACKET_DATA + 2] = data->cstype;
    msg[RN_PACKET_DATA + 3] = data->csdevice;
    break;

  case RN_PROGRAMMING_WROPT:
  {
    int prevcstype = data->csdevice;
    data->iotype = rn[RN_PACKET_DATA + 0];
    data->sack   = (rn[RN_PACKET_DATA + 1] & 0x01) ? True:False;
    data->rfid   = (rn[RN_PACKET_DATA + 1] & 0x02) ? True:False;
    data->usepb = (rn[RN_PACKET_DATA + 1] & 0x04) ? True:False;
    data->tl_info = (rn[RN_PACKET_DATA + 1] & 0x10) ? True:False;
    data->tl_monitor = (rn[RN_PACKET_DATA + 1] & 0x20) ? True:False;
    data->cstype   = rn[RN_PACKET_DATA + 2];
    data->csdevice = rn[RN_PACKET_DATA + 3];
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "options: iotype=%d cstype=%d csdevice=%d", data->iotype, data->cstype, data->csdevice );
    msg = allocMem(128);
    msg[RN_PACKET_NETID] = data->location;
    msg[RN_PACKET_GROUP] = RN_GROUP_PT_STATIONARY;
    rnReceipientAddresToPacket( sndr, msg, 0 );
    rnSenderAddresToPacket( data->id, msg, 0 );
    msg[RN_PACKET_ACTION] = RN_PROGRAMMING_WROPT;
    msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
    msg[RN_PACKET_LEN] = 4;
    msg[RN_PACKET_DATA + 0] = data->iotype;
    msg[RN_PACKET_DATA + 1] = (data->sack ? 0x01:0x00) | (data->rfid ? 0x02:0x00) | (data->usepb ? 0x04:0x00);
    msg[RN_PACKET_DATA + 2] = data->cstype;
    msg[RN_PACKET_DATA + 3] = data->csdevice;
    /* Save the rocnetnode.ini to persistent the new ID. */
    {
      iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
      iONode optionsini = NodeOp.findNode(rocnet, wRocNetNodeOptions.name());

      if( wRocNetNodeOptions.getiotype(optionsini) != data->iotype )
        __initIO(rocnetnode);

      wRocNetNodeOptions.setiotype( optionsini, data->iotype );
      wRocNetNodeOptions.setsack( optionsini, data->sack );
      wRocNetNodeOptions.setrfid( optionsini, data->rfid );
      wRocNetNodeOptions.setusepb( optionsini, data->usepb );

      if( !data->rfid || data->cstype !=  prevcstype ) {
        __unloadDigInt(rocnetnode, prevcstype);
        if( data->cstype == 0 && data->digintini != NULL ) {
          NodeOp.removeChild(data->ini, data->digintini );
          NodeOp.base.del(data->digintini);
          data->digintini = NULL;
        }
      }

      if( data->cstype > 0) {
        if( data->digintini == NULL ) {
          data->digintini = NodeOp.inst(wDigInt.name(), data->ini, ELEMENT_NODE);
          NodeOp.addChild(data->ini, data->digintini);
        }

        wDigInt.setiid(data->digintini, "dcc");

        if( data->cstype==1 )
          wDigInt.setlib(data->digintini, wDigInt.dcc232);
        else if( data->cstype==2 )
          wDigInt.setlib(data->digintini, wDigInt.sprog);

        if( data->csdevice==1 )
          wDigInt.setdevice(data->digintini, "/dev/ttyUSB1");
        else if( data->csdevice==2 )
          wDigInt.setdevice(data->digintini, "/dev/ttyACM0");
        else
          wDigInt.setdevice(data->digintini, "/dev/ttyUSB0");

        __initDigInt(rocnetnode);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "%s: lib=%s device=%s", wDigInt.getiid(data->digintini), wDigInt.getlib(data->digintini), wDigInt.getdevice(data->digintini) );
      }
    }
    __saveIni(rocnetnode);
  }
  break;


  }

  return msg;
}


static void _sysHalt(void) {
  iORocNetNodeData data = Data(__RocNetNode);
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "shutdown the system...");
  SystemOp.system("halt -p", True, True);
}


static void _sysUpdate(int revision, int offline) {
  iORocNetNodeData data = Data(__RocNetNode);
  char cmd[256] = {'\0'};
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "update the software to revision %d...", revision);
  StrOp.fmtb(cmd, "nohup /opt/rocnet/update%s.sh %d &", offline?"-offline":"", revision );
  SystemOp.system(cmd, True, True);
}


static void __macro(iORocNetNode rocnetnode, int macro, Boolean on, int offset) {
  iORocNetNodeData data = Data(rocnetnode);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "macro %d %s", macro, on?"ON":"OFF");
  if( offset > 0 )
    offset--;
  if( on && data->macro[macro] != NULL ) {
    int i = 0;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "processing macro %d %s offset=%d", macro, on?"ON":"OFF", offset);
    for( i = 0; i < 8; i++ ) {
      if( data->macro[macro]->line[i].port > 0 && data->macro[macro]->line[i].type == 0) {
        int port = data->macro[macro]->line[i].port;
        port += offset;

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "processing macro line %d port=%d delay=%d value=%d blink=%d", i,
            data->macro[macro]->line[i].port, data->macro[macro]->line[i].delay, data->macro[macro]->line[i].value, data->macro[macro]->line[i].blink);

        if( on && data->macro[macro]->line[i].blink ) {
          if( data->ports[port] != NULL ) {
            data->ports[port]->type |= IO_BLINK;
            data->ports[port]->offtimer = SystemOp.getTick();
            data->ports[port]->delay = data->macro[macro]->line[i].delay;
          }
        }
        else {
          if( data->ports[port] != NULL ) {
            data->ports[port]->type &= IO_TYPE;
            data->ports[port]->delay = 0;
            ThreadOp.sleep( data->macro[macro]->line[i].delay * 10);
          }
        }

        if( (data->macro[macro]->line[i].type&IO_TYPE) == 0 && data->ports[port] != NULL ) {
          __writePort( rocnetnode, port, data->macro[macro]->line[i].value, 2);
          data->ports[port]->state = (data->macro[macro]->line[i].value);
        }
        else if( (data->macro[macro]->line[i].type&IO_TYPE) == 2 && data->channels[port] != NULL) {
          int pos = (data->macro[macro]->line[i].value ? data->channels[port]->onpos:data->channels[port]->offpos);
          if( data->macro[macro]->line[i].value > 1 ) {
              data->channels[port]->onpos = data->macro[macro]->line[i].value * 16;
          }
          data->channels[i]->ready = False;
          data->channels[i]->sleep = False;
          data->channels[i]->idle  = 0;
          data->channels[port]->state = (data->macro[macro]->line[i].value?1:0);
        }
      }

    }
  }

}

static void __macroProcessor( void* threadinst ) {
  iOThread         th         = (iOThread)threadinst;
  iORocNetNode     rocnetnode = (iORocNetNode)ThreadOp.getParm( th );
  iORocNetNodeData data       = Data(rocnetnode);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "macroProcessor started");
  do {
    byte* post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      __macro(rocnetnode, post[0], post[1], post[2]);
      freeMem( post);
    }
    ThreadOp.sleep(10);
  } while(data->run);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "macroProcessor ended");

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

  if( actionType == RN_ACTIONTYPE_EVENT ) {
    /* ignore */
    return NULL;
  }

  switch( action ) {
  case RN_STATIONARY_ACK:
    if( rn[RN_PACKET_DATA + 0] == RN_STATIONARY_IDENTIFY ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "queryids acknowleged from %d to %d", sndr, rcpt );
      data->identack = True;
      data->startofday = True;
    }
    else if( rn[RN_PACKET_DATA + 0] == RN_SENSOR_REPORT ) {
      int port = rn[RN_PACKET_DATA + 1];
      if( port < 128 && data->ports[port] != NULL ) {
        data->ports[port]->acktimer = 0;
        data->ports[port]->ackretry = 0;
        data->ports[port]->ackpending = False;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensor %d acknowleged from %d to %d", port, sndr, rcpt );
      }
    }
    break;

  case RN_STATIONARY_IDENTIFY:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "queryids request from %d to %d", sndr, rcpt );
    msg = allocMem(32);
    msg[RN_PACKET_NETID] = data->location;
    msg[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
    rnReceipientAddresToPacket( sndr, msg, 0 );
    rnSenderAddresToPacket( data->id, msg, 0 );
    msg[RN_PACKET_ACTION] = RN_STATIONARY_IDENTIFY;
    msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
    msg[RN_PACKET_LEN] = 7;
    msg[RN_PACKET_DATA+0] = data->class;
    msg[RN_PACKET_DATA+1] = 70;
    msg[RN_PACKET_DATA+2] = revisionnr/256;
    msg[RN_PACKET_DATA+3] = revisionnr%256;
    msg[RN_PACKET_DATA+4] = 128;
    msg[RN_PACKET_DATA+5] = data->ip[data->ipsize-2];
    msg[RN_PACKET_DATA+6] = data->ip[data->ipsize-1];
    break;

  case RN_STATIONARY_STARTOFDAY:
    data->startofday = True;
    break;

  case RN_STATIONARY_SHUTDOWN:
    if( rn[RN_PACKET_LEN] > 0 && rn[RN_PACKET_DATA+0] == 1 ) {
      RocNetNodeOp.sysHalt();
    }
    break;

  case RN_STATIONARY_SHOW:
    /* Flash LED. */
    if( data->show )
      data->show = False;
    else {
      data->LED2timer = 0;
      data->show = True;
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "show LED2 %s", data->show?"on":"off" );
    break;

  }
  return msg;
}

static byte* __handleOutput( iORocNetNode rocnetnode, byte* rn ) {
  iORocNetNodeData data       = Data(rocnetnode);
  int port       = rn[RN_PACKET_DATA + 3];
  int offset     = rn[RN_PACKET_DATA + 4];
  int rcpt       = 0;
  int sndr       = 0;
  int action     = rnActionFromPacket(rn);
  int actionType = rnActionTypeFromPacket(rn);
  Boolean isThis = __isThis( rocnetnode, rn);
  byte* msg = NULL;

  rcpt = rnReceipientAddrFromPacket(rn, 0);
  sndr = rnSenderAddrFromPacket(rn, 0);

  if( actionType == RN_ACTIONTYPE_EVENT ) {
    return NULL;
  }

  switch( action ) {
  case RN_OUTPUT_SWITCH:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "output SWITCH(%s) port=%d type=%d %s action for %d%s from %d, %d data bytes",
        rnActionTypeString(rn), port, rn[RN_PACKET_DATA + 1], rn[RN_PACKET_DATA + 0] & RN_OUTPUT_ON ? "on":"off",
        rcpt, isThis?"(this)":"", sndr, rn[RN_PACKET_LEN] );
    if( rn[RN_PACKET_DATA + 1] == wProgram.porttype_macro ) {
      byte* post = allocMem(32);
      post[0] = port;
      post[1] = ((rn[RN_PACKET_DATA + 0] & RN_OUTPUT_ON) ?1:0);
      post[2] = offset;
      ThreadOp.post(data->macroprocessor, (obj)post);
    }
    else if( rn[RN_PACKET_DATA + 1] == wProgram.porttype_servo ) {
      if( port < 129 && data->channels[port] != NULL ) {
        data->channels[port]->state = (rn[RN_PACKET_DATA + 0] & RN_OUTPUT_ON) ? 1:0;
        data->channels[port]->blink = (rn[RN_PACKET_DATA + 0] & RN_OUTPUT_ON) ? True:False;
        data->channels[port]->ready = False;
        data->channels[port]->sleep = False;
        data->channels[port]->idle  = 0;
      }
    }
    else {
      if( port < 129 && data->ports[port] != NULL ) {
        if( rn[RN_PACKET_DATA + 0] & RN_OUTPUT_ON ) {
          data->ports[port]->offtimer = SystemOp.getTick();
          data->ports[port]->state = 1;
        }
        else {
          data->ports[port]->offtimer = 0;
          data->ports[port]->state = 0;
        }
      }
      if(data->ports[port] != NULL) {
        __writePort(rocnetnode, port, rn[RN_PACKET_DATA + 0] & RN_OUTPUT_ON ? 1:0, data->ports[port]->iotype);

        msg = allocMem(32);
        msg[RN_PACKET_NETID] = data->location;
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
  SocketOp.sendto( data->writeUDP, (char*)rn, 8 + rn[RN_PACKET_LEN], NULL, 0 );

}


static void __checkPortEvents( iORocNetNode rocnetnode, byte* rn ) {
  iORocNetNodeData data = Data(rocnetnode);
  int group     = rn[RN_PACKET_GROUP];
  int eventidA  = rnSenderAddrFromPacket(rn, 0);
  int eventidB  = rnReceipientAddrFromPacket(rn, 0);
  int eventport = 0;
  int eventval  = 0;

  switch( group ) {
    case RN_GROUP_OUTPUT:
      eventval  = rn[RN_PACKET_DATA + 0];
      eventport = rn[RN_PACKET_DATA + 3];
      break;
    case RN_GROUP_SENSOR:
      eventval  = rn[RN_PACKET_DATA + 2];
      eventport = rn[RN_PACKET_DATA + 3];
      break;
  }

  if( (eventidA > 0 || eventidB > 0) && eventport > 0 ) {
    int i = 0;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "event: id=%d,%d port=%d val=%d", eventidA, eventidB, eventport, eventval );
    for( i = 0; i < 129; i++ ) {
      if( data->ports[i] != NULL  && data->ports[i]->eventid > 0 && data->ports[i]->eventid > 0 ) {
        if( data->ports[i]->type == IO_OUTPUT &&
            (data->ports[i]->eventid == eventidA || data->ports[i]->eventid == eventidB) && data->ports[i]->eventport == eventport )
        {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "match port %d event: id=%d port=%d val=%d", i, data->ports[i]->eventid, eventport, eventval );
          __writePort(rocnetnode, i, eventval, data->ports[i]->iotype);
        }
      }
    }
  }

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
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "ignore %s [%s] from %d to %d; address does not match", rnActionTypeString(rn), str, sndr, rcpt );
    StrOp.free(str);
    __checkPortEvents(rocnetnode, rn);
    return;
  }
  else {
    char* str = StrOp.byteToStr(rn, 8 + rn[RN_PACKET_LEN]);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "evaluate %s [%s] from %d to %d", rnActionTypeString(rn), str, sndr, rcpt );
    StrOp.free(str);
  }

  if(rcpt != data->id && rcpt == 0) {
    __checkPortEvents(rocnetnode, rn);
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

  __checkPortEvents(rocnetnode, rn);
}


static int __readPort(iORocNetNode rocnetnode, int port, int iotype) {
  iORocNetNodeData data = Data(rocnetnode);
  if( iotype == IO_DIRECT ) {
    return (raspiRead(port) ? 0:1);
  }
  else if( data->i2cdescriptor > 0 && port > 0 && port < 129 ) {
    byte rdata = 0;
    /* the __scanI2C must have been called */
    int idx = (port-1)/8;
    byte mask = 1 << (((port-1)%16)%8);
    return (data->iodata[idx] & mask) ? 0:1;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "port out of range [%d]", port );
    return 0;
  }
}

static void __scanI2C(iORocNetNode rocnetnode) {
  iORocNetNodeData data = Data(rocnetnode);
  int i = 0;
  int rc = 0;
  MutexOp.wait( data->i2cmux );
  for(i = 0; i < 8; i++) {
    if( !data->i2caddr[i] )
      continue;

    if( data->iomap[i] & 0x00FF ) {
      byte iodata = data->iodata[i*2+0];
      rc = raspiReadRegI2C(data->i2cdescriptor, 0x20+i, 0x12, &data->iodata[i*2+0]);
      if( rc < 0 ) {
        data->i2caddr[i] = False;
        __errorReport(rocnetnode, RN_ERROR_RC_I2C, RN_ERROR_RS_READ, i);
      }
      TraceOp.trc( name, iodata != data->iodata[i*2+0]?TRCLEVEL_INFO:TRCLEVEL_DEBUG, __LINE__, 9999, "i2c %dA [0x%02X]", i, data->iodata[i*2+0] );
    }
    if( data->iomap[i] & 0xFF00 ) {
      byte iodata = data->iodata[i*2+1];
      rc = raspiReadRegI2C(data->i2cdescriptor, 0x20+i, 0x13, &data->iodata[i*2+1]);
      if( rc < 0 ) {
        data->i2caddr[i] = False;
        __errorReport(rocnetnode, RN_ERROR_RC_I2C, RN_ERROR_RS_READ, i);
      }
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "i2c %dB [0x%02X]", i, data->iodata[i*2+1] );
    }
  }
  MutexOp.post( data->i2cmux );
}


static void __writeChannel(iORocNetNode rocnetnode, int channel, int pos) {
  iORocNetNodeData data = Data(rocnetnode);
  int rc  = 0;
  int i2caddr = 0x40;

  if( data->channels[channel] == NULL ) {
    return;
  }

  i2caddr = (data->channels[channel]->channel - 1) / 16;

  MutexOp.wait( data->i2cmux );
  rc = pwmSetChannel(data->i2cdescriptor, 0x40+i2caddr, (data->channels[channel]->channel%16) - 1, -1, pos);
  MutexOp.post( data->i2cmux );

  if( rc < 0 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not write to I2C device %s addr 0x%02X errno=%d", data->i2cdevice, 0x40+i2caddr, errno );
    data->i2caddr40[i2caddr] = False;
    __errorReport(rocnetnode, RN_ERROR_RC_I2C, RN_ERROR_RS_WRITE, 0x40+i2caddr);
  }

}


static void __writePort(iORocNetNode rocnetnode, int port, int value, int iotype) {
  iORocNetNodeData data = Data(rocnetnode);

  if( data->ports[port] != NULL && (data->ports[port]->type & IO_TOGGLE) ) {
    if( value ) {
      data->ports[port]->toggle = !data->ports[port]->toggle;
      value = data->ports[port]->toggle ? 1:0;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "toggle port %d value=%d", port, value );
    }
    else {
      /* ignore off */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ignore low value for toggle port %d toggle=%d", port, data->ports[port]->toggle );
      return;
    }
  }

  if( data->ports[port] != NULL && (data->ports[port]->type & IO_INVERT)) {
    value ^= 1;
  }
  if( iotype == IO_DIRECT ) {
    /* direct GPIO */
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Direct GPIO writeport %d=%d", port, value );
    raspiWrite(port, value);
  }
  else if( data->i2cdescriptor > 0 && port > 0 && port < 129 ) {
    /* Read the I2C, then write. */
    int i2caddr = (port-1)/16;
    int shift   = (port-1)%16;
    int mask16  = (1 << shift); /* create the port mask */
    int wdata16 = (value << shift); /* shift the new value on its place */
    byte mask8  = 0;
    byte wdata8 = 0;
    byte rdata  = 0;

    if(!data->i2caddr[i2caddr]) {
      /* return; not initialised */
      return;
    }
    /* read the latch byte */
    MutexOp.wait( data->i2cmux );
    raspiReadRegI2C(data->i2cdescriptor, 0x20+i2caddr, (shift > 7) ? 0x15:0x14, &rdata);

    if( shift > 7 ) {
      mask8  = (mask16 >> 8) & 0x00FF;
      wdata8 = (wdata16 >> 8) & 0x00FF;
    }
    else {
      mask8  = mask16 & 0x00FF;
      wdata8 = wdata16 & 0x00FF;
    }

    rdata &= ~mask8; /* fileter out the wanted port */
    wdata8 |= rdata; /* save other port values */

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
        "I2C writeport %d=0x%02X i2caddr=%d shift=%d mask=0x%02X", port, wdata8, i2caddr, shift, mask8 );
    raspiWriteRegI2C(data->i2cdescriptor, 0x20+i2caddr, (shift > 7) ? 0x13:0x12, wdata8);
    MutexOp.post( data->i2cmux );
  }
}


static void __pwm( void* threadinst ) {
  iOThread         th         = (iOThread)threadinst;
  iORocNetNode     rocnetnode = (iORocNetNode)ThreadOp.getParm( th );
  iORocNetNodeData data       = Data(rocnetnode);
  int i = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RocNet pwm started" );

  while( data->run ) {
    for( i = 0; i < 129; i++ ) {
      if( data->channels[i] != NULL ) {
        int delay = (data->channels[i]->options & 0x0F);
        if( delay > 0 ) {
          if( data->channels[i]->delay < delay ) {
            data->channels[i]->delay++;
            continue;
          }
          data->channels[i]->delay = 0;
        }

        int gotopos = data->channels[i]->curpos;
        int steps = 0;
        if( data->channels[i]->state && data->channels[i]->curpos != data->channels[i]->onpos ) {
          gotopos = data->channels[i]->onpos;
          steps   = data->channels[i]->onsteps;
        }
        else if( data->channels[i]->state == 0 && data->channels[i]->curpos != data->channels[i]->offpos ) {
          gotopos = data->channels[i]->offpos;
          steps   = data->channels[i]->offsteps;
        }

        if( data->channels[i]->curpos != gotopos ) {
          int oldcurpos = data->channels[i]->curpos;
          if( data->channels[i]->curpos > gotopos ) {
            data->channels[i]->curpos -= steps;
            if( data->channels[i]->curpos <= gotopos || steps == 0 ) {
              data->channels[i]->curpos = gotopos;
              if( data->channels[i]->options & PWM_REPORT ) {
                byte* msg = allocMem(32);
                msg[RN_PACKET_NETID] = data->location;
                msg[RN_PACKET_GROUP] = RN_GROUP_OUTPUT;
                rnReceipientAddresToPacket( 0, msg, 0 );
                rnSenderAddresToPacket( data->id, msg, 0 );
                msg[RN_PACKET_ACTION] = RN_STATIONARY_SINGLE_PORT;
                msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
                msg[RN_PACKET_LEN] = 4;
                msg[RN_PACKET_DATA + 0] = 0; /* off */
                msg[RN_PACKET_DATA + 1] = wProgram.porttype_servo;
                msg[RN_PACKET_DATA + 2] = 0;
                msg[RN_PACKET_DATA + 3] = data->channels[i]->channel;
                __sendRN(rocnetnode, msg);
                data->channels[i]->ready = True;
              }
              if( data->channels[i]->options & PWM_BLINK && data->channels[i]->blink ) {
                data->channels[i]->state = !data->channels[i]->state;
                data->channels[i]->ready = False;
                data->channels[i]->sleep = False;
                data->channels[i]->idle  = 0;
              }
            }
          }
          else {
            data->channels[i]->curpos += steps;
            if( data->channels[i]->curpos >= gotopos || steps == 0 ) {
              data->channels[i]->curpos = gotopos;
              if( data->channels[i]->options & PWM_REPORT ) {
                byte* msg = allocMem(32);
                msg[RN_PACKET_NETID] = data->location;
                msg[RN_PACKET_GROUP] = RN_GROUP_OUTPUT;
                rnReceipientAddresToPacket( 0, msg, 0 );
                rnSenderAddresToPacket( data->id, msg, 0 );
                msg[RN_PACKET_ACTION] = RN_STATIONARY_SINGLE_PORT;
                msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
                msg[RN_PACKET_LEN] = 4;
                msg[RN_PACKET_DATA + 0] = 1; /* off */
                msg[RN_PACKET_DATA + 1] = wProgram.porttype_servo;
                msg[RN_PACKET_DATA + 2] = 0;
                msg[RN_PACKET_DATA + 3] = data->channels[i]->channel;
                __sendRN(rocnetnode, msg);
                data->channels[i]->ready = True;
              }
              if( data->channels[i]->options & PWM_BLINK && data->channels[i]->blink ) {
                data->channels[i]->state = !data->channels[i]->state;
                data->channels[i]->ready = False;
                data->channels[i]->sleep = False;
                data->channels[i]->idle  = 0;
              }
            }
          }
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "set channel %d pwm from %d to %d, gotopos=%d",
              data->channels[i]->channel-1, oldcurpos, data->channels[i]->curpos, gotopos );
          __writeChannel(rocnetnode, data->channels[i]->channel, data->channels[i]->curpos);
          ThreadOp.sleep(0);
        }

        if( data->channels[i]->options & PWM_SERVO && data->channels[i]->ready ) {
          if( data->channels[i]->idle < 100 ) {
            data->channels[i]->idle++;
          }
          if( data->channels[i]->idle >= 100 && !data->channels[i]->sleep ) {
            data->channels[i]->sleep = True;
            __writeChannel(rocnetnode, data->channels[i]->channel, 4095);
          }
        }

      }
    }

    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RocNet pwm stopped" );

}


static void __scanner( void* threadinst ) {
  iOThread         th         = (iOThread)threadinst;
  iORocNetNode     rocnetnode = (iORocNetNode)ThreadOp.getParm( th );
  iORocNetNodeData data       = Data(rocnetnode);
  int inputVal[128];
  byte msg[256];
  int identwait = 0;
  Boolean LED1 = False;
  Boolean LED2 = False;


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RocNet scanner started" );

  MemOp.set( inputVal, 0, sizeof(inputVal) );
  /* negative logic level */
  MemOp.set( data->iodata, 0xFF, sizeof(data->iodata) );

  __writePort(rocnetnode, data->LED1, 1, IO_DIRECT );
  __writePort(rocnetnode, data->LED2, 1, IO_DIRECT );

  while( data->run ) {
    int i;
    int nrios = 128;
    Boolean startofday = data->startofday;

    data->LED1timer++;
    if( data->LED1timer >= 50 || (data->pendingHalt && data->LED1timer >= 10) ) {
      data->LED1timer = 0;
      LED1 = !LED1;
      __writePort(rocnetnode, data->LED1, LED1?1:0, IO_DIRECT );
    }

    if( data->show ) {
      data->LED2timer++;
      if( data->LED2timer >= 10 ) {
        data->LED2timer = 0;
        LED2 = !LED2;
        __writePort(rocnetnode, data->LED2, LED2?1:0, IO_DIRECT );
      }
    }
    else
      __writePort(rocnetnode, data->LED2, 1, IO_DIRECT );

    if( data->iorc == 0 ) {
      __scanI2C(rocnetnode);

      for( i = 0; i < nrios; i++ ) {
        if( data->ports[i] != NULL && (data->ports[i]->type&IO_TYPE) == 0 ) {
          if( (data->ports[i]->type & IO_BLINK)  && data->ports[i]->state && data->ports[i]->delay > 0 ) {
            if( data->ports[i]->offtimer + data->ports[i]->delay <= SystemOp.getTick() ) {
              data->ports[i]->offtimer = SystemOp.getTick();
              data->ports[i]->blink = !data->ports[i]->blink;
              __writePort(rocnetnode, i, data->ports[i]->blink?1:0, data->ports[i]->iotype);
            }
          }
          else if( data->ports[i]->delay > 0 && data->ports[i]->state ) {
            if( data->ports[i]->offtimer + data->ports[i]->delay <= SystemOp.getTick() ) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "pulse off port %d", i );
              data->ports[i]->state = 0;
              __writePort(rocnetnode, i, 0, data->ports[i]->iotype);

              msg[RN_PACKET_NETID] = data->location;
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
        if( data->sack && data->ports[i] != NULL && (data->ports[i]->type&IO_TYPE) == 1 && data->ports[i]->ackpending) {
          data->ports[i]->acktimer++;
          if( data->ports[i]->acktimer > 50 ) {
            data->ports[i]->ackretry++;
            data->ports[i]->acktimer = 0;
            if( data->ports[i]->ackretry <= 10 ) {
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no ack for port %d; resend", i );
              msg[RN_PACKET_NETID] = data->location;
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

        /* reset input values to zero to trigger a start of day */
        if( data->ports[i] != NULL && (data->ports[i]->type&IO_TYPE) == 1 && startofday ) {
          inputVal[i] = 0;
        }

        if( data->ports[i] != NULL && (data->ports[i]->type&IO_TYPE) == 1 && (!data->sack || !data->ports[i]->ackpending) ) {
          int val = 0;
          if( data->ports[i]->iotype == IO_DIRECT )
            val = __readPort(rocnetnode, data->ports[i]->ionr, data->ports[i]->iotype);
          else
            val = __readPort(rocnetnode, i, data->ports[i]->iotype);

          Boolean report = inputVal[i] != val;

          if( data->ports[i]->delay > 0 ) {
            report = False;
            if( val > 0 ) {
              data->ports[i]->offtimer = SystemOp.getTick();
              if( !data->ports[i]->state ) {
                data->ports[i]->state = 1;
                TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "delayed on port %d", i );
                report = True;
              }
            }
            else if( data->ports[i]->state && data->ports[i]->offtimer + data->ports[i]->delay <= SystemOp.getTick() ) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "delayed off port %d", i );
              data->ports[i]->state = 0;
              report = True;
            }
          }

          /* Check PB1 for a 5 second ON */
          if( i == 0 ) {
            if( inputVal[i] > 0 && data->usepb ) {
              data->PB1timer++;
              if( data->PB1timer >= 500 ) {
                /* Shutdown */
                RocNetNodeOp.sysHalt();
                data->PB1timer = 0;
                report = False;
              }
              else if( data->PB1timer >= 100 ) {
                data->pendingHalt = True;
              }
            }
            else {
              data->PB1timer = 0;
              data->pendingHalt = False;
            }
          }

          /* Report the changed port */
          if( report ) {
            if( i == 0 ) {
              /* Normal PB1 handling */
              inputVal[i] = val;
              if( val && ! data->pendingHalt && data->usepb ) {
                if( data->show ) {
                  data->show = False;
                }
                else {
                  msg[RN_PACKET_NETID] = data->location;
                  msg[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
                  msg[RN_PACKET_ACTION] = RN_STATIONARY_SHOW;
                  msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
                  msg[RN_PACKET_LEN] = 0;
                  rnSenderAddresToPacket( data->id, msg, 0 );
                  __sendRN(rocnetnode, msg);
                }
              }
            }
            else {
              data->ports[i]->ackpending = True;
              data->ports[i]->ackretry = 0;
              data->ports[i]->acktimer = 0;
              inputVal[i] = val;
              if( data->ports[i]->type & IO_INVERT ) {
                val ^= 1;
              }
              msg[RN_PACKET_NETID] = data->location;
              msg[RN_PACKET_GROUP] = RN_GROUP_SENSOR;
              msg[RN_PACKET_ACTION] = RN_SENSOR_REPORT;
              msg[RN_PACKET_LEN] = 4;
              msg[RN_PACKET_DATA+2] = val;
              msg[RN_PACKET_DATA+3] = i;
              rnSenderAddresToPacket( data->id, msg, 0 );
              __sendRN(rocnetnode, msg);
            }
            ThreadOp.sleep(raspiDummy()?500:10);
          }
        }
      }

      if( startofday )
        data->startofday = False;

    }

    if( !data->identack ) {
      identwait++;
      if( identwait > 100 ) {
        identwait = 0;
        msg[RN_PACKET_NETID] = data->location;
        msg[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
        rnReceipientAddresToPacket( 0, msg, 0 );
        rnSenderAddresToPacket( data->id, msg, 0 );
        msg[RN_PACKET_ACTION] = RN_STATIONARY_IDENTIFY;
        msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
        msg[RN_PACKET_LEN] = 7;
        msg[RN_PACKET_DATA+0] = data->class;
        msg[RN_PACKET_DATA+1] = 70;
        msg[RN_PACKET_DATA+2] = revisionnr/256;
        msg[RN_PACKET_DATA+3] = revisionnr%256;
        msg[RN_PACKET_DATA+4] = 128;
        msg[RN_PACKET_DATA+5] = data->ip[data->ipsize-2];
        msg[RN_PACKET_DATA+6] = data->ip[data->ipsize-1];
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
  int idx = 0;
  iOStrTok tok = StrTokOp.inst(getLocalIP(), '.');
  while( StrTokOp.hasMoreTokens(tok) ) {
    data->ip[idx] = atoi(StrTokOp.nextToken( tok ));
    idx++;
  }
  StrTokOp.base.del(tok);
  data->ipsize = idx;

  if( data->ip[2] == 0 && data->ip[3] == 0 ) {
    int subip = SystemOp.getMillis() + SystemOp.getpid();
    data->ipsize = 4;
    data->ip[2] = (subip / 256) % 256;
    data->ip[3] = subip % 256;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RocNet reader started on: %d.%d.%d.%d",
      data->ip[0], data->ip[1], data->ip[2], data->ip[3] );

  while( data->run ) {
    SocketOp.recvfrom( data->readUDP, (char*)msg, 0x7F, NULL, NULL );
    __evaluateRN(rocnetnode, msg);
    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RocNet reader stopped" );
}


static void __listener( obj inst, iONode nodeC, int level ) {
  iORocNetNodeData data = Data(inst);
  if( nodeC != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "listener: %s", NodeOp.getName(nodeC) );
    if(StrOp.equals(NodeOp.getName(nodeC), wFeedback.name())) {
      byte msg[256];
      const char* ident = wFeedback.getidentifier(nodeC);
      msg[RN_PACKET_NETID] = data->location;
      msg[RN_PACKET_GROUP] = RN_GROUP_SENSOR;
      msg[RN_PACKET_ACTION] = RN_SENSOR_REPORT;
      msg[RN_PACKET_LEN] = 4;
      msg[RN_PACKET_DATA+2] = wFeedback.isstate(nodeC);
      msg[RN_PACKET_DATA+3] = wFeedback.getaddr(nodeC);
      rnSenderAddresToPacket( data->id, msg, 0 );
      if( ident != NULL ) {
        int len = StrOp.len(ident);
        int i = 0;
        msg[RN_PACKET_LEN] += len;
        for( i = 0; i < len; i++ )
          msg[RN_PACKET_DATA+4+i] = ident[i];
      }
      __sendRN((iORocNetNode)inst, msg);
    }
    NodeOp.base.del(nodeC);
  }
}

static void __unloadDigInt(iORocNetNode inst, int prevcstype) {
  iORocNetNodeData data = Data(inst);

  if( !data->rfid && data->pRFID != NULL ) {
    iIDigInt pRFID = data->pRFID;
    data->pRFID = NULL;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unloading RFID..." );
    pRFID->halt((obj)pRFID, True);
    ThreadOp.sleep(100);
    pRFID->base.del(pRFID);
    data->class = (data->class & ~RN_CLASS_RFID);
  }

  if( data->cstype != prevcstype && data->pDI != NULL && data->digintini != NULL) {
    iIDigInt pDI = data->pDI;
    data->pDI = NULL;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unloading %s...", wDigInt.getlib(data->digintini) );
    pDI->halt((obj)pDI, True);
    ThreadOp.sleep(100);
    pDI->base.del(pDI);
    data->class = (data->class & ~RN_CLASS_DCC);
  }
}

static Boolean __initDigInt(iORocNetNode inst) {
  iORocNetNodeData data = Data(inst);

  if( data->rfid && data->pRFID == NULL ) {
    iOLib    pLib = NULL;
    LPFNROCGETDIGINT pInitFun = (void *) NULL;
    char* libpath = StrOp.fmt( "%s%c%s", data->libpath, SystemOp.getFileSeparator(), "rfid12" );
    pLib = LibOp.inst( libpath );
    StrOp.free( libpath );
    if (pLib != NULL) {
      pInitFun = (LPFNROCGETDIGINT)LibOp.getProc(pLib,"rocGetDigInt");
      if (pInitFun != NULL) {
        iONode digintini = NodeOp.inst(wDigInt.name(), NULL, ELEMENT_NODE);
        wDigInt.setiid(digintini, "rfid-1");
        wDigInt.setdevice(digintini, "/dev/ttyAMA0"); /* on every RasPi? */
        wDigInt.setfboffset( digintini, 129 );
        data->pRFID = pInitFun(digintini,TraceOp.get());
        data->pRFID->setListener( (obj)data->pRFID, (obj)inst, &__listener );
        data->class |= RN_CLASS_RFID;
      }
    }
  }

  if( data->cstype > 0 && data->pDI == NULL && data->digintini != NULL ) {
    const char*  lib = wDigInt.getlib( data->digintini );
    const char*  iid = wDigInt.getiid( data->digintini );
    iOLib    pLib = NULL;
    LPFNROCGETDIGINT pInitFun = (void *) NULL;
    char* libpath = StrOp.fmt( "%s%c%s", data->libpath, SystemOp.getFileSeparator(), lib );
    pLib = LibOp.inst( libpath );
    StrOp.free( libpath );
    if (pLib == NULL)
      return False;
    pInitFun = (LPFNROCGETDIGINT)LibOp.getProc(pLib,"rocGetDigInt");
    if (pInitFun == NULL)
      return False;
    data->pDI = pInitFun(data->digintini,TraceOp.get());
    data->pDI->setListener( (obj)data->pDI, (obj)inst, &__listener );
    data->class |= RN_CLASS_DCC;

    return True;
  }
  return False;
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

static iONode __findChannel(iORocNetNode inst, int channel) {
  iORocNetNodeData data = Data(inst);
  iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
  iONode channelsetup = NULL;

  if( rocnet != NULL ) {
    iONode channelsetup = wRocNet.getchannelsetup(rocnet);
    while( channelsetup != NULL ) {
      if( wChannelSetup.getchannel(channelsetup) == channel ) {
        return channelsetup;
      }
      channelsetup = wRocNet.nextchannelsetup(rocnet, channelsetup);
    }
  }

  return channelsetup;
}

static iONode __findMacro(iORocNetNode inst, int nr) {
  iORocNetNodeData data = Data(inst);
  iONode macro = wRocNet.getmacro(data->ini);

  while( macro != NULL ) {
    if( wMacro.getnr(macro) == nr ) {
      return macro;
    }
    macro = wRocNet.nextmacro(data->ini, macro);
  }

  return NULL;
}

static void __errorReport( iORocNetNode inst, int rc, int rs, int addr) {
  iORocNetNodeData data = Data(inst);
  byte msg[32];
  msg[RN_PACKET_NETID] = data->location;
  msg[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
  rnReceipientAddresToPacket( 0, msg, 0 );
  rnSenderAddresToPacket( data->id, msg, 0 );
  msg[RN_PACKET_ACTION] = RN_STATIONARY_ERROR;
  msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
  msg[RN_PACKET_LEN] = 4;
  msg[RN_PACKET_DATA+0] = rc;
  msg[RN_PACKET_DATA+1] = rs;
  msg[RN_PACKET_DATA+2] = addr/256;
  msg[RN_PACKET_DATA+3] = addr%256;
  __sendRN(inst, msg);
}


static void __initI2CPWM(iORocNetNode inst) {
  iORocNetNodeData data = Data(inst);
  iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init I2C PWM [%s]", data->i2cdevice );

  if( data->i2cdescriptor < 0 ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "I2C device %s is not initialized", data->i2cdevice );
  }

  MutexOp.wait( data->i2cmux );

  MemOp.set(data->i2caddr40, 0, sizeof(data->i2caddr40));

  if( rocnet != NULL ) {
    int i2caddr = 0;
    iONode channelsetup = wRocNet.getchannelsetup(rocnet);
    while( channelsetup != NULL ) {
      int channelnr = wChannelSetup.getchannel(channelsetup);

      if( channelnr < 129 ) {
        iOChannel channel = data->channels[channelnr];
        if( channel == NULL ) {
          channel = allocMem( sizeof( struct Channel) );
          data->channels[channelnr] = channel;
        }

        channel->channel  = channelnr;
        channel->options  = wChannelSetup.getoptions(channelsetup);
        channel->offsteps = wChannelSetup.getoffsteps(channelsetup);
        channel->onsteps  = wChannelSetup.getonsteps(channelsetup);
        channel->offpos   = wChannelSetup.getoffpos(channelsetup);
        channel->onpos    = wChannelSetup.getonpos(channelsetup);
        channel->state    = wChannelSetup.getstate(channelsetup);

        i2caddr = (channelnr-1) / 16;
        if( i2caddr >= 0 && i2caddr < 8 ) {
          int rc = 0;

          if( !data->i2caddr40[i2caddr] ) {
            data->i2caddr40[i2caddr] = True;
            rc = pwmStop(data->i2cdescriptor, 0x40+i2caddr);
            if( rc < 0 ) {
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not write to I2C device %s addr 0x%02X errno=%d", data->i2cdevice, 0x40+i2caddr, errno );
              data->i2caddr40[i2caddr] = False;
              __errorReport(inst, RN_ERROR_RC_I2C, RN_ERROR_RS_WRITE, 0x40+i2caddr);
            }
          }

          if( data->i2caddr40[i2caddr] ) {
            channel->curpos = channel->state ? channel->onpos:channel->offpos;
            rc = pwmSetChannel(data->i2cdescriptor, 0x40+i2caddr, (channel->channel%16)-1, 0, channel->curpos);
            if( rc < 0 ) {
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not write to I2C device %s addr 0x%02X errno=%d", data->i2cdevice, 0x40+i2caddr, errno );
              data->i2caddr40[i2caddr] = False;
              __errorReport(inst, RN_ERROR_RC_I2C, RN_ERROR_RS_WRITE, 0x40+i2caddr);
            }
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                "channelsetup: channel=%d i2caddr=0x%02X offpos=%d onpos=%d offsteps=%d onsteps=%d options=0s%02X state=%d",
                channel->channel, i2caddr + 0x40, channel->offpos, channel->onpos, channel->offsteps, channel->onsteps, channel->options, channel->state );
          }

        }
      }

      channelsetup = wRocNet.nextchannelsetup(rocnet, channelsetup);
    }

    for( i2caddr = 0; i2caddr < 8; i2caddr++ ) {
      if( data->i2caddr40[i2caddr] ) {
        int rc = pwmSetFreq(data->i2cdescriptor, 0x40+i2caddr, 60);
        if( rc < 0 ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not write to I2C device %s addr 0x%02X errno=%d", data->i2cdevice, 0x40+i2caddr, errno );
          data->i2caddr40[i2caddr] = False;
          __errorReport(inst, RN_ERROR_RC_I2C, RN_ERROR_RS_WRITE, 0x40+i2caddr);
        }
      }
    }

  }

  MutexOp.post( data->i2cmux );
}


static void __initI2C(iORocNetNode inst, int iotype) {
  iORocNetNodeData data = Data(inst);
  iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init I2C [%s]", data->i2cdevice );

  MutexOp.wait( data->i2cmux );

  if( data->iotype == IO_I2C_0 ) {
    raspiGPIOAlt(28, 0);
    raspiGPIOAlt(29, 0);
  }
  else {
    raspiGPIOAlt(2, 0);
    raspiGPIOAlt(3, 0);
  }
  ThreadOp.sleep(100);

  MemOp.set(data->iomap, 0, sizeof(data->iomap));
  MemOp.set(data->i2caddr, 0, sizeof(data->i2caddr));

  if( rocnet != NULL ) {
    int i = 0;
    int i2caddr = 0;
    int shift = 0;
    iONode portsetup = wRocNet.getportsetup(rocnet);
    MemOp.set( data->iomap, 0, sizeof(data->iomap));
    while( portsetup != NULL ) {
      int portnr = wPortSetup.getport(portsetup);

      if( portnr < 129 ) {
        iOPort port = data->ports[portnr];
        if( port == NULL ) {
          port = allocMem( sizeof( struct Port) );
          data->ports[portnr] = port;
        }

        port->port = portnr;
        port->delay = wPortSetup.getdelay(portsetup);
        port->iotype = iotype;
        port->type = wPortSetup.gettype(portsetup);
        port->eventid = wPortSetup.geteventid(portsetup);
        port->eventport = wPortSetup.geteventport(portsetup);
        if( (port->type&IO_TYPE) == 0 )
          port->state = wPortSetup.getstate(portsetup);
        if( (port->type&IO_TYPE) == 1 && port->delay == 0 )
          port->delay = 10;

        i2caddr = (portnr-1) / 16;
        shift = (portnr-1) % 16;
        if( i2caddr >= 0 && i2caddr < 8 ) {
          data->i2caddr[i2caddr] = True;
          if( wPortSetup.gettype(portsetup) == 1 )
            data->iomap[i2caddr] |= (1 << shift );

          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
              "portsetup: port=%d i2caddr=0x%02X shift=%d mask=0x%02X type=%d delay=%d blink=%d",
              port->port, i2caddr + 0x20, shift, data->iomap[i2caddr], (port->type&IO_TYPE), port->delay, (port->type&IO_BLINK)?1:0 );
        }
      }
      portsetup = wRocNet.nextportsetup(rocnet, portsetup);
    }

    for( i = 0; i < 8; i++ ) {
      if( data->i2caddr[i] )
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "i2c addr=%d A=0x%02X B=0x%02X", i, data->iomap[i]&0x00FF, (data->iomap[i]&0xFF00) >> 8 );
    }

    data->i2cdescriptor = raspiOpenI2C(data->i2cdevice);
    if( data->i2cdescriptor < 0 ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not open I2C device %s errno=%d", data->i2cdevice, errno );
      MutexOp.post( data->i2cmux );
      return;
    }
    for( i = 0; i < 8; i++ ) {
      int rc = 0;
      if( data->i2caddr[i] ) {
        ThreadOp.sleep(50);
        rc = raspiWriteRegI2C(data->i2cdescriptor, 0x20+i, 0x00, data->iomap[i]&0x00FF);
        if( rc < 0 ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not write to I2C device %s addr 0x%02X errno=%d", data->i2cdevice, 0x20+i, errno );
          data->i2caddr[i] = False;
          __errorReport(inst, RN_ERROR_RC_I2C, RN_ERROR_RS_WRITE, i+0x20);
          continue;
        }
        ThreadOp.sleep(50);
        rc = raspiWriteRegI2C(data->i2cdescriptor, 0x20+i, 0x01, (data->iomap[i]&0xFF00) >> 8);
        if( rc < 0 ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not write to I2C device %s addr 0x%02X errno=%d", data->i2cdevice, 0x20+i, errno );
          data->i2caddr[i] = False;
          __errorReport(inst, RN_ERROR_RC_I2C, RN_ERROR_RS_WRITE, i+0x20);
          continue;
        }
      }
    }
  }

  MutexOp.post( data->i2cmux );

}


static void __initControl(iORocNetNode inst) {
  iORocNetNodeData data = Data(inst);
  int iomap = 0;
  iOPort port = NULL;

  data->LED1 = IO_LED1;
  data->LED2 = IO_LED2;
  data->PB1  = IO_PB1;

  ThreadOp.sleep(50);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init LED1 on port %d", data->LED1 );
  raspiGPIOAlt(data->LED1, 0);
  raspiConfigPort(data->LED1, 0);
  ThreadOp.sleep(50);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init LED2 on port %d", data->LED2 );
  raspiGPIOAlt(data->LED2, 0);
  raspiConfigPort(data->LED2, 0);
  ThreadOp.sleep(50);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init PB1 on port %d", data->PB1 );
  raspiGPIOAlt(data->PB1, 0);
  raspiConfigPort(data->PB1, 1);

  ThreadOp.sleep(50);
  port = allocMem( sizeof( struct Port) );
  port->port = 0;
  port->ionr = data->PB1;
  port->delay = 50;
  port->iotype = IO_DIRECT;
  port->type = IO_INPUT;
  port->state = 0;
  data->ports[0] = port;
}


static void __initIO(iORocNetNode inst) {
  iORocNetNodeData data = Data(inst);

  data->iorc = raspiSetupIO();

  if( data->iorc != 0 ) {
    __errorReport(inst, RN_ERROR_RC_IO, RN_ERROR_RS_SETUP, 0);
  }

  if(data->iotype == IO_I2C_0) {
    /* i2c-0 Rev. 1*/
    data->i2cdevice = "/dev/i2c-0";
    __initI2C(inst, 1);
  }
  else {
    /* default i2c-1 Rev.2 */
    data->i2cdevice = "/dev/i2c-1";
    __initI2C(inst, 2);
  }

  __initI2CPWM(inst);

  if( data->iorc == 0) {
    data->class |= RN_CLASS_IO;
    __initControl(inst);
  }

}

static void __checkConsole( iORocNetNodeData data ) {
  /* Check for command. */
  int c = getchar();

  if( c == wConCmd.debug ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Toggle debug tracelevel." );
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) ^ TRCLEVEL_DEBUG );
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) ^ TRCLEVEL_XMLH );
  }
  else if( c == wConCmd.byte ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Toggle byte tracelevel." );
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) ^ TRCLEVEL_BYTE );
  }
  else if( c == wConCmd.memtrc ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Toggle memory tracelevel." );
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) ^ TRCLEVEL_MEMORY );
  }
  else if( c == wConCmd.automat ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Toggle auto tracelevel." );
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) ^ TRCLEVEL_USER1 );
  }
  else if( c == wConCmd.monitor ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Toggle monitor tracelevel." );
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) ^ TRCLEVEL_MONITOR );
  }
  else if( c == wConCmd.info ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Toggle info tracelevel." );
    TraceOp.setLevel( NULL, TraceOp.getLevel( NULL ) ^ TRCLEVEL_INFO );
  }
  else if( c == wConCmd.quit ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutdown requested." );
    data->consoleMode = False;
    RocNetNodeOp.shutdown();
  }
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
  Boolean     console = CmdLnOp.hasKey( arg, wCmdline.console );

  const char* nf     = CmdLnOp.getStr( arg, wCmdline.inifile );
  data->libpath       = CmdLnOp.getStr( arg, wCmdline.libpath );

  if( data->libpath == NULL ) {
    data->libpath = ".";
  }

  if( nf == NULL )
    data->inifile = ROCNETNODEINI;
  else
    data->inifile = nf;

  data->consoleMode = console;

  /* Read the Inifile: */
  {
    char* iniXml = NULL;
    iODoc iniDoc = NULL;
    iOFile iniFile = FileOp.inst( data->inifile, True );
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
      printf( "Invalid ini file! [%s]", data->inifile );
      return -1;
    }
  }

  /* set defaults */
  data->id     = 65535;
  data->addr   = "224.0.0.1";
  data->port   = 4321;
  data->iotype = IO_I2C_1;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Up and running the RocNetNode" );
  if( NodeOp.findNode(data->ini, wRocNet.name()) != NULL ) {
    iONode rocnet = NodeOp.findNode(data->ini, wRocNet.name());
    data->id    = wRocNet.getid(rocnet);
    data->location = wRocNet.getnet(rocnet);
    data->addr  = wRocNet.getaddr(rocnet);
    data->port  = wRocNet.getport(rocnet);
    if( data->id < 2 ) {
      data->id = 65535;
      wRocNet.setid(rocnet, data->id);
    }

    if( NodeOp.findNode(rocnet, wRocNetNodeOptions.name()) != NULL ) {
      iONode optionsini = NodeOp.findNode(rocnet, wRocNetNodeOptions.name());
      data->sack  = wRocNetNodeOptions.issack(optionsini);
      data->rfid  = wRocNetNodeOptions.isrfid(optionsini);
      data->usepb = wRocNetNodeOptions.isusepb(optionsini);
      data->iotype = wRocNetNodeOptions.getiotype(optionsini);
      if( data->iotype == IO_NOT_USED ) {
        /* convert to new type: i2c-1 */
        data->iotype = IO_I2C_1;
      }
    }
    else {
      iONode optionsini = NodeOp.inst(wRocNetNodeOptions.name(), rocnet, ELEMENT_NODE );
      NodeOp.addChild( rocnet, optionsini);
    }

    if( NodeOp.findNode(data->ini, wTrace.name()) != NULL ) {
      iONode traceini = NodeOp.findNode(data->ini, wTrace.name());
      tf = wTrace.getrfile(traceini);
      trc = TraceOp.inst( debug | dump | monitor | (wTrace.ismonitor(traceini)?TRCLEVEL_MONITOR:0) | parse |
                          (wTrace.isinfo(traceini)?TRCLEVEL_INFO:0) | TRCLEVEL_CALC, tf, True );
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
    else {
      trc = TraceOp.inst( debug | dump | monitor | parse | TRCLEVEL_INFO | TRCLEVEL_WARNING | TRCLEVEL_CALC, tf, True );
    }

    data->tl_info = TraceOp.getLevel( trc ) & TRCLEVEL_INFO;
    data->tl_monitor = TraceOp.getLevel( trc ) & TRCLEVEL_MONITOR;

    data->digintini = NodeOp.findNode(data->ini, wDigInt.name());
    if( data->digintini != NULL ) {
      if(StrOp.equals(wDigInt.dcc232, wDigInt.getlib(data->digintini)))
        data->cstype = 1;
      else if(StrOp.equals(wDigInt.sprog, wDigInt.getlib(data->digintini)))
        data->cstype = 2;

      if( StrOp.equals("/dev/ttyUSB0", wDigInt.getdevice(data->digintini)) )
        data->csdevice = 0;
      else if( StrOp.equals("/dev/ttyUSB1", wDigInt.getdevice(data->digintini)) )
        data->csdevice = 1;
      else if( StrOp.equals("/dev/ttyACM0", wDigInt.getdevice(data->digintini)) )
        data->csdevice = 2;

    }
    else {
      data->cstype = 0;
    }

    if( NodeOp.findNode(data->ini, wMacro.name()) != NULL ) {
      iONode macro = NodeOp.findNode(data->ini, wMacro.name());
      while( macro != NULL ) {
        int nr = wMacro.getnr(macro);
        if( nr < 129 && data->macro[nr] == NULL ) {
          iONode line = wMacro.getmacroline(macro);
          int lineIdx = 0;
          data->macro[nr] = allocMem(sizeof(struct Macro));
          data->macro[nr]->slowdown = wMacro.getslowdown(macro);
          data->macro[nr]->repeat   = wMacro.getrepeat(macro);
          data->macro[nr]->hours    = wMacro.gethours(macro);
          data->macro[nr]->minutes  = wMacro.getminutes(macro);
          data->macro[nr]->wday     = wMacro.getwday(macro);
          while( line != NULL && lineIdx < 8 ) {
            data->macro[nr]->line[lineIdx].delay = wMacroLine.getdelay(line);
            data->macro[nr]->line[lineIdx].port  = wMacroLine.getport(line);
            data->macro[nr]->line[lineIdx].type  = wMacroLine.getporttype(line);
            data->macro[nr]->line[lineIdx].value = wMacroLine.getstatus(line);
            data->macro[nr]->line[lineIdx].blink = wMacroLine.isblink(line);
            lineIdx++;
            line = wMacro.nextmacroline(macro, line);
          }
        }
        macro = NodeOp.findNextNode(data->ini, macro);
      }
    }

  }
  else {
    trc = TraceOp.inst( debug | dump | monitor | parse | TRCLEVEL_INFO | TRCLEVEL_WARNING | TRCLEVEL_CALC, tf, True );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "using default setup" );
    data->id    = 65535;
    data->addr  = "224.0.0.1";
    data->port  = 4321;
  }
  TraceOp.setAppID( trc, "r" );

  data->i2cmux = MutexOp.inst( NULL, True );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  revision [%d]", revisionnr );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  ID [%d]", data->id );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast address [%s]", data->addr );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast port    [%d]", data->port );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  I/O type [%d]", data->iotype );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  sensor ack [%s]", data->sack?"ON":"OFF" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  RFID [%s]", data->sack?"ON":"OFF" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  data->readUDP = SocketOp.inst( data->addr, data->port, False, True, True );
  SocketOp.bind(data->readUDP);
  data->writeUDP = SocketOp.inst( data->addr, data->port, False, True, True );

  ThreadOp.sleep(500); /* startup sleep */
  data->class = 0;
  __initIO(inst);

  __initDigInt(inst);

  data->run = True;
  data->reader = ThreadOp.inst( "rnreader", &__reader, __RocNetNode );
  ThreadOp.start( data->reader );
  data->scanner = ThreadOp.inst( "rnscanner", &__scanner, __RocNetNode );
  ThreadOp.start( data->scanner );
  data->pwm = ThreadOp.inst( "rnpwm", &__pwm, __RocNetNode );
  ThreadOp.start( data->pwm );
  data->macroprocessor = ThreadOp.inst( "rnmacro", &__macroProcessor, __RocNetNode );
  ThreadOp.start( data->macroprocessor );

  /* Memory watcher */
  while( !bShutdown ) {
    static int cnt1 = 0;
    int cnt2 = MemOp.getAllocCount();
    if( cnt2 > cnt1 ) {
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "memory allocations old=%u new=%u", cnt1, cnt2 );
    }
    cnt1 = cnt2;
    ThreadOp.sleep( 1000 );

    /* Check for command. */
    if( data->consoleMode )
      __checkConsole( data );
  }

  return 0;
}


static Boolean _shutdown( void ) {
  iORocNetNodeData data = Data(__RocNetNode);
  byte msg[32];
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutdown the RocNetNode" );

  MemOp.set( msg, 0, sizeof(msg));
  msg[RN_PACKET_NETID] = data->location;
  msg[RN_PACKET_GROUP] = RN_GROUP_STATIONARY;
  rnReceipientAddresToPacket( 0, msg, 0 );
  rnSenderAddresToPacket( data->id, msg, 0 );
  msg[RN_PACKET_ACTION] = RN_STATIONARY_SHUTDOWN;
  msg[RN_PACKET_ACTION] |= (RN_ACTIONTYPE_EVENT << 5);
  msg[RN_PACKET_LEN] = 0;
  __sendRN(__RocNetNode, msg);

  if(data->pRFID != NULL) {
    data->pRFID->halt((obj)data->pRFID, True);
  }

  if(data->pDI != NULL) {
    data->pDI->halt((obj)data->pDI, True);
  }

  data->run = False;
  ThreadOp.sleep(1000);
  bShutdown = True;
  __writePort(__RocNetNode, data->LED1, 1, IO_DIRECT );
  __writePort(__RocNetNode, data->LED2, 1, IO_DIRECT );
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
    data->libpath = ".";
    data->inifile = ROCNETNODEINI;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Instance created of the RocNetNode" );

  }
  return __RocNetNode;
}




/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocnetnode/impl/rocnetnode.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
