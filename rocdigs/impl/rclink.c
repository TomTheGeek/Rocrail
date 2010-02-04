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

/* PROTOCOL
 * ------------------------------------------------------------
Nach dem Einschalten des RC-Link befindet sich dieser in einem Schlafmodus. Um den RC-Link zu aktivieren reicht es,
ein beliebiges Kommando, ausser E0, zu senden.

RC-Talk-Meldungen:

Generell beginnen Meldungen richtung PC mit einem Startbyte, welches die Meldungsart beschreibt,
gefolgt von einer Detektoradresse (Hex-Format) oder bei Systemmeldung mit der Softwareversion.
Ein derartiges Datagramm enthaelt entweder ein oder zwei Bytes und endet stets mit 0xFF

Es wird zwischen Adressmeldung, CV-Meldung und Systemmeldung unterschieden.

Adressmeldung:

0xFC DD* A1* A2* 0xFF

DD = Detektoradresse (0x01 - 0x18)
A1 = Adressteil 1 higher byte (bei langer DCC-Adresse ist Bit 7 gesetzt!)
A2 = Adressteil 2 lower byte
*Werte im Hex-Format

Eine Adressmeldung wird einmal generiert sobald ein Detektorabschnitt die Daten eines RailCom-Senders empfaengt
und wenn der Abschnitt frei ist (dann werden 00 00 fuer A1 und A2 gesendet). Bei der Inbetriebnahme werden die
Stati aller Detektoren einmal gesendet.
Ferner kann eine Adressmeldung mittels eines RC-Talk-Kommandos aus dem PC fuer einen Detektor angefordert werden.

CV-Meldung:

0xFE DD* CV* 0xFF
*Werte im Hex-Format

Eine CV-Meldung wird nur generiert, wenn diese von einem im Detektorabschnitt befindlichen Decoder ausgesendet wird.
Dies wird ueber eine geeignete Zentrale initiiert. CV-Werte koennen nicht ueber RC-Talk angefordert werden.

Systemmeldung:

0xFD Sh* Sl* SV* HV* 0xFF
*Werte im Hex-Format

Sh = Seriennummer higher Byte
Sl = Seriennummer lower Byte
SV = Softwareversion ( /10 also z.B. 0x04 == Version 0.4)
HV = Hardwareversion ( /10 also z.B. 0x0A == Version 1.0)


Diagnosemeldung:
0xD1 A1 A2 0xD2 V1 V2 0xD3 SB AB 0xD4 OSCCAL 0xFF
A1, A2 = Adressbyte 1 und 2 (bestaetigt)
V1, V2 = Vergleichsbytes 1 und 2 (gemeldete, unbestaetigte Adressen)
SB = Statusbyte
AB = Alivebyte
OSCCAL = Kallibrierwert fuer internen Oszillator

System-Aus-Meldung:

0xFA 0xFF
Diese Meldung wird generiert, wenn der RC-Link deaktiviert wird, z.B. durch das Kommando E0 oder durch ein ungueltiges Kommando.


Eine Systemmeldung wird durch ein RC-Talk-Kommando initiiert und enthaelt Informationen ueber die Hard-bzw.
Software des RC-Link

RC-Talk-Kommandos:

Grundsaetzlich ist ein RC-Talk-Kommando 1 Byte gross und erlaubt, verschiedene Funktionen des RC-Link auszufuehren.

0x20 = Initialisiere alle:

Wird der Wert 0x20 an den RC-Link gesendet werden alle Detektoren initialisiert und die Adressdaten
aller Detektoren gesendet (entspricht funktional einem Reset)

0x40 + DD = Abfrage explizit

DD = Detektoradresse als Hexwert (0x01 - 0x18)

Wird der Hexwert 0x40, addiert mit der gewuenschten Detektoradresse, an den RC-Link gesendet wird eine
Adressmeldung fuer diesen Detektor generiert. Z.B. wird ueber 0x42 der Detektor 2 abgefragt.

0x60 = Sende Info

Auf dieses Kommando erfolgt eine Systemmeldung, mit deren Hilfe die Softwareversion und die Seriennummer
abgefragt werden kann

0x80 + DD = Programmiere Detektor

Wird der Hexwert 0x80, addiert mit der gewuenschten Detektoradresse, an das RC-Talk-Modul gesendet wird dem am
Bus angeschlossenen Detektor, welcher vorher via Steckbruecke in den Programmiermodus versetzt wurde,
die Detektoradresse DD zugewiesen.

0xA0 + DD = Anforderung Diagnosedaten

Wird das Kommando 0x80, addiert mit der Detektoradresse, gesendet, werden alle Daten zum gewuenschten
Detektor (Adressbytes 1, 2, Vergleichsbytes 1, 2, Alive-Byte, Stausbyte) aus dem Speicher abgefragt und zur Anzeige gebracht.

0xC0 = Umschalten auf Scope-Modus

Das Kommando 0xC0 bewirkt, dass der RC-Link in einen Scope-Modus umgeschalten wird. Damit werden die Daten,
welche ueber den RS485-Bus ankommen, ohne weitere Bearbeitung an die PC-Schnittstelle durchgereicht.
In den Talk-Modus bringt man das Modul indem man es neu initialisiert (Kommando 0x20) oder aus/ein-schaltet.

0x21 = Datenverkehr ausschalten

Nach dem Kommando 21 wird der RS485-Bus nicht mehr ausgelesen und keine Daten mehr an die RS232 zum PC gesendet.
Der RC-Link wird damit quasi ausser Betrieb gesetzt und kann mit jedem anderen Kommando wieder aktiviert werden.

*/

