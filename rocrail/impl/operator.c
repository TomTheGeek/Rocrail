/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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


#include "rocrail/impl/operator_impl.h"

#include "rocrail/public/app.h"
#include "rocrail/public/car.h"
#include "rocrail/public/model.h"

#include "rocs/public/mem.h"
#include "rocs/public/strtok.h"

#include "rocrail/wrapper/public/Operator.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOOperatorData data = Data(inst);
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
  iOOperatorData data = Data((iOOperator)inst);
  return data->props;
}

static const char* __id( void* inst ) {
  iOOperatorData data = Data((iOOperator)inst);
  return wOperator.getid(data->props);
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OOperator ----- */


/**  */
static char* _getForm( void* object ) {
  return 0;
}


/**  */
static char* _postForm( void* object ,const char* data ) {
  return 0;
}


/**  */
static const char* _tableHdr( void ) {
  return 0;
}


/**  */
static char* _toHtml( void* object ) {
  return 0;
}


/**  */
static struct OOperator* _inst( iONode ini ) {
  iOOperator __Operator = allocMem( sizeof( struct OOperator ) );
  iOOperatorData data = allocMem( sizeof( struct OOperatorData ) );
  MemOp.basecpy( __Operator, &OperatorOp, 0, sizeof( struct OOperator ), data );

  /* Initialize data->xxx members... */
  data->props = ini;

  instCnt++;
  return __Operator;
}


static Boolean _cmd( iOOperator inst, iONode nodeA ) {
  iOOperatorData data = Data(inst);
  iOStrTok tok = StrTokOp.inst(wOperator.getcarids(data->props), ',');

  const char* nodename = NodeOp.getName( nodeA );
  const char* cmd      = wOperator.getcmd( nodeA );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "command %s:%s for operator %s",
      nodename, (cmd==NULL?"-":cmd), wOperator.getid(data->props) );

  while( StrTokOp.hasMoreTokens(tok) ) {
    iOCar car = ModelOp.getCar(AppOp.getModel(), StrTokOp.nextToken(tok) );
    if( car != NULL ) {
      CarOp.cmd(car, (iONode)NodeOp.base.clone(nodeA));
    }
  }
  StrTokOp.base.del(tok);

  nodeA->base.del(nodeA);
  return True;
}


/**  */
static void _modify( struct OOperator* inst ,iONode props ) {
  iOOperatorData data = Data(inst);
  int cnt = NodeOp.getAttrCnt( props );
  int i = 0;
  for( i = 0; i < cnt; i++ ) {
    iOAttr attr = NodeOp.getAttr( props, i );
    const char* name  = AttrOp.getName( attr );
    const char* value = AttrOp.getVal( attr );
    NodeOp.setStr( data->props, name, value );
  }

  /* Broadcast to clients. */
  {
    iONode clone = (iONode)NodeOp.base.clone( data->props );
    AppOp.broadcastEvent( clone );
  }
  props->base.del(props);
}

static int _getLen( struct OOperator* inst ) {
  iOOperatorData data = Data(inst);
  /* ToDo: Calculate consist length. */
  int len = 0;
  iOStrTok tok = StrTokOp.inst(wOperator.getcarids(data->props), ',');
  while( StrTokOp.hasMoreTokens(tok) ) {
    iOCar car = ModelOp.getCar(AppOp.getModel(), StrTokOp.nextToken(tok) );
    if( car != NULL ) {
      len += CarOp.getLen(car);
    }
  }
  StrTokOp.base.del(tok);
  return len;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/operator.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
