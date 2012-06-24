/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */
#ifndef __ROCS_WUTILS_H
#define __ROCS_WUTILS_H

#include "rocs/public/rocs.h"
#include "rocs/public/mem.h"

/* C++ */
#ifdef __cplusplus
  extern "C" {
#endif

/* 
 * wUtils.
 *----------------------------------------*/
struct __wUtils {
  Boolean (*checkAttrRange)(const char* nodeName,const char* attrname,const char* vartype,const char* range,const char* valstr );
};

/* Object funktion struct: global; defined in str.c */
extern struct __wUtils wUtils;


/* C++ */
#ifdef __cplusplus
  }
#endif

#endif
