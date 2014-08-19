/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

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


#include "rocdigs/impl/spl_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/SPL.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOSPLData data = Data(inst);
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

/** ----- OSPL ----- */

/*
    Adressering van de module
    Voer de volgende tekenreeks in om een adres in het interne EEPROM-geheugen van de PIC- te zetten:

    ‘H’ : Huis
    ‘F’ : Hoogste byte van het hexadecimale adres: F voor niet-geprogrammeerde modules
    ‘F’ : Laagste byte van het hexadecimale adres: F voor niet-geprogrammeerde modules
    ‘P’ : Commando P voor programmeren
    ‘a’
    ‘a’
    ‘5’: Beveiligingscode tegen per ongeluk programmeren
    ‘5’: Beveiligingscode tegen per ongeluk programmeren
    ‘x’: Hoogste byte van het te programmeren hex-adres: x = 0-9, A-F
    ‘y’: Laagste byte van het te programmeren hex-adres: y = 0-9, A-F
    ‘CR’: Carriage Return als afsluiting

    Voer in het terminalprogramma ‘HFFPaa55’ in. De module reageert door alle LED’s in te schakelen.
    Voer dan het gewenste adres in (xy) en stuur een CR (druk op de Enter-toets). De LED’s doven dan.
    Als nu de voeding wordt uit- en weer inge- schakeld, is het adres geprogrammeerd.
    Gebruik dezelfde procedure om het adres van de module later te veranderen als dat nodig is.
    Voorbeeld: Om een nog niet geprogrammeerde module in te stellen op adres 23h, voeren we ‘HFFPaa5523’ in en drukken op ‘ENTER’.

    LED-patronen instellen
    Aansturen van een module:
    ‘H’ : Huis
    ‘x’: Hoogste byte van het adres van het aan te sturen huis, x = 0-9, A-F
    ‘y’: Laagste byte van het adres van het aan te sturen huis, y = 0-9, A-F
    ‘S’: Commando S voor Set
    ‘a’: In te schakelen LED’s: hoogste byte, 0-1
    ‘b’: In te schakelen LED’s: laagste byte, 0-9, A-F
    ‘CR’: Carriage Return als afsluiting

    De invoer is dus HxySab ENTER, waarbij xy het adres van de module aangeeft en na
    de S de hexadecimale waarde van de in te schakelen LED’s volgt. Met het bereik van
    00h tot 1Fh zijn alle 32 mogelijke combinaties van de vijf LED’s afgedekt.

    Voorbeeld: Om LED 3 en 5 van module 12 te activeren, voeren we H12S14 ENTER in op de terminal.

 */

static void __setLED(iOSPL inst, int addr, int port, Boolean state, Boolean trace ) {
  iOSPLData data = Data(inst);

  if( addr < 256 && addr > 0 && port <= 8 && port > 0 ) {
    TraceOp.trc( name, trace?TRCLEVEL_MONITOR:TRCLEVEL_DEBUG, __LINE__, 9999, "home=%d LED=%d %s", addr, port, state?"ON":"OFF" );
    byte led = 1 << (port-1);
    data->home[addr] = data->home[addr] & (~led);
    if( state )
      data->home[addr] |= led;
    char* cmd = allocMem( 32 );
    StrOp.fmtb(cmd+1, "H%02XS%02X\r", addr, data->home[addr]);
    cmd[0] = StrOp.len(cmd+1);
    ThreadOp.post( data->writer, (obj)cmd );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unsupported addressing: home=%d LED=%d", addr, port );
  }
}


static void __flipLED(iOSPL inst, int addr, int port ) {
  iOSPLData data = Data(inst);
  byte led = 1 << (port-1);
  Boolean state = (data->home[addr] & led) ? True:False;
  __setLED(inst, addr, port, !state, False );
}


static iONode __translate( iOSPL inst, iONode node ) {
  iOSPLData data = Data(inst);
  iONode    rsp  = NULL;

  /* Switch command. */
  if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int addr  = wSwitch.getaddr1( node );
    int port  = wSwitch.getport1( node );
    Boolean state = StrOp.equals( wSwitch.getcmd( node ), wSwitch.straight ) ? 1:0;
    __setLED(inst, addr, port, state, True);
  }

  /* Output command */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int addr   = wOutput.getaddr( node );
    int port   = wOutput.getport( node );
    Boolean state = StrOp.equals( wOutput.getcmd( node ), wOutput.on );
    __setLED(inst, addr, port, state, True);
  }

  /* Signal command */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    int addr   = wSignal.getaddr(node);
    int aspect = wSignal.getaspect(node);
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "signal addr=%d aspect=%d", addr, aspect );
    char* cmd = allocMem( 32 );
    StrOp.fmtb(cmd+1, "H%02XS%02X\r", addr, aspect);
    cmd[0] = StrOp.len(cmd+1);
    ThreadOp.post( data->writer, (obj)cmd );
  }

  /* Program command */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    if( wProgram.getcmd( node ) == wProgram.set ) {
      if( wProgram.getcv(node) == 1 ) {
        char* cmd = allocMem( 32 );
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "programming module address from 0x%02X to 0x%02X",
            wProgram.getaddr(node), wProgram.getvalue(node) );
        StrOp.fmtb(cmd+1, "H%02XPaa55%02X\r", wProgram.getaddr(node), wProgram.getvalue(node));
        cmd[0] = StrOp.len(cmd+1);
        ThreadOp.post( data->writer, (obj)cmd );

        rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
        if( data->iid != NULL )
          wProgram.setiid( rsp, data->iid );
        wProgram.setcmd( rsp, wProgram.datarsp );
        wProgram.setcv( rsp, wProgram.getcv( node ) );
        wProgram.setvalue( rsp, wProgram.getvalue( node ) );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "unsupported cv %d", wProgram.getcv(node) );
        rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
        if( data->iid != NULL )
          wProgram.setiid( rsp, data->iid );
        wProgram.setcmd( rsp, wProgram.statusrsp );
        wProgram.setcv( rsp, wProgram.getcv( node ) );
        wProgram.setvalue( rsp, -1 );
      }
    }

    else if( wProgram.getcmd( node ) == wProgram.nvset ) {
      if( wProgram.getcv(node) == 1 ) {
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "random range address from 0x%02X to 0x%02X with %d LEDs",
            wProgram.getval1(node), wProgram.getval2(node), wProgram.getval3(node) );
        data->fromAddr = wProgram.getval1(node);
        data->toAddr = wProgram.getval2(node);
        data->nrLEDs = wProgram.getval3(node);
        wSPL.setfromaddr(data->splini, data->fromAddr );
        wSPL.settoaddr(data->splini, data->toAddr );
        wSPL.setnrleds(data->splini, data->nrLEDs );
      }
    }
  }

  return rsp;
}

