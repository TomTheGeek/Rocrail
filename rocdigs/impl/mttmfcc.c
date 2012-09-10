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

/* Protocol
 * http://www.mttm.de/FCC_Interface_Doku.pdf
 */

#include "rocdigs/impl/mttmfcc_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"
#include "rocs/public/system.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/SwitchList.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/FbInfo.h"
#include "rocrail/wrapper/public/FbMods.h"

static int instCnt = 0;

static __evaluateFB( iOMttmFccData data );


/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOMttmFccData data = Data(inst);
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

/** ----- OMttmFcc ----- */
static Boolean __updateSlot(iOMttmFccData data, iOSlot slot, Boolean* vdfChanged, Boolean* funChanged) {
  Boolean changed = False;
  int     speed   = 0;
  Boolean dir     = True;
  Boolean lights  = False;
  byte    f1_8    = 0;
  byte    f9_16   = 0;

  if( !slot->sx1 ) {
    int index = slot->index;
    int bus   = index > 15 ? 1:0;

    index = index-(bus*16);
    {
      byte    format = data->sx2[bus][ 0 + index] & 0x0F;
      byte    haddr  = data->sx2[bus][16 + index];
      byte    laddr  = data->sx2[bus][32 + index];

      f1_8   = data->sx2[bus][64 + index];
      f9_16  = data->sx2[bus][80 + index];

      lights = (data->sx2[bus][32 + index] & 0x02) ? True:False;
      speed  = data->sx2[bus][48 + index] & 0x7F;
      dir    = (data->sx2[bus][48 + index] & 0x80) ? False:True;
    }
  }
  else {
    /* SX1 */
    byte sx1 = data->sx1[slot->bus&0x01][slot->addr&0x7F];
    speed  = sx1 & 0x1F;
    dir    = (sx1 & 0x20) ? False:True;
    lights = (sx1 & 0x40) ? True:False;
    f1_8   = (sx1 & 0x80) ? 0x01:0x00;
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
  if( slot->f1_8 != f1_8 ) {
    /* trace functions changed */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "function 1-8 change event from 0x%02X to 0x%02X for %s", slot->f1_8, f1_8, slot->id );
    slot->f1_8 = f1_8;
    *funChanged = True;
    changed = True;
  }
  if( slot->f9_16 != f9_16 ) {
    /* trace functions changed */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "function 9-16 change event from 0x%02X to 0x%02X for %s", slot->f9_16, f9_16, slot->id );
    slot->f9_16 = f9_16;
    *funChanged = True;
    changed = True;
  }

  return changed;
}


