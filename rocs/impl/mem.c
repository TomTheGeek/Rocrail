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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/mutex.h"

#include "rocs/impl/mem_impl.h"

/*
 ***** _Private members.
 */
static Boolean m_bDebug = False;
static long m_lAllocated = 0;
static long m_lAllocatedSize = 0;
static long m_lAllocatedID[RocsLASTID];
static struct __OMemTrace mt;
static iOMutex mux = NULL;
#define MAXSTRINGS 100
static void* m_Strings[MAXSTRINGS];
/*
 ***** _Public functions.
 */
static void _mem_setDebug( Boolean l_bDebug ) {
  m_bDebug = l_bDebug;
}


static void _mem_init( void ) {
  if( mux == NULL ) {
    mux = MutexOp.inst( NULL, True );
  }
}


static char __opStr[1024];
static const char* _mem_getLastOperation( void ) {
  const char* op = "?";
  if( mt.type = MEMTYPE_ALLOC )
    op = "alloc";
  else if( mt.type = MEMTYPE_FREE )
    op = "free";
  else if( mt.type = MEMTYPE_CHECK )
    op = "check";
  else if( mt.type = MEMTYPE_REALLOC )
    op = "realloc";
  sprintf( __opStr, ">>>>> memLastOp: op=%s p=0x%08X file=%s line=%d <<<<<", op, mt.p, mt.file, mt.line );
  return __opStr;
}

static Boolean __isMemValid( char* p, const char* file, int line, long* size, int id ) {
  if( p != NULL ) {
    __iOMemAlloc m = (__iOMemAlloc)(p - sizeof( struct __OMemAlloc ));
    mt.type = MEMTYPE_CHECK;
    mt.line = line;
    mt.file = file;
    mt.p    = p;
    if( memcmp( m->magic, __magic, MAGIC_SIZE ) != 0 ) {
      printf( ">>>>> Unknown memory block( 0x%08X ) %s:%d <<<<<\n", m, file, line );
      return False;
    }
    else if( m->id != id ) {
      printf( ">>>>> memory block id=%d freeID=%d file=%s line=%d <<<<<\n", m->id, id, file, line );
      return False;
    }
    else {
      *size = m->size;
      return True;
    }
  }
  return False;
}

static char* __mem_alloc_magic( long size, const char* file, int line, int id ) {
  long     msize = size + sizeof( struct __OMemAlloc );
  void*        p = malloc( msize );
  __iOMemAlloc m = p;
  mt.type = MEMTYPE_ALLOC;
  mt.line = line;
  mt.file = file;
  mt.p    = p;
  if( p == NULL ) {
    printf( ">>>>> malloc( %ld ) failed! %s:%d <<<<<\n", msize, file, line );
    return NULL;
  }
  memset( m, 0, msize );
  memcpy( m->magic, __magic, MAGIC_SIZE );
  m->size = size;
  m->id   = id;
  if( mux == NULL || MutexOp.wait( mux ) ) {
    m_lAllocatedSize += msize;
    m_lAllocated++;
    if( id != -1 && id < RocsLASTID )
      m_lAllocatedID[id]++;
    if( mux != NULL ) {
      MutexOp.post( mux );
    }
  }
  return (char*)( (char*)p + sizeof( struct __OMemAlloc ) );
}

static void __mem_free_magic( char* p, const char* file, int line, int id ) {
  if( p != NULL ) {
    __iOMemAlloc m = (__iOMemAlloc)(p - sizeof( struct __OMemAlloc ));
    long oldsize = 0;
    if( __isMemValid( p, file, line, &oldsize, id ) ) {
      long msize = m->size + sizeof( struct __OMemAlloc );
      /* Set last operation. */
      mt.type = MEMTYPE_FREE;
      mt.line = line;
      mt.file = file;
      mt.p    = p;
      /* Reset memory before freeing it. */
      memset( m, 0, sizeof( struct __OMemAlloc ) );
      free( m );
      if( mux == NULL || MutexOp.wait( mux ) ) {
        m_lAllocatedSize -= msize;
        m_lAllocated--;
        if( id != -1 && id < RocsLASTID )
          m_lAllocatedID[id]--;
        if( mux != NULL ) {
          MutexOp.post( mux );
        }
      }
    }
  }
}

static void* __mem_realloc_magic( char* p, long newsize, const char* file, int line ) {
  if( p != NULL ) {
    __iOMemAlloc m = (__iOMemAlloc)(p - sizeof( struct __OMemAlloc ));
    long oldsize = 0;
    if( __isMemValid( p, file, line, &oldsize, m->id ) ) {
      void* newP = __mem_alloc_magic( newsize, file, line, m->id );
      /* Set last operation. */
      mt.type = MEMTYPE_REALLOC;
      mt.line = line;
      mt.file = file;
      mt.p    = p;
      if( newP != NULL ) {
        long cpsize = oldsize;
        if( newsize < oldsize )
          cpsize = newsize;
        memcpy( newP, p, cpsize );
        __mem_free_magic( p, file, line, m->id );
        return newP;
      }
      else
        return NULL;
    }
  }
  else {
    printf( ">>>>> realloc( 0x%08X, %ld ) with NULL pointer! %s:%d <<<<<\n", p, newsize, file, line );
    return __mem_alloc_magic( newsize, file, line, -1 );
  }
  return NULL;
}


