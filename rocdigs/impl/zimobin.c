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


/*
Example of a reset message:

[22/01/2011 16:32:05] - Written data
soh soh  seq dle pc^0x20 msg dle crc8^0x20 eot
01  01   21  10  30      00  10  21        17

seq  = 0x21
info = 0x10 -> message sent by pc
msg  = 0x00 -> reset
crc8 = 0x01


Reply:

[22/01/2011 16:32:10] - Read data
soh soh seq ack pri seq crc8 eot
01  01  80  40  00  21  7e   17
*/

/* data->protver 
        0 acessory decoder invers       no error handling
        1 acessory decoder normal       no error handling
        2 acessory decoder normal       on error 1 resend of packet
        3 acessory decoder normal       on error resend while no ack for msg received
                this let you start rocrail and then power up yout layout

  data->fbmod
        number of MX9 * 2 
  data->fboffset
        offset for MX9 sensor address
        +1000 shortcut sensor address for this MX9 section
        +2000 Loco address railcom  sensor address for this MX9 section (UNTESTED)
*/


#include "rocdigs/impl/zimobin_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/LocList.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/SwitchList.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/BinCmd.h"
#include "rocrail/wrapper/public/Clock.h"

#include "rocutils/public/addr.h"


static int instCnt = 0;

static void __initComm( iOZimoBin zimobin );

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOZimoBinData data = Data(inst);
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
  iOZimoBinData data = Data(inst);
  return NULL;
}

static const char* __id( void* inst ) {
  iOZimoBinData data = Data(inst);
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  iOZimoBinData data = Data(inst);
  return NULL;
}

/** ----- OZimoBin ----- */




/* Update 8-bit CRC value

CRC-8-Dallas/Maxim
x8 + x5 + x4 + 1 (1-Wire bus)

Representations: normal / **reversed** / reverse of reciprocal
0x31 / 0x8C / 0x98

Initialized with 0xFF

 */

const byte crc8_table[256] = {
        0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83,
        0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
        0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e,
        0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
        0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0,
        0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
        0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d,
        0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
        0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5,
        0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
        0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58,
        0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
        0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6,
        0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
        0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b,
        0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
        0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f,
        0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
        0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92,
        0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
        0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c,
        0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
        0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1,
        0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
        0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49,
        0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
        0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4,
        0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
        0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a,
        0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
        0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7,
        0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35
};

static byte __checkSum8(byte* packet, int len) {
  byte checksum = 0xff;
  int i = 0;
  for( i = 0; i < len; i++ ) {
    checksum = crc8_table[checksum ^ packet[i]];
  }

  return checksum;
}

/* Update 16-bit CRC value
 * CRC parameters used:
 *   bits:       16
 *   poly:       0x1021
 *   init:       0xffff
 *   xor:        0x0000
 *   reverse:    false
 *   non-direct: false
 *
 * CRC of the string "123456789" is 0x29b1
 */


const unsigned short crc16_table[256] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

static unsigned short __checkSum16(byte* packet, int len) {
  unsigned short checksum = 0xffff;
  int i = 0;
  for( i = 0; i < len; i++ ) {
    checksum = (checksum << 8) ^ crc16_table[((checksum >> 8) & 0xff) ^ packet[i]];
  }

  return checksum;
}



/* Structure for post/get
 *
 * byte type 1 short tele (8bit crc), 0 long tele (16bit crc)
 * byte packetlen
 * byte data
 * byte data
 * .....
 *
 */



/*
 * Escape control bytes:
 * SOH -> DLE, SOH^0x20
 * EOT -> DLE, EOT^0x20
 * DLE -> DLE, DLE^0x20
 */
#define SOH 0x01
#define EOT 0x17
#define DLE 0x10

/* Zimo Error Code */
#define NO_ERROR        0x00
#define ERR_ADRESSE     0x01
#define ERR_INDEX       0x02
#define ERR_FORWARD     0x03
#define ERR_BUSY        0x04
#define ERR_NO_MOT      0x05
#define ERR_NO_DCC      0x06
#define ERR_CV_ADRESSE  0x07
#define ERR_SECTION     0x08
#define ERR_NO_MODUL    0x09
#define ERR_MESSAGE     0x0a
#define ERR_SPEED       0x0b

#define TOOLID  0

static const char* __getErrCode( int errcode ) {
  static char stext[32];

  switch(errcode) {
    case NO_ERROR:
      sprintf(stext, "OK");
      break;
    case ERR_ADRESSE:
      sprintf(stext, "Address");
      break;
    case ERR_INDEX:
      sprintf(stext, "Index");
      break;
    case ERR_FORWARD:
      sprintf(stext, "Forward");
      break;
    case ERR_BUSY:
      sprintf(stext, "Busy");
      break;
    case ERR_NO_MOT:
      sprintf(stext, "no Mot");
      break;
    case ERR_NO_DCC:
      sprintf(stext, "no DCC");
      break;
    case ERR_CV_ADRESSE:
      sprintf(stext, "CV Adress");
      break;
    case ERR_SECTION:
      sprintf(stext, "Section");
      break;
    case ERR_NO_MODUL:
      sprintf(stext, "no Modul");
      break;
    case ERR_MESSAGE:
      sprintf(stext, "Message");
      break;
    case ERR_SPEED:
      sprintf(stext, "Speed");
      break;
    default:
      sprintf(stext,"Fatal Error: %d", errcode);
      break;
  }
  return stext;
}

static int __escapePacket(byte* packet, int inlen) {
  byte buf[64];
  int len = inlen;
  int i = 0;
  int idx = 0;

  for( i = 0; i < inlen; i++ ) {
    if(  packet[i] == SOH || packet[i] == EOT || packet[i] == DLE ) {
      buf[idx] = DLE;
      idx++;
      buf[idx] = packet[i] ^ 0x20;
      idx++;
    } else {
      buf[idx] = packet[i];
      idx++;
    }
  }

  len = idx;
  MemOp.copy( packet, buf, len );

  return len;

}


static int __unescapePacket(byte* packet, int inlen) {
  byte buf[64];
  int len = inlen;
  int i = 0;
  int idx = 0;

  for( i = 0; i < inlen; i++ ) {
    if(  packet[i] == DLE ) {
      i++;
      buf[idx] = packet[i] ^ 0x20;
      idx++;
    }
    else {
      buf[idx] = packet[i];
      idx++;
    }
  }

  len = idx;
  MemOp.copy( packet, buf, len );

  return len;

}


static int __controlPacket(byte* packet, int inlen) {
  byte buf[256];
  int len = inlen + 3;

  buf[0] = SOH;
  buf[1] = SOH;
  MemOp.copy(buf+2, packet, inlen);
  buf[inlen+2] = EOT;

  MemOp.copy(packet, buf, len);
  return len;

}


static int __uncontrolPacket(byte* packet, int inlen) {
  byte buf[256];
  int len = inlen - 3;

  MemOp.copy(buf, packet+2, len);
  MemOp.copy(packet, buf, len);

  return len;
}

static void __send_ack(iOZimoBin zimobin, byte seqid, byte msgt) {
  iOZimoBinData data = Data(zimobin);

  byte* outa = allocMem(32);
  outa[0] = 1;      /* short packet */
  outa[1] = 3;      /* packet length */
  outa[2] = 0x50;   /* short ack */
  outa[3] = msgt;       /* message type */
  outa[4] = seqid;      /* seqid */

  ThreadOp.prioPost( data->transactor, (obj)outa, high );

}

static void __send_lack(iOZimoBin zimobin, byte seqid, byte msgt) {
  iOZimoBinData data = Data(zimobin);

  byte* outa = allocMem(32);
  outa[0] = 0;      /* long packet */
  outa[1] = 4;      /* packet length */
  outa[2] = 0xd0;   /* long ack */
  outa[3] = msgt;       /* msgt */
  outa[4] = 5;          /* header length */
  outa[5] = seqid;      /* seqid */

  ThreadOp.prioPost( data->transactor, (obj)outa, high );

}

