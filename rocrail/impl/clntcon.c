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

#include "rocrail/impl/clntcon_impl.h"
#include "rocrail/public/app.h"
#include "rocrail/public/model.h"

#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/xmlh.h"

#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/AutoCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Tcp.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/DataReq.h"
#include "rocrail/wrapper/public/Exception.h"
#include "rocrail/wrapper/public/Loc.h"

static int instCnt = 0;

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
  iOClntConData data = Data(inst);
  freeMem( data );
  freeMem( inst );
  instCnt--;
}
static void* __properties(void* inst) {
  return NULL;
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



struct __OClntService {
  iOClntCon     ClntCon;
  iOSocket      clntSocket;
  Boolean       readonly;
  Boolean       quit;
  Boolean       disablemonitor;
};
typedef struct __OClntService* __iOClntService;


static void __infoWriter( void* threadinst ) {
  iOThread       th = (iOThread)threadinst;
  __iOClntService o = (__iOClntService)ThreadOp.getParm(th);
  Boolean        ok = True;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "infoWriter started for:%s.", SocketOp.getPeername(o->clntSocket) );

  do {
    obj post = ThreadOp.waitPost( th );
    if( post != NULL ) {
      iONode node = (iONode)post;

      if( StrOp.equals( "quit", NodeOp.getName( node ) ) ) {
        node->base.del( node );
        break;
      }

      {
        iOXmlh   xmlh = XmlhOp.inst( True, NULL, NULL );
        char*    info = NodeOp.base.toString( node );
        int   infoLen = StrOp.len( info ) + 1;
        iONode    xml = NodeOp.inst( XmlhOp.xml_tagname, NULL, ELEMENT_NODE );
        long  xmlhLen = 0;
        char* xmlhStr = NULL;

        NodeOp.setInt( xml, "size", infoLen );
        XmlhOp.addNode( xmlh, xml );
        xmlhStr = XmlhOp.base.serialize( xmlh, &xmlhLen );
        XmlhOp.base.del( xmlh );

        TraceOp.trc( name, TRCLEVEL_XMLH, __LINE__, 9999, "%s", xmlhStr );

        TraceOp.trc( name, TRCLEVEL_XMLH, __LINE__, 9999, "%80.80s...", info );

        if( SocketOp.write( o->clntSocket, xmlhStr, xmlhLen ) )
          ok = SocketOp.write( o->clntSocket, info, infoLen );
        else
          ok = False;

        /* plan node will not be cloned! */
        if( !StrOp.equals( wPlan.name(), NodeOp.getName( node ) ) ) {
          /* Cleanup: endstation for all nodes. */
          node->base.del( node );
        }

        /* free the serialized info and xmlh: */
        StrOp.free( xmlhStr );
        StrOp.free( info );
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_ERROR, __LINE__, 9999, "InfoService() waitPost returns NULL!" );
    }

    ThreadOp.sleep( 10 );
  } while( !o->quit );

  /* Lock the semaphore: */
  MutexOp.trywait( Data(o->ClntCon)->muxMap, 1000 );
  {
    obj me = MapOp.remove( Data(o->ClntCon)->infoWriters, ThreadOp.getName( th ) );
    if( me != NULL )
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "InfoService() Removed from Map." );
  }
  /* Unlock the semaphore: */
  MutexOp.post( Data(o->ClntCon)->muxMap );

  /* Cleanup. */
  /* ToDo: Inform cmdReader! */
  {
    iOSocket s = o->clntSocket;
    o->clntSocket = NULL;
    SocketOp.base.del( s );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "InfoService ended." );
    ThreadOp.base.del( th );
    freeMem(o);
  }
}



/**----------------------------------------------------------------------
 * __readXmlh()
 * ----------------------------------------------------------------------
 */
static Boolean __readXmlh( iOSocket sh, iOXmlh xmlh ) {
  char b = 0;
  while( SocketOp.peek( sh, &b, 1 ) ) {
    SocketOp.read( sh, &b, 1 );
    if( XmlhOp.read( xmlh, &b, 1 ) ) {
      return True;
    }
    else if( XmlhOp.isError( xmlh ) ){
      return False;
    }
  };
  return False;
}


