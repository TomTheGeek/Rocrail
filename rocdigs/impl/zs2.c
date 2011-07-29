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


#include "rocdigs/impl/zs2_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/FbInfo.h"
#include "rocrail/wrapper/public/FbMods.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOZS2Data data = Data(inst);
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
  iOZS2Data data = Data(inst);
  return NULL;
}

static const char* __id( void* inst ) {
  iOZS2Data data = Data(inst);
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  iOZS2Data data = Data(inst);
  return NULL;
}

/** ----- OZS2 ----- */
static iOPoint __getPointByAddr(iOZS2Data data, int bus, int addr, int port) {
  char key[32] = {'\0'};
  iOPoint point = NULL;

  StrOp.fmtb( key, "%d_%d_%d", bus, addr, port );

  if( MutexOp.wait( data->pointmux ) ) {
    point = (iOPoint)MapOp.first( data->pointmap);
    while( point != NULL ) {
      if( point->bus == bus && point->addr == addr && point->port == port) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "point found for %s by address %s", point->id, key );
        break;
      }
      point = (iOPoint)MapOp.next( data->pointmap);
    };
    MutexOp.post(data->pointmux);
  }
  return point;
}


static iOPoint __getPoint(iOZS2Data data, iONode node) {
  int bus   = wSwitch.getbus(node);
  int addr  = wSwitch.getaddr1(node);
  int port  = wSwitch.getport1(node);
  char key[32] = {'\0'};
  iOPoint point = NULL;

  StrOp.fmtb( key, "%d_%d_%d", bus, addr, port );

  point = (iOPoint)MapOp.get( data->pointmap, key );
  if( point != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "point exist for %s", key );
    return point;
  }

  point = allocMem( sizeof( struct point) );
  point->bus  = bus;
  point->addr = addr;
  point->port = port;
  point->id = StrOp.dup(wSwitch.getid(node));
  if( MutexOp.wait( data->pointmux ) ) {
    MapOp.put( data->pointmap, key, (obj)point);
    MutexOp.post(data->pointmux);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "point created for %s", key );
  return point;
}


static const char* __getSlotID(iOZS2Data data, int slotnr, int bus) {
  iOSlot slot = NULL;
  if( MutexOp.wait( data->lcmux ) ) {
    slot = (iOSlot)MapOp.first( data->lcmap);
    while(  slot != NULL ) {
      if( slot->bus == bus && slot->nr == slotnr ) {
        break;
      }
      slot = (iOSlot)MapOp.next( data->lcmap);
    };
    MutexOp.post(data->lcmux);
  }

  if( slot != NULL )
    return slot->id;

  return "";
}


static void __dumpZS2Slots(iOZS2Data data) {
  int i = 0;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "S#|PA AH AL DS F1 F9 ID" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "--+--------------------------------------" );
  for( i = 0; i < 16; i++ ) {

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "%02d|%02X %02X %02X %02X %02X %02X %s",
        i, data->sx[2][i*6+0], data->sx[2][i*6+1], data->sx[2][i*6+2],
        data->sx[2][i*6+3], data->sx[2][i*6+4], data->sx[2][i*6+5], __getSlotID(data, i, 2) );
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "--+--------------------------------------" );
}


static iOSlot __getSlotByAddr(iOZS2Data data, int lcaddr, Boolean sx2) {
  iOSlot slot = NULL;
  if( MutexOp.wait( data->lcmux ) ) {
    slot = (iOSlot)MapOp.first( data->lcmap);
    while( slot != NULL ) {
      if( sx2 ) {
		    if( slot->nr == (lcaddr/6) ) {
		      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot found for %s by nr %d", slot->id, lcaddr/6 );
		      break;
		    }
      }
      else if( slot->addr == lcaddr ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot found for %s by address %d", slot->id, lcaddr );
        break;
      }
      slot = (iOSlot)MapOp.next( data->lcmap);
    };
    MutexOp.post(data->lcmux);
  }
  
  if( sx2 && slot == NULL ) {
    /* TODO: create a slot or ignore? */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "undefined sx2 slotnr: %d", lcaddr/6 );
  }
  
  return slot;
}


static int __getFreeSlot(iOZS2Data data) {
  int i = 0;
  for( i = 0; i < 16; i++ ) {
    if( data->sx[2][i*6] == 0) {
      return i + 1;
    }
  }
  return -1;
}

