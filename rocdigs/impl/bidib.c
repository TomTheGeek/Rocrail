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



#include "rocdigs/impl/bidib_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/BiDiB.h"
#include "rocrail/wrapper/public/BiDiBnode.h"
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
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"

#include "rocdigs/impl/bidib/bidib_messages.h"
#include "rocdigs/impl/bidib/serial.h"

static int instCnt = 0;
static Boolean TEST = False;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOBiDiBData data = Data(inst);
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

/** ----- OBiDiB ----- */


static void __inform( iOBiDiB inst ) {
  iOBiDiBData data = Data(inst);
  iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
  wState.setiid( node, wDigInt.getiid( data->ini ) );
  wState.setpower( node, data->power );
  data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
}


/*
 */
static const char* __getFeatureName(int feature) {

  /* Occupancy detector */
  if( feature == FEATURE_BM_SIZE ) return "number of sensors";
  if( feature == FEATURE_BM_ON ) return "activated sensors events";
  if( feature == FEATURE_BM_SECACK_AVAILABLE ) return "secure-ACK available";
  if( feature == FEATURE_BM_SECACK_ON ) return "secure-ACK interval";
  if( feature == FEATURE_BM_CURMEAS_AVAILABLE ) return "current measurement available";
  if( feature == FEATURE_BM_CURMEAS_INTERVAL ) return "current measurement interval";
  if( feature == FEATURE_BM_DC_MEAS_AVAILABLE ) return "replacement detection available";
  if( feature == FEATURE_BM_DC_MEAS_ON ) return "replacement detection enabled";
  if( feature == FEATURE_BM_ADDR_DETECT_AVAILABLE ) return "address detection available";
  if( feature == FEATURE_BM_ADDR_DETECT_ON ) return "address detection enabled";
  if( feature == FEATURE_BM_ADDR_AND_DIR ) return "direction available";
  if( feature == FEATURE_BM_ISTSPEED_AVAILABLE ) return "dcc-speed available";
  if( feature == FEATURE_BM_ISTSPEED_ON ) return "dcc-speed enabled";
  if( feature == FEATURE_BM_CV_AVAILABLE ) return "cv-messages available";
  if( feature == FEATURE_BM_CV_ON ) return "cv-messages enabled";

  /* Booster */
  if( feature == FEATURE_BST_VOLT_ADJUSTABLE ) return "adjustable output voltage";
  if( feature == FEATURE_BST_VOLT ) return "output voltage value in V";
  if( feature == FEATURE_BST_CUTOUT_AVAIALABLE ) return "cutout available";
  if( feature == FEATURE_BST_CUTOUT_ON ) return "cutout enabled";
  if( feature == FEATURE_BST_TURNOFF_TIME ) return "turnoff time";
  if( feature == FEATURE_BST_INRUSH_TURNOFF_TIME ) return "inrush turnoff time";
  if( feature == FEATURE_BST_AMPERE_ADJUSTABLE ) return "ampere adjustable";
  if( feature == FEATURE_BST_AMPERE ) return "ampere";

  /* Control */
  if( feature == FEATURE_CTRL_INPUT_COUNT ) return "input count";
  if( feature == FEATURE_CTRL_INPUT_NOTIFY ) return "input notify";
  if( feature == FEATURE_CTRL_SPORT_COUNT ) return "short port count";
  if( feature == FEATURE_CTRL_LPORT_COUNT ) return "long port count";
  if( feature == FEATURE_CTRL_SERVO_COUNT ) return "servo count";
  if( feature == FEATURE_CTRL_SOUND_COUNT ) return "sound count";
  if( feature == FEATURE_CTRL_MOTOR_COUNT ) return "motor count";
  if( feature == FEATURE_CTRL_ANALOG_COUNT ) return "analog count";
  if( feature == FEATURE_CTRL_MAC_LEVEL ) return "MAC level";
  if( feature == FEATURE_CTRL_MAC_SAVE ) return "MAC save";
  if( feature == FEATURE_CTRL_MAC_COUNT ) return "MAC count";
  if( feature == FEATURE_CTRL_MAC_SIZE ) return "MAC size";
  if( feature == FEATURE_CTRL_MAC_START_MAN ) return "MAC start manuel";
  if( feature == FEATURE_CTRL_MAC_START_DCC ) return "MAC start DCC";

  /* DCC Gen */
  if( feature == FEATURE_GEN_SPYMODE ) return "DCC spymode";
  if( feature == FEATURE_GEN_WATCHDOG ) return "DCC watchdog";
  if( feature == FEATURE_GEN_DRIVE_ACK ) return "DCC drive ack";
  if( feature == FEATURE_GEN_SWITCH_ACK ) return "DCC switch ack";
  if( feature == FEATURE_GEN_LOK_DB_SIZE ) return "DCC loco DB size";
  if( feature == FEATURE_GEN_LOK_DB_STRING ) return "DCC loco DB string";
  if( feature == FEATURE_GEN_SERVICE_MODES ) return "DCC service modes";

  return "*** unknown feature ***";
}