static void __send_nak(iOZimoBin zimobin) {
  iOZimoBinData data = Data(zimobin);

  byte* outa = allocMem(32);
  outa[0] = 1;      /* short packet */
  outa[1] = 2;      /* packet length */
  outa[2] = 0x10;   /* command station instruction */
  outa[3] = 1;          /* nak */

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "send nak");

  ThreadOp.prioPost( data->transactor, (obj)outa, high );

}

static int __dccSteps(int steps) {
  if( steps < 28 )
    return 0x04;
  if( steps > 28 )
    return 0x0C;
  return 0x08;
}

#define PTIME  500

static iONode __translate( iOZimoBin zimobin, iONode node ) {
  iOZimoBinData data = Data(zimobin);
  iONode rsp = NULL;

  /* Clock command. */
  if( StrOp.equals( NodeOp.getName( node ), wClock.name() ) ) {
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {

    int addr  = wSwitch.getaddr1( node );
    int port  = wSwitch.getport1( node );
    int gate  = wSwitch.getgate1( node );
    int delay = wSwitch.getdelay( node );

    byte addrFormat = 0x00;
    Boolean lock = False;
    int state;

    if ( !wSwitch.isactdelay( node ) ) {
      delay = 0;
    }

    if( data->protver > 0 ) {
      state = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 0:1;
    } else {
      state = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 1:0;
    }

    if( StrOp.equals( wSwitch.prot_M, wSwitch.getprot( node ) ) ) /* Motorola */
      addrFormat = 0x40;
    if( StrOp.equals( wSwitch.prot_N, wSwitch.getprot( node ) ) ) /* DCC */
      addrFormat = 0x80;

    if( port == 0 ) {
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    }
    else if( addr == 0 && port > 0 ) {
      AddrOp.fromPADA( port, &addr, &port );
    }

    if( port > 0 )
      port--;

    {
      byte* outa = allocMem(32);

      outa[0] = 1;    /* short packet */
      outa[1] = 6;    /* packet length */
      outa[2] = 0x10; /* command station instruction */
      outa[3] = 10;   /* address control */
      outa[4] = (addr / 256) & 0x3f | addrFormat;
      outa[5] = addr % 256;
      outa[6] = 0x80 | 0x20 | (lock ? 0x02 : 0x00) | 0x01;
      outa[7] = 0x03 << (port * 2);

      ThreadOp.post( data->transactor, (obj)outa );
    }

    {
      byte* outa = allocMem(32);

      outa[0] = 1;    /* short packet */
      outa[1] = 5;    /* packet length */
      outa[2] = 0x10; /* command station instruction */
      outa[3] = 7;    /* accessory command */
      outa[4] = (addr / 256) & 0x3f | addrFormat;
      outa[5] = addr % 256;
      outa[6] = ((port * 2) + state) & 0x07 | 0x08;  /* gate 0 is red, gate 1 is green */

      ThreadOp.post( data->transactor, (obj)outa );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch %d:%d:%d %s, 0x%02X", addr, port, gate, state?"green":"red", outa[6]);
    }

    if (False) {                /* switch off gate after swtime */
      byte* outa = allocMem(32);

      outa[0] = 1;    /* short packet */
      outa[1] = 5;    /* packet length */
      outa[2] = 0x10; /* command station instruction */
      outa[3] = 7;    /* accessory command */
      outa[4] = (addr / 256) & 0x3f | addrFormat;
      outa[5] = addr % 256;
      outa[6] = ((port * 2) + state) & 0x07;  /* gate 0 is red, gate 1 is green */

      if ( !delay ) {
        delay = data->swtime;
      }

      outa[7] = delay / 256;
      outa[8] = delay % 256;

      MutexOp.wait( data->tmux );
      QueueOp.post( data->tqueue, (obj)outa, normal );
      MutexOp.post( data->tmux );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch %d:%d:%d %d 0x%02X", addr, port, gate, outa[7]*256 + outa[8], outa[6] );
    }
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {

    int addr   = wOutput.getaddr( node );
    int port   = wOutput.getport( node );
    int gate   = wOutput.getgate( node );
    byte addrFormat = 0x00;
    int action;

    if( data->protver > 0 ) {
      action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x08:0x00;
    } else {
      action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x00:0x08;
    }

    if( StrOp.equals( wOutput.prot_M, wOutput.getprot( node ) ) ) /* Motorola */
      addrFormat = 0x40;
    if( StrOp.equals( wOutput.prot_N, wOutput.getprot( node ) ) ) /* DCC */
      addrFormat = 0x80;

    if( port == 0 )
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      AddrOp.fromPADA( port, &addr, &port );

    if( port > 0 )
      port--;

    byte* outa = allocMem(32);
    outa[0] = 1;    /* short packet */
    outa[1] = 5;    /* packet length */
    outa[2] = 0x10; /* command station instruction */
    outa[3] = 7;    /* accessory command */
    outa[4] = (addr / 256) & 0x3f | addrFormat;
    outa[5] = addr % 256;
    outa[6] = ((port * 2) + gate) & 0x07 | action;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "output %d:%d:%d %s, 0x%02X", addr, port, gate, action?"ON":"OFF", outa[6]);
    ThreadOp.post( data->transactor, (obj)outa );

  }


  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );
  }

  /* Loc release command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) && StrOp.equals( wLoc.release, wLoc.getcmd(node) ) ) {

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco: addr=%d released", wLoc.getaddr( node ));

  }

  /* Loc and Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) || StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int addr = wLoc.getaddr( node );
    int V = 0;
    int steps = wLoc.getspcnt( node );
    byte addrFormat = 0x00;

    if( StrOp.equals( wLoc.prot_M, wLoc.getprot( node ) ) ) /* Motorola */
      addrFormat = 0x40;
    if( StrOp.equals( wLoc.prot_N, wLoc.getprot( node ) ) ) /* DCC */
      addrFormat = 0x80;
    if( StrOp.equals( wLoc.prot_L, wLoc.getprot( node ) ) ) /* DCC Long */
      addrFormat = 0x80;

    if( steps > 126 ) 
      steps = 126;

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        V = (wLoc.getV( node ) * steps) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        V = (wLoc.getV( node ) * steps) / wLoc.getV_max( node );
    }
    if( V < 1 && wLoc.getV( node ) > 0)
      V=1;

    if( V > 126 )
      V=126;      /* 127 not used, 128 ist emergency stop for this loco */
                  /* deocder has 0 = 0, 1 = emg stop, 2-127 drive       */


    Boolean f0 = wFunCmd.isf0( node );
    Boolean f1 = wFunCmd.isf1( node );
    Boolean f2 = wFunCmd.isf2( node );
    Boolean f3 = wFunCmd.isf3( node );
    Boolean f4 = wFunCmd.isf4( node );
    Boolean f5 = wFunCmd.isf5( node );
    Boolean f6 = wFunCmd.isf6( node );
    Boolean f7 = wFunCmd.isf7( node );
    Boolean f8 = wFunCmd.isf8( node );
    Boolean f9  = wFunCmd.isf9 ( node );
    Boolean f10 = wFunCmd.isf10( node );
    Boolean f11 = wFunCmd.isf11( node );
    Boolean f12 = wFunCmd.isf12( node );
    Boolean f13 = wFunCmd.isf13( node );
    Boolean f14 = wFunCmd.isf14( node );
    Boolean f15 = wFunCmd.isf15( node );
    Boolean f16 = wFunCmd.isf16( node );
    Boolean f17 = wFunCmd.isf17( node );
    Boolean f18 = wFunCmd.isf18( node );
    Boolean f19 = wFunCmd.isf19( node );
    Boolean f20 = wFunCmd.isf20( node );
    Boolean f21 = wFunCmd.isf21( node );
    Boolean f22 = wFunCmd.isf22( node );
    Boolean f23 = wFunCmd.isf23( node );
    Boolean f24 = wFunCmd.isf24( node );
    Boolean f25 = wFunCmd.isf25( node );
    Boolean f26 = wFunCmd.isf26( node );
    Boolean f27 = wFunCmd.isf27( node );
    Boolean f28 = wFunCmd.isf28( node );



    byte functions1 = (f1 ?0x01:0) + (f2 ?0x02:0) + (f3 ?0x04:0) + (f4 ?0x08:0) +
                      (f5 ?0x10:0) + (f6 ?0x20:0) + (f7 ?0x40:0) + (f8 ?0x80:0);
    byte functions2 = (f9 ?0x01:0) + (f10?0x02:0) + (f11?0x04:0) + (f12?0x08:0) +
                      (f13?0x10:0) + (f14?0x20:0) + (f15?0x40:0) + (f16?0x80:0);
    byte functions3 = (f17?0x01:0) + (f18?0x02:0) + (f19?0x04:0) + (f20?0x08:0) +
                      (f21?0x10:0) + (f22?0x20:0) + (f23?0x40:0) + (f24?0x80:0);
    byte functions4 = (f25?0x01:0) + (f26?0x02:0) + (f27?0x04:0) + (f28?0x08:0);


    byte* outa = allocMem(32);
    outa[0] = 1;    /* short packet */
    outa[1] = 8;    /* packet length */
    outa[2] = 0x10; /* command station instruction */
    outa[3] = 3;    /* loco control */
    outa[4] = addr / 256 | addrFormat;
    outa[5] = addr % 256;
    outa[6] = V;
    outa[7] = (wLoc.isdir( node )?0x00:0x20) | (wLoc.isfn( node )?0x10:0x00) | __dccSteps(steps);
    outa[8] = functions1;
    outa[9] = functions2;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco: V=%d, dir=%s, lights=%s nf=%s",
        V, wLoc.isdir( node )?"fwd":"rev", wLoc.isfn( node )?"on":"off", NodeOp.getName( node ) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "f1=%s f2=%s f3=%s f4=%s f5=%s f6=%s f7=%s f8=%s f9=%s f10=%s f11=%s f12=%s",
        (f1?"ON":"OFF"), (f2?"ON":"OFF"), (f3?"ON":"OFF"), (f4?"ON":"OFF"),
        (f5?"ON":"OFF"), (f6?"ON":"OFF"), (f7?"ON":"OFF"), (f8?"ON":"OFF"),
        (f9?"ON":"OFF"), (f10?"ON":"OFF"), (f11?"ON":"OFF"), (f12?"ON":"OFF") );
    ThreadOp.post( data->transactor, (obj)outa );
  }


  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      byte* outa = allocMem(32);

      outa[0] = 1;    /* short packet */
      outa[1] = 3;    /* packet length */
      outa[2] = 0x10; /* command station instruction */
      outa[3] = 2;    /* track control */
      outa[4] = 1; /* switch track voltage OFF */
      ThreadOp.post( data->transactor, (obj)outa );
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      byte* outa = allocMem(32);

      outa[0] = 1;    /* short packet */
      outa[1] = 3;    /* packet length */
      outa[2] = 0x10; /* command station instruction */
      outa[3] = 2;    /* track control */
      outa[4] = 2; /* switch track voltage ON */
      ThreadOp.post( data->transactor, (obj)outa );
    }
    else if( StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Emergency STOP" );
      byte* outa = allocMem(32);

      outa[0] = 1;    /* short packet */
      outa[1] = 3;    /* packet length */
      outa[2] = 0x10; /* command station instruction */
      outa[3] = 2;    /* track control */
      outa[4] = 0; /* stop broadcast */
      ThreadOp.post( data->transactor, (obj)outa );
    }
    else if( StrOp.equals( cmd, wSysCmd.sod ) ) {

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Start of Day" );

      int MX9_id;
      int MX9_sec;

      for (MX9_id=1; MX9_id < ((( data->fbmod + 1 ) / 2 ) + 1 ); MX9_id++) {
        byte* outa = allocMem(32);
        int delay  = (MX9_id * 20) + 800;

        outa[0] = 1;         /* short packet */
        outa[1] = 3;         /* packet length */
        outa[2] = 0x12;      /* command station instruction */
        outa[3] = 3;         /* MX9 control */
        outa[4] = MX9_id;    /* id */

        ThreadOp.post( data->transactor, (obj)outa );

        for (MX9_sec=0; MX9_sec<8; MX9_sec++) {
          byte* outa = allocMem(32);

          outa[0] = 1;         /* short packet */
          outa[1] = 4;         /* packet length */
          outa[2] = 0x12;      /* command station instruction */
          outa[3] = 4;         /* MX9 control */
          outa[4] = MX9_id;    /* id */
          outa[5] = MX9_sec;   /* section */

          ThreadOp.post( data->transactor, (obj)outa );
        }
      }
    } else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "SYSCMD %s", cmd );
    }
  }
  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {

    if( wProgram.getcmd( node ) == wProgram.get ) {
      int cv = wProgram.getcv( node );

      byte* outa = allocMem(32);
      byte  addrFormat = 0x80;  /* Only DCC */

      outa[0] = 1;              /* short packet */
      outa[1] = 6;              /* packet length */
      outa[2] = 0x10;           /* command station instruction */
      outa[3] = 19;             /* read/set cv */
      outa[4] = 0 | addrFormat; /* addr high  0=use PT */
      outa[5] = 0;              /* addr low   0=use PT */
      outa[6] = cv / 256;       /* cv addr high */
      outa[7] = cv % 256;       /* cv addr low */
      outa[8] = PTIME / 256;
      outa[9] = PTIME % 256;

      MutexOp.wait( data->tmux );
      QueueOp.post( data->tqueue, (obj)outa, normal );
      MutexOp.post( data->tmux );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get CV %d...", cv );
    }
    else if( wProgram.getcmd( node ) == wProgram.set ) {

      int cv    = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int addr  = wProgram.getaddr( node );


      if( wProgram.ispom(node) ) {
        byte* outa = allocMem(32);
        byte  addrFormat = 0x80;  /* Only DCC */

        outa[0] = 1;              /* short packet */
        outa[1] = 7;              /* packet length */
        outa[2] = 0x10;           /* command station instruction */
        outa[3] = 19;             /* read/set cv */
        outa[4] = (addr / 256) | addrFormat; /* addr high  0=use PT */
        outa[5] = addr % 256;                /* addr low   0=use PT */
        outa[6] = cv / 256;       /* cv addr high */
        outa[7] = cv % 256;       /* cv addr low */
        outa[8] = value;          /* cv value */
        outa[9] = PTIME / 256;
        outa[10] = PTIME % 256;

        MutexOp.wait( data->tmux );
        QueueOp.post( data->tqueue, (obj)outa, normal );
        MutexOp.post( data->tmux );

        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "POM: set CV %d of loc %d to %d...", cv, addr, value );
      } else {
        byte* outa = allocMem(32);
        byte  addrFormat = 0x80;  /* Only DCC */

        outa[0] = 1;              /* short packet */
        outa[1] = 7;              /* packet length */
        outa[2] = 0x10;           /* command station instruction */
        outa[3] = 19;             /* read/set cv */
        outa[4] = 0 | addrFormat; /* addr high  0=use PT */
        outa[5] = 0;                /* addr low   0=use PT */
        outa[6] = cv / 256;       /* cv addr high */
        outa[7] = cv % 256;       /* cv addr low */
        outa[8] = value;          /* cv value */
        outa[9] = PTIME / 256;
        outa[10] = PTIME % 256;

        MutexOp.wait( data->tmux );
        QueueOp.post( data->tqueue, (obj)outa, normal );
        MutexOp.post( data->tmux );

        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: set CV %d to %d...", cv, value );
      }
    }
    else if(  wProgram.getcmd( node ) == wProgram.pton ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT ON" );
      data->pt = True;
    }  /* PT off, send: All ON" */
    else if( wProgram.getcmd( node ) == wProgram.ptoff ) {
      data->pt = False;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT OFF" );
    }
  }

  return rsp;
}



