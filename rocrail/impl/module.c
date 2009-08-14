/** ------------------------------------------------------------
  * Module: RocRail
  * Object: Module
  * ------------------------------------------------------------
  */

#include "rocrail/impl/module_impl.h"
#include "rocrail/public/clntcon.h"
#include "rocrail/public/app.h"

#include "rocs/public/mem.h"

#include "rocrail/wrapper/public/Module.h"
#include "rocrail/wrapper/public/ModelCmd.h"

#include "rocrail/wrapper/public/BlockList.h"
#include "rocrail/wrapper/public/TrackList.h"
#include "rocrail/wrapper/public/LocList.h"
#include "rocrail/wrapper/public/TurntableList.h"
#include "rocrail/wrapper/public/SelTabList.h"
#include "rocrail/wrapper/public/SwitchList.h"
#include "rocrail/wrapper/public/OutputList.h"
#include "rocrail/wrapper/public/SignalList.h"
#include "rocrail/wrapper/public/FeedbackList.h"
#include "rocrail/wrapper/public/TextList.h"
#include "rocrail/wrapper/public/ZLevel.h"

#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Track.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Text.h"
#include "rocrail/wrapper/public/Item.h"


static int instCnt = 0;
static void __getItemSize( iONode item, int* iCX, int* iCY, Boolean defSize );


/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOModuleData data = Data(inst);
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

/** ----- OModule ----- */
/*
 * find the extremes in a list of objects
 */
static Boolean __getXYextremes( const char* listname, iONode module, int* Xmax, int* Ymax ) {
  iONode list = NodeOp.findNode( module, listname );
  *Xmax = -1;
  *Ymax = -1;
  if( list != NULL ) {
    int i = 0;
    int cnt = NodeOp.getChildCnt( list );
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( list, i );
      int x = wItem.getx(child);
      int y = wItem.gety(child);
      int iCX = 0;
      int iCY = 0;
      __getItemSize( child, &iCX, &iCY, False );
      iCX--;
      iCY--;

      x += iCX;
      y += iCY;

      if( *Xmax == -1 || x > *Xmax )
        *Xmax = x;
      if( *Ymax == -1 || y > *Ymax )
        *Ymax = y;
    }
  }


  if( *Xmax != -1 && *Ymax != -1 )
    return True;
  else
    return False;
}


/* determine the rotation area by looking up the upper most left symbol and the lowest most right symbol */
/* TODO: regard default symbol sizes. */
static void _getRotationArea( iONode moduleRoot, int* Xmax, int* Ymax ) {
  int xmax = 0;
  int ymax = 0;

  *Xmax = -1;
  *Ymax = -1;

  if( __getXYextremes(wTrackList.name()    , moduleRoot, &xmax, &ymax) ) {
    if( *Xmax == -1 || xmax > *Xmax ) *Xmax = xmax;
    if( *Ymax == -1 || ymax > *Ymax ) *Ymax = ymax;
  }

  if( __getXYextremes(wBlockList.name()    , moduleRoot, &xmax, &ymax) ) {
    if( *Xmax == -1 || xmax > *Xmax ) *Xmax = xmax;
    if( *Ymax == -1 || ymax > *Ymax ) *Ymax = ymax;
  }

  if( __getXYextremes(wSwitchList.name()   , moduleRoot, &xmax, &ymax) ) {
    if( *Xmax == -1 || xmax > *Xmax ) *Xmax = xmax;
    if( *Ymax == -1 || ymax > *Ymax ) *Ymax = ymax;
  }

  if( __getXYextremes(wSignalList.name()   , moduleRoot, &xmax, &ymax) ) {
    if( *Xmax == -1 || xmax > *Xmax ) *Xmax = xmax;
    if( *Ymax == -1 || ymax > *Ymax ) *Ymax = ymax;
  }

  if( __getXYextremes(wOutputList.name()   , moduleRoot, &xmax, &ymax) ) {
    if( *Xmax == -1 || xmax > *Xmax ) *Xmax = xmax;
    if( *Ymax == -1 || ymax > *Ymax ) *Ymax = ymax;
  }

  if( __getXYextremes(wFeedbackList.name() , moduleRoot, &xmax, &ymax) ) {
    if( *Xmax == -1 || xmax > *Xmax ) *Xmax = xmax;
    if( *Ymax == -1 || ymax > *Ymax ) *Ymax = ymax;
  }

  if( __getXYextremes(wTextList.name()     , moduleRoot, &xmax, &ymax) ) {
    if( *Xmax == -1 || xmax > *Xmax ) *Xmax = xmax;
    if( *Ymax == -1 || ymax > *Ymax ) *Ymax = ymax;
  }

  if( __getXYextremes(wTurntableList.name(), moduleRoot, &xmax, &ymax) ) {
    if( *Xmax == -1 || xmax > *Xmax ) *Xmax = xmax;
    if( *Ymax == -1 || ymax > *Ymax ) *Ymax = ymax;
  }

  if( __getXYextremes(wSelTabList.name(), moduleRoot, &xmax, &ymax) ) {
    if( *Xmax == -1 || xmax > *Xmax ) *Xmax = xmax;
    if( *Ymax == -1 || ymax > *Ymax ) *Ymax = ymax;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "calculated module size %d, %d", *Xmax, *Ymax );

}





