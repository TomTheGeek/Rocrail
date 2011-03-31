/*
 Rocrail - Model Railroad Software

 Copyright (C) 2009 Rob Versluis <r.j.versluis@rocrail.net>

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

/* ------------------------------------------------------------
 Communication protocol:
   UDP

 Format of the netroutes.xml: (stlist)
   bkx = "netid:blockid"

 Requests:
   getNetRoutes() -> The rocrail server who has the routes will respond.
   isFree(netid, blockid, lcprops)
   reserve(netid, blockid, lcid)

   ------------------------------------------------------------ */

#include "rocrail/impl/r2rnet_impl.h"

#include "rocrail/public/app.h"
#include "rocrail/public/modplan.h"
#include "rocrail/public/model.h"
#include "rocrail/public/block.h"

#include "rocrail/wrapper/public/R2RnetIni.h"
#include "rocrail/wrapper/public/NetReq.h"
#include "rocrail/wrapper/public/NetRsp.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Route.h"
#include "rocrail/wrapper/public/RouteList.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Loc.h"


#include "rocs/public/mem.h"
#include "rocs/public/doc.h"

static int instCnt = 0;

static const char* __getBlockID( const char* bkid, char* rrid );


/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOR2RnetData data = Data(inst);
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

/** ----- OR2Rnet ----- */


/**  */
static Boolean _cmd( struct OR2Rnet* inst ,iONode cmd ) {
  return 0;
}


