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


#include "rocdigs/impl/xpressnet_impl.h"
#include "rocdigs/impl/xpressnet/li101.h"
#include "rocdigs/impl/xpressnet/liusb.h"
#include "rocdigs/impl/xpressnet/elite.h"
#include "rocdigs/impl/xpressnet/roco.h"
#include "rocdigs/impl/xpressnet/opendcc.h"
#include "rocdigs/impl/xpressnet/atlas.h"
#include "rocdigs/impl/xpressnet/xntcp.h"
#include "rocdigs/impl/xpressnet/common.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"
#include "rocs/public/system.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/FbInfo.h"
#include "rocrail/wrapper/public/FbMods.h"

#include "rocdigs/impl/common/fada.h"

#include <time.h>


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOXpressNetData data = Data(inst);
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

/** ----- OXpressNet ----- */
/* extract the bits */
static void __dec2bin(int *b0, int num) {
  int i, rest;
  for (i =0; i < 8; i++){
    rest = num % 2;
    num = num / 2;
    b0[7-i] = rest;
  }
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

static void __handleSwitch(iOXpressNet xpressnet, int addr, int port, int value) {
  iOXpressNetData data = Data(xpressnet);

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


static iONode __translate( iOXpressNet xpressnet, iONode node ) {
  iOXpressNetData data = Data(xpressnet);
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
    if( data->fastclock && StrOp.equals( wClock.set, wClock.getcmd( node ) ) ) {
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
    }

  }
  return rsp;
}


static void __evaluateResponse( iOXpressNet xpressnet, byte* in, int datalen ) {
  iOXpressNetData data = Data(xpressnet);

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
        __handleSwitch(xpressnet, baseadress, start+k, b2[7-k*2]);
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
        if( data->fbState[iAddr] != bState ) {
          /* inform listener: Node3 */
          iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
          wFeedback.setaddr( nodeC, i+data->fboffset );
          wFeedback.setstate( nodeC, data->fbState[iAddr] );
          if( data->iid != NULL )
            wFeedback.setiid( nodeC, data->iid );

          if( data->listenerFun != NULL && data->listenerObj != NULL )
            data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
              "Sensor %d=%d", i + data->fboffset, data->fbState[iAddr]);
        }

      }
    }
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


static Boolean __rspExpected( byte* out ) {
  rspExpected = False;

  return rspExpected;
}

static void __transactor( void* threadinst ) {
  iOThread        th = (iOThread)threadinst;
  iOXpressNet     xpressnet = (iOXpressNet)ThreadOp.getParm(th);
  iOXpressNetData data = Data(xpressnet);

  byte out[256];
  byte in[256];
  Boolean responceRecieved = True;
  int rspExpected = 0;
  obj post = NULL;

  ThreadOp.setDescription( th, "XpressNet transactor" );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "transactor started." );

  while( data->run ) {

    /* get next command only if the last command was successfull,
       otherwise work on the current node until the cs will answer, or give up after numtries */
    if (responceRecieved) {
      post = ThreadOp.getPost( th );
      if (post != NULL) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "processing post..." );
        MemOp.copy(out, post, 32);
        freeMem( post);
        if( data->subWrite( (obj)xpressnet, out, &rspExpected ) ) {
          responceRecieved = !rspExpected;
        }
        else {
          /* TODO: unable to send request */
        }
      }
    }

    if( !data->subAvail( (obj)xpressnet ) ) {
      ThreadOp.sleep(10);
      continue;
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "processing response..." );

    if ( data->subRead((obj)xpressnet, in) > 0 ) {

      byte bXor = 0;
      int i = 0;
      int datalen = (in[0] & 0x0f) + 1;

      for( i = 0; i < datalen; i++ ) {
        bXor ^= in[ i ];
      }

      if( bXor != in[datalen]) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
            "XOR error: datalength=%d calculated=0x%02X received=0x%02X", datalen, bXor, in[datalen] );
        ThreadOp.sleep(10);
        continue;
        /* flush buffer */
      }

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
      }
      /* Version of Interface*/
      else if (in[0] == 0x02){
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Interface version: %1.1f-%d",
            in[1]/10.0 , in[2] );
        responceRecieved = True;
        data->interfaceVersion = (int) in[1];
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
      /* Nasty Elite, response on loc command or loc operated on elite*/
      else if (in[0] == 0xE3 || in[0] == 0xE4 || in[0] == 0xE5 ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Elite: Loc command");
        responceRecieved = True;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Unknown command.");
        TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, 15);
      }

      /* anything will go to rocgui ...*/
      __evaluateResponse( xpressnet, in, datalen );

    }


    ThreadOp.sleep(10);
  };

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "transactor ended." );
}


static void __initializer( void* threadinst ) {
  iOThread        th = (iOThread)threadinst;
  iOXpressNet     xpressnet = (iOXpressNet)ThreadOp.getParm(th);
  iOXpressNetData data = Data(xpressnet);

  unsigned char out[256];

  ThreadOp.setDescription( th, "XpressNet initializer" );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "initializer started." );

  ThreadOp.sleep(100);
  data->subInit((obj)xpressnet);

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "initializer ended." );
  ThreadOp.base.del(th);
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOXpressNetData data = Data(inst);
  iONode rsp = NULL;

  if( cmd != NULL ) {
    rsp = __translate( (iOXpressNet)inst, cmd );
    /* Cleanup Node1 */
    cmd->base.del(cmd);
  }

  return rsp;
}