static int __getSlotNr4Addr(iOZS2Data data, byte preamble, byte addrh, byte addrl) {
  int i = 0;
  for( i = 0; i < 16; i++ ) {
    if( data->sx[2][i*6 + 0] == preamble && data->sx[2][i*6 + 1] == addrh && (data->sx[2][i*6 + 2]&0xFC) == (addrl&0xFC) ) {
      return i;
    }
  }
  return -1;
}


static iOSlot __getSlot(iOZS2Data data, iONode node) {
  int addr  = wLoc.getaddr(node);
  iOSlot slot = NULL;
  Boolean longAddr = False;
  Boolean dcc      = False;
  int preamble = 0;
  int speed = 0;

  slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot exist for %s", wLoc.getid(node) );
    return slot;
  }

  slot = allocMem( sizeof( struct slot) );
  slot->addr   = addr;
  slot->bus    = wLoc.getbus(node);
  slot->id     = StrOp.dup(wLoc.getid(node));
  slot->steps  = wLoc.getspcnt(node);
  slot->lights = True;
  slot->dir    = wLoc.isdir(node);
  slot->fx1    = wLoc.getfx(node) & 0x00FF;
  slot->fx2    = (wLoc.getfx(node) & 0xFF00) >> 8;
  
  if( slot->bus != 2 && addr > 99 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "use sx2 for %s because address %d > 99", wLoc.getid(node), addr );
    slot->bus = 2;
  }

  if( slot->bus == 2 ) {
    slot->sx2 = True;
    
    if( StrOp.equals( wLoc.prot_N, wLoc.getprot(node) ) && slot->addr < 100 ) {
      /* DCC short addresses */
      longAddr = False;
      dcc = True;
      if( slot->steps == 14 )
        preamble = 1;
      else if( slot->steps == 28 )
        preamble = 1;
      else
        preamble = 5;
    }
    else if( StrOp.equals( wLoc.prot_L, wLoc.getprot(node) ) || slot->addr > 100 ) {
      /* DCC long addresses */
      longAddr = True;
      dcc = True;
      if( slot->steps == 14 )
        preamble = 3;
      else if( slot->steps == 28 )
        preamble = 3;
      else
        preamble = 7;
    }
    else {
      /* SX2 */
      longAddr = False;
      dcc = False;
      preamble = 4;
    }
    
  }
  
  if( wLoc.getV( node ) != -1 ) {
    if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
      speed = (wLoc.getV( node ) * slot->steps) / 100;
    else if( wLoc.getV_max( node ) > 0 )
      speed = (wLoc.getV( node ) * slot->steps) / wLoc.getV_max( node );
  }

  /*
		Lokadressen
		Zur Übertragung der (dezimal) 4-stelligen Lokadresse werden 2
		Bitgruppen à 7 Bit benutzt um jeweils die Dezimalzahlen von 0 bis
		99 (eigentlich bis 127) darzustellen. Bei der 2-mal-7-bit Lokadres-
		se (Bit a13 bis a0) entsprechen die Bits a13 bis a7 der Tausender-
		und Hunderterstelle, die Bits a6 bis a0 entsprechen den Zehnern
		und Einern. Sie wird in 2 Bytes geteilt übertragen. Das „High-Byte“
		beinhaltet die oberen 8 bit a13 bis a6 der Lokadresse, das „Low-
		Byte“ beinhaltet die unteren 6 bit a5 bis a0 & 2 Lichtbits, von de-
		nen im Standardfall nur das obere Lichtbit relevant ist.
  
		Soll beispielsweise die (dezimale) Adresse 2250 mit (Standard-)
		Licht an gesendet oder empfangen werden, so haben High- und
		Low-Byte folgenden Inhalt:

		High-Byte = 00101100 (= 2 x 22 + (50 > 63?)),
		Low-Byte  = 11001010 (= 4 x 50 + 2 x (Std-Licht an?) + (Zusatzlichan?).
  */
  
  if(slot->sx2 ) { 
    byte addrh = 0;
    byte addrl = 0;
    if( dcc ) {
      addrh = (slot->addr >> 6);
      addrl = (slot->addr << 2)  + (slot->lights ? 2:0) + (slot->fn ? 1:0);
    }
    else {
      addrh = (2 * (slot->addr/100)) + ((slot->addr%100) > 63 ? 1:0);
      addrl = ((4 * (slot->addr%100)) & 0xFF) + (slot->lights ? 2:0) + (slot->fn ? 1:0);
    }
    int slotnr = __getSlotNr4Addr(data, preamble, addrh, addrl);
    if( slotnr != -1 ) {
      slot->nr = slotnr;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "using SX2 slot %d for %s, preamble=%d, addr=%d",
          slot->nr, wLoc.getid(node), preamble, slot->addr );
    }
    else {
      slotnr = __getFreeSlot(data);
      if( slotnr != -1 ) {
        byte* cmd = allocMem(32);
        cmd[ 0] = 2;
        cmd[ 1] = 12;
        cmd[ 2] = slot->nr * 6 + 0 + WRITE_FLAG;
        cmd[ 3] = preamble;
        cmd[ 4] = slot->nr * 6 + 1 + WRITE_FLAG;
        cmd[ 5] = addrh;
        cmd[ 6] = slot->nr * 6 + 2 + WRITE_FLAG;
        cmd[ 7] = addrl;
        cmd[ 8] = slot->nr * 6 + 3 + WRITE_FLAG;
        cmd[ 9] = speed + (slot->dir ? 0x00:0x80);
        cmd[10] = slot->nr * 6 + 4 + WRITE_FLAG;
        cmd[11] = slot->fx1;
        cmd[12] = slot->nr * 6 + 5 + WRITE_FLAG;
        cmd[13] = slot->fx2;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "create SX2 slot for %s, preamble=%d, addr=%d (high=%02X low=%02X)",
            wLoc.getid(node), preamble, slot->addr, cmd[ 5], cmd[ 7] );
        ThreadOp.post(data->writer, (obj)cmd);
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no free sx2 slot for %s", wLoc.getid(node) );
        freeMem(slot);
        return NULL;
      }
    }
  }

  if( MutexOp.wait( data->lcmux ) ) {
    MapOp.put( data->lcmap, wLoc.getid(node), (obj)slot);
    MutexOp.post(data->lcmux);
  }


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for %s", wLoc.getid(node) );
  if( slot->bus == 2 )
    __dumpZS2Slots(data);
  return slot;
}




