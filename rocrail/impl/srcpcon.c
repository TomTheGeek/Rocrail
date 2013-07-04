/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

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
#include "rocrail/wrapper/public/SrcpBus.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Route.h"

extern const int bzr;

static int instCnt = 0;

static int idCnt = 1;
struct __OSrcpService {
  iOSrcpCon     SrcpCon;
  iOSocket      clntSocket;
  iOSerial      clntSerial;
  Boolean       readonly;
  Boolean       quit;
  Boolean       disablemonitor;
  int           id;
  Boolean       infomode;
  Boolean       handshake;
};
typedef struct __OSrcpService* __iOSrcpService;

static const char* SRCPVERSION="Rocrail 2.0r%d; SRCP 0.8.4; SRCPOTHER 0.8.3";

static volatile Boolean srcpPwCmdInProgress = False;
static char* srcpPwFreetext = NULL;
static int divider = -1 ;
static Boolean clockRunning = True;

#define JULIAN_DAY_1970_01_01 2440588L /* julian day of 1/1/1970  ( 32bit unix time 00 00 00 00 ) */
#define JULIAN_DAY_2038_01_19 2465443L /* julian day of 1/19/2038 ( 32bit unix time 7F FF FF FF ) */
#define JULIAN_DAY_2106_02_07 2490298L /* julian day of 2/7/2106  ( 32bit unix time FF FF FF FF ) */

/*
200 OK <ID>
201 OK PROTOCOL SRCP
202 OK CONNECTIONMODEOK
400 ERROR unsupported protocol
401 ERROR unsupported connection mode
402 ERROR insufficient data
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
    /* Cleanup data->xxx members... */

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


/** helper functions **/
static void __writeRsp(__iOSrcpService o, const char* rsp) {
  if( o->clntSocket != NULL )
    SocketOp.fmt(o->clntSocket, rsp);
  else
    SerialOp.fmt(o->clntSerial, rsp);
}



/* convert model time into SRCP time notation */
static char* convModelTimeToSRCP(time_t modeltime) {
  static char srcpTimeStr[128] ;

  /* calc timezone offset in seconds */
  struct tm lcl = *localtime(&modeltime);
  struct tm gmt = *gmtime(&modeltime);
  int tzOffset = ((24 + lcl.tm_hour - gmt.tm_hour)%24) * 3600 + ((60 + lcl.tm_min - gmt.tm_min)%60) * 60 ;
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "convModelTimeToSRCP: tzOffset=%d", tzOffset) ;

  long julDay = (long)( (modeltime+tzOffset) / 86400L) + JULIAN_DAY_1970_01_01 ;

  StrOp.fmtb(srcpTimeStr, "%ld %d %d %d", julDay, lcl.tm_hour, lcl.tm_min, lcl.tm_sec );

  return srcpTimeStr;
}

/* convert SRCP time into model time */
static time_t convSRCP2ModelTime( long julDay, int hour, int min, int sec ) {
  /* initialize with a reasonable value */
  time_t modeltime = ControlOp.getTime( AppOp.getControl() );

  /* calc timezone offset in seconds */
  struct tm lcl = *localtime(&modeltime);
  struct tm gmt = *gmtime(&modeltime);
  int tzOffset = ((24 + lcl.tm_hour - gmt.tm_hour)%24) * 3600 + ((60 + lcl.tm_min - gmt.tm_min)%60) * 60 ;
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "convSRCP2ModelTime: tzOffset=%d", tzOffset ) ;


  modeltime = (julDay - JULIAN_DAY_1970_01_01) * 86400L + hour * 3600L + min * 60L + sec - tzOffset ;

  TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "convSRCP2ModelTime: (julDay[%ld] - JULIAN_DAY_1970_01_01[%ld]) * 86400L + hour[%d] * 3600L + min[%d] * 60L + sec[%d] - tzOffset[%d] = modeltime[%ld]",
      julDay, JULIAN_DAY_1970_01_01, hour, min, sec, tzOffset, modeltime ) ;

  return modeltime;
}


static char *__setSrcpPwFreetext( const char *newText ) {

  if( srcpPwFreetext == NULL ) {
    srcpPwFreetext = StrOp.cat( NULL, newText );
  }else if( StrOp.equals( srcpPwFreetext, newText ) ) {
    /* new is like current -> nothing to do */
  }else {
    StrOp.free( srcpPwFreetext );
    srcpPwFreetext = StrOp.cat( NULL, newText );
  }

  return( srcpPwFreetext );
}

static char *__getSrcpPwFreetext() {
  return( srcpPwFreetext );
}

static Boolean __setClockRunning( Boolean value ) {
  clockRunning = value;
  return( clockRunning  );
}

static Boolean __isClockRunning() {
  return( clockRunning );
}

static const char *getDefaultIid() {
  return( wDigInt.getiid(AppOp.getIniNode( wDigInt.name() ) ) );
}

static int getSrcpBus( iOSrcpConData data, const char *iid ) {
  /* initialize with default */
  int srcpBus = 1;

  iONode srcpbus = wSrcpCon.getsrcpbus(data->ini);
  while( srcpbus != NULL ) {
    if( StrOp.equals( iid, wSrcpBus.getiid( srcpbus ) ) ) {
      srcpBus = wSrcpBus.getbus( srcpbus );
      break;
    }
    if( 0 == StrOp.len( wSrcpBus.getiid( srcpbus ) ) ){
      /* read system default value */
      srcpBus = wSrcpBus.getbus( srcpbus ) ;
    }
    srcpbus = wSrcpCon.nextsrcpbus(data->ini, srcpbus);
  }
  return( srcpBus );
}

static char *getSrcpIid( iOSrcpConData data, const int bus, char* srcpIid) {
  /* initialize with default */
  StrOp.copy( srcpIid, getDefaultIid() );

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "getSrcpIid: iid[%d]", bus );

  iONode srcpbus = wSrcpCon.getsrcpbus(data->ini);
  while( srcpbus != NULL ) {
    if( bus == wSrcpBus.getbus( srcpbus ) ) {
      if( StrOp.len(wSrcpBus.getiid( srcpbus )) ) {
        StrOp.copy( srcpIid, wSrcpBus.getiid( srcpbus ));
      }
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "getSrcpIid: iid[%d]=\"%s\" \"%s\"", bus, srcpIid, wSrcpBus.getiid( srcpbus ) );
      break;
    }
    srcpbus = wSrcpCon.nextsrcpbus(data->ini, srcpbus);
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "getSrcpIid: iid[%d]=\"%s\"", bus, srcpIid );

  return srcpIid ;
}

/* "adapted" version of ModelOp.getSwByAddress */
static iOSwitch SRCPgetSwByAddressAndIid( iOModel model, int pAddr, int srcpPort, char *iid ) {
  iOMap switchMap = ModelOp.getSwitchMap( model );

  iOSwitch sw = (iOSwitch)MapOp.first( switchMap );

  while( sw != NULL ) {
    iONode swProps = SwitchOp.base.properties(sw);
    int swPaddr1 = AddrOp.toPADA( wSwitch.getaddr1( swProps ), wSwitch.getport1( swProps ) );
    int swPaddr2 = AddrOp.toPADA( wSwitch.getaddr2( swProps ), wSwitch.getport2( swProps ) );

    if( srcpPort >= 0 && wSwitch.issinglegate(swProps) ) {
      if( ( ( pAddr == swPaddr1 ) && srcpPort == wSwitch.getgate1( swProps ) ) && StrOp.equals( wSwitch.getiid(swProps), iid ) )
        break;
    }else {
      if( ( ( pAddr == swPaddr1 ) || ( pAddr == swPaddr2 ) ) && StrOp.equals( wSwitch.getiid(swProps), iid ) )
        break;
    }
    sw = (iOSwitch)MapOp.next( switchMap );
  };
  if( sw != NULL ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SRCPgetSwByAddressAndIid sw != NULL iid %s , swIid %s", iid, wSwitch.getiid(SwitchOp.base.properties(sw)) );
    return sw;
  }else {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCPgetSwByAddressAndIid: defaultIid %s", getDefaultIid() );

    if( StrOp.equals( iid, getDefaultIid() ) ) {
      sw = (iOSwitch)MapOp.first( switchMap );

      while( sw != NULL ) {
        iONode swProps = SwitchOp.base.properties(sw);
        int swPaddr1 = AddrOp.toPADA( wSwitch.getaddr1( swProps ), wSwitch.getport1( swProps ) );
        int swPaddr2 = AddrOp.toPADA( wSwitch.getaddr2( swProps ), wSwitch.getport2( swProps ) );

        if( srcpPort >= 0 && wSwitch.issinglegate(swProps) ) {
          if( ( ( pAddr == swPaddr1 ) && srcpPort == wSwitch.getgate1( swProps ) ) && StrOp.equals( wSwitch.getiid(swProps), "" ) )
            break;
        }else {
          if( ( ( pAddr == swPaddr1 ) || ( pAddr == swPaddr2 ) ) && StrOp.equals( wSwitch.getiid(swProps), "" ) )
            break;
        }
        sw = (iOSwitch)MapOp.next( switchMap );
      };
    }
  }
  if( sw != NULL ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SRCPgetSwByAddressAndIid sw != NULL iid %s , swIid %s", iid, wSwitch.getiid(SwitchOp.base.properties(sw)) );
    return sw;
  }else {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCPgetSwByAddressAndIid sw == NULL iid %s", iid );
  }
  return NULL;
}

/* "adapted" version of ModelOp.getSgByAddress */
static iOSignal SRCPgetSgByAddressAndIid( iOModel model, int pAddr, char *iid ) {
  iOMap signalMap = ModelOp.getSignalMap( model );

  iOSignal sg = (iOSignal)MapOp.first( signalMap );

  while( sg != NULL ) {
    iONode sgProps = SignalOp.base.properties(sg);
    int aspects = wSignal.getaspects( sgProps );

    if( ( pAddr == AddrOp.toPADA( wSignal.getaddr( sgProps ), wSignal.getport1( sgProps ) ) ) && StrOp.equals( wSignal.getiid(sgProps), iid ) )
      break;
    if( ( pAddr == AddrOp.toPADA( wSignal.getaddr2( sgProps ), wSignal.getport2( sgProps ) ) ) && StrOp.equals( wSignal.getiid(sgProps), iid ) )
      break;
    if( (aspects > 2) && ( pAddr == AddrOp.toPADA( wSignal.getaddr3( sgProps ), wSignal.getport3( sgProps ) ) ) && StrOp.equals( wSignal.getiid(sgProps), iid ) )
      break;
    if( (aspects > 3) && ( pAddr == AddrOp.toPADA( wSignal.getaddr4( sgProps ), wSignal.getport4( sgProps ) ) ) && StrOp.equals( wSignal.getiid(sgProps), iid ) )
      break;
    sg = (iOSignal)MapOp.next( signalMap );
  };
  if( sg != NULL ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SRCPgetSgByAddressAndIid sw != NULL iid %s , sgIid %s", iid, wSignal.getiid(SignalOp.base.properties(sg)) );
    iONode sgProps = SignalOp.base.properties(sg);
    int patternMode = wSignal.getusepatterns(sgProps);
    int aspects = wSignal.getaspects( sgProps );
    if( (patternMode == 0) && (aspects <= 4) ) {
      /* signal in standard mode with maximum of 4 aspects */
      return sg;
    }else {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCPgetSgByAddressAndIid signal[%s] not suitable", wSignal.getid(sgProps) );
    }
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCPgetSgByAddressAndIid: defaultIid %s", getDefaultIid() );
  sg = NULL;
  if( StrOp.equals( iid, getDefaultIid() ) ) {
    sg = (iOSignal)MapOp.first( signalMap );

    while( sg != NULL ) {
      iONode sgProps = SignalOp.base.properties(sg);
      int aspects = wSignal.getaspects( sgProps );

      if( ( pAddr == AddrOp.toPADA( wSignal.getaddr( sgProps ), wSignal.getport1( sgProps ) ) ) && StrOp.equals( wSignal.getiid(sgProps), "" ) )
        break;
      if( ( pAddr == AddrOp.toPADA( wSignal.getaddr2( sgProps ), wSignal.getport2( sgProps ) ) ) && StrOp.equals( wSignal.getiid(sgProps), "" ) )
        break;
      if( (aspects > 2) && ( pAddr == AddrOp.toPADA( wSignal.getaddr3( sgProps ), wSignal.getport3( sgProps ) ) ) && StrOp.equals( wSignal.getiid(sgProps), "" ) )
        break;
      if( (aspects > 3) && ( pAddr == AddrOp.toPADA( wSignal.getaddr4( sgProps ), wSignal.getport4( sgProps ) ) ) && StrOp.equals( wSignal.getiid(sgProps), "" ) )
        break;
      sg = (iOSignal)MapOp.next( signalMap );
    };
  }

  if( sg != NULL ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SRCPgetSgByAddressAndIid sg != NULL iid %s , sgIid %s", iid, wSignal.getiid(SignalOp.base.properties(sg)) );
    iONode sgProps = SignalOp.base.properties(sg);
    int patternMode = wSignal.getusepatterns(sgProps);
    int aspects = wSignal.getaspects( sgProps );
    if( (patternMode == 0) && (aspects <= 4) ) {
      /* signal in standard mode with maximum of 4 aspects */
      return sg;
    }else {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCPgetSgByAddressAndIid signal[%s] not suitable", wSignal.getid(sgProps) );
    }
  }else {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCPgetSgByAddressAndIid sg == NULL iid %s", iid );
  }
  return NULL;
}

/* "extended" version of ModelOp.getLocByAddress */
static iOLoc SRCPgetLocByAddressAndIid( iOModel model, int addr, const char *iid ) {
  iOMap lcMap = ModelOp.getLocoMap( model );
  
  iOLoc lc = (iOLoc)MapOp.first( lcMap );
  while( lc != NULL ) {
    if( LocOp.getAddress(lc) == addr ) {
      iONode lcProps = LocOp.base.properties(lc);
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SRCPgetLocByAddressAndIid addr %d searchIid=%s lcIid=%s defIid=%s", addr, iid, wLoc.getiid(lcProps), getDefaultIid() );

      if( StrOp.equals( wLoc.getiid(lcProps), iid ) || ( StrOp.len(wLoc.getiid(lcProps)) == 0 && StrOp.equals( iid, getDefaultIid() ) ) ) {
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SRCPgetLocByAddressAndIid ( addr=%d) iid=%s , locIid=%s", addr, iid, wLoc.getiid(lcProps) );
        return lc;
      }
    }
    lc = (iOLoc)MapOp.next( lcMap );
  };

  return NULL;
}

/* get feedback-object by iid and address */
static iOFBack getFeedbackByIidAndAddr( char *iid, int addr) {
  iOModel model = AppOp.getModel();

  iOMap fbackMap = ModelOp.getFeedbackMap( model );
  iOFBack fb = (iOFBack)MapOp.first( fbackMap );

  while( fb != NULL ) {
    iONode fbProps = FBackOp.base.properties(fb);
    int fbAddr = wFeedback.getaddr(fbProps);
    const char *fbIid = wFeedback.getiid(fbProps);
    
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "getFeedbackByIidAndAddr: iid %s addr %d ; fbIid=%s fbAddr = %d",
                 iid, addr, fbIid, fbAddr );
    if( StrOp.equals( "", fbIid ) ) {
      fbIid = getDefaultIid();
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "getFeedbackByIidAndAddr: iid %s addr %d ; fbIid=%s fbAddr = %d",
                   iid, addr, fbIid, fbAddr );
    }

    if( ( addr == fbAddr ) && StrOp.equals( iid, fbIid) )
      return fb;

    fb = (iOFBack)MapOp.next( fbackMap );
  };
  return NULL;
}


/* is the given iid a virtual command station */
static Boolean isDigintVirtual( const char *iid )
{
  if( iid == NULL || StrOp.len( iid ) == 0 ) {
    /* this should never happen */
    return True;
  }

  iONode  ini           = AppOp.getIni();
  iONode  digint        = wRocRail.getdigint( ini );
  iONode  plan          = ModelOp.getModel(AppOp.getModel());
  iONode  modeldigint   = plan?wPlan.getdigint( plan ):NULL;
  Boolean bModelDigints = modeldigint == NULL ? False:True;

  while( digint != NULL ) {
    const char* digintIid = wDigInt.getiid( digint );
    const char* digintLib = wDigInt.getlib( digint );

    if( StrOp.equals( digintIid, iid ) ) {
      /* command station detected */
      return( StrOp.equals( digintLib, wDigInt.vcs ) );
    }

    if( bModelDigints )
      digint = wPlan.nextdigint( plan, digint );
    else
      digint = wRocRail.nextdigint( ini, digint );
  }
  return True;
}


/* check if the given srcoBus is a valid bus number */
static Boolean isValidSrcpBus( iOSrcpConData data, int srcpBus ) {
  char srcpBusIid[1024];

  if( srcpBus == 0 ) {
    /* system nus number is always valid */
    return True;
  }

  if( srcpBus > 0 ) {
    /* command sation bus: check by translating into iid and back to number */
    getSrcpIid( data, srcpBus, srcpBusIid);
    int checkSrcpBus = getSrcpBus( data, srcpBusIid );
    /* if bus number changed (to default and it was not before) during translations it is inavlid */
    return( srcpBus == checkSrcpBus );
  }

  return False;
}

/* create SRCP answer with powerState of all bus-ids */
static char *creaRspBusDescr(char *str, struct timeval *time, int srcpBus ) {

  if( srcpBus == 0 ) {
    /* server capabilities */
    StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d DESCRIPTION SESSION SERVER TIME\n",
        (*time).tv_sec, (*time).tv_usec / 1000L, 0);
  }else if( srcpBus > 0 ){
    /* announce "normal" capabilities for all command station *
    /* SM and LOCK currently not supported through srcp server */
    StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d DESCRIPTION GA GL FB POWER DESCRIPTION\n",
        (*time).tv_sec, (*time).tv_usec / 1000L, srcpBus );
  }else
    return NULL;
  return str;
}

