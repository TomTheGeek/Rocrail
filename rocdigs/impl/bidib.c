/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>
 http://www.rocrail.net

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

#include "rocdigs/impl/bidib/bidib.h"
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




/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOBiDiBData data = Data(inst);
  if( cmd != NULL ) {
    cmd->base.del(cmd);
  }
  return NULL;
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


static void __handleSensor(iOBiDiB bidib, int addr, Boolean state) {
  iOBiDiBData data = Data(bidib);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensor=%d state=%s", addr, state?"occ":"free" );
  {
    /* inform listener: Node3 */
    iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );

    wFeedback.setaddr( nodeC, addr );
    wFeedback.setfbtype( nodeC, wFeedback.fbtype_sensor );

    if( data->iid != NULL )
      wFeedback.setiid( nodeC, data->iid );

    wFeedback.setstate( nodeC, state );

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}


static void __handleMultipleSensors(iOBiDiB bidib, const byte* msg, int size) {
  // 06 00 02 A2 00 08 01 8B
  int baseAddr = msg[4] * 16;
  int cnt = msg[5] / 8;
  int i = 0;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensor-base=%d cnt=%d", baseAddr, cnt );
  for( i = 0; i < cnt; i++ ) {
    int addr = baseAddr + (i / 2) + 1;
    int bit = 0;
    for( bit = 0; bit < 8; bit++ ) {
      __handleSensor(bidib, addr+bit+((i%2)*8), msg[6+i] & (0x01 << bit));
    }
  }

}



/**
 * len addr seq type data  crc
 * 05  00   00  81   FE AF 89
 */
static void __processBidiMsg(iOBiDiB bidib, byte* msg, int size) {
  iOBiDiBData data = Data(bidib);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "processing bidib message..." );

  int Addr = msg[1];
  int  Seq = msg[2];
  int Type = msg[3]; // MSG_SYS_MAGIC

  switch( Type ) {
  case MSG_SYS_MAGIC:
  { // len = 5
    int Magic = (msg[5]<<8)+msg[4];
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_SYS_MAGIC, addr=%d seq=%d magic=0x%04X", Addr, Seq, Magic );
    data->upSeq = msg[2];
    // query MSG_SYS_GET_P_VERSION
    msg[0] = 3; // length
    msg[1] = 0; // address
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = MSG_SYS_GET_P_VERSION; //data

    size = __makeMessage(msg, 4);
    data->subWrite((obj)bidib, msg, size);
    data->downSeq++;
    break;
  }

  case MSG_SYS_SW_VERSION:
  { // len = 6
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_SYS_SW_VERSION, addr=%d seq=%d version=%d.%d.%d", Addr, Seq, msg[4], msg[5], msg[6] );
    // query MSG_SYS_ENABLE
    msg[0] = 3; // length
    msg[1] = 0; // address
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = MSG_SYS_ENABLE; //data

    size = __makeMessage(msg, 4);
    data->subWrite((obj)bidib, msg, size);
    data->downSeq++;

    // MSG_FEATURE_GET
    msg[0] = 4; // length
    msg[1] = 0; // address
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = MSG_FEATURE_GET; //data
    msg[4] = 8; // feature

    size = __makeMessage(msg, 5);
    data->subWrite((obj)bidib, msg, size);
    data->downSeq++;

    // MSG_BM_GET_RANGE
    msg[0] = 5; // length
    msg[1] = 0; // address
    msg[2] = data->downSeq; // sequence number 1...255
    msg[3] = MSG_BM_GET_RANGE; //data
    msg[4] = 0; // address range
    msg[5] = 0; // address range

    size = __makeMessage(msg, 6);
    data->subWrite((obj)bidib, msg, size);
    data->downSeq++;

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
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_BM_OCC, addr=%d seq=%d local-addr=%d", Addr, Seq, msg[4] );
    __handleSensor(bidib, Addr*16+msg[4]+1, True);
    break;
  }

  case MSG_BM_FREE:
  { // len = 4
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_BM_FREE, addr=%d seq=%d local-addr=%d", Addr, Seq, msg[4] );
    __handleSensor(bidib, Addr*16+msg[4]+1, False);
    break;
  }

  case MSG_BM_MULTIPLE:
  { // 06 00 02 A2 00 08 01 8B
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_BM_MULTIPLE, addr=%d seq=%d local-addr=%d nr-occ=%d, occ=0x%02X", Addr, Seq, msg[4], msg[5], msg[6] );
    __handleMultipleSensors(bidib, msg, size);
    break;
  }

  case MSG_FEATURE:
  { // 05 00 02 90 08 01 EC
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "MSG_FEATURE, addr=%d seq=%d feature=%d value=%d", Addr, Seq, msg[4], msg[5] );
    __handleSensor(bidib, Addr*16+msg[4]+1, False);
    break;
  }

  }
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

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "BiDiB reader started." );

  ThreadOp.sleep(100); /* resume some time to get it all being setup */

  msg[0] = 3; // length
  msg[1] = 0; // address
  msg[2] = data->downSeq; // sequence number 1...255
  msg[3] = MSG_SYS_GET_MAGIC; //data

  size = __makeMessage(msg, 4);
  data->subWrite((obj)bidib, msg, size);
  data->downSeq++;

  while( data->run ) {

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

      if( TEST ) {
        ThreadOp.sleep(2500); // TEST
        msg[0] = 3; // length
        msg[1] = 0; // address
        msg[2] = data->downSeq; // sequence number 1...255
        msg[3] = MSG_SYS_GET_MAGIC; //data

        size = __makeMessage(msg, 4);
        data->subWrite((obj)bidib, msg, size);
        data->downSeq++;
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


/**  */
static struct OBiDiB* _inst( const iONode ini ,const iOTrace trc ) {
  iOBiDiB __BiDiB = allocMem( sizeof( struct OBiDiB ) );
  iOBiDiBData data = allocMem( sizeof( struct OBiDiBData ) );
  MemOp.basecpy( __BiDiB, &BiDiBOp, 0, sizeof( struct OBiDiB ), data );

  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */

  data->ini      = ini;
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  data->run      = True;

  data->commOK = False;

  data->mux      = MutexOp.inst( NULL, True );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "BiDiB %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib  = %s", wDigInt.getsublib( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );


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