/* fbmods is a comman separated address list of connected feedback modules. */
static void __updateFB( iOZS2 zs2, iONode fbInfo ) {
  iOZS2Data data = Data(zs2);
  int cnt = NodeOp.getChildCnt( fbInfo );
  int i = 0;

  char* str = NodeOp.base.toString( fbInfo );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "updateFB\n%s", str );
  StrOp.free( str );

  /* reset the list: */
  MemOp.set( data->fbmodcnt, 0, 2 * sizeof(int) );
  MemOp.set( data->fbmods, 0, 2*256 );

  MapOp.clear(data->fbmap);

  for( i = 0; i < cnt; i++ ) {
    iONode fbmods = NodeOp.getChild( fbInfo, i );
    const char* mods = wFbMods.getmodules( fbmods );
    int bus = wFbMods.getbus( fbmods );
    if( mods != NULL && StrOp.len( mods ) > 0 ) {

      iOStrTok tok = StrTokOp.inst( mods, ',' );
      int idx = 0;
      while( StrTokOp.hasMoreTokens( tok ) ) {
        byte *cmd = NULL;
        char key[32] = {'\0'};
        int addr = atoi( StrTokOp.nextToken(tok) );
        data->fbmods[bus][idx] = addr & 0x7f;

        /* create key */
        StrOp.fmtb(key, "%d_%d", bus, addr );
        MapOp.put( data->fbmap, key, (obj)&data->fbmods[bus][idx]); /* dummy object */

        idx++;
      };
      data->fbmodcnt[bus] = idx;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "updateFB bus=%d count=%d", bus, idx );
    }
  }

}