static Boolean __updateSlots(iOMttmFccData data) {
  if( MutexOp.wait( data->lcmux ) ) {
    iOSlot slot = (iOSlot)MapOp.first( data->lcmap );
    while( slot != NULL ) {
      Boolean vdfChanged = False;
      Boolean funChanged = False;

      if( __updateSlot(data, slot, &vdfChanged, &funChanged) ) {
        iONode nodeC = NULL;
        if( vdfChanged ) {
          nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
          if( data->iid != NULL )
            wLoc.setiid( nodeC, data->iid );
          wLoc.setid( nodeC, slot->id );
          wLoc.setaddr( nodeC, slot->addr );
          if( slot->ebreak && slot->speed > 0 ) {
            /* correction  for ebreak */
            wLoc.setV_raw( nodeC, slot->speed - 1 );
          }
          else {
            wLoc.setV_raw( nodeC, slot->speed );
          }
          wLoc.setV_rawMax( nodeC, slot->steps );
          wLoc.setfn( nodeC, slot->lights);
          wLoc.setdir( nodeC, slot->dir );
          wLoc.setthrottleid( nodeC, "fcc" );
          wLoc.setcmd( nodeC, wLoc.direction );
          data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
        }

        if( funChanged ) {
          nodeC = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
          if( data->iid != NULL )
            wLoc.setiid( nodeC, data->iid );
          wFunCmd.setid( nodeC, slot->id );
          wFunCmd.setaddr( nodeC, slot->addr );
          wFunCmd.setf0( nodeC, slot->lights );
          wFunCmd.setf1( nodeC, (slot->f1_8 & 0x01) ? True:False );
          wFunCmd.setf2( nodeC, (slot->f1_8 & 0x02) ? True:False );
          wFunCmd.setf3( nodeC, (slot->f1_8 & 0x04) ? True:False );
          wFunCmd.setf4( nodeC, (slot->f1_8 & 0x08) ? True:False );
          wFunCmd.setf5( nodeC, (slot->f1_8 & 0x10) ? True:False );
          wFunCmd.setf6( nodeC, (slot->f1_8 & 0x20) ? True:False );
          wFunCmd.setf7( nodeC, (slot->f1_8 & 0x40) ? True:False );
          wFunCmd.setf8( nodeC, (slot->f1_8 & 0x80) ? True:False );

          wFunCmd.setf9 ( nodeC, (slot->f9_16 & 0x01) ? True:False );
          wFunCmd.setf10( nodeC, (slot->f9_16 & 0x02) ? True:False );
          wFunCmd.setf11( nodeC, (slot->f9_16 & 0x04) ? True:False );
          wFunCmd.setf12( nodeC, (slot->f9_16 & 0x08) ? True:False );
          wFunCmd.setf13( nodeC, (slot->f9_16 & 0x10) ? True:False );
          wFunCmd.setf14( nodeC, (slot->f9_16 & 0x20) ? True:False );
          wFunCmd.setf15( nodeC, (slot->f9_16 & 0x40) ? True:False );
          wFunCmd.setf16( nodeC, (slot->f9_16 & 0x80) ? True:False );

          wLoc.setthrottleid( nodeC, "fcc" );
          data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
        }

      }

      slot = (iOSlot)MapOp.next( data->lcmap );
    }
    MutexOp.post(data->lcmux);
  }

}


static void __evaluatePT( iOMttmFccData data, byte* out, byte* in) {
  int cv = out[1] * 256 + out[2];
  if( in[0] == 0x00 ) {
    /* write cv */
    if( data->listenerFun != NULL && data->listenerObj != NULL ) {
      iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      wProgram.setcv( node, cv );
      wProgram.setvalue( node, -1 );
      wProgram.setcmd( node, wProgram.datarsp );
      if( data->iid != NULL )
        wProgram.setiid( node, data->iid );
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
  }
  else if( in[0] == 0x01 ) {
    /* read cv */
    int val = in[1];
    if( data->listenerFun != NULL && data->listenerObj != NULL ) {
      iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
      wProgram.setcv( node, cv );
      wProgram.setvalue( node, val );
      wProgram.setcmd( node, wProgram.datarsp );
      if( data->iid != NULL )
        wProgram.setiid( node, data->iid );
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
    }
  }
}


static void __evaluateRsp( iOMttmFccData data, byte* out, int outsize, byte* in, int insize ) {
  switch( in[0] ) {
  case 0x00:
  case 0x01:
    if( insize == 3 )
      __evaluatePT(data, out, in);
    break;
  }
}


static Boolean __transact( iOMttmFccData data, byte* out, int outsize, byte* in, int insize ) {
  Boolean rc = data->dummyio;

  if( MutexOp.wait( data->mux ) ) {
    TraceOp.dump( name, TRCLEVEL_BYTE, out, outsize );
    if( !data->dummyio ) {
      if( rc = SerialOp.write( data->serial, out, outsize ) ) {
        if( insize > 0 ) {
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "insize=%d", insize);
          rc = SerialOp.read( data->serial, in, insize );
          if( rc ) {
            TraceOp.dump( name, TRCLEVEL_BYTE, in, insize );
            __evaluateRsp(data, out, outsize, in, insize);
          }
        }
      }
    }
    MutexOp.post( data->mux );
  }
  return rc;
}


