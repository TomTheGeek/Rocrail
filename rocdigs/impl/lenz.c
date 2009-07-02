
/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocdigs/impl/lenz_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
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
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/ThrottleCmd.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/BinCmd.h"
#include "rocrail/wrapper/public/Clock.h"

#include "rocdigs/impl/common/fada.h"

#include <time.h>

static int instCnt = 0;
static int interfaceVersion = 0;

static Boolean expectProgResult = False;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOLenzData data = Data(inst);
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

/** ----- OLenz ----- */


/**  */


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOLenzData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _supportPT( obj inst ) {
  /* ProgrammingTrack (ServiceMode) */
  return True;
}

static void __setLocAddr( int addr, byte* addrbytes ) {
  if( addr < 100 ) {
    addrbytes[0] = 0;
    addrbytes[1] = addr & 0x00FF;
  }
  else {
    addrbytes[0] = ((addr & 0xFF00) >> 8) + 0xC0;
    addrbytes[1] = addr & 0x00FF;
  }
}

static int __getLocAddr( byte* addrbytes ) {
  int addr = addrbytes[1];
  if( (addrbytes[0] & 0xC0) == 0xC0 ) {
    int addrH = (addrbytes[0] & ~0xC0);
    addr = addr + (addrH << 8);
  }
  return addr;
}

static void __handleSwitch(iOLenz lenz, int addr, int port, int value) {
  iOLenzData data = Data(lenz);
  iOLenzData dataw = Data(lenz);

  int valuew = value;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sw %d %d = %s", addr+1, port, value?"straight":"thrown" );

  {
    iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );

    wSwitch.setaddr1( nodeC, ( addr+1  ));
    wSwitch.setport1( nodeC, ( port  ));

    if( data->iid != NULL )
      wSwitch.setiid( nodeC, data->iid );

    wSwitch.setstate( nodeC, value?"straight":"turnout" );

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }


  {
    iONode nodeC = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );

    wOutput.setaddr( nodeC, ( addr+1  ));
    wOutput.setport( nodeC, ( port  ));
    wOutput.setgate( nodeC, 0);

    if( data->iid != NULL )
      wOutput.setiid( nodeC, data->iid );

    wOutput.setstate( nodeC, value?"on":"off" );

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }

}

/* extract the bits */
static void __dec2bin(int *b0, int num) {
  int i, rest;
  for (i =0; i < 8; i++){
    rest = num % 2;
    num = num / 2;
    b0[7-i] = rest;
  }
}

static void __evaluateResponse( iOLenz lenz, byte* in, int datalen ) {
  iOLenzData data = Data(lenz);

  int i0 = in[0];
  int i1 = in[1];
  int i2 = in[2];
  int i3 = in[3];

  int b0[8], b1[8], b2[8], b3[8];

  if( i0 == 0x05 && i1 == 0x01 ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "clock response" );
    return;
  }

  __dec2bin( &b0[0], i0);
  __dec2bin( &b1[0], i1);
  __dec2bin( &b2[0], i2);
  __dec2bin( &b3[0], i3);

  /* Turnout broadcast: */
  if ( i0 == 0x42 && i1 <= 0x80 && (b2[1] == 0 && b2[2] == 0) || (b2[1] == 0 && b2[2] == 1)) {
    int baseadress = i1;
    int k, start;

    if( b2[3] == 0 )
      start = 1;
    else
      start = 3;

    for (k = 0; k < 2; k++) {
      if( (b2[7-k*2] + b2[6-k*2]) == 1 ) {       // only handle changed turnouts ignore those unchanged (00) or invalid (11)
        __handleSwitch(lenz, baseadress, start+k, b2[7-k*2]);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Lenz turnout status change address %d port %d", baseadress+1, start+k );
      } else {
        if( (b2[7-k*2] + b2[6-k*2]) == 2 )       // turnout reported invalid position
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Lenz turnout reports invalid position address %d port %d", baseadress+1, start+k );
        else                                     // turnout not yet operated since power on
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Lenz turnout not operated yet address %d port %d", baseadress+1, start+k );
      }
    }
  }

  /* Feedback broadcast: */
  if( (in[0] & 0xF0) == 0x40 && (b2[1] == 1 && b2[2] == 0) ) {
    int datalen = in[0] & 0x0F;
    int i = 0;
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%d feedback bytes received", datalen );
    for( i = 0; i < datalen; i+=2 ) {
      int n = 0;
      int state = in[ i + 2 ] & 0x0f;
      int addr = in[ i+1 ] * 8;  /* 8 inputs per card */
      addr += ( in[ i + 2 ] & 0x10 ) ? 4:0; /* first or second nibble? */
      for( n = 0; n < 4; n++ ) {
        byte mask = 0x01 << n;
        Boolean bState = state & mask ? True:False;
        int iAddr = addr + n;

        /* evaluated in the debounce code */
        data->fbPreState[iAddr] = bState;
      }
    }
  }

  /* Throttle Broadcast */
  if( (in[0] & 0xF0) == 0x30) {
    iONode node = NodeOp.inst( wThrottleCmd.name(), NULL, ELEMENT_NODE );

    /* TODO: translate the lenz packet in a global command format
     *  with predefined wThrottleCmd attributes.
     */

    NodeOp.setInt( node, "slot", in[1] );
    NodeOp.setInt( node, "type", in[2] );
    NodeOp.setInt( node, "key", in[3] );
    NodeOp.setInt( node, "val", in[4] );

    /* fill the node with data... */
    if( data->listenerFun != NULL && data->listenerObj != NULL )
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }

  /* SM response Direct CV mode: */
  if( in[0] == 0x63 && in[1] == 0x14 ) {
    int cv = in[2];
    int value = in[3];
    iONode node = NULL;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cv %d has a value of %d", cv, value );

    node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setcv( node, cv );
    wProgram.setvalue( node, value );
    wProgram.setcmd( node, wProgram.datarsp );
    if( data->iid != NULL )
      wProgram.setiid( node, data->iid );

    if( data->listenerFun != NULL && data->listenerObj != NULL )
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }

}


