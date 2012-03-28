/** ------------------------------------------------------------
  * Module: RocRail
  * XML: $Source: /cvsroot/rojav/rocrail/rocrail.xml,v $
  * Object: MVTrack
  * Date: Tue Apr  8 18:25:30 2008
  * ------------------------------------------------------------
  * $Source$
  * $Author$
  * $Date$
  * $Revision$
  * $Name$
  */

#include <time.h>

#include "rocrail/impl/mvtrack_impl.h"

#include "rocrail/public/app.h"
#include "rocrail/public/fback.h"

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/MVTrack.h"
#include "rocrail/wrapper/public/Item.h"
#include "rocrail/wrapper/public/Feedback.h"


static void _fbEvent( obj inst ,Boolean puls ,const char* id ,const char* ident, int val );

static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  iOMVTrackData data = Data(inst);
  iONode node = (iONode)evt;
  if( node != NULL && StrOp.equals( wFeedback.name(), NodeOp.getName(node) ) ) {
    _fbEvent( inst ,wFeedback.isstate(node), wFeedback.getid(node), wFeedback.getidentifier(node), wFeedback.getval(node) );
  }
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOMVTrackData data = Data(inst);
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
  iOMVTrackData data = Data(inst);
  return data->props;
}

/** ----- OMVTrack ----- */


static void _fbEvent( obj inst ,Boolean puls ,const char* id ,const char* ident, int val ) {
  iOMVTrackData data = Data(inst);
  int event = 0; /* 1=enter, 2=in */

  time_t     t = time(NULL);
  int        ms = SystemOp.getMillis();

  TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "fbid=%s state=%s ident=%s",
                 id, puls?"true":"false", ident );
  
  
  if( !puls ) {
    /* only interested in up flanks */
    return;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fbid=[%s] ident=[%s]", id, ident );
  
  /* check the event */
  if( StrOp.equals( id, wMVTrack.gets1( data->props ) ) ) {
    if( data->triggerS1 && data->triggerS2 ) {
      /* reset */
      MVTrackOp.reset((iOMVTrack)inst);
      data->triggerS1 = True;
      event = 1;
    }
    else if( !data->triggerS1 && !data->triggerS2 ) {
      /* enter event */  
      data->triggerS1 = True;
      event = 1;
    }
  }
  else if( StrOp.equals( id, wMVTrack.gets2( data->props ) ) ) {
    if( data->triggerS1 && data->triggerS2 ) {
      /* ignore */  
    }
    else if( data->triggerS1 && !data->triggerS2 ) {
      /* in event */  
      data->triggerS2 = True;
      event = 2;
    }
  }
  
  if( event == 1 ) {
    /* enter */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "**enter**" );
    data->mvMillis = ms;
    data->mvTime   = t;
  }
  else if( event == 2 ) {
    /* in */
    int seconds = t - data->mvTime; 
    int millis = seconds * 1000 + (ms - data->mvMillis); 
    
    if( millis > 0 ) {
      float distanceMM = wMVTrack.getdistance(data->props) * wMVTrack.getscale(data->props);
      float factor = 3600000 / millis;
      float kmh =  (factor * distanceMM) / (1000*1000);

      if( wMVTrack.ismph( data->props ) ) {
      /* kmh * 0.621371192 = mph */
        TraceOp.trc( name, TRCLEVEL_CALC, __LINE__, 9999, 
          "average velocity on MVTrack was %.1f MPH", kmh * 0.621371192 );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_CALC, __LINE__, 9999, 
          "average velocity on MVTrack was %.1f KM/H", kmh );
      }
    }
  }
  
  return;
}


static void __initSensors( iOMVTrack inst ) {
  iOMVTrackData data = Data(inst);
  iOModel model = AppOp.getModel();
  iOFBack s1 = ModelOp.getFBack( model, wMVTrack.gets1(data->props) );
  iOFBack s2 = ModelOp.getFBack( model, wMVTrack.gets2(data->props) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Init sensors for the MVTrack..." );

  if( s1 != NULL && s2 != NULL ) {
    FBackOp.addListener( s1, (obj)inst );
    FBackOp.addListener( s2, (obj)inst );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Sensors for the MVTrack could not be initialized." );
  }
}

static void _modify( iOMVTrack inst, iONode props ) {
  iOMVTrackData data = Data(inst);
  iOModel model = AppOp.getModel();
  iOFBack s1 = ModelOp.getFBack( model, wMVTrack.gets1(data->props) );
  iOFBack s2 = ModelOp.getFBack( model, wMVTrack.gets2(data->props) );
  if( s1 != NULL && s2 != NULL ) {
    FBackOp.removeListener( s1, (obj)inst );
    FBackOp.removeListener( s2, (obj)inst );
  }
  wMVTrack.setscale( data->props, wMVTrack.getscale(props) );
  wMVTrack.setdistance( data->props, wMVTrack.getdistance(props) );
  wMVTrack.sets1( data->props, wMVTrack.gets1(props) );
  wMVTrack.sets2( data->props, wMVTrack.gets2(props) );
  wMVTrack.setmph( data->props, wMVTrack.ismph(props) );
  
  s1 = ModelOp.getFBack( model, wMVTrack.gets1(data->props) );
  s2 = ModelOp.getFBack( model, wMVTrack.gets2(data->props) );
  if( s1 != NULL && s2 != NULL ) {
    FBackOp.addListener( s1, (obj)inst );
    FBackOp.addListener( s2, (obj)inst );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Sensors for the MVTrack could not be initialized." );
  }
}

/**  */
static struct OMVTrack* _inst( iONode props ) {
  iOMVTrack __MVTrack = allocMem( sizeof( struct OMVTrack ) );
  iOMVTrackData data = allocMem( sizeof( struct OMVTrackData ) );
  MemOp.basecpy( __MVTrack, &MVTrackOp, 0, sizeof( struct OMVTrack ), data );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "MVTrack" );
  
  /* Initialize data->xxx members... */

  data->props = props;

  __initSensors(__MVTrack);
  
  instCnt++;
  return __MVTrack;
}


/**  */
static void _reset( struct OMVTrack* inst ) {
  iOMVTrackData data = Data(inst);
  data->s1State  = False;
  data->s2State  = False;
  data->triggerS1= False;
  data->triggerS2= False;
  data->mvMillis = 0;
  data->mvTime   = 0L;
  return;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/mvtrack.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