static iOPoint __getPointByAddr(iOMttmFccData data, int bus, int addr, int port) {
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


static iOPoint __getPoint(iOMttmFccData data, iONode node) {
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

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch %s created for %s", wSwitch.getid(node), key );
  return point;
}




static iOSlot __getSlot(iOMttmFccData data, iONode node) {
  int steps = wLoc.getspcnt(node);
  int addr  = wLoc.getaddr(node);
  int fncnt = wLoc.getfncnt(node);
  Boolean ebreak = True;
  Boolean sx1 = False;
  byte index = 0xFF;
  iOSlot slot = NULL;
  byte cmd[32] = {0x79, 0x01};

  slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot exist for %s", wLoc.getid(node) );
    return slot;
  }

  if( StrOp.equals( wLoc.prot_S, wLoc.getprot(node) ) ) {
    steps = 31;
    sx1 = True;
    ebreak = False;

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sx1, steps=%d, fncnt=%d", steps, fncnt );
  }
  /*
    Die DCC-Lok mit der langen Adresse 1234 und 126 Fahrstufen soll an die FCC- Digitalzentrale angemeldet werden:
    Bestimmung: 1234 (binär: 00010011010010) DCC-Lokadresse: 00010011010010 00 entspricht 0x1348
    Es ist daher Folgendes an die FCC-Digitalzentrale zu senden: Vom PC:  0x79  0x01  0x13  0x48  0x07
   */
  else if( StrOp.equals( wLoc.prot_N, wLoc.getprot(node) ) ) {
    /* short DCC */
    addr = addr << 2;
    cmd[4] = steps > 100 ? 0x05:(steps > 14 ? 0x81:0x91);
    steps =  steps > 100 ? 126:(steps > 14 ? 28:14);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "short DCC, steps=%d, fncnt=%d", steps, fncnt );
  }
  else if( StrOp.equals( wLoc.prot_L, wLoc.getprot(node) ) ) {
    /* long DCC */
    addr = addr << 2;
    cmd[4] = steps > 100 ? 0x07:(steps > 14 ? 0x83:0x93);
    steps =  steps > 100 ? 126:(steps > 14 ? 28:14);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "long DCC, steps=%d, fncnt=%d", steps, fncnt );
  }
  else if( StrOp.equals( wLoc.prot_M, wLoc.getprot(node) ) ) {
    /* MM */
    /*
    Die MM-Lok mit der Adresse 218 soll an die FCC-Digitalzentrale angemeldet werden:
    Bestimmung: 218 (binär: 00000011011010) MM-Lokadresse: 00000011011010 00 entspricht 0x0368
    Es ist daher Folgendes an die FCC-Digitalzentrale zu senden: Vom PC:  0x79  0x01  0x03  0x68  0x02
     */
    addr = addr << 2;
    cmd[4] = fncnt == 4 ? 0x82:0x92;
    steps = 14;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "MM, steps=%d, fncnt=%d", steps, fncnt );
  }
  else {
    /*
    Die SX2-Lok mit der Adresse 5678 soll an die FCC-Digitalzentrale angemeldet werden:
    Rechnung: 5678 / 100 = 56 (binär: 0111000) Rest 78 (binär: 1001110)
    SX2-Lokadresse: 0111000 1001110 00 entspricht 0x7138
    Es ist daher Folgendes an die FCC-Digitalzentrale zu senden: Vom PC:  0x79  0x01  0x71  0x38  0x04
    */
    addr = (wLoc.getaddr(node) / 100) << 9;
    addr = addr + ((wLoc.getaddr(node) % 100) << 2);

    /* default SX2 */
    cmd[4] = 0x04;
    steps = 127;
    ebreak = False;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "default SX2, steps=%d, fncnt=%d", steps, fncnt );
  }

  cmd[2] = addr / 256;
  cmd[3] = addr % 256;


  if( sx1 || __transact( data, cmd, 5, &index, 1 ) ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "got index %d for %s", index, wLoc.getid(node) );
    slot = allocMem( sizeof( struct slot) );
    slot->addr = addr;
    slot->index = index;
    slot->protocol = cmd[4];
    slot->steps = steps;
    slot->ebreak = ebreak;
    slot->sx1 = sx1;
    slot->bus = wLoc.getbus(node);
    slot->id = StrOp.dup(wLoc.getid(node));
    if( MutexOp.wait( data->lcmux ) ) {
      MapOp.put( data->lcmap, wLoc.getid(node), (obj)slot);
      MutexOp.post(data->lcmux);
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for %s", wLoc.getid(node) );
  }

  return slot;
}