static Boolean __sendRequest( iOLenz lenz, byte* outin ) {
  iOLenzData data = Data(lenz);
  int len = 0;
  int i = 0;
  Boolean rc = True;
  byte bXor = 0;
  unsigned char out[256];

  for( i = 0; i < 256; i++)
    out[i] = outin[i];

  len = out[0] & 0x0f;
  len++; /* header */

  if( out[0] == 0x00 ) {
    return False;
  }

  for ( i = 0; i < len; i++ ) {
    bXor ^= out[i];
  }
  out[i] = bXor;
  len++; /* checksum */

  /* make extra header for LI-USB*/
  if ( data->usb) {
    for( i = 0; i < len; i++)
      out[i+2] = outin[i];

    len = len+2;
    out[0] = 0xFF;
    out[1] = 0xFE;

    out[len-1] = bXor;
  }

  if( MutexOp.wait( data->mux ) ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "out buffer" );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
    if( !data->dummyio ) {
      rc = SerialOp.write( data->serial, (char*)out, len );
    }

    MutexOp.post( data->mux );
  }

  return rc;
}

/* Maybe obsolete*/
static void __statusRequestSender( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOLenz lenz = (iOLenz)ThreadOp.getParm( th );
  iOLenzData data = Data(lenz);
  int i;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "statusRequestSender started." );

  unsigned char out[256];

  out[0] = 0x21;
  out[1] = 0x24;
  out[2] = 0x05;

  do {
    ThreadOp.sleep( 1000 );
    __sendRequest( lenz, out );
  } while( data->run );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "statusRequestSender ended." );

}

static void __initializer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOLenz lenz = (iOLenz)ThreadOp.getParm( th );
  iOLenzData data = Data(lenz);
  int i;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Initializer started.");

  /* XpressNet
     Asking for Interface version*/
  byte* outa = allocMem(256);
  outa[0] = 0xF0;
  ThreadOp.post( data->transactor, (obj)outa );

  /* Asking for CS version */
  byte* outb = allocMem(256);
  outb[0] = 0x21;
  outb[1] = 0x21;
  outb[2] = 0x00;
  ThreadOp.post( data->transactor, (obj)outb );



  if( data->startpwstate) {
    /* ALL ON */
    byte* outd = allocMem(256);
    outd[0] = 0x21;
    outd[1] = 0x81;
    outd[2] = 0xA0;
    ThreadOp.post( data->transactor, (obj)outd );
  } else {
    /* ALL OFF*/
    byte* outc = allocMem(256);
    outc[0] = 0x21;
    outc[1] = 0x80;
    outc[2] = 0xA1;
    ThreadOp.post( data->transactor, (obj)outc );
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Initializer ended." );

}

static iONode __translate_bin( iOLenz lenz, iONode nodeA ) {
  iOLenzData data = Data(lenz);
  iONode nodeB = NULL;

  data->bincmd = True;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bin command" );

  int outLen = wBinCmd.getoutlen(nodeA);
  byte* outBytes = StrOp.strToByte( wBinCmd.getout(nodeA));

  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "bin command out I" );
  TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)outBytes, outLen );

  if( outBytes[0] == 0x78 ) {
    /* Flip around the address bytes*/
    byte tmp = outBytes[2];
    outBytes[2] = outBytes[3];
    outBytes[3] = tmp;

    /* READ */
    if( outBytes[1] == 0xA4) {
      outBytes[0] = 0x24;
      outBytes[1] = 0x28;
      __sendRequest( lenz, outBytes );
    }

    /* WRITE */
    else if( outBytes[1] == 0xA3) {
      outBytes[0] = 0x24;
      outBytes[1] = 0x29;
      __sendRequest( lenz, outBytes );
    }
  }
  else {
    __sendRequest( lenz, outBytes );
  }

  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "bin command out II" );
  TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)outBytes, outLen );

  freeMem(outBytes);

  int datalen;
  byte in[256];

  /* deny the transactor to read we want the answer! */
  Boolean wait = True, ok = False;
  int timeout = 100;

  while( wait && (timeout > 0)) {
    timeout--;

    if ( !data->dummyio && SerialOp.available(data->serial) ) {
      if( MutexOp.wait( data->mux ) ) {

        if( !SerialOp.read( data->serial, (char*) in, 1 ) ) {
          MutexOp.post( data->mux );
        }

        datalen = (in[0] & 0x0f);
        SerialOp.read( data->serial, (char*)in+1, datalen+1);
        MutexOp.post( data->mux );

        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "bin command in %d", datalen );
        TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, datalen + 2 );

        /* SO Answer */
        if( in[0] == 0x24 && in[1] == 0x28) {
          in[0] = 0x00;
          in[1] = in[4];
          ok = True;
        }

        /* SO Error */
        else if( in[0] == 0x61 && in[1] == 0x82) {
          in[0] = 0x01;
          in[1] = 0x01;
          ok = True;
        } else {
          in[0] = 0x01;
        }
        wait = False;
      }
    }
    ThreadOp.sleep(10);
  }

  /* allow the transactor to read again */
  data->bincmd = False;

  if (ok) {
    char* s = StrOp.byteToStr( in, 2 );
    nodeB = NodeOp.inst( NodeOp.getName( nodeA ), NULL, ELEMENT_NODE );
    wResponse.setdata( nodeB, s );
    StrOp.free(s);
    return nodeB;
  }

  /* is NULL */
  return NULL;
}