static void __translate( iOZS2 zs2, iONode node ) {
  iOZS2Data data = Data(zs2);

   if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
    __updateFB( zs2, node );
  }
  

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.stop ) || StrOp.equals( cmdstr, wSysCmd.ebreak ) ) {
      /* CS on */
      byte* cmd = allocMem(32);
      cmd[0] = 0;
      cmd[1] = 2;
      cmd[2] = 127;
      cmd[2] |= WRITE_FLAG;
      cmd[3] = 0x00;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power OFF" );
      ThreadOp.post(data->writer, (obj)cmd);
    }
    if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS on */
      byte* cmd = allocMem(32);
      cmd[0] = 0;
      cmd[1] = 2;
      cmd[2] = 127;
      cmd[2] |= WRITE_FLAG;
      cmd[3] = 0x80;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power ON" );
      ThreadOp.post(data->writer, (obj)cmd);
    }
    if( StrOp.equals( cmdstr, wSysCmd.slots ) ) {
      __dumpZS2Slots(data);
    }
  }
  
  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    byte pin = 0x01 << ( wSwitch.getport1( node ) - 1 );
    byte mask = ~pin;
    int bus = wSwitch.getbus( node ) & 0x1F;

    iOPoint point = __getPoint(data, node);
    if( point != NULL ) {
      point->lastcmd = SystemOp.getTick();
    }

    byte *cmd = allocMem(32);
    cmd[0] = bus;
    cmd[1] = 2;
    cmd[2] = wSwitch.getaddr1( node ) & 0x7F;
    cmd[3] = 0x01 << ( wSwitch.getport1( node ) - 1 );
    cmd[2] |= WRITE_FLAG;

    /* reset pin to 0: */
    cmd[3] = data->swstate[bus][cmd[2]] & mask;

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      cmd[3] |= pin;
    /* save new state: */
    data->swstate[bus][cmd[2]] = cmd[3];
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch %d", wSwitch.getaddr1( node ) );
    ThreadOp.post(data->writer, (obj)cmd);
  }

  /* Output command */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;
    byte pin = 0x01 << ( port - 1 );
    byte mask = ~pin;

    int bus = wOutput.getbus(node);
    byte *cmd = allocMem(32);
    cmd[0] = bus;
    cmd[1] = 2;
    cmd[2] = addr & 0x7F;
    cmd[2] |= WRITE_FLAG;
    /* reset pin to 0: */
    cmd[3] = data->swstate[bus][cmd[2]] & mask;

    if( action )
      cmd[3] |= pin;
    /* save new state: */
    data->swstate[bus][cmd[2]] = cmd[3];
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "output %d, %d", wOutput.getaddr( node ), wOutput.getport( node ) );
    ThreadOp.post(data->writer, (obj)cmd);
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    Boolean fn  = wLoc.isfn( node );
    Boolean dir = wLoc.isdir( node ); /* True == forwards */

    iOSlot slot = __getSlot(data, node );

    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get slot for loco %s", wLoc.getid(node) );
      return;
    }

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * slot->steps) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * slot->steps) / wLoc.getV_max( node );
    }

		slot->speed = speed;
		slot->dir = wLoc.isdir(node);
		slot->lights = wLoc.isfn(node);

		if(slot->sx2 ) { 
			byte* cmd = allocMem(32);
			cmd[ 0] = 2;
			cmd[ 1] = 2;
			cmd[ 2] = slot->nr * 6 + 3 + WRITE_FLAG;
			cmd[ 3] = speed + (slot->dir ? 0x00:0x80);
		  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SX2 loco %d", addr );
			ThreadOp.post(data->writer, (obj)cmd);
		}
		else {
		  byte *cmd = allocMem(32);
		  cmd[0] = slot->bus;
		  cmd[1] = 2;
		  cmd[2] = addr & 0x7F;
		  cmd[2] |= WRITE_FLAG;
		  cmd[3] = speed & 0x1F;
		  cmd[3] |= dir ? 0x00:0x20;
		  cmd[3] |= fn  ? 0x00:0x40;
		  cmd[3] |= slot->fn ? 0x80:0x00;
		  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco %d", addr );
		  ThreadOp.post(data->writer, (obj)cmd);
		}

  }  
  
  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );
    Boolean f1 = wFunCmd.isf1( node );

    iOSlot slot = __getSlot(data, node );

    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get slot for loco %s", wLoc.getid(node) );
      return;
    }

		if(slot->sx2 ) {
		  int fidx = wFunCmd.getfnchanged(node); 
			byte* cmd = allocMem(32);
			cmd[ 0] = 2;
			cmd[ 1] = 2;
			
		  if( fidx > 0 && fidx < 9 ) {
		    /* fx1 */
		    if( fidx == 1 ) {
		      slot->fx1 &= ~0x01;
		      slot->fx1 |= (wFunCmd.isf1(node)?0x01:0x00);
		    }
		    else if( fidx == 2 ) {
		      slot->fx1 &= ~0x02;
		      slot->fx1 |= (wFunCmd.isf2(node)?0x02:0x00);
		    }
		    else if( fidx == 3 ) {
		      slot->fx1 &= ~0x04;
		      slot->fx1 |= (wFunCmd.isf3(node)?0x04:0x00);
		    }
		    else if( fidx == 4 ) {
		      slot->fx1 &= ~0x08;
		      slot->fx1 |= (wFunCmd.isf4(node)?0x08:0x00);
		    }
		    else if( fidx == 5 ) {
		      slot->fx1 &= ~0x10;
		      slot->fx1 |= (wFunCmd.isf5(node)?0x10:0x00);
		    }
		    else if( fidx == 6 ) {
		      slot->fx1 &= ~0x20;
		      slot->fx1 |= (wFunCmd.isf6(node)?0x20:0x00);
		    }
		    else if( fidx == 7 ) {
		      slot->fx1 &= ~0x40;
		      slot->fx1 |= (wFunCmd.isf7(node)?0x40:0x00);
		    }
		    else if( fidx == 8 ) {
		      slot->fx1 &= ~0x80;
		      slot->fx1 |= (wFunCmd.isf8(node)?0x80:0x00);
		    }
				cmd[ 2] = slot->nr * 6 + 4 + WRITE_FLAG;
				cmd[ 3] = slot->fx1;
		  }
		  else {
		    /* fx2 */
		    if( fidx == 9 ) {
		      slot->fx2 &= ~0x01;
		      slot->fx2 |= (wFunCmd.isf9(node)?0x01:0x00);
		    }
		    else if( fidx == 10 ) {
		      slot->fx2 &= ~0x02;
		      slot->fx2 |= (wFunCmd.isf10(node)?0x02:0x00);
		    }
		    else if( fidx == 11 ) {
		      slot->fx2 &= ~0x04;
		      slot->fx2 |= (wFunCmd.isf11(node)?0x04:0x00);
		    }
		    else if( fidx == 12 ) {
		      slot->fx2 &= ~0x08;
		      slot->fx2 |= (wFunCmd.isf12(node)?0x08:0x00);
		    }
		    else if( fidx == 13 ) {
		      slot->fx2 &= ~0x10;
		      slot->fx2 |= (wFunCmd.isf13(node)?0x10:0x00);
		    }
		    else if( fidx == 14 ) {
		      slot->fx2 &= ~0x20;
		      slot->fx2 |= (wFunCmd.isf14(node)?0x20:0x00);
		    }
		    else if( fidx == 15 ) {
		      slot->fx2 &= ~0x40;
		      slot->fx2 |= (wFunCmd.isf15(node)?0x40:0x00);
		    }
		    else if( fidx == 16 ) {
		      slot->fx2 &= ~0x80;
		      slot->fx2 |= (wFunCmd.isf16(node)?0x80:0x00);
		    }
				cmd[ 2] = slot->nr * 6 + 5 + WRITE_FLAG;
				cmd[ 3] = slot->fx2;
		  }
		  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SX2 function %d", addr );
			ThreadOp.post(data->writer, (obj)cmd);
		}
		else {
		  byte *cmd = allocMem(32);
		  cmd[0] = slot->bus;
		  cmd[1] = 2;

		  cmd[2] = addr & 0x7F;
		  cmd[2] |= WRITE_FLAG;
		  cmd[3] = slot->speed;
		  cmd[3] |= slot->dir ? 0x00:0x20;
		  cmd[3] |= slot->lights  ? 0x00:0x40;
		  cmd[3] |= f1 ? 0x80:0x00;

		  slot->fn = f1;
		  slot->lastcmd = SystemOp.getTick();

		  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "function %d", addr );
		  ThreadOp.post(data->writer, (obj)cmd);
		}
  }

}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOZS2Data data = Data(inst);

  if( cmd != NULL ) {
    int bus = 0;
    __translate( (iOZS2)inst, cmd );
    cmd->base.del(cmd);
  }
  return NULL;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  iOZS2Data data = Data(inst);
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOZS2Data data = Data(inst);
  /* TODO: bus */
  data->run = False;
  if( poweroff ) {
    byte* cmd = allocMem(32);
    cmd[0] = 0;
    cmd[1] = 2;
    cmd[2] = 127;
    cmd[2] |= WRITE_FLAG;
    cmd[3] = 0x00;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power OFF" );
    ThreadOp.post(data->writer, (obj)cmd);
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  ThreadOp.sleep(100);
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOZS2Data data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _setRawListener( obj inst ,obj listenerObj ,const digint_rawlistener listenerRawFun ) {
  iOZS2Data data = Data(inst);
  return 0;
}


/** external shortcut event */
static void _shortcut( obj inst ) {
  iOZS2Data data = Data(inst);
  return;
}


/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  iOZS2Data data = Data(inst);
  return 0;
}


