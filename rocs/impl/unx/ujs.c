/*
 Rocs - OS independent C library

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
#if defined __linux__

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

#include <linux/joystick.h>
#include <fcntl.h>

int rocs_js_init( iOJSData data, int* devicemap ) {
  unsigned char axes = 0;
  unsigned char buttons = 0;
  int version = 0;
  char name[128] = "\0";
  int devs, cnt = 0;

  *devicemap = 0;

  for (devs = 0; devs < 4; devs++) {
    char* jsdevname = StrOp.fmt( "/dev/js%d", devs );
    char* jsdevname_alt = StrOp.fmt( "/dev/input/js%d", devs );
    if( data->jsfd[devs] > 0 )
      close( data->jsfd[devs] );
    data->jsfd[devs] = 0;
    if( ( data->jsfd[devs] = open( jsdevname, O_RDONLY|O_NONBLOCK ) ) < 0 ) {
      //TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "JS Device \"%s\" not available", jsdevname );
      StrOp.free( jsdevname ); jsdevname = NULL;
      data->jsfd[devs] = 0;
      if( ( data->jsfd[devs] = open( jsdevname_alt, O_RDONLY|O_NONBLOCK ) ) < 0 ) {
        //TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "JS Device \"%s\" not available", jsdevname_alt );
        StrOp.free( jsdevname_alt ); jsdevname_alt = NULL;
        data->jsfd[devs] = 0;
        continue;
      }
    }

    {
      ioctl (data->jsfd[devs], JSIOCGVERSION, &version);
      ioctl (data->jsfd[devs], JSIOCGAXES, &axes);
      ioctl (data->jsfd[devs], JSIOCGBUTTONS, &buttons);
      ioctl (data->jsfd[devs], JSIOCGNAME (128), name);

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "JS %d(%s) has %d axes and %d buttons. Driver version is %d.%d.%d.",
        devs + 1, name, axes, buttons, version >> 16, (version >> 8) & 0xff, version & 0xff);

      *devicemap |= 0x01 << devs;
      cnt++;
    }
    StrOp.free( jsdevname );
    StrOp.free( jsdevname_alt );
  }
  data->devcnt = cnt;

  return cnt;
}


Boolean rocs_js_read( iOJSData data, int devnr, int* type, int* number, int* value, ulong* msec) {
  struct js_event js;

  *type   = 0;
  *number = 0;
  *value  = 0;
  *msec   = 0L;

  if( data->jsfd[devnr] > 0 ) {

    if( read ( data->jsfd[devnr], &js, sizeof (struct js_event) ) != sizeof (struct js_event) ) {
      if( errno != EAGAIN ) {
        TraceOp.terrno( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, errno, "Error reading Joystick %d", devnr );
        close( data->jsfd[devnr] );
        data->jsfd[devnr] = 0;
        data->devcnt--;
      }
      return False;
    }

    if( js.type & JS_EVENT_INIT ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "handle=%d, devnr=%d JS_EVENT_INIT",
          data->jsfd[devnr], devnr );
      return False;
    }

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "handle=%d, devnr=%d, type=%d, number=%d, value=%d, msec=%ld",
        data->jsfd[devnr], devnr, js.type, js.number, js.value, js.time );
    *type   = js.type;
    *number = js.number;
    *value  = js.value;
    *msec   = js.time;
    return True;
  }
  return False;
}
#else
/* dummy functions for not supported OS's: */
int rocs_js_init( void* data, int* devicemap ) {
  return 0;
}
int rocs_js_read( void* data, int devnr, int* type, int* number, int* value, unsigned long* msec) {
  return 0;
}
#endif