static Boolean __evaluateNetRoutes( iOR2Rnet inst, iONode netroutes ) {
  iOR2RnetData data = Data(inst);
  Boolean ok = False;

  if( netroutes != NULL ) {
    iONode route = wRouteList.getst(netroutes);
    while( route != NULL ) {
      ListOp.add( data->routelist, (obj)route );
      /* add to model list */
      ModelOp.addNetRoute(AppOp.getModel(), route);
      route = wRouteList.nextst(netroutes, route);
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%d netroutes added", ListOp.size(data->routelist) );
    ok = True;
  }

  return ok;
}


static Boolean __readNetRoutes( iOR2Rnet inst ) {
  iOR2RnetData data = Data(inst);
  Boolean ok = False;
  iONode root = ModPlanOp.parsePlan( wR2RnetIni.getroutes(data->props) );
  if( root != NULL ) {
    iONode route = NULL;

    data->netroutes = wPlan.getstlist(root);

    if( data->netroutes != NULL ) {
      data->netroutes = (iONode)NodeOp.base.clone(data->netroutes);
      ok = __evaluateNetRoutes(inst, data->netroutes);
      data->netroutesprovider = ok;
    }

    NodeOp.base.del( root );

  }

  return ok;
}


static void __handleNetReq(iOR2Rnet inst, iONode req) {
  iOR2RnetData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "handle netreq %s", wNetReq.getreq(req) );

  /* netroutes request */
  if( StrOp.equals( wNetReq.req_netroutes, wNetReq.getreq(req) ) && data->netroutesprovider ) {
    char* s = NodeOp.base.toString(data->netroutes);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "providing netroutes..." );
    ThreadOp.post(data->writer, (obj)s);
  }

  /* getblock request */
  else if( StrOp.equals( wNetReq.req_getblock, wNetReq.getreq(req) ) &&
    StrOp.equals( wNetReq.getremoteid(req), wR2RnetIni.getid(data->props) ) )
  {
    iIBlockBase block = ModelOp.getBlock( AppOp.getModel(), wNetReq.getremotebk(req) );
    if( block != NULL ) {
      iONode rsp = NodeOp.inst( wNetRsp.name(), NULL, ELEMENT_NODE );
      wNetRsp.setlocalid( rsp, wR2RnetIni.getid(data->props) );
      wNetRsp.setlocalbk( rsp, wNetReq.getremotebk(req) );
      wNetRsp.setremoteid( rsp, wNetReq.getlocalid(req) );
      wNetRsp.setrsp( rsp, wNetRsp.rsp_block );
      NodeOp.addChild( rsp, (iONode)NodeOp.base.clone(block->base.properties(block)) );
      char* s = NodeOp.base.toString(rsp);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "block %s response to %s", wNetReq.getremotebk(req), wNetReq.getlocalid(req) );
      ThreadOp.post(data->writer, (obj)s);
      NodeOp.base.del(rsp);
    }
  }

  /* isfree request */
  else if( StrOp.equals( wNetReq.req_isfree, wNetReq.getreq(req) ) &&
    StrOp.equals( wNetReq.getremoteid(req), wR2RnetIni.getid(data->props) ) )
  {
    iIBlockBase block = ModelOp.getBlock( AppOp.getModel(), wNetReq.getremotebk(req) );
    if( block != NULL ) {
      iONode rsp = NodeOp.inst( wNetRsp.name(), NULL, ELEMENT_NODE );
      iONode lc = NodeOp.findNode(req, wLoc.name());

      wNetRsp.setlocalid( rsp, wR2RnetIni.getid(data->props) );
      wNetRsp.setlocalbk( rsp, wNetReq.getremotebk(req) );
      wNetRsp.setremoteid( rsp, wNetReq.getlocalid(req) );
      wNetRsp.setrsp( rsp, block->isFree(block, wLoc.getid(lc)) ? wNetRsp.rsp_isfree:wNetRsp.rsp_occupied );
      char* s = NodeOp.base.toString(rsp);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "block %s response to %s", wNetReq.getremotebk(req), wNetReq.getlocalid(req) );
      ThreadOp.post(data->writer, (obj)s);
      NodeOp.base.del(rsp);
    }
  }

  /* reserve request */
  else if( StrOp.equals( wNetReq.req_reserve, wNetReq.getreq(req) ) &&
    StrOp.equals( wNetReq.getremoteid(req), wR2RnetIni.getid(data->props) ) )
  {
    iIBlockBase block = ModelOp.getBlock( AppOp.getModel(), wNetReq.getremotebk(req) );
    if( block != NULL ) {
      iONode lc = NodeOp.findNode(req, wLoc.name());
      iONode bk = NodeOp.findNode(req, wBlock.name());
      Boolean reserved = block->lock(block, wLoc.getid(lc), wNetReq.getlocalbk(req), wNetReq.getrouteid(req), False, False, False, 0);
      iORoute route = ModelOp.getRoute( AppOp.getModel(), wNetReq.getrouteid(req) );

      if( route != NULL ) {
        iONode rsp = NodeOp.inst( wNetRsp.name(), NULL, ELEMENT_NODE );

        if( reserved ) {
          /* append the loco to the list */
          iOLoc loc = ModelOp.addNetLoc( AppOp.getModel(), lc );

          /* re-lock again to provide the block with the cloned ID pointer */
          block->lock(block, LocOp.getId(loc), wNetReq.getlocalbk(req), wNetReq.getrouteid(req), False, False, False, 0);

          wBlock.setremote( bk, True );
          wBlock.setrrid( bk, wNetReq.getlocalid(req) );
          ModelOp.addNetBlock( AppOp.getModel(), bk );

          /* put loco in automode in the wait4event mode */
          LocOp.goNet(loc, wNetReq.getlocalbk(req), block->base.id(block), RouteOp.getId(route));
        }

        wNetRsp.setlocalbk( rsp, wNetReq.getremotebk(req) );
        wNetRsp.setlocalid( rsp, wR2RnetIni.getid(data->props) );
        wNetRsp.setremoteid( rsp, wNetReq.getlocalid(req) );
        wNetRsp.setrsp( rsp, reserved ? wNetRsp.rsp_reserved:wNetRsp.rsp_occupied );
        char* s = NodeOp.base.toString(rsp);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "block %s response to %s", wNetReq.getremotebk(req), wNetReq.getlocalid(req) );
        ThreadOp.post(data->writer, (obj)s);
        NodeOp.base.del(rsp);
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "route %s is not known", wNetReq.getrouteid(req) );
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "block %s is not known", wNetReq.getremotebk(req) );
    }
  }

  /* unlock request */
  else if( StrOp.equals( wNetReq.req_unlock, wNetReq.getreq(req) ) &&
    StrOp.equals( wNetReq.getremoteid(req), wR2RnetIni.getid(data->props) ) )
  {
    char rrid[64];
    iIBlockBase block = ModelOp.getBlock( AppOp.getModel(), __getBlockID(wNetReq.getremotebk(req), rrid) );
    if( block != NULL && StrOp.equals( rrid, wR2RnetIni.getid(data->props)) ) {
      const char* lcid = __getBlockID(wNetReq.getlcid(req), rrid);
      Boolean unlocked = False;
      iOLoc loc = NULL;

      if( StrOp.equals( rrid, wR2RnetIni.getid(data->props)) )
        unlocked = block->unLock(block, lcid );
      else {
        unlocked = block->unLock(block, wNetReq.getlcid(req) );
        lcid = wNetReq.getlcid(req);
      }

      loc = ModelOp.getLoc(AppOp.getModel(), lcid);

      if( unlocked && loc != NULL) {
        iONode rsp = NodeOp.inst( wNetRsp.name(), NULL, ELEMENT_NODE );
        wNetRsp.setlocalid( rsp, wR2RnetIni.getid(data->props) );
        wNetRsp.setremoteid( rsp, wNetReq.getlocalid(req) );
        wNetRsp.setrsp( rsp, wNetRsp.rsp_unlocked );
        char* s = NodeOp.base.toString(rsp);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "block %s response to %s", wNetReq.getremotebk(req), wNetReq.getlocalid(req) );
        ThreadOp.post(data->writer, (obj)s);
        NodeOp.base.del(rsp);
        LocOp.stopNet(loc);
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unable to unlock %s from block %s", lcid, wNetReq.getremotebk(req) );
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "mismatching unlock block[%s] from %s", wNetReq.getremotebk(req), wNetReq.getlocalid(req) );
    }
  }

  /* locoisin request */
  else if( StrOp.equals( wNetReq.req_locoisin, wNetReq.getreq(req) ) &&
    StrOp.equals( wNetReq.getremoteid(req), wR2RnetIni.getid(data->props) ) )
  {
    iOLoc loco = ModelOp.getLoc( AppOp.getModel(), wNetReq.getlcid(req) );
    if( loco == NULL && StrOp.find(wNetReq.getlcid(req), "::") != NULL ) {
      char* s = StrOp.find(wNetReq.getlcid(req), "::");
      loco = ModelOp.getLoc( AppOp.getModel(), s + 2 );
    }
    if( loco == NULL ) {
      LocOp.stop(loco, False);
      LocOp.reset(loco, False);
    }
  }

  /* client connection request */
  else if( StrOp.equals( wNetReq.req_clientconn, wNetReq.getreq(req) ) )
  {
    iONode rsp = NodeOp.inst( wNetRsp.name(), NULL, ELEMENT_NODE );
    iOClntCon clntcon = AppOp.getClntCon();
    wNetRsp.sethost( rsp, ClntConOp.getClientHost(clntcon) );
    wNetRsp.setport( rsp, ClntConOp.getClientPort(clntcon) );
    wNetRsp.setplan( rsp, ModelOp.getTitle(AppOp.getModel()));
    wNetRsp.setrsp( rsp, wNetRsp.rsp_clientconn );
    char* s = NodeOp.base.toString(rsp);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "client connection response: %s", s );
    ThreadOp.post(data->writer, (obj)s);
  }

}


