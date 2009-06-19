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

#include "rocrail/impl/powerman_impl.h"

#include "rocrail/public/app.h"
#include "rocrail/public/model.h"

#include "rocrail/wrapper/public/BoosterList.h"
#include "rocrail/wrapper/public/Booster.h"

#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/PwrCmd.h"
#include "rocrail/wrapper/public/Feedback.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/thread.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"


static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOPowerManData data = Data(inst);
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


static void __processEvent( obj inst ,Boolean pulse ,const char* id ,int ident, int val ) {
  iOPowerManData data = Data(inst);
  iONode scbooster = (iONode)MapOp.get( data->scmap, id );
  iONode pwbooster = (iONode)MapOp.get( data->pwmap, id );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "booster sensors [%s] event", id );

  if( scbooster != NULL ) {
    TraceOp.trc( name, pulse?TRCLEVEL_WARNING:TRCLEVEL_INFO, __LINE__, 9999,
        "shortcut %s[%s] event for booster [%s]", pulse?"":"cleared ", id, wBooster.getid(scbooster) );
  }
  if( pwbooster != NULL ) {
    TraceOp.trc( name, pulse?TRCLEVEL_INFO:TRCLEVEL_WARNING, __LINE__, 9999,
        "power %s [%s] event for booster [%s]", pulse?"on":"off", id, wBooster.getid(pwbooster) );
  }
}

static void* __event( void* inst, const void* evt ) {
  iOPowerManData data = Data(inst);
  iONode node = (iONode)evt;
  if( node != NULL && StrOp.equals( wFeedback.name(), NodeOp.getName(node) ) ) {
    __processEvent( inst ,wFeedback.isstate(node), wFeedback.getid(node), wFeedback.getidentifier(node), wFeedback.getval(node) );
  }
  return NULL;
}

/** ----- OPowerMan ----- */


/**  */
static char* _getForm( void* object ) {
  return 0;
}


/**  */
static char* _postForm( void* object ,const char* data ) {
  return 0;
}


/**  */
static const char* _tableHdr( void ) {
  return 0;
}


/**  */
static char* _toHtml( void* object ) {
  return 0;
}

static void __initSensors( iOPowerMan inst ) {
  iOPowerManData data = Data(inst);
  iOModel model = AppOp.getModel();

  iONode booster = wBoosterList.getbooster(data->props);
  while( booster != NULL ) {
    iOFBack scfb = ModelOp.getFBack( model, wBooster.getscfb( booster ) );
    iOFBack pwfb = ModelOp.getFBack( model, wBooster.getpowerfb( booster ) );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "Init sensors for booster [%s]...", wBooster.getid(booster) );

    if( scfb != NULL && pwfb != NULL ) {
      FBackOp.addListener( scfb, (obj)inst );
      FBackOp.addListener( pwfb, (obj)inst );
      MapOp.put( data->scmap, wBooster.getscfb( booster ), (obj)booster );
      MapOp.put( data->pwmap, wBooster.getpowerfb( booster ), (obj)booster );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
          "Sensors for booster %s could not be initialized.", wBooster.getid(booster) );
    }

    booster = wBoosterList.nextbooster(data->props, booster);
  }

}


/**  */
static struct OPowerMan* _inst( iONode ini ) {
  iOPowerMan __PowerMan = allocMem( sizeof( struct OPowerMan ) );
  iOPowerManData data = allocMem( sizeof( struct OPowerManData ) );
  MemOp.basecpy( __PowerMan, &PowerManOp, 0, sizeof( struct OPowerMan ), data );

  /* Initialize data->xxx members... */
  data->props = ini;
  data->scmap = MapOp.inst();
  data->pwmap = MapOp.inst();

  __initSensors(__PowerMan);


  TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Power Manager instantiated.");

  instCnt++;
  return __PowerMan;
}


/**  */
static void _modify( struct OPowerMan* inst ,iONode mod ) {
  return;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/powerman.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