#include "rocdigs/impl/rclink_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Feedback.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iORcLinkData data = Data(inst);
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

/** ----- ORcLink ----- */


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  /* Cleanup cmd node to avoid memory leak. */
  cmd->base.del(cmd);
  return NULL;
}


/**  */
static void _halt( obj inst ) {
  iORcLinkData data = Data(inst);
  data->run = False;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  SerialOp.close( data->serial );
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iORcLinkData data = Data(inst);
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


static void __RcLinkTicker( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iORcLink inst = (iORcLink)ThreadOp.getParm( th );
  iORcLinkData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "RcLink ticker started." );
  ThreadOp.sleep(1000);

  while( data->run ) {
    int i = 0;
    for( i = 0; i < 24; i++ ) {
      if( data->readerTick[i] > 0 && (SystemOp.getTick() - data->readerTick[i]) > 250 ) {
        iONode evt = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
        wFeedback.setstate( evt, False );
        wFeedback.setaddr( evt, i + 1 + data->fboffset );
        wFeedback.setbus( evt, wFeedback.fbtype_railcom );
        wFeedback.setidentifier( evt, 0 );
        if( data->iid != NULL )
          wFeedback.setiid( evt, data->iid );

        data->listenerFun( data->listenerObj, evt, TRCLEVEL_INFO );

        data->readerTick[i] = 0;
        ThreadOp.sleep( 100 );
      }
    }

    ThreadOp.sleep( 100 );
  };

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "RcLink ticker ended." );
}


static void __evaluateRC(iORcLink inst, byte* packet, int idx) {
  iORcLinkData data = Data(inst);

  TraceOp.dump( NULL, TRCLEVEL_BYTE, packet, idx );

  switch( packet[0] ) {
  case 0xD1:
    break;
  case 0xFA:
    break;
  case 0xFC:
    /* Address report */
  {
    iONode evt = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );

    wFeedback.setstate( evt, False );
    wFeedback.setaddr( evt, packet[1] );
    wFeedback.setbus( evt, wFeedback.fbtype_railcom );
    wFeedback.setidentifier( evt, packet[2]*256 + packet[3] );
    if( data->iid != NULL )
      wFeedback.setiid( evt, data->iid );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "detector %d reported address %d",
        packet[1], wFeedback.getidentifier(evt) );

    data->listenerFun( data->listenerObj, evt, TRCLEVEL_INFO );

    data->readerTick[packet[1]] = SystemOp.getTick();
  }
    break;
  case 0xFD:
    /* System report
      Sh = Seriennummer higher Byte
      Sl = Seriennummer lower Byte
      SV = Softwareversion ( /10 also z.B. 0x04 == Version 0.4)
      HV = Hardwareversion ( /10 also z.B. 0x0A == Version 1.0)
     */
  {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "SN=%d Software=%.1f Hardware=%.1f", packet[1]*256 + packet[2],
        ((float)packet[3])/10.0, ((float)packet[4])/10.0 );
  }
  break;
  case 0xFE:
    break;
  }
}