static void __handleNetRsp(iOR2Rnet inst, iONode rsp) {
  iOR2RnetData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "handle netrsp %s", wNetRsp.getrsp(rsp) );

  /* get block response */
  if( StrOp.equals( wNetRsp.rsp_block, wNetRsp.getrsp(rsp) ) && data->openreq != NULL ) {
    /* check if it matches the open request */
    if( StrOp.equals( wNetRsp.getlocalid(rsp), wNetReq.getremoteid( data->openreq ) ) &&
        StrOp.equals( wNetRsp.getlocalbk(rsp), wNetReq.getremotebk( data->openreq ) ) &&
        StrOp.equals( wNetRsp.getremoteid(rsp), wR2RnetIni.getid(data->props) ) )
    {
      iONode bk = NodeOp.findNode( rsp, wBlock.name() );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "matching block response" );
      data->response = (iONode)NodeOp.base.clone(bk);
      EventOp.set( data->rspEvt );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "block response did not match the open request" );
    }
  }

  /* isfree block response */
  else if( StrOp.equals( wNetRsp.rsp_isfree, wNetRsp.getrsp(rsp) ) && data->openreq != NULL ) {
    /* check if it matches the open request */
    if( StrOp.equals( wNetRsp.getlocalid(rsp), wNetReq.getremoteid( data->openreq ) ) &&
        StrOp.equals( wNetRsp.getlocalbk(rsp), wNetReq.getremotebk( data->openreq ) ) &&
        StrOp.equals( wNetRsp.getremoteid(rsp), wR2RnetIni.getid(data->props) ) )
    {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "matching reserve response" );
      data->response = (iONode)NodeOp.base.clone(rsp);
      EventOp.set( data->rspEvt );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reserve response did not match the open request" );
    }
  }

  /* reserve block response */
  else if( StrOp.equals( wNetRsp.rsp_reserved, wNetRsp.getrsp(rsp) ) && data->openreq != NULL ) {
    /* check if it matches the open request */
    if( StrOp.equals( wNetRsp.getlocalid(rsp), wNetReq.getremoteid( data->openreq ) ) &&
        StrOp.equals( wNetRsp.getlocalbk(rsp), wNetReq.getremotebk( data->openreq ) ) &&
        StrOp.equals( wNetRsp.getremoteid(rsp), wR2RnetIni.getid(data->props) ) )
    {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "matching reserve response" );
      data->response = (iONode)NodeOp.base.clone(rsp);
      EventOp.set( data->rspEvt );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reserve response did not match the open request" );
    }
  }

  /* unlock block response */
  else if( StrOp.equals( wNetRsp.rsp_unlocked, wNetRsp.getrsp(rsp) ) && data->openreq != NULL ) {
    /* check if it matches the open request */
    if( StrOp.equals( wNetRsp.getlocalid(rsp), wNetReq.getremoteid( data->openreq ) ) &&
        StrOp.equals( wNetRsp.getlocalbk(rsp), wNetReq.getremotebk( data->openreq ) ) &&
        StrOp.equals( wNetRsp.getremoteid(rsp), wR2RnetIni.getid(data->props) ) )
    {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "matching unlock response" );
      data->response = (iONode)NodeOp.base.clone(rsp);
      EventOp.set( data->rspEvt );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unlock response did not match the open request" );
    }
  }

}


