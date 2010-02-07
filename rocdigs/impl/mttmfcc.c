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
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/FbInfo.h"
#include "rocrail/wrapper/public/FbMods.h"

#include "rocdigs/impl/common/fada.h"

static int instCnt = 0;

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
static void __evaluateRsp( iOMttmFccData data, byte* out, int outsize, byte* in, int insize ) {
}

static Boolean __setActiveBus( iOMttmFccData data, int bus ) {

  if( bus < 32 && data->activebus != bus ) {
    byte cmd[2];
    cmd[0] = 126;
    cmd[1] = bus;
    cmd[0] |= 0x80;

    data->activebus = bus;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set active bus to [%d]", bus );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)cmd, 2 );
    if( data->dummyio )
      return True;
    else
      return SerialOp.write( data->serial, (char*)cmd, 2 );
  }
  return True;
}


static Boolean __transact( iOMttmFccData data, byte* out, int outsize, byte* in, int insize, int bus ) {
  Boolean rc = False;
  if( MutexOp.wait( data->mux ) ) {
    Boolean ok = __setActiveBus( data, bus );
    TraceOp.dump( name, TRCLEVEL_BYTE, out, outsize );
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
    MutexOp.post( data->mux );
  }
  return rc;
}


static iOSlot __getSlot(iOMttmFccData data, iONode node) {
  int steps = wLoc.getspcnt(node);
  int addr  = wLoc.getaddr(node);
  byte index = 0xFF;
  iOSlot slot = NULL;
  byte cmd[32] = {0x79, 0x01};


  /*
    Die DCC-Lok mit der langen Adresse 1234 und 126 Fahrstufen soll an die FCC- Digitalzentrale angemeldet werden:
    Bestimmung: 1234 (binär: 00010011010010) DCC-Lokadresse: 00010011010010 00 entspricht 0x1348
    Es ist daher Folgendes an die FCC-Digitalzentrale zu senden: Vom PC:  0x79  0x01  0x13  0x48  0x07
   */
  if( StrOp.equals( wLoc.prot_N, wLoc.getprot(node) ) ) {
    /* short DCC */
    addr = addr << 2;
    cmd[4] = steps > 100 ? 0x05:0x01;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "short DCC" );
  }
  else if( StrOp.equals( wLoc.prot_L, wLoc.getprot(node) ) ) {
    /* long DCC */
    addr = addr << 2;
    cmd[4] = steps > 100 ? 0x07:0x03;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "long DCC" );
  }
  else if( StrOp.equals( wLoc.prot_M, wLoc.getprot(node) ) ) {
    /* MM */
    /*
    Die MM-Lok mit der Adresse 218 soll an die FCC-Digitalzentrale angemeldet werden:
    Bestimmung: 218 (binär: 00000011011010) MM-Lokadresse: 00000011011010 00 entspricht 0x0368
    Es ist daher Folgendes an die FCC-Digitalzentrale zu senden: Vom PC:  0x79  0x01  0x03  0x68  0x02
     */
    addr = addr << 2;
    cmd[4] = 0x02;
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "MM" );
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
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "default SX2" );
  }

  cmd[2] = addr / 256;
  cmd[3] = addr % 256;


  if( __transact( data, cmd, 5, &index, 1, wLoc.getbus(node) ) ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "got index %d for %s", index, wLoc.getid(node) );
    slot = allocMem( sizeof( struct slot) );
    slot->index = index;
    slot->protocol = cmd[4];
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
  MemOp.set( data->fbmodcnt, 0, 32 * sizeof(int) );
  MemOp.set( data->fbmods, 0, 32*256 );

  for( i = 0; i < cnt; i++ ) {
    iONode fbmods = NodeOp.getChild( fbInfo, i );
    const char* mods = wFbMods.getmodules( fbmods );
    int bus = wFbMods.getbus( fbmods );
    if( mods != NULL && StrOp.len( mods ) > 0 ) {

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

}


static int __translate( iOMttmFccData data, iONode node, byte* out, int *insize, int* bus ) {
  *insize = 0;
  *bus = 0;

  if( StrOp.equals( NodeOp.getName( node ), wFbInfo.name() ) ) {
    __updateFB( data, node );
  }
  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    byte pin = 0x01 << ( wSwitch.getport1( node ) - 1 );
    byte mask = ~pin;
    *bus = wSwitch.getbus( node ) & 0x1F;
    out[0] = wSwitch.getaddr1( node );
    out[1] = 0x01 << ( wSwitch.getport1( node ) - 1 );
    out[0] |= 0x80;

    /* reset pin to 0: */
    out[1] = data->swstate[*bus][out[0]] & mask;

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      out[1] |= pin;
    /* save new state: */
    data->swstate[*bus][out[0]] = out[1];
    return 2;
  }

  /* Output command */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x01:0x00;
    byte pin = 0x01 << ( port - 1 );
    byte mask = ~pin;

    *bus = wOutput.getbus(node);

    out[0] = addr;
    out[0] |= 0x80;

    /* reset pin to 0: */
    out[1] = data->swstate[*bus][out[0]] & mask;

    if( action )
      out[1] |= pin;
    /* save new state: */
    data->swstate[*bus][out[0]] = out[1];


    return 2;
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
    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
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
    Boolean isSx1 = False;
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    byte cmd = 0;
    byte rc = 0;
    Boolean fn = wLoc.isfn( node );
    int    dir = wLoc.isdir( node );
    int  spcnt = wLoc.getspcnt( node );

    int index = 0;

    *bus = wLoc.getbus(node);


    iOSlot slot = (iOSlot)MapOp.get( data->lcmap, wLoc.getid(node) );

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * spcnt) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * spcnt) / wLoc.getV_max( node );
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d speed=%d steps=%d lights=%s dir=%s",
        addr, speed, spcnt, fn?"on":"off", dir?"forwards":"reverse" );

    if( StrOp.equals( wLoc.prot_S, wLoc.getprot(node) ) ) {
      /* native selectrix SX1 */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "native SX1" );
      out[0] = wLoc.getaddr(node);
      out[0] |= 0x80;
      /*
      SX1-Loks werden wie bisher direkt über die entsprechenden SX1-Kanäle des SX1/2-Bus- Systems 0 gesteuert.
      Die Kanalbelegung ist daher selbstverständlich unverändert geblieben:
      Bits 0 bis 4 Bit 5 Bit 6 Bit 7
      Fahrstufen von 0 bis 31 Fahrtrichtung (0 entspricht vorwärts, 1 entspricht rückwärts)
      Licht (0 bedeutet Licht aus, 1 bedeutet Licht ein) Horn (0 bedeutet Horn aus, 1 bedeutet Horn ein)
      */
      out[1] = speed & 0x1F;
      out[1] |= wLoc.isdir(node) ? 0x00:0x20;
      out[1] |= wLoc.isfn(node)  ? 0x00:0x40;
      return 2;
    }


    if( slot == NULL ) {
      slot = __getSlot(data, node );
      if( slot != NULL ) {
        MapOp.put( data->lcmap, wLoc.getid(node), (obj)slot);
      }
    }

    if( slot == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not get index for loco %s", wLoc.getid(node) );
      return 0;
    }

    index = slot->index;


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
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
    return 5;

  }

  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    if( StrOp.equals( wLoc.prot_S, wLoc.getprot(node) ) ) {
      /* native selectrix SX1 */
      int   addr = wFunCmd.getaddr( node );
      Boolean f1 = wFunCmd.isf1( node );
      Boolean f2 = wFunCmd.isf2( node );
      Boolean f3 = wFunCmd.isf3( node );
      Boolean f4 = wFunCmd.isf4( node );
      *bus  = wFunCmd.getbus( node ) & 0x1F;

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "native SX1" );

      out[0] = addr;
      out[0] |= 0x80;
      out[1] = data->lcstate[*bus][addr] & 0x7F;
      out[1] |= f1 ? 0x80:0x00;

      data->lcstate[*bus][addr] = out[1];

      return 2;
    }
  }

  return 0;
}


