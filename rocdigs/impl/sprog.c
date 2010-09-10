/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>
 http://www.rocrail.net

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


/******************************************************************************
SPROG II User Guide
SPROG Commands:
The (virtual) COM port should be set for 8 bits, no parity, one stop bit at a
speed of 9600 baud. SPROG does not echo characters that are sent to it. All
commands must be entered on a single line terminated by carriage return.
Maximum input line length is 64 characters, including carriage return. Format
of parameters is dependent upon the command. The maximum number of
parameters on any line is 6.


General Commands
      M [n] – Display [Set] operating mode
      R – Read mode from EEPROM
      S – Display Status
      W – Write mode to EEPROM
      Z [n] – ZTC Compatibility Mode
      ? - Display Help
      ESC - immediately shutdown power to track

Programmer Commands
      C CV [Val] - Read [program] CV using direct bit mode
      V CV [Val] - Read [program] CV using paged mode

Rolling Road Tester Commands
      A [n] – Display [Set] Address
      O byte [byte] [byte] [byte] - Output bytes as DCC packet.
      + - Track power on
      - - Track power off
      <[step | <] - Reverse speed step[s]
      >[step | >] - Forward speed step[s]

Bootloader Command
      B a b c – Start Bootloader

Input Format
Input values are always parsed as decimal, unless overridden with 'b' or 'h'
prefix for binary or hexadecimal, respectively. E.g. h15 is equivalent to 21
decimal. The O command is an exception to this rule.
Acknowledgement Messages
CV values are given in hexadecimal.
                    Message    Meaning
                       !O      Overload
                       !E      Error
                     No-ack    No acknowledge pulse received
                               during programming
                       OK      Programming operation
                               completed

The Mode Word
The “Mode Word” determines the operational mode of SPROG. If the mode is
changed, the new mode may be stored permanently in EEPROM memory.
The Mode Word is read from the EEPROM each time the SPROG powers up.
The Mode Word is a 16 bit binary value, with each bit corresponding to a
particular feature, as shown in the table.
       Bit    Name          Feature
        0     UNLOCK        Unlock the firmware ready to receive an update via the
                            bootloader. This bit is not stored in EEPROM and is cleared
                            each time SPROG is reset
        1     Reserved      SPROG II echoes all received characters if this bit is set
        2     Reserved      Do not use, always set to 0 for future compatibility
        3     CALC_ERROR    Set to calculate error byte for O command. If clear then
                            error byte must be supplied on the command line
        4     RR_MODE       Set for rolling road/test mode
        5     ZTC_MODE      SPROG II uses modified DCC timing for older ZTC
                            decoders
        6     BLUELINE      Modify direct mode programming algorithm to suit
                            Blueline decoders
        7     Reserved      Do not use, always set to 0 for future compatibility
        8     DIR           Direction for rolling road/test mode and booster mode. Set
                            for reverse
        9   SP14     Select 14 speed step mode for rolling road/test mode and
                     booster mode.
       10   SP28     Select 28 speed step mode for rolling road/test mode and
                     booster mode.
       11   SP128    Select 128 speed step mode for rolling road/test mode and
                     booster mode.
       12   LONG     Use long addresses in rolling road/test mode and booster
                     mode
      13-15 Reserved Do not use, always set to 0 for future compatibility





Commands in Detail
M - Display Mode Word
Display the current mode word value.

M n - Set Mode Word
Set the Mode Word with the value n.

R – Read Mode from EEPROM
Read a previously saved mode word from EEPROM.

S – Display Status
Display SPROG II status - TBD

W – Write Mode to EEPROM
Write the current mode word to EEPROM.

Z [n] – ZTC Compatibility Mode
Some older ZTC decoders (e.g. ZTC401) require modified DCC timing.

Z 0 – return to normal DCC timing
Z 1 – Enable ZTC compatibility mode.
This command manipulates the ZTC_MODE bit of the mode word.

? - Display Help
Displays the SPROG II firmware version, e.g.:
SPROG II USB Ver 2.4
>

ESC - Shutdown
DCC output is turned off immediately.


Programmer Commands
C cv - Read a CV using direct bit mode
C cv val - Program a CV using direct bit mode
V cv - Read a CV using paged mode
V cv val - Program a CV using paged mode
If no val value is given, then these command read the specified CV and
display the value in hexadecimal.
If val is given then it is written to the CV.


Rolling Road Tester Commands
A – Display Address
A n – Set Address
Display or set the decoder address (decimal) to be used in speed/direction
packets. If a new address is set then current speed step will be reset to zero.
This command does not perform any programming of decoder CVs.

O byte [byte] [byte] [byte] - Output bytes as DCC packet.
Any arbitrary DCC packet may be generated using this command. SPROG II
will add the correct pre-amble bits, start bits, and error byte. Note that all
address and data bytes and, optionally, the error byte must be given on the
command line, this command does not use the address set by the 'A'
command. If the mode word CALC_ERROR bit is set then SPROG II will
calculate the correct error byte which must not be given on the command line.
If CALC_ERROR is not set then the error byte must be given on the
command line, allowing erroneous packets to be generated for decoder
testing.
!  Unlike other commands, bytes must be given as two hex digits without a h
   prefix.

+ - Track power on
Turn on track power and check for overload condition after 100ms. When
there is no DCC data being transmitted, DCC pre-amble will be transmitted.

- - Track power off
Turn off track power.

<<[<] - Reduce/Reverse speed step[s]
>>[>] - Increase/Forward speed step[s]
Adjust speed step relative to current speed. If decoder is running in reverse
then Reduce/Reverse will increase the reverse speed and Increase/Forward
will decrease the reverse speed. If decoder is running forward then
Reduce/Reverse will decrease the forward speed and Increase/Forward will
increase the forward speed. Increment or decrement is determined by the
number of '<' or '>' characters in the command. Speed will not
increment/decrement past maximum forward or reverse speed step nor
through zero. The current speed step will be reported after performing this
command:
< – display Reverse speed step
< step – Set Reverse speed step
> - display Forward speed step
> step – Set Forward speed step
 Set forward or reverse speed step directly.


Bootloader Command
B a b c – Start Bootloader
Exactly three arguments, a, b, c, must be given with the B command but their
values are not checked. This helps prevent inadvertent issuing of the b
command. The B commands starts the bootloader ready to receive updated
SPROG II firmware. In addition, the firmware must be unlocked by setting the
unlock bit of the Mode Word.


*/