static long _mem_getAllocCount(void) {
  return m_lAllocated;
}

static long _mem_getAllocSize(void) {
  return m_lAllocatedSize;
}

static long _mem_getAllocCntID( int id ) {
  return m_lAllocatedID[id];
}

static const long* _mem_dumpAllocCntID(void) {
  return &m_lAllocatedID[0];
}

static int _mem_getDumpSize(void) {
  return sizeof( m_lAllocatedID );
}

static void _mem_resetDump(void) {
  memset( m_lAllocatedID, 0L, sizeof( m_lAllocatedID ) );
}


static void _dumpStrings(void) {
  RocsMemID id = RocsStrID;
  if( id == RocsStrID ) {
    int i = 0;
    for( i = 0; i < MAXSTRINGS; i++ ) {
      if( m_Strings[i] != NULL ) {
        printf("***DUMP STRING[%d]: [%s]\n", i, (char*)m_Strings[i]);
      }
    }
  }
}

static void* _mem_alloc( long size, const char* file, int line ) {
  void* mp = __mem_alloc_magic( size, file, line, -1 );
  if( mp == NULL ) {
    printf( "__mem_alloc_magic(%d) failed!", size );
  }
  if( m_bDebug )
    printf( " 0x%08X = allocMem( %d ) %s line=%d\n", mp, size, file, line );
  return mp;
}

static void* _mem_allocTID( long size, int id, const char* file, int line ) {
  void* p = __mem_alloc_magic( size, file, line, id );
  if( p == NULL ) {
    printf( "__mem_alloc_magic(%d) failed!", size );
  }
  if( id == -1 ) {
    printf( " allicIDMem( 0x%08X, %d ) %s line=%d: id -1 not allowed!!!\n", p, size, file, line );
  }
  /*if( m_bDebug )
    printf( " 0x%08X = allocIDMem( 0x%08X ) %s line=%d\n", p, size, file, line );*/

  if( id == RocsStrID ) {
    int i = 0;
    for( i = 0; i < MAXSTRINGS; i++ ) {
      if( m_Strings[i] == NULL ) {
        m_Strings[i] = p;
        break;
      }
    }
  }

  return p;
}

static void* _mem_realloc( void* p, long size, const char* file, int line ) {
  void* mp = __mem_realloc_magic( p, size, file, line );
  if( mp == NULL ) {
    printf( "__mem_realloc_magic(%08X, %d) failed!", p, size );
  }
  /*if( m_bDebug )
    printf( " 0x%08X = reallocMem( 0x%08X, %d ) %s line=%d\n", p, p2, size, file, line );*/
  return mp;
}

static void _mem_free( void* p, const char* file, int line ) {
  if( p != NULL ) {
    __mem_free_magic( p, file, line, -1 );
  }
  if( m_bDebug )
    printf( " freeMem( 0x%08X ) %s line=%d\n", p, file, line );
}

static void _mem_freeTID( void* p, int id, const char* file, int line ) {
  if( id == -1 && p!= NULL ) {
    printf( " freeIDMem( 0x%08X ) %s line=%d: id -1 not allowed!!!\n", p, file, line );
  }
  /*if( m_bDebug )
    printf( " freeMem( 0x%08X ) %s line=%d\n", p, file, line );*/

  if( id == RocsStrID ) {
    int i = 0;
    for( i = 0; i < MAXSTRINGS; i++ ) {
      if( m_Strings[i] == p ) {
        m_Strings[i] = NULL;
        break;
      }
    }
  }



  __mem_free_magic( p, file, line, id );
}

static void _mem_freeObj( void** o ) {
  if( *o != NULL ) {
    ((obj)*o)->del((obj)*o);
    *o = NULL;
  }
}

static void _mem_copy( void* dst, const void* src, int size ) {
  if( dst != NULL && src != NULL )
    memcpy( dst, src, size );
}


static Boolean _mem_cmp( const void* dst, const void* src, int size ) {
  if( dst != NULL && src != NULL )
    return memcmp( dst, src, size ) == 0 ? True:False;
  else
    return False;
}


static char* _mem_chr( const void* buffer, char c, int size ) {
  return memchr( buffer, c, size );
}


/*
 * asize = additional struct size of optional interfaces.
 * tsize = total size of dst struct.
 * data  = points to a data object.
 */
static void _mem_basecpy( void* dst, void* src, int asize, int tsize, void* data ) {
  char*  pDst  = dst;
  char*  pSrc  = src;
  iOBase pBase = dst;
  int    sbase = sizeof( struct OBase );

  /* Copy OBase function pointers. */
  memcpy( pDst, pSrc, sbase );

  if( asize > 0 )
    memcpy( pDst + sbase, pSrc + sbase, asize );

  /* Copy object specific function pointers. */
  memcpy( pDst + asize + sbase,
          pSrc + asize + sbase,
          tsize - ( asize + sbase )
          );

  /* Copy data pointer into OBase. */
  pBase->data = data;
}


static void _mem_set( void* p, int val, int size ) {
  memset( p, val, size );
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/mem.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