/*
Ein serielles Paket ist prinzipiell wie folgt aufgebaut:
  PAKET ::= MAGIC MESSAGE_SEQ CRC [MAGIC]
  MESSAGE_SEQ ::= MESSAGE MESSAGE_SEQ

Ein serielles PAKET beginnt immer mit speziellen Zeichen ([MAGIC]=0xFE) und kann eine oder mehrere Nachrichten (MESSAGE) enthalten.
Das ganze Paket ist mit einer CRC (Cyclic Redundancy Check) abgesichert, um Datenfehler bei der Übertragung erkennen zu können.
MAGIC-Zeichen, welche innerhalb von Nachrichten auftauchen, werden 'Escaped'. Hierzu wird ein ESCAPE Zeichen (=0xFD) eingefügt und
das nachfolgende Zeichen mit 0x20 xor-verknüpft. Auch das ESCAPE-Zeichen selbst wird innerhalb der Nachricht Escaped.
Das heißt: Anstelle des MAGIC wird ein ESCAPE-Zeichen (=0xFD), gefolgt von MAGIC ^ 0x20 = 0xDE gesendet.
Anstelle des ESCAPE-Zeichen wird 0xFD + 0xDD gesendet. Das Escapen erfolgt auf dem fertig kodierten PAKET inkl.
*/
static void __escapeMessage(byte* msg, int* newLen, int inLen) {
  int outLen = 0;
  int i = 0;
  byte buffer[256];

  for( i = 0; i < inLen; i++ ) {
    if( (msg[i] == BIDIB_PKT_MAGIC) || (msg[i] == BIDIB_PKT_ESCAPE) )
    {
      buffer[outLen] = BIDIB_PKT_ESCAPE;        // escape this char
      outLen++;
      buffer[outLen] = msg[i] ^ 0x20;           // 'veraendern'
      outLen++;
    }
    else {
      buffer[outLen] = msg[i];
      outLen++;
    }
  }

  *newLen = outLen;
  MemOp.copy( msg, buffer, outLen );
  TraceOp.dump ( name, TRCLEVEL_DEBUG, (char*)msg, outLen );
}


static int __deEscapeMessage(byte* msg, int inLen) {
  int outLen = 0;
  int i = 0;
  byte buffer[256];
  Boolean escape = False;

  for( i = 0; i < inLen; i++ ) {
    if( msg[i] == BIDIB_PKT_ESCAPE ) {
      escape = True;
    }
    else {
      buffer[outLen] = (escape ? msg[i]^0x20:msg[i]);
      outLen++;
      escape = False;
    }
  }

  MemOp.copy( msg, buffer, outLen );
  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "message de-escaped" );
  TraceOp.dump ( name, TRCLEVEL_DEBUG, (char*)msg, outLen );
  return outLen;
}



/*
CRC (d.h. die CRC wird über die Nachricht(en) ohne MAGIC, ohne ESCAPE gebildet).
Die MESSAGE ist vom Host an einen bestimmten Knoten adressiert. In einem Paket können MESSAGES auch an verschiedene Knoten
adressiert sein.

CRC bezeichnet das CRC8-Byte; Auf der Senderseite wird das gemäß Polynom x8 + x5 + x4 + 1 über die Nachricht gebildet,
beginnend beim ersten Byte der Nachricht, Init=0, nicht invertiert. Empfängerseitig wird die CRC mit dem gleichen Polynom über
die gesamte Nachricht inkl. CRC gebildet, das Ergebnis muß 0 sein.
Nach den Paket schließt sich ein MAGIC an, dies kann auch gleichzeitig der Beginn des nächsten Paketes sein.
Wenn kein weiteres Paket zum Senden bereit ist, so wird trotzdem die MAGIC übertragen.
*/
/* Update 8-bit CRC value
   using polynomial X^8 + X^5 + X^4 + 1 */
#define POLYVAL 0x8C
static void __updateCRC(byte new, byte* crc)
{
  int i;
  byte c = *crc;
  for (i = 0; i < 8; i++) {
    if ((c ^ new) & 1)
      c = (c >> 1 ) ^ POLYVAL;
    else
      c >>= 1;
    new >>= 1;
  }
  *crc = c;
}

/*
CRC-8-Dallas/Maxim
x8 + x5 + x4 + 1 (1-Wire bus)

Representations: normal / **reversed** / reverse of reciprocal
0x31 / 0x8C / 0x98

Initialized with 0x00

 */
static byte __checkSum(byte* packet, int len) {
  byte checksum = 0x00;
  int i = 0;
  for( i = 0; i < len; i++ ) {
    __updateCRC(packet[i], &checksum);
  }

  return checksum;
}

static int __makeMessage(byte* msg, int inLen) {
  int outLen = 0;
  byte buffer[256];
  buffer[outLen] = BIDIB_PKT_MAGIC;
  outLen++;
  MemOp.copy( buffer + 1, msg, inLen );
  outLen += inLen;
  buffer[outLen] = __checkSum(buffer+1, outLen-1 );
  outLen++;
  __escapeMessage(buffer+1, &outLen, outLen-1);
  outLen++;
  buffer[outLen] = BIDIB_PKT_MAGIC;
  outLen++;
  MemOp.copy(msg, buffer, outLen);
  return outLen;
}




