/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rocrail/impl/model_impl.h"
#include "rocrail/public/loc.h"
#include "rocrail/public/car.h"
#include "rocrail/public/operator.h"
#include "rocrail/public/block.h"
#include "rocrail/public/blockgroup.h"
#include "rocrail/public/stage.h"
#include "rocrail/public/fback.h"
#include "rocrail/public/switch.h"
#include "rocrail/public/output.h"
#include "rocrail/public/signal.h"
#include "rocrail/public/route.h"
#include "rocrail/public/track.h"
#include "rocrail/public/tt.h"
#include "rocrail/public/text.h"
#include "rocrail/public/clntcon.h"
#include "rocrail/public/app.h"
#include "rocrail/public/r2rnet.h"
#include "rocrail/public/location.h"
#include "rocrail/public/analyse.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/file.h"
#include "rocs/public/thread.h"
#include "rocs/public/lib.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/Global.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Module.h"
#include "rocrail/wrapper/public/ModPlan.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/BlockList.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/RouteList.h"
#include "rocrail/wrapper/public/Track.h"
#include "rocrail/wrapper/public/TrackList.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/LocList.h"
#include "rocrail/wrapper/public/Car.h"
#include "rocrail/wrapper/public/CarList.h"
#include "rocrail/wrapper/public/Waybill.h"
#include "rocrail/wrapper/public/WaybillList.h"
#include "rocrail/wrapper/public/Operator.h"
#include "rocrail/wrapper/public/OperatorList.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Turntable.h"
#include "rocrail/wrapper/public/TurntableList.h"
#include "rocrail/wrapper/public/SelTab.h"
#include "rocrail/wrapper/public/SelTabList.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/SwitchList.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/OutputList.h"
#include "rocrail/wrapper/public/Link.h"
#include "rocrail/wrapper/public/LinkList.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/SignalList.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/FeedbackList.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/AutoCmd.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/ZLevel.h"
#include "rocrail/wrapper/public/FbInfo.h"
#include "rocrail/wrapper/public/FbMods.h"
#include "rocrail/wrapper/public/Text.h"
#include "rocrail/wrapper/public/TextList.h"
#include "rocrail/wrapper/public/Location.h"
#include "rocrail/wrapper/public/LocationList.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/ScheduleEntry.h"
#include "rocrail/wrapper/public/ScheduleList.h"
#include "rocrail/wrapper/public/Ctrl.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/ModOcc.h"
#include "rocrail/wrapper/public/Occupancy.h"
#include "rocrail/wrapper/public/MVTrack.h"
#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/ActionList.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/ModuleConnection.h"
#include "rocrail/wrapper/public/Booster.h"
#include "rocrail/wrapper/public/BoosterList.h"
#include "rocrail/wrapper/public/Stage.h"
#include "rocrail/wrapper/public/StageList.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Exception.h"
#include "rocrail/wrapper/public/Accessory.h"
#include "rocrail/wrapper/public/Tour.h"
#include "rocrail/wrapper/public/TourList.h"
#include "rocrail/wrapper/public/SystemActions.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"

static int instCnt = 0;


static Boolean __removeLoco(iOModelData o, iONode item );
static void __initMasterLocMap(iOModel inst);


/*
 ***** OBase functions.
 */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static const char* __name(void) {
  return name;
}
static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}
static void __deserialize(void* inst, unsigned char* a) {
}
static char* __toString(void* inst) {
  return "";
}
static void __del(void* inst) {
  iOModelData data = Data(inst);
  data->planDoc->base.del(data->planDoc);
  data->model->base.del(data->model);
  freeMem( data );
  freeMem( inst );
  instCnt--;
}
static void* __properties(void* inst) {
  iOModelData data = Data(inst);
  return data->model;
}
static struct OBase* __clone( void* inst ) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}
static int __count(void) {
  return instCnt;
}


/* comparator for sorting by id: */
static int __sortStr(obj* _a, obj* _b)
{
    const char* a = (const char*)*_a;
    const char* b = (const char*)*_b;
    return strcmp( a, b );
}


/* comparator for sorting by id: */
static int __sortID(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    const char* idA = wItem.getid( a );
    const char* idB = wItem.getid( b );
    return strcmp( idA, idB );
}


/*ListOp.sort( list, &__sortLocation );*/
/* comparator for sorting by location: */
static int __sortLocation(obj* _a, obj* _b)
{
    iONode a = (iONode)*_a;
    iONode b = (iONode)*_b;
    int xA = wItem.getx( a );
    int yA = wItem.gety( a );
    int xB = wItem.getx( b );
    int yB = wItem.gety( b );

    if( yA > yB )
      return 1;
    else if( yA < yB )
      return -1;
    else if( xA > xB && yA == yB )
      return 1;
    else if( xA < xB && yA == yB )
      return -1;
    /* xA == xB && yA == yB */
    return 0;
}



/*
 ***** _Public functions.
 */
static void _addListener( iOModel inst, model_listener cbFun, obj cbCargo ) {
  iOModelData data = Data(inst);
  data->callback = cbFun;
  data->cbCargo  = cbCargo;
}

static const iONode _getModel( iOModel inst ) {
  iOModelData data = Data(inst);
  return data->model;
}

static const iONode _getModPlan( iOModel inst ) {
  iOModelData data = Data(inst);
  if(data->moduleplan != NULL )
    return ModPlanOp.getModPlan( data->moduleplan );

  return NULL;
}

static const iONode _getModule( iOModel inst, const char* modid ) {
  iOModelData data = Data(inst);
  return ModPlanOp.getModule(data->moduleplan, modid );
}

static void _setModel( iOModel inst, iONode model ) {
  iOModelData data = Data(inst);
  data->model = model;
}

static void _setModPlan( iOModel inst, iONode modplan ) {
  iOModelData data = Data(inst);
  if( data->moduleplan == NULL ) {
    return;
  }
  ModPlanOp.setModPlan(data->moduleplan, modplan);
}

static void _setModule( iOModel inst, iONode module ) {
  iOModelData data = Data(inst);
  iONode item = NULL;

  if( data->moduleplan == NULL ) {
    return;
  }

  item = ModPlanOp.setModule(data->moduleplan, module);
  if( item != NULL )
    ModelOp.modifyItem( inst, item );

}

static Boolean _modify( iOModel inst, iONode model ) {
  iOModelData data = Data(inst);
  return True;
}



static void __backupSave( const char* fileName, const char* xml ) {
  char* backupfile;
  iOFile planFile;

  if( wRocRail.isbackup(AppOp.getIni()) ) {
    if( !FileOp.exist(wRocRail.getbackuppath(AppOp.getIni())) ) {
      FileOp.mkdir(wRocRail.getbackuppath(AppOp.getIni()));
    }
    if( FileOp.exist(wRocRail.getbackuppath(AppOp.getIni())) ) {
      char* stamp = StrOp.createStampNoDots();
      backupfile = StrOp.fmt( "%s%c%s-%s",
          wRocRail.getbackuppath(AppOp.getIni()), SystemOp.getFileSeparator(), stamp, FileOp.ripPath( fileName ) );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "backup %s to %s", fileName, backupfile);
      FileOp.cp(fileName,backupfile);
      StrOp.free(stamp);
      StrOp.free(backupfile);
    }
  }

  backupfile = StrOp.fmt( "%s.bak",fileName );
  /* Make Backup copy! Somtimes rocrail loses the plan and writes an empty plan! */
  if( FileOp.exist(backupfile) )
    FileOp.remove(backupfile);
  if( FileOp.exist(fileName) )
    FileOp.rename(fileName,backupfile);
  StrOp.free(backupfile);


  planFile = FileOp.inst( fileName, False );
  if( planFile != NULL ) {
    FileOp.write( planFile, xml, StrOp.len( xml ) );
    FileOp.close( planFile );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%ld bytes saved in %s.",
        FileOp.getWritten( planFile ), fileName );
    /* Cleanup. */
    planFile->base.del(planFile);
  }
}

static Boolean _createEmptyPlan( iOModelData o ) {
  if( o->planFile != NULL ) {
    FileOp.close( o->planFile );
    o->planFile->base.del(o->planFile);
    o->planFile = NULL;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Creating Plan file: %s", o->fileName );
  o->planFile = FileOp.inst( o->fileName, OPEN_WRITE );
  if( o->planFile != NULL ) {
    Boolean createmodplan = AppOp.isCreateModplan();
    char* planXml = NULL;

    if( createmodplan ) {
      iONode root = NodeOp.inst( wModPlan.name(), NULL, ELEMENT_NODE );
      o->moduleplan = ModPlanOp.inst( root );
      o->model = ModPlanOp.parse( o->moduleplan );
      o->title = wModPlan.gettitle( o->model );
      planXml = NodeOp.base.toString( root );
    }
    else {
      iONode zlevel = NULL;
      o->model = NodeOp.inst( wPlan.name(), NULL, ELEMENT_NODE );
      zlevel = NodeOp.inst( wZLevel.name(), o->model, ELEMENT_NODE );
      wZLevel.settitle(zlevel, "Level 0" );
      NodeOp.addChild(o->model, zlevel );
      o->title = wPlan.gettitle( o->model );
      planXml = NodeOp.base.toString( o->model );
    }

    FileOp.write( o->planFile, planXml, StrOp.len( planXml ) );
    FileOp.flush( o->planFile );
    FileOp.close( o->planFile );
    FileOp.base.del( o->planFile );
    StrOp.free( planXml );

    return True;
  }
  return False;
}



static Boolean _parsePlan( iOModelData o ) {
  if( o->planFile != NULL ) {
    FileOp.close( o->planFile );
    o->planFile->base.del(o->planFile);
    o->planFile = NULL;
  }
  o->planFile = FileOp.inst( o->fileName, True );

  if( o->planFile != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "parsePlan file: %s", o->fileName );
    {
      char* planXml = allocMem( FileOp.size( o->planFile ) + 1 );
      FileOp.read( o->planFile, planXml, FileOp.size( o->planFile ) );
      FileOp.close( o->planFile );
      FileOp.base.del( o->planFile );
      o->planFile = NULL;
      o->planDoc = DocOp.parse( planXml );
      freeMem( planXml );
      if( o->planDoc != NULL ) {
        iONode root = DocOp.getRootNode( o->planDoc );
        if( root != NULL ) {
          if( StrOp.equalsi( wPlan.name(), NodeOp.getName(root) ) ) {
            o->model = root;
            o->title = wPlan.gettitle( o->model );
          }
          else if( StrOp.equalsi( wModPlan.name(), NodeOp.getName(root) ) ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Module Plan [%s]", wModPlan.gettitle( o->model ) );
            o->moduleplan = ModPlanOp.inst(root);
            o->model = ModPlanOp.parse( o->moduleplan );
            o->title = wModPlan.gettitle( o->model );
          }
          else {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Unsupported Plan: %s", NodeOp.getName(root) );
            return False;
          }

          /* check for multiple xyz positions and ID's */
          iOAnalyse analyser = AnalyseOp.inst();
          if( analyser ) {
            o->healthy = AnalyseOp.checkPlanHealth( analyser ); 
            AnalyseOp.base.del(analyser);
          }

          if( !o->healthy ) {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "------------------------------------------------------------" );
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " ***** This plan is not healthy! *****" );
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " One or more double IDs and or overlapping symbols are found." );
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, " Check the trace and correct the exceptions before using it." );
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "------------------------------------------------------------" );
          }
          else {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "plan is healthy." );
          }

        }
        else {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Invalid Plan!" );
          /* rename plan */
          char* invalPlan = StrOp.fmt("%s.invalid.%s", o->fileName, StrOp.createStampNoDots() );
          FileOp.rename(o->fileName, invalPlan);
          StrOp.free(invalPlan);
          return False;
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Invalid Plan file: %s", o->fileName );
        /* rename plan */
        char* invalPlan = StrOp.fmt("%s.invalid.%s", o->fileName, StrOp.createStampNoDots() );
        FileOp.rename(o->fileName, invalPlan);
        StrOp.free(invalPlan);
        return False;
      }
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "parsePlan title: %s", o->title );
    return True;
  }

  return False;
}


static void __addItemInList( iOModelData o, const char* dbkey, iONode node ) {
  iONode db = NodeOp.findNode( o->model, dbkey );
  if( db == NULL ) {
    db = NodeOp.inst( dbkey, o->model, ELEMENT_NODE );
    NodeOp.addChild( o->model, db );
  }
  NodeOp.addChild( db, node );
}

static Boolean __removeItemFromList( iOModelData o, const char* dbkey, iONode node ) {
  iONode db = NodeOp.findNode( o->model, dbkey );
  if( db != NULL ) {
    if( NodeOp.removeChild( db, node ) != NULL ) {
      return True;
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "node [%s] not found in list [%s]; try by id...", wItem.getid(node), dbkey );
      int cnt = NodeOp.getChildCnt(db);
      int i = 0;
      for( i = 0; i < cnt; i++ ) {
        iONode child = NodeOp.getChild( db, i );
        if( StrOp.equals( wItem.getid(child), wItem.getid(node) ) ) {
          if( NodeOp.removeChild( db, child ) != NULL ) {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "node [%s] removed by id from list [%s]", wItem.getid(node), dbkey );
            return True;
          }
        }
      }
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "list [%s] not found", dbkey );
  }
  return False;
}


static void __initGroups( iOModel inst ) {
  iOModelData data = Data(inst);
  iONode link = NULL;
  iONode linklist = wPlan.getlinklist( data->model );
  if( linklist != NULL ) {
    link = wLinkList.getlink( linklist );
    while( link != NULL ) {
      const char* group = wLink.getid( link );
      /* set block group */
      iOStrTok tok = StrTokOp.inst( wLink.getdst( link ), ',' );

      while( StrTokOp.hasMoreTokens( tok ) )  {
        const char* id = StrTokOp.nextToken( tok );
        if( StrOp.len( id ) > 0 ) {
          iIBlockBase block = ModelOp.getBlock( inst, id );
          if( block != NULL )
            block->setGroup( block, group);
        }
      }
      StrTokOp.base.del(tok);

      link = wLinkList.nextlink( linklist, link );
    }
  }
}


static void __updateDigInt( iOModel inst ) {
  iOModelData data = Data(inst);
  iONode db = NULL;
  byte addresses[32][256]; /* 8K on stack? */
  char* addrStr[32];
  char* mods[32];
  Boolean changed[32];
  const char* iid = NULL;

  if( data->model == NULL )
    return;

  db = NodeOp.findNode( data->model, wFeedbackList.name() );

  MemOp.set( addresses, 0, 32*256 );
  MemOp.set( changed, 0, 32 * sizeof(Boolean) );
  MemOp.set( addrStr, 0, 32 * sizeof(char*) );

  /* get the fb list: */
  if( db != NULL ) {
    Boolean mustReport = False;
    int size = NodeOp.getChildCnt( db );
    int i = 0;
    int n = 0;
    /* check all fb module addresses: */
    for( i = 0; i < size; i++ ) {
      iONode fb = NodeOp.getChild( db, i );
      int bus  = wFeedback.getbus( fb );
      int unit = wFeedback.getaddr( fb ) / 8 ; /* assuming sensor modules with 8 contacts */
      /* Assuming all sensors are for the same CS... */
      if( wFeedback.getiid(fb) != NULL && StrOp.len(wFeedback.getiid(fb)) > 0 ) {
        iid = wFeedback.getiid(fb);
      }
      if( bus < 32 && unit < 256 ) {
        addresses[bus][unit] = 1;
        if( !changed[bus] && addresses[bus][unit] != data->fbAddresses[bus][unit] ) {
          data->fbAddresses[bus][unit] = addresses[bus][unit];
          changed[bus] = True;
          mustReport = True;
        }
      }
    }

    if( !mustReport ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensors are unchanged; quit update" );
      return;
    }

    /* create the address string: */
    for( n = 0; n < 32; n++ ) {
      if( changed[n] ) {
        for( i = 0; i < 256; i++ ) {
          if( addresses[n][i] == 1 ) {
            if( addrStr[n] == NULL )
              addrStr[n] = StrOp.fmt( "%d", i );
            else {
              char* s = addrStr[n];
              addrStr[n] = StrOp.fmt( "%s,%d", s, i );
              StrOp.free( s );
            }
          }
        }
      }
    }

    /* send a cmd to the digint: */
    {
      iOControl cntrl = AppOp.getControl(  );
      iONode cmd = NodeOp.inst( wFbInfo.name(), NULL, ELEMENT_NODE );
      wCommand.setcmd( cmd, wCommand.fbmods );
      if( iid != NULL )
        wCommand.setiid( cmd, iid );
      for( n=0; n < 32; n++ ) {
        if( addrStr[n] != NULL ) {
          iONode fmods = NodeOp.inst( wFbMods.name(), cmd, ELEMENT_NODE );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "add bus %d, %s...", n, addrStr[n] );
          wFbMods.setbus( fmods, n );
          wFbMods.setmodules( fmods, addrStr[n] );
          NodeOp.addChild( cmd, fmods );
          StrOp.free(addrStr[n]);
        }
      }
      ControlOp.cmd( cntrl, cmd, NULL );
    }

  }
}

static void _updateFB( iOModel inst ) {
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "updateFB" );
  __updateDigInt( inst );
}

