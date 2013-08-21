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

#include "rocrail/wrapper/public/Cmdline.h"

#include "rocnetnode/impl/rocnetnode_impl.h"

#include "rocdigs/impl/rocnet/rocnet-const.h"
#include "rocdigs/impl/rocnet/rn-utils.h"


static int instCnt = 0;

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


static void __reader( void* threadinst ) {
  iOThread            th         = (iOThread)threadinst;
  iORocNetNode     rocnetnode = (iORocNetNode)ThreadOp.getParm( th );
  iORocNetNodeData data       = Data(rocnetnode);
  byte msg[256];
  Boolean reported = False;


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RocNet reader started" );

  while( data->run ) {
    SocketOp.recvfrom( data->readUDP, msg, 0x7F, NULL, NULL );
    TraceOp.dump ( name, TRCLEVEL_INFO, (char*)msg, 8 + msg[7] );
    ThreadOp.sleep(10);

    if( !reported) {
      /* Fake test report: */
      msg[RN_PACKET_GROUP] = RN_GROUP_SENSOR;
      msg[RN_PACKET_ACTION] = RN_SENSOR_REPORT;
      msg[RN_PACKET_LEN] = 3;
      msg[RN_PACKET_DATA+2] = 1;
      rnSenderAddresToPacket( 4711, msg, 0 );
      SocketOp.sendto( data->writeUDP, msg, 32, NULL, 0 );
      reported = True;
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RocNet reader stopped" );
}


#define rnaddr "224.0.0.1"
#define rnport 4321

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

  trc = TraceOp.inst( debug | dump | monitor | parse | TRCLEVEL_INFO | TRCLEVEL_WARNING | TRCLEVEL_CALC, tf, True );
  TraceOp.setAppID( trc, "r" );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Up and running the RocNetNode" );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast address [%s]", rnaddr );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  multicast port    [%d]", rnport );
  data->readUDP = SocketOp.inst( rnaddr, rnport, False, True, True );
  SocketOp.bind(data->readUDP);
  data->writeUDP = SocketOp.inst( rnaddr, rnport, False, True, True );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );


  data->run = True;
  data->reader = ThreadOp.inst( "rocnetreader", &__reader, __RocNetNode );
  ThreadOp.start( data->reader );

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
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutdown the RocNetNode" );
  data->run = False;
  ThreadOp.sleep(100);
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