/**  */
static iONode _cmd( obj inst ,const iONode nodeA ) {
  iOZimoBinData data = Data(inst);
  iONode rsp = NULL;

  if( nodeA != NULL ) {
    if( StrOp.equals( NodeOp.getName( nodeA ), wSwitchList.name() ) ) {
      data->switchlist = nodeA;
    }
    else if( StrOp.equals( NodeOp.getName( nodeA ), wLocList.name() ) ) {
      data->locolist = nodeA;
    }
    else {
      rsp = __translate( (iOZimoBin)inst, nodeA );
      /* Cleanup Node1 */
      nodeA->base.del(nodeA);
    }
  }

  return rsp;
}


/**  */
static void _halt( obj inst, Boolean poweroff ) {
  iOZimoBinData data = Data(inst);

  byte* outa = allocMem(32);

  outa[0] = 1;    /* short packet */
  outa[1] = 4;    /* packet length */
  outa[2] = 0x10; /* command station instruction */
  outa[3] = 17;   /* serial info  */
  outa[4] = TOOLID;    /* toolid */
  outa[5] = 0;    /* action halt */
  ThreadOp.post( data->transactor, (obj)outa );

  ThreadOp.sleep(500);

  data->run = False;
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOZimoBinData data = Data(inst);
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
  iOZimoBinData data = Data(inst);
  return;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  iOZimoBinData data = Data(inst);

  int state = 0;
  state |= data->power << 0;
  state |= data->pt    << 1;
  state |= data->comm  << 2;
  return state;
}