static Boolean _addItem( iOModel inst, iONode item ) {
  iOModelData data = Data(inst);
  const char* itemName = NodeOp.getName( item );
  Boolean added = False;

  if(  !StrOp.equals(wZLevel.name(), NodeOp.getName(item) ) && (wItem.getid(item) == NULL || StrOp.len(wItem.getid(item)) == 0 ||
      StrOp.equals("(null)", wItem.getid(item)) ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "invalid id for new [%s]", itemName );
    return False;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "adding %s %s...", itemName, wItem.getid(item) );

  if( StrOp.equals( wBlock.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOBlock bk = BlockOp.inst( clone );
    __addItemInList( data, wBlockList.name(), clone );
    MapOp.put( data->blockMap, wBlock.getid( item ), (obj)bk );
    added = True;
  }
  else if( StrOp.equals( wLink.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOBlockGroup bg = BlockGroupOp.inst( clone );
    __addItemInList( data, wLinkList.name(), clone );
    MapOp.put( data->blockGroupMap, wLink.getid( item ), (obj)bg );
    added = True;
  }
  else if( StrOp.equals( wTurntable.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOTT tt = TTOp.inst( clone );
    __addItemInList( data, wTurntableList.name(), clone );
    MapOp.put( data->ttMap, wTurntable.getid( item ), (obj)tt );
    added = True;
  }
  else if( StrOp.equals( wSelTab.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOSelTab seltab = SelTabOp.inst( clone );
    __addItemInList( data, wSelTabList.name(), clone );
    MapOp.put( data->seltabMap, wSelTab.getid( item ), (obj)seltab );
    added = True;
  }
  else if( StrOp.equals( wStage.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOStage stage = StageOp.inst( clone );
    __addItemInList( data, wStageList.name(), clone );
    MapOp.put( data->stageMap, wStage.getid( item ), (obj)stage );
    added = True;
  }
  else if( StrOp.equals( wAction.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOAction action = ActionOp.inst( clone );
    __addItemInList( data, wActionList.name(), clone );
    MapOp.put( data->actionMap, wAction.getid( item ), (obj)action );
    added = True;
  }
  else if( StrOp.equals( wTrack.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOTrack tk = TrackOp.inst( clone );
    __addItemInList( data, wTrackList.name(), clone );
    MapOp.put( data->trackMap, wTrack.getid( item ), (obj)tk );
    added = True;
  }
  else if( StrOp.equals( wZLevel.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    NodeOp.addChild( data->model, clone );
    added = True;
  }
  else if( StrOp.equals( wFeedback.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOFBack fb = FBackOp.inst( clone );
    __addItemInList( data, wFeedbackList.name(), clone );
    MapOp.put( data->feedbackMap, wFeedback.getid( item ), (obj)fb );
    __updateDigInt( inst );
    added = True;
  }
  else if( StrOp.equals( wLoc.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOLoc lc = LocOp.inst( clone );
    __addItemInList( data, wLocList.name(), clone );
    MapOp.put( data->locMap, wLoc.getid( item ), (obj)lc );
    added = True;
  }
  else if( StrOp.equals( wCar.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOCar car = CarOp.inst( clone );
    __addItemInList( data, wCarList.name(), clone );
    MapOp.put( data->carMap, wCar.getid( item ), (obj)car );
    added = True;
  }
  else if( StrOp.equals( wOperator.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOOperator operator = OperatorOp.inst( clone );
    __addItemInList( data, wOperatorList.name(), clone );
    MapOp.put( data->operatorMap, wOperator.getid( item ), (obj)operator );
    added = True;
  }
  else if( StrOp.equals( wRoute.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iORoute st = RouteOp.inst( clone );
    __addItemInList( data, wRouteList.name(), clone );
    MapOp.put( data->routeMap, wRoute.getid( clone ), (obj)st );
    ListOp.add( data->routeList, (obj)st);
    added = True;
  }
  else if( StrOp.equals( wSwitch.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOSwitch sw = SwitchOp.inst( clone );
    __addItemInList( data, wSwitchList.name(), clone );
    MapOp.put( data->switchMap, wSwitch.getid( item ), (obj)sw );
    ListOp.add( data->switchList, (obj)sw );
    added = True;
  }
  else if( StrOp.equals( wSignal.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOSignal sg = SignalOp.inst( clone );
    __addItemInList( data, wSignalList.name(), clone );
    MapOp.put( data->signalMap, wSignal.getid( item ), (obj)sg );
    added = True;
  }
  else if( StrOp.equals( wOutput.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOOutput co = OutputOp.inst( clone );
    __addItemInList( data, wOutputList.name(), clone );
    MapOp.put( data->outputMap, wOutput.getid( item ), (obj)co );
    added = True;
  }
  else if( StrOp.equals( wText.name(), itemName ) ) {
    iONode clone = (iONode)item->base.clone( item );
    iOText tx = TextOp.inst( clone );
    __addItemInList( data, wTextList.name(), clone );
    MapOp.put( data->textMap, wText.getid( item ), (obj)tx );
    added = True;
  }
  else if( StrOp.equals( wLink.name(), itemName ) ) {
    iONode linklist = wPlan.getlinklist( data->model );
    iONode clone = (iONode)item->base.clone( item );
    if( linklist == NULL ) {
      linklist = NodeOp.inst( wLinkList.name(), data->model, ELEMENT_NODE );
      NodeOp.addChild( data->model, linklist );
    }
    NodeOp.addChild( linklist, clone );
    added = True;
  }
  else if( StrOp.equals( wBooster.name(), itemName ) ) {
    iONode boosterlist = wPlan.getboosterlist( data->model );
    iONode clone = (iONode)item->base.clone( item );
    if( boosterlist == NULL ) {
      boosterlist = NodeOp.inst( wBoosterList.name(), data->model, ELEMENT_NODE );
      NodeOp.addChild( data->model, boosterlist );
    }
    NodeOp.addChild( boosterlist, clone );
    added = True;
  }
  else if( StrOp.equals( wLocation.name(), itemName ) ) {
    iONode locationlist = wPlan.getlocationlist( data->model );
    iONode clone = (iONode)item->base.clone( item );
    iOLocation location = LocationOp.inst( clone );
    if( locationlist == NULL ) {
      locationlist = NodeOp.inst( wLocationList.name(), data->model, ELEMENT_NODE );
      NodeOp.addChild( data->model, locationlist );
    }
    NodeOp.addChild( locationlist, clone );
    MapOp.put( data->locationMap, wLocation.getid(clone), (obj)location );
    added = True;
  }
  else if( StrOp.equals( wWaybill.name(), itemName ) ) {
    iONode waybilllist = wPlan.getwaybilllist( data->model );
    iONode clone = (iONode)item->base.clone( item );
    if( waybilllist == NULL ) {
      waybilllist = NodeOp.inst( wWaybillList.name(), data->model, ELEMENT_NODE );
      NodeOp.addChild( data->model, waybilllist );
    }
    NodeOp.addChild( waybilllist, clone );
    MapOp.put( data->waybillMap, wWaybill.getid(clone), (obj)clone );
    added = True;
  }
  else if( StrOp.equals( wSchedule.name(), itemName ) ) {
    iONode sclist = wPlan.getsclist( data->model );
    iONode clone = (iONode)item->base.clone( item );
    if( sclist == NULL ) {
      sclist = NodeOp.inst( wScheduleList.name(), data->model, ELEMENT_NODE );
      NodeOp.addChild( data->model, sclist );
    }
    NodeOp.addChild( sclist, clone );
    MapOp.put( data->scheduleMap, wSchedule.getid(clone), (obj)clone );

    added = True;
  }

  else if( StrOp.equals( wTour.name(), itemName ) ) {
    iONode tourlist = wPlan.gettourlist( data->model );
    iONode clone = (iONode)item->base.clone( item );
    if( tourlist == NULL ) {
      tourlist = NodeOp.inst( wTourList.name(), data->model, ELEMENT_NODE );
      NodeOp.addChild( data->model, tourlist );
    }
    NodeOp.addChild( tourlist, clone );
    MapOp.put( data->tourMap, wTour.getid(clone), (obj)clone );

    added = True;
  }

  if(added) {
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.add );
    NodeOp.addChild( cmd, (iONode)NodeOp.base.clone( item ) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "new item added %s %s", itemName, wItem.getid(item) );
    /* Broadcast to clients. */
    AppOp.broadcastEvent( cmd );
  }

  return added;
}


static Boolean _modifyItem( iOModel inst, iONode item ) {
  iOModelData data = Data(inst);
  const char* name = NodeOp.getName( item );
  const char* id = wItem.getid( item );
  const char* prev_id = wItem.getprev_id( item );
  Boolean modified = False;

  if( !StrOp.equals(wMVTrack.name(), NodeOp.getName(item) ) &&
      !StrOp.equals(wSystemActions.name(), NodeOp.getName(item) ) &&
      !StrOp.equals(wZLevel.name(), NodeOp.getName(item) ) && (id == NULL ||
      StrOp.len(id) == 0 || StrOp.equals("(null)", id) ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "invalid id for modify [%s]", name );
    return False;
  }


  if( StrOp.equals( wModule.name(), name ) ) {
    modified = ModPlanOp.modify(data->moduleplan, item);
  }
  else if( StrOp.equals( wBlock.name(), name ) ) {
    iOBlock bk = (iOBlock)MapOp.get( data->blockMap, id );
    if( bk != NULL ) {
      BlockOp.modify( bk, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (bk = (iOBlock)MapOp.get( data->blockMap, prev_id ) ) ) {
      BlockOp.modify( bk, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->blockMap, prev_id );
      MapOp.put( data->blockMap, id, (obj)bk );
      modified = True;
    }
    else if( wBlock.getid( item ) != NULL && StrOp.len( wBlock.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wLink.name(), name ) ) {
    iOBlockGroup bg = (iOBlockGroup)MapOp.get( data->blockGroupMap, id );
    if( bg != NULL ) {
      BlockGroupOp.modify( bg, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (bg = (iOBlockGroup)MapOp.get( data->blockGroupMap, prev_id ) ) ) {
      BlockGroupOp.modify( bg, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->blockGroupMap, prev_id );
      MapOp.put( data->blockGroupMap, id, (obj)bg );
      modified = True;
    }
    else if( wLink.getid( item ) != NULL && StrOp.len( wLink.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wTrack.name(), name ) ) {
    iOTrack tk = (iOTrack)MapOp.get( data->trackMap, id );
    if( tk != NULL ) {
      TrackOp.modify( tk, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (tk = (iOTrack)MapOp.get( data->trackMap, prev_id ) ) ) {
      TrackOp.modify( tk, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->trackMap, prev_id );
      MapOp.put( data->trackMap, id, (obj)tk );
      modified = True;
    }
    else if( wTrack.getid( item ) != NULL && StrOp.len( wTrack.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wFeedback.name(), name ) ) {
    iOFBack fb = (iOFBack)MapOp.get( data->feedbackMap, wFeedback.getid( item ) );
    if( fb != NULL ) {
      FBackOp.modify( fb, (iONode)NodeOp.base.clone( item ) );
      modified = True;
      __updateDigInt( inst );
    }
    else if( StrOp.len(prev_id) > 0 && (fb = (iOFBack)MapOp.get( data->feedbackMap, prev_id ) ) ) {
      FBackOp.modify( fb, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->feedbackMap, prev_id );
      MapOp.put( data->feedbackMap, id, (obj)fb );
      modified = True;
    }
    else if( wFeedback.getid( item ) != NULL && StrOp.len( wFeedback.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wLoc.name(), name ) ) {
    iOLoc lc = (iOLoc)MapOp.get( data->locMap, wLoc.getid( item ) );
    if( lc != NULL ) {
      LocOp.modify( lc, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (lc = (iOLoc)MapOp.get( data->locMap, prev_id ) ) ) {
      LocOp.modify( lc, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->locMap, prev_id );
      MapOp.put( data->locMap, id, (obj)lc );
      modified = True;
    }
    else if( wLoc.getid( item ) != NULL && StrOp.len( wLoc.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
    if( modified ) {
      __initMasterLocMap(inst);
    }
  }
  else if( StrOp.equals( wCar.name(), name ) ) {
    iOCar car = (iOCar)MapOp.get( data->carMap, wCar.getid( item ) );
    if( car != NULL ) {
      CarOp.modify( car, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (car = (iOCar)MapOp.get( data->carMap, prev_id ) ) ) {
      CarOp.modify( car, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->carMap, prev_id );
      MapOp.put( data->carMap, id, (obj)car );
      modified = True;
    }
    else if( wCar.getid( item ) != NULL && StrOp.len( wCar.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wOperator.name(), name ) ) {
    iOOperator operator = (iOOperator)MapOp.get( data->operatorMap, wOperator.getid( item ) );
    if( operator != NULL ) {
      OperatorOp.modify( operator, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (operator = (iOOperator)MapOp.get( data->operatorMap, prev_id ) ) ) {
      OperatorOp.modify( operator, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->carMap, prev_id );
      MapOp.put( data->operatorMap, id, (obj)operator );
      modified = True;
    }
    else if( wOperator.getid( item ) != NULL && StrOp.len( wOperator.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wRoute.name(), name ) ) {
    iORoute st = (iORoute)MapOp.get( data->routeMap, wRoute.getid( item ) );
    if( st != NULL ) {
      RouteOp.modify( st, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (st = (iORoute)MapOp.get( data->routeMap, prev_id ) ) ) {
      RouteOp.modify( st, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->routeMap, prev_id );
      MapOp.put( data->routeMap, id, (obj)st );
      modified = True;
    }
    else if( wRoute.getid( item ) != NULL && StrOp.len( wRoute.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wSwitch.name(), name ) ) {
    iOSwitch sw = (iOSwitch)MapOp.get( data->switchMap, wSwitch.getid( item ) );
    if( sw != NULL ) {
      SwitchOp.modify( sw, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (sw = (iOSwitch)MapOp.get( data->switchMap, prev_id ) ) ) {
      SwitchOp.modify( sw, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->switchMap, prev_id );
      MapOp.put( data->switchMap, id, (obj)sw );
      modified = True;
    }
    else if( wSwitch.getid( item ) != NULL && StrOp.len( wSwitch.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wSignal.name(), name ) ) {
    iOSignal sg = (iOSignal)MapOp.get( data->signalMap, wSignal.getid( item ) );
    if( sg != NULL ) {
      SignalOp.modify( sg, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (sg = (iOSignal)MapOp.get( data->signalMap, prev_id ) ) ) {
      SignalOp.modify( sg, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->signalMap, prev_id );
      MapOp.put( data->signalMap, id, (obj)sg );
      modified = True;
    }
    else if( wSignal.getid( item ) != NULL && StrOp.len( wSignal.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wOutput.name(), name ) ) {
    iOOutput co = (iOOutput)MapOp.get( data->outputMap, wOutput.getid( item ) );
    if( co != NULL ) {
      OutputOp.modify( co, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (co = (iOOutput)MapOp.get( data->outputMap, prev_id ) ) ) {
      OutputOp.modify( co, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->outputMap, prev_id );
      MapOp.put( data->outputMap, id, (obj)co );
      modified = True;
    }
    else if( wOutput.getid( item ) != NULL && StrOp.len( wOutput.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wText.name(), name ) ) {
    iOText txt = (iOText)MapOp.get( data->textMap, wText.getid( item ) );
    if( txt != NULL ) {
      TextOp.modify( txt, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (txt = (iOText)MapOp.get( data->textMap, prev_id ) ) ) {
      TextOp.modify( txt, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->textMap, prev_id );
      MapOp.put( data->textMap, id, (obj)txt );
      modified = True;
    }
    else if( wText.getid( item ) != NULL && StrOp.len( wText.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wTurntable.name(), name ) ) {
    iOTT tt = (iOTT)MapOp.get( data->ttMap, wTurntable.getid( item ) );
    if( tt != NULL ) {
      TTOp.modify( tt, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (tt = (iOTT)MapOp.get( data->ttMap, prev_id ) ) ) {
      TTOp.modify( tt, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->ttMap, prev_id );
      MapOp.put( data->ttMap, id, (obj)tt );
      modified = True;
    }
    else if( wTurntable.getid( item ) != NULL && StrOp.len( wTurntable.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wSelTab.name(), name ) ) {
    iOSelTab seltab = (iOSelTab)MapOp.get( data->seltabMap, wSelTab.getid( item ) );
    if( seltab != NULL ) {
      SelTabOp.modify( seltab, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (seltab = (iOSelTab)MapOp.get( data->seltabMap, prev_id ) ) ) {
      SelTabOp.modify( seltab, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->seltabMap, prev_id );
      MapOp.put( data->seltabMap, id, (obj)seltab );
      modified = True;
    }
    else if( wSelTab.getid( item ) != NULL && StrOp.len( wSelTab.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wStage.name(), name ) ) {
    iOStage stage = (iOStage)MapOp.get( data->stageMap, wStage.getid( item ) );
    if( stage != NULL ) {
      StageOp.modify( stage, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (stage = (iOStage)MapOp.get( data->stageMap, prev_id ) ) ) {
      StageOp.modify( stage, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->stageMap, prev_id );
      MapOp.put( data->stageMap, id, (obj)stage );
      modified = True;
    }
    else if( wStage.getid( item ) != NULL && StrOp.len( wStage.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wAction.name(), name ) ) {
    iOAction action = (iOAction)MapOp.get( data->actionMap, wAction.getid( item ) );
    if( action != NULL ) {
      ActionOp.modify( action, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (action = (iOAction)MapOp.get( data->actionMap, prev_id ) ) ) {
      ActionOp.modify( action, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->actionMap, prev_id );
      MapOp.put( data->actionMap, id, (obj)action );
      modified = True;
    }
    else if( wAction.getid( item ) != NULL && StrOp.len( wAction.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wLink.name(), name ) ) {
    /* modify links... */
    iONode link = NULL;
    iONode linklist = wPlan.getlinklist( data->model );
    if( linklist != NULL ) {
      iONode node = wLinkList.getlink( linklist );
      while( node != NULL ) {
        if( StrOp.equals( wLink.getid( item ), wLink.getid( node ) ) ) {
          NodeOp.mergeNode( node, item, True, True, True );
          link = node;
          break;
        }
        node = wLinkList.nextlink( linklist, node );
      }
    }
    if( link == NULL && wLink.getid( item ) != NULL && StrOp.len( wLink.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wBooster.name(), name ) ) {
    /* modify bosters... */
    iONode booster = NULL;
    iONode boosterlist = wPlan.getboosterlist( data->model );
    if( boosterlist != NULL ) {
      iONode node = wBoosterList.getbooster( boosterlist );
      while( node != NULL ) {
        if( StrOp.equals( wBooster.getid( item ), wBooster.getid( node ) ) ) {
          NodeOp.mergeNode( node, item, True, True, True );
          booster = node;
          break;
        }
        node = wBoosterList.nextbooster( boosterlist, node );
      }
    }
    if( booster == NULL && wBooster.getid( item ) != NULL && StrOp.len( wBooster.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wLocation.name(), name ) ) {
    /* modify location... */
    iOLocation location = (iOLocation)MapOp.get( data->locationMap, wLocation.getid( item ) );
    if( location != NULL ) {
      LocationOp.modify( location, (iONode)NodeOp.base.clone( item ) );
      modified = True;
    }
    else if( StrOp.len(prev_id) > 0 && (location = (iOLocation)MapOp.get( data->locationMap, prev_id ) ) ) {
      LocationOp.modify( location, (iONode)NodeOp.base.clone( item ) );
      MapOp.remove( data->locationMap, prev_id );
      MapOp.put( data->locationMap, id, (obj)location );
      modified = True;
    }
    else if( wLocation.getid( item ) != NULL && StrOp.len( wLocation.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }

  }
  else if( StrOp.equals( wWaybill.name(), name ) ) {
    /* modify waybill... */
    iONode waybill = NULL;
    iONode waybilllist = wPlan.getwaybilllist( data->model );
    if( waybilllist != NULL ) {
      iONode node = wWaybillList.getwaybill( waybilllist );
      while( node != NULL ) {
        Boolean prev_id_matched = StrOp.equals( prev_id, wWaybill.getid( node ) );
        if( StrOp.equals( wWaybill.getid( item ), wWaybill.getid( node ) ) || prev_id_matched ) {
          NodeOp.mergeNode( node, item, True, True, False );
          waybill = node;

          /* update waybill map */
          if( prev_id_matched ) {
            MapOp.remove( data->waybillMap, prev_id );
            MapOp.put( data->waybillMap, id, (obj)node );
          }
          break;
        }
        node = wWaybillList.nextwaybill( waybilllist, node );
      }
    }
    if( waybill == NULL && wWaybill.getid( item ) != NULL && StrOp.len( wWaybill.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wSchedule.name(), name ) ) {
    /* modify location... */
    iONode schedule = NULL;
    iONode sclist = wPlan.getsclist( data->model );
    if( sclist != NULL ) {
      iONode node = wScheduleList.getsc( sclist );
      while( node != NULL ) {
        Boolean prev_id_matched = StrOp.equals( prev_id, wSchedule.getid( node ) );
        if( StrOp.equals( id, wSchedule.getid( node ) ) || prev_id_matched ) {
          int cnt = NodeOp.getAttrCnt( item );
          int i = 0;
          for( i = 0; i < cnt; i++ ) {
            iOAttr attr = NodeOp.getAttr( item, i );
            const char* name  = AttrOp.getName( attr );
            const char* value = AttrOp.getVal( attr );
            NodeOp.setStr( node, name, value );
          }
          cnt = NodeOp.getChildCnt( node );
          while( cnt > 0 ) {
            iONode child = NodeOp.getChild( node, 0 );
            NodeOp.removeChild( node, child );
            cnt = NodeOp.getChildCnt( node );
          }
          cnt = NodeOp.getChildCnt( item );
          for( i = 0; i < cnt; i++ ) {
            iONode child = NodeOp.getChild( item, i );
            NodeOp.addChild( node, (iONode)NodeOp.base.clone(child) );
          }
          schedule = node;

          /* update schedule map */
          if( prev_id_matched ) {
            MapOp.remove( data->scheduleMap, prev_id );
            MapOp.put( data->scheduleMap, id, (obj)node );
          }

          break;
        }
        node = wScheduleList.nextsc( sclist, node );
      }
    }
    if( schedule == NULL && wSchedule.getid( item ) != NULL && StrOp.len( wSchedule.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wTour.name(), name ) ) {
    /* modify tour... */
    iONode tour = NULL;
    iONode tourlist = wPlan.gettourlist( data->model );
    if( tourlist != NULL ) {
      iONode node = wTourList.gettour( tourlist );
      while( node != NULL ) {
        Boolean prev_id_matched = StrOp.equals( prev_id, wTour.getid( node ) );
        if( StrOp.equals( id, wTour.getid( node ) ) || prev_id_matched ) {
          int cnt = NodeOp.getAttrCnt( item );
          int i = 0;
          for( i = 0; i < cnt; i++ ) {
            iOAttr attr = NodeOp.getAttr( item, i );
            const char* name  = AttrOp.getName( attr );
            const char* value = AttrOp.getVal( attr );
            NodeOp.setStr( node, name, value );
          }
          tour = node;

          /* update schedule map */
          if( prev_id_matched ) {
            MapOp.remove( data->tourMap, prev_id );
            MapOp.put( data->tourMap, id, (obj)node );
          }

          break;
        }
        node = wTourList.nexttour( tourlist, node );
      }
    }
    if( tour == NULL && wTour.getid( item ) != NULL && StrOp.len( wTour.getid( item ) ) > 0 ) {
      _addItem( inst, item );
    }
  }
  else if( StrOp.equals( wMVTrack.name(), name ) ) {
    /* modify mvtrack... */
    iONode mv = wPlan.getmv( data->model );
    if( mv == NULL ) {
      mv = (iONode)NodeOp.base.clone( item );
      NodeOp.addChild( data->model, mv );
      data->mvtrack = MVTrackOp.inst( mv );
    }
    else {
      MVTrackOp.modify(data->mvtrack, item);
    }
  }
  else if( StrOp.equals( wZLevel.name(), name ) ) {
    iONode zlevel = wPlan.getzlevel( data->model );
    while( zlevel != NULL ) {
      if( wZLevel.getz( zlevel ) == wZLevel.getz( item ) ) {
        wZLevel.settitle( zlevel, wZLevel.gettitle( item ) );
        break;
      }
      zlevel = wPlan.nextzlevel( data->model, zlevel );
    };
  }
  else if( StrOp.equals( wSystemActions.name(), name ) ) {
    iONode system = wPlan.getsystem( data->model );
    if( system == NULL ) {
      system = (iONode)NodeOp.base.clone( item );
      NodeOp.addChild( data->model, system );
    }
    else {
      int cnt = NodeOp.getChildCnt( system );
      int i = 0;
      while( cnt > 0 ) {
        iONode child = NodeOp.getChild( system, 0 );
        NodeOp.removeChild( system, child );
        cnt = NodeOp.getChildCnt( system );
      }
      cnt = NodeOp.getChildCnt( item );
      for( i = 0; i < cnt; i++ ) {
        iONode child = NodeOp.getChild( item, i );
        NodeOp.addChild( system, (iONode)NodeOp.base.clone(child) );
      }
    }
  }

  return modified;
}

static Boolean _removeItem( iOModel inst, iONode item ) {
  iOModelData o = Data(inst);
  const char* name = NodeOp.getName( item );
  Boolean removed = False;

  if( StrOp.equals( wBlock.name(), name ) ) {
    iIBlockBase bk = (iIBlockBase)MapOp.get( o->blockMap, wBlock.getid( item ) );
    if( bk != NULL ) {
      iONode props = bk->base.properties( bk );
      MapOp.remove( o->blockMap, wBlock.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wBlockList.name(), props );
      bk->base.del( bk );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wLink.name(), name ) ) {
    iOBlockGroup bg = (iOBlockGroup)MapOp.get( o->blockGroupMap, wLink.getid( item ) );
    if( bg != NULL ) {
      iONode props = BlockGroupOp.base.properties( bg );
      MapOp.remove( o->blockGroupMap, wLink.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wLinkList.name(), props );
      bg->base.del( bg );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wTrack.name(), name ) ) {
    iOTrack tk = (iOTrack)MapOp.get( o->trackMap, wTrack.getid( item ) );
    if( tk != NULL ) {
      iONode props = TrackOp.base.properties( tk );
      MapOp.remove( o->trackMap, wTrack.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wTrackList.name(), props );
      tk->base.del( tk );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wTurntable.name(), name ) ) {
    iOTT tt = (iOTT)MapOp.get( o->ttMap, wTurntable.getid( item ) );
    if( tt != NULL ) {
      iONode props = TTOp.base.properties( tt );
      MapOp.remove( o->ttMap, wTurntable.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wTurntableList.name(), props );
      tt->base.del( tt );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wSelTab.name(), name ) ) {
    iOSelTab seltab = (iOSelTab)MapOp.get( o->seltabMap, wSelTab.getid( item ) );
    if( seltab != NULL ) {
      iONode props = SelTabOp.base.properties( seltab );
      MapOp.remove( o->seltabMap, wSelTab.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wSelTabList.name(), props );
      seltab->base.del( seltab );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wStage.name(), name ) ) {
    iOStage stage = (iOStage)MapOp.get( o->stageMap, wStage.getid( item ) );
    if( stage != NULL ) {
      iONode props = StageOp.base.properties( stage );
      MapOp.remove( o->stageMap, wStage.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wStageList.name(), props );
      stage->base.del( stage );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wAction.name(), name ) ) {
    iOAction action = (iOAction)MapOp.get( o->actionMap, wAction.getid( item ) );
    if( action != NULL ) {
      iONode props = ActionOp.base.properties( action );
      MapOp.remove( o->actionMap, wAction.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wActionList.name(), props );
      action->base.del( action );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wFeedback.name(), name ) ) {
    iOFBack fb = (iOFBack)MapOp.get( o->feedbackMap, wFeedback.getid( item ) );
    if( fb != NULL ) {
      iONode props = FBackOp.base.properties( fb );
      MapOp.remove( o->feedbackMap, wFeedback.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wFeedbackList.name(), props );
      fb->base.del( fb );
      props->base.del( props );
      removed = True;
      __updateDigInt( inst );
    }
  }
  else if( StrOp.equals( wLoc.name(), name ) ) {
    removed = __removeLoco(o, item);
  }
  else if( StrOp.equals( wCar.name(), name ) ) {
    iOCar car = (iOCar)MapOp.get( o->carMap, wCar.getid( item ) );
    if( car != NULL ) {
      iONode props = CarOp.base.properties( car );
      MapOp.remove( o->carMap, wCar.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wCarList.name(), props );
      car->base.del( car );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wOperator.name(), name ) ) {
    iOOperator operator = (iOOperator)MapOp.get( o->operatorMap, wOperator.getid( item ) );
    if( operator != NULL ) {
      iONode props = OperatorOp.base.properties( operator );
      MapOp.remove( o->operatorMap, wOperator.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wOperatorList.name(), props );
      operator->base.del( operator );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wRoute.name(), name ) ) {
    iORoute st = (iORoute)MapOp.get( o->routeMap, wRoute.getid( item ) );
    if( st != NULL ) {
      iONode props = RouteOp.base.properties( st );
      MapOp.remove( o->routeMap, wRoute.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wRouteList.name(), props );
      ListOp.removeObj( o->routeList, (obj)st);

      st->base.del( st );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wSwitch.name(), name ) ) {
    iOSwitch sw = (iOSwitch)MapOp.get( o->switchMap, wSwitch.getid( item ) );
    if( sw != NULL ) {
      iONode props = SwitchOp.base.properties( sw );
      MapOp.remove( o->switchMap, wSwitch.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wSwitchList.name(), props );
      ListOp.removeObj( o->switchList, (obj)sw);
      sw->base.del( sw );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wSignal.name(), name ) ) {
    iOSignal sg = (iOSignal)MapOp.get( o->signalMap, wSignal.getid( item ) );
    if( sg != NULL ) {
      iONode props = SignalOp.base.properties( sg );
      MapOp.remove( o->signalMap, wSignal.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wSignalList.name(), props );
      sg->base.del( sg );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wOutput.name(), name ) ) {
    iOOutput co = (iOOutput)MapOp.get( o->outputMap, wOutput.getid( item ) );
    if( co != NULL ) {
      iONode props = OutputOp.base.properties( co );
      MapOp.remove( o->outputMap, wOutput.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wOutputList.name(), props );
      co->base.del( co );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wText.name(), name ) ) {
    iOText tx = (iOText)MapOp.get( o->textMap, wText.getid( item ) );
    if( tx != NULL ) {
      iONode props = TextOp.base.properties( tx );
      MapOp.remove( o->textMap, wOutput.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wTextList.name(), props );
      tx->base.del( tx );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wZLevel.name(), name ) ) {
    iONode zlevel = wPlan.getzlevel( o->model );
    while( zlevel != NULL ) {
      if( StrOp.equals( wZLevel.gettitle( zlevel ), wZLevel.gettitle( item ) ) ) {
        NodeOp.removeChild( o->model, zlevel );
        if( o->moduleplan != NULL ) {
          ModPlanOp.modify(o->moduleplan, zlevel);
        }
        removed = True;
        break;
      }
      zlevel = wPlan.nextzlevel( o->model, zlevel );
    };
  }
  else if( StrOp.equals( wLocation.name(), name ) ) {
    iOLocation location = (iOLocation)MapOp.get( o->locationMap, wLocation.getid( item ) );
    if( location != NULL ) {
      iONode props = LocationOp.base.properties( location );
      MapOp.remove( o->locationMap, wLocation.getid( item ) );
      /* Remove item from list: */
      __removeItemFromList( o, wLocationList.name(), props );
      location->base.del( location );
      props->base.del( props );
      removed = True;
    }
  }
  else if( StrOp.equals( wWaybill.name(), name ) ) {
    iONode waybill = NULL;
    iONode waybilllist = wPlan.getwaybilllist( o->model );
    if( waybilllist != NULL ) {
      iONode node = wWaybillList.getwaybill( waybilllist );
      while( node != NULL ) {
        if( StrOp.equals( wWaybill.getid( item ), wWaybill.getid( node ) ) ) {
          NodeOp.removeChild( waybilllist, node );
          MapOp.remove( o->waybillMap, wWaybill.getid( item ) );
          node->base.del( node );
          removed = True;
          break;

        }
        node = wWaybillList.nextwaybill( waybilllist, node );
      };
    }
  }
  else if( StrOp.equals( wSchedule.name(), name ) ) {
    iONode schedule = NULL;
    iONode sclist = wPlan.getsclist( o->model );
    if( sclist != NULL ) {
      iONode node = wScheduleList.getsc( sclist );
      while( node != NULL ) {
        if( StrOp.equals( wSchedule.getid( item ), wSchedule.getid( node ) ) ) {
          NodeOp.removeChild( sclist, node );
          MapOp.remove( o->scheduleMap, wSchedule.getid( item ) );
          node->base.del( node );
          removed = True;
          break;
        }
        node = wScheduleList.nextsc( sclist, node );
      };
    }
  }
  else if( StrOp.equals( wTour.name(), name ) ) {
    iONode tour = NULL;
    iONode tourlist = wPlan.gettourlist( o->model );
    if( tourlist != NULL ) {
      iONode node = wTourList.gettour( tourlist );
      while( node != NULL ) {
        if( StrOp.equals( wTour.getid( item ), wTour.getid( node ) ) ) {
          NodeOp.removeChild( tourlist, node );
          MapOp.remove( o->tourMap, wTour.getid( item ) );
          node->base.del( node );
          removed = True;
          break;
        }
        node = wTourList.nexttour( tourlist, node );
      };
    }
  }
  else if( StrOp.equals( wBooster.name(), name ) ) {
    iONode booster = NULL;
    iONode boosterlist = wPlan.getboosterlist( o->model );
    if( boosterlist != NULL ) {
      iONode node = wBoosterList.getbooster( boosterlist );
      while( node != NULL ) {
        if( StrOp.equals( wBooster.getid( item ), wBooster.getid( node ) ) ) {
          NodeOp.removeChild( boosterlist, node );
          node->base.del( node );
          removed = True;
          break;
        }
        node = wBoosterList.nextbooster( boosterlist, node );
      };
    }
  }
  return removed;
}

static void __reset( iOModel inst, Boolean saveCurBlock ) {
  iOModelData data = Data(inst);

  if( wCtrl.ispoweroffatreset( wRocRail.getctrl( AppOp.getIni() ) ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Sending safety Power Off before Reset." );
    AppOp.stop(  );
  }

  {
    iOLoc loc = (iOLoc)MapOp.first( data->locMap );
    while( loc != NULL ) {
      LocOp.reset( loc, saveCurBlock );
      loc = (iOLoc)MapOp.next( data->locMap );
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reset blocks..." );
  {
    iIBlockBase block = (iIBlockBase)MapOp.first( data->blockMap );
    while( block != NULL ) {
      block->reset( block, saveCurBlock );
      if( block->getLoc( block ) != NULL && StrOp.len(block->getLoc( block )) > 0 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "Block [%s] is occupied by [%d] after reset.", block->base.id(block), block->getLoc( block ) );
      }
      block = (iIBlockBase)MapOp.next( data->blockMap );
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reset block groups..." );
  {
    iOBlockGroup bg = (iOBlockGroup)MapOp.first( data->blockGroupMap );
    while( bg != NULL ) {
      bg->reset( bg );
      bg = (iOBlockGroup)MapOp.next( data->blockGroupMap );
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reset routes..." );
  {
    iORoute route = (iORoute)MapOp.first( data->routeMap );
    while( route != NULL ) {
      RouteOp.reset( route );
      route = (iORoute)MapOp.next( data->routeMap );
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reset turntables..." );
  {
    iIBlockBase tt = (iIBlockBase)MapOp.first( data->ttMap );
    while( tt != NULL ) {
      tt->reset( tt, saveCurBlock );
      tt = (iIBlockBase)MapOp.next( data->ttMap );
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reset switches..." );
  {
    iOSwitch sw = (iOSwitch)MapOp.first( data->switchMap );
    while( sw != NULL ) {
      SwitchOp.reset( sw );
      sw = (iOSwitch)MapOp.next( data->switchMap );
    }
  }

  /* save the cleaned occupancy */
  if( saveCurBlock )
    ModelOp.saveBlockOccupancy(inst, NULL);

}

static Boolean _isAuto( iOModel inst ) {
  iOModelData data = Data(inst);
  return data->autoMode;
}

static Boolean _isHealthy( iOModel inst ) {
  iOModelData data = Data(inst);
  return data->healthy;
}

static Boolean _isCheck2In( iOModel inst ) {
  iOModelData data = Data(inst);
  return data->check2in;
}

static Boolean _isEnableSwFb( iOModel inst ) {
  iOModelData data = Data(inst);
  return data->enableswfb;
}

static void __stopAllLocs( iOModel inst ) {
  iOModelData data = Data(inst);
  iOLoc loc = (iOLoc)MapOp.first( data->locMap );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Stopping all Locs..." );
  while( loc != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Stop request to Loc [%s]", LocOp.getId(loc) );
    LocOp.stop( loc, True );
    ThreadOp.sleep( 10 );
    loc = (iOLoc)MapOp.next( data->locMap );
  }
}

static Boolean __anyRunningLoco( iOModel inst ) {
  iOModelData data = Data(inst);
  iOLoc loc = (iOLoc)MapOp.first( data->locMap );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Checking all Loco's for V=0..." );
  while( loc != NULL ) {
    if( LocOp.getV( loc ) > 0 )
      return True;
    ThreadOp.sleep( 10 );
    loc = (iOLoc)MapOp.next( data->locMap );
  }
  return False;
}


static void __timedoffRunner( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOModel model = (iOModel)ThreadOp.getParm( th );
  iOModelData data = Data(model);
  iOFBack fb = NULL;
  int i = 0;
  ThreadOp.sleep(500);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timed off thread started");
  while( True ) {
    ThreadOp.sleep(100);
    fb = (iOFBack)MapOp.first( data->feedbackMap );
    while( fb != NULL ) {
      FBackOp.doTimedOff(fb);
      fb = (iOFBack)MapOp.next(data->feedbackMap);
    }
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timed off thread ended");
}


static void __startAllLocosRunner( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOModel model = (iOModel)ThreadOp.getParm( th );
  iOModelData data = Data(model);
  int gap = wCtrl.getlocostartgap( wRocRail.getctrl( AppOp.getIni(  ) ) );

  iOLoc loc = (iOLoc)MapOp.first( data->locMap );
  Boolean resume = StrOp.equals("resumeall", ThreadOp.getName(th));
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s locos...", ThreadOp.getName(th) );
  while( loc != NULL ) {
    Boolean lcgo = True;
    if( resume && !LocOp.isResumeAutomode(loc) )
      lcgo = False;

    if( lcgo && LocOp.go( loc ) ) {
      ThreadOp.sleep( 10 + gap * 1000 );
    }
    else
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Loco %s could not be started; skipping start gap.", LocOp.getId(loc) );
    loc = (iOLoc)MapOp.next( data->locMap );
  }

  if( MapOp.size(data->stageMap) > 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s stages...", ThreadOp.getName(th) );
    iOStage stage = (iOStage)MapOp.first( data->stageMap );
    while( stage != NULL ) {
      iONode cmd = NodeOp.inst( wStage.name(), NULL, ELEMENT_NODE );
      wStage.setcmd( cmd, wStage.compress );
      StageOp.cmd((iIBlockBase)stage, cmd);
      ThreadOp.sleep( 10 + gap * 1000 );
      stage = (iOStage)MapOp.next( data->stageMap );
    }
  }

  data->pendingstartall = False;
  ThreadOp.base.del(threadinst);
}

static void __startAllLocs( iOModel inst ) {
  /* Start a thread for starting all loco's with bigger intervals then 10ms. */
  /* The auto section of the rocrail.ini must be extended with a start delay parameter in seconds. */

  iOModelData data = Data(inst);
  if( data->pendingstartall ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Start all locos canceled: pending operation." );
  }
  else {
    iOThread t = ThreadOp.inst( "startall", &__startAllLocosRunner, inst );
    data->pendingstartall = True;
    ThreadOp.start( t );
  }
 }

static void __resumeAllLocs( iOModel inst ) {
  iOModelData data = Data(inst);
  if( data->pendingstartall ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Start all locos canceled: pending operation." );
  }
  else {
    iOThread t = ThreadOp.inst( "resumeall", &__startAllLocosRunner, inst );
    data->pendingstartall = True;
    ThreadOp.start( t );
  }
}


static Boolean _cmd( iOModel inst, iONode cmd ) {
  iOModelData data = Data(inst);
  const char* cmdName = NodeOp.getName( cmd );
  const char* cmdVal  = wCommand.getcmd( cmd );

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "%s: %s", cmdName, cmdVal );

  if( StrOp.equals( wSysCmd.name(), cmdName ) && !StrOp.equals( wSysCmd.dcc, cmdVal ) &&
      !StrOp.equals( wSysCmd.loccnfg, cmdVal ) && !StrOp.equals( wSysCmd.link, cmdVal ) &&
      !StrOp.equals( wSysCmd.resetblock, cmdVal ) )
  {
    if( StrOp.equals( wSysCmd.ebreak, cmdVal ) ) {
      if( wCtrl.isebreakforceunlock( wRocRail.getctrl( AppOp.getIni() ) ) ) {
        ModelOp.forceUnlock(inst);
      }
    }

    /* inform objects of a power on/off */
    if( MutexOp.trywait(data->muxSysEvent, 10) ) {
      int idx = 0;
      obj listener = ListOp.first( data->sysEventListeners );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
          "informing %d listeners of a system event...", ListOp.size( data->sysEventListeners ) );
      while( listener != NULL ) {
        idx++;
        wSysCmd.setval(cmd, idx);
        listener->event(listener, cmd);
        listener = ListOp.next( data->sysEventListeners );
      };
      MutexOp.post( data->muxSysEvent );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "pending system event cycle; %s:%s rejected", cmdName, cmdVal );
    }
  }
  else if( StrOp.equals( wAutoCmd.name(), cmdName ) ) {
    if( StrOp.equals( wAutoCmd.on, cmdVal ) || StrOp.equals( wAutoCmd.off, cmdVal ) ) {
      Boolean autoMode = StrOp.equals( wAutoCmd.on, cmdVal );
      if( !autoMode && data->autoMode ) {
        __stopAllLocs( inst );
      }
      if( autoMode && !data->autoMode ) {
        /* TODO: signal to all blocks */
        iIBlockBase block = (iIBlockBase)MapOp.first( data->blockMap );
        while( block != NULL ) {
          block->init( block );
          ThreadOp.sleep( wCtrl.getblockinitpause( wRocRail.getctrl( AppOp.getIni() ) ) );
          block = (iIBlockBase)MapOp.next( data->blockMap );
        }
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Block init ready" );
      }
      data->autoMode = autoMode;
      /* Broadcast to clients. */
      AppOp.broadcastEvent( (iONode)NodeOp.base.clone( cmd ) );
    }
    else if( StrOp.equals( wAutoCmd.stop, cmdVal ) ) {
      __stopAllLocs( inst );
    }
    else if( StrOp.equals( wAutoCmd.reset, cmdVal ) ) {
      __reset( inst, False );
    }
    else if( StrOp.equals( wAutoCmd.softreset, cmdVal ) ) {
      __reset( inst, True );
    }
    else if( StrOp.equals( wAutoCmd.start, cmdVal ) ) {
      __startAllLocs( inst );
    }
    else if( StrOp.equals( wAutoCmd.resume, cmdVal ) ) {
      __resumeAllLocs( inst );
    }
  }
  else if( StrOp.equals( wModelCmd.addmodule, cmdVal ) ) {
    /* adding new module to the model */
    if( data->moduleplan != NULL ) {
      iONode module = NodeOp.getChild(cmd,0);
      ModPlanOp.addModule( data->moduleplan, module);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "can not add module: no active module plan..." );
    }
  }
  else if( StrOp.equals( wModelCmd.plantitle, cmdVal ) ) {
    wPlan.settitle(data->model, wModelCmd.getval(cmd) );
  }
  else if( StrOp.equals( wModelCmd.themes, cmdVal ) ) {
    /* TODO: Send the preferred themes to the Rocview.*/
  }
  else if( StrOp.equals( wModelCmd.save, cmdVal ) ) {
    ModelOp.save( inst, False );
  }
  else if( StrOp.equals( wModelCmd.initfield, cmdVal ) ) {
    ModelOp.initField( inst, True );
  }
  else if( StrOp.equals( wModelCmd.add, cmdVal ) ) {
    int childCnt = NodeOp.getChildCnt( cmd );
    int i = 0;
    for( i = 0; i < childCnt; i++ ) {
      iONode child = NodeOp.getChild( cmd, i );
      _addItem( inst, child );
    }
  }
  else if( StrOp.equals( wModelCmd.modify, cmdVal ) ) {
    int childCnt = NodeOp.getChildCnt( cmd );
    int i = 0;
    for( i = 0; i < childCnt; i++ ) {
      iONode child = NodeOp.getChild( cmd, i );
      _modifyItem( inst, child );
    }
  }
  else if( StrOp.equals( wModelCmd.remove, cmdVal ) ) {
    int childCnt = NodeOp.getChildCnt( cmd );
    int i = 0;
    for( i = 0; i < childCnt; i++ ) {
      iONode child = NodeOp.getChild( cmd, i );
      _removeItem( inst, child );
    }
    /* Broadcast to clients. */
    AppOp.broadcastEvent( (iONode)NodeOp.base.clone( cmd ) );
  }
  else if( StrOp.equals( wModelCmd.plan, cmdVal ) ) {
    /* Post AutoState and Model to client. */
    iONode stateevent = ControlOp.getState(AppOp.getControl());
    iONode autoevent = NodeOp.inst( wAutoCmd.name(), NULL, ELEMENT_NODE );

    char* version = StrOp.fmt( "%d.%d-%d", wGlobal.vmajor, wGlobal.vminor, AppOp.getrevno() );
    wPlan.setrocrailversion( data->model, version );
    StrOp.free(version);

    unsigned char* donkey = StrOp.strToByte(AppOp.getdonkey());
    char* decodedKey = SystemOp.decode(donkey, StrOp.len(AppOp.getdonkey())/2, AppOp.getdoneml());

    if( !SystemOp.isExpired(decodedKey, NULL, NULL) ) {
      wPlan.setdonkey(data->model, True);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "valid donation key: %s", wRocRail.getdoneml(AppOp.getIni()) );
    }
    else {
      wPlan.setdonkey(data->model, False);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no valid donation key found" );
    }

    freeMem(decodedKey);
    freeMem(donkey);

    wAutoCmd.setcmd( autoevent, ModelOp.isAuto(inst)?wAutoCmd.on:wAutoCmd.off );
    wState.setconsolemode( stateevent, AppOp.isConsoleMode() );
    wPlan.sethealthy( data->model, ModelOp.isHealthy(inst) );
    ClntConOp.postEvent( AppOp.getClntCon(), autoevent, wCommand.getserver( cmd ) );
    ClntConOp.postEvent( AppOp.getClntCon(), stateevent, wCommand.getserver( cmd ) );
    ClntConOp.postEvent( AppOp.getClntCon(), data->model, wCommand.getserver( cmd ) );
  }
  else if( StrOp.equals( wModelCmd.lclist, cmdVal ) ) {
    if( wPlan.getlclist(data->model) == NULL ) {
      iONode lclist = NodeOp.inst( wLocList.name(), data->model, ELEMENT_NODE );
      NodeOp.addChild( data->model, lclist );
    }
    ClntConOp.postEvent( AppOp.getClntCon(), (iONode)NodeOp.base.clone(wPlan.getlclist(data->model)), wCommand.getserver( cmd ) );
  }
  else if( StrOp.equals( wModelCmd.lcprops, cmdVal ) ) {
    const char* lcID = wModelCmd.getval(cmd);
    iOLoc lc = ModelOp.getLoc( inst, lcID );
    if( lc != NULL ) {
      iONode props = LocOp.base.properties(lc);
      ClntConOp.postEvent( AppOp.getClntCon(), (iONode)NodeOp.base.clone(props), wCommand.getserver( cmd ) );
    }
  }
  else if( StrOp.equals( wModelCmd.swlist, cmdVal ) ) {
    if( wPlan.getswlist(data->model) == NULL ) {
      iONode swlist = NodeOp.inst( wSwitchList.name(), data->model, ELEMENT_NODE );
      NodeOp.addChild( data->model, swlist );
    }
    ClntConOp.postEvent( AppOp.getClntCon(), (iONode)NodeOp.base.clone(wPlan.getswlist(data->model)), wCommand.getserver( cmd ) );
  }
  else if( StrOp.equals( wModelCmd.stlist, cmdVal ) ) {
    if( wPlan.getstlist(data->model) == NULL ) {
      iONode stlist = NodeOp.inst( wRouteList.name(), data->model, ELEMENT_NODE );
      NodeOp.addChild( data->model, stlist );
    }
    ClntConOp.postEvent( AppOp.getClntCon(), (iONode)NodeOp.base.clone(wPlan.getstlist(data->model)), wCommand.getserver( cmd ) );
  }
  else if( StrOp.equals( wModelCmd.sclist, cmdVal ) ) {
    if( wPlan.getsclist(data->model) == NULL ) {
      iONode sclist = NodeOp.inst( wScheduleList.name(), data->model, ELEMENT_NODE );
      NodeOp.addChild( data->model, sclist );
    }
    ClntConOp.postEvent( AppOp.getClntCon(), (iONode)NodeOp.base.clone(wPlan.getsclist(data->model)), wCommand.getserver( cmd ) );
  }
  else if( StrOp.equals( wModelCmd.bklist, cmdVal ) ) {
    if( wPlan.getbklist(data->model) == NULL ) {
      iONode bklist = NodeOp.inst( wBlockList.name(), data->model, ELEMENT_NODE );
      NodeOp.addChild( data->model, bklist );
    }
    ClntConOp.postEvent( AppOp.getClntCon(), (iONode)NodeOp.base.clone(wPlan.getbklist(data->model)), wCommand.getserver( cmd ) );
  }
  else if( StrOp.equals( wModelCmd.fstat, cmdVal ) ) {
    /* post to client. */
    iOList fstats = ModelOp.getFBStat( inst );
    iONode  fstat = NULL;
    /* Send all feedbacks with high state to the client. */
    fstat = (iONode)ListOp.first( fstats );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Sending %d fstats to client...", ListOp.size( fstats ) );
    while( fstat != NULL ) {
      ClntConOp.postEvent( AppOp.getClntCon(), (iONode)NodeOp.base.clone( fstat ), wCommand.getserver( cmd ) );
      fstat = (iONode)ListOp.next( fstats );
      ThreadOp.sleep( 0 );
    };
    ListOp.base.del(fstats);
  }

  cmd->base.del(cmd);

  return True;
}

typedef obj (*item_inst)(iONode);

static void _createMap( iOModelData o, iOMap map, const char* dbKey, const char* itemKey, item_inst instFn, iOList list ) {
  iONode db = NodeOp.findNode( o->model, dbKey );
  if( db != NULL ) {
    iONode item   = NodeOp.findNode( db, itemKey );
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "createMap: %s found.", dbKey );
    while( item != NULL ) {
      const char* key = wItem.getid( item );
      if( key != NULL ) {
        if( instFn != NULL ) {
          obj l_instFn = (obj)instFn(item);
          MapOp.put( map, key, l_instFn );
          if( list != NULL ) {
            ListOp.add( list, l_instFn );
          }
        }
        else {
          MapOp.put( map, key, (obj)item );
          if( list != NULL ) {
            ListOp.add( list, (obj)item );
          }
        }
      }
      item = NodeOp.findNextNode( db, item );
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "createMap() %s,%d.", dbKey, MapOp.size(map) );
  }
  else
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "createMap: %s NOT found.", dbKey );
}



static Boolean __removeLoco(iOModelData o, iONode item ) {
  iOLoc lc = (iOLoc)MapOp.get( o->locMap, wLoc.getid( item ) );
  if( lc != NULL ) {
    iONode props = LocOp.base.properties( lc );
    ModelOp.removeSysEventListener( AppOp.getModel(), (obj)lc );
    MapOp.remove( o->locMap, wLoc.getid( item ) );
    /* Remove item from list: */
    __removeItemFromList( o, wLocList.name(), props );
    lc->base.del( lc );
    props->base.del( props );
    return True;
  }
  return False;
}


static void _removeGenerated( iOModelData o, const char* dbKey, const char* itemKey ) {
  iONode db = NodeOp.findNode( o->model, dbKey );
  if( db != NULL ) {
    iONode item   = NodeOp.findNode( db, itemKey );
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "removeGenerated: %s found.", dbKey );
    while( item != NULL ) {
      iONode nextitem = NodeOp.findNextNode( db, item );
      if( wItem.isgenerated(item) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "removing: %s", wItem.getid(item) );
        if(StrOp.equals( wLoc.name(), NodeOp.getName(item) ) ) {
          __removeLoco(o, item);
        }
        else {
          /* TODO: if used for other objects than locos */
          NodeOp.removeChild( db, item );
          NodeOp.base.del( item );
        }
      }
      item = nextitem;
    }
  }
}

static void _removeFbKey(iOModel inst, const char* key, obj fb) {
  iOModelData data = Data(inst);
  iOList list = (iOList)MapOp.get( data->fbAddrMap, key );
  if( list != NULL ) {
    ListOp.removeObj(list, fb);
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "removeFbKey: size=%d.", MapOp.size(data->fbAddrMap) );
}

static void _addFbKey(iOModel inst, const char* key, obj fb) {
  iOModelData data = Data(inst);
  Boolean multiple = False;
  iOList list = (iOList)MapOp.get( data->fbAddrMap, key );
  if( list == NULL ) {
    list = ListOp.inst();
    MapOp.put( data->fbAddrMap, key, (obj)list );
  }
  else {
    multiple = True;
  }
  ListOp.add( list, fb );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "addFbKey %s%s: size=%d.", key, multiple?"(**multiple**)":"", MapOp.size(data->fbAddrMap) );
}

static void _createFbAddrMap( iOModel inst ) {
  iOModelData o = Data(inst);
  iOFBack fb = (iOFBack)MapOp.first( o->feedbackMap );
  /*MapOp.clear(o->fbAddrMap);*/
  while( fb != NULL ) {
    ModelOp.addFbKey(inst, FBackOp.getAddrKey(fb), (obj)fb );
    fb = (iOFBack)MapOp.next( o->feedbackMap );
  };
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "createFbAddrMap: size=%d.", MapOp.size(o->fbAddrMap) );
}

static void _removeSwKey(iOModel inst, const char* key) {
  iOModelData data = Data(inst);
  MapOp.remove( data->swAddrMap, key );
}

static void _addSwKey(iOModel inst, const char* key, iOSwitch sw) {
  iOModelData data = Data(inst);
  MapOp.put( data->swAddrMap, key, (obj)sw );
}

static void _createSwAddrMap( iOModelData o ) {
  iOSwitch sw = (iOSwitch)MapOp.first( o->switchMap );
  MapOp.clear(o->swAddrMap);
  while( sw != NULL ) {
    MapOp.put( o->swAddrMap, SwitchOp.getAddrKey(sw), (obj)sw );
    if( SwitchOp.getAddrKey2(sw) != NULL )
      MapOp.put( o->swAddrMap, SwitchOp.getAddrKey2(sw), (obj)sw );
    sw = (iOSwitch)MapOp.next( o->switchMap );
  };
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "createSwAddrMap: size=%d.", MapOp.size(o->swAddrMap) );
}

static void _removeCoKey(iOModel inst, const char* key) {
  iOModelData data = Data(inst);
  MapOp.remove( data->coAddrMap, key );
}

static void _addCoKey(iOModel inst, const char* key, iOOutput co) {
  iOModelData data = Data(inst);
  MapOp.put( data->coAddrMap, key, (obj)co );
}

static void _createCoAddrMap( iOModelData o ) {
  iOOutput co = (iOOutput)MapOp.first( o->outputMap );
  MapOp.clear(o->coAddrMap);

  while( co != NULL ) {
    MapOp.put( o->coAddrMap, OutputOp.getAddrKey(co), (obj)co );
    co = (iOOutput)MapOp.next( o->outputMap );
  };
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "createCoAddrMap: size=%d.", MapOp.size(o->coAddrMap) );
}

static void _save( iOModel inst, Boolean removeGen ) {
  iOModelData o = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Saving plan [%s]...", o->fileName );

  if( o->model != NULL ) {
    _removeGenerated(o, wLocList.name(), wLoc.name());
    _removeGenerated(o, wRouteList.name(), wRoute.name());
  }


  if( o->model != NULL && o->moduleplan != NULL ) {
    ModPlanOp.save( o->moduleplan, o->fileName );
  }
  if( o->model != NULL && o->moduleplan == NULL ){
    /* save regular plan */
    char* xml = NULL;
    char* version = StrOp.fmt( "%d.%d.%d-%d", wGlobal.vmajor, wGlobal.vminor, wGlobal.patch, AppOp.getrevno() );
    wPlan.setrocrailversion( o->model, version );
    StrOp.free(version);
    /* Serialize plan. */
    xml = o->model->base.toString( o->model );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serialized Plan=%d", StrOp.len( xml ) );
    if( StrOp.len( xml ) > 0 ) {
      __backupSave( o->fileName, xml );
    }
    StrOp.free( xml );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Plan Saved." );
  }
  ModelOp.saveBlockOccupancy(inst, NULL);
}

static void _saveAs( iOModel inst, const char* fileName ) {
  iOModelData o = Data(inst);
  o->fileName = fileName;
  _save( inst, False );
}


static iOLoc _addNetLoc(iOModel inst, iONode lcprops) {
  iOModelData data = Data(inst);
  iOLoc loc = (iOLoc)MapOp.get( data->locMap, wLoc.getid(lcprops) );
  if( loc == NULL ) {
    iONode cmd = NULL;
    loc = LocOp.inst( (iONode)NodeOp.base.clone(lcprops) );
    MapOp.put( data->locMap, wLoc.getid(lcprops), (obj)loc );

    cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.add );
    NodeOp.addChild( cmd, (iONode)NodeOp.base.clone( lcprops ) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "r2rnet loco added %s", wItem.getid(lcprops) );
    /* Broadcast to clients. */
    AppOp.broadcastEvent( cmd );
  }
  return loc;
}

static iIBlockBase _addNetBlock(iOModel inst, iONode bkprops) {
  iOModelData data = Data(inst);
  iIBlockBase block = (iIBlockBase)MapOp.get( data->blockMap, wBlock.getid(bkprops) );
  if( block == NULL ) {
    block = (iIBlockBase)BlockOp.inst((iONode)NodeOp.base.clone(bkprops));
    MapOp.put( data->blockMap, wBlock.getid(bkprops), (obj)block);
  }
  return block;
}


static iOLoc _getLoc( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  iOLoc loc = (iOLoc)MapOp.get( o->locMap, id );
  if( loc == NULL && id != NULL && StrOp.len(id) > 0 ) {
    int addr = atoi(id);
    if( addr > 0 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "try to find loco by addres [%d]", addr );
      loc = ModelOp.getLocByAddress(inst, addr);
      if( loc != NULL )
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco by addres [%d] is [%s]", addr, LocOp.getId(loc) );
      else {
        iONode lc = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "generating a loco for addres [%d]", addr );
        wLoc.setid( lc, id );
        wLoc.setaddr( lc, addr );
        wLoc.setspcnt( lc, 128 );
        wLoc.setprot( lc, addr > 127 ? wLoc.prot_L:wLoc.prot_N );
        wLoc.setshow( lc, True );
        wItem.setgenerated( lc, True );
        _addItem(inst, lc);
        loc = (iOLoc)MapOp.get( o->locMap, id );
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "undefined loco [%s]", id );
    }
  }
  return loc;
}

static iOCar _getCar( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iOCar)MapOp.get( o->carMap, id );
}

static iOOperator _getOperator( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iOOperator)MapOp.get( o->operatorMap, id );
}

static iOSwitch _getSwByAddress( iOModel inst, int addr, int port ) {
  iOModelData o = Data(inst);
  iOSwitch sw = (iOSwitch)MapOp.first( o->switchMap );
  while( sw != NULL ) {
    if( wSwitch.getaddr1( SwitchOp.base.properties(sw)) == addr && wSwitch.getport1( SwitchOp.base.properties(sw)) == port )
      return sw;
    if( wSwitch.getaddr2( SwitchOp.base.properties(sw)) == addr && wSwitch.getport2( SwitchOp.base.properties(sw)) == port )
      return sw;
    sw = (iOSwitch)MapOp.next( o->switchMap );
  };
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no switch found by address [%d,%d]", addr, port );
  return NULL;
}



static Boolean __isSignalAddres(int addr, int port, int sgaddr, int sgport) {
  if( sgport == 0 && sgaddr > 0 ) {
    int fada = sgaddr;
    sgaddr = fada / 8 + 1;
    sgport = (fada % 8) /2 + 1;
  }
  else if( sgaddr == 0 && sgport > 0 ) {
    int pada = sgport;
    sgaddr = (pada - 1) / 4 + 1;
    sgport = (pada - 1) % 4 + 1;
  }
  if( sgaddr == addr && sgport == port )
    return True;
  return False;
}

static iOSignal _getSgByAddress( iOModel inst, int addr, int port ) {
  iOModelData o = Data(inst);
  iOSignal sg = (iOSignal)MapOp.first( o->signalMap );
  while( sg != NULL ) {
    int sgaddr = wSignal.getaddr( SignalOp.base.properties(sg));
    int sgport = wSignal.getport1( SignalOp.base.properties(sg));
    if( __isSignalAddres( addr, port, sgaddr, sgport ) )
      return sg;

    sgaddr = wSignal.getaddr2( SignalOp.base.properties(sg));
    sgport = wSignal.getport2( SignalOp.base.properties(sg));
    if( __isSignalAddres( addr, port, sgaddr, sgport ) )
      return sg;

    sgaddr = wSignal.getaddr3( SignalOp.base.properties(sg));
    sgport = wSignal.getport3( SignalOp.base.properties(sg));
    if( __isSignalAddres( addr, port, sgaddr, sgport ) )
      return sg;

    sgaddr = wSignal.getaddr4( SignalOp.base.properties(sg));
    sgport = wSignal.getport4( SignalOp.base.properties(sg));
    if( __isSignalAddres( addr, port, sgaddr, sgport ) )
      return sg;

    sg = (iOSignal)MapOp.next( o->signalMap );
  };
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "no signal found by address [%d,%d]", addr, port );
  return NULL;
}


static iOLoc _getLocByAddress( iOModel inst, int addr ) {
  iOModelData o = Data(inst);
  iOLoc loc = (iOLoc)MapOp.first( o->locMap );
  while( loc != NULL ) {
    if( LocOp.getAddress(loc) == addr )
      return loc;
    loc = (iOLoc)MapOp.next( o->locMap );
  };

  return NULL;
}

static iOLoc _getLocByIdent( iOModel inst, const char* ident ) {
  iOModelData o = Data(inst);
  iOLoc locAddr = NULL;
  iOLoc loc = (iOLoc)MapOp.first( o->locMap );
  while( loc != NULL ) {
    char locoAddrStr[32];
    StrOp.fmtb(locoAddrStr, "%d", LocOp.getAddress(loc) );
    if( StrOp.equals(LocOp.getIdent(loc), ident) )
      return loc;
    else if( LocOp.getAddress(loc) > 0 && StrOp.equals(locoAddrStr, ident) )
      locAddr = loc;
    loc = (iOLoc)MapOp.next( o->locMap );
  };

  return locAddr;
}

static iOList _getLocIDs( iOModel inst ) {
  iOModelData o = Data(inst);
  iOList list = ListOp.inst();
  iOLoc loc = (iOLoc)MapOp.first( o->locMap );
  while( loc != NULL ) {
    ListOp.add( list, (obj)LocOp.getId( loc ) );
    loc = (iOLoc)MapOp.next( o->locMap );
  }
  ListOp.sort( list, &__sortStr );
  return list;
}


static void __addLevelItem( iOList list, iONode item, int level, int* cx, int* cy ) {
  if( wItem.getz( item ) == level ) {
    int x = wItem.getx( item );
    int y = wItem.gety( item );
    if( StrOp.equals( NodeOp.getName( item ), wFeedback.name() ) && !wFeedback.isshow( item ) )
      return;
    if( x < 0 || y < 0 )
      return;
    ListOp.add( list, (obj)item );
    if( x > *cx ) *cx = x;
    if( y > *cy ) *cy = y;
  }
}


/**
 * get all items sorted by location for the given zlevel
 */
static iOList _getLevelItems( iOModel inst, int level, int* cx, int* cy, Boolean forcerefresh ) {
  iOModelData data = Data(inst);
  char* levelKey = StrOp.fmt( "%d", level);
  iOList list = NULL;
  iOLevelList levellist = (iOLevelList)MapOp.get( data->levelItemsMap, levelKey );

  if( forcerefresh || levellist == NULL ) {
    iONode itemlist = NULL;
    iONode item = NULL;
    if( levellist != NULL ) {
      list = levellist->list;
      MapOp.remove( data->levelItemsMap, levelKey );
      ListOp.base.del( list );
      freeMem( levellist );
    }
    levellist = allocMem( sizeof( struct LevelList ) );
    list = ListOp.inst();

    itemlist = wPlan.gettklist( data->model );
    *cx = 0;
    *cy = 0;

    if( itemlist != NULL ) {
      item = wTrackList.gettk( itemlist );
      while( item != NULL ) {
        __addLevelItem( list, item, level, cx, cy );
        item =  wTrackList.nexttk( itemlist, item );
      }
    }

    itemlist = wPlan.getbklist( data->model );
    if( itemlist != NULL ) {
      item = wBlockList.getbk( itemlist );
      while( item != NULL ) {
        __addLevelItem( list, item, level, cx, cy );
        item =  wBlockList.nextbk( itemlist, item );
      }
    }

    itemlist = wPlan.getsblist( data->model );
    if( itemlist != NULL ) {
      item = wStageList.getsb( itemlist );
      while( item != NULL ) {
        __addLevelItem( list, item, level, cx, cy );
        item =  wStageList.nextsb( itemlist, item );
      }
    }

    itemlist = wPlan.getfblist( data->model );
    if( itemlist != NULL ) {
      item = wFeedbackList.getfb( itemlist );
      while( item != NULL ) {
        __addLevelItem( list, item, level, cx, cy );
        item =  wFeedbackList.nextfb( itemlist, item );
      }
    }

    itemlist = wPlan.getsglist( data->model );
    if( itemlist != NULL ) {
      item = wSignalList.getsg( itemlist );
      while( item != NULL ) {
        __addLevelItem( list, item, level, cx, cy );
        item =  wSignalList.nextsg( itemlist, item );
      }
    }

    itemlist = wPlan.getswlist( data->model );
    if( itemlist != NULL ) {
      item = wSwitchList.getsw( itemlist );
      while( item != NULL ) {
        __addLevelItem( list, item, level, cx, cy );
        item =  wSwitchList.nextsw( itemlist, item );
      }
    }

    itemlist = wPlan.getcolist( data->model );
    if( itemlist != NULL ) {
      item = wOutputList.getco( itemlist );
      while( item != NULL ) {
        __addLevelItem( list, item, level, cx, cy );
        item =  wOutputList.nextco( itemlist, item );
      }
    }

    itemlist = wPlan.getseltablist( data->model );
    if( itemlist != NULL ) {
      item = wSelTabList.getseltab( itemlist );
      while( item != NULL ) {
        __addLevelItem( list, item, level, cx, cy );
        item =  wSelTabList.nextseltab( itemlist, item );
      }
    }

    ListOp.sort( list, &__sortLocation );
    levellist->list = list;
    levellist->level = level;
    levellist->cx = *cx;
    levellist->cy = *cy;

    MapOp.put( data->levelItemsMap, levelKey, (obj)levellist );
  }
  StrOp.free( levelKey );
  *cx = levellist->cx;
  *cy = levellist->cy;
  return levellist->list;
}



static iIBlockBase _getBlock( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  iIBlockBase bk = (iIBlockBase)MapOp.get( o->blockMap, id );
  if( bk == NULL )
    bk = (iIBlockBase)MapOp.get( o->stageMap, id );
  return bk;
}

static iIBlockBase _getBlockByAddr( iOModel inst, const char* iid, int addr ) {
  iOModelData o = Data(inst);
  iIBlockBase bk = NULL;
  if( addr >= 0 ) {
    iIBlockBase b = (iIBlockBase)MapOp.first(o->blockMap);
    while( b != NULL ) {
      if( wBlock.istd(b->base.properties(b)) && b->getTDport(b) == addr ) {
        bk = b;
        break;
      }
      b = (iIBlockBase)MapOp.next(o->blockMap);
    }
  }
  return bk;
}

static iIBlockBase _getBlock4Signal( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  iIBlockBase block = (iIBlockBase)MapOp.first( o->blockMap );
  while( block != NULL ) {
    iONode props = block->base.properties(block);
    if( wBlock.getsignal(props) != NULL && StrOp.equals(wBlock.getsignal(props), id))
      return (iIBlockBase)block;
    if( wBlock.getwsignal(props) != NULL && StrOp.equals(wBlock.getwsignal(props), id))
      return (iIBlockBase)block;
    if( wBlock.getsignalR(props) != NULL && StrOp.equals(wBlock.getsignalR(props), id))
      return (iIBlockBase)block;
    if( wBlock.getwsignalR(props) != NULL && StrOp.equals(wBlock.getwsignalR(props), id))
      return (iIBlockBase)block;
    block = (iIBlockBase)MapOp.next( o->blockMap );
  };
  return NULL;
}

static iOFBack _getFBack( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  if( id == NULL || StrOp.len( id ) == 0 )
    return NULL;
  return (iOFBack)MapOp.get( o->feedbackMap, id );
}

static iOList _getFBStat( iOModel inst ) {
  iOModelData data = Data(inst);
  iOList list = ListOp.inst();
  iOFBack fback = (iOFBack)MapOp.first( data->feedbackMap );
  while( fback != NULL ) {
    if( FBackOp.getState( fback ) ) {
      iONode node = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
      wFeedback.setid( node, FBackOp.getId( fback ) );
      wFeedback.setstate( node, True );
      wFeedback.setaddr( node, wFeedback.getaddr(fback->base.properties(fback) ) );
      ListOp.add( list, (obj)node );
    }
    fback = (iOFBack)MapOp.next( data->feedbackMap );
  }
  return list;
}

static iOSwitch _getSwitch( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iOSwitch)MapOp.get( o->switchMap, id );
}

static iOMap _getSwitchMap( iOModel inst ) {
  iOModelData o = Data(inst);
  return o->switchMap;
}

static iOMap _getSignalMap( iOModel inst ) {
  iOModelData o = Data(inst);
  return o->signalMap;
}

static iOMap _getFeedbackMap( iOModel inst ) {
  iOModelData o = Data(inst);
  return o->feedbackMap;
}

static iOMap _getLocoMap( iOModel inst ) {
  iOModelData o = Data(inst);
  return o->locMap;
}

static iOSignal _getSignal( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iOSignal)MapOp.get( o->signalMap, id );
}

static iOOutput _getOutput( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iOOutput)MapOp.get( o->outputMap, id );
}

static iORoute _getRoute( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  iORoute route = (iORoute)MapOp.get( o->routeMap, id );
  if( route == NULL && o->moduleplan != NULL ) {
    const char* routeID = ModPlanOp.getResolvedRouteID( o->moduleplan, id );
    if( routeID != NULL ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "use resolved route [%s]", routeID );
      route = (iORoute)MapOp.get( o->routeMap, routeID );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "route [%s] undefined", id );
    }
  }
  else if( route == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "route [%s] undefined", id );
    return NULL;
  }
  return route;
}

static void _addNetRoute(iOModel inst, iONode netroute) {
  iOModelData data = Data(inst);
  iORoute route = RouteOp.inst(netroute);
  ListOp.add( data->routeList, (obj)route );
  MapOp.put( data->routeMap, RouteOp.getId(route), (obj)route );
}


static iOTT _getTurntable( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iOTT)MapOp.get( o->ttMap, id );
}


static iOStage _getStage( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iOStage)MapOp.get( o->stageMap, id );
}


static iOSelTab _getSelectiontable( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iOSelTab)MapOp.get( o->seltabMap, id );
}


static iOAction _getAction( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iOAction)MapOp.get( o->actionMap, id );
}


static iOLocation _getLocation( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iOLocation)MapOp.get( o->locationMap, id );
}


static iONode _getSchedule( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iONode)MapOp.get( o->scheduleMap, id );
}


static iONode _getTour( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iONode)MapOp.get( o->tourMap, id );
}


static iOText _getText( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iOText)MapOp.get( o->textMap, id );
}


static iOTrack _getTrack( iOModel inst, const char* id ) {
  iOModelData o = Data(inst);
  return (iOTrack)MapOp.get( o->trackMap, id );
}


static long _getTime( iOModel inst ) {
  iOModelData o = Data(inst);
  return ControlOp.getTime( AppOp.getControl() );
}


static void __clearMap( iOMap map ) {
  obj item = MapOp.first( map );
  while( item != NULL ) {
    item->del( item );
    item = MapOp.next( map );
  }
  MapOp.clear( map );
}


static void __initTDBlocks(iOModel inst) {
  iOModelData o = Data(inst);
  int pause = wCtrl.getinitfieldpause( wRocRail.getctrl( AppOp.getIni(  ) ) );
  iIBlockBase bk = (iIBlockBase)MapOp.first(o->blockMap);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init Track Driver blocks..." );
  while( bk != NULL ) {
    if( bk->isTD(bk) ) {
      bk->resetTD(bk);
      ThreadOp.sleep( pause );
    }
    bk = (iIBlockBase)MapOp.next( o->blockMap );
  };
}

/** ----------------------------------------------------------------------
  * Init all the switches in the field.
  * ----------------------------------------------------------------------*/
static void __initFieldRunner( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOModel model = (iOModel)ThreadOp.getParm( th );
  iOModelData o = Data(model);
  iOControl cntrl = AppOp.getControl(  );
  iOSwitch sw = NULL;
  iOSignal sg = NULL;
  iOFBack fb = NULL;
  int error = 0;
  int pause = wCtrl.getinitfieldpause( wRocRail.getctrl( AppOp.getIni(  ) ) );
  Boolean gpON = wCtrl.isinitfieldpower( wRocRail.getctrl( AppOp.getIni(  ) ) );

  iONode cmd = NULL;

  /* check if the control is already initialized: */
  while( cntrl == NULL ) {
    ThreadOp.sleep(1000);
    cntrl = AppOp.getControl();
  };

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reset [%d] sensor counters", MapOp.size( o->feedbackMap ) );
  fb = (iOFBack)MapOp.first( o->feedbackMap );
  while( fb != NULL && !ThreadOp.isQuit(th) ) {
    FBackOp.resetCounter(fb);
    fb = (iOFBack)MapOp.next( o->feedbackMap );
  }


  if( gpON ) {
    cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
    wSysCmd.setcmd( cmd, wSysCmd.go );
    wSysCmd.setinformall(cmd, True);
    ControlOp.cmd( cntrl, cmd, NULL );
  }

  __initTDBlocks(model);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Init [%d] switches", MapOp.size( o->switchMap ) );

  sw = (iOSwitch)MapOp.first( o->switchMap );
  while( sw != NULL && !ThreadOp.isQuit(th) ) {
    iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
    const char* cmdStr = NULL;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Init sw [%s]", SwitchOp.getId( sw ) );

    /* Flip the switch. */
    wSwitch.setcmd( cmd, wSwitch.flip );
    SwitchOp.cmd( sw, cmd, False, 0, &error, NULL );

    ThreadOp.sleep( pause );

    /* Set the switch. */
    cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
    wSwitch.setcmd( cmd, wSwitch.flip );
    SwitchOp.cmd( sw, cmd, True, 0, &error, NULL );

    sw = (iOSwitch)MapOp.next( o->switchMap );
    ThreadOp.sleep( pause );
  }

  ThreadOp.sleep( pause );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Init [%d] signals", MapOp.size( o->signalMap ) );

  sg = (iOSignal)MapOp.first( o->signalMap );
  while( sg != NULL && !ThreadOp.isQuit(th) ) {
    const char* cmdStr = NULL;
    const char* state = NULL;

    if(sg->base.properties != NULL) {
      iONode cmd = NodeOp.inst( wSignal.name(), NULL, ELEMENT_NODE );
      iONode sgProps = SignalOp.base.properties(sg);
      state = SignalOp.getState(sg);

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Init sg [%s]", SignalOp.getId( sg ) );

      if( StrOp.equals( wSignal.semaphore, wSignal.gettype(sgProps) ) ) {
        iONode semcmd = NodeOp.inst( wSignal.name(), NULL, ELEMENT_NODE );
        NodeOp.setBool( semcmd, "force", True );
        wSignal.setcmd( semcmd, wSignal.flip );
        SignalOp.cmd( sg, semcmd, True );
        ThreadOp.sleep( pause );
      }

      /* Set the signal to its last known state. */
      if( state != NULL && StrOp.len(state) > 0 ) {
        wSignal.setcmd( cmd, state );
      }
      else
        wSignal.setcmd( cmd, wSignal.red );

      NodeOp.setBool( cmd, "force", True );
      SignalOp.cmd( sg, cmd, True );
    }


    sg = (iOSignal)MapOp.next( o->signalMap );
    ThreadOp.sleep( pause );
  }

  o->pendinginitfield = False;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init field ready" );
  ThreadOp.base.del( threadinst );
}

static void _initField( iOModel inst, Boolean full ) {
  iOModelData data = Data(inst);
  if( __anyRunningLoco(inst) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Init field canceled: one or more loco's are running." );
  }
  else if( data->pendinginitfield ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Init field canceled: pending operation." );
  }
  else {
    if( full ) {
      iOThread t = ThreadOp.inst( "initField", &__initFieldRunner, inst );
      data->pendinginitfield = True;
      ThreadOp.start( t );
    }
    else {
      __initTDBlocks(inst);
    }
  }
}


static void __reinitRoutes( iOModel inst ) {
  iOModelData o = Data(inst);

  __clearMap( o->routeMap );
  ListOp.clear( o->routeList);
  _createMap( o, o->routeMap   , wRouteList.name(), wRoute.name(), (item_inst)RouteOp.inst, o->routeList );

  ThreadOp.sleep(100);
  if(ListOp.size( o->routeList) > 0 ) {
    int i = 0;
    iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
    wModelCmd.setcmd( cmd, wModelCmd.add );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "broadcast add %d routes", ListOp.size( o->routeList) );
    for( i = 0; i < ListOp.size( o->routeList); i++ ) {
      iORoute item = (iORoute)ListOp.get( o->routeList, i);
      NodeOp.addChild( cmd, (iONode)NodeOp.base.clone( RouteOp.base.properties(item) ) );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "add route %s", RouteOp.getId(item) );
    }
    AppOp.broadcastEvent( cmd );
  }

}


static iOLoc _getMasterLoc(iOModel inst, const char* slaveID ) {
  iOModelData data = Data(inst);
  return (iOLoc)MapOp.get( data->masterLocMap, slaveID);
}


static void __initMasterLocMap(iOModel inst) {
  iOModelData data = Data(inst);
  MapOp.clear( data->masterLocMap );

  iOLoc loc = (iOLoc)MapOp.first( data->locMap );
  while( loc != NULL ) {
    const char* consist = wLoc.getconsist( LocOp.base.properties(loc) );
    if( consist != NULL && StrOp.len( consist ) > 0 ) {
      iOStrTok tok = StrTokOp.inst( consist, ',' );
      while( StrTokOp.hasMoreTokens(tok) ) {
        const char* slaveID = StrTokOp.nextToken(tok);
        MapOp.put( data->masterLocMap, slaveID, (obj)loc );
      }
      StrTokOp.base.del(tok);
    }
    loc = (iOLoc)MapOp.next( data->locMap );
  }

}


static Boolean _init( iOModel inst ) {
  iOModelData o = Data(inst);

  if( !_parsePlan( o ) ) {
    if( !_createEmptyPlan(o) ) {
      return False;
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init clearingMaps..." );
  __clearMap( o->blockMap );
  __clearMap( o->blockGroupMap );
  __clearMap( o->feedbackMap );
  __clearMap( o->locMap );
  __clearMap( o->carMap );
  __clearMap( o->operatorMap );
  __clearMap( o->routeMap );
  __clearMap( o->switchMap );
  __clearMap( o->signalMap );
  __clearMap( o->outputMap );
  __clearMap( o->ttMap );
  __clearMap( o->seltabMap );
  __clearMap( o->stageMap );
  __clearMap( o->actionMap );
  __clearMap( o->textMap );
  __clearMap( o->trackMap );
  __clearMap( o->locationMap );
  __clearMap( o->scheduleMap );
  __clearMap( o->tourMap );

  ListOp.clear( o->routeList);
  ListOp.clear( o->switchList);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init creatingMaps..." );
  _createMap( o, o->trackMap   , wTrackList.name(), wTrack.name(), (item_inst)TrackOp.inst, NULL  );

  _createMap( o, o->feedbackMap, wFeedbackList.name(), wFeedback.name(), (item_inst)FBackOp.inst, NULL  );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init creatingFbAddrMap..." );
  _createFbAddrMap( inst );

  _createMap( o, o->switchMap  , wSwitchList.name(), wSwitch.name(), (item_inst)SwitchOp.inst, o->switchList );
  _createMap( o, o->signalMap  , wSignalList.name(), wSignal.name(), (item_inst)SignalOp.inst, NULL );
  _createMap( o, o->outputMap  , wOutputList.name(), wOutput.name(), (item_inst)OutputOp.inst, NULL );
  _createMap( o, o->routeMap   , wRouteList.name(), wRoute.name(), (item_inst)RouteOp.inst, o->routeList );
  _createMap( o, o->ttMap      , wTurntableList.name(), wTurntable.name(), (item_inst)TTOp.inst, NULL  );
  _createMap( o, o->seltabMap  , wSelTabList.name(), wSelTab.name(), (item_inst)SelTabOp.inst, NULL  );
  _createMap( o, o->stageMap   , wStageList.name(), wStage.name(), (item_inst)StageOp.inst, NULL  );
  _createMap( o, o->actionMap  , wActionList.name(), wAction.name(), (item_inst)ActionOp.inst, NULL  );
  _createMap( o, o->blockMap   , wBlockList.name(), wBlock.name(), (item_inst)BlockOp.inst, NULL  );
  _createMap( o, o->blockGroupMap, wLinkList.name(), wLink.name(), (item_inst)BlockGroupOp.inst, NULL  );
  _createMap( o, o->textMap    , wTextList.name(), wText.name(), (item_inst)TextOp.inst, NULL  );
  _createMap( o, o->locMap     , wLocList.name(), wLoc.name(), (item_inst)LocOp.inst, NULL );
  _createMap( o, o->carMap     , wCarList.name(), wCar.name(), (item_inst)CarOp.inst, NULL );
  _createMap( o, o->waybillMap , wWaybillList.name(), wWaybill.name(), NULL, NULL );
  _createMap( o, o->operatorMap, wOperatorList.name(), wOperator.name(), (item_inst)OperatorOp.inst, NULL );
  _createMap( o, o->locationMap, wLocationList.name(), wLocation.name(), (item_inst)LocationOp.inst, NULL );
  _createMap( o, o->scheduleMap, wScheduleList.name(), wSchedule.name(), NULL, NULL );
  _createMap( o, o->tourMap, wTourList.name(), wTour.name(), NULL, NULL );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init creatingFbAddrMap..." );
  _createSwAddrMap( o );
  _createCoAddrMap( o );

  ModelOp.loadBlockOccupancy(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init blocks..." );
  {
    iIBlockBase block = (iIBlockBase)MapOp.first( o->blockMap );
    while( block != NULL ) {
      block->init( block );
      block = (iIBlockBase)MapOp.next( o->blockMap );
    };

    /* adding the selection tables to the block map: */
    block = (iIBlockBase)MapOp.first( o->seltabMap );
    while( block != NULL ) {
      block->init( block );
      MapOp.put( o->blockMap, block->base.id( block ), (obj)block );
      block = (iIBlockBase)MapOp.next( o->seltabMap );
    };

    /* adding the stageblocks to the block map: */
    block = (iIBlockBase)MapOp.first( o->stageMap );
    while( block != NULL ) {
      block->init( block );
      MapOp.put( o->blockMap, block->base.id( block ), (obj)block );
      block = (iIBlockBase)MapOp.next( o->stageMap );
    };

    /* adding the turntables to the block map: */
    block = (iIBlockBase)MapOp.first( o->ttMap );
    while( block != NULL ) {
      block->init( block );
      if( wTurntable.isembeddedblock(block->base.properties(block) ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "adding TT %s as block",  block->base.id(block));
        MapOp.put( o->blockMap, block->base.id(block), (obj)block );
      }
      block = (iIBlockBase)MapOp.next( o->ttMap );
    };
  }

  if( o->moduleplan != NULL ) {
    if( wModPlan.isinitfield(ModPlanOp.getModPlan(o->moduleplan)) )
      ModelOp.initField(inst, True);
  }

  if( wPlan.getmv( o->model ) != NULL ) {
    iONode mvProps = wPlan.getmv( o->model );
    o->mvtrack = MVTrackOp.inst( mvProps );
  }

  __initGroups(inst);

  __initMasterLocMap(inst);

  /* Reset FxSp flag. */
  wRocRail.setresetspfx(AppOp.getIni(), False);

  return True;
}

static void _event( iOModel inst, iONode nodeC ) {
  iOModelData o = Data(inst);

  if( TraceOp.getLevel(NULL) & TRCLEVEL_DEBUG ) {
    char* strNode = (char*)NodeOp.base.toString( nodeC );
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "event %s", strNode );
    StrOp.free( strNode );
  }


  /* Block track driver event. */
  if( StrOp.equals( wBlock.name(), NodeOp.getName( nodeC ) ) ) {
    iIBlockBase block = ModelOp.getBlock(inst, wBlock.getid(nodeC));
    if( block != NULL ) {
      BlockOp.base.event(block, nodeC);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block [%s] not found", wBlock.getid(nodeC) );
      nodeC->base.del(nodeC);
    }
    return;
  }


  /* Accessory: Sensor or Switch? */
  if( StrOp.equals( wAccessory.name(), NodeOp.getName( nodeC ) ) ) {
    int bus = wAccessory.getnodenr( nodeC );
    int addr = wAccessory.getdevid( nodeC );
    int val = wAccessory.getval1( nodeC );
    const char* iid = wAccessory.getiid( nodeC );

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Accessory event: %d:%d=%d", bus, addr, val);

    /* Check for a sensor match: */
    char* key = FBackOp.createAddrKey( bus, addr, iid );
    iOList list = (iOList)MapOp.get( o->fbAddrMap, key );
    StrOp.free( key );
    if( list != NULL ) {
      obj fb = ListOp.first( list );

      NodeOp.setName(nodeC, wFeedback.name());
      wFeedback.setaddr(nodeC, addr);
      wFeedback.setbus(nodeC, bus);
      wFeedback.setstate(nodeC, val?True:False);

      while( fb != NULL ) {
        fb->event(fb, (iONode)NodeOp.base.clone(nodeC));
        fb = ListOp.next(list);
      }
      nodeC->base.del(nodeC);
      return;
    }
    else {
      /* Try a switch object */
      NodeOp.setName(nodeC, wSwitch.name());
      wSwitch.setbus( nodeC, bus );
      wSwitch.setaddr1( nodeC, addr );
      wSwitch.setport1( nodeC, 0 );
      wSwitch.setstate( nodeC, val?"straight":"turnout" );
      wSwitch.setgatevalue(nodeC, val);
    }

  }


  /* Sensor */
  if( StrOp.equals( wFeedback.name(), NodeOp.getName( nodeC ) ) ) {
    int bus = wFeedback.getbus( nodeC );
    int addr = wFeedback.getaddr( nodeC );
    const char* iid = wFeedback.getiid( nodeC );

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Sensor event: %d:%d", bus, addr);

    char* key = FBackOp.createAddrKey( bus, addr, iid );
    iOList list = (iOList)MapOp.get( o->fbAddrMap, key );
    if( list != NULL ) {
      obj fb = ListOp.first( list );
      while( fb != NULL ) {
        fb->event(fb, (iONode)NodeOp.base.clone(nodeC));
        /*FBackOp.event( fb, nodeC );*/
        fb = ListOp.next(list);
      }
      NodeOp.base.del(nodeC);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "unregistered sensor event: %s %s",
                   key, wFeedback.isstate( nodeC )?"ON":"OFF" );
      /* Cleanup Node3 */
      nodeC->base.del(nodeC);
    }
    StrOp.free( key );
    return;
  }


  /* Loco */
  if( StrOp.equals( wLoc.name(), NodeOp.getName( nodeC ) ) ||
      StrOp.equals( wFunCmd.name(), NodeOp.getName( nodeC ) ) )
  {
    int addr = wLoc.getaddr( nodeC );
    const char* id = wLoc.getid( nodeC );
    const char* iid = wLoc.getiid( nodeC );

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Loco event: %d", addr);

    iOLoc lc = ModelOp.getLocByAddress(inst, addr);

    /* check if the loco ID ist set if not found by address */
    if( lc == NULL && id != NULL && StrOp.len(id) > 0 ) {
      lc = ModelOp.getLoc(inst, id);
    }

    if( lc != NULL ) {
      LocOp.base.event( lc, nodeC );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "UNKNOWN LC %d", addr );
      /* Cleanup Node3 */
    }
    nodeC->base.del(nodeC);
    return;
  }


  /* Switch */
  if( StrOp.equals( wSwitch.name(), NodeOp.getName( nodeC ) ) ) {
    int bus = wSwitch.getbus( nodeC );
    int addr = wSwitch.getaddr1( nodeC );
    int port = wSwitch.getport1( nodeC );
    int matchaddr1;
    int matchport1;
    int matchaddr2;
    int matchport2;
    int fada;
    int pada;
    const char* iid = wSwitch.getiid( nodeC );
    const char* defiid;
    iONode ini    = AppOp.getIni();
    iONode digint = wRocRail.getdigint( ini );
    Boolean matched = False;
    obj sw = NULL;

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Switch event: %d:%d:%d", bus, addr, port);

    if( digint != NULL)
      defiid = wDigInt.getiid( digint );
    else
      defiid = "vcs-1";

    if( wSwitch.getid(nodeC) != NULL && StrOp.len(wSwitch.getid(nodeC)) > 0 ) {
      sw = (obj)ModelOp.getSwitch(inst, wSwitch.getid(nodeC) );
      if( sw != NULL ) {
        sw->event( sw, nodeC );
        return;
      }
    }


    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "trying to match %s event: %d:%d:%d",
        wAccessory.isaccevent(nodeC)?"accessory":"switch", bus, addr, port );

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "iterating switch list %d", ListOp.size(o->switchList) );
    sw = ListOp.first(o->switchList);
    while( sw != NULL ) {
      iONode props = sw->properties(sw);

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "testing: %d:%d:%d", bus, wSwitch.getaddr1(props), wSwitch.getport1(props) );

      matchaddr1 = wSwitch.getaddr1(props);
      matchport1 = wSwitch.getport1(props);
      if( matchaddr1 > 0 && matchport1 == 0 && addr > 0 && port == 0 ) {
        /* flat */
      }
      else if( matchport1 == 0 && matchaddr1 > 0 ) {
        fada = matchaddr1;
        matchaddr1 = fada / 8 + 1;
        matchport1 = (fada % 8) /2 + 1;
      }
      else if( matchaddr1 == 0 && matchport1 > 0 ) {
        pada = matchport1;
        matchaddr1 = (pada - 1) / 4 + 1;
        matchport1 = (pada - 1) % 4 + 1;
      }

      matchaddr2 = wSwitch.getaddr2(props);
      matchport2 = wSwitch.getport2(props);
      if( matchaddr2 > 0 && matchport2 == 0 && addr > 0 && port == 0 ) {
        /* flat */
      }
      else if( matchport2 == 0 && matchaddr2 > 0 ) {
        fada = matchaddr2;
        matchaddr2 = fada / 8 + 1;
        matchport2 = (fada % 8) /2 + 1;
      }
      else if( matchaddr2 == 0 && matchport2 > 0 ) {
        pada = matchport2;
        matchaddr2 = (pada - 1) / 4 + 1;
        matchport2 = (pada - 1) % 4 + 1;
      }

      if( wSwitch.getbus(props) == bus && matchaddr1 == addr && matchport1 == port ||
          wSwitch.getbus(props) == bus && matchaddr2 == addr && matchport2 == port )
      {
        matched = True;
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
            "matching sw=%s iid=%s (iid=%s defiid=%s)",
            sw->id(sw), wSwitch.getiid(props)!=NULL?wSwitch.getiid(props):"?", iid, defiid );

        if( StrOp.len( wSwitch.getiid(props) ) > 0 && StrOp.equals(iid, wSwitch.getiid(props)) )
          sw->event( sw, (iONode)NodeOp.base.clone(nodeC) );
        else if( iid == NULL || StrOp.equals( iid, defiid ) || StrOp.len(iid) == 0 )
          sw->event( sw, (iONode)NodeOp.base.clone(nodeC) );
        else
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "IID does not match" );

        NodeOp.base.del(nodeC);
        return;
      }
      sw = ListOp.next(o->switchList);
    }

    /* Try a signal object... */
    NodeOp.setName(nodeC, wSignal.name() );
  }


  /* Signal */
  if( StrOp.equals( wSignal.name(), NodeOp.getName( nodeC ) ) ) {
    int bus = wSwitch.getbus( nodeC );
    int addr = wSwitch.getaddr1( nodeC );
    int port = wSwitch.getport1( nodeC );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Signal event: %d:%d:%d", bus, addr, port);
    iOSignal sg = ModelOp.getSgByAddress(inst, addr, port);
    if( sg != NULL && wCtrl.issgevents( wRocRail.getctrl( AppOp.getIni() ) ) ) {
      SignalOp.event( sg, (iONode)NodeOp.base.clone(nodeC) );
      NodeOp.base.del(nodeC);
      return;
    }
    /* Try an output object... */
    NodeOp.setName(nodeC, wOutput.name() );
  }


  /* Output */
  if( StrOp.equals( wOutput.name(), NodeOp.getName( nodeC ) ) ) {
    int bus  = wSwitch.getbus( nodeC );
    int addr = wSwitch.getaddr1( nodeC );
    int port = wSwitch.getport1( nodeC );

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Output event: %d:%d:%d", bus, addr, port);

    if( addr == 0 && wOutput.getaddr(nodeC) > 0 )
      addr = wOutput.getaddr(nodeC);
    if( port == 0 && wOutput.getport(nodeC) > 0 )
      port = wOutput.getport(nodeC);

    const char* iid = wOutput.getiid( nodeC );
    char* key = OutputOp.createAddrKey( bus, addr, port, iid );
    iOOutput co = (iOOutput)MapOp.get( o->coAddrMap, key );
    StrOp.free( key );

    if( co != NULL ) {
      OutputOp.event( co, nodeC );
      return;
    }
  }


  /* Default: Nothing matching found. */
  {
    int bus = wSwitch.getbus( nodeC );
    int addr = wSwitch.getaddr1( nodeC );
    int port = wSwitch.getport1( nodeC );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unregistered event: %d:%d:%d", bus, addr, port );
    /* Cleanup Node3 */
    nodeC->base.del(nodeC);
    return;
  }


}

static const char* _getTitle( iOModel inst ) {
  iOModelData o = Data(inst);
  return o->title;
}

static void _analyse( iOModel inst, int mode ) {
  iOModelData data = Data(inst);
  int modified = 0;
  Boolean requirements = True; /* modfying plan allowed ? */

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_analyse (%d)", mode );

  Boolean analyzerEnabled  = wCtrl.isenableanalyzer( wRocRail.getctrl( AppOp.getIni() ));
  Boolean blocksideEnabled = wCtrl.isuseblockside( wRocRail.getctrl( AppOp.getIni() ));
  Boolean planIsHealthy    = ModelOp.isHealthy(inst);
  Boolean automode         = ModelOp.isAuto(inst);
  Boolean isPowerOn        = wState.ispower(ControlOp.getState(AppOp.getControl()));

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "pw %d auto %d health %d bs %d anaE %d",
      isPowerOn, automode, planIsHealthy, blocksideEnabled, analyzerEnabled);

  if( mode == AN_HEALTH ) {
    /* health check is allowed all the time */
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "call AnalyseOp.checkPlanHealth");
    iOAnalyse analyser = AnalyseOp.inst();
    if( analyser ) {
      data->healthy = AnalyseOp.checkPlanHealth( analyser );
      AnalyseOp.base.del(analyser);

      /* set variables according to result */
      planIsHealthy = ModelOp.isHealthy(inst);
      wPlan.sethealthy( data->model, planIsHealthy );
    }
    return;
  }
  if( mode == AN_EXTCHK ) {
    /* extended check is allowed all the time (but not if repair option is set...) */
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "call AnalyseOp.checkExtended" );
    iOAnalyse analyser = AnalyseOp.inst();
    if( analyser ) {
      AnalyseOp.checkExtended( analyser );
      AnalyseOp.base.del(analyser);
    }
    return;
  }

  /* check requirements for analyzer or other plan modifing calls (clean/repair) */

  if( ! analyzerEnabled ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Analyzer is disabled.");
    requirements = False;
  }
  
  if( ! blocksideEnabled ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Use block side routes is not set.");
    requirements = False;
  }

  if( ! planIsHealthy ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "This plan is not healthy.");
    requirements = False;
  }

  if( automode ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Automode is on. Switch off to use analyzer.");
    requirements = False;
  }

  if( isPowerOn ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Track power is on. Switch off to use analyzer.");
    requirements = False;
  }


  if( requirements ) {
    char* stampfile = StrOp.fmt("%s.anabak", data->fileName);
    const char* filename = data->fileName;
    iOAnalyse analyser = NULL;
    ModelOp.saveAs(inst, stampfile);
    data->fileName = filename;

    /* Make sure the route list is available before analyzing or cleaning the track plan. */
    if( wPlan.getstlist(data->model) == NULL ) {
      iONode stlist = NodeOp.inst( wRouteList.name(), data->model, ELEMENT_NODE);
      NodeOp.addChild( data->model, stlist );
    }


    if( mode == AN_EXTCLEAN ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "starting extended cleanup...");
      iOAnalyse analyser = AnalyseOp.inst();
      if( analyser ) {
        AnalyseOp.cleanExtended( analyser );
        AnalyseOp.base.del(analyser);
      }
      return;
    }

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "init analyser..." );
    analyser = AnalyseOp.inst();

    if( analyser != NULL ) {
      if( mode == AN_JOB ) {
        /* first check plan health (as a side effect some variables of the current analyser instance are set) */
        data->healthy = AnalyseOp.checkPlanHealth( analyser );

        /* set variables according to result */
        planIsHealthy = ModelOp.isHealthy(inst);
        wPlan.sethealthy( data->model, planIsHealthy );

        if( ! planIsHealthy ) {
          AnalyseOp.base.del( analyser );
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "This plan is not healthy.");
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "analyzer skipped see lines above.");
        }
      }

      iONode e = NodeOp.inst( wException.name(), NULL, ELEMENT_NODE );
      int nrRoutesBefore = ListOp.size(data->routeList);
      int nrRoutesAfter = 0;
      char* msg = NULL;

      if(ListOp.size( data->routeList) > 0 ) {
        int i = 0;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "broadcast delete %d routes", ListOp.size( data->routeList) );
        iONode cmd = NodeOp.inst( wModelCmd.name(), NULL, ELEMENT_NODE );
        wModelCmd.setcmd( cmd, wModelCmd.remove );
        for( i = 0; i < ListOp.size( data->routeList); i++ ) {
          iORoute item = (iORoute)ListOp.get( data->routeList, i);
          NodeOp.addChild( cmd, (iONode)NodeOp.base.clone( RouteOp.base.properties(item) ) );
        }
        AppOp.broadcastEvent( cmd );
        ThreadOp.sleep(10);
      }

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "starting analyzer...");
      if( mode == AN_JOB ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "starting analyzer...");
        modified = AnalyseOp.analyse( analyser );
      }
      if( mode == AN_CLEAN ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "starting analyzer cleanup...");
        modified = AnalyseOp.cleanup( analyser );
      }

      /* re-initialize routes */
      ThreadOp.sleep(100);
      __reinitRoutes(inst);
      nrRoutesAfter = ListOp.size(data->routeList);

      /* Broadcast to clients. */
      switch( mode ) {
        case AN_JOB:
          msg = StrOp.fmt("route generator created %d new routes (%d -> %d)", nrRoutesAfter-nrRoutesBefore, nrRoutesBefore, nrRoutesAfter);
          break;
        case AN_CLEAN:
          msg = StrOp.fmt("route generator cleanup removed %d routes (%d -> %d)", nrRoutesBefore-nrRoutesAfter, nrRoutesBefore, nrRoutesAfter);
          break;
      }
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s", msg );
      wException.settext( e, msg );
      wException.setlevel( e, TRCLEVEL_CALC );
      AppOp.broadcastEvent( e );

      if( modified > 0 ) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Please restart Rocrail server." );
        /* force disconnect/reconnect ?*/
        /* AppOp.shutdown(); -> sometimes crashes */
        /* ==> need AppOpp.reinit(); */
      }

      /* clean up*/
      AnalyseOp.base.del( analyser );
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "analyzer skipped see lines above.");
  }
}


/**
 * search recursive a route to the destination block
 * first caller should provide an empty list which will be filled with streets
 * needed to get to the destination.
 */
static iORoute __lookup( iOModel inst, iOLoc loc, iOList stlist, const char* fromid, const char* destid,
    int cnt, iOList searchlist, int* foundlevel, Boolean forceSameDir, Boolean swapPlacingInPrevRoute ) {
  iOModelData data = Data(inst);

  iOList list = NULL;
  int   stcnt = 0;
  int       i = 0;
  int  flevel = 0;

  int  shortestcnt = 0;
  iORoute shortestST = NULL;

  if( cnt == 0 ) {
    /* get the street list */
    list = MapOp.getList( data->routeMap );
    searchlist = list;
    foundlevel = &flevel;
  }
  stcnt = ListOp.size( searchlist );
  shortestcnt = stcnt;

  if( cnt > stcnt ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "no route from [%s] to [%s] level=%d", fromid, destid, cnt );
    return NULL;
  }

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "find a route from [%s] to [%s] level=%d", fromid, destid, cnt );

  for( i = 0; i < stcnt; i++ ) {
    iORoute route = (iORoute)ListOp.get( searchlist, i );
    const char* stFrom = RouteOp.getFromBlock( route );
    const char* stTo = RouteOp.getToBlock( route );
    Boolean dir = RouteOp.getDir( route );

    if( !StrOp.equals( stFrom, fromid ) && !StrOp.equals( stTo, fromid ) ) {
      /* not useable; go on */
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "route[%d]-[%s][%s] does not match: level=%d", i, stFrom, stTo, cnt );
      continue;
    }

    if( !route->hasPermission( route, loc, fromid, False ) ) {
      /* not useable; go on */
      continue;
    }

    /* TODO: if( StrOp.equals( stTo, fromid ) && !dir && !forceSameDir) {*/
    if( StrOp.equals( stTo, fromid ) && !dir) {
      /* swap direction */
      const char* tmp = stTo;
      stTo = stFrom;
      stFrom = tmp;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "swapped route[%d]-[%s][%s]: level=%d", i, stFrom, stTo, cnt );
    }

    if( StrOp.equals( stTo, destid ) ) {
      if( stlist != NULL )
        ListOp.insert( stlist, 0, (obj)route );
      /* a hit: */
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "route[%d]-[%s][%s] fits: level=%d", i, stFrom, stTo, cnt );
      *foundlevel = cnt;
      return route;
    }
    /* do a recursion: */
    /*
    else {
      iORoute stfound = __lookup( inst, stlist, stTo, destid, cnt+1, searchlist, foundlevel );
      if( stfound != NULL ) {
        if( stlist != NULL )
          ListOp.insert( stlist, 0, (obj)stfound );
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "route[%d]-[%s][%s] fits in the long run: level=%d", i, stFrom, stTo, cnt );

        if( cnt == 0 && shortestcnt < *foundlevel ) {
          shortestcnt = *foundlevel;
          shortestST = street;
        }

        if( cnt > 0 )
          return street;
      }
    }
    */

  }

  if( shortestST == NULL ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "no route from [%s] to [%s] level=%d", fromid, destid, cnt );
    return NULL;
  }
  /* nothing found. */
  return shortestST;
}


/**
 * Check if the blockid is part of the location.
 *
 */
static Boolean __isInLocation( iOModel inst, const char* entryLocation, const char* blockid ) {
  if( entryLocation != NULL ) {
    /* look in location for the wanted block: */
    iOLocation location = _getLocation( inst, entryLocation );
    if( location != NULL ) {
      return LocationOp.hasBlock(location, blockid);
    }
  }
  return False;
}


static iOLocation _getBlockLocation(iOModel inst, const char* blockid) {
  iOModelData data = Data(inst);
  iOLocation location = (iOLocation)MapOp.first(data->locationMap);
  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "try to find location for block [%s]", blockid);
  while(location != NULL) {
    if( LocationOp.hasBlock(location, blockid) )
      return location;
    location = (iOLocation)MapOp.next(data->locationMap);
  }
  return NULL;
}



static Boolean _isScheduleFree( iOModel inst, const char* id , const char* locoId) {
  iOModelData data = Data(inst);
  Boolean isFree = True;
  iONode sc = ModelOp.getSchedule( inst, id );

  if( sc != NULL ) {
    iONode entry = wSchedule.getscentry(sc);
    while( entry != NULL ) {

      if( wScheduleEntry.isfree2go(entry) ) {
        /* entry must be free before use */
        const char* blockid = wScheduleEntry.getblock(entry);
        const char* locationid = wScheduleEntry.getlocation(entry);

        if( blockid != NULL && StrOp.len( blockid ) > 0 ) {
          iIBlockBase block = ModelOp.getBlock( inst, blockid );
          if( block != NULL && !block->isFree(block, locoId) ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "schedule block entry [%s] is not free2go", blockid );
            return False;
          }
        }
        else if( locationid != NULL && StrOp.len( locationid ) > 0 ) {
          /* check for a free block in the location */
          iOLocation location = ModelOp.getLocation(inst, locationid);
          if( location != NULL && !LocationOp.hasFreeBlock(location, locoId) ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "schedule location entry [%s] is not free2go", locationid );
            return False;
          }
        }
      }

      entry = wSchedule.nextscentry(sc, entry);
    }
  }

  return isFree;
}




/**
 * Check if the schedule index matches the blockid.
 * If it does not match try to findout at what index it is.
 */
static iONode __findScheduleEntry( iOModel inst, iONode schedule, int* scheduleIdx, const char* blockid, Boolean doNotRewind ) {
  int idx = 0;
  iONode entry = wSchedule.getscentry( schedule );
  iONode preventry = NULL;
  iIBlockBase block = ModelOp.getBlock( inst, blockid);
  Boolean idxChecked = False;

  blockid = ModelOp.getManagedID(inst, blockid);

  /* check if the schedule index is correct: */
  while( entry != NULL ) {
    if( idx == *scheduleIdx || idxChecked ) {
      const char* entryBlock = wScheduleEntry.getblock( entry );
      const char* entryLocation = wScheduleEntry.getlocation( entry );
      
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
          "checking schedule index[%d] prev=0x%08X...", *scheduleIdx, preventry );
      if( entryBlock != NULL && StrOp.len(entryBlock) > 0 ) {
        if( StrOp.equals( blockid, entryBlock ) ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
              "schedule index[%d] matches block %s", *scheduleIdx, blockid );
          if( idxChecked )
            *scheduleIdx = idx;
          return entry;
        }
      }

      if( __isInLocation( inst, entryLocation, blockid ) ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
            "schedule index[%d] matches location %s for block %s", *scheduleIdx, entryLocation, blockid );
        if( idxChecked )
          *scheduleIdx = idx;
        return entry;
      }

      if(preventry != NULL ) {
        /* does the previous entry match? */
        const char* entryBlock = wScheduleEntry.getblock( preventry );
        const char* entryLocation = wScheduleEntry.getlocation( preventry );
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
            "checking (prev)schedule index[%d]...", *scheduleIdx );
        if( entryBlock != NULL && StrOp.len(entryBlock) > 0 ) {
          if( StrOp.equals( blockid, entryBlock ) ) {
            *scheduleIdx -= 1;
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                "(prev)schedule index[%d] matches block %s", *scheduleIdx, blockid );
            return preventry;
          }
        }
        else if( __isInLocation( inst, entryLocation, blockid ) ) {
          *scheduleIdx -= 1;
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
              "(prev)schedule index[%d] matches location %s for block %s", *scheduleIdx, entryLocation, blockid );
          return preventry;
        }
      }
      
      if( doNotRewind )
        idxChecked = True;
    }
    idx++;
    preventry = entry;
    entry = wSchedule.nextscentry( schedule, entry );
  };

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
      "schedule index[%d] does not match for block %s", *scheduleIdx, blockid );
      
  if( doNotRewind ) {
    return NULL;
  }

  idx = 0;
  entry = wSchedule.getscentry( schedule );

  /* find the location: */
  while( entry != NULL ) {
    const char* entryBlock = wScheduleEntry.getblock( entry );
    const char* entryLocation = wScheduleEntry.getlocation( entry );

    if( entryBlock != NULL && StrOp.len(entryBlock) > 0 ) {
      if( StrOp.equals( blockid, entryBlock ) ) {
        *scheduleIdx = idx;
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
            "schedule index[%d] found for block %s", *scheduleIdx, blockid );
        return entry;
      }
    }

    if( __isInLocation( inst, entryLocation, blockid ) ) {
      *scheduleIdx = idx;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
          "schedule index[%d] found in location %s for block %s", *scheduleIdx, entryLocation, blockid );
      return entry;
    }

    idx++;
    entry = wSchedule.nextscentry( schedule, entry );
  }

  return NULL;
}

