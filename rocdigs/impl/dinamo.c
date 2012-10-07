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
/** ------------------------------------------------------------
  * Dinamo: http://dinamo.vanperlo.net
  *   Supports version 3.x of the Dinamo protocol.
  * ------------------------------------------------------------
  */

#include "rocdigs/impl/dinamo_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Link.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/State.h"

static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iODINAMOData data = Data(inst);
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

/** ----- ODINAMO ----- */

/** ------------------------------------------------------------
 * proof the integrity of the datagram.
 * (sum(all_datagram_bytes) mod 128) == 0
 */
static Boolean __controlChecksum( byte* datagram ) {
  /* (sum(all_datagram_bytes) mod 128) == 0 */
  int cnt = datagram[0] & CNT_MASK;
  int checksum = datagram[0];
  int i = 0;
  /* add the data bytes: */
  for( i = 0; i < cnt; i++ ) {
    checksum += datagram[i+1];
  }
  /* add the checksum bytes: */
  checksum += datagram[i+1];

  return checksum % 128 == 0 ? True:False;
}


/** ------------------------------------------------------------
 * csum = 128 | (128 - (sum(all_other_bytes) mod 128))
 */
static int __generateChecksum( byte* datagram ) {
  static byte toggle = TOGGLE_FLAG;
  /* csum = 128 | (128 - (sum(all_other_bytes) mod 128)) */
  int cnt = datagram[0] & CNT_MASK;
  int checksum = 0;
  int i = 0;

  /* trace the datagram */
  TraceOp.dump( "datagram", TRCLEVEL_DEBUG, (char*)datagram, cnt+1 );

  toggle ^= TOGGLE_FLAG;
  toggle &= TOGGLE_FLAG;

  datagram[0] |= toggle;

  checksum = datagram[0];

  /* add the data bytes: */
  for( i = 0; i < cnt; i++ ) {
    checksum += datagram[i+1];
    datagram[i+1] |= 0x80; /* mark as data byte */
  }
  /* calculate the checksum: */
  checksum = 128 | (128 - (checksum % 128));

  return checksum | 0x80;
}

static void __mapBlockID(iODINAMO dinamo, iONode node) {
  iODINAMOData data = Data(dinamo);
  char* sAddr = StrOp.fmt( "%d", wSysCmd.getport( node ) );
  if( MapOp.get( data->blockMap, sAddr ) == NULL ) {
    if( wSysCmd.getid( node ) != NULL && StrOp.len(wSysCmd.getid( node )) > 0 )
      MapOp.put( data->blockMap, sAddr, (obj)StrOp.dup(wSysCmd.getid( node )) );
  }
}

/** ------------------------------------------------------------
 * translate rocrail node into dinamo datagram
 */