#include "rocdigs/impl/sprog_impl.h"
#include "rocdigs/impl/nmra/nmrapacket.h"

#include "rocs/public/str.h"
#include "rocs/public/mem.h"
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

#include "rocdigs/impl/common/fada.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOSprogData data = Data(inst);
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
  iOSprogData data = Data(inst);
  return data->ini;
}

static const char* __id( void* inst ) {
  iOSprogData data = Data(inst);
  return data->iid;
}

static void* __event( void* inst, const void* evt ) {
  iOSprogData data = Data(inst);
  return NULL;
}

/** ----- OSprog ----- */

static char* __byteToStr( char* s, unsigned char* data, int size ) {
  static char cHex[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  int i = 0;
  for( i = 0; i < size; i++ ) {
    int b = data[i];
    s[i*3]   = cHex[(b&0xF0)>>4 ];
    s[i*3+1] = cHex[ b&0x0F     ];
    s[i*3+2] = ' ';
  }
  s[size*3] = '\0';
  return s;
}




static Boolean __transact( iOSprog sprog, char* out, int outsize, char* in, int insize, int repeat ) {
  iOSprogData data = Data(sprog);
  Boolean     rc = False;

  if( MutexOp.wait( data->mux ) ) {
    int i = 0;
    ThreadOp.sleep(5);

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_transact outsize=%d insize=%d", outsize, insize );

    for( i = 0; i < repeat; i++ ) {
      if( rc = SerialOp.write( data->serial, out, outsize ) ) {
        if( insize > 0 ) {
          rc = SerialOp.read( data->serial, in, insize );
        }
      }
    }

    /* Release the mutex. */
    MutexOp.post( data->mux );

  }

  return rc;
}


static int __getLocoSlot(iOSprog sprog, iONode node) {
  iOSprogData data = Data(sprog);
  int i    = 0;
  int addr = wLoc.getaddr(node);

  /* lookup slot for address: */
  for( i = 0; i < 128; i++ ) {
    if( data->slots[i].addr == addr || data->slots[i].addr == 0 ) {
      return i;
    }
  }
  return -1;
}


static int __translate( iOSprog sprog, iONode node, char* outa, int* insize ) {
  iOSprogData data = Data(sprog);
  int repeat = 1;

  outa[0] = '\0';
  *insize = 0;


  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      StrOp.fmtb( outa, "-\r" );
      data->power = False;
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      StrOp.fmtb( outa, "+\r" );
      data->power = True;
    }
  }

  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {

    int addr = wSwitch.getaddr1( node );
    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );
    int fada = 0;
    int pada = 0;
    int dir  = 1;
    int action = 1;
    int cmdsize = 0;
    byte dcc[12];
    char cmd[32] = {0};

    if( port == 0 ) {
      fada = addr;
      fromFADA( addr, &addr, &port, &gate );
    }
    else if( addr == 0 && port > 0 ) {
      pada = port;
      fromPADA( port, &addr, &port );
    }

    if( fada == 0 )
      fada = toFADA( addr, port, gate );
    if( pada == 0 )
      pada = toPADA( addr, port );

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      dir = 0; /* thrown */

    if( wSwitch.issinglegate( node ) ) {
      dir = gate;
      if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.straight ) )
        action = 0;
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "turnout %04d %d %-10.10s fada=%04d pada=%04d addr=%d port=%d gate=%d dir=%d action=%d",
        addr, port, wSwitch.getcmd( node ), fada, pada, addr, port, gate, dir, action );

    cmdsize = accDecoderPkt2(dcc, addr, 1, (port-1)*2+dir);
    __byteToStr( cmd, dcc, cmdsize );
    StrOp.fmtb( outa, "O %s\r", cmd );
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "DCC out: %s", outa );
    *insize = 3;
    repeat = 2;
  }
  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {

    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    int fada = 0;
    int pada = 0;
    int cmdsize = 0;
    byte dcc[12];
    char cmd[32] = {0};
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;

    if( port == 0 ) {
      fada = addr;
      fromFADA( addr, &addr, &port, &gate );
    }
    else if( addr == 0 && port > 0 ) {
      pada = port;
      fromPADA( port, &addr, &port );
    }

    if( fada == 0 )
      fada = toFADA( addr, port, gate );
    if( pada == 0 )
      pada = toPADA( addr, port );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %04d %d %d fada=%04d pada=%04d",
        addr, port, gate, fada, pada );


    cmdsize = accDecoderPkt2(dcc, addr, action, (port-1)*2+gate);
    /*cmdsize = accDecoderPkt(dcc, fada, action);*/
    __byteToStr( cmd, dcc, cmdsize );
    StrOp.fmtb( outa, "O %s\r", cmd );
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "DCC out: %s", outa );
    *insize = 3;
    repeat = 2;
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    Boolean pom = wProgram.ispom( node );

    if( !pom && !data->power ) {
      if( wProgram.getcmd( node ) == wProgram.get ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CV %d get", wProgram.getcv(node) );
        StrOp.fmtb( outa, "%c %d\r", wProgram.isdirect(node)?'C':'V', wProgram.getcv(node) );
        data->lastcmd = CV_READ;
        data->lastvalue = 0;
      }
      else if( wProgram.getcmd( node ) == wProgram.set ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CV %d set %d", wProgram.getcv(node), wProgram.getvalue(node) );
        StrOp.fmtb( outa, "%c %d %d\r", wProgram.isdirect(node)?'C':'V', wProgram.getcv(node), wProgram.getvalue(node) );
        data->lastcmd = CV_WRITE;
        data->lastvalue = wProgram.getvalue(node);
      }
    }
    else if( pom && data->power ) {
      if( wProgram.getcmd( node ) == wProgram.set ) {
        byte dcc[12];
        char cmd[32] = {0};
        int cmdsize = opsCvWriteByte(dcc, wProgram.getaddr(node), wProgram.islongaddr(node), wProgram.getcv(node), wProgram.getvalue(node) );
        __byteToStr( cmd, dcc, cmdsize );
        StrOp.fmtb( outa, "O %s\r", cmd );
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "POM DCC out: %s", outa );
        *insize = 3;
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "turn power %s before programming", pom?"ON (POM)":"OFF" );
    }
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int slot   = 0;
    int size   = 0;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Loco command for address %d", wLoc.getaddr( node ) );

    slot =  __getLocoSlot( sprog, node);

    if( slot >= 0 ) {
      int V = 0;
      int steps = wLoc.getspcnt( node );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Loco slot=%d", slot );

      if( wLoc.getV( node ) != -1 ) {
        if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
          V = (wLoc.getV( node ) * steps) / 100;
        else if( wLoc.getV_max( node ) > 0 )
          V = (wLoc.getV( node ) * steps) / wLoc.getV_max( node );
      }

      /* keep this value for the ping thread */
      data->slots[slot].dir = wLoc.isdir( node );
      data->slots[slot].V = V;
      data->slots[slot].steps = steps;
      data->slots[slot].addr = wLoc.getaddr( node );
      data->slots[slot].lights = wLoc.isfn( node );
      data->slots[slot].fn[0]  = wLoc.isfn( node );
      data->slots[slot].changedfgrp = wLoc.isfn( node ) ? 1:-1;
      data->slots[slot].idle = SystemOp.getTick();

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "slot=%d addr=%d V=%d steps=%d dir=%d long=%d", slot,
          data->slots[slot].addr, data->slots[slot].V, data->slots[slot].steps, data->slots[slot].dir, data->slots[slot].longaddr );

      size = 0;
    }
  }

  /* Function */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int addr  = wFunCmd.getaddr( node );
    int group = wFunCmd.getgroup( node );

    int slot =  __getLocoSlot( sprog, node);

    if( slot >= 0 ) {
      if( data->slots[slot].addr == 0 ) {
        /* first use of this slot */
        data->slots[slot].addr = addr;
      }
      data->slots[slot].changedfgrp = group;
      data->slots[slot].lights = wFunCmd.isf0 ( node );
      data->slots[slot].fn[ 0] = wFunCmd.isf0 ( node );
      data->slots[slot].fn[ 1] = wFunCmd.isf1 ( node );
      data->slots[slot].fn[ 2] = wFunCmd.isf2 ( node );
      data->slots[slot].fn[ 3] = wFunCmd.isf3 ( node );
      data->slots[slot].fn[ 4] = wFunCmd.isf4 ( node );
      data->slots[slot].fn[ 5] = wFunCmd.isf5 ( node );
      data->slots[slot].fn[ 6] = wFunCmd.isf6 ( node );
      data->slots[slot].fn[ 7] = wFunCmd.isf7 ( node );
      data->slots[slot].fn[ 8] = wFunCmd.isf8 ( node );
      data->slots[slot].fn[ 9] = wFunCmd.isf9 ( node );
      data->slots[slot].fn[10] = wFunCmd.isf10( node );
      data->slots[slot].fn[11] = wFunCmd.isf11( node );
      data->slots[slot].fn[12] = wFunCmd.isf12( node );
      data->slots[slot].fn[13] = wFunCmd.isf13( node );
      data->slots[slot].fn[14] = wFunCmd.isf14( node );
      data->slots[slot].fn[15] = wFunCmd.isf15( node );
      data->slots[slot].fn[16] = wFunCmd.isf16( node );
      data->slots[slot].fn[17] = wFunCmd.isf17( node );
      data->slots[slot].fn[18] = wFunCmd.isf18( node );
      data->slots[slot].fn[19] = wFunCmd.isf19( node );
      data->slots[slot].fn[20] = wFunCmd.isf20( node );
      data->slots[slot].fn[21] = wFunCmd.isf21( node );
      data->slots[slot].fn[22] = wFunCmd.isf22( node );
      data->slots[slot].fn[23] = wFunCmd.isf23( node );
      data->slots[slot].fn[24] = wFunCmd.isf24( node );
      data->slots[slot].fn[25] = wFunCmd.isf25( node );
      data->slots[slot].fn[26] = wFunCmd.isf26( node );
      data->slots[slot].fn[27] = wFunCmd.isf27( node );
      data->slots[slot].fn[28] = wFunCmd.isf28( node );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
          "slot=%d function group %d changed for loco %d", slot, group, addr );
    }
  }


  return repeat;

}