static iONode __translate( iOBiDiB inst, iONode node ) {
  iOBiDiBData data = Data(inst);
  iONode rsp = NULL;
  byte msg[256];

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "cmd=%s", NodeOp.getName( node ) );

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      msg[0] = 3; // length
      msg[1] = 0; // address
      msg[2] = data->downSeq; // sequence number 1...255
      msg[3] = MSG_BOOST_OFF; //data
      int size = __makeMessage(msg, 4);
      data->subWrite((obj)inst, msg, size);
      data->downSeq++;
      data->power = False;
      __inform(inst);
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      msg[0] = 3; // length
      msg[1] = 0; // address
      msg[2] = data->downSeq; // sequence number 1...255
      msg[3] = MSG_BOOST_ON; //data
      int size = __makeMessage(msg, 4);
      data->subWrite((obj)inst, msg, size);
      data->downSeq++;
      data->power = True;
      __inform(inst);
    }
    else if( StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Emergency break" );
      msg[0] = 3; // length
      msg[1] = 0; // address
      msg[2] = data->downSeq; // sequence number 1...255
      msg[3] = MSG_BOOST_OFF; //data
      int size = __makeMessage(msg, 4);
      data->subWrite((obj)inst, msg, size);
      data->downSeq++;
      data->power = False;
      __inform(inst);
    }
    else if( StrOp.equals( cmd, wSysCmd.sod ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Start of Day" );
      // MSG_BM_GET_RANGE
      msg[0] = 5; // length
      msg[1] = 0; // address
      msg[2] = data->downSeq; // sequence number 1...255
      msg[3] = MSG_BM_GET_RANGE; //data
      msg[4] = 0; // address range
      msg[5] = 16; // address range

      int size = __makeMessage(msg, 6);
      data->subWrite((obj)inst, msg, size);
      data->downSeq++;

    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int delay = wSwitch.getdelay(node) > 0 ? wSwitch.getdelay(node):data->swtime;
    int addr = wSwitch.getaddr1( node );
    if( addr == 0 ) {
      addr = wSwitch.getport1( node );
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "switch %d:%d",
        wSwitch.getbus( node ), wSwitch.getaddr1( node ) );

    msg[0] = 7; // length
    msg[1] = 0; // address
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = addr / 256;
    msg[4] = addr % 256;
    msg[5] = StrOp.equals(wSwitch.turnout, wSwitch.getcmd(node)) ? 1:0;
    msg[5] += 0x10;
    msg[6] = ((delay / 40) << 4);
    msg[7] = MSG_CS_ACCESSORY; //data
    int size = __makeMessage(msg, 8);
    data->subWrite((obj)inst, msg, size);
    data->downSeq++;
  }


  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    byte cmd[5];
    byte* frame = allocMem(32);
    Boolean on = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

    int addr = wOutput.getaddr( node );
    if( addr == 0 ) {
      addr = wOutput.getport( node );
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d:%d.%d %s",
        wOutput.getbus( node ), wOutput.getaddr( node ), wOutput.getgate(node), on?"ON":"OFF" );

    msg[0] = 7; // length
    msg[1] = 0; // address
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = addr / 256;
    msg[4] = addr % 256;
    msg[5] = wOutput.getgate(node);
    msg[5] += on ? 0x10:0x00;
    msg[6] = 0;
    msg[7] = MSG_CS_ACCESSORY; //data
    int size = __makeMessage(msg, 8);
    data->subWrite((obj)inst, msg, size);
    data->downSeq++;
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  steps = wLoc.getspcnt( node );
    int  speed = 0;
    Boolean fn  = wLoc.isfn( node );
    Boolean dir = wLoc.isdir( node ); /* True == forwards */

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * steps) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * steps) / wLoc.getV_max( node );
    }

    msg[ 0] = 12; // length
    msg[ 1] = 0; // address
    msg[ 2] = data->downSeq; // sequence number 1...255
    msg[ 3] = addr / 256;
    msg[ 4] = addr % 256;
    msg[ 5] = (dir ? 0x00:0x80) + (steps==128?0x30:0x20); // 128 speed steps
    msg[ 6] = 0x3F;
    msg[ 7] = speed;
    msg[ 8] = (fn?0x10:0x00);
    msg[ 9] = 0;
    msg[10] = 0;
    msg[11] = 0;
    msg[12] = MSG_CS_DRIVE; //data
    int size = __makeMessage(msg, 13);
    data->subWrite((obj)inst, msg, size);
    data->downSeq++;
  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    if( wFeedback.isactivelow(node) )
      wFeedback.setstate( node, !state);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );
  }


  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOBiDiBData data = Data(inst);
  iONode rsp = NULL;

  if( cmd != NULL ) {
    rsp = __translate((iOBiDiB)inst, cmd);
    cmd->base.del(cmd);
  }
  return rsp;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOBiDiBData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "halt BiDiB..." );
  data->run = False;
  ThreadOp.sleep(500);
  data->subDisconnect(inst);
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOBiDiBData data = Data(inst);
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