/* fbmods is a comman separated address list of connected feedback modules. */
static void __updateFB( iOMttmFccData data, iONode fbInfo ) {
  int cnt = NodeOp.getChildCnt( fbInfo );
  int i = 0;

  char* str = NodeOp.base.toString( fbInfo );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "updateFB\n%s", str );
  StrOp.free( str );

  /* reset the list: */
  MemOp.set( data->fbmodcnt, 0, 2 * sizeof(int) );
  MemOp.set( data->fbmods, 0, 2*128 );

  for( i = 0; i < cnt; i++ ) {
    iONode fbmods = NodeOp.getChild( fbInfo, i );
    const char* mods = wFbMods.getmodules( fbmods );
    int bus = wFbMods.getbus( fbmods );
    if( bus > 1 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unsupported bus=%d", bus );
    }
    else if( mods != NULL && StrOp.len( mods ) > 0 ) {

      iOStrTok tok = StrTokOp.inst( mods, ',' );
      int idx = 0;
      while( StrTokOp.hasMoreTokens( tok ) ) {
        int addr = atoi( StrTokOp.nextToken(tok) );
        data->fbmods[bus][idx] = addr & 0x7f;
        idx++;
      };
      data->fbmodcnt[bus] = idx;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "updateFB bus=%d count=%d", bus, idx );
    }
  }

  if( data->dummyio ) {
    __evaluateFB(data);
  }
}