/**  */
static iONode _cmd( obj inst ,const iONode nodeA ) {
  iOSprogData data = Data(inst);
  iONode nodeB = NULL;
  char outa[100] = {'\0'};
  char ina[100]  = {'\0'};
  int insize = 0;

  if( nodeA != NULL ) {
    int repeat = __translate( (iOSprog)inst, nodeA, outa, &insize );
    if( StrOp.len(outa) > 0 ) {
      __transact( (iOSprog)inst, outa, StrOp.len(outa), ina, insize, repeat );
    }
    /* Cleanup Node1 */
    nodeA->base.del(nodeA);
  }

  /* return Node2 */
  return nodeB;
}


/**  */
static void _halt( obj inst, Boolean poweroff ) {
  iOSprogData data = Data(inst);
  data->run = False;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down [%s]...", data->iid );
  SerialOp.close( data->serial );
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOSprogData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  iOSprogData data = Data(inst);
  return 0;
}


/* external shortcut event */
static void _shortcut(obj inst) {
  iOSprogData data = Data( inst );
}


/**  */
static Boolean _supportPT( obj inst ) {
  iOSprogData data = Data(inst);
  return False;
}


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOSprogData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static int __parseCVValue(const char* in) {
  int val = 0;
  int idx = 0;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "string to parse: \"%s\"", in );

  if( StrOp.findi(in, "No Ack") ) {
	  return -1;
  }

  /* parse the value string */
  while( in[idx] != 0 ) {
    if( in[idx] == 'h' ) {
      /* hex value */
      val = (int)strtol( in+(idx+1), NULL, 16 );
      break;
    }
    idx++;
  }
  return val;
}

