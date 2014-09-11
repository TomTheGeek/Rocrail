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


#include "rocrail/impl/var_impl.h"

#include "rocrail/public/action.h"
#include "rocrail/public/app.h"
#include "rocrail/public/model.h"

#include "rocrail/wrapper/public/ActionCtrl.h"
#include "rocrail/wrapper/public/Variable.h"

#include "rocs/public/mem.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
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

/** ----- OVar ----- */


/**  */
static void _checkActions( iONode var ) {
  iOModel model = AppOp.getModel();
  iONode actionctrl = wVariable.getactionctrl( var );

  /* loop over all actions */
  while( actionctrl != NULL ) {
    if( wVariable.getvalue(var) == atoi(wActionCtrl.getstate(actionctrl)) ) {
      iOAction action = ModelOp.getAction(model, wActionCtrl.getid( actionctrl ));
      if( action != NULL )
        ActionOp.exec(action, actionctrl);

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "action with value=%d", wVariable.getvalue(var) );
    }

    actionctrl = wVariable.nextactionctrl( var, actionctrl );
  }
  return;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/var.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