/**  */
static Boolean _supportPT( obj inst ) {
  iOZimoBinData data = Data(inst);
  return 0;
}

/* Zimo MX9 has 8 Sections with two current sensors */
/* each Section has zimo bidi feedback and short detection */
/* +0    normal feedback sensor nr */
/* +1000 short detection feedback */
/* +2000 bidi feedback */

static void __handleTrackSection(iOZimoBin zimobin, byte mx9_id, byte mx9_sec, byte bstat, byte *lstat) {
  iOZimoBinData data = Data(zimobin);

  byte valid   = bstat & 0x80;
  byte changed = bstat & 0x40;
  byte ues     = bstat & 0x20;
  byte secb    = (bstat & 0x10) ? 0x01 : 0x00;
  byte seca    = (bstat & 0x08) ? 0x01 : 0x00;
  byte nrloco  = bstat & 0x07;
  byte fstat;
  int i;

  if( lstat != NULL ) {
    fstat = lstat[0];
  } else {
    fstat = 255;
  }

  if ( nrloco && (lstat != NULL) ) {
    int  addr    = ((((mx9_id-1) * 16) + (mx9_sec * 2) + 1) + (data->fboffset * 8)) + 2000;

    lstat++;
    for (i=0; i<nrloco; i++) {

      int  loco = lstat[0] * 256 + lstat[1];
      char ident[32];

      iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );

      wFeedback.setaddr( nodeC, addr );
      wFeedback.setfbtype( nodeC, wFeedback.fbtype_railcom );
      wFeedback.setstate( nodeC, True );
      StrOp.fmtb(ident, "%d", loco);
      wFeedback.setidentifier( nodeC, ident);

      if ( data->iid != NULL )
        wFeedback.setiid( nodeC, data->iid );
      if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "tracksection: BiDi MX9=%d, section=%d:%d, value=%d", mx9_id, mx9_sec, fstat, loco );

      lstat++;
      lstat++;
    }
  } else {
    int  addr    = (((mx9_id-1) * 16) + (mx9_sec * 2) + 1) + (data->fboffset * 8) + 2000;
    char ident[32];
    iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );

    wFeedback.setaddr( nodeC, addr );
    wFeedback.setfbtype( nodeC, wFeedback.fbtype_railcom );
    wFeedback.setstate( nodeC, False );
    StrOp.fmtb(ident, "%d", 0);
    wFeedback.setidentifier( nodeC, ident);

    if ( data->iid != NULL )
      wFeedback.setiid( nodeC, data->iid );
    if( data->listenerFun != NULL && data->listenerObj != NULL )
      data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "tracksection: BiDi MX9=%d, section=%d no Loc", mx9_id, mx9_sec );
  }

  if( valid ) {
    if( changed ) {
      int addr = (((mx9_id-1) * 16) + (mx9_sec * 2) + 1) + (data->fboffset * 8);

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "tracksection: MX9=%d, section=%d, addr=%d status=0x%02X:0x%02X", mx9_id, mx9_sec, addr, seca, ues );
      {
        iONode nodeF = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
        wFeedback.setaddr( nodeF, addr );
        wFeedback.setstate( nodeF, seca?True:False );
        if ( data->iid != NULL )
          wFeedback.setiid( nodeF, data->iid );
        if( data->listenerFun != NULL && data->listenerObj != NULL )
          data->listenerFun( data->listenerObj, nodeF, TRCLEVEL_INFO );
      }
      addr++;

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "tracksection: MX9=%d, section=%d, addr=%d status=0x%02X:0x%02X", mx9_id, mx9_sec, addr, secb, ues );
      {
        iONode nodeF = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
        wFeedback.setaddr( nodeF, addr );
        wFeedback.setstate( nodeF, secb?True:False );
        if ( data->iid != NULL )
          wFeedback.setiid( nodeF, data->iid );
        if( data->listenerFun != NULL && data->listenerObj != NULL )
          data->listenerFun( data->listenerObj, nodeF, TRCLEVEL_INFO );
      }
      addr = addr + 999;
      {
        iONode nodeF = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
        wFeedback.setaddr( nodeF, addr );
        wFeedback.setstate( nodeF, ues?True:False );
        if ( data->iid != NULL )
          wFeedback.setiid( nodeF, data->iid );
        if( data->listenerFun != NULL && data->listenerObj != NULL )
          data->listenerFun( data->listenerObj, nodeF, TRCLEVEL_INFO );
      }
      addr++;
      {
        iONode nodeF = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
        wFeedback.setaddr( nodeF, addr );
        wFeedback.setstate( nodeF, ues?True:False );
        if ( data->iid != NULL )
          wFeedback.setiid( nodeF, data->iid );
        if( data->listenerFun != NULL && data->listenerObj != NULL )
          data->listenerFun( data->listenerObj, nodeF, TRCLEVEL_INFO );
      }
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "tracksection: MX9=%d, section=%d, speed=%d", mx9_id, mx9_sec, fstat );
  } else {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "tracksection: MX9=%d, section=%d, invalid", mx9_id, mx9_sec );
  }
}

static void __handleAccessoryFeedback(iOZimoBin zimobin, byte* swp) {
  iOZimoBinData data = Data(zimobin);

  int daddr     = ((swp[0] & 0x3f) * 256) + swp[1];
  int cPair     = swp[2];
  int cOutput   = swp[3];

  int i;
  int port;
  int state;
  int gate;


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "AccessoryFeedback: daddr=%d 0x%02X:0x%02X", daddr, cPair, cOutput );

  for (i=0; i<4; i++) {
    port  = i + 1;
    state = (cOutput >> (i*2)) & 0x02;
    gate  = (cOutput >> (i*2)) & 0x03;

    /* inform listener */
    iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
    if( data->iid != NULL )
      wSwitch.setiid( nodeC, data->iid );

    wSwitch.setaddr1( nodeC, daddr );
    wSwitch.setport1( nodeC, port );
    wSwitch.setstate( nodeC, state?"straight":"turnout" );

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }

}