/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOSPLData data = Data(inst);
  iONode    rsp  = NULL;

  if( cmd != NULL ) {
    rsp = __translate((iOSPL)inst, cmd);
    cmd->base.del(cmd);
  }
  return rsp;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  iOSPLData data = Data(inst);
  data->run = False;
  ThreadOp.sleep(500);
  if( data->serial != NULL ) {
    SerialOp.base.del(data->serial);
  }
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOSPLData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


/**  */
static Boolean _setRawListener( obj inst ,obj listenerObj ,const digint_rawlistener listenerRawFun ) {
  return 0;
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
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOSPLData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static void __control( void* threadinst ) {
  iOThread  th       = (iOThread)threadinst;
  iOSPL     spl      = (iOSPL)ThreadOp.getParm( th );
  iOSPLData data     = Data(spl);
  Boolean   serialOK = False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SPL control started." );

  while( data->run ) {
    int randSleep = (rand() % 3000) + 500;
    ThreadOp.sleep(randSleep);
    if( data->fromAddr > 0 && data->toAddr >= data->fromAddr ) {
      int randAddr   = rand() % ((data->toAddr - data->fromAddr) + 2);
      int randPort   = rand() % (data->nrLEDs + 1);
      if( randAddr > 0 && randPort > 0 )
        __flipLED(spl, randAddr, randPort);
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SPL control ended." );
}


static void __serialWriter( void* threadinst ) {
  iOThread  th       = (iOThread)threadinst;
  iOSPL     spl      = (iOSPL)ThreadOp.getParm( th );
  iOSPLData data     = Data(spl);
  Boolean   serialOK = False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SPL serial writer started." );

  ThreadOp.sleep(500); /* resume some time to get it all being setup */


  while( data->run ) {

    if( !serialOK ) {
      data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );
      SerialOp.setFlow( data->serial, 0 );
      SerialOp.setLine( data->serial, 9600, 8, 1, none, wDigInt.isrtsdisabled( data->ini ) );
      SerialOp.setTimeout( data->serial, wDigInt.gettimeout(data->ini), wDigInt.gettimeout(data->ini) );
      serialOK = SerialOp.open( data->serial );
      if( !serialOK ) {
        SerialOp.base.del(data->serial);
        data->serial = NULL;
        ThreadOp.sleep(2500);
        continue;
      }
    }

    byte* cmd = (byte*)ThreadOp.getPost( th );
    if (cmd != NULL) {
      int len = cmd[0];
      TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)cmd+1, len );
      if( !SerialOp.write( data->serial, (char*)cmd+1, len ) ) {
        SerialOp.base.del(data->serial);
        data->serial = NULL;
        serialOK = False;
      }
      freeMem( cmd );
    }

    ThreadOp.sleep(100);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SPL serial writer end." );
}

/**  */
static struct OSPL* _inst( const iONode ini ,const iOTrace trc ) {
  iOSPL __SPL = allocMem( sizeof( struct OSPL ) );
  iOSPLData data = allocMem( sizeof( struct OSPLData ) );
  MemOp.basecpy( __SPL, &SPLOp, 0, sizeof( struct OSPL ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  SystemOp.inst();

  data->ini      = ini;
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );
  data->run      = True;
  data->splini   = wDigInt.getspl(ini);
  data->nrLEDs   = 5;

  if( data->splini == NULL ) {
    data->splini = NodeOp.inst( wSPL.name(), ini, ELEMENT_NODE );
    NodeOp.addChild(ini, data->splini);
  }

  data->fromAddr    = wSPL.getfromaddr(data->splini);
  data->toAddr      = wSPL.gettoaddr(data->splini);
  data->nrLEDs      = wSPL.getnrleds(data->splini);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SPL %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid        = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device     = %s", wDigInt.getdevice( data->ini ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "addr range = %d-%d", data->fromAddr, data->toAddr );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LEDs       = %d", data->nrLEDs );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  /* 9600-n-1 */
  data->writer = ThreadOp.inst( "splwriter", &__serialWriter, __SPL );
  ThreadOp.start( data->writer );

  data->control = ThreadOp.inst( "splctrl", &__control, __SPL );
  ThreadOp.start( data->control );

  return __SPL;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/spl.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