/* create SRCP answer with powerState of all bus-ids */
static char *creaRspAllPwSts(iOSrcpConData data ) {
  struct timeval time;
  gettimeofday(&time, NULL);

  char str[1025] = {'\0'};
  char *infoStr = NULL;

  /* current overall system state (ON/OFF) */
  Boolean isPower = wState.ispower(ControlOp.getState(AppOp.getControl()));

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "creaRspAllPwSts: isPower[%d]", isPower );

  /* generate strings for all srcp busses */
  iONode  ini           = AppOp.getIni();
  iONode  digint        = wRocRail.getdigint( ini );
  iONode  plan          = ModelOp.getModel(AppOp.getModel());
  iONode  modeldigint   = plan?wPlan.getdigint( plan ):NULL;
  Boolean bModelDigints = modeldigint == NULL ? False:True;

  while( digint != NULL ) {
    const char* digintIid = wDigInt.getiid( digint );
    int srcpBus = getSrcpBus( data, digintIid );

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "creaRspAllPwSts: srcpBus[%d] srcpIid[%s]", srcpBus, digintIid );

    StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d POWER %s%s\n",
        time.tv_sec, time.tv_usec / 1000L, srcpBus, isPower?"ON":"OFF", __getSrcpPwFreetext() );
    infoStr = StrOp.cat( infoStr, str );

    if( bModelDigints )
      digint = wPlan.nextdigint( plan, digint );
    else
      digint = wRocRail.nextdigint( ini, digint );
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "creaRspAllPwSts: infoStr[\n%s]", infoStr );

  return infoStr;
}


/* send infoStr to all srcp info connections */
static int sendRsp2AllInfoChannels(iOSrcpConData data, const char* infoStr) {
  iOList thList = ThreadOp.getAll();
  int cnt = ListOp.size( thList );
  int j;
  int num = 0;

  /* go through all threads, search the SRCP server connections ("cmdrSRCP") and send the data to the info channel */

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sendRsp2AllInfoChannels: %d active threads.", cnt );

  for( j = 0; j < cnt; j++ ) {
    iOThread th = (iOThread)ListOp.get( thList, j );
    const char* tname = ThreadOp.getName( th );

    iOSrcpCon srcpcon = (iOSrcpCon)ThreadOp.getParm(th);
    iOSrcpConData data = Data(srcpcon);
    __iOSrcpService   oI = (__iOSrcpService)ThreadOp.getParm(th);

    if( StrOp.startsWithi( tname, "cmdrSRCP" ) ) {
      if ( oI->infomode ) {
        __writeRsp(oI, infoStr);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sendRsp2AllInfoChannels: SRCP SEND: %p [%p] %d : %s", oI, oI->clntSocket, oI->id, infoStr ) ;
        num++;
        ThreadOp.sleep( 10 );
      }
    }
  }
  return num;
}

/* Send powerState of all bus-ids to all srcp info connections */
static void sendAllPWstates2AllInfoChannels(iOSrcpConData data) {
  char* rsp = NULL;
  int num = 0;
  rsp = creaRspAllPwSts( data );
  if( rsp )
    num = sendRsp2AllInfoChannels( data, rsp );

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sendAllPWstates2AllInfoChannels: text[\n%s] sent [%d] times", rsp, num );

}

/* Send sessionState to all srcp info connections */
static void sendSessionstate2InfoChannels( int busId, int sessId, int srcpCode, Boolean mode ) {
  char str[1025] = {'\0'};
  int j;
  struct timeval time;
  gettimeofday(&time, NULL);

  switch(srcpCode) {
    case 101:
      /* Initialisation of session */
      StrOp.fmtb(str, "%lu.%.3lu %d INFO %d SESSION %d %s\n", 
              time.tv_sec, time.tv_usec / 1000L, srcpCode, busId, sessId, mode?"INFO":"COMMAND");
      break;
    case 102:
      /* Termination of session */
      StrOp.fmtb(str, "%lu.%.3lu %d INFO %d SESSION %d\n", 
              time.tv_sec, time.tv_usec / 1000L, srcpCode, busId, sessId);
      break;
    default:
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sendSessionstate2InfoChannels wrong srcpCode %d", srcpCode );
      return;
      break;
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%s", str);

  /* go through all threads, search the SRCP server connections ("cmdrSRCP") and send the data to the info channel */
  {
    iOList thList = ThreadOp.getAll();
    int cnt = ListOp.size( thList );

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%d active threads.", cnt );

    for( j = 0; j < cnt; j++ ) {
      iOThread th = (iOThread)ListOp.get( thList, j );
      const char* tname = ThreadOp.getName( th );

      iOSrcpCon srcpcon = (iOSrcpCon)ThreadOp.getParm(th);
      iOSrcpConData data = Data(srcpcon);
      __iOSrcpService   oI = (__iOSrcpService)ThreadOp.getParm(th);

      if( StrOp.startsWithi( tname, "cmdrSRCP" ) ) {
        if ( oI->infomode ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCP SEND: %p [%p] %d : %s", oI, oI->clntSocket, oI->id, str ) ;
          __writeRsp(oI, str);
          ThreadOp.sleep( 10 );
        }
      }
    }
    ListOp.base.del(thList);
  }


}


/** ----- OSrcpCon ----- */

static char* __rr2srcp(iOSrcpCon srcpcon, __iOSrcpService o, iONode evt, char* str ) {
  iOSrcpConData data = Data(srcpcon);
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
      int gate1 = wSwitch.getgate1(swProps);
      int gate2 = wSwitch.getgate2(swProps);
      Boolean singlegate = wSwitch.issinglegate(swProps);
      const char* state = wSwitch.getstate(evt);
      const char* swstate = wSwitch.getstate(swProps);
      int srcpBus = getSrcpBus( data, wSwitch.getiid(swProps));

      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "__rr2srcp: id[%s] state[%s] swstate[%s]",
          wSwitch.getid(swProps), state, swstate );

      if( ! StrOp.equals( state, swstate ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "__rr2srcp: id[%s] state[%s] swstate[%s]",
            wSwitch.getid(swProps), state, swstate );
        return str;
      }

      if(  StrOp.equals( wSwitch.gettype(swProps), wSwitch.left)
        || StrOp.equals( wSwitch.gettype(swProps), wSwitch.right)
        || StrOp.equals( wSwitch.gettype(swProps), wSwitch.twoway)
        || StrOp.equals( wSwitch.gettype(swProps), wSwitch.crossing) 
        || StrOp.equals( wSwitch.gettype(swProps), wSwitch.ccrossing)
        || StrOp.equals( wSwitch.gettype(swProps), wSwitch.accessory)
        ) {
        /* 100 INFO <bus> GA <addr> <port> <value> */
        if( singlegate ) {
          int action = 1;
          if( StrOp.equals( state, wSwitch.straight ) )
            action = 0;
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d %d\n",
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, gate1, action );
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__rr2srcp: str[%s]", str ) ;
        }else {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, StrOp.equals( state, wSwitch.straight)? 1:0,
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, StrOp.equals( state, wSwitch.straight)? 1:0);
        }
      }
      else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.threeway) 
            || StrOp.equals( wSwitch.gettype(swProps), wSwitch.dcrossing)) {
        if( StrOp.equals( state, wSwitch.straight)) {
          if( singlegate ) {
            StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, gate1, time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2);
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__rr2srcp: 3wDx straight str[%s]", str ) ;
          }else {
            StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 0, time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, 0,
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 0, time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, 0);
          }
        }
        else if( StrOp.equals( state, wSwitch.left)) {
          if( singlegate ) {
            StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
                time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, gate1, time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2);
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__rr2srcp: 3wDx left str[%s]", str ) ;
          }else {
            StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
                time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 1, time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, 0,
                time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 1, time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, 0);
          }
        }
        else if( StrOp.equals( state, wSwitch.right)) {
          if( singlegate ) {
            StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 1\n",
                time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, gate1, time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2);
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__rr2srcp: 3wDx right str[%s]", str ) ;
          }else {
            StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
                time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 0, time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, 1,
                time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 0, time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, 1);
          }
        }
        else if( StrOp.equals( state, wSwitch.turnout)) {
          if( singlegate ) {
            StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 1\n",
                time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, gate1, time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2);
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__rr2srcp: 3wDx turnout str[%s]", str ) ;
          }else {
            StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
                time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 1, time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, 1,
                time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 1, time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, 1);
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UNHANDLED swEvent type %s, SWaddr1 %d, SWaddr2 %d, stateE %s, stateP %s", wSwitch.gettype(swProps) , addr, addr2, wSwitch.getstate(evt), wSwitch.getstate(swProps));
        }
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "DCROSSING rr2srcp sends %s", str );
      }
      else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.decoupler )) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "TYPE %s # %d set to %d", wSwitch.gettype(swProps), addr, StrOp.equals( state, wSwitch.straight)? 1:0 );
        if( ! wSwitch.issinglegate(evt)){
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, StrOp.equals( state, wSwitch.straight)? 1:0,
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, StrOp.equals( state, wSwitch.straight)? 1:0);
        }else {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, wSwitch.getgate1(evt),
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, wSwitch.getgate1(evt));
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
      int patternMode = wSignal.getusepatterns(sgProps);
      int aspects = wSignal.getaspects( sgProps );
      int addr1 = AddrOp.toPADA( wSignal.getaddr (sgProps), wSignal.getport1(sgProps) );
      int gate1 = wSignal.getgate1(sgProps);
      int addr2 = AddrOp.toPADA( wSignal.getaddr2(sgProps), wSignal.getport2(sgProps) );
      int gate2 = wSignal.getgate2(sgProps);
      int addr3 = AddrOp.toPADA( wSignal.getaddr3(sgProps), wSignal.getport3(sgProps) );
      int gate3 = wSignal.getgate3(sgProps);
      int addr4 = AddrOp.toPADA( wSignal.getaddr4(sgProps), wSignal.getport4(sgProps) );
      int gate4 = wSignal.getgate4(sgProps);

      if( (patternMode == 0) && (aspects <= 4) ) {
        /* signal in standard mode with maximum of 4 aspects */
        int srcpBus = getSrcpBus( data, wSignal.getiid(sgProps));

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sgEvent typeP[%s] signal[%s] aspects[%d] a1[%d] g1[%d] a2[%d] g2[%d] a3[%d] g3[%d] a4[%d] g4[%d], stateE %s, stateP %s",
            wSignal.gettype(sgProps), wSignal.getid(sgProps), aspects, addr1, gate1, addr2, gate2, addr3, gate3, addr4, gate4, wSignal.getstate(evt), wSignal.getstate(sgProps));

        /* 100 INFO <bus> GA <addr> <port> <value> */
        if( StrOp.equals( wSignal.getstate(evt), wSignal.red)) {
        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr1, gate1,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr1, gate1);
        }
        else if( StrOp.equals( wSignal.getstate(evt), wSignal.green)) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2,
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2);
        }
        else if( StrOp.equals( wSignal.getstate(evt), wSignal.yellow)) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr3, gate3,
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr3, gate3);
        }
        else if( StrOp.equals( wSignal.getstate(evt), wSignal.white)) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr4, gate4,
              time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr4, gate4);
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UNHANDLED sgEvent unknown state[%s] signal[%s] aspects[%d] a1[%d] g1[%d] a2[%d] g2[%d] a3[%d] g3[%d] a4[%d] g4[%d]",
              wSignal.getid(sgProps), wSignal.getstate(evt), aspects, addr1, gate1, addr2, gate2, addr3, gate3, addr4, gate4 );
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "IGNORED sgEvent: signal[%s] not in standard mode[%d] or too many aspects[%d]",
            wSignal.getid(sgProps), patternMode, aspects );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "IGNORED sgEvent: signal[%s] not in standard mode[%d] or too many aspects[%d] a1[%d] g1[%d] a2[%d] g2[%d] a3[%d] g3[%d] a4[%d] g4[%d] stateE[%s] stateP[%s]",
            wSignal.getid(sgProps), patternMode, aspects, addr1, gate1, addr2, gate2, addr3, gate3, addr4, gate4, wSignal.getstate(evt), wSignal.getstate(sgProps) );
      }
    }
  }

  else if( StrOp.equals( wState.name(), NodeOp.getName(evt))) {
    const char *iid = wState.getiid(evt);
    if( iid != NULL ) {

      int srcpBus = getSrcpBus( data, iid );

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "StateName %s EvtName %s srcpBus %d PW %s", wState.name(), NodeOp.getName(evt), srcpBus, wState.ispower(evt)?"ON":"OFF" );

      if( ! srcpPwCmdInProgress )
        __setSrcpPwFreetext( " by Rocrail" );

      /* 100 INFO <bus> POWER ON/OFF <freetext> */
      StrOp.fmtb(str, "%lu.%.3lu %d INFO %d POWER %s%s\n",
            time.tv_sec, time.tv_usec / 1000L, 100, 
            srcpBus, wState.ispower(evt)?"ON":"OFF", __getSrcpPwFreetext() );
    }
  }

  else if( StrOp.equals( wClock.name(), NodeOp.getName(evt)) ) {
    if( StrOp.equals( wClock.getcmd(evt), wClock.go ) ) {
      /* GO */
      __setClockRunning( True );
      /* 101 INFO 0 TIME fx fy */
       StrOp.fmtb(str, "%lu.%.3lu 101 INFO 0 TIME %d %d\n", time.tv_sec, time.tv_usec / 1000L, divider, 1 );
       __writeRsp(o, str);

      /* 100 INFO 0 TIME <JulDay> <Hour> <Minute> <Second> */
      time_t evt_time = wClock.gettime(evt);
      StrOp.fmtb(str, "%lu.%.3lu 100 INFO 0 TIME %s\n",
          time.tv_sec, time.tv_usec / 1000L, convModelTimeToSRCP(evt_time) );
    }
    else if( StrOp.equals( wClock.getcmd(evt), wClock.freeze ) ) {
      /* FREEZE */
      __setClockRunning( False );
      /* 102 INFO 0 TIME */
      StrOp.fmtb(str, "%lu.%.3lu 102 INFO 0 TIME\n", time.tv_sec, time.tv_usec / 1000L );
    }
    else if( StrOp.equals( wClock.getcmd(evt), wClock.set ) ) {
      /* SET */
      /* divider changed -> 101 INFO 0 TIME fx fy */
      int mt_divider = wClock.getdivider(evt);
      if( divider != mt_divider ) {
        /* time divider changed -> first send current time divider */
        divider = mt_divider;
        if( __isClockRunning() ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__rr2srcp: new time divider[%d]", divider );
          StrOp.fmtb(str, "%lu.%.3lu 101 INFO 0 TIME %d %d\n", time.tv_sec, time.tv_usec / 1000L, divider, 1 );
          __writeRsp(o, str);
        }
      }
      if( __isClockRunning() ) {
        /* 100 INFO 0 TIME <JulDay> <Hour> <Minute> <Second> */
        time_t evt_time = wClock.gettime(evt);
        StrOp.fmtb(str, "%lu.%.3lu 100 INFO 0 TIME %s\n",
            time.tv_sec, time.tv_usec / 1000L, convModelTimeToSRCP(evt_time) );
      }
    }
    else if( StrOp.equals( wClock.getcmd(evt), wClock.sync ) ) {
      /* SYNC */
      int mt_divider = wClock.getdivider(evt);
      if( divider != mt_divider ) {
        /* time divider changed -> first send current time divider */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "__rr2srcp: SYNC divider change detected old[%d] new[%s]", divider, mt_divider );
        divider = mt_divider;
        if( __isClockRunning() ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__rr2srcp: new time divider[%d]", divider );
          StrOp.fmtb(str, "%lu.%.3lu 101 INFO 0 TIME %d %d\n", time.tv_sec, time.tv_usec / 1000L, divider, 1 );
          __writeRsp(o, str);
        }
      }
      /* 100 INFO 0 TIME <JulDay> <Hour> <Minute> <Second> */
      time_t evt_time = wClock.gettime(evt);
      StrOp.fmtb(str, "%lu.%.3lu 100 INFO 0 TIME %s\n",
          time.tv_sec, time.tv_usec / 1000L, convModelTimeToSRCP(evt_time) );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Unknown/unsupported clock command", wClock.getcmd(evt) );
    }
  } /* CLOCK / TIME */

  else if( StrOp.equals( wFeedback.name(), NodeOp.getName(evt))) {
    iOFBack fb = ModelOp.getFBack(model, wFeedback.getid(evt));
    if( fb != NULL ) {
      iONode fbProps = FBackOp.base.properties(fb);
      int srcpBus = getSrcpBus( data, wFeedback.getiid(fbProps));

      /*100 INFO <bus> FB <addr> <value>*/
      StrOp.fmtb(str, "%lu.%.3lu %d INFO %d FB %d %d\n",
          time.tv_sec, time.tv_usec / 1000L,
          100, srcpBus, wFeedback.getaddr(evt), wFeedback.isstate(evt));
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "FBcmd [%s]", str );
    }
  } /* FB */

  else if( StrOp.equals( wLoc.name(), NodeOp.getName(evt)) 
        || StrOp.equals( wFunCmd.name(), NodeOp.getName(evt))) {
    /* 100 INFO <bus> GL <addr>  <drivemode> <V>  <V_max> <f1> . . <fn> */
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "[%s] wLoc or wFunCmd detected", NodeOp.getName(evt) ); 

    iOLoc lo = ModelOp.getLoc(model, wLoc.getid(evt), NULL, False);
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

      iOLoc loco = SRCPgetLocByAddressAndIid(model, loAddr, loIid);
      int i, mask ;

      int decStep = (wLoc.getV( loProps ) * loSpcnt) / wLoc.getV_max( loProps );
      char funcString[1023];
      funcString[0] = '\0';

      for( i=0 ; i < loFnCnt ; i++ ) {
        mask = 1 << i ;
        funcString[2*i] = ' ';
        funcString[2*i+1] = loFx&mask?'1':'0';
      }
      funcString[2*loFnCnt] = '\0';
      
      int srcpBus = getSrcpBus( data, wLoc.getiid(loProps));

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "addr %d isdir %d V %d Vmode %s decStep %d spcnt %d loFx %d loId %s loFnCnt %d functions %s", 
              loAddr, loDir, loV, loVmode, decStep, loSpcnt, loFx, loId, loFnCnt, funcString );

      StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GL %d %d %d %d %d%s\n",
          time.tv_sec, time.tv_usec / 1000L,
          100, srcpBus, loAddr,  loDir?1:0, decStep, loSpcnt, loFn?1:0, funcString );
    }
  }

  else if( StrOp.equals( wFunDef.name(), NodeOp.getName(evt))) {
    /* */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "[%s] wFunDef detected", NodeOp.getName(evt) ); 
  }

  else if( StrOp.equals( wException.name(), NodeOp.getName(evt))){
    const char* text = wException.gettext( evt );
    int        level = wException.getlevel( evt );
      
    /* 
      TODO: Do we need an exception handling? 
      Not now. Currently we don't miss any commands or infos
    */
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "rr2srcp %s lvl %d text [%s]", NodeOp.getName(evt), level, text );
  }

  else if( StrOp.equals( wBlock.name(), NodeOp.getName(evt))){
    const char*   cmd = wBlock.getcmd( evt );
    const char*    id = wBlock.getid( evt );
    int          addr = wBlock.getaddr( evt );
    const char* locId = wBlock.getlocid( evt );
    int	         port = wBlock.getport( evt );
    Boolean       pow = wBlock.ispower( evt );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rr2srcp %s id[%s] addr %d cmd [%s] locId [%s] port %d power %s",
        NodeOp.getName(evt), id, addr, cmd, locId, port, pow?"yes":"no" );
  }

  else if( StrOp.equals( wRoute.name(), NodeOp.getName(evt))){
    const char*    id = wRoute.getid( evt );
    iORoute route = ModelOp.getRoute( model, id );
    const char* fb  = RouteOp.getFromBlock(route);
    const char* fbs = RouteOp.getFromBlockSide(route)?"-":"+";
    const char* tb  = RouteOp.getToBlock(route);
    const char* tbs = RouteOp.getToBlockSide(route)?"-":"+";
    Boolean locked = RouteOp.isLocked(route);
    
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rr2srcp %s id[%s] from [%s%s] to [%s%s] locked[%s]",
        NodeOp.getName(evt), id, fb, fbs, tb, tbs, locked?"yes":"no" );
  }

  else if( StrOp.equals( wAutoCmd.name(), NodeOp.getName(evt))){
    Boolean autoMode = StrOp.equals( wAutoCmd.on, wAutoCmd.getcmd(evt) );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rr2srcp type[%s] autoMode[%d][%s]",
        NodeOp.getName(evt), autoMode, wAutoCmd.getcmd(evt) );
  }

  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rr2srcp unhandled event [%s] returns [%s]",
        NodeOp.getName(evt), str );
  }

  return str;
}


