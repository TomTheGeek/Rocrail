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

#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Ctrl.h"
#include "rocrail/wrapper/public/FunDef.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/CVByte.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FeedbackEvent.h"
#include "rocrail/wrapper/public/Schedule.h"
#include "rocrail/wrapper/public/Tour.h"
#include "rocrail/wrapper/public/ActionCtrl.h"

static int instCnt = 0;

static iONode __resetTimedFunction(iOLoc loc, iONode cmd, int function);
static void __checkConsist( iOLoc inst, iONode nodeA, Boolean byEvent );
static void __funEvent( iOLoc inst, const char* blockid, int evt, int timer );
static void __swapConsist( iOLoc inst, iONode cmd );
static int __getFnAddr( iOLoc inst, int function, int* mappedfn);

/*
 ***** OBase functions.
 */
static const char* __id( void* inst ) {
  iOLocData data     = Data(inst);
  return wLoc.getid( data->props );
}

static void __checkAction( iOLoc inst, const char* state ) {

  iOLocData data     = Data(inst);
  iOModel   model    = AppOp.getModel();
  iONode    lcaction = wLoc.getactionctrl( data->props );

  while( lcaction != NULL) {
      if( StrOp.equals(wActionCtrl.getstate(lcaction), state ) )
      {

        iOAction action = ModelOp.getAction( AppOp.getModel(), wActionCtrl.getid( lcaction ));
        if( action != NULL ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco action: %s", wActionCtrl.getid( lcaction ));
          ActionOp.exec(action, lcaction);
        }
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action state does not match: [%s-%s]",
            wActionCtrl.getstate( lcaction ), state );
      }
    lcaction = wLoc.nextactionctrl( data->props, lcaction );
  }

}



static void __FnOnOff(iOLoc inst, int fn, Boolean OnOff, iONode cmd, Boolean save) {
  iOLocData data = Data(inst);
  int fgroup = 0;

  switch( fn ) {
    case  0: wFunCmd.setf0 ( cmd, OnOff ); if(save) data->fn0  = OnOff; fgroup = 0; break;
    case  1: wFunCmd.setf1 ( cmd, OnOff ); if(save) data->fn1  = OnOff; fgroup = 1; break;
    case  2: wFunCmd.setf2 ( cmd, OnOff ); if(save) data->fn2  = OnOff; fgroup = 1; break;
    case  3: wFunCmd.setf3 ( cmd, OnOff ); if(save) data->fn3  = OnOff; fgroup = 1; break;
    case  4: wFunCmd.setf4 ( cmd, OnOff ); if(save) data->fn4  = OnOff; fgroup = 1; break;
    case  5: wFunCmd.setf5 ( cmd, OnOff ); if(save) data->fn5  = OnOff; fgroup = 2; break;
    case  6: wFunCmd.setf6 ( cmd, OnOff ); if(save) data->fn6  = OnOff; fgroup = 2; break;
    case  7: wFunCmd.setf7 ( cmd, OnOff ); if(save) data->fn7  = OnOff; fgroup = 2; break;
    case  8: wFunCmd.setf8 ( cmd, OnOff ); if(save) data->fn8  = OnOff; fgroup = 2; break;
    case  9: wFunCmd.setf9 ( cmd, OnOff ); if(save) data->fn9  = OnOff; fgroup = 3; break;
    case 10: wFunCmd.setf10( cmd, OnOff ); if(save) data->fn10 = OnOff; fgroup = 3; break;
    case 11: wFunCmd.setf11( cmd, OnOff ); if(save) data->fn11 = OnOff; fgroup = 3; break;
    case 12: wFunCmd.setf12( cmd, OnOff ); if(save) data->fn12 = OnOff; fgroup = 3; break;
    case 13: wFunCmd.setf13( cmd, OnOff ); if(save) data->fn13 = OnOff; fgroup = 4; break;
    case 14: wFunCmd.setf14( cmd, OnOff ); if(save) data->fn14 = OnOff; fgroup = 4; break;
    case 15: wFunCmd.setf15( cmd, OnOff ); if(save) data->fn15 = OnOff; fgroup = 4; break;
    case 16: wFunCmd.setf16( cmd, OnOff ); if(save) data->fn16 = OnOff; fgroup = 4; break;
    case 17: wFunCmd.setf17( cmd, OnOff ); if(save) data->fn17 = OnOff; fgroup = 5; break;
    case 18: wFunCmd.setf18( cmd, OnOff ); if(save) data->fn18 = OnOff; fgroup = 5; break;
    case 19: wFunCmd.setf19( cmd, OnOff ); if(save) data->fn19 = OnOff; fgroup = 5; break;
    case 20: wFunCmd.setf20( cmd, OnOff ); if(save) data->fn20 = OnOff; fgroup = 5; break;
    case 21: wFunCmd.setf21( cmd, OnOff ); if(save) data->fn21 = OnOff; fgroup = 6; break;
    case 22: wFunCmd.setf22( cmd, OnOff ); if(save) data->fn22 = OnOff; fgroup = 6; break;
    case 23: wFunCmd.setf23( cmd, OnOff ); if(save) data->fn23 = OnOff; fgroup = 6; break;
    case 24: wFunCmd.setf24( cmd, OnOff ); if(save) data->fn24 = OnOff; fgroup = 6; break;
    case 25: wFunCmd.setf25( cmd, OnOff ); if(save) data->fn25 = OnOff; fgroup = 7; break;
    case 26: wFunCmd.setf26( cmd, OnOff ); if(save) data->fn26 = OnOff; fgroup = 7; break;
    case 27: wFunCmd.setf27( cmd, OnOff ); if(save) data->fn27 = OnOff; fgroup = 7; break;
    case 28: wFunCmd.setf28( cmd, OnOff ); if(save) data->fn28 = OnOff; fgroup = 7; break;
  }
  wFunCmd.setfncnt( cmd, wLoc.getfncnt( data->props ) );
  wFunCmd.setgroup( cmd, fgroup );
  wFunCmd.setfnchanged( cmd, fn );
}


static void __cpFn2Node(iOLoc inst, iONode cmd, int fn, int addr) {
  iOLocData data = Data(inst);
  int mappedfn = 0;
  wFunCmd.setfncnt( cmd, wLoc.getfncnt( data->props ) );

  if( addr == 0 )
    if( fn == -1 || fn != 0 ) wFunCmd.setf0 ( cmd, data->fn0  );

  if( addr == 0 || __getFnAddr(inst, 1, &mappedfn) == addr )
    if( fn == -1 || fn != 1 ) __FnOnOff(inst, mappedfn, data->fn1, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 2, &mappedfn) == addr )
    if( fn == -1 || fn != 2 ) __FnOnOff(inst, mappedfn, data->fn2, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 3, &mappedfn) == addr )
    if( fn == -1 || fn != 3 ) __FnOnOff(inst, mappedfn, data->fn3, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 4, &mappedfn) == addr )
    if( fn == -1 || fn != 4 ) __FnOnOff(inst, mappedfn, data->fn4, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 5, &mappedfn) == addr )
    if( fn == -1 || fn != 5 ) __FnOnOff(inst, mappedfn, data->fn5, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 6, &mappedfn) == addr )
    if( fn == -1 || fn != 6 ) __FnOnOff(inst, mappedfn, data->fn6, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 7, &mappedfn) == addr )
    if( fn == -1 || fn != 7 ) __FnOnOff(inst, mappedfn, data->fn7, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 8, &mappedfn) == addr )
    if( fn == -1 || fn != 8 ) __FnOnOff(inst, mappedfn, data->fn8, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 9, &mappedfn) == addr )
    if( fn == -1 || fn != 9 ) __FnOnOff(inst, mappedfn, data->fn9, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 10, &mappedfn) == addr )
    if( fn == -1 || fn != 10 ) __FnOnOff(inst, mappedfn, data->fn10, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 11, &mappedfn) == addr )
    if( fn == -1 || fn != 11 ) __FnOnOff(inst, mappedfn, data->fn11, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 12, &mappedfn) == addr )
    if( fn == -1 || fn != 12 ) __FnOnOff(inst, mappedfn, data->fn12, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 13, &mappedfn) == addr )
    if( fn == -1 || fn != 13 ) __FnOnOff(inst, mappedfn, data->fn13, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 14, &mappedfn) == addr )
    if( fn == -1 || fn != 14 ) __FnOnOff(inst, mappedfn, data->fn14, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 15, &mappedfn) == addr )
    if( fn == -1 || fn != 15 ) __FnOnOff(inst, mappedfn, data->fn15, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 16, &mappedfn) == addr )
    if( fn == -1 || fn != 16 ) __FnOnOff(inst, mappedfn, data->fn16, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 17, &mappedfn) == addr )
    if( fn == -1 || fn != 17 ) __FnOnOff(inst, mappedfn, data->fn17, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 18, &mappedfn) == addr )
    if( fn == -1 || fn != 18 ) __FnOnOff(inst, mappedfn, data->fn18, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 19, &mappedfn) == addr )
    if( fn == -1 || fn != 19 ) __FnOnOff(inst, mappedfn, data->fn19, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 20, &mappedfn) == addr )
    if( fn == -1 || fn != 20 ) __FnOnOff(inst, mappedfn, data->fn20, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 21, &mappedfn) == addr )
    if( fn == -1 || fn != 21 ) __FnOnOff(inst, mappedfn, data->fn21, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 22, &mappedfn) == addr )
    if( fn == -1 || fn != 22 ) __FnOnOff(inst, mappedfn, data->fn22, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 23, &mappedfn) == addr )
    if( fn == -1 || fn != 23 ) __FnOnOff(inst, mappedfn, data->fn23, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 24, &mappedfn) == addr )
    if( fn == -1 || fn != 24 ) __FnOnOff(inst, mappedfn, data->fn24, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 25, &mappedfn) == addr )
    if( fn == -1 || fn != 25 ) __FnOnOff(inst, mappedfn, data->fn25, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 26, &mappedfn) == addr )
    if( fn == -1 || fn != 26 ) __FnOnOff(inst, mappedfn, data->fn26, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 27, &mappedfn) == addr )
    if( fn == -1 || fn != 27 ) __FnOnOff(inst, mappedfn, data->fn27, cmd, False);
  if( addr == 0 || __getFnAddr(inst, 28, &mappedfn) == addr )
    if( fn == -1 || fn != 28 ) __FnOnOff(inst, mappedfn, data->fn28, cmd, False);
}


