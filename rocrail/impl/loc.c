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

#include "rocrail/impl/loc_impl.h"
#include "rocrail/public/block.h"
#include "rocrail/public/app.h"
#include "rocrail/public/model.h"
#include "rocrail/public/control.h"
#include "rocrail/public/http.h"

#include "rocint/public/lcdriverint.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/msg.h"
#include "rocs/public/lib.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Ctrl.h"
#include "rocrail/wrapper/public/FunDef.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/CVByte.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"

static int instCnt = 0;

static iONode __resetTimedFunction(iOLoc loc, iONode cmd, int function);

/*
 ***** OBase functions.
 */
static const char* __id( void* inst ) {
  return NULL;
}


static void __FnOnOff(iOLoc inst, int fn, Boolean OnOff, iONode cmd) {
  iOLocData data = Data(inst);
  int fgroup = 0;

  switch( fn ) {
    case  0: wFunCmd.setf0 ( cmd, OnOff ); data->fn0  = OnOff; fgroup = 0; break;
    case  1: wFunCmd.setf1 ( cmd, OnOff ); data->fn1  = OnOff; fgroup = 1; break;
    case  2: wFunCmd.setf2 ( cmd, OnOff ); data->fn2  = OnOff; fgroup = 1; break;
    case  3: wFunCmd.setf3 ( cmd, OnOff ); data->fn3  = OnOff; fgroup = 1; break;
    case  4: wFunCmd.setf4 ( cmd, OnOff ); data->fn4  = OnOff; fgroup = 1; break;
    case  5: wFunCmd.setf5 ( cmd, OnOff ); data->fn5  = OnOff; fgroup = 2; break;
    case  6: wFunCmd.setf6 ( cmd, OnOff ); data->fn6  = OnOff; fgroup = 2; break;
    case  7: wFunCmd.setf7 ( cmd, OnOff ); data->fn7  = OnOff; fgroup = 2; break;
    case  8: wFunCmd.setf8 ( cmd, OnOff ); data->fn8  = OnOff; fgroup = 2; break;
    case  9: wFunCmd.setf9 ( cmd, OnOff ); data->fn9  = OnOff; fgroup = 3; break;
    case 10: wFunCmd.setf10( cmd, OnOff ); data->fn10 = OnOff; fgroup = 3; break;
    case 11: wFunCmd.setf11( cmd, OnOff ); data->fn11 = OnOff; fgroup = 3; break;
    case 12: wFunCmd.setf12( cmd, OnOff ); data->fn12 = OnOff; fgroup = 3; break;
    case 13: wFunCmd.setf13( cmd, OnOff ); data->fn13 = OnOff; fgroup = 4; break;
    case 14: wFunCmd.setf14( cmd, OnOff ); data->fn14 = OnOff; fgroup = 4; break;
    case 15: wFunCmd.setf15( cmd, OnOff ); data->fn15 = OnOff; fgroup = 4; break;
    case 16: wFunCmd.setf16( cmd, OnOff ); data->fn16 = OnOff; fgroup = 4; break;
    case 17: wFunCmd.setf17( cmd, OnOff ); data->fn17 = OnOff; fgroup = 5; break;
    case 18: wFunCmd.setf18( cmd, OnOff ); data->fn18 = OnOff; fgroup = 5; break;
    case 19: wFunCmd.setf19( cmd, OnOff ); data->fn19 = OnOff; fgroup = 5; break;
    case 20: wFunCmd.setf20( cmd, OnOff ); data->fn20 = OnOff; fgroup = 5; break;
    case 21: wFunCmd.setf21( cmd, OnOff ); data->fn21 = OnOff; fgroup = 6; break;
    case 22: wFunCmd.setf22( cmd, OnOff ); data->fn22 = OnOff; fgroup = 6; break;
    case 23: wFunCmd.setf23( cmd, OnOff ); data->fn23 = OnOff; fgroup = 6; break;
    case 24: wFunCmd.setf24( cmd, OnOff ); data->fn24 = OnOff; fgroup = 6; break;
    case 25: wFunCmd.setf25( cmd, OnOff ); data->fn25 = OnOff; fgroup = 7; break;
    case 26: wFunCmd.setf26( cmd, OnOff ); data->fn26 = OnOff; fgroup = 7; break;
    case 27: wFunCmd.setf27( cmd, OnOff ); data->fn27 = OnOff; fgroup = 7; break;
    case 28: wFunCmd.setf28( cmd, OnOff ); data->fn28 = OnOff; fgroup = 7; break;
  }
  wFunCmd.setfncnt( cmd, wLoc.getfncnt( data->props ) );
  wFunCmd.setgroup( cmd, fgroup );
  wFunCmd.setfnchanged( cmd, fn );
}


static void __cpFn2Node(iOLoc inst, iONode cmd) {
  iOLocData data = Data(inst);
  wFunCmd.setfncnt( cmd, wLoc.getfncnt( data->props ) );
  wFunCmd.setf0 ( cmd, data->fn0  );
  wFunCmd.setf1 ( cmd, data->fn1  );
  wFunCmd.setf2 ( cmd, data->fn2  );
  wFunCmd.setf3 ( cmd, data->fn3  );
  wFunCmd.setf4 ( cmd, data->fn4  );
  wFunCmd.setf5 ( cmd, data->fn5  );
  wFunCmd.setf6 ( cmd, data->fn6  );
  wFunCmd.setf7 ( cmd, data->fn7  );
  wFunCmd.setf8 ( cmd, data->fn8  );
  wFunCmd.setf9 ( cmd, data->fn9  );
  wFunCmd.setf10( cmd, data->fn10 );
  wFunCmd.setf11( cmd, data->fn11 );
  wFunCmd.setf12( cmd, data->fn12 );
  wFunCmd.setf13( cmd, data->fn13 );
  wFunCmd.setf14( cmd, data->fn14 );
  wFunCmd.setf15( cmd, data->fn15 );
  wFunCmd.setf16( cmd, data->fn16 );
  wFunCmd.setf17( cmd, data->fn17 );
  wFunCmd.setf18( cmd, data->fn18 );
  wFunCmd.setf19( cmd, data->fn19 );
  wFunCmd.setf20( cmd, data->fn20 );
  wFunCmd.setf21( cmd, data->fn21 );
  wFunCmd.setf22( cmd, data->fn22 );
  wFunCmd.setf23( cmd, data->fn23 );
  wFunCmd.setf24( cmd, data->fn24 );
  wFunCmd.setf25( cmd, data->fn25 );
  wFunCmd.setf26( cmd, data->fn26 );
  wFunCmd.setf27( cmd, data->fn27 );
  wFunCmd.setf28( cmd, data->fn28 );
}


static void __cpNode2Fn(iOLoc inst, iONode cmd) {
  iOLocData data = Data(inst);
  data->fn0  = wFunCmd.isf0 ( cmd );
  data->fn1  = wFunCmd.isf1 ( cmd );
  data->fn2  = wFunCmd.isf2 ( cmd );
  data->fn3  = wFunCmd.isf3 ( cmd );
  data->fn4  = wFunCmd.isf4 ( cmd );
  data->fn5  = wFunCmd.isf5 ( cmd );
  data->fn6  = wFunCmd.isf6 ( cmd );
  data->fn7  = wFunCmd.isf7 ( cmd );
  data->fn8  = wFunCmd.isf8 ( cmd );
  data->fn9  = wFunCmd.isf9 ( cmd );
  data->fn10 = wFunCmd.isf10( cmd );
  data->fn11 = wFunCmd.isf11( cmd );
  data->fn12 = wFunCmd.isf12( cmd );
  data->fn13 = wFunCmd.isf13( cmd );
  data->fn14 = wFunCmd.isf14( cmd );
  data->fn15 = wFunCmd.isf15( cmd );
  data->fn16 = wFunCmd.isf16( cmd );
  data->fn17 = wFunCmd.isf17( cmd );
  data->fn18 = wFunCmd.isf18( cmd );
  data->fn19 = wFunCmd.isf19( cmd );
  data->fn20 = wFunCmd.isf20( cmd );
  data->fn21 = wFunCmd.isf21( cmd );
  data->fn22 = wFunCmd.isf22( cmd );
  data->fn23 = wFunCmd.isf23( cmd );
  data->fn24 = wFunCmd.isf24( cmd );
  data->fn25 = wFunCmd.isf25( cmd );
  data->fn26 = wFunCmd.isf26( cmd );
  data->fn27 = wFunCmd.isf27( cmd );
  data->fn28 = wFunCmd.isf28( cmd );
}

