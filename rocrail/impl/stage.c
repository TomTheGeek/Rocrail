/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2010 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocrail/impl/stage_impl.h"
#include "rocrail/public/app.h"
#include "rocrail/public/fback.h"
#include "rocrail/public/route.h"
#include "rocrail/public/control.h"
#include "rocrail/public/modplan.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"

#include "rocrail/wrapper/public/Stage.h"
#include "rocrail/wrapper/public/StageSection.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/Feedback.h"



static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOStageData data = Data(inst);
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

/** ----- OStage ----- */


/**  */
static Boolean _cmd( iIBlockBase inst ,iONode cmd ) {
  return 0;
}


/**  */
static void _enterBlock( iIBlockBase inst ,const char* locid ) {
  return;
}


/**  */
static void _event( iIBlockBase inst ,Boolean puls ,const char* id ,long ident ,int val ,iONode evtDescr ) {
  iOStageData data = Data(inst);
  iONode section = (iONode)MapOp.get( data->fbMap, id );

  if( section != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensors [%s] %s event for stage [%s] section [%s][%d] of [%d]",
        id, puls?"on":"off", data->id, wStageSection.getid(section), wStageSection.getnr(section), data->sectionCount );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unknown sensors [%s] for stage [%s]...", id, data->id );
  }

  return;
}

static void _fbEvent( obj inst, Boolean puls, const char* id, int ident, int val ) {
  _event( (iIBlockBase)inst, puls, id, ident, val, NULL );
}


/**  */
static const char* _getFromBlockId( iIBlockBase inst ) {
  return 0;
}


/**  */
static const char* _getInLoc( iIBlockBase inst ) {
  return 0;
}


/**  */
static const char* _getLoc( iIBlockBase inst ) {
  return 0;
}


/**  */
static iIBlockBase _getManager( iIBlockBase inst ) {
  return 0;
}


/**  */
static int _getOccTime( iIBlockBase inst ) {
  return 0;
}


/**  */
static const char* _getState( iIBlockBase inst ) {
  return 0;
}


/**  */
static int _getTDaddress( iIBlockBase inst ) {
  return 0;
}


/**  */
static const char* _getTDiid( iIBlockBase inst ) {
  return 0;
}


/**  */
static int _getTDport( iIBlockBase inst ) {
  return 0;
}


/**  */
static const char* _getVelocity( iIBlockBase inst ,int* percent ,Boolean onexit ,Boolean reverse ) {
  return 0;
}


/**  */
static int _getVisitCnt( iIBlockBase inst ,const char* locid ) {
  return 0;
}


/**  */
static int _getWait( iIBlockBase inst ,iOLoc loc ,Boolean reverse ) {
  return 0;
}


/**  */
static Boolean _green( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  return 0;
}


/**  */
static Boolean _hasEnter2Route( iIBlockBase inst ,const char* fromBlockId ) {
  return 0;
}


/**  */
static Boolean _hasExtStop( iIBlockBase inst ) {
  return 0;
}


/**  */
static obj _hasManualSignal( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  return 0;
}


/**  */
static Boolean _hasPre2In( iIBlockBase inst ,const char* fromBlockId ) {
  return 0;
}


/**  */
static void _inBlock( iIBlockBase inst ,const char* locid ) {
  return;
}


/**  */
static void _init( iIBlockBase inst ) {
  return;
}


/**  */
static Boolean _isFree( iIBlockBase inst ,const char* locid ) {
  return 0;
}


/**  */
static Boolean _isLinked( iIBlockBase inst ) {
  return 0;
}


/**  */
static Boolean _isReady( iIBlockBase inst ) {
  return 0;
}


/**  */
static int _isSuited( iIBlockBase inst ,iOLoc loc ) {
  return 0;
}


/**  */
static Boolean _isTerminalStation( iIBlockBase inst ) {
  return 0;
}


/**  */
static Boolean _link( iIBlockBase inst ,iIBlockBase linkto ) {
  return 0;
}


/**  */
static Boolean _lock( iIBlockBase inst ,const char* locid ,const char* blockid ,const char* routeid ,Boolean crossing ,Boolean reset ,Boolean reverse ,int indelay ) {
  return 0;
}


/**  */
static Boolean _lockForGroup( iIBlockBase inst ,const char* locid ) {
  return 0;
}


/**  */
static Boolean _red( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  return 0;
}


/**  */
static void _reset( iIBlockBase inst ) {
  return;
}


/**  */
static void _resetTrigs( iIBlockBase inst ) {
  return;
}


/**  */
static void _setAnalog( iIBlockBase inst ,Boolean analog ) {
  return;
}


/**  */
static void _setDefaultAspect( iIBlockBase inst ,Boolean signalpair ) {
  return;
}


/**  */
static void _setGroup( iIBlockBase inst ,const char* group ) {
  return;
}


/**  */
static Boolean _setLocSchedule( iIBlockBase inst ,const char* scid ) {
  return 0;
}


/**  */
static void _setManager( iIBlockBase inst ,iIBlockBase manager ) {
  return;
}


/**  */
static Boolean _unLink( iIBlockBase inst ) {
  return 0;
}


/**  */
static Boolean _unLock( iIBlockBase inst ,const char* locid ) {
  return 0;
}


/**  */
static Boolean _unLockForGroup( iIBlockBase inst ,const char* locid ) {
  return 0;
}


/**  */
static Boolean _wait( iIBlockBase inst ,iOLoc loc ,Boolean reverse ) {
  return 0;
}


/**  */
static Boolean _white( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  return 0;
}


/**  */
static Boolean _yellow( iIBlockBase inst ,Boolean distant ,Boolean reverse ) {
  return 0;
}


static void _modify( iOStage inst, iONode props ) {
  iOStageData data = Data(inst);
}


/**
 * map all fb's and set the listener to the common _event
 */
static void __initSensors( iOStage inst ) {
  iOStageData data = Data(inst);
  iOModel model = AppOp.getModel();
  int sectionNr = 0;
  iONode section = wStage.getsection( data->props );

  MapOp.clear( data->fbMap );
  ListOp.clear( data->sectionList );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init sensors for stage %s...", data->id );

  while( section != NULL ) {
    const char* fbid = wStageSection.getfbid( section );
    wStageSection.setnr(section, sectionNr);

    if( StrOp.len( fbid ) > 0 ) {
    iOFBack fb = ModelOp.getFBack( model, fbid );

      if( fb != NULL ) {
        FBackOp.setListener( fb, (obj)inst, &_fbEvent );
        MapOp.put( data->fbMap, fbid, (obj)section);
      }
    }
    ListOp.add( data->sectionList, (obj)section );
    sectionNr++;
    section = wStage.nextsection( data->props, section );
  };
  data->sectionCount = sectionNr;

}


static void _setCarCount( iIBlockBase inst, int count ) {
}


/**  */
static struct OStage* _inst( iONode props ) {
  iOStage __Stage = allocMem( sizeof( struct OStage ) );
  iOStageData data = allocMem( sizeof( struct OStageData ) );
  MemOp.basecpy( __Stage, &StageOp, 0, sizeof( struct OStage ), data );

  /* Initialize data->xxx members... */
  data->props       = props;
  data->id          = wStage.getid( props );
  data->fbMap       = MapOp.inst();
  data->sectionList = ListOp.inst();

  __initSensors(__Stage);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "stage %s created", data->id );

  instCnt++;
  return __Stage;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/stage.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