static void __saveFxState(iOLoc inst) {
  iOLocData data = Data(inst);
  int fx = 0;
  fx |= data->fn1  ? 0x0000001:0;
  fx |= data->fn2  ? 0x0000002:0;
  fx |= data->fn3  ? 0x0000004:0;
  fx |= data->fn4  ? 0x0000008:0;
  fx |= data->fn5  ? 0x0000010:0;
  fx |= data->fn6  ? 0x0000020:0;
  fx |= data->fn7  ? 0x0000040:0;
  fx |= data->fn8  ? 0x0000080:0;
  fx |= data->fn9  ? 0x0000100:0;
  fx |= data->fn10 ? 0x0000200:0;
  fx |= data->fn11 ? 0x0000400:0;
  fx |= data->fn12 ? 0x0000800:0;
  fx |= data->fn13 ? 0x0001000:0;
  fx |= data->fn14 ? 0x0002000:0;
  fx |= data->fn15 ? 0x0004000:0;
  fx |= data->fn16 ? 0x0008000:0;
  fx |= data->fn17 ? 0x0010000:0;
  fx |= data->fn18 ? 0x0020000:0;
  fx |= data->fn19 ? 0x0040000:0;
  fx |= data->fn20 ? 0x0080000:0;
  fx |= data->fn21 ? 0x0100000:0;
  fx |= data->fn22 ? 0x0200000:0;
  fx |= data->fn23 ? 0x0400000:0;
  fx |= data->fn24 ? 0x0800000:0;
  fx |= data->fn25 ? 0x1000000:0;
  fx |= data->fn26 ? 0x2000000:0;
  fx |= data->fn27 ? 0x4000000:0;
  fx |= data->fn28 ? 0x8000000:0;
  wLoc.setfx( data->props, fx );
}

static Boolean __cpNode2Fn(iOLoc inst, iONode cmd) {
  iOLocData data = Data(inst);
  if( StrOp.equals( wLoc.function, wLoc.getcmd(cmd) ) ) {
    int function = wFunCmd.getfnchanged(cmd);
    switch ( function ) {
      case 0 : data->fn0 = wFunCmd.isf0(cmd); return data->fn0;
      case 1 : data->fn1 = wFunCmd.isf1(cmd); return data->fn1;
      case 2 : data->fn2 = wFunCmd.isf2(cmd); return data->fn2;
      case 3 : data->fn3 = wFunCmd.isf3(cmd); return data->fn3;
      case 4 : data->fn4 = wFunCmd.isf4(cmd); return data->fn4;
      case 5 : data->fn5 = wFunCmd.isf5(cmd); return data->fn5;
      case 6 : data->fn6 = wFunCmd.isf6(cmd); return data->fn6;
      case 7 : data->fn7 = wFunCmd.isf7(cmd); return data->fn7;
      case 8 : data->fn8 = wFunCmd.isf8(cmd); return data->fn8;
      case 9 : data->fn9 = wFunCmd.isf9(cmd); return data->fn9;
      case 10 : data->fn10 = wFunCmd.isf10(cmd); return data->fn10;
      case 11 : data->fn11 = wFunCmd.isf11(cmd); return data->fn11;
      case 12 : data->fn12 = wFunCmd.isf12(cmd); return data->fn12;
      case 13 : data->fn13 = wFunCmd.isf13(cmd); return data->fn13;
      case 14 : data->fn14 = wFunCmd.isf14(cmd); return data->fn14;
      case 15 : data->fn15 = wFunCmd.isf15(cmd); return data->fn15;
      case 16 : data->fn16 = wFunCmd.isf16(cmd); return data->fn16;
      case 17 : data->fn17 = wFunCmd.isf17(cmd); return data->fn17;
      case 18 : data->fn18 = wFunCmd.isf18(cmd); return data->fn18;
      case 19 : data->fn19 = wFunCmd.isf19(cmd); return data->fn19;
      case 20 : data->fn20 = wFunCmd.isf20(cmd); return data->fn20;
      case 21 : data->fn21 = wFunCmd.isf21(cmd); return data->fn21;
      case 22 : data->fn22 = wFunCmd.isf22(cmd); return data->fn22;
      case 23 : data->fn23 = wFunCmd.isf23(cmd); return data->fn23;
      case 24 : data->fn24 = wFunCmd.isf24(cmd); return data->fn24;
      case 25 : data->fn25 = wFunCmd.isf25(cmd); return data->fn25;
      case 26 : data->fn26 = wFunCmd.isf26(cmd); return data->fn26;
      case 27 : data->fn27 = wFunCmd.isf27(cmd); return data->fn27;
      case 28 : data->fn28 = wFunCmd.isf28(cmd); return data->fn28;
    }
  } else {
    if( wFunCmd.getgroup(cmd) == 0)
      data->fn0  = wFunCmd.isf0 ( cmd );
    if( wFunCmd.getgroup(cmd) == 0 || wFunCmd.getgroup(cmd) == 1 ) {
    data->fn1  = wFunCmd.isf1 ( cmd );
    data->fn2  = wFunCmd.isf2 ( cmd );
    data->fn3  = wFunCmd.isf3 ( cmd );
    data->fn4  = wFunCmd.isf4 ( cmd );
    }
    if( wFunCmd.getgroup(cmd) == 0 || wFunCmd.getgroup(cmd) == 2 ) {
    data->fn5  = wFunCmd.isf5 ( cmd );
    data->fn6  = wFunCmd.isf6 ( cmd );
    data->fn7  = wFunCmd.isf7 ( cmd );
    data->fn8  = wFunCmd.isf8 ( cmd );
    }
    if( wFunCmd.getgroup(cmd) == 0 || wFunCmd.getgroup(cmd) == 3 ) {
    data->fn9  = wFunCmd.isf9 ( cmd );
    data->fn10 = wFunCmd.isf10( cmd );
    data->fn11 = wFunCmd.isf11( cmd );
    data->fn12 = wFunCmd.isf12( cmd );
    }
    if( wFunCmd.getgroup(cmd) == 0 || wFunCmd.getgroup(cmd) == 4 ) {
    data->fn13 = wFunCmd.isf13( cmd );
    data->fn14 = wFunCmd.isf14( cmd );
    data->fn15 = wFunCmd.isf15( cmd );
    data->fn16 = wFunCmd.isf16( cmd );
    }
    if( wFunCmd.getgroup(cmd) == 0 || wFunCmd.getgroup(cmd) == 5 ) {
    data->fn17 = wFunCmd.isf17( cmd );
    data->fn18 = wFunCmd.isf18( cmd );
    data->fn19 = wFunCmd.isf19( cmd );
    data->fn20 = wFunCmd.isf20( cmd );
    }
    if( wFunCmd.getgroup(cmd) == 0 || wFunCmd.getgroup(cmd) == 6 ) {
    data->fn21 = wFunCmd.isf21( cmd );
    data->fn22 = wFunCmd.isf22( cmd );
    data->fn23 = wFunCmd.isf23( cmd );
    data->fn24 = wFunCmd.isf24( cmd );
    }
    if( wFunCmd.getgroup(cmd) == 0 || wFunCmd.getgroup(cmd) == 7 ) {
    data->fn25 = wFunCmd.isf25( cmd );
    data->fn26 = wFunCmd.isf26( cmd );
    data->fn27 = wFunCmd.isf27( cmd );
    data->fn28 = wFunCmd.isf28( cmd );
    }
  }
  return False;
}



static void __restoreFx( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOLoc loc = (iOLoc)ThreadOp.getParm( th );
  iOLocData data = Data(loc);

  int fx = wLoc.getfx(data->props);
  int i = 0;

  ThreadOp.sleep(100 + 200 * data->fxsleep );

  if( wLoc.isrestorefx(data->props) ) {
    /* Test for restoring the lights function. */
    if( wLoc.isfn(data->props) ) {
      iONode vcmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "restoring lights for %s", wLoc.getid(data->props) );
      if ( wLoc.isrestorespeed(data->props) ) {
        wLoc.setV( vcmd, wLoc.getV(data->props) );
      }
      else {
        wLoc.setV( vcmd, 0 );
      }
      wLoc.setfn( vcmd, wLoc.isfn(data->props) );
      LocOp.cmd(loc, vcmd);
      ThreadOp.sleep(500);
    }

    for( i = 0; i < 28; i++ ) {
      int f = (1 << i);
      if( fx & f ) {
        iONode fcmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "restoring function %d for %s", i+1, wLoc.getid(data->props) );
        wFunCmd.setf0 ( fcmd, wLoc.isfn(data->props));
        wFunCmd.setfnchanged( fcmd, i + 1);
        switch( i ) {
          case 0 : wFunCmd.setf1 ( fcmd, True); break;
          case 1 : wFunCmd.setf2 ( fcmd, True); break;
          case 2 : wFunCmd.setf3 ( fcmd, True); break;
          case 3 : wFunCmd.setf4 ( fcmd, True); break;
          case 4 : wFunCmd.setf5 ( fcmd, True); break;
          case 5 : wFunCmd.setf6 ( fcmd, True); break;
          case 6 : wFunCmd.setf7 ( fcmd, True); break;
          case 7 : wFunCmd.setf8 ( fcmd, True); break;
          case 8 : wFunCmd.setf9 ( fcmd, True); break;
          case 9 : wFunCmd.setf10( fcmd, True); break;
          case 10: wFunCmd.setf11( fcmd, True); break;
          case 11: wFunCmd.setf12( fcmd, True); break;
          case 12: wFunCmd.setf13( fcmd, True); break;
          case 13: wFunCmd.setf14( fcmd, True); break;
          case 14: wFunCmd.setf15( fcmd, True); break;
          case 15: wFunCmd.setf16( fcmd, True); break;
          case 16: wFunCmd.setf17( fcmd, True); break;
          case 17: wFunCmd.setf18( fcmd, True); break;
          case 18: wFunCmd.setf19( fcmd, True); break;
          case 19: wFunCmd.setf20( fcmd, True); break;
          case 20: wFunCmd.setf21( fcmd, True); break;
          case 21: wFunCmd.setf22( fcmd, True); break;
          case 22: wFunCmd.setf23( fcmd, True); break;
          case 23: wFunCmd.setf24( fcmd, True); break;
          case 24: wFunCmd.setf25( fcmd, True); break;
          case 25: wFunCmd.setf26( fcmd, True); break;
          case 26: wFunCmd.setf27( fcmd, True); break;
          case 27: wFunCmd.setf28( fcmd, True); break;
        }
        LocOp.cmd(loc, fcmd);
        ThreadOp.sleep(500);
      }
    }
  }

  if( wLoc.getV(data->props) > 0 && wLoc.isrestorespeed(data->props) ) {
    iONode vcmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "restoring speed for %s", wLoc.getid(data->props) );
    wLoc.setV( vcmd, wLoc.getV(data->props) );
    wLoc.setfn( vcmd, wLoc.isfn(data->props) );
    LocOp.cmd(loc, vcmd);
  }


  ThreadOp.base.del(th);
}