static void __copyNode2SelectiveFunction( iOLoc inst, iONode cmd) {
  iOLocData data = Data(inst);
  int fnchanged = wFunCmd.getfnchanged( cmd);

  if ( fnchanged != -1) {

    switch ( fnchanged) {
      case 0 : data->fn0  = wFunCmd.isf0 ( cmd ); break;
      case 1 : data->fn1  = wFunCmd.isf1 ( cmd ); break;
      case 2 : data->fn2  = wFunCmd.isf2 ( cmd ); break;
      case 3 : data->fn3  = wFunCmd.isf3 ( cmd ); break;
      case 4 : data->fn4  = wFunCmd.isf4 ( cmd ); break;
      case 5 : data->fn5  = wFunCmd.isf5 ( cmd ); break;
      case 6 : data->fn6  = wFunCmd.isf6 ( cmd ); break;
      case 7 : data->fn7  = wFunCmd.isf7 ( cmd ); break;
      case 8 : data->fn8  = wFunCmd.isf8 ( cmd ); break;
      case 9 : data->fn9  = wFunCmd.isf9 ( cmd ); break;
      case 10 : data->fn10 = wFunCmd.isf10( cmd ); break;
      case 11 : data->fn11 = wFunCmd.isf11( cmd ); break;
      case 12 : data->fn12 = wFunCmd.isf12( cmd ); break;
      case 13 : data->fn13 = wFunCmd.isf13( cmd ); break;
      case 14 : data->fn14 = wFunCmd.isf14( cmd ); break;
      case 15 : data->fn15 = wFunCmd.isf15( cmd ); break;
      case 16 : data->fn16 = wFunCmd.isf16( cmd ); break;
      case 17 : data->fn17 = wFunCmd.isf17( cmd ); break;
      case 18 : data->fn18 = wFunCmd.isf18( cmd ); break;
      case 19 : data->fn19 = wFunCmd.isf19( cmd ); break;
      case 20 : data->fn20 = wFunCmd.isf20( cmd ); break;
      case 21 : data->fn21 = wFunCmd.isf21( cmd ); break;
      case 22 : data->fn22 = wFunCmd.isf22( cmd ); break;
      case 23 : data->fn23 = wFunCmd.isf23( cmd ); break;
      case 24 : data->fn24 = wFunCmd.isf24( cmd ); break;
      case 25 : data->fn25 = wFunCmd.isf25( cmd ); break;
      case 26 : data->fn26 = wFunCmd.isf26( cmd ); break;
      case 27 : data->fn27 = wFunCmd.isf27( cmd ); break;
      case 28 : data->fn28 = wFunCmd.isf28( cmd ); break;
    }
  } else {
    data->fn0  = wFunCmd.isf0 ( cmd );
    data->fn1  = wFunCmd.isf1 ( cmd );
    data->fn2  = wFunCmd.isf2 ( cmd );
    data->fn3  = wFunCmd.isf3 ( cmd );
    data->fn4  = wFunCmd.isf4 ( cmd );
    data->fn5  = wFunCmd.isf5 ( cmd );
    data->fn6  = wFunCmd.isf6 ( cmd );
    data->fn7  = wFunCmd.isf7 ( cmd );
    data->fn8  = wFunCmd.isf8 ( cmd );
    data->fn9  = wFunCmd.isf9 ( cmd );
    data->fn10 = wFunCmd.isf10( cmd );
    data->fn11 = wFunCmd.isf11( cmd );
    data->fn12 = wFunCmd.isf12( cmd );
    data->fn13 = wFunCmd.isf13( cmd );
    data->fn14 = wFunCmd.isf14( cmd );
    data->fn15 = wFunCmd.isf15( cmd );
    data->fn16 = wFunCmd.isf16( cmd );
    data->fn17 = wFunCmd.isf17( cmd );
    data->fn18 = wFunCmd.isf18( cmd );
    data->fn19 = wFunCmd.isf19( cmd );
    data->fn20 = wFunCmd.isf20( cmd );
    data->fn21 = wFunCmd.isf21( cmd );
    data->fn22 = wFunCmd.isf22( cmd );
    data->fn23 = wFunCmd.isf23( cmd );
    data->fn24 = wFunCmd.isf24( cmd );
    data->fn25 = wFunCmd.isf25( cmd );
    data->fn26 = wFunCmd.isf26( cmd );
    data->fn27 = wFunCmd.isf27( cmd );
    data->fn28 = wFunCmd.isf28( cmd );
  }
}



static void* __event( void* inst, const void* evt ) {
  iOLocData data = Data(inst);
  iONode evtNode = (iONode)evt;

  if( evtNode == NULL )
    return NULL;

  if( data->go ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
        "ignore field event for [%s] while running in auto mode", wLoc.getid(data->props) );
    return NULL;
  }

  if( StrOp.equals( wLoc.name(), NodeOp.getName(evtNode) )) {
    int spcnt = wLoc.getspcnt( data->props );
    int V = 0;
    int V_raw = wLoc.getV_raw(evtNode);
    int V_rawMax = wLoc.getV_rawMax(evtNode);

    if( V_raw != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( data->props ), wLoc.V_mode_percent ) )
        V =  (100 * V_raw) / (V_rawMax == -1 ? spcnt:V_rawMax);
      else
        V =  (wLoc.getV_max( evtNode ) * V_raw) / (V_rawMax == -1 ? spcnt:V_rawMax);
    }
    else {
      V = wLoc.getV( data->props );
      if( data->curSpeed > V )
        V = data->curSpeed;
    }

    if( !StrOp.equals( wLoc.velocity, wLoc.getcmd(evtNode) ) ) {
      /* function and dir update */

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lc=%s dir=%d fn=%d",
          wLoc.getid(data->props), wLoc.isdir(evtNode), wLoc.isfn(evtNode));

      if ( StrOp.equals( wLoc.direction, wLoc.getcmd( evtNode) ) ) {
        /* only direction */
        if ( !data->go ) {
          wLoc.setdir( data->props, wLoc.isplacing(data->props) ? wLoc.isdir(evtNode):!wLoc.isdir(evtNode) );
        }
      } else if ( StrOp.equals( wLoc.direction, wLoc.getcmd( evtNode) ) ) {
        /* only function */
        if ( !data->go ) {
          wLoc.setfn( data->props, wLoc.isfn(evtNode) );
        }
      } else {
        /* other, set both */
        if ( !data->go ) {
          wLoc.setdir( data->props, wLoc.isplacing(data->props) ? wLoc.isdir(evtNode):!wLoc.isdir(evtNode) );
          wLoc.setfn( data->props, wLoc.isfn(evtNode) );
        }
      }
    }

    if( !data->go ) {
      wLoc.setV( data->props, V);
    }

    wLoc.setthrottleid( data->props, wLoc.getthrottleid(evtNode) );


    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lc=%s V_raw=%d V=%d dir=%s throttleID=%d",
        wLoc.getid(data->props), V_raw, V, wLoc.isdir(data->props)?"Forwards":"Reverse", wLoc.getthrottleid(data->props) );
    /* Broadcast to clients. */
    {
      iONode node = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setid( node, wLoc.getid( data->props ) );
      wLoc.setdir( node, wLoc.isdir( data->props ) );
      wLoc.setaddr( node, wLoc.getaddr( data->props ) );
      wLoc.setV( node, V );
      wLoc.setplacing( node, wLoc.isplacing( data->props ) );
      wLoc.setmode( node, wLoc.getmode( data->props ) );
      wLoc.setresumeauto( node, wLoc.isresumeauto(data->props) );
      wLoc.setblockid( node, data->curBlock );
      /* double ??? wLoc.setdir( node, wLoc.isdir(data->props) );*/
      wLoc.setfn( node, wLoc.isfn(data->props) );
      wLoc.setruntime( node, wLoc.getruntime(data->props) );
      wLoc.setmtime( node, wLoc.getmtime(data->props) );
      wLoc.setmint( node, wLoc.getmint(data->props) );
      wLoc.setthrottleid( node, wLoc.getthrottleid(data->props) );
      if( data->driver != NULL ) {
        wLoc.setscidx( node, data->driver->getScheduleIdx( data->driver ) );
      }

      ClntConOp.broadcastEvent( AppOp.getClntCon(  ), node );
    }

  }
  else if( StrOp.equals( wFunCmd.name(), NodeOp.getName(evtNode) )) {

    /* TODO: the digint library should provide the function group to prevent overwriting not reported functions */

    __copyNode2SelectiveFunction( inst, evtNode);
    if ( ( wFunCmd.getfnchanged( evtNode) == -1) || ( wFunCmd.getfnchanged( evtNode) == 0)) {
      wLoc.setfn( data->props, wFunCmd.isf0( evtNode));
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lc=%s f0=%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
        wLoc.getid( data->props ),
        wLoc.isfn(data->props) ? "on":"off",
        data->fn1  ? "01":"--", data->fn2  ? "02":"--", data->fn3  ? "03":"--", data->fn4  ? "04":"--",
        data->fn5  ? "05":"--", data->fn6  ? "06":"--", data->fn7  ? "07":"--", data->fn8  ? "08":"--",
        data->fn9  ? "09":"--", data->fn10 ? "10":"--", data->fn11 ? "11":"--", data->fn12 ? "12":"--",
        data->fn13 ? "13":"--", data->fn14 ? "14":"--", data->fn15 ? "15":"--", data->fn16 ? "16":"--",
        data->fn17 ? "17":"--", data->fn18 ? "18":"--", data->fn19 ? "19":"--", data->fn20 ? "20":"--",
        data->fn21 ? "21":"--", data->fn22 ? "22":"--", data->fn23 ? "23":"--", data->fn24 ? "24":"--",
        data->fn25 ? "25":"--", data->fn26 ? "26":"--", data->fn27 ? "27":"--", data->fn28 ? "28":"--"
    );
    /* Broadcast to clients. */
    {
      iONode node = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
      wFunCmd.setid( node, wLoc.getid( data->props ) );
      wFunCmd.setaddr( node, wLoc.getaddr( data->props ) );
      __cpFn2Node(inst, node);
      wFunCmd.setf0( node, wLoc.isfn(data->props) );
      wLoc.setfn( node, wLoc.isfn(data->props) );
      ClntConOp.broadcastEvent( AppOp.getClntCon(  ), node );
    }
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
  return (char*)LocOp.getId( (iOLoc)inst );
}
static void __del(void* inst) {
  iOLocData data = Data(inst);
  int retry = 0;
  data->run = False;
  /* wait for thread to stop. */
  while( data->running && retry < 10 ) {
    ThreadOp.sleep( 100 );
    retry++;
  };
  data->runner->base.del(data->runner);
  freeMem( data );
  freeMem( inst );
  instCnt--;
}
static void* __properties(void* inst) {
  iOLocData data = Data(inst);
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


static int __translateVhint(iOLoc inst, const char* V_hint ) {
  iOLocData data = Data(inst);
  int       V_new  = -1;

  int V_max = wLoc.getV_max( data->props );
  int V_mid = wLoc.getV_mid( data->props );
  int V_min = wLoc.getV_min( data->props );

  if( !wLoc.isdir(data->props) ){
    if( wLoc.getV_Rmax( data->props ) > 0 ) {
      V_max = wLoc.getV_Rmax( data->props );
    }
    if( wLoc.getV_Rmid( data->props ) > 0 ) {
      V_mid = wLoc.getV_Rmid( data->props );
    }
    if( wLoc.getV_Rmin( data->props ) > 0 ) {
      V_min = wLoc.getV_Rmin( data->props );
    }
  }

  if( StrOp.equals( wLoc.min, V_hint ) )
    V_new = V_min;

  else if( StrOp.equals( wLoc.mid, V_hint ) )
    V_new = V_mid;

  else if( StrOp.equals( wLoc.max, V_hint ) )
    V_new = V_max;

  else if( StrOp.equals( wLoc.cruise, V_hint ) ) {
    V_new = V_max;
    V_new = (V_new * 80) / 100;
  }

  else if( StrOp.equals( wLoc.climb, V_hint ) ) {
    V_new = V_max;
    V_new = (V_new * 90) / 100;
  }
  else {
    /* percent hint */
    int percent = atoi(V_hint);
    V_new = V_max;
    V_new = (V_new * percent) / 100;
    if( percent == 0 )
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "V_hint: \"%s\" = %d", V_hint, V_new );
  }

  return V_new;
}