/* Find a route for a schedule entry. */
static iORoute _findRoute( iOModel inst, const char* scheduleid,
                            int* scheduleIdx, const char* curblockid, iOLoc loc )
{
  return NULL;
}



/**
 * lookup the current block in the schedule and calculate the route to the next destination
 */
static iORoute _calcRouteFromCurBlock( iOModel inst, iOList stlist, const char* scheduleid,
                                        int* scheduleIdx, const char* curblockid, const char* currouteid, iOLoc loc,
                                        Boolean forceSameDir, Boolean swapPlacingInPrevRoute, int *indelay ) {
  iONode schedule = ModelOp.getSchedule( inst, scheduleid );
  iONode entry = NULL;
  int entryIndex = *scheduleIdx;
  int maxLoop = 0;

  if( schedule == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "schedule [%s] not found!", scheduleid );
    return NULL;
  }

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "schedule [%s] index %d", scheduleid, *scheduleIdx );

  /* https://bugs.launchpad.net/rocrail/+bug/706593
   * TODO: Evaluate the block enterside using ModelOp.findDest(...gotoBlock...) or
   *   ModelOp.calcRoute() within the __findScheduleEntry() function.
   *
   * Needed info:
   *   wCtrl.isuseblockside( wRocRail.getctrl( AppOp.getIni(  ) ) )
   *   LocOp.getBlockEnterSide(loc)
   */

  if( wCtrl.isuseblockside( wRocRail.getctrl( AppOp.getIni() ) ) ) {
    curblockid = ModelOp.getManagedID(inst, curblockid);
    entry = __findScheduleEntry( inst, schedule, scheduleIdx, curblockid, False );

    if( entry != NULL ) {
      /* save real index */
      entryIndex = *scheduleIdx;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "schedule [%s] real index %d", scheduleid, *scheduleIdx );
    }

    while( entry != NULL && maxLoop < 2 ) {
      /* entry found, get the next destination... */
      entry = wSchedule.nextscentry( schedule, entry );
      *scheduleIdx += 1;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "schedule [%s] index %d", scheduleid, *scheduleIdx );
      maxLoop++;

      if( entry != NULL ) {
        const char* nextlocation = wScheduleEntry.getlocation( entry );
        const char* nextblock    = wScheduleEntry.getblock( entry );

        if( (nextlocation == NULL || StrOp.len(nextlocation) == 0 ) && (nextblock == NULL || StrOp.len(nextblock) == 0) ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "entry in schedule [%s] is undefined.", scheduleid );
          return NULL;
        }

        *indelay = wScheduleEntry.getindelay( entry );
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "entry %d in schedule [%s] has indelay=%d", *scheduleIdx, scheduleid, *indelay );
        if( nextlocation != NULL && StrOp.len(nextlocation) > 0 )
          nextblock = NULL;
        iORoute route = ModelOp.calcRoute( inst, stlist, curblockid, nextlocation, nextblock, loc, forceSameDir, swapPlacingInPrevRoute );
        if( route != NULL ) {
          iORoute routeref = NULL;
          const char* gotoBlock = NULL;
          /* check if findDest with gotoBlock will return positively */
          if( StrOp.equals( curblockid, RouteOp.getFromBlock(route) ) )
            gotoBlock = RouteOp.getToBlock(route);
          else
            gotoBlock = RouteOp.getFromBlock(route);

          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "curblockid [%s], gotoBlock [%s]", curblockid, gotoBlock );

          iIBlockBase destBlock = ModelOp.findDest( inst, curblockid, currouteid, loc, &routeref, gotoBlock,
                                    False, False, forceSameDir, swapPlacingInPrevRoute, False);

          if( destBlock != NULL && StrOp.equals( gotoBlock, destBlock->base.id(destBlock) ) ) {
            return routeref;
          }
          else {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "route [%s] to block [%s] is not usable", RouteOp.getId(route), gotoBlock );
            return NULL;
          }


        }

      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "last entry[%d] in schedule [%s] is reached.", *scheduleIdx, scheduleid );
        *scheduleIdx += 1;
        return NULL;
      }

      *scheduleIdx += 1;
      entry = __findScheduleEntry( inst, schedule, scheduleIdx, curblockid, True );
    };

    *scheduleIdx = entryIndex;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no fitting entry in schedule [%s] found.", scheduleid );
    return NULL;
  }


  /* for none blockside only */

  entry = __findScheduleEntry( inst, schedule, scheduleIdx, curblockid, False );


  if( entry == NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "in schedule [%s] no fitting entry found with blockid [%s]: routing to first entry...", scheduleid, curblockid );
    /* take first schedule entry: */
    entry = wSchedule.getscentry( schedule );
    *scheduleIdx = 0;
  }
  else {
    /* take next schedule entry: */
    entry = wSchedule.nextscentry( schedule, entry );
    if( entry == NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "last entry[%d] in schedule [%s] is reached.", *scheduleIdx, scheduleid );
      *scheduleIdx += 1;
      return NULL;
    }
    *scheduleIdx += 1;
  }

  {
    const char* nextlocation = wScheduleEntry.getlocation( entry );
    const char* nextblock    = wScheduleEntry.getblock( entry );

    if( (nextlocation == NULL || StrOp.len(nextlocation) == 0 ) && (nextblock == NULL || StrOp.len(nextblock) == 0) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "entry in schedule [%s] is undefined.", scheduleid );
      return NULL;
    }

    *indelay = wScheduleEntry.getindelay( entry );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "entry %d in schedule [%s] has indelay=%d", *scheduleIdx, scheduleid, *indelay );
    return ModelOp.calcRoute( inst, stlist, curblockid, nextlocation, nextblock, loc, forceSameDir, swapPlacingInPrevRoute );

  }

  return NULL;
}