static void __sysEvent( obj inst, iONode evtNode ) {
  iOLocData data = Data(inst);
  const char* cmd = wSysCmd.getcmd(evtNode);

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sysEvent [%s] for [%s]...", cmd, LocOp.getId((iOLoc)inst) );

  if( wLoc.isshow(data->props) && StrOp.equals( wSysCmd.go, cmd ) && !data->fxrestored ) {
    /* restore fx */
    data->fxrestored = True;
    data->fxsleep = wSysCmd.getval(evtNode);
    if( wLoc.isrestorefx(data->props) || wLoc.isrestorespeed(data->props) ) {
      iOThread th = ThreadOp.inst( NULL, &__restoreFx, inst );
      ThreadOp.start(th);
    }
  }
}


static int __getVfromRaw(void* inst, iONode evtNode) {
  iOLocData data = Data(inst);

  int spcnt = wLoc.getspcnt( data->props );
  int V = 0;
  int V_raw = wLoc.getV_raw(evtNode);
  int V_rawMax = wLoc.getV_rawMax(evtNode);

  if( V_raw != -1 ) {
    float fV = wLoc.getV_max( data->props ) * V_raw;
    float div = (V_rawMax == -1 ? spcnt:V_rawMax);
    if( StrOp.equals( wLoc.getV_mode( data->props ), wLoc.V_mode_percent ) )
      fV = 100 * V_raw;
    fV = fV / div;
    V = (int)fV;
    if( fV - V >= 0.5 )
      V++;
  }
  else {
    V = wLoc.getV( data->props );
    if( data->curSpeed > V )
      V = data->curSpeed;
  }
  return V;
}


static void _depart(iOLoc inst) {
  iOLocData data = Data(inst);

  if( data->curBlock != NULL ) {
    iIBlockBase curblock  = ModelOp.getBlock( AppOp.getModel(), data->curBlock );
    if( curblock != NULL ) {
      curblock->depart(curblock);
    }
  }
}


