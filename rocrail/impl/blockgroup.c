/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2010 - Rob Versluis <r.j.versluis@rocrail.net>

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

#include "rocrail/impl/blockgroup_impl.h"

#include "rocs/public/mem.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOBlockGroupData data = Data(inst);
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

static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

/** ----- OBlockGroup ----- */


/**  */
static Boolean _cmd( struct OBlockGroup* inst ,iONode cmd ) {
  return 0;
}


/**  */
static struct OBlockGroup* _inst( iONode ini ) {
  iOBlockGroup __BlockGroup = allocMem( sizeof( struct OBlockGroup ) );
  iOBlockGroupData data = allocMem( sizeof( struct OBlockGroupData ) );
  MemOp.basecpy( __BlockGroup, &BlockGroupOp, 0, sizeof( struct OBlockGroup ), data );

  /* Initialize data->xxx members... */
  data->props = ini;
  data->lockmap = MapOp.inst();

  instCnt++;
  return __BlockGroup;
}


/**  */
static Boolean _lock( struct OBlockGroup* inst ,const char* BlockId ,const char* LocoId ) {
  return 0;
}


/**  */
static void _modify( struct OBlockGroup* inst ,iONode mod ) {
  return;
}


/**  */
static Boolean _unlock( struct OBlockGroup* inst ,const char* LocoId ) {
  return 0;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/blockgroup.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