/*return equal=0, lower=-1, higher=1*/
int _compareVhint(iOLoc inst, const char* V_hint) {
  iOLocData data  = Data(inst);
  int       V_new = __translateVhint( inst, V_hint );

  if( V_new == data->drvSpeed )
    return 0;
  else if( V_new > data->drvSpeed )
    return 1;
  else
    return -1;
}


static int __getFnTimer( iOLoc inst, int function) {
  iOLocData    data = Data(inst);

  iONode fundef = wLoc.getfundef( data->props );
  while( fundef != NULL ) {
    if( wFunDef.getfn(fundef) == function ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "function timer for %d = %d", function, wFunDef.gettimer(fundef) );
      return wFunDef.gettimer(fundef);
    }
    fundef = wLoc.nextfundef( data->props, fundef );
  };
  return 0;
}

/*
  Using the new V_* attributes for controlling loc speed.
*/
/*
   Called by the loc runner in a 100ms cycle or
   some object called it with a command node.
*/
static void __engine( iOLoc inst, iONode cmd ) {
  iOLocData    data = Data(inst);
  iOControl control = AppOp.getControl();

  const char* V_hint = NULL;
  int         V_new  = -1;
  iONode      cmdTD  = NULL;
  iONode      cmdFn  = NULL;
  static Boolean f0changed = False;

  if( cmd != NULL )
  {
    V_new  = wLoc.getV( cmd );
    V_hint = wLoc.getV_hint( cmd );

    if( NodeOp.findAttr(cmd,"dir") && wLoc.isdir(cmd) != wLoc.isdir( data->props ) ) {
      /* Informing the P50 interface. */
      wLoc.setsw( cmd, True );
      wLoc.setdir( data->props, wLoc.isdir(cmd) );
    }
    else if( wLoc.issw( cmd ) ) {
      /* Could be generated by the switch button of the locdlg. */
      wLoc.setdir( cmd, !wLoc.isdir( data->props ) );
      wLoc.setdir( data->props, wLoc.isdir(cmd) );
    }

    if( NodeOp.findAttr(cmd,"fn") ) {
      /* Informing the P50 interface. */
      wLoc.setfn( data->props, wLoc.isfn( cmd ) );
      if( data->fn0 != wLoc.isfn( cmd ) )
        f0changed = True;
      data->fn0 = wLoc.isfn( cmd );
    }

    if( StrOp.equals( wFunCmd.name(), NodeOp.getName(cmd )) ) {

      int fnchanged = -1;

      /* function timers */
      if( !data->fn0 && wFunCmd.isf0( cmd ) )
        data->fxtimer[0] = __getFnTimer( inst, 0);
      if( (!data->fn0 && wFunCmd.isf0( cmd ) ) || (data->fn0 && !wFunCmd.isf0( cmd ) ) || f0changed ) {
        fnchanged = 0;
        f0changed = False;
      }

      if( !data->fn1 && wFunCmd.isf1( cmd ) )
        data->fxtimer[1] = __getFnTimer( inst, 1);
      if( (!data->fn1 && wFunCmd.isf1( cmd ) ) || (data->fn1 && !wFunCmd.isf1( cmd ) ) )
        fnchanged = 1;

      if( !data->fn2 && wFunCmd.isf2( cmd ) )
        data->fxtimer[2] = __getFnTimer( inst, 2);
      if( (!data->fn2 && wFunCmd.isf2( cmd ) ) || (data->fn2 && !wFunCmd.isf2( cmd ) ) )
        fnchanged = 2;

      if( !data->fn3 && wFunCmd.isf3( cmd ) )
        data->fxtimer[3] = __getFnTimer( inst, 3);
      if( (!data->fn3 && wFunCmd.isf3( cmd ) ) || (data->fn3 && !wFunCmd.isf3( cmd ) ) )
        fnchanged = 3;

      if( !data->fn4 && wFunCmd.isf4( cmd ) )
        data->fxtimer[4] = __getFnTimer( inst, 4);
      if( (!data->fn4 && wFunCmd.isf4( cmd ) ) || (data->fn4 && !wFunCmd.isf4( cmd ) ) )
        fnchanged = 4;

      if( !data->fn5 && wFunCmd.isf5( cmd ) )
        data->fxtimer[5] = __getFnTimer( inst, 5);
      if( (!data->fn5 && wFunCmd.isf5( cmd ) ) || (data->fn5 && !wFunCmd.isf5( cmd ) ) )
        fnchanged = 5;

      if( !data->fn6 && wFunCmd.isf6( cmd ) )
        data->fxtimer[6] = __getFnTimer( inst, 6);
      if( (!data->fn6 && wFunCmd.isf6( cmd ) ) || (data->fn6 && !wFunCmd.isf6( cmd ) ) )
        fnchanged = 6;

      if( !data->fn7 && wFunCmd.isf7( cmd ) )
        data->fxtimer[7] = __getFnTimer( inst, 7);
      if( (!data->fn7 && wFunCmd.isf7( cmd ) ) || (data->fn7 && !wFunCmd.isf7( cmd ) ) )
        fnchanged = 7;

      if( !data->fn8 && wFunCmd.isf8( cmd ) )
        data->fxtimer[8] = __getFnTimer( inst, 8);
      if( (!data->fn8 && wFunCmd.isf8( cmd ) ) || (data->fn8 && !wFunCmd.isf8( cmd ) ) )
        fnchanged = 8;

      if( !data->fn9 && wFunCmd.isf9( cmd ) )
        data->fxtimer[9] = __getFnTimer( inst, 9);
      if( (!data->fn9 && wFunCmd.isf9( cmd ) ) || (data->fn9 && !wFunCmd.isf9( cmd ) ) )
        fnchanged = 9;

      if( !data->fn10 && wFunCmd.isf10( cmd ) )
        data->fxtimer[10] = __getFnTimer( inst, 10);
      if( (!data->fn10 && wFunCmd.isf10( cmd ) ) || (data->fn10 && !wFunCmd.isf10( cmd ) ) )
        fnchanged = 10;

      if( !data->fn11 && wFunCmd.isf11( cmd ) )
        data->fxtimer[11] = __getFnTimer( inst, 11);
      if( (!data->fn11 && wFunCmd.isf11( cmd ) ) || (data->fn11 && !wFunCmd.isf11( cmd ) ) )
        fnchanged = 11;

      if( !data->fn12 && wFunCmd.isf12( cmd ) )
        data->fxtimer[12] = __getFnTimer( inst, 12);
      if( (!data->fn12 && wFunCmd.isf12( cmd ) ) || (data->fn12 && !wFunCmd.isf12( cmd ) ) )
        fnchanged = 12;

      if( !data->fn13 && wFunCmd.isf13( cmd ) )
        data->fxtimer[13] = __getFnTimer( inst, 13);
      if( (!data->fn13 && wFunCmd.isf13( cmd ) ) || (data->fn13 && !wFunCmd.isf13( cmd ) ) )
        fnchanged = 13;

      if( !data->fn14 && wFunCmd.isf14( cmd ) )
        data->fxtimer[14] = __getFnTimer( inst, 14);
      if( (!data->fn14 && wFunCmd.isf14( cmd ) ) || (data->fn14 && !wFunCmd.isf14( cmd ) ) )
        fnchanged = 14;

      if( !data->fn15 && wFunCmd.isf15( cmd ) )
        data->fxtimer[15] = __getFnTimer( inst, 15);
      if( (!data->fn15 && wFunCmd.isf15( cmd ) ) || (data->fn15 && !wFunCmd.isf15( cmd ) ) )
        fnchanged = 15;

      if( !data->fn16 && wFunCmd.isf16( cmd ) )
        data->fxtimer[16] = __getFnTimer( inst, 16);
      if( (!data->fn16 && wFunCmd.isf16( cmd ) ) || (data->fn16 && !wFunCmd.isf16( cmd ) ) )
        fnchanged = 16;

      if( !data->fn17 && wFunCmd.isf17( cmd ) )
        data->fxtimer[17] = __getFnTimer( inst, 17);
      if( (!data->fn17 && wFunCmd.isf17( cmd ) ) || (data->fn17 && !wFunCmd.isf17( cmd ) ) )
        fnchanged = 17;

      if( !data->fn18 && wFunCmd.isf18( cmd ) )
        data->fxtimer[18] = __getFnTimer( inst, 18);
      if( (!data->fn18 && wFunCmd.isf18( cmd ) ) || (data->fn18 && !wFunCmd.isf18( cmd ) ) )
        fnchanged = 18;

      if( !data->fn19 && wFunCmd.isf19( cmd ) )
        data->fxtimer[19] = __getFnTimer( inst, 19);
      if( (!data->fn19 && wFunCmd.isf19( cmd ) ) || (data->fn19 && !wFunCmd.isf19( cmd ) ) )
        fnchanged = 19;

      if( !data->fn20 && wFunCmd.isf20( cmd ) )
        data->fxtimer[20] = __getFnTimer( inst, 20);
      if( (!data->fn20 && wFunCmd.isf20( cmd ) ) || (data->fn20 && !wFunCmd.isf20( cmd ) ) )
        fnchanged = 20;

      if( !data->fn21 && wFunCmd.isf21( cmd ) )
        data->fxtimer[21] = __getFnTimer( inst, 21);
      if( (!data->fn21 && wFunCmd.isf21( cmd ) ) || (data->fn21 && !wFunCmd.isf21( cmd ) ) )
        fnchanged = 21;

      if( !data->fn22 && wFunCmd.isf22( cmd ) )
        data->fxtimer[22] = __getFnTimer( inst, 22);
      if( (!data->fn22 && wFunCmd.isf22( cmd ) ) || (data->fn22 && !wFunCmd.isf22( cmd ) ) )
        fnchanged = 22;

      if( !data->fn23 && wFunCmd.isf23( cmd ) )
        data->fxtimer[23] = __getFnTimer( inst, 23);
      if( (!data->fn23 && wFunCmd.isf23( cmd ) ) || (data->fn23 && !wFunCmd.isf23( cmd ) ) )
        fnchanged = 23;

      if( !data->fn24 && wFunCmd.isf24( cmd ) )
        data->fxtimer[24] = __getFnTimer( inst, 24);
      if( (!data->fn24 && wFunCmd.isf24( cmd ) ) || (data->fn24 && !wFunCmd.isf24( cmd ) ) )
        fnchanged = 24;

      if( !data->fn25 && wFunCmd.isf25( cmd ) )
        data->fxtimer[25] = __getFnTimer( inst, 25);
      if( (!data->fn25 && wFunCmd.isf25( cmd ) ) || (data->fn25 && !wFunCmd.isf25( cmd ) ) )
        fnchanged = 25;

      if( !data->fn26 && wFunCmd.isf26( cmd ) )
        data->fxtimer[26] = __getFnTimer( inst, 26);
      if( (!data->fn26 && wFunCmd.isf26( cmd ) ) || (data->fn26 && !wFunCmd.isf26( cmd ) ) )
        fnchanged = 26;

      if( !data->fn27 && wFunCmd.isf27( cmd ) )
        data->fxtimer[27] = __getFnTimer( inst, 27);
      if( (!data->fn27 && wFunCmd.isf27( cmd ) ) || (data->fn27 && !wFunCmd.isf27( cmd ) ) )
        fnchanged = 27;

      if( !data->fn28 && wFunCmd.isf28( cmd ) )
        data->fxtimer[28] = __getFnTimer( inst, 28);
      if( (!data->fn28 && wFunCmd.isf28( cmd ) ) || (data->fn28 && !wFunCmd.isf28( cmd ) ) )
        fnchanged = 28;

      wFunCmd.setfnchanged(cmd, fnchanged);

      /* save the function status: */
      __cpNode2Fn(inst, cmd);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lc=%s lights=%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
          wLoc.getid( data->props ),
          data->fn0  ? "on":"off",
          data->fn1  ? "01":"--", data->fn2  ? "02":"--", data->fn3  ? "03":"--", data->fn4  ? "04":"--",
          data->fn5  ? "05":"--", data->fn6  ? "06":"--", data->fn7  ? "07":"--", data->fn8  ? "08":"--",
          data->fn9  ? "09":"--", data->fn10 ? "10":"--", data->fn11 ? "11":"--", data->fn12 ? "12":"--",
          data->fn13 ? "13":"--", data->fn14 ? "14":"--", data->fn15 ? "15":"--", data->fn16 ? "16":"--",
          data->fn17 ? "17":"--", data->fn18 ? "18":"--", data->fn19 ? "19":"--", data->fn20 ? "20":"--",
          data->fn21 ? "21":"--", data->fn22 ? "22":"--", data->fn23 ? "23":"--", data->fn24 ? "24":"--",
          data->fn25 ? "25":"--", data->fn26 ? "26":"--", data->fn27 ? "27":"--", data->fn28 ? "28":"--"
      );

      if( data->timedfn >= 0 && wFunCmd.gettimedfn( cmd ) >= 0 ) {
        /* reset previous timed function */
        __resetTimedFunction(inst, cmd, -1);
      }
      if( wFunCmd.gettimedfn( cmd ) >= 0 ) {
        data->timedfn = wFunCmd.gettimedfn( cmd );
        data->fntimer = wFunCmd.gettimer( cmd );

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
            "timedfn=%d fntimer=%d fnevent=%s", data->timedfn, data->fntimer, wFunCmd.getevent( cmd ) );

        if( StrOp.equals( wFeedbackEvent.enter_event, wFunCmd.getevent( cmd ) ) )
          data->fnevent = enter_event;
        else if( StrOp.equals( wFeedbackEvent.in_event, wFunCmd.getevent( cmd ) ) )
          data->fnevent = in_event;
        else
          data->fnevent = 0;

        data->fneventblock = StrOp.dup( wFunCmd.geteventblock( cmd ) );

      }
    }
  }



  /* New speed attributes: */
  if( V_hint != NULL ) {
    V_new = __translateVhint( inst, V_hint );

    if( data->drvSpeed != V_new || StrOp.equals( wFunCmd.name(), NodeOp.getName(cmd )) ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "V_hint: \"%s\" = %d", V_hint, V_new );
      data->drvSpeed = V_new;
      wLoc.setV( data->props, V_new);
      if( cmd == NULL )
        cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setV( cmd, V_new );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "ignore cmd because there is no diff \"%s\" == %d", V_hint, V_new );
      if( cmd != NULL) {
        NodeOp.base.del(cmd);
        cmd = NULL;
      }
    }
  }
  else if( V_new != -1 ) {
    data->drvSpeed = V_new;
    wLoc.setV( data->props, V_new);
    if( cmd == NULL )
      cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setV( cmd, V_new );
  }


  /* Check for simple decoders like "Maerklin Delta": */
  if( StrOp.equals( wLoc.V_mode_percent, wLoc.getV_mode( data->props ) ) &&
      wLoc.getV_step( data->props ) > 0 && wLoc.getspcnt( data->props ) <= 14 && !wLoc.isregulated( data->props ) )
  {
    if( data->step >= wLoc.getV_step( data->props ) ) {
      data->step = 0;
      if( data->curSpeed != data->drvSpeed ) {
        if( data->curSpeed < data->drvSpeed ) {
          int dif = data->drvSpeed - data->curSpeed;
          data->curSpeed += (dif > 10 ? 10:dif);
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "V=%d dif=%d", data->curSpeed, dif );
        }
        else if( data->curSpeed > data->drvSpeed ) {
          int dif = data->curSpeed - data->drvSpeed;
          data->curSpeed -= (dif < 10 ? dif:10);
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "V=%d dif=%d", data->curSpeed, dif );
        }
        if( cmd == NULL )
          cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setV( cmd, data->curSpeed );
      }
    }
    else if( cmd != NULL ) {
      /* Initial speed change. */
      wLoc.setV( cmd, data->curSpeed );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Initial V=%d", data->curSpeed );
    }
  }


  /* Send the command to the controller with all mandatory attributes: */
  if( cmd != NULL && control != NULL )
  {
    iIBlockBase curblock  = NULL;
    iIBlockBase gotoblock = NULL;
    if( data->curBlock != NULL )
      curblock  = ModelOp.getBlock( AppOp.getModel(), data->curBlock );
    if( data->gotoBlock != NULL )
      gotoblock = ModelOp.getBlock( AppOp.getModel(), data->gotoBlock );

    wLoc.setmass( cmd, wLoc.getmass( data->props ) );
    wLoc.setV_step( cmd, wLoc.getV_step( data->props ) );
    wLoc.setV_min( cmd, wLoc.getV_min( data->props ) );
    wLoc.setV_max( cmd, wLoc.getV_max( data->props ) );
    wLoc.setV_mode( cmd, wLoc.getV_mode( data->props ) );
    wLoc.setprot( cmd, wLoc.getprot( data->props ) );
    wLoc.setprotver( cmd, wLoc.getprotver( data->props ) );
    wLoc.setspcnt( cmd, wLoc.getspcnt( data->props ) );
    wLoc.setfncnt( cmd, wLoc.getfncnt( data->props ) );
    wLoc.setdir( cmd, wLoc.isdir( data->props ) );
    wLoc.setfn( cmd, wLoc.isfn( data->props ) );
    wLoc.setoid( cmd, wLoc.getoid(data->props) );
    wLoc.setid( cmd, wLoc.getid(data->props) );

    /* some controllers use this information because they make no diff between loc or fun cmd: */
    __cpFn2Node(inst, cmd);

    if( wLoc.getV( cmd ) == -1 )
      wLoc.setV( cmd, data->drvSpeed );

    /* Flip direction incase of front to back placing: */
    if( !wLoc.isplacing( data->props ) )
      wLoc.setdir( cmd, wLoc.isdir( cmd )?False:True );

    /* Analog loc: */
    if( StrOp.equals( wLoc.prot_A, wLoc.getprot( data->props ) ) ) {
      /* Track Driver */
      const char* iid = NULL;

      if( curblock != NULL ) {
        curblock->setAnalog( curblock, True );
        iid = curblock->getTDiid( curblock );
        if( iid != NULL )
          wLoc.setiid( cmd, iid );
        wLoc.setaddr( cmd, curblock->getTDaddress( curblock ) );
        wBlock.setport( cmd, curblock->getTDport( curblock ) );
        cmdFn = (iONode)NodeOp.base.clone( cmd );
        NodeOp.setName( cmdFn, wFunCmd.name() );
      }

      if( gotoblock != NULL && !gotoblock->isLinked( gotoblock ) ) {
        gotoblock->setAnalog( gotoblock, True );
        iid = gotoblock->getTDiid( gotoblock );
        cmdTD = (iONode)NodeOp.base.clone( cmd );
        if( iid != NULL )
          wLoc.setiid( cmd, iid );
        wLoc.setaddr( cmd, gotoblock->getTDaddress( gotoblock ) );
        wBlock.setport( cmd, gotoblock->getTDport( gotoblock ) );
      }

    }
    else {
      const char* iid = wLoc.getiid( data->props );
      if( curblock != NULL )
        curblock->setAnalog( curblock, False );
      if( gotoblock != NULL )
        gotoblock->setAnalog( gotoblock, False );

      if( iid != NULL )
        wLoc.setiid( cmd, iid );

      wLoc.setaddr( cmd, wLoc.getaddr( data->props ) );

      if( curblock != NULL ) {
        wBlock.setport( cmd, curblock->getTDport( curblock ) );
        if( wBlock.istd( curblock->base.properties(curblock) ) ) {
          /* send functions in every block */
          cmdFn = (iONode)NodeOp.base.clone( cmd );
          NodeOp.setName( cmdFn, wFunCmd.name() );
        }
      }

    }

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Sending command...V=%d",wLoc.getV(cmd) );
    if( cmdFn != NULL )
      ControlOp.cmd( control, cmdFn, NULL );

    ControlOp.cmd( control, cmd, NULL );

    if( cmdTD != NULL )
      ControlOp.cmd( control, cmdTD, NULL );
  }

  data->step++;
}


