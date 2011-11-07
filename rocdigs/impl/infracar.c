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



/*
Baudrate 2400 8N1

Der gesamte Befehl setzt sich normal aus 3 Bytes zusammen.
1.Byte:Adresse LSB
2.Byte:Adresse MSB
3.Byte:Steuerbyte

Die Bytes sind durch die beiden höchstwertigen Bits gekennzeichnet.
Die Reihenfolge der Übertragung ist jedoch nicht beliebig, sondern
zwingend. Dadurch ergibt sich ein Stück Übertragungssicherheit.

Adresse LSB:
00aaaaaa
Kennung ist "00" danach folgt die Adresse (0..63)

Adresse MSB:
01bbbbbb

Kennung ist "01" danach folgt der MSB Teil der Adresse (0..63)
Diese Adresse ist derzeit generell 0, muss aber übertragen werden !

Steuerbyte:
Hier wird zwischen zwei Arten unterschieden.

Schaltfunktionen:
10cccccc
Kennung ist "10" danach kommen die Zustände der Schaltfunktionen.
Das niederwertigste Bit entspricht Kanal 1

Motorsteuerung und mehr:
11dddddd
Die Kennung für die Motorsteuerdaten ist "11",
die folgenden 6 Bit sind noch weiter unterteilt.

Motor direkt:
11ee1eee
"eeeee" gibt die PWM Rate in 32 Schritten an. Die "1" mittendrin ist kein
Tippfehler sondern muss entsprechen "zwischengefummelt" werden.

Motor Beschleunigung / bremsen
11000ppp
"ppp" ist der Fahrbefehl. 0....7
Dabei bedeuten:
0: Stop PWM sofort auf null
1: Keine Änderung
2: Stark bremsen
3: Bremsen
4: Beschleunigen
5: Stark beschleunigen
6: reserviert
7: reserviert
Beschleunigen und Bremsen wird durchgeführt bis PWM 0% oder 100% oder
der Befehl "keine Änderung" eintrifft.
Diese Befehlsart entspricht den Tasten des Handsenders.

Not Aus
Abweichend vom Standardprotokoll gibt es den Befehl "Not Halt",
der für alle Fahrzeuge gilt:
11110000
Dieser Befehl muss 2 mal unmittelbar hintereinader gesendet werden.
Auf diesen Befehl reagieren alle Empfänger sofort. Adressunabhängig.
  */

#include "rocdigs/impl/infracar_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOInfracarData data = Data(inst);
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

/** ----- OInfracar ----- */


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  return 0;
}


/**  */
static byte* _cmdRaw( obj inst ,const byte* cmd ) {
  return 0;
}


/**  */
static void _halt( obj inst ,Boolean poweroff ) {
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOInfracarData data = Data(inst);
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


static int vmajor = 2;
static int vminor = 0;
static int patch  = 99;
static int _version( obj inst ) {
  iOInfracarData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}




/**  */
static struct OInfracar* _inst( const iONode ini ,const iOTrace trc ) {
  iOInfracar __Infracar = allocMem( sizeof( struct OInfracar ) );
  iOInfracarData data = allocMem( sizeof( struct OInfracarData ) );
  MemOp.basecpy( __Infracar, &InfracarOp, 0, sizeof( struct OInfracar ), data );

  TraceOp.set( trc );
  SystemOp.inst();
  /* Initialize data->xxx members... */

  data->ini  = ini;
  data->iid  = StrOp.dup( wDigInt.getiid( ini ) );
  data->run  = True;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Infracar %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "http://www.merg.org.uk" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid           = %s", data->iid );

  instCnt++;
  return __Infracar;
}

iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/infracar.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