static int __translate( iODINAMO dinamo, iONode node, byte* datagram, Boolean* response ) {
  iODINAMOData data = Data(dinamo);
  int size = 0;
  *response = False;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "translating: %s", NodeOp.getName( node ) );

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    int cmdval = wSysCmd.getval( node );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "processing system command [%s]", cmdstr );

    if( StrOp.equals( cmdstr, wSysCmd.stop ) || StrOp.equals( cmdstr, wSysCmd.ebreak ) ) {
      data->header |= FAULT_FLAG;
      datagram[0] = 0x00 | data->header;
      datagram[1] = (byte)__generateChecksum( datagram );
      size = 2;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.go ) ||
             StrOp.equals( cmdstr, wSysCmd.reset ) ) {
      data->header &= !FAULT_FLAG;
      datagram[0] = 2 | VER3_FLAG;
      datagram[1] = SYS_CMD;
      datagram[2] = SYS_RESET_FAULT;
      datagram[3] = (byte)__generateChecksum( datagram );
      size = 4;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.hfi ) ) {
      datagram[0] = 3 | VER3_FLAG | data->header;
      datagram[1] = SYS_CMD;
      datagram[2] = SYS_SET_HFI;
      datagram[3] = (byte)wSysCmd.getval( node );
      datagram[4] = (byte)__generateChecksum( datagram );
      size = 5;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.version ) && cmdval == 0 ) {
      datagram[0] = 2 | VER3_FLAG | data->header;
      datagram[1] = SYS_CMD;
      datagram[2] = SYS_GET_VER;
      datagram[3] = (byte)__generateChecksum( datagram );
      size = 4;
      *response = True;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.version ) && cmdval == 1) {
      datagram[0] = 2 | VER3_FLAG | data->header;
      datagram[1] = SYS_CMD;
      datagram[2] = SYS_GET_FIRMVER;
      datagram[3] = (byte)__generateChecksum( datagram );
      size = 4;
      *response = False; /* only 3.1 responds */
    }
    else if( StrOp.equals( cmdstr, wSysCmd.info ) ) {
      int type = wSysCmd.getval( node );
      datagram[0] = 3 | VER3_FLAG | data->header;
      datagram[1] = SYS_CMD;
      datagram[2] = SYS_GET_INFO;
      datagram[3] = (byte)type;
      datagram[4] = (byte)__generateChecksum( datagram );
      size = 5;
      *response = True;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.link ) ) {
      wSysCmd.setport( node, wSysCmd.getvalA( node ) );
      __mapBlockID(dinamo, node);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "link %d(%s) to %d",
          wSysCmd.getvalA( node ), wSysCmd.getid( node ), wSysCmd.getvalB( node ) );
      datagram[0] = 4 | VER3_FLAG | data->header;
      datagram[1] = 0x3A | ((wSysCmd.getvalA( node ) / 128) & 0x01 );
      datagram[2] = wSysCmd.getvalA( node ) % 128;
      datagram[3] = 0x02 | ((wSysCmd.getvalB( node ) / 128) & 0x01 );
      datagram[4] = wSysCmd.getvalB( node ) % 128;
      datagram[5] = (byte)__generateChecksum( datagram );
      size = 6;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.ulink ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "unlink %d", wSysCmd.getvalA( node ) );
      datagram[0] = 3 | VER3_FLAG | data->header;
      datagram[1] = 0x38 | ((wSysCmd.getvalA( node ) / 128) & 0x01 );
      datagram[2] = wSysCmd.getvalA( node ) % 128;
      datagram[3] = 0x04 | 0x02; /* unlink up, clear blocks*/
      datagram[4] = (byte)__generateChecksum( datagram );
      size = 5;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.dcc ) ) {
      __mapBlockID(dinamo, node);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set trackport %d to dcc", wSysCmd.getport( node ) );
      datagram[0] = 3 | VER3_FLAG | data->header;
      datagram[1] = 0x3E | (wSysCmd.getport( node ) / 128) ;
      datagram[2] = wSysCmd.getport( node ) % 128;
      datagram[3] = 0x33; // turn dcc block on
      datagram[4] = (byte)__generateChecksum( datagram );
      size = 5;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.analog ) ) {
      __mapBlockID(dinamo, node);
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set trackport %d to analog", wSysCmd.getport( node ) );
      datagram[0] = 3 | VER3_FLAG | data->header;
      datagram[1] = 0x3E | (wSysCmd.getport( node ) / 128) ;
      datagram[2] = wSysCmd.getport( node ) % 128;
      datagram[3] = 0x53; // turn analog block on
      datagram[4] = (byte)__generateChecksum( datagram );
      size = 5;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.resetblock ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "resetblock %d", wSysCmd.getport( node ) );
      datagram[0] = 3 | VER3_FLAG | data->header;
      datagram[1] = 0x3E | (wSysCmd.getport( node ) / 128) ;
      datagram[2] = wSysCmd.getport( node ) % 128;
      datagram[3] = 0x32; // turn block off, DCC clear all packet info
      datagram[4] = (byte)__generateChecksum( datagram );
      size = 5;
    }
  }

  /* Car command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) &&
           StrOp.equals( wLoc.prot_C, wLoc.getprot( node ) ) )
    {
    int   addr = wLoc.getaddr( node ); /* car decoder address */
    int    dir = wLoc.isdir( node );
    int   mass = wLoc.getmass( node );
    int  speed = 0;
    int  range = 15;


    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = ( wLoc.getV( node ) * range) / 100;
      else  if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * range) / wLoc.getV_max( node );
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "dinamo car speed=%d", speed );
    /*
     Snelheid (0000110) (00AAAAA) (aaaaaaa) (00DSSSS) [(0000 XXX)]
       ·   AAAAAaaaaaaa = decoder adres (1..4095)
       ·   SSSS = snelheid. 0 = stop, 1 = minimum snelheid, 15 = maximaal
       ·   D = richting, 1 = vooruit, 0 = achteruit, momenteel niet gebruikt, dus altijd 1
       ·   XXX = acceleratie-parameter.
    */
    datagram[0] = 5 | VER3_FLAG | data->header;
    datagram[1] = 0x06;
    datagram[2] = addr / 128;
    datagram[3] = addr % 128;
    datagram[4] = (dir ? 0x10:0x00) | (speed & 0x0F);
    datagram[5] = mass & 0x07;
    datagram[6] = (byte)__generateChecksum( datagram );
    size = 7;
  }



  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node ); /* loc decoder address */
    int  block = wLoc.getport( node ); /* block number*/
    int    dir = wLoc.isdir( node );
    int  Vstep = wLoc.getV_step( node );
    int  speed = 0;
    Boolean analog = StrOp.equals( wLoc.prot_A, wLoc.getprot( node ) );
    int  range = analog ? 63:28;
    Boolean longAddr = addr > 127;


    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = ( wLoc.getV( node ) * range) / 100;
      else
        speed = (wLoc.getV( node ) * range) / wLoc.getV_max( node );
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "loco %s (%s %d) speed=%d dir=%s trackport=%d",
        wLoc.getid(node), analog?"analog":"DCC", addr, speed, dir?"fwd":"rev", block );

    if( analog ) {
      datagram[0] = 4 | VER3_FLAG | data->header;
      datagram[1] = 0x22 | (block / 128) ;
      datagram[2] = block % 128;
      datagram[3] = (dir ? 0x00:0x40) | (speed & 0x3F);
      datagram[4] = wLoc.getmass( node ) & 0x7F;
      datagram[5] = (byte)__generateChecksum( datagram );
      size = 6;
    }
    else {
      datagram[0] = (longAddr ? 4:5) | VER3_FLAG | data->header;
      datagram[1] = 0x28 | (block / 128) ;
      datagram[2] = block % 128;
      datagram[3] = 0x40 | (dir ? 0x20:0x00) | (speed & 0x1F);
      datagram[4] = addr % 128; /* base address */
      if( longAddr ) {
        datagram[5] = addr / 128; /* base address */
        datagram[6] = (byte)__generateChecksum( datagram );
      }
      else
        datagram[5] = (byte)__generateChecksum( datagram );
      size = 6;
    }
  }

  /* Car Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) &&
      StrOp.equals( wLoc.prot_C, wLoc.getprot( node ) ) )
  {
    int   addr = wLoc.getaddr( node ); /* car decoder address */

    byte f0 = wLoc.isfn( node )    ? 0x01:0x00;
    byte f1 = wFunCmd.isf1( node ) ? 0x02:0x00;
    byte f2 = wFunCmd.isf2( node ) ? 0x04:0x00;
    byte f3 = wFunCmd.isf3( node ) ? 0x08:0x00;
    byte f4 = wFunCmd.isf4( node ) ? 0x00:0x00;
    byte f5 = wFunCmd.isf5( node ) ? 0x01:0x00;
    byte f6 = wFunCmd.isf6( node ) ? 0x02:0x00;
    byte f7 = wFunCmd.isf7( node ) ? 0x04:0x00;
    byte f8 = wFunCmd.isf8( node ) ? 0x08:0x00;
    /*
    Functies (0000111) (00AAAAA) (aaaaaaa) (000 R L B H) [(000 F4 F3 F2 F1)]
      ·   AAAAAaaaaaaa = decoder adres (1..4095)
      ·   H = Licht (Headlights)
          B = Remlicht (Brake) 1
      ·
      ·   L = richting Links
      ·   R = richting Rechts
      ·   F1..F4 = Aanvullende functies

      Rocrail mapping:
      fn = light
      f1 = brake
      f2 = left blink
      f3 = right blink
      f4 = not used

      f5-f8 -> F1..F4
     */
    datagram[0] = 5 | VER3_FLAG | data->header;
    datagram[1] = 0x07;
    datagram[2] = addr / 128;
    datagram[3] = addr % 128;
    datagram[4] = f0 | f1 | f2 | f3 | f4;
    datagram[5] = f5 | f6 | f7 | f8;
    datagram[6] = (byte)__generateChecksum( datagram );
    size = 7;

  }

  /* Loc Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wLoc.getaddr( node ); /* loc decoder address */
    int  block = wLoc.getport( node ); /* block number*/
    Boolean analog = StrOp.equals( wLoc.prot_A, wLoc.getprot( node ) );
    int fnchanged = wFunCmd.getfnchanged(node);
    int fngroup   = wFunCmd.getgroup(node);
    Boolean longAddr = addr > 127;

    Boolean lights = wLoc.isfn(node);
    Boolean f1 = wFunCmd.isf1( node );
    Boolean f2 = wFunCmd.isf2( node );
    Boolean f3 = wFunCmd.isf3( node );
    Boolean f4 = wFunCmd.isf4( node );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "function %s (%s %d) trackport=%d lights=%s f1=%s f2=%s f3=%s f4=%s",
        wFunCmd.getid(node), analog?"analog":"DCC", addr, block, lights?"on":"off", f1?"on":"off", f2?"on":"off", f3?"on":"off", f4?"on":"off" );

    if( !analog ) {
      byte f0  = wLoc.isfn( node ) ? 0x10:0x00;

      byte f1  = wFunCmd.isf1 ( node ) ? 0x01:0x00;
      byte f2  = wFunCmd.isf2 ( node ) ? 0x02:0x00;
      byte f3  = wFunCmd.isf3 ( node ) ? 0x04:0x00;
      byte f4  = wFunCmd.isf4 ( node ) ? 0x08:0x00;
      byte f5  = wFunCmd.isf5 ( node ) ? 0x01:0x00;
      byte f6  = wFunCmd.isf6 ( node ) ? 0x02:0x00;
      byte f7  = wFunCmd.isf7 ( node ) ? 0x04:0x00;
      byte f8  = wFunCmd.isf8 ( node ) ? 0x08:0x00;
      byte f9  = wFunCmd.isf9 ( node ) ? 0x01:0x00;
      byte f10 = wFunCmd.isf10( node ) ? 0x02:0x00;
      byte f11 = wFunCmd.isf11( node ) ? 0x04:0x00;
      byte f12 = wFunCmd.isf12( node ) ? 0x08:0x00;

      datagram[0] = (longAddr ? 4:5) | VER3_FLAG | data->header;
      datagram[1] = 0x28 | (block / 128) ;
      datagram[2] = block % 128;
      if( fnchanged < 5 || fngroup == 1 ) {
        datagram[3] = f0 | f1 | f2 | f3 | f4;
      }
      else if( fnchanged < 9 || fngroup == 2 ) {
        datagram[3] = 0x30 | f5 | f6 | f7 | f8;
      }
      else if( fnchanged < 13 || fngroup == 3 ) {
        datagram[3] = 0x20 | f9 | f10 | f11 | f12;
      }
      datagram[4] = addr % 128; /* base address */
      if( longAddr ) {
        datagram[5] = addr / 128; /* base address */
        datagram[6] = (byte)__generateChecksum( datagram );
      }
      else
        datagram[5] = (byte)__generateChecksum( datagram );
      size = 6;
    }
    else {

      byte f0 = wLoc.isfn( node ) ? 0x10:0x00;

      datagram[0] = 3 | VER3_FLAG | data->header;
      datagram[1] = 0x20 | (block / 128) ;
      datagram[2] = block % 128;
      datagram[3] = f0;
      datagram[4] = (byte)__generateChecksum( datagram );
      size = 5;

    }
  }

  /* Switch command.
   *
   * 00010CC is for Digital Outputs
   * 00011Cx is for Virtual Outputs
   * 0010Cxx is for Magnetic Devices (currently supported by Rocrail)
   * 0011xxx is for OM32 outputs
   *
   * Note in case of OM32 outputs you also need to send the correct command and
   * in some cases additional parameters
   * To start: OM32 command 8 = output off, OM32 command 9 = output on
   *
   * Er zijn 2 commando's om OM32 berichten te genereren:
   *
   * 0011MMM - mmuuuuu - commando - parameter
   * 0000000 - 11MMMMM - commando - 00uuuuu - parameter
   *
   * MMMMM = Module 0..31 (momenteel wordt 0..15 ondersteund)
   * uuuuu = Uitgang op de module (0..31)
   * Voor bovenstaande comando's staat het Dinamo adresbyte (of header-byte) erachter de checksum.
   * De eerste is de compacte methode
   * De tweede is de send-subsystem methode
   * Het effect is exact identiek.
   * Welke jij nu gebruikt weet ik niet, maar dat moet jij simpel kunnen zien lijkt me.
   * Als je een doorlopende adressering verkiest is de compacte methode het handigst, want MMMMMuuuuu zijn dan gemerged.
   * Als je een subsytem/uitgang addressering verkiest is de send-subsystem methode het handigst, want daar zijn modulenummer en uitgang gescheiden.
   * Send-subsystem heeft verder als voordeel dat als je ook een routine voor OM32serial aansturing hebt je deze commado's simpel kunt omzetten naar de corresponderende Dinamo commando's, namelijk door het adresbyte te delen door 4, er 96 bij op te tellen en er het byte 0 voor te zetten
   *
   * Let op dat het hierboven steeds 7 bits zijn die zijn weergegeven
   *
   * commando 8, parameter 0 = uitgang uit
   * commando 9, parameter 0 = uitgang aan
   * commando 9, parameter 1..63 = puls van parameter/60 seconden
   *
   * Er zijn nog veel meer OM32 commando voor uiteenlopende zaken als het moduleren van uitgangen en het zetten van bitpatronen, maar bovenstaande 2 voldoen voor het zetten van wissels:
   *
   * "single gate" = false:
   * rechtdoor (puls) = commando 9, parameter = tijd(s)*60, uitgang = (port -1) mod 32, module = (port-1)/32
   * afbuigend (puls) = commando 9, parameter = tijd(s)*60, uitgang = port mod 32, module = (port)/32
   * Het bovenstaande doe je nu al, alleen met uitgang = 1 meer dan hierboven staat
   *
   * "single gate" = true:
   * rechtdoor = commando 8, parameter = 0, uitgang = (port -1) mod 32, module = (port-1)/32
   * afbuigend = commando 9, parameter = 0, uitgang = (port -1) mod 32, module = (port-1)/32
   */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int  addr    = wSwitch.getaddr1( node );
    int  port    = wSwitch.getport1( node );
    int  delay   = wSwitch.getdelay( node );
    int  nr      = 0;

    if( delay == 0 )
      delay = data->swtime;

    if( port < 1 || addr < 1 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "address out of range [%d-%d]", addr, port);
      addr = 1;
      port = 1;
    }
    addr--;
    port--;
    nr = addr * 4 + port;

    /* OM32 output */
    if( StrOp.equals( wSwitch.getprot( node ), wSwitch.prot_OM32 ) ) {
      byte command = 0;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "om32 %s [%d-%d] %s",
          wSwitch.getcmd( node ), addr+1, port+1, wSwitch.issinglegate( node )?" (single gate)":"" );

      if( wSwitch.issinglegate( node ) ) {
        command = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 9:8;
        delay = 0;
      }
      else {
        command = 9; /* ON */
        if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
          port++;
      }

      datagram[0] = 4 | VER3_FLAG | data->header;
      datagram[1] = 0x18 | ((addr & 0x1C) >> 2 );
      datagram[2] = ((addr & 0x03) << 5) | (port & 0x1F);
      datagram[3] = command; /* ON command */
      datagram[4] = delay;
      datagram[5] = (byte)__generateChecksum( datagram );
      size = 6;
    }

    /* digital output */
    else if( StrOp.equals( wSwitch.getprot( node ), wSwitch.prot_DO ) ) {
      byte command = 0;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "DO %s [%d-%d]",
          wSwitch.getcmd( node ), addr+1, port+1 );

      if( wSwitch.issinglegate( node ) ) {
        command = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 10:0;

        datagram[0] = 2 | VER3_FLAG | data->header;
        datagram[1] = 0x08 | command;
        datagram[2] = addr & 0x7F;
        datagram[3] = (byte)__generateChecksum( datagram );
        size = 4;
      }
      else {
        command = 9; /* ON */
        if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
          addr++;

        datagram[0] = 3 | VER3_FLAG | data->header;
        datagram[1] = 0x08 | command;
        datagram[2] = addr & 0x7F;
        datagram[3] = delay;
        datagram[4] = (byte)__generateChecksum( datagram );
        size = 5;
      }

    }

    /* virtual output */
    else if( StrOp.equals( wSwitch.getprot( node ), wSwitch.prot_VO ) ) {
      byte command = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 1:0;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "VO %s [%d-%d]",
          wSwitch.getcmd( node ), addr+1, port+1 );

      if( !wSwitch.issinglegate( node ) ) {
        command = 1;
        addr++;
      }
      datagram[0] = 2 | VER3_FLAG | data->header;
      datagram[1] = 0x0C | (command << 1) | ((addr & 0xFF) >> 7);
      datagram[2] = addr & 0x7F;
      datagram[3] = (byte)__generateChecksum( datagram );
      size = 4;
    }

    /* MDD output (default) */
    else {
      byte turnout = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 0x01:0x00;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "MDD %s [%d-%d]",
          wSwitch.getcmd( node ), addr+1, port+1 );

      datagram[0] = 3 | VER3_FLAG | data->header;
      datagram[1] = 0x10 | (turnout << 2) | (nr / 128) ;
      datagram[2] = nr % 128;
      datagram[3] = delay;
      datagram[4] = (byte)__generateChecksum( datagram );
      size = 5;
    }
  }

  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
    size = 0;
  }


  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int   addr = wOutput.getaddr( node );
    int   port = wOutput.getport( node );

    if( StrOp.equals( wOutput.getprot( node ), wOutput.prot_OM32 ) ) {
      /* OM32 output */
      byte param = StrOp.equals( wOutput.on, wOutput.getcmd( node ) ) ? 10:0;
      datagram[0] = 4 | VER3_FLAG | data->header;
      datagram[1] = 0x18 | (addr >> 2);
      datagram[2] = port & 0x1F | ((addr&0x03) << 5);
      datagram[3] = 5; /* linear */
      datagram[4] = param;
      datagram[5] = (byte)__generateChecksum( datagram );
      size = 6;
    }
    else if( StrOp.equals( wOutput.getprot( node ), wOutput.prot_DO ) ) {
      /* digital output */
      byte command = StrOp.equals( wOutput.on, wOutput.getcmd( node ) ) ? 10:0;
      datagram[0] = 2 | VER3_FLAG | data->header;
      datagram[1] = 0x08 | command;
      datagram[2] = addr & 0x7F;
      datagram[3] = (byte)__generateChecksum( datagram );
      size = 4;
    }
    else if( StrOp.equals( wOutput.getprot( node ), wOutput.prot_VO ) ) {
      /* virtual output */
      byte command = StrOp.equals( wOutput.on, wOutput.getcmd( node ) ) ? 1:0;
      datagram[0] = 2 | VER3_FLAG | data->header;
      datagram[1] = 0x0C | (command << 1) | ((addr & 0xFF) >> 7);
      datagram[2] = addr & 0x7F;
      datagram[3] = (byte)__generateChecksum( datagram );
      size = 4;
    }
  }


  /* Block command. */
  else if( StrOp.equals( NodeOp.getName( node ), wBlock.name() ) ) {
    int   addr = wBlock.getport( node ); /* trackport */
    Boolean power = wBlock.ispower( node );

    datagram[0] = 3 | VER3_FLAG | data->header;
    datagram[1] = 0x3E | (addr / 128) ;
    datagram[2] = addr % 128;
    datagram[3] = 0x02 | power ? 0x01:0x00;
    datagram[4] = (byte)__generateChecksum( datagram );
    size = 5;
  }


  /* FeedBack command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int   addr = wFeedback.getaddr( node ); /* feedback address */
    Boolean state = wFeedback.isstate( node );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "simulate fb addr=%d state=%s", addr, state?"true":"false" );
    if( data->listenerFun != NULL && data->listenerObj != NULL )
      data->listenerFun( data->listenerObj, (iONode)NodeOp.base.clone(node), TRCLEVEL_INFO );
    size = 0;
  }

  return size;
}