/**  */
static Boolean _supportPT( obj inst ) {
  iOZS2Data data = Data(inst);
  return 0;
}


static Boolean __setActiveBus( iOZS2 zs2, int bus ) {
  iOZS2Data data = Data(zs2);

  if( bus < 3 && data->activebus != bus ) {
    byte cmd[2];
    cmd[0] = 126;
    cmd[1] = bus;
    cmd[0] |= WRITE_FLAG;

    data->activebus = bus;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set active bus to sx[%d]", bus );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)cmd, 2 );
    if( data->dummyio )
      return True;
    else
      return SerialOp.write( data->serial, (char*)cmd, 2 );
  }
  return True;
}


static __evaluateFB( iOZS2 zs2, byte in, int addr, int bus ) {
  iOZS2Data data = Data(zs2);

  if( in != data->fbstate[bus][addr] ) {
    int n = 0;
    int pin = 0;
    int state = 0;
    for( n = 0; n < 8; n++ ) {
      if( (in & (0x01 << n)) != (data->fbstate[bus][addr] & (0x01 << n)) ) {
        pin = n;
        state = (in & (0x01 << n)) ? 1:0;
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "fb %d = %d", addr*8+pin+1, state );
        {
          /* inform listener: Node3 */
          iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
          wFeedback.setaddr( nodeC, addr*8+pin+1 );
          wFeedback.setbus( nodeC, bus );
          wFeedback.setstate( nodeC, state?True:False );
          if( data->iid != NULL )
            wFeedback.setiid( nodeC, data->iid );

          data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
        }
      }
    }
    data->fbstate[bus][addr] = in;
  }
}