/** external shortcut event */
static void _shortcut( obj inst ) {
  return;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  return 0;
}


/**  */
static Boolean _supportPT( obj inst ) {
  return 0;
}


static int __getOffset4LocalAddr(iOBiDiB bidib, int local) {
  iOBiDiBData data = Data(bidib);
  char key[32];
  StrOp.fmtb( key, "%d", local );
  iONode node = (iONode)MapOp.get(data->localmap, key );
  if( node != NULL )
    return wBiDiBnode.getoffset(node);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no mapping found for local addr [%s]", key );
  return 1;
}


/*
 * Type:
 * Bit 15,14  Bits 13...0
 * 00  Lokadresse, Fahrtrichtung vorwärts
 * 10  Lokadresse, Fahrtrichtung rückwärts
 * 01  Accessory-Adresse
 * 11  Extended Accessory
 */
static void __handleSensor(iOBiDiB bidib, int localAddr, int port, Boolean state, int locoAddr, int type ) {
  iOBiDiBData data = Data(bidib);
  char ident[32];
  int addr = __getOffset4LocalAddr(bidib, localAddr) + port;

  /* Type:
    00  Lokadresse, Fahrtrichtung vorwärts
    10  Lokadresse, Fahrtrichtung rückwärts
    01  Accessory-Adresse
    11  Extended Accessory
  */

  char* sType = "loco-addr-fwd";
  if( type == 2 ) sType = "loco-addr-rev";
  if( type == 1 ) sType = "accessory-addr";
  if( type == 3 ) sType = "ext-accessory";
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "sensor-addr=%d state=%s ident=%d type=%s", addr, state?"occ":"free", locoAddr, sType );

  if( type == -1 || type == 0 || type == 2 ) {
    /* occ event */
    /* inform listener: Node3 */
    iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );

    wFeedback.setaddr( nodeC, addr );
    wFeedback.setfbtype( nodeC, wFeedback.fbtype_sensor );

    if( data->iid != NULL )
      wFeedback.setiid( nodeC, data->iid );

    wFeedback.setstate( nodeC, state );
    StrOp.fmtb(ident, "%d", locoAddr);
    wFeedback.setidentifier( nodeC, ident);
    if( type == 0 || type == 2 )
      wFeedback.setdirection( nodeC, type == 0 ? True:False );

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }
  else if( type == 1 || type == 3 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "accessory events not jet supported" );
  }
}


static void __handleMultipleSensors(iOBiDiB bidib, const byte* msg, int size) {
  iOBiDiBData data = Data(bidib);

  // 06 00 02 A2 00 08 01 8B
  int baseAddr = msg[4];
  int cnt = msg[5] / 8;
  int i = 0;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensor-base=%d cnt=%d", baseAddr, cnt );
  for( i = 0; i < cnt; i++ ) {
    int addr = baseAddr + (i / 2);
    int bit = 0;
    for( bit = 0; bit < 8; bit++ ) {
      __handleSensor(bidib, addr, bit+((i%2)*8), msg[6+i] & (0x01 << bit), 0, -1);
    }
  }

}


static void __handleCV(iOBiDiB bidib, int addr, int cv, int val) {
  iOBiDiBData data = Data(bidib);

  iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CV response" );

  wProgram.setvalue( node, val );
  wProgram.setcmd( node, wProgram.datarsp );
  wProgram.setcv( node, cv );
  wProgram.setdecaddr( node, addr );
  if( data->iid != NULL )
    wProgram.setiid( node, data->iid );

  if( data->listenerFun != NULL && data->listenerObj != NULL )
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

}

static void __seqAck(iOBiDiB bidib, byte* msg, int size) {
  iOBiDiBData data = Data(bidib);

  if( data->secAck && data->secAckInt > 0 ) {
    size--; // strip crc
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "seqAck for addr=%d seq=%d...", msg[1], msg[2] );
    TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)msg, size );
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = MSG_BM_MIRROR_MULTIPLE;
    size = __makeMessage(msg, size);
    TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)msg, size );
    data->subWrite((obj)bidib, msg, size);
    data->downSeq++;
  }
}


static void __handleError(iOBiDiB bidib, byte* msg, int size) {
  iOBiDiBData data = Data(bidib);

  switch( msg[4] ) {
  case BIDIB_ERR_TXT: // Txt
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "error message" );
    break;
  case BIDIB_ERR_CRC: // CRC
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "CRC error" );
    break;
  case BIDIB_ERR_SIZE: // Size
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Size error" );
    break;
  case BIDIB_ERR_SEQUENCE: // Sequence
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Sequence error" );
    break;
  case BIDIB_ERR_PARAMETER: // Parameter
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Parameter error" );
    break;
  case BIDIB_ERR_BUS: // Bus fault
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Bus fault: %d", msg[5] );
    break;
  case BIDIB_ERR_HW: // Hardware error
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Hardware error: %d", msg[5] );
    break;
  }
}


static const char* __getClass(int classid ) {
  return wBiDiBnode.class_sensor;
}

