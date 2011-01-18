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



#include "rocdigs/impl/muet_impl.h"

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

#include "rocdigs/impl/muet/muet.h"


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOMuetData data = Data(inst);
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

/** ----- OMuet ----- */

static iOSlot __getSlotByAddr(iOMuetData data, int lcaddr) {
  iOSlot slot = NULL;
  if( MutexOp.wait( data->lcmux ) ) {
    slot = (iOSlot)MapOp.first( data->lcmap);
    while( slot != NULL ) {
      if( slot->addr == lcaddr ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot found for %s by address %d", slot->id, lcaddr );
        break;
      }
      slot = (iOSlot)MapOp.next( data->lcmap);
    };
    MutexOp.post(data->lcmux);
  }
  return slot;
}


static iOSlot __getSlot(iOMuetData data, iONode node) {
  int addr  = wLoc.getaddr(node);
  iOSlot slot = NULL;

  slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );
  if( slot != NULL ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "slot exist for %s", wLoc.getid(node) );
    return slot;
  }

  slot = allocMem( sizeof( struct slot) );
  slot->addr = addr;
  slot->bus = wLoc.getbus(node);
  slot->id = StrOp.dup(wLoc.getid(node));
  if( MutexOp.wait( data->lcmux ) ) {
    MapOp.put( data->lcmap, wLoc.getid(node), (obj)slot);
    MutexOp.post(data->lcmux);
  }

  /* activate monitoring for the loco */
  byte* cmd = allocMem(32);
  cmd[0] = slot->bus;
  cmd[1] = 3;
  cmd[2] = MONITORING;
  cmd[3] = MONITORING_ADD;
  cmd[4] = slot->addr & 0x7F;
  ThreadOp.post(data->writer, (obj)cmd);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "add monitoring for loco addr %d on bus %d", slot->addr, slot->bus );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "slot created for %s", wLoc.getid(node) );
  return slot;
}




static iOPoint __getPointByAddr(iOMuetData data, int bus, int addr, int port) {
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


static iOPoint __getPoint(iOMuetData data, iONode node) {
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

  /* activate monitoring for the point */
  byte* cmd = allocMem(32);
  cmd[0] = point->bus;
  cmd[1] = 3;
  cmd[2] = MONITORING;
  cmd[3] = MONITORING_ADD;
  cmd[4] = point->addr & 0x7F;
  ThreadOp.post(data->writer, (obj)cmd);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "add monitoring for point addr %d on bus %d", point->addr, point->bus );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "point created for %s", key );
  return point;
}




/* fbmods is a comman separated address list of connected feedback modules. */
static void __updateFB( iOMuet muet, iONode fbInfo ) {
  iOMuetData data = Data(muet);
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
        /* control register for ident */
        StrOp.fmtb(key, "%d_%d", bus, addr+1 );
        MapOp.put( data->identmap, key, (obj)&data->fbmods[bus][idx]); /* dummy object */

        /* activate monitoring for the unit occupancy */
        cmd = allocMem(32);
        cmd[0] = bus;
        cmd[1] = 3;
        cmd[2] = MONITORING;
        cmd[3] = MONITORING_ADD;
        cmd[4] = addr & 0x7F;
        ThreadOp.post(data->writer, (obj)cmd);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "add monitoring for occ addr %d on bus %d", addr, bus );

        /* activate monitoring for the unit control register */
        cmd = allocMem(32);
        cmd[0] = bus;
        cmd[1] = 3;
        cmd[2] = MONITORING;
        cmd[3] = MONITORING_ADD;
        cmd[4] = (addr+1) & 0x7F;
        ThreadOp.post(data->writer, (obj)cmd);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "add monitoring for ctrl addr %d on bus %d", addr+1, bus );

        idx++;
      };
      data->fbmodcnt[bus] = idx;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "updateFB bus=%d count=%d", bus, idx );
    }
  }

}