/* synchronized!!! */
static iORoute _calcRoute( iOModel inst, iOList stlist, const char* currBlockId, const char* toLocationId,
                             const char* toBlockId, iOLoc loc, Boolean forceSameDir, Boolean swapPlacingInPrevRoute ) {
  iOModelData data = Data(inst);
  iOLocation location = NULL;
  iORoute street = NULL;

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "currentBlock \"%s\" ", currBlockId );

  if( currBlockId == NULL || ( toLocationId == NULL && toBlockId == NULL ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Invalid parameters" );
    return NULL;
  }

  if( toLocationId != NULL && StrOp.len( toLocationId ) > 0 ) {
    location = (iOLocation)MapOp.get( data->locationMap, toLocationId );
    if( location == NULL ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "location \"%s\" not found.", toLocationId );
    }
  }

  currBlockId = ModelOp.getManagedID(inst, currBlockId);

  /* Lock the semaphore: */
  MutexOp.wait( data->muxFindDest );
  {
    iIBlockBase block = NULL;
    Boolean destdir = False;
    Boolean samedir = False;

    if( location != NULL && (toBlockId == NULL || StrOp.len(toBlockId) == 0) ) {
      iOStrTok blocks = StrTokOp.inst( wLocation.getblocks( LocationOp.base.properties(location) ), ',' );
      const char* id = NULL;
      while( StrTokOp.hasMoreTokens( blocks ) ) {
        id = StrTokOp.nextToken( blocks );
        if( stlist != NULL )
          ListOp.clear( stlist );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Try to find a route to block \"%s\".", id );
        street = __lookup( inst, loc, stlist, currBlockId, id, 0, NULL, NULL, forceSameDir, swapPlacingInPrevRoute );
        if( street == NULL || !RouteOp.isFree( street, LocOp.getId(loc) )) {
          continue;
        }
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Got a route to block \"%s\".", id );
        block = (iIBlockBase)MapOp.get( data->blockMap, id );
        if( block->isFree( block, LocOp.getId( loc ) ) ) {
          /* OK, first free block. */
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Block \"%s\" is free.", id );
          break;
        }
      }
      StrTokOp.base.del( blocks );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Try to find a route to block \"%s\".", toBlockId );
      street = __lookup( inst, loc, stlist, currBlockId, toBlockId, 0, NULL, NULL, forceSameDir, swapPlacingInPrevRoute );
    }

    /* check if the direction is the same if wanted to be */
    if( street != NULL && forceSameDir ) {
      Boolean fromTo = False;
      Boolean locdir  = LocOp.getDir( loc );
      destdir = RouteOp.getDirection( street, currBlockId, &fromTo );
      /* TODO: for second next block the "swapPlacingInPrevRoute" must be ignored? */
      samedir = ( ( swapPlacingInPrevRoute ? !locdir : locdir ) == destdir ? True : False);
      if( ! samedir ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "route[%s] found but it is not in the same direction: reject.", RouteOp.getId(street) );
        street = NULL;
      }
    }


  }
  /* Unlock the semaphore: */
  MutexOp.post( data->muxFindDest );

  if( street != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "The found way has %d routes.", stlist==NULL ? 1:ListOp.size( stlist ) );
  }
  return street;
}

