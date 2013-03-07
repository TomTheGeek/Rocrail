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

#include "rocs/public/str.h"
#include "rocrail/wrapper/public/BiDiBnode.h"
#include "rocdigs/impl/bidib/bidib_messages.h"

int bidibDeEscapeMessage(byte* msg, int inLen) {
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
  TraceOp.trc( "bidib", TRCLEVEL_DEBUG, __LINE__, 9999, "message de-escaped" );
  TraceOp.dump ( "bidib", TRCLEVEL_DEBUG, (char*)msg, outLen );
  return outLen;
}
/* Update 8-bit CRC value
   using polynomial X^8 + X^5 + X^4 + 1 */
#define POLYVAL 0x8C
void bidibUpdateCRC(byte newb, byte* crc)
{
  int i;
  byte c = *crc;
  for (i = 0; i < 8; i++) {
    if ((c ^ newb) & 1)
      c = (c >> 1 ) ^ POLYVAL;
    else
      c >>= 1;
    newb >>= 1;
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
byte bidibCheckSum(byte* packet, int len) {
  byte checksum = 0x00;
  int i = 0;
  for( i = 0; i < len; i++ ) {
    bidibUpdateCRC(packet[i], &checksum);
  }

  return checksum;
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
void bidibEscapeMessage(byte* msg, int* newLen, int inLen) {
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
  TraceOp.dump ( "bidib", TRCLEVEL_DEBUG, (char*)msg, outLen );
}



int bidibMakeMessage(byte* msg, int inLen) {
  int outLen = 0;
  byte buffer[256];
  buffer[outLen] = BIDIB_PKT_MAGIC;
  outLen++;
  MemOp.copy( buffer + 1, msg, inLen );
  outLen += inLen;
  buffer[outLen] = bidibCheckSum(buffer+1, outLen-1 );
  outLen++;
  bidibEscapeMessage(buffer+1, &outLen, outLen-1);
  outLen++;
  buffer[outLen] = BIDIB_PKT_MAGIC;
  outLen++;
  MemOp.copy(msg, buffer, outLen);
  return outLen;
}



char* bidibGetClassName(int classid, char* mnemonic ) {
  char* classname = NULL;
  int idx = 0;
  mnemonic[idx] = '\0';
  if( classid & 0x80 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_bridge);
    mnemonic[idx] = 'L';
    idx++;
    mnemonic[idx] = '\0';
  }
  if( classid & 0x40 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_sensor);
    mnemonic[idx] = 'O';
    idx++;
    mnemonic[idx] = '\0';
  }
  if( classid & 0x20 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_ui);
    mnemonic[idx] = 'U';
    idx++;
    mnemonic[idx] = '\0';
  }
  if( classid & 0x10 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_dcc_main);
    mnemonic[idx] = 'T';
    idx++;
    mnemonic[idx] = '\0';
  }
  if( classid & 0x08 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_dcc_prog);
    mnemonic[idx] = 'A';
    idx++;
    mnemonic[idx] = '\0';
  }
  if( classid & 0x04 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_accessory);
    mnemonic[idx] = 'P';
    idx++;
    mnemonic[idx] = '\0';
  }
  if( classid & 0x02 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_booster);
    mnemonic[idx] = 'B';
    idx++;
    mnemonic[idx] = '\0';
  }
  if( classid & 0x01 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_switch);
    mnemonic[idx] = 'S';
    idx++;
    mnemonic[idx] = '\0';
  }

  if( classname == NULL )
    classname = StrOp.dup("");
  return classname;
}

const char* bidibGetFeatureName(int feature) {

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
  if( feature == FEATURE_BM_ISTSPEED_INTERVAL ) return "dcc-speed enabled";
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
  if( feature == FEATURE_BST_CURMEAS_INTERVAL ) return "current measurement interval";
  if( feature == FEATURE_BST_CV_AVAILABLE ) return "read CV available";
  if( feature == FEATURE_BST_CV_ON ) return "read CV on";
  if( feature == FEATURE_BST_INHIBIT_AUTOSTART ) return "inhibit auto start";
  if( feature == FEATURE_BST_INHIBIT_LOCAL_ONOFF ) return "inhibit local on/off";


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
  if( feature == FEATURE_GEN_DRIVE_BUS ) return "DCC bus driver";
  if( feature == FEATURE_GEN_LOK_LOST_DETECT ) return "Loco lost detection";
  if( feature == FEATURE_GEN_NOTIFY_DRIVE_MANUAL ) return "Manual drive notification";

  if( feature == FEATURE_FW_UPDATE_MODE ) return "firmware update mode";
  if( feature == FEATURE_EXTENSION ) return "reserved for future expansion";

  /* accessory */
  if( feature == FEATURE_ACCESSORY_COUNT ) return "number of objects";
  if( feature == FEATURE_ACCESSORY_SURVEILLED ) return "annouce if operated outside bidib";
  if( feature == FEATURE_ACCESSORY_MACROMAPPED ) return "accessory aspects are mapped to macros";

  return "*** unknown feature ***";
}
