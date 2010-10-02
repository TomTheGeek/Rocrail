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
#include <sys/time.h>

#include "rocrail/impl/srcpcon_impl.h"

#include "rocrail/public/app.h"
#include "rocrail/public/model.h"
#include "rocrail/public/loc.h"

#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/xmlh.h"

#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/AutoCmd.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Tcp.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/DataReq.h"
#include "rocrail/wrapper/public/Exception.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/SrcpCon.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/State.h"


static int instCnt = 0;

static int idCnt = 1;
struct __OSrcpService {
  iOSrcpCon     SrcpCon;
  iOSocket      clntSocket;
  Boolean       readonly;
  Boolean       quit;
  Boolean       disablemonitor;
  int           id;
};
typedef struct __OSrcpService* __iOSrcpService;

static const char* SRCPVERSION="SRCP 0.8.3;Rocrail 2.0";

/*
200 OK <ID>
201 OK PROTOCOL SRCP
202 OK CONNECTIONMODEOK
400 ERROR unsupported protocol
401 ERROR unsupported connection mode
402 ERROR unsufficient data
500 ERROR out of ressources

100 INFO <more data>
101 INFO <more data>
102 INFO <more data>
200 OK
410 ERROR unknown command
411 ERROR unknown value
412 ERROR wrong value
414 ERROR device locked
415 ERROR forbidden
416 ERROR no data
417 ERROR timeout
418 ERROR list too long
419 ERROR list too short
420 ERROR unsupported device protocol
421 ERROR unsupported device
422 ERROR unsupported device group
423 ERROR unsupported operation
424 ERROR device reinitialized
425 ERROR not supported
499 ERROR unspecified error
*/


/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOSrcpConData data = Data(inst);
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

/** ----- OSrcpCon ----- */


static char* __rr2srcp(iONode evt, char* str) {
  struct timeval time;
  gettimeofday(&time, NULL);
  iOModel model = AppOp.getModel();

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "[%s] broadcast event", NodeOp.getName(evt) );

  if( StrOp.equals( wSwitch.name(), NodeOp.getName(evt))) {
    iOSwitch sw = ModelOp.getSwitch(model, wSwitch.getid(evt));
    if( sw != NULL ) {
      iONode swProps = SwitchOp.base.properties(sw);
      int addr = ((wSwitch.getaddr1(swProps)-1) * 4) + wSwitch.getport1(swProps);

      /*100 INFO <bus> GA <addr> <port> <value>*/
      StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n",
          time.tv_sec, time.tv_usec / 1000,
          100, 1, addr, StrOp.equals(wSwitch.getstate(evt), wSwitch.straight)? 1:0);
    }
  }

  else if( StrOp.equals( wState.name(), NodeOp.getName(evt))) {
    /*100 INFO <bus> POWER ON/OFF <freetext>*/
    StrOp.fmtb(str, "%lu.%.3lu %d INFO %d POWER %s\n",
        time.tv_sec, time.tv_usec / 1000,
        100, 1, wState.ispower(evt)?"ON":"OFF" );
  }

  else if( StrOp.equals( wFeedback.name(), NodeOp.getName(evt))) {
    /*100 INFO <bus> FB <addr> <value>*/
    StrOp.fmtb(str, "%lu.%.3lu %d INFO %d FB %d %d\n",
        time.tv_sec, time.tv_usec / 1000,
        100, 1, wFeedback.getaddr(evt), wFeedback.isstate(evt));
  }
  else if( StrOp.equals( wLoc.name(), NodeOp.getName(evt))) {

  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "[%s]", str );
  return str;
}


