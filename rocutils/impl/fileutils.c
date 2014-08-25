/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net




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

#include "rocutils/impl/fileutils_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/system.h"
#include "rocs/public/dir.h"
#include "rocs/public/str.h"
#include "rocs/public/list.h"
#include "rocs/public/trace.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
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

/** ----- OFileUtils ----- */


/**  */
static char* _findFile( const char* directory ,const char* filename ) {
  char* filepath = NULL;
  if( FileOp.exist(directory) ) {
    iOList listdir = DirOp.listdir( directory, NULL, -1 );
    if( listdir != NULL ) {
      int listsize = ListOp.size(listdir);
      int i = 0;
      for( i = 0; i < listsize; i++) {
        iDirEntry dir = (iDirEntry)ListOp.get( listdir, i );
        filepath = StrOp.fmt("%s%c%s", directory, SystemOp.getFileSeparator(), dir->name);
        if( (StrOp.len(dir->name) == 1 && StrOp.equals(dir->name, ".")) || (StrOp.len(dir->name) == 2 && StrOp.equals(dir->name, "..")) ) {
          StrOp.free(filepath);
          filepath = NULL;
          continue;
        }
        else if( FileOp.isDirectory( filepath ) ) {
          char* filepath2 = FileUtilsOp.findFile(filepath, filename);
          if( filepath2 != NULL ) {
            StrOp.free(filepath);
            filepath = filepath2;
            break;
          }
        }
        else {
          TraceOp.trc( "findfile", TRCLEVEL_INFO, __LINE__, 9999, "getDirEntry: %s", dir->name );
          if( StrOp.equals( dir->name, filename ) ) {
            break;
          }
        }
        StrOp.free(filepath);
        filepath = NULL;
      }
      DirOp.cleandirlist( listdir );
    }
  }
  return filepath;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocutils/impl/fileutils.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
