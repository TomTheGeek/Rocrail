/*
 Rocs - OS independent C library

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "rocs/public/rocs.h"
#include "rocs/public/objbase.h"
#include "rocs/public/trace.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/thread.h"
#include "rocs/public/file.h"
#include "rocs/public/mem.h"
#include "rocs/public/socket.h"
#include "rocs/public/mutex.h"
#include "rocs/public/event.h"
#include "rocs/public/dir.h"
#include "rocs/public/str.h"
#include "rocs/public/string.h"
#include "rocs/public/cmdln.h"
#include "rocs/public/system.h"
#include "rocs/public/queue.h"
#include "rocs/public/ebcdic.h"
#include "rocs/public/lib.h"
#include "rocs/public/msg.h"
#include "rocs/public/strtok.h"


static long instCnt[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
static long __allocCnt  = 0;
static long __allocSize = 0;

void rocsStatistics(Boolean info) {
  const char* name = "#Rocs";
  int level = info ? TRCLEVEL_INFO:TRCLEVEL_MEMORY;

  if( info ) {
    MemOp.set(instCnt, 0, 32 * sizeof(long));
    __allocCnt  = 0;
    __allocSize = 0;
  }

  if(instCnt[RocsAttrID] != MemOp.getAllocCntID(RocsAttrID) ) {
    instCnt[RocsAttrID] = MemOp.getAllocCntID(RocsAttrID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", AttrOp.base.name() , AttrOp.base.count(), MemOp.getAllocCntID(RocsAttrID) );
  }
  if(instCnt[RocsCmdLnID] != MemOp.getAllocCntID(RocsCmdLnID) ) {
    instCnt[RocsCmdLnID] = MemOp.getAllocCntID(RocsCmdLnID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", CmdLnOp.base.name(), CmdLnOp.base.count(), MemOp.getAllocCntID(RocsCmdLnID) );
  }
  if(instCnt[RocsDirID] != MemOp.getAllocCntID(RocsDirID) ) {
    instCnt[RocsDirID] = MemOp.getAllocCntID(RocsDirID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", DirOp.base.name(), DirOp.base.count(), MemOp.getAllocCntID(RocsDirID) );
  }
  if(instCnt[RocsDocID] != MemOp.getAllocCntID(RocsDocID) ) {
    instCnt[RocsDocID] = MemOp.getAllocCntID(RocsDocID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", DocOp.base.name(), DocOp.base.count(), MemOp.getAllocCntID(RocsDocID) );
  }
  if(instCnt[RocsEbcdicID] != MemOp.getAllocCntID(RocsEbcdicID) ) {
    instCnt[RocsEbcdicID] = MemOp.getAllocCntID(RocsEbcdicID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", EbcdicOp.base.name(), EbcdicOp.base.count(), MemOp.getAllocCntID(RocsEbcdicID) );
  }
  if(instCnt[RocsEventID] != MemOp.getAllocCntID(RocsEventID) ) {
    instCnt[RocsEventID] = MemOp.getAllocCntID(RocsEventID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", EventOp.base.name(), EventOp.base.count(), MemOp.getAllocCntID(RocsEventID) );
  }
  if(instCnt[RocsFileID] != MemOp.getAllocCntID(RocsFileID) ) {
    instCnt[RocsFileID] = MemOp.getAllocCntID(RocsFileID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", FileOp.base.name(), FileOp.base.count(), MemOp.getAllocCntID(RocsFileID) );
  }
  if(instCnt[RocsLibID] != MemOp.getAllocCntID(RocsLibID) ) {
    instCnt[RocsLibID] = MemOp.getAllocCntID(RocsLibID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", LibOp.base.name(), LibOp.base.count(), MemOp.getAllocCntID(RocsLibID) );
  }
  if(instCnt[RocsListID] != MemOp.getAllocCntID(RocsListID) ) {
    instCnt[RocsListID] = MemOp.getAllocCntID(RocsListID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", ListOp.base.name(), ListOp.base.count(), MemOp.getAllocCntID(RocsListID) );
  }
  if(instCnt[RocsMapID] != MemOp.getAllocCntID(RocsMapID) ) {
    instCnt[RocsMapID] = MemOp.getAllocCntID(RocsMapID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", MapOp.base.name(), MapOp.base.count(), MemOp.getAllocCntID(RocsMapID) );
  }
  if(instCnt[RocsMutexID] != MemOp.getAllocCntID(RocsMutexID) ) {
    instCnt[RocsMutexID] = MemOp.getAllocCntID(RocsMutexID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", MutexOp.base.name(), MutexOp.base.count(), MemOp.getAllocCntID(RocsMutexID) );
  }
  if(instCnt[RocsNodeID] != MemOp.getAllocCntID(RocsNodeID) ) {
    instCnt[RocsNodeID] = MemOp.getAllocCntID(RocsNodeID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", NodeOp.base.name(), NodeOp.base.count(), MemOp.getAllocCntID(RocsNodeID) );
  }
  if(instCnt[RocsQueueID] != MemOp.getAllocCntID(RocsQueueID) ) {
    instCnt[RocsQueueID] = MemOp.getAllocCntID(RocsQueueID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", QueueOp.base.name(), QueueOp.base.count(), MemOp.getAllocCntID(RocsQueueID) );
  }
  if(instCnt[RocsSocketID] != MemOp.getAllocCntID(RocsSocketID) ) {
    instCnt[RocsSocketID] = MemOp.getAllocCntID(RocsSocketID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", SocketOp.base.name(), SocketOp.base.count(), MemOp.getAllocCntID(RocsSocketID) );
  }
  if(instCnt[RocsStrID] != MemOp.getAllocCntID(RocsStrID) ) {
    instCnt[RocsStrID] = MemOp.getAllocCntID(RocsStrID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", "OStr", 0, MemOp.getAllocCntID(RocsStrID) );
  }
  if(instCnt[RocsStringID] != MemOp.getAllocCntID(RocsStringID) ) {
    instCnt[RocsStringID] = MemOp.getAllocCntID(RocsStringID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", StringOp.base.name(), StringOp.base.count(), MemOp.getAllocCntID(RocsStringID) );
  }
  if(instCnt[RocsSystemID] != MemOp.getAllocCntID(RocsSystemID) ) {
    instCnt[RocsSystemID] = MemOp.getAllocCntID(RocsSystemID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", SystemOp.base.name(), SystemOp.base.count(), MemOp.getAllocCntID(RocsSystemID) );
  }
  if(instCnt[RocsThreadID] != MemOp.getAllocCntID(RocsThreadID) ) {
    instCnt[RocsThreadID] = MemOp.getAllocCntID(RocsThreadID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", ThreadOp.base.name(), ThreadOp.base.count(), MemOp.getAllocCntID(RocsThreadID) );
  }
  if(instCnt[RocsTraceID] != MemOp.getAllocCntID(RocsTraceID) ) {
    instCnt[RocsTraceID] = MemOp.getAllocCntID(RocsTraceID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", TraceOp.base.name(), TraceOp.base.count(), MemOp.getAllocCntID(RocsTraceID) );
  }
  if(instCnt[RocsMsgID] != MemOp.getAllocCntID(RocsMsgID) ) {
    instCnt[RocsMsgID] = MemOp.getAllocCntID(RocsMsgID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", MsgOp.base.name(), MsgOp.base.count(), MemOp.getAllocCntID(RocsMsgID) );
  }
  if(instCnt[RocsStrTokID] != MemOp.getAllocCntID(RocsStrTokID) ) {
    instCnt[RocsStrTokID] = MemOp.getAllocCntID(RocsStrTokID);
    TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", StrTokOp.base.name(), StrTokOp.base.count(), MemOp.getAllocCntID(RocsStrTokID) );
  }
  
  if( __allocCnt != MemOp.getAllocCount() ) {
    __allocCnt = MemOp.getAllocCount();
    TraceOp.trc( name, level, __LINE__, 9999, "MemOp.getAllocCount = %ld", MemOp.getAllocCount() );
  }
  if( __allocSize != MemOp.getAllocSize() ) {
    __allocSize = MemOp.getAllocSize();
    TraceOp.trc( name, level, __LINE__, 9999, "MemOp.getAllocSize  = %ld", MemOp.getAllocSize() );
  }
  
  if( TraceOp.getLevel( NULL ) & TRCLEVEL_MEMORY )
    MemOp.dumpStrings();

}