static Boolean __updateSlot(iOZS2Data data, iOSlot slot, int addr, int val, Boolean* vdfChanged, Boolean* funChanged) {
  Boolean changed = False;
  int     speed   = slot->speed;
  Boolean dir     = slot->dir;
  Boolean lights  = slot->lights;
  Boolean fn      = slot->fn;
  int     fx1     = slot->fx1;
  int     fx2     = slot->fx2;

  /* virtual SX2 */
  if( slot->sx2 ) {
    int idx = addr % 6;
    switch( idx ) {
      case 0: /* preamble */
        break;
      case 1: /* address high */
        break;
      case 2: /* addres low & light */
				lights = (val & 0x02) ? True:False;
				fn     = (val & 0x01) ? True:False;
        break;
      case 3: /* speed & direction */ 
				speed  = val & 0x7F;
				dir    = (val & 0x80) ? False:True;
        break;
      case 4: /* f1-f8 */
        fx1 = val;
        break;
      case 5: /* f9-f16 */
        fx2 = val;
        break;
    }
  }
  /* SX0 or SX1 */
  else {
		speed  = val & 0x1F;
		dir    = (val & 0x20) ? False:True;
		lights = (val & 0x40) ? False:True;
		fn     = (val & 0x80) ? True:False;
  }
  
  if( slot->speed != speed ) {
    /* trace speed changed */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "speed change event from %d to %d for %s", slot->speed, speed, slot->id );
    slot->speed = speed;
    *vdfChanged = True;
    changed = True;
  }
  if( slot->dir != dir ) {
    /* trace dir changed */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "direction change event from %s to %s for %s", slot->dir?"reverse":"forwards", dir?"reverse":"forwards", slot->id );
    slot->dir = dir;
    *vdfChanged = True;
    changed = True;
  }
  if( slot->lights != lights ) {
    /* trace lights changed */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "lights change event from %s to %s for %s", slot->lights?"on":"off", lights?"on":"off", slot->id );
    slot->lights = lights;
    *vdfChanged = True;
    *funChanged = True;
    changed = True;
  }
  if( slot->fn != fn || slot->fx1 != fx1 || slot->fx2 != fx2 ) {
    /* trace functions changed */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "function change event from %s to %s for %s (fx1=0x%02X, fx2=0x%02X)", slot->fn?"on":"off", fn?"on":"off", slot->id, fx1, fx2 );
    slot->fn = fn;
    slot->fx1 = fx1;
    slot->fx2 = fx2;
    *funChanged = True;
    changed = True;
  }

  return changed;
}