static void __handleLocoFeedback(iOZimoBin zimobin, byte* locop) {
  iOZimoBinData data = Data(zimobin);

  int daddr     = ((locop[0] & 0x3f) * 256) + locop[1];
  int cspeed    = locop[2];
  Boolean man   = locop[3] & 0x80;
  Boolean dir   = locop[3] & 0x20;
  Boolean f0    = locop[3] & 0x10;
  byte sstep    = (locop[3] & 0x0c) >> 2;
  int f0108     = locop[4];
  int f0912     = locop[5];
  int azbz      = locop[6];
  Boolean lakt  = locop[7];

  if( data->protver >= 0 ) {
    char* sthrottleid = StrOp.fmt("zimo/%d", daddr);

    switch (sstep) {
      case 1:
        sstep = 14;
        break;
      case 2:
        sstep = 28;
        break;
      case 3:
        sstep = 126;
        break;
      default:
        sstep = 0;
    }

    /* inform listener */
    {
      iONode node = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      if( data->iid != NULL )
        wLoc.setiid( node, data->iid );
      wLoc.setaddr( node, daddr );
      wLoc.setV_raw( node, cspeed );
      wLoc.setV_rawMax( node, sstep );
      wLoc.setcmd( node, wLoc.velocity );
      wLoc.setthrottleid( node, sthrottleid );
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
    {
      iONode node = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      if( data->iid != NULL )
        wLoc.setiid( node, data->iid );
      wLoc.setaddr( node, daddr );
      wLoc.setdir( node, dir ? False:True );
      wLoc.setfn( node, f0 ? True:False );
      wLoc.setcmd( node, wLoc.dirfun );
      wLoc.setthrottleid( node, sthrottleid );
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
    {
      iONode node = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
      if( data->iid != NULL )
        wLoc.setiid( node, data->iid );
      wFunCmd.setaddr( node, daddr );

      wFunCmd.setf1( node, (f0108 & 0x01) ? True:False );
      wFunCmd.setf2( node, (f0108 & 0x02) ? True:False );
      wFunCmd.setf3( node, (f0108 & 0x04) ? True:False );
      wFunCmd.setf4( node, (f0108 & 0x08) ? True:False );
      wFunCmd.setgroup( node, 1 );
      wLoc.setthrottleid( node, sthrottleid );
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
    {
      iONode node = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
      if( data->iid != NULL )
        wLoc.setiid( node, data->iid );
      wFunCmd.setaddr( node, daddr );

      wFunCmd.setf5( node, (f0108 & 0x10) ? True:False );
      wFunCmd.setf6( node, (f0108 & 0x20) ? True:False );
      wFunCmd.setf7( node, (f0108 & 0x40) ? True:False );
      wFunCmd.setf8( node, (f0108 & 0x80) ? True:False );
      wFunCmd.setgroup( node, 2 );
      wLoc.setthrottleid( node, sthrottleid );
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
    {
      iONode node = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
      if( data->iid != NULL )
        wLoc.setiid( node, data->iid );
      wFunCmd.setaddr( node, daddr );

      wFunCmd.setf9(  node, (f0912 & 0x01) ? True:False );
      wFunCmd.setf10( node, (f0912 & 0x02) ? True:False );
      wFunCmd.setf11( node, (f0912 & 0x04) ? True:False );
      wFunCmd.setf12( node, (f0912 & 0x08) ? True:False );
      wFunCmd.setgroup( node, 3 );
      wLoc.setthrottleid( node, sthrottleid );
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
    StrOp.free(sthrottleid);
  }
}

static void __ackHandler(iOZimoBin zimobin, byte* packet, int len) {
  iOZimoBinData data    = Data(zimobin);
  obj omsg = NULL;
  byte out[256];
  int msgack = packet[3];
  int msgerr = packet[4];

  if (len < 6) msgerr = 0;
  if (msgerr) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "ack for %d message=0x%02X type=%d error=%s", msgack, packet[1], packet[2], __getErrCode(msgerr));
    switch (msgerr) {
      case ERR_BUSY:
        return;
      default:
        break;
    }
  } else {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "ack for %d message=0x%02X type=%d error=%s", msgack, packet[1], packet[2], __getErrCode(msgerr));
  }

  int cnt;
  MutexOp.wait( data->pmux );

  for( cnt = QueueOp.count( data->pqueue ); cnt > 0; cnt-- ) {
    omsg = QueueOp.get( data->pqueue );
    if (omsg != NULL) {
      int msgid = ((byte*) omsg)[3] & 0xFF;

      if ( msgid == msgack ) {
        freeMem( omsg );
        break;
      } else {
        QueueOp.post( data->pqueue, omsg, normal );
      }
    } else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "got ack for msg=%d and no msgs", msgack );
    }
  }
  MutexOp.post( data->pmux );
}

