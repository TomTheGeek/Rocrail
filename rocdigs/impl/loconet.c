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
  * Object: LocoNet
  * Date: Wed Nov 29 17:19:34 2006
  * ------------------------------------------------------------
  * $Source$
  * $Author$
  * $Date$
  * $Revision$
  * $Name$
  */

#include "rocdigs/impl/loconet_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/LocoNet.h"
#include "rocrail/wrapper/public/LNSlotServer.h"
#include "rocrail/wrapper/public/CSOptions.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/FbInfo.h"
#include "rocrail/wrapper/public/FbMods.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/Clock.h"


/* loconet opcodes */
#include "rocdigs/impl/loconet/lnconst.h"
#include "rocdigs/impl/loconet/lncmdstn.h"
#include "rocdigs/impl/loconet/lnmon.h"
#include "rocdigs/impl/loconet/lbserial.h"
#include "rocdigs/impl/loconet/lbserver.h"
#include "rocdigs/impl/loconet/lbudp.h"
#include "rocdigs/impl/loconet/lbtcp.h"
#include "rocdigs/impl/loconet/ulni.h"
#include "rocdigs/impl/loconet/lnmaster.h"
#include "rocdigs/impl/loconet/lncv.h"
#include "rocdigs/impl/loconet/locoio.h"
#include "rocdigs/impl/loconet/ibcom-cv.h"

#include "rocutils/public/addr.h"

#ifndef min
  #define min( a, b ) ( ((a) < (b)) ? (a) : (b) )
#endif

static int instCnt = 0;

/* proto types */
static void __evaluatePacket(iOLocoNet loconet, byte* rsp, int size );

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOLocoNetData data = Data(inst);
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


/** ----- OLocoNet ----- */


static byte _checksum(const byte *cmd, int len)
{
    byte chksum = 0xff;
    int i;
    for (i = 0; i < len; i++) {
        chksum ^= cmd[i];
    }
    return chksum;
}


static void _stateChanged( iOLocoNet loconet ) {
  iOLocoNetData data = Data(loconet);

  if( data->listenerFun != NULL && data->listenerObj != NULL ) {
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
    wState.setiid( node, data->iid );
    wState.setpower( node, data->power?True:False );
    wState.setprogramming( node, data->pt?True:False );
    wState.settrackbus( node, data->comm?True:False );

    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}

static int __rwLNCV(iOLocoNet loconet, int cvnum, int val, byte* cmd, Boolean writeLNCV, int modid, int modaddr, int extracmd) {
  iOLocoNetData data = Data(loconet);
  if( extracmd == 1 ) {
    /* some modules need this to get in programming mode */
    LocoNetOp.getSlot( loconet, 0, OPC_SL_RD_DATA );
  }
  /* call lncv utilis */
  int size = makereqLNCV(cmd, modid, modaddr, cvnum, val, writeLNCV, extracmd);
  cmd[size-1] = LocoNetOp.checksum( cmd, size-1 );
  return size;
}


static int __rwLNSV(iOLocoNet loconet, int cvnum, int val, byte* cmd, Boolean writeLNCV, int modaddr, int subaddr, int extracmd) {
  iOLocoNetData data = Data(loconet);
  /* call lncv utilis */
  int size = makereqLocoIOSV(cmd, modaddr, subaddr, cvnum, val, writeLNCV);
  cmd[size-1] = LocoNetOp.checksum( cmd, size-1 );
  return size;
}


static int __rwLNMP(iOLocoNet loconet, int mask, int val, byte* cmd, Boolean writeLNMP, int modaddr, int subaddr, int extracmd) {
  iOLocoNetData data = Data(loconet);
  /* call lncv utilis */
  int size = makereqLocoIOMultiPort(cmd, modaddr, subaddr, mask, val, writeLNMP);
  cmd[size-1] = LocoNetOp.checksum( cmd, size-1 );
  return size;
}


static int __rwLNOPSW(iOLocoNet loconet, int addr, int type, int opsw, int val, byte* cmd, Boolean write) {
  iOLocoNetData data = Data(loconet);
  /* call lncv utilis */
  int size = makereqLNOPSW(cmd, addr, type, opsw, val, write);
  cmd[size-1] = LocoNetOp.checksum( cmd, size-1 );
  return size;
}


static int __rwCV(iOLocoNet loconet, int cvnum, int val, byte* cmd, Boolean writeCV, Boolean pom, Boolean direct, int decaddr) {
  iOLocoNetData data = Data(loconet);
  int addr  = cvnum-1; /* cvnum is in human readable form; addr is what's sent over loconet */
  int lopsa = decaddr & 0x007F;
  int hopsa = (decaddr & 0x3F80) >> 7;
  int pcmd  = 0;

  if( pom && decaddr == 0 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "POM for address 0 is not supported" );
    return 0;
  }

  if( writeCV ) {
    pcmd = 0x43; /* LPE imples 0x40, but 0x43 is observed */
  }
  else {
    pcmd = 0x03; /* LPE imples 0x00, but 0x03 is observed */
  }

  if( direct )
    pcmd = pcmd | 0x28; /* DIRECTBYTEMODE */
  else
    pcmd = pcmd | 0x20; /* PAGEBYTEMODE */


  if(pom)
    pcmd = pcmd | 0x04;

  cmd[0] = OPC_WR_SL_DATA;
  cmd[1] = 0x0E;
  cmd[2] = 0x7C;

  cmd[3] = pcmd;

  /* set zero, then HOPSA, LOPSA, TRK */
  cmd[4] = 0;;
  cmd[5] = hopsa;
  cmd[6] = lopsa;
  cmd[7] = 0; /* TRK was 0, then 7 for PR2, now back to zero */

  /* store address in CVH, CVL. Note CVH format is truely wierd... */
  cmd[8] = (addr&0x300)/16 + (addr&0x80)/128 + (val&0x80)/128*2;
  cmd[9] = addr & 0x7F;

  /* store low bits of CV value */
  cmd[10] = val&0x7F;

  /* throttle ID */
  cmd[11] = 0x7F;
  cmd[12] = 0x7F;
  cmd[11] = 0x00;
  cmd[12] = 0x00;
  cmd[13] = LocoNetOp.checksum( cmd, 13 );
  return 14;
}

static void __post2SlotServer( iOLocoNet loconet, byte* rsp, int len );


static Boolean _transact( iOLocoNet loconet, byte* out, int outsize, byte* in, int* insize, byte waitforOPC_OK, byte waitforOPC_FAIL, Boolean post ) {
  iOLocoNetData data = Data(loconet);
  Boolean     ok = False;
  int i = 0;

  if( !data->commOK ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "No inited LocoNet interface!" );
    return False;
  }

  if( MutexOp.trywait( data->mux, 5000 ) ) {

    ok = data->lnWrite( (obj)loconet, out, outsize );

    if( post )
      __post2SlotServer( loconet, out, outsize );


    if(ok) {
      data->sndpkg++;
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "*** transact dump:" );
      TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)out, outsize );

      if( in != NULL && insize != NULL ) {
        int retry = 0;
        do {
          ThreadOp.sleep(50);
          *insize = data->lnRead( (obj)loconet, in );
          if( *insize > 0 ) {
            data->rcvpkg++;
            traceLocoNet(in);
            TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "*** transact dump:" );
            TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)in, *insize );
            if( waitforOPC_OK > 0 && in[0] == waitforOPC_OK ) {
              break;
            }
            if( waitforOPC_FAIL > 0 && in[0] == waitforOPC_FAIL ) {
              break;
            }
            __evaluatePacket(loconet, in, *insize);
          }
          retry++;
        } while( retry < 10 );
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not send the packet!" );
    }

    /* Release the mutex. */
    MutexOp.post( data->mux );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "timeout on mutex." );
  }

  return ok;
}


static Boolean _write( iOLocoNet loconet, byte* out, int outsize ) {
  iOLocoNetData data = Data(loconet);
  Boolean     ok = False;
  int i = 0;

  if( !data->commOK ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "No inited LocoNet interface!" );
    return False;
  }

  ok = data->lnWrite( (obj)loconet, out, outsize );

  if(ok) {
    data->sndpkg++;
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "*** write dump:" );
    TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)out, outsize );

  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not send the packet!" );
  }

  return ok;
}


/**
 * Runs the thread - sends 8 commands to query status of all stationary sensors
 *     per LocoNet PE Specs, page 12-13
 * Thread waits 500 msec between commands after a 2 sec initial wait.
 */