static void __evaluateSX( iOZS2 zs2, int bus, int addr, int val ) {
  iOZS2Data data = Data(zs2);
  char key[32] = {'\0'};
  iOSlot slot = NULL;

  StrOp.fmtb(key, "%d_%d", bus, addr );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
      "monitor event: bus=%d, addr=%d val=%02X key=%s.", bus, addr, val, key );
  
  if( bus == 2 ) {
    /* SX2 virtual bus */
    if( addr % 6 == 0 && addr < 96) {
      slot = __getSlotByAddr( data, addr, True );
    }
  }
  else {
    if( addr == 127 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power %s", (val & 0x80) ? "ON":"OFF" );
      if( data->listenerFun != NULL && data->listenerObj != NULL ) {
        iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
        wState.setiid( node, data->iid );
        wState.setpower( node, (val & 0x80)?True:False );
        data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
      }
      return;
    }
    else if( MapOp.haskey( data->fbmap, key) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "occupancy for unit %d is %02X", addr, val );
      __evaluateFB( zs2, val, addr, bus );
      return;
    }
    else {
      /* Loco or Point */
      slot = __getSlotByAddr( data, addr, False );
    }
  }
  
  if( slot != NULL ) {
    Boolean vdfChanged = False;
    Boolean funChanged = False;

    if( __updateSlot(data, slot, addr, val, &vdfChanged, &funChanged) ) {
      iONode nodeC = NULL;
      if( vdfChanged ) {
        nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        if( data->iid != NULL )
          wLoc.setiid( nodeC, data->iid );
        wLoc.setid( nodeC, slot->id );
        wLoc.setaddr( nodeC, slot->addr );
        wLoc.setV_raw( nodeC, slot->speed );
        wLoc.setV_rawMax( nodeC, 31 );
        wLoc.setfn( nodeC, slot->lights);
        wLoc.setdir( nodeC, slot->dir );
        wLoc.setcmd( nodeC, wLoc.direction );
        wLoc.setthrottleid( nodeC, "sx-bus" );
        data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
      }

      if( funChanged ) {
        nodeC = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
        if( data->iid != NULL )
          wLoc.setiid( nodeC, data->iid );
        wFunCmd.setid( nodeC, slot->id );
        wFunCmd.setaddr( nodeC, slot->addr );
        wFunCmd.setf0( nodeC, slot->lights );
        wFunCmd.setf1( nodeC, slot->fn );

        wLoc.setthrottleid( nodeC, "sx-bus" );
        data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
      }
    }
  }
  
  else if( bus < 2 && data->sx[bus][addr] != val ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "point update %d:%d=%d", bus, addr, val );
    int i = 0;
    for( i = 0; i < 8; i++ ) {
      int oldval = data->sx[bus][addr] & (0x01 << i);
      int newval = val & (0x01 << i);
      if( oldval != newval ) {
        iOPoint point = __getPointByAddr( data, bus, addr, i+1 );
        if( point != NULL  && ( SystemOp.getTick() - point->lastcmd > 100 )   ) {
          iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
          if( data->iid != NULL )
            wSwitch.setiid( nodeC, data->iid );
          wSwitch.setid( nodeC, point->id );
          wSwitch.setstate( nodeC, newval?"straight":"turnout" );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "point update %s", point->id );
          data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
        }
      }
    }
  }
  
  
}


