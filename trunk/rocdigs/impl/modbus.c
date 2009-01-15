/*
 Rocrail - Model Railroad Control System

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

/** ------------------------------------------------------------
  * Module: RocDigs
  * Object: Modbus
  * ------------------------------------------------------------
  */

#include "rocdigs/impl/modbus_impl.h"
#include "rocdigs/impl/modbus/modbus-const.h"

#include "rocs/public/mem.h"

static int instCnt = 0;


/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOModbusData data = Data(inst);
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

/** ----- OModbus ----- */
static unsigned short __ModbusCRC (int len, byte* buf)
{
  unsigned short  reg;
  int    i;
  int    times;

  reg = 0xFFFF;

  for (i = 0; i < len; i++)
  {
    reg ^= buf[i];

    for (times = 0; times <= 7; times++)
    {
      if (reg & 1)
      {
        reg >>= 1;
        reg ^= 0xA001;
      }
      else
        reg >>= 1;
    }
  }
  return reg;
}






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
static int _version( obj inst ) {
  return 0;
}


/**  */
static struct OModbus* _inst( const iONode ini ,const iOTrace trc ) {
  iOModbus __Modbus = allocMem( sizeof( struct OModbus ) );
  iOModbusData data = allocMem( sizeof( struct OModbusData ) );
  MemOp.basecpy( __Modbus, &ModbusOp, 0, sizeof( struct OModbus ), data );

  TraceOp.set( trc );
  /* Initialize data->xxx members... */
  data->ini      = ini;

  instCnt++;
  return __Modbus;
}

/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/modbus.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