static iONode __srcp2rr(iOSrcpCon srcpcon, __iOSrcpService o, const char* req, int *reqRespCode) {
  iOSrcpConData data = Data(srcpcon);
  iONode cmd = NULL;
  iOModel model = AppOp.getModel();
  struct timeval time;
  gettimeofday(&time, NULL);

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__srcp2rr: %s", req );

  /*
   * INIT <bus> FB <optional parameters for initialization>
   * INIT <bus> GA <addr>  <protocol> <optional further parameters>
   * INIT <bus> GL <addr>  <protocol> <optional further parameters>
   * INIT <bus> SM <protocol>
   * INIT <bus> POWER
   * INIT 0 TIME <fx> <fy> 
   */
  if( StrOp.startsWithi( req, "INIT " ) ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "INIT req: %s", req );

    int idx = 0;
    int srcpBus = 0;
    char srcpBusIid[1025] = {'\0'};
    char busType[1025] = {'\0'};
    char sPar3[1025] = {'\0'};
    char sPar4[1025] = {'\0'};
    char sPar5[1025] = {'\0'};
    iOStrTok tok = StrTokOp.inst(req, ' ');

    while( StrTokOp.hasMoreTokens(tok)) {
      const char* s = StrTokOp.nextToken(tok);
      switch(idx) {
      case 1: srcpBus = atoi(s) ; break;
      case 2: StrOp.copy( busType, s ) ; break;
      case 3: StrOp.copy( sPar3, s); break;
      case 4: StrOp.copy( sPar4, s); break;
      case 5: StrOp.copy( sPar5, s); break;
      }
      idx++;
    };
    StrTokOp.base.del(tok);

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Parts INIT bus[%d] type[%s] par3[%s] par4[%s] par5[%s]",
          srcpBus, busType, sPar3, sPar4, sPar5 );

    getSrcpIid( data, srcpBus, srcpBusIid);

    if( StrOp.equalsi( busType, "POWER" )) {
      /* INIT <bus> POWER */
      if( idx < 3 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 3 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }
      if( srcpBus == 0 ) {
        /* server itself has no power state */
        /* 422 ERROR unsupported device group */
        *reqRespCode = (int) 422;
        return cmd;
      }
      if( ! isValidSrcpBus( data, srcpBus ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 412 ERROR wrong value", req);
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
        return cmd;
      }

      /* answer for successful init is "200 OK", generated automatically in calling function */
    } /* POWER */

    else if( StrOp.equalsi( busType, "GA" )) {
      /* INIT <bus> GA <addr> <protocol> <optional further parameters> */
      if( idx < 5 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }

      if( srcpBus == 0 ) {
        /* server itself has no accessory bus */
        /* 422 ERROR unsupported device group */
        *reqRespCode = (int) 422;
        return cmd;
      }

      if( ! isValidSrcpBus( data, srcpBus ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 412 ERROR wrong value", req);
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
        return cmd;
      }

      char protoGA[1025] = {'\0'};
      iOSwitch sw;
      iOSignal sg;
      int addrGA = atoi( sPar3 );
      StrOp.copy( protoGA, sPar4 ); /* TODO: verify reasonable value ? */

      sw = SRCPgetSwByAddressAndIid( model, addrGA, -1, srcpBusIid );
      sg = SRCPgetSgByAddressAndIid( model, addrGA, srcpBusIid );

      if( (sw == 0) && (sg == 0) ) {
        /* no suitable switch or signal found */
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Invalid GA at %d, reqRespCode %d", addrGA, *reqRespCode );
      }else{
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Valid GA at %d", addrGA );
      }

      /* answer for successful init is "200 OK", generated automatically in calling function */
    } /* GA */

    else if( StrOp.equalsi( busType, "GL" )) {
      /* INIT <bus> GL <addr>  <protocol> <optional further parameters> */
      if( idx < 5 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }

      if( srcpBus == 0 ) {
        /* server itself has no loco bus */
        /* 422 ERROR unsupported device group */
        *reqRespCode = (int) 422;
        return cmd;
      }

      if( ! isValidSrcpBus( data, srcpBus ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 412 ERROR wrong value", req);
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
        return cmd;
      }

      int addrGL = atoi( sPar3 );
      iOLoc loco = SRCPgetLocByAddressAndIid(model, addrGL, srcpBusIid );

      if( loco == NULL ) {
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Invalid GL at %d, reqRespCode %d", addrGL, *reqRespCode );
      }

      /* answer for successful init is "200 OK", generated automatically in calling function */
    } /* GL */

    else if( StrOp.equalsi( busType, "FB" )) {
      /* INIT <bus> FB <optional parameters for initialization> */
      if( idx < 3 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }

      if( srcpBus == 0 ) {
        /* server itself has no feedback bus */
        /* 422 ERROR unsupported device group */
        *reqRespCode = (int) 422;
        return cmd;
      }

      if( ! isValidSrcpBus( data, srcpBus ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 412 ERROR wrong value", req);
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
        return cmd;
      }

      /* answer for successful init is "200 OK", generated automatically in calling function */
    } /* FB */

    else if( StrOp.equalsi( busType, "SM" )) {
      /* INIT <bus> SM <protocol> */
      /* 422 ERROR unsupported device group */
      *reqRespCode = (int) 422;
      return cmd;
    } /* SM */

    else if( StrOp.equalsi( busType, "TIME" )) {
      /* INIT 0 TIME <fx> <fy>  */
      if( idx < 5 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 5 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }
      if( srcpBus != 0 ) {
        /* only server has a time device */
        /* 422 ERROR unsupported device group */
        *reqRespCode = (int) 422;
        return cmd;
      }

      int tFx = atoi( sPar3 );
      int tFy = atoi( sPar4 );

      if( (tFx <= 0) || (tFx > 100) || (tFy != 1) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 412 ERROR wrong value", req);
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
        return cmd;
      }

      iONode clockini = wRocRail.getclock( AppOp.getIni() );
      if( clockini == NULL ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s : RocrailClock not available -> 416 ERROR no data", req);
        /* 416 ERROR no data */
        *reqRespCode = (int) 416 ;
        return cmd;
      }

      int ini_divider = wClock.getdivider(clockini);
      /* use current rocrail time */
      long modeltime = ControlOp.getTime( AppOp.getControl() );

      if( tFx != ini_divider ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__srcp2rr: ini_divider_ini[%d] new_divider[%d]", ini_divider, tFx );

        iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
        wClock.setcmd( tick, wClock.set );
        wClock.settime( tick, modeltime );
        wClock.setdivider( tick, tFx );
        data->callback( data->callbackObj, tick );
        divider = tFx;
      }

      if( ! __isClockRunning() ) {
        iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
        wClock.setcmd( tick, wClock.go );
        data->callback( data->callbackObj, tick );

        __setClockRunning( True );
      }

      /* answer for successful init is "200 OK", generated automatically in calling function */
    } /* TIME */

    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Unhandled INIT REQ %s", req );
      /* 422 ERROR unsupported device group */
      *reqRespCode = (int) 422 ;
    }
  } /* INIT */

  else if( StrOp.startsWithi( req, "SET " ) ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET req: %s", req );

    if( StrOp.findi( req, "POWER" ) ) {
      /* SET <bus> POWER [ON|OFF] [freetext] */
      int idx = 0;
      char str[1025] = {'\0'};
      int srcpBus = 0;
      char busType[1025] = {'\0'};
      char optionString[1025] = {'\0'};
      char *freeText = NULL;
      iOStrTok tok = StrTokOp.inst(req, ' ');

      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 0: break;
        case 1: srcpBus = atoi(s); break;
        case 2: StrOp.copy( busType, s); break;
        case 3: StrOp.copy( optionString, s ); break;
        default:
          if( StrOp.len( s ) > 0 ) {
            freeText = StrOp.cat( freeText, " ");
            freeText = StrOp.cat( freeText, s);
          }
          break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      if( idx < 4 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }

      if( srcpBus == 0 ) {
        /* server itself has no power state */
        /* 422 ERROR unsupported device group */
        *reqRespCode = (int) 422;
        return cmd;
      }

      if( ! StrOp.equalsi( busType, "POWER" ) ) {
        /* 410 ERROR unknown command */
        *reqRespCode = (int) 410;
        return cmd;
      }
      if( ! isValidSrcpBus( data, srcpBus ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 412 ERROR wrong value", req);
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
        return cmd;
      }

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__srcp2rr: SET %d POWER %s freeText[%s] (srcpPwFreetext[%s])",
          srcpBus, optionString, freeText, __getSrcpPwFreetext() );

      if( StrOp.len( freeText ) > 0 && ! StrOp.equals( freeText, __getSrcpPwFreetext() ))
        __setSrcpPwFreetext( freeText );
      else
        __setSrcpPwFreetext( "" );

      if( freeText != NULL )
        StrOp.free( freeText );

      /* current overall system state (ON/OFF) */
      Boolean isPower = wState.ispower(ControlOp.getState(AppOp.getControl()));

      if( StrOp.equalsi( optionString, "OFF" ) ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__srcp2rr: SET %d POWER %s %s", srcpBus, optionString, freeText);
        if( isPower ) { /* execute only if different from current state */
          srcpPwCmdInProgress = True;
          iONode localCmd = NodeOp.inst(wSysCmd.name(), NULL, ELEMENT_NODE );
          wSysCmd.setcmd( localCmd, wSysCmd.stop );
          data->callback( data->callbackObj, localCmd );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP sent wSysCmd.stop" );
          ThreadOp.sleep( 250 );

          sendAllPWstates2AllInfoChannels( data );
          srcpPwCmdInProgress = False;
        }
      }
      else if( StrOp.equalsi( optionString, "ON" ) ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__srcp2rr: SET %d POWER %s %s", srcpBus, optionString, freeText);
        if( ! isPower ) { /* execute only if different from current state */
          srcpPwCmdInProgress = True;
          iONode localCmd = NodeOp.inst(wSysCmd.name(), NULL, ELEMENT_NODE );
          wSysCmd.setcmd( localCmd, wSysCmd.go );
          data->callback( data->callbackObj, localCmd );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP sent wSysCmd.go" );
          ThreadOp.sleep( 250 );

          sendAllPWstates2AllInfoChannels( data );
          srcpPwCmdInProgress = False;
        }
      }

      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "unknown/unhandled POWER option in req %s", req );
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412;
      }

      /* answer for successful SET is "200 OK", generated automatically in calling function */
    } /* POWER */

    else if( StrOp.findi( req, "GL" ) ) {
      /* SET <bus> GL <addr> <drivemode> <V> <V_max> <f1> . . <fn> */
      int idx = 0;
      const char* lcID = NULL;
      int srcpBus = 0;
      char srcpBusIid[1025] ;
      int srcpLoco = 0;
      Boolean srcpDir = True;
      int srcpNewStep = 0;
      int srcpMaxStep = 0;
      Boolean srcpF0 = False;
      int srcpFx = 0;

      iOStrTok tok = StrTokOp.inst(req, ' ');

      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 1:
          srcpBus = atoi(s);
          getSrcpIid( data, srcpBus, srcpBusIid );
          break;
        case 3: {
          iOLoc loco = NULL;
          srcpLoco = atoi(s);
          loco = SRCPgetLocByAddressAndIid(model, srcpLoco, srcpBusIid );
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
          if( s[0] == '0') srcpF0 = False;
          if( s[0] == '1') srcpF0 = True;
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
        iONode loProps = LocOp.base.properties(ModelOp.getLoc(model, lcID, NULL, False));

        int loAddr    = wLoc.getaddr(loProps);
        int loV       = wLoc.getV(loProps);
        int loVmax    = wLoc.getV_max(loProps);
        int loSpcnt   = wLoc.getspcnt(loProps);
        int loDir     = wLoc.isdir( loProps );
        Boolean loFn  = wLoc.isfn( loProps );
        int loFnCnt   = wLoc.getfncnt(loProps);
        int loFx      = wLoc.getfx(loProps);
        int newSpeed  = wLoc.getV(loProps) != -1 ? wLoc.getV(loProps):0;
        int newStep   = 0 ;
        int oldStep   = 0 ;
        int divisor   = 1 ;

        if( wLoc.getV( loProps ) != -1 ) {
          if( StrOp.equals( wLoc.getV_mode( loProps ), wLoc.V_mode_percent ) ){
            divisor = 100;
          }
          else if( loVmax > 0 ){
            divisor = loVmax;
          }

          newSpeed = loV;
          newStep = ( newSpeed * loSpcnt) / divisor;
          oldStep = ( loV      * loSpcnt) / divisor;

          if( newStep > loSpcnt ) {
            newStep = loSpcnt;
          }

          if( srcpNewStep == 0) { /* halt loco */
            /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "srcpNewStep %d == 0 %d loco halt", srcpNewStep); */
            newSpeed = 0;
          }
          else if( newStep == srcpNewStep ) {
              /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "newStep %d == srcpNewStep %d adjust none", newStep, srcpNewStep); */
          }
          else if( srcpNewStep > oldStep ) { /* increase speed */
            /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "srcpNewStep %d > oldStep %d", srcpNewStep, oldStep); */
            while( newStep < srcpNewStep ) {
              /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "newStep %d < srcpNewStep %d adjust ++", newStep, srcpNewStep); */
              newSpeed++;
              newStep = (newSpeed * loSpcnt) / divisor ;
            }
          }
          else if( srcpNewStep < oldStep ) { /* slow down */
            /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "srcpNewStep %d < oldStep %d", srcpNewStep, oldStep); */
            while( newStep > srcpNewStep ) {
              /* TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "newStep %d < srcpNewStep %d adjust --", newStep, srcpNewStep); */
              newSpeed--;
              newStep = (newSpeed * loSpcnt) / divisor ;
            }
          }
          newSpeed = ( newSpeed > loVmax ) ? loVmax : newSpeed;
        }
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "loAddr %d : OldSpeed %d OldStep %d NewSpeed %d newStep %d",
              loAddr, loV, oldStep, newSpeed, srcpNewStep);

        if( loFnCnt > 0 ) {
          /* send all functions as group 0 */ 
          iONode fcmd = NodeOp.inst(wFunCmd.name(), NULL, ELEMENT_NODE );
          int group = 0;

          wFunCmd.setid( fcmd, lcID);
          wFunCmd.setgroup(fcmd, group );
          wFunCmd.setfncnt ( fcmd, loFnCnt );
          wFunCmd.setf28(fcmd, (srcpFx & 0x08000000)?True:False);
          wFunCmd.setf27(fcmd, (srcpFx & 0x04000000)?True:False);
          wFunCmd.setf26(fcmd, (srcpFx & 0x02000000)?True:False);
          wFunCmd.setf25(fcmd, (srcpFx & 0x01000000)?True:False);
          wFunCmd.setf24(fcmd, (srcpFx & 0x00800000)?True:False);
          wFunCmd.setf23(fcmd, (srcpFx & 0x00400000)?True:False);
          wFunCmd.setf22(fcmd, (srcpFx & 0x00200000)?True:False);
          wFunCmd.setf21(fcmd, (srcpFx & 0x00100000)?True:False);
          wFunCmd.setf20(fcmd, (srcpFx & 0x00080000)?True:False);
          wFunCmd.setf19(fcmd, (srcpFx & 0x00040000)?True:False);
          wFunCmd.setf18(fcmd, (srcpFx & 0x00020000)?True:False);
          wFunCmd.setf17(fcmd, (srcpFx & 0x00010000)?True:False);
          wFunCmd.setf16(fcmd, (srcpFx & 0x00008000)?True:False);
          wFunCmd.setf15(fcmd, (srcpFx & 0x00004000)?True:False);
          wFunCmd.setf14(fcmd, (srcpFx & 0x00002000)?True:False);
          wFunCmd.setf13(fcmd, (srcpFx & 0x00001000)?True:False);
          wFunCmd.setf12(fcmd, (srcpFx & 0x00000800)?True:False);
          wFunCmd.setf11(fcmd, (srcpFx & 0x00000400)?True:False);
          wFunCmd.setf10(fcmd, (srcpFx & 0x00000200)?True:False);
          wFunCmd.setf9( fcmd, (srcpFx & 0x00000100)?True:False);
          wFunCmd.setf8( fcmd, (srcpFx & 0x00000080)?True:False);
          wFunCmd.setf7( fcmd, (srcpFx & 0x00000040)?True:False);
          wFunCmd.setf6( fcmd, (srcpFx & 0x00000020)?True:False);
          wFunCmd.setf5( fcmd, (srcpFx & 0x00000010)?True:False);
          wFunCmd.setf4( fcmd, (srcpFx & 0x00000008)?True:False);
          wFunCmd.setf3( fcmd, (srcpFx & 0x00000004)?True:False);
          wFunCmd.setf2( fcmd, (srcpFx & 0x00000002)?True:False);
          wFunCmd.setf1( fcmd, (srcpFx & 0x00000001)?True:False);
          wFunCmd.setf0( fcmd,  srcpF0);
          data->callback( data->callbackObj, fcmd );
        }
        /* send new loco basic settings after sending all functions */
        cmd = NodeOp.inst(wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setid(cmd, lcID);
        wLoc.setdir(cmd, srcpDir);
        wLoc.setfn(cmd, srcpF0);
        wLoc.setV(cmd, newSpeed);
        data->callback( data->callbackObj, cmd );
        cmd = NULL ;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "No loco with addr %d found", srcpLoco ) ;
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
      }
    } /* GL */

    else if( StrOp.findi( req, "GA" ) ) {
      /* SET <bus> GA <addr> <port> <value> <delay> */
      int idx = 0;
      const char* swID = NULL;
      int  srcpBus  = 0;
      char srcpBusIid[1024];
      int  srcpAddr = 0;
      int  addr     = 0;
      int  port     = 0;
      int  gate     = 0;
      int  value    = 0;
      int  delay    = 0;
      iOSwitch sw;
      iOSignal sg;
      iOStrTok tok = StrTokOp.inst(req, ' ');

      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 1: srcpBus  = atoi(s); break;
        case 3: srcpAddr = atoi(s); break;
        case 4: gate     = atoi(s); break;
        case 5: value    = atoi(s); break;
        case 6: delay    = atoi(s); break; /* TODO: use delay ? No, not yet */
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      if( idx < 7 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 7 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET GA: srcpBus %d srcpAddr %d [%d-%d] gate %d value %d delay %d ",
                   srcpBus, srcpAddr, addr, port, gate, value, delay );

      if( delay == 0 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 412 ERROR wrong value (0 not allowed)", req);
        *reqRespCode = (int) 412 ;
        return cmd;
      }

      getSrcpIid( data, srcpBus, srcpBusIid);

      /* Find switch */
      sw = SRCPgetSwByAddressAndIid( model, srcpAddr, gate, srcpBusIid );

      if( (sw != NULL) && SwitchOp.isLocked( sw, NULL, True ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Switch \"%s\" is locked", SwitchOp.getId( sw ) );
        /* 414 ERROR device locked */
        *reqRespCode = (int) 414 ;
        return cmd;
      }
 
      if( sw != NULL ) {
        iONode swProps = SwitchOp.base.properties(sw);
        const char *swIid = StrOp.equals( wSwitch.getiid(swProps), "") ? getDefaultIid() : wSwitch.getiid(swProps);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "swIid \"%s\" srcpBusIid \"%s\"", wSwitch.getiid(swProps), srcpBusIid ) ;

        int addr1 = AddrOp.toPADA( wSwitch.getaddr1(swProps), wSwitch.getport1(swProps) );
        int addr2 = AddrOp.toPADA( wSwitch.getaddr2(swProps), wSwitch.getport2(swProps) );
        int gate1 = wSwitch.getgate1(swProps);
        int gate2 = wSwitch.getgate2(swProps);
        Boolean singlegate = wSwitch.issinglegate(swProps);

        if( singlegate ) {
          cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
          wSwitch.setiid( cmd, srcpBusIid );
          wSwitch.setid(  cmd, SwitchOp.getId(sw) );
          wSwitch.setcmd( cmd, value?wSwitch.turnout:wSwitch.straight );
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "srcpAddr %d gate %d value %d addr %d port %d srcpBus %d srcpBusIid \"%s\"", srcpAddr, gate, value, addr, port, srcpBus, srcpBusIid ) ;
        }
        else if(  ( StrOp.equals( wSwitch.gettype(swProps), wSwitch.left))
          || ( StrOp.equals( wSwitch.gettype(swProps), wSwitch.right))
          || ( StrOp.equals( wSwitch.gettype(swProps), wSwitch.twoway))
          || ( StrOp.equals( wSwitch.gettype(swProps), wSwitch.crossing))
          || ( StrOp.equals( wSwitch.gettype(swProps), wSwitch.ccrossing))
          || ( StrOp.equals( wSwitch.gettype(swProps), wSwitch.accessory)) ) {
          cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
          wSwitch.setiid( cmd, srcpBusIid );
          wSwitch.setid(  cmd, SwitchOp.getId(sw) );
          wSwitch.setcmd( cmd, gate?wSwitch.straight:wSwitch.turnout );
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "srcpAddr %d addr %d port %d srcpBus %d srcpBusIid \"%s\"", srcpAddr, addr, port, srcpBus, srcpBusIid ) ;
        }
        else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.threeway)) {
          /* straight left right */
          const char *currState = wSwitch.getstate(swProps);

          if( ( addr1 == srcpAddr ) && ( gate == 0 ) ) {
            if( StrOp.equals( currState, wSwitch.left) ) {
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setiid( cmd, srcpBusIid );
              wSwitch.setid(  cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, wSwitch.straight );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d gate %d : type %s currstate %s set to straight", srcpAddr, gate, wSwitch.threeway, currState ) ;
            }
          }else if( ( addr1 == srcpAddr ) && ( gate == 1 ) ) {
            if( StrOp.equals( currState, wSwitch.straight) || StrOp.equals( currState, wSwitch.right) ) {
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setiid( cmd, srcpBusIid );
              wSwitch.setid(  cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, wSwitch.left );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d gate %d : type %s currstate %s set to left", srcpAddr, gate, wSwitch.threeway, currState ) ;
            }
          }else if( ( addr2 == srcpAddr ) && ( gate == 0 ) ) {
            if( StrOp.equals( currState, wSwitch.right) ) {
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setiid( cmd, srcpBusIid );
              wSwitch.setid(  cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, wSwitch.straight );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d gate %d : type %s currstate %s set to straight", srcpAddr, gate, wSwitch.threeway, currState ) ;
            }
          }else if( ( addr2 == srcpAddr ) && ( gate == 1 ) ) {
            if( StrOp.equals( currState, wSwitch.straight) || StrOp.equals( currState, wSwitch.left) ) {
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setiid( cmd, srcpBusIid );
              wSwitch.setid(  cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, wSwitch.right );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d gate %d : type %s currstate %s set to right", srcpAddr, gate, wSwitch.threeway, currState ) ;
            }
          }
        }
        else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.dcrossing)) {
          /* straight right left turnout */

          const char *currState = wSwitch.getstate(swProps);
          const char *nextState ;

          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "srcp2rr DCROSSING currState %s addr1 %d addr2 %d gate1 %d gate2 %d value %d req %s", currState, addr1, addr2, gate1, gate2, value, req );

          if( ( addr1 == srcpAddr ) && ( 0 == gate ) ) {
            if( StrOp.equals( currState, wSwitch.turnout) ) {
              nextState = wSwitch.right;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setiid( cmd, srcpBusIid );
              wSwitch.setid(  cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d gate %d : type %s Set from %s to %s ", srcpAddr, port, gate, wSwitch.dcrossing, currState, nextState ) ;
            }else if( StrOp.equals( currState, wSwitch.left) ) {
              nextState = wSwitch.straight;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setiid( cmd, srcpBusIid );
              wSwitch.setid(  cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d gate %d : type %s Set from %s to %s ", srcpAddr, port, gate, wSwitch.dcrossing, currState, nextState ) ;
            }
          }else if( ( addr1 == srcpAddr ) && ( 1 == gate ) ) {
            if( StrOp.equals( currState, wSwitch.straight) ) {
              nextState = wSwitch.left;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setiid( cmd, srcpBusIid );
              wSwitch.setid(  cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d gate %d : type %s Set from %s to %s ", srcpAddr, port, gate, wSwitch.dcrossing, currState, nextState ) ;
            }else if( StrOp.equals( currState, wSwitch.right) ) {
              nextState = wSwitch.turnout;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setiid( cmd, srcpBusIid );
              wSwitch.setid(  cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d gate %d : type %s Set from %s to %s ", srcpAddr, port, gate, wSwitch.dcrossing, currState, nextState ) ;
            }
          }else if( ( addr2 == srcpAddr ) && ( 0 == gate ) ) {
            if( StrOp.equals( currState, wSwitch.right) ) {
              nextState = wSwitch.straight;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setiid( cmd, srcpBusIid );
              wSwitch.setid(  cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d gate %d : type %s Set from %s to %s ", srcpAddr, port, gate, wSwitch.dcrossing, currState, nextState ) ;
            }else if( StrOp.equals( currState, wSwitch.turnout) ) {
              nextState = wSwitch.left;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setiid( cmd, srcpBusIid );
              wSwitch.setid(  cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d gate %d : type %s Set from %s to %s ", srcpAddr, port, gate, wSwitch.dcrossing, currState, nextState ) ;
            }
          }else if( ( addr2 == srcpAddr ) && ( 1 == gate ) ) {
            if( StrOp.equals( currState, wSwitch.straight) ) {
              nextState = wSwitch.right;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setiid( cmd, srcpBusIid );
              wSwitch.setid(  cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d gate %d : type %s Set from %s to %s ", srcpAddr, port, gate, wSwitch.dcrossing, currState, nextState ) ;
            }else if( StrOp.equals( currState, wSwitch.left) ) {
              nextState = wSwitch.turnout;
              cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
              wSwitch.setiid( cmd, srcpBusIid );
              wSwitch.setid(  cmd, SwitchOp.getId(sw) );
              wSwitch.setcmd( cmd, nextState );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SW %d port %d gate %d : type %s Set from %s to %s ", srcpAddr, port, gate, wSwitch.dcrossing, currState, nextState ) ;
            }
          }else{
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SET for SW NO COMBO FOR addr/port/gate found aGA %d a %d p %d gate %d value %d : type %s a1 %d a2 %d g1 %d g2 %d REQ %s", srcpAddr, addr, port, gate, value, wSwitch.gettype(swProps), addr1, addr2, gate1, gate2, req );
          }
        }
        else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.decoupler)) {
          Boolean isSingle = wSwitch.issinglegate(swProps);

          if( ! isSingle ) {
            cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
            wSwitch.setiid( cmd, srcpBusIid );
            wSwitch.setid(  cmd, SwitchOp.getId(sw) );
            wSwitch.setcmd( cmd, gate?wSwitch.straight:wSwitch.turnout );
          }else {
            iOMap swMap = NULL;

            swMap = ModelOp.getSwitchMap(model);
            iOSwitch swM = (iOSwitch)MapOp.first( swMap );

            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "TYPE %s # %d set to %s\naddr %d port %d gate %d value %d isSingle %d", wSwitch.gettype(swProps), srcpAddr, gate?wSwitch.straight:wSwitch.turnout, addr, port, gate, value, isSingle?1:0 );

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

                TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "desc %s id %s type %s addr1 %d port 1 %d gate1 %d isSingle %d currState %s", desc, id, wSwitch.gettype(swPropsM), addr1, wSwitch.getport1(swPropsM), gate1,  isSingle?1:0, currState ) ;

                cmd = NodeOp.inst(wSwitch.name(), NULL, ELEMENT_NODE );
                wSwitch.setiid( cmd, srcpBusIid );
                wSwitch.setid(  cmd, id );
                wSwitch.setcmd( cmd, gate?wSwitch.straight:wSwitch.turnout );
              }
              swM = (iOSwitch)MapOp.next( swMap );
            }
          }
        }else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Unknown GA-type %s with address %d", wSwitch.gettype(swProps),srcpAddr );
        }
      }
      
      sg = SRCPgetSgByAddressAndIid( model, srcpAddr, srcpBusIid );

      if( sg != NULL ) {
        iONode sgProps = SignalOp.base.properties(sg);
        const char *sgIid = StrOp.equals( wSignal.getiid(sgProps), "") ? getDefaultIid() : wSignal.getiid(sgProps);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sgIid \"%s\" srcpBusIid \"%s\"", wSignal.getiid(sgProps), srcpBusIid ) ;

        if( StrOp.equals( sgIid, srcpBusIid) ){
          int aspects = wSignal.getaspects( sgProps );
          int addr1 = AddrOp.toPADA( wSignal.getaddr (sgProps), wSignal.getport1(sgProps) );
          int addr2 = AddrOp.toPADA( wSignal.getaddr2(sgProps), wSignal.getport2(sgProps) );
          int addr3 = AddrOp.toPADA( wSignal.getaddr3(sgProps), wSignal.getport3(sgProps) );
          int addr4 = AddrOp.toPADA( wSignal.getaddr4(sgProps), wSignal.getport4(sgProps) );
          int gate1 = wSignal.getgate1(sgProps);
          int gate2 = wSignal.getgate2(sgProps);
          int gate3 = wSignal.getgate3(sgProps);
          int gate4 = wSignal.getgate4(sgProps);

          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET for SG aGA %d a %d p %d gate %d : aspects %d a1 %d a2 %d a3 %d a4 %d g1 %d g2 %d g3 %d g4 %d REQ %s", 
              srcpAddr, addr, port, gate, aspects, addr1, addr2, addr3, addr4, gate1, gate2, gate3, gate4, req );

          /* if cmd != NULL (a sw command was already created) execute that command before creating new command for sg */
          if ( cmd != NULL ) {
            data->callback( data->callbackObj, cmd );
          }

          cmd = NodeOp.inst(wSignal.name(), NULL, ELEMENT_NODE );
          wSignal.setiid( cmd, srcpBusIid );
          wSignal.setid( cmd, SignalOp.getId(sg) );

          if( ( addr1 == srcpAddr ) && ( gate1 == gate ) ) {
            wSignal.setcmd( cmd, wSignal.red );
          }else if( ( addr2 == srcpAddr ) && ( gate2 == gate ) ) {
            wSignal.setcmd( cmd, wSignal.green );
          }else if( ( aspects >= 3 ) && ( addr3 == srcpAddr ) && ( gate3 == gate ) ) {
            wSignal.setcmd( cmd, wSignal.yellow );
          }else if( ( aspects >= 4 ) && ( addr4 == srcpAddr ) && ( gate4 == gate ) ) {
            wSignal.setcmd( cmd, wSignal.white );
          }else {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "for SG NO COMOBO FOR addr/gate found iid %s aGA %d a %d p %d gate %d : aspects %d a1 %d a2 %d a3 %d a4 %d g1 %d g2 %d g3 %d g4 %d REQ %s", 
                         srcpBusIid, srcpAddr, addr, port, gate, aspects, addr1, addr2, addr3, addr4, gate1, gate2, gate3, gate4, req );
          }
        }
      }
      if( ( sw == NULL ) && ( sg == NULL ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no switch or signal found for found iid %s aGA %d a %d p %d gate %d : REQ %s", 
                     srcpBusIid, srcpAddr, addr, port, gate, req );
        /* 416 ERROR no data */
        *reqRespCode = (int) 416 ;
      }

      /* answer for successful SET is "200 OK", generated automatically in calling function */
    } /* GA */

    else if( StrOp.findi( req, "FB" ) ) {
      /* SET <bus> FB <addr> <value> */
      int idx = 0;
      const char* fbID = NULL;
      int  srcpBus  = 0;
      char srcpBusIid[1024];
      int  srcpAddr = 0;
      int  value    = -1;
      iOFBack fb;
      iOStrTok tok = StrTokOp.inst(req, ' ');

      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 1: srcpBus  = atoi(s); break;
        case 3: srcpAddr = atoi(s); break;
        case 4: value    = atoi(s); break;
        case 5: break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      if( idx < 5 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 5 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }
      getSrcpIid( data, srcpBus, srcpBusIid);

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET FB srcpBus[%d]=[%s] srcpAddr[%d] value[%d]",
                   srcpBus, srcpAddr, value, srcpBusIid );

      /* find feedback */
      fb = getFeedbackByIidAndAddr( srcpBusIid, srcpAddr );

      if( fb != NULL ) {
        if( (value == 0) || (value == 1) ) {
          if( ! isDigintVirtual( srcpBusIid ) ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sensor simulation on none vcs: bus[%d]=[%s] : REQ %s", 
                         srcpBus, srcpBusIid, req );
          }
          fbID = FBackOp.getId( fb );
          iONode cmd = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
          wFeedback.setid( cmd, fbID );
          wFeedback.setstate( cmd, value?True:False);
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "simulate sensor[%s] addr[%s][%d] state[%s]",
                       fbID, srcpBusIid, srcpAddr, value?"true":"false" );
          FBackOp.event( fb, cmd );
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "feedback value invalid iid %s aFB %d val %d : REQ %s", 
                       srcpBusIid, srcpAddr, value, req );
          /* 412 ERROR wrong value */
          *reqRespCode = (int) 412 ;
        }
      }

      if( fb == NULL ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no feedback found for found iid %s aFB %d val %d : REQ %s", 
                     srcpBusIid, srcpAddr, value, req );
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
      }

      /* answer for successful SET is "200 OK", generated automatically in calling function */
    } /* FB */

    else if( StrOp.startsWithi( req, "SET 0 TIME" ) ) {
      /* SET 0 TIME <JulDay> <Hour> <Minute> <Second> */
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SET 0 TIME ..." );

      int idx = 0;
      long julDay = -1;
      int hour = -1;
      int min  = -1;
      int sec  = -1;
      iOStrTok tok = StrTokOp.inst(req, ' ');

      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 3: julDay = atol(s); break;
        case 4: hour = atoi(s); break;
        case 5: min  = atoi(s); break;
        case 6: sec  = atoi(s); break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      if( idx < 7 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 7 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "__srcp2rr: SET 0 TIME %ld %d %d %d",
                   julDay, hour, min, sec );

      if( (julDay < JULIAN_DAY_1970_01_01 ) || (hour < 0) || (hour >=24) || (min < 0) || (min >= 60) || (sec < 0 ) || ( sec >= 60 ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "invalid value in date detected: %s", 
            req );
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
      }

      iONode clockini = wRocRail.getclock( AppOp.getIni() );
      int ini_divider = wClock.getdivider(clockini);
      if( ini_divider == 1 ) {
        /* rocrail running with realtime -> time setting not allowed */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "running with realtime -> 415 ERROR forbidden");
        /* 415 ERROR forbidden */
        *reqRespCode = (int) 415 ;
        return cmd;
      }

      time_t new_time = convSRCP2ModelTime( julDay, hour, min, sec );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> new model time[%d] == SRCP time[%s] ",
          req, (int) new_time, convModelTimeToSRCP(new_time) );

      /* send new_time to RR */
      iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
      wClock.setcmd( tick, wClock.set );
      wClock.setdivider( tick, ini_divider );
      wClock.settime( tick, new_time );
      data->callback( data->callbackObj, tick );
      /* answer for successful SET is "200 OK", generated automatically in calling function */

    } /* SET 0 TIME ... */

    else if( StrOp.findi( req, "SM" ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SET <bus> SM not supported" );
      /* 422 ERROR unsupported device group */
      *reqRespCode = (int) 422 ;
    } /* SM */

    else if( StrOp.findi( req, "GM" ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SET <bus> GM not supported" );
      /* 422 ERROR unsupported device group */
      *reqRespCode = (int) 422 ;
    } /* GM */

    else if( StrOp.findi( req, "LOCK" ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SET <bus> LOCK not supported" );
      /* 422 ERROR unsupported device group */
      *reqRespCode = (int) 422 ;
    } /* LOCK */

    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Unhandled SET REQ %s", req );
      /* 410 ERROR unknown command */
      *reqRespCode = (int) 410 ;
    }
  } /* SET */

  else if( StrOp.startsWithi( req, "GET " ) ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "GET req: %s", req );

    if( StrOp.findi( req, "POWER" ) ) {
      /* GET <bus> POWER */
      int idx = 0;
      char str[1025] = {'\0'};
      int srcpBus = 0;
      char busType[1025] = {'\0'};
      iOStrTok tok = StrTokOp.inst(req, ' ');

      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 1: srcpBus = atoi(s); break;
        case 2: StrOp.copy( busType, s); break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      if( idx < 3 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 3 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }
      if( ! isValidSrcpBus( data, srcpBus ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 412 ERROR wrong value", req);
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
        return cmd;
      }

      if( StrOp.equalsi( busType, "POWER" ) ) {
        /* GET <bus> POWER */
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "POWER[%d]", srcpBus );

        if( srcpBus == 0 ) {
          /* server itself has no power state */
          /* 422 ERROR unsupported device group */
          *reqRespCode = (int) 422;
          return cmd;
        }

        /* current overall system state (ON/OFF) */
        Boolean isPower = wState.ispower(ControlOp.getState(AppOp.getControl()));

        /* create response */
        StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d POWER %s%s\n",
            time.tv_sec, time.tv_usec / 1000L, srcpBus, isPower?"ON":"OFF", __getSrcpPwFreetext() );

        /* send INFO <bus> POWER answer back to requesting command channel */
        __writeRsp(o, str);

        *reqRespCode = (int) 0;
      }
      else {
        /* 422 ERROR unsupported device group */
        *reqRespCode = (int) 422;
      }
    } /* GET <bus> POWER */

    else if( StrOp.findi( req, " DESCRIPTION GL " ) ) {
      /* GET <bus> DESCRIPTION GL <addr> */
      /* 101 INFO 1 DESCRIPTION GL 3 N 1 128 5 */
      /* 101 INFO 1     GL 3      N       1         14           5 */
      /* 101 INFO <bus> GL <addr> <proto> <protver> <speedsteps> <num functions> */
      int idx = 0;
      const char* lcID = NULL;
      int srcpBus = 0;
      int addrGL = 0;
      char srcpBusIid[1024];

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "GET DESCRIPTION REQ %s", req);

      iOStrTok tok = StrTokOp.inst(req, ' ');
      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
          case 1: srcpBus = atoi(s) ; break;
          case 4: addrGL = atoi(s); break;
          case 5: break;
        }
        idx++;
      }
      StrTokOp.base.del(tok);

      if( idx < 5 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 5 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }
      getSrcpIid( data, srcpBus, srcpBusIid);

      if( addrGL > 0 ) {
        iOLoc loco = SRCPgetLocByAddressAndIid(model, addrGL, srcpBusIid);
        if( loco != NULL ) {
          iONode  loProps = LocOp.base.properties(loco);
          const char *loIid = StrOp.equals(wLoc.getiid(loProps), "" ) ? getDefaultIid() : wLoc.getiid(loProps) ;

          if( StrOp.equals(loIid, srcpBusIid) ) {
            char        rsp[1025] = {'\0'};
            int           loSpcnt = wLoc.getspcnt(loProps);
            const char *loDecType = wLoc.getdectype(loProps);
            int           loFnCnt = wLoc.getfncnt(loProps);
            const char    *loProt = wLoc.getprot(loProps);
            int         loProtver = wLoc.getprotver(loProps);
            int           decStep = (wLoc.getV( loProps ) * loSpcnt) / wLoc.getV_max( loProps );
            char         srcpProt = loProt[0];
            int       srcpProtver = loProtver;

            switch(srcpProt) {
              case 'P': break;
              case 'M': break;
              case 'N': srcpProtver=1;break;
              case 'L': srcpProt='N';srcpProtver=2;break;
              case 'A': break;
              case 'C': srcpProt='N';break;
              case 'S': break;
              case 'X': srcpProt='S';break;
            }

            StrOp.fmtb(rsp, "%lu.%.3lu 101 INFO %d GL %d %c %d %d %d\n", 
                time.tv_sec, time.tv_usec / 1000L, srcpBus, addrGL, srcpProt, srcpProtver, loSpcnt, loFnCnt+1 );
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "101 %s", rsp);
            __writeRsp(o, rsp);

            *reqRespCode = (int) 0 ;
          }else {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET REQ %s -> loco with address %d found on %s and not on srcpBus %d (%s) not found", req, addrGL, wLoc.getiid(loProps), srcpBus, srcpBusIid );
            /* 416 ERROR no data */
            *reqRespCode = (int) 416;
          }
        }else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET REQ %s -> locoID for address %d not found", req, addrGL );
          /* 416 ERROR no data */
          *reqRespCode = (int) 416;
        }
      }else {
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412;
      }
    } /* DESCRIPTION GL */

    else if( StrOp.findi( req, "GL" ) ) {
      /* GET <bus> GL <addr> */
      /* "100 INFO <bus> GL <addr> <drivemode> <V> <V_max> <f1> . . <fn>" */
      int idx = 0;
      const char* lcID = NULL;
      int srcpBus = 0;
      int addrGL = 0;
      char srcpBusIid[1024];

      Boolean dir = True;
      int V = 0;

      iOStrTok tok = StrTokOp.inst(req, ' ');
      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
          case 1: srcpBus = atoi(s) ; break;
          case 3: addrGL = atoi(s); break;
          case 4: break;
        }
        idx++;
      }
      StrTokOp.base.del(tok);

      if( idx < 4 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 4 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }

      getSrcpIid( data, srcpBus, srcpBusIid);

      if( addrGL > 0) {
        iOLoc loco = SRCPgetLocByAddressAndIid(model, addrGL, srcpBusIid);
        if( loco != NULL ) {
          iONode loProps = LocOp.base.properties(loco);
          const char *loIid = StrOp.equals(wLoc.getiid(loProps), "" ) ? getDefaultIid() : wLoc.getiid(loProps) ;

          if( StrOp.equals( loIid, srcpBusIid)) {
            Boolean loDir = wLoc.isdir(loProps);
            int   loV = wLoc.getV(loProps);
            int   loVmax = wLoc.getV_max(loProps);
            const char *loVmode = wLoc.getV_mode(loProps);
            int   loSpcnt = wLoc.getspcnt(loProps);
            int decStep = (wLoc.getV( loProps ) * loSpcnt) / wLoc.getV_max( loProps );
            Boolean loFn = wLoc.isfn( loProps );    
            int   loFnCnt = wLoc.getfncnt(loProps);
            int   loFx = wLoc.getfx(loProps);
            char rsp[1025] = {'\0'};
            char freeText[1025] = {'\0'};
            int dir = LocOp.getDir(loco);
            int V = LocOp.getV(loco);
            char funcString[1023];
            funcString[0] = '\0';
            int i, mask;

            for( i=0 ; i < loFnCnt ; i++ ) {
              mask = 1 << i ;
              funcString[2*i] = ' ';
              funcString[2*i+1] = loFx&mask?'1':'0';
            }
            funcString[2*loFnCnt] = '\0';

            StrOp.fmtb(rsp, "%lu.%.3lu 100 INFO %d GL %d %d %d %d %d%s\n", 
                time.tv_sec, time.tv_usec / 1000L, srcpBus, addrGL, loDir?1:0, decStep, loSpcnt, loFn?1:0, funcString);
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s", rsp);
            __writeRsp(o, rsp);

            *reqRespCode = (int) 0 ;
          }else {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET REQ %s -> loco with address %d found on %s and not on srcpBus %d (%s) not found", req, addrGL, wLoc.getiid(loProps), srcpBus, srcpBusIid );
            /* 416 ERROR no data */
            *reqRespCode = (int) 416;
          }
        }
        else{
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET REQ %s -> locoID for address %d not found", req, addrGL );
          /* 416 ERROR no data */
          *reqRespCode = (int) 416;
        }
      }else {
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412;
      }
    } /* GET GL */

    else if( StrOp.findi( req, " DESCRIPTION GA " ) ) {
      /* GET <bus> DESCRIPTION GA <addr> */
      /* 101 INFO <bus> GA <addr> <device protocol> */
      char str[1025] = {'\0'};
      int idx = 0;
      int srcpBus = 0;
      int srcpAddr = 0;
      int addr = 0;
      char srcpBusIid[1024];

      iOStrTok tok = StrTokOp.inst(req, ' ');
      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 1: srcpBus = atoi(s); break;
        case 4: srcpAddr = atoi(s); break;
        case 5: break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      if( idx < 5 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 5 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }

      getSrcpIid( data, srcpBus, srcpBusIid);

      if( srcpAddr > 0 ) {
        char srcpBusIid[1025];
        iOSwitch sw;
        iOSignal sg;
        char prot[1025] = {'\0'};

        sw = SRCPgetSwByAddressAndIid( model, srcpAddr, -1, srcpBusIid );
        sg = SRCPgetSgByAddressAndIid( model, srcpAddr, srcpBusIid );

        /*
          SRCP GA protocols
          M     Maerklin/Motorola Format
                Valid addresses are from 1 to 324, valid ports are 0 or 1, valid values are: 0 and 1 
          N     NMRA-DCC Format
                Valid addresses are from 1 to 511, valid ports are 0 or 1, valid values are: 0 and 1 
          S     Selectrix Format
                Valid addresses are from 0 to 111, valid ports are 1 to 8, valid values are: 0 and 1 
          P     Protocol by server
          
          Rocrail sw/sg protocols: M,N,D,mdd,om32,do,vo
          M      -> M
          N      -> N
          others -> P
        */

        if( sw || sg ) {
          if( sw ) {
            iONode swProps = SwitchOp.base.properties(sw);

            StrOp.copy( prot, wSwitch.getprot(swProps));
            TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SW %d at controller %s : swProt %s", 
                        srcpAddr, srcpBusIid, prot);
          }else {
            iONode sgProps = SignalOp.base.properties(sg);

            StrOp.copy( prot, wSignal.getprot(sgProps));
            TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SG %d at controller %s : sgProt %s", 
                  srcpAddr, srcpBusIid, prot);
          }
          switch( prot[0] & 0xFF ) {
            case 'M':
            case 'N':
              TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "Prot %s OK", prot);
              break;
            default:
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Prot %s not srcp conform, set to P", prot);
              StrOp.copy( prot, "P");
              break;
          }
          StrOp.fmtb(str, "%lu.%.3lu 101 INFO %d GA %d %s\n", time.tv_sec, time.tv_usec / 1000L, srcpBus, srcpAddr, prot );
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Answer: %s", str);
          __writeRsp(o, str);
          *reqRespCode = (int) 0 ;
        }

        if( ! sw && ! sg ){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET REQ %s -> switch/signal on %s for address %d not found", req, srcpBusIid, srcpAddr );
          /* 416 ERROR no data */
          *reqRespCode = (int) 416;
        }
      }else {
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412;
      }
    } /* DESCRIPTION GA */

    else if( StrOp.findi( req, "GA" ) ) {
      /* GET <bus> GA <addr> <port> */
      /* "100 INFO <bus> GA <addr> <port> <value>" */
      char str[1025] = {'\0'};
      int idx = 0;
      int srcpBus = 0;
      int srcpAddr = 0;
      int srcpPort = 0;
      int addr = 0;
      char srcpBusIid[1024];

      iOStrTok tok = StrTokOp.inst(req, ' ');
      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 1: srcpBus = atoi(s); break;
        case 3: srcpAddr = atoi(s); break;
        case 4: srcpPort = atoi(s); break;
        case 5: break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      if( idx < 5 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 5 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }

      getSrcpIid( data, srcpBus, srcpBusIid);

      if( srcpAddr > 0 ) {
        int value;
        iOSwitch sw;
        iOSignal sg;
        char valStr[1025] = {'\0'};
        Boolean twoMotors = False;

        sw = SRCPgetSwByAddressAndIid( model, srcpAddr, srcpPort, srcpBusIid );
        sg = SRCPgetSgByAddressAndIid( model, srcpAddr, srcpBusIid );

        /* Q: "GET <bus> GA <addr> <port>" */
        /* A: "100 INFO <bus> GA <addr> <port> <value>" */

        value = 0;
        if( sw || sg ) {

          str[0] = '\0';
          /* 416 ERROR no data */
          *reqRespCode = (int) 416 ;
          if( sw ) {
            iONode swProps = SwitchOp.base.properties(sw);
            const char* type = wSwitch.gettype( swProps );

            if( StrOp.equals( wSwitch.dcrossing, type ) || StrOp.equals( wSwitch.threeway, type ) ) {
              twoMotors = True;
            }

            StrOp.copy( valStr, wSwitch.getstate(swProps) );
            TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SW %d at controller %s : twoMotors %d value %s", 
                        srcpAddr, srcpBusIid, twoMotors, valStr );

            Boolean singlegate = wSwitch.issinglegate(swProps);
            if( singlegate ) {
              int addr1 = wSwitch.getaddr1(swProps);
              int port1 = wSwitch.getport1(swProps);
              int gate1 = wSwitch.getgate1(swProps);
              const char* state = wSwitch.getstate(swProps);
              int action = 1;
              if( StrOp.equals( state, wSwitch.straight ) )
                action = 0;

              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "srcpAddr %d srcpPort %d addr %d port %d gate1 %d srcpBus %d srcpBusIid \"%s\"", srcpAddr, srcpPort, addr1, port1, gate1, srcpBus, srcpBusIid ) ;
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "id[%s] singlegate[%d] action[%d] state[%s]", wSwitch.getid(swProps), singlegate, action, state ) ;
              /* 100 INFO <bus> GA <addr> <port> <value> */
              StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d GA %d %d %d\n",
                  time.tv_sec, time.tv_usec / 1000L, srcpBus, srcpAddr, gate1, action );
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "str[%s]", str ) ;
              SocketOp.fmt(o->clntSocket, str);
              *reqRespCode = (int) 0 ;
            }else if(  StrOp.equals( type, wSwitch.left )
                    || StrOp.equals( type, wSwitch.right )
                    || StrOp.equals( type, wSwitch.twoway )
                    || StrOp.equals( type, wSwitch.crossing ) 
                    || StrOp.equals( type, wSwitch.ccrossing )
                    || StrOp.equals( type, wSwitch.accessory )
                    || StrOp.equals( type, wSwitch.decoupler )
                    ) {
              /* 100 INFO <bus> GA <addr> <port> <value> */
              StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d GA %d %d 0\n",
                  time.tv_sec, time.tv_usec / 1000L, srcpBus, srcpAddr,
                  StrOp.equals(wSwitch.getstate(swProps), wSwitch.straight)? 1:0 );
              __writeRsp(o, str);
              *reqRespCode = (int) 0 ;
            }else if(  StrOp.equals( type, wSwitch.threeway )
                    || StrOp.equals( type, wSwitch.dcrossing )
                    ) {
              int addr  = AddrOp.toPADA( wSwitch.getaddr1(swProps), wSwitch.getport1(swProps) );
              int addr2 = AddrOp.toPADA( wSwitch.getaddr2(swProps), wSwitch.getport2(swProps) );
              if( srcpAddr == addr ) {
                StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d GA %d %d 0\n",
                    time.tv_sec, time.tv_usec / 1000L, srcpBus, srcpAddr, 
                    (StrOp.equals(wSwitch.getstate(swProps), wSwitch.left)||StrOp.equals(wSwitch.getstate(swProps), wSwitch.turnout))? 1:0 );
                __writeRsp(o, str);
                *reqRespCode = (int) 0 ;
              }
              if( srcpAddr == addr2 ) {
                StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d GA %d %d 0\n",
                    time.tv_sec, time.tv_usec / 1000L, srcpBus, srcpAddr, 
                    (StrOp.equals(wSwitch.getstate(swProps), wSwitch.right)||StrOp.equals(wSwitch.getstate(swProps), wSwitch.turnout))? 1:0 );
                __writeRsp(o, str);
                *reqRespCode = (int) 0 ;
              }
            }else {
              /* no well known switch type */
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "SRCP RESPONSE: unsupported switchtype[%s]", type ) ;
            }
          }else {
            iONode sgProps = SignalOp.base.properties(sg);
            int aspects = wSignal.getaspects( sgProps );
            const char* state = wSignal.getstate(sgProps);

            StrOp.copy( valStr, wSignal.getstate(sgProps) );
            TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SG %d at controller %s : aspects %d value %s", 
                  srcpAddr, srcpBusIid, aspects, valStr );

            int addr1 = AddrOp.toPADA( wSignal.getaddr (sgProps), wSignal.getport1(sgProps) );
            int addr2 = AddrOp.toPADA( wSignal.getaddr2(sgProps), wSignal.getport2(sgProps) );
            int addr3 = AddrOp.toPADA( wSignal.getaddr3(sgProps), wSignal.getport3(sgProps) );
            int addr4 = AddrOp.toPADA( wSignal.getaddr4(sgProps), wSignal.getport4(sgProps) );
            int gate1 = wSignal.getgate1(sgProps);
            int gate2 = wSignal.getgate2(sgProps);
            int gate3 = wSignal.getgate3(sgProps);
            int gate4 = wSignal.getgate4(sgProps);

            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "GET for SG aGA %d : aspects %d a1 %d a2 %d a3 %d a4 %d g1 %d g2 %d g3 %d g4 %d REQ %s", 
                srcpAddr, aspects, addr1, addr2, addr3, addr4, gate1, gate2, gate3, gate4, req );

            if( aspects >= 1 && srcpAddr == addr1 && srcpPort == gate1 ) {
              StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d GA %d %d %d\n", 
                    time.tv_sec, time.tv_usec / 1000L, srcpBus, srcpAddr, srcpPort, 0 );
              TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SG answer aspect 1: %s", str);
              __writeRsp(o, str);
              *reqRespCode = (int) 0 ;
            }
            if( aspects >= 2 && srcpAddr == addr2 && srcpPort == gate2 ) {
              StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d GA %d %d %d\n", 
                    time.tv_sec, time.tv_usec / 1000L, srcpBus, srcpAddr, srcpPort, 0 );
              TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SG answer aspect 2: %s", str);
              __writeRsp(o, str);
              *reqRespCode = (int) 0 ;
            }
            if( aspects >= 3 && srcpAddr == addr3 && srcpPort == gate3 ) {
              StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d GA %d %d %d\n", 
                    time.tv_sec, time.tv_usec / 1000L, srcpBus, srcpAddr, srcpPort, 0 );
              TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SG answer aspect 3: %s", str);
              __writeRsp(o, str);
              *reqRespCode = (int) 0 ;
            }
            if( aspects >= 4 && srcpAddr == addr4 && srcpPort == gate4 ) {
              StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d GA %d %d %d\n", 
                    time.tv_sec, time.tv_usec / 1000L, srcpBus, srcpAddr, srcpPort, 0 );
              TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "SG answer aspect 4: %s", str);
              __writeRsp(o, str);
              *reqRespCode = (int) 0 ;
            }
          }
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Answer: %s", str);
        }

        if( ! sw && ! sg ){
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET REQ %s -> switch/signal on %s for address %d not found", req, srcpBusIid, srcpAddr );
          /* 416 ERROR no data */
          *reqRespCode = (int) 416;
        }
      }else {
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412;
      }
    } /* GA */

    else if( StrOp.findi( req, "FB" ) ) {
      /* GET <bus> FB <addr> */
      /* "100 INFO <bus> FB <addr> <value>" */
      char str[1025] = {'\0'};
      int idx = 0;
      int srcpBus = 0;
      int srcpAddr = 0;
      int addr = 0;
      char srcpBusIid[1024];

      iOStrTok tok = StrTokOp.inst(req, ' ');
      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 1: srcpBus = atoi(s); break;
        case 3: srcpAddr = atoi(s); break;
        case 4: break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      if( idx < 4 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 4 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }

      getSrcpIid( data, srcpBus, srcpBusIid);

      if( srcpAddr > 0 ) {
        iOFBack fb;
        /*
         * Q: "GET <bus> FB <addr>"
         * A: "100 INFO <bus> FB <addr> <value>"
         * or
         * A: "412 wrong value"
         */

        fb = getFeedbackByIidAndAddr( srcpBusIid, srcpAddr);
        if( fb != NULL) {
          iONode fbProps = FBackOp.base.properties(fb);
          int value = wFeedback.isstate(fbProps);

          StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d FB %d %d\n",
                time.tv_sec, time.tv_usec / 1000L, srcpBus, srcpAddr, value );
          __writeRsp(o, str);
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "Answer: %s", str);
          *reqRespCode = (int) 0 ;
        }
        else {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET REQ for undefined Feedback: bus %d, addr %d", srcpBus, srcpAddr );
          /* 412 wrong value */
          *reqRespCode = (int) 412 ;
        }
      }
    } /* FB */

    else if( StrOp.equalsi( req, "GET 0 SERVER" ) ) {
      char str[1025] = {'\0'};
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET 0 SERVER" );
      StrOp.fmtb(str, "%lu.%.3lu 100 INFO 0 SERVER RUNNING\n",
          time.tv_sec, time.tv_usec / 1000L );
      /* send back to requesting command channel */
      __writeRsp(o, str);
      *reqRespCode = (int) 0;
    } /* SERVER */

    else if( StrOp.startsWithi( req, "GET 0 SESSION" ) ) {
      /* GET 0 SESSION <SESSIONID> */
      /* 100 INFO 0 SESSION <SESSIONID> [further optional parameters] */
      char str[1025] = {'\0'};
      if( StrOp.len( req ) > 14 ) {
        int reqSessId = atoi( &req[14] );

        if( reqSessId == o->id ) {
          iOSocket clientSocket = o->clntSocket ;
          StrOp.fmtb(str, "%lu.%.3lu 100 INFO 0 SESSION %d peer[%s]\n",
              time.tv_sec, time.tv_usec / 1000L, o->id, SocketOp.getPeername( clientSocket ) );
          /* send back to requesting command channel */
          __writeRsp(o, str);
          *reqRespCode = (int) 0;
          return cmd;
        }else {
          /* 416 ERROR no data */
          *reqRespCode = (int) 416 ;
          return cmd;
        }
      }
      /* 419 ERROR list too short */
      *reqRespCode = (int) 419 ;
      return cmd;
    } /* SESSION */

    else if( StrOp.equalsi( req, "GET 0 TIME" ) ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "GET 0 TIME" );
      char str[1025] = {'\0'};

      if( ! __isClockRunning() ) {
        /* 425 ERROR not supported */
        *reqRespCode = (int) 425 ;
        return cmd;
      }

      /* 100 INFO 0 TIME <JulDay> <Hour> <Minute> <Second> */
      time_t rr_time = ControlOp.getTime( AppOp.getControl() );
      StrOp.fmtb(str, "%lu.%.3lu 100 INFO 0 TIME %s\n",
          time.tv_sec, time.tv_usec / 1000L, convModelTimeToSRCP(rr_time) );
      __writeRsp(o, str);

      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "Answer: %s", str);
      *reqRespCode = (int) 0 ;
    } /* TIME */

    /* GET <bus> DESCRIPTION */
    else if( StrOp.findi( req, "DESCRIPTION" ) ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "GET <bus> DESCRIPTION" );
      char str[1025] = {'\0'};
      int idx = 0;
      int  srcpBus  = -1;
      char srcpBusIid[1024];
      iOStrTok tok = StrTokOp.inst(req, ' ');

      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 1: srcpBus = atoi(s); break;
        case 2: if( ! StrOp.equalsi( s, "DESCRIPTION" ) ) {
                  /* 422 ERROR unsupported device group */
                  *reqRespCode = (int) 422 ;
                  return cmd;
                }
                break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      if( idx < 3 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 3 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }
      if( ! isValidSrcpBus( data, srcpBus ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 412 ERROR wrong value", req);
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
        return cmd;
      }

      creaRspBusDescr( str, &time, srcpBus );
      /* send back to requesting command channel */
      __writeRsp(o, str);
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "Answer: %s", str);
          *reqRespCode = (int) 0 ;
    }

    else if( StrOp.findi( req, "SM" ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET <bus> SM not supported" );
      /* 422 ERROR unsupported device group */
      *reqRespCode = (int) 422 ;
    }

    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Unhandled GET REQ %s", req );
      /* 410 ERROR unknown command */
      *reqRespCode = (int) 410 ;
    }
  } /* GET */

  else if( StrOp.startsWithi( req, "TERM " ) ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "TERM req: %s", req );

    if( StrOp.startsWithi( req, "TERM 0 SESSION" ) ) {
      /* TERM 0 SESSION [<id>] */
      int idx = 0;
      char str[1025] = {'\0'};
      int srcpBus = 0;
      int sessId = 0;
      char busType[1025] = {'\0'};
      iOStrTok tok = StrTokOp.inst(req, ' ');

      while( StrTokOp.hasMoreTokens(tok)) {
        const char* s = StrTokOp.nextToken(tok);
        switch(idx) {
        case 1: srcpBus = atoi(s) ; break;
        case 3: sessId = atoi(s) ; break;
        }
        idx++;
      };
      StrTokOp.base.del(tok);

      if( idx < 3 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 4 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }

      /* check for optional parameter <SESSIONID> */
      if( sessId == 0 ){
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "TERM request from session %d without (optional) session ID", o->id );
        sessId = o->id;
      }

      if( sessId != o->id ){
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "TERM request for Id %d but session Id is %d", sessId, o->id );
        /* 415 ERROR forbidden */
        *reqRespCode = (int) 415 ;
      } else if( srcpBus == 0 ){
        /* terminate session */
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Terminating session %d", sessId );
        /* last action is to send back confirmation */
        /* 102 INFO 0 SESSION <SESSIONID> */

        StrOp.fmtb(str, "%lu.%.3lu 102 INFO %d SESSION %d\n", 
              time.tv_sec, time.tv_usec / 1000L, srcpBus, sessId );
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%s [%p]", str, o->clntSocket);
        __writeRsp(o, str);
        /* give some time to send response */
        ThreadOp.sleep( 100 );
        /* close IP socket */
        SocketOp.disConnect( o->clntSocket );
        ThreadOp.sleep( 100 );
        /* terminate loop in service thread */
        o->quit = True;
        ThreadOp.sleep( 100 );
        /* inform all info sessions about closing a session */
        /* -> this will be automatically invoked for all sessions when closing the socket ! */
        /* sendSessionstate2InfoChannels( 0, sessId, 102, o->infomode ); */
        *reqRespCode = (int) 0 ;
      }
      else {
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
      }
    } /* TERM 0 SESSION [<id>] */

    else if( StrOp.equalsi( req, "TERM 0 TIME" ) ) {
      /* TERM 0 TIME */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "__srcp2rr: wClock.freeze" );
      iONode tick = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
      wClock.setcmd( tick, wClock.freeze );
      data->callback( data->callbackObj, tick );

      /* answer for successful SET is "200 OK", generated automatically in calling function */
    } /* TERM 0 TIME */

    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UNHANDLED TERM req %s from session %d [%s]", req, o->id, o->infomode?"INFO":"COMMAND" );
      /* 410 ERROR unknown command */
      *reqRespCode = (int) 410 ;
    }
  } /* TERM */

  else if( StrOp.startsWithi( req, "WAIT " ) ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "WAIT req: %s", req );

    int idx = 0;
    char str[1025] = {'\0'};
    int srcpBus = 0;
    char srcpBusIid[1025] = {'\0'};
    char busType[1025] = {'\0'};
    char sPar3[1025] = {'\0'};
    char sPar4[1025] = {'\0'};
    char sPar5[1025] = {'\0'};
    char sPar6[1025] = {'\0'};

    iOStrTok tok = StrTokOp.inst(req, ' ');

    while( StrTokOp.hasMoreTokens(tok)) {
      const char* s = StrTokOp.nextToken(tok);
      switch(idx) {
      case 1: srcpBus = atoi(s) ; break;
      case 2: StrOp.copy( busType, s ) ; break;
      case 3: StrOp.copy( sPar3, s); break;
      case 4: StrOp.copy( sPar4, s); break;
      case 5: StrOp.copy( sPar5, s); break;
      case 6: StrOp.copy( sPar6, s); break;
      }
      idx++;
    };
    StrTokOp.base.del(tok);

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "WAIT bus[%d] type[%s] par3[%s] par4[%s] par5[%s] par6[%s]",
          srcpBus, busType, sPar3, sPar4, sPar5, sPar6 );

    getSrcpIid( data, srcpBus, srcpBusIid);

    if( StrOp.equalsi( busType, "FB" )) {
      /* WAIT <bus> FB  */
      /*
       * Q: "WAIT <bus> FB <addr> <value> <timeout>"
       * <addr> out of range:
       *   A: "412 wrong value"
       * <value> reached within <timeout>:
       *   A: "100 INFO <bus> FB <addr> <value>"
       * <value> not reached within <timeout>
       *   A: "417 ERROR timeout"
       */
      if( idx < 6 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 6 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }
      if( srcpBus == 0 ) {
        /* server itself has no feedback */
        /* 422 ERROR unsupported device group */
        *reqRespCode = (int) 422;
        return cmd;
      }
      if( ! isValidSrcpBus( data, srcpBus ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 412 ERROR wrong value", req);
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
        return cmd;
      }

      int srcpAddr = atoi( sPar3 );
      int srcpValue = atoi( sPar4 );
      int srcpTimeout = atoi( sPar5 );

      iOFBack fb = getFeedbackByIidAndAddr( srcpBusIid, srcpAddr);
      if( fb != NULL) {
        iONode fbProps = FBackOp.base.properties(fb);
        int value = wFeedback.isstate(fbProps);
        int timeout_reached = 0;
        unsigned long endTime;
        struct timeval currTime;

        gettimeofday(&currTime, NULL);
        endTime = currTime.tv_sec + (unsigned long) srcpTimeout;

        while( currTime.tv_sec <= endTime ) {
          if( srcpValue == wFeedback.isstate(fbProps) ) {
            StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d FB %d %d\n",
                  currTime.tv_sec, currTime.tv_usec / 1000L, srcpBus, srcpAddr, srcpValue );
            __writeRsp(o, str);
            TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "Answer: %s", str);
            *reqRespCode = (int) 0 ;
            return cmd;
          }
          else {
            /* wait 100 ms and let others do their jobs */
            ThreadOp.sleep( 100 );
            gettimeofday(&currTime, NULL);
          }
        }
        /* 417 timeout */
        *reqRespCode = (int) 417 ;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "GET REQ for undefined Feedback: bus %d, addr %d", srcpBus, srcpAddr );
        /* 412 wrong value */
        *reqRespCode = (int) 412 ;
      }

    } /* WAIT <bus> FB */
    else if( StrOp.equalsi( busType, "TIME" )) {
      /* WAIT 0 TIME <JulDay> <Hour> <Minute> <Second> */
      /*
       It waits until the model time reaches or outruns the given point and reports an INFO string with the then active model time.

       If the timer is not running the error message " 416 ERROR no data " is generated.
       If the current model time is later than the given time already the condition is fulfilled without further waiting time.
       Obviously wrong time data are reported to the calling client by " 412 ERROR wrong value " or ignored.
       The WAIT MUST always evaluate the currently valid model time that can be changed by SET if applicable.
      */

      if( idx < 7 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 419 ERROR list too short", req);
        *reqRespCode = (int) 419 ;
        return cmd;
      }
      if( idx > 7 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s -> 418 ERROR list too long", req);
        *reqRespCode = (int) 418 ;
        return cmd;
      }
      if( srcpBus != 0 ) {
        /* TIME device only on server itself */
        /* 422 ERROR unsupported device group */
        *reqRespCode = (int) 422;
        return cmd;
      }
      if( ! __isClockRunning() ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s : RocrailClock not running -> 416 ERROR no data", req);
        /* 416 ERROR no data */
        *reqRespCode = (int) 416 ;
        return cmd;
      }

      long julDay = atol( sPar3 );
      int  hour   = atoi( sPar4 );
      int  min    = atoi( sPar5 );
      int  sec    = atoi( sPar6 );

      if( (julDay < JULIAN_DAY_1970_01_01 ) || (hour < 0) || (hour >=24) || (min < 0) || (min >= 60) || (sec < 0 ) || ( sec >= 60 ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "invalid value in date detected: %s", 
            req );
        /* 412 ERROR wrong value */
        *reqRespCode = (int) 412 ;
        return cmd;
      }

      /* 
       * transform WAIT-time into seconds (model time format)
       * compare transfomed time with model time
       */
      time_t wait_time  = convSRCP2ModelTime( julDay, hour, min, sec );
      time_t model_time = ControlOp.getTime( AppOp.getControl() );

      char str_wait_time[128] ;
      StrOp.copy( str_wait_time, convModelTimeToSRCP(wait_time) );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "wait_time[%d] model_time[%d] wait[%s] model[%s]",
          wait_time, model_time, str_wait_time, convModelTimeToSRCP(model_time) );
      /* wait until model_time reaches or outruns wait_time */
      while( model_time < wait_time && ! o->quit ) {
        if( divider > 1 ) {
          /* use divider for sleep => sleep one model second */
          ThreadOp.sleep( 1000 / divider );
        }else {
          /* sleep a real second */
          ThreadOp.sleep( 1000 );
        }

        if( ! __isClockRunning() ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s : RocrailClock stopped during WAIT -> 417 ERROR timeout", req);
          /* 417 ERROR timeout */
          *reqRespCode = (int) 417 ;
          return cmd;
        }

        model_time = ControlOp.getTime( AppOp.getControl() );

        /* check if socket is still alive */
        SocketOp.peek( o->clntSocket, str, 1 );
        if( SocketOp.isBroken( o->clntSocket ) ) {
          o->quit = True;
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s : socket broke while waiting (isBroken[%d] errno[%d])",
              req, SocketOp.isBroken( o->clntSocket ), SocketOp.getRc( o->clntSocket ) );
          *reqRespCode = (int) 0 ;
          return cmd;
        }
        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "wait_time[%d] model_time[%d] wait[%s] model[%s] id[%d], clntSocket[%p] divider[%d]",
            wait_time, model_time, str_wait_time, convModelTimeToSRCP(model_time), o->id, o->clntSocket, divider );
      }
      if( model_time >= wait_time ) {
        StrOp.fmtb(str, "%lu.%.3lu 100 INFO 0 TIME %s\n",
            time.tv_sec, time.tv_usec / 1000L, convModelTimeToSRCP(model_time) );
        __writeRsp(o, str);

        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "Answer: %s", str);
      }
      *reqRespCode = (int) 0 ;
    } /* WAIT 0 TIME */
    else {
      /* WAIT for unsupported device group */
      /* 422 ERROR unsupported device group */
      *reqRespCode = (int) 422;
      return cmd;
    }
    
  } /* WAIT */

  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UNHANDLED req/unkown command %s from session %d [%s]", req, o->id, o->infomode?"INFO":"COMMAND" );
    /* 410 ERROR unknown command */
    *reqRespCode = (int) 410 ;
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

