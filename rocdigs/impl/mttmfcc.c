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

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Feedback.h"

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


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  /* Cleanup cmd node to avoid memory leak. */
  cmd->base.del(cmd);
  return NULL;
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

  instCnt++;
  return __MttmFcc;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/MttmFcc.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