static void __loconetSensorQuery( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOLocoNet loconet = (iOLocoNet)ThreadOp.getParm( th );
  iOLocoNetData data = Data(loconet);
  int reportaddr = wLocoNet.getreportaddr(data->loconet);
  byte cmd[32];
  int k = 0;

  if( reportaddr > 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet Sensor Query started with address %d.", reportaddr );
    cmd[0] = OPC_SW_REQ;
    cmd[1] = reportaddr&0x7F;
    cmd[2] = (reportaddr/128)&0x0F;
    cmd[3] = LocoNetOp.checksum( cmd, 3 );
    LocoNetOp.transact( loconet, cmd, 4, NULL, NULL, 0, 0, False );
  }
  else {
    byte sw1[] = {0x78,0x79,0x7a,0x7b,0x78,0x79,0x7a,0x7b};
    byte sw2[] = {0x27,0x27,0x27,0x27,0x07,0x07,0x07,0x07};
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet Sensor Query started for all known report addresses." );

    for( k = 0; k < 8; k++ ) {
      ThreadOp.sleep( 500 );

      cmd[0] = OPC_SW_REQ;
      cmd[1] = sw1[k];
      cmd[2] = sw2[k];
      cmd[3] = LocoNetOp.checksum( cmd, 3 );

      LocoNetOp.transact( loconet, cmd, 4, NULL, NULL, 0, 0, False );

    }
  }


  ThreadOp.sleep( 100 );

    /*================================================================
    Opcode    OPC_PEER_XFER (0E5h) - for replies
        OPC_IMM_PACKET (0EDh) - for messages with reply
    length    15 bytes (0Fh)
    12 message data bytes
    Checksum

    format of data bytes:
    SRC, DSTL, DSTH,
    ReqId,
    PXCT1, D1, D2, D3, D4, D5, D6, D7

    SRC    0 = master, 1 = KPU, 2 = DAISY, 3 = TB or FRED
            4 = IB-Switch, 5 = LocoNet modules, 6 = IntelliSound,
        7 = LIA, 8 = PC
        70h..7Eh = reserved

    DSTL/H    destination (addressed) device, 0/0 = broadcast
        "I"/"B" = Intellibox (SPU)
        "I"/"K" = Intellibox (KPU)
        0..15/"T" = Twin-Box
        "I"/"S" = IB-Switch
        "D"/"Y" = DAISY throttle
        a module/device address (low/high)

    PXCT1    0, D7.7, D6.7, D5.7, D4.7, D3.7, D2.7, D1.7

    ReqId
        ...

        17 = s88 status read: (this is a reply)
            D1    s88 module number (-1)
            ...
            D6/7    s88 status (contacts 1..8, 9..16) from s88LNow

        ...

        19 = s88 status request: (reply: LACK fail or s88 status read)
            D1    s88 module number (-1)
            D2..D7    not used

        ...
    ================================================================*/

  if( wDigInt.getfbmod(data->ini) > 0 ) {
    /* IB with s88 modules connected: */
    byte cmd[32];
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet Sensor Query for %d S88. (IB only)", wDigInt.getfbmod(data->ini) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "(if not wanted set fbmod=\"0\")" );

    /*
    SRC = 1 (KPU)
    DSTL/H = "I"/"B" = Intellibox (SPU) (i.e., 0x49 and 0x42)
    ReqId = 19 (same as you write)
    PXCT1 = 0 (unless you have an s88 module # higher than 128!)
    D1 = s88 module # (minus 1), i.e. 0 for the 1st s88 module
    D2..D7 = 0

    */

    cmd[ 0] = OPC_IMM_PACKET;
    cmd[ 1] = 0x0F;
    cmd[ 2] =  1; /* SRC   */
    cmd[ 3] =  0x49; /* DSTL  */
    cmd[ 4] =  0x42; /* DSTH  */
    cmd[ 5] = 19; /* ReqID */
    cmd[ 6] =  0; /* PXCT1 */
    cmd[ 7] =  0; /* D1    */
    cmd[ 8] =  0; /* D2    */
    cmd[ 9] =  0; /* D3    */
    cmd[10] =  0; /* D4    */
    cmd[11] =  0; /* D5    */
    cmd[12] =  0; /* D6    */
    cmd[13] =  0; /* D7    */


    for( k=0; k < wDigInt.getfbmod(data->ini); k++ ) {
      cmd[ 7] =  k; /* D1    */
      cmd[14] = LocoNetOp.checksum( cmd, 14 );
      LocoNetOp.transact( loconet, cmd, 15, NULL, NULL, 0, 0, False );
      ThreadOp.sleep( 500 );
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet Sensor Query ended." );
  ThreadOp.base.del(th);
}




static void __handleLissy(iOLocoNet loconet, byte* msg) {
  iOLocoNetData data   = Data(loconet);

  int         lissyaddr = msg[4] & 0x7F;
  int         sensdata  = ( msg[6] & 0x7F ) + 128 * ( msg[5] & 0x7F );
  Boolean     dir       = ( msg[3] & 0x20 ) ? True:False;
  Boolean     wheelcnt  = ( msg[2] & 0x01 ) ? True:False;
  char        ident[32];

  if( wheelcnt )
    lissyaddr = (msg[4] & 0x7F) + (128 * ( msg[3] & 0x7F ));


  /* inform listener: Node3 */
  iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );

  wFeedback.setaddr( nodeC, lissyaddr );
  if( data->iid != NULL ) wFeedback.setiid( nodeC, data->iid );
  wFeedback.setstate( nodeC, True );

  if(wheelcnt) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "wheelcounter=%d count=%d", lissyaddr, sensdata );
    wFeedback.setfbtype( nodeC, wFeedback.fbtype_wheelcounter );
    wFeedback.setwheelcount( nodeC, sensdata );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lissy=%d ident=%d dir=%d", lissyaddr, sensdata, dir );

    wFeedback.setfbtype( nodeC, wFeedback.fbtype_lissy );

    StrOp.fmtb(ident, "%d", sensdata);
    wFeedback.setidentifier( nodeC, ident );
    wFeedback.setdirection( nodeC, dir );
  }

  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
}


static void __handleSensor(iOLocoNet loconet, int addr, int value) {
  iOLocoNetData data   = Data(loconet);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensor=%d value=%d", addr, value );
  {
    /* inform listener: Node3 */
    iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );

    wFeedback.setaddr( nodeC, addr );
    wFeedback.setfbtype( nodeC, wFeedback.fbtype_sensor );

    if( data->iid != NULL )
      wFeedback.setiid( nodeC, data->iid );

    wFeedback.setstate( nodeC, value?True:False );

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}


static void __powerMultiSenseMessage(iOLocoNet loconet, byte* msg) {
  iOLocoNetData data = Data(loconet);
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "*** powerMultiSenseMessage not implemented" );
}


static void __handleTransponding(iOLocoNet loconet, byte* msg) {
  iOLocoNetData data = Data(loconet);

  int type         = msg[1] & OPC_MULTI_SENSE_MSG;
  int addr         = ( (msg[1]&0x1F) * 128 ) + msg[2];
  int boardaddr    = addr/16;
  int locoaddr     = 0;
  const char* zone = "";
  Boolean present  = False;
  Boolean enter    = (msg[1] & 0x20) != 0 ? True:False;
  char ident[32];

  boardaddr++;
  addr++;

  if      ((msg[2]&0x0F) == 0x00) zone = "A";
  else if ((msg[2]&0x0F) == 0x02) zone = "B";
  else if ((msg[2]&0x0F) == 0x04) zone = "C";
  else if ((msg[2]&0x0F) == 0x06) zone = "D";
  else if ((msg[2]&0x0F) == 0x08) zone = "E";
  else if ((msg[2]&0x0F) == 0x0A) zone = "F";
  else if ((msg[2]&0x0F) == 0x0C) zone = "G";
  else if ((msg[2]&0x0F) == 0x0E) zone = "H";


  if (msg[3]==0x7D)
    locoaddr=msg[4];
  else
    locoaddr=msg[3]*128+msg[4];

  switch (type) {
  case OPC_MULTI_SENSE_POWER:
    __powerMultiSenseMessage(loconet, msg);
    return;
  case OPC_MULTI_SENSE_PRESENT:  // from transponding app note
    present  = True;
    break;
  case OPC_MULTI_SENSE_ABSENT:
    present  = False;
    break;
  default:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "*** unknown multi sense type: 0x%02X (0x%02X)", type, msg[1] );
    return;
  }

  {
    iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );

    wFeedback.setaddr( nodeC, addr );
    wFeedback.setbus( nodeC, wFeedback.fbtype_transponder );
    wFeedback.setzone( nodeC, zone );
    wFeedback.setfbtype( nodeC, wFeedback.fbtype_transponder );

    if( data->iid != NULL )
      wFeedback.setiid( nodeC, data->iid );

    StrOp.fmtb(ident, "%d", locoaddr);
    wFeedback.setidentifier( nodeC, ident );
    wFeedback.setstate( nodeC, present );
/*
D0 20 06 7D 01 75
loconet  0549 Transponder [7] [present] in section [96] zone [D] decoder address [1]
 */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "BDL[%d] RX[%d] zone [%s] reports [%s] of decoder address [%d]",
        boardaddr, addr, zone, present?"present":"absend", locoaddr );

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }

}


static void __handleSwitch(iOLocoNet loconet, int addr, int port, int value) {
  iOLocoNetData data = Data(loconet);

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sw addr=%d port=%d value=%d", addr, port, value );
  {
    /* inform listener: Node3 */
    iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );

    wSwitch.setaddr1( nodeC, ( addr / 4 ) + 1 );
    wSwitch.setport1( nodeC, ( addr % 4 ) + 1 );

    if( data->iid != NULL )
      wSwitch.setiid( nodeC, data->iid );

    wSwitch.setstate( nodeC, port?"straight":"turnout" );
    wSwitch.setgatevalue(nodeC, value);

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}


static void __handleLoco(iOLocoNet loconet, byte* rsp ) {
  iOLocoNetData data = Data(loconet);
  int slot = rsp[1];
  int spd  = rsp[2];
  int dirf = rsp[2];
  int snd  = rsp[2];
  int addr = data->locoslot[slot];
  int throttleid = data->locothrottle[slot];
  char* sthrottleid = StrOp.fmt("%d", throttleid);


  if( rsp[0] == OPC_LOCO_SPD ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot=%d addr=%d spd=%d id=%d", slot, addr, spd, throttleid );
    /* inform listener: Node3 */
    iONode nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wLoc.setiid( nodeC, data->iid );
    wLoc.setaddr( nodeC, addr );
    wLoc.setV_raw( nodeC, spd );
    wLoc.setV_rawMax( nodeC, 127 );
    wLoc.setthrottleid( nodeC, sthrottleid );
    wLoc.setcmd( nodeC, wLoc.velocity );
    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
    data->slotV[slot] = spd;
  }
  else if( rsp[0] == OPC_LOCO_DIRF ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot=%d addr=%d dirf=0x%02X id=%d", slot, addr, dirf, throttleid );
    /* inform listener: Node3 */
    iONode nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wLoc.setiid( nodeC, data->iid );
    wLoc.setaddr( nodeC, addr );
    wLoc.setdir( nodeC, (dirf & DIRF_DIR)? False:True );
    wLoc.setfn( nodeC, (dirf & DIRF_F0) ? True:False );
    wLoc.setthrottleid( nodeC, sthrottleid );
    wLoc.setcmd( nodeC, wLoc.dirfun );
    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

    iONode nodeD = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wLoc.setiid( nodeD, data->iid );
    wFunCmd.setaddr( nodeD, addr );
    wLoc.setfn( nodeD, (dirf & DIRF_F0) ? True:False );
    wFunCmd.setf0( nodeD, (dirf & DIRF_F0) ? True:False );
    wFunCmd.setf1( nodeD, (dirf & DIRF_F1) ? True:False );
    wFunCmd.setf2( nodeD, (dirf & DIRF_F2) ? True:False );
    wFunCmd.setf3( nodeD, (dirf & DIRF_F3) ? True:False );
    wFunCmd.setf4( nodeD, (dirf & DIRF_F4) ? True:False );
    wLoc.setthrottleid( nodeD, sthrottleid );
    wFunCmd.setgroup( nodeD, 1 );
    data->listenerFun( data->listenerObj, nodeD, TRCLEVEL_INFO );

  }
  else if( rsp[0] == OPC_LOCO_SND ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot=%d addr=%d snd=0x%02X id=%d", slot, addr, snd, throttleid );
    iONode nodeD = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wLoc.setiid( nodeD, data->iid );
    wFunCmd.setaddr( nodeD, addr );
    wFunCmd.setf5( nodeD, (snd & SND_F5) ? True:False );
    wFunCmd.setf6( nodeD, (snd & SND_F6) ? True:False );
    wFunCmd.setf7( nodeD, (snd & SND_F7) ? True:False );
    wFunCmd.setf8( nodeD, (snd & SND_F8) ? True:False );
    wFunCmd.setgroup( nodeD, 2 );
    wLoc.setthrottleid( nodeD, sthrottleid );
    data->listenerFun( data->listenerObj, nodeD, TRCLEVEL_INFO );
  }

  StrOp.free(sthrottleid);
}