static const char* srcpFmtMsg(int errorcode, char *msg, struct timeval time, int id) {
    switch (errorcode) {
        case 100:
            sprintf(msg, "%lu.%.3lu %d INFO\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 101:
            sprintf(msg, "%lu.%.3lu %d INFO\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 102:
            sprintf(msg, "%lu.%.3lu %d INFO\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 110:
            sprintf(msg, "%lu.%.3lu %d INFO\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 200:
          if( id > 0 )
            sprintf(msg, "%lu.%.3lu %d OK GO %d\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode, id);
          else
            sprintf(msg, "%lu.%.3lu %d OK\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 202:
            sprintf(msg, "%lu.%.3lu %d OK CONNECTIONMODE\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 201:
            sprintf(msg, "%lu.%.3lu %d OK PROTOCOL SRCP\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 400:
            sprintf(msg, "%lu.%.3lu %d ERROR unsupported protocol\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 401:
            sprintf(msg,
                    "%lu.%.3lu %d ERROR unsupported connection mode\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 402:
            sprintf(msg, "%lu.%.3lu %d ERROR insufficient data\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 410:
            sprintf(msg, "%lu.%.3lu %d ERROR unknown command\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 411:
            sprintf(msg, "%lu.%.3lu %d ERROR unknown value\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 412:
            sprintf(msg, "%lu.%.3lu %d ERROR wrong value\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 413:
            sprintf(msg, "%lu.%.3lu %d ERROR temporarily prohibited\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 414:
            sprintf(msg, "%lu.%.3lu %d ERROR device locked\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 415:
            sprintf(msg, "%lu.%.3lu %d ERROR forbidden\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 416:
            sprintf(msg, "%lu.%.3lu %d ERROR no data\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 417:
            sprintf(msg, "%lu.%.3lu %d ERROR timeout\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 418:
            sprintf(msg, "%lu.%.3lu %d ERROR list too long\n", time.tv_sec,
                    time.tv_usec / 1000L, errorcode);
            break;
        case 419:
            sprintf(msg, "%lu.%.3lu %d ERROR list too short\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 420:
            sprintf(msg,
                    "%lu.%.3lu %d ERROR unsupported device protocol\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 421:
            sprintf(msg, "%lu.%.3lu %d ERROR unsupported device\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 422:
            sprintf(msg, "%lu.%.3lu %d ERROR unsupported device group\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 423:
            sprintf(msg, "%lu.%.3lu %d ERROR unsupported operation\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 424:
            sprintf(msg, "%lu.%.3lu %d ERROR device reinitialized\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 425:
            sprintf(msg, "%lu.%.3lu %d ERROR not supported\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 499:
            sprintf(msg, "%lu.%.3lu %d ERROR unspecified error\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        case 500:
            sprintf(msg, "%lu.%.3lu %d ERROR out of resources\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
        default:
            sprintf(msg, "%lu.%.3lu 600 ERROR internal error %d, "
                    "please report to srcpd-devel@srcpd.sorceforge.net\n",
                    time.tv_sec, time.tv_usec / 1000L, errorcode);
            break;
    }
    return msg;
}


/* create SRCP answer with description of all bus-ids */
static void sendBusList2InfoChannel( __iOSrcpService o, iOSrcpCon srcpcon ) {
  iOSrcpConData data = Data(srcpcon);
  struct timeval time;
  gettimeofday(&time, NULL);

  char str[1025] = {'\0'};
  char *infoStr = NULL;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sendBusList2InfoChannel: sendBusList2InfoChannel" );

  /* generate strings for all srcp busses */
  iONode  ini           = AppOp.getIni();
  iONode  digint        = wRocRail.getdigint( ini );
  iONode  plan          = ModelOp.getModel(AppOp.getModel());
  iONode  modeldigint   = plan?wPlan.getdigint( plan ):NULL;
  Boolean bModelDigints = modeldigint == NULL ? False:True;

  while( digint != NULL ) {
    const char* digintIid = wDigInt.getiid( digint );
    int srcpBus = getSrcpBus( data, digintIid );

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sendBusList2InfoChannel: srcpBus[%d] srcpIid[%s]", srcpBus, digintIid );

    /* announce "normal" capabilities for all command station *
    /* SM and LOCK currently not supported through srcp server */
    StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d DESCRIPTION GA GL FB POWER DESCRIPTION\n",
        time.tv_sec, time.tv_usec / 1000L, srcpBus );
    infoStr = StrOp.cat( infoStr, str );

    if( bModelDigints )
      digint = wPlan.nextdigint( plan, digint );
    else
      digint = wRocRail.nextdigint( ini, digint );
  }
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "creaRspAllPwSts: infoStr[\n%s]", infoStr );
  __writeRsp(o, infoStr);
  ThreadOp.sleep( 10 );
}

static void sendFeedbackList2InfoChannel( __iOSrcpService o, iOSrcpCon srcpcon ) {
  struct timeval time;
  gettimeofday(&time, NULL);
  iOModel model = AppOp.getModel();
  char str[1025] = {'\0'};
  
  iOMap fbackMap = ModelOp.getFeedbackMap( model );
  iOFBack fb = (iOFBack)MapOp.first( fbackMap );

  while( fb != NULL ) {
    iONode fbProps = FBackOp.base.properties(fb);
    int srcpBus = getSrcpBus( Data(srcpcon), wFeedback.getiid(fbProps));

    /*100 INFO <bus> FB <addr> <value>*/
    StrOp.fmtb(str, "%lu.%.3lu %d INFO %d FB %d %d\n",
        time.tv_sec, time.tv_usec / 1000L,
        100, srcpBus, wFeedback.getaddr(fbProps), wFeedback.isstate(fbProps));

    if( StrOp.len( str ) ){
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sendFeedbackList2InfoChannel: snd2info[%d]: %s", o->id, str ) ;
      __writeRsp(o, str);
    }

    fb = (iOFBack)MapOp.next( fbackMap );
  };
}

static void sendSwitchList2InfoChannel( __iOSrcpService o, iOSrcpCon srcpcon ) {
  struct timeval time;
  gettimeofday(&time, NULL);
  iOModel model = AppOp.getModel();
  char str[1025] = {'\0'};
  
  iOMap switchMap = ModelOp.getSwitchMap( model );
  iOSwitch sw = (iOSwitch)MapOp.first( switchMap );

  while( sw != NULL ) {
    /* 101 INFO <bus> GA <addr> <device> <protocol> */
    /* 100 INFO <bus> GA <addr> <port> <value> */
    iONode swProps = SwitchOp.base.properties(sw);
    int srcpBus = getSrcpBus( Data(srcpcon), wSwitch.getiid(swProps));
    int addr  = AddrOp.toPADA( wSwitch.getaddr1(swProps), wSwitch.getport1(swProps) );
    int addr2 = AddrOp.toPADA( wSwitch.getaddr2(swProps), wSwitch.getport2(swProps) );
    int gate1 = wSwitch.getgate1(swProps);
    int gate2 = wSwitch.getgate2(swProps);
    const char* state = wSwitch.getstate(swProps);
    Boolean singlegate = wSwitch.issinglegate(swProps);
    const char *swProt = wSwitch.getprot(swProps);
    char prot[256] = {'\0'} ;

    if( StrOp.startsWithi( swProt, "M" )) {
      StrOp.copy( prot, "M" );
    }else {
      StrOp.copy( prot, "N" );
    }
    
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sendSwitchList2InfoChannel: iidSP %s srcpBus %d swType %s",
        wSwitch.getiid(swProps), srcpBus, wSwitch.gettype(swProps) );

    if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.crossing)
     && (wSwitch.getaddr1(swProps) == 0)
     && (wSwitch.getport1(swProps) == 0) ) {
      /* crossing without address -> cross -> empty string */
      StrOp.fmtb(str, "");
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sendSwitchList2InfoChannel: skipping cross\n" );
    }
    else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.left)
          || StrOp.equals( wSwitch.gettype(swProps), wSwitch.right)
          || StrOp.equals( wSwitch.gettype(swProps), wSwitch.twoway)
          || StrOp.equals( wSwitch.gettype(swProps), wSwitch.crossing) 
          || StrOp.equals( wSwitch.gettype(swProps), wSwitch.ccrossing)
          || StrOp.equals( wSwitch.gettype(swProps), wSwitch.accessory) ) {
      int lastActivePort = StrOp.equals(wSwitch.getstate(swProps),wSwitch.straight)?1:0 ;
      if( singlegate ) {
        int action = 1;
        if( StrOp.equals( state, wSwitch.straight ) )
          action = 0;
        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n%lu.%.3lu %d INFO %d GA %d %d %d\n",
          time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr2, prot,
          time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2, action );
      }else {
        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
            time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr, prot,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, lastActivePort,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 0,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 1);
      }
    }
    else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.threeway) 
          || StrOp.equals( wSwitch.gettype(swProps), wSwitch.dcrossing) ) {
      const char* swState = wSwitch.getstate(swProps);
      int lastActivePort1 = 0;
      int lastActivePort2 = 0;
      if( StrOp.equals(swState,wSwitch.straight) ) {
        lastActivePort1 = 0;
        lastActivePort2 = 0;
      }else if( StrOp.equals(swState,wSwitch.turnout) ) {
        lastActivePort1 = 1;
        lastActivePort2 = 1;
      }else if( StrOp.equals(swState,wSwitch.left) ) {
        lastActivePort1 = 1;
        lastActivePort2 = 0;
      }else if( StrOp.equals(swState,wSwitch.right) ) {
        lastActivePort1 = 0;
        lastActivePort2 = 1;
      }
      if( singlegate ) {
        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n%lu.%.3lu %d INFO %d GA %d %d %d\n%lu.%.3lu %d INFO %d GA %d %s\n%lu.%.3lu %d INFO %d GA %d %d %d\n%",
            time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr, prot,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, gate1, lastActivePort1,
            time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr2, prot,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2, lastActivePort2 );
      }else {
        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %s\n%lu.%.3lu %d INFO %d GA %d %d 1\n%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
            time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr, prot,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, lastActivePort1,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 0,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 1,
            time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr2, prot,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, lastActivePort2,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, 0,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, 1);
      }
    }
    else if( StrOp.equals( wSwitch.gettype(swProps), wSwitch.decoupler )) {
      if( singlegate ){
        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
            time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr, prot,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, wSwitch.getgate1(swProps));
      }else {
        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
            time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr, prot,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 0,
            time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, 1);
      }
    }
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UNHANDLED swDevice typeP %s, SWaddr1 %d, SWaddr2 %d, stateE %s, stateP %s", wSwitch.gettype(swProps) , addr, addr2, wSwitch.getstate(swProps), wSwitch.getstate(swProps));
      str[0] = '\0' ;
    }

    if( StrOp.len( str ) ){
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sendSwitchList2InfoChannel: snd2info[%d]: %s", o->id, str ) ;
      __writeRsp(o, str);
    }

    sw = (iOSwitch)MapOp.next( switchMap );
  };
}

static void sendSignalList2InfoChannel( __iOSrcpService o, iOSrcpCon srcpcon ) {
  struct timeval time;
  gettimeofday(&time, NULL);
  iOModel model = AppOp.getModel();
  char str[1025] = {'\0'};
  
  iOMap signalMap = ModelOp.getSignalMap( model );
  iOSignal sg = (iOSignal)MapOp.first( signalMap );

  while( sg != NULL ) {
    iONode sgProps = SignalOp.base.properties(sg);
    int aspects = wSignal.getaspects( sgProps );
    const char *sgProt = wSignal.getprot(sgProps);
    const char* sgState = wSignal.getstate(sgProps);
    char prot[256] = {'\0'} ;
    int addr  = -1;
    int addr2 = -1;
    int addr3 = -1;
    int addr4 = -1;
    int gate1 = -1;
    int gate2 = -1;
    int gate3 = -1;
    int gate4 = -1;

    str[0] = '\0' ;

    if( StrOp.startsWithi( sgProt, "M" )) {
      StrOp.copy( prot, "M" );
    }else {
      StrOp.copy( prot, "N" );
    }
    
    /* 101 INFO <bus> GA <addr> <device> <protocol> */
    int srcpBus = getSrcpBus( Data(srcpcon), wSignal.getiid(sgProps));

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sendSignalList2InfoChannel: iidSP %s srcpBus %d sgProt %s sgState %s",
        wSignal.getiid(sgProps), srcpBus, sgProt, sgState );

    /* 101 INFO <bus> GA <addr> <device> <protocol> */
    /* 100 INFO <bus> GA <addr> <port> <value> */
    switch( aspects ){
      case 1:
        addr  = AddrOp.toPADA( wSignal.getaddr( sgProps), wSignal.getport1(sgProps) );
        gate1 = wSignal.getgate1(sgProps);

        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n",   time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr, prot );
        __writeRsp(o, str);

        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 0\n", time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, gate1 );
        __writeRsp(o, str);
        break;
      case 2:
        addr  = AddrOp.toPADA( wSignal.getaddr( sgProps), wSignal.getport1(sgProps) );
        gate1 = wSignal.getgate1(sgProps);
        addr2 = AddrOp.toPADA( wSignal.getaddr2(sgProps), wSignal.getport2(sgProps) );
        gate2 = wSignal.getgate2(sgProps);

        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n", time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr, prot );
        __writeRsp(o, str);
        if( addr2 != addr ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n", time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr2, prot );
          __writeRsp(o, str);
        }

        if( StrOp.equals(sgState,wSignal.red) ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n", time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, gate1 );
          __writeRsp(o, str);
        }else if( StrOp.equals(sgState,wSignal.green) ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n", time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2 );
          __writeRsp(o, str);
        }

        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
                          time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr,  gate1,
                          time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2 );
        __writeRsp(o, str);
        break;
      case 3:
        addr  = AddrOp.toPADA( wSignal.getaddr( sgProps), wSignal.getport1(sgProps) );
        gate1 = wSignal.getgate1(sgProps);
        addr2 = AddrOp.toPADA( wSignal.getaddr2(sgProps), wSignal.getport2(sgProps) );
        gate2 = wSignal.getgate2(sgProps);
        addr3 = AddrOp.toPADA( wSignal.getaddr3(sgProps), wSignal.getport3(sgProps) );
        gate3 = wSignal.getgate3(sgProps);

        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n", time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr, prot );
        __writeRsp(o, str);
        if( addr2 != addr ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n", time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr2, prot );
          __writeRsp(o, str);
        }
        if( (addr3 != addr) && (addr3 != addr2) ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n", time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr3, prot );
          __writeRsp(o, str);
        }

        if( StrOp.equals(sgState,wSignal.red) ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n", time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, gate1 );
          __writeRsp(o, str);
        }else if( StrOp.equals(sgState,wSignal.green) ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n", time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2 );
          __writeRsp(o, str);
        }else if( StrOp.equals(sgState,wSignal.yellow) ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n", time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr3, gate3 );
          __writeRsp(o, str);
        }

        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
                          time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr,  gate1,
                          time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2,
                          time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr3, gate3 );
        __writeRsp(o, str);
        break;
      case 4:
        addr  = AddrOp.toPADA( wSignal.getaddr( sgProps), wSignal.getport1(sgProps) );
        gate1 = wSignal.getgate1(sgProps);
        addr2 = AddrOp.toPADA( wSignal.getaddr2(sgProps), wSignal.getport2(sgProps) );
        gate2 = wSignal.getgate2(sgProps);
        addr3 = AddrOp.toPADA( wSignal.getaddr3(sgProps), wSignal.getport3(sgProps) );
        gate3 = wSignal.getgate3(sgProps);
        addr4 = AddrOp.toPADA( wSignal.getaddr4(sgProps), wSignal.getport4(sgProps) );
        gate4 = wSignal.getgate4(sgProps);

        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n", time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr, prot );
        __writeRsp(o, str);
        if( addr2 != addr ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n", time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr2, prot );
          __writeRsp(o, str);
        }
        if( (addr3 != addr) && (addr3 != addr2) ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n", time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr3, prot );
          __writeRsp(o, str);
        }
        if( (addr4 != addr) && (addr4 != addr2) && (addr4 != addr3) ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %s\n", time.tv_sec, time.tv_usec / 1000L, 101, srcpBus, addr4, prot );
          __writeRsp(o, str);
        }

        if( StrOp.equals(sgState,wSignal.red) ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n", time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr, gate1 );
          __writeRsp(o, str);
        }else if( StrOp.equals(sgState,wSignal.green) ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n", time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2 );
          __writeRsp(o, str);
        }else if( StrOp.equals(sgState,wSignal.yellow) ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n", time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr3, gate3 );
          __writeRsp(o, str);
        }else if( StrOp.equals(sgState,wSignal.white) ) {
          StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 1\n", time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr4, gate4 );
          __writeRsp(o, str);
        }

        StrOp.fmtb(str, "%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n%lu.%.3lu %d INFO %d GA %d %d 0\n",
                          time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr,  gate1,
                          time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr2, gate2,
                          time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr3, gate3,
                          time.tv_sec, time.tv_usec / 1000L, 100, srcpBus, addr4, gate4 );
        __writeRsp(o, str);
        break;
      default:
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "UNHANDLED sgDevice aspects %d, SGaddr %d, SGaddr2 %d, stateE %s, stateP %s", aspects, addr, addr2, wSignal.getstate(sgProps), wSignal.getstate(sgProps));
        break;
    }

    sg = (iOSignal)MapOp.next( signalMap );
  };
}

static void sendLocoList2InfoChannel( __iOSrcpService o, iOSrcpCon srcpcon ) {
  struct timeval time;
  gettimeofday(&time, NULL);
  iOModel model = AppOp.getModel();
  
  iOMap locoMap = ModelOp.getLocoMap( model );
  iOLoc loco = (iOLoc)MapOp.first( locoMap );

  while( loco != NULL ) {
    /* 101 INFO <bus> GL <addr> <protocol> <optional further parameters> */
    /* 100 INFO <bus> GL <addr> <drivemode> <V> <V_max> <f1> . . <fn> */
    if( loco != NULL ) {
      iONode loProps = LocOp.base.properties(loco);

      int            loAddr = wLoc.getaddr(loProps);
      const char     *loIid = wLoc.getiid(loProps);
      const char    *loProt = wLoc.getprot(loProps);
      int         loProtver = wLoc.getprotver(loProps);
      int             loBus = wLoc.getbus(loProps);
      Boolean         loDir = wLoc.isdir(loProps);
      int               loV = wLoc.getV(loProps);
      int            loVmax = wLoc.getV_max(loProps);
      const char   *loVmode = wLoc.getV_mode(loProps);
      int           loSpcnt = wLoc.getspcnt(loProps);
      int           decStep = (wLoc.getV( loProps ) * loSpcnt) / wLoc.getV_max( loProps );
      Boolean          loFn = wLoc.isfn( loProps );    
      int           loFnCnt = wLoc.getfncnt(loProps);
      int              loFx = wLoc.getfx(loProps);
      int               dir = LocOp.getDir(loco);
      int                 V = LocOp.getV(loco);
      char        str[1025] = {'\0'};
      char   freeText[1025] = {'\0'};
      char funcString[1023] = {'\0'};
      int i, mask;

      for( i=0 ; i < loFnCnt ; i++ ) {
        mask = 1 << i ;
        funcString[2*i] = ' ';
        funcString[2*i+1] = loFx&mask?'1':'0';
      }
      funcString[2*loFnCnt] = '\0';

      int     srcpBus = getSrcpBus( Data(srcpcon), loIid);
      char   srcpProt = loProt[0] ;
      int srcpProtver = loProtver ;

      switch(srcpProt) {
        case 'P': break;
        case 'M': break;
        case 'N': srcpProtver=1;break;
        case 'L': srcpProt='N';srcpProtver=2;break;
        case 'A': break;
        case 'C': srcpProt='N';break;
        case 'S': break;
        case 'X': srcpProt='S';break;
      }

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "addr %d iid %s prot %s/%c protver %d/%d loBus %d srcpBus %d", loAddr, loIid, loProt, srcpProt, loProtver, srcpProtver, loBus, srcpBus);

      /* 101 INFO <bus> GL <addr> <protocol> <optional further parameters> */
      StrOp.fmtb(str, "%lu.%.3lu 101 INFO %d GL %d %c %d %d %d\n",
              time.tv_sec, time.tv_usec / 1000L, srcpBus, loAddr, srcpProt, srcpProtver, loSpcnt, loFnCnt+1 );
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%s", str);
      __writeRsp(o, str);
      /* 100 INFO <bus> GL <addr> <drivemode> <V> <V_max> <f1> . . <fn> */
      StrOp.fmtb(str, "%lu.%.3lu 100 INFO %d GL %d %d %d %d %d%s\n", 
              time.tv_sec, time.tv_usec / 1000L, srcpBus, loAddr, loDir?1:0, decStep, loSpcnt, loFn?1:0, funcString);
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%s", str);
      __writeRsp(o, str);
    }
    loco = (iOLoc)MapOp.next( locoMap );
  };
}

static void sendPowerstate2InfoChannel( __iOSrcpService o, iOSrcpCon srcpcon ) {
  iOSrcpConData data = Data(srcpcon);
  char *rsp = creaRspAllPwSts( data );
  __writeRsp(o, rsp);
}


static void __evalRequest(iOSrcpCon srcpcon, __iOSrcpService o, const char* req) {
  char rsp[1025] = {'\0'};
  struct timeval time;
  gettimeofday(&time, NULL);

  if( o->handshake ) {
    if( StrOp.startsWithi( req, "SET PROTOCOL SRCP" ) ) {
      if( StrOp.startsWithi( req, "SET PROTOCOL SRCP 0.8" ) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP Session %d PROTOCOL accepted", o->id ) ;
        __writeRsp(o, srcpFmtMsg(201, rsp, time, 0));
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "req %s on socket %p: response %s", req, o->clntSocket, rsp ) ;
      }else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP Session %d PROTOCOL denied: %s", o->id, req ) ;
        __writeRsp(o, srcpFmtMsg(400, rsp, time, 0));
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "req %s on socket %p: response %s", req, o->clntSocket, rsp ) ;
      }
    }
    else if( StrOp.startsWithi( req, "SET CONNECTIONMODE" ) ) {
      if( StrOp.equalsi( req, "SET CONNECTIONMODE SRCP INFO" ) ) {
        o->infomode = True;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP Session %d set to %s", o->id, o->infomode?"INFO":"COMMAND" ) ;
        __writeRsp(o, srcpFmtMsg(202, rsp, time, 0));
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "req %s on socket %p: response %s", req, o->clntSocket, rsp ) ;
      }
      else if( StrOp.equalsi( req, "SET CONNECTIONMODE SRCP COMMAND" ) ) {
        o->infomode = False;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP Session %d set to %s", o->id, o->infomode?"INFO":"COMMAND" ) ;
        __writeRsp(o, srcpFmtMsg(202, rsp, time, 0));
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "req %s on socket %p: response %s", req, o->clntSocket, rsp ) ;
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP Session %d set to %s", o->id, o->infomode?"INFO":"COMMAND" ) ;
        __writeRsp(o, srcpFmtMsg(401, rsp, time, 0));
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "req %s on socket %p: response %s", req, o->clntSocket, rsp ) ;
      }
    }
    else if( StrOp.equalsi( req, "GO" ) ) {
      o->handshake = False;
      __writeRsp(o, srcpFmtMsg(200, rsp, time, o->id));
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP RESPONSE: %s", rsp ) ;

      /* if we started an info channel send all information about server to that channel */
      if( o->infomode ) {
        char rspInfo[1025] = {'\0'};
        int defaultSrcpBus = getSrcpBus( Data(srcpcon), getDefaultIid() );
        /*
        100 INFO 0 DESCRIPTION SESSION SERVER TIME GM
        101 INFO 0 TIME 0 0
        100 INFO 1 DESCRIPTION GA GL SM POWER LOCK DESCRIPTION
        */
        /* announce basic system capabilities (server) */
        /* the system itself is always bus 0 */
        StrOp.fmtb(rspInfo, "%lu.%.3lu 100 INFO 0 DESCRIPTION SESSION SERVER TIME\n", time.tv_sec, time.tv_usec / 1000L);
        __writeRsp(o, rspInfo);

        /* time data */
        iONode clockini = wRocRail.getclock( AppOp.getIni() );
        if( clockini != NULL ) {
          if( wClock.getdivider(clockini) > 0 ) {
            divider = wClock.getdivider( clockini );
          }
        }

        if( divider > 0 && __isClockRunning() ) {
          /* send current time divider */
          StrOp.fmtb(rspInfo, "%lu.%.3lu 101 INFO 0 TIME %d %d\n", time.tv_sec, time.tv_usec / 1000L, divider, 1 );
          __writeRsp(o, rspInfo);

          /* send current rocrail time */
          time_t rr_time = ModelOp.getTime( AppOp.getModel() );
          StrOp.fmtb(rspInfo, "%lu.%.3lu 100 INFO 0 TIME %s\n",
              time.tv_sec, time.tv_usec / 1000L, convModelTimeToSRCP(rr_time) );
          __writeRsp(o, rspInfo);
        }else {
          StrOp.fmtb(rspInfo, "%lu.%.3lu 101 INFO 0 TIME 0 0\n", time.tv_sec, time.tv_usec / 1000L );
          __writeRsp(o, rspInfo);
        }

        sendBusList2InfoChannel( o, srcpcon); /* list of command stations */
        sendFeedbackList2InfoChannel( o, srcpcon);
        sendSwitchList2InfoChannel( o, srcpcon);
        sendSignalList2InfoChannel( o, srcpcon);
        sendLocoList2InfoChannel( o, srcpcon);
        sendPowerstate2InfoChannel( o, srcpcon);
        /* TODO: if Rocrail lock is similar to SRCP lock...
        sendLock2InfoChannel( o, time);
        */
      }
      /* inform all info sessions about a new session */
      sendSessionstate2InfoChannels( 0, o->id, 101, o->infomode );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unexpected request in handshake phase. SRCP REQUEST: %s", req ) ;
      /* 410 ERROR unknown command */
      __writeRsp(o, srcpFmtMsg(410, rsp, time, 0));
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCP RESPONSE: %s", rsp ) ;
    }
  } /* handshake in progress */
  else { /* handshake ok */
    if( o->infomode ) {
      /* ignore all commands in infomode */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "ignoring command (INFO mode): %s", req ) ;
      return;
    }
    if( StrOp.startsWithi( req, "SET PROTOCOL" )
      || StrOp.startsWithi( req, "SET CONNECTIONMODE" )
      || StrOp.startsWithi( req, "GO" )
      ) { 
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCP REQUEST: %s", req ) ;
      /* 422 ERROR unsupported device group */
      __writeRsp(o, srcpFmtMsg(422, rsp, time, 0));
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCP RESPONSE: %s", rsp ) ;
    }
    else if( StrOp.startsWithi( req, "TERM 0 SERVER" ) 
          || StrOp.startsWithi( req, "RESET 0 SERVER" ) ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP REQUEST: %s", req ) ;
      /* 415 ERROR forbidden */
      __writeRsp(o, srcpFmtMsg(415, rsp, time, 0));
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP RESPONSE: %s", rsp ) ;
    }
    else {
      int reqRespCode = 200 ;
      iONode cmd = __srcp2rr(srcpcon, o, req, &reqRespCode);
      if( cmd != NULL ){
        const char* nodeName = NodeOp.getName(cmd);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__evalRequest __srcp2rr() returned with cmd for RRtype[%s] srcp session[%d] socket[%p]",
            nodeName, o->id, o->clntSocket );
        Data(srcpcon)->callback( Data(srcpcon)->callbackObj, cmd );
      }
      else{
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__srcp2rr( REQ ) returned with CMD == NULL, reqRespCode == %d", reqRespCode );
      }
      if( reqRespCode > 0 ){
        __writeRsp(o, srcpFmtMsg(reqRespCode, rsp, time, 0));
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCP RESPONSE: %s", rsp ) ;
      }
    }
  }
}