static Boolean __evaluatePacket(iOZimoBin zimobin, byte* packet, int len) {
  iOZimoBinData data    = Data(zimobin);

  Boolean ok = True;
  byte seqid = packet[0];
  byte msgt  = packet[1];
  byte subt  = packet[2];
  int i;
  Boolean power = False;
  Boolean halt = True;

  if ( len < 3 ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Invalid packet." );
    return ok;
  }

  if (!(msgt == 0x40 && subt == 17))
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "read %d:%d message=0x%02X type=%d %d %d %d", len, seqid, msgt, subt, packet[3], packet[4], packet[5] );
  if (msgt & 0x80) {  /* long packet */
    unsigned short crc = __checkSum16(packet, len);
    if (crc) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "wrong crc sequenceID=%d message=0x%02X", seqid, msgt );
      __send_nak(zimobin);
      return 1;
    }
    switch (msgt) {
      case 0xc0:  /* command station */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "long CMD %d", subt );
        break;
      case 0xc1:  /* MX8 */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "long MX8 %d", subt );
        break;
      case 0xc2:  /* MX9 */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "long MX9 %d", subt );
        switch (subt) {
          case 0x04:
            __handleTrackSection(zimobin, packet[5], packet[6], packet[7] | 0x40, &packet[8]);
            packet[3] = packet[4];
            packet[4] = 0;
            __ackHandler( zimobin, packet, len );
            break;
        }
        break;
      default:
        break;
    }
  } else {      /* short packet */
    byte crc = __checkSum8(packet, len);
    if (crc) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "wrong crc sequenceID=%d message=0x%02X", seqid, msgt );
      __send_nak(zimobin);
      return 1;
    }
    switch (msgt) {
      case 0x00:    /* command station */
        switch (subt) {
          case 0:    /* reset message */
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get reset message" );
            __send_ack(zimobin, seqid, msgt);
            __initComm( zimobin );
            break;
          case 1:    /* got nak */
            /* Do nothing, the packethandler will resend */
            break;
          case 254:   /* monitor external accessory decoder change */
                      /* cOutput is wrong in Zimo 3.06 */
                      /* the last switched turnout is not in the data */
                      /* workaround query Accessory decoder memory after a short delay */ 
            __send_ack(zimobin, seqid, msgt);
            /* in case off correction
            __handleAccessoryFeedback( zimobin, &packet[3] );
            */
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get external accesory decoder change" );

            {
              byte* outa = allocMem(32);

              outa[0] = 1;    /* short packet */
              outa[1] = 4;    /* packet length */
              outa[2] = 0x10; /* command station instruction */
              outa[3] = 9;    /* accessory command */
              outa[4] = packet[3];
              outa[5] = packet[4];
              outa[6] = 500 / 256;
              outa[7] = 500 % 256;

              MutexOp.wait( data->tmux );
              QueueOp.post( data->tqueue, (obj)outa, normal );
              MutexOp.post( data->tmux );
            }
            break;
          case 255:    /* monitor external loco memory change */
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get external loco memory change" );
            __send_ack(zimobin, seqid, msgt);
            __handleLocoFeedback( zimobin, &packet[3] );
            break;
        }
        break;
      case 0x01:    /* MX8 */
        switch (subt) {
          case 255:    /* MX8 feedback */
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get MX8 feedback" );
            __send_ack(zimobin, seqid, msgt);
            break;
          }
          break;
      case 0x02:    /* MX9 */
        switch (subt) {
          case 255:    /* MX9 feedback */
          __send_ack(zimobin, seqid, msgt);
          __handleTrackSection(zimobin, packet[3], packet[4], packet[5], &packet[6]);
          break;
        }
        break;
      case 0x20:    /* CV Read */
        switch (subt) {
          case 19:
            __send_ack(zimobin, seqid, msgt);
            if ( len == 8 ) {
              int addr   = ((packet[4] & 0x3f) * 256) + packet[5];
              byte err   = packet[6];
              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CV addr=%d error=%s",addr ,__getErrCode(err) );
            } else if ( len == 10 ) {
              iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
              int addr   = ((packet[4] & 0x3f) * 256) + packet[5];
              int cvaddr = (packet[6] * 256) + packet[7];
              int val    = packet[8];

              wProgram.setdecaddr( node, addr );
              wProgram.setcv( node, cvaddr );
              wProgram.setvalue( node, val );
              wProgram.setcmd( node, wProgram.datarsp );

              if( data->iid != NULL )
                wProgram.setiid( node, data->iid );
              if( data->listenerFun != NULL && data->listenerObj != NULL )
                data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CV addr=%d cvaddr=%d val=%d",addr, cvaddr, val );
                                                  
            } else {
              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CV unknown length %d",len );
            }
            break;
          default:
            __send_ack(zimobin, seqid, msgt);
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "TODO message=0x%02X type=%d",
              msgt, subt );
            break;
        }
        break;
      case 0x21:    /* MX8 CV Read */
        __send_ack(zimobin, seqid, msgt);
        break;
      case 0x22:    /* MX9 CV Read */
        __send_ack(zimobin, seqid, msgt);
        break;
      case 0x40:
        switch (subt) {
          case 0:    /* ack for reset message */
            __ackHandler( zimobin, packet, len );
            break;
          case 2:    /* ack for power status */

            if ( packet[4] & 0x01 ) halt = True; else halt = False;
            if ( packet[4] & 0x02 ) power = False; else power = True;

            packet[4] = 0;
            __ackHandler( zimobin, packet, len );

            if ( (power != data->power) || (halt != data->halt) ) {
              data->power = power;
              data->halt = halt;

              if( data->listenerFun != NULL && data->listenerObj != NULL ) {
                iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );

                if( data->iid != NULL )
                  wState.setiid( node, data->iid );
                wState.setpower( node, data->power );
                wState.settrackbus( node, !data->halt );
                wState.setsensorbus( node, True );
                wState.setaccessorybus( node, True );
                wState.setload( node, data->ues );

                data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
              }
            }
            break;
          case 3:    /* ack for loco */
          case 4:    /* ack for func */
          case 5:    /* ack for ifunc */
          case 6:    /* ack for shuttle */
          case 7:    /* ack for accessory decoder */
            __ackHandler( zimobin, packet, len );
            if ( (len > 6) && ( packet[5] & 0x01 ) ) {
              byte* outa = allocMem(32);

              outa[0] = 1;    /* short packet */
              outa[1] = 3;    /* packet length */
              outa[2] = 0x10; /* command station instruction */
              outa[3] = 2;    /* track control */
              outa[4] = 3;    /* query status */
              ThreadOp.post( data->transactor, (obj)outa );
            }
            break;
          case 8:    /* loc status */
            __ackHandler( zimobin, packet, len );
            __handleLocoFeedback( zimobin, &packet[5] );
            break;
          case 9:     /* accessory status */
            __ackHandler( zimobin, packet, len );
            __handleAccessoryFeedback( zimobin, &packet[5] );
            break;
          case 10:    /* address control */
            __ackHandler( zimobin, packet, len );
            break;
          case 11:    /* command station io state */
            __ackHandler( zimobin, packet, len );
            int i1 = packet[5] * 256 + packet[6];       /* in 0.01 A */
            int u1 = packet[7];                         /* in 0.10 V */
            int i2 = packet[8] * 256 + packet[9];
            int u2 = packet[10];
            byte aux = packet[11];

            if ( i1 & 0x8000 ) i1 = 0;
            if ( i2 & 0x8000 ) i2 = 0;

            int ues = (i1 + i2) * 10;

            if ( ues != data->ues ) {
              data->ues = ues;

              if( data->listenerFun != NULL && data->listenerObj != NULL ) {
                iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );

                if( data->iid != NULL )
                  wState.setiid( node, data->iid );
                wState.setpower( node, data->power );
                wState.settrackbus( node, !data->halt );
                wState.setsensorbus( node, True );
                wState.setaccessorybus( node, True );
                wState.setload( node, data->ues );

                data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
              }
            }

            break;
          case 12:    /* command station cv */
          case 13:    /* command station equipment */
            __ackHandler( zimobin, packet, len );
            break;
          case 17:    /* ack 17 */
            __ackHandler( zimobin, packet, len );
            break;
          case 18:    /* mx1 display write ack */
            __ackHandler( zimobin, packet, len );
            break;
          case 19:    /* cv read/set level 1 reply */
            if ( packet[4] ) {
              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "ERROR on Programming Track %s", __getErrCode(packet[4]) );
              packet[4] = 0;
            }
            __ackHandler( zimobin, packet, len );
            break;
          default:
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "TODO message=0x%02X type=%d", msgt, subt );
            __ackHandler( zimobin, packet, len );
            break;
        }
        break;
      case 0x41:  /* MX8 */
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "TODO message=0x%02X type=%d", msgt, subt );
        __ackHandler( zimobin, packet, len );
        break;
      case 0x42:  /* MX9 */
        switch (subt) {
          case 0:	/* ack */
          case 1:	/* ack */
          case 2:	/* ack */
            __ackHandler( zimobin, packet, len );
            break;
          case 3:
            packet[4] = 0;
            __ackHandler( zimobin, packet, len );
            for (i=0; i<8; i++) {
              __handleTrackSection(zimobin, packet[5], i, packet[6+i], NULL);
            }
            break;
          case 5:	/* ack set speed limit */
            __ackHandler( zimobin, packet, len );
            break;
          default:
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "TODO message=0x%02X type=%d", msgt, subt );
            __ackHandler( zimobin, packet, len );
            break;
        }
        break;
    }
  }

  return ok;
}

#define PSLEEP 100

static void __packethandler( void* threadinst ) {
  iOThread      th      = (iOThread)threadinst;
  iOZimoBin     zimobin = (iOZimoBin)ThreadOp.getParm(th);
  iOZimoBinData data    = Data(zimobin);
  obj omsg = NULL;

  int pcnt;

  /* SystemOp.getTick(); */

  ThreadOp.setDescription( th, "Packet Handler for ZimoBin" );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Packet Handler started." );

  do {

    MutexOp.wait( data->pmux );
    for (pcnt = QueueOp.count( data->pqueue ); pcnt > 0; pcnt--) {

      omsg = QueueOp.get( data->pqueue );
      if ( omsg != NULL ) {
        byte cnt       = ((byte*) omsg)[0] & 0xFF;
        byte packettyp = ((byte*) omsg)[1] & 0xFF;
        byte packetlen = ((byte*) omsg)[2] & 0xFF;
        byte msgid     = ((byte*) omsg)[3] & 0xFF;


        if ( cnt ) {
          ((byte*) omsg)[0] = --cnt;
          QueueOp.post( data->pqueue, omsg, normal);
          pcnt--;
        } else {
          byte out[64];

          MemOp.copy( out, (byte*) omsg+3, packetlen );
          freeMem( omsg );

          if( data->protver > 2 ) {
            byte* outa = allocMem(128);
            MemOp.copy( outa+3, (byte*) out, packetlen);
            outa[0] = 10;
            outa[1] = packettyp;
            outa[2] = packetlen;
            QueueOp.post( data->pqueue, (obj)outa, normal );
          }
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "write again %d:%d message=0x%02X type=%d %d %d %d", packetlen, out[0], out[1], out[2], out[3], out[4], out[5] );
          packetlen = __escapePacket(out, packetlen);
          packetlen = __controlPacket(out, packetlen);
          SerialOp.write( data->serial, (char*) out, packetlen );
        }
      } else if ( QueueOp.count( data->pqueue ) > 0 ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Queue pqueue obj=NULL and count=%d", QueueOp.count( data->pqueue) );
      }
    }
    MutexOp.post( data->pmux );

    MutexOp.wait( data->tmux );
    for (pcnt = QueueOp.count( data->tqueue ); pcnt > 0; pcnt--) {
      omsg = QueueOp.get( data->tqueue );
      if ( omsg != NULL ) {
        int packetlen = ((byte*) omsg)[1] & 0xFF;
        int stime     = ((((byte*) omsg)[packetlen + 2] & 0xFF) * 256) + (((byte*) omsg)[packetlen + 3] & 0xFF);

        if ( (stime / PSLEEP) > 0 ) {
          stime = stime - PSLEEP;
          ((byte*) omsg)[packetlen + 2] = stime / 256;
          ((byte*) omsg)[packetlen + 3] = stime % 256;
          QueueOp.post( data->tqueue, omsg, normal );
        } else {
          ThreadOp.post( data->transactor, omsg );
        }
      } else if ( QueueOp.count( data->tqueue ) > 0 ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Queue tqueue obj=NULL and count=%d", QueueOp.count( data->tqueue) );
      }
    }
    MutexOp.post( data->tmux );

    ThreadOp.sleep( PSLEEP );

  } while ( data->run );
}

