/** ------------------------------------------------------------
  * $Source: /cvsroot/rojav/rocs/public/objbase.h,v $
  * $Author: robvrs $
  * $Date: 2004/02/27 08:12:39 $
  * $Revision: 1.7 $
  * $Name:  $
  * ------------------------------------------------------------ */
#ifndef __ROCS_OBJBASE_H
#define __ROCS_OBJBASE_H

/* C++ */
#ifdef __cplusplus
  extern "C" {
#endif

#include "rocs/public/rocs.h"
#include <stdlib.h>
/* 
 * obj.
 *----------------------------------------*/
typedef struct OBase {
  /* data pointer */
  void* data;
  /* dtor */
  void           (*del        )(void* inst);
  /* base operations */
  const char*    (*name       )(void);
  unsigned char* (*serialize  )(void* inst,long* size);
  void           (*deserialize)(void* inst,unsigned char* bytestream);
  char*          (*toString   )(void* inst);
  int            (*count      )(void);
  struct OBase*  (*clone      )(void* inst);
  Boolean        (*equals     )(void* inst1, void* inst2);
  void*          (*properties )(void* inst);
  const char*    (*id         )(void* inst);
  /* the event data, evt, is read only */
  void*          (*event      )(void* inst, const void* evt);
} *iOBase, *obj;

/* Helper function for getting the data. */
static void* bData( void* p ) { return ((iOBase)p)->data; }

/* Base operation which can be used as placeholders. 
   But much better: implement those object specific. */
static iOBase  _bclone ( void* inst ) { return (iOBase)NULL; }
static Boolean _bequals( void* inst1, void* inst2 ) { return False; }
static void*   _bproperties( void* inst1 ) { return NULL; }

/* C++ */
#ifdef __cplusplus
  }
#endif

#endif
