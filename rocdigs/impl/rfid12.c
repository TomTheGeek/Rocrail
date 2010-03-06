 /*
 Rocrail - Model Railroad Software

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

/*
The Data OutPut Format – ASCII
STX (ASCII 02) DATA (10 ASCII) CHECK SUM (2 ASCII) CR (ASCII 13) LF (ASCII 10) ETX (ASCII 03)
The communication starts with a start-of-communication (STX) byte (ASCII 02) and ends with end-of-communication (ETX) byte
(ASCII 03). The STX byte is immediately followed by the ten-byte tag ID (unique signature), a checksum, a carriage return
(ASCII 13), a linefeed (ASCII 10) and then the ETX byte.
- serial line setting: 9600 baud, 8 bits, no parity, hardware handshake.
- The checksum is calculated as follows:
It outputs serial data in the following format. Each [] is one ascii byte.
[STX]
[D1] [D2] [D3] [D4] [D5] [D6] [D7] [D8] [D9] [D10]
[CS1] [CS2]
[CR]
[LF]
[ETX]
D1-D10 is the actual data, 10 ascii charactors. CS1 and CS2 are 2 ascii charactors that are the checksum for the data.
Here is how the data sheet says the checksum is done to the data
if the 10 digits of ascii come in and lets say they equal
2 4 0 0 C C 5 7 8 3
and the check sum is
3 C
then to get the checksum you need to take pairs of ascii and convert them into one HEX byte and then XOR the hex bytes.
which would look like this.
2 4 0 0 C C 5 7 8 3
becomes
[24] [00] [CC] [57] [83]
and 3 C becomes [3C]
then you can do a simple [24] ^ [00] ^ [CC] ^ [57] ^ [83] (^ is XOR) to see if it = [3C]
- The reader outputs the serial data any time it reads a valid RFID tag, it doesn't need to be polled.
- If a 8-port concentrator is used, it connects 8 readers and converts them to a single serial port;
to identify the reader, it overwrites the [STX] character of the above data format with the bankid ( 1 to 8 ) in ASCII.

*/

/*
The task level code intercepts a reader's STX character and exchanges it for
a reader ID character A - H appropriate to the incoming line. It then echoes
the next 12 bytes, CR, LF from the reader and exchanges the ETX for a >.
Framing errors are identified by a ! and overrun errors by a ?

Normally you get an STX character at the start of the RFID tag string and an ETX at the end,
the concentrator substitutes these as described above, so I would expect to see data such as

<STX>0413276BC19A
<ETX><STX>0413275FCAA5
<ETX><STX>0413276ADB81
<ETX><STX>041327722163
<ETX>


coming from a reader without a concentrator. <STX> is the non-printable ASCII character for "start of text"
and <ETX> is the "end of text" character.

And

A0413276BC19A
>B0413275FCAA5
>A0413276ADB81
>B041327722163
>

with a concentrator.

*/

#include "rocdigs/impl/rfid12_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"
#include "rocs/public/system.h"

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
#include "rocrail/wrapper/public/RocNet.h"
#include "rocrail/wrapper/public/BinCmd.h"
#include "rocrail/wrapper/public/Clock.h"

#include "rocdigs/impl/common/fada.h"



static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iORFID12Data data = Data(inst);
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

/** ----- ORFID12 ----- */


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  /* Cleanup cmd node to avoid memory leak. */
  cmd->base.del(cmd);
  return NULL;
}


/**  */
static void _halt( obj inst ) {
  iORFID12Data data = Data(inst);
  data->run = False;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  SerialOp.close( data->serial );
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iORFID12Data data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "listener set" );
  return True;
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


/* VERSION: */
static int vmajor = 1;
static int vminor = 4;
static int patch  = 0;
static int _version( obj inst ) {
  iORFID12Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __evaluateRFID(iORFID12 inst, char* rfid, int idx) {
  /* STX data[10] CRC[2] cr lf ETX */
  iORFID12Data data = Data(inst);
  iONode evt = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
  long id = 0;
  int addr = 1;
  int i = 0;

  rfid[11] = '\0';
  byte* b = StrOp.strToByte(rfid + 1);

  for( i = 0; i < 5; i++ ) {
    long tmp = b[i];
    tmp = tmp << ((4-i)*8);
    id = id + tmp;
  }
  freeMem(b);

  if( rfid[0] >= 'A' ) {
    addr = (rfid[0] - 'A') + 1;
  }

  data->readerTick[addr-1] = SystemOp.getTick();
  addr = addr + data->fboffset;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "evaluateRFID[%c][%s]: addr=%d id=%ld", rfid[0], rfid+1, addr, id );

  wFeedback.setstate( evt, True );
  wFeedback.setaddr( evt, addr );
  wFeedback.setbus( evt, wFeedback.fbtype_rfid );
  wFeedback.setidentifier( evt, id );
  if( data->iid != NULL )
    wFeedback.setiid( evt, data->iid );

  data->listenerFun( data->listenerObj, evt, TRCLEVEL_INFO );
}