static int __translate( iOMttmFccData data, iONode node, byte* out, int *insize ) {
  *insize = 0;

  if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
    __updateFB( data, node );
  }
  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int bus   = wSwitch.getbus( node ) & 0x01;
    int addr  = wSwitch.getaddr1( node ) & 0x7F;
    byte pin  = 0x01 << ( wSwitch.getport1( node ) - 1 );
    byte mask = ~pin;

    iOPoint point = __getPoint(data, node);
    if( point != NULL ) {
      point->lastcmd = SystemOp.getTick();
    }

    out[0] = bus;
    out[1] = addr | 0x80;
    out[2] = 0x01 << ( wSwitch.getport1( node ) - 1 );

    /* reset pin to 0: */
    out[2] = data->swstate[bus][addr] & mask;

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      out[2] |= pin;
    /* save new state: */
    data->swstate[bus][addr] = out[2];
    *insize = 1;
    return 3;
  }

  /* Output command */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int bus  = wOutput.getbus( node ) & 0x01;
    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;
    byte pin = 0x01 << ( port - 1 );
    byte mask = ~pin;

    out[0] = bus;
    out[1] = addr | 0x80;

    /* reset pin to 0: */
    out[2] = data->swstate[bus][addr] & mask;

    if( action )
      out[2] |= pin;
    /* save new state: */
    data->swstate[bus][addr] = out[2];
    *insize = 1;
    return 3;
  }


  /* System command. */
  /*
    Gleisspannung ein (SX1/2-Bus 0): Vom PC:  0x00  0xFF  0x01
    Gleisspannung aus (SX1/2-Bus 0): Vom PC:  0x00  0xFF  0x00
    Gleisspannung ein (SX1/2-Bus 1): Vom PC:  0x01  0xFF  0x01
    Gleisspannung aus (SX1/2-Bus 1): Vom PC:  0x01  0xFF  0x00
    Zum PC: 0x00 Zum PC:  0x00 Zum PC:  0x00 Zum PC:  0x00
  */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) || StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      out[0] = 0x00;
      out[1] = 0xFF;
      out[2] = 0x00;
      *insize = 1; /* Return code from FCC. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      return 3;
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      out[0] = 0x00;
      out[1] = 0xFF;
      out[2] = 0x01;
      *insize = 1; /* Return code from FCC. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      return 3;
    }
  }

  /* Loc command.*/
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int  speed = 0;
    byte in = 0;
    Boolean fn = wLoc.isfn( node );
    int    dir = wLoc.isdir( node );
    int  spcnt = wLoc.getspcnt( node );

    int index = 0;

    iOSlot slot = __getSlot(data, node );

    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get slot for loco %s", wLoc.getid(node) );
      return 0;
    }

    if( data->dummyio ) {
      __updateSlots(data);
    }


    spcnt = slot->steps;

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * spcnt) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * spcnt) / wLoc.getV_max( node );
    }

    if( slot->ebreak && speed > 0 ) {
      speed++;
    }

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc=%s addr=%d speed=%d steps=%d lights=%s dir=%s",
        wLoc.getid(node), wLoc.getaddr(node), speed, spcnt, fn?"on":"off", dir?"forwards":"reverse" );

    if( slot->sx1 ) {
      /* native selectrix SX1 */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "SX1 loco command for %s", wLoc.getid(node) );
      out[0] = wLoc.getbus(node)&0x01;
      out[1] = wLoc.getaddr(node);
      out[1] |= 0x80;
      /*
      SX1-Loks werden wie bisher direkt über die entsprechenden SX1-Kanäle des SX1/2-Bus- Systems 0 gesteuert.
      Die Kanalbelegung ist:
      Bits 0 bis 4: Fahrstufen von 0 bis 31 
      Bit 5:        Fahrtrichtung (0 entspricht vorwaerts, 1 entspricht rueckwaerts) 
      Bit 6:        Licht (0 bedeutet Licht aus, 1 bedeutet Licht ein) 
      Bit 7:        Horn (0 bedeutet Horn aus, 1 bedeutet Horn ein) 
      */
      out[2] = speed & 0x1F;
      out[2] |= (wLoc.isdir(node) ? 0x00:0x20);
      out[2] |= (wLoc.isfn(node)  ? 0x40:0x00);
      out[2] |= ((slot->f1_8 & 0x01)  ? 0x80:0x00);

      slot->speed = speed;
      slot->dir = wLoc.isdir(node);
      slot->lights = wLoc.isfn(node);

      *insize = 1;
      return 3;
    }


    index = slot->index;
    /* Lights 
      ON : 0x79 0x05 Index 0x02 0x00
      OFF: 0x79 0x05 Index 0x00 0x00
    */
    out[0] = 0x79;
    out[1] = 0x05;
    out[2] = index;
    out[3] = fn?0x02:0x00;
    out[4] = 0x00;
    __transact( data, out, 5, &in, 1 );

    /*
      Verändern der Fahrstufe einer SX2-, DCC- oder MM-Lok:
      Vom PC: 0x79  0x03  Index FS  0x00
      Zum PC: gleich 0x00 (im Erfolgsfalle)
      ungleich 0x00 (im Fehlerfalle)

      Verändern der Fahrstufe und der Fahrtrichtung einer SX2-, DCC- oder MM-Lok:
      Vom PC: 0x79  0x13  Index FSFR  0x00
      Zum PC:
      gleich 0x00
      ungleich 0x00
      Index ist der bei der Anmeldung der betreffenden Lok an die FCC-Digitalzentrale zurückgegebene Wert.
      FSFR ist der, gemäß der oben stehenden Tabelle, umgerechnete Wert der neuen Fahrstufe, wobei das höchstwertige
      Bit die neue Fahrtrichtung bestimmt!
     */
    out[0] = 0x79;
    out[1] = 0x13;
    out[2] = index;
    out[3] = speed + (dir?0x00:0x80);
    out[4] = 0x00;
    *insize = 1; /* Return code from FCC. */
    return 5;

  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    Boolean f1  = wFunCmd.isf1 ( node );
    Boolean f2  = wFunCmd.isf2 ( node );
    Boolean f3  = wFunCmd.isf3 ( node );
    Boolean f4  = wFunCmd.isf4 ( node );
    Boolean f5  = wFunCmd.isf5 ( node );
    Boolean f6  = wFunCmd.isf6 ( node );
    Boolean f7  = wFunCmd.isf7 ( node );
    Boolean f8  = wFunCmd.isf8 ( node );
    Boolean f9  = wFunCmd.isf9 ( node );
    Boolean f10 = wFunCmd.isf10( node );
    Boolean f11 = wFunCmd.isf11( node );
    Boolean f12 = wFunCmd.isf12( node );
    Boolean f13 = wFunCmd.isf13( node );
    Boolean f14 = wFunCmd.isf14( node );
    Boolean f15 = wFunCmd.isf15( node );
    Boolean f16 = wFunCmd.isf16( node );

    iOSlot slot = __getSlot(data, node );

    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get slot for loco %s", wLoc.getid(node) );
      return 0;
    }


    if( slot->sx1 ) {
      /* native selectrix SX1 */
      int   addr = wFunCmd.getaddr( node );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "SX1 function command for %s", wLoc.getid(node) );
      out[0] = wLoc.getbus(node)&0x01;
      out[1] = wLoc.getaddr(node);
      out[1] |= 0x80;
      out[2] = slot->speed & 0x1F;
      out[2] |= (slot->dir ? 0x00:0x20);
      out[2] |= (slot->lights ? 0x40:0x00);
      out[2] |= (f1 ? 0x80:0x00);
      slot->f1_8 = f1 ? 0x01:0x00;

      *insize = 1;
      return 3;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "function command for %s", wLoc.getid(node) );
      out[0] = 0x79;
      out[1] = 0x16;
      out[2] = slot->index;
      out[3] = (f1 << 0 | f2 << 1 | f3 << 2 | f4 << 3 | f5 << 4 | f6 << 5 | f7 << 6 | f8 << 7);
      out[4] = (f9 << 0 | f10 << 1 | f11 << 2 | f12 << 3 | f13 << 4 | f14 << 5 | f15 << 6 | f16 << 7);
      out[5] = 0x00;
      slot->f1_8  = out[3];
      slot->f9_16 = out[4];
      *insize = 1;
      return 6;
    }
    
  }


  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    if( wProgram.getcmd( node ) == wProgram.set ) {
      int cv    = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int addr  = wProgram.getaddr( node );

      if( wProgram.ispom(node) ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "POM: set CV%d of SX2 loc %d to %d...", cv, addr, value );
        /* SX2: 0x7A Adresse (High) Adresse (Low) PA (High) PA (Low) Wert */
        out[0] = 0x7A;
        out[1] = addr/256;
        out[2] = addr%256;
        out[3] = cv/256;
        out[4] = cv%256;
        out[5] = value;
        *insize = 1;
        return 6;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set CV%d of SX2 loc %d to %d...", cv, addr, value );
        /* SX2: 0x83 0xCA High Low Wert */
        out[0] = 0x83;
        out[1] = 0xCA;
        out[2] = cv/256;
        out[3] = cv%256;
        out[4] = value;
        *insize = 3;
        return 5;
      }
    }
    else if( wProgram.getcmd( node ) == wProgram.get ) {
      int cv    = wProgram.getcv( node );
      int addr  = wProgram.getaddr( node );
      if( wProgram.ispom(node) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*** not supported *** POM: read CV%d of loc %d...", cv, addr );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "read CV%d of SX2 loc %d...", cv, addr );
        /* SX2: 0x83 0xC2 High Low 0x00 */
        out[0] = 0x83;
        out[1] = 0xC2;
        out[2] = cv/256;
        out[3] = cv%256;
        out[4] = 0;
        *insize = 3;
        return 5;
      }
    }

  }

  return 0;
}


