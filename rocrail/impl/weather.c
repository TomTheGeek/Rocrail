/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


#include "rocrail/impl/weather_impl.h"

#include <time.h>

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/thread.h"

#include "rocrail/public/app.h"
#include "rocrail/public/control.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOWeatherData data = Data(inst);
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

/** ----- OWeather ----- */
static void __doDaylight(iOWeather weather, int hour, int min ) {
  iOWeatherData data = Data(weather);

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "do daylight at %02d:%02d", hour, min );
}


static void __checkWeatherThemes(iOWeather weather, int hour, int min ) {
  iOWeatherData data = Data(weather);

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "check weather themes at %02d:%02d", hour, min );
}


static void __makeWeather( void* threadinst ) {
  iOThread        th = (iOThread)threadinst;
  iOWeather  weather = (iOWeather)ThreadOp.getParm(th);
  iOWeatherData data = Data(weather);

  iOControl control = AppOp.getControl();
  int lastMin = 0;
  int loopCnt = 10;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "make weather started..." );

  while( data->run ) {
    if( loopCnt >= 10 ) {
      loopCnt = 0;
      long t = ControlOp.getTime(control);
      struct tm* ltm = localtime( &t );

      if( lastMin != ltm->tm_min ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "weather time is %02d:%02d", ltm->tm_hour, ltm->tm_min );
        lastMin = ltm->tm_min;
        __doDaylight(weather, ltm->tm_hour, ltm->tm_min );
        __checkWeatherThemes(weather, ltm->tm_hour, ltm->tm_min );
      }
    }
    else {
      loopCnt++;
    }

    ThreadOp.sleep(100);
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "make weather ended..." );

}

static void _halt( iOWeather inst ) {
  iOWeatherData data = Data(inst);
  data->run = False;
  ThreadOp.sleep(120);
}


/**  */
static struct OWeather* _inst( iONode ini ) {
  iOWeather __Weather = allocMem( sizeof( struct OWeather ) );
  iOWeatherData data = allocMem( sizeof( struct OWeatherData ) );
  MemOp.basecpy( __Weather, &WeatherOp, 0, sizeof( struct OWeather ), data );

  /* Initialize data->xxx members... */
  data->props = ini;
  data->run = True;

  data->makeWeather = ThreadOp.inst( "makeWeather", __makeWeather, __Weather );
  ThreadOp.start( data->makeWeather );

  instCnt++;
  return __Weather;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/weather.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