static void __SrcpService( void* threadinst ) {
  iOThread         th = (iOThread)threadinst;
  __iOSrcpService   o = (__iOSrcpService)ThreadOp.getParm(th);
  iOSrcpCon   srcpcon = o->SrcpCon;
  iOSrcpConData  data = Data(srcpcon);
  char*         sname = NULL;
  Boolean          ok = False;
  iOThread infoWriter = NULL;
  char str[1025] = {'\0'};
  int sessId;

  ThreadOp.setDescription( th, "SRCP Client command reader" );

  StrOp.fmtb(str, SRCPVERSION, (int) bzr );
  if( o->clntSocket != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP service started for: %s, with session ID %d",
        SocketOp.getPeername(o->clntSocket), o->id );
    SocketOp.write( o->clntSocket, str, StrOp.len(str) );
    SocketOp.write( o->clntSocket, "\n", 1 );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP service started for: Serial, with session ID %d", o->id );
    SerialOp.write( o->clntSerial, str, StrOp.len(str) );
    SerialOp.write( o->clntSerial, "\n", 1 );
  }

  sname = StrOp.fmt( "srcp%08X", o->clntSocket );

  /* Lock the semaphore: */
  if( MutexOp.trywait( Data(srcpcon)->muxMap, 100 ) ) {
    MapOp.put( Data(srcpcon)->infoWriters, sname, (obj)threadinst );
    /* Unlock the semaphore: */
    MutexOp.post( Data(srcpcon)->muxMap );
  }

  do {
    char str[1025] = {'\0'};
    int strLen;

    obj post = ThreadOp.getPost( th );
    if( post != NULL ) {
      iONode node = (iONode)post;
      if( node != NULL ) {
        if( o->infomode ) {
          __rr2srcp(srcpcon, o, node, str);
          if( o->clntSocket != NULL )
            SocketOp.write( o->clntSocket, str, StrOp.len(str) );
          else
            SerialOp.write( o->clntSerial, str, StrOp.len(str) );

          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__SrcpService __rr2srcp() returned with %s %p [%p] ID: %d", str, o, o->clntSocket, o->id );
        }
        NodeOp.base.del(node);
      }
    }

    if( o->clntSocket != NULL ) {
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
    }
    else {
      if( !SerialOp.available(data->serial) ) {
        ThreadOp.sleep( 10 );
        continue;
      }
      else {
        SerialOp.readln(data->serial, str);
      }
    }

    strLen = StrOp.len(str);

    /* TODO: scan string, replace and/or delete redundant white chars */
    /* Currently we just fix the known */
    /* In case string starts with <CR> <\r> 0x0D (may be from JMRI), remove first char */
    if( (byte) str[0] == 0x0D ) {
      int i;

      /* copy strlen bytes (includes '\0') */
      for( i = 0; i < strLen; i++ ) {
        str[i] = str[i+1];
      }
      /* adjust strLen 'manually' for next usage */
      strLen--;
      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "__SrcpService removed leading <CR> from string" ) ;
    }

    /* Strip trailing "\n", makes debug output more readable */
    if( ( strLen != 0 ) && ( str[strLen-1] == '\n' ) ) {
      str[strLen-1] = 0;
      strLen--;
    }
    /* Strip trailing "\r" in case we had \r or \r\n at end of line (windows telnet) */
    if( ( strLen != 0 ) && ( str[strLen-1] == '\r' ) ) {
      str[strLen-1] = 0;
      strLen--;
    }

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "__evalRequest %s for ID: %d", str, o->id );
    __evalRequest( srcpcon, o, str);
    ThreadOp.sleep(10);

  } while( !o->quit );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP service ended for session [%d]", o->id );
  sessId = o->id;

  /* Lock the semaphore: */
  if( MutexOp.trywait( Data(srcpcon)->muxMap, 100 ) ) {
    MapOp.remove( Data(srcpcon)->infoWriters, sname );
    /* Unlock the semaphore: */
    MutexOp.post( Data(srcpcon)->muxMap );
  }

  if( o->clntSocket != NULL ) {
    SocketOp.base.del(o->clntSocket);
  }
  ThreadOp.base.del( th );
  freeMem(o);

  sendSessionstate2InfoChannels( 0, sessId, 102, False );
}