static void __handleResponse(iOSprog sprog, const char* in) {
  iOSprogData data = Data(sprog);
  iONode rsp = NULL;

  switch( data->lastcmd ) {
  case CV_READ:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CV read response" );
    rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setcv( rsp, data->lastcv );
    wProgram.setvalue( rsp, __parseCVValue(in) );
    wProgram.setcmd( rsp, wProgram.datarsp );
    if( data->iid != NULL )
      wProgram.setiid( rsp, data->iid );
    data->lastcmd = 0;
    break;
  case CV_WRITE:
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CV write response" );
    rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setcv( rsp, data->lastcv );
    wProgram.setvalue( rsp, StrOp.find(in, "OK") ?data->lastvalue:0 );
    wProgram.setcmd( rsp, wProgram.datarsp );
    if( data->iid != NULL )
      wProgram.setiid( rsp, data->iid );
    data->lastcmd = 0;
    break;
  }

  if( rsp != NULL ) {
    if( data->listenerFun != NULL && data->listenerObj != NULL )
      data->listenerFun( data->listenerObj, rsp, TRCLEVEL_INFO );
  }

}


static void __sprogWriter( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOSprog sprog = (iOSprog)ThreadOp.getParm( th );
  iOSprogData data = Data(sprog);
  int slotidx = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SPROG writer started." );

  while(data->run) {

    ThreadOp.sleep(25);

    if( data->power ) {
      if( data->slots[slotidx].addr > 0 ) {
        byte dcc[12];
        char cmd[32] = {0};
        char out[64] = {0};
        char in [64] = {0};
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot refresh for %d", data->slots[slotidx].addr );

        if( data->slots[slotidx].V == data->slots[slotidx].V_prev && data->slots[slotidx].changedfgrp == 0 ) {
          if( data->slots[slotidx].idle + 8000 < SystemOp.getTick() ) {
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
                "slot %d purged for loco address %d", slotidx, data->slots[slotidx].addr );
            data->slots[slotidx].addr = 0;
            data->slots[slotidx].idle = 0;
            data->slots[slotidx].fgrp = 0;
            data->slots[slotidx].changedfgrp = 0;
            data->slots[slotidx].V_prev = 0;
            data->slots[slotidx].V = 0;
            slotidx++;
            continue;
          }
        }
        else {
          data->slots[slotidx].V_prev == data->slots[slotidx].V;
          data->slots[slotidx].fgrp = data->slots[slotidx].changedfgrp;
          data->slots[slotidx].changedfgrp = 0;
          data->slots[slotidx].idle = SystemOp.getTick();
        }


        if( data->slots[slotidx].steps == 128 )  {
          int size = speedStep128Packet(dcc, data->slots[slotidx].addr,
              data->slots[slotidx].longaddr, data->slots[slotidx].V, data->slots[slotidx].dir );
          __byteToStr( cmd, dcc, size );
          StrOp.fmtb( out, "O %s\r", cmd );
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "128 DCC out: %s", out );
          __transact( sprog, out, StrOp.len(out), in, 3, 1 );
        }
        else if( data->slots[slotidx].steps == 28 )  {
          int size = speedStep28Packet(dcc, data->slots[slotidx].addr,
              data->slots[slotidx].longaddr, data->slots[slotidx].V, data->slots[slotidx].dir );
          __byteToStr( cmd, dcc, size );
          StrOp.fmtb( out, "O %s\r", cmd );
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "28 DCC out: %s", out );
          __transact( sprog, out, StrOp.len(out), in, 3, 1 );
        }
        else {
          int size = speedStep14Packet(dcc, data->slots[slotidx].addr,
              data->slots[slotidx].longaddr, data->slots[slotidx].V,
              data->slots[slotidx].dir, data->slots[slotidx].lights );
          __byteToStr( cmd, dcc, size );
          StrOp.fmtb( out, "O %s\r", cmd );
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "14 DCC out: %s", out );
          __transact( sprog, out, StrOp.len(out), in, 3, 1 );
        }

        if( data->slots[slotidx].fgrp > 0 ) {
          int size = 0;

          ThreadOp.sleep(25);
          if( data->slots[slotidx].fgrp == 1 ) {
            size = function0Through4Packet(dcc, data->slots[slotidx].addr,
                data->slots[slotidx].longaddr,
                data->slots[slotidx].fn[0],
                data->slots[slotidx].fn[1],
                data->slots[slotidx].fn[2],
                data->slots[slotidx].fn[3],
                data->slots[slotidx].fn[4] );
          }
          else if( data->slots[slotidx].fgrp == 2 ) {
            size = function5Through8Packet(dcc, data->slots[slotidx].addr,
                data->slots[slotidx].longaddr,
                data->slots[slotidx].fn[5],
                data->slots[slotidx].fn[6],
                data->slots[slotidx].fn[7],
                data->slots[slotidx].fn[8] );
          }
          else if( data->slots[slotidx].fgrp == 3 ) {
            size = function9Through12Packet(dcc, data->slots[slotidx].addr,
                data->slots[slotidx].longaddr,
                data->slots[slotidx].fn[9],
                data->slots[slotidx].fn[10],
                data->slots[slotidx].fn[11],
                data->slots[slotidx].fn[12] );
          }
          else if( data->slots[slotidx].fgrp == 4 || data->slots[slotidx].fgrp == 5 ) {
            size = function13Through20Packet(dcc, data->slots[slotidx].addr,
                data->slots[slotidx].longaddr,
                data->slots[slotidx].fn[13],
                data->slots[slotidx].fn[14],
                data->slots[slotidx].fn[15],
                data->slots[slotidx].fn[16],
                data->slots[slotidx].fn[17],
                data->slots[slotidx].fn[18],
                data->slots[slotidx].fn[19],
                data->slots[slotidx].fn[20] );
          }
          else if( data->slots[slotidx].fgrp == 6 || data->slots[slotidx].fgrp == 7 ) {
            size = function21Through28Packet(dcc, data->slots[slotidx].addr,
                data->slots[slotidx].longaddr,
                data->slots[slotidx].fn[21],
                data->slots[slotidx].fn[22],
                data->slots[slotidx].fn[23],
                data->slots[slotidx].fn[24],
                data->slots[slotidx].fn[25],
                data->slots[slotidx].fn[26],
                data->slots[slotidx].fn[27],
                data->slots[slotidx].fn[28] );
          }

          __byteToStr( cmd, dcc, size );
          StrOp.fmtb( out, "O %s\r", cmd );
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "function group %d DCC out: %s", data->slots[slotidx].changedfgrp, out );
          __transact( sprog, out, StrOp.len(out), in, 3, 1 );
        }

        slotidx++;
      }
      else {
        slotidx = 0;
      }
    }


  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SPROG writer ended." );
}