/** ------------------------------------------------------------
 * generate a rocrail event node and call the event listener
 */
static void __fbEvent( iODINAMO dinamo, byte* datagram ) {
  iODINAMOData data = Data(dinamo);

  Boolean state = (datagram[1]&0x10) ? True:False;
  int addr = (datagram[2] & 0x7F);
  addr |= (datagram[1] & 0x0F) << 7;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "fb %d = %s", addr+1, state?"on":"off" );
  {
    /* inform listener: */
    iONode node = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
    wFeedback.setaddr( node, addr + 1 );
    wFeedback.setstate( node, state );
    if( data->iid != NULL )
      wFeedback.setiid( node, data->iid );

    if( data->listenerFun != NULL && data->listenerObj != NULL )
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }
}


/** ------------------------------------------------------------
 * generate a rocrail event node and call the event listener 4A B0 80 86
 * < Alarm event (01100CB) (bbbbbbb)
 * Dit is een EVENT gegenereerd door Dinamo
 * C=1: blok Bbbbbbbb heeft kortsluiting
 * C=0: kortsluiting van blok Bbbbbbbb is opgeheven
 */
static void __alEvent( iODINAMO dinamo, byte* datagram ) {
  iODINAMOData data = Data(dinamo);

  Boolean shortcircuit = datagram[1]&0x02 ? True:False;
  int block = datagram[2] & 0x7F;
  block |= (datagram[1] & 0x01) << 7;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Alarm Event" );

  {
    char* sAddr = StrOp.fmt("%d", block );
    const char* blockID = (const char*)MapOp.get( data->blockMap, sAddr );
    StrOp.free(sAddr);

    /* Inform listener. */
    TraceOp.trc( name, (shortcircuit ? TRCLEVEL_EXCEPTION:TRCLEVEL_INFO), __LINE__, 9999,
        "block [%s][%d] has %sshort-circuit", blockID!=NULL?blockID:"?", block, shortcircuit ? "":"no longer " );

    iONode node = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
    if( blockID != NULL ) {
      wBlock.setid( node, blockID );
      wBlock.setport( node, block );
      if( data->iid != NULL )
        wBlock.setiid( node, data->iid );
      wBlock.setstate( node, shortcircuit ? wBlock.shortcut:wBlock.shortcutcleared );
      if( data->listenerFun != NULL && data->listenerObj != NULL )
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
  }
}


/** ------------------------------------------------------------
 *  A NULL datagram is created when node == NULL
 */
static int __translateNode2Datagram( iODINAMO dinamo, iONode node, byte* datagram, Boolean* response ) {
  iODINAMOData data = Data(dinamo);
  int size = 0;

  if( node == NULL ) {
    datagram[0] = 0x00 | VER3_FLAG | data->header;
    datagram[1] = (byte)__generateChecksum( datagram );
    size = 2;
    if( response != NULL )
      *response = False;
  }
  else {
    Boolean l_response = False;
    size = __translate( dinamo, node, datagram, response!=NULL?response:&l_response );
  }

  return size;
}


/** ------------------------------------------------------------
 * Check if response maches request.
 */
static Boolean __checkResponse( iODINAMO dinamo, byte* rbuffer ) {
  iODINAMOData data = Data(dinamo);
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Checking response..." );
  {
    int cnt = rbuffer[0] & CNT_MASK;
    char fault = (rbuffer[0]& FAULT_FLAG)?'F':'-';
    if( cnt > 0 ) {
      byte commandoR = rbuffer[1];

      if( commandoR & SYS_CMD == SYS_CMD && cnt > 1 ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "System response 0x%02X", rbuffer[2] );

        if( (rbuffer[2] & 0x60) == 0 ) {

          /*
              >> System Version Request (0000001) (0001010)

              Geeft als antwoord de versie van Dinamo:

              << (0000001) (0001010) (YYYYYYY) (0MMMmmm) (0sssbbb)

              YYYYYYY  = System Type (bv RM-H, RM-U, UCCI)
              MMM = Major Release
              mmm  = Minor Release
              sss  = subrelease
              bbb  = bugfix

              Als System Type zijn op dit moment gedefinieerd:
               1 = RM-H
               2 = RM-U
               10  = UCCI

           */
          if( (rbuffer[2] & 0x7F) == SYS_GET_FIRMVER ) {
            int majorRelease = (rbuffer[4] & 0x38) >> 3;
            int minorRelease = rbuffer[4] & 0x07;
            int subRelease = (rbuffer[5] & 0x38) >> 3;
            int bugfix = rbuffer[5] & 0x07;
            const char* type = "?";
            if( (rbuffer[3] & 0x7F) == 1 )
              type = "RM-H";
            else if( (rbuffer[3] & 0x7F)  == 2 )
              type = "RM-U";
            else if( (rbuffer[3] & 0x7F)  == 10 )
              type = "UCCI";
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%c] %s firmware version %d.%d%d-%d",
              fault, type, majorRelease, minorRelease, subRelease, bugfix );
          }

          /*
              >> Version Info Request (0000001) (0000010)

              Geeft als antwoord de versie van Dinamo:

              << (0000001) (0000010) (0MMMmmm) (0sssbbb)

              MMM = Major Release
              mmm = Minor Release
              sss = subrelease
              bbb = bugfix

           */

          else if( (rbuffer[2] & 0x07) == SYS_GET_VER ) {
            int majorRelease = (rbuffer[3] & 0x38) >> 3;
            int minorRelease = rbuffer[3] & 0x07;
            int subRelease = (rbuffer[4] & 0x38) >> 3;
            int bugfix = rbuffer[4] & 0x07;
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%c] protocol version %d.%d%d-%d",
              fault, majorRelease, minorRelease, subRelease, bugfix );
          }
          else if( (rbuffer[2] & 0x07) == SYS_GET_INFO ) {
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "got system info" );
            TraceOp.dump( "sysinfo", TRCLEVEL_INFO, (char*)rbuffer, cnt+2 );
          }
          else if( (rbuffer[2] & 0x07) == SYS_RESET_FAULT ) {
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "reset fault" );
          }
          else {
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Unknown system response: 0x%02X, %d", rbuffer[2], cnt );
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Unsupported system response: 0x%02X, %d", rbuffer[2], cnt );
        }

      }
      else {
        /* none system command */
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "[%c] response 0x%02X received", fault, commandoR );
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "null datagram received" );
    }
  }
  return True;
}