static void __initComm( iOZimoBin zimobin ) {
  iOZimoBinData data    = Data(zimobin);

  if ( !data->comm ) {
    while ( !SerialOp.open( data->serial ) && !data->dummyio ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init ZimoBin port %s!", wDigInt.getdevice( data->ini ) );

      ThreadOp.sleep(2500); /* wait some time for usb-devices */
    }

    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Init ZimoBin port %s!", wDigInt.getdevice( data->ini ) );

  }

  data->comm = True;


  /* prepare init packet */
  { /* reset packet       MX1 resets sometimes after power on if missed */
    byte* outa = allocMem(32);

    outa[0] = 1;    /* short packet */
    outa[1] = 2;    /* packet length */
    outa[2] = 0x10; /* command station instruction */
    outa[3] = 0;    /* reset */
    ThreadOp.post( data->transactor, (obj)outa );
  }

  {
    byte* outa = allocMem(32);

    outa[0] = 1;    /* short packet */
    outa[1] = 4;    /* packet length */
    outa[2] = 0x10; /* command station instruction */
    outa[3] = 17;   /* serial info  */
    outa[4] = TOOLID;    /* toolid */
    outa[5] = 1;    /* action start */
    ThreadOp.post( data->transactor, (obj)outa );
  }

  /* init mx9 */
  int MX9_id;
  int MX9_sec;

  for (MX9_id=1; MX9_id < ((( data->fbmod + 1 ) / 2 ) + 1 ); MX9_id++) {
    byte* outa = allocMem(32);
    int delay  = (MX9_id * 250) + 3000;

    outa[0] = 1;         /* short packet */
    outa[1] = 4;         /* packet length */
    outa[2] = 0x12;      /* command station instruction */
    outa[3] = 0;         /* MX9 control */
    outa[4] = MX9_id;    /* id */
    outa[5] = 0xff;      /* all sections */

    outa[6] = delay / 256;
    outa[7] = delay % 256;

    MutexOp.wait( data->tmux );
    QueueOp.post( data->tqueue, (obj)outa, normal );
    MutexOp.post( data->tmux );

    for (MX9_sec=0; MX9_sec<8; MX9_sec++) {
      byte* outa = allocMem(32);
      int delay  = (MX9_sec * 250) + (MX9_id * 8000);

      outa[0] = 1;         /* short packet */
      outa[1] = 5;         /* packet length */
      outa[2] = 0x12;      /* command station instruction */
      outa[3] = 5;         /* MX9 control */
      outa[4] = MX9_id;    /* id */
      outa[5] = MX9_sec;   /* section */
      outa[6] = 3;         /* Speed Limit 3=Fahrt */

      outa[7] = delay / 256;
      outa[8] = delay % 256;

      MutexOp.wait( data->tmux );
      QueueOp.post( data->tqueue, (obj)outa, normal );
      MutexOp.post( data->tmux );
    }
  }

  if( data->switchlist != NULL ) {
    int cnt = 0;
    iONode swProps = wSwitchList.getsw( data->switchlist );

    while ( swProps != NULL ) {
      const char *swName = wSwitch.getid( swProps );
      int delay  = (cnt * 100) + 1000;

      int addr  = wSwitch.getaddr1( swProps );
      int port  = wSwitch.getport1( swProps );
      int gate  = wSwitch.getgate1( swProps );

      byte addrFormat = 0x00;

      if( StrOp.equals( wSwitch.prot_M, wSwitch.getprot( swProps ) ) ) /* Motorola */
        addrFormat = 0x40;
      if( StrOp.equals( wSwitch.prot_N, wSwitch.getprot( swProps ) ) ) /* DCC */
        addrFormat = 0x80;

      if( port == 0 ) {
        AddrOp.fromFADA( addr, &addr, &port, &gate );
      }
      else if( addr == 0 && port > 0 ) {
        AddrOp.fromPADA( port, &addr, &port );
      }

      if( port > 0 )
        port--;
      
      byte* outa = allocMem(32);
      byte lock = 0;

      outa[0] = 1;    /* short packet */
      outa[1] = 6;    /* packet length */
      outa[2] = 0x10; /* command station instruction */
      outa[3] = 10;   /* address control */
      outa[4] = (addr / 256) & 0x3f | addrFormat;
      outa[5] = addr % 256;
      outa[6] = 0x80 | 0x20 | (lock ? 0x02 : 0x00) | 0x01;
      outa[7] = 0x03 << (port * 2);

      outa[8] = delay / 256;
      outa[9] = delay % 256;

      MutexOp.wait( data->tmux );
      QueueOp.post( data->tqueue, (obj)outa, normal );
      MutexOp.post( data->tmux );

      cnt++;
      swProps = wSwitchList.nextsw(data->switchlist, swProps);
    }
  }

  if( data->locolist != NULL ) {
    int cnt = 0;
    iONode loProps = wLocList.getlc( data->locolist );

    while ( loProps != NULL ) {
      const char *loName = wLoc.getid( loProps );
      int delay  = (cnt * 100) + 7000;

      int addr  = wLoc.getaddr(loProps);

      byte addrFormat = 0x00;

      if( StrOp.equals( wLoc.prot_M, wLoc.getprot( loProps ) ) ) /* Motorola */
        addrFormat = 0x40;
      if( StrOp.equals( wLoc.prot_N, wLoc.getprot( loProps ) ) ) /* DCC */
        addrFormat = 0x80;

      byte* outa = allocMem(32);
      byte lock = 0;

      outa[0] = 1;    /* short packet */
      outa[1] = 6;    /* packet length */
      outa[2] = 0x10; /* command station instruction */
      outa[3] = 10;   /* address control */
      outa[4] = (addr / 256) & 0x3f | addrFormat;
      outa[5] = addr % 256;
      outa[6] = 0x81;
      outa[7] = delay / 256;
      outa[8] = delay % 256;

      MutexOp.wait( data->tmux );
      QueueOp.post( data->tqueue, (obj)outa, normal );
      MutexOp.post( data->tmux );

      cnt++;
      loProps = wLocList.nextlc(data->locolist, loProps);
    }
  }
}

