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

/*
;*************************************************************************
;
; The RS-232 port binary commands are designed to work
; in a computer friendly mode.
;
; Command format is: <cmd number> <data> <data> ...
;
; Commands range from 0x80 to 0xBF
;
;
; NOTE: For commands 0x9F,0xA0,0xA1 and 0xA6- 0xA9 a single
; byte of 0 will be returned if not in programming mode
;
;
;
; Errors returned: '0'= command not supported
; '1'= loco/accy/signal address out of range
; '2'= cab address or op code out of range
; '3'= CV address or data out of range
; '4'= byte count out of range
; '!'= command completed successfully
;
;*************************************************************************
;
; CMD FORMAT DESCRIPTION # OF BYTES RESPONSES RETURNED
;-------------------------------------------------------------------------
; 0x80 NOP, dummy instruction (1) !
;-------------------------------------------------------------------------
; 0x81 xx xx yy assign loco (1) !,1,2
;-------------------------------------------------------------------------
; 0x82 read clock (2) <hours>
; <minutes>
;-------------------------------------------------------------------------
; 0x83 Clock stop (1) !
;-------------------------------------------------------------------------
; 0x84 Clock start (1) !
;-------------------------------------------------------------------------
; 0x85 xx xx Set clock hr/min (1) !,3
;-------------------------------------------------------------------------
; 0x86 xx Set clock 12/24 (1) !,3
; 0=12 hr 1=24 hr
;-------------------------------------------------------------------------
; 0x87 xx Set clock ratio (1) !,3
;-------------------------------------------------------------------------
; 0x88 xx xx Dequeue packet by loco addr (1) !,1,2
;-------------------------------------------------------------------------
; 0x89 Enable main trk, kill prog (1) !
;-------------------------------------------------------------------------
; 0x8A yy Return status of AIU yy (4) <current hi byte>
; <current lo byte>
; <change hi byte>
; <change lo byte>
;-------------------------------------------------------------------------
; 0x8B Kill main trk, enable prog (1) !
;-------------------------------------------------------------------------
; 0x8C Dummy instruction returns "!" (3) !,0x0D,0x0A
; followed by CR/LF
;-------------------------------------------------------------------------
; 0x8D xxxx mm Set speed mode of loco xxxx (1) !,1,3
; to mode mm, 1=14, 2=28, 3=128 <speed mode, 0 to 3>
;-------------------------------------------------------------------------
; 0x8E aaaa nn <16 data bytes> (1) !,4
; Write nn bytes, start at aaaa
; Must have 16 data bytes, pad
; them out to 16 if necessary
;-------------------------------------------------------------------------
; 0x8F aaaa Read 16 bytes, start at aaaa (16) 16 bytes
;-------------------------------------------------------------------------
; 0x90 cc xx... Send 16 char message to Cab cc (1) !,2
; LCD line 3. xx = 16 ASCII char
;--------------------------------------------------------------------------
; 0x91 cc xx... Send 16 char message to Cab cc (1) !,2
; LCD line 4. xx = 16 ASCII char
;-------------------------------------------------------------------------
; 0x92 cc xx... Send 8 char message to Cab cc (1) !,2
; LCD line 2 right. xx = 8 char
;-------------------------------------------------------------------------
; 0x93 ss xx xx xx (1) !
; Queue 3 byte packet to TEMP_Q
; Send ss times
;-------------------------------------------------------------------------
; 0x94 ss xx xx xx xx (1) !
; Queue 4 byte packet to TEMP_Q
; Send ss times
;-------------------------------------------------------------------------
; 0x95 ss xx xx xx xx xx (1) !
; Queue 5 byte packet to TEMP_Q
; Send ss times
;-------------------------------------------------------------------------
; 0x96 ss xx xx xx xx xx xx (1) !
; Queue 6 byte packet to TEMP_Q
; Send ss times
;-------------------------------------------------------------------------
; 0x97 aaaa xx Write 1 byte,to aaaa (1) !
;-------------------------------------------------------------------------
; 0x98 aaaa xx xx (1) !
; Write 2 bytes to aaaa
;-------------------------------------------------------------------------
; 0x99 aaaa <4 data bytes> (1) !
; Write 4 bytes to aaaa
;-------------------------------------------------------------------------
; 0x9A aaaa <8 data bytes> (1) !
; Write 8 bytes to aaaa
;-------------------------------------------------------------------------
; 0x9B yy Return status of AIU yy (2) <current hi byte>
; (short form of command 0x8A) <current lo byte>
;-------------------------------------------------------------------------
; 0x9C xx Execute macro number xx (1) !,0,3
;-------------------------------------------------------------------------
; 0x9D aaaa Read 1 bytes from aaaa (1) 1 byte
;-------------------------------------------------------------------------
; 0x9E Enter Programming track mode (1) ! = success
; 3 = short circuit
;-------------------------------------------------------------------------
; 0x9F Exit Programming track mode (1) ! = success
;-------------------------------------------------------------------------
; 0xA0 aaaa xx Program CV aa with data xx in (1) ! = success
; paged mode 0 = program track no enabled
;-------------------------------------------------------------------------
; 0xA1 aaaa Read CV aa in paged mode (2) !,0,3
; NOTE: cv data followed ! for ok,
; 0xff followed by 3 for can't read
;-------------------------------------------------------------------------
; 0xA2 <4 data bytes>
; Locomotive control command (1) !,1
;
; Sends a speed or function packet to a locomotive.
; Command Format: 0xA2 <addr_h> <addr_l> <op_1> <data_1>
;
; Addr_h and Addr_l are the loco address in DCC format.
; If a long address is in use, bits 6 and 7 of the high byte are set.
; Ex: Long address 3 = 0xc0 0x03
; Short address 3 = 0x00 0x03
;
;
; op_1 data_1 Operation description
;
; 01 0-7f Reverse 28 speed command
; 02 0-7f Forward 28 speed command
; 03 0-7f Reverse 128 speed command
; 04 0-7f Forward 128 speed command
; 05 0 Estop reverse command
; 06 0 Estop forward command
; 07 0-1f Function group 1, bits 0-3 = F1-F4, bit 4=F0
; 08 0-0f Function group 2, bits 0-3 = F5-F8
; 09 0-0f Function group 3, bits 0-3 = F9-F12
; 0a 0-7f Set reverse consist address for lead loco **Last revised: 25 April 2008 Page 8 USB
; 0b 0-7f Set forward consist address for lead loco
; 0c 0-7f Set reverse consist address for rear loco
; 0d 0-7f Set forward consist address for rear loco
; 0e 0-7f Set reverse consist address for additional loco
; 0f 0-7f Set forward consist address for additional loco
; 10 0 Del loco from consist
; 11 0 Kill consist
; 12 0-9 Set momentum
; 13 0-7f No action, always returns success
; 14 0-7f No action, always returns success
; 15 0-ff Functions 13-20 control (bit 0=F13, bit 7=F20)
; 16 0-ff Functions 21-28 control (bit 0=F21, bit 7=F28)
; 17-7f reserved reserved
;
;-------------------------------------------------------------------------
; 0xA3 xx xx Queue 3 byte packet to TRK_Q (1) !,1
; (replaces any packet with same
; address if it exists)
;-------------------------------------------------------------------------
; 0xA4 xx xx... Queue 4 byte packet to TRK_Q (1) !,1
; (replaces any packet with same
; address if it exists)
;-------------------------------------------------------------------------
; 0xA5 xx xx... Queue 5 byte packet to TRK_Q (1) !,1
; (replaces any packet with same
; address if it exists)
;-------------------------------------------------------------------------
; 0xA6 rr xx Program register rr with data xx (1) ! = success
; in register mode 0 = program track no enabled
;-------------------------------------------------------------------------
; 0xA7 rr Read register rr in register mode(2) !,3
; NOTE: cv data followed ! for ok, 0 = program track no enabled
; 0xff followed by 3 for can't read
;-------------------------------------------------------------------------
; 0xA8 aaaa xx Program CV aaaa with data xx (1) ! = success
; in direct mode 0 = program track no enabled
;-------------------------------------------------------------------------
; 0xA9 aaaa Read CV aaaa in direct mode (2) !,3
; NOTE: cv data followed ! for ok, 0 = program track no enabled
; 0xff followed by 3 for can't read
;-------------------------------------------------------------------------
; 0xAA Return software revision number (3) <data1>,<data2>,<data3>
; FORMAT: VV.MM.mm
;-------------------------------------------------------------------------
; 0xAB Perform a soft reset of command (0) Returns nothing
; station (like cycling the power)
;-------------------------------------------------------------------------
; 0xAC Perform a hard reset of command (0) Returns nothing
; station (reset to factory defaults)
;-------------------------------------------------------------------------
; 0xAD <4 data bytes>
; Accy/signal and macro commands (1) !,1
;
; Command Format: 0xAD <addr_h> <addr_l> <op_1> <data_1>
;
; Addr_h and Addr_l are the accessory/signal address (NOT in DCC format).
; Ex: Accessory Address 513 = 0x02 0x01 (hi byte first)
;
; NOTE: accy/signal address 0 is not a valid address
;
; Op_1 Data_1 Operation description
;
; 01 0-255 NCE macro number 0-255
; 02 0-255 Duplicate of Op_1 command
; 03 0 Accessory Normal direction (ON)
; 04 0 Accessory Reverse direction (OFF)
; 05 0-1f Signal Aspect 0-31
; 05-7f reserved reserved
;
;--------------------------------------------------------------------------
; 0xAE <5 data bytes>
; ***USB COMMAND ONLY***
; OPs program loco CV (1) !,0,1,3
;
; Command Format: 0xA2 <addr_h> <addr_l> <CV_h> <CV_l> <data>
;
; addr_h,addr_l are loco address (same as 0xA2 command)
; CV_h, CV_l are cv address high byte first
; data is 8 bit data for CV
;-------------------------------------------------------------------------
; 0xAF <5 data bytes>
; ***USB COMMAND ONLY***
; OPs program accessory/signal (1) !,0,1,3
;
; Command Format: 0xA2 <addr_h> <addr_l> <CV_h> <CV_l> <data>
;
; addr_h,addr_l are accy/sig address (same as 0xAD command)
; CV_h, CV_l are CV address high byte first
; data is 8 bit data for CV
;-------------------------------------------------------------------------
; 0xB0 <4 data bytes> Reserved for future PowerCab use (1) 0=not supported
; ***USB COMMAND ONLY*** !=success
;
;-------------------------------------------------------------------------
; 0xB1 xx (1) 0=not supported
; ***USB COMMAND ONLY*** !=success
; SET cab bus address of USB board to xx
;
; Command Format: 0xB1 <data>
;
;-------------------------------------------------------------------------
; 0xB2 xx (1) 0=not supported
; Enable/disable echo of binary command !=success
; to RS-232 port. This command is used
; when AIU broadcast is enabled so computer
; can distinguish between command response
; or AIU broadcast
;
; Command Format: 0xB2 <data>
;
; 0=no echo, 1=echo 1st byte of command, 2=echo entire command
;
;--------------------------------------------------------------------------
*/

#include "rocdigs/impl/nce_impl.h"

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
    iONCEData data = Data(inst);
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

/** ----- ONCE ----- */


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ) {
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  return 0;
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
static int vmajor = 0;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iONCEData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**
 * The NCE interface supports 9600 and 19200 BAUD
 *
 */
static struct ONCE* _inst( const iONode ini ,const iOTrace trc ) {
  iONCE __NCE = allocMem( sizeof( struct ONCE ) );
  iONCEData data = allocMem( sizeof( struct ONCEData ) );
  MemOp.basecpy( __NCE, &NCEOp, 0, sizeof( struct ONCE ), data );
  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "nce %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  instCnt++;
  return __NCE;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/nce.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
