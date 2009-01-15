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

#include "rocs/public/mem.h"

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


static Boolean __transact( iOSprog sprog, char* out, int outsize, char* in, int insize ) {
  iOSprogData data = Data(sprog);
  Boolean     rc = False;

  if( MutexOp.wait( data->mux ) ) {

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_transact outsize=%d insize=%d", outsize, insize );

    if( rc = SerialOp.write( data->serial, out, outsize ) ) {
      if( insize > 0 ) {
        rc = SerialOp.read( data->serial, in, insize );
      }
    }

    /* Release the mutex. */
    MutexOp.post( data->mux );

  }

  return rc;
}


static iONode __translate( iOSprog sprog, iONode node, char* outa, int* insize ) {
  iOSprogData data = Data(sprog);
  iONode rsp = NULL;

  outa[0] = '\0';
  *insize = 0;

  /* Switch command. */
  if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int addr = wSwitch.getaddr1( node );
    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );
    int fada = 0;
    int pada = 0;

    int state = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 0x01:0x00;

    if( port == 0 ) {
      fromFADA( addr, &addr, &port, &gate );
    }
    else if( addr == 0 && port > 0 ) {
      fromPADA( port, &addr, &port );
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "turnout %04d %d %-10.10s fada=%04d pada=%04d",
        addr, port, wSwitch.getcmd( node ), fada, pada );
  }


  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {

    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    int fada = 0;
    int pada = 0;

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


  /* Loco command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    Boolean fn = wLoc.isfn( node );
    int    dir = wLoc.isdir( node );
    int  spcnt = wLoc.getspcnt( node );

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * spcnt) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * spcnt) / wLoc.getV_max( node );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "speed=%d", speed );
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "addr=%d V=%d(%d) dir=%s lights=%s",
        addr, speed, wLoc.getV( node ), wLoc.isdir( node )?"fwd":"rev", fn?"on":"off" );

    StrOp.fmtb( outa, "A %d\r", addr );
  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );
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

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "addr=%d f1=%s f2=%s f3=%s f4=%s",
        addr, f1?"on":"off", f2?"on":"off", f3?"on":"off", f4?"on":"off" );
  }


  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      StrOp.fmtb( outa, "-\r" );
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      StrOp.fmtb( outa, "+\r" );
    }
  }

}


/**  */
static iONode _cmd( obj inst ,const iONode nodeA ) {
  iOSprogData data = Data(inst);
  iONode nodeB = NULL;
  char outa[100] = {'\0'};
  char ina[100]  = {'\0'};
  int insize = 0;

  if( nodeA != NULL ) {
    nodeB = __translate( (iOSprog)inst, nodeA, outa, &insize );
    if( StrOp.len(outa) > 0 ) {
      __transact( (iOSprog)inst, outa, StrOp.len(outa), ina, insize );
    }
    /* Cleanup Node1 */
    nodeA->base.del(nodeA);
  }

  /* return Node2 */
  return nodeB;
}


/**  */
static void _halt( obj inst ) {
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


/**  */
static Boolean _supportPT( obj inst ) {
  iOSprogData data = Data(inst);
  return False;
}


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 0;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOSprogData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}




/**  */
static struct OSprog* _inst( const iONode ini ,const iOTrace trc ) {
  iOSprog __Sprog = allocMem( sizeof( struct OSprog ) );
  iOSprogData data = allocMem( sizeof( struct OSprogData ) );
  MemOp.basecpy( __Sprog, &SprogOp, 0, sizeof( struct OSprog ), data );
  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->mux    = MutexOp.inst( NULL, True );

  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->device = StrOp.dup( wDigInt.getdevice( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sprog %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid     = [%s]", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device  = [%s]", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timeout = [%d]ms", wDigInt.gettimeout( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, cts );
  SerialOp.setLine( data->serial, 9600, 8, 1, 0 );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );
  SerialOp.open( data->serial );

  SerialOp.setDTR(data->serial, True);
  SerialOp.setRTS(data->serial, True);


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
