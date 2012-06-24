/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 Some parts are copied from the DDL project of Torsten Vogt: http://www.vogt-it.com/OpenSource/DDL

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
#include "queue.h"

#include "rocs/public/trace.h"
#include "rocs/public/mutex.h"
#include "rocs/public/mem.h"

#include <stdlib.h>

#define QSIZE 2000

iOMutex queue_mutex;   /* mutex to synchronize queue inserts */

static int queue_initialized = 0;
static int out=0, in=0;

typedef struct _tQData {
   int  packet_type;
   int  packet_size;
   char packet[PKTSIZE];
   int  addr;
} tQData ;
tQData QData[QSIZE];

int queue_init() {

   int error,i;
 
   queue_mutex = MutexOp.inst(NULL, True);

   for (i=0; i<QSIZE; i++) {
      QData[i].packet_type=QNOVALIDPKT;
      QData[i].addr=0;
      MemOp.set(QData[i].packet, 0, PKTSIZE);
   }
   in  = 0;
   out = 0;

   queue_initialized = 1;
   
   TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "Queue initialized." );
   return 0;
}       

int queue_empty() {
   return (in==out);
}

void queue_add(int addr, char *packet, int packet_type, int packet_size) {

   if (!queue_initialized) queue_init();
	
   MutexOp.wait(queue_mutex);
   MemOp.set(QData[in].packet,0,PKTSIZE);
   MemOp.copy(QData[in].packet,packet,packet_size);
   QData[in].packet_type=packet_type;
   QData[in].packet_size=packet_size;
   QData[in].addr=addr;
   in++;
   if (in==QSIZE) in=0;
   MutexOp.post(queue_mutex);
}

int queue_get(int *addr, char *packet, int *packet_size) {

   int rtc;

   if (!queue_initialized || queue_empty()) return QEMPTY;

   MemOp.copy(packet,QData[out].packet,PKTSIZE);   
   rtc=QData[out].packet_type;
   *packet_size=QData[out].packet_size;
   *addr=QData[out].addr;
   QData[out].packet_type=QNOVALIDPKT;
   out++;
   if (out==QSIZE) out=0;
   return rtc;   
}