/**
 *
 */
static void __checkFlags( iODINAMO dinamo, byte* rbuffer ) {
  iODINAMOData data = Data(dinamo);
  byte header = rbuffer[0];
  Boolean fault = False;
  Boolean hold  = False;

  if( !(header & VER3_FLAG) ) {
    /* TODO: version 3 bit is not set */
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Version 3 bit is not set!" );
  }

  if( header & FAULT_FLAG ) fault = True;

  if( header & HOLD_FLAG ) hold = True;

  if( fault != data->fault || hold != data->hold ) {
    data->fault = fault;
    data->hold  = hold;

    if( data->listenerFun != NULL && data->listenerObj != NULL ) {
      iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
      if( data->iid != NULL )
        wState.setiid( node, wDigInt.getiid( data->ini ) );

      wState.setpower( node, data->fault );
      wState.settrackbus( node, data->fault );
      wState.setsensorbus( node, data->fault );
      wState.setaccessorybus( node, data->fault );

      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "state changed" );
    if( data->fault ) {
      /* inform listener: */
      iONode node = NodeOp.inst( wResponse.name(), NULL, ELEMENT_NODE );
      wResponse.setmsg( node, "System signals FAULT" );
      if( data->iid != NULL )
        wResponse.setiid( node, data->iid );

      if( data->listenerFun != NULL && data->listenerObj != NULL ) {
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
      }
    }
  }


}