/**  */
static void _halt( obj inst ) {
  iOXpressNetData data = Data(inst);
  data->run = False;

  /* ALL OFF */
  int rspExpected = False;
  byte* outc = allocMem(256);
  outc[0] = 0x21;
  outc[1] = 0x80;
  outc[2] = 0xA1;
  data->subWrite(inst, outc, &rspExpected);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  data->subDisConn(inst);
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOXpressNetData data = Data(inst);
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


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 1;
static int vminor = 4;
static int patch  = 99;
static int _version( obj inst ) {
  iOXpressNetData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**  */
static struct OXpressNet* _inst( const iONode ini ,const iOTrace trc ) {
  iOXpressNet __XpressNet = allocMem( sizeof( struct OXpressNet ) );
  iOXpressNetData data = allocMem( sizeof( struct OXpressNetData ) );
  MemOp.basecpy( __XpressNet, &XpressNetOp, 0, sizeof( struct OXpressNet ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  SystemOp.inst();

  data->ini           = ini;
  data->iid           = StrOp.dup( wDigInt.getiid( ini ) );
  data->serialmux     = MutexOp.inst( StrOp.fmt( "serialMux%08X", data ), True );
  data->swtime        = wDigInt.getswtime( ini );
  data->dummyio       = wDigInt.isdummyio( ini );
  data->fboffset      = wDigInt.getfboffset( ini );
  data->serial        = SerialOp.inst( wDigInt.getdevice( ini ) );
  data->startpwstate  = wDigInt.isstartpwstate( ini );
  data->fastclock     = wDigInt.isfastclock(ini);
  data->fbmod         = wDigInt.getfbmod( ini );
  data->readfb        = wDigInt.isreadfb( ini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "WIP: Do not use!" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "XpressNet %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  if( StrOp.equals( wDigInt.sublib_lenz_xntcp, wDigInt.getsublib( ini ) ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "tcp             = %s:%d", wDigInt.gethost( ini ), wDigInt.getport( ini ) );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device          = %s", wDigInt.getdevice( ini ) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "bps             = %d", wDigInt.getbps( ini ) );
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib          = %s", wDigInt.getsublib( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switchtime      = %d", data->swtime );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensor offset   = %d", data->fboffset );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fast clock      = %s", data->fastclock ? "yes":"no" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );


  /* choose interface: */
  if( StrOp.equals( wDigInt.sublib_usb, wDigInt.getsublib( ini ) ) ) {
    /* LI-USB */
    data->subConnect = liusbConnect;
    data->subInit    = liusbInit;
    data->subRead    = liusbRead;
    data->subWrite   = liusbWrite;
    data->subDisConn = liusbDisConnect;
    data->subAvail   = liusbAvail;
  }
  else if( StrOp.equals( wDigInt.sublib_lenz_elite, wDigInt.getsublib( ini ) ) ) {
    /* Hornby Elite */
    data->subConnect = eliteConnect;
    data->subInit    = eliteInit;
    data->subRead    = eliteRead;
    data->subWrite   = eliteWrite;
    data->subDisConn = eliteDisConnect;
    data->subAvail   = eliteAvail;
  }
  else if( StrOp.equals( wDigInt.sublib_lenz_roco, wDigInt.getsublib( ini ) ) ) {
    /* Roco */
    data->subConnect = rocoConnect;
    data->subInit    = rocoInit;
    data->subRead    = rocoRead;
    data->subWrite   = rocoWrite;
    data->subDisConn = rocoDisConnect;
    data->subAvail   = rocoAvail;
  }
  else if( StrOp.equals( wDigInt.sublib_lenz_opendcc, wDigInt.getsublib( ini ) ) ) {
    /* OpenDCC */
    data->subConnect = opendccConnect;
    data->subInit    = opendccInit;
    data->subRead    = opendccRead;
    data->subWrite   = opendccWrite;
    data->subDisConn = opendccDisConnect;
    data->subAvail   = opendccAvail;
  }
  else if( StrOp.equals( wDigInt.sublib_lenz_atlas, wDigInt.getsublib( ini ) ) ) {
    /* Atlas */
    data->subConnect = atlasConnect;
    data->subInit    = atlasInit;
    data->subRead    = atlasRead;
    data->subWrite   = atlasWrite;
    data->subDisConn = atlasDisConnect;
    data->subAvail   = atlasAvail;
  }
  else if( StrOp.equals( wDigInt.sublib_lenz_xntcp, wDigInt.getsublib( ini ) ) ) {
    /* Atlas */
    data->subConnect = xntcpConnect;
    data->subInit    = xntcpInit;
    data->subRead    = xntcpRead;
    data->subWrite   = xntcpWrite;
    data->subDisConn = xntcpDisConnect;
    data->subAvail   = xntcpAvail;
  }
  else {
    /* default LI101 */
    data->subConnect = li101Connect;
    data->subInit    = li101Init;
    data->subRead    = li101Read;
    data->subWrite   = li101Write;
    data->subDisConn = li101DisConnect;
    data->subAvail   = li101Avail;
  }

  if( data->subConnect((obj)__XpressNet) ) {
    /* start transactor */
    data->run = True;

    data->transactor = ThreadOp.inst( "transactor", &__transactor, __XpressNet );
    ThreadOp.start( data->transactor );

    /* give the transactor thread time to start up */
    ThreadOp.sleep( 10 );

    data->initializer = ThreadOp.inst( "initializer", &__initializer, __XpressNet );
    ThreadOp.start( data->initializer );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "unable to initialize [%s]", wDigInt.getdevice( ini ) );
  }


  instCnt++;
  return __XpressNet;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/xpressnet.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