static void __addNode(iOBiDiB bidib, byte* msg) {
  iOBiDiBData data = Data(bidib);

  //                                 UID
  // locaddr class res vid productid   crc
  // 00      40    00  0D  65 00 01 00 E1
  char localKey[32];
  char uidKey[32];
  int uid = msg[4] + (msg[5] << 8) + (msg[6] << 16) + (msg[7] << 24);
  StrOp.fmtb( localKey, "%d", msg[0] );
  StrOp.fmtb( uidKey, "%d", uid );

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
      "local=%s class=0x%02X vid=%d uid=%s", localKey, msg[1], msg[3], uidKey);

  iONode node = (iONode)MapOp.get( data->nodemap, uidKey );
  if( node != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "mapping product ID [%s] with local [%s] to offset [%d]", uidKey, localKey, wBiDiBnode.getoffset(node) );
    MapOp.put( data->localmap, localKey, (obj)node);
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no mapping found for product ID [%s]; adding to list", uidKey );
    node = NodeOp.inst(wBiDiBnode.name(), data->bidibini, ELEMENT_NODE);
    wBiDiBnode.setuid(node, uid);
    wBiDiBnode.setclass(node, __getClass(msg[1]));
    wBiDiBnode.setvendor(node, msg[3]);
    NodeOp.addChild(data->bidibini, node);
  }
}


static void __handleNodeFeature(iOBiDiB bidib, byte* msg, int size) {
  iOBiDiBData data = Data(bidib);
  byte l_msg[32];

  int Addr = msg[1];
  int  Seq = msg[2];
  int Type = msg[3]; // MSG_SYS_MAGIC

  if( Type == MSG_FEATURE_COUNT ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_FEATURE_COUNT, addr=%d seq=%d features=%d", Addr, Seq, msg[4] );
    l_msg[0] = 3; // length
    l_msg[1] = 0; // address
    l_msg[2] = data->downSeq; // sequence number 1...255
    l_msg[3] = MSG_FEATURE_GETNEXT; //data
    int size = __makeMessage(l_msg, 4);
    data->subWrite((obj)bidib, l_msg, size);
    data->downSeq++;
  }
  else if( Type == MSG_FEATURE ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_FEATURE, addr=%d seq=%d feature=(%d) %s value=%d", Addr, Seq, msg[4], __getFeatureName(msg[4]), msg[5] );
    l_msg[0] = 3; // length
    l_msg[1] = 0; // address
    l_msg[2] = data->downSeq; // sequence number 1...255
    l_msg[3] = MSG_FEATURE_GETNEXT; //data
    int size = __makeMessage(l_msg, 4);
    data->subWrite((obj)bidib, l_msg, size);
    data->downSeq++;
  }

}


static void __handleNodeTab(iOBiDiB bidib, byte* msg, int size) {
  iOBiDiBData data = Data(bidib);
  //                                 UID
  //             ver len start locaddr class res vid productid   crc
  // 0E 00 04 89 01  01  00    00      40    00  0D  65 00 01 00 E1
  int Addr     = msg[1];
  int  Seq     = msg[2];
  int Type     = msg[3];
  data->tabver = msg[4];
  int entries  = msg[5];
  int entry    = 0;
  int offset   = 7;


  if( Type == MSG_NODETAB_COUNT ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_NODETAB_COUNT, addr=%d seq=%d count=%d", Addr, Seq, msg[4] );
    // request next
    if( msg[4] > 0 ) {
      byte l_msg[32];
      l_msg[0] = 3; // length
      l_msg[1] = 0; // address
      l_msg[2] = data->downSeq; // sequence number 1...255
      l_msg[3] = MSG_NODETAB_GETNEXT; //data
      int size = __makeMessage(l_msg, 4);
      data->subWrite((obj)bidib, l_msg, size);
      data->downSeq++;
    }
    return;
  }
  else if( Type == MSG_NODETAB ) {
    byte l_msg[32];

    /*
    00000000: 0C 00 07 89 01 00 40 00 0D 66 00 00 01 00
    20120519.082709.379 r9999c bidibrea OBiDiB   0779 MSG_NODETAB, addr=0 seq=7 tab-ver=1 tab-len=0

    MSG_NODETAB:
    Es folgt ein Eintrag der Zuordnungstabelle, dieser hat folgenden Aufbau:
      MSG_NODETAB_DATA ::= NODETAB_VERSION NODETAB_ENTRY
      NODE_TAB_VERSION ::= [ 0x01 .. 0xff ] (Wird bei jeder Änderung inkrementiert, Überlauf: 255→1)
      NODETAB_ENTRY ::= NODE_ADDR UNIQUE_ID

    NODE_ADDR Zugewiesene lokale Adresse des Melders (Wertebereich 0..127)
    UNIQUE_ID die eindeutige Hardwarekennung des Melders, diese besteht aus 7 Bytes
    */

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_NODETAB, addr=%d seq=%d tab-ver=%d tab-len=%d", Addr, Seq, data->tabver, entries );
    l_msg[0] = 3; // length
    l_msg[1] = 0; // address
    l_msg[2] = data->downSeq; // sequence number 1...255
    l_msg[3] = MSG_NODETAB_GETNEXT; //data
    int size = __makeMessage(l_msg, 4);
    data->subWrite((obj)bidib, l_msg, size);
    data->downSeq++;

    __addNode(bidib, msg+5 );
  }

    /*

    char localKey[32];
    char uidKey[32];
    int uid = msg[offset+4+entry*8] + (msg[offset+5+entry*8] << 8) + (msg[offset+6+entry*8] << 16) + (msg[offset+7+entry*8] << 24);
    StrOp.fmtb( localKey, "%d", msg[offset+0+entry*8] );
    StrOp.fmtb( uidKey, "%d", uid );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "entry=%d local=%s class=0x%02X vid=%d uid=%s", entry,
        localKey, msg[offset+1+entry*8], msg[offset+3+entry*8], uidKey);

    iONode node = (iONode)MapOp.get( data->nodemap, uidKey );
    if( node != NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "mapping product ID [%s] with local [%s] to offset [%d]", uidKey, localKey, wBiDiBnode.getoffset(node) );
      MapOp.put( data->localmap, localKey, (obj)node);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no mapping found for product ID [%s]", uidKey );
    }
    */
}


