/*
 Rocs - OS independent C library

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#ifdef _WIN32

#include "rocs/impl/js_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/thread.h"
#include "rocs/public/map.h"
#include "rocs/public/objbase.h"
#include "rocs/public/str.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <windows.h>
#include <mmsystem.h>

/*
typedef struct joyinfoex_tag { 
    DWORD dwSize; 
    DWORD dwFlags; 
    DWORD dwXpos; 
    DWORD dwYpos; 
    DWORD dwZpos; 
    DWORD dwRpos; 
    DWORD dwUpos; 
    DWORD dwVpos; 
    DWORD dwButtons;
    DWORD dwButtonNumber; 
    DWORD dwPOV; 
    DWORD dwReserved1; 
    DWORD dwReserved2; 
} JOYINFOEX; 
*/

static unsigned int __buttons[2];
static unsigned int __buttonsOn[2];
static unsigned int __buttonsOff[2];
static unsigned int __pov[2];
static int __lastpov[2];
static int __lastX[2];
static int __lastY[2];

int rocs_js_init( iOJSData data, int* devicemap ) {
  Boolean dev0, dev1;
  int cnt = 0;
  JOYINFOEX joyInfoEx;
  MemOp.set( &joyInfoEx, 0, sizeof(joyInfoEx) );
  joyInfoEx.dwSize = sizeof(joyInfoEx);
  
  __buttons[0] = 0;
  __buttons[1] = 0;
  __buttonsOn[0] = 0;
  __buttonsOn[1] = 0;
  __buttonsOff[0] = 0;
  __buttonsOff[1] = 0;
  __pov[0] = 0;
  __pov[1] = 0;
  __lastpov[0] = 0;
  __lastpov[1] = 0;
  __lastX[0] = 0;
  __lastX[1] = 0;
  __lastY[0] = 0;
  __lastY[1] = 0;

  dev0 = (joyGetPosEx(JOYSTICKID1, &joyInfoEx) == JOYERR_NOERROR)?True:False;
  dev1 = (joyGetPosEx(JOYSTICKID2, &joyInfoEx) == JOYERR_NOERROR)?True:False;
  if( dev0 ) {
    *devicemap |= 0x01 << 0;
    cnt++;
  }
  if( dev1 ) {
    *devicemap |= 0x01 << 1;
    cnt++;
  }
  data->devcnt = cnt;
  return cnt;
}

/*
typedef struct joyinfoex_tag { 
    DWORD dwSize; 
    DWORD dwFlags; 
    DWORD dwXpos; 
    DWORD dwYpos; 
    DWORD dwZpos; 
    DWORD dwRpos; 
    DWORD dwUpos; 
    DWORD dwVpos; 
    DWORD dwButtons; 
    DWORD dwButtonNumber; 
    DWORD dwPOV; 
    DWORD dwReserved1; 
    DWORD dwReserved2; 
} JOYINFOEX; 
*/