static void __cmdReader( void* threadinst ) {
  iOThread         th = (iOThread)threadinst;
  __iOClntService   o = (__iOClntService)ThreadOp.getParm(th);
  iOClntCon   clntcon = o->ClntCon;
  char*         sname = NULL;
  Boolean          ok = False;
  iOThread infoWriter = NULL;
  iOXmlh         xmlh = XmlhOp.inst( False, NULL, NULL );
  char*           cmd = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cmdReader started for:%s.", SocketOp.getPeername(o->clntSocket) );

  /* InfoWriter. TODO: sync Map access. */
  sname = StrOp.fmt( "infw%08X", o->clntSocket );
  infoWriter = ThreadOp.inst( sname, __infoWriter, o );
  ThreadOp.setDescription( infoWriter, SocketOp.getPeername(o->clntSocket) );

  ThreadOp.start( infoWriter );

  /* Lock the semaphore: */
  MutexOp.trywait( Data(clntcon)->muxMap, 1000 );
  MapOp.put( Data(clntcon)->infoWriters, sname, (obj)infoWriter );
  /* Unlock the semaphore: */
  MutexOp.post( Data(clntcon)->muxMap );

  ThreadOp.sleep( 1000 );
  do {
    char b;
    if( o->clntSocket == NULL ) {
      /* not jet initialized */
      ThreadOp.sleep( 100 );
      continue;
    }
    if( !SocketOp.peek( o->clntSocket, &b, 1 ) ) {
      if( SocketOp.isBroken( o->clntSocket ) ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                    "__readSiHdr Socket errno=%d", SocketOp.getRc( o->clntSocket ) );
        break;
      }
      ThreadOp.sleep( 10 );
      continue;
    }
    XmlhOp.reset( xmlh );
    if( ok = __readXmlh( o->clntSocket, xmlh ) ) {
      long size = XmlhOp.getSizeByTagName( xmlh, XmlhOp.xml_tagname, 0 );
      int len = 0;
      freeMem( cmd );
      cmd = allocMem( size + 1 );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "cmdReader: reading %d bytes...", size );
      SocketOp.read( o->clntSocket, cmd, size );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "cmdReader: %d bytes read", SocketOp.getReceived(o->clntSocket));
      cmd[size] = '\0';
      len = StrOp.len( cmd );
      if( len > 0 ) {
        iODoc doc = DocOp.parse( cmd );
        if( doc != NULL ) {
          iONode nodeA = DocOp.getRootNode( doc );
          if( cmd[len-1] == '\n' ) cmd[len-1] = '\0';
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "cmdReader[%d,%d] %.256s", size, len, cmd );
          if( nodeA != NULL ) {
            const char* thName = ThreadOp.getName( infoWriter );
            wCommand.setserver( nodeA, thName );

            if(StrOp.equals( wModelCmd.name(), NodeOp.getName(nodeA) ) && StrOp.equals( wModelCmd.plan, wCommand.getcmd( nodeA ) ) ) {
              /* inform broadcaster */
              o->disablemonitor = wModelCmd.isdisablemonitor(nodeA);
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "monitoring for client is %s", o->disablemonitor?"off":"on" );
            }

            if( !o->readonly ||
                StrOp.equals( wDataReq.name(), NodeOp.getName(nodeA) ) ||
                (StrOp.equals( wModelCmd.name(), NodeOp.getName(nodeA) ) && StrOp.equals( wModelCmd.plan, wCommand.getcmd( nodeA ) ) ) ||
                (StrOp.equals( wModelCmd.name(), NodeOp.getName(nodeA) ) && StrOp.equals( wModelCmd.fstat, wCommand.getcmd( nodeA ) ) )
            )
            {
              Data(o->ClntCon)->callback( Data(o->ClntCon)->callbackObj, nodeA );
            }
            else {
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                  "readonly mode for %s:\n%.256s", SocketOp.getPeername(o->clntSocket), cmd );
            }

          }
          doc->base.del( doc );
        }
        else {
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "could not parse:\n%.256s", cmd );
        }
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Client %s is gone.", sname );
      break;
    }
    freeMem( cmd );
    cmd = NULL;
    ThreadOp.sleep( 10 );
  } while( !o->quit );

  /* Cleanup. */
  if( cmd != NULL )
    freeMem(cmd);
  /* Lock the semaphore: */
  MutexOp.trywait( Data(clntcon)->muxMap, 1000 );
  {
    iOThread iw = (iOThread)MapOp.get( Data(clntcon)->infoWriters, sname );
    if( iw != NULL ) {
      iONode quitNode = NULL;
      MapOp.remove( Data(clntcon)->infoWriters, sname );
      quitNode = NodeOp.inst( "quit", NULL, ELEMENT_NODE );
      ThreadOp.post( iw, (obj)quitNode );
      o->quit = True;
    }
  }
  /* Unlock the semaphore: */
  MutexOp.post( Data(clntcon)->muxMap );


  /* InfoWriter should cleanup the socket object. */
  StrOp.free( sname );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Server ended." );
  XmlhOp.base.del( xmlh );
  ThreadOp.base.del( th );
}