static void* __event( void* inst, const void* evt ) {
  iOLocData data = Data(inst);
  iONode evtNode = (iONode)evt;
  Boolean broadcast = False;

  if( evtNode == NULL )
    return NULL;

  if( data->go && !data->gomanual ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999,
        "ignore field event for [%s] while running in auto mode", wLoc.getid(data->props) );
    return NULL;
  }

  if( StrOp.equals( wLoc.name(), NodeOp.getName(evtNode) )) {
    int V = __getVfromRaw(inst, evtNode);
    int spcnt = wLoc.getspcnt( data->props );
    int V_raw = wLoc.getV_raw(evtNode);
    int V_rawMax = wLoc.getV_rawMax(evtNode);

    if( StrOp.equals( wLoc.direction, wLoc.getcmd(evtNode) ) || StrOp.equals( wLoc.dirfun, wLoc.getcmd(evtNode) ) ) {
      /* function and dir update */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lc=%s dir=%d fn=%d",
          wLoc.getid(data->props), wLoc.isdir(evtNode), wLoc.isfn(evtNode) );
      if( !data->go ) {
        wLoc.setdir( data->props, wLoc.isplacing(data->props) ? wLoc.isdir(evtNode):!wLoc.isdir(evtNode) );
        if( StrOp.equals( wLoc.dirfun, wLoc.getcmd(evtNode) ) ) {
          wLoc.setfn( data->props, wLoc.isfn(evtNode) );
          data->fn0 = wLoc.isfn(evtNode);
        }
      }
    }

    if( !data->go ) {
      wLoc.setV( data->props, V);
    }

    if( wCtrl.isallowzerothrottleid( AppOp.getIniNode( wCtrl.name() ) ) ||
        StrOp.len(wLoc.getthrottleid(evtNode)) > 0 && !StrOp.equals( "0", wLoc.getthrottleid(evtNode) ) ) {
      wLoc.setthrottleid( data->props, wLoc.getthrottleid(evtNode) );
      /* TODO: inform consist slaves */
      __checkConsist(inst, evtNode, True);
      broadcast = True;
    }
    else {
      /* this is an echo comming from the loconet reader; do not broadcast it */
      broadcast = False;
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lc=%s V_raw=%d V=%d fn=%d dir=%s throttleID=%s",
        wLoc.getid(data->props), V_raw, V, wLoc.isfn(data->props), wLoc.isdir(data->props)?"Forwards":"Reverse", wLoc.getthrottleid(data->props) );
    /* Broadcast to clients. */
    if( broadcast ) {
      iONode node = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setid( node, wLoc.getid( data->props ) );
      wLoc.setdir( node, wLoc.isdir( data->props ) );
      wLoc.setaddr( node, wLoc.getaddr( data->props ) );
      wLoc.setV( node, V );
      wLoc.setplacing( node, wLoc.isplacing( data->props ) );
      wLoc.setblockenterside( node, wLoc.isblockenterside( data->props ) );
      wLoc.setmode( node, wLoc.getmode( data->props ) );
      wLoc.setresumeauto( node, wLoc.isresumeauto(data->props) );
      wLoc.setmanual( node, data->gomanual );
      wLoc.setblockid( node, data->curBlock );
      wLoc.setdir( node, wLoc.isdir(data->props) );
      wLoc.setfn( node, wLoc.isfn(data->props) );
      wLoc.setruntime( node, wLoc.getruntime(data->props) );
      wLoc.setmtime( node, wLoc.getmtime(data->props) );
      wLoc.setmint( node, wLoc.getmint(data->props) );
      wLoc.setthrottleid( node, wLoc.getthrottleid(data->props) );
      wLoc.setactive( node, wLoc.isactive(data->props) );
      if( data->driver != NULL ) {
        wLoc.setscidx( node, data->driver->getScheduleIdx( data->driver ) );
        wLoc.setscheduleid(node, LocOp.getSchedule(inst, NULL));
      }

      AppOp.broadcastEvent( node );
    }

  }
  else if( StrOp.equals( wFunCmd.name(), NodeOp.getName(evtNode) )) {
    /* TODO: the digint library should provide the function group to prevent overwriting not reported functions */
    __cpNode2Fn(inst, evtNode);
    wLoc.setfn( data->props, data->fn0);

    if( wCtrl.isallowzerothrottleid( AppOp.getIniNode( wCtrl.name() ) ) ||
        StrOp.len(wLoc.getthrottleid(evtNode)) > 0 && !StrOp.equals( "0", wLoc.getthrottleid(evtNode) ) )
    {
      wLoc.setthrottleid( data->props, wLoc.getthrottleid(evtNode) );
      __checkConsist(inst, evtNode, True);
      broadcast = True;
    }
    else {
      /* this is an echo comming from the loconet reader; do not broadcast it */
      broadcast = False;
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lc=%s throttleid=%s f0=%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
        wLoc.getid( data->props ),
        wLoc.getthrottleid( data->props),
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
    if( broadcast ) {
      iONode node = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
      wFunCmd.setid( node, wLoc.getid( data->props ) );
      wFunCmd.setaddr( node, wLoc.getaddr( data->props ) );
      __cpFn2Node(inst, node, -1, 0);
      wFunCmd.setf0( node, wLoc.isfn(data->props) );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "broadcasting function command %d...", wFunCmd.isf0( node));
      AppOp.broadcastEvent( node );
    }
  }
  else if( StrOp.equals( wSysCmd.name(), NodeOp.getName(evtNode) ) ) {
    __sysEvent( inst, evtNode );
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

  ModelOp.removeSysEventListener( AppOp.getModel(), (obj)inst );

  /* wait for thread to stop. */
  while( data->running && retry < 10 ) {
    ThreadOp.sleep( 100 );
    retry++;
  };
  if( data->runner != NULL )
    ThreadOp.base.del(data->runner);
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


static int __translateVhint(iOLoc inst, const char* V_hint, int V_maxkmh ) {
  iOLocData data = Data(inst);
  int       V_new  = -1;

  int V_max = wLoc.getV_max( data->props );
  int V_mid = wLoc.getV_mid( data->props );
  int V_min = wLoc.getV_min( data->props );

  if( !wLoc.isdir(data->props) || (wLoc.isdir(data->props) && !wLoc.isplacing( data->props ) ) ){
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
  
  if(StrOp.equals( wLoc.V_mode_kmh, wLoc.getV_mode(data->props) ) && V_maxkmh > 0 ) {
    if( V_new > V_maxkmh ) {
      V_new = V_maxkmh;
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "reduce max. speed from %dKmh to %dKmh", V_new, V_maxkmh );
    }
  }

  return V_new;
}

/*return equal=0, lower=-1, higher=1*/
int _compareVhint(iOLoc inst, const char* V_hint) {
  iOLocData data  = Data(inst);
  int       V_new = __translateVhint( inst, V_hint, 0 );

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

static int __getFnAddr( iOLoc inst, int function, int* mappedfn) {
  iOLocData    data = Data(inst);

  iONode fundef = wLoc.getfundef( data->props );
  while( fundef != NULL ) {
    if( wFunDef.getfn(fundef) == function ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
          "function address for %d = %d:%d", function, wFunDef.getaddr(fundef), wFunDef.getmappedfn(fundef) );
      if( mappedfn != NULL ) {
        if( wFunDef.getmappedfn(fundef) > 0 )
          *mappedfn = wFunDef.getmappedfn(fundef);
        else
          *mappedfn = function;
      }
      return wFunDef.getaddr(fundef);
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
  int         V_maxkmh = 0;
  int         V_new  = -1;
  int         V_old  = wLoc.getV(data->props);
  iONode      cmdTD  = NULL;
  iONode      cmdFn  = NULL;
  static Boolean  f0changed = False;

  if( cmd != NULL )
  {
    V_new    = wLoc.getV( cmd );
    V_hint   = wLoc.getV_hint( cmd );
    V_maxkmh = wLoc.getV_maxkmh( cmd );

    if( NodeOp.findAttr(cmd,"dir") && wLoc.isdir(cmd) != wLoc.isdir( data->props ) ) {
      /* Informing the P50 interface. */
      wLoc.setsw( cmd, True );
      wLoc.setdir( data->props, wLoc.isdir(cmd) );
    __checkAction(inst, "dirchange");
    }
    else if( wLoc.issw( cmd ) ) {
      /* Could be generated by the switch button of the locdlg. */
      wLoc.setdir( cmd, !wLoc.isdir( data->props ) );
      wLoc.setdir( data->props, wLoc.isdir(cmd) );
    __checkAction(inst, "dirchange");
    }

    if( NodeOp.findAttr(cmd,"fn") ) {
      /* Informing the P50 interface. */
      wLoc.setfn( data->props, wLoc.isfn( cmd ) );
      if( data->fn0 != wLoc.isfn( cmd ) )
        f0changed = True;
      data->fn0 = wLoc.isfn( cmd );
      __checkAction(inst, "lights");
    }

    if( StrOp.equals( wFunCmd.name(), NodeOp.getName(cmd )) ) {

      int fnchanged = -1;

      wFunCmd.setaddr(cmd, wLoc.getaddr( data->props ));

      /* function timers
         when f0 is turned on, data->fn0 is set true above at informing the P50 interface, 
         so if data->fn0 is true and f0changed is true, fn0 is turned on and we must check for the function timer */
      if( data->fn0 && f0changed && wFunCmd.isf0( cmd ) )
        data->fxtimer[0] = __getFnTimer( inst, 0);
      if( (!data->fn0 && wFunCmd.isf0( cmd ) ) || (data->fn0 && !wFunCmd.isf0( cmd ) ) || f0changed ) {
        fnchanged = 0;
        cmdFn = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setdir( cmdFn, wLoc.isplacing(data->props) ? wLoc.isdir(data->props):!wLoc.isdir(data->props) );
        wLoc.setfn( cmdFn, wFunCmd.isf0( cmd ) );
        wLoc.setfncnt( cmdFn, wLoc.getfncnt( data->props ) );
        wLoc.setid( cmdFn, wLoc.getid(data->props) );
        wLoc.setoid( cmdFn, wLoc.getoid(data->props) );
        wLoc.setaddr( cmdFn, wLoc.getaddr(data->props) );
        wLoc.setiid( cmdFn, wLoc.getiid(data->props) );
        wLoc.setbus( cmdFn, wLoc.getbus(data->props) );
        wLoc.setV( cmdFn, wLoc.getV(data->props) );
        wLoc.setspcnt( cmdFn, wLoc.getspcnt( data->props ) );
        wLoc.setmass( cmdFn, wLoc.getmass( data->props ) );
        wLoc.setV_step( cmdFn, wLoc.getV_step( data->props ) );
        wLoc.setV_min( cmdFn, wLoc.getV_min( data->props ) );
        wLoc.setV_max( cmdFn, wLoc.getV_max( data->props ) );
        wLoc.setV_mode( cmdFn, wLoc.getV_mode( data->props ) );
        wLoc.setprot( cmdFn, wLoc.getprot( data->props ) );
        wLoc.setprotver( cmdFn, wLoc.getprotver( data->props ) );
        wLoc.setfncnt( cmdFn, wLoc.getfncnt( data->props ) );
        wFunCmd.setfnchanged(cmdFn, wFunCmd.getfnchanged(cmd));
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

      if( wFunCmd.getfnchanged(cmd) != -1 ) {
        /* use the fnchanged send from client */
        fnchanged = wFunCmd.getfnchanged(cmd);
      }

      wFunCmd.setfnchanged(cmd, fnchanged);

      if( data->timedfn >= 0 && wFunCmd.gettimedfn( cmd ) >= 0 && wFunCmd.gettimer( cmd ) > 0) {
        /* reset previous timed function */
        __resetTimedFunction(inst, cmd, -1);
      }
      if( wFunCmd.gettimedfn( cmd ) >= 0 && wFunCmd.gettimer( cmd ) > 0 ) {
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

      /* save the function status: */
      Boolean fon = __cpNode2Fn(inst, cmd);
      __saveFxState(inst);

      if( fnchanged != -1 && fon ) {
        char fstr[32] = {'\0'};
        StrOp.fmtb(fstr, "f%d", fnchanged );
        __checkAction(inst, fstr);
      }


      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "lc=%s [addr=%d] [fn=%d] lights=%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
          wLoc.getid( data->props ), cmdFn==NULL?wLoc.getaddr( cmd ):wFunCmd.getaddr(cmdFn), fnchanged,
          data->fn0  ? "on":"off",
          data->fn1  ? "01":"--", data->fn2  ? "02":"--", data->fn3  ? "03":"--", data->fn4  ? "04":"--",
          data->fn5  ? "05":"--", data->fn6  ? "06":"--", data->fn7  ? "07":"--", data->fn8  ? "08":"--",
          data->fn9  ? "09":"--", data->fn10 ? "10":"--", data->fn11 ? "11":"--", data->fn12 ? "12":"--",
          data->fn13 ? "13":"--", data->fn14 ? "14":"--", data->fn15 ? "15":"--", data->fn16 ? "16":"--",
          data->fn17 ? "17":"--", data->fn18 ? "18":"--", data->fn19 ? "19":"--", data->fn20 ? "20":"--",
          data->fn21 ? "21":"--", data->fn22 ? "22":"--", data->fn23 ? "23":"--", data->fn24 ? "24":"--",
          data->fn25 ? "25":"--", data->fn26 ? "26":"--", data->fn27 ? "27":"--", data->fn28 ? "28":"--"
      );


      /* secondary decoder check */
      if( wFunCmd.getfnchanged(cmd) != -1 ) {
        int mappedfn = 0;
        int decaddr = __getFnAddr(inst, wFunCmd.getfnchanged(cmd), &mappedfn );
        if( decaddr > 0 ) {
          int ifn = 0;
          wLoc.setaddr( cmdFn==NULL?cmd:cmdFn, decaddr > 0 ? decaddr:wLoc.getaddr(data->props) );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
              "function %d address=%d:%d", wFunCmd.getfnchanged(cmd), decaddr, mappedfn );
          /* reset */
          for( ifn = 0; ifn < 29; ifn++ )
            __FnOnOff(inst, ifn, False, cmdFn==NULL?cmd:cmdFn, False);
          __cpFn2Node(inst, cmdFn==NULL?cmd:cmdFn, -1, decaddr);
          wFunCmd.setfnchanged(cmdFn==NULL?cmd:cmdFn, mappedfn);
        }
      }




    }
  }

  else if( !LocOp.isAutomode(inst) || data->gomanual ) {
    if( wLoc.isinfo4throttle(data->props ) ) {
      data->infocheck++;
      if( data->infocheck > 10 ) {
        if( cmd == NULL ) {
          cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
          wLoc.setaddr( cmd, wLoc.getaddr(data->props) );
        }
        wLoc.setcmd( cmd, wLoc.info );
        data->infocheck = 0;
      }
    }
  }



  /* New speed attributes: */
  if( V_hint != NULL ) {
    __checkAction(inst, V_hint);
    V_new = __translateVhint( inst, V_hint, V_maxkmh );

    if( data->drvSpeed != V_new || StrOp.equals( wFunCmd.name(), NodeOp.getName(cmd )) ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "V_hint: [%s][%d maxkmh] = %d", V_hint, V_maxkmh, V_new );
      data->drvSpeed = V_new;
      wLoc.setV( data->props, V_new);
      wLoc.setV_hint( data->props, V_hint );
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


  /* check for run and stall event */
  if( V_old != data->drvSpeed ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "check function event (curV=%d drvV=%d)", V_old, data->drvSpeed );
    if( V_old == 0 ) {
      __funEvent(inst, NULL, run_event, 0);
      __checkAction(inst, "run");
    }
    if( data->drvSpeed == 0 ) {
      __funEvent(inst, NULL, stall_event, 0);
      __checkAction(inst, "stall");
    }
  }


  /* Check for simple decoders like "Maerklin Delta": */
  if( StrOp.equals( wLoc.V_mode_percent, wLoc.getV_mode( data->props ) ) &&
      wLoc.getV_step( data->props ) > 0 && !wLoc.isregulated( data->props ) )
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
    wLoc.setbus( cmd, wLoc.getbus( data->props ) );
    wLoc.setprot( cmd, wLoc.getprot( data->props ) );
    wLoc.setprotver( cmd, wLoc.getprotver( data->props ) );
    wLoc.setspcnt( cmd, wLoc.getspcnt( data->props ) );
    wLoc.setfncnt( cmd, wLoc.getfncnt( data->props ) );
    wLoc.setdir( cmd, wLoc.isdir( data->props ) );
    wLoc.setfn( cmd, data->fn0 );
    wLoc.setoid( cmd, wLoc.getoid(data->props) );
    wLoc.setid( cmd, wLoc.getid(data->props) );

    /* some controllers use this information because they make no diff between loc or fun cmd: */
    __cpFn2Node(inst, cmd, -1, 0);

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

      /*wLoc.setaddr( cmd, wLoc.getaddr( data->props ) );*/

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
    if( cmdFn != NULL ) {
      if( wLoc.getaddr( cmdFn ) == 0 && !StrOp.equals( wLoc.prot_A, wLoc.getprot( data->props ))) {
        wLoc.setaddr( cmdFn, wLoc.getaddr(data->props) );
      }
      ControlOp.cmd( control, cmdFn, NULL );
    }

    if( wLoc.getaddr( cmd ) == 0 && !StrOp.equals( wLoc.prot_A, wLoc.getprot( data->props ))) {
      wLoc.setaddr( cmd, wLoc.getaddr(data->props) );
    }

    ControlOp.cmd( control, cmd, NULL );

    if( cmdTD != NULL ) {
      ControlOp.cmd( control, cmdTD, NULL );
    }
  }

  if( wCtrl.isreleaseonidle( AppOp.getIniNode( wCtrl.name() )) ) {
    if( cmd == NULL && wLoc.getV(data->props) == 0 && !data->go && !data->released ) {
      /* Release loco? */
      cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setaddr(cmd, wLoc.getaddr(data->props));
      wLoc.setid(cmd, wLoc.getid(data->props));
      wLoc.setcmd(cmd, wLoc.release );
      ControlOp.cmd( control, cmd, NULL );
      data->released = True;
    }
  }

  data->step++;
}


static iONode __resetTimedFunction(iOLoc loc, iONode cmd, int function) {
  iOLocData data = Data(loc);
  iONode fncmd = cmd==NULL?NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE ):cmd;
  int timedfn = data->timedfn;
  int newtimedfn = wFunCmd.gettimedfn( cmd );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reset timed function %d,%d Lights=%d", timedfn, function, data->fn0 );

  if( function >= 0 ) {
    timedfn = function;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "function [%d] deactivated", timedfn );
    wFunCmd.settimedfn( cmd, -1 );
  }

  if( timedfn == newtimedfn ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "keep state of function [%d]; is same as new", timedfn );
    timedfn = -1;
  }

  if( function != -1 )
    wFunCmd.setfnchanged( fncmd, function );

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

  if( timedfn == 0 )
    wLoc.setfn(data->props, wFunCmd.isf0( fncmd ) );

  return fncmd;
}

