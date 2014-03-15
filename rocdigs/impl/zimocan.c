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

#include "rocdigs/impl/zimocan_impl.h"

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
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Accessory.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/Text.h"

#include "rocdigs/impl/zimocan/zimocan_const.h"
#include "rocdigs/impl/zimocan/serial.h"
#include "rocdigs/impl/zimocan/udp.h"

static int instCnt = 0;


static int __makePacket( byte* msg, int group, int cmd, int mode, int dlc, int id, int addr, int d2, int d3, int d4, int d5, int d6, int d7 );

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOZimoCANData data = Data(inst);
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

/** ----- OZimoCAN ----- */

static iONode __translate( iOZimoCAN inst, iONode node ) {
  iOZimoCANData data = Data(inst);
  iONode rsp = NULL;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "translate: %s", NodeOp.getName( node ) );

  if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      /* CS off */
      byte* msg = allocMem(32);
      msg[0] = __makePacket(msg+1, SYSTEM_CONTROL_GROUP, SYSTEM_POWER, 0x01, 4, 0, 0xC000, 0, 0x40, 0, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power OFF" );
      ThreadOp.post(data->writer, (obj)msg);
    }

    else if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS on */
      byte* msg = allocMem(32);
      msg[0] = __makePacket(msg+1, SYSTEM_CONTROL_GROUP, SYSTEM_POWER, 0x01, 4, 0, 0xC000, 0, 0x20, 0, 0, 0, 0);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "request power ON" );
      ThreadOp.post(data->writer, (obj)msg);
    }
  }

  return rsp;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOZimoCANData data = Data(inst);
  iONode rsp = NULL;

  if( cmd != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "zimocan cmd = %s", NodeOp.getName(cmd) );
    rsp = __translate( (iOZimoCAN)inst, cmd );
    cmd->base.del(cmd);
  }
  return rsp;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOZimoCANData data = Data(inst);
  data->run = False;
  ThreadOp.sleep(100);
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOZimoCANData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _setRawListener( obj inst ,obj listenerObj ,const digint_rawlistener listenerRawFun ) {
  return 0;
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


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;
static int _version( obj inst ) {
  iOZimoCANData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


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

static int __checkSum16(byte* packet, int len) {
  unsigned short checksum = 0xffff;
  int i = 0;
  for( i = 0; i < len; i++ ) {
    checksum = (checksum << 8) ^ crc16_table[((checksum >> 8) & 0xff) ^ packet[i]];
  }

  return checksum;
}


/*
Header Size  Group Cmd+Mode NId    Data0...7 CRC16 Tail
16Bit  8Bit  8Bit  8Bit     16Bit  8x8Bit    16Bit 16 Bit
 */
static int __makePacket( byte* msg, int group, int cmd, int mode, int dlc, int id, int addr, int d2, int d3, int d4, int d5, int d6, int d7 ) {
  int crc = 0;

  msg[0]  = ((HEADER >> 8) & 0xFF);
  msg[1]  = (HEADER & 0xFF);
  msg[2]  = dlc;
  msg[3]  = group;
  msg[4]  = (cmd << 2);
  msg[4] |= (mode & 0x03);
  msg[5]  = (id & 0xFF);
  msg[6]  = ((id >> 8) & 0xFF);
  msg[7]  = (addr & 0xFF);
  msg[8]  = ((addr >> 8) & 0xFF);
  msg[9]  = d2;
  msg[10] = d3;
  msg[11] = d4;
  msg[12] = d5;
  msg[13] = d6;
  msg[14] = d7;

  crc = __checkSum16(msg+2, 13);
  msg[15] = (crc & 0xFF);
  msg[16] = ((crc >> 8) & 0xFF);

  msg[17] = ((TAIL >> 8) & 0xFF);
  msg[18] = (TAIL & 0xFF);

  return 19;
}


static void __reader( void* threadinst ) {
  iOThread      th      = (iOThread)threadinst;
  iOZimoCAN     zimocan = (iOZimoCAN)ThreadOp.getParm( th );
  iOZimoCANData data    = Data(zimocan);
  byte msg[256];
  int size = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ZimoCAN reader started." );

  while( data->run ) {
    if( !data->subAvailable( (obj)zimocan) ) {
      ThreadOp.sleep( 10 );
      continue;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CAN message available" );
    }

    size = data->subRead( (obj)zimocan, msg );
    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ZimoCAN reader ended." );
}


static void __writer( void* threadinst ) {
  iOThread      th      = (iOThread)threadinst;
  iOZimoCAN     zimocan = (iOZimoCAN)ThreadOp.getParm( th );
  iOZimoCANData data    = Data(zimocan);
  int connAckTimer = 0;
  byte eyeCatcher[] = {'Z','1','1','Z',0};
  byte msg[256];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ZimoCAN writer started." );

  while( data->run ) {
    byte * post = NULL;
    int len = 0;

    post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      /* first byte is the message length */
      len = post[0];
      MemOp.copy( msg, post+1, len);
      freeMem( post);
      if( data->subWrite((obj)zimocan, msg, len) ) {

      }
    }

    if( !data->connAck ) {
      connAckTimer++;
      if( connAckTimer > 5000 ) {
        if( data->subWrite((obj)zimocan, eyeCatcher, 4) ) {

        }
      }
    }

    ThreadOp.sleep(10);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ZimoCAN writer ended." );
}


/**  */
static struct OZimoCAN* _inst( const iONode ini ,const iOTrace trc ) {
  iOZimoCAN __ZimoCAN = allocMem( sizeof( struct OZimoCAN ) );
  iOZimoCANData data = allocMem( sizeof( struct OZimoCANData ) );
  MemOp.basecpy( __ZimoCAN, &ZimoCANOp, 0, sizeof( struct OZimoCAN ), data );

  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */

  data->ini      = ini;
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ZimoCAN %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid    = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib = %s", wDigInt.getsublib( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  /* choose interface: */
  if( StrOp.equals( wDigInt.sublib_serial, wDigInt.getsublib( ini ) ) ) {
    /* serial */
    data->subConnect    = SerialConnect;
    data->subDisconnect = SerialDisconnect;
    data->subRead       = SerialRead;
    data->subWrite      = SerialWrite;
    data->subAvailable  = SerialAvailable;
  }
  else  {
    /* UDP */
    data->subConnect    = UDPConnect;
    data->subDisconnect = UDPDisconnect;
    data->subRead       = UDPRead;
    data->subWrite      = UDPWrite;
    data->subAvailable  = UDPAvailable;
  }

  data->connOK = data->subConnect((obj)__ZimoCAN);
  data->run = True;

  data->reader = ThreadOp.inst( "zcanreader", &__reader, __ZimoCAN );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( "zcanwriter", &__writer, __ZimoCAN );
  ThreadOp.start( data->writer );


  instCnt++;
  return __ZimoCAN;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/zimocan.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