static void __sprogReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOSprog sprog = (iOSprog)ThreadOp.getParm( th );
  iOSprogData data = Data(sprog);

  char in[256] = {0};
  int idx = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SPROG reader started." );
  ThreadOp.sleep( 1000 );

  StrOp.fmtb( in, "?\r" );
  SerialOp.write(data->serial, in, StrOp.len(in));

  do {

    ThreadOp.sleep( 10 );

    if( MutexOp.wait( data->mux ) ) {

      if( SerialOp.available(data->serial) ) {
        if( SerialOp.read(data->serial, &in[idx], 1) ) {
          TraceOp.dump( NULL, TRCLEVEL_DEBUG, (char*)in, StrOp.len(in) );
          if( idx > 254 ) {
            in[idx] = 0;
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "reader overflow [%d]\n%s", idx, in );
            idx = 0;
          }
          else if( in[idx] == '\r' || in[idx] == '\n' ) {
            in[idx+1] = '\0';
            idx = 0;
            StrOp.replaceAll( in, '\n', ' ' );
            StrOp.replaceAll( in, '\r', ' ' );
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "SPROG read: [%s]", in );
            __handleResponse(sprog, in);
            in[idx] = '\0';
          }
          else if( StrOp.equals( in, "P> ") || StrOp.equals( in, " P>") || StrOp.equals( in, " P> ") ) {
            idx = 0;
            in[idx] = '\0'; /* ignore prompt */
          }
          else {
            idx++;
          }
        }
      }

      /* Release the mutex. */
      MutexOp.post( data->mux );
    }

  } while(data->run);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SPROG reader ended." );
}