static void __runner( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOLoc loc = (iOLoc)ThreadOp.getParm( th );
  iOLocData data = Data(loc);
  int   tick = 0;
  Boolean cnfgsend = False;
  Boolean loccnfg = wCtrl.isloccnfg( AppOp.getIniNode( wCtrl.name() ) );

  ThreadOp.sleep(500);
  ThreadOp.setDescription( th, wLoc.getdesc( data->props ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Runner for \"%s\" started.", LocOp.getId( loc ) );
  data->running = True;

  data->speedstep = wLoc.getV_step( data->props );

  data->runtime = wLoc.getruntime( data->props );

  if( wLoc.getstartupscid(data->props) != NULL && StrOp.len(wLoc.getstartupscid(data->props)) > 0 ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "startup schedule: %s", wLoc.getstartupscid(data->props) );
    LocOp.useSchedule( loc, wLoc.getstartupscid(data->props) );
  }

  do {
    iOMsg msg = (iOMsg)ThreadOp.getPost( th );
    obj    emitter = NULL;
    iONode fncmd   = NULL;
    iONode broadcast = NULL;

    int   i     = 0;
    int   event = -1;
    int   timer = 0;
    int   type  = 0;
    obj   udata = NULL;

    if( msg != NULL ) {
      emitter = MsgOp.getSender( msg );
      event   = MsgOp.getEvent( msg );
      timer   = MsgOp.getTimer( msg );
      type    = MsgOp.getUsrDataType( msg );
      udata   = MsgOp.getUsrData(msg);
      msg->base.del( msg );
    }

    if( data->driver != NULL ) {
      if( event == swap_event ) {
        iONode  cmd     = (iONode)udata;
        Boolean swap    = (type & 0x01 ? True:False);
        Boolean consist = (type & 0x02 ? True:False);
        TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "swap event %d ms", timer );

        /* The swap timer. */
        if( timer > 0 )
          ThreadOp.sleep( timer );

        /* The swap: */
        wLoc.setplacing( data->props, swap );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "placing for [%s] set to [%s]", wLoc.getid(data->props), wLoc.isplacing( data->props )?"FWD":"REV" );
        /* inform model to keep this setting in the occupancy file */
        ModelOp.setBlockOccupancy( AppOp.getModel(), data->curBlock, wLoc.getid(data->props), False, wLoc.isplacing( data->props) ? 1:2, wLoc.isblockenterside( data->props) ? 1:2, NULL );

        /* swap the block enter side flag to be able to use other direction routes */
        LocOp.swapBlockEnterSide(loc, NULL);

        if( !consist ) {
          /* only swap if this command did not come from a multiple unit loop */
          __swapConsist(loc, cmd);
        }
      }
      else {
        if( timer > 0 ) {
          if( type == 0 && wLoc.getevttimer(data->props) > 0 ) {
            timer = wLoc.getevttimer(data->props);
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "loc evttimer %d ms", timer );
          }
          else if( event == in_event ) {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "loc evttimer %d ms * %d %%", timer, wLoc.getent2incorr(data->props) );
            timer = timer * wLoc.getent2incorr(data->props) / 100;
            if( timer < 1 )
              timer = 1;
          }
          else {
            TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "loc evttimer %d ms", timer );
          }
          TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "timed event[%d] %d ms", event, timer );
          ThreadOp.sleep( timer );
        }
        data->driver->drive( data->driver, emitter, event );
      }
    }


    if( !cnfgsend && loccnfg ) {
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
        cnfgsend = True;
      }
    }

    /* this is approximately a second */
    if( tick % 10 == 0 && tick != 0 ) {
      if( data->drvSpeed > 0 || (!data->go && wLoc.getV(data->props) > 0) ) {
        data->runtime++;
        wLoc.setruntime( data->props, data->runtime );
      }

      for( i = 0; i < 28; i++ ) {
        if( data->fxtimer[i] > 0 ) {
          data->fxtimer[i]--;
          if( data->fxtimer[i] == 0 ) {
            fncmd = __resetTimedFunction(loc, NULL, i);
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
      wLoc.setblockenterside( broadcast, wLoc.isblockenterside( data->props ) );
      wLoc.setblockid(broadcast, data->curBlock );
      wLoc.setmode( broadcast, wLoc.getmode( data->props ) );
      wLoc.setresumeauto( broadcast, wLoc.isresumeauto(data->props) );
      wLoc.setmanual( broadcast, data->gomanual );
      wLoc.setruntime( broadcast, wLoc.getruntime(data->props) );
      wLoc.setmtime( broadcast, wLoc.getmtime(data->props) );
      wLoc.setmint( broadcast, wLoc.getmint(data->props) );
      wLoc.setthrottleid( broadcast, wLoc.getthrottleid(data->props) );
      wLoc.setactive( broadcast, wLoc.isactive(data->props) );
      if( data->driver != NULL ) {
        wLoc.setscidx( broadcast, data->driver->getScheduleIdx( data->driver ) );
        wLoc.setscheduleid(broadcast, LocOp.getSchedule(loc, NULL));
      }
      AppOp.broadcastEvent( broadcast );
    }
    else {
      /* call this function for updating velocity for unmanaged decoders */
      __engine( loc, NULL );
    }


    ThreadOp.sleep( 100 );
    tick++;
  } while( data->run && !ThreadOp.isQuit(th) );

  data->running = False;
}



static void __funEvent( iOLoc inst, const char* blockid, int evt, int timer ) {
  iOLocData data = Data(inst);

  iONode fundef = wLoc.getfundef( data->props );

  while( fundef != NULL ) {
    const char* onevent = wFunDef.getonevent( fundef );
    const char* offevent = wFunDef.getoffevent( fundef );
    int fn = wFunDef.getfn( fundef );
    iOStrTok  onblocks = StrTokOp.inst( wFunDef.getonblockid ( fundef ), ',' );
    iOStrTok offblocks = StrTokOp.inst( wFunDef.getoffblockid( fundef ), ',' );
    Boolean  isonevent = False;
    Boolean isoffevent = False;

    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "checking event for function \"%s\"", wFunDef.gettext(fundef) );

    if( blockid != NULL ) {
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
    }
    else {
      isonevent = True;
      isoffevent = True;
    }

    if( isonevent ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "onevent[%s] evt[%d]", onevent, evt );
      if( StrOp.equals( wFunDef.enter_block, onevent ) && evt == enter_event ||
          StrOp.equals( wFunDef.in_block   , onevent ) && evt == in_event    ||
          StrOp.equals( wFunDef.exit_block , onevent ) && evt == exit_event  ||
          StrOp.equals( wFunDef.run        , onevent ) && evt == run_event
         ) {
        iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "On Event for funcion %d.", fn );
        wFunCmd.setid( cmd, LocOp.getId( inst ) );
        __cpFn2Node(inst, cmd, -1, 0);
        __FnOnOff(inst, fn, True, cmd, True);
        LocOp.cmd( inst, cmd );
      }
    }

    if( isoffevent ) {
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "offevent[%s] evt[%d]", offevent, evt );
      if( StrOp.equals( wFunDef.enter_block, offevent ) && evt == enter_event ||
          StrOp.equals( wFunDef.in_block   , offevent ) && evt == in_event    ||
          StrOp.equals( wFunDef.exit_block , offevent ) && evt == exit_event  ||
          StrOp.equals( wFunDef.stall      , offevent ) && evt == stall_event
         ) {
        iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Off Event for funcion %d.", fn );
        wFunCmd.setid( cmd, LocOp.getId( inst ) );
        __cpFn2Node(inst, cmd, -1, 0);
        __FnOnOff(inst, fn, False, cmd, True);
        LocOp.cmd( inst, cmd );
      }
    }

    /* Cleanup */
    StrTokOp.base.del( onblocks );
    StrTokOp.base.del( offblocks );

    fundef = wLoc.nextfundef( data->props, fundef );
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "timedfn=%d fnevent=%d fnblock=%s evt=%d",
      data->timedfn, data->fnevent, data->fneventblock, evt );
  if( data->timedfn >= 0 && data->fnevent > 0 && data->fneventblock != NULL ) {
    if( blockid != NULL && StrOp.equals(data->fneventblock, blockid)) {
      if( data->fnevent == evt || data->fnevent == evt ) {
        iONode cmd = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Off Event for funcion %d.", data->timedfn );
        wFunCmd.setid( cmd, LocOp.getId( inst ) );
        __cpFn2Node(inst, cmd, -1, 0);
        __FnOnOff(inst, data->timedfn, False, cmd, True);
        LocOp.cmd( inst, cmd );
        StrOp.free( data->fneventblock );
        data->fneventblock = NULL;
        data->fnevent = 0;
      }
    }
  }
}

