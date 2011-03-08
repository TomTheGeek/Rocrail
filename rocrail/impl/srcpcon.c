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
#include <time.h>

#include "rocrail/impl/srcpcon_impl.h"

#include "rocrail/public/app.h"
#include "rocrail/public/model.h"
#include "rocrail/public/loc.h"
#include "rocrail/public/srcpcon.h"

#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/xmlh.h"

#include "rocutils/public/addr.h"

#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/AutoCmd.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Tcp.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/DataReq.h"
#include "rocrail/wrapper/public/Exception.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/FunDef.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/SrcpCon.h"
#include "rocrail/wrapper/public/SrcpConOffset.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/Block.h"


static int instCnt = 0;

static int idCnt = 1;
struct __OSrcpService {
  iOSrcpCon     SrcpCon;
  iOSocket      clntSocket;
  Boolean       readonly;
  Boolean       quit;
  Boolean       disablemonitor;
  int           id;
  Boolean	infomode;
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

static char* __rr2srcp(iOSrcpConData data, iONode evt, char* str) {
  struct timeval time;
  gettimeofday(&time, NULL);
  iOModel model = AppOp.getModel();

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "[%s] broadcast event", NodeOp.getName(evt) );

  if( StrOp.equals( wSwitch.name(), NodeOp.getName(evt))) {
    iOSwitch sw = ModelOp.getSwitch(model, wSwitch.getid(evt));
    if( sw != NULL ) {
      iONode swProps = SwitchOp.base.properties(sw);
      int addr  = AddrOp.toPADA( wSwitch.getaddr1(swProps), wSwitch.getport1(swProps) );
      int addr2 = AddrOp.toPADA( wSwitch.getaddr2(swProps), wSwitch.getport2(swProps) );

      if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.left) 
       || StrOp.equals( wSwitch.gettype(swProps), wSwitch.right) 
       || StrOp.equals( wSwitch.gettype(swProps), wSwitch.crossing) 
       || StrOp.equals( wSwitch.gettype(swProps), wSwitch.ccrossing)) {
        /*100 INFO <bus> GA <addr> <port> <value>*/
        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n",
            time.tv_sec, time.tv_usec / 1000,
            100, 1, addr, StrOp.equals(wSwitch.getstate(evt), wSwitch.straight)? 1:0);
      }
      else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.threeway) 
            || StrOp.equals( wSwitch.gettype(swProps), wSwitch.dcrossing)) {
        if( StrOp.equals( wSwitch.getstate(evt), wSwitch.straight)) {
        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 1\n",
            time.tv_sec, time.tv_usec / 1000,
            100, 1, addr, 0,
            time.tv_sec, time.tv_usec / 1000,
            100, 1, addr2, 0);
        }
        else if( StrOp.equals( wSwitch.getstate(evt), wSwitch.left)) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 1\n",
              time.tv_sec, time.tv_usec / 1000,
              100, 1, addr2, 0,
              time.tv_sec, time.tv_usec / 1000,
              100, 1, addr, 1);
        }
        else if( StrOp.equals( wSwitch.getstate(evt), wSwitch.right)) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 1\n",
              time.tv_sec, time.tv_usec / 1000,
              100, 1, addr, 0,
              time.tv_sec, time.tv_usec / 1000,
              100, 1, addr2, 1);
        }
        else if( StrOp.equals( wSwitch.getstate(evt), wSwitch.turnout)) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 1\n",
              time.tv_sec, time.tv_usec / 1000,
              100, 1, addr, 1,
              time.tv_sec, time.tv_usec / 1000,
              100, 1, addr2, 1);
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UNHANDLED swEvent type %s, SWaddr1 %d, SWaddr2 %d, stateE %s, stateP %s", wSwitch.gettype(swProps) , addr, addr2, wSwitch.getstate(evt), wSwitch.getstate(swProps));
        }
      }
      else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.decoupler )) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "TYPE %s # %d set to %d", wSwitch.gettype(swProps), addr, StrOp.equals(wSwitch.getstate(evt), wSwitch.straight)? 1:0 );
        if( ! wSwitch.issinglegate(evt)){
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n",
              time.tv_sec, time.tv_usec / 1000,
              100, 1, addr, StrOp.equals(wSwitch.getstate(evt), wSwitch.straight)? 1:0);
        }else {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n",
              time.tv_sec, time.tv_usec / 1000,
              100, 1, addr, wSwitch.getport1(evt));
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UNHANDLED swDevice typeP %s, SWaddr1 %d, SWaddr2 %d, stateE %s, stateP %s", wSwitch.gettype(swProps) , addr, addr2, wSwitch.getstate(evt), wSwitch.getstate(swProps));
      }
    }
  }

  else if( StrOp.equals( wSignal.name(), NodeOp.getName(evt))) {
    iOSignal sg = ModelOp.getSignal(model, wSignal.getid(evt));
    if( sg != NULL ) {
      iONode sgProps = SignalOp.base.properties(sg);
      int addr = AddrOp.toPADA( wSignal.getaddr(sgProps), wSignal.getport1(sgProps) );
      int aspects = wSignal.getaspects( sgProps );

      /*100 INFO <bus> GA <addr> <port> <value>*/
      if( aspects == 2 ) {
        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n",
            time.tv_sec, time.tv_usec / 1000,
            100, 1, addr, StrOp.equals(wSignal.getstate(evt), wSignal.red)? 0:1);
      }else if( aspects > 2 ) {
        int addr2 = AddrOp.toPADA( wSignal.getaddr2(sgProps), wSignal.getport2(sgProps) );
        int addr3 = AddrOp.toPADA( wSignal.getaddr3(sgProps), wSignal.getport3(sgProps) );
        int addr4 = AddrOp.toPADA( wSignal.getaddr4(sgProps), wSignal.getport4(sgProps) );

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sgEvent typeP %s, SGaddr1 %d, SGaddr2 %d, SGaddr3 %d, SGaddr4 %d, stateE %s, stateP %s", wSignal.gettype(sgProps) , addr, addr2, addr3, addr4, wSignal.getstate(evt), wSignal.getstate(sgProps));

        if( StrOp.equals( wSignal.getstate(evt), wSignal.red)) {
        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n",
            time.tv_sec, time.tv_usec / 1000,
            100, 1, addr, 0);
        }
        else if( StrOp.equals( wSignal.getstate(evt), wSignal.green)) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n",
              time.tv_sec, time.tv_usec / 1000,
              100, 1, addr2, 1);
        }
        else if( StrOp.equals( wSignal.getstate(evt), wSignal.yellow)) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n",
              time.tv_sec, time.tv_usec / 1000,
              100, 1, addr3, 0);
        }
        else if( StrOp.equals( wSignal.getstate(evt), wSignal.white)) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n",
              time.tv_sec, time.tv_usec / 1000,
              100, 1, addr4, 1);
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UNHANDLED sgEvent typeP %s, SWaddr1 %d, SWaddr2 %d, stateE %s, stateP %s", wSignal.gettype(sgProps) , addr, addr2, wSignal.getstate(evt), wSignal.getstate(sgProps));
        }
      }else {
        int addr2 = AddrOp.toPADA( wSignal.getaddr2(sgProps), wSignal.getport2(sgProps) );

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UNHANDLED sgEvent typeP %s, SWaddr1 %d, SWaddr2 %d, stateE %s, stateP %s", wSignal.gettype(sgProps) , addr, addr2, wSignal.getstate(evt), wSignal.getstate(sgProps));
      }
    }
  }

  else if( StrOp.equals( wState.name(), NodeOp.getName(evt))) {
    const char *iid = wState.getiid(evt);
    if( iid != NULL ) {
      int s88busOffset = 0;

      iONode srcpconoffset = wSrcpCon.getsrcpconoffset(data->ini);
      while( srcpconoffset != NULL ) {
        if( StrOp.equals( iid, wSrcpConOffset.getiid( srcpconoffset ) ) ) {
          s88busOffset = wSrcpConOffset.getoffset( srcpconoffset );
          break;
        }
        srcpconoffset = wSrcpCon.nextsrcpconoffset(data->ini, srcpconoffset);
      }

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "StName %s EvName %s srcpconoffset %d PW %s", wState.name(), NodeOp.getName(evt), s88busOffset, wState.ispower(evt)?"ON":"OFF" );

      /*100 INFO <bus> POWER ON/OFF <freetext>*/
      StrOp.fmtb(str, "%lu.%.3lu %d INFO %d POWER %s\n",
        time.tv_sec, time.tv_usec / 1000,
        100, s88busOffset + 1, wState.ispower(evt)?"ON":"OFF" );
    }
  }

  else if( StrOp.equals( wClock.name(), NodeOp.getName(evt)) ) {
    long l_time = wClock.gettime(evt);
    struct tm* lTime = localtime( &l_time );

    int mins    = lTime->tm_min;
    int hours   = lTime->tm_hour;
    int day    = lTime->tm_mday;

    /*100 INFO 0 TIME <JulDay> <Hour> <Minute> <Second>*/
    StrOp.fmtb(str, "%lu.%.3lu %d INFO %d TIME %d %d %d %d\n",
        time.tv_sec, time.tv_usec / 1000,
        100, 1, day, hours, mins, 0 );
  }

  else if( StrOp.equals( wFeedback.name(), NodeOp.getName(evt))) {

    iOFBack fb = ModelOp.getFBack(model, wFeedback.getid(evt));
    if( fb != NULL ) {
      int s88busOffset = 0;
      iONode fbProps = FBackOp.base.properties(fb);

      iONode srcpconoffset = wSrcpCon.getsrcpconoffset(data->ini);
      while( srcpconoffset != NULL ) {
        if( StrOp.equals( wFeedback.getiid(fbProps), wSrcpConOffset.getiid( srcpconoffset ) ) ) {
          s88busOffset = wSrcpConOffset.getoffset( srcpconoffset );
          break;
        }
        srcpconoffset = wSrcpCon.nextsrcpconoffset(data->ini, srcpconoffset);
      }

      /*100 INFO <bus> FB <addr> <value>*/
      StrOp.fmtb(str, "%lu.%.3lu %d INFO %d FB %d %d\n",
          time.tv_sec, time.tv_usec / 1000,
          100, wFeedback.getbus(evt) + s88busOffset + 1, wFeedback.getaddr(evt), wFeedback.isstate(evt));
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "FBcmd [%s]", str );
    }
  }

  else if( StrOp.equals( wLoc.name(), NodeOp.getName(evt)) 
        || StrOp.equals( wFunCmd.name(), NodeOp.getName(evt))) {
    /* 100 INFO <bus> GL <addr>  <drivemode> <V>  <V_max> <f1> . . <fn> */
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "[%s] wLoc or wFunCmd detected", NodeOp.getName(evt) ); 

    iOLoc lo = ModelOp.getLoc(model, wLoc.getid(evt));
    if( lo != NULL ) {
      iONode loProps = LocOp.base.properties(lo);

      int   loAddr = wLoc.getaddr(loProps);
      Boolean loDir = wLoc.isdir(loProps);
      int   loV = wLoc.getV(loProps);
      int   loVmax = wLoc.getV_max(loProps);
      const char *loVmode = wLoc.getV_mode(loProps);
      int   loSpcnt = wLoc.getspcnt(loProps);
      Boolean loFn = wLoc.isfn( loProps );    

      const char *loCmd = wLoc.getcmd(loProps);
      const char *loDecType = wLoc.getdectype(loProps);
      const char *loDesc = wLoc.getdesc(loProps);
      int   loFnCnt = wLoc.getfncnt(loProps);
      int   loFx = wLoc.getfx(loProps);
      const char *loId = wLoc.getid(loProps);
      const char *loIid = wLoc.getiid(loProps);
      const char *loOid = wLoc.getoid(loProps);

      iOLoc loco = ModelOp.getLocByAddress(model, loAddr);
      int i, mask ;


      int decStep = (wLoc.getV( loProps ) * 127) / wLoc.getV_max( loProps );
      char funcString[1023];
      funcString[0] = '\0';

      for( i=0 ; i < loFnCnt ; i++ ) {
        mask = 1 << i ;
        funcString[2*i] = ' ';
        funcString[2*i+1] = loFx&mask?'1':'0';
      }
      funcString[2*loFnCnt] = '\0';

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "addr %d isdir %d V %d Vmode %s decStep %d spcnt %d loFx %d loId %s loFnCnt %d functions %s", 
              loAddr, loDir, loV, loVmode, decStep, loSpcnt, loFx, loId, loFnCnt, funcString );

      StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GL %d %d %d %d %d%s\n",
          time.tv_sec, time.tv_usec / 1000,
          100, wLoc.getbus(loProps) + 1, loAddr,  loDir?1:0, decStep, loSpcnt, loFn?1:0, funcString );
    }
  }

  else if( StrOp.equals( wFunDef.name(), NodeOp.getName(evt))) {
    /* */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "[%s] wFunDef detected", NodeOp.getName(evt) ); 
  }

  else if( StrOp.equals( wException.name(), NodeOp.getName(evt))){
    const char* text = wException.gettext( evt );
    int        level = wException.getlevel( evt );
      
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rr2srcp %s lvl %d text [%s]", NodeOp.getName(evt), level, text );
  }
  else if( StrOp.equals( wBlock.name(), NodeOp.getName(evt))){
    const char*   cmd = wBlock.getcmd( evt );
    int          addr = wBlock.getaddr( evt );
    const char* locId = wBlock.getlocid( evt );
    int	         port = wBlock.getport( evt );
    Boolean       pow = wBlock.ispower( evt );
      
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rr2srcp %s addr %d cmd [%s] locId [%s] port %d power %s", NodeOp.getName(evt), addr, cmd, locId, port, pow?"yes":"no" );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rr2srcp unhandled event [%s] returns [%s]", NodeOp.getName(evt), str );
  }

  return str;
}