static void __handleNewNode(iOBiDiB bidib, byte* msg, int size) {
  iOBiDiBData data = Data(bidib);
  int Addr     = msg[1];
  int  Seq     = msg[2];
  data->tabver = msg[4];
  __addNode(bidib, msg+5);
}


static void __handleLostNode(iOBiDiB bidib, byte* msg, int size) {
  iOBiDiBData data = Data(bidib);
  int Addr      = msg[1];
  int Seq       = msg[2];
  int localAddr = msg[4];
  TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
      "MSG_NODE_LOST, addr=%d seq=%d local-addr=%d TODO: POWER OFF", Addr, Seq, localAddr );
}


/**
 * len addr seq type data  crc
 * 05  00   00  81   FE AF 89
 */
static Boolean __processBidiMsg(iOBiDiB bidib, byte* msg, int size) {
  iOBiDiBData data = Data(bidib);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "processing bidib message..." );

  int Addr = msg[1];
  int  Seq = msg[2];
  int Type = msg[3]; // MSG_SYS_MAGIC

  switch( Type ) {
  case MSG_SYS_MAGIC:
  { // len = 5
    int Magic = (msg[5]<<8)+msg[4];
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_SYS_MAGIC, addr=%d seq=%d magic=0x%04X", Addr, Seq, Magic );
    data->upSeq   = msg[2];
    data->magicOK = True;
    // query MSG_SYS_GET_P_VERSION
    msg[0] = 3; // length
    msg[1] = 0; // address
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = MSG_SYS_GET_SW_VERSION; //data

    size = __makeMessage(msg, 4);
    data->subWrite((obj)bidib, msg, size);
    data->downSeq++;
    break;
  }

  case MSG_SYS_SW_VERSION:
  { // len = 6
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_SYS_SW_VERSION, addr=%d seq=%d version=%d.%d.%d", Addr, Seq, msg[4], msg[5], msg[6] );
    // query MSG_SYS_ENABLE
    msg[0] = 3; // length
    msg[1] = 0; // address
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = MSG_SYS_ENABLE; //data

    size = __makeMessage(msg, 4);
    data->subWrite((obj)bidib, msg, size);
    data->downSeq++;

    if( data->secAck && data->secAckInt > 0 ) {
      // MSG_FEATURE_SET
      msg[0] = 5; // length
      msg[1] = 0; // address
      msg[2] = data->downSeq; // sequence number 1...255
      msg[3] = MSG_FEATURE_SET; //data
      msg[4] = 2;
      msg[5] = 1;
      size = __makeMessage(msg, 6);
      data->subWrite((obj)bidib, msg, size);
      data->downSeq++;

      msg[0] = 5; // length
      msg[1] = 0; // address
      msg[2] = data->downSeq; // sequence number 1...255
      msg[3] = MSG_FEATURE_SET; //data
      msg[4] = 3;
      msg[5] = data->secAckInt;
      size = __makeMessage(msg, 6);
      data->subWrite((obj)bidib, msg, size);
      data->downSeq++;
    }
    else {
      msg[0] = 5; // length
      msg[1] = 0; // address
      msg[2] = data->downSeq; // sequence number 1...255
      msg[3] = MSG_FEATURE_SET; //data
      msg[4] = 3;
      msg[5] = 0;
      size = __makeMessage(msg, 6);
      data->subWrite((obj)bidib, msg, size);
      data->downSeq++;
    }

    // MSG_NODETAB_GETALL
    msg[0] = 3; // length
    msg[1] = 0; // address
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = MSG_NODETAB_GETALL; //data

    size = __makeMessage(msg, 4);
    data->subWrite((obj)bidib, msg, size);
    data->downSeq++;


    // MSG_FEATURE_GETALL
    msg[0] = 3; // length
    msg[1] = 0; // address
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = MSG_FEATURE_GETALL; //data

    size = __makeMessage(msg, 4);
    data->subWrite((obj)bidib, msg, size);
    data->downSeq++;

    // MSG_BM_GET_RANGE
    msg[0] = 5; // length
    msg[1] = 0; // address
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = MSG_BM_GET_RANGE; //data
    msg[4] = 0; // address range
    msg[5] = 16; // address range

    size = __makeMessage(msg, 6);
    data->subWrite((obj)bidib, msg, size);
    data->downSeq++;

    break;
  }

  case MSG_NODETAB_COUNT:
  case MSG_NODETAB:
  {
    __handleNodeTab(bidib, msg, size);
    break;
  }

  case MSG_NODE_NEW:
  {
    __handleNewNode(bidib, msg, size);
    __seqAck(bidib, msg, size);
    break;
  }

  case MSG_NODE_LOST:
  {
    __handleLostNode(bidib, msg, size);
    __seqAck(bidib, msg, size);
    break;
  }

  /*
   * 04 00 02 A0 00 BE MSG_BM_OCC
   * 04 00 03 A0 10 88
   * 04 00 04 A1 00 AB MSG_BM_FREE
   * 04 00 05 A0 10 59
   */
  case MSG_BM_OCC:
  { // len = 4
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_BM_OCC, addr=%d seq=%d local-addr=%d", Addr, Seq, msg[4] );
    __handleSensor(bidib, Addr, msg[4], True, 0, -1);
    __seqAck(bidib, msg, size);
    break;
  }

  case MSG_BM_FREE:
  { // len = 4
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_BM_FREE, addr=%d seq=%d local-addr=%d", Addr, Seq, msg[4] );
    __handleSensor(bidib, Addr, msg[4], False, 0, -1);
    __seqAck(bidib, msg, size);
    break;
  }

  case MSG_BM_MULTIPLE:
  { // 06 00 02 A2 00 08 01 8B
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_BM_MULTIPLE, addr=%d seq=%d local-addr=%d nr-occ=%d, occ=0x%02X", Addr, Seq, msg[4], msg[5], msg[6] );
    __handleMultipleSensors(bidib, msg, size);
    __seqAck(bidib, msg, size);
    break;
  }

  case MSG_FEATURE_COUNT:
  case MSG_FEATURE:
  case MSG_FEATURE_NA:
  {
    __handleNodeFeature(bidib, msg, size);
    break;
  }

  case MSG_BM_ADDRESS:
  { //             MNUM, ADDRL, ADDRH
    // 06 00 0C A3 04    5E     13 C4
    int locoAddr = (msg[6]&0x3F) * 256 + msg[5];
    int type = msg[6] >> 6;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_BM_ADDRESS, addr=%d seq=%d local-addr=%d loco-addr=%d type=%d", Addr, Seq, msg[4], locoAddr, type );
    __handleSensor(bidib, Addr, msg[4], locoAddr > 0, locoAddr, type );
    break;
  }

  case MSG_BM_CV:
  { //             ADDRL, ADDRH, CVL, CVH, DAT
    // 08 00 0D A5 5E     13     06   00   02 38
    int locoAddr = (msg[5]&0x3F) * 256 + msg[4];
    int cv       = msg[7] * 256 + msg[6];
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_BM_CV, addr=%d seq=%d loco-addr=%d cv=%d val=%d", Addr, Seq, locoAddr, cv, msg[8] );
    __handleCV(bidib, locoAddr, cv, msg[8]);
    break;
  }


  case MSG_BM_SPEED:
  { //             ADDRL, ADDRH, DAT
    // 08 00 0D A6 5E     13     02
    int locoAddr = (msg[5]&0x3F) * 256 + msg[4];
    int speed    = msg[6];
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "MSG_BM_SPEED, addr=%d seq=%d loco-addr=%d dcc-speed=%d", Addr, Seq, locoAddr, speed );
    break;
  }


  case MSG_BM_CURRENT:
  { //             MNUM, DAT
    // 08 00 0D A7 00    00
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_BM_CURRENT, addr=%d seq=%d current=%d", Addr, Seq, msg[5] );
    break;
  }

  case MSG_SYS_ERROR:
  { // MSG_SYS_ERROR
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
        "MSG_SYS_ERROR, addr=%d seq=%d error=%d", Addr, Seq, msg[4] );
    __handleError(bidib, msg, size);
    break;
  }


  case MSG_NODE_NA:
  {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_NODE_NA, addr=%d seq=%d na-node=%d", Addr, Seq, msg[4] );

    // MSG_BM_GET_RANGE
    msg[0] = 5; // length
    msg[1] = 0; // address
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = MSG_BM_GET_RANGE; //data
    msg[4] = 0; // address range
    msg[5] = 16; // address range

    size = __makeMessage(msg, 6);
    data->subWrite((obj)bidib, msg, size);
    data->downSeq++;

    break;
  }

  case MSG_SYS_P_VERSION:
  {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_SYS_P_VERSION %d.%d", msg[2], msg[1] );
    break;
  }

  default:
  {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "UNSUPPORTED: msg=0x%02X, addr=%d seq=%d", Type, Addr, Seq );
    break;
  }

  }

  return data->magicOK;
}