static iONode __resetTimedFunction(iOLoc loc, iONode cmd, int function) {
  iOLocData data = Data(loc);
  iONode fncmd = cmd==NULL?NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE ):cmd;
  int timedfn = data->timedfn;

  if( function >= 0 ) {
    timedfn = function;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "function [%d] deactivated", timedfn );
  }

  wFunCmd.setid ( fncmd, wLoc.getid(data->props) );
  wFunCmd.setf0 ( fncmd, timedfn== 0?False:data->fn0 );
  wFunCmd.setf1 ( fncmd, timedfn== 1?False:data->fn1 );
  wFunCmd.setf2 ( fncmd, timedfn== 2?False:data->fn2 );
  wFunCmd.setf3 ( fncmd, timedfn== 3?False:data->fn3 );
  wFunCmd.setf4 ( fncmd, timedfn== 4?False:data->fn4 );
  wFunCmd.setf5 ( fncmd, timedfn== 5?False:data->fn5 );
  wFunCmd.setf6 ( fncmd, timedfn== 6?False:data->fn6 );
  wFunCmd.setf7 ( fncmd, timedfn== 7?False:data->fn7 );
  wFunCmd.setf8 ( fncmd, timedfn== 8?False:data->fn8 );
  wFunCmd.setf9 ( fncmd, timedfn== 9?False:data->fn9 );
  wFunCmd.setf10( fncmd, timedfn==10?False:data->fn10 );
  wFunCmd.setf11( fncmd, timedfn==11?False:data->fn11 );
  wFunCmd.setf12( fncmd, timedfn==12?False:data->fn12 );
  wFunCmd.setf13( fncmd, timedfn==13?False:data->fn13 );
  wFunCmd.setf14( fncmd, timedfn==14?False:data->fn14 );
  wFunCmd.setf15( fncmd, timedfn==15?False:data->fn15 );
  wFunCmd.setf16( fncmd, timedfn==16?False:data->fn16 );
  wFunCmd.setf17( fncmd, timedfn==17?False:data->fn17 );
  wFunCmd.setf18( fncmd, timedfn==18?False:data->fn18 );
  wFunCmd.setf19( fncmd, timedfn==19?False:data->fn19 );
  wFunCmd.setf20( fncmd, timedfn==20?False:data->fn20 );
  wFunCmd.setf21( fncmd, timedfn==21?False:data->fn21 );
  wFunCmd.setf22( fncmd, timedfn==22?False:data->fn22 );
  wFunCmd.setf23( fncmd, timedfn==23?False:data->fn23 );
  wFunCmd.setf24( fncmd, timedfn==24?False:data->fn24 );
  wFunCmd.setf25( fncmd, timedfn==25?False:data->fn25 );
  wFunCmd.setf26( fncmd, timedfn==26?False:data->fn26 );
  wFunCmd.setf27( fncmd, timedfn==27?False:data->fn27 );
  wFunCmd.setf28( fncmd, timedfn==28?False:data->fn28 );
  data->timedfn = -1;

  wFunCmd.setgroup( fncmd, timedfn/4 + ((timedfn%4 > 0) ? 1:0) );

  wLoc.setfn(data->props, wFunCmd.isf0( fncmd ) );

  return fncmd;
}

