/** ------------------------------------------------------------
  * $Source: /home/cvs/xspooler/rocs/impl/os2/ojs.c,v $
  * $Author: rob $
  * $Date: 2006-02-23 14:20:09 $
  * $Revision: 1.1 $
  * $Name:  $
  * ------------------------------------------------------------ */
#if defined __OS2__

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

int rocs_js_init( iOJSData data ) {
  return 0;
}


Boolean rocs_js_read( iOJSData data, int devnr, int* type, int* number, int* value, unsigned long* msec) {
  return False;
}

#endif
