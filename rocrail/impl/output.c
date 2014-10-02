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
#include "rocrail/impl/output_impl.h"

#include "rocrail/public/app.h"

#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Ctrl.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/thread.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/Action.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Item.h"


static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOOutputData data = Data(inst);
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
  iOOutputData data = Data((iOOutput)inst);
  return data->props;
}

/** ----- OOutput ----- */


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

/**  */
static const char* _getAddrKey( iOOutput inst ) {
  iOOutputData data = Data(inst);
  return data->addrKey;
}

/**  */
static char* _createAddrKey( int bus, int addr, int port, int type, const char* iid ) {
  iONode node = AppOp.getIniNode( wDigInt.name() );
  const char* def_iid = wDigInt.getiid( node );
  return StrOp.fmt( "%d_%d_%d_%d_%s", bus, addr, port, type, (iid != NULL && StrOp.len( iid ) > 0) ? iid:def_iid );
}

/**
 * check and execute actions
 */
static void __checkActions(iOOutput inst, const char* cmd) {
  iOOutputData data     = Data(inst);
  iOModel      model    = AppOp.getModel();
  iONode       coaction = wOutput.getactionctrl(data->props);


  while( coaction != NULL ) {
    if( (StrOp.equals(wOutput.on, wOutput.getstate(data->props)) && StrOp.len( wActionCtrl.getstate(coaction) ) == 0) ||
        StrOp.equals(wActionCtrl.getstate(coaction), wOutput.getstate(data->props) ) )
    {
      iOAction action = ModelOp.getAction(model, wActionCtrl.getid(coaction) );
      if( action != NULL ) {
        ActionOp.exec(action, coaction);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "%s action state does not match: [%s-%s]",
          wOutput.getid(data->props), wActionCtrl.getstate( coaction ), wOutput.getstate(data->props) );
    }


    coaction = wOutput.nextactionctrl(data->props, coaction);
  }

}


/**  */
static Boolean __doCmd( struct OOutput* inst ,iONode nodeA ,Boolean update ) {
  iOOutputData o = Data(inst);
  iOControl control = AppOp.getControl(  );

  const char* state = wOutput.getcmd( nodeA );
  const char* iid = wOutput.getiid( o->props );
  int value = wOutput.getvalue( nodeA );
  Boolean inv = wOutput.isinv(o->props);

  if( StrOp.equals( wOutput.flip, state ) ) {
    if( wOutput.istristate(o->props) ) {
      if( StrOp.equals( wOutput.on, wOutput.getstate( o->props ) ) )
        state = wOutput.off;
      else if( StrOp.equals( wOutput.off, wOutput.getstate( o->props ) ) )
        state = wOutput.active;
      else
        state = wOutput.on;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "flip tristate output %s to %s", wOutput.getid( o->props ), state);
    }
    else if( StrOp.equals( wOutput.on, wOutput.getstate( o->props ) ) )
      state = wOutput.off;
    else
      state = wOutput.on;
  }


  /* remember state */
  if( StrOp.equals(wOutput.active, state ) )
    wOutput.setstate( o->props, wOutput.active );
  else if( StrOp.equals(wOutput.off, state ) )
    wOutput.setstate( o->props, wOutput.off );
  else if( StrOp.equals(wOutput.on, state ) )
    wOutput.setstate( o->props, wOutput.on );
  else if( StrOp.equals(wOutput.value, state ) ) {
    wOutput.setvalue( o->props, value );
    if( value > 0 )
      wOutput.setstate( o->props, wOutput.on );
    else
      wOutput.setstate( o->props, wOutput.off );
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setting output %s to %s [%d]",
               wOutput.getid( o->props ), state, StrOp.equals(wOutput.off, state )?0:wOutput.getvalue(o->props) );

  if( iid != NULL )
    wOutput.setiid( nodeA, iid );

  wOutput.setbus( nodeA, wOutput.getbus( o->props ) );
  wItem.setuidname(nodeA, wItem.getuidname(o->props));

  wOutput.setprot( nodeA, wOutput.getprot( o->props ) );
  wOutput.setblink( nodeA, wOutput.isblink( o->props ) );
  wOutput.setparam( nodeA, wOutput.getparam( o->props ) );
  if( inv )
    wOutput.setvalue( nodeA, StrOp.equals(wOutput.off, state ) ? wOutput.getvalue( o->props ):0 );
  else
    wOutput.setvalue( nodeA, StrOp.equals(wOutput.off, state ) ? 0:wOutput.getvalue( o->props ) );

  if( wOutput.getaddr( o->props ) > 0 || wOutput.getport( o->props ) > 0 ){
    if( wOutput.isasswitch(o->props) ) {
      NodeOp.setName( nodeA, wSwitch.name() );

      wSwitch.setaddr1( nodeA, wOutput.getaddr( o->props ) );
      wSwitch.setport1( nodeA, wOutput.getport( o->props ) );

      wSwitch.setcmd( nodeA, ( StrOp.equals(state, wOutput.on) || StrOp.equals(state, wOutput.active) ) ? (inv?wSwitch.straight:wSwitch.turnout):(inv?wSwitch.turnout:wSwitch.straight) );

    }
    else {
      wOutput.setaddr( nodeA, wOutput.getaddr( o->props ) );
      wOutput.setport( nodeA, wOutput.getport( o->props ) );
      wOutput.setgate( nodeA, wOutput.getgate( o->props ) );
      wOutput.setcmd( nodeA, state );
      wOutput.setcmd( nodeA, ( StrOp.equals(state, wOutput.on) || StrOp.equals(state, wOutput.active) ) ? (inv?wOutput.off:wOutput.on):(inv?wOutput.on:wOutput.off) );
    }

    wOutput.setaccessory( nodeA, wOutput.isaccessory(o->props) );
    wOutput.setporttype( nodeA, wOutput.getporttype( o->props ) );
    if( !ControlOp.cmd( control, nodeA, NULL ) ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Output [%s] could not be set!",
                     wOutput.getid( o->props ) );
      return False;
    }
  }
  else {
    /* no longer needed */
    NodeOp.base.del(nodeA);
  }


  /* Broadcast to clients. Node6 */
  if( update ) {
    iONode nodeF = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
    wOutput.setid( nodeF, wOutput.getid( o->props ) );
    wOutput.setstate( nodeF, wOutput.getstate( o->props ) );
    if( wOutput.getiid( o->props ) != NULL )
      wOutput.setiid( nodeF, wOutput.getiid( o->props ) );
    AppOp.broadcastEvent( nodeF );
  }

  __checkActions(inst, state );

  return True;
}

