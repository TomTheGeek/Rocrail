/*
 Rocs - OS independent C library

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
#include "rocs/public/xmlh.h"



void rocsStatistics(Boolean info) {
  const char* name = "#Rocs";
  int level = info ? TRCLEVEL_INFO:TRCLEVEL_MEMORY;
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", AttrOp.base.name() , AttrOp.base.count(), MemOp.getAllocCntID(RocsAttrID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", CmdLnOp.base.name(), CmdLnOp.base.count(), MemOp.getAllocCntID(RocsCmdLnID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", DirOp.base.name(), DirOp.base.count(), MemOp.getAllocCntID(RocsDirID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", DocOp.base.name(), DocOp.base.count(), MemOp.getAllocCntID(RocsDocID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", EbcdicOp.base.name(), EbcdicOp.base.count(), MemOp.getAllocCntID(RocsEbcdicID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", EventOp.base.name(), EventOp.base.count(), MemOp.getAllocCntID(RocsEventID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", FileOp.base.name(), FileOp.base.count(), MemOp.getAllocCntID(RocsFileID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", LibOp.base.name(), LibOp.base.count(), MemOp.getAllocCntID(RocsLibID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", ListOp.base.name(), ListOp.base.count(), MemOp.getAllocCntID(RocsListID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", MapOp.base.name(), MapOp.base.count(), MemOp.getAllocCntID(RocsMapID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", MsgOp.base.name(), MsgOp.base.count(), MemOp.getAllocCntID(RocsMsgID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", MutexOp.base.name(), MutexOp.base.count(), MemOp.getAllocCntID(RocsMutexID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", NodeOp.base.name(), NodeOp.base.count(), MemOp.getAllocCntID(RocsNodeID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", QueueOp.base.name(), QueueOp.base.count(), MemOp.getAllocCntID(RocsQueueID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", SocketOp.base.name(), SocketOp.base.count(), MemOp.getAllocCntID(RocsSocketID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", "OStr", 0, MemOp.getAllocCntID(RocsStrID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", StringOp.base.name(), StringOp.base.count(), MemOp.getAllocCntID(RocsStringID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", SystemOp.base.name(), SystemOp.base.count(), MemOp.getAllocCntID(RocsSystemID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", ThreadOp.base.name(), ThreadOp.base.count(), MemOp.getAllocCntID(RocsThreadID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", TraceOp.base.name(), TraceOp.base.count(), MemOp.getAllocCntID(RocsTraceID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", XmlhOp.base.name(), XmlhOp.base.count(), MemOp.getAllocCntID(RocsXmlHID) );
  TraceOp.trc( name, level, __LINE__, 9999, "%-12.12s instCnt = %u(%ld)", StrTokOp.base.name(), StrTokOp.base.count(), MemOp.getAllocCntID(RocsStrTokID) );
  
  TraceOp.trc( name, level, __LINE__, 9999, "MemOp.getAllocCount = %ld", MemOp.getAllocCount() );
  TraceOp.trc( name, level, __LINE__, 9999, "MemOp.getAllocSize  = %ld", MemOp.getAllocSize() );
  
}