static void __manager( void* threadinst ) {
  iOThread       th = (iOThread)threadinst;
  iOSrcpCon srcpcon = (iOSrcpCon)ThreadOp.getParm(th);
  iOSrcpConData data = Data(srcpcon);

  ThreadOp.setDescription( th, "SRCP Client Manager" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Manager started." );

  if( data->serial != NULL && data->serialOK ) {
    iOThread SrcpService = NULL;
    char*      servername = NULL;
    __iOSrcpService cargo = allocMem( sizeof( struct __OSrcpService ) );
    cargo->SrcpCon    = srcpcon;
    cargo->clntSocket = NULL;
    cargo->clntSerial = data->serial;
    cargo->quit       = False;
    cargo->id         = idCnt;
    cargo->infomode   = False;
    cargo->handshake  = True; /* new connection -> initial handshake in progress */

    servername  = StrOp.fmt( "cmdrSRCP%08X", data->serial );
    SrcpService = ThreadOp.inst( servername, __SrcpService, cargo );
    ThreadOp.start( SrcpService );
    StrOp.free( servername );
    idCnt++;
  }

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
      cargo->handshake  = True;	/* new connection -> initial handshake in progress */
      idCnt++;

      /* we need non blocking ports but at least one OS uses blocking as default */
      SocketOp.setBlocking( cargo->clntSocket, False );

      servername  = StrOp.fmt( "cmdrSRCP%08X", client );
      SrcpService = ThreadOp.inst( servername, __SrcpService, cargo );
      divider     = wClock.getdivider(wRocRail.getclock(AppOp.getIni()));
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

  if( wSrcpCon.getdevice(ini) != NULL && StrOp.len(wSrcpCon.getdevice(ini)) > 0 ) {
    data->serial = SerialOp.inst( wDigInt.getdevice( data->ini ) );

    SerialOp.setFlow( data->serial, none );

    SerialOp.setLine( data->serial, 115200, 8, 1, none, False );
    SerialOp.setTimeout( data->serial, 500, 500 );
    data->serialOK = SerialOp.open( data->serial );
  }

  instCnt++;

  if( wSrcpCon.getsrcpbus(ini) == NULL ) {
    iONode bus = NodeOp.inst( wSrcpBus.name(), ini, ELEMENT_NODE );
    wSrcpBus.setbus(bus, 1);
    NodeOp.addChild(ini, bus);
  }

  if( wRocRail.ispoweroffonexit(AppOp.getIni()) ) {
    /* initialize with " AUTO POWER OFF" (leading blank!) */
    __setSrcpPwFreetext( " AUTO POWER OFF by Rocrail" );
  }else {
    __setSrcpPwFreetext( " by Rocrail" ); /* Default */
  }

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