static void __getItemSize( iONode item, int* iCX, int* iCY, Boolean defSize ) {
  int cx = wItem.getcx( item );
  int cy = wItem.getcy( item );
  const char* ori = wItem.getori( item );
  Boolean defOri = True;

  if( StrOp.equals( wItem.north, ori ) || StrOp.equals( wItem.south, ori ) ) {
    defOri = defSize ? True:False;
  }

  if( cx == 0 || cy == 0 ) {
    /* default symbol sizes */
    *iCX = 1;
    *iCY = 1;
    if( StrOp.equals( wSwitch.name(), NodeOp.getName(item) ) ) {
      if( StrOp.equals( wSwitch.dcrossing, wSwitch.gettype(item) ) ) {
        *iCX = defOri ? 2:1;
        *iCY = defOri ? 1:2;
      }
      else if( StrOp.equals( wSwitch.crossing, wSwitch.gettype(item) ) ) {
        *iCX = defOri ? 2:1;
        *iCY = defOri ? 1:2;
      }
      else if( StrOp.equals( wSwitch.ccrossing, wSwitch.gettype(item) ) ) {
        *iCX = defOri ? 2:1;
        *iCY = defOri ? 1:2;
      }
      else if( StrOp.equals( wSwitch.accessory, wSwitch.gettype(item) ) ) {
        if( wSwitch.getaccnr(item) == 1 ) {
          /* double track road crossing */
          *iCX = defOri ? 1:2;
          *iCY = defOri ? 2:1;
        }
      }
    }
    else if( StrOp.equals( wBlock.name(), NodeOp.getName(item) ) ) {
      Boolean m_bSmall = wBlock.issmallsymbol(item);
      int blocklen = m_bSmall ? 2:4;
      *iCX = defOri ? blocklen:1;
      *iCY = defOri ? 1:blocklen;
    }
    else if( StrOp.equals( wSelTab.name(), NodeOp.getName(item) ) ) {
      *iCX = defOri ? wSelTab.getnrtracks(item):1;
      *iCY = defOri ? 1:wSelTab.getnrtracks(item);
    }
    else if( StrOp.equals( wText.name(), NodeOp.getName(item) ) ) {
      *iCX = defOri ? wText.getcx(item):1;
      *iCY = defOri ? 1:wText.getcx(item);
    }
    else if( StrOp.equals( wTurntable.name(), NodeOp.getName(item) ) ) {
      *iCX = 5;
      *iCY = 5;
    }
  }
  else {
    *iCX = cx;
    *iCY = cy;
  }
}


/* rotate 90 degrees */
static void _rotate90( iONode item, int cx, int cy ) {
  int iX = wItem.getx( item );
  int iY = wItem.gety( item );
  int iCX = 0;
  int iCY = 0;
  int iXr = 0;
  int iYr = 0;

  const char* ori = wItem.getori( item );

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Rotate 90" );

  __getItemSize( item, &iCX, &iCY, True );
  iCX--;
  iCY--;

  if( StrOp.equals( wItem.east, ori ) )
    wItem.setori( item, wItem.south );
  else if( StrOp.equals( wItem.north, ori ) )
    wItem.setori( item, wItem.east );
  else if( StrOp.equals( wItem.south, ori ) )
    wItem.setori( item, wItem.west );
  else {
    iX += iCX;
    wItem.setori( item, wItem.north );
  }

  iXr = iY;
  iYr = (cx-1) - iX;

  wItem.setx( item, iXr );
  wItem.sety( item, iYr );
}


/* rotate 270 degrees */
static void _rotate270( iONode item, int cx, int cy ) {
  int iX = wItem.getx( item );
  int iY = wItem.gety( item );
  int iCX = 0;
  int iCY = 0;
  int iYr = 0;
  int iXr = 0;

  const char* ori = wItem.getori( item );

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Rotate 270" );

  __getItemSize( item, &iCX, &iCY, True );
  iCX--;
  iCY--;

  if( StrOp.equals( wItem.east, ori ) ) {
    iY += iCY;
    wItem.setori( item, wItem.north );
  }
  else if( StrOp.equals( wItem.north, ori ) ) {
    iY += iCX;
    wItem.setori( item, wItem.west );
  }
  else if( StrOp.equals( wItem.south, ori ) ) {
    iY += iCX;
    wItem.setori( item, wItem.east );
  }
  else {
    /* west is default */
    iY += iCY;
    wItem.setori( item, wItem.south );
  }

  iYr = iX;
  iXr = (cy-1) - iY;

  wItem.setx( item, iXr );
  wItem.sety( item, iYr );

}


