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

#include "rocrail/impl/display_impl.h"

#include "rocs/public/str.h"
#include "rocs/public/doc.h"
#include "rocs/public/thread.h"
#include "rocs/public/node.h"
#include "rocs/public/js.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"

#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/JsMap.h"
#include "rocrail/wrapper/public/LCD.h"
#include "rocrail/public/app.h"

static int instCnt = 0;
static iODisplay __displayinst = NULL;


/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iODisplayData data = Data(inst);
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

/** ----- ODisplay ----- */


static const char* __getSystemMenu(Boolean select) {
  iODisplayData data = NULL;
  
  if( __displayinst == NULL ) {
    return "";
  }
  data = Data(__displayinst);
  if( data->menulevel == 0 ) { 
    return "System";
  }
  else if( data->menulevel == 1 ) {
    switch( data->menuindex[1] ) {
      default:
        data->menuindex[1] = 0;
      case 0:
        return "System -> Init Turnouts";
      case 1:
        if( select ) AppOp.shutdown();
        return "System -> Stop";
      case 2:
        return "System -> Restart";
      case 3:
        return "System -> Shutdown";
    }
  }
  return "";
}


static void __showDisplay(Boolean select) {
  iODisplayData data = NULL;
  const char* menu;
  
  if( __displayinst == NULL ) {
    return;
  }
  data = Data(__displayinst);
  
  menu = "";
  switch( data->menuindex[0] ) {
    default:
      data->menulevel = 0;
      data->menuindex[data->menulevel] = 0;
    case 0:
      menu = __getSystemMenu(select);
      break;
    case 1:
      menu = "Layout";
      break;
    case 2:
      menu = "PT";
      break;
    case 3:
      menu = "Loc";
      break;
    case 4:
      menu = "Auto";
      break;
  }
  
  TraceOp.println( "--------------------------------------------------------------------------------" );
  TraceOp.println( "%s", menu );
  TraceOp.println( "--------------------------------------------------------------------------------" );
}


static void __myjsListener( int device, int type, int number, int value, unsigned long msec ) {
  iODisplayData data = NULL;
  
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
    "JsEvent dev=%d, type=%d, number=%d, value=%d, msec=%ld",
    device, type, number, value, msec );

  if( __displayinst == NULL ) {
    return;
  }
  data = Data(__displayinst);
  /*TODO: processing the event...*/
  
  if( type == 1 ) {
    /* buttons */
    if( number == wJsMap.getselect( data->jsmap ) ) {
      if( value == 1 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "select" );
        __showDisplay(True);
      }
    }
    else if( number == wJsMap.getpoweroff( data->jsmap ) ) {
      if( value == 1 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power OFF" );
      }
    }
    else if( number == wJsMap.getpoweron( data->jsmap ) ) {
      if( value == 1 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "power ON" );
      }
    }
  }
    
    
  if( type == 2 ) {
    /* axis */
    if( number == wJsMap.getleftright( data->jsmap ) ) {
      if( value > 0 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "right" );
        /* right */
        data->menulevel++;
        data->menuindex[data->menulevel] = 0;
        __showDisplay(False);
      }
      else if( value < 0 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "left" );
        /* left */
        data->menulevel--;
        __showDisplay(False);
      }
    }
    else if( number == wJsMap.getupdown( data->jsmap ) ) {
      if( value > 0 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "down" );
        data->menuindex[data->menulevel]++;
        __showDisplay(False);
      }
      else if( value < 0 ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "up" );
        data->menuindex[data->menulevel]--;
        __showDisplay(False);
      }
    }
  }    
        

}




/**  */
static struct ODisplay* _inst( iONode ini ) {
  if( __displayinst == NULL ) {
    iODisplay __Display = allocMem( sizeof( struct ODisplay ) );
    iODisplayData data = allocMem( sizeof( struct ODisplayData ) );
    MemOp.basecpy( __Display, &DisplayOp, 0, sizeof( struct ODisplay ), data );
  
    /* Initialize data->xxx members... */
    data->props = ini;
    
    if( wRocRail.getlcd( ini ) == NULL )
      data->lcd = NodeOp.inst( wLCD.name(), ini, ELEMENT_NODE ); 
    else 
      data->lcd = wRocRail.getlcd( ini ); 
    
    if( wRocRail.getjsmap( ini ) == NULL )
      data->jsmap = NodeOp.inst( wJsMap.name(), ini, ELEMENT_NODE ); 
    else 
      data->jsmap = wRocRail.getjsmap( ini ); 
      
    data->js = JSOp.inst();
    JSOp.setListener( data->js, &__myjsListener, 0 );
    JSOp.setListener( data->js, &__myjsListener, 1 );
    JSOp.setListener( data->js, &__myjsListener, 2 );
    JSOp.setListener( data->js, &__myjsListener, 3 );
    JSOp.init(data->js, &data->map);
    JSOp.start( data->js );
  
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "LCDisplay service started.");
    data->menulevel = 0;
    data->menuindex[0] = 0;
    __showDisplay(False);
    
    instCnt++;
    __displayinst = __Display;
  }
  return __displayinst;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/display.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