static void __slotPing( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOLocoNet loconet = (iOLocoNet)ThreadOp.getParm( th );
  iOLocoNetData data = Data(loconet);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet slotPing started." );

  while( data->run ) {
    time_t currtime = time(NULL);
    int i;

    if( MutexOp.trywait( data->slotmux, 500 ) ) {
    /* lookup slot for address: */
      for( i = 0; i < 120; i++ ) {

        if( data->locoslot[i] > 0 && ((currtime - data->slotaccessed[i]) >= (data->purgetime/2)) ) {
          byte cmd[4];
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sending a ping for slot# %d", i );

          cmd[0] = OPC_LOCO_SPD;
          cmd[1] = i; /* slot number */
          cmd[2] = data->slotV[i] & 0x7F;
          cmd[3] = LocoNetOp.checksum( cmd, 3 );
          if( LocoNetOp.transact( loconet, cmd, 4, NULL, NULL, 0, 0, False ) ) {
            data->slotaccessed[i] = currtime;
          }
        }
      }

      /* Release the mutex. */
      MutexOp.post( data->slotmux );
    }

    ThreadOp.sleep(1000);
  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet slotPing ended." );
}


static void __swReset( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOLocoNet loconet = (iOLocoNet)ThreadOp.getParm( th );
  iOLocoNetData data = Data(loconet);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet swReset started." );

  while( data->run ) {
    obj post = ThreadOp.waitPost( th );
    if( post != NULL ) {
      iONode node = (iONode)post;

      if( StrOp.equals( "quit", NodeOp.getName( node ) ) ) {
        node->base.del( node );
        break;
      }

      ThreadOp.sleep( wSwitch.getdelay( node ) > 0 ? wSwitch.getdelay( node ):data->swtime );
      {
        byte cmd[32];
        int addr = wSwitch.getaddr1( node );
        int port  = wSwitch.getport1( node );
        int gate = 0;
        int dir  = 1;
        int action = 0;

        if( port == 0 )
          AddrOp.fromFADA( addr, &addr, &port, &gate );
        else if( addr == 0 && port > 0 )
          AddrOp.fromPADA( port, &addr, &port );

        addr = (addr-1) * 4 + (port-1);

        if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
          dir = 0; /* thrown */

        cmd[0] = OPC_SW_REQ;

        cmd[1]  = (unsigned short int) (addr & 0x007f);
        cmd[2]  = (unsigned short int) (( addr >> 7) & 0x000f);
        cmd[2] |= (unsigned short int) ( (dir & 0x0001) << 5);
        cmd[2] |= (unsigned short int) ( (action & 0x0001) << 4);
        cmd[3] = LocoNetOp.checksum( cmd, 3 );

        LocoNetOp.transact( loconet, cmd, 4, NULL, NULL, 0, 0, False );

      }
      node->base.del( node );

    }
    else {
      TraceOp.trc( name, TRCLEVEL_ERROR, __LINE__, 9999, "swReset waitPost returns NULL!" );
    }

    ThreadOp.sleep( 10 );

  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet swReset ended." );
}


static void __showConfig( iOLocoNet loconet, byte* msg ) {
  iOLocoNetData data = Data(loconet);
  int i = 0;
  const int* cs_opsw = NULL;
  char opsw[65] = {'\0'};

  MemOp.set( opsw, '0', 64 );
  opsw[64] = '\0';

  /* keep them for the next write */
  MemOp.copy( data->opsw, msg+3, 10);
  data->opswreaded = True;

  for ( i = 0; i <= MAX_OPTION; i++){
    // i indexes over closed/thrown buttons
    int byteIndex = 0; // index = 0 is the first payload byte
    int bitIndex  = 0;
    int bitMask   = 0;
    int data      = 0;

    byteIndex = i / 8; // index = 0 is the first payload byte
    if (byteIndex > 3)
      byteIndex++; // Skip the 4th payload byte for some reason

    byteIndex += 3 ; // Add base offset to first data byte

    bitIndex = i % 8;
    bitMask = 0x01 << bitIndex ;
    data = msg[byteIndex];  // data is the payload byte
    opsw[i] = ( data & bitMask )?'1':'0';

  }

  wCSOptions.setopsw( data->options, opsw );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "opsw[%s]", opsw);

  if( StrOp.equals( wLocoNet.cs_dcs100, wLocoNet.getcmdstn( data->loconet ) ) )
    cs_opsw = opsw_dcs100;
  else
    cs_opsw = opsw_db150;

  for( i = 0; i < MAX_OPTION; i++ ) {
    if( cs_opsw[i] == 0 )
      break;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s [%s]",
        cmdstnOPTIONS[cs_opsw[i]], (opsw[cs_opsw[i]]=='1')?"Closed":"Thrown" );
  }

}


static void __writeConfig( iOLocoNet loconet ) {
  iOLocoNetData data = Data(loconet);
  int i = 0;
  byte msg[14];
  const char* opsw = wCSOptions.getopsw( data->options );

  if( !data->opswreaded ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Only write the options to the CS if the options were read.");
    return;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Write the options to the CS...[%s]", opsw);

  msg[0] = OPC_WR_SL_DATA;
  msg[1] = 0x0E;
  msg[2] = CONFIG_SLOT;

  // load last seen contents into message
  MemOp.copy( msg+3, data->opsw, 10);

  // load contents to message
  for( i = 0; i <= MAX_OPTION; i++ ) {
    // i indexes over closed buttons
    int byteIndex = i / 8; // byteIndex = 0 is the first payload byte
    if (byteIndex > 3)
      byteIndex++; // Skip the 4th payload byte for some reason

    byteIndex += 3 ; // Add base offset into slot message to first data byte

    int bitIndex = i % 8;
    int bitMask = 0x01 << bitIndex ;

    if( opsw[i] == '1' )
      msg[byteIndex] |= bitMask;
    else
      msg[byteIndex] &= ~bitMask;
  }

  msg[13] = LocoNetOp.checksum( msg, 13 );

  LocoNetOp.transact( loconet, msg, 14, NULL, NULL, 0, 0, False );
}


static int __address(int a1, int a2) {
  // the "+ 1" in the following converts to throttle-visible numbering
  return (((a2 & 0x0f) * 128) + (a1 & 0x7f));
}


static void __post2SlotServer( iOLocoNet loconet, byte* rsp, int len ) {
  iOLocoNetData data = Data(loconet);
  if( data->activeSlotServer ) {
    iONode cmd = NodeOp.inst( wCommand.name(), NULL, ELEMENT_NODE );
    char msg[64];
    LocoNetOp.byte2ascii( rsp, len, msg );
    wCommand.setarg( cmd, msg );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "posting [%s] to slotServer", msg );
    ThreadOp.post( data->slotServer, (obj)cmd );
  }
}


static void __loconetWriter( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOLocoNet loconet = (iOLocoNet)ThreadOp.getParm( th );
  iOLocoNetData data = Data(loconet);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet writer started." );

  while( data->run ) {
    byte * post = NULL;
    byte out[64] = {0};

    ThreadOp.sleep(10);
    post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      MemOp.copy( out, post, 64);
      freeMem( post);
    }
    else {
      continue;
    }
    /* first byte is the message length */
    if( !LocoNetOp.transact( (iOLocoNet)loconet, out+1, out[0], NULL, NULL, 0, 0, False ) ) {
      /* sleep and send it again? */
    }

  };


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet writer ended." );
}

static void __stressRunner( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOLocoNet loconet = (iOLocoNet)ThreadOp.getParm( th );
  iOLocoNetData data = Data(loconet);

  ThreadOp.sleep(5000);
  if( data->stress )
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet stress runner started." );

  /* try to get the system status: */
  while( data->run && data->stress ) {
    byte cmd[4];
    cmd[0] = 0x80;
    cmd[1] = LocoNetOp.checksum( cmd, 1 );
    LocoNetOp.transact( loconet, cmd, 2, NULL, NULL, 0, 0, False );

    ThreadOp.sleep(5);
  };

  if( data->stress )
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet stress runner ended." );
}


