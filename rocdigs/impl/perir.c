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
 * http://home.swipnet.se/perz/ir.html
 *
Serial protocol

The serial protocol is 9600 baud, 8 data bits, no parity, 2 stop bits.

The most upstream decoder in the chain sends out a 3-byte message once every 5 ms.
The message is either an IDLE message or a VALID message. The message format is as follows:

Byte no:  VALID message:  IDLE message:
0   Sent as 0x01 by the originating decoder, incremented with 2 by each forwarding decoder.
      IDLE: Sent as 0x00 by the originating decoder, incremented with 2 by each forwarding decoder.
1   IR detector port number on the decoder (0-7).   IDLE: 0xff
2   Received code from the loco.                    IDLE: 0xff

A downstream decoder will always forward the VALID messages. If it has received data on one of its IR
detector ports, it will replace an IDLE message with a VALID message, otherwise it will forward the
IDLE messages too. Each downstream decoder increments byte 0 of all forwarded messages by 2. In this way,
byte 0 of a VALID message coming out from the most downstream decoder will contain the decoder number * 2,
where the decoders are numbered from 0 and up starting with the most downstream one. Byte 0 of the IDLE
messages will contain 2 * (total number of decoders) - 1.

I have chosen to let the most upstream decoder generate the IDLE message, but it is also possible to
connect the serial input of the most upstream decoder to the serial data out from the computer
(after appropriate translation of the voltage levels). In this case you should send one byte
containing 0x00 each time you want to read the decoders. The VALID messages will be the same as
described above, but the IDLE messages will only contain byte 0 in this case. The PIC program supports
both methods.
*/

#include "rocdigs/impl/perir_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Feedback.h"


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOPerIRData data = Data(inst);
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

/** ----- OPerIR ----- */


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  return 0;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOPerIRData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "listener set" );
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
  iOPerIRData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

static void __irTicker( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOPerIR inst = (iOPerIR)ThreadOp.getParm( th );
  iOPerIRData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "ticker started." );
  ThreadOp.sleep(1000);

  while( data->run ) {
    int i = 0;
    for( i = 0; i < 256; i++ ) {
      if( data->readerTick[i] > 0 && (SystemOp.getTick() - data->readerTick[i]) > 250 ) {
        iONode evt = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
        wFeedback.setstate( evt, False );
        wFeedback.setbus( evt, wFeedback.fbtype_lissy );
        wFeedback.setaddr( evt, i );
        wFeedback.setidentifier( evt, 0 );
        if( data->iid != NULL )
          wFeedback.setiid( evt, data->iid );

        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "reset reader %d", i );
        data->listenerFun( data->listenerObj, evt, TRCLEVEL_INFO );

        data->readerTick[i] = 0;
        ThreadOp.sleep( 10 );
      }
    }

    ThreadOp.sleep( 100 );
  };

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "ticker ended." );
}