static void __bidibReader( void* threadinst ) {
  iOThread    th    = (iOThread)threadinst;
  iOBiDiB     bidib = (iOBiDiB)ThreadOp.getParm( th );
  iOBiDiBData data  = Data(bidib);
  byte msg[256];
  int size = 0;
  int addr = 0;
  int value = 0;
  int port = 0;
  int magicreq = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "BiDiB reader started." );

  ThreadOp.sleep(100); /* resume some time to get it all being setup */

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "magic request #%d", magicreq );
  magicreq++;
  data->lastMagicReq = SystemOp.getTick();
  msg[0] = 3; // length
  msg[1] = 0; // address
  msg[2] = data->downSeq; // sequence number 1...255
  msg[3] = MSG_SYS_GET_MAGIC; //data

  size = __makeMessage(msg, 4);
  data->subWrite((obj)bidib, msg, size);
  data->downSeq++;

  ThreadOp.sleep(100);

  while( data->run ) {

    if( !data->magicOK && SystemOp.getTick() - data->lastMagicReq > 100 ) {
      /* no magic received; request again */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "magic request #%d", magicreq );
      magicreq++;
      data->lastMagicReq = SystemOp.getTick();
      msg[0] = 3; // length
      msg[1] = 0; // address
      msg[2] = data->downSeq; // sequence number 1...255
      msg[3] = MSG_SYS_GET_MAGIC; //data

      size = __makeMessage(msg, 4);
      data->subWrite((obj)bidib, msg, size);
      data->downSeq++;
    }

    if( !data->subAvailable( (obj)bidib) ) {
      ThreadOp.sleep( 10 );
      continue;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "bidib message available" );
    }

    size = data->subRead( (obj)bidib, msg );
    if( size > 0 ) {
      TraceOp.dump ( name, TRCLEVEL_DEBUG, (char*)msg, size );

      size = __deEscapeMessage(msg, size);
      TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)msg, size );
      byte crc = __checkSum(msg, size );
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "crc=0x%02X", crc );

      if( crc == 0 ) {
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "valid message received; processing" );
        __processBidiMsg(bidib, msg, size);
      }

    }

  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "BiDiB reader ended." );
}