/* Checks the linklist if this block belongs to a group */
static const char* _checkForBlockGroup(iOModel inst, const char* BlockId) {
  iOModelData data = Data(inst);
  iONode linklist = wPlan.getlinklist(data->model);

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "check blockgroup for [%s]", BlockId);

  if( linklist != NULL ) {
    iONode link = wLinkList.getlink(linklist);
    while( link != NULL ) {
      iOStrTok tok = StrTokOp.inst( wLink.getdst(link), ',' );
      while( StrTokOp.hasMoreTokens(tok) ) {
        const char* id = StrTokOp.nextToken( tok );
        if( id != NULL && StrOp.equals(id, BlockId) && wLink.getusage(link) == wLink.usage_critsect ) {
          return wLink.getid(link);
        }
      };
      link = wLinkList.nextlink(linklist, link);
      StrTokOp.base.del(tok);
    };
  }
  return NULL;
}

static Boolean _isBlockGroupLockedForLoco(iOModel inst, const char* BlockId, const char* LocoId) {
  iOModelData data = Data(inst);

  Boolean locked = False;
  const char* group = ModelOp.checkForBlockGroup(inst, BlockId);
  if( group != NULL ) {
    iOBlockGroup bg = (iOBlockGroup)MapOp.get( data->blockGroupMap, group );

    if( bg != NULL ) {
      locked = BlockGroupOp.isLockedForLoco(bg, LocoId );
    }
  }
  return locked;
}