static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOZS2 zs2 = (iOZS2)ThreadOp.getParm( th );
  iOZS2Data data = Data(zs2);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader started." );

  int junk = 0;
  byte in[32] = {0};
  /* clean input buffer: */
  while( data->run && SerialOp.available(data->serial) && junk < 32 ) {
    if( SerialOp.read(data->serial, in+junk, 1) ) {
      junk++;
    }
  }

  if( junk > 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%d junk bytes dumped", junk );
    TraceOp.dump( NULL, TRCLEVEL_INFO, (char*)in, junk );
  }
  
  while( data->run ) {
    if( SerialOp.available(data->serial) ) {
      /* read 3-Byte-Auto-Mode: in[0]=bus, in[1]=addr, in[2]=value */
      if( SerialOp.read(data->serial, in, 3) ) {
        int bus  = (in[0]-128) & 0x7F;
        int addr = in[1] & 0x7F;
        int val  = in[2] & 0xFF;
        TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, 3 );
        
        if( bus < 3 ) {
          __evaluateSX(zs2, bus, addr, val);
          /* save new value */
          data->sx[bus&0x03][addr] = val;
        }
        else {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "invalid bus read: %d", bus );
          ThreadOp.sleep(500);
        }
       
      }  
    }

    ThreadOp.sleep(10);
  }
  
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader ended." );
}


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOZS2 zs2 = (iOZS2)ThreadOp.getParm( th );
  iOZS2Data data = Data(zs2);
  byte* cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer started." );
  ThreadOp.sleep(1000);

  /* Monitoring off */
  cmd = allocMem(32);
  cmd[0] = 0;
  cmd[1] = 2;
  cmd[2] = 125;
  cmd[2] |= WRITE_FLAG;
  cmd[3] = 0x40;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "monitoring OFF" );
  ThreadOp.post(th, (obj)cmd);

  /* Monitoring on */
  cmd = allocMem(32);
  cmd[0] = 0;
  cmd[1] = 2;
  cmd[2] = 125;
  cmd[2] |= WRITE_FLAG;
  cmd[3] = 0x80;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "monitoring ON" );
  ThreadOp.post(th, (obj)cmd);
  
  /* CS on */
  cmd = allocMem(32);
  cmd[0] = 0;
  cmd[1] = 2;
  cmd[2] = 127;
  cmd[2] |= WRITE_FLAG;
  cmd[3] = 0x80;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power ON" );
  ThreadOp.post(th, (obj)cmd);
  
  
  while( data->run ) {
    byte * post = NULL;
    int len = 0;
    int bus = 0;
    byte out[64] = {0};

    ThreadOp.sleep(10);
    post = (byte*)ThreadOp.getPost( th );

    if (post != NULL) {
      /* first byte is the message length */
      bus = post[0];
      len = post[1];
      MemOp.copy( out, post+2, len);
      freeMem( post);

      __setActiveBus(zs2, bus);

      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
      if( !SerialOp.write( data->serial, (char*)out, len ) ) {
        /* sleep and send it again? */
      }
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer ended." );
}


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOZS2Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**  */
static struct OZS2* _inst( const iONode ini ,const iOTrace trc ) {
  iOZS2 __ZS2 = allocMem( sizeof( struct OZS2 ) );
  iOZS2Data data = allocMem( sizeof( struct OZS2Data ) );
  MemOp.basecpy( __ZS2, &ZS2Op, 0, sizeof( struct OZS2 ), data );

  TraceOp.set( trc );
  SystemOp.inst();

  /* Initialize data->xxx members... */
  data->device   = StrOp.dup( wDigInt.getdevice( ini ) );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  data->bps      = wDigInt.getbps( ini );
  data->timeout  = wDigInt.gettimeout( ini );
  data->swtime   = wDigInt.getswtime( ini );
  data->dummyio  = wDigInt.isdummyio( ini );

  data->run      = True;

  data->serialOK = False;
  data->initOK   = False;

  data->activebus = 0;

  data->mux      = MutexOp.inst( NULL, True );
  data->pointmux = MutexOp.inst( NULL, True );
  data->lcmux    = MutexOp.inst( NULL, True );
  data->lcmap    = MapOp.inst();
  data->fbmap    = MapOp.inst();
  data->pointmap = MapOp.inst();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "zs2 %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid[%s]: %s,%d,%d",
        wDigInt.getiid( ini ) != NULL ? wDigInt.getiid( ini ):"",
        data->device, data->bps, data->timeout );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, none );
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, none, wDigInt.isrtsdisabled( ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );

  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {

    data->reader = ThreadOp.inst( "zs2reader", &__reader, __ZS2 );
    ThreadOp.start( data->reader );
    data->writer = ThreadOp.inst( "zs2writer", &__writer, __ZS2 );
    ThreadOp.start( data->writer );

  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init ZS2 port!" );
  }

  instCnt++;
  return __ZS2;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/zs2.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
