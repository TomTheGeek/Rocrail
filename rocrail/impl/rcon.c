/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rocrail/impl/rcon_impl.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/thread.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/xmlh.h"


static int instCnt = 0;

/*
 ***** OBase functions.
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
  return "";
}
static void __del(void* inst) {
  iORConData data = Data(inst);
  StrOp.free( data->host );
  freeMem( data );
  freeMem( inst );
  instCnt--;
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
static int __count(void) {
  return instCnt;
}


/*
 ***** _Private functions.
 */
/**----------------------------------------------------------------------
 * __readXmlh()
 * ----------------------------------------------------------------------
 */
static Boolean __readXmlh( iOSocket sh, iOXmlh xmlh ) {
  char b = 0;
  while( SocketOp.peek( sh, &b, 1 ) ) {
    SocketOp.read( sh, &b, 1 );
    if( XmlhOp.read( xmlh, (byte*)&b, 1 ) ) {
      return True;
    }
    else if( XmlhOp.isError( xmlh ) ){
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "XmlhOp.isError" );
      return False;
    }
  };
  return False;
}



static void _infoReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iORCon rcon = (iORCon)ThreadOp.getParm(th);
  iORConData o = Data(rcon);
  iOSocket sock = o->sh;
  iOXmlh xmlh = XmlhOp.inst( False, NULL, NULL );
  char* info = NULL;

  while( o->run ) {
    char b;
    if( !SocketOp.peek( sock, &b, 1 ) ) {
      if( SocketOp.isBroken( sock ) ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                    "__readSiHdr Socket errno=%d", SocketOp.getRc( sock ) );
        break;
      }
      ThreadOp.sleep( 10 );
      continue;
    }

    XmlhOp.reset( xmlh );
    if( __readXmlh( sock, xmlh ) ) {
      Boolean ok = False;
      /* Allocate read buffer: */
      long size = XmlhOp.getSizeByTagName( xmlh, XmlhOp.xml_tagname, 0 );
      info = allocMem( size + 1 );
      info[0] = '\0';
      ok = SocketOp.read( sock, info, size );

      /* Call clallback: */
      if( ok && o->callback != NULL ) {
        /* Try to parse the XML string: */
        iODoc infoDoc = DocOp.parse( info );
        iONode root = NULL;
        if( infoDoc == NULL ) {
          /* Invalid XML string? */
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Invalid XML string: [%.20s]...", info );
          freeMem(info);
          continue;
        }
        root = DocOp.getRootNode( infoDoc );
        /* Cleanup document: */
        infoDoc->base.del(infoDoc);

        if( root != NULL ) {
          long mem = MemOp.getAllocSize();
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "infoReader: %s", NodeOp.getName( root ) );
          /* Call the callback: */
          if( o->callback != NULL )
            o->callback( o->cbCargo, root );
          /* Calulate used memory by the callback: */
          mem = MemOp.getAllocSize()-mem;
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Callback used %ld of rocs memory.", mem );
          /* Cleanup root node: */
          root->base.del( root );
        }
        else {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "XML string not parsed: [%.20s]...", info );
        }
      }
      else if( o->callback == NULL ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "infoReader: callback not initialized!" );
      }

      /* Cleanup read buffer: */
      freeMem( info );
    }
    else {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Close connection!" );
      SocketOp.disConnect( o->sh );
      ThreadOp.sleep( 1000 );
      SocketOp.connect( o->sh );
    }

    ThreadOp.sleep( 10 );
  }
}



/*
 ***** _Public functions.
 */
static void _setCallback( iORCon inst, rcon_callback cbFun, obj cbCargo ) {
  iORConData o = Data(inst);
  o->callback = cbFun;
  o->cbCargo = cbCargo;
}

static void _close( iORCon inst ) {
  if( inst != NULL ) {
    iORConData o = Data(inst);
    o->run = False;
    SocketOp.disConnect( o->sh );
  }
}

static Boolean _write( iORCon inst, char* cmdStr ) {
  iORConData o = Data(inst);

  /* Write string incl. terminating zero. */
  if( !SocketOp.isBroken( o->sh ) ) {
    iOXmlh xmlh    = XmlhOp.inst( True, NULL, NULL );
    iONode node    = NodeOp.inst( XmlhOp.xml_tagname, NULL, ELEMENT_NODE );
    long   hdrLen  = 0;
    int    len     = 0;
    char*  xmlhStr = NULL;

    /* Write string incl. terminating zero. */
    len = strlen( cmdStr ) + 1;
    NodeOp.setInt( node, "size", len );
    XmlhOp.addNode( xmlh, node );
    xmlhStr = (char*)XmlhOp.base.serialize( xmlh, &hdrLen );
    XmlhOp.base.del( xmlh );

    if( SocketOp.write( o->sh, xmlhStr, hdrLen ) )
      return SocketOp.write( o->sh, cmdStr, len );
    else
      return False;
  }
  else {
    /* Try to recover connection: */
    o->sh = SocketOp.inst( o->host, o->port, False, False, False );
    if( o->sh != NULL )
      return SocketOp.connect( o->sh );
    return False;
  }
}

static iORCon _inst( const char* host, int port ) {
  iORCon     rcon = allocMem( sizeof( struct ORCon ) );
  iORConData data = allocMem( sizeof( struct ORConData ) );

  /* OBase operations */
  MemOp.basecpy( rcon, &RConOp, 0, sizeof( struct ORCon ), data );

  data->host = StrOp.dup(host);
  data->port = port;
  data->run = True;

  data->sh = SocketOp.inst( host, port, False, False, False );
  if( !SocketOp.connect( data->sh ) ) {
    freeMem( data );
    freeMem( rcon );
    return NULL;
  }

  data->infoReader = ThreadOp.inst( "iReader", _infoReader, rcon );
  ThreadOp.start( data->infoReader );


  instCnt++;

  return rcon;
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/rcon.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