static Boolean _lockBlockGroup(iOModel inst, const char* group, const char* BlockId, const char* LocoId) {
  iOModelData data = Data(inst);
  iOBlockGroup bg = (iOBlockGroup)MapOp.get( data->blockGroupMap, group );

  if( bg != NULL ) {
    return BlockGroupOp.lock(bg, BlockId, LocoId );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "blockgroup %s not defined", group);
    return False;
  }

}

static Boolean _unlockBlockGroup(iOModel inst, const char* group, const char* LocoId) {
  iOModelData data = Data(inst);

  if( group != NULL ) {
    iOBlockGroup bg = (iOBlockGroup)MapOp.get( data->blockGroupMap, group );

    if( bg != NULL ) {
      return BlockGroupOp.unlock(bg, LocoId );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "blockgroup %s not defined", group);
      return False;
    }
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "blockgroup ID is not set for unlock");
  return True;
}




static const char* _getManagedID(iOModel inst, const char* fromBlockId) {
  /* check if the block is managed by a selectioin table */
  iIBlockBase block = ModelOp.getBlock(inst, fromBlockId);
  if( block != NULL && block->getManager(block) != NULL ) {
    iIBlockBase seltab = block->getManager(block);
    return seltab->base.id(seltab);
  }
  return fromBlockId;
}