static void __handleNetRoutes(iOR2Rnet inst, iONode routes) {
  iOR2RnetData data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "handle routes" );
  if( !data->gotnetroutes ) {
    data->netroutes = (iONode)NodeOp.base.clone(routes);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "netroutes received" );
    data->gotnetroutes = __evaluateNetRoutes(inst, data->netroutes);
  }
}


static void __evaluateMessage(iOR2Rnet inst, const char* s) {
  iOR2RnetData data = Data(inst);

  iODoc msgDoc = DocOp.parse( s );
  if( msgDoc != NULL ) {
    iONode msg = DocOp.getRootNode( msgDoc );
    DocOp.base.del( msgDoc );

    if( msg != NULL ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%s message parsed", NodeOp.getName(msg) );
      if( StrOp.equals( wNetReq.name(), NodeOp.getName(msg) ) )
        __handleNetReq(inst, msg);
      else if( StrOp.equals( wNetRsp.name(), NodeOp.getName(msg) ) )
        __handleNetRsp(inst, msg);
      else if( StrOp.equals( wRouteList.name(), NodeOp.getName(msg) ) )
        __handleNetRoutes(inst, msg);

      /* clean up node */
      NodeOp.base.del(msg);
    }
  }

}


#define MSGSIZE 10*1024

static void __reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOR2Rnet r2rnet = (iOR2Rnet)ThreadOp.getParm( th );
  iOR2RnetData data = Data(r2rnet);
  char* msg = allocMem(MSGSIZE);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "R2Rnet reader started." );

  data->gotnetroutes = __readNetRoutes(r2rnet);

  do {
    int extended = False;
    int event   = False;

    MemOp.set( msg, 0, MSGSIZE );

    SocketOp.recvfrom( data->readUDP, msg, MSGSIZE, NULL, NULL );
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "msg:\n%s", msg );
    __evaluateMessage(r2rnet, msg);

  } while( data->run && !ThreadOp.isQuit(th) );

  freeMem(msg);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "R2Rnet reader stopped." );
}