/**
 * @param node <sw unit="1" pin="1" cmd="straight"/>
 * @param node <lc addr="1" speed="0" fn="false" sw="false"/>
 * @param node <fn addr="1" f1="false" f2="false" f3="false" f4="false"/>
 * @param node <fb unit="1"/>
 * @param node <sys cmd="stop"/>
 * @param out Request buffer.
 * @return Request size.
 */
static iONode __translate( iOLenz lenz, iONode node ) {
  iOLenzData data = Data(lenz);
  iONode rsp = NULL;

  /* Clock command. */
  if( StrOp.equals( NodeOp.getName( node ), wClock.name() ) ) {
    /*
    from Slave to Command Station / and from Command Station to Slave:

    0x00 0x01 TCODE0 {TCODE1 TCODE2 TCODE3} Timecode transfer, accelerated layout time.
    A TCODE consists of one byte, coded binary as CCDDDDDD, where CC denotes the type of
    code and DDDDDDD the corresponding data.
    TCODE Content
    CC=00 DDDDDD = mmmmmm, this denotes the minute, range 0..59.
    CC=10 DDDDDD = 0HHHHHH, this denotes the hour, range 0..23
    CC=01 DDDDDD = 000WWW, this denotes the day of week,
    0=Monday, 1=Tuesday, 2=Wednesday, 3=Thursday, 4=Friday, 5=Saturday, 6=Sunday.
    CC=11 DDDDDD = 00FFFFF, this denotes the acceleration factor, range 0..31;
    an acceleration factor of 0 means clock is stopped, a factor of 1 means clock is running
    real time, a factor of 2 means clock is running twice as fast a real time.
    This message is issued as broadcast once every (layout-) minute. The command is not repeated.

    When no Parameters are given, it is a query and the answer will be sent only to the requesting slave.
     */
    if( data->fastclock ) {
      long l_time = wClock.gettime(node);
      struct tm* lTime = localtime( &l_time );

      int mins    = lTime->tm_min;
      int hours   = lTime->tm_hour;
      int wday    = lTime->tm_wday;
      int divider = wClock.getdivider(node);
      byte* outa  = NULL;

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set clock to %02d:%02d divider=%d", hours, mins, divider );

      outa = allocMem(8);
      outa[0] = 0x05;
      outa[1] = 0xF1;
      outa[2] = 0x00 + mins;
      outa[3] = 0x80 + hours;
      outa[4] = 0x40 + wday;
      outa[5] = 0xC0 + divider;
      ThreadOp.post( data->transactor, (obj)outa );
    }
  }


  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {

    int addr = wSwitch.getaddr1( node );
    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );

    int state = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 0x01:0x00;

    if( port == 0 ) {
      fromFADA( addr, &addr, &port, &gate );
    }
    else if( addr == 0 && port > 0 ) {
      fromPADA( port, &addr, &port );
    }

    /* when sending to elite we have to correct for elite (version 1.3) addressing fault
       address 1, port 1 does not exist in elite, address 1 port 2 becomes decoder 1 port 1,
       address 1 port 3 becomes decoder 1 port 2, address 2 port 1 becomes decoder 1 port 4
     */
    if (data->elite) {
      port++;
      if (port > 4) {
        port =1;
        addr++;
      }
    }

    /* Rocrail starts at address 1, port 1, Lenz at address 0, port 0 */
    if( port > 0 ) port--;
    if( addr > 0 ) addr--;


    if( wSwitch.issinglegate( node ) ) {

      /* make message:*/
      /* activate the gate not to be used */
      byte* outa = allocMem(256);
      outa[0] = 0x52;
      outa[1] = addr;
      outa[2] = 0x80 | 0x08 | (port << 1) | gate;
      ThreadOp.post( data->transactor, (obj)outa );

      /* deactivate the gate to be used */
      byte* outb = allocMem(256);
      outb[0] = 0x52;
      outb[1] = addr;
      outb[2] = 0x80 | 0x00 | (port << 1) | gate;
      ThreadOp.post( data->transactor, (obj)outb );


    } else {

      /* make message: */
      byte* outa = allocMem(256);
      outa[0] = 0x52;
      outa[1] = addr;
      outa[2] = 0x80 | 0x08 | (port << 1) | state;
      ThreadOp.post( data->transactor, (obj)outa );

      byte* outb = allocMem(256);
      outb[0] = 0x52;
      outb[1] = addr;
      outb[2] = 0x80 | 0x00 | (port << 1) | state;
      ThreadOp.post( data->transactor, (obj)outb );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "gate %d %d ",gate,state);
    }

    /* command has been sent, in case of elite undo the correction
       to get address, port and gate right again */
    if (data->elite) {
      port--;
      if (port < 0) {
        port = 3;
        addr--;
      }
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "turnout %d %d %s",
        addr+1, port+1, wSwitch.getcmd( node ) );
    ThreadOp.sleep( data->swtime );

  }
  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int action1 = 0x00;
    int action2 = 0x00;

    int addr   = wOutput.getaddr( node );
    int port   = wOutput.getport( node );
    int gate   = wOutput.getgate( node );

    int state = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 0x01:0x00;

    if( port == 0 )
      fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      fromPADA( port, &addr, &port );

    if (data->elite) {
      port++;
      if (port > 4) {
        port =1;
        addr++;
      }
    }

    if( port > 0 ) port--;
    if( addr > 0 ) addr--;

    if (StrOp.equals( wOutput.getcmd( node ), wOutput.on )){

      byte* outa = allocMem(256);
      outa[0] = 0x52;
      outa[1] = addr;
      outa[2] = 0x80 | 0x08 | (port << 1) | gate;
      ThreadOp.post( data->transactor, (obj)outa );

      byte* outb = allocMem(256);
      outb[0] = 0x52;
      outb[1] = addr;
      outb[2] = 0x80 | 0x00 | (port << 1) | gate;
      ThreadOp.post( data->transactor, (obj)outb );
    }

    if (data->elite) {
      port--;
      if (port < 0) {
        port = 3;
        addr--;
      }
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d %d %d %s",
        addr, port, gate, wOutput.getcmd( node ) );
  }
  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
  }

  /* Sensor command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int addr = wFeedback.getaddr( node );
    Boolean state = wFeedback.isstate( node );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    rsp = (iONode)NodeOp.base.clone( node );
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    int  lenzspeed = 0;
    Boolean fn = wLoc.isfn( node );
    int    dir = wLoc.isdir( node );
    int  spcnt = wLoc.getspcnt( node );

    int reqid = 0x10; /* default 14 speed steps */
    switch( spcnt ) {
    case 27:
      reqid = 0x11;
      break;
    case 28:
      reqid = 0x12;
      break;
    case 127:
    case 128:
      reqid = 0x13;
      spcnt = 127;
      break;
    default:
      reqid = 0x10;
      spcnt = 14;
      break;
    }
    /*
      General Lenz speed values

      14
      Step 0 Stop
      Step 1 E-Stop
      Step 2-15 Train in motion

      27
      Step 0 Stop
      Step 1 Not used
      Step 2 E-Stop
      Step 3 Not used
      Step 4-30 Train in motion
      Step 31 Not used

      28
      Step 0 Stop
      Step 1 Not used
      Step 2 E-Stop
      Step 3 Not used
      Step 4-31 Train in motion

      128
      Step 0 Stop
      Step 1 E-Stop
      Step 2-127 Train in motion
     */

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * spcnt) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * spcnt) / wLoc.getV_max( node );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "speed=%d", speed );
    }

    lenzspeed = speed;

    /* Remove e-stop in 14 and 127 speed step mode */
    if ( spcnt == 14 || spcnt == 127 ){
      if ( lenzspeed > 0 )
        lenzspeed = lenzspeed + 1;
      /* prevent bit overflow at max speed */
      if ( lenzspeed > spcnt)
        lenzspeed = spcnt;
    }

    /* Adjust bit positions for lenz command stations */
    if (( spcnt == 27 ) || ( spcnt == 28 )){

      /* Remove 2 unused speed step and e-stop speed step */
      if ( lenzspeed > 0 )
        lenzspeed = lenzspeed + 3;

      /* Transfer LSB to front of bit 0-3 */
      if ( lenzspeed & 0x01 )
        lenzspeed = lenzspeed | 0x20;

      /* Move all bit in right position */
      lenzspeed = lenzspeed >> 1;
    }


    if( (data->lcfn[addr] & 0x10) != (fn?0x10:0) ) {
      byte* outa = allocMem(256);
      outa[0] = 0xE4;
      outa[1] = 0x20;
      __setLocAddr( addr, outa+2 );
      outa[4] = (data->lcfn[addr]&0x0F) + (fn?0x10:0);

      int test[8];
      __dec2bin( &test[0], outa[4]);
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Lights: %d %d %d %d %d %d %d %d",
          test[0], test[1], test[2], test[3], test[4], test[5], test[6], test[7]);


      data->lcfn[addr] = outa[4];
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "function group 1 (Lights ON)" );
      ThreadOp.post( data->transactor, (obj)outa );
    }

    byte* outb = allocMem(256);
    outb[0] = 0xE4;
    outb[1] = reqid;
    __setLocAddr( addr, outb+2 );
    outb[4] = dir ? 0x80:0x00;
    outb[4] |= lenzspeed;
    ThreadOp.post( data->transactor, (obj)outb );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d velocity=%d direction=%s", addr, speed, (dir?"fwd":"rev") );
  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int addr  = wFunCmd.getaddr( node );
    int group = wFunCmd.getgroup( node );

    Boolean f0 = wFunCmd.isf0( node );
    Boolean f1 = wFunCmd.isf1( node );
    Boolean f2 = wFunCmd.isf2( node );
    Boolean f3 = wFunCmd.isf3( node );
    Boolean f4 = wFunCmd.isf4( node );
    Boolean f5 = wFunCmd.isf5( node );
    Boolean f6 = wFunCmd.isf6( node );
    Boolean f7 = wFunCmd.isf7( node );
    Boolean f8 = wFunCmd.isf8( node );
    Boolean f9  = wFunCmd.isf9 ( node );
    Boolean f10 = wFunCmd.isf10( node );
    Boolean f11 = wFunCmd.isf11( node );
    Boolean f12 = wFunCmd.isf12( node );
    Boolean f13 = wFunCmd.isf13( node );
    Boolean f14 = wFunCmd.isf14( node );
    Boolean f15 = wFunCmd.isf15( node );
    Boolean f16 = wFunCmd.isf16( node );
    Boolean f17 = wFunCmd.isf17( node );
    Boolean f18 = wFunCmd.isf18( node );
    Boolean f19 = wFunCmd.isf19( node );
    Boolean f20 = wFunCmd.isf20( node );
    Boolean f21 = wFunCmd.isf21( node );
    Boolean f22 = wFunCmd.isf22( node );
    Boolean f23 = wFunCmd.isf23( node );
    Boolean f24 = wFunCmd.isf24( node );
    Boolean f25 = wFunCmd.isf25( node );
    Boolean f26 = wFunCmd.isf26( node );
    Boolean f27 = wFunCmd.isf27( node );
    Boolean f28 = wFunCmd.isf28( node );

    byte functions1 = (f1 ?0x01:0) + (f2 ?0x02:0) + (f3 ?0x04:0) + (f4 ?0x08:0) + (f0 ?0x10:0);
    byte functions2 = (f5 ?0x01:0) + (f6 ?0x02:0) + (f7 ?0x04:0) + (f8 ?0x08:0);
    byte functions3 = (f9 ?0x01:0) + (f10?0x02:0) + (f11?0x04:0) + (f12?0x08:0);
    byte functions4 = (f13?0x01:0) + (f14?0x02:0) + (f15?0x04:0) + (f16?0x08:0) + (f17?0x10:0) + (f18?0x20:0) + (f19?0x40:0) + (f20?0x80:0);
    byte functions5 = (f21?0x01:0) + (f22?0x02:0) + (f23?0x04:0) + (f24?0x08:0) + (f25?0x10:0) + (f26?0x20:0) + (f27?0x40:0) + (f28?0x80:0);

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "function %d light=%s f1=%s f2=%s f3=%s f4=%s f5=%s f6=%s f7=%s f8=%s f9=%s f10=%s f11=%s f12=%s",
        addr, (f0?"ON":"OFF"), (f1?"ON":"OFF"), (f2?"ON":"OFF"), (f3?"ON":"OFF"), (f4?"ON":"OFF"),
        (f5?"ON":"OFF"), (f6?"ON":"OFF"), (f7?"ON":"OFF"), (f8?"ON":"OFF"),
        (f9?"ON":"OFF"), (f10?"ON":"OFF"), (f11?"ON":"OFF"), (f12?"ON":"OFF") );

    if( group == 0 || group == 1 ) {
      byte* outa = allocMem(256);
      outa[0] = 0xE4;
      outa[1] = 0x20;
      __setLocAddr( addr, outa+2 );
      outa[4] = functions1;
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "function group 1" );
      ThreadOp.post( data->transactor, (obj)outa );
    }

    if( group == 0 || group == 2 ) {
      byte* outb = allocMem(256);
      outb[0] = 0xE4;
      outb[1] = 0x21;
      __setLocAddr( addr, outb+2 );
      outb[4] = functions2;
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "function group 2" );
      ThreadOp.post( data->transactor, (obj)outb );
    }


    if( group == 0 || group == 3 ) {
      byte* outc = allocMem(256);
      outc[0] = 0xE4;
      outc[1] = 0x22;
      __setLocAddr( addr, outc+2 );
      outc[4] = functions3;
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "function group 3" );
      ThreadOp.post( data->transactor, (obj)outc );
    }

    if( group == 0 || group == 4 || group == 5 ) {
      byte* outc = allocMem(256);
      outc[0] = 0xE4;
      outc[1] = 0x23;
      __setLocAddr( addr, outc+2 );
      outc[4] = functions4;
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "function group 4" );
      ThreadOp.post( data->transactor, (obj)outc );
    }

    if( group == 0 || group == 6 || group == 7 ) {
      byte* outc = allocMem(256);
      outc[0] = 0xE4;
      outc[1] = 0x28;
      __setLocAddr( addr, outc+2 );
      outc[4] = functions5;
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "function group 5" );
      ThreadOp.post( data->transactor, (obj)outc );
    }

    /* save the function1 byte to use for setting the lights function... */
    data->lcfn[addr] = functions1;

  }
  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    byte* outa = allocMem(256);
    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      outa[0] = 0x21;
      outa[1] = 0x80;
      outa[2] = 0xA1;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      outa[0] = 0x21;
      outa[1] = 0x81;
      outa[2] = 0xA0;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
    }

    ThreadOp.post( data->transactor, (obj)outa );

  }
  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {

    if( wProgram.getcmd( node ) == wProgram.get ) {
      int cv = wProgram.getcv( node );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get CV%d...", cv );

      byte* outa = allocMem(256);
      outa[0] = 0x22;
      outa[1] = 0x15;
      outa[2] = cv & 0xFF;
      ThreadOp.post( data->transactor, (obj)outa );

      byte* outb = allocMem(256);
      outb[0] = 0x21;
      outb[1] = 0x10;
      outb[2] = 0x31;
      ThreadOp.post( data->transactor, (obj)outb );


      /* if no interfaceVersion is set it might be a LI100
       * put off PT after each read. Elite on/off goes automatic */
      if ( interfaceVersion == 0 && !data->elite) {
        byte* outc = allocMem(256);
        outc[0] = 0x21;
        outc[1] = 0x81;
        outc[2] = 0xA0;
        ThreadOp.post( data->transactor, (obj)outc );
      }

    }
    else if( wProgram.getcmd( node ) == wProgram.set ) {
      int cv = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int decaddr = wProgram.getdecaddr( node );


      if( wProgram.ispom(node) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "POM: set CV%d of loc %d to %d...", cv, decaddr, value );

        if (cv > 0) cv--;

        byte* outb = allocMem(256);
        outb[0] = 0xE6;
        outb[1] = 0x30;
        __setLocAddr( decaddr, outb+2 );
        outb[4] = ((cv & 0xFF00) >> 8) + 0xEC;
        outb[5] = cv & 0x00FF;
        outb[6] = value & 0xFF;

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "POM: 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X",
            outb[0], outb[1], outb[2], outb[3], outb[4], outb[5], outb[6]);

        if ( cv != 0 )
          ThreadOp.post( data->transactor, (obj)outb );
        else
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "POM does not allow writing of adress!");

      } else {

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set CV%d to %d...", cv, value );

        expectProgResult = True;

        byte* outa = allocMem(256);
        outa[0] = 0x23;
        outa[1] = 0x16;
        outa[2] = cv & 0xFF;
        outa[3] = value & 0xFF;
        ThreadOp.post( data->transactor, (obj)outa );

        byte* outb = allocMem(256);
        outb[0] = 0x21;
        outb[1] = 0x10;
        outb[2] = 0x31;
        ThreadOp.post( data->transactor, (obj)outb );

        /* if no interfaceVersion is set it might be a LI100
         * put off PT after each write. Elite goes on/off automatic */
        if ( interfaceVersion == 0 && !data->elite) {
          byte* outc = allocMem(256);
          outc[0] = 0x21;
          outc[1] = 0x81;
          outc[2] = 0xA0;
          ThreadOp.post( data->transactor, (obj)outc );
        }

      }

    }
    else if(  wProgram.getcmd( node ) == wProgram.pton ) {
      /* CS will go ton Pt on on first programming request */
    }  /* PT off, send: All ON" */
    else if( wProgram.getcmd( node ) == wProgram.ptoff ) {
      byte* outb = allocMem(256);
      outb[0] = 0x21;
      outb[1] = 0x81;
      outb[2] = 0xA0;
      ThreadOp.post( data->transactor, (obj)outb );

      expectProgResult = False;
    }

  }
  return rsp;
}