static void __runner( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOLoc loc = (iOLoc)ThreadOp.getParm( th );
  iOLocData data = Data(loc);
  int   tick = 0;
  Boolean sendedcnfg = False;
  Boolean loccnfg = wCtrl.isloccnfg( AppOp.getIniNode( wCtrl.name() ) );

  ThreadOp.setDescription( th, wLoc.getdesc( data->props ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Runner for \"%s\" started.", LocOp.getId( loc ) );
  data->running = True;

  data->speedstep = wLoc.getV_step( data->props );

  data->runtime = wLoc.getruntime( data->props );

  do {
    iOMsg msg = (iOMsg)ThreadOp.getPost( th );
    obj    emitter = NULL;
    iONode fncmd   = NULL;
    iONode broadcast = NULL;

    int   i     = 0;
    int   event = -1;
    int   timer = 0;

    if( msg != NULL ) {
      emitter = MsgOp.getSender( msg );
      event   = MsgOp.getEvent( msg );
      timer   = MsgOp.getTimer( msg );
      msg->base.del( msg );
    }

    if( data->driver != NULL ) {
      if( timer > 0 ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "loc evttimer %d ms", wLoc.getevttimer(data->props) );
        if( wLoc.getevttimer(data->props) > 0 )
          timer = wLoc.getevttimer(data->props);
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "timed event[%d] %d ms", event, timer );
        ThreadOp.sleep( timer );
      }
      data->driver->drive( data->driver, emitter, event );
    }

    if( !sendedcnfg && loccnfg ) {
      iOControl control = AppOp.getControl();
      if( control != NULL ) {
        iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
        const char* prot = wLoc.getprot(data->props);
        int protver = wLoc.getprotver(data->props);
        wSysCmd.setcmd( cmd, wSysCmd.loccnfg );
        wSysCmd.setid( cmd, wLoc.getid(data->props) );

        /* supply object ID: */
        if( wLoc.getoid(data->props) != NULL )
          wSysCmd.setoid( cmd, wLoc.getoid(data->props) );

        wSysCmd.setval( cmd, wLoc.getaddr(data->props) );
        if( prot[0] == wLoc.prot_M[0] && protver == 1)
          wSysCmd.setvalA( cmd, 0 );
        else if( prot[0] == wLoc.prot_M[0] && protver == 2)
          wSysCmd.setvalA( cmd, 1 );
        else
          wSysCmd.setvalA( cmd, 2 );
        wSysCmd.setvalB( cmd, wLoc.getspcnt(data->props) );
        ControlOp.cmd( control, cmd, NULL );
        sendedcnfg = True;
      }
    }

    /* this is approximately a second */
    if( tick % 10 == 0 && tick != 0 ) {
      if( data->drvSpeed > 0  ) {
        data->runtime++;
        wLoc.setruntime( data->props, data->runtime );
      }

      for( i = 0; i < 12; i++ ) {
        if( data->fxtimer[i] > 0 ) {
          data->fxtimer[i]--;
          if( data->fxtimer[i] == 0 ) {
            fncmd = __resetTimedFunction(loc, NULL, i+1);
          }
        }
      }

      if( fncmd == NULL && data->timedfn >= 0 && data->fntimer >= 0 ) {
        data->fntimer--;
        if( data->fntimer == 0 ) {
          fncmd = __resetTimedFunction(loc, NULL, -1);
        }
      }
      tick = 0;
    }


    if( fncmd != NULL ) {
      wLoc.setV( fncmd, -1 );
      broadcast = (iONode)NodeOp.base.clone(fncmd);
      __engine( loc, fncmd );

      /* Broadcast to clients. */
      wLoc.setid( broadcast, wLoc.getid( data->props ) );
      wLoc.setdir( broadcast, wLoc.isdir( data->props ) );
      wLoc.setaddr( broadcast, wLoc.getaddr( data->props ) );
      wLoc.setV( broadcast, data->drvSpeed );
      wLoc.setfn( broadcast, wLoc.isfn( data->props ) );
      wLoc.setplacing( broadcast, wLoc.isplacing( data->props ) );
      wLoc.setmode( broadcast, wLoc.getmode( data->props ) );
      wLoc.setresumeauto( broadcast, wLoc.isresumeauto(data->props) );
      wLoc.setruntime( broadcast, wLoc.getruntime(data->props) );
      wLoc.setmtime( broadcast, wLoc.getmtime(data->props) );
      wLoc.setmint( broadcast, wLoc.getmint(data->props) );
      wLoc.setthrottleid( broadcast, wLoc.getthrottleid(data->props) );
      if( data->driver != NULL ) {
        wLoc.setscidx( broadcast, data->driver->getScheduleIdx( data->driver ) );
      }
      ClntConOp.broadcastEvent( AppOp.getClntCon(  ), broadcast );
    }


    ThreadOp.sleep( 100 );
    tick++;
  } while( data->run && !ThreadOp.isQuit(th) );

  data->running = False;
}