static Boolean __isStartByte( byte c, int *datalen ) {
  switch( c ) {
  case 0xD1:
    *datalen = 10;
    return True;
  case 0xFA:
    *datalen = 0;
    return True;
  case 0xFC:
    *datalen = 3;
    return True;
  case 0xFD:
    *datalen = 4;
    return True;
  case 0xFE:
    *datalen = 2;
    return True;
  }
  return False;
}


static void __RcLinkReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iORcLink inst = (iORcLink)ThreadOp.getParm( th );
  iORcLinkData data = Data(inst);
  Boolean ok = False;

  /* IO buffer */
  byte packet[256] = {0};
  int idx = 0;
  Boolean packetStart = False;

  ThreadOp.sleep(1000);

  data->initOK = False;

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "RcLink reader started." );

  while( data->run ) {
    int bAvail = SerialOp.available(data->serial);
    if (bAvail < 0) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error; exit reader." );
      break;
    }

    while (bAvail > 0) {
      byte c;
      int datalen = 0;
      SerialOp.read( data->serial, &c, 1 );
      if( !packetStart && __isStartByte( c, &datalen ) ) {
        /* STX */
        packetStart = True;
        idx = 0;
        packet[idx] = c;
        idx++;
      }
      else if(packetStart) {
        if( (idx-1) == datalen && c == 0xFF ) {
          /* ETX */
          packetStart = False;
          packet[idx] = c;
          idx++;
          /* evaluate the packet */
          __evaluateRC(inst, packet, idx);
        }
        else if( (idx-1) < datalen ) {
          packet[idx] = c;
          idx++;
        }
      }

      bAvail = SerialOp.available(data->serial);
      if (bAvail < 0) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error; exit reader." );
        break;
      }
    }


    ThreadOp.sleep( 10 );
  }

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "RcLink reader ended." );

}




/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 1;
static int vminor = 4;
static int patch  = 0;
static int _version( obj inst ) {
  iORcLinkData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**  */
static struct ORcLink* _inst( const iONode ini ,const iOTrace trc ) {
  iORcLink __RcLink = allocMem( sizeof( struct ORcLink ) );
  iORcLinkData data = allocMem( sizeof( struct ORcLinkData ) );
  MemOp.basecpy( __RcLink, &RcLinkOp, 0, sizeof( struct ORcLink ), data );

  TraceOp.set( trc );
  SystemOp.inst();

  /* Initialize data->xxx members... */
  data->device   = StrOp.dup( wDigInt.getdevice( ini ) );
  data->iid      = StrOp.dup( wDigInt.getiid( ini ) );

  data->bps      = wDigInt.getbps( ini );
  data->fboffset = wDigInt.getfboffset( ini );

  MemOp.set( data->readerTick, 0, sizeof(data->readerTick) );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "RcLink %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid      = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "device   = %s", data->device );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "baudrate = %d", data->bps );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fboffset = %d", data->fboffset );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, none );
  SerialOp.setLine( data->serial, data->bps, 8, 1, none, wDigInt.isrtsdisabled( ini ) );
  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {
    char* thname = NULL;
    data->run = True;

    thname = StrOp.fmt("rclinkread%X", __RcLink);
    data->reader = ThreadOp.inst( thname, &__RcLinkReader, __RcLink );
    StrOp.free(thname),
    ThreadOp.start( data->reader );

    thname = StrOp.fmt("rclinktick%X", __RcLink);
    data->ticker = ThreadOp.inst( thname, &__RcLinkTicker, __RcLink );
    StrOp.free(thname),
    ThreadOp.start( data->ticker );
  }
  else
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init rclink port!" );



  instCnt++;
  return __RcLink;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/rclink.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