static iONode __srcp2rr(const char* req) {
  iONode cmd = NULL;
  iOModel model = AppOp.getModel();

  if( StrOp.findi( req, "POWER" ) && StrOp.findi( req, "ON" ) ) {
    cmd = NodeOp.inst(wSysCmd.name(), NULL, ELEMENT_NODE );
    wSysCmd.setcmd( cmd, wSysCmd.go );
  }
  else if( StrOp.findi( req, "POWER" ) && StrOp.findi( req, "OFF" ) ) {
    cmd = NodeOp.inst(wSysCmd.name(), NULL, ELEMENT_NODE );
    wSysCmd.setcmd( cmd, wSysCmd.stop );
  }
  else if( StrOp.startsWithi( req, "SET" ) ) {

    /* SET <bus> GL <addr> <drivemode> <V> <V_max> <f1> . . <fn> */
    if( StrOp.findi( req, "GL" ) ) {
      int idx = 0;
      const char* lcID = NULL;
      Boolean dir = True;
      int V = 0;
      iOStrTok tok = StrTokOp.inst(req, ' ');
      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 3: {
          iOLoc loco = ModelOp.getLocByAddress(model, atoi(s));
          if( loco != NULL ) {
            lcID = LocOp.getId(loco);
            dir = LocOp.getDir(loco);
          }
        }
        break;
        case 4:
          if( s[0] == '0') dir = False;
          if( s[0] == '1') dir = True;
          break;
        case 5:
          V = atoi(s);
          break;
        }

        idx++;
      };
      StrTokOp.base.del(tok);

      if( lcID != NULL ) {
        cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setid(cmd, lcID);
        wLoc.setdir(cmd, dir);
        wLoc.setV(cmd, V);
      }
    }

    /* SET <bus> GA <addr> <port> <value> <delay> */
    else if( StrOp.findi( req, "GA" ) ) {
      int idx = 0;
      const char* swID = NULL;
      int addr = 0;
      int port = 0;
      int gate = 0;
      iOStrTok tok = StrTokOp.inst(req, ' ');
      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 3: addr = atoi(s); break;
        case 4: gate = atoi(s); break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      /* Find switch */
      port = (addr-1) % 4 + 1;
      addr = (addr-1) / 4 + 1;

      iOSwitch sw = ModelOp.getSwByAddress(model, addr, port);
      if( sw != NULL ) {
        cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
        wSwitch.setid( cmd, SwitchOp.getId(sw) );
        wSwitch.setcmd( cmd, gate?wSwitch.straight:wSwitch.turnout );
      }

    }


  }

  return cmd;
}