static void _event( iOLoc inst, obj emitter, int evt, int timer ) {
  iOLocData data = Data(inst);

  iOMsg msg = MsgOp.inst( emitter, evt );
  iIBlockBase block = (iIBlockBase)MsgOp.getSender(msg);
  MsgOp.setTimer( msg, timer );
  ThreadOp.post( data->runner, (obj)msg );

  {
    iONode fundef = wLoc.getfundef( data->props );
    iIBlockBase emitterblock = (iIBlockBase)emitter;
    const char* blockid = emitterblock->base.id( emitterblock );

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Event %d from \"%s\"", evt, blockid );

    while( fundef != NULL ) {
      const char* onevent = wFunDef.getonevent( fundef );
      const char* offevent = wFunDef.getoffevent( fundef );
      int fn = wFunDef.getfn( fundef );
      iOStrTok  onblocks = StrTokOp.inst( wFunDef.getonblockid ( fundef ), ',' );
      iOStrTok offblocks = StrTokOp.inst( wFunDef.getoffblockid( fundef ), ',' );
      Boolean  isonevent = False;
      Boolean isoffevent = False;

      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checking event for function \"%s\"", wFunDef.gettext(fundef) );

      while( StrTokOp.hasMoreTokens( onblocks ) ) {
        const char* tok = StrTokOp.nextToken( onblocks );
        if( StrOp.equals( blockid, tok ) ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "ON Event %d from \"%s\"", evt, blockid );
          isonevent = True;
          break;
        }
      };
      while( StrTokOp.hasMoreTokens( offblocks ) ) {
        const char* tok = StrTokOp.nextToken( offblocks );
        if( StrOp.equals( blockid, tok ) ) {
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 99949, "OFF Event %d from \"%s\"", evt, blockid );
          isoffevent = True;
          break;
        }
      };

      if( isonevent ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "onevent[%s] evt[%d]", onevent, evt );
        if( StrOp.equals( wFunDef.enter_block, onevent ) && evt == enter_event ||
            StrOp.equals( wFunDef.in_block   , onevent ) && evt == in_event    ||
            StrOp.equals( wFunDef.exit_block , onevent ) && evt == exit_event
           ) {
          iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "On Event for funcion %d.", fn );
          wFunCmd.setid( cmd, LocOp.getId( inst ) );
          __cpFn2Node(inst, cmd);
          __FnOnOff(inst, fn, True, cmd);
          LocOp.cmd( inst, cmd );
        }
      }

      if( isoffevent ) {
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "offevent[%s] evt[%d]", offevent, evt );
        if( StrOp.equals( wFunDef.enter_block, offevent ) && evt == enter_event ||
            StrOp.equals( wFunDef.in_block   , offevent ) && evt == in_event    ||
            StrOp.equals( wFunDef.exit_block , offevent ) && evt == exit_event
           ) {
          iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Off Event for funcion %d.", fn );
          wFunCmd.setid( cmd, LocOp.getId( inst ) );
          __cpFn2Node(inst, cmd);
          __FnOnOff(inst, fn, False, cmd);
          LocOp.cmd( inst, cmd );
        }
      }

      /* Cleanup */
      StrTokOp.base.del( onblocks );
      StrTokOp.base.del( offblocks );

      fundef = wLoc.nextfundef( data->props, fundef );
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "timedfn=%d fnevent=%d fnblock=%s evt=%d",
        data->timedfn, data->fnevent, data->fneventblock, evt );
    if( data->timedfn >= 0 && data->fnevent > 0 && data->fneventblock != NULL ) {
      if( StrOp.equals(data->fneventblock, block->base.id(block))) {
        if( data->fnevent == evt || data->fnevent == evt ) {
          iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Off Event for funcion %d.", data->timedfn );
          wFunCmd.setid( cmd, LocOp.getId( inst ) );
          __cpFn2Node(inst, cmd);
          __FnOnOff(inst, data->timedfn, False, cmd);
          LocOp.cmd( inst, cmd );
          StrOp.free( data->fneventblock );
          data->fneventblock = NULL;
          data->fnevent = 0;
        }
      }
    }
  }
}

/*
 ***** _Public functions.
 */
static const char* _getId( iOLoc inst ) {
  iOLocData data = Data(inst);
  return wLoc.getid( data->props );
}

static int _getLen( iOLoc inst ) {
  iOLocData data = Data(inst);
  return wLoc.getlen( data->props );
}

static void* _getProperties( void* inst ) {
  iOLocData data = Data((iOLoc)inst);
  return data->props;
}

static const char* _getCurBlock( iOLoc inst ) {
  iOLocData data = Data(inst);
  return data->curBlock;
}

/* CRjBlock calls this function.
 * Loc can't go in automatic when curBlock in not set.
 */
static void _setCurBlock( iOLoc inst, const char* id ) {
  iOLocData data = Data(inst);



  if( id == NULL || StrOp.len(id) == 0 ) {
    if( data->driver != NULL && data->driver->isRun( data->driver ) ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
          "Deny block reset in running mode! (%s)", LocOp.getId(inst) );
      return;
    }
  }

  data->curBlock = id;

  if( data->driver != NULL )
    data->driver->curblock( data->driver, id );

  /* Broadcast to clients. */
  {
    iONode node = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( node, wLoc.getid( data->props ) );
    wLoc.setaddr( node, wLoc.getaddr( data->props ) );
    wLoc.setdir( node, wLoc.isdir( data->props ) );
    wLoc.setfn( node, wLoc.isfn( data->props ) );
    wLoc.setV( node, data->drvSpeed );
    wLoc.setplacing( node, wLoc.isplacing( data->props ) );
    wLoc.setmode( node, wLoc.getmode( data->props ) );
    wLoc.setresumeauto( node, wLoc.isresumeauto(data->props) );
    wLoc.setblockid( node, data->curBlock );
    wLoc.setruntime( node, wLoc.getruntime(data->props) );
    wLoc.setmtime( node, wLoc.getmtime(data->props) );
    wLoc.setmint( node, wLoc.getmint(data->props) );
    wLoc.setthrottleid( node, wLoc.getthrottleid(data->props) );
    if( data->driver != NULL ) {
      wLoc.setscidx( node, data->driver->getScheduleIdx( data->driver ) );
    }

    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), node );
  }
}

static void _informBlock( iOLoc inst, const char* destid, const char* curid ) {
  iOLocData data = Data(inst);
  iONode node = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  data->destBlock = destid;
  data->curBlock  = curid;
  /* Broadcast to clients. */
  wLoc.setid( node, wLoc.getid( data->props ) );
  wLoc.setaddr( node, wLoc.getaddr( data->props ) );
  wLoc.setdir( node, wLoc.isdir( data->props ) );
  wLoc.setfn( node, wLoc.isfn( data->props ) );
  wLoc.setV( node, data->drvSpeed );
  wLoc.setplacing( node, wLoc.isplacing( data->props ) );
  wLoc.setmode( node, wLoc.getmode( data->props ) );
  wLoc.setresumeauto( node, wLoc.isresumeauto(data->props) );
  wLoc.setdestblockid( node, destid );
  wLoc.setruntime( node, wLoc.getruntime(data->props) );
  wLoc.setmtime( node, wLoc.getmtime(data->props) );
  wLoc.setmint( node, wLoc.getmint(data->props) );
  wLoc.setthrottleid( node, wLoc.getthrottleid(data->props) );
  wLoc.setblockid( node, curid );
  if( data->driver != NULL ) {
    wLoc.setscidx( node, data->driver->getScheduleIdx( data->driver ) );
  }
  ClntConOp.broadcastEvent( AppOp.getClntCon(  ), node );
}

static void _gotoBlock( iOLoc inst, const char* id ) {
  iOLocData data = Data(inst);
  data->gotoBlock = id;
  if( data->driver != NULL )
    data->driver->gotoblock( data->driver, id );
}

static void _useSchedule( iOLoc inst, const char* id ) {
  iOLocData data = Data(inst);
  if( data->driver != NULL ) {
    iONode schedule = ModelOp.getSchedule( AppOp.getModel(), id );
    if( schedule != NULL )
      data->driver->useschedule( data->driver, id );
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Schedule [%s] not found!", id );
  }
}

static const char* _getSchedule( iOLoc inst ) {
  iOLocData data = Data(inst);
  if( data->driver != NULL ) {
    const char* schedule = data->driver->getschedule( data->driver );
    if( schedule != NULL )
      return schedule;
  }
  return "";
}

static void _go( iOLoc inst ) {
  iOLocData data = Data(inst);
  wLoc.setresumeauto( data->props, False);
  if( data->curBlock != NULL && StrOp.len(data->curBlock) > 0 ) {
    data->go = True;
    data->gomanual = False;
    if( data->driver != NULL )
      data->driver->go( data->driver, data->gomanual );
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "current block not set for [%s]", LocOp.getId(inst) );
  }
}

static void _stop( iOLoc inst, Boolean resume ) {
  iOLocData data = Data(inst);
  if( resume && data->go ) {
    wLoc.setresumeauto( data->props, True);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "resume flag set for [%s]", LocOp.getId(inst) );
  }

  data->go = False;
  if( data->driver != NULL )
    data->driver->stop( data->driver );
}

static void _dispatch( iOLoc inst ) {
  iOLocData data = Data(inst);
  iOControl control = AppOp.getControl();

  iONode cmd = (iONode)NodeOp.base.clone(data->props);
  wLoc.setcmd( cmd, wLoc.dispatch );
  ControlOp.cmd( control, cmd, NULL );

}

static void _reset( iOLoc inst, Boolean saveCurBlock ) {
  iOLocData data = Data(inst);
  data->go    = False;
  data->enter = False;
  data->in    = False;
  data->exit  = False;
  data->out   = False;
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
      "reset [%s] in block [%s]", LocOp.getId(inst), data->curBlock==NULL?"?":data->curBlock );
  if( data->driver != NULL )
    data->driver->reset( data->driver, saveCurBlock );
}

static void __stopgo( iOLoc inst ) {
  iOLocData data = Data(inst);
  data->go = !data->go;
  data->gomanual = False;
  if( data->go )
    _go( inst );
  else
    _stop( inst, False );
}


static void __gomanual( iOLoc inst ) {
  iOLocData data = Data(inst);
  data->go = True;
  data->gomanual = True;
  if( data->driver != NULL )
    data->driver->go( data->driver, data->gomanual );
}


static void _brake( iOLoc inst ) {
  iOLocData data = Data(inst);
  data->brake = True;
  if( data->driver != NULL )
    data->driver->brake( data->driver );
}


static void __checkConsist( iOLoc inst, iONode nodeA ) {
  iOLocData data = Data(inst);

  /* check consist and send a copy of the nodeA */
  if( nodeA != NULL && StrOp.len( wLoc.getconsist(data->props) ) > 0 ) {
    iOStrTok  consist = StrTokOp.inst( wLoc.getconsist ( data->props ), ',' );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sending command to the consist member [%s]",
                   wLoc.getconsist(data->props) );

    while( StrTokOp.hasMoreTokens( consist ) ) {
      const char* tok = StrTokOp.nextToken( consist );
      iOLoc consistloc = ModelOp.getLoc( AppOp.getModel(), tok );
      if( consistloc != NULL ) {
        iONode consistcmd = (iONode)NodeOp.base.clone( nodeA );

        /* check consist details */
        if( wLoc.isconsist_lightsoff(data->props) ) {
          wLoc.setfn( consistcmd, False );
        }

        LocOp.cmd( consistloc, consistcmd );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "consist loco [%s] not found", tok );
      }
    };
    StrTokOp.base.del( consist );
  }

}