static void __manager( void* threadinst ) {
  iOThread       th = (iOThread)threadinst;
  iOClntCon clntcon = (iOClntCon)ThreadOp.getParm(th);
  iOClntConData data = Data(clntcon);

  ThreadOp.setDescription( th, "ClientCon Manager" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Manager started." );

  do {
    iOThread cmdReader = NULL;
    iOSocket    client = SocketOp.accept( Data(clntcon)->srvrsocket );

    if( client != NULL ) {
      char*      servername = NULL;
      __iOClntService cargo = allocMem( sizeof( struct __OClntService ) );
      cargo->ClntCon    = clntcon;
      cargo->clntSocket = client;
      cargo->readonly   = wTcp.isonlyfirstmaster(data->ini);
      cargo->quit       = False;

      if( cargo->readonly && MapOp.size( data->infoWriters ) == 0 ) {
        cargo->readonly = False;
      }

      data->concount++;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "client connect count: %d", data->concount );

      servername        = StrOp.fmt( "cmdr%08X", client );
      cmdReader         = ThreadOp.inst( servername, __cmdReader, cargo );
      ThreadOp.setDescription( cmdReader, SocketOp.getPeername(client) );

      ThreadOp.start( cmdReader );
      StrOp.free( servername );
    }
    else
      break;
    ThreadOp.sleep( 10 );
  } while( True );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Manager ended." );
}


static void __stress( void* threadinst ) {
  iOThread       th = (iOThread)threadinst;
  iOClntCon clntcon = (iOClntCon)ThreadOp.getParm(th);
  iOClntConData data = Data(clntcon);

  ThreadOp.setDescription( th, "ClientCon Stress" );
  ThreadOp.sleep( 5000 );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Stress started." );

  do {
    iONode node = NodeOp.inst( wException.name(), NULL, ELEMENT_NODE );
    wException.settext(node, "STRESS!!!");
    wException.setlevel( node, TRCLEVEL_MONITOR );
    AppOp.broadcastEvent( node );

    ThreadOp.sleep( 100 );
    node = NodeOp.inst( wException.name(), NULL, ELEMENT_NODE );
    wException.settext(node, "Warning!!!");
    wException.setlevel( node, TRCLEVEL_WARNING );
    AppOp.broadcastEvent( node );

    ThreadOp.sleep( 100 );
    node = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    /* test values: change to fit */
    wLoc.setid(node, "G&O");
    wLoc.setdestblockid( node, "2");
    wLoc.setaddr(node, 4136);
    wLoc.setV(node, 3);
    AppOp.broadcastEvent( node );

    ThreadOp.sleep( 100 );
  } while( True );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Stress ended." );
}

/*
 ***** _Public functions.
 */
static void __doBroadcast( iOClntCon inst, iONode nodeDF ) {
  if( inst != NULL && MutexOp.trywait( Data(inst)->muxMap, 1000 ) ) {
    iOClntConData data = Data(inst);
    iOThread iw = (iOThread)MapOp.first( data->infoWriters );
    while( iw != NULL ) {
      __iOClntService param = (__iOClntService)ThreadOp.getParm(iw);
      if( param->disablemonitor && StrOp.equals( NodeOp.getName(nodeDF), wException.name() ) ) {
        /* skipping this broadcast for the client */
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Skipping exception broadcast for %s.", ThreadOp.getName(iw) );
      }
      else {
        iONode clone = (iONode)nodeDF->base.clone( nodeDF );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "broadcasting %s...", NodeOp.getName(clone) );
        if( !ThreadOp.post( iw, (obj)clone ) ) {
          NodeOp.base.del(clone);
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Unable to broadcast event to %s; removing from list.", ThreadOp.getName(iw) );
          MapOp.remove( data->infoWriters, ThreadOp.getName(iw) );
          iw = (iOThread)MapOp.first( data->infoWriters );
        }
      }
      if( iw != NULL )
        iw = (iOThread)MapOp.next( data->infoWriters );
      ThreadOp.sleep( 0 );
    }
    /* Unlock the semaphore: */
    MutexOp.post( data->muxMap );
  }

  nodeDF->base.del(nodeDF);
}


