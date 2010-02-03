/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocrail/impl/fback_impl.h"
#include "rocrail/public/app.h"
#include "rocrail/public/control.h"
#include "rocrail/public/clntcon.h"
#include "rocrail/public/model.h"
#include "rocrail/public/switch.h"
#include "rocrail/public/route.h"
#include "rocrail/public/action.h"

#include "rocint/public/blockbase.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/AutoCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Output.h"


static int instCnt = 0;

/*
 ***** OBase functions.
 */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  if( StrOp.equals( wFeedback.name(), NodeOp.getName( (iONode)evt ) ) ) {
    FBackOp.event(inst, (iONode)evt);
  }
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
  return (char*)FBackOp.getId( (iOFBack)inst );
}
static void __del(void* inst) {
  iOFBackData data = Data(inst);
  freeMem( data );
  freeMem( inst );
  instCnt--;
}
static void* __properties(void* inst) {
  iOFBackData data = Data(inst);
  return data->props;
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

static void __ctcAction( iOFBack inst ) {
  iOFBackData data  = Data(inst);
  iOModel     model = AppOp.getModel();
  iOControl control = AppOp.getControl(  );

  if( control == NULL )
    return;

  if( wFeedback.getctcaddr( data->props ) > 0 || wFeedback.getctcport( data->props ) ) {
    iONode node = NULL;
    if( wFeedback.isctcasswitch(data->props) ) {
      node = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
      if( wFeedback.getctciid( data->props ) != NULL )
        wSwitch.setiid( node, wFeedback.getctciid( data->props ) );
      wSwitch.setaddr1( node, wFeedback.getctcaddr( data->props ) );
      wSwitch.setport1( node, wFeedback.getctcport( data->props ) );
      wSwitch.setgate1( node, wFeedback.getctcgate( data->props ) );
      wSwitch.setcmd( node, data->state ? wSwitch.straight:wSwitch.turnout );
    }
    else {
      node = NodeOp.inst(wOutput.name(), NULL, ELEMENT_NODE );
      if( wFeedback.getctciid( data->props ) != NULL )
        wOutput.setiid( node, wFeedback.getctciid( data->props ) );
      wOutput.setaddr( node, wFeedback.getctcaddr( data->props ) );
      wOutput.setport( node, wFeedback.getctcport( data->props ) );
      wOutput.setgate( node, wFeedback.getctcgate( data->props ) );
      wOutput.setcmd( node, data->state ? wOutput.on:wOutput.off );
    }
    ControlOp.cmd( control, node, NULL );
  }
}


static void __checkAction( iOFBack inst ) {
  iOFBackData data     = Data(inst);
  iOModel     model    = AppOp.getModel();
  iONode      fbaction = wFeedback.getactionctrl( data->props );

  /* loop over all actions */
  while( fbaction != NULL ) {
    if( StrOp.equals( data->state?"on":"off"    , wActionCtrl.getstate( fbaction ) ) ||
        StrOp.equals( data->state?"true":"false", wActionCtrl.getstate( fbaction ) ) ||
        data->state && StrOp.len(wActionCtrl.getstate( fbaction )) == 0 ) {

      if( data->listenerObj != NULL ) {
        iIBlockBase bk = (iIBlockBase)data->listenerObj;
        wActionCtrl.setlcid(fbaction, bk->getLoc(bk) );
      }
      iOAction action = ModelOp.getAction(model, wActionCtrl.getid( fbaction ));
      if( action != NULL )
        ActionOp.exec(action, fbaction);
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action state does not match: [%s-%s]",
          wActionCtrl.getstate( fbaction ), data->state?"on/true":"off/false" );
    }

    fbaction = wFeedback.nextactionctrl( data->props, fbaction );
  }
}

/*
 ***** _Public functions.
 */
static Boolean _setListener( iOFBack inst, obj listenerObj, const fback_listener listenerFun ) {
  iOFBackData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%s listener set for %s",
    FBackOp.getId( inst ),
    listenerObj->toString(listenerObj) );
  return True;
}

static Boolean _addListener( iOFBack inst, obj listener ) {
  iOFBackData data = Data(inst);
  ListOp.add( data->listeners, listener );
  return True;
}
static Boolean _removeListener( iOFBack inst, obj listener ) {
  iOFBackData data = Data(inst);
  ListOp.removeObj( data->listeners, listener );
  return True;
}