static Boolean _cmd( iOLoc inst, iONode nodeA ) {
  iOLocData data = Data(inst);
  iOControl control = AppOp.getControl(  );
  iONode nodeF = NULL;

  iOModel model = AppOp.getModel(  );

  const char* nodename = NodeOp.getName( nodeA );
  const char* cmd  = wLoc.getcmd( nodeA );

  if( TraceOp.getLevel(NULL) & TRCLEVEL_USER1 ) {
    char* cmdstr = NodeOp.base.toString( nodeA );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, cmdstr );
    StrOp.free( cmdstr );
  }

  if( cmd != NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cmd \"%s\" for %s.",
                   cmd, LocOp.getId( inst ) );

    if( ModelOp.isAuto( AppOp.getModel() ) ) {
      if( StrOp.equals( wLoc.go, cmd ) ) {
        _go( inst );
      }
      else if( StrOp.equals( wLoc.gomanual, cmd ) ) {
        __gomanual( inst );
      }
      else if( StrOp.equals( wLoc.stopgo, cmd ) ) {
        __stopgo( inst );
      }
    }
    else {
      if( StrOp.equals( wLoc.go, cmd ) || StrOp.equals( wLoc.stopgo, cmd ) ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
                     "Ignoring go commands for %s when not in AutoMode!",
                      LocOp.getId( inst ) );
        nodeA->base.del(nodeA);
        return False;
      }
    }


    if( StrOp.equals( wLoc.stop, cmd ) ) {
      _stop( inst, False );
    }
    else if( StrOp.equals( wLoc.reset, cmd ) ) {
      _reset( inst, False );
    }
    else if( StrOp.equals( wLoc.swap, cmd ) ) {
      LocOp.swapPlacing(inst);
      nodeF = (iONode)NodeOp.base.clone( nodeA );
      /* Broadcast to clients. */
      wLoc.setid( nodeF, wLoc.getid( data->props ) );
      wLoc.setaddr( nodeF, wLoc.getaddr( data->props ) );
      wLoc.setdir( nodeF, wLoc.isdir( data->props ) );
      wLoc.setV( nodeF, data->drvSpeed );
      wLoc.setfn( nodeF, wLoc.isfn( data->props ) );
      wLoc.setplacing( nodeF, wLoc.isplacing( data->props ) );
      wLoc.setmode( nodeF, wLoc.getmode( data->props ) );
      wLoc.setresumeauto( nodeF, wLoc.isresumeauto(data->props) );
      wLoc.setruntime( nodeF, wLoc.getruntime(data->props) );
      wLoc.setmtime( nodeF, wLoc.getmtime(data->props) );
      wLoc.setmint( nodeF, wLoc.getmint(data->props) );
      wLoc.setthrottleid( nodeF, wLoc.getthrottleid(data->props) );
      ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeF );
    }
    else if( StrOp.equals( wLoc.dispatch, cmd ) ) {
      _dispatch( inst );
    }
    else if( StrOp.equals( wLoc.brake, cmd ) ) {
      _brake( inst );
    }
    else if( StrOp.equals( wLoc.gotoblock, cmd ) ) {
      const char* blockid = wLoc.getblockid( nodeA );
      LocOp.gotoBlock( inst, blockid );
    }
    else if( StrOp.equals( wLoc.useschedule, cmd ) ) {
      const char* scheduleid = wLoc.getscheduleid( nodeA );
      LocOp.useSchedule( inst, scheduleid );
    }
    else if( StrOp.equals( wLoc.shortid, cmd ) ) {
      /* send short ID to command station */
      iONode cmdNode = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setid( cmdNode, wLoc.getid(data->props) );
      wLoc.setaddr( cmdNode, wLoc.getaddr(data->props) );
      wLoc.setshortid( cmdNode, wLoc.getshortid(data->props) );
      wLoc.setprot( cmdNode, wLoc.getprot( data->props ) );
      wLoc.setprotver( cmdNode, wLoc.getprotver( data->props ) );
      wLoc.setspcnt( cmdNode, wLoc.getspcnt( data->props ) );
      wLoc.setfncnt( cmdNode, wLoc.getfncnt( data->props ) );
      wLoc.setcmd( cmdNode, wLoc.shortid );
      ControlOp.cmd( control, cmdNode, NULL );
    }
    else if( StrOp.equals( wLoc.block, cmd ) ) {
      const char* blockid = wLoc.getblockid( nodeA );

      if( data->curBlock != NULL ) {
        iIBlockBase block = ModelOp.getBlock( model, data->curBlock );
        if( block ) {
          iONode cmd = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
          wBlock.setid( cmd, data->curBlock );
          wBlock.setlocid( cmd, "" );
          block->cmd( block, cmd );
        }
      }

      if( blockid != NULL ) {
        iIBlockBase block = ModelOp.getBlock( model, blockid );
        if( block ) {
          iONode cmd = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
          wBlock.setid( cmd, blockid );
          wBlock.setlocid( cmd, LocOp.getId( inst ) );
          block->cmd( block, cmd );
        }
      }

      nodeA->base.del(nodeA);
    }

    return True;
  }

  nodeF = (iONode)NodeOp.base.clone( nodeA );

  /* Inform Driver. */
  if( data->driver != NULL )
    data->driver->info( data->driver, nodeA );

  __engine( inst, nodeA );
  __checkConsist(inst, nodeF);

  /* Broadcast to clients. */
  wLoc.setid( nodeF, wLoc.getid( data->props ) );
  wLoc.setaddr( nodeF, wLoc.getaddr( data->props ) );
  wLoc.setdir( nodeF, wLoc.isdir( data->props ) );
  wLoc.setV( nodeF, data->drvSpeed );
  wLoc.setfn( nodeF, wLoc.isfn( data->props ) );
  wLoc.setplacing( nodeF, wLoc.isplacing( data->props ) );
  wLoc.setmode( nodeF, wLoc.getmode( data->props ) );
  wLoc.setresumeauto( nodeF, wLoc.isresumeauto(data->props) );
  wLoc.setruntime( nodeF, wLoc.getruntime(data->props) );
  wLoc.setmtime( nodeF, wLoc.getmtime(data->props) );
  wLoc.setmint( nodeF, wLoc.getmint(data->props) );
  wLoc.setthrottleid( nodeF, wLoc.getthrottleid(data->props) );
  if( data->driver != NULL ) {
    wLoc.setscidx( nodeF, data->driver->getScheduleIdx( data->driver ) );
  }
  ClntConOp.broadcastEvent( AppOp.getClntCon(  ), nodeF );

  return True;
}

/**
 * Checks for property changes.
 * todo: Range checking?
 */
static void _modify( iOLoc inst, iONode props ) {
  iOLocData data = Data(inst);
  int cnt = NodeOp.getAttrCnt( props );
  int i = 0;

  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );
    if( !StrOp.equals( "runtime", name ) )
      NodeOp.setStr( data->props, name, value );
  }

  /* Leave the childs if no new are comming */
  if( NodeOp.getChildCnt( props ) > 0 ) {
    cnt = NodeOp.getChildCnt( data->props );
    while( cnt > 0 ) {
      iONode child = NodeOp.getChild( data->props, 0 );
      NodeOp.removeChild( data->props, child );
      cnt = NodeOp.getChildCnt( data->props );
    }
    cnt = NodeOp.getChildCnt( props );
    for( i = 0; i < cnt; i++ ) {
      iONode child = NodeOp.getChild( props, i );
      NodeOp.addChild( data->props, (iONode)NodeOp.base.clone(child) );
    }
  }

  data->secondnextblock = wLoc.issecondnextblock( data->props );

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)props->base.clone( props );
    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), clone );
  }
  props->base.del(props);
}


static const char* _tableHdr(void) {
  return "<tr bgcolor=\"#CCCCCC\"><th>LocID</th><th>Addr</th><th>Remark</th><th>CatNr</th><th>f1</th><th>f2</th><th>f3</th><th>f4</th></tr>\n";
}


static char* _normalizeID( const char* id ) {
  char* nid = StrOp.dup( id );
  char* p = nid;
  int i = 0;
  while( *p != '\0' ) {
    if( *p == ' ' )
      *p = '_';
    p++;
  }
  return nid;
}

static char* _toHtml( void* inst ) {
  iOLocData data = Data((iOLoc)inst);
  char* hrefid = _normalizeID( wLoc.getid( data->props ) );
  char* html = NULL;
  const char* fn1 = "";
  const char* fn2 = "";
  const char* fn3 = "";
  const char* fn4 = "";

  iONode fundef = wLoc.getfundef( data->props );
  while( fundef != NULL ) {
    const char* fntxt = wFunDef.gettext( fundef );
    switch( wFunDef.getfn( fundef ) ) {
      case 1: fn1 = fntxt; break;
      case 2: fn2 = fntxt; break;
      case 3: fn3 = fntxt; break;
      case 4: fn4 = fntxt; break;
    }
    fundef = wLoc.nextfundef( data->props, fundef );
  }

  html = StrOp.fmt( "<tr><td><a href=\"%s%s\">%s</a></td><td align=\"right\">%d</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td><td>%s</td></tr>\n",
      "loc?id=",
      hrefid,
      wLoc.getid( data->props ),
      wLoc.getaddr( data->props ),
      wLoc.getdesc( data->props ),
      wLoc.getcatnr( data->props ),
      fn1,
      fn2,
      fn3,
      fn4
      );
  StrOp.free( hrefid );
  return html;
}