static void _event( iOLoc inst, obj emitter, int evt, int timer, Boolean forcewait ) {
  iOLocData data = Data(inst);
  if( data->runner != NULL ) {

    iOMsg msg = MsgOp.inst( emitter, evt );
    iIBlockBase block = (iIBlockBase)MsgOp.getSender(msg);
    const char* blockid = block->base.id( block );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "event %d from [%s], timer=%d", evt, blockid, timer );
    MsgOp.setTimer( msg, timer );
    MsgOp.setUsrData( msg, NULL, forcewait ? 1000:0 );
    ThreadOp.post( data->runner, (obj)msg );
    __funEvent(inst, blockid, evt, timer);
  }
}


/*
 ***** _Public functions.
 */
static const char* _getId( iOLoc inst ) {
  iOLocData data = Data(inst);
  return wLoc.getid( data->props );
}

static const char* _getEngine( iOLoc inst ) {
  iOLocData data = Data(inst);
  return wLoc.getengine( data->props );
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


static const char* _getPrevBlock( iOLoc inst ) {
  iOLocData data = Data(inst);
  return data->prevBlock;
}


static const char* _getDestination( iOLoc inst ) {
  iOLocData data = Data(inst);
  return data->destBlock;
}


static void _setCarCount( iOLoc inst, int carcount ) {
  iOLocData data = Data(inst);
  wLoc.setnrcars(data->props, carcount);
}


static void _resetPrevBlock( iOLoc inst ) {
  iOLocData data = Data(inst);
  data->prevBlock = NULL;
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

  if( data->curBlock != NULL && StrOp.len(data->curBlock) > 0 && !StrOp.equals(id, data->curBlock) || data->prevBlock == NULL ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "set previous block to [%s]", data->curBlock == NULL ? "":data->curBlock );
    data->prevBlock = data->curBlock;
  }
  data->curBlock = id;
  wLoc.setblockid( data->props, id );

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
    wLoc.setblockenterside( node, wLoc.isblockenterside( data->props ) );
    wLoc.setmode( node, wLoc.getmode( data->props ) );
    wLoc.setresumeauto( node, wLoc.isresumeauto(data->props) );
    wLoc.setmanual( node, data->gomanual );
    wLoc.setblockid( node, data->curBlock );
    wLoc.setdestblockid( node, data->destBlock );
    wLoc.setruntime( node, wLoc.getruntime(data->props) );
    wLoc.setmtime( node, wLoc.getmtime(data->props) );
    wLoc.setmint( node, wLoc.getmint(data->props) );
    wLoc.setthrottleid( node, wLoc.getthrottleid(data->props) );
    wLoc.setactive( node, wLoc.isactive(data->props) );
    if( data->driver != NULL ) {
      wLoc.setscidx( node, data->driver->getScheduleIdx( data->driver ) );
      wLoc.setscheduleid(node, LocOp.getSchedule(inst, NULL));
    }

    AppOp.broadcastEvent( node );
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
  wLoc.setblockenterside( node, wLoc.isblockenterside( data->props ) );
  wLoc.setmode( node, wLoc.getmode( data->props ) );
  wLoc.setresumeauto( node, wLoc.isresumeauto(data->props) );
  wLoc.setmanual( node, data->gomanual );
  wLoc.setdestblockid( node, destid );
  wLoc.setruntime( node, wLoc.getruntime(data->props) );
  wLoc.setmtime( node, wLoc.getmtime(data->props) );
  wLoc.setmint( node, wLoc.getmint(data->props) );
  wLoc.setthrottleid( node, wLoc.getthrottleid(data->props) );
  wLoc.setblockid( node, curid );
  wLoc.setactive( node, wLoc.isactive(data->props) );
  if( data->driver != NULL ) {
    wLoc.setscidx( node, data->driver->getScheduleIdx( data->driver ) );
    wLoc.setscheduleid(node, LocOp.getSchedule(inst, NULL));
  }
  AppOp.broadcastEvent( node );
}

static void _gotoBlock( iOLoc inst, const char* id ) {
  iOLocData data = Data(inst);
  iIBlockBase block = ModelOp.getBlock( AppOp.getModel(), id );
  if( block != NULL ) {
    data->gotoBlock = block->base.id(block);
    if( data->driver != NULL )
      data->driver->gotoblock( data->driver, data->gotoBlock );
  }
}

static void _useSchedule( iOLoc inst, const char* id ) {
  iOLocData data = Data(inst);
  if( data->driver != NULL ) {
    iONode schedule = ModelOp.getSchedule( AppOp.getModel(), id );
    if( schedule != NULL )
      data->driver->useschedule( data->driver, wSchedule.getid(schedule) );
    else {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Schedule [%s] not found; try for tour...", id );
      LocOp.useTour(inst, id);
    }
  }
}

static void _useTour( iOLoc inst, const char* id ) {
  iOLocData data = Data(inst);
  if( data->driver != NULL ) {
    iONode tour = ModelOp.getTour( AppOp.getModel(), id );
    if( tour != NULL )
      data->driver->usetour( data->driver, wTour.getid(tour) );
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Tour [%s] not found!", id );
  }
}

static const char* _getSchedule( iOLoc inst, int* scidx ) {
  iOLocData data = Data(inst);
  if( data->driver != NULL ) {
    const char* schedule = data->driver->getschedule( data->driver );
    if( schedule != NULL ) {
      if( scidx != NULL )
        *scidx = data->driver->getScheduleIdx(data->driver);
      return schedule;
    }
  }
  return "";
}


static void _setMode( iOLoc inst, const char* mode ) {
  iOLocData data = Data(inst);

  /* Only take over the new mode if it is different; Broadcast to clients. */
  if( !StrOp.equals( wLoc.getmode(data->props), mode ) ) {
    iONode node = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setmode(data->props, mode);

    wLoc.setid( node, wLoc.getid( data->props ) );
    wLoc.setaddr( node, wLoc.getaddr( data->props ) );
    wLoc.setdir( node, wLoc.isdir( data->props ) );
    wLoc.setfn( node, wLoc.isfn( data->props ) );
    wLoc.setV( node, data->drvSpeed );
    wLoc.setplacing( node, wLoc.isplacing( data->props ) );
    wLoc.setblockenterside( node, wLoc.isblockenterside( data->props ) );
    wLoc.setmode( node, wLoc.getmode( data->props ) );
    wLoc.setresumeauto( node, wLoc.isresumeauto(data->props) );
    wLoc.setmanual( node, data->gomanual );
    wLoc.setblockid( node, data->curBlock );
    wLoc.setruntime( node, wLoc.getruntime(data->props) );
    wLoc.setmtime( node, wLoc.getmtime(data->props) );
    wLoc.setmint( node, wLoc.getmint(data->props) );
    wLoc.setthrottleid( node, wLoc.getthrottleid(data->props) );
    wLoc.setactive( node, wLoc.isactive(data->props) );
    if( data->driver != NULL ) {
      wLoc.setscidx( node, data->driver->getScheduleIdx( data->driver ) );
      wLoc.setscheduleid(node, LocOp.getSchedule(inst, NULL));
    }

    AppOp.broadcastEvent( node );
  }
}

static void _goNet( iOLoc inst, const char* curblock, const char* nextblock, const char* nextroute ) {
  iOLocData data = Data(inst);
  wLoc.setresumeauto( data->props, False);
  data->curBlock = StrOp.dup(curblock); /* make a copy before it is freed up */
  data->goNet = True; /* signal that the current block is from the net */
  data->go = True;
  data->released = False;
  data->gomanual = False;
  if( data->driver != NULL )
    data->driver->goNet( data->driver, data->gomanual, curblock, nextblock, nextroute );
}

static Boolean _go( iOLoc inst ) {
  iOLocData data = Data(inst);

  if( data->driver != NULL && data->driver->isRun( data->driver ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "Loco [%s] is already running in auto mode; reset wait.", LocOp.getId(inst) );
    data->driver->gogo(data->driver);
    return False;
  }

  wLoc.setresumeauto( data->props, False);
  if( wLoc.isactive(data->props)) {
    if( data->curBlock != NULL && StrOp.len(data->curBlock) > 0 && ModelOp.isAuto( AppOp.getModel() ) ) {
      data->go = True;
      data->released = False;
      data->gomanual = False;
      if( data->driver != NULL )
        data->driver->go( data->driver, data->gomanual );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Loco [%s] cannot be started because it is not in a block.", LocOp.getId(inst) );
      return False;
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Loco [%s] is deactivated.", LocOp.getId(inst) );
    return False;
  }
  return True;
}

static void _stop( iOLoc inst, Boolean resume ) {
  iOLocData data = Data(inst);
  if( resume && data->go ) {
    wLoc.setresumeauto( data->props, True);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "resume flag set for [%s]", LocOp.getId(inst) );
  }

  data->go = False;
  data->released = False;

  if( data->driver != NULL )
    data->driver->stop( data->driver );
}

static void _stopNet( iOLoc inst ) {
  iOLocData data = Data(inst);
  data->go = False;
  if( data->driver != NULL ) {
    iONode broadcast = NULL;
    data->driver->stopNet( data->driver );
    data->driver->brake( data->driver );

    /* Broadcast to clients. */
    broadcast = (iONode)NodeOp.base.clone(data->props);
    wLoc.setV( broadcast, data->drvSpeed );
    AppOp.broadcastEvent( broadcast );
  }
}

static void _dispatch( iOLoc inst ) {
  iOLocData data = Data(inst);
  iOControl control = AppOp.getControl();

  iONode cmd = (iONode)NodeOp.base.clone(data->props);
  wLoc.setcmd( cmd, wLoc.dispatch );
  ControlOp.cmd( control, cmd, NULL );

}