static void __evaluatePacket(iOLocoNet loconet, byte* rsp, int size ) {
  iOLocoNetData data = Data(loconet);
  int addr = 0;
  int value = 0;
  int port = 0;

  data->rcvpkg++;
  traceLocoNet(rsp);
  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "*** read dump:" );
  TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)rsp, size );

  switch(rsp[0]) {

    case OPC_GPOFF:
    case OPC_IDLE:
      data->power = False;
      _stateChanged(loconet);
      __post2SlotServer( loconet, rsp, 2 );
      break;

    case OPC_GPON:
      data->power = True;
      _stateChanged(loconet);
      if( !data->didSensorQuery && data->doSensorQuery ) {
        iOThread t =  NULL;
        data->didSensorQuery = True;
        t =  ThreadOp.inst( "lnqGPON", &__loconetSensorQuery, loconet );
        ThreadOp.start( t );
      }
      __post2SlotServer( loconet, rsp, 2 );
      break;

  case OPC_SW_REP: {    // B1
    int thrown = (rsp[2] & 0x10) >> 4;
    int closed = (rsp[2] & 0x20) >> 5;
    addr = __address(rsp[1], rsp[2]);
    break;
  }

  case OPC_SW_REQ: // B0
    if( !data->serveLConly )
      __post2SlotServer( loconet, rsp, 4 );
  case OPC_SW_STATE:    // BC
    addr = __address(rsp[1], rsp[2]);
    value = (rsp[2] & 0x10) >> 4;
    port  = (rsp[2] & 0x20) >> 5;
    __handleSwitch(loconet, addr, port, value);
    break;

  case OPC_INPUT_REP: // B2
    addr = ((unsigned int) rsp[1] & 0x007f) |
           (((unsigned int) rsp[2] & 0x000f) << 7);
    addr = 1 + addr * 2 + ((((unsigned int) rsp[2] & 0x0020) >> 5));
    value = (rsp[2] & 0x10) >> 4;

    __handleSensor(loconet, addr, value);
    break;

  case OPC_LOCO_DIRF:
  case OPC_LOCO_SND:
  case OPC_LOCO_SPD:
    /* 4 byte message post to slotServer */
    __post2SlotServer( loconet, rsp, 4 );
    __handleLoco( loconet, rsp );
    break;

  case OPC_LOCO_ADR:
  case OPC_RQ_SL_DATA:
  case OPC_MOVE_SLOTS:
  case OPC_SLOT_STAT1:
    /* 4 byte message post to slotServer */
    __post2SlotServer( loconet, rsp, 4 );
    break;
  case OPC_WR_SL_DATA:
    /* variable byte message post to slotServer */
    __post2SlotServer( loconet, rsp, rsp[1] );
    break;

  case OPC_LISSY_REP: // E4
    /* sensor 1: E4 08 00 00 20 00 03 30 */
    /* sensor 2: E4 08 00 00 02 00 03 12 */
    __handleLissy(loconet, rsp);
    break;

  case OPC_MULTI_SENSE: // D0
    __handleTransponding(loconet, rsp);
    break;

  case OPC_PEER_XFER: // E5
  {
    /* IB response s88 status */
    /* E5 0F 00 49 4B 11 06 05 7F 7F 00 00 0A 44 4B */
    if( rsp[1] == 0x0F && rsp[2] == UB_SRC_MASTER && rsp[3] == 0x49 && rsp[4] == 0x4B ) {
      int saddr = 0;
      byte pxct = rsp[6];
      byte d6 = rsp[12];
      byte d7 = rsp[13];

      addr  = rsp[7]*16;

      /* move in the high bit: */
      d6 = d6 | ((pxct&0x20)?0x80:0x00);
      d7 = d7 | ((pxct&0x40)?0x80:0x00);

      for( saddr = 0; saddr < 8; saddr++ ) {
        value = d6 & (0x01 << saddr);
        if( value )
          __handleSensor(loconet, addr+(7-saddr)+1, 1);
      }

      addr  = rsp[7]*16 + 8;

      for( saddr = 0; saddr < 8; saddr++ ) {
        value = d7 & (0x01 << saddr);
        if( value )
          __handleSensor(loconet, addr+(7-saddr)+1, 1);
      }
    }
    else if( isLNCV(rsp) ) {
      int modid, addr, cv, val;
      Boolean lncvset = evaluateLNCV(rsp, &modid, &addr, &cv, &val);
      iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LNCV response" );

      wProgram.setlncv( node, True );
      wProgram.setlntype( node, wProgram.lntype_cv );
      wProgram.setvalue( node, val );
      wProgram.setcmd( node, wProgram.datarsp );
      wProgram.setcv( node, cv );
      wProgram.setdecaddr( node, addr );
      wProgram.setmodid( node, modid );
      if( data->iid != NULL )
        wProgram.setiid( node, data->iid );

      if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

    }
    else if( isLocoIOSV(rsp) ) {
      int addr, subaddr, cv, val, ver;
      Boolean lncvset = evaluateLocoIOSV(rsp, &addr, &subaddr, &cv, &val, &ver);
      iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LocoIO SV response" );

      wProgram.setlncv( node, True );
      wProgram.setlntype( node, wProgram.lntype_sv );
      wProgram.setvalue( node, val );
      wProgram.setcmd( node, wProgram.datarsp );
      wProgram.setcv( node, cv );
      wProgram.setdecaddr( node, addr );
      wProgram.setmodid( node, subaddr );
      wProgram.setversion( node, ver );
      if( data->iid != NULL )
        wProgram.setiid( node, data->iid );

      if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

    }
    else if( isLocoIOMP(rsp) ) {
      int addr, subaddr, cv, val, ver;
      Boolean lncvset = evaluateLocoIOMultiPort(rsp, &addr, &subaddr, &cv, &val, &ver);
      iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LocoIO MP response" );

      wProgram.setlncv( node, True );
      wProgram.setlntype( node, wProgram.lntype_mp );
      wProgram.setvalue( node, val );
      wProgram.setcmd( node, wProgram.mprsp );
      wProgram.setcv( node, cv );
      wProgram.setdecaddr( node, addr );
      wProgram.setmodid( node, subaddr );
      wProgram.setversion( node, ver );
      if( data->iid != NULL )
        wProgram.setiid( node, data->iid );

      if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

      node = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
      if( data->iid != NULL )
        wFeedback.setiid( node, data->iid );
      wFeedback.setaddr( node, addr * 1000 + subaddr );
      wFeedback.setval( node, val );
      wFeedback.setinfo( node, "LocoIO MultiPort" );

      if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }

    break;
  }

  case OPC_GPBUSY:
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Busy" );
    break;

  case OPC_LONG_ACK:
    if( isLNOPSW(rsp) ) {
      int addr, opsw, val, cv;
      Boolean lncvset = evaluateLNOPSW(rsp, &addr, &cv, &val);
      iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LN OpSw response" );

      wProgram.setlncv( node, True );
      wProgram.setlntype( node, wProgram.lntype_opsw );
      wProgram.setvalue( node, val );
      wProgram.setcmd( node, lncvset?wProgram.statusrsp:wProgram.datarsp );
      wProgram.setcv( node, cv );
      wProgram.setaddr( node, addr );
      if( data->iid != NULL )
        wProgram.setiid( node, data->iid );

      if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

    }
    else if( isLNCV(rsp) ) {
      int modid, addr, cv, val;
      Boolean lncvset = evaluateLNCV(rsp, &modid, &addr, &cv, &val);
      iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "LNCV response" );

      wProgram.setlncv( node, True );
      wProgram.setlntype( node, wProgram.lntype_cv );
      wProgram.setvalue( node, val );
      wProgram.setcmd( node, wProgram.datarsp );
      wProgram.setcv( node, cv );
      wProgram.setdecaddr( node, addr );
      wProgram.setmodid( node, modid );
      if( data->iid != NULL )
        wProgram.setiid( node, data->iid );

      if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

    }
    break;

  case OPC_SL_RD_DATA:
    {
      int slot = rsp[2];
      int pcmd = rsp[3];
      int addr = rsp[4];
      int track= rsp[7];  // hi 3 bits of CV# and msb of data7
      int cvh  = rsp[8];  // hi 3 bits of CV# and msb of data7
      int cvl  = rsp[9];  // lo 7 bits of CV#
      int cvdata = rsp[10]; // 7 bits of data to program, msb is in cvh above
      int idl    = rsp[11];
      int idh    = rsp[12];

      /* IB-Com: E7 0E 7C 00 00 00 71 06 00 00 03 00 00 1E */

      if( slot == 0x7C ) {
        /* PT read */
        int cv = (cvl & 0x7F) + ((cvh & 0x01) << 7);
        int value = (cvdata & 0x7F) + ((cvh & 0x02) << 6);
        /* inform listener */
        iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
        wProgram.setvalue( node, value );
        wProgram.setcmd( node, (pcmd & PCMD_RW) ? wProgram.statusrsp:wProgram.datarsp );
        if( data->iid != NULL )
          wProgram.setiid( node, data->iid );

        if( data->listenerFun != NULL && data->listenerObj != NULL )
          data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
      }
      else if( slot == FC_SLOT ) {
        /* TODO: Fast Clock */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "TODO: Fast Clock slot read" );
      }
      else if( slot > 0 && slot < 0x70 ) {
        int addrL = rsp[4];     // loco address
        int addrH = rsp[9];     // loco address high
        int addr = lnLocoAddr(addrH, addrL);
        data->locoslot[slot] = addr;
        data->locothrottle[slot] = idl + idh * 127;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "slot=%d addr=%d throttleid=%d", slot, addr, data->locothrottle[slot] );
      }
      else if( slot == CONFIG_SLOT ) {
        /* system slot */
        __showConfig(loconet, rsp);

        /* if power is on and the sensors are not already readed:
         *   start the sensor reading thread...
         */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "System slot data: track = 0x%02X", track );
        if( (track & GTRK_POWER) && (!data->didSensorQuery) ) {
          iOThread t =  NULL;
          data->didSensorQuery = True;
          t =  ThreadOp.inst( "lnqCNFG", &__loconetSensorQuery, loconet );
          ThreadOp.start( t );
        }
      }
    }
    break;

  default:
    /* unkown loconet packet received, ignored */
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "unsupported loconet packet received: 0x%02X", rsp[0] );
    break;
  }

}


static void __loconetReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOLocoNet loconet = (iOLocoNet)ThreadOp.getParm( th );
  iOLocoNetData data = Data(loconet);
  byte rsp[128];
  int size = 0;
  int addr = 0;
  int value = 0;
  int port = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet reader started." );

  ThreadOp.sleep(100); /* resume some time to get it all being setup */

  /* try to get the system status: */
  {
    byte cmd[4];
    cmd[0] = OPC_RQ_SL_DATA;
    cmd[1]  = CONFIG_SLOT;
    cmd[2]  = 0;
    cmd[3] = LocoNetOp.checksum( cmd, 3 );
    LocoNetOp.transact( loconet, cmd, 4, NULL, NULL, 0, 0, False );

    ThreadOp.sleep(100);

    cmd[0] = OPC_RQ_SL_DATA;
    cmd[1]  = 0; /* dispatch slot */
    cmd[2]  = 0;
    cmd[3] = LocoNetOp.checksum( cmd, 3 );
    LocoNetOp.transact( loconet, cmd, 4, NULL, NULL, 0, 0, False );

    if( StrOp.equals( wLocoNet.cs_ibcom, wLocoNet.getcmdstn( data->loconet ) ) )
      initIBCom(loconet);
  }

  while( data->run && !data->dummyio ) {
    int available = data->lnAvailable( (obj)loconet);
    if( available == -1 ) {
      /* device error */
      data->dummyio = True;
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error; switch to dummy mode" );
      continue;
    }

    if( available == 0 ) {
      ThreadOp.sleep( 10 );
      continue;
    }
    else {
      // give up rest of timeslice
      ThreadOp.sleep( 0 );
    }

    size = 0;
    if( MutexOp.trywait( data->mux, 1000 ) ) {
      size = data->lnRead( (obj)loconet, rsp );
      MutexOp.post( data->mux );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout on mutex" );
      continue;
    }

    if( size > 0 ) {
      __evaluatePacket(loconet, rsp, size);
    }

  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LocoNet reader ended." );
}


static void _getSlot(iOLocoNet loconet, int slot, byte wait4opc) {
  iOLocoNetData data = Data(loconet);
  byte cmd[8];
  int i = 0;

  cmd[0] = OPC_RQ_SL_DATA;
  cmd[1] = slot;
  cmd[2] = 0;
  cmd[3] = LocoNetOp.checksum( cmd, 3 );
  LocoNetOp.transact( loconet, cmd, 4, NULL, NULL, wait4opc, 0, False );
}


static int __getSlots(iOLocoNet loconet) {
  iOLocoNetData data = Data(loconet);
  byte cmd[8];
  byte rsp[128];
  int insize = 0;
  int i = 0;

  for( i = 0; i < data->slots; i++ ) {
    LocoNetOp.getSlot(loconet, i, 0);
    ThreadOp.sleep( 100 );
  }
}


static int __getConfig(iOLocoNet loconet) {
  iOLocoNetData data = Data(loconet);
  byte cmd[8];
  int i = 0;

  cmd[0] = OPC_RQ_SL_DATA;
  cmd[1] = CONFIG_SLOT;
  cmd[2] = 0;
  cmd[3] = LocoNetOp.checksum( cmd, 3 );
  LocoNetOp.transact( loconet, cmd, 4, NULL, NULL, 0, 0, False );
}