static void __transactor( void* threadinst ) {
  iOThread      th      = (iOThread)threadinst;
  iOZimoBin     zimobin = (iOZimoBin)ThreadOp.getParm(th);
  iOZimoBinData data    = Data(zimobin);

  obj post = NULL;
  byte esqid = 0x20;
  byte inbuf[512];
  unsigned long ctick = SystemOp.getTick();
  unsigned long sswt = SystemOp.getTick();
  unsigned long ssqt = SystemOp.getTick();
  unsigned long suit = SystemOp.getTick();
  unsigned long spat = SystemOp.getTick();

  ThreadOp.setDescription( th, "Transactor for ZimoBin" );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor started." );

  ThreadOp.sleep(500); /* make sure the loco and switch list did arrive... */

  __initComm( zimobin );

  do {

    int dataAvailable = SerialOp.available(data->serial);
    int inIdx = 0;
    Boolean packetReceived = False;
    while( dataAvailable > 0 && inIdx < sizeof(inbuf)) {
      Boolean ok = SerialOp.read( data->serial, (char*) &inbuf[inIdx], 1 );
      if( !ok ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Read from port failed." );
        data->comm = False;
        break;
      }
      /*
       * Escape control bytes:
       * SOH -> DLE, SOH^0x20
       * EOT -> DLE, EOT^0x20
       * DLE -> DLE, DLE^0x20
      #define SOH 0x01
      #define EOT 0x17
      #define DLE 0x10
      --------------------------------------------------------- |----------------|
      00000000: 01 01 00 0A 00 02 3D 17                         |......=.        |
      20101231.154747.106 r9999c transact OZimoBin 0524 No valid start sequence: idx=1 in=0A
      */
      if( inIdx == 1  && inbuf[inIdx] != SOH  && inbuf[inIdx-1] != SOH  ) {
        inIdx = 0;
        break;
      }

      if( inIdx > 1 ) {
        if( inbuf[inIdx] == EOT && inbuf[inIdx-1] != DLE ) {
          /* end of packet */
          if ( inIdx > 4 ) packetReceived = True;   /* SOH SOH DAT CHK EOT, shortest packet with data */
          inIdx++;
          break;
        }
      }

      /* 1char at 2400bps 4ms */
      dataAvailable = SerialOp.available(data->serial);
      int waitformore = 50;
      while( dataAvailable == 0 && waitformore > 0) {
        ThreadOp.sleep(1);
        dataAvailable = SerialOp.available(data->serial);
        waitformore--;
      }
      inIdx++;
    }

    if( packetReceived ) {
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)inbuf, inIdx );
      inIdx = __unescapePacket(inbuf, inIdx);
      inIdx = __uncontrolPacket(inbuf, inIdx);
      __evaluatePacket(zimobin, inbuf, inIdx);
    }
    else if(inIdx > 1) {
      /* Invalid packet? */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Invalid packet. len %d", inIdx );
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)inbuf, inIdx );
    } 

    ctick = SystemOp.getTick();

    if( (ctick - sswt) < (data->swtime / 10) ) {
      post = NULL;
    } else {
      post = ThreadOp.getPost( th );
    }
    if (post != NULL) {
      byte out[64];

      int packettyp = ((byte*) post)[0] & 0xFF;
      int packetlen = ((byte*) post)[1] & 0xFF;
      esqid++;

      /* sequence byte */
      out[0] = esqid;
      MemOp.copy( out+1, (byte*) post+2, packetlen);
      packetlen++; /* add one for the sequence byte */
      freeMem( post );

      if (packettyp) {
        out[packetlen] = __checkSum8(out, packetlen);
        packetlen++;
      }
      else {
        unsigned short crc = __checkSum16(out, packetlen);
        out[packetlen] = crc / 256;
        packetlen++;
        out[packetlen] = crc % 256;
        packetlen++;
      }
      if ( data->protver > 1 && 
           ((((out[1] & 0xf0) == 0x10) || ((out[1] & 0xf0) == 0x90)) && 
           !((out[2] == 17) || out[2] == 1)) ) {
        byte* outa = allocMem(128);
        MemOp.copy( outa+3, (byte*) out, packetlen);

        outa[0] = 10;
        outa[1] = packettyp;
        outa[2] = packetlen;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "write %d:%d message=0x%02X type=%d %d %d %d", outa[2], outa[3], outa[4], outa[5], outa[6], outa[7], outa[8] );
        MutexOp.wait( data->pmux );
        QueueOp.post( data->pqueue, (obj)outa, normal );
        MutexOp.post( data->pmux );
      }
      if( ((out[1] & 0xf0) == 0x10) && (out[2] == 7) ) { /* accessory command */
        sswt = SystemOp.getTick();
      }
      packetlen = __escapePacket(out, packetlen);
      packetlen = __controlPacket(out, packetlen);
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, packetlen );
      data->comm = SerialOp.write( data->serial, (char*) out, packetlen );

      spat = SystemOp.getTick();
    }

    if ( (ctick - spat) > 110 ) {
      byte* outa = allocMem(32);
      outa[0] = 1;    /* short packet */
      outa[1] = 4;    /* packet length */
      outa[2] = 0x10; /* command station instruction */
      outa[3] = 17;    /* serial info  */
      outa[4] = TOOLID;     /* toolid */
      outa[5] = 2;     /* action refresh */
      ThreadOp.post( data->transactor, (obj)outa );
    }

    if ( (ctick - ssqt) > 590 ) {
      byte* outa = allocMem(32);
      outa[0] = 1;    /* short packet */
      outa[1] = 3;    /* packet length */
      outa[2] = 0x10; /* command station instruction */
      outa[3] = 2;    /* track control */
      outa[4] = 3;    /* query status */
      ThreadOp.post( data->transactor, (obj)outa );
      ssqt = ctick;
    }

    if ( (ctick - suit) > 190 ) {
      byte* outa = allocMem(32);
      outa[0] = 1;    /* short packet */
      outa[1] = 3;    /* packet length */
      outa[2] = 0x10; /* command station instruction */
      outa[3] = 11;   /* ui status */
      outa[4] = 0;    /* query status */
      ThreadOp.post( data->transactor, (obj)outa );
      suit = ctick;
    }

    if( !data->comm ) {
      SerialOp.close( data->serial );
      __initComm( zimobin );
    }

    /* Give up timeslice:*/
    ThreadOp.sleep( 10 );

  } while( data->run );
}


/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;
static int _version( obj inst ) {
  iOZimoBinData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}



/**  */
static struct OZimoBin* _inst( const iONode ini ,const iOTrace trc ) {
  iOZimoBin __ZimoBin = allocMem( sizeof( struct OZimoBin ) );
  iOZimoBinData data = allocMem( sizeof( struct OZimoBinData ) );
  MemOp.basecpy( __ZimoBin, &ZimoBinOp, 0, sizeof( struct OZimoBin ), data );

  TraceOp.set( trc );
  SystemOp.inst();

  /* Initialize data->xxx members... */
  data->ini      = ini;
  data->dummyio  = wDigInt.isdummyio( ini );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );
  data->timeout  = wDigInt.gettimeout( ini );
  data->fbmod    = wDigInt.getfbmod( ini );
  data->fboffset = wDigInt.getfboffset( ini );
  data->swtime   = wDigInt.getswtime( ini );
  data->protver  = wDigInt.getprotver( ini );

  data->power = False;
  data->halt = True;
  data->pt = False;
  data->comm = False;
  data->ues = 1;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ZimoBin %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device          = %s", wDigInt.getdevice( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps             = %d", wDigInt.getbps( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout         = %d", data->timeout );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensors         = %d", data->fbmod );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "offset          = %d", data->fboffset );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch time     = %d", data->swtime );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );

  SerialOp.setBlocking( data->serial, True );
  SerialOp.setFlow( data->serial, cts );
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, 0, wDigInt.isrtsdisabled( ini ) );
  SerialOp.setTimeout( data->serial, data->timeout, data->timeout );

  /*
  if( !SerialOp.open( data->serial ) && !data->dummyio ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init ZimoBin port %s!", wDigInt.getdevice( ini ) );
  }
  else {
    data->run = True;
    data->pqueue = QueueOp.inst( 100 );
    data->pmux = MutexOp.inst( NULL, True );
    data->tqueue = QueueOp.inst( 100 );
    data->tmux = MutexOp.inst( NULL, True );
    data->transactor = ThreadOp.inst( "transactor", &__transactor, __ZimoBin );
    ThreadOp.start( data->transactor );
    data->packethandler = ThreadOp.inst( "packethandler", &__packethandler, __ZimoBin );
    ThreadOp.start( data->packethandler );
    data->comm = True;
  }
  */

  data->run = True;
  data->pqueue = QueueOp.inst( 100 );
  data->pmux = MutexOp.inst( NULL, True );
  data->tqueue = QueueOp.inst( 100 );
  data->tmux = MutexOp.inst( NULL, True );
  data->transactor = ThreadOp.inst( "transactor", &__transactor, __ZimoBin );
  ThreadOp.start( data->transactor );
  data->packethandler = ThreadOp.inst( "packethandler", &__packethandler, __ZimoBin );
  ThreadOp.start( data->packethandler );

  instCnt++;
  return __ZimoBin;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/zimobin.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