static void __translate( iOMuet muet, iONode node ) {
  iOMuetData data = Data(muet);

  if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
    __updateFB( muet, node );
  }
  
  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmdstr = wSysCmd.getcmd( node );
    if( StrOp.equals( cmdstr, wSysCmd.stop ) ) {
      /* CS on */
      byte* cmd = allocMem(32);
      cmd[0] = 0;
      cmd[1] = 2;
      cmd[2] = CS_SET_STATUS;
      cmd[3] = CS_OFF;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command: power OFF" );
      ThreadOp.post(data->writer, (obj)cmd);
    }
    if( StrOp.equals( cmdstr, wSysCmd.go ) ) {
      /* CS on */
      byte* cmd = allocMem(32);
      cmd[0] = 0;
      cmd[1] = 2;
      cmd[2] = CS_SET_STATUS;
      cmd[3] = CS_ON;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command: power ON" );
      ThreadOp.post(data->writer, (obj)cmd);
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
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command: switch %d", wSwitch.getaddr1( node ) );
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
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command: output %d, %d", wOutput.getaddr( node ), wOutput.getport( node ) );
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
        speed = (wLoc.getV( node ) * 31) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * 31) / wLoc.getV_max( node );
    }

    byte *cmd = allocMem(32);
    cmd[0] = slot->bus;
    cmd[1] = 2;

    cmd[2] = addr & 0x7F;
    cmd[2] |= WRITE_FLAG;
    cmd[3] = speed & 0x1F;
    cmd[3] |= dir ? 0x00:0x20;
    cmd[3] |= fn  ? 0x00:0x40;
    cmd[3] |= slot->fn ? 0x80:0x00;

    slot->speed = speed;
    slot->dir = wLoc.isdir(node);
    slot->lights = wLoc.isfn(node);
    slot->lastcmd = SystemOp.getTick();

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command: loco %d", addr );
    ThreadOp.post(data->writer, (obj)cmd);
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

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command: function %d", addr );
    ThreadOp.post(data->writer, (obj)cmd);
  }


}


/**  */
static iONode _cmd( obj inst ,const iONode nodeA ) {
  iOMuetData data = Data(inst);

  if( nodeA != NULL ) {
    int bus = 0;
    __translate( (iOMuet)inst, nodeA );
    nodeA->base.del(nodeA);
  }
  return NULL;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOMuetData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );

  /* CS off */
  byte* cmd = allocMem(32);
  cmd[0] = 0;
  cmd[1] = 2;
  cmd[2] = CS_SET_STATUS;
  cmd[3] = CS_OFF;
  ThreadOp.post(data->writer, (obj)cmd);
  ThreadOp.sleep(500);
  data->run = False;
  ThreadOp.sleep(100);
  SerialOp.close( data->serial );
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOMuetData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
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


static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOMuetData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMuet muet = (iOMuet)ThreadOp.getParm( th );
  iOMuetData data = Data(muet);
  byte* cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer started." );

  /* monitoring off and query active bus */
  cmd = allocMem(32);
  cmd[0] = 0;
  cmd[1] = 3;
  cmd[2] = MONITORING;
  cmd[3] = MONITORING_OFF;
  cmd[4] = SX_GET_BUS;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command: monitoring OFF and get active bus" );
  ThreadOp.post(th, (obj)cmd);

  /* CS on */
  cmd = allocMem(32);
  cmd[0] = 0;
  cmd[1] = 2;
  cmd[2] = CS_SET_STATUS;
  cmd[3] = CS_ON;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command: power ON" );
  ThreadOp.post(th, (obj)cmd);

  /* monitoring bus 0 */
  cmd = allocMem(32);
  cmd[0] = 0;
  cmd[1] = 3;
  cmd[2] = MONITORING;
  cmd[3] = MONITORING_ON;
  cmd[4] = SX_BUS0;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command: monitoring ON" );
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

      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
      if( !SerialOp.write( data->serial, (char*)out, len ) ) {
        /* sleep and send it again? */
      }
    }

  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writer ended." );
}


static __evaluateFB( iOMuet muet, byte in, int addr, int bus ) {
  iOMuetData data = Data(muet);

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


static Boolean __updateSlot(iOMuetData data, iOSlot slot, Boolean* vdfChanged, Boolean* funChanged) {
  Boolean changed = False;
  int     speed   = 0;
  Boolean dir     = True;
  Boolean lights  = False;
  Boolean fn      = False;

  /* SX1 */
  byte sx1 = data->sx1[slot->bus&0x01][slot->addr&0x7F];
  speed  = sx1 & 0x1F;
  dir    = (sx1 & 0x20) ? False:True;
  lights = (sx1 & 0x40) ? False:True;
  fn     = (sx1 & 0x80) ? True:False;

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
  if( slot->fn != fn ) {
    /* trace functions changed */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "function change event from %s to %s for %s", slot->fn?"on":"off", fn?"on":"off", slot->id );
    slot->fn = fn;
    *funChanged = True;
    changed = True;
  }

  return changed;
}