static Boolean _addBlock( iOFBack inst, iIBlockBase listener ) {
  iOFBackData data = Data(inst);
  /* TODO: use own list? */
  ListOp.add( data->listeners, (obj)listener );
  return True;
}
static Boolean _removeBlock( iOFBack inst, iIBlockBase listener ) {
  iOFBackData data = Data(inst);
  /* TODO: use own list? */
  ListOp.removeObj( data->listeners, (obj)listener );
  return True;
}

static const char* _getId( iOFBack inst ) {
  iOFBackData data = Data(inst);
  return wFeedback.getid( data->props );
}

static void* _getProperties( void* inst ) {
  iOFBackData data = Data((iOFBack)inst);
  return data->props;
}

static char* _createAddrKey( int bus, int addr, const char* iid ) {
  iONode node = AppOp.getIniNode( wDigInt.name() );
  const char* def_iid = wDigInt.getiid( node );
  return StrOp.fmt( "%d_%d_%s", bus, addr, (iid != NULL && StrOp.len( iid ) > 0) ? iid:def_iid );
}

static const char* _getAddrKey( iOFBack inst ) {
  iOFBackData data = Data(inst);
  return data->addrKey;
}

static Boolean _getState( iOFBack inst ) {
  iOFBackData data = Data(inst);
  return data->state;
}

static int _getIdentifier( iOFBack inst ) {
  iOFBackData data = Data(inst);
  return wFeedback.getidentifier( data->props );
}

static void _setState( iOFBack inst, Boolean state ) {
  iOFBackData data = Data(inst);
  data->state = state;
  wFeedback.setstate( data->props, data->state );
  /* Broadcast to clients. Node4 */
  {
    iONode nodeD = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
    wFeedback.setid( nodeD, FBackOp.getId( inst ) );
    wFeedback.setstate( nodeD, data->state );
    wFeedback.setaddr( nodeD, wFeedback.getaddr( data->props ) );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeD );
  }
}


static Boolean _cmd( iOFBack inst, iONode cmd, Boolean update ) {
  iOFBackData data = Data(inst);
  iOControl control = AppOp.getControl();
  int error = 0;
  wFeedback.setiid( cmd, wFeedback.getiid( data->props ) );
  wFeedback.setbus( cmd, wFeedback.getbus( data->props ) );
  wFeedback.setaddr( cmd, wFeedback.getaddr( data->props ) );
  ControlOp.cmd( control, cmd, &error );
  return True;
}


static void _event( iOFBack inst, iONode nodeC ) {
  iOFBackData data = Data(inst);
  Boolean hasListener = False;

  if( TraceOp.getLevel(NULL) & TRCLEVEL_DEBUG ) {
    char* strNode = (char*)NodeOp.base.toString( nodeC );
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "event %s", strNode );
    StrOp.free( strNode );
  }

  data->state = wFeedback.isstate( nodeC );
  /* check for active low */
  if( wFeedback.isactivelow( data->props ) )
    data->state = !data->state;

  wFeedback.setstate( data->props, data->state );
  wFeedback.setidentifier( data->props, wFeedback.getidentifier( nodeC ) );

  /* Call listener. */
  if( data->listenerFun != NULL ) {
    data->listenerFun( data->listenerObj, data->state, FBackOp.getId( inst ), wFeedback.getidentifier( nodeC ), wFeedback.getval( nodeC ) );
    hasListener = True;
  }

  {
    obj listener = ListOp.first( data->listeners );
    while( listener != NULL ) {
      hasListener = True;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "fb [%s](%s) ident=%d val=%d call listener 0x%08X...",
                   FBackOp.getId(inst), data->state?"ON":"OFF", wFeedback.getidentifier( nodeC ), wFeedback.getval( nodeC ), listener );
      listener->event( listener, data->props );
      listener = ListOp.next( data->listeners );
    };
  }
  __ctcAction( inst );
  __checkAction( inst );

  if(!hasListener) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "fb \"%s\"(%s) ident=%d val=%d has no listener...",
                 FBackOp.getId(inst), data->state?"ON":"OFF", wFeedback.getidentifier( nodeC ), wFeedback.getval( nodeC ) );
  }

  /* Broadcast to clients. Node4 */
  {
    iONode nodeD = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
    wFeedback.setid( nodeD, FBackOp.getId( inst ) );
    wFeedback.setstate( nodeD, data->state );
    wFeedback.setval( nodeD, wFeedback.getval( nodeC ) );
    wFeedback.setaddr( nodeD, wFeedback.getaddr( data->props ) );
    wFeedback.setidentifier( nodeD, wFeedback.getidentifier( nodeC ) );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeD );
  }

  /* Cleanup Node3 */
  nodeC->base.del(nodeC);
}