/* **/
static void _event( iOOutput inst, iONode nodeC ) {
  iOOutputData data = Data(inst);
  Boolean inv = wOutput.isinv(data->props);

  const char* state = wOutput.getstate( nodeC );

  if( StrOp.equals( state, wOutput.on ) )
    wOutput.setstate( data->props, inv?wOutput.off:wOutput.on );
  else if( StrOp.equals( state, wOutput.off ) )
    wOutput.setstate( data->props, inv?wOutput.on:wOutput.off );

  /* Broadcast to clients. Node4 */
  {
    iONode nodeD = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
    wOutput.setid( nodeD, OutputOp.getId( inst ) );
    wOutput.setstate( nodeD, wOutput.getstate( data->props) );
    wOutput.setaddr( nodeD, wOutput.getaddr( data->props ) );
    wOutput.setport( nodeD, wOutput.getport( data->props ) );
    AppOp.broadcastEvent( nodeD );
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "informing %d listeners [%s:%s]",
      ListOp.size( data->listeners ), wOutput.getid( data->props ), wOutput.getstate(data->props) );

  {
    obj listener = ListOp.first( data->listeners );
    while( listener != NULL ) {
      listener->event( listener, data->props );
      listener = ListOp.next( data->listeners );
    };
  }

  NodeOp.base.del(nodeC);
}

/**  */
static const char* _getId( struct OOutput* inst ) {
  iOOutputData data = Data(inst);
  return wOutput.getid( data->props );
}

static Boolean _addListener( iOOutput inst, obj listener ) {
  iOOutputData data = Data(inst);
  ListOp.add( data->listeners, listener );
  return True;
}
static Boolean _removeListener( iOOutput inst, obj listener ) {
  iOOutputData data = Data(inst);
  ListOp.removeObj( data->listeners, listener );
  return True;
}