static void __transactor( void* threadinst ) {
  iOThread       th = (iOThread)threadinst;
  iOLenz         lenz = (iOLenz)ThreadOp.getParm(th);
  iOLenzData     data = Data(lenz);

  unsigned char out[256];

  ThreadOp.setDescription( th, "Transactor for Lenz" );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor started." );

  ThreadOp.setHigh( th );

  out[0] = 0;

  byte in[256];
  int datalen = 0;
  int i = 0;
  int retry = 0;
  byte bXor = 0;

  obj post = NULL;

  Boolean responceRecieved = True,
  waitForAnswer = False,
  expectEliteAnswer = True;


  int timeoutval = 100;
  int timeout = timeoutval;

  unsigned char* outc = NULL;

  Boolean ok = False;
  byte* outy;
  int numtries = 5;

  do {
    /* get next command only if the last command was successfull,
       otherwise work on the current node until the cs will answer, or give up after numtries */
    if (responceRecieved) {
      post = ThreadOp.getPost( th );
      numtries = 5;
      if (post != NULL) {
        outy = (byte*) post;
        for (i = 0; i < 256; i++)
          out[i] = (unsigned char) outy[i];
        freeMem( post);
      }
      if (post != NULL) {
        responceRecieved = !__sendRequest( lenz, out );
        waitForAnswer = True;

        timeout = timeoutval;

        expectEliteAnswer = True;

        /* special treatment for the elite and LI-USB*/
        /* TODO: this is not state of art ... we have to do something here !!!*/
        if (data->elite || data->usb) {
          if ( out[0] == 0x22 && (out[1] == 0x11 || out[1] == 0x14 || out[1] == 0x15)) {
            expectEliteAnswer = False;
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "... reading cv %d", out[2] );
            if(data->elite)
              ThreadOp.sleep(9000);
            if(data->usb)
              ThreadOp.sleep(1000);
          }
          if (out[0] == 0x23 && (out[1] == 0x12 || out[1] == 0x16 || out[1] == 0x17)) {
            expectEliteAnswer = False;
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "... writing cv %d with value %d", out[2], out[3]);
            if(data->elite)
              ThreadOp.sleep(9000);
            if(data->usb)
              ThreadOp.sleep(1000);
          }
          if (out[0] == 0x21 && (out[1] == 0x80 || out[1] == 0x81)) {
            expectEliteAnswer = False;
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "No response expected" );
          }
        }


      }
    } else {
      if( post != NULL && numtries > 0 && expectEliteAnswer) {
        /* send again */
        __sendRequest( lenz, out );

        waitForAnswer = True;
        numtries--;

      } else {
        responceRecieved = True;
        waitForAnswer = False;

        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Command not confirmed!" );
      }

    }


    /* Give up timeslice:*/
    ThreadOp.sleep( 50 );


    Boolean dataAvailable = SerialOp.available(data->serial);

    // Wait or timeout
    while(  !(timeout != 0 || !dataAvailable) ) {
      if( timeout > 0)
        timeout --;
      ThreadOp.sleep( 25 );
    }

    if ( !data->bincmd && !data->dummyio && dataAvailable) {
      if( MutexOp.wait( data->mux ) ) {

        dataAvailable = False;

        if ( data->usb) {

          if( !SerialOp.read( data->serial, (char*) in, 3 ) ) {
            MutexOp.post( data->mux );
            continue;
          }

          datalen = (in[2] & 0x0f) + 1;

          ok = SerialOp.read( data->serial, (char*)in+3, datalen );
          MutexOp.post( data->mux );

          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "in buffer" );
          TraceOp.dump( NULL, TRCLEVEL_DEBUG, (char*)in, 10 );

          /* remove extra header from LI-USB */
          for (i = 0; i < 254; i++)
            in[i] = in[i+2];

          TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, in[0]+2 );

        } else {

          if( !SerialOp.read( data->serial, (char*) in, 2 ) ) {
            MutexOp.post( data->mux );
            continue;
          }

          datalen = (in[0] & 0x0f) + 1;
          ok = SerialOp.read( data->serial, (char*)in+2, datalen-1 );
          MutexOp.post( data->mux );


          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "in buffer" );
          TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, datalen+2 );
        }

        if( !ok )
          continue;

        waitForAnswer = False;

        bXor = 0;
        for( i = 0; i < datalen; i++ ) {
          bXor ^= in[ i ];
        }

        if( bXor != in[datalen])
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Xor bytes are not equal!" );

        /* Evaluate XprerssNet Answers
         check if last command was recieved, the cs answers: 1 4 5 */
        if( in[0] == 1 && in[1] == 4 && in[2] == 5 ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Command OK");
          responceRecieved = True;
        }
        /* Feedback */
        else if( in[0] == 0x42 ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Feedback OK");
          responceRecieved = True;
        }
        /* Feedback */
        else if( (in[0] >> 4) == 0x4 ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Feedback OK");
          responceRecieved = True;
        }
        /* Track Power OFF */
        else if( in[0] == 0x81 && in[1] == 0x00) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "STOP");

          iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
          if( data->iid != NULL )
            wState.setiid( node, data->iid );
          wState.setpower( node, False );
          wState.settrackbus( node, False );

          if( data->listenerFun != NULL && data->listenerObj != NULL )
            data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

          responceRecieved = True;
        }
        /* Track Power OFF */
        else if( in[0] == 0x61 && in[1] == 0x00) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Track power OFF");

          iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
          if( data->iid != NULL )
            wState.setiid( node, data->iid );
          wState.setpower( node, False );
          wState.settrackbus( node, False );
          wState.setsensorbus( node, False );
          wState.setaccessorybus( node, False );

          if( data->listenerFun != NULL && data->listenerObj != NULL )
            data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

          responceRecieved = True;
        }
        /* Normal operation resumed */
        else if( in[0] == 0x61 && in[1] == 0x01) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Normal operation resumed.");

          iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
          if( data->iid != NULL )
            wState.setiid( node, data->iid );
          wState.setpower( node, True );
          wState.settrackbus( node, True );
          wState.setsensorbus( node, True );
          wState.setaccessorybus( node, True );

          if( data->listenerFun != NULL && data->listenerObj != NULL )
            data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

          responceRecieved = True;
        }
        /* Prog Mode*/
        else if (in[0] == 0x61 && in[1] == 0x02){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Programming mode entered");
          responceRecieved = True;
          expectProgResult = True;
        }
        /* Prog Mode*/
        else if (in[0] == 0x61 && in[1] == 0x11){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Prog. Ready");
          responceRecieved = True;
        }
        /* transaction error*/
        else if (in[0] == 0x61 && in[1] == 0x80){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "transaction error ... try again");
          responceRecieved = True;
        }
        /* CS busy*/
        else if (in[0] == 0x61 && in[1] == 0x81){
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "cs busy ... try again");
        }
        /* PT busy*/
        else if (in[0] == 0x61 && in[1] == 0x1F){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "PT busy ... try again");
          expectProgResult = True;
        }
        /* Command not known*/
        else if (in[0] == 0x61 && in[1] == 0x82){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Command not known.");
          responceRecieved = True;
        }
        /* Shortcut*/
        else if (in[0] == 0x61 && in[1] == 0x12){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shortcut!");
          responceRecieved = True;
        }
        /* No data*/
        else if (in[0] == 0x61 && in[1] == 0x13){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "No Data");
          responceRecieved = True;
        }
        /* cv answer*/
        else if ((in[0] == 0x63 && in[1] == 0x10) || (in[0] == 0x63 && in[1] == 0x14)){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Programming answer recieved ...");
          responceRecieved = True;
          expectProgResult = False;
        }
        /* Version of Interface*/
        else if (in[0] == 0x02){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Interface version: %1.1f-%d",
              in[1]/10.0 , in[2] );
          responceRecieved = True;
          interfaceVersion = (int) in[1];
        }

        /* Version of Interface*/
        else if (in[0] == 0x01 && in[1] == 0x06){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LI-USB buffer overflow ...");
        }

        /* Version of Command Station from version 3.0*/
        else if (in[0] == 0x63 && in[1] == 0x21){
          char* csname = NULL;
          if( in[3] == 0x00 )
            csname = "LZ 100";
          else if( in[3] == 0x01 )
            csname = "LH 200";
          else if( in[3] == 0x02 )
            csname = "DPC";
          else if( in[3] == 0x03 )
            csname = "Control Plus";

          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Command Station: %s version: %1.0x.%1.0d",
              csname, (in[2] & 0xF0)/16 , (in[2] & 0x0F));
          responceRecieved = True;
        }
        /* SO */
        else if (in[0] == 0x78){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SO xx %d = %d",in[2], in[3]);
        }
        /* clock */
        else if (in[0] == 0x05){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "clock...");
          responceRecieved = True;
        }

        else {

          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Unknown command.");
          TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, 15);
        }

        /* anything will go to rocgui ...*/
        __evaluateResponse( lenz, in, datalen );

      }
    }

    /* Sensor Debounce */
    data->fbCounter--;
    if( data->fbCounter < 0) {
      data->fbCounter = data->sensordebounce;
      for( i=0; i<128*8; i++) {
        if( data->fbState[i] != data->fbPreState[i] ) {
          /* inform listener: Node3 */
          iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
          data->fbState[i] = data->fbPreState[i];
          wFeedback.setaddr( nodeC, i+data->fboffset );
          wFeedback.setstate( nodeC, data->fbPreState[i] );
          if( data->iid != NULL )
            wFeedback.setiid( nodeC, data->iid );

          if( data->listenerFun != NULL && data->listenerObj != NULL )
            data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
              "Sensor %d=%d", i + data->fboffset, data->fbPreState[i]);
        }
      }
    }

    /* Give up timeslice:*/
    ThreadOp.sleep( 1 );
  } while( data->run );

}