static void __writer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOR2Rnet r2rnet = (iOR2Rnet)ThreadOp.getParm( th );
  iOR2RnetData data = Data(r2rnet);
  int retryGetNetRoutes = 0;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "R2Rnet writer started." );

  do {
    char* req = (char*)ThreadOp.getPost( th );
    if (req != NULL) {
      int plen     = 0;

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "write request from queue:\n%s", req );
      SocketOp.sendto( data->writeUDP, req, StrOp.len(req), NULL, 0 );
      StrOp.free(req);
    }

    if( !data->gotnetroutes ) {
      if( retryGetNetRoutes > 100 ) {
        char* s = NULL;
        iONode req = NodeOp.inst( wNetReq.name(), NULL, ELEMENT_NODE );
        wNetReq.setreq( req, wNetReq.req_netroutes );
        wNetReq.setlocalid( req, wR2RnetIni.getid(data->props) );
        wNetReq.setremoteid( req, "*" );
        s = NodeOp.base.toString(req);
        SocketOp.sendto( data->writeUDP, s, StrOp.len(s), NULL, 0 );
        StrOp.free(s);
        NodeOp.base.del(req);
        retryGetNetRoutes = 0;
      }
      else {
        retryGetNetRoutes++;
      }
    }

    ThreadOp.sleep(10);
  } while( data->run && !ThreadOp.isQuit(th) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "R2Rnet writer stopped." );
}


static void _quit( iOR2Rnet inst ) {
  iOR2RnetData data = Data(inst);
  data->run = False;
}


static const char* __getBlockID( const char* bkid, char* rrid ) {
  char* s = StrOp.find( bkid, "::" );
  if( s == NULL ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "invalid netid: [%s]", bkid );
    return NULL;
  }

  if( rrid != NULL ) {
    MemOp.copy( rrid, bkid, s - bkid );
    rrid[s - bkid] = '\0';
  }

  return s + 2;
}


static Boolean _reserveBlock( iOR2Rnet inst, const char* rrid, const char* bkid, const char* stid, iONode lc, iONode bk, Boolean check ) {
  iOR2RnetData data = Data(inst);
  iONode lcprops = (iONode)NodeOp.base.clone(lc);
  iONode bkprops = check ? NULL:(iONode)NodeOp.base.clone(bk);

  Boolean isReserved = False;

  if( !MutexOp.trywait( data->reqMux, 1000 ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "timeout on request mutex" );
    return isReserved;
  }

  if( StrOp.find(wLoc.getid(lcprops), "::") == NULL ) {
    /* prefix the rrid */
    char tmp[256];
    StrOp.fmtb( tmp, "%s::%s", wR2RnetIni.getid(data->props), wLoc.getid(lc) );
    wLoc.setid( lcprops, tmp );
  }

  if( !check && StrOp.find(wBlock.getid(bkprops), "::") == NULL ) {
    /* prefix the rrid */
    char tmp[256];
    StrOp.fmtb( tmp, "%s::%s", wR2RnetIni.getid(data->props), wBlock.getid(bk) );
    wBlock.setid( bkprops, tmp );
  }

  {
    char* s = NULL;
    iONode req   = NodeOp.inst( wNetReq.name(), NULL, ELEMENT_NODE );
    wNetReq.setreq( req, check ? wNetReq.req_isfree:wNetReq.req_reserve );
    wNetReq.setlocalid( req, wR2RnetIni.getid(data->props) );
    wNetReq.setremoteid( req, rrid );
    wNetReq.setremotebk( req, bkid );
    wNetReq.setlocalbk( req, wBlock.getid(bkprops) );
    if( stid != NULL )
      wNetReq.setrouteid( req, stid );
    NodeOp.addChild(req, lcprops);
    if( !check )
      NodeOp.addChild(req, bkprops);
    s = NodeOp.base.toString(req);
    data->openreq = req;

    EventOp.reset(data->rspEvt);
    ThreadOp.post( data->writer, (obj)s);

    if( EventOp.trywait(data->rspEvt, 1000) ) {
      data->openreq = NULL;

      if( data->response != NULL ) {
        if( check )
          isReserved = StrOp.equals(wNetRsp.rsp_isfree, wNetRsp.getrsp(data->response));
        else
          isReserved = StrOp.equals(wNetRsp.rsp_reserved, wNetRsp.getrsp(data->response));

        NodeOp.base.del(data->response);
        data->response = NULL;
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "timeout on response event" );
    }

    NodeOp.base.del(req);
  }


  MutexOp.post( data->reqMux );
  return isReserved;
}


