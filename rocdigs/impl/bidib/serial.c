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

#include "rocdigs/impl/bidib/serial.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/thread.h"
#include "rocs/public/serial.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"

#include "rocdigs/impl/bidib/bidib_messages.h"


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
  TraceOp.trc( "bidib", TRCLEVEL_DEBUG, __LINE__, 9999, "message de-escaped" );
  TraceOp.dump ( "bidib", TRCLEVEL_DEBUG, (char*)msg, outLen );
  return outLen;
}
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
  TraceOp.dump ( "bidib", TRCLEVEL_DEBUG, (char*)msg, outLen );
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




static void __writer( void* threadinst ) {
  iOThread    th    = (iOThread)threadinst;
  iOBiDiB     bidib = (iOBiDiB)ThreadOp.getParm( th );
  iOBiDiBData data  = Data(bidib);
  char msg[256];

  TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "BIDIB sub writer started." );

  do {
    if( data->commOK ) {
      byte* post = (byte*)ThreadOp.getPost( th );

      if (post != NULL) {
        int len = post[0];
        MemOp.copy( msg, post+1, len);
        freeMem( post);
        TraceOp.dump ( "bidibWrite", TRCLEVEL_BYTE, (char*)msg, len );
        if( SerialOp.write( data->serial, (char*)msg, len ) ) {
        }
        else {
          /* ToDo: Resend? */
        }
      }
      else
        ThreadOp.sleep(10);
    }
    else {
      TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "waiting for connection..." );
      ThreadOp.sleep(2500);
    }
  } while( data->run );

  TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "BIDIB sub writer stopped." );
}


static void __reader( void* threadinst ) {
  iOThread    th    = (iOThread)threadinst;
  iOBiDiB     bidib = (iOBiDiB)ThreadOp.getParm( th );
  iOBiDiBData data  = Data(bidib);
  byte msg[256];
  byte c;
  int index = 0;

  TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "BIDIB sub reader started." );

  do {
    if( data->commOK ) {
      int available = SerialOp.available(data->serial);
      if( available > 0 ) {

        if(SerialOp.read(data->serial, &c, 1) ) {
          TraceOp.trc( "bidib", TRCLEVEL_DEBUG, __LINE__, 9999, "byte read: 0x%02X", c );

          if( c == BIDIB_PKT_MAGIC ) {
            if( index > 0 ) {
              byte* p = allocMem(index+1);
              p[0] = index;
              MemOp.copy( p+1, msg, index);
              QueueOp.post( data->subReadQueue, (obj)p, normal);
              TraceOp.dump ( "bidibserial", TRCLEVEL_DEBUG, (char*)msg, index );
              index = 0;
            }
          }
          else {
            msg[index] = c;
            index++;
            if( index > 127 ) {
              TraceOp.trc( "bidib", TRCLEVEL_WARNING, __LINE__, 9999, "reset index %d !", index );
              index = 0;
            }
            TraceOp.dump ( "bidibserial", TRCLEVEL_DEBUG, (char*)msg, index );
          }
        }

      }
      else if( available == -1 ) {
        /* device error */
        data->commOK = False;
        SerialOp.close(data->serial);
        TraceOp.trc( "bidibserial", TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error" );
      }
      ThreadOp.sleep(10);
    }
    else {
      ThreadOp.sleep(1000);
    }
  } while( data->run );

  TraceOp.trc( "bidib", TRCLEVEL_INFO, __LINE__, 9999, "BIDIB sub reader stopped." );
}


Boolean serialInit( obj inst ) {
  iOBiDiBData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = %s", wDigInt.getdevice( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps     = %d", wDigInt.getbps( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "line    = 8N1 (fix)" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "flow    = CTS (fix)" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout = %d", wDigInt.gettimeout( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
  SerialOp.setFlow( data->serial, cts );
  SerialOp.setLine( data->serial, wDigInt.getbps( data->ini ), 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( data->ini ), wDigInt.gettimeout( data->ini ) );

  return True;
}


Boolean serialConnect( obj inst ) {
  iOBiDiBData data = Data(inst);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "connecting to [%s]", wDigInt.getdevice( data->ini ) );

  if( SerialOp.open( data->serial ) ) {
    if( data->subReadQueue == NULL ) {
      data->subReadQueue  = QueueOp.inst(1000);
      data->subWriteQueue = QueueOp.inst(1000);
      data->subReader = ThreadOp.inst( "bidibsubreader", &__reader, inst );
      ThreadOp.start( data->subReader );
      data->subWriter = ThreadOp.inst( "bidibsubwriter", &__writer, inst );
      ThreadOp.start( data->subWriter );
    }
    return True;
  }

  return False;
}


void serialDisconnect( obj inst ) {
  iOBiDiBData data = Data(inst);

  if( data->serial != NULL ) {
    data->commOK = False;
    SerialOp.close( data->serial );
    SerialOp.base.del( data->serial );
    data->serial = NULL;
  }
}


int serialRead ( obj inst, unsigned char *msg ) {
  iOBiDiBData data = Data(inst);

  if( !QueueOp.isEmpty(data->subReadQueue) ) {
    byte* p = (byte*)QueueOp.get(data->subReadQueue);
    int size = p[0];
    MemOp.copy( msg, &p[1], size );
    freeMem(p);
    size = __deEscapeMessage(msg, size);
    TraceOp.dump ( "bidibRead", TRCLEVEL_BYTE, (char*)msg, size );
    byte crc = __checkSum(msg, size );
    if( crc != 0 ) {
      TraceOp.trc( "bidibserial", TRCLEVEL_EXCEPTION, __LINE__, 9999, "invalid checksum" );
      return 0;
    }
    return size;
  }
  else {
    TraceOp.trc( "bidibserial", TRCLEVEL_DEBUG, __LINE__, 9999, "could not read queue %d", QueueOp.count(data->subReadQueue)  );
  }
  return 0;
}

Boolean serialWrite( obj inst, unsigned char *path, unsigned char code, unsigned char* pdata, int datalen, int seq ) {
  iOBiDiBData data = Data(inst);
  int   size = 0;
  byte  msg[127];
  byte* post = NULL;

  if( MutexOp.wait( data->mux ) ) {
    int msgidx  = 0;
    int dataidx = 0;

    /*                  pathidx: 0  1  2  3
     * Addr can be 4 bytes long. XX XX XX XX */
    for( msgidx = 0; msgidx < 4; msgidx++ ) {
      msg[1+msgidx] = path[msgidx]; // address
      if( msg[1+msgidx] == 0 )
        break;
    }

    msgidx += 2; // point to sequence offset

    msg[msgidx] = seq;
    msgidx++;

    msg[msgidx] = code;
    msgidx++;

    if( pdata != NULL ) {
      for( dataidx = 0; dataidx < datalen; dataidx++ )
        msg[msgidx+dataidx] = pdata[dataidx]; // address
    }

    size = msgidx+dataidx;
    msg[0] = size;

    TraceOp.dump ( "preWrite", TRCLEVEL_BYTE, (char*)msg, size+1 );
    size = __makeMessage(msg, size+1);
    post = allocMem(size+1);

    post[0] = (size & 0xFF);
    MemOp.copy(post+1, msg, size );
    ThreadOp.post(data->subWriter, (obj)post);
    MutexOp.post(data->mux);
  }

  return True;
}


Boolean serialAvailable( obj inst ) {
  iOBiDiBData data = Data(inst);
  return !QueueOp.isEmpty(data->subReadQueue);
}