/**  */
static struct OOutput* _inst( iONode props ) {
  iOOutput __Output = allocMem( sizeof( struct OOutput ) );
  iOOutputData data = allocMem( sizeof( struct OOutputData ) );
  MemOp.basecpy( __Output, &OutputOp, 0, sizeof( struct OOutput ), data );

  /* Initialize data->xxx members... */
  data->props = props;
  data->listeners = ListOp.inst();

  data->addrKey = _createAddrKey(
    wOutput.getbus( props ),
    wOutput.getaddr( props ),
    wOutput.getport( props ),
    wOutput.getporttype( props ),
    wOutput.getiid( props )
    );

  NodeOp.removeAttrByName(data->props, "cmd");

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "inst[%d] for %s", instCnt, _getId(__Output) );


  instCnt++;
  return __Output;
}


/**  */
static void _modify( struct OOutput* inst ,iONode props ) {
  iOOutputData o = Data(inst);
  iOModel model = AppOp.getModel(  );
  Boolean move = StrOp.equals( wModelCmd.getcmd( props ), wModelCmd.move );

  int cnt = NodeOp.getAttrCnt( props );
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );

    if( StrOp.equals("id", name) && StrOp.equals( value, wOutput.getid(o->props) ) )
      continue; /* skip to avoid making invalid pointers */

    NodeOp.setStr( o->props, name, value );
  }

  if(!move) {
    ModelOp.removeCoKey( model, o->addrKey );
    o->addrKey = _createAddrKey(
      wOutput.getbus( o->props ),
      wOutput.getaddr( o->props ),
      wOutput.getport( o->props ),
      wOutput.getporttype( o->props ),
      wOutput.getiid( o->props )
      );
    ModelOp.addCoKey( model, o->addrKey, inst );


    /* delete all childs to make 'room' for the new ones: */
    cnt = NodeOp.getChildCnt( o->props );
    while( cnt > 0 ) {
      iONode child = NodeOp.getChild( o->props, 0 );
      iONode removedChild = NodeOp.removeChild( o->props, child );
      if( removedChild != NULL) {
        NodeOp.base.del(removedChild);
      }
      cnt = NodeOp.getChildCnt( o->props );
    }

    /* add the new or modified childs: */
    cnt = NodeOp.getChildCnt( props );
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( props, i );
      NodeOp.addChild( o->props, (iONode)NodeOp.base.clone(child) );
    }
  }
  else {
    NodeOp.removeAttrByName(o->props, "cmd");
  }

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( o->props );
    AppOp.broadcastEvent( clone );
  }
  props->base.del(props);
}


/**  */
static void _off( struct OOutput* inst ) {
  iOOutputData data = Data(inst);
  iONode node = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
  wOutput.setcmd( node, wOutput.off);
  OutputOp.cmd( inst, node, True );
  return;
}


/**  */
static void _on( struct OOutput* inst ) {
  iOOutputData data = Data(inst);
  iONode node = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
  wOutput.setcmd( node, wOutput.on);
  OutputOp.cmd( inst, node, True );
  return;
}

static Boolean _isState( iOOutput inst, const char* state ) {
  iOOutputData data = Data(inst);
  return StrOp.equals( state, wOutput.getstate(data->props) );
}

static void __doCmdThread( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOOutput op = (iOOutput)ThreadOp.getParm( th );
  iOOutputData data = Data(op);

  iONode nodeA = (iONode)ThreadOp.getPost(th);
  if( nodeA != NULL ) {
    Boolean update = wSwitch.iscmd_update(nodeA);
    int error = 0;
    if( wOutput.getpause(nodeA) > 0 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "delay command for output[%s] %dms", OutputOp.getId(op), wOutput.getpause(nodeA) );
      ThreadOp.sleep(wOutput.getpause(nodeA));
    }
    __doCmd(op, nodeA, update);
  }
  ThreadOp.base.del(th);
}



static Boolean _cmd( iOOutput inst, iONode nodeA, Boolean update ) {
  iOOutputData data = Data(inst);

  if( wOutput.getpause(nodeA) > 0 ) {
    iOThread th = ThreadOp.inst(NULL, &__doCmdThread, inst);
    wSwitch.setcmd_update(nodeA, update);
    ThreadOp.post(th, (obj)nodeA);
    ThreadOp.start(th);
  }
  else {
    return __doCmd(inst, nodeA, update);
  }

  return True;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/output.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
