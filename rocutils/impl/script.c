/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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



#include "rocutils/impl/script_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/strtok.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del( void* inst ) {
  if( inst != NULL ) {
    iOScriptData data = Data(inst);
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
  iOScriptData data = Data(inst);
  return data->record;
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

/** ----- OScript ----- */


/**  */
static iONode _getLine( iOScript inst, int linenr ) {
  iOScriptData data = Data(inst);
  return NULL;
}


/* Used for recording a node. */
static char* _convertNode(iONode node) {
  char* scriptline = NULL;

  if( node != NULL ) {
    scriptline = StrOp.fmt( "%s,%s", NodeOp.getName(node), NodeOp.getStr(node, "id", "?") );
    if( NodeOp.getStr(node, "cmd", NULL) != NULL ) {
      scriptline = StrOp.cat( scriptline, ",");
      scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, "cmd", NULL));
    }
    else if( NodeOp.getStr(node, "state", NULL) != NULL ) {
      scriptline = StrOp.cat( scriptline, ",");
      scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, "state", NULL));
    }
    scriptline = StrOp.cat( scriptline, "\n");
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "node converted: [%s]", scriptline);

  return scriptline;
}


/* Create a node from a script line.  */
static iONode _parseLine(const char* scriptline) {
  iONode node = NULL;

  if( scriptline != NULL ) {
    const char* nodename = NULL;
    const char* parm1    = NULL; /* id */
    const char* parm2    = NULL; /* cmd */
    const char* parm3    = NULL;

    iOStrTok tok = StrTokOp.inst( scriptline, ',' );

    if( StrTokOp.hasMoreTokens( tok ) )  {
      nodename = StrTokOp.nextToken( tok );
    }
    if( StrTokOp.hasMoreTokens( tok ) )  {
      parm1 = StrTokOp.nextToken( tok );
    }
    if( StrTokOp.hasMoreTokens( tok ) )  {
      parm2 = StrTokOp.nextToken( tok );
    }
    if( StrTokOp.hasMoreTokens( tok ) )  {
      parm3 = StrTokOp.nextToken( tok );
    }
    StrTokOp.base.del(tok);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "parsing command: %s", nodename);

    if( StrOp.equalsi( "fb", nodename ) && parm1 != NULL && parm2 != NULL ) {
      /* fb,<id>,<true>,<ident> */
      node = NodeOp.inst( "fb", NULL, ELEMENT_NODE );
      NodeOp.setStr( node, "id", parm1 );
      NodeOp.setBool( node, "state", StrOp.equalsi("true", parm2) );
      if( parm3 != NULL )
        NodeOp.setStr(node, "identifier", parm3);
    }

    else if( StrOp.equalsi( "lc", nodename ) && parm1 != NULL && parm2 != NULL ) {
      /* lc,<id>,go */
      node = NodeOp.inst( "lc", NULL, ELEMENT_NODE );
      NodeOp.setStr( node, "id", parm1 );
      NodeOp.setStr( node, "cmd", parm2 );
      if( parm3 != NULL ) {
        /* lc,<id>,gotoblock,<bkid> */
        if( StrOp.equalsi("gotoblock", parm2) )
          NodeOp.setStr( node, "id", parm3);
        /* lc,<id>,useschedule,<scid> */
        if( StrOp.equalsi("useschedule", parm2) )
          NodeOp.setStr( node, "scheduleid", parm3);
      }
    }

  }
  return node;
}



/**  */
static iONode _nextLine(iOScript inst) {
  iOScriptData data = Data(inst);
  iONode node = NULL;

  if( data->pline != NULL ) {
    node = ScriptOp.parseLine(data->pline);

    /* prepare next line pointer */
    data->pline = StrOp.findc(data->pline, '\n');
    if( data->pline != NULL )
      data->pline++;
  }
  return node;
}


static int __analyseScript(iOScript inst) {
  iOScriptData data = Data(inst);
  int len = 0;
  int idx = 0;

  if( data->script == NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no script available");
    return 0;
  }

  data->nrlines = 0;
  len = StrOp.len(data->script);
  while( idx < len ) {
    if( data->script[idx] == '\n' ) {
      data->nrlines++;
    }
    idx++;
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "script has %d lines", data->nrlines );

  return 0;
}


static void _setScript(iOScript inst, const char* script) {
  iOScriptData data = Data(inst);
  data->script = script;
  data->linenr = 0;
  data->pline  = script;
  __analyseScript(inst);
}


/**  */
static void _recordNode( struct OScript* inst ,iONode node ) {
  iOScriptData data = Data(inst);
  if( data->recording ) {
    char* scriptline = ScriptOp.convertNode(node);
    if( scriptline != NULL ) {
      data->record = StrOp.cat( data->record, scriptline );
      StrOp.free(scriptline);
      data->pline  = data->record;
    }
  }
}


/**  */
static void _setRecording( struct OScript* inst ,Boolean recording ) {
  iOScriptData data = Data(inst);
  if( !data->recording && recording && data->record != NULL ) {
    StrOp.free( data->record );
    data->record = NULL;
  }
  data->recording = recording;
}


/**  */
static Boolean _isRecording( struct OScript* inst ) {
  iOScriptData data = Data(inst);
  return data->recording;
}


/**  */
static struct OScript* _inst( const char* script ) {
  iOScript __Script = allocMem( sizeof( struct OScript ) );
  iOScriptData data = allocMem( sizeof( struct OScriptData ) );
  MemOp.basecpy( __Script, &ScriptOp, 0, sizeof( struct OScript ), data );

  /* Initialize data->xxx members... */
  ScriptOp.setScript(__Script, script);

  instCnt++;
  return __Script;
}




/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocutils/impl/script.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