static __evaluateFB( iOMttmFcc fcc, byte in, int addr, int bus ) {
  iOMttmFccData data = Data(fcc);

  if( in != data->fbstate[bus][addr] ) {
    int n = 0;
    int pin = 0;
    int state = 0;
    for( n = 0; n < 8; n++ ) {
      if( (in & (0x01 << n)) != (data->fbstate[bus][addr] & (0x01 << n)) ) {
        pin = n;
        state = (in & (0x01 << n)) ? 1:0;
        TraceOp.dump ( name, TRCLEVEL_BYTE, (char*)&in, 1 );
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


static void __feedbackReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOMttmFcc fcc = (iOMttmFcc)ThreadOp.getParm( th );
  iOMttmFccData data = Data(fcc);
  unsigned char* fb = allocMem(256);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback reader started." );

  /* TODO: bus support > 0 */

  while( data->run ) {
    int i, n = 0;

    ThreadOp.sleep( 100 );

    for( n=0; n < 32; n++ ) {
      if( data->fbmodcnt[n] == 0 )
        continue;

      for( i = 0; i < data->fbmodcnt[n]; i++ ) {
        byte cmd[2];
        byte  in[2];
        cmd[0] = data->fbmods[n][i] & 0x7F;
        cmd[1] = 0;
        if( __transact( data, cmd, 2, in, 1, n) ) {
          __evaluateFB( fcc, in[0], data->fbmods[n][i], n );
        }
      }
    }
  };
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback reader ended." );
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
    int bus = 0;
    int size = __translate( data, cmd, out, &insize, &bus );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, out, size );
    if( __transact( data, (char*)out, size, (char*)in, insize, bus ) ) {
    }
  }

  return reply;
}


/**  */
static void _halt( obj inst ) {
  iOMttmFccData data = Data(inst);
  data->run = False;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
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
static int patch  = 0;
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
  data->mux     = MutexOp.inst( NULL, True );
  data->lcmap   = MapOp.inst();

  data->device   = StrOp.dup( wDigInt.getdevice( ini ) );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MTTM FCC %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid      = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device   = %s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, none );
  SerialOp.setLine( data->serial, 230400, 8, 1, none, wDigInt.isrtsdisabled( ini ) );
  data->serialOK = SerialOp.open( data->serial );

  if(data->serialOK) {
    data->feedbackReader = ThreadOp.inst( "feedbackReader", &__feedbackReader, __MttmFcc );
    ThreadOp.start( data->feedbackReader );
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