static iONode _cmd( obj inst ,const iONode nodeA ) {
  iOLenzData data = Data(inst);
  char out[256];
  iONode nodeB = NULL;

  if( nodeA != NULL ) {
    if( StrOp.equals( wThrottleCmd.name(), NodeOp.getName(nodeA) ) ) {

      /* TODO: throttle command processing */
      /*
      const char * str  = NodeOp.getStr(nodeA, "str",  0 );
      int msgsize = StrOp.len( str);
      int i;

      char* outc = allocMem(256);
      outc[0] = 0x34 + msgsize;
      outc[1] = NodeOp.getInt(nodeA, "slot", 0 );
      outc[2] = NodeOp.getInt(nodeA, "type", 0);
      outc[3] = NodeOp.getInt(nodeA, "xpos", 0 );
      outc[4] = NodeOp.getInt(nodeA, "ypos", 0 );

      for (i = 0; i < msgsize; i++) {
          outc[i + 5] = str[i];
      }

      TraceOp.trc(name, TRCLEVEL_DEBUG, __LINE__, 9999,
          "Repeat: %X %X %X %X", outc[0],outc[1],outc[2],outc[3] );

      ThreadOp.post(data->transactor, (obj) outc);
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Throttle Command sended.");
       */

      /* Bin Commands (opendcc)*/
    } else if (StrOp.equals( NodeOp.getName( nodeA ), wBinCmd.name() )) {
      nodeB = __translate_bin( (iOLenz)inst, nodeA );
    }
    else
      __translate( (iOLenz)inst, nodeA );


    /* Cleanup Node1 */
    nodeA->base.del(nodeA);
  }

  /* return Node2 */
  return nodeB;
}