Boolean rocs_js_read( iOJSData data, int devnr, int* type, int* number, int* value, unsigned long* msec) {
  JOYINFOEX joyInfoEx;
  MemOp.set( &joyInfoEx, 0, sizeof(joyInfoEx) );
  joyInfoEx.dwSize = sizeof(joyInfoEx);
  joyInfoEx.dwFlags = JOY_RETURNALL;

  *type   = 0;
  *number = 0;
  *value  = 0;
  *msec   = 0;
  
  if( devnr >= 0 && devnr < 2 ) {
    int devname = (devnr == 0 ? JOYSTICKID1:JOYSTICKID2);
    
    if( joyGetPosEx(devname, &joyInfoEx) != JOYERR_NOERROR)
      return False;
      
    if( __buttons[devnr] != joyInfoEx.dwButtons ) {
      unsigned int mask = 0;
      unsigned int div = __buttons[devnr] ^ joyInfoEx.dwButtons;
      int i  = 0;
      for( i = 0; i < 32; i++ ) {
        mask |= 0x0001 << i;
        if( div & 0x0001 ) {
          Boolean newevent = False;
          unsigned int button = 0x0001 << i;

          int on = ( joyInfoEx.dwButtons & button ) ? 1:0;

          /* check for already reported: */
          if( on == 0 ) {
            if( __buttonsOff[devnr] & button )
              continue;
            else {
              __buttonsOff[devnr] |= button;
              __buttonsOn[devnr]  &= !button;
              newevent = True;
            }
          }
          else if( on == 1 ) {
            if( __buttonsOn[devnr] & button )
              continue;
            else {
              __buttonsOn[devnr]  |= button;
              __buttonsOff[devnr] &= !button;
              newevent = True;
            }
          }

          *number = i;
          *type = 1;
          *value = on;
          if( newevent ) {
            printf( " button 0x%08x on 0x%08x off 0x%08x\n", 
              button, __buttonsOn[devnr], __buttonsOff[devnr] );
            break;
          }
        }
        div = div >> 1;
      }
      __buttons[devnr] = (joyInfoEx.dwButtons & mask) | (__buttons[devnr] & !mask);
    }

    if ( __pov[devnr] != joyInfoEx.dwPOV ) {
      if( joyInfoEx.dwPOV == 0xffff ) {
        *value = 0;
        *number = __lastpov[devnr];
      }
      else if( joyInfoEx.dwPOV == 0x0000 ) {
        *value = -1;
        *number = 5;
      }
      else if( joyInfoEx.dwPOV == 0x4650 ) {
        *value = 1;
        *number = 5;
      }
      else if( joyInfoEx.dwPOV == 0x6978 ) {
        *value = -1;
        *number = 4;
      }
      else if( joyInfoEx.dwPOV == 0x2328 ) {
        *value = 1;
        *number = 4;
      }
      __lastpov[devnr] = *number;
      
      *type = 2;
      __pov[devnr] = joyInfoEx.dwPOV;
    }
    
    else if(joyInfoEx.dwXpos != 0x7fff && __lastX[devnr] != joyInfoEx.dwXpos ) {
      __lastX[devnr] = joyInfoEx.dwXpos;
      if(joyInfoEx.dwXpos == 0x00ff) {
        *value = -1;
        *number = 4;
      }
      else if(joyInfoEx.dwXpos == 0xffff) {
        *value = 1;
        *number = 4;
      }
      
      *type = 2;
    }
    
    else if(joyInfoEx.dwYpos != 0x7fff && __lastY[devnr] != joyInfoEx.dwYpos) {
      __lastY[devnr] = joyInfoEx.dwYpos;
      if(joyInfoEx.dwYpos == 0x00ff) {
        *value = -1;
        *number = 5;
      }
      else if(joyInfoEx.dwYpos == 0xffff) {
        *value = 1;
        *number = 5;
      }
      
      *type = 2;
    }
    
    else if(joyInfoEx.dwXpos == 0x7fff && __lastX[devnr] != 0x7fff ) {
      if(__lastX[devnr] == 0x00ff) {
        *value = 0;
        *number = 4;
      }
      else if(__lastX[devnr] == 0xffff) {
        *value = 0;
        *number = 4;
      }
      
      *type = 2;
      __lastX[devnr] = joyInfoEx.dwXpos;
    }
    
    else if(joyInfoEx.dwYpos == 0x7fff && __lastY[devnr] != 0x7fff) {
      if(__lastY[devnr] == 0x00ff) {
        *value = 0;
        *number = 5;
      }
      else if(__lastY[devnr] == 0xffff) {
        *value = 0;
        *number = 5;
      }
      
      *type = 2;
      __lastY[devnr] = joyInfoEx.dwYpos;
    }
    
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "bn0x%x bt0x%x p0x%x x0x%x y0x%x z0x%x r0x%x u0x%x v0x%x\n", 
      joyInfoEx.dwButtonNumber, joyInfoEx.dwButtons, joyInfoEx.dwPOV,
      joyInfoEx.dwXpos, joyInfoEx.dwYpos, joyInfoEx.dwZpos,
      joyInfoEx.dwRpos, joyInfoEx.dwUpos, joyInfoEx.dwVpos );
      
  }

  return True;
}

#endif