/* rotate 180 degrees */
static void _rotate180( iONode item, int cx, int cy ) {
  int iX = wItem.getx( item );
  int iY = wItem.gety( item );
  int iCX = 0;
  int iCY = 0;

  const char* ori = wItem.getori( item );
  int rX = cx - 1;
  int rY = cy - 1;
  int nX = 0;
  int nY = 0;

  /* item size must be regarded */
  __getItemSize( item, &iCX, &iCY, True );
  iCX--;
  iCY--;

  if( StrOp.equals( wItem.east, ori ) )
    wItem.setori( item, wItem.west );
  else if( StrOp.equals( wItem.north, ori ) ) {
    int tmp = iCX;
    iCX = iCY;
    iCY = tmp;
    wItem.setori( item, wItem.south );
  }
  else if( StrOp.equals( wItem.south, ori ) ) {
    int tmp = iCX;
    iCX = iCY;
    iCY = tmp;
    wItem.setori( item, wItem.north );
  }
  else
    wItem.setori( item, wItem.east );

  iX = iX + iCX;
  iY = iY + iCY;

  nX = rX - iX;
  nY = rY - iY;

  wItem.setx( item, nX );
  wItem.sety( item, nY );
}


static void __rotateList( iONode model, iONode module, int level, const char* dbname, int rotation ) {

  iONode list = NodeOp.findNode( model, dbname );
  if( list != NULL ) {
    int childs = NodeOp.getChildCnt(list);
    int i = 0;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "rotating %d childs of %s for level %d and rotation %d", childs, dbname, level, rotation );

    for( i = 0; i < childs; i++ ) {
      iONode child = NodeOp.getChild( list, i );
      if( wItem.getz(child) == level ) {
        int cx = wModule.getcx(module);
        int cy = wModule.getcy(module);

        wItem.setx  ( child, wItem.getprev_x(child) );
        wItem.sety  ( child, wItem.getprev_y(child) );
        wItem.setori( child, wItem.getprev_ori(child) );

        if( rotation == 90 )
          ModuleOp.rotate90( child, cx, cy );
        else if( rotation == 180 )
          ModuleOp.rotate180( child, cx, cy );
        else if( rotation == 270 )
          ModuleOp.rotate270( child, cx, cy );

        ThreadOp.sleep(10);

        /* Notify the clients */
        {
          iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
          wModelCmd.setcmd( cmd, wModelCmd.modify );
          NodeOp.addChild( cmd, (iONode)NodeOp.base.clone(child));
          ClntConOp.broadcastEvent( AppOp.getClntCon(), cmd );
        }
      }
    }
  }

}

static void _rotateModule( iONode model, iONode module, int level, int rotation ) {

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "rotate module [%s] to %d, level=%d (%d,%d)", wModule.getid(module), rotation,
      level, wModule.getcx(module), wModule.getcy(module) );

  /* iterate all objects, rotate and inform clients */

  __rotateList( model, module, level, wTrackList.name(), rotation );
  __rotateList( model, module, level, wSwitchList.name(), rotation );
  __rotateList( model, module, level, wFeedbackList.name(), rotation );
  __rotateList( model, module, level, wSignalList.name(), rotation );
  __rotateList( model, module, level, wOutputList.name(), rotation );
  __rotateList( model, module, level, wBlockList.name(), rotation );
  __rotateList( model, module, level, wTextList.name(), rotation );
  __rotateList( model, module, level, wTurntableList.name(), rotation );
  __rotateList( model, module, level, wSelTabList.name(), rotation );

  wModule.setrotation( module, rotation );

  /* Notify the clients */
  {
    iONode cmd = (iONode)NodeOp.base.clone(module);
    iONode zlevel = NodeOp.inst( wZLevel.name(), NULL, ELEMENT_NODE );
    NodeOp.addChild(cmd, zlevel);
    wZLevel.settitle(zlevel, wModule.gettitle( module ) );
    wZLevel.setmodid(zlevel, wModule.getid( module ) );
    wZLevel.setz(zlevel, level );
    wZLevel.setmodviewx( zlevel, wModule.getx(module));
    wZLevel.setmodviewy( zlevel, wModule.gety(module));
    if( rotation == 0 || rotation == 180 ) {
      wZLevel.setmodviewcx( zlevel, wModule.getcx(module));
      wZLevel.setmodviewcy( zlevel, wModule.getcy(module));
    }
    else {
      wZLevel.setmodviewcx( zlevel, wModule.getcy(module));
      wZLevel.setmodviewcy( zlevel, wModule.getcx(module));
    }
    ClntConOp.broadcastEvent( AppOp.getClntCon(), cmd );
  }
}










/**  */
static struct OModule* _inst( iONode ini ) {
  iOModule __Module = allocMem( sizeof( struct OModule ) );
  iOModuleData data = allocMem( sizeof( struct OModuleData ) );
  MemOp.basecpy( __Module, &ModuleOp, 0, sizeof( struct OModule ), data );

  /* Initialize data->xxx members... */

  instCnt++;
  return __Module;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/module.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