static void __broadcaster( void* threadinst ) {
  iOThread       th = (iOThread)threadinst;
  iOClntCon clntcon = (iOClntCon)ThreadOp.getParm(th);
  iOClntConData data = Data(clntcon);

  ThreadOp.setDescription( th, "ClientCon Broadcaster" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Broadcaster started." );

  do {
    obj post = ThreadOp.waitPost( th );
    if( post != NULL ) {
      iONode node = (iONode)post;
      __doBroadcast(clntcon, (iONode)post);
    }
    else
      ThreadOp.sleep( 10 );

    ThreadOp.sleep( 0 );
  } while(True);
}

static void _broadcastEvent( iOClntCon inst, iONode nodeDF ) {
  if( inst != NULL ) {
    iOClntConData data = Data(inst);
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Broadcast event...%s", NodeOp.getName(nodeDF) );
    if( !ThreadOp.post( data->broadcaster, (obj)nodeDF ) ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Unable to broadcast event!" );
    }
  }
}


/* nodeDF is already cloned! */
static void _postEvent( iOClntCon inst, iONode nodeDF, const char* iwname )
{
  iOClntConData data = Data(inst);
  /* Bugfix: If plan.xml is empty (0 bytes), rocgui will make rocrail crash here else */
  if (!nodeDF)
  {
    return;
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "ClntCon post %s to %s...",
                NodeOp.getName(nodeDF), iwname );
  if( inst != NULL && MutexOp.trywait( data->muxMap, 1000 ) ) {
    iOThread iw = (iOThread)MapOp.get( data->infoWriters, iwname );
    if( iw != NULL ) {
      ThreadOp.post( iw, (obj)nodeDF );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "InfoWriter %s not found!", iwname );
      nodeDF->base.del(nodeDF);
    }
    /* Unlock the semaphore: */
    MutexOp.post( data->muxMap );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "ClntCon not initialized! (%s)", iwname );
    nodeDF->base.del(nodeDF);
  }
}

static void _setCallback( iOClntCon inst, clntcon_callback pfun, obj callbackObj ) {
  iOClntConData data = Data(inst);
  data->callback    = pfun;
  data->callbackObj = callbackObj;
}

static int _getClientCount( iOClntCon inst ) {
  iOClntConData data = Data(inst);
  return MapOp.size( data->infoWriters );
}

static int _getClientPort( iOClntCon inst ) {
  iOClntConData data = Data(inst);
  return data->port;
}

static const char* _getClientHost( iOClntCon inst ) {
  iOClntConData data = Data(inst);
  return SocketOp.gethostname(); /* return the name; DHCP server should solve it */
}

static iOClntCon _inst( iONode ini, int port, clntcon_callback pfun, obj callbackObj ) {
  iOClntCon     clntcon = allocMem( sizeof( struct OClntCon ) );
  iOClntConData data    = allocMem( sizeof( struct OClntConData ) );

  /* OBase operations */
  MemOp.basecpy( clntcon, &ClntConOp, 0, sizeof( struct OClntCon ), data );

  data->ini         = ini;
  data->port        = port;
  data->srvrsocket  = SocketOp.inst( "localhost", port, False, False, False );
  data->callback    = pfun;
  data->callbackObj = callbackObj;

  data->infoWriters = MapOp.inst();
  data->muxMap      = MutexOp.inst( NULL, True );

  instCnt++;

  data->manager = ThreadOp.inst( "cconmngr", __manager, clntcon );
  data->broadcaster = ThreadOp.inst( "broadcast", __broadcaster, clntcon );
  ThreadOp.start( data->manager );
  ThreadOp.start( data->broadcaster );

  /* TEST THREAD
  data->stress = ThreadOp.inst( "cconstress", __stress, clntcon );
  ThreadOp.start( data->stress );
  */

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ClientConnection started on port %d.", port );

  return clntcon;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/clntcon.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