static void __flush( iODINAMO dinamo ) {
  iODINAMOData o = Data(dinamo);
  int flushed = 0;

  TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "flushing..." );

  if( !o->dummyio ) {
    while( SerialOp.available(o->serial) ) {
      char b;
      SerialOp.read( o->serial, &b, 1 );
      flushed++;
      ThreadOp.sleep(10);
    };
  }

  TraceOp.trc(name, TRCLEVEL_MONITOR, __LINE__, 9999, "flushed %d bytes", flushed);
}


/** ------------------------------------------------------------
 * datagram pump thread
 */
static void __transactor( void* threadinst ) {
  iOThread       th = (iOThread)threadinst;
  iODINAMO   dinamo = (iODINAMO)ThreadOp.getParm(th);
  iODINAMOData data = Data(dinamo);
  Boolean        ok = True;
  Boolean    gotrsp = True;

  byte lastdatagram[32];
  int lastdatagramsize = 0;
  
  byte wbuffer[32];
  byte rbuffer[32];
  int wsize = 0; /* request size  */
  int dsize = 0; /* data size     */
  int timer = 0;

  ThreadOp.setDescription( th, "Transactor for Dinamo 3.x" );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor started: the datagram pump." );
  
  __flush( dinamo );

  ThreadOp.setHigh( th );
  
  timer = SystemOp.getTick();

  do {
    obj post = NULL;

    /* get next node */
    if( gotrsp && lastdatagramsize == 0 ) {
      post = ThreadOp.getPost( th );
      if( post != NULL ) {
        iONode node = (iONode)post;
        Boolean responseExpected = False;

        /* create datagram from node and send to Dinamo: */
        wsize = __translateNode2Datagram( dinamo, node, wbuffer, &responseExpected );

        if( responseExpected ) {
          /* TODO: put the request in a list to be wached for a matching response. */
        }
        if( wsize > 0 ) {
          TraceOp.dump( "cmdreq", TRCLEVEL_BYTE, (char*)wbuffer, wsize );
          MemOp.copy(lastdatagram, wbuffer, wsize);
          lastdatagramsize = responseExpected ? wsize:0;
          if( !data->dummyio ) {
            SerialOp.write( data->serial, (char*)wbuffer, wsize );
            timer = SystemOp.getTick();
            gotrsp = False;
          }
        }

        /* Cleanup: endstation for all nodes. */
        node->base.del( node );
      }
    }

    if( !data->dummyio ) {
      if( !gotrsp && lastdatagramsize > 0 && (SystemOp.getTick() - timer) > 20 ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "resend last datagram size=%d timer=%d", lastdatagramsize, timer );
        TraceOp.dump( "lastdatagram", TRCLEVEL_BYTE, (char*)lastdatagram, lastdatagramsize );
        SerialOp.write( data->serial, (char*)lastdatagram, lastdatagramsize );
        timer = SystemOp.getTick();
        gotrsp = False;
      }

      if(gotrsp || (SystemOp.getTick() - timer) > 25 ) {
        int  lsize = 0;
        byte lbuffer[32]; /* make a local send buffer to preserve the datagram for checking */
        /* Send NULL datagram to signal Rocrail is still a live: */
        lsize = __translateNode2Datagram( dinamo, NULL, lbuffer, NULL );
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "send null datagram size=%d", lsize );
        TraceOp.dump( "nullreq", TRCLEVEL_BYTE, (char*)lbuffer, lsize );
        SerialOp.write( data->serial, (char*)lbuffer, lsize );
        lastdatagramsize = 0;
        gotrsp = False;
      }
    }

    /* check if there is a response waiting: */
    dsize = 0;
    ok = False;
    if( !data->dummyio && SerialOp.available(data->serial) > 0 ) {
      MemOp.set( rbuffer, 0, 32 );
      do {
        /* check if it is the start of the datagram */
        ok = SerialOp.read( data->serial, (char*)rbuffer, 1 );
      } while( ok && (rbuffer[0] & 0x80) != 0 && SerialOp.available(data->serial) );

      if( ok  && (rbuffer[0] & 0x80) == 0 ) {
        dsize = rbuffer[0] & CNT_MASK;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "number of waiting data bytes = %d", dsize );
        if( dsize <= 7 ) {
          int ismore = 0;
          if( !data->dummyio ) {
            ok = SerialOp.read( data->serial, (char*)rbuffer+1, dsize+1 );
            if( ok ) {
              TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "%d bytes read in buffer:", dsize + 2 );
              TraceOp.dump( "cmdrsp", TRCLEVEL_BYTE, (char*)rbuffer, dsize + 2 );
            }
            ismore = SerialOp.available(data->serial);
            if( ismore > 0 )
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "more bytes in read buffer! %d", ismore );
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "number of data bytes exceeds allowed quantity! %d", dsize );
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
            "no startbyte = 0x%02X  rbuffer[0] & 0x80 = 0x%02X (ok=%d gotrsp=%d)", rbuffer[0], (rbuffer[0] & 0x80), ok, gotrsp );
        ok = False;
      }
    }


    /* verify checksum: */
    if( ok ) {
      ok = __controlChecksum( rbuffer );
    }
    /* check flags of header byte: */
    if( ok ) {
      __checkFlags( dinamo, rbuffer );
    }


    if( ok ) {
      gotrsp = True;
      /* check for events: */

      if( dsize > 0 && (rbuffer[1] & 0x60) == 0x40 ) {
        /* Switch event. (Feedback) */
        __fbEvent( dinamo, rbuffer );
      }

      else if( dsize > 0 && (rbuffer[1] & 0x60) == 0x60 ) {
        /* Switch response. (Feedback) */
        __fbEvent( dinamo, rbuffer );
      }

      else if( dsize > 0 && (rbuffer[1] & 0x7C) == 0x30 ) {
        /* Alarm event. */
        __alEvent( dinamo, rbuffer );
      }

      /* No event, so it must be a response: */
      else {
        /* response should be received within 200ms, otherwise flip the toggle bit and resend: */
        /* if no response will come within 2 seconds: Panic!
           Maybe switched off or lost connection? */
        __checkResponse( dinamo, rbuffer );
        if( dsize > 0 )
          lastdatagramsize = 0;
      }

      timer = SystemOp.getTick();
    }


    /* Give up timeslize: */
    ThreadOp.sleep( data->dummyio?1000:10 );
  } while( data->run );


  ThreadOp.base.del( th );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor ended." );
}