/*
 * return True if decoder type matches:
 *   DEC_MODE_128A
 *   DEC_MODE_28A
 *   DEC_MODE_128
 *   DEC_MODE_14
 *   DEC_MODE_28TRI
 * wLoc.getprot() and wLoc.getspdcnt() should be used to determine the decoder type.
 * P = protocol by server, M = Marklin, N = NMRA-DCC, L = NMRA-DCC long addresses, A = Analog
 */
static Boolean __checkDecoderType(byte status, iONode lc) {
  byte dectype = status & DEC_MODE_MASK;
  int steps = wLoc.getspcnt(lc);
  const char* prot = wLoc.getprot(lc);
  Boolean motorola = (prot[0] == 'M') ? True:False;

  if( !motorola && prot[0] == 'P' ) {
    /* leave to CS default decoder type */
    return True;
  }

  if( motorola && dectype == DEC_MODE_28TRI ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "decoder type for [%s] is 28 step motorola", wLoc.getid(lc) );
    return True;
  }

  /* DCC decoders */
  if( steps == 14 && dectype == DEC_MODE_14 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "decoder type for [%s] is 14 step DCC", wLoc.getid(lc) );
    return True;
  }
  if( steps == 28 && dectype == DEC_MODE_28 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "decoder type for [%s] is 28 step DCC", wLoc.getid(lc) );
    return True;
  }
  if( steps == 28 && dectype == DEC_MODE_28A ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "decoder type for [%s] is 28A step DCC", wLoc.getid(lc) );
    return True;
  }
  if( steps == 128 && dectype == DEC_MODE_128 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "decoder type for [%s] is 128 step DCC", wLoc.getid(lc) );
    return True;
  }
  if( steps == 128 && dectype == DEC_MODE_128A ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "decoder type for [%s] is 128A step DCC", wLoc.getid(lc) );
    return True;
  }

  /* decoder type does not match */
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "decoder type for [%s] does not match", wLoc.getid(lc) );
  return False;
}


/*
 * return status byte
 *   DEC_MODE_128A
 *   DEC_MODE_28A
 *   DEC_MODE_128
 *   DEC_MODE_14
 *   DEC_MODE_28TRI
 * wLoc.getprot() and wLoc.getspdcnt() should be used to determine the decoder type.
 * P = protocol by server, M = Marklin, N = NMRA-DCC, L = NMRA-DCC long addresses, A = Analog
 */
static byte __setDecoderType(byte status, iONode lc) {
  int steps = wLoc.getspcnt(lc);
  const char* prot = wLoc.getprot(lc);
  Boolean motorola = (prot[0] == 'M') ? True:False;


  if( !motorola && prot[0] == 'P' ) {
    /* leave to CS default decoder type */
    return status;
  }

  if( motorola ) {
    status = status & 0xD8;
    status |= DEC_MODE_28TRI;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting decoder type for [%s][0x%02X] to 28 step motorola", wLoc.getid(lc), status );
    return status;
  }

  /* DCC decoders */
  if( steps == 14 ) {
    status = status & 0xD8;
    status |= DEC_MODE_14;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting decoder type for [%s][0x%02X] to 14 step DCC", wLoc.getid(lc), status );
    return status;
  }
  if( steps == 28 ) {
    status = status & 0xD8;
    status |= DEC_MODE_28;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting decoder type for [%s][0x%02X] to 28 step DCC", wLoc.getid(lc), status );
    return status;
  }
  if( steps == 128 ) {
    status = status & 0xD8;
    status |= DEC_MODE_128;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting decoder type for [%s][0x%02X] to 128 step DCC", wLoc.getid(lc), status );
    return status;
  }

  return status;
}


static int __getLocoSlot(iOLocoNet loconet, iONode node, int* status) {
  iOLocoNetData data = Data(loconet);
  Boolean ok = True;
  byte cmd[8];
  byte rsp[128];
  int insize = 0;
  int i = 0;
  int addr = wLoc.getaddr(node);
  int slot = 0;
  time_t currtime = time(NULL);

  /* lookup slot for address: */
  for( i = 0; i < 120; i++ ) {
    if( data->locoslot[i] == addr ) {
      slot = i;
      break;
    }
  }

  /* check slot if it could be purged by the command station: */
  if( slot != 0 && data->purgetime != 0 && ( currtime - data->slotaccessed[slot] ) >= data->purgetime ) {
    data->locoslot[slot] = 0;
    data->slotV[slot] = 0;
    data->slotaccessed[slot] = 0;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Slot#%d for loco addr=%d could be purged...", slot, addr );
    slot = 0;
  }

  if( slot == 0 ) {
    cmd[0] = OPC_LOCO_ADR;
    cmd[1] = (addr/128)&0x7F;
    cmd[2] = addr&0x7F;
    cmd[3] = LocoNetOp.checksum( cmd, 3 );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Trying to get slot for loco addr=%d.", addr );

    ok = LocoNetOp.transact( loconet, cmd, 4, rsp, &insize, OPC_SL_RD_DATA, OPC_LONG_ACK, True );
    if( ok && insize > 0 ) {
      if( rsp[0] == OPC_SL_RD_DATA ) {
        slot = rsp[2];
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Slot#%d for loco addr=%d.", slot, addr );
        data->locoslot[slot] = addr;
        data->slotaccessed[slot] = currtime;

        *status = rsp[3];

        /* check if it is not in use to perform a move: */
        if( (rsp[3] & LOCOSTAT_MASK) != LOCO_IN_USE ) {
          cmd[0] = OPC_MOVE_SLOTS;
          cmd[1] = rsp[2];
          cmd[2] = rsp[2];
          cmd[3] = LocoNetOp.checksum( cmd, 3 );
          ok= LocoNetOp.transact( loconet, cmd, 4, rsp, &insize, OPC_SL_RD_DATA, OPC_LONG_ACK, False );
          if( ok && insize > 0 ) {
            if( rsp[0] == OPC_SL_RD_DATA ) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Slot# %d move was accepted.", slot );
            }
            if( rsp[0] == OPC_LONG_ACK ) {
              /* illegal move! */
              slot = 0;
              data->locoslot[slot] = 0;
              data->slotV[slot] = 0;
              data->slotaccessed[slot] = 0;
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Slot# %d move was illegal!", slot );
            }
          }
        }

        /* send a OPC_SLOT_STAT1 to set decoder type.
         * The default is set by option switches on the CS.
         * wLoc.getprot() and wLoc.getspdcnt() should be used to determine the decoder type.
         */
        if( slot > 0 && !__checkDecoderType( rsp[3], node ) ) {
          cmd[0] = OPC_SLOT_STAT1;
          cmd[1] = rsp[2];
          cmd[2] = __setDecoderType( rsp[3], node );
          cmd[3] = LocoNetOp.checksum( cmd, 3 );
          LocoNetOp.transact( loconet, cmd, 4, NULL, NULL, 0, 0, False );
        }

        if( StrOp.equals( wLocoNet.cs_ibcom, wLocoNet.getcmdstn( data->loconet ) ) )
          initIBCom(loconet);


      }
      else if(rsp[0] == OPC_LONG_ACK) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "No free slot available for loco addr=%d.", addr );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Could not get slot for loco addr=%d. (un expected response 0x%02X...)", addr, rsp[0] );
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not get slot for loco addr=%d. (no response...)", addr );
    }
  }
  else {
    data->slotaccessed[slot] = currtime;
  }

  return slot;
}