static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMuet muet = (iOMuet)ThreadOp.getParm( th );
  iOMuetData data = Data(muet);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader started." );
  
  while( data->run ) {
    byte in[8] = {0};
    if( SerialOp.available(data->serial) ) {
      if( SerialOp.read(data->serial, in, 1) ) {
        if(in[0] == 126 ) {
          if( SerialOp.read(data->serial, in+1, 1) ) {
            TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, 2 );
            data->activebus = in[1] & 0x7F;
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "active bus=%d.", data->activebus );
          }
        }
        else if(in[0] >= 128 && in[0] <= (128 + 31) ) {
          if( SerialOp.read(data->serial, in+1, 2) ) {
            char key[32] = {'\0'};
            int bus  = (in[0]-128) & 0x7F;
            int addr = in[1] & 0x7F;
            int val  = in[2] & 0xFF;
            TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, 3 );

            StrOp.fmtb(key, "%d_%d", bus, addr );
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                "monitor event: bus=%d, addr=%d val=%02X key=%s.", bus, addr, val, key );

            if( MapOp.haskey( data->identmap, key) ) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "key=%s is a ident sensor unit.", key );
              data->fbstate[bus][addr] = val;
              /* get the loco number */
              byte *cmd = allocMem(32);
              cmd[0] = bus;
              cmd[1] = 1;
              cmd[2] = (addr+1) & 0x7F;
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command: get loco number for unit %d", addr );
              ThreadOp.post(data->writer, (obj)cmd);
            }
            else if( MapOp.haskey( data->fbmap, key) ) {
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "occupancy for unit %d is %02X", addr, val );
              __evaluateFB( muet, val, addr, data->activebus );
            }
            else {
              /* Loco or Point */
              iOSlot slot = __getSlotByAddr( data, addr );
              /* this is considerred as an echo if the last command was not longer ago then 1 second */
              if( slot != NULL && ( SystemOp.getTick() - slot->lastcmd > 100 ) ) {
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
                    wLoc.setV_raw( nodeC, slot->speed );
                    wLoc.setV_rawMax( nodeC, 31 );
                    wLoc.setfn( nodeC, slot->lights);
                    wLoc.setdir( nodeC, slot->dir );
                    wLoc.setcmd( nodeC, wLoc.direction );
                    wLoc.setthrottleid( nodeC, "slx-bus" );
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

                    wLoc.setthrottleid( nodeC, "slx-bus" );
                    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
                  }
                }
              }
              else {
                if( data->sx1[bus&0x01][addr] != val ) {
                  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "point update %d=%d", addr, val );
                  int i = 0;
                  for( i = 0; i < 8; i++ ) {
                    int oldval = data->sx1[bus&0x01][addr] & (0x01 << i);
                    int newval = val & (0x01 << i);
                    if( oldval != newval ) {
                      iOPoint point = __getPointByAddr( data, bus, addr, i+1 );
                      if( point != NULL  && ( SystemOp.getTick() - point->lastcmd > 100 )  ) {
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
                  data->sx1[bus&0x01][addr] = val;
                }
              }


            }

          }
        }
        else if(in[0] < 126 ) {
          int addr = in[0] & 0x7F;
          if( SerialOp.read(data->serial, in+1, 1) ) {
            char key[32] = {'\0'};
            int val = in[1] & 0x7F;
            TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, 2 );

            StrOp.fmtb(key, "%d_%d", data->activebus, addr-1 );
            if( val > 0 && MapOp.haskey( data->identmap, key) ) {
              byte ctrl = data->fbstate[data->activebus][addr-1];
              int port = ctrl & 0x07;
              int rraddr = addr*8+port+1;
              Boolean arrived = (ctrl & 0x08) ? True:False;
              iONode evt = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );

              port++; /* port is zero based offset */
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                  "loco address for unit %d:%d(%d) is %d and did %s", addr, port, rraddr, val, arrived?"arrive":"depart" );

              wFeedback.setstate( evt, arrived );
              wFeedback.setaddr( evt, rraddr );
              wFeedback.setbus( evt, data->activebus );
              wFeedback.setfbtype( evt, wFeedback.fbtype_lissy );
              wFeedback.setidentifier( evt, arrived?val:0 );
              if( data->iid != NULL )
                wFeedback.setiid( evt, data->iid );

              data->listenerFun( data->listenerObj, evt, TRCLEVEL_INFO );
            }
          }
        }
      }
    }
    else {
      ThreadOp.sleep(10);
    }
  }
  
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reader ended." );
}


/**  */
static struct OMuet* _inst( const iONode ini ,const iOTrace trc ) {
  iOMuet __Muet = allocMem( sizeof( struct OMuet ) );
  iOMuetData data = allocMem( sizeof( struct OMuetData ) );
  MemOp.basecpy( __Muet, &MuetOp, 0, sizeof( struct OMuet ), data );

  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */

  /* Evaluate attributes. */
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
  data->identmap = MapOp.inst();
  data->pointmap = MapOp.inst();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "muet %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid[%s]: %s,%d,%d",
        wDigInt.getiid( ini ) != NULL ? wDigInt.getiid( ini ):"",
        data->device, data->bps, data->timeout );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, cts );
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, none, wDigInt.isrtsdisabled( ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );

  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {

    data->reader = ThreadOp.inst( "muetreader", &__reader, __Muet );
    ThreadOp.start( data->reader );
    data->writer = ThreadOp.inst( "muetwriter", &__writer, __Muet );
    ThreadOp.start( data->writer );

  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init Muet port!" );
  }

  instCnt++;
  return __Muet;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/muet.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
