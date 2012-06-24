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
#include "rocs/impl/ebcdic_impl.h"
#include "rocs/impl/converter.h"
#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/doc.h"
#include "rocs/public/node.h"
#include "rocs/public/file.h"


static int instCnt = 0;

/*
 ***** OBase operations.
 */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static const char* __name(void) {
  return name;
}
static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}
static void __deserialize(void* inst, unsigned char* a) {
}

static char* __toString(void* inst) {
  iOEbcdicData data = Data(inst);
  char* str = StrOp.fmtID( RocsEbcdicID, "CodePage=%d", data->CodePage );
  return str;
}

static void __del(void* inst) {
  iOEbcdic     ebcdic = inst;
  iOEbcdicData data   = Data(inst);
  freeIDMem( data, RocsEbcdicID );
  freeIDMem( ebcdic, RocsEbcdicID );
  instCnt--;
}
static int __count(void) {
  return instCnt;
}
static void* __properties(void* inst) {
  return NULL;
}
static struct OBase* __clone( void* inst ) {
  return NULL;
}
static Boolean __equals( void* inst1, void* inst2 ) {
  return False;
}

/*
 ***** OEbcdic operations.
 */

static Boolean __parseConverterFile( iOEbcdic inst ) {
  iOEbcdicData data = Data(inst);
  Boolean ok = True;
  char* convXml = NULL;

  iOFile f = FileOp.inst( data->file, True );

  if( f == NULL ) {
    return False;
  }

  convXml = allocMem( FileOp.size( f ) + 1 );
  FileOp.read( f, convXml, FileOp.size( f ) );
  FileOp.close( f );
  FileOp.base.del( f );
  if( StrOp.len( convXml ) == 0 )
    ok = False;

  if( ok ) {
    iONode convmap = NULL;
    iONode    conv = NULL;
    iODoc  convDoc = NULL;
    int  convCount = 0;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Parsing %s...", data->file );
    convDoc = DocOp.parse( convXml );


    if( convDoc != NULL )
      convmap = DocOp.getRootNode( convDoc );
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Document == NULL!" );

    if( convmap != NULL )
      conv = NodeOp.findNode( convmap, "conv" );
    else
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Document has no rootnode!" );

    if( convmap != NULL && conv == NULL )
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Rootnode(%s) has no \"conv\" childnodes!", NodeOp.getName(convmap) );

    MemOp.set( data->AsciiToEbcdicTable, 0, 256 );
    MemOp.set( data->EbcdicToAsciiTable, 0, 256 );

    while( conv != NULL ) {
      const char* ebcdicStr = NodeOp.getStr( conv, "ebcdic", NULL );
      const char* latin1Str = NodeOp.getStr( conv, "latin1", NULL );
      if( ebcdicStr != NULL && latin1Str != NULL ) {
        int ebcdicVal = (int)strtol( ebcdicStr, (char **)NULL, 0 );
        int latin1Val = (int)strtol( latin1Str, (char **)NULL, 0 );
        if( latin1Val != 0 && ebcdicVal != 0) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Mapping 0x%02X to 0x%02X", ebcdicVal, latin1Val );
          data->AsciiToEbcdicTable[latin1Val & 0xFF] = ((byte)ebcdicVal) & 0xFF;
          data->EbcdicToAsciiTable[ebcdicVal & 0xFF] = ((byte)latin1Val) & 0xFF;
          convCount++;
        }
      }
      conv = NodeOp.findNextNode( convmap, conv );
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%d conv nodes mapped.", convCount );

  }

  freeMem( convXml );

  return ok;
}

static Boolean __InitializeTables( iOEbcdic inst ) {
  iOEbcdicData data = Data(inst);
  Boolean ok = False;

  if( data->file != NULL ) {
    if( !__parseConverterFile( inst ) ) {
      MemOp.copy( data->AsciiToEbcdicTable, Cp1252_AsciiToEbcdicTable, 256 );
      MemOp.copy( data->EbcdicToAsciiTable, Cp1252_EbcdicToAsciiTable, 256 );
      ok = True;
    }
  }
  else if( data->CodePage == CODEPAGE_1252 || data->CodePage == CODEPAGE_DEFAULT ) {
    MemOp.copy( data->AsciiToEbcdicTable, Cp1252_AsciiToEbcdicTable, 256 );
    MemOp.copy( data->EbcdicToAsciiTable, Cp1252_EbcdicToAsciiTable, 256 );
    ok = True;
  }
  else if( data->CodePage == CODEPAGE_437 ) {
    MemOp.copy( data->AsciiToEbcdicTable, Cp437_AsciiToEbcdicTable, 256 );
    MemOp.copy( data->EbcdicToAsciiTable, Cp437_EbcdicToAsciiTable, 256 );
    ok = True;
  }
  else {
    /* Unknown codepage! */
    ok = False;
  }
  return ok;
}

static char _getEbcdic( iOEbcdic inst, char c ) {
  iOEbcdicData data = Data(inst);
  return data->EbcdicToAsciiTable[(unsigned char)c];
}

static char _getAscii( iOEbcdic inst, char c ) {
  iOEbcdicData data = Data(inst);
  return data->AsciiToEbcdicTable[(unsigned char)c];
}

static char* _Ascii2Ebcdic( iOEbcdic inst, char * pBuffer, int iLen ) {
  iOEbcdicData data = Data(inst);
  int i = 0;
  for( i = 0; i < iLen; i++ )
    pBuffer[i] = data->AsciiToEbcdicTable[(unsigned char)pBuffer[i]];
  return pBuffer;
}

static char* _Ebcdic2Ascii( iOEbcdic inst, char * pBuffer, int iLen ) {
  iOEbcdicData data = Data(inst);
  int i = 0;
  for( i = 0; i < iLen; i++ ) {
    pBuffer[i] = data->EbcdicToAsciiTable[(unsigned char)pBuffer[i]];
  }
  return pBuffer;
}

static char* _Ebcdic2TrueAscii( iOEbcdic inst, char * pBuffer, int iLen ) {
  iOEbcdicData data = Data(inst);
  int i = 0;
  for( i = 0; i < iLen; i++ ) {
    pBuffer[i] = data->EbcdicToAsciiTable[(unsigned char)pBuffer[i]];
    if( pBuffer[i] < 32 || (unsigned char)pBuffer[i] > 127 )
      pBuffer[i] = '.';
  }
  return pBuffer;
}

static char* _Ebcdic2ExtAscii( iOEbcdic inst, char * pBuffer, int iLen ) {
  iOEbcdicData data = Data(inst);
  int i = 0;
  for( i = 0; i < iLen; i++ ) {
    pBuffer[i] = data->EbcdicToAsciiTable[(unsigned char)pBuffer[i]];
    if( pBuffer[i] < ' ' || pBuffer[i] > '~' )
      pBuffer[i] = '.';
  }
  return pBuffer;
}

static iOEbcdic _inst( codepage CodePage, const char* converterFile ) {
  iOEbcdic     object = allocIDMem( sizeof( struct OEbcdic     ), RocsEbcdicID );
  iOEbcdicData data   = allocIDMem( sizeof( struct OEbcdicData ), RocsEbcdicID );

  /* OEbcdicData */
  data->CodePage = CodePage;
  data->file = converterFile;

  /* OBase operations */
  MemOp.basecpy( object, &EbcdicOp, 0, sizeof( struct OEbcdic ), data );

  if( !__InitializeTables( object ) ) {
    /* Error! */
  }

  instCnt++;

  return object;
}



/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocs/impl/ebcdic.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