static void __RFIDTicker( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iORFID12 inst = (iORFID12)ThreadOp.getParm( th );
  iORFID12Data data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "RFID ticker started." );
  ThreadOp.sleep(1000);

  while( data->run ) {
    int i = 0;
    for( i = 0; i < 8; i++ ) {
      if( data->readerTick[i] > 0 && (SystemOp.getTick() - data->readerTick[i]) > 250 ) {
        iONode evt = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
        wFeedback.setstate( evt, False );
        wFeedback.setaddr( evt, i + 1 + data->fboffset );
        wFeedback.setbus( evt, wFeedback.fbtype_rfid );
        wFeedback.setidentifier( evt, 0 );
        if( data->iid != NULL )
          wFeedback.setiid( evt, data->iid );

        data->listenerFun( data->listenerObj, evt, TRCLEVEL_INFO );
        
        data->readerTick[i] = 0;
      }
      ThreadOp.sleep( 100 );
    }
    
    ThreadOp.sleep( 100 );
  };

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "RFID ticker ended." );
}


static void __RFIDReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iORFID12 inst = (iORFID12)ThreadOp.getParm( th );
  iORFID12Data data = Data(inst);
  Boolean ok = False;

  /* IO buffer */
  char rfid[256] = {'A', '2', '4', '0', '0', 'C', 'C', '5', '7', '8', '3', '\0'};
  int idx = 0;
  Boolean packetStart = False;

  /* STX (ASCII 02) DATA (10 ASCII) CHECK SUM (2 ASCII) CR (ASCII 13) LF (ASCII 10) ETX (ASCII 03) */
  /* ChID, 10 Alphanumeric characters, 2 checksum characters, CR, LF, > */
  /* "A 0413276C1C 40 \r\n>" */
  ThreadOp.sleep(1000);

  /* test */
  /*__evaluateRFID(inst, rfid, idx);*/

  data->initOK = False;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "RFID reader started." );

  while( data->run ) {
    int bAvail = SerialOp.available(data->serial);
    if (bAvail < 0) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error; exit reader." );
      break;
    }

    while (bAvail > 0) {
      char c;
      SerialOp.read( data->serial, &c, 1 );
      TraceOp.dump( NULL, TRCLEVEL_BYTE, &c, 1 );

      if( !packetStart && (c == 0x02 || c >= 'A' && c <= 'H' ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "packet start detected: [0x%02X]", c );
        /* STX */
        packetStart = True;
        idx = 0;
        rfid[idx] = c;
        idx++;
      }
      else if(packetStart) {
        if( c == 0x03 || c == '>' ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "packet end detected: [0x%02X] idx=%d", c, idx );
          /* ETX */
          packetStart = False;
          rfid[idx] = c;
          idx++;
          /* evaluate the paket */
          TraceOp.dump( NULL, TRCLEVEL_BYTE, rfid, idx );
          __evaluateRFID(inst, rfid, idx);
        }
        else if( idx < 15 ) {
          rfid[idx] = c;
          idx++;
          TraceOp.dump( NULL, TRCLEVEL_BYTE, rfid, idx );
        }
      }

      bAvail = SerialOp.available(data->serial);
      if (bAvail < 0) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error; exit reader." );
        break;
      }
    }


    ThreadOp.sleep( 10 );
  }

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "RFID reader ended." );

}



/**  */
static struct ORFID12* _inst( const iONode ini ,const iOTrace trc ) {
  iORFID12 __RFID12 = allocMem( sizeof( struct ORFID12 ) );
  iORFID12Data data = allocMem( sizeof( struct ORFID12Data ) );
  MemOp.basecpy( __RFID12, &RFID12Op, 0, sizeof( struct ORFID12 ), data );

  TraceOp.set( trc );
  SystemOp.inst();

  /* Initialize data->xxx members... */
  data->device   = StrOp.dup( wDigInt.getdevice( ini ) );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  data->bps      = wDigInt.getbps( ini );
  data->fboffset = wDigInt.getfboffset( ini );
  
  MemOp.set( data->readerTick, 0, sizeof(data->readerTick) );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RFID-12 %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid      = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device   = %s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fboffset = %d", data->fboffset );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );


  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, none );
  SerialOp.setLine( data->serial, data->bps, 8, 1, none, wDigInt.isrtsdisabled( ini ) );
  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {
    char* thname = NULL;
    data->run = True;

    thname = StrOp.fmt("rfid12read%X", __RFID12);
    data->reader = ThreadOp.inst( thname, &__RFIDReader, __RFID12 );
    StrOp.free(thname),
    ThreadOp.start( data->reader );

    thname = StrOp.fmt("rfid12tick%X", __RFID12);
    data->ticker = ThreadOp.inst( thname, &__RFIDTicker, __RFID12 );
    StrOp.free(thname),
    ThreadOp.start( data->ticker );
  }
  else
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init rfid12 port!" );



  instCnt++;
  return __RFID12;
}

iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/rfid12.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