/* synchronized!!! */
static iIBlockBase _findDest( iOModel inst, const char* fromBlockId, const char* fromRouteId, iOLoc loc,
                          iORoute* routeref, const char* gotoBlockId,
                          Boolean trysamedir, Boolean tryoppositedir, Boolean forceSameDir,
                          Boolean swapPlacingInPrevRoute, Boolean forceOppDir) {
  iOModelData o = Data(inst);

  int size = ListOp.size( o->routeList );

  iIBlockBase   blockBest = NULL;
  iIBlockBase   blockAlt  = NULL;
  iORoute       routeBest = NULL;
  iORoute       routeAlt  = NULL;
  iOList        fitBlocks  = ListOp.inst();
  iOList        fitRoutes  = ListOp.inst();
  int           fitRestLen[size];
  iOList        altBlocks  = ListOp.inst();
  iOList        altRoutes  = ListOp.inst();
  int           altRestLen[size];
  iOMap         swapRoutes = MapOp.inst();

  /* try to find a block in the same direction of the train */
  Boolean locdir  = LocOp.getDir( loc );
  Boolean usemanualroutes  = wLoc.isusemanualroutes(LocOp.base.properties( loc ));
  Boolean stEnterSide = False; /* the To side of the from route */
  Boolean stExitSide  = True;  /* the From side of the new to route */

  /* The use blockside option works only with one way type, so both directions will fail. */
  Boolean useBlockSide     = wCtrl.isuseblockside( wRocRail.getctrl( AppOp.getIni(  ) ) );
  Boolean selectShortest   = wCtrl.isselectshortestblock( wRocRail.getctrl( AppOp.getIni(  ) ) );

  Boolean destdir = False;
  Boolean samedir = False;

  iIBlockBase fromBlock = ModelOp.getBlock( inst, fromBlockId );

  if( fromRouteId != NULL ) {
    iORoute fromRoute = ModelOp.getRoute( inst, fromRouteId );
    if( fromRoute != NULL ) {
      if( StrOp.equals(fromBlockId, RouteOp.getFromBlock(fromRoute) ) )
        stEnterSide = wRoute.isbkaside(fromRoute->base.properties(fromRoute));
      else
        stEnterSide = wRoute.isbkbside(fromRoute->base.properties(fromRoute));

      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block enter side is [%s]", stEnterSide?"+":"-" );
    }
  }
  else {
    /* initial startup; use the saved ahead side */
    if( fromBlock != NULL )
      stEnterSide = LocOp.getBlockEnterSide(loc);
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "using saved loco enter block side [%s]", stEnterSide?"+":"-" );
  }

  fromBlockId = ModelOp.getManagedID(inst, fromBlockId);

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                 "findDest fromBlockID [%s]", fromBlockId );

  /* Lock the semaphore: */
  MutexOp.wait( o->muxFindDest );
  {
    /* Iterate all streets for destinations: */
    int i = 0;
    Boolean allowChgDir = True;

    if( fromBlock != NULL ) {
      allowChgDir = wBlock.isallowchgdir( fromBlock->base.properties(fromBlock) );
      if( !allowChgDir ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                       "block [%s] does NOT allow a direction change", fromBlockId );
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "from block [%s] is not known in the model!!!", fromBlockId );
    }


    /* make a list of all possible and free routes, use rand()%count to pick one.
     * srand is already set with the seed at init. */

    for( i = 0; i < size; i++ ) {
      iORoute route = (iORoute)ListOp.get( o->routeList, i );

      if( route != NULL )
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Evaluating route [%s].", RouteOp.getId(route));

      if( route != NULL ) {
        Boolean fromTo = True;
        Boolean isset = False;
        Boolean ismanual = RouteOp.isManual( route, &isset );
        Boolean isfree = RouteOp.isFree( route, LocOp.getId(loc) );
        const char* stFrom = RouteOp.getFromBlock( route );
        const char* stTo = RouteOp.getToBlock( route );
        Boolean swap4BlockSide = False;

        stExitSide = wRoute.isbkaside(route->base.properties(route));
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "block exit side is [%s]", stExitSide?"+":"-" );

        if( usemanualroutes ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Loco must use manual routes.");
          if( !ismanual ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Skip none manual route [%s].", RouteOp.getId(route));
            continue;
          }
          else if( ismanual && !isset ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Skip manual route [%s] because it is not set free to use.", RouteOp.getId(route));
            continue;
          }
        }

        if( !isfree ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                "ignoring route [%s] because it is not free",
                RouteOp.getId(route) );
          continue;
        }

        if( !RouteOp.getDir( route ) ) {
          /* route is useable for both directions */
          if( StrOp.equals( fromBlockId, stTo ) ) {
            stFrom = RouteOp.getToBlock( route );
            stTo = RouteOp.getFromBlock( route );
            stExitSide = wRoute.isbkbside(route->base.properties(route));
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "<-> route [%s] enter side [%s], exit side [%s]",
                RouteOp.getId(route), stEnterSide?"+":"-", stExitSide?"+":"-");
          }
        }

        if( useBlockSide && stEnterSide == stExitSide ) {
          /* need to change direction but blocksides are used, check if allowed*/
          if( LocOp.getV(loc) == 0 &&  (fromBlock->isTTBlock(fromBlock) || wLoc.iscommuter( LocOp.base.properties(loc) ) ) ) {
            if( fromBlock->isTTBlock(fromBlock) ) {
              /* for turntable allow routes with swap still as best when suited well, do not set swap4blockside in that case (REB)*/
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                "allow route [%s] from a turntable: the exit side is equal to the enter side [%s]. Swap needed.",
                RouteOp.getId(route), stEnterSide?"+":"-" );
            }
            else {
              /* commuter: allow, set swap4BlockSide flag so that well suited routes are put in the alt list and not in the best list. (REB)*/
              swap4BlockSide = True;
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                "allow route [%s] for a commuter train: the exit side is equal to the enter side [%s]. Swap needed.",
                RouteOp.getId(route), stEnterSide?"+":"-" );
            }
            MapOp.put( swapRoutes, route->base.id(route), (obj)route );
          }
          else {
            /* other case, do not allow */
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                "ignoring route [%s] because the exit side is equal to the enter side [%s]",
                RouteOp.getId(route), stEnterSide?"+":"-" );
            continue;
          }
        }



        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                       "[%d] FromBlock [%s] ToBlock [%s]", i, stFrom, stTo );

        destdir = RouteOp.getDirection( route, fromBlockId, &fromTo );
        samedir = ( ( swapPlacingInPrevRoute ? !locdir : locdir ) == destdir ? True : False);

        if( useBlockSide && swap4BlockSide ) {
          samedir = False;
        }

        /* Must match the fromBlock: */
        if( R2RnetOp.compare( fromBlockId, stFrom ) )
        {
          iIBlockBase block = (iIBlockBase)MapOp.get( o->blockMap, stTo );

          /* check if it is a net block */
          if( block == NULL && StrOp.find( stTo, "::" ) != NULL ) {
            iOR2Rnet r2rnet = ControlOp.getR2Rnet(AppOp.getControl());
            if( r2rnet != NULL ) {
              iONode bk = R2RnetOp.getBlock( r2rnet, stTo );
              if( bk != NULL ) {
                block = (iIBlockBase)BlockOp.inst(bk);
                MapOp.put( o->blockMap, stTo, (obj)block);
              }
            }
          }

          /* regards selection tables too
           * use iIBlockBase for block and seltab objects
           */
          if( block == NULL ) {
            /* id could be a seltab: */
            block = (iIBlockBase)MapOp.get( o->seltabMap, stTo );
          }
          if( block == NULL ) {
            /* id could be a tt: */
            block = (iIBlockBase)MapOp.get( o->ttMap, stTo );
          }
          if( block == NULL ) {
            /* id could be a stage block: */
            block = (iIBlockBase)MapOp.get( o->stageMap, stTo );
          }

          if( block != NULL ) {
            const char* blockId = block->base.id( block );
            /* Is it free? Does it fit? */
            if( block->isFree( block, LocOp.getId( loc ) ) ) {
              block_suits suits;
              int restlen = 0;

              /* Check for wanted block: */
              if( gotoBlockId != NULL && StrOp.equals( gotoBlockId, blockId ) ) {
                TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                               "found the GOTO block [%s] for [%s]", gotoBlockId, LocOp.getId( loc ) );
                blockBest = block;
                routeBest = route;
                /* ignore all other found fitting blocks */
                ListOp.clear(fitBlocks);
                ListOp.clear(fitRoutes);
                /* add the goto block as the one and only */
                ListOp.add( fitBlocks, (obj)block );
                ListOp.add( fitRoutes, (obj)route );
                break;
              }

              suits = block->isSuited( block, loc, &restlen, !selectShortest );
              if( !route->hasPermission( route, loc, fromBlockId, !samedir ) ) {
                suits = suits_not;
              }

              /*
               * check if it realy suits well:
               */
              if( suits == suits_well ) {
                Boolean dirOK = True;
                /* using blockside, in case of a commuter changing direction making sure that it is an alternative route */
                if( (!samedir && !allowChgDir)  || (swap4BlockSide && useBlockSide) || (forceOppDir && samedir))
                  dirOK = False;

                if( dirOK && (!trysamedir && !forceSameDir && !tryoppositedir) ) {
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "found a BEST block [%s] for [%s]", blockId, LocOp.getId( loc ) );
                  blockBest = block;
                  routeBest = route;
                  ListOp.add( fitBlocks, (obj)block );
                  ListOp.add( fitRoutes, (obj)route );
                  fitRestLen[ListOp.size(fitBlocks)-1] = restlen;
                }
                else if( (dirOK && ( trysamedir || forceSameDir) && samedir) || (dirOK && tryoppositedir && !destdir) ) {
                  /* direction flags fits */
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                                 "found a BEST block [%s] for [%s] in the wanted direction",
                                 blockId, LocOp.getId( loc ) );
                  blockBest = block;
                  routeBest = route;
                  ListOp.add( fitBlocks, (obj)block );
                  ListOp.add( fitRoutes, (obj)route );
                  fitRestLen[ListOp.size(fitBlocks)-1] = restlen;
                }
                else if( !forceSameDir && allowChgDir ) {
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "found a block [%s] for [%s] in the other direction", blockId, LocOp.getId( loc ) );
                  blockAlt = block;
                  routeAlt = route;
                  ListOp.add( altBlocks, (obj)block );
                  ListOp.add( altRoutes, (obj)route );
                  altRestLen[ListOp.size(altBlocks)-1] = restlen;
                }
                else if( forceSameDir ) {
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                                 "block [%s] for [%s] is in the wrong direction (force same direction)",
                                 blockId, LocOp.getId( loc ) );
                }
              }
              else if( suits == suits_ok ) {
                Boolean dirOK = True;
                if( (!samedir && !allowChgDir) || (forceOppDir && samedir) )
                  dirOK = False;

                TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                    "dirOK=%d locdir=%d destdir=%d samedir=%d allowChgDir=%d trysamedir=%d tryoppositedir=%d forceSameDir=%d swapPlacingInPrevRoute=%d",
                     dirOK,   locdir,   destdir,   samedir,   allowChgDir,   trysamedir,   tryoppositedir,   forceSameDir,   swapPlacingInPrevRoute);

                if( blockBest == NULL ) {
                  if( (dirOK && ( trysamedir | forceSameDir | useBlockSide ) && samedir) || (dirOK && tryoppositedir && !samedir) ) {
                    /* direction flags fits */
                    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                                   "found an ALT block [%s] for [%s] in the wanted direction",
                                   blockId, LocOp.getId( loc ) );
                    blockAlt = block;
                    routeAlt = route;
                    ListOp.add( altBlocks, (obj)block );
                    ListOp.add( altRoutes, (obj)route );
                    altRestLen[ListOp.size(altBlocks)-1] = restlen;
                  }
                  else if( dirOK && !forceSameDir ) {
                    /* wrong direction alternative */
                    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                                   "found an ALT block [%s] for [%s] in a permitted direction",
                                   blockId, LocOp.getId( loc ) );
                    blockAlt = block;
                    routeAlt = route;
                    ListOp.add( altBlocks, (obj)block );
                    ListOp.add( altRoutes, (obj)route );
                    altRestLen[ListOp.size(altBlocks)-1] = restlen;
                    MapOp.put( swapRoutes, route->base.id(route), (obj)route );
                  }
                  else {
                    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                                   "block [%s] for [%s] does not fit",
                                   blockId, LocOp.getId( loc ) );
                  }
                }
                else if( ( dirOK && forceSameDir && samedir ) || ( dirOK && !forceSameDir ) ) {
                  /* normal case alternative */
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                                 "found an ALT block [%s] for [%s]",
                                 blockId, LocOp.getId( loc ) );
                  blockAlt = block;
                  routeAlt = route;
                  ListOp.add( altBlocks, (obj)block );
                  ListOp.add( altRoutes, (obj)route );
                  altRestLen[ListOp.size(altBlocks)-1] = restlen;
                }
                else {
                  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                                 "block [%s] for [%s] does not fit",
                                 blockId, LocOp.getId( loc ) );
                }
              }
              else {
                TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                               "block [%s] for [%s] does not fit",
                               blockId, LocOp.getId( loc ) );
              }

            }
            else {
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                             "Block \"%s\" is not free for \"%s\"",
                             blockId, LocOp.getId( loc ) );
            }
          }
          else {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                           "Block [%s] not found", stTo );
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                         "from block %s is not equal to route from %s", fromBlockId, stFrom );
        }

      }
    }
  }



  if( ListOp.size( fitBlocks ) > 0 ) {
    int cnt = ListOp.size( fitBlocks );
    if( cnt == 1 ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Block [%s] is well suited for [%s]",
                     blockBest->base.id(blockBest), LocOp.getId( loc ) );
      *routeref = routeBest;
    }
    else if( selectShortest ) {
      int i = 0;
      int shortest = fitRestLen[0];
      blockBest = (iIBlockBase)ListOp.get( fitBlocks, 0 );
      *routeref = (iORoute)ListOp.get( fitRoutes, 0 );
      for( i = 1; i < cnt; i++ ) {
        if( fitRestLen[i] < shortest ) {
          shortest = fitRestLen[i];
          blockBest = (iIBlockBase)ListOp.get( fitBlocks, i );
          *routeref = (iORoute)ListOp.get( fitRoutes, i );
        }
      }
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Block [%s] is shortest, restlen=%d,  of the well suited for [%s]",
                     blockBest->base.id(blockBest), shortest, LocOp.getId( loc ) );
    }
    else {
      int randNumber = rand();
      int randChoice = randNumber % cnt;
      blockBest = (iIBlockBase)ListOp.get( fitBlocks, randChoice );
      *routeref = (iORoute)ListOp.get( fitRoutes, randChoice );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Block [%s] is well suited for [%s] and picked random[%d,%d] from [%d] choices",
                     blockBest->base.id(blockBest), LocOp.getId( loc ), randChoice, randNumber, cnt );
    }
    /* when using blocksides the best route can be, in case of commuter,
       a destination in the other direction. For a commuter to change direction
       the block must allow change direction and the loc must be swapped. */
    if( useBlockSide &&
        wBlock.isallowchgdir( fromBlock->base.properties(fromBlock) ) &&
        MapOp.haskey( swapRoutes, (*routeref)->base.id(*routeref) ) )
    {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Loco [%s] must swap for this route.",
                     LocOp.getId( loc ) );
      LocOp.swapPlacing(loc, NULL, False);
    }
  }
  else if( ListOp.size( altBlocks ) > 0 ) {
    int cnt = ListOp.size( altBlocks );

    if( cnt == 1 ) {
      blockBest = blockAlt;
      *routeref = routeAlt;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Block [%s] is suited for [%s]",
                     blockBest->base.id(blockBest), LocOp.getId( loc ) );
    }
    else if( selectShortest ) {
      int i = 0;
      int shortest = altRestLen[0];
      blockBest = (iIBlockBase)ListOp.get( altBlocks, 0 );
      *routeref = (iORoute)ListOp.get( altRoutes, 0 );
      for( i = 1; i < cnt; i++ ) {
        if( altRestLen[i] < shortest ) {
          shortest = altRestLen[i];
          blockBest = (iIBlockBase)ListOp.get( altBlocks, i );
          *routeref = (iORoute)ListOp.get( altRoutes, i );
        }
      }
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Block [%s] is shortest, restlen=%d, of the well suited for [%s]",
                     blockBest->base.id(blockBest), shortest, LocOp.getId( loc ) );
    }
    else {
      int randNumber = rand();
      int randChoice = randNumber % cnt;
      blockBest = (iIBlockBase)ListOp.get( altBlocks, randChoice );
      *routeref = (iORoute)ListOp.get( altRoutes, randChoice );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Block [%s] is suited for [%s] and picked random[%d,%d] from [%d] choices",
                     blockBest->base.id(blockBest), LocOp.getId( loc ), randChoice, randNumber, cnt );
    }

    /* when using blocksides the alternative route can be a mismatch between properties or, in case of commuter,
       a destination in the other direction. For a commuter to change direction the block must allow change direction and
       the loc must be swapped. In case of a mismatch the loc must not be swapped */
    if( useBlockSide &&
        wBlock.isallowchgdir( fromBlock->base.properties(fromBlock) ) &&
        MapOp.haskey( swapRoutes, (*routeref)->base.id(*routeref) ) )
    {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
                     "Loco [%s] must swap for this route.",
                     LocOp.getId( loc ) );
      LocOp.swapPlacing(loc, NULL, False);
    }

  } else {
    *routeref = routeBest;
  }

  /* Cleanup: */
  ListOp.base.del( fitBlocks );
  ListOp.base.del( fitRoutes );
  ListOp.base.del( altBlocks );
  ListOp.base.del( altRoutes );
  MapOp.base.del(swapRoutes);

  /* Unlock the semaphore: */
  MutexOp.post( o->muxFindDest );

  /* TODO: return the iIBlockBase interface; could be a FY */
  return blockBest;
}