/**  */
static struct OSprog* _inst( const iONode ini ,const iOTrace trc ) {
  iOSprog __Sprog = allocMem( sizeof( struct OSprog ) );
  iOSprogData data = allocMem( sizeof( struct OSprogData ) );
  MemOp.basecpy( __Sprog, &SprogOp, 0, sizeof( struct OSprog ), data );
  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->mux     = MutexOp.inst( NULL, True );
  data->slotmux = MutexOp.inst( NULL, True );

  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->device = StrOp.dup( wDigInt.getdevice( ini ) );
  data->run    = True;

  MemOp.set( data->slots, 0, 128 * sizeof( struct slot ) );

  SystemOp.inst();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sprog %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     = [%s]", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = [%s]", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout = [%d]ms", wDigInt.gettimeout( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, cts );
  SerialOp.setLine( data->serial, 9600, 8, 1, 0, wDigInt.isrtsdisabled( ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );
  SerialOp.open( data->serial );

  SerialOp.setDTR(data->serial, True);
  SerialOp.setRTS(data->serial, True);

  data->reader = ThreadOp.inst( "sprogrx", &__sprogReader, __Sprog );
  ThreadOp.start( data->reader );
  data->writer = ThreadOp.inst( "sprogtx", &__sprogWriter, __Sprog );
  ThreadOp.start( data->writer );

  instCnt++;
  return __Sprog;
}

/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/sprog.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