/**
 * Checks for property changes.
 * todo: Range checking?
 */
static void _modify( iOFBack inst, iONode props ) {
  iOFBackData data  = Data(inst);
  iOModel     model = AppOp.getModel();
  Boolean move = StrOp.equals( wModelCmd.getcmd( props ), wModelCmd.move );

  int cnt = NodeOp.getAttrCnt( props );
  int i = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "modify %s", wFeedback.getid(data->props) );
  /* TODO: inform model with current addrkey to remove from map. */

  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );
    NodeOp.setStr( data->props, name, value );
  }

  if(!move) {
    /* delete all childs to make 'room' for the new ones: */
    cnt = NodeOp.getChildCnt( data->props );
    while( cnt > 0 ) {
      iONode child = NodeOp.getChild( data->props, 0 );
      NodeOp.removeChild( data->props, child );
      cnt = NodeOp.getChildCnt( data->props );
    }

    /* add the new or modified childs: */
    cnt = NodeOp.getChildCnt( props );
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( props, i );
      NodeOp.addChild( data->props, (iONode)NodeOp.base.clone(child) );
    }

    /* TODO: addrkey changed!!! */
    ModelOp.removeFbKey( model, data->addrKey, (obj)inst );
    StrOp.free( data->addrKey );
    data->addrKey = FBackOp.createAddrKey( wFeedback.getbus(data->props),
                                           wFeedback.getaddr(data->props),
                                           wFeedback.getiid(data->props) );
    /* inform model with new addrkey to add to map. */
    ModelOp.addFbKey( model, data->addrKey, (obj)inst );
  }
  else {
    NodeOp.removeAttrByName(data->props, "cmd");
  }

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)props->base.clone( props );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), clone );
  }
  props->base.del(props);
}

static const char* _tableHdr(void) {
  return "<tr bgcolor=\"#CCCCCC\"><th>ID</th><th>Addr</th><th>x</th><th>y</th><th>z</th></tr>\n";
}

static char* _toHtml( void* inst ) {
  iOFBackData data = Data((iOFBack)inst);
  return StrOp.fmt( "<tr><td>%s</td><td align=\"right\">%d</td><td align=\"right\">%d</td><td align=\"right\">%d</td><td align=\"right\">%d</td></tr>\n",
      wFeedback.getid( data->props ),
      wFeedback.getaddr( data->props ),
      wFeedback.getx( data->props ),
      wFeedback.gety( data->props ),
      wFeedback.getz( data->props )
      );
}

static char* _getForm( void* inst ) {
  iOFBackData data = Data((iOFBack)inst);
  return NULL;
}


static char* _postForm( void* inst, const char* postdata ) {
  iOFBackData data = Data((iOFBack)inst);
  char* reply = StrOp.fmt( "Thanks!<br>" );
  return reply;
}


static iOFBack _inst( iONode props ) {
  iOFBack     fback = allocMem( sizeof( struct OFBack ) );
  iOFBackData data  = allocMem( sizeof( struct OFBackData ) );

  /* OBase operations */
  MemOp.basecpy( fback, &FBackOp, 0, sizeof( struct OFBack ), data );

  data->props = props;
  /* initially the state is off: */
  wFeedback.setstate( props, False );

  data->listeners = ListOp.inst();

  data->addrKey = _createAddrKey(
    wFeedback.getbus( props ),
    wFeedback.getaddr( props ),
    wFeedback.getiid( props )
    );

  NodeOp.removeAttrByName(data->props, "cmd");

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "inst[%d] for %s, %s",
        instCnt,
        _getId(fback),
        data->addrKey
        );

  instCnt++;

  return fback;
}

static Boolean _isState( iOFBack inst, const char* state ) {
  iOFBackData data = Data(inst);
  if( StrOp.equals( state, "true" ) && data->state )
    return True;
  if( StrOp.equals( state, "false" ) && !data->state )
    return True;

  return False;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/fback.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