static void __printObjects2Stream( iOMap map, const char* title, FILE* f ) {
  iIHtmlInt o = (iIHtmlInt)MapOp.first( map );
  fprintf( f, "<html><head>\n" );
  {
    char* stamp = StrOp.createStamp();
    fprintf( f, "<title>RocRail %s %s</title>\n", title, stamp );
    StrOp.free( stamp );
  }
  fprintf( f, "<STYLE type=\"text/css\">\n" );
  fprintf( f, "<!--" );
  fprintf( f, "A { text-decoration:none }\n" );
  fprintf( f, "a:link { text-decoration:none; color:#000000; }\n" );
  fprintf( f, "a:visited { text-decoration:none; color:#000000; }\n" );
  fprintf( f, "-->\n" );
  fprintf( f, "</STYLE>\n" );

  fprintf( f, "</head><body>\n" );
  fprintf( f, "<table rules=\"all\" border=\"1\" cellspacing=\"0\" cellpadding=\"4\">\n" );
  fprintf( f, "%s", o->tableHdr() );
  while( o != NULL ) {
    char* htmlStr = o->toHtml( o );
    fprintf( f, "%s", htmlStr );
    StrOp.free( htmlStr );
    o = (iIHtmlInt)MapOp.next( map );
  }
  fprintf( f, "</table>\n" );
  fprintf( f, "</body>\n" );
  fprintf( f, "</html>\n" );
}

static void _printLocs( iOModel inst, FILE* f ) {
  iOModelData data = Data(inst);
  FILE* fstr = f;
  if( f == NULL )
    fstr = fopen( "locs.html", "w" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Printing LocList" );
  __printObjects2Stream( data->locMap, "LocList", fstr );
  if( f == NULL )
    fclose( fstr );
}

static void _printRoutes( iOModel inst, FILE* f  ) {
  iOModelData data = Data(inst);
  FILE* fstr = f;
  if( f == NULL )
    fstr = fopen( "streets.html", "w" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Printing StreetList" );
  __printObjects2Stream( data->routeMap, "StreetList", fstr );
  if( f == NULL )
    fclose( fstr );
}

static void _printBlocks( iOModel inst, FILE* f ) {
  iOModelData data = Data(inst);
  FILE* fstr = f;
  if( f == NULL )
    fstr = fopen( "blocks.html", "w" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Printing BlockList" );
  __printObjects2Stream( data->blockMap, "BlockList", fstr );
  if( f == NULL )
    fclose( fstr );
}

static void _printFBacks( iOModel inst, FILE* f ) {
  iOModelData data = Data(inst);
  FILE* fstr = f;
  if( f == NULL )
    fstr = fopen( "fbacks.html", "w" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Printing FeedbackList" );
  __printObjects2Stream( data->feedbackMap, "FeedbackList", fstr );
  if( f == NULL )
    fclose( fstr );
}

static void _printSwitches( iOModel inst, FILE* f ) {
  iOModelData data = Data(inst);
  FILE* fstr = f;
  if( f == NULL )
    fstr = fopen( "switches.html", "w" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Printing SwitchList" );
  __printObjects2Stream( data->switchMap, "SwitchList", fstr );
  if( f == NULL )
    fclose( fstr );
}

static void _printSignals( iOModel inst, FILE* f ) {
  iOModelData data = Data(inst);
  FILE* fstr = f;
  if( f == NULL )
    fstr = fopen( "signals.html", "w" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Printing SignalList" );
  __printObjects2Stream( data->signalMap, "SignalList", fstr );
  if( f == NULL )
    fclose( fstr );
}

static void _printOutputs( iOModel inst, FILE* f ) {
  iOModelData data = Data(inst);
  FILE* fstr = f;
  if( f == NULL )
    fstr = fopen( "outputs.html", "w" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Printing OutputList" );
  __printObjects2Stream( data->outputMap, "OutputList", fstr );
  if( f == NULL )
    fclose( fstr );
}

static void _stop( iOModel inst ) {
  iOModelData data = Data(inst);
  AppOp.stop();
}


static void _setBlockOccupancy( iOModel inst, const char* BlockId, const char* LocId, Boolean closed, int placing, int enterside, const char* SectionId ) {
  iOModelData data = Data(inst);
  iONode occ = NULL;
  char key[256] = {'\0'};

  /* Lock the semaphore: */
  MutexOp.wait( data->occMux );

  StrOp.fmtb( key, "%s%s", BlockId, SectionId!=NULL ? SectionId:"");

  /* get the node */
  occ = (iONode)MapOp.get( data->occMap, key );

  if( occ == NULL ) {
    /* create a node */
    occ = NodeOp.inst( wOccupancy.name(), NULL, ELEMENT_NODE );
    wOccupancy.setbkid( occ, BlockId );
    MapOp.put( data->occMap, key, (obj)occ );
  }

  /* modify the node */
  wOccupancy.setlcid( occ, LocId );
  wOccupancy.setauto( occ, False );
  wOccupancy.setclosed( occ, closed );

  if( SectionId != NULL ) {
    wOccupancy.setsecid( occ, SectionId );
  }

  if( LocId != NULL && StrOp.len(LocId) > 0 ) {
    iOLoc loc = ModelOp.getLoc( AppOp.getModel(), LocId );
    if( loc != NULL ) {
      wOccupancy.setauto( occ, LocOp.isResumeAutomode(loc) );
      wOccupancy.setscid( occ, LocOp.getSchedule(loc, NULL) );
    }
  }
  else {
    wOccupancy.setauto( occ, False );
    wOccupancy.setscid( occ, NULL );
  }

  if( placing > 0 ) {
    /* 0 = Not set, 1 = True, 2 = False*/
    wOccupancy.setplacing( occ, placing );
  }

  if( enterside > 0 ) {
    /* 0 = Not set, 1 = True, 2 = False*/
    wOccupancy.setblockenterside( occ, enterside );
  }

  if( LocId == NULL || StrOp.len(LocId) == 0 ) {
    wOccupancy.setplacing( occ, 0 );
  }

  /* Unlock the semaphore: */
  MutexOp.post( data->occMux );
}


static void _saveBlockOccupancy( iOModel inst, const char* occfilename ) {
  iOModelData data = Data(inst);
  iONode modocc = NodeOp.inst( wModOcc.name(), NULL, ELEMENT_NODE );
  iONode occ = NULL;

  /* Lock the semaphore: */
  MutexOp.wait( data->occMux );

  /* get the node */
  occ = (iONode)MapOp.first( data->occMap );

  while( occ != NULL ) {
    NodeOp.addChild( modocc, occ );
    occ = (iONode)MapOp.next( data->occMap );
  }

  /* save */
  {
    char* modoccStr = NodeOp.base.toString( modocc );
    iOFile f = NULL;

    /* file name */

    const char* occFileName =  (occfilename==NULL ? wRocRail.getoccupancy( AppOp.getIni() ):occfilename);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "writing occupancy file [%s]", occFileName );

    f = FileOp.inst( occFileName, OPEN_WRITE );
    if( f != NULL ) {
      FileOp.write( f, modoccStr, StrOp.len(modoccStr) );
      FileOp.close( f );
      FileOp.base.del( f );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unable to write occupancy file [%s]", occFileName );
    }
  }

  /* Unlock the semaphore: */
  MutexOp.post( data->occMux );
}


static void _loadBlockOccupancy( iOModel inst ) {
  iOModelData data = Data(inst);
  iONode modocc = NULL;
  iONode occ = NULL;

  /* Lock the semaphore: */
  MutexOp.wait( data->occMux );

  /* load */
  {
    iODoc modoccDoc = NULL;
    iOFile f = NULL;

    /* file name */
    const char* occFileName =  wRocRail.getoccupancy( AppOp.getIni() );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loading occupancy file [%s]", occFileName );

    f = FileOp.inst( occFileName, OPEN_READONLY );
    if( f != NULL ) {
      long fsize = FileOp.size(f);
      char* modoccStr = allocMem( fsize );
      FileOp.read( f, modoccStr, fsize );
      FileOp.close( f );
      FileOp.base.del( f );
      modoccDoc = DocOp.parse( modoccStr );
      if( modoccDoc != NULL ) {
        modocc = DocOp.getRootNode( modoccDoc );
        DocOp.base.del( modoccDoc );
      }
    }
  }

  if( modocc != NULL ) {
    int cnt = NodeOp.getChildCnt( modocc );
    int i = 0;
    for( i = 0; i < cnt; i++ ) {
      iONode occ = NodeOp.getChild( modocc, i );
      const char* BlockID  = wOccupancy.getbkid( occ );
      const char* Section  = wOccupancy.getsecid( occ );
      const char* LocoID   = wOccupancy.getlcid( occ );
      const char* ScID     = wOccupancy.getscid( occ );
      int         placing  = wOccupancy.getplacing( occ );
      int         enterside= wOccupancy.getblockenterside( occ );
      Boolean     closed   = wOccupancy.isclosed( occ );
      Boolean     automode = wOccupancy.isauto( occ );
      iOLoc       loco     = NULL;
      char        key[256] = {'\0'};

      if( LocoID != NULL && StrOp.len(LocoID) > 0 ) {
        loco = ModelOp.getLoc( inst, LocoID );
      }

      StrOp.fmtb( key, "%s%s", BlockID, Section!=NULL ? Section:"");
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "occupancy key [%s]", key );
      MapOp.put( data->occMap, key, NodeOp.base.clone( occ ) );

      /* inform loco of placing flag */
      if( loco != NULL ) {
        iONode props = LocOp.base.properties(loco);
        wLoc.setresumeauto( props, automode );

        if( wLoc.isshow(props) ) {
          Boolean restoreSc = wCtrl.isrestoreschedule( wRocRail.getctrl( AppOp.getIni(  ) ) );

          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "restore loco placing for [%s]", LocOp.getId(loco));

          if( restoreSc && ScID != NULL && StrOp.len(ScID) > 0) {
            LocOp.useSchedule(loco, ScID);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "restore scheduleID [%s] for [%s]",
                ScID, LocOp.getId(loco));
          }

          if( enterside > 0 ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set enterside to [%s] for [%s]",
                placing == 1 ?"plus":"min", LocOp.getId(loco) );
            wLoc.setblockenterside( props, enterside == 1 ? True:False );
          }
          if( placing > 0 ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set placing to [%s] for [%s]",
                placing == 1 ?"default":"reverse", LocOp.getId(loco) );
            wLoc.setplacing( props, placing == 1 ? True:False );
          }
        }
      }
      else if( StrOp.len(LocoID) > 0 ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "loco [%s] not found", LocoID );
      }

      /* inform block */
      {
        iIBlockBase block = ModelOp.getBlock( inst, BlockID );
        if( block != NULL ) {
          iONode props = block->base.properties(block);
          iOLocation location = ModelOp.getBlockLocation( inst, BlockID);
          /* quick and dirty solution to fix the problem of being locked by something strange...
             the block expects a const char*, but the parsed xml is freed up after setting the
             occupancy so all LocoID's are invalid.
          */
          /* stageblock sections do not store real open/closed data for the state of the stageblock
             set state only for "real" (stage-)block (Section is "")
          */
          if( Section != NULL && StrOp.len( Section ) > 0 ) {
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "skip state setting block [%s] section[%s]", BlockID, Section);
          }
          else {
            wBlock.setstate( props, closed?wBlock.closed:wBlock.open);
          }

          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "block [%s]", BlockID );

          if( loco != NULL ) {
            iONode lcprops = LocOp.base.properties(loco);
            if( wLoc.isshow(lcprops) ) {
              if( Section != NULL && StrOp.len( Section ) > 0 && StrOp.equals( block->base.name(), StageOp.base.name()) ) {
                TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco [%s], section [%s]", LocoID, Section );
                StageOp.setSectionOcc((iOStage)block, Section, LocoID);
              }
              else if( !StrOp.equals( block->base.name(), StageOp.base.name()) )
                wBlock.setlocid( props, StrOp.dup( LocoID ) );
            }
          }
          else if( wBlock.getlocid(props) != NULL && StrOp.len(wBlock.getlocid(props)) > 0 ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reset block [%s] occupancy", BlockID );
            wBlock.setlocid( props, "" );
          }

          if( location != NULL && loco != NULL ) {
            LocationOp.locoDidArrive(location, LocOp.getId(loco));
          }

        }
        else {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "block [%s] not found", BlockID );
        }

      }

    }
    NodeOp.base.del(modocc);
  }


  /* Unlock the semaphore: */
  MutexOp.post( data->occMux );
}

static iOList _getRouteAliases(iOModel inst, const char* routeId) {
  iOModelData data = Data(inst);
  iOList aliases = NULL;
  if( data->moduleplan != NULL ) {
    aliases = ModPlanOp.getModuleRouteIDs(data->moduleplan, routeId);
    if( aliases != NULL ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%d route alias found for [%s]", ListOp.size(aliases), routeId );
    }
  }

  return aliases;
}


static void _addSysEventListener(iOModel inst, obj listener) {
  iOModelData data = Data(inst);
  ListOp.add( data->sysEventListeners, listener );
}


static void _removeSysEventListener(iOModel inst, obj listener) {
  iOModelData data = Data(inst);
  ListOp.removeObj( data->sysEventListeners, listener );
}


static void _forceUnlock(iOModel inst) {
  iOModelData data = Data(inst);
  int size = ListOp.size(data->routeList);
  int i = 0;
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Force unlock %d routes...", size);

  for( i = 0; i < size; i++ ) {
    iORoute route = (iORoute)ListOp.get(data->routeList, i);
    if( route != NULL )
      RouteOp.unLock(route, "*forceUnlock*", NULL, True, True);
  }
  size = ListOp.size(data->switchList);
  TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Force unlock %d switches...", size);
  for( i = 0; i < size; i++ ) {
    iOSwitch sw = (iOSwitch)ListOp.get(data->switchList, i);
    if( sw != NULL )
      SwitchOp.unLock(sw, "*forceUnlock*", NULL, True);
  }
}


static iOModel _inst( const char* fileName ) {
  iOModel     model = allocMem( sizeof( struct OModel ) );
  iOModelData data  = allocMem( sizeof( struct OModelData ) );

  /* OBase operations */
  MemOp.basecpy( model, &ModelOp, 0, sizeof( struct OModel ), data );

  /* Init fbAddresses: */
  MemOp.set( data->fbAddresses, 0, 256 );

  data->fileName = fileName;

  data->locMap      = MapOp.inst();
  data->masterLocMap = MapOp.inst();
  data->carMap      = MapOp.inst();
  data->waybillMap  = MapOp.inst();
  data->operatorMap = MapOp.inst();
  data->switchMap   = MapOp.inst();
  data->switchList  = ListOp.inst();
  data->signalMap   = MapOp.inst();
  data->outputMap   = MapOp.inst();
  data->feedbackMap = MapOp.inst();
  data->blockMap    = MapOp.inst();
  data->blockGroupMap= MapOp.inst();
  data->routeMap    = MapOp.inst();
  data->routeList   = ListOp.inst();
  data->trackMap    = MapOp.inst();
  data->ttMap       = MapOp.inst();
  data->seltabMap   = MapOp.inst();
  data->stageMap    = MapOp.inst();
  data->actionMap   = MapOp.inst();
  data->textMap     = MapOp.inst();
  data->locationMap = MapOp.inst();
  data->scheduleMap = MapOp.inst();
  data->tourMap = MapOp.inst();

  data->fbAddrMap   = MapOp.inst();
  data->swAddrMap   = MapOp.inst();
  data->coAddrMap   = MapOp.inst();

  data->levelItemsMap = MapOp.inst();

  data->sysEventListeners = ListOp.inst();

  data->muxFindDest = MutexOp.inst( "muxFindDest", True );

  data->muxSysEvent = MutexOp.inst( "muxSysEvent", True );

  /* occupancy map */
  data->occMap      = MapOp.inst();
  data->occMux      = MutexOp.inst( NULL, True );

  data->check2in   = wCtrl.ischeck2in( wRocRail.getctrl( AppOp.getIni(  ) ) );
  data->enableswfb = wCtrl.isenableswfb( wRocRail.getctrl( AppOp.getIni(  ) ) );

  /* Initialize random seed. */
  srand( wCtrl.getseed( wRocRail.getctrl( AppOp.getIni() ) ) );

  if( wCtrl.istimedsensors( wRocRail.getctrl( AppOp.getIni() ) ) ) {
    data->timedoff = ThreadOp.inst( "timedoff", &__timedoffRunner, model );
    ThreadOp.start( data->timedoff );
  }

  instCnt++;

  return model;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/model.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