static int __setFastClock(iOLocoNet loconet, iONode node, byte* cmd) {
  iOLocoNetData data = Data(loconet);
  Boolean ok = True;
  byte rsp[64];
  int insize = 0;

  cmd[0] = OPC_RQ_SL_DATA;
  cmd[1] = 123;
  cmd[2] = 0;
  cmd[3] = LocoNetOp.checksum( cmd, 3 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Trying to get fast clock slot." );

  ok = LocoNetOp.transact( loconet, cmd, 4, rsp, &insize, OPC_SL_RD_DATA, 0, True );
  if( ok && insize > 0 ) {
    if( rsp[0] == OPC_SL_RD_DATA ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Got fast clock slot" );

      int size = 0;
      int hours = 10;
      int mins = 30;

      long l_time = wClock.gettime(node);
      struct tm* lTime = localtime( &l_time );

      mins  = lTime->tm_min;
      hours = lTime->tm_hour;

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set clock" );

      cmd[0] = OPC_WR_SL_DATA;
      cmd[1] = 0x0E;
      cmd[2] = 0x7B;
      cmd[3] = wClock.getdivider(node);

      cmd[4] = 0x7F; // fractional minutes L
      cmd[5] = 0x7F; // fractional minutes H
      cmd[6] = (255-(60-mins))&0x7F; // 256 - minutes 43
      cmd[7] = rsp[7]; // track status

      cmd [8] = (256-(24-hours))&0x7F; // 256 - hours 14
      cmd [9] = 0; // clock rollovers
      cmd[10] = 0x70;
      cmd[11] = 0x7F;
      cmd[12] = 0x70;
      cmd[13] = LocoNetOp.checksum( cmd, 13 );

      ok = LocoNetOp.transact( loconet, cmd, 14, rsp, &insize, 0, 0, True );

      return 0;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Could not get fast clock slot. (un expected response 0x%02X...)", rsp[0] );
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Could not get fast clock slot. (no response...)" );
  }

  return 0;
}


/**
 * Create packet for functions 9-28.
 * Group 3=f9-f12, 4=f13-f16, 5=f17-f20, 6=f21-f24, 7=f25-f28
 */
static int __processFunctions(iOLocoNet loconet_inst, iONode node, byte* cmd) {
  iOLocoNetData data = Data(loconet_inst);
  int addr      = wFunCmd.getaddr(node);
  int group     = wFunCmd.getgroup(node);
  int fnchanged = wFunCmd.getfnchanged(node);
  int Fn        = 0;


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "function command for address [%d] in group [%d]", addr, group );

  if( StrOp.equals( wDigInt.sublib_ulni, wDigInt.getsublib( data->ini ) ) ) {
    cmd[0] = 0xD4;
    cmd[1] = 0x20;
    cmd[2] = 0x01;
    if( fnchanged > 8 && fnchanged < 12 ) {
      cmd[3] = 0x07;
      cmd[4]  = wFunCmd.isf9  (node)?0x10:0x00;;
      cmd[4] |= wFunCmd.isf10 (node)?0x20:0x00;;
      cmd[4] |= wFunCmd.isf11 (node)?0x40:0x00;;
      cmd[5] = LocoNetOp.checksum( cmd, 5 );
      return 6;
    }
    if( fnchanged == 12 || fnchanged == 20 || fnchanged == 28 ) {
      cmd[3] = 0x05;
      cmd[4]  = wFunCmd.isf12 (node)?0x10:0x00;;
      cmd[4] |= wFunCmd.isf20 (node)?0x20:0x00;;
      cmd[4] |= wFunCmd.isf28 (node)?0x40:0x00;;
      cmd[5] = LocoNetOp.checksum( cmd, 5 );
      return 6;
    }
    if( fnchanged > 12 && fnchanged < 20 ) {
      cmd[3] = 0x08;
      cmd[4]  = wFunCmd.isf13 (node)?0x01:0x00;;
      cmd[4] |= wFunCmd.isf14 (node)?0x02:0x00;;
      cmd[4] |= wFunCmd.isf15 (node)?0x04:0x00;;
      cmd[4] |= wFunCmd.isf16 (node)?0x08:0x00;;
      cmd[4] |= wFunCmd.isf17 (node)?0x10:0x00;;
      cmd[4] |= wFunCmd.isf18 (node)?0x20:0x00;;
      cmd[4] |= wFunCmd.isf19 (node)?0x40:0x00;;
      cmd[5] = LocoNetOp.checksum( cmd, 5 );
      return 6;
    }
    if( fnchanged > 20 ) {
      cmd[3] = 0x09;
      cmd[4]  = wFunCmd.isf21 (node)?0x01:0x00;;
      cmd[4] |= wFunCmd.isf22 (node)?0x02:0x00;;
      cmd[4] |= wFunCmd.isf23 (node)?0x04:0x00;;
      cmd[4] |= wFunCmd.isf24 (node)?0x08:0x00;;
      cmd[4] |= wFunCmd.isf25 (node)?0x10:0x00;;
      cmd[4] |= wFunCmd.isf26 (node)?0x20:0x00;;
      cmd[4] |= wFunCmd.isf27 (node)?0x40:0x00;;
      cmd[5] = LocoNetOp.checksum( cmd, 5 );
      return 6;
    }

  }
  else {
    byte REPS  = 0;
    byte DHI   = 0;
    byte IM1   = 0;
    byte IM2   = 0;
    byte IM3   = 0;
    byte IM4   = 0;

    /* static part of packet */
    cmd[0] = OPC_IMM_PACKET;
    cmd[1] = 0x0B;
    cmd[2] = 0x7F;
    cmd[9] = 0x00; /* IM5 */

    if( group == 3 ) {
      Fn |= wFunCmd.isf9 (node)?0x01:0x00;
      Fn |= wFunCmd.isf10(node)?0x02:0x00;
      Fn |= wFunCmd.isf11(node)?0x04:0x00;
      Fn |= wFunCmd.isf12(node)?0x08:0x00;
      REPS  = (addr < 128) ? 0x24:0x34;
      DHI   = (addr < 128) ? 0x02:0x04;
      if( addr < 128 ) {
        IM2 = 0x20 | (Fn & 0x0F);
      }
      else {
        IM3 = 0x20 | (Fn & 0x0F);
      }
    }

    else if( group == 4 || group == 5 ) {
      Fn |= wFunCmd.isf13(node)?0x01:0x00;
      Fn |= wFunCmd.isf14(node)?0x02:0x00;
      Fn |= wFunCmd.isf15(node)?0x04:0x00;
      Fn |= wFunCmd.isf16(node)?0x08:0x00;
      Fn |= wFunCmd.isf17(node)?0x10:0x00;
      Fn |= wFunCmd.isf18(node)?0x20:0x00;
      Fn |= wFunCmd.isf19(node)?0x40:0x00;
      Fn |= wFunCmd.isf20(node)?0x80:0x00;
      REPS  = (addr < 128) ? 0x34:0x44;
      DHI   = (addr < 128) ? 0x02:0x04;
      DHI  |= (Fn & 0x80)  ? 0x40:0x00;
      if( addr < 128 ) {
        IM2 = 0x5E;
        IM3 = Fn & 0x7F;
      }
      else {
        IM3 = 0x5E;
        IM4 = Fn & 0x7F;
      }
    }

    else if( group == 6 || group == 7 ) {
      Fn |= wFunCmd.isf21(node)?0x01:0x00;
      Fn |= wFunCmd.isf22(node)?0x02:0x00;
      Fn |= wFunCmd.isf23(node)?0x04:0x00;
      Fn |= wFunCmd.isf24(node)?0x08:0x00;
      Fn |= wFunCmd.isf25(node)?0x10:0x00;
      Fn |= wFunCmd.isf26(node)?0x20:0x00;
      Fn |= wFunCmd.isf27(node)?0x40:0x00;
      Fn |= wFunCmd.isf28(node)?0x80:0x00;
      REPS  = (addr < 128) ? 0x34:0x44;
      DHI   = (addr < 128) ? 0x06:0x06;
      DHI  |= (Fn & 0x80)  ? 0x80:0x00;
      if( addr < 128 ) {
        IM2 = 0x5F;
        IM3 = Fn & 0x7F;
      }
      else {
        IM3 = 0x5F;
        IM4 = Fn & 0x7F;
      }
    }


    if( addr < 128 ) {
      cmd[3] = REPS;  /* REPS */
      cmd[4] = DHI;   /* DHI  */
      cmd[5] = addr;  /* IM1  */
      cmd[6] = IM2;   /* IM2 */
      cmd[7] = IM3;   /* IM3  */
      cmd[8] = IM4;   /* IM4  */
    }
    else {
      cmd[3] = REPS;  /* REPS */
      cmd[4] = DHI;   /* DHI  */

      if( ((addr / 256) + 192) & 0x80 > 0 )
        cmd[4] |= 0x01;

      if( ((addr % 256) & 0x80) > 0 )
        cmd[4] |= 0x02;

      cmd[5] = ((addr / 256) + 192) & 0x7F; /* IM1 */
      cmd[6] = (addr % 256) & 0x7F;         /* IM2 */
      cmd[7] = IM3;   /* IM3 */
      cmd[8] = IM4;   /* IM4  */
    }


    cmd[10] = LocoNetOp.checksum( cmd, 10 );
    return 11;
  }

  return 0;
}


static int __translate( iOLocoNet loconet_inst, iONode node, byte* cmd, Boolean* delnode ) {
  iOLocoNetData data = Data(loconet_inst);
  *delnode = True;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "cmd=%s", NodeOp.getName( node ) );

  /* new ini: update the references and write, if wanted the options  */
  if( StrOp.equals( NodeOp.getName( node ), wDigInt.name() ) ) {
    iONode loconet = wDigInt.getloconet(node);
    data->ini = node;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "modified ini received." );
    if( loconet != NULL ) {
      data->loconet = loconet;
      iONode options = wLocoNet.getoptions( loconet );
      if( options != NULL && wCSOptions.isstore( options ) ) {
        data->options = options;
        /* write the options in the config slot: */
        __writeConfig(loconet_inst);
      }
    }
    *delnode = False;
    return 0;
  }
  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int addr = wSwitch.getaddr1( node );
    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );
    int dir  = 1;
    int action = 1;

    if( port == 0 )
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      AddrOp.fromPADA( port, &addr, &port );

    addr = (addr-1) * 4 + (port-1);

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      dir = 0; /* thrown */

    if( wSwitch.issinglegate( node ) ) {
      dir = gate;
      if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.straight ) )
        action = 0;
    }

    cmd[0] = OPC_SW_REQ;

    cmd[1]  = (unsigned short int) (addr & 0x007f);
    cmd[2]  = (unsigned short int) (( addr >> 7) & 0x000f);
    cmd[2] |= (unsigned short int) ( (dir & 0x0001) << 5);
    cmd[2] |= (unsigned short int) ( (action & 0x0001) << 4);
    cmd[3] = LocoNetOp.checksum( cmd, 3 );

    if( wSwitch.isactdelay( node ) ) {
      /* decoder does not deactivate the output */
      ThreadOp.post( data->swReset, NodeOp.base.clone(node) );
    }

    return 4;
  }
  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
    return 0;
  }
  /* Output command */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

    if( port == 0 )
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      AddrOp.fromPADA( port, &addr, &port );

    addr = (addr-1) * 4 + (port-1);

    cmd[0] = OPC_SW_REQ;

    cmd[1]  = (unsigned short int) (addr & 0x007f);
    cmd[2]  = (unsigned short int) (( addr >> 7) & 0x000f);
    cmd[2] |= (unsigned short int) ( (gate & 0x0001) << 5);
    cmd[2] |= (unsigned short int) ( (action & 0x0001) << 4);
    cmd[3] = LocoNetOp.checksum( cmd, 3 );

    return 4;
  }

  /* Function command groups > 1 */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) && wFunCmd.getgroup(node) > 2 ) {
    return __processFunctions(loconet_inst, node, cmd);
  }


  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ||
           StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int status = 0;
    int slot   = 0;
    int size   = 0;

    if( MutexOp.trywait( data->slotmux, 1000 ) ) {

      slot =  __getLocoSlot( loconet_inst, node, &status);

      if( slot > 0 && StrOp.equals( wLoc.dispatch, wLoc.getcmd(node) ) ) {
        /* set as purged: */
        data->locoslot[slot] = 0;
        data->slotaccessed[slot] = 0;
        size = makereqDispatch(data, cmd, slot, node, status, data->activeSlotServer );
      }
      else if( slot > 0 ) {
        Boolean fn1 = wFunCmd.isf1(node);
        Boolean fn2 = wFunCmd.isf2(node);
        Boolean fn3 = wFunCmd.isf3(node);
        Boolean fn4 = wFunCmd.isf4(node);
        Boolean fn5 = wFunCmd.isf5(node);
        Boolean fn6 = wFunCmd.isf6(node);
        Boolean fn7 = wFunCmd.isf7(node);
        Boolean fn8 = wFunCmd.isf8(node);

        int V = 0;
        int snd = 0;
        int step = 0x7F; /*wLoc.getspcnt( node );*/
        int dirf = wLoc.isdir( node )?0x00:0x20;
        dirf |= wLoc.isfn( node )?0x10:0x00;
        dirf |= (fn1 << 0);
        dirf |= (fn2 << 1);
        dirf |= (fn3 << 2);
        dirf |= (fn4 << 3);
        snd  |= (fn5 << 0);
        snd  |= (fn6 << 1);
        snd  |= (fn7 << 2);
        snd  |= (fn8 << 3);

        if( wLoc.getV( node ) != -1 ) {
          float fV = wLoc.getV( node ) * 127;
          float div = 100;
          if( !StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) && wLoc.getV_max( node ) > 0 )
            div = wLoc.getV_max( node );
          fV = fV / div;
          V = (int)fV;
          if( fV - V >= 0.5 )
            V++;
        }

        /* Remove e-stop in 128 speed step mode */
        if ( wLoc.getspcnt( node ) == 128 ){
          if ( V == 1 )
            V = 0;
        }


        /* keep this value for the ping thread */
        data->slotV[slot] = V;

        {
          cmd[0] = 4;
          cmd[1] = OPC_LOCO_SPD;
          cmd[2] = slot; /* slot number */
          cmd[3] = V;
          cmd[4] = LocoNetOp.checksum( cmd+1, 3 );

          byte* bcmd = allocMem( 64 );
          MemOp.copy( bcmd, cmd, 32 );
          ThreadOp.prioPost( data->loconetWriter, (obj)bcmd, high );
        }

        {
          cmd[0] = 4;
          cmd[1] = OPC_LOCO_DIRF;
          cmd[2] = slot; /* slot number */
          cmd[3] = dirf;
          cmd[4] = LocoNetOp.checksum( cmd+1, 3 );

          byte* bcmd = allocMem( 64 );
          MemOp.copy( bcmd, cmd, 32 );
          ThreadOp.prioPost( data->loconetWriter, (obj)bcmd, high );
        }

        cmd[0] = OPC_LOCO_SND;
        cmd[1] = slot; /* slot number */
        cmd[2] = snd;
        cmd[3] = LocoNetOp.checksum( cmd, 3 );

        size = 4;
      }

      /* Release the mutex. */
      MutexOp.post( data->slotmux );
    }
    return size;
  }


  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SysCmd %s", cmdstr );

    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      if( !wLocoNet.isignorepowercmds(data->loconet) || !data->powerison ) {
        cmd[0] = wLocoNet.isuseidle(data->loconet)?OPC_IDLE:OPC_GPOFF;
        cmd[1] = LocoNetOp.checksum( cmd, 1 );
        data->powerison = False;
        return 2;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Ignoring power off command for %s.", data->iid );
        return 0;
      }
    }
    if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      if( !wLocoNet.isignorepowercmds(data->loconet) || !data->powerison ) {
        cmd[0] = OPC_GPON;
        cmd[1] = LocoNetOp.checksum( cmd, 1 );
        data->powerison = True;
        return 2;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Ignoring power on command for %s.", data->iid );
        return 0;
      }
    }
    if( StrOp.equals( cmdstr, wSysCmd.ebreak ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "emergency break for [%s]", data->iid );
      cmd[0] = OPC_IDLE;
      cmd[1] = LocoNetOp.checksum( cmd, 1 );
      return 2;
    }
    if( StrOp.equals( cmdstr, wSysCmd.slots ) ) {
      __getSlots(loconet_inst);
      return 0;
    }
    if( StrOp.equals( cmdstr, wSysCmd.config ) ) {
      __getConfig(loconet_inst);
      return 0;
    }
    if( StrOp.equals( cmdstr, wSysCmd.loccnfg ) ) {
      int addr = wSysCmd.getval( node );
      int decformat = wSysCmd.getvalA( node );
      int speedsteps = wSysCmd.getvalB( node );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Loc[%d]: format=%d, steps=%d", addr, decformat, speedsteps  );
      return 0;
    }
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    if(  wProgram.getcmd( node ) == wProgram.ptstat ) {
      return 0;
    }
    else if(  wProgram.getcmd( node ) == wProgram.pton ) {
      /* if cs == ibcom */
      if( StrOp.equals( wLocoNet.cs_ibcom, wLocoNet.getcmdstn( data->loconet ) ) ) {
        byte* bcmd = allocMem( 64 );
        int outsize = startIBComPT(cmd+1);
        cmd[0] = outsize;
        MemOp.copy( bcmd, cmd, 64 );
        ThreadOp.prioPost( data->loconetWriter, (obj)bcmd, normal );
        cmd[0] = wLocoNet.isuseidle(data->loconet)?OPC_IDLE:OPC_GPOFF;
        cmd[1] = LocoNetOp.checksum( cmd, 1 );
        return 2;
      }
      return 0;
    }
    else if( wProgram.getcmd( node ) == wProgram.ptoff ) {
      /* if cs == ibcom */
      if( StrOp.equals( wLocoNet.cs_ibcom, wLocoNet.getcmdstn( data->loconet ) ) ) {
        return stopIBComPT(cmd);
      }
      return 0;
    }
    else if( wProgram.getcmd( node ) == wProgram.get ) {
      int cv = wProgram.getcv( node );
      int decaddr = wProgram.getdecaddr( node );
      int addr = decaddr == 0 ? wProgram.getaddr( node ):decaddr;
      Boolean pom = wProgram.ispom( node );
      Boolean direct = wProgram.isdirect( node );
      int size = 0;
      if( StrOp.equals( wLocoNet.cs_ibcom, wLocoNet.getcmdstn( data->loconet ) ) )
        size = makeIBComCVPacket( cv, 0, cmd, False);
      else
        size = __rwCV(loconet_inst, cv, 0, cmd, False, pom, direct, addr);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get CV%d of %d (ops=%d)...", cv, addr, pom );
      return size;
    }
    else if( wProgram.getcmd( node ) == wProgram.set ) {
      int cv = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int decaddr = wProgram.getdecaddr( node );
      int addr = decaddr == 0 ? wProgram.getaddr( node ):decaddr;
      Boolean pom = wProgram.ispom( node );
      Boolean direct = wProgram.isdirect( node );
      int size = 0;
      if( StrOp.equals( wLocoNet.cs_ibcom, wLocoNet.getcmdstn( data->loconet ) ) )
        size = makeIBComCVPacket( cv, value, cmd, True);
      else
        size = __rwCV(loconet_inst, cv, value, cmd, True, pom, direct, addr);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set CV%d to %d of %d (ops=%d)...", cv, value, addr, pom );
      return size;
    }
    else if( wProgram.getcmd( node ) == wProgram.lncvget ) {
      int cv = wProgram.getcv( node );
      int addr = wProgram.getaddr( node );
      int value = wProgram.getvalue( node );
      int modid = wProgram.getmodid( node );
      int lncvcmd = wProgram.getlncvcmd( node );
      int size = 0;
      if( wProgram.getlntype(node) == wProgram.lntype_cv ) {
        size = __rwLNCV(loconet_inst, cv, value, cmd, False, modid, addr, lncvcmd);
      }
      else if( wProgram.getlntype(node) == wProgram.lntype_sv ) {
        size = __rwLNSV(loconet_inst, cv, value, cmd, False, addr, modid, lncvcmd);
      }
      else if( wProgram.getlntype(node) == wProgram.lntype_mp ) {
        size = __rwLNMP(loconet_inst, cv, value, cmd, False, addr, modid, lncvcmd);
      }
      else if( wProgram.getlntype(node) == wProgram.lntype_opsw ) {
        size = __rwLNOPSW(loconet_inst, addr, modid, cv, value, cmd, False);
      }
      return size;
    }
    else if( wProgram.getcmd( node ) == wProgram.lncvset ) {
      int cv = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int addr = wProgram.getaddr( node );
      int modid = wProgram.getmodid( node );
      int lncvcmd = wProgram.getlncvcmd( node );
      int size = 0;
      if( wProgram.getlntype(node) == wProgram.lntype_cv ) {
        size = __rwLNCV(loconet_inst, cv, value, cmd, True, modid, addr, lncvcmd);
      }
      else if( wProgram.getlntype(node) == wProgram.lntype_sv ) {
        size = __rwLNSV(loconet_inst, cv, value, cmd, True, addr, modid, lncvcmd);
      }
      else if( wProgram.getlntype(node) == wProgram.lntype_mp ) {
        size = __rwLNMP(loconet_inst, cv, value, cmd, True, addr, modid, lncvcmd);
      }
      else if( wProgram.getlntype(node) == wProgram.lntype_opsw ) {
        size = __rwLNOPSW(loconet_inst, addr, modid, cv, value, cmd, True);
      }
      return size;
    }
  }

  /* Clock command. */
  else if( wLocoNet.isusefc(data->loconet) && StrOp.equals( NodeOp.getName( node ), wClock.name() ) ) {
    /* Fast Clock */

    if(  StrOp.equals( wClock.getcmd( node ), wClock.freeze ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "freeze clock" );
      wClock.setcmd( node, wClock.set );
      wClock.setdivider( node, 0 );
    }
    else if(  StrOp.equals( wClock.getcmd( node ), wClock.go ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "go clock" );
      wClock.setcmd( node, wClock.set );
    }
    else if(  StrOp.equals( wClock.getcmd( node ), wClock.sync ) ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sync clock" );
      if( wLocoNet.issyncfc(data->loconet) || !data->fcsync ) {
        wClock.setcmd( node, wClock.set );
        data->fcsync = True;
      }
    }

    if(  StrOp.equals( wClock.getcmd( node ), wClock.set ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set clock" );
      return __setFastClock(loconet_inst, node, cmd);
    }
  }


  return 0;
}