static __evaluateFB( iOMttmFccData data ) {
  int bus = 0;
  int mod = 0;

  if( data->sx1[0][112] != data->power ) {
    iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
    data->power = data->sx1[0][112];
    wState.setiid( node, data->iid );
    wState.setpower( node, data->power?True:False );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power event; changed to %s", data->power?"ON":"OFF" );
    data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );
  }

  TraceOp.trc( name, data->dummyio ? TRCLEVEL_INFO:TRCLEVEL_DEBUG, __LINE__, 9999, "evaluate sensors..." );

  for( bus = 0; bus < 2; bus++ ) {
    if( data->fbmodcnt[bus] == 0 )
      continue;
      
    for( mod = 0; mod < data->fbmodcnt[bus]; mod++ ) {
      int addr = data->fbmods[bus][mod];
      byte in = data->sx1[bus][addr];
      
      if( in != data->fbstate[bus][addr] ) {
        int n = 0;
        int port = 0;
        int state = 0;
        for( n = 0; n < 8; n++ ) {
          if( (in & (0x01 << n)) != (data->fbstate[bus][addr] & (0x01 << n)) ) {
            port = n;
            state = (in & (0x01 << n)) ? 1:0;
            TraceOp.dump ( name, data->dummyio ? TRCLEVEL_INFO:TRCLEVEL_BYTE, (char*)&in, 1 );
            TraceOp.trc( name, data->dummyio ? TRCLEVEL_INFO:TRCLEVEL_DEBUG, __LINE__, 9999, "fb %d = %d", addr*8+port+1, state );
            {
              /* inform listener: Node3 */
              iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
              wFeedback.setaddr( nodeC, addr*8+port+1 );
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
  }
  

  /* Check switches */
  if( MutexOp.wait( data->pointmux ) ) {
    iOPoint point = (iOPoint)MapOp.first( data->pointmap );
    while( point != NULL ) {
      byte pin  = 0x01 << ( point->port - 1 );
      byte mask = ~pin;

      if( !data->swInited || (data->sx1[point->bus][point->addr] & pin) != (data->swstate[point->bus][point->addr] & pin) ) {
        data->swstate[point->bus][point->addr] &= mask;
        data->swstate[point->bus][point->addr] |= (data->sx1[point->bus][point->addr] & pin);

        if( SystemOp.getTick() - point->lastcmd > 100  ) {
          iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
          if( data->iid != NULL )
            wSwitch.setiid( nodeC, data->iid );
          wSwitch.setid( nodeC, point->id );
          wSwitch.setstate( nodeC, (data->sx1[point->bus][point->addr] & pin) ? "straight":"turnout" );
          wSwitch.setbus(nodeC, point->bus);
          wSwitch.setaddr1(nodeC, point->addr);
          wSwitch.setport1(nodeC, point->port);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "switch update %s", point->id );
          data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
        }
      }

      point = (iOPoint)MapOp.next(data->pointmap);
    }
    data->swInited = True;
    MutexOp.post(data->pointmux);
  }

}


static void __sxReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMttmFcc fcc = (iOMttmFcc)ThreadOp.getParm( th );
  iOMttmFccData data = Data(fcc);
  byte buffer[256];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SX reader started." );
  ThreadOp.sleep( 1000 );
  
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "trying to get the FCC version..." );
  buffer[0] = 0x81;
  if( __transact(data, buffer, 1, buffer, 7 ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "FCC version %d.%d", buffer[5], buffer[4] );
  }
  ThreadOp.sleep( 100 );

  while( data->run ) {
    byte cmd[2];
    Boolean ok = True;
    
    ThreadOp.sleep( 100 );
    if( ok ) {
      cmd[0] = 0x78;
      cmd[1] = 0x03;
      /*loopback test __transact( data, buffer, 224, NULL, 0);*/
      ok = __transact( data, cmd, 2, buffer, 226);
      if(ok) {
        MemOp.copy( data->sx1[0], buffer, 113 );
        MemOp.copy( data->sx1[1], buffer+113, 113 );
      }
    }
    
    ThreadOp.sleep( 100 );
    if( ok ) {
      cmd[0] = 0x78;
      cmd[1] = 0xC0;
      /*loopback test __transact( data, buffer, 190, NULL, 0);*/
      ok = __transact( data, cmd, 2, buffer, 192);
      if(ok) {
        MemOp.copy( data->sx2[0], buffer, 96 );
        MemOp.copy( data->sx2[1], buffer+96, 96 );
      }
    }

    if( ok ) {
      __evaluateFB( data );
      __updateSlots( data );
    }
    
  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SX reader ended." );
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOMttmFccData data = Data(inst);
  unsigned char out[32];
  unsigned char in [32];
  int    insize    = 0;
  iONode reply     = NULL;

  MemOp.set( in, 0x00, sizeof( in ) );

  if( cmd != NULL ) {
    if( StrOp.equals( NodeOp.getName( cmd ), wSwitchList.name() ) ) {
      int cnt = NodeOp.getChildCnt(cmd);
      int i;
      for( i = 0; i < cnt; i++ ) {
        /* build the point map */
        __getPoint(data, NodeOp.getChild(cmd, i) );
      }
      data->swInited = False;
    }
    else {
      int size = __translate( data, cmd, out, &insize );
      TraceOp.dump( NULL, TRCLEVEL_BYTE, out, size );
      if( __transact( data, (char*)out, size, (char*)in, insize ) ) {
      }
    }
  }

  /* Cleanup Node1 */
  cmd->base.del(cmd);

  return reply;
}


/**  */
static void _halt( obj inst, Boolean poweroff ) {
  iOMttmFccData data = Data(inst);
  data->run = False;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "shutting down <%s>...", data->iid );
  SerialOp.close( data->serial );
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOMttmFccData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "listener set" );
  return True;
}


