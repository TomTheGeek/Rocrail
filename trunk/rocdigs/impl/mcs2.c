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
  * Module: RocDigs
  * Object: MCS2
  * ------------------------------------------------------------
  */

/*
 * CAN over TCP/IP 13 byte format:
 *
 *  |  0   |  1    | 2 | 3 |  4  | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 |
 *  | PRIO CMD RSP | HASH  | DLC |             DATA                 |
 *
 *
 *
 *
 */

#include "rocdigs/impl/mcs2_impl.h"
#include "rocdigs/impl/mcs2/mcs2-const.h"

#include "rocs/public/mem.h"

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
#include "rocrail/wrapper/public/BinCmd.h"

#include "rocdigs/impl/common/fada.h"


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOMCS2Data data = Data(inst);
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

/** ----- OMCS2 ----- */

static void __setSysMsg( byte* msg, int prio, int cmd, Boolean rsp, int len, long addr, int subcmd ) {
  msg[0]  = (prio << 1);
  msg[0] |= (cmd >> 7);
  msg[1]  = ((cmd & 0x7F) << 1 );
  msg[1] |= rsp;
  msg[2]  = 0x03;
  msg[3]  = 0x00;
  msg[4]  = len;
  msg[5]  = (addr & 0xFF000000) >> 24;
  msg[6]  = (addr & 0x00FF0000) >> 16;
  msg[7]  = (addr & 0x0000FF00) >> 8;
  msg[8]  = (addr & 0x000000FF);
  msg[9]  = subcmd;
}

static iONode __translate( iOMCS2 inst, iONode node ) {
  iOMCS2Data data = Data(inst);
  byte*  out = allocMem(32);
  iONode rsp = NULL;

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "System STOP" );
      __setSysMsg(out, 0, CMD_SYSTEM, False, 5, 0, CMD_SYSSUB_STOP);
      ThreadOp.post( data->writer, (obj)out );
      return rsp;
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "System GO" );
      __setSysMsg(out, 0, CMD_SYSTEM, False, 5, 0, CMD_SYSSUB_GO);
      ThreadOp.post( data->writer, (obj)out );
      return rsp;
    }
  }

  freeMem(out);
  return NULL;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOMCS2Data data = Data(inst);
  iONode rsp = __translate( (iOMCS2)inst, cmd );

  /* Cleanup Node1 */
  cmd->base.del(cmd);

  return rsp;
}


/**  */
static void _halt( obj inst ) {
  iOMCS2Data data = Data(inst);
  data->run = False;
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  return 0;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  return 0;
}


/**  */
static Boolean _supportPT( obj inst ) {
  return False;
}


static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);
  char in[16];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 reader started." );

  do {

    SocketOp.recvfrom( data->readUDP, in, 13 );
    TraceOp.dump( NULL, TRCLEVEL_INFO, in, 13 );

    ThreadOp.sleep(0);

  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 reader stopped." );
}


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMCS2 mcs2 = (iOMCS2)ThreadOp.getParm( th );
  iOMCS2Data data = Data(mcs2);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 writer started." );

  do {
    byte* cmd = (byte*)ThreadOp.getPost( th );
    if (cmd != NULL) {
      TraceOp.dump( NULL, TRCLEVEL_INFO, cmd, 13 );
      SocketOp.sendto( data->writeUDP, cmd, 13 );
      freeMem( cmd );
    }

    ThreadOp.sleep(10);
  } while( data->run );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2 writer stopped." );
}



/* VERSION: */
static int vmajor = 0;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOMCS2Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**
 * UDP Broadcast address = wDigInt.gethost()
 * UDP send port 15730 (fix)
 * UDP recv port 15731 (fix)
 *
 * */
static struct OMCS2* _inst( const iONode ini ,const iOTrace trc ) {
  iOMCS2 __MCS2 = allocMem( sizeof( struct OMCS2 ) );
  iOMCS2Data data = allocMem( sizeof( struct OMCS2Data ) );
  MemOp.basecpy( __MCS2, &MCS2Op, 0, sizeof( struct OMCS2 ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MCS2(1.0) %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->ini = ini;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast address [%s]", wDigInt.gethost(data->ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast tx port [%d]", 15731 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast rx port [%d]", 15730 );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->readUDP = SocketOp.inst( wDigInt.gethost(data->ini), 15730, False, True );
  SocketOp.bind(data->readUDP);
  data->writeUDP = SocketOp.inst( wDigInt.gethost(data->ini), 15731, False, True );

  data->run = True;

  data->reader = ThreadOp.inst( "mcs2reader", &__reader, __MCS2 );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( "mcs2writer", &__writer, __MCS2 );
  ThreadOp.start( data->writer );

  instCnt++;
  return __MCS2;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/mcs2.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