static iONode __srcp2rr(iOSrcpCon srcpcon, __iOSrcpService o, const char* req, int *reqRespCode, iONode *cmd2) {
  iONode cmd = NULL;
  iOModel model = AppOp.getModel();
  struct timeval time;
  gettimeofday(&time, NULL);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "__srcp2rr: %s", req );

  /*
   * INIT <bus> FB <optional parameters for initialization>
   * INIT <bus> GA <addr>  <protocol> <optional further parameters>
   * INIT <bus> GL <addr>  <protocol> <optional further parameters>
   * INIT <bus> SM <protocol>
   * INIT <bus> POWER
   */
  if( StrOp.startsWithi( req, "INIT " ) ) {
    int idx = 0;
    int busId = 0;
    char busType[1025] = {'\0'};
    char optionString[1025] = {'\0'};
    int addrGA = 0 ;
    char protoGA[1025] = {'\0'};
    iOStrTok tok = StrTokOp.inst(req, ' ');

    while( StrTokOp.hasMoreTokens(tok)) {
      const char* s = StrTokOp.nextToken(tok);
      switch(idx) {
      case 1:
        busId = atoi(s) ;
        break;
      case 2:
        StrOp.copy( busType, s ) ;
        break;
      case 3:
        if( StrOp.equals( busType, "GA" ))
          addrGA = atoi( s );
        StrOp.copy( optionString, s ) ;
        break;
      case 4:
        if( StrOp.equals( busType, "GA" ))
          StrOp.copy( protoGA, s);
        break;
      case 5:
        if( StrOp.equals( busType, "GA" ))
          StrOp.copy( optionString, s ) ;
        break;
      }
      idx++;
    };
    StrTokOp.base.del(tok);
    /*
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Parts INIT %d %s %s", busId, busType, optionString );
    */
    if( StrOp.equals( busType, "POWER" )) {
      /* INIT <bus> POWER */
      /* Answer for init Power is "200 OK", generated automatically in calling function */
      *reqRespCode = (int) 200 ;
    }else if( StrOp.equals( busType, "GA" )) {
      /* INIT <bus> GA <addr>  <protocol> <optional further parameters> */
      int addr ;
      int port ;
      iOSwitch sw;
      iOSignal sg;

      AddrOp.fromPADA( addrGA, &addr, &port );
      sw = ModelOp.getSwByAddress(model, addr, port);
      sg = ModelOp.getSgByAddress(model, addr, port);

      if( sw || sg ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Valid GA at %d", addrGA );
      }else{
        /* ERROR 412 */
        *reqRespCode = (int) 412 ;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "INValid GA at %d, reqRespCode %d", addrGA, *reqRespCode );
      }
    }
  }

  else if( StrOp.startsWithi( req, "SET" ) ) {
    if( StrOp.findi( req, "POWER" ) ) {
      /* SET <bus> POWER [ON|OFF] [freetext] */
      int idx = 0;
      char str[1025] = {'\0'};
      int busId = 0;
      char busType[1025] = {'\0'};
      char optionString[1025] = {'\0'};
      char freeText[1025] = {'\0'};
      iOStrTok tok = StrTokOp.inst(req, ' ');

      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 1:
          busId = atoi(s) ;
          break;
        case 2:
          StrOp.copy( busType, s) ;
          break;
        case 3:
          StrOp.copy( optionString, s ) ;
          break;
        case 4:
          StrOp.copy( freeText, s);
          break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      if( StrOp.equals( busType, "POWER" ) && StrOp.equals( optionString, "OFF" ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "__srcp2rr: SET %d POWER OFF %s", busId, freeText);
        if( 1 == busId ) { /* We use only the srcp-bus number 1 (the bus with the master command station) to turn on/off power of the rcorail system */
          iONode localCmd = NodeOp.inst(wSysCmd.name(), NULL, ELEMENT_NODE );
          wSysCmd.setcmd( localCmd, wSysCmd.stop );
          Data(srcpcon)->callback( Data(srcpcon)->callbackObj, localCmd );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "wSysCmd.stop" );
        }

        StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d POWER OFF %s\n", time.tv_sec, time.tv_usec / 1000, busId, freeText );
        SocketOp.fmt(o->clntSocket, str);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP SEND: %s", str ) ;
        ThreadOp.sleep( 10 );

      } else if( StrOp.equals( busType, "POWER" ) && StrOp.equals( optionString, "ON" ) ) {
        /* SET <bus> POWER ON <freetext> */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "__srcp2rr: SET %d POWER ON %s", busId, freeText);
        if( 1 == busId ) { /* We use only the srcp-bus number 1 (the bus with the master command station) to turn on/off power of the rcorail system */
          iONode localCmd = NodeOp.inst(wSysCmd.name(), NULL, ELEMENT_NODE );
          wSysCmd.setcmd( localCmd, wSysCmd.go );
          Data(srcpcon)->callback( Data(srcpcon)->callbackObj, localCmd );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "wSysCmd.go" );
        }

        StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d POWER ON %s\n", time.tv_sec, time.tv_usec / 1000, busId, freeText );
        SocketOp.fmt(o->clntSocket, str);
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP SEND: %s", str ) ;
        ThreadOp.sleep( 10 );
      } else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unknown/unhandled POWER option in req %s", req );
      }
    }

    /* SET <bus> GL <addr> <drivemode> <V> <V_max> <f1> . . <fn> */
    else if( StrOp.findi( req, "GL" ) ) {
      int idx = 0;
      const char* lcID = NULL;
      int srcpLoco = 0;
      Boolean srcpDir = True;
      int srcpNewStep = 0;
      int srcpMaxStep = 0;
      Boolean srcpFn = False;
      int srcpFx = 0;

      iOStrTok tok = StrTokOp.inst(req, ' ');

      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 3: {
          iOLoc loco = NULL;
          srcpLoco = atoi(s);
          loco = ModelOp.getLocByAddress(model, srcpLoco);
          if( loco != NULL ) {
            lcID = LocOp.getId(loco);
          }
        }
        break;
        case 4:
          if( s[0] == '0') srcpDir = False;
          if( s[0] == '1') srcpDir = True;
          break;
        case 5:
          srcpNewStep = atoi(s);
          break;
        case 6:
          srcpMaxStep = atoi(s);
          break;
        case 7:
          if( s[0] == '0') srcpFn = False;
          if( s[0] == '1') srcpFn = True;
          break;
        }
        /* Functions F1, F2, .... start at text position 8 with a value of "1" representing an active function, 
           Fn is internally represented by bit 2^^(n-1) */
        if( ( idx >= 8 ) && ( s[0] == '1') ) {
          srcpFx |= 1 << (idx-8);
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      if( lcID != NULL ) {
        iONode loProps = LocOp.base.properties(ModelOp.getLoc(model, lcID));

        int loAddr   = wLoc.getaddr(loProps);
        int loV      = wLoc.getV(loProps);
        int loVmax   = wLoc.getV_max(loProps);
        int loSpcnt  = wLoc.getspcnt(loProps);
        int loOldStep  = 0 ;
        int loDir    = wLoc.isdir( loProps );
        Boolean loFn = wLoc.isfn( loProps );
        int loFnCnt  = wLoc.getfncnt(loProps);
        int loFx     = wLoc.getfx(loProps);

        int newSpeed = wLoc.getV(loProps) != -1 ? wLoc.getV(loProps):0;
        int newStep = 0 ;
        int divisor = 1 ;

        /* formulas for loOldStep from p50x.c */
        /* @ROB: Why 127 and not ( wLoc.getspcnt(loProps) -1 ) ??? */
        if( wLoc.getV( loProps ) != -1 ) {
          if( StrOp.equals( wLoc.getV_mode( loProps ), wLoc.V_mode_percent ) ){
            loOldStep = ( loV * 127) / 100;
            divisor = 100;
          }
          else if( loVmax > 0 ){
            loOldStep = ( loV * 127) / loVmax;
            divisor = loVmax;
          }

          newSpeed = loV;
          newStep = (newSpeed * 127) / divisor ;

          if( srcpNewStep == 0) { /* halt loco */
            /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "srcpNewStep %d == 0 %d loco halt", srcpNewStep); */
            newSpeed = 0;
          }
          else if( newStep == srcpNewStep ) {
              /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "newStep %d == srcpNewStep %d adjust none", newStep, srcpNewStep); */
          }
          else if( srcpNewStep > loOldStep ) { /* increase speed */
            /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "srcpNewStep %d > loOldStep %d", srcpNewStep, loOldStep); */
            while( newStep < srcpNewStep ) {
              /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "newStep %d < srcpNewStep %d adjust ++", newStep, srcpNewStep); */
              newSpeed++;
              newStep = (newSpeed * 127) / divisor ;
            }
          }
          else if( srcpNewStep < loOldStep ) { /* slow down */
            /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "srcpNewStep %d < loOldStep %d", srcpNewStep, loOldStep); */
            while( newStep > srcpNewStep ) {
              /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "newStep %d < srcpNewStep %d adjust --", newStep, srcpNewStep); */
              newSpeed--;
              newStep = (newSpeed * 127) / divisor ;
            }
          }
          newSpeed = ( newSpeed > loVmax ) ? loVmax : newSpeed;
        }
        /*
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loAddr %d : OldSpeed %d OldStep %d NewSpeed %d newStep %d",
              loAddr, loV, loOldStep, newSpeed, srcpNewStep);
        */
        cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setid(cmd, lcID);
        wLoc.setdir(cmd, srcpDir);
        wLoc.setfn(cmd, srcpFn);
        wLoc.setV(cmd, newSpeed);
        /* wLoc.setfx(cmd, srcpFx); *//* setfx doesn't work as I expected, so we have to generate and use a FunCmd */
        if( loFnCnt > 0 ) {
          int group=0;
          iONode fcmd = NodeOp.inst(wFunCmd.name(), NULL, ELEMENT_NODE );
          wFunCmd.setid( fcmd, lcID);
          wFunCmd.setgroup(fcmd, group+1 );
          switch( loFnCnt ) {
          case 8:
            wFunCmd.setf8(fcmd, (srcpFx & 0x80)?True:False);
          case 7:
            wFunCmd.setf7(fcmd, (srcpFx & 0x40)?True:False);
          case 6:
            wFunCmd.setf6(fcmd, (srcpFx & 0x20)?True:False);
          case 5:
            wFunCmd.setf5(fcmd, (srcpFx & 0x10)?True:False);
          case 4:
            wFunCmd.setf4(fcmd, (srcpFx & 0x08)?True:False);
          case 3:
            wFunCmd.setf3(fcmd, (srcpFx & 0x04)?True:False);
          case 2:
            wFunCmd.setf2(fcmd, (srcpFx & 0x02)?True:False);
          case 1:
            wFunCmd.setf1(fcmd, (srcpFx & 0x01)?True:False);
          }
          *cmd2 = fcmd ;
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "No loco with addr %d found", srcpLoco ) ;
        *reqRespCode = 412 ;
      }
    }

    /* SET <bus> GA <addr> <port> <value> <delay> */
    else if( StrOp.findi( req, "GA" ) ) {
      int idx = 0;
      const char* swID = NULL;
      int addrGA = 0;
      int addr = 0;
      int port = 0;
      int gate = 0;
      int value = 0;
      iOSwitch sw;
      iOSignal sg;
      iOStrTok tok = StrTokOp.inst(req, ' ');
      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 3: addrGA = atoi(s); break;
        case 4: gate = atoi(s); break;
        case 5: value = atoi(s); break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      /* Find switch */
      AddrOp.fromPADA( addrGA, &addr, &port );

      sw = ModelOp.getSwByAddress(model, addr, port);
      if( sw != NULL ) {
        iONode swProps = SwitchOp.base.properties(sw);
        int addr1 = AddrOp.toPADA( wSwitch.getaddr1(swProps), wSwitch.getport1(swProps) );
        int addr2 = AddrOp.toPADA( wSwitch.getaddr2(swProps), wSwitch.getport2(swProps) );
        int gate1 = wSwitch.getgate1(swProps);
        int gate2 = wSwitch.getgate2(swProps);

        if(( StrOp.equals( wSwitch.gettype(swProps), wSwitch.left))
        || ( StrOp.equals( wSwitch.gettype(swProps), wSwitch.right))
        || ( StrOp.equals( wSwitch.gettype(swProps), wSwitch.crossing))
        || ( StrOp.equals( wSwitch.gettype(swProps), wSwitch.ccrossing)) ) {
          cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
          wSwitch.setid( cmd, SwitchOp.getId(sw) );
          wSwitch.setcmd( cmd, gate?wSwitch.straight:wSwitch.turnout );
        }
        else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.threeway)) {
          /* straight left right */
          const char *currState = wSwitch.getstate(swProps);

          if( ( addr1 == addrGA ) && ( gate == 0 ) ) {
            if( StrOp.equals( currState, wSwitch.left) ) {
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setid( cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, wSwitch.straight );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d gate %d : type %s currstate %s set to straight", addrGA, gate, wSwitch.threeway, currState ) ;
            }
          }else if( ( addr1 == addrGA ) && ( gate == 1 ) ) {
            if( StrOp.equals( currState, wSwitch.straight) || StrOp.equals( currState, wSwitch.right) ) {
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setid( cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, wSwitch.left );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d gate %d : type %s currstate %s set to left", addrGA, gate, wSwitch.threeway, currState ) ;
            }
          }else if( ( addr2 == addrGA ) && ( gate == 0 ) ) {
            if( StrOp.equals( currState, wSwitch.right) ) {
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setid( cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, wSwitch.straight );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d gate %d : type %s currstate %s set to straight", addrGA, gate, wSwitch.threeway, currState ) ;
            }
          }else if( ( addr2 == addrGA ) && ( gate == 1 ) ) {
            if( StrOp.equals( currState, wSwitch.straight) || StrOp.equals( currState, wSwitch.left) ) {
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setid( cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, wSwitch.right );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d gate %d : type %s currstate %s set to right", addrGA, gate, wSwitch.threeway, currState ) ;
            }
          }
        }
        else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.dcrossing)) {
          /* straight right left turnout */

          const char *currState = wSwitch.getstate(swProps);
          const char *nextState ;

          if( ( addr1 == addrGA ) && ( 0 == value ) ) {
            if( StrOp.equals( currState, wSwitch.turnout) ) {
              nextState = wSwitch.right;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setid( cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d value %d : type %s Set from %s to %s ", addrGA, port, value, wSwitch.dcrossing, currState, nextState ) ;
            }else if( StrOp.equals( currState, wSwitch.left) ) {
              nextState = wSwitch.straight;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setid( cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d value %d : type %s Set from %s to %s ", addrGA, port, value, wSwitch.dcrossing, currState, nextState ) ;
            }
          }else if( ( addr1 == addrGA ) && ( 1 == value ) ) {
            if( StrOp.equals( currState, wSwitch.straight) ) {
              nextState = wSwitch.left;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setid( cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d value %d : type %s Set from %s to %s ", addrGA, port, value, wSwitch.dcrossing, currState, nextState ) ;
            }else if( StrOp.equals( currState, wSwitch.right) ) {
              nextState = wSwitch.turnout;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setid( cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d value %d : type %s Set from %s to %s ", addrGA, port, value, wSwitch.dcrossing, currState, nextState ) ;
            }
          }else if( ( addr2 == addrGA ) && ( 0 == value ) ) {
            if( StrOp.equals( currState, wSwitch.right) ) {
              nextState = wSwitch.straight;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setid( cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d value %d : type %s Set from %s to %s ", addrGA, port, value, wSwitch.dcrossing, currState, nextState ) ;
            }else if( StrOp.equals( currState, wSwitch.turnout) ) {
              nextState = wSwitch.left;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setid( cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d value %d : type %s Set from %s to %s ", addrGA, port, value, wSwitch.dcrossing, currState, nextState ) ;
            }
          }else if( ( addr2 == addrGA ) && ( 1 == value ) ) {
            if( StrOp.equals( currState, wSwitch.straight) ) {
              nextState = wSwitch.right;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setid( cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d value %d : type %s Set from %s to %s ", addrGA, port, value, wSwitch.dcrossing, currState, nextState ) ;
            }else if( StrOp.equals( currState, wSwitch.left) ) {
              nextState = wSwitch.turnout;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setid( cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d value %d : type %s Set from %s to %s ", addrGA, port, value, wSwitch.dcrossing, currState, nextState ) ;
            }
          }else{
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SET for SW NO COMBO FOR addr/gate/value found aGA %d a %d p %d gate %d value %d : type %s a1 %d a2 %d g1 %d g2 %d REQ %s", addrGA, addr, port, gate, value, wSwitch.gettype(swProps), addr1, addr2, gate1, gate2, req );
          }
        }
        else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.decoupler)) {
          Boolean isSingle = wSwitch.issinglegate(swProps);

          if( ! isSingle ) {
            cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
            wSwitch.setid( cmd, SwitchOp.getId(sw) );
            wSwitch.setcmd( cmd, gate?wSwitch.straight:wSwitch.turnout );

          }else {
            iOMap        swMap = NULL;

            swMap = ModelOp.getSwitchMap(model);
            iOSwitch swM = (iOSwitch)MapOp.first( swMap );
            /* TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "LOTHAR: map %p sw %p", swMap, sw ); */

            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "TYPE %s # %d set to %s\naddr %d port %d gate %d value %d isSingle %d", wSwitch.gettype(swProps), addrGA, gate?wSwitch.straight:wSwitch.turnout, addr, port, gate, value, isSingle?1:0 );

            while( swM != NULL ) {
              /* check the switch */
              iONode swPropsM = SwitchOp.base.properties(swM);

              if( StrOp.equals( wSwitch.gettype(swPropsM), wSwitch.decoupler ) 
                  && wSwitch.issinglegate(swPropsM) 
                  && ( wSwitch.getaddr1(swPropsM) == addr ) 
                  && ( wSwitch.getport1(swPropsM) == port )) {
                const char *desc =  wSwitch.getdesc(swPropsM);
                const char *id   =  wSwitch.getid(swPropsM);
                const char *currState = wSwitch.getstate(swPropsM);

                TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "LOTHAR desc %s id %s type %s addr1 %d port 1 %d gate1 %d isSingle %d currState %s", desc, id, wSwitch.gettype(swPropsM), addr1, wSwitch.getport1(swPropsM), gate1,  isSingle?1:0, currState ) ;

                cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
                wSwitch.setid( cmd, id );
                wSwitch.setcmd( cmd, gate?wSwitch.straight:wSwitch.turnout );
              }
              swM = (iOSwitch)MapOp.next( swMap );
            };
          };
        }else{
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Unknown GA-type %s with address %d", wSwitch.gettype(swProps),addrGA );
        }
      }
      
      sg = ModelOp.getSgByAddress(model, addr, port);
      if( sg != NULL ) {
        iONode sgProps = SignalOp.base.properties(sg);
        int aspects = wSignal.getaspects( sgProps );
        int addr1 = AddrOp.toPADA( wSignal.getaddr (sgProps), wSignal.getport1(sgProps) );
        int addr2 = AddrOp.toPADA( wSignal.getaddr2(sgProps), wSignal.getport2(sgProps) );
        int addr3 = AddrOp.toPADA( wSignal.getaddr3(sgProps), wSignal.getport3(sgProps) );
        int addr4 = AddrOp.toPADA( wSignal.getaddr4(sgProps), wSignal.getport4(sgProps) );
        int gate1 = wSignal.getgate1(sgProps);
        int gate2 = wSignal.getgate2(sgProps);
        int gate3 = wSignal.getgate3(sgProps);
        int gate4 = wSignal.getgate4(sgProps);

        /* TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SG aGA %d a %d p %d gate %d : aspects %d a1 %d a2 %d a3 %d a4 %d g1 %d g2 %d g3 %d g4 %d REQ %s", addrGA, addr, port, gate, aspects, addr1, addr2, addr3, addr4, gate1, gate2, gate3, gate4, req ); */

        cmd = NodeOp.inst(wSignal.name(), NULL, ELEMENT_NODE );
        wSignal.setid( cmd, SignalOp.getId(sg) );

        if( ( addr1 == addrGA ) && ( gate1 == gate ) ) {
          wSignal.setcmd( cmd, wSignal.red );
        }else if( ( addr2 == addrGA ) && ( gate2 == gate ) ) {
          wSignal.setcmd( cmd, wSignal.green );
        }else if( ( aspects >= 3 ) && ( addr3 == addrGA ) && ( gate3 == gate ) ) {
          wSignal.setcmd( cmd, wSignal.yellow );
        }else if( ( aspects >= 4 ) && ( addr4 == addrGA ) && ( gate4 == gate ) ) {
          wSignal.setcmd( cmd, wSignal.white );
        }else{
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "for SG NO COMOBO FOR addr/gate found aGA %d a %d p %d gate %d : aspects %d a1 %d a2 %d a3 %d a4 %d g1 %d g2 %d g3 %d g4 %d REQ %s", addrGA, addr, port, gate, aspects, addr1, addr2, addr3, addr4, gate1, gate2, gate3, gate4, req );
        }
      }
      if( ( sw == NULL ) && ( sg == NULL ) ) {
        *reqRespCode = (int) 412 ;
      }
    }
  }

  else if( StrOp.startsWithi( req, "GET" ) ) {
/*  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "GET req: %s", req );*/
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET req: %s", req );
  
    if( StrOp.findi( req, "POWER" ) ) {
      /* GET <bus> POWER */
      int idx = 0;
      char str[1025] = {'\0'};
      int busId = 0;
      char busType[1025] = {'\0'};
      iOStrTok tok = StrTokOp.inst(req, ' ');

      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 1:
          busId = atoi(s) ;
          break;
        case 2:
          StrOp.copy( busType, s) ;
          break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);
/* LRLRLR */
      if( StrOp.equals( busType, "POWER" ) ) {
        /* GET <bus> POWER */
        int i =0;

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "POWER[%d]", busId );

        iOList fstats = ModelOp.getFBStat( model );
        iONode  fstat = NULL;
        iONode fbcmd = NodeOp.inst(wFeedback.name(), NULL, ELEMENT_NODE );

        /* current overall system state (ON/OFF) */
        Boolean isPower = wState.ispower(ControlOp.getState(AppOp.getControl()));


        /* 
          We get the request through a command channel, but the answers have to be send also on the info channel.
          (If it is the first/initial request, we also have to send the answers back through the commanchannel!)
          Because we don't know which info channel belongs to which command channel, we send the answers on all info channels

          MY strategy:
              send INFO to all srcp client sessions currently in infomode
          OTHER strategy:
              send only to session with an ID exactly 1 higher than current command session ID
              PROBLEM: Does the corresponding info session always get the session ID exaclty 1 higher ?
        */

        /* Send powerState to all srcp info connections */
        iOList thList = ThreadOp.getAll(); 
        int cnt = ListOp.size( thList );
        int j;

        StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d POWER %s\n",
            time.tv_sec, time.tv_usec / 1000, busId, isPower?"ON":"OFF" );

        /* send INFO <bus> POWER answer back to requesting command channel */
        SocketOp.fmt(o->clntSocket, str);

        /* go through all threads, search the SRCP server connections ("cmdrSRCP") and send the data to the info channel */

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%d active threads.", cnt );

        for( j = 0; j < cnt; j++ ) {
          iOThread th = (iOThread)ListOp.get( thList, j );
          const char* tname = ThreadOp.getName( th );

          iOSrcpCon srcpcon = (iOSrcpCon)ThreadOp.getParm(th);
          iOSrcpConData data = Data(srcpcon);
          __iOSrcpService   oI = (__iOSrcpService)ThreadOp.getParm(th);

          if( StrOp.startsWithi( tname, "cmdrSRCP" ) ) {
            if ( oI->infomode ) {
              SocketOp.fmt(oI->clntSocket, str);
              TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP SEND: %p [%p] %d : %s", oI, oI->clntSocket, oI->id, str ) ;
              ThreadOp.sleep( 10 );
            }
          }
        }

        /* Send all active feedback stati of the currently requested bus to the clients */
        /* go through all active feedbacks */
        fstat = (iONode)ListOp.first( fstats );
        while( fstat != NULL ) {
          iOSrcpConData data = Data(srcpcon);
          iOFBack fb = ModelOp.getFBack(model, wFeedback.getid(fstat));
          if( fb != NULL ) {
            int s88busOffset = 0;
            iONode fbProps = FBackOp.base.properties(fb);

            iONode srcpconoffset = wSrcpCon.getsrcpconoffset(data->ini);
            while( srcpconoffset != NULL ) {
              if( StrOp.equals( wFeedback.getiid(fbProps), wSrcpConOffset.getiid( srcpconoffset ) ) ) {
                s88busOffset = wSrcpConOffset.getoffset( srcpconoffset );
                break;
              }
              srcpconoffset = wSrcpCon.nextsrcpconoffset(data->ini, srcpconoffset);
            }
            /* Is fb on the current bus? */
            if( busId == (wFeedback.getbus(fbProps) + s88busOffset + 1) ) {
              /*100 INFO <bus> FB <addr> <value>*/
              StrOp.fmtb(str, "%lu.%.3lu %d INFO %d FB %d %d\n",
                  time.tv_sec, time.tv_usec / 1000,
                  100, wFeedback.getbus(fbProps) + s88busOffset + 1, wFeedback.getaddr(fbProps), wFeedback.isstate(fbProps));

              for( j = 0; j < cnt; j++ ) {
                iOThread th = (iOThread)ListOp.get( thList, j );
                const char* tname = ThreadOp.getName( th );

                iOSrcpCon srcpcon = (iOSrcpCon)ThreadOp.getParm(th);
                iOSrcpConData data = Data(srcpcon);
                __iOSrcpService   oI = (__iOSrcpService)ThreadOp.getParm(th);

                if( StrOp.startsWithi( tname, "cmdrSRCP" ) ) {
                  if ( oI->infomode ) {
                    SocketOp.fmt(oI->clntSocket, str);
                    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP SEND: %p [%p] %d : %s", oI, oI->clntSocket, oI->id, str ) ;
                    ThreadOp.sleep( 10 );
                  }
                }
              }
            }
          }
          fstat = (iONode)ListOp.next( fstats );
          ThreadOp.sleep( 0 );
        };
        /* Cleanup. */
        thList->base.del( thList );
        ListOp.base.del(fstats);
      }
    }
    /* GET <bus> GL <addr> <drivemode> <V> <V_max> <f1> . . <fn> */
    else if( StrOp.findi( req, "GL" ) ) {
      int idx = 0;
      const char* lcID = NULL;
      Boolean dir = True;
      int V = 0;

      iOStrTok tok = StrTokOp.inst(req, ' ');
      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
          case 3: 
            {
              iOLoc loco = ModelOp.getLocByAddress(model, atoi(s));
              if( loco != NULL ) {
                lcID = LocOp.getId(loco);
                dir = LocOp.getDir(loco);
                V = LocOp.getV(loco);

                TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET REQ %s -> lcID %d dir %d speed %d TODO: send info back", req, lcID, dir, V );
                /* LRLRLR TODO: Send back Loco Info */
              }
              else{
                TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET REQ %s -> locoID for %d not found", req, atoi(s) );
              }
            }
            break;
          case 4:
            /* too many arguments ; 418 ERROR list too long */
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET REQ %s -> 418 ERROR list too long", req);
            *reqRespCode = 418 ;
            break;
        }
        idx++;
      }
      StrTokOp.base.del(tok);
    }/* GET GL */
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UNHANDLED GET req: %s", req );
    }
  } /* GET */
  else{
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UNHANDLED req: %s", req );
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
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Broadcast received." );
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
    if( StrOp.equalsi( req, "SET CONNECTIONMODE SRCP INFO" ) ) {
      o->infomode = True;
    }else {
      o->infomode = False;
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP Session %d set to %s", o->id, o->infomode?"INFOMODE":"COMMANDMODE" ) ;
    SocketOp.fmt(o->clntSocket, srcpFmtMsg(202, rsp, time, 0));
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP RESPONSE: %s for req %s socket %p", rsp, req, o->clntSocket ) ;
  }
  else if( StrOp.startsWithi( req, "GO" ) ) {
    SocketOp.fmt(o->clntSocket, srcpFmtMsg(200, rsp, time, o->id));
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP RESPONSE: %s", rsp ) ;

    /* LRLRLR if we started an info channel send information about server to client */
    if( o->infomode ) {
      char rspInfo[1025] = {'\0'};
      /*
      100 INFO 0 DESCRIPTION SESSION SERVER TIME GM
      101 INFO 0 TIME 0 0
      100 INFO 1 DESCRIPTION GA GL SM POWER LOCK DESCRIPTION
      */
      /* announce basic system capabilities (server) */
      sprintf(rspInfo, "%lu.%.3lu 100 INFO 0 DESCRIPTION SESSION SERVER TIME GM\n", time.tv_sec, time.tv_usec / 1000);
      SocketOp.fmt(o->clntSocket, rspInfo);
      sprintf(rspInfo, "%lu.%.3lu 100 INFO 0 TIME 0 0\n", time.tv_sec, time.tv_usec / 1000);
      SocketOp.fmt(o->clntSocket, rspInfo);
      /* LRLRLR TODO: loop over all cmd stations, calc srcp bus id(s) and announce capabilities */
      /* get first (master) command station and report capabilities (including those of Rocrail -> Locking) */
      /* usually we have: accessories, locos, service mode (programming locos), power (on/off), locking (from rockrail) */
      /* perhaps we need an array of command stations with a list of capabilities (or put them directly into the command station struct !) */
      sprintf(rspInfo, "%lu.%.3lu 100 INFO 1 DESCRIPTION GA GL SM POWER LOCK DESCRIPTION\n", time.tv_sec, time.tv_usec / 1000);
      SocketOp.fmt(o->clntSocket, rspInfo);
      /* loop over cmd stations and over all feedbacks and announce the srcp busses */
      /* LRLRLR: do we also have to loop through all switches and signals ?*/
      /*
      sprintf(rspInfo, "%lu.%.3lu 100 INFO 2 DESCRIPTION FB POWER\n", time.tv_sec, time.tv_usec / 1000);
      SocketOp.fmt(o->clntSocket, rspInfo);
      sprintf(rspInfo, "%lu.%.3lu 100 INFO 3 DESCRIPTION FB\n", time.tv_sec, time.tv_usec / 1000);
      SocketOp.fmt(o->clntSocket, rspInfo);
      */
    }
  }
  else if( StrOp.startsWithi( req, "TERM 0" ) ) {
    SocketOp.fmt(o->clntSocket, srcpFmtMsg(200, rsp, time, 0));
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP RESPONSE: %s", rsp ) ;
  }
  else {
    int reqRespCode = 200 ;
    iONode cmd2 = NULL;
    iONode cmd = __srcp2rr(srcpcon, o, req, &reqRespCode, &cmd2);
    if( cmd != NULL ){

      /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "__evalRequest __srcp2rr() returned with %s [%d]", cmd, o->clntSocket );*/

      Data(srcpcon)->callback( Data(srcpcon)->callbackObj, cmd );
      if( cmd2 != NULL ){
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__evalRequest __srcp2rr( REQ ) returned with CMD2 != NULL %s [%d]", cmd2, o->clntSocket );
        Data(srcpcon)->callback( Data(srcpcon)->callbackObj, cmd2 );
      }
    }
    else{
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__srcp2rr( REQ ) returned with CMD == NULL, reqRespCode == %d", reqRespCode );
    }
    SocketOp.fmt(o->clntSocket, srcpFmtMsg(reqRespCode, rsp, time, 0));
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP RESPONSE: %s", rsp ) ;
  }
}

static void __SrcpService( void* threadinst ) {
  iOThread         th = (iOThread)threadinst;
  __iOSrcpService   o = (__iOSrcpService)ThreadOp.getParm(th);
  iOSrcpCon   srcpcon = o->SrcpCon;
  iOSrcpConData data = Data(srcpcon);
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
        if( o->infomode ) {
          __rr2srcp(data, node, str);
          SocketOp.write( o->clntSocket, str, StrOp.len(str) );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "__SrcpService __rr2srcp() returned with %s %p [%p] ID: %d", str, o, o->clntSocket, o->id );
        }
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
    /* Strip trailing "\n", makes debug output more readable */
    int strLen = StrOp.len(str);
    if( ( strLen != 0 ) && ( str[strLen-1] == '\n' ) ) {
      str[strLen-1] = 0;
    }

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__evalRequest %s for ID: %d", str, o->id );
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
      cargo->infomode	= False;
      idCnt++;

      servername        = StrOp.fmt( "cmdrSRCP%08X", client );
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
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP: _postEvent not implemented");
  return;
}


/**  */
static void _setCallback( struct OSrcpCon* inst ,clntcon_callback callbackfun ,obj callbackobj ) {
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP: _setCallback not implemented");
  return;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/srcpcon.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
