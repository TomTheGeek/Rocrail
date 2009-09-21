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


/** ------------------------------------------------------------
 * translate rocrail node into dinamo datagram
 */
static int __translate( iODINAMO dinamo, iONode node, byte* datagram, Boolean* response ) {
  iODINAMOData data = Data(dinamo);
  int size = 0;
  *response = True;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "translating: %s", NodeOp.getName( node ) );

  /* System command. */
  if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "translating: cmd=%s", cmdstr );

    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      data->header |= FAULT_FLAG;
      datagram[0] = 0x00 | data->header;
      datagram[1] = (byte)__generateChecksum( datagram );
      size = 2;
      *response = False;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.go ) ||
             StrOp.equals( cmdstr, wSysCmd.reset ) ) {
      data->header &= !FAULT_FLAG;
      datagram[0] = 2 | VER3_FLAG;
      datagram[1] = SYS_CMD;
      datagram[2] = SYS_RESET_FAULT;
      datagram[3] = (byte)__generateChecksum( datagram );
      size = 4;
      *response = False;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.hfi ) ) {
      datagram[0] = 3 | VER3_FLAG | data->header;
      datagram[1] = SYS_CMD;
      datagram[2] = SYS_SET_HFI;
      datagram[3] = (byte)wSysCmd.getval( node );
      datagram[4] = (byte)__generateChecksum( datagram );
      size = 5;
      *response = False;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.version ) ) {
      datagram[0] = 2 | VER3_FLAG | data->header;
      datagram[1] = SYS_CMD;
      datagram[2] = SYS_GET_VER;
      datagram[3] = (byte)__generateChecksum( datagram );
      size = 4;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.info ) ) {
      int type = wSysCmd.getval( node );
      datagram[0] = 3 | VER3_FLAG | data->header;
      datagram[1] = SYS_CMD;
      datagram[2] = SYS_GET_INFO;
      datagram[3] = (byte)type;
      datagram[4] = (byte)__generateChecksum( datagram );
      size = 5;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.link ) ) {
      /* TODO: */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "link %d to %d", wSysCmd.getvalA( node ), wSysCmd.getvalB( node ) );
      datagram[0] = 4 | VER3_FLAG | data->header;
      datagram[1] = 0x3A | ((wSysCmd.getvalA( node ) / 128) & 0x01 );
      datagram[2] = wSysCmd.getvalA( node ) % 128;
      datagram[3] = 0x02 | ((wSysCmd.getvalB( node ) / 128) & 0x01 );
      datagram[4] = wSysCmd.getvalB( node ) % 128;
      datagram[5] = (byte)__generateChecksum( datagram );
      size = 6;
    }
    else if( StrOp.equals( cmdstr, wSysCmd.ulink ) ) {
      /* TODO: */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "unlink %d", wSysCmd.getvalA( node ) );
      datagram[0] = 3 | VER3_FLAG | data->header;
      datagram[1] = 0x38 | ((wSysCmd.getvalA( node ) / 128) & 0x01 );
      datagram[2] = wSysCmd.getvalA( node ) % 128;
      datagram[3] = 0x04 | 0x02; /* unlink up, clear blocks*/
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
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "dinamo car speed=%d", speed );
    }
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


    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = ( wLoc.getV( node ) * range) / 100;
      else
        speed = (wLoc.getV( node ) * range) / wLoc.getV_max( node );
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "dinamo speed=%d", speed );
    }

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
      datagram[0] = 4 | VER3_FLAG | data->header;
      datagram[1] = 0x28 | (block / 128) ;
      datagram[2] = block % 128;
      datagram[3] = 0x40 | (dir ? 0x20:0x00) | (speed & 0x1F);
      datagram[4] = addr; /* base address */
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

    if( ! analog ) {
      byte f0 = wLoc.isfn( node ) ? 0x10:0x00;
      byte f1 = wFunCmd.isf1( node ) ? 0x01:0x00;
      byte f2 = wFunCmd.isf2( node ) ? 0x02:0x00;
      byte f3 = wFunCmd.isf3( node ) ? 0x04:0x00;
      byte f4 = wFunCmd.isf4( node ) ? 0x08:0x00;

      datagram[0] = 4 | VER3_FLAG | data->header;
      datagram[1] = 0x28 | (block / 128) ;
      datagram[2] = block % 128;
      datagram[3] = f0 | f1 | f2 | f3 | f4;
      datagram[4] = addr; /* base address */
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
    int   addr = wBlock.getaddr( node ); /* block address */
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

    datagram[0] = 2 | VER3_FLAG | data->header;
    datagram[1] = 0x60 | (addr / 128) ;
    datagram[2] = addr % 128;
    datagram[3] = (byte)__generateChecksum( datagram );
    size = 4;
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
 * generate a rocrail event node and call the event listener
 */
static void __alEvent( iODINAMO dinamo, byte* datagram ) {
  iODINAMOData data = Data(dinamo);

  Boolean shortcircuit = datagram[1]&0x02 ? True:False;
  int block = datagram[2] & 0x7F;
  block |= (datagram[1] & 0x01) << 7;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Alarm Event" );

  if( shortcircuit )
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "block %d has short-circuit", block );
  {
    /* inform listener: */
    /* TODO: */
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

          if( (rbuffer[2] & 0x07) == SYS_GET_VER ) {
            int majorRelease = (rbuffer[3] & 0x38) >> 3;
            int minorRelease = rbuffer[3] & 0x07;
            int subRelease = (rbuffer[4] & 0x38) >> 3;
            int bugfix = rbuffer[4] & 0x07;
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "[%c] version %d.%d%d-%d",
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
    if( lastdatagramsize == 0 ) {
      post = ThreadOp.getPost( th );
      if( post != NULL ) {
        iONode node = (iONode)post;
        Boolean responseExpected = False;

        /* create datagram from node and send to Dinamo: */
        wsize = __translateNode2Datagram( dinamo, node, wbuffer, &responseExpected );

        if( responseExpected ) {
          /* TODO: put the request in al list to be wached for a matching response. */
        }
        if( wsize > 0 ) {
          TraceOp.dump( "cmdreq", TRCLEVEL_BYTE, (char*)wbuffer, wsize );
          MemOp.copy(lastdatagram, wbuffer, wsize);
          lastdatagramsize = responseExpected ? wsize:0;
          if( !data->dummyio )
            SerialOp.write( data->serial, (char*)wbuffer, wsize );
        }

        /* Cleanup: endstation for all nodes. */
        node->base.del( node );
      }
    }

    if( !data->dummyio && !SerialOp.available(data->serial) ) {
      if( SystemOp.getTick() - timer >= 20 ) {
        if( lastdatagramsize > 0 ) {
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "resend last datagram size=%d timer=%d", lastdatagramsize, timer );
          TraceOp.dump( "lastdatagram", TRCLEVEL_BYTE, (char*)lastdatagram, lastdatagramsize );
          SerialOp.write( data->serial, (char*)lastdatagram, lastdatagramsize );
        }
        else {
          int  lsize = 0;
          byte lbuffer[32]; /* make a local send buffer to preserve the datagram for checking */
          /* Send NULL datagram to signal Rocrail is still a live: */
          lsize = __translateNode2Datagram( dinamo, NULL, lbuffer, NULL );
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "send null datagram size=%d", lsize );
          TraceOp.dump( "nullreq", TRCLEVEL_BYTE, (char*)lbuffer, lsize );
          SerialOp.write( data->serial, (char*)lbuffer, lsize );
        }
        timer = SystemOp.getTick();
      }
    }

    /* check if there is a response waiting: */
    dsize = 0;
    ok = False;
    if( (!data->dummyio && SerialOp.available(data->serial)) ) {

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
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "no startbyte = 0x%02X  rbuffer[0] & 0x80 = 0x%02X (ok=%d)", rbuffer[0], (rbuffer[0] & 0x80), ok );
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
    ThreadOp.sleep( data->dummyio?1000:1 );
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
static void _halt( obj inst ) {
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
static int vmajor = 1;
static int vminor = 3;
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
  data->swtime = (wDigInt.getswtime( ini ) * 60) / 1000; /* units of 1/60 sec. */
  data->dummyio = wDigInt.isdummyio( ini );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "dinamo %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Init serial %s", wDigInt.getdevice( ini ) );

  if( !data->dummyio ) {
    data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );

    SerialOp.setFlow( data->serial, -1 );
    SerialOp.setLine( data->serial, 19200, 8, 1, odd );
    SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), 200 );
  }


  if( data->dummyio || SerialOp.open( data->serial ) ) {
    SystemOp.inst();
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