/**  */
static void _halt( obj inst ) {
  iOLenzData data = Data(inst);
  iOLenz lenz = (iOLenz) inst;

  /* ALL OFF */
  byte* outc = allocMem(256);
  outc[0] = 0x21;
  outc[1] = 0x80;
  outc[2] = 0xA1;
  __sendRequest( lenz, outc );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", "Lenz" );

  data->run = False;
  SerialOp.close( data->serial );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  return;
}

/* Status */
static int _state( obj inst ) {
  iOLenzData data = Data(inst);
  int state = 0;
  return state;
}

/* external shortcut event */
static void _shortcut(obj inst) {
  iOLenzData data = Data( inst );
}


/* VERSION: */
static int vmajor = 1;
static int vminor = 3;
static int patch  = 0;
static int _version( obj inst ) {
  iOLenzData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}



/**  */
static struct OLenz* _inst( const iONode ini ,const iOTrace trc ) {
  iOLenz __Lenz = allocMem( sizeof( struct OLenz ) );
  iOLenzData data = allocMem( sizeof( struct OLenzData ) );
  MemOp.basecpy( __Lenz, &LenzOp, 0, sizeof( struct OLenz ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini           = ini;
  data->iid           = StrOp.dup( wDigInt.getiid( ini ) );
  data->elite         = StrOp.equals( wDigInt.sublib_lenz_elite, wDigInt.getsublib(ini) );
  data->usb           = StrOp.equals( wDigInt.sublib_usb, wDigInt.getsublib(ini) );
  data->mux           = MutexOp.inst( StrOp.fmt( "serialMux%08X", data ), True );
  data->swtime        = wDigInt.getswtime( ini );
  data->dummyio       = wDigInt.isdummyio( ini );
  data->fbCounter     = 0;
  data->fboffset      = wDigInt.getfboffset( ini );
  data->serial        = SerialOp.inst( wDigInt.getdevice( ini ) );
  data->startpwstate  = wDigInt.isstartpwstate( ini );
  data->bincmd        = False;
  data->sensordebounce= wDigInt.getsensordebounce( ini );
  data->fastclock     = wDigInt.isfastclock(data->ini);

  MemOp.set( data->swTime0, -1, sizeof( data->swTime0 ) );
  MemOp.set( data->swTime1, -1, sizeof( data->swTime1 ) );
  MemOp.set( data->fbState, 0, sizeof( data->fbState ) );

  SerialOp.setFlow( data->serial, cts );

  if( data->usb) /* force to 57600 ignoring the ini.*/
    SerialOp.setLine( data->serial, 57600, 8, 1, 0 );
  else
    SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, 0 );

  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  if ( data->elite || data->usb)
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lenz %d.%d.%d [%s]", vmajor, vminor, patch, wDigInt.getsublib(ini));
  else
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lenz %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device          = %s", wDigInt.getdevice( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps             = %d", wDigInt.getbps( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switchtime      = %d", data->swtime );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensor offset   = %d", data->fboffset );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensor debounce = %s", data->sensordebounce ? "yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fast clock      = %s", data->fastclock ? "yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  if( !SerialOp.open( data->serial ) && !data->dummyio ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init Lenz port!" );
  }
  else {
    data->run = True;

    data->transactor = ThreadOp.inst( "transactor", &__transactor, __Lenz );
    ThreadOp.start( data->transactor );

    data->initializer = ThreadOp.inst( "initializer", &__initializer, __Lenz );
    ThreadOp.start( data->initializer );
  }

  instCnt++;
  return __Lenz;
}

/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/lenz.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