static Boolean _unlockBlock( iOR2Rnet inst, const char* rrid, const char* bkid, const char* lcid ) {
  iOR2RnetData data = Data(inst);
  Boolean isUnlocked = False;

  if( !MutexOp.trywait( data->reqMux, 1000 ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "timeout on request mutex" );
    return isUnlocked;
  }

  {
    char* s = NULL;
    iONode req   = NodeOp.inst( wNetReq.name(), NULL, ELEMENT_NODE );
    wNetReq.setreq( req, wNetReq.req_unlock );
    wNetReq.setlocalid( req, wR2RnetIni.getid(data->props) );
    wNetReq.setremoteid( req, rrid );
    wNetReq.setremotebk( req, bkid );
    wNetReq.setlcid( req, lcid );
    s = NodeOp.base.toString(req);
    data->openreq = req;

    EventOp.reset(data->rspEvt);
    ThreadOp.post( data->writer, (obj)s);

    if( EventOp.trywait(data->rspEvt, 1000) ) {
      data->openreq = NULL;

      if( data->response != NULL ) {
        isUnlocked = StrOp.equals(wNetRsp.rsp_unlocked, wNetRsp.getrsp(data->response));

        NodeOp.base.del(data->response);
        data->response = NULL;
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "timeout on response event" );
    }

    NodeOp.base.del(req);
  }


  MutexOp.post( data->reqMux );
  return isUnlocked;
}


/**
 * The locomotive has reached the IN event of the remote block.
 * This call is for the origin rocrail server to free up the current block
 * and to stop the auto mode for this loco.
 *
 */
static void _locoIsIn( iOR2Rnet inst, const char* rrid, const char* lcid, const char* bkid ) {
  iOR2RnetData data = Data(inst);

  if( !MutexOp.trywait( data->reqMux, 1000 ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "timeout on request mutex" );
    return;
  }
  else {
    char* s = NULL;
    iONode req   = NodeOp.inst( wNetReq.name(), NULL, ELEMENT_NODE );
    wNetReq.setreq( req, wNetReq.req_locoisin );
    wNetReq.setlocalid( req, wR2RnetIni.getid(data->props) );
    wNetReq.setremoteid( req, rrid );
    wNetReq.setlocalbk( req, bkid );
    s = NodeOp.base.toString(req);

    ThreadOp.post( data->writer, (obj)s);

    NodeOp.base.del(req);
  }
  MutexOp.post( data->reqMux );
}