static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;
static int _version( obj inst ) {
  iOBiDiBData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __initNodeMap(iOBiDiB bidib) {
  iOBiDiBData data = Data(bidib);
  iONode node = wBiDiB.getbidibnode(data->bidibini);

  while( node != NULL ) {
    char uid[256];
    StrOp.fmtb( uid, "%d", wBiDiBnode.getuid(node) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "mapping node %s, offset=%d", uid, wBiDiBnode.getoffset(node) );
    MapOp.put( data->nodemap, uid, (obj)node );
    node = wBiDiB.nextbidibnode(data->bidibini, node);
  }

}


/**  */
static struct OBiDiB* _inst( const iONode ini ,const iOTrace trc ) {
  iOBiDiB __BiDiB = allocMem( sizeof( struct OBiDiB ) );
  iOBiDiBData data = allocMem( sizeof( struct OBiDiBData ) );
  MemOp.basecpy( __BiDiB, &BiDiBOp, 0, sizeof( struct OBiDiB ), data );

  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */

  data->ini      = ini;
  data->bidibini = wDigInt.getbidib(data->ini);
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  data->run      = True;

  data->commOK  = False;
  data->magicOK = False;

  data->mux      = MutexOp.inst( NULL, True );
  data->nodemap  = MapOp.inst();
  data->localmap = MapOp.inst();
  data->swtime   = wDigInt.getswtime( ini );

  if( data->bidibini == NULL ) {
    data->bidibini = NodeOp.inst( wBiDiB.name(), data->ini, ELEMENT_NODE);
    NodeOp.addChild( data->ini, data->bidibini);
  }

  data->secAck    = wBiDiB.issecAck( data->bidibini );
  data->secAckInt = wBiDiB.getsecAckInt( data->bidibini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "BiDiB %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "http://www.bidib.org/" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib  = %s", wDigInt.getsublib( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "secAck  = %s, interval=%dms",
      wBiDiB.issecAck( data->bidibini ) ? "enabled":"disabled", wBiDiB.getsecAckInt(data->bidibini) * 10 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  __initNodeMap(__BiDiB);

  /* choose interface: */
  if( StrOp.equals( wDigInt.sublib_default, wDigInt.getsublib( ini ) ) ) {
    /* serial */
    data->subConnect    = serialConnect;
    data->subDisconnect = serialDisconnect;
    data->subRead       = serialRead;
    data->subWrite      = serialWrite;
    data->subAvailable  = serialAvailable;
  }
  else if( StrOp.equals( wDigInt.sublib_serial, wDigInt.getsublib( ini ) ) ) {
    /* serial */
    data->subConnect    = serialConnect;
    data->subDisconnect = serialDisconnect;
    data->subRead       = serialRead;
    data->subWrite      = serialWrite;
    data->subAvailable  = serialAvailable;
  }


  data->commOK = data->subConnect((obj)__BiDiB);

  if( data->commOK ) {
    data->reader = ThreadOp.inst( "bidibreader", &__bidibReader, __BiDiB );
    ThreadOp.start( data->reader );
  }

  instCnt++;
  return __BiDiB;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/bidib.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