/** ------------------------------------------------------------
 * command from rocrail
 */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iODINAMOData data = Data(inst);
  ThreadOp.post( data->transactor, (obj)cmd );
  return 0;
}


/** ------------------------------------------------------------
 * shutdown
 */
static void _halt( obj inst, Boolean poweroff ) {
  iODINAMOData data = Data(inst);
  data->header |= FAULT_FLAG;
  /* TODO: shutdown? */
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "halt command received" );
  data->run = False;
  return;
}


/** ------------------------------------------------------------
 * register event listener
 */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iODINAMOData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


static Boolean _setRawListener(obj inst, obj listenerObj, const digint_rawlistener listenerFun ) {
  return True;
}

static byte* _cmdRaw( obj inst, const byte* cmd ) {
  return NULL;
}

/** ------------------------------------------------------------
 * support programming track?
 */
static Boolean _supportPT( obj inst ) {
  return False;
}


/* Status */
static int _state( obj inst ) {
  iODINAMOData data = Data(inst);
  int state = 0;
  return state;
}

/* external shortcut event */
static void _shortcut(obj inst) {
  iODINAMOData data = Data( inst );
}


/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iODINAMOData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

/** ------------------------------------------------------------
 * constructor of dinamo support
 */
static struct ODINAMO* _inst( const iONode ini ,const iOTrace trc ) {
  iODINAMO __DINAMO = allocMem( sizeof( struct ODINAMO ) );
  iODINAMOData data = allocMem( sizeof( struct ODINAMOData ) );
  MemOp.basecpy( __DINAMO, &DINAMOOp, 0, sizeof( struct ODINAMO ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini = ini;
  data->iid = StrOp.dup( wDigInt.getiid( ini ) );
  data->swtime = wDigInt.getswtime( ini );
  data->dummyio = wDigInt.isdummyio( ini );
  data->blockMap = MapOp.inst();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "dinamo %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  IID     [%s]", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  device  [%s]", wDigInt.getdevice( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  timeout [%d]", wDigInt.gettimeout( ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  swtime  [%d]", data->swtime );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );


  if( !data->dummyio ) {
    data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );

    SerialOp.setFlow( data->serial, none );
    SerialOp.setLine( data->serial, 19200, 8, 1, odd, wDigInt.isrtsdisabled( ini ) );
    SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );
  }


  if( data->dummyio || SerialOp.open( data->serial ) ) {
    SystemOp.inst();
    SerialOp.flush(data->serial);
    data->run = True;
    data->transactor = ThreadOp.inst( "transactor", &__transactor, __DINAMO );
    ThreadOp.start( data->transactor );
    ThreadOp.sleep( 100 );

    { /* request dinamo version: */
      iONode cmd = NULL;

      cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
      wSysCmd.setcmd( cmd, wSysCmd.reset );
      ThreadOp.post( data->transactor, (obj)cmd );

      cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
      wSysCmd.setcmd( cmd, wSysCmd.version );
      ThreadOp.post( data->transactor, (obj)cmd );

      cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
      wSysCmd.setcmd( cmd, wSysCmd.version );
      wSysCmd.setval( cmd, 1 );
      ThreadOp.post( data->transactor, (obj)cmd );

      cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
      wSysCmd.setcmd( cmd, wSysCmd.hfi );
      wSysCmd.setval( cmd, 10 );
      ThreadOp.post( data->transactor, (obj)cmd );

/*
      cmd = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
      wFeedback.setpin( cmd, 1 );
      ThreadOp.post( data->transactor, (obj)cmd );
*/
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init dinamo port! (%)", wDigInt.getdevice( ini ) );
  }

  instCnt++;
  return __DINAMO;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/dinamo.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