static iONode _getBlock( iOR2Rnet inst, const char* bkid ) {
  iOR2RnetData data = Data(inst);
  iONode block = NULL;
  char rrid[64];
  const char* blockid = NULL;

  if( !MutexOp.trywait( data->reqMux, 1000 ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "timeout on request mutex" );
    return NULL;
  }

  blockid = __getBlockID(bkid, rrid);

  if( blockid == NULL ) {
    MutexOp.post( data->reqMux );
    return NULL;
  }
  else {
    char* s = NULL;
    iONode req   = NodeOp.inst( wNetReq.name(), NULL, ELEMENT_NODE );
    wNetReq.setreq( req, wNetReq.req_getblock );
    wNetReq.setlocalid( req, wR2RnetIni.getid(data->props) );
    wNetReq.setremoteid( req, rrid );
    wNetReq.setremotebk( req, blockid );
    s = NodeOp.base.toString(req);
    data->openreq = req;

    EventOp.reset(data->rspEvt);
    ThreadOp.post( data->writer, (obj)s);

    if( EventOp.trywait(data->rspEvt, 1000) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "got response event" );
      data->openreq = NULL;

      if( data->response != NULL ) {
        block = (iONode)NodeOp.base.clone(data->response);
        NodeOp.base.del(data->response);
        data->response = NULL;
        wBlock.setremote( block, True );
        wBlock.setrrid( block, rrid );
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "timeout on response event" );
    }

    NodeOp.base.del(req);


  }

  MutexOp.post( data->reqMux );
  return block;
}


static Boolean _compare( const char* id1, const char* id2 ) {
  Boolean equal = False;

  if( ControlOp.getR2Rnet(AppOp.getControl()) != NULL ) {
    iOR2Rnet r2rnet = ControlOp.getR2Rnet(AppOp.getControl());
    iOR2RnetData data = Data(r2rnet);
    const char* rrid = wR2RnetIni.getid(data->props);
    if( StrOp.find(id1, "::") != NULL && StrOp.find(id2, "::") != NULL ) {
      equal = StrOp.equals(id1, id2);
    }
    else if( StrOp.find(id1, "::") != NULL || StrOp.find(id2, "::") != NULL ) {
      if( StrOp.find(id1, "::") == NULL ) {
        /* local block id compare */
        char id[256];
        StrOp.fmtb( id, "%s::%s", rrid, id1 );
        equal = StrOp.equals(id, id2);
      }
      else if( StrOp.find(id2, "::") == NULL ) {
        /* local block id compare */
        char id[256];
        StrOp.fmtb( id, "%s::%s", rrid, id2 );
        equal = StrOp.equals(id1, id);
      }
    }
    else
      equal = StrOp.equals(id1, id2);
  }
  else
    equal = StrOp.equals(id1, id2);

  return equal;
}


/**  */
static struct OR2Rnet* _inst( iONode ini ) {
  iOR2Rnet __R2Rnet = allocMem( sizeof( struct OR2Rnet ) );
  iOR2RnetData data = allocMem( sizeof( struct OR2RnetData ) );
  MemOp.basecpy( __R2Rnet, &R2RnetOp, 0, sizeof( struct OR2Rnet ), data );

  /* Initialize data->xxx members... */
  data->props = ini;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "R2Rnet multicast address [%s]", wR2RnetIni.getaddr(ini) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "R2Rnet multicast port    [%d]", wR2RnetIni.getport(ini) );

  data->reqMux = MutexOp.inst( "r2rnetmux", True );
  data->rspEvt = EventOp.inst( "r2rnetevt", True );

  data->readUDP = SocketOp.inst( wR2RnetIni.getaddr(ini), wR2RnetIni.getport(ini), False, True, True );
  SocketOp.bind(data->readUDP);
  data->writeUDP = SocketOp.inst( wR2RnetIni.getaddr(ini), wR2RnetIni.getport(ini), False, True, True );

  data->run = True;
  data->routelist = ListOp.inst();

  data->reader = ThreadOp.inst( "r2rreader", &__reader, __R2Rnet );
  ThreadOp.start( data->reader );

  data->writer = ThreadOp.inst( "r2rwriter", &__writer, __R2Rnet );
  ThreadOp.start( data->writer );

  instCnt++;
  return __R2Rnet;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/r2rnet.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
