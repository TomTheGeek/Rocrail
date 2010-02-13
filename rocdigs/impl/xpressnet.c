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


#include "rocdigs/impl/xpressnet_impl.h"
#include "rocdigs/impl/xpressnet/li101.h"
#include "rocdigs/impl/xpressnet/liusb.h"
#include "rocdigs/impl/xpressnet/elite.h"
#include "rocdigs/impl/xpressnet/roco.h"
#include "rocdigs/impl/xpressnet/opendcc.h"
#include "rocdigs/impl/xpressnet/atlas.h"
#include "rocdigs/impl/xpressnet/common.h"

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
    iOXpressNetData data = Data(inst);
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

/** ----- OXpressNet ----- */


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
  iOXpressNetData data = Data(inst);
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
static int patch  = 999;
static int _version( obj inst ) {
  iOXpressNetData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


/**  */
static struct OXpressNet* _inst( const iONode ini ,const iOTrace trc ) {
  iOXpressNet __XpressNet = allocMem( sizeof( struct OXpressNet ) );
  iOXpressNetData data = allocMem( sizeof( struct OXpressNetData ) );
  MemOp.basecpy( __XpressNet, &XpressNetOp, 0, sizeof( struct OXpressNet ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );
  SystemOp.inst();

  /* choose interface: */
  if( StrOp.equals( wDigInt.sublib_usb, wDigInt.getsublib( ini ) ) ) {
    /* LI-USB */
    data->subInit  = liusbInit;
    data->subRead  = liusbRead;
    data->subWrite = liusbWrite;
  }
  else if( StrOp.equals( wDigInt.sublib_lenz_elite, wDigInt.getsublib( ini ) ) ) {
    /* Hornby Elite */
    data->subInit  = eliteInit;
    data->subRead  = eliteRead;
    data->subWrite = eliteWrite;
  }
  else if( StrOp.equals( wDigInt.sublib_lenz_roco, wDigInt.getsublib( ini ) ) ) {
    /* Roco */
    data->subInit  = rocoInit;
    data->subRead  = rocoRead;
    data->subWrite = rocoWrite;
  }
  else if( StrOp.equals( wDigInt.sublib_lenz_opendcc, wDigInt.getsublib( ini ) ) ) {
    /* OpenDCC */
    data->subInit  = opendccInit;
    data->subRead  = opendccRead;
    data->subWrite = opendccWrite;
  }
  else if( StrOp.equals( wDigInt.sublib_lenz_atlas, wDigInt.getsublib( ini ) ) ) {
    /* Atlas */
    data->subInit  = atlasInit;
    data->subRead  = atlasRead;
    data->subWrite = atlasWrite;
  }
  else {
    /* default LI101 */
    data->subInit  = li101Init;
    data->subRead  = li101Read;
    data->subWrite = li101Write;
  }

  instCnt++;
  return __XpressNet;
}


iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/xpressnet.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