static Boolean _setRawListener(obj inst, obj listenerObj, const digint_rawlistener listenerFun ) {
  return True;
}

static byte* _cmdRaw( obj inst, const byte* cmd ) {
  return NULL;
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
  return False;
}


/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 1;
static int _version( obj inst ) {
  iOMttmFccData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**  */
static struct OMttmFcc* _inst( const iONode ini ,const iOTrace trc ) {
  iOMttmFcc __MttmFcc = allocMem( sizeof( struct OMttmFcc ) );
  iOMttmFccData data = allocMem( sizeof( struct OMttmFccData ) );
  MemOp.basecpy( __MttmFcc, &MttmFccOp, 0, sizeof( struct OMttmFcc ), data );

  TraceOp.set( trc );
  SystemOp.inst();

  /* Initialize data->xxx members... */
  data->mux      = MutexOp.inst( NULL, True );
  data->lcmux    = MutexOp.inst( NULL, True );
  data->lcmap    = MapOp.inst();
  data->pointmux = MutexOp.inst( NULL, True );
  data->pointmap = MapOp.inst();

  data->device   = StrOp.dup( wDigInt.getdevice( ini ) );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );
  data->dummyio  = wDigInt.isdummyio(ini);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MTTM FCC %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid      = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device   = %s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serialOK = False;
  if( !data->dummyio ) {
    data->serial = SerialOp.inst( data->device );
    SerialOp.setFlow( data->serial, none );
    SerialOp.setLine( data->serial, 230400, 8, 1, none, wDigInt.isrtsdisabled( ini ) );
    SerialOp.setTimeout( data->serial, wDigInt.gettimeout(ini), wDigInt.gettimeout(ini) );
    data->serialOK = SerialOp.open( data->serial );
  }

  if(data->serialOK) {
    data->run = True;
    data->sxReader = ThreadOp.inst( "sxReader", &__sxReader, __MttmFcc );
    ThreadOp.start( data->sxReader );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unable to initialize device; switch to dummy mode" );
    data->dummyio = True;
    data->sx1[0][8] = 0x40;
    data->sx1[0][112] = 0x01;
  }

  instCnt++;
  return __MttmFcc;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/mttmfcc.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