static void __writeStatus( iOLocoNet loconet, int slot, byte status, int statusflag ) {
  byte cmd[4];
  cmd[0] = OPC_SLOT_STAT1;
  cmd[1] = slot;
  cmd[2] = (status&~LOCOSTAT_MASK)|statusflag;
  cmd[3] = LocoNetOp.checksum( cmd, 3 );
  LocoNetOp.transact( loconet, cmd, 4, NULL, NULL, 0, 0, False );
}


/*
 * A Fred will try to take over the loco dispathed in slot 0.
 *
 * So, to prepare a loco for Fred it should be moved into slot 0.
 * A Fred queries slot 0, and when there is a dispatched loco in it,
 * it will get a OPC_SL_RD_DATA reply.
 */
int makereqDispatch(iOLocoNetData data, byte *msg, int slot, iONode node, int status, Boolean activeSlotServer) {
  int addr = wLoc.getaddr(node);

  msg[0] = OPC_SLOT_STAT1;
  msg[1] = slot;
  msg[2] = (status&~LOCOSTAT_MASK)|LOCO_COMMON;
  msg[3] = LocoNetOp.checksum( msg, 3 );


  {
    msg[0] = 4;
    msg[1] = OPC_SLOT_STAT1;
    msg[2] = slot;
    msg[3] = (status&~LOCOSTAT_MASK)|LOCO_COMMON;
    msg[4] = LocoNetOp.checksum( msg+1, 3 );

    byte* bcmd = allocMem( 64 );
    MemOp.copy( bcmd, msg, 32 );
    ThreadOp.prioPost( data->loconetWriter, (obj)bcmd, high );
  }


  msg[0] = OPC_MOVE_SLOTS;
  msg[1] = slot & 0x7F;
  msg[2] = 0;
  msg[3] = LocoNetOp.checksum( msg, 3 );


  return 4;
}