static void _release( iOLoc inst, iONode cmd ) {
  iOLocData data = Data(inst);
  if( wLoc.getthrottleid( data->props ) == NULL || StrOp.len(wLoc.getthrottleid( data->props )) == 0 )
    return;
  if( wLoc.getthrottleid( cmd ) == NULL || StrOp.len(wLoc.getthrottleid( cmd )) == 0 )
    return;

  if( StrOp.equals(wLoc.getthrottleid( data->props ), wLoc.getthrottleid( cmd ) ) ) {
    wLoc.setthrottleid( data->props, "" );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %s is released from throttle %s", wLoc.getid(data->props), wLoc.getthrottleid( cmd ) );
  }
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
  data->released = False;
  if( data->driver != NULL )
    data->driver->go( data->driver, data->gomanual );
}


static void _brake( iOLoc inst ) {
  iOLocData data = Data(inst);
  data->brake = True;
  if( data->driver != NULL )
    data->driver->brake( data->driver );
}


static void __checkConsist( iOLoc inst, iONode nodeA, Boolean byEvent ) {
  iOLocData data = Data(inst);

  if( wLoc.isconsistcmd( nodeA ) ) {
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "skip check consist; command is issued from a consist context" );
    return;
  }

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

        wLoc.setconsistcmd( consistcmd, True );

        if( byEvent ) {
          int V = __getVfromRaw(inst, consistcmd);
          wLoc.setV(consistcmd, V);
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

static void __swapConsist( iOLoc inst, iONode cmd ) {
  iOLocData data = Data(inst);

  /* swap consist */
  if( StrOp.len( wLoc.getconsist(data->props) ) > 0 ) {
    iOStrTok  consist = StrTokOp.inst( wLoc.getconsist ( data->props ), ',' );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "swapping the consist member [%s]",
                   wLoc.getconsist(data->props) );

    while( StrTokOp.hasMoreTokens( consist ) ) {
      const char* tok = StrTokOp.nextToken( consist );
      iOLoc consistloc = ModelOp.getLoc( AppOp.getModel(), tok );
      if( consistloc != NULL ) {
        LocOp.swapPlacing( consistloc, cmd, True );
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

  if( wCtrl.isdisablesteal( AppOp.getIniNode( wCtrl.name() ) ) ) {
    if( wLoc.getthrottleid( nodeA ) != NULL && StrOp.len(wLoc.getthrottleid( nodeA)) > 0 ) {
      if( wLoc.getthrottleid( data->props ) != NULL && StrOp.len(wLoc.getthrottleid( data->props)) > 0 ) {
        if( !StrOp.equals(wLoc.getthrottleid( data->props ), wLoc.getthrottleid( nodeA)) ) {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
              "cmd from %s rejected because this loco is already controlled by %s",
              wLoc.getthrottleid( nodeA),
              wLoc.getthrottleid( data->props )
              );
          NodeOp.base.del(nodeA);
          return False;
        }
      }
    }
  }

  wLoc.setthrottleid( data->props, wLoc.getthrottleid(nodeA) );

  if( TraceOp.getLevel(NULL) & TRCLEVEL_USER1 ) {
    char* cmdstr = NodeOp.base.toString( nodeA );
    TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, cmdstr );
    StrOp.free( cmdstr );
  }

  if( cmd != NULL && !StrOp.equals( wLoc.direction, cmd )  && !StrOp.equals( wLoc.velocity, cmd ) && !StrOp.equals( wLoc.dirfun, cmd ) ) {
    Boolean broadcast = False;
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
        LocOp.resetPrevBlock(inst);
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
      LocOp.resetPrevBlock(inst);
    }
    else if( StrOp.equals( wLoc.reset, cmd ) ) {
      _reset( inst, False );
      LocOp.resetPrevBlock(inst);
    }
    else if( StrOp.equals( wLoc.softreset, cmd ) ) {
      _reset( inst, True );
      LocOp.resetPrevBlock(inst);
    }
    else if( StrOp.equals( wLoc.activate, cmd ) ) {
      wLoc.setactive(data->props, True);
      broadcast = True;
    }
    else if( StrOp.equals( wLoc.deactivate, cmd ) ) {
      wLoc.setactive(data->props, False);
      broadcast = True;
    }
    else if( StrOp.equals( wLoc.swap, cmd ) ) {
      LocOp.swapPlacing(inst, nodeA, False);
      broadcast = True;
    }
    else if( StrOp.equals( wLoc.blockside, cmd ) ) {
      LocOp.swapBlockEnterSide(inst, NULL);
      broadcast = True;
    }
    else if( StrOp.equals( wLoc.dispatch, cmd ) ) {
      _dispatch( inst );
    }
    else if( StrOp.equals( wLoc.release, cmd ) ) {
      _release(inst, nodeA);
      broadcast = True;
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
    else if( StrOp.equals( wLoc.usetour, cmd ) ) {
      const char* tourid = wLoc.gettourid( nodeA );
      LocOp.useTour( inst, tourid );
    }
    else if( StrOp.equals( wLoc.shortid, cmd ) ) {
      /* send short ID to command station */
      if( wLoc.isuseshortid(data->props) && wLoc.getshortid(data->props) != NULL &&
          StrOp.len(wLoc.getshortid(data->props)) > 0 )
      {
        iONode cmdNode = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setid( cmdNode, wLoc.getid(data->props) );
        wLoc.setaddr( cmdNode, wLoc.getaddr(data->props) );
        wLoc.setshortid( cmdNode, wLoc.getshortid(data->props) );
        wLoc.setthrottlenr( cmdNode, wLoc.getthrottlenr(data->props) );
        wLoc.setprot( cmdNode, wLoc.getprot( data->props ) );
        wLoc.setprotver( cmdNode, wLoc.getprotver( data->props ) );
        wLoc.setspcnt( cmdNode, wLoc.getspcnt( data->props ) );
        wLoc.setfncnt( cmdNode, wLoc.getfncnt( data->props ) );
        wLoc.setcmd( cmdNode, wLoc.shortid );
        ControlOp.cmd( control, cmdNode, NULL );
      }
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
    }


    if(broadcast) {
      nodeF = (iONode)NodeOp.base.clone( nodeA );
      /* Broadcast to clients. */
      wLoc.setid( nodeF, wLoc.getid( data->props ) );
      wLoc.setaddr( nodeF, wLoc.getaddr( data->props ) );
      wLoc.setdir( nodeF, wLoc.isdir( data->props ) );
      wLoc.setV( nodeF, data->drvSpeed );
      wLoc.setfn( nodeF, wLoc.isfn( data->props ) );
      wLoc.setfx( nodeF, wLoc.getfx( data->props ) );
      wLoc.setplacing( nodeF, wLoc.isplacing( data->props ) );
      wLoc.setblockenterside( nodeF, wLoc.isblockenterside( data->props ) );
      wLoc.setblockid( nodeF, data->curBlock );
      if( StrOp.equals( wLoc.blockside, cmd ) && data->curBlock != NULL )
        wLoc.setblockid( nodeF, data->curBlock );
      wLoc.setmode( nodeF, wLoc.getmode( data->props ) );
      wLoc.setresumeauto( nodeF, wLoc.isresumeauto(data->props) );
      wLoc.setmanual( nodeF, data->gomanual );
      wLoc.setruntime( nodeF, wLoc.getruntime(data->props) );
      wLoc.setmtime( nodeF, wLoc.getmtime(data->props) );
      wLoc.setmint( nodeF, wLoc.getmint(data->props) );
      wLoc.setthrottleid( nodeF, wLoc.getthrottleid(data->props) );
      wLoc.setactive( nodeF, wLoc.isactive(data->props) );
      AppOp.broadcastEvent( nodeF );
    }

    nodeA->base.del(nodeA);

    return True;
  }

  nodeF = (iONode)NodeOp.base.clone( nodeA );

  /* Inform Driver. */
  if( data->driver != NULL )
    data->driver->info( data->driver, nodeA );

  __engine( inst, nodeA );
  __checkConsist(inst, nodeF, False);

  /* Broadcast to clients. */
  wLoc.setid( nodeF, wLoc.getid( data->props ) );
  wLoc.setaddr( nodeF, wLoc.getaddr( data->props ) );
  wLoc.setdir( nodeF, wLoc.isdir( data->props ) );
  wLoc.setV( nodeF, data->drvSpeed );
  wLoc.setfn( nodeF, wLoc.isfn( data->props ) );
  wLoc.setplacing( nodeF, wLoc.isplacing( data->props ) );
  wLoc.setblockenterside( nodeF, wLoc.isblockenterside( data->props ) );
  wLoc.setblockid( nodeF, data->curBlock );
  wLoc.setmode( nodeF, wLoc.getmode( data->props ) );
  wLoc.setresumeauto( nodeF, wLoc.isresumeauto(data->props) );
  wLoc.setmanual( nodeF, data->gomanual );
  wLoc.setruntime( nodeF, wLoc.getruntime(data->props) );
  wLoc.setmtime( nodeF, wLoc.getmtime(data->props) );
  wLoc.setmint( nodeF, wLoc.getmint(data->props) );
  wLoc.setthrottleid( nodeF, wLoc.getthrottleid(data->props) );
  wLoc.setactive( nodeF, wLoc.isactive(data->props) );
  if( data->driver != NULL ) {
    wLoc.setscidx( nodeF, data->driver->getScheduleIdx( data->driver ) );
    wLoc.setscheduleid(nodeF, LocOp.getSchedule(inst, NULL));
  }
  AppOp.broadcastEvent( nodeF );

  return True;
}

/**
 * Checks for property changes.
 * todo: Range checking?
 */
static void _modify( iOLoc inst, iONode props ) {
  iOLocData data = Data(inst);

  /* Do not replace all attributes and child nodes in auto mode! */

  if( LocOp.isAutomode( inst ) ) {
    /* only replace none destructive attributes */
    if(NodeOp.findAttr(props, "secondnextblock"))
      wLoc.setsecondnextblock( data->props, wLoc.issecondnextblock(props) );
    if(NodeOp.findAttr(props, "len"))
      wLoc.setlen( data->props, wLoc.getlen(props) );
    if(NodeOp.findAttr(props, "shortin"))
      wLoc.setshortin( data->props, wLoc.isshortin(props) );
    if(NodeOp.findAttr(props, "inatpre2in"))
      wLoc.setinatpre2in( data->props, wLoc.isinatpre2in(props) );
    if(NodeOp.findAttr(props, "V_max"))
      wLoc.setV_max( data->props, wLoc.getV_max(props) );
    if(NodeOp.findAttr(props, "V_mid"))
      wLoc.setV_mid( data->props, wLoc.getV_mid(props) );
    if(NodeOp.findAttr(props, "V_min"))
      wLoc.setV_min( data->props, wLoc.getV_min(props) );
    if(NodeOp.findAttr(props, "V_Rmax"))
      wLoc.setV_Rmax( data->props, wLoc.getV_Rmax(props) );
    if(NodeOp.findAttr(props, "V_Rmid"))
      wLoc.setV_Rmid( data->props, wLoc.getV_Rmid(props) );
    if(NodeOp.findAttr(props, "V_Rmin"))
      wLoc.setV_Rmin( data->props, wLoc.getV_Rmin(props) );
    if(NodeOp.findAttr(props, "trysamedir"))
      wLoc.settrysamedir( data->props, wLoc.istrysamedir(props) );
    if(NodeOp.findAttr(props, "tryoppositedir"))
      wLoc.settryoppositedir( data->props, wLoc.istryoppositedir(props) );
    if(NodeOp.findAttr(props, "forcesamedir"))
      wLoc.setforcesamedir( data->props, wLoc.isforcesamedir(props) );
    if(NodeOp.findAttr(props, "desc"))
      wLoc.setdesc( data->props, wLoc.getdesc(props) );
    if(NodeOp.findAttr(props, "rmark"))
      wLoc.setremark( data->props, wLoc.getremark(props) );
    if(NodeOp.findAttr(props, "catnr"))
      wLoc.setcatnr( data->props, wLoc.getcatnr(props) );
    if(NodeOp.findAttr(props, "fncnt"))
      wLoc.setfncnt( data->props, wLoc.getfncnt(props) );
    if(NodeOp.findAttr(props, "cargo"))
      wLoc.setcargo( data->props, wLoc.getcargo(props) );
    if(NodeOp.findAttr(props, "engine"))
      wLoc.setengine( data->props, wLoc.getengine(props) );
    if(NodeOp.findAttr(props, "consist"))
      wLoc.setconsist( data->props, wLoc.getconsist(props) );
  }
  else {
    int cnt = NodeOp.getAttrCnt( props );
    int i = 0;

    for( i = 0; i < cnt; i++ ) {
      iOAttr attr = NodeOp.getAttr( props, i );
      const char* name  = AttrOp.getName( attr );
      const char* value = AttrOp.getVal( attr );

      if( StrOp.equals("id", name) && StrOp.equals( value, wLoc.getid(data->props) ) )
        continue; /* skip to avoid making invalid pointers */

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
  }

  data->secondnextblock = wLoc.issecondnextblock( data->props );

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)props->base.clone( props );
    wLoc.setcmd(clone, wModelCmd.modify );
    AppOp.broadcastEvent( clone );
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
static long _getIdent( iOLoc loc ) {
  iOLocData data = Data(loc);
  return wLoc.getidentifier( data->props );
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
    wLoc.setcmd( clone, wModelCmd.modify );
    AppOp.broadcastEvent( clone );
  }
}

/**
 * swap placing to run in defaults routes after reaching an terminal station
 */
static void _swapPlacing( iOLoc loc, iONode cmd, Boolean consist ) {
  iOLocData data = Data(loc);

  Boolean swap = wLoc.isplacing( cmd );

  if( data->runner != NULL ) {
    iOMsg msg = MsgOp.inst( NULL, swap_event );
    MsgOp.setTimer( msg, wLoc.getswaptimer(data->props) );
    MsgOp.setEvent( msg, swap_event );
    if( cmd == NULL || !NodeOp.findAttr(cmd, "placing"))
      swap = !wLoc.isplacing( data->props );
    if( cmd == NULL )
      MsgOp.setUsrData(msg, NULL, (swap ? 0x01:0x00) | (consist ? 0x02:0x00) );
    else
      MsgOp.setUsrData(msg, (iONode)NodeOp.base.clone(cmd), (swap ? 0x01:0x00) | (consist ? 0x02:0x00) );

    ThreadOp.post( data->runner, (obj)msg );
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


static const char* _getV_hint( iOLoc loc ) {
  iOLocData data = Data(loc);
  return wLoc.getV_hint( data->props );
}


static Boolean _isAutomode( iOLoc loc ) {
  iOLocData data = Data(loc);
  return data->go;
}


static Boolean _isResumeAutomode( iOLoc loc ) {
  iOLocData data = Data(loc);
  return wLoc.isresumeauto( data->props );
}


static Boolean _isShortin( iOLoc loc ) {
  iOLocData data = Data(loc);
  return wLoc.isshortin( data->props );
}


static Boolean _isUseManualRoutes( iOLoc loc ) {
  iOLocData data = Data(loc);
  return wLoc.isusemanualroutes( data->props );
}


static Boolean _matchIdent( iOLoc loc, long ident ) {
  iOLocData data = Data(loc);
  Boolean match = False;

  if( wLoc.getidentifier( data->props ) == ident )
    match = True;
  else {
    /* check consist */
    iOStrTok  consist = StrTokOp.inst( wLoc.getconsist ( data->props ), ',' );
    while( StrTokOp.hasMoreTokens( consist ) ) {
      const char* tok = StrTokOp.nextToken( consist );
      iOLoc consistloc = ModelOp.getLoc( AppOp.getModel(), tok );
      if( consistloc != NULL ) {
        if( ident == LocOp.getIdent(consistloc) ) {
          match = True;
          break;
        }
      }
    };
    StrTokOp.base.del( consist );
  }

  return match;
}


static iONode _getFunctionStatus( iOLoc loc, iONode cmd ) {
  iOLocData data = Data(loc);
  /* save the function status: */
  __cpFn2Node(loc, cmd, -1, 0);
  wFunCmd.setf0( cmd, wLoc.isfn(data->props) );
  return cmd;
}


static int _getCarCount( iOLoc loc ) {
  iOLocData data = Data(loc);
  /* save the function status: */
  return wLoc.getnrcars(data->props);
}


static Boolean _getBlockEnterSide( iOLoc loc ) {
  iOLocData data = Data(loc);
  return wLoc.isblockenterside(data->props);
}


static void _setBlockEnterSide( iOLoc loc, Boolean enterside, const char* blockId ) {
  iOLocData data = Data(loc);

  wLoc.setblockenterside(data->props, enterside);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "block[%s] enter side for [%s] set to [%s]",
      blockId!=NULL?blockId:"-", wLoc.getid(data->props), wLoc.isblockenterside( data->props )?"+":"-" );
  ModelOp.setBlockOccupancy( AppOp.getModel(), data->curBlock, wLoc.getid(data->props), False, wLoc.isplacing( data->props) ? 1:2, wLoc.isblockenterside( data->props) ? 1:2, NULL );

  /* Broadcast to clients. */
  {
    iONode node = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    wLoc.setid( node, wLoc.getid( data->props ) );
    wLoc.setdir( node, wLoc.isdir( data->props ) );
    wLoc.setaddr( node, wLoc.getaddr( data->props ) );
    wLoc.setV( node, data->drvSpeed );
    wLoc.setfn( node, wLoc.isfn( data->props ) );
    wLoc.setplacing( node, wLoc.isplacing( data->props ) );
    wLoc.setblockenterside( node, wLoc.isblockenterside( data->props ) );
    wLoc.setblockid( node, data->curBlock );
    wLoc.setmode( node, wLoc.getmode( data->props ) );
    wLoc.setresumeauto( node, wLoc.isresumeauto(data->props) );
    wLoc.setmanual( node, data->gomanual );
    if( blockId != NULL )
      wLoc.setdestblockid(node, blockId );
    else
      wLoc.setdestblockid( node, data->curBlock );
    wLoc.setruntime( node, wLoc.getruntime(data->props) );
    wLoc.setmtime( node, wLoc.getmtime(data->props) );
    wLoc.setmint( node, wLoc.getmint(data->props) );
    wLoc.setthrottleid( node, wLoc.getthrottleid(data->props) );
    wLoc.setactive( node, wLoc.isactive(data->props) );
    if( data->driver != NULL ) {
      wLoc.setscidx( node, data->driver->getScheduleIdx( data->driver ) );
      wLoc.setscheduleid(node, LocOp.getSchedule(loc, NULL));
    }

    AppOp.broadcastEvent( node );
  }
}


static void _swapBlockEnterSide( iOLoc loc, const char* blockId ) {
  iOLocData data = Data(loc);
  LocOp.setBlockEnterSide(loc, !wLoc.isblockenterside(data->props), blockId );
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
  data->released = True;

  if( wRocRail.isresetspfx(AppOp.getIni()) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "global reset speed and functions for loco [%s]", wLoc.getid(props));
    wLoc.setV( data->props, 0 );
    wLoc.setfx( data->props, 0 );
    wLoc.setfn( data->props, False );
  }
  else {
    /* reset velocity to zero */
    if( !wLoc.isrestorespeed(data->props)) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no restore wanted: reset speed for loco [%s]", wLoc.getid(props));
      wLoc.setV( data->props, 0 );
    }
    if( !wLoc.isrestorefx(data->props)) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no restore wanted: reset functions for loco [%s]", wLoc.getid(props));
      wLoc.setfx( data->props, 0 );
    }
  }

  data->fn0 = wLoc.isfn(data->props);
  wLoc.setthrottleid( data->props, "" );

  __initCVmap( loc );

  ModelOp.addSysEventListener( AppOp.getModel(), (obj)loc );

  /*data->driver = (iILcDriverInt)LcDriverOp.inst( loc );*/
  if( wLoc.isshow(data->props) && __loadDriver( loc ) ) {
    data->runner = ThreadOp.inst( _getId(loc), &__runner, loc );
    data->run = True;
    ThreadOp.start( data->runner );
  }
  else if(!wLoc.isshow(data->props)) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "loco [%s] is invisible; no runner started", wLoc.getid(props));
  }

  instCnt++;

  return loc;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/loc.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