static void __PerIRReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOPerIR inst = (iOPerIR)ThreadOp.getParm( th );
  iOPerIRData data = Data(inst);
  Boolean ok = False;

  /* IO buffer */
  byte packet[32] = {0};
  byte prev = 0;
  int idx = 0;
  int datalen = 0;
  Boolean insync = False;
  Boolean test = False;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PerIR reader started." );
  ThreadOp.sleep(500);

  data->initOK = False;

  if(test){ // Test
    int decoder = 13;
    int port = 6;
    iONode evt = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
    wFeedback.setaddr( evt, (decoder-1) * 8 + port );
    wFeedback.setbus( evt, wFeedback.fbtype_lissy );
    wFeedback.setidentifier( evt, "4711" );
    wFeedback.setstate( evt, wFeedback.getidentifier(evt) > 0 ? True:False );
    if( data->iid != NULL )
      wFeedback.setiid( evt, data->iid );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "detecoder [%d.%d][%d] reported loco address [%d]",
        decoder, port, wFeedback.getaddr(evt), wFeedback.getidentifier(evt) );

    data->listenerFun( data->listenerObj, evt, TRCLEVEL_INFO );
    data->readerTick[(decoder-1) * 8 + port] = SystemOp.getTick();
  }

  /*
    Byte no:  VALID message:  IDLE message:
    0   Sent as 0x01 by the originating decoder, incremented with 2 by each forwarding decoder.
          IDLE: Sent as 0x00 by the originating decoder, incremented with 2 by each forwarding decoder.
    1   IR detector port number on the decoder (0-7).   IDLE: 0xff
    2   Received code from the loco.                    IDLE: 0xff
   */

   while( data->run ) {
    int bAvail = SerialOp.available(data->serial);
    if (bAvail < 0) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error; exit reader." );
      break;
    }

    while (bAvail > 0) {
      byte c;
      SerialOp.read( data->serial, &c, 1 );

      if( insync ) {
        packet[idx] = c;
        idx++;
        if( idx == 3 ) {
          idx = 0;
          TraceOp.dump( NULL, TRCLEVEL_BYTE, packet, 3 );
          if( packet[0] % 2 != 0 ) {
            /* data packet */
            int decoder = packet[0] / 2 + 1;
            int port    = packet[1] + 1;
            int loco    = packet[2];
            char ident[32];

            iONode evt = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
            wFeedback.setaddr( evt, (decoder-1) * 8 + port );
            wFeedback.setbus( evt, wFeedback.fbtype_lissy );
            StrOp.fmtb(ident, "%d", loco);
            wFeedback.setidentifier( evt, ident );
            wFeedback.setstate( evt, wFeedback.getidentifier(evt) > 0 ? True:False );
            if( data->iid != NULL )
              wFeedback.setiid( evt, data->iid );

            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "detecoder [%d.%d][%d] reported loco address [%d]",
                decoder, port, wFeedback.getaddr(evt), wFeedback.getidentifier(evt) );

            data->listenerFun( data->listenerObj, evt, TRCLEVEL_INFO );
            data->readerTick[(decoder-1) * 8 + port] = SystemOp.getTick();

          }
          else {
            /* idle packet */
          }
        }
      }
      else if( c == 0xff && prev == 0xff ) {
        insync = True;
      }
      prev = c;

      bAvail = SerialOp.available(data->serial);

      if (bAvail < 0) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error; exit reader." );
        break;
      }
    }
  }

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PerIR reader ended." );
}



static Boolean __flush( iOPerIRData data ) {
  /* Read all pending information on serial port. Interface Hickups if data is pending from previous init! */
  int bAvail = SerialOp.available(data->serial);
  if( bAvail > 0 && bAvail < 1000 ) {
    char c;
    TraceOp.trc(name, TRCLEVEL_WARNING, __LINE__, 9999, "Flushing %d bytes...", bAvail);
    while( SerialOp.available(data->serial) > 0 ) {
      SerialOp.read( data->serial, &c, 1 );
    };
  }
  else if(bAvail >= 1000) {
    TraceOp.trc(name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Can not flush %d bytes, check your hardware!", bAvail);
    return False;
  }
  else {
    TraceOp.trc(name, TRCLEVEL_DEBUG, __LINE__, 9999, "flushed");
  }
  return True;
}




/**  */
static struct OPerIR* _inst( const iONode ini ,const iOTrace trc ) {
  iOPerIR __PerIR = allocMem( sizeof( struct OPerIR ) );
  iOPerIRData data = allocMem( sizeof( struct OPerIRData ) );
  MemOp.basecpy( __PerIR, &PerIROp, 0, sizeof( struct OPerIR ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  SystemOp.inst();

  /* Initialize data->xxx members... */
  data->device   = StrOp.dup( wDigInt.getdevice( ini ) );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );
  data->fboffset = wDigInt.getfboffset( ini );

  MemOp.set( data->readerTick, 0, sizeof(data->readerTick) );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "PerIR %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid       = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device    = %s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "baudrate  = %d", 9600 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "handshake = %s", wDigInt.getflow(ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fboffset  = %d", data->fboffset );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, StrOp.equals( wDigInt.cts, wDigInt.getflow( data->ini ) ) ? cts:none );
  SerialOp.setLine( data->serial, 9600, 8, 2, none, wDigInt.isrtsdisabled( ini ) );
  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {
    char* thname = NULL;
    data->run = True;
    /* empty UART receive buffer */
    __flush(data);

    thname = StrOp.fmt("perirread%X", __PerIR);
    data->reader = ThreadOp.inst( thname, &__PerIRReader, __PerIR );
    StrOp.free(thname),
    ThreadOp.start( data->reader );

    thname = StrOp.fmt("perirtick%X", __PerIR);
    data->ticker = ThreadOp.inst( thname, &__irTicker, __PerIR );
    StrOp.free(thname),
    ThreadOp.start( data->ticker );
  }
  else
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init rclink port!" );

  instCnt++;
  return __PerIR;
}

iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/perir.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