static char* _getForm( void* inst ) {
  iOLocData data = Data((iOLoc)inst);
  char* hrefid = _normalizeID( wLoc.getid( data->props ) );
  char* form = StrOp.fmt( "<form method=\"post\" action=\"loc?id=%s\">\n"
                          "<table>\n"
                          "<tr><td>id</td><td><input name=\"id\" value=\"%s\" readonly></td></tr>\n"
                          "<tr><td>description</td><td><input name=\"desc\" value=\"%s\" readonly></td></tr>\n"
                          "<tr><td>address</td><td><input name=\"addr\" value=\"%d\"></td></tr>\n"
                          "<tr><td>train length</td><td><input name=\"len\" value=\"%d\"></td></tr>\n"
                          "<tr><td>min speed</td><td><input name=\"mins\" value=\"%d\"></td></tr>\n"
                          "<tr><td>max speed</td><td><input name=\"maxs\" value=\"%d\"></td></tr>\n"
                          "<tr><td>acceleration</td><td><input name=\"accel\" value=\"%d\"></td></tr>\n"
                          "</table>\n"
                          "<input type=\"submit\" value=\"Submit\"><br>\n"
                          "</form>\n",
                          hrefid,
                          wLoc.getid( data->props ),
                          wLoc.getdesc( data->props ),
                          wLoc.getaddr( data->props ),
                          wLoc.getlen( data->props ),
                          wLoc.getV_min( data->props ),
                          wLoc.getV_max( data->props ),
                          wLoc.getV_step( data->props )
                          );
  StrOp.free( hrefid );
  return form;
}


static char* _postForm( void* inst, const char* postdata ) {
  iOLocData data = Data((iOLoc)inst);
  char* reply = StrOp.fmt( "OK, properties changed for %s.<br>", wLoc.getid( data->props ) );
  iOMap map = HttpOp.createPostDataMap( postdata );

  char* val = (char*)MapOp.get( map, "addr" );
  if( val != NULL ) {
    int ival = atoi( val );
    if( ival != wLoc.getaddr( data->props ) )
      wLoc.setaddr( data->props, ival );
  }

  val = (char*)MapOp.get( map, "mins" );
  if( val != NULL ) {
    int ival = atoi( val );
    if( ival != wLoc.getV_min( data->props ) )
      wLoc.setV_min( data->props, ival );
  }

  val = (char*)MapOp.get( map, "maxs" );
  if( val != NULL ) {
    int ival = atoi( val );
    if( ival != wLoc.getV_max( data->props ) )
      wLoc.setV_max( data->props, ival );
  }

  val = (char*)MapOp.get( map, "accel" );
  if( val != NULL ) {
    int ival = atoi( val );
    if( ival != wLoc.getV_step( data->props ) )
      wLoc.setV_step( data->props, ival );
  }

  /* Cleanup map: */
  HttpOp.deletePostDataMap( map );
  return reply;
}


typedef iILcDriverInt (* LPFNGETLCDIRINT)( const iOLoc, const iOModel ,const iOTrace, iONode ctrl );

static Boolean __loadDriver( iOLoc inst ) {
  iOLocData data = Data(inst);
  int ignevt = wCtrl.getignevt( AppOp.getIniNode( wCtrl.name() ) );
  int eventtimeout = wCtrl.geteventtimeout( AppOp.getIniNode( wCtrl.name() ) );
  int signalreset  = wCtrl.getsignalreset( AppOp.getIniNode( wCtrl.name() ) );
  Boolean secondnextblock = wCtrl.issecondnextblock( AppOp.getIniNode( wCtrl.name() ) );

  iOLib    pLib = NULL;
  /*iILcDriverInt rocGetLcDrInt( const iOLoc loc, const iOModel model, const iOTrace trc )*/
  LPFNGETLCDIRINT pInitFun = (void *) NULL;

  data->ignevt = ignevt;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "load LcDriver lib=\"%s\"", wLoc.getlclib( data->props ) );

  if( FileOp.isAbsolute( wLoc.getlclib( data->props ) ) ) {
    pLib = LibOp.inst(wLoc.getlclib( data->props ));
  }
  else {
    char* libpath = StrOp.fmt( "%s%c%s", AppOp.getLibPath(), SystemOp.getFileSeparator(), wLoc.getlclib( data->props ) );
    pLib = LibOp.inst( libpath );
    StrOp.free( libpath );
  }

  if (pLib == NULL)
    return False;
  pInitFun = (LPFNGETLCDIRINT)LibOp.getProc( pLib, "rocGetLcDrInt" );
  if (pInitFun == NULL)
    return False;

  data->driver = pInitFun( inst, AppOp.getModel(), TraceOp.get(), AppOp.getIniNode( wCtrl.name() ) );

  return True;
}


static int _getAddress( iOLoc loc ) {
  iOLocData data = Data(loc);
  return wLoc.getaddr( data->props );
}


static void __initCVmap( iOLoc loc ) {
  iOLocData data = Data(loc);
  iONode cv = NodeOp.findNode( data->props, wCVByte.name() );
  while( cv != NULL ) {
    char* key = StrOp.fmt( "%d", wCVByte.getnr( cv ) );
    MapOp.put( data->cvMap, key, (obj)cv );
    StrOp.free( key );
    cv = NodeOp.findNextNode( data->props, cv );
  };
}

static int _getCV( iOLoc loc, int nr ) {
  iOLocData data = Data(loc);
  int val = -1;
  char* key = StrOp.fmt( "%d", nr );
  iONode cv = (iONode)MapOp.get( data->cvMap, key );
  StrOp.free( key );
  if( cv != NULL )
    val = wCVByte.getvalue( cv );
  return val;
}


static void _setCV( iOLoc loc, int nr, int value ) {
  iOLocData data = Data(loc);
  char* key = StrOp.fmt( "%d", nr );
  iONode cv = (iONode)MapOp.get( data->cvMap, key );

  if( cv != NULL )
    wCVByte.setvalue( cv, value );
  else {
    cv = NodeOp.inst( wCVByte.name(), data->props, ELEMENT_NODE );
    wCVByte.setnr( cv, nr );
    wCVByte.setvalue( cv, value );
    NodeOp.addChild( data->props, cv );
    MapOp.put( data->cvMap, key, (obj)cv );
  }

  StrOp.free( key );

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)data->props->base.clone( data->props );
    ClntConOp.broadcastEvent( AppOp.getClntCon(), clone );
  }
}

/**
 * swap placing to run in defaults routes after reaching an terminal station
 */
static void _swapPlacing( iOLoc loc ) {
  iOLocData data = Data(loc);
  wLoc.setplacing( data->props, !wLoc.isplacing( data->props ) );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "placing for [%s] set to [%s]", wLoc.getid(data->props), wLoc.isplacing( data->props )?"FWD":"REV" );
  /* inform model to keep this setting in the occupation file */
  ModelOp.setBlockOccupation( AppOp.getModel(), data->curBlock, wLoc.getid(data->props), False, wLoc.isplacing( data->props) ? 1:2 );

  /* Broadcast to clients. */
  {
    iONode node = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( node, wLoc.getid( data->props ) );
    wLoc.setdir( node, wLoc.isdir( data->props ) );
    wLoc.setaddr( node, wLoc.getaddr( data->props ) );
    wLoc.setV( node, data->drvSpeed );
    wLoc.setfn( node, wLoc.isfn( data->props ) );
    wLoc.setplacing( node, wLoc.isplacing( data->props ) );
    wLoc.setmode( node, wLoc.getmode( data->props ) );
    wLoc.setresumeauto( node, wLoc.isresumeauto(data->props) );
    wLoc.setblockid( node, data->curBlock );
    wLoc.setruntime( node, wLoc.getruntime(data->props) );
    wLoc.setmtime( node, wLoc.getmtime(data->props) );
    wLoc.setmint( node, wLoc.getmint(data->props) );
    wLoc.setthrottleid( node, wLoc.getthrottleid(data->props) );
    if( data->driver != NULL ) {
      wLoc.setscidx( node, data->driver->getScheduleIdx( data->driver ) );
    }

    ClntConOp.broadcastEvent( AppOp.getClntCon(  ), node );
  }
}


/**
 * get the direction regarding the placing of the locomotive
 */
static Boolean _getDir( iOLoc loc ) {
  iOLocData data = Data(loc);
  Boolean dir     = wLoc.isdir( data->props );
  Boolean placing = wLoc.isplacing( data->props );

  if( !placing ) {
    /* the placing flag is handled at engine level, so the logic does not care... */
    /* dir = !dir; */
  }

  return dir;
}


/**
 * get the placing of the locomotive
 */
static Boolean _getPlacing( iOLoc loc ) {
  iOLocData data = Data(loc);
  return wLoc.isplacing( data->props );
}


static int _getV( iOLoc loc ) {
  iOLocData data = Data(loc);
  return data->drvSpeed;
}


static Boolean _isAutomode( iOLoc loc ) {
  iOLocData data = Data(loc);
  return data->go;
}


static Boolean _isResumeAutomode( iOLoc loc ) {
  iOLocData data = Data(loc);
  return wLoc.isresumeauto( data->props );
}


static iONode _getFunctionStatus( iOLoc loc, iONode cmd ) {
  iOLocData data = Data(loc);
  /* save the function status: */
  __cpFn2Node(loc, cmd);
  wFunCmd.setf0( cmd, wLoc.isfn(data->props) );
  return cmd;
}


static Boolean _trySecondNextBlock( iOLoc inst ) {
  iOLocData data = Data(inst);

  return data->secondnextblock;
}


static iOLoc _inst( iONode props ) {
  iOLoc     loc  = allocMem( sizeof( struct OLoc ) );
  iOLocData data = allocMem( sizeof( struct OLocData ) );

  /* OBase operations */
  MemOp.basecpy( loc, &LocOp, 0, sizeof( struct OLoc ), data );

  data->props = props;
  data->cvMap = MapOp.inst();
  data->secondnextblock = wLoc.issecondnextblock( data->props );
  data->timedfn = -1; /* function 0 is also used */

  /* reset velocity to zero */
  wLoc.setV( data->props, 0 );
  wLoc.setfx( data->props, 0 );
  wLoc.setthrottleid( data->props, 0 );

  __initCVmap( loc );

  /*data->driver = (iILcDriverInt)LcDriverOp.inst( loc );*/
  if( __loadDriver( loc ) ) {
    data->runner = ThreadOp.inst( _getId(loc), &__runner, loc );
    data->run = True;
    ThreadOp.start( data->runner );
  }

  instCnt++;

  return loc;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/loc.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