/**  */
static void _broadcastEvent( struct OSrcpCon* inst ,iONode evt ) {
  iOSrcpConData data = Data(inst);
  if( !ThreadOp.post( data->broadcaster, (obj)evt ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Unable to broadcast event!" );
  }
  return;
}


/**  */
static int _getClientCount( struct OSrcpCon* inst ) {
  return 0;
}


/**  */
static const char* _getClientHost( struct OSrcpCon* inst ) {
  return 0;
}


/**  */
static int _getClientPort( struct OSrcpCon* inst ) {
  return 0;
}


static void __doBroadcast( iOSrcpCon inst, iONode nodeDF ) {
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Broadcast received." );
  if( inst != NULL && MutexOp.trywait( Data(inst)->muxMap, 100 ) ) {
    iOSrcpConData data = Data(inst);

    iOThread iw = (iOThread)MapOp.first( data->infoWriters );
    while( iw != NULL ) {
      iONode clone = (iONode)nodeDF->base.clone( nodeDF );
      ThreadOp.post( iw, (obj)clone );
      ThreadOp.sleep( 0 );
      iw = (iOThread)MapOp.next( data->infoWriters );
    }
    /* Unlock the semaphore: */
    MutexOp.post( data->muxMap );
  }

  nodeDF->base.del(nodeDF);
}


static void __broadcaster( void* threadinst ) {
  iOThread       th = (iOThread)threadinst;
  iOSrcpCon srcpcon = (iOSrcpCon)ThreadOp.getParm(th);
  iOSrcpConData data = Data(srcpcon);

  ThreadOp.setDescription( th, "SRCP Client Broadcaster" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Broadcaster started." );

  do {
    obj post = ThreadOp.waitPost( th );
    if( post != NULL ) {
      iONode node = (iONode)post;

      __doBroadcast(srcpcon, node);

    }
    else
      ThreadOp.sleep( 10 );

    ThreadOp.sleep( 0 );
  } while(True);
}

static const char* srcpFmtMsg(int errorcode, char *msg, struct timeval time, int id)
{
    switch (errorcode) {
        case 100:
            sprintf(msg, "%lu.%.3lu %d INFO\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 101:
            sprintf(msg, "%lu.%.3lu %d INFO\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 102:
            sprintf(msg, "%lu.%.3lu %d INFO\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 110:
            sprintf(msg, "%lu.%.3lu %d INFO\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 200:
          if( id > 0 )
            sprintf(msg, "%lu.%.3lu %d OK GO %d\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode, id);
          else
            sprintf(msg, "%lu.%.3lu %d OK\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);

            break;
        case 202:
            sprintf(msg, "%lu.%.3lu %d OK CONNECTIONMODE\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 201:
            sprintf(msg, "%lu.%.3lu %d OK PROTOCOL SRCP\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 400:
            sprintf(msg, "%lu.%.3lu %d ERROR unsupported protocol\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
            break;
        case 401:
            sprintf(msg,
                    "%lu.%.3lu %d ERROR unsupported connection mode\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
            break;
        case 402:
            sprintf(msg, "%lu.%.3lu %d ERROR insufficient data\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
            break;
        case 410:
            sprintf(msg, "%lu.%.3lu %d ERROR unknown command\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
            break;
        case 411:
            sprintf(msg, "%lu.%.3lu %d ERROR unknown value\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 412:
            sprintf(msg, "%lu.%.3lu %d ERROR wrong value\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 413:
            sprintf(msg, "%lu.%.3lu %d ERROR temporarily prohibited\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
            break;
        case 414:
            sprintf(msg, "%lu.%.3lu %d ERROR device locked\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 415:
            sprintf(msg, "%lu.%.3lu %d ERROR forbidden\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 416:
            sprintf(msg, "%lu.%.3lu %d ERROR no data\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 417:
            sprintf(msg, "%lu.%.3lu %d ERROR timeout\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 418:
            sprintf(msg, "%lu.%.3lu %d ERROR list too long\n", time.tv_sec,
                    time.tv_usec / 1000, errorcode);
            break;
        case 419:
            sprintf(msg, "%lu.%.3lu %d ERROR list too short\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
            break;
        case 420:
            sprintf(msg,
                    "%lu.%.3lu %d ERROR unsupported device protocol\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
            break;
        case 421:
            sprintf(msg, "%lu.%.3lu %d ERROR unsupported device\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
            break;
        case 422:
            sprintf(msg, "%lu.%.3lu %d ERROR unsupported device group\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
            break;
        case 423:
            sprintf(msg, "%lu.%.3lu %d ERROR unsupported operation\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
            break;
        case 424:
            sprintf(msg, "%lu.%.3lu %d ERROR device reinitialized\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
            break;
        case 500:
            sprintf(msg, "%lu.%.3lu %d ERROR out of resources\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
            break;
        default:
            sprintf(msg, "%lu.%.3lu 600 ERROR internal error %d, "
                    "please report to srcpd-devel@srcpd.sorceforge.net\n",
                    time.tv_sec, time.tv_usec / 1000, errorcode);
    }
    return msg;
}

static void __evalRequest(iOSrcpCon srcpcon, __iOSrcpService o, const char* req) {
  char rsp[1025] = {'\0'};
  struct timeval time;
  gettimeofday(&time, NULL);

  if( StrOp.startsWithi( req, "SET CONNECTIONMODE" ) ) {
    SocketOp.fmt(o->clntSocket, srcpFmtMsg(202, rsp, time, 0));
  }
  else if( StrOp.startsWithi( req, "GO" ) ) {
    SocketOp.fmt(o->clntSocket, srcpFmtMsg(200, rsp, time, o->id));
  }
  else if( StrOp.startsWithi( req, "TERM 0" ) ) {
    SocketOp.fmt(o->clntSocket, srcpFmtMsg(200, rsp, time, 0));
  }
  else {
    iONode cmd = __srcp2rr(req);
    if( cmd != NULL )
      Data(srcpcon)->callback( Data(srcpcon)->callbackObj, cmd );
    SocketOp.fmt(o->clntSocket, srcpFmtMsg(200, rsp, time, 0));
  }
}

static void __SrcpService( void* threadinst ) {
  iOThread         th = (iOThread)threadinst;
  __iOSrcpService   o = (__iOSrcpService)ThreadOp.getParm(th);
  iOSrcpCon   srcpcon = o->SrcpCon;
  char*         sname = NULL;
  Boolean          ok = False;
  iOThread infoWriter = NULL;

  ThreadOp.setDescription( th, "SRCP Client command reader" );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP service started for: %s, with session ID %d",
      SocketOp.getPeername(o->clntSocket), o->id );
  SocketOp.write( o->clntSocket, SRCPVERSION, StrOp.len(SRCPVERSION) );
  SocketOp.write( o->clntSocket, "\n", 1 );

  sname = StrOp.fmt( "srcp%08X", o->clntSocket );

  /* Lock the semaphore: */
  if( MutexOp.trywait( Data(srcpcon)->muxMap, 100 ) ) {
    MapOp.put( Data(srcpcon)->infoWriters, sname, (obj)threadinst );
    /* Unlock the semaphore: */
    MutexOp.post( Data(srcpcon)->muxMap );
  }

  do {
    char str[1025] = {'\0'};

    obj post = ThreadOp.getPost( th );
    if( post != NULL ) {
      iONode node = (iONode)post;
      if( node != NULL ) {
        __rr2srcp(node, str);
        SocketOp.write( o->clntSocket, str, StrOp.len(str) );
        NodeOp.base.del(node);
      }
    }

    if( !SocketOp.peek( o->clntSocket, str, 1 ) ) {
      if( SocketOp.isBroken( o->clntSocket ) ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                    "SRCP Service: Socket errno=%d", SocketOp.getRc( o->clntSocket ) );
        break;
      }
      ThreadOp.sleep( 10 );
      continue;
    }

    SocketOp.readln(o->clntSocket, str);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, str );
    __evalRequest( srcpcon, o, str);
    ThreadOp.sleep(10);

  } while( !o->quit );
  /*} while( !o->quit && SocketOp.isConnected(o->clntSocket) && !SocketOp.isBroken(o->clntSocket));*/


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP service ended for session [%d]", o->id );

  /* Lock the semaphore: */
  if( MutexOp.trywait( Data(srcpcon)->muxMap, 100 ) ) {
    MapOp.remove( Data(srcpcon)->infoWriters, sname );
    /* Unlock the semaphore: */
    MutexOp.post( Data(srcpcon)->muxMap );
  }

  SocketOp.base.del(o->clntSocket);
  freeMem(o);
  ThreadOp.base.del( th );
}


static void __manager( void* threadinst ) {
  iOThread       th = (iOThread)threadinst;
  iOSrcpCon srcpcon = (iOSrcpCon)ThreadOp.getParm(th);
  iOSrcpConData data = Data(srcpcon);

  ThreadOp.setDescription( th, "SRCP Client Manager" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Manager started." );

  do {
    iOThread SrcpService = NULL;
    iOSocket    client = SocketOp.accept( Data(srcpcon)->srvrsocket );

    if( client != NULL ) {
      char*      servername = NULL;
      __iOSrcpService cargo = allocMem( sizeof( struct __OSrcpService ) );
      cargo->SrcpCon    = srcpcon;
      cargo->clntSocket = client;
      cargo->quit       = False;
      cargo->id         = idCnt;
      idCnt++;

      servername        = StrOp.fmt( "cmdr%08X", client );
      SrcpService         = ThreadOp.inst( servername, __SrcpService, cargo );
      ThreadOp.setDescription( SrcpService, SocketOp.getPeername(client) );

      ThreadOp.start( SrcpService );
      StrOp.free( servername );
    }
    else
      break;
    ThreadOp.sleep( 10 );
  } while( True );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Manager ended." );
}





/**  */
static struct OSrcpCon* _inst( iONode ini, srcpcon_callback callbackfun, obj callbackobj ) {
  iOSrcpCon __SrcpCon = allocMem( sizeof( struct OSrcpCon ) );
  iOSrcpConData data = allocMem( sizeof( struct OSrcpConData ) );
  MemOp.basecpy( __SrcpCon, &SrcpConOp, 0, sizeof( struct OSrcpCon ), data );

  /* Initialize data->xxx members... */
  data->ini         = ini;
  data->port        = wSrcpCon.getport(ini);
  data->srvrsocket  = SocketOp.inst( "localhost", data->port, False, False, False );
  data->callback    = callbackfun;
  data->callbackObj = callbackobj;

  data->infoWriters = MapOp.inst();
  data->muxMap      = MutexOp.inst( NULL, True );

  instCnt++;

  data->manager = ThreadOp.inst( "srcpmngr", __manager, __SrcpCon );
  data->broadcaster = ThreadOp.inst( "srcpbrdcst", __broadcaster, __SrcpCon );
  ThreadOp.start( data->manager );
  ThreadOp.start( data->broadcaster );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP Client connection started on port %d.", wSrcpCon.getport(ini) );

  instCnt++;
  return __SrcpCon;
}


/**  */
static void _postEvent( struct OSrcpCon* inst ,iONode evt ,const char* iwname ) {
  return;
}


/**  */
static void _setCallback( struct OSrcpCon* inst ,clntcon_callback callbackfun ,obj callbackobj ) {
  return;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/srcpcon.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