static __writeSlotData(byte* cmd, int slot, int addr, int V, int dirf) {
  cmd[ 0] = OPC_WR_SL_DATA;
  cmd[ 1] = 0x0E; /* message length */
  cmd[ 2] = slot; /* slot number */
  cmd[ 3] = 0x00; /* slot status */
  cmd[ 4] = 0x00; /* address */
  cmd[ 5] = V;    /* speed 0-127 1=emergency stop*/
  cmd[ 6] = dirf; /* direction(5) and functions 1-4,0*/
  cmd[ 7] = 0x00; /* track */
  cmd[ 8] = 0x00; /* slot status */
  cmd[ 9] = 0x00; /* slot high address part */
  cmd[10] = 0x00; /* functions 5-8 */
  cmd[11] = 0x00; /* ID1 */
  cmd[12] = 0x00; /* ID2 */
  cmd[13] = LocoNetOp.checksum( cmd, 13 );
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOLocoNetData data = Data(inst);
  char out[256];
  Boolean delnode = True;

  if( !data->commOK ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "No inited LocoNet interface!" );
    return NULL;
  }

  if( cmd != NULL ) {
    int outsize = __translate( (iOLocoNet)inst, cmd, out+1, &delnode );
    Boolean lccmd = StrOp.equals( NodeOp.getName(cmd), wLoc.name() );

    if( outsize > 0 ) {
      byte* bcmd = allocMem( 64 );
      out[0] = outsize;
      MemOp.copy( bcmd, out, 64 );
      ThreadOp.prioPost( data->loconetWriter, (obj)bcmd, lccmd ? high:normal );

      /*LocoNetOp.transact( (iOLocoNet)inst, out+1, outsize, NULL, NULL, 0, 0 );*/
    }

    /* Cleanup Node1 */
    if( delnode )
      cmd->base.del(cmd);
  }

  return NULL;
}


/**  */
static void _halt( obj inst, Boolean poweroff ) {
  iOLocoNetData data = Data(inst);
  if( data->swReset != NULL ) {
    iONode quitNode = NodeOp.inst( "quit", NULL, ELEMENT_NODE );
    ThreadOp.post( data->swReset, (obj)quitNode );
  }

  if( !data->commOK ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "No inited LocoNet interface!" );
    return;
  }
  else {
    if( wDigInt.ispoweroffexit(data->ini) || poweroff ) {
      byte* bcmd = allocMem(64);
      bcmd[0] = 2;
      bcmd[1] = wLocoNet.isuseidle(data->loconet)?OPC_IDLE:OPC_GPOFF;
      bcmd[2] = LocoNetOp.checksum( bcmd+1, 1 );
      ThreadOp.prioPost( data->loconetWriter, (obj)bcmd, high );
    }

    if( data->activeSlotServer ) {
      iONode quitNode = NodeOp.inst( "quit", NULL, ELEMENT_NODE );
      ThreadOp.post( data->slotServer, (obj)quitNode );
    }

  }
  ThreadOp.sleep(400); /* time for the last commands to send */
  data->run = False;
  ThreadOp.sleep(100); /* time for the last commands to send */
  data->lnDisconnect(inst);
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOLocoNetData data = Data(inst);
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
  iOLocoNetData data = Data(inst);
  return True;
}


/* Status */
static int _state( obj inst ) {
  iOLocoNetData data = Data(inst);
  int state = 0;
  state |= data->power << 0;
  state |= data->pt << 1;
  state |= data->comm << 2;
  return state;
}

/* external shortcut event */
static void _shortcut(obj inst) {
  iOLocoNetData data = Data( inst );
}


/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOLocoNetData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __initLocoSlots( iOLocoNet loconet ) {
  iOLocoNetData data = Data(loconet);
  int i = 0;
  for( i = 0; i < 256; i++ ) {
    data->locoslot[i] = 0;
  }
}


/**  */
static struct OLocoNet* _inst( const iONode ini ,const iOTrace trc ) {
  iOLocoNet __LocoNet = allocMem( sizeof( struct OLocoNet ) );
  iOLocoNetData data = allocMem( sizeof( struct OLocoNetData ) );
  MemOp.basecpy( __LocoNet, &LocoNetOp, 0, sizeof( struct OLocoNet ), data );

  TraceOp.set( trc );
  /* Initialize data->xxx members... */


  /* Evaluate attributes. */
  data->ini      = ini;

  data->dummyio = wDigInt.isdummyio( ini );

  data->loconet = wDigInt.getloconet(ini);
  if( data->loconet == NULL ) {
    data->loconet = NodeOp.inst( wLocoNet.name(), ini, ELEMENT_NODE );
    NodeOp.addChild( ini, data->loconet );
  }

  data->options = wLocoNet.getoptions(data->loconet);
  if( data->options == NULL ) {
    data->options = NodeOp.inst( wCSOptions.name(), ini, ELEMENT_NODE );
    NodeOp.addChild( data->loconet, data->options );
  }

  data->slotserver = wLocoNet.getslotserver(data->loconet);
  if( data->slotserver == NULL ) {
    data->slotserver = NodeOp.inst( wLNSlotServer.name(), ini, ELEMENT_NODE );
    NodeOp.addChild( data->loconet, data->slotserver );
  }

  data->device   = StrOp.dup( wDigInt.getdevice( ini ) );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  data->timeout  = wDigInt.gettimeout( ini );
  data->swtime   = wDigInt.getswtime( ini );

  data->run      = True;

  data->commOK   = False;
  data->initOK   = False;

  data->purgetime = wLocoNet.getpurgetime(data->loconet);
  data->slots = wLocoNet.getslots(data->loconet);
  data->activeSlotServer = wLNSlotServer.isactive(data->slotserver);
  data->serveLConly = wLNSlotServer.islconly(data->slotserver);
  data->doSensorQuery = wLocoNet.issensorquery(data->loconet);
  data->stress = wDigInt.isstress(ini);

  data->didSensorQuery = False;

  data->mux = MutexOp.inst( NULL, True );
  data->slotmux = MutexOp.inst( NULL, True );
  data->initPacket[0] = 0;

  __initLocoSlots(__LocoNet);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loconet %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  if( data->activeSlotServer ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Slotserver is active;\nDo not use this if there is a Command Station in this LocoNet!" );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     =%s", wDigInt.getiid( ini ) != NULL ? wDigInt.getiid( ini ):"" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib  =%s", wDigInt.getsublib( ini ) );

  /* choose interface: */
  if( StrOp.equals( wDigInt.sublib_socket, wDigInt.getsublib( ini ) ) ) {
    /* lbserver */
    data->lnConnect    = lbserverConnect;
    data->lnDisconnect = lbserverDisconnect;
    data->lnRead       = lbserverRead;
    data->lnWrite      = lbserverWrite;
    data->lnAvailable  = lbserverAvailable;
  }
  else if( StrOp.equals( wDigInt.sublib_serial, wDigInt.getsublib( ini ) ) ||
           StrOp.equals( wDigInt.sublib_digitrax_pr3, wDigInt.getsublib( ini ) ) ||
           StrOp.equals( wDigInt.sublib_native, wDigInt.getsublib( ini ) ) ||
           StrOp.equals( wDigInt.sublib_default, wDigInt.getsublib( ini ) ) ) {
    /* lbserial */
    data->lnConnect    = lbserialConnect;
    data->lnDisconnect = lbserialDisconnect;
    data->lnRead       = lbserialRead;
    data->lnWrite      = lbserialWrite;
    data->lnAvailable  = lbserialAvailable;
  }
  else if( StrOp.equals( wDigInt.sublib_udp, wDigInt.getsublib( ini ) ) ) {
    /* lbudp */
    data->lnConnect    = lbUDPConnect;
    data->lnDisconnect = lbUDPDisconnect;
    data->lnRead       = lbUDPRead;
    data->lnWrite      = lbUDPWrite;
    data->lnAvailable  = lbUDPAvailable;
  }
  else if( StrOp.equals( wDigInt.sublib_tcp, wDigInt.getsublib( ini ) ) ) {
    /* lbudp */
    data->lnConnect    = lbTCPConnect;
    data->lnDisconnect = lbTCPDisconnect;
    data->lnRead       = lbTCPRead;
    data->lnWrite      = lbTCPWrite;
    data->lnAvailable  = lbTCPAvailable;
  }
  else if( StrOp.equals( wDigInt.sublib_ulni, wDigInt.getsublib( ini ) ) ) {
    /* ulni */
    data->lnConnect    = ulniConnect;
    data->lnDisconnect = ulniDisconnect;
    data->lnRead       = ulniRead;
    data->lnWrite      = ulniWrite;
    data->lnAvailable  = ulniAvailable;
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unsupported sublib [%s], using default.", wDigInt.getsublib( ini ) );
    wDigInt.setsublib( ini, wDigInt.sublib_serial );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib  =%s", wDigInt.getsublib( ini ) );
    /* lbserial */
    data->lnConnect    = lbserialConnect;
    data->lnDisconnect = lbserialDisconnect;
    data->lnRead       = lbserialRead;
    data->lnWrite      = lbserialWrite;
    data->lnAvailable  = lbserialAvailable;
  }

  data->commOK = data->lnConnect((obj)__LocoNet);

  if( data->commOK ) {
    if( data->stress ) {
      data->stressRunner = ThreadOp.inst( "lnstress", &__stressRunner, __LocoNet );
      ThreadOp.start( data->stressRunner );
    }

    data->loconetReader = ThreadOp.inst( "lnreader", &__loconetReader, __LocoNet );
    ThreadOp.start( data->loconetReader );

    data->loconetWriter = ThreadOp.inst( "lnwriter", &__loconetWriter, __LocoNet );
    ThreadOp.start( data->loconetWriter );

    data->swReset = ThreadOp.inst( "swreset", &__swReset, __LocoNet );
    ThreadOp.start( data->swReset );

    if( data->purgetime > 0 && wLocoNet.isslotping(data->loconet) ) {
      data->slotPing = ThreadOp.inst( "slotping", &__slotPing, __LocoNet );
      ThreadOp.start( data->slotPing );
    }

    if( data->activeSlotServer ) {
      data->slotServer = ThreadOp.inst( "slotsrvr", &lnmasterThread, __LocoNet );
      ThreadOp.start( data->slotServer );
    }

    if( data->initPacket[0] > 0 ) {
      byte* bcmd = allocMem( 128 );
      MemOp.copy( bcmd, data->initPacket, min(data->initPacket[0],127) );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Send %d byte init packet", data->initPacket[0] & 0xFF );
      ThreadOp.prioPost( data->loconetWriter, (obj)bcmd, high );
    }

  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init LocoNet interface!" );
  }

  instCnt++;
  return __LocoNet;
}


/* Both functions are optimized for speed and will not do any range checking.
 * Supported are ASCII and UTF-8. (single byte encoding if < 128)
 */
static void _ascii2byte( const char* in, int len, byte* out ) {
  static byte __ascii2byte[] = {0,1,2,3,4,5,6,7,8,9,0,0,0,0,0,0,0,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F};
  int i = 0;
  for( i = 0; i < len; i+=2 ) {
    out[i/2] = ( __ascii2byte[ in[i]-'0' ] << 4 ) + __ascii2byte[ in[i+1]-'0' ];
  }
}
static void _byte2ascii( const byte* in, int len, char* out ) {
  static char __byte2ascii[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  int i = 0;
  for( i = 0; i < len; i++ ) {
    out[i*2+0] = __byte2ascii[ (in[i] >> 4) & 0x0F ];
    out[i*2+1] = __byte2ascii[ in[i] & 0x0F ];
  }
  out[i*2+0] = '\0';
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/loconet.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
