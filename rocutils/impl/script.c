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

#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Command.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Feedback.h"

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

/** ----- OScript ----- */


/**  */
static iONode _getLine( iOScript inst, int linenr ) {
  iOScriptData data = Data(inst);
  return NULL;
}


/**  */
static iONode _nextLine(iOScript inst) {
  iOScriptData data = Data(inst);
  iONode node = NULL;

  if( data->pline != NULL ) {
    const char* cmd   = NULL;
    const char* parm1 = NULL;
    const char* parm2 = NULL;
    const char* parm3 = NULL;

    iOStrTok tok = StrTokOp.inst( data->pline, ',' );

    if( StrTokOp.hasMoreTokens( tok ) )  {
      cmd = StrTokOp.nextToken( tok );
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

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "parsing command: %s", cmd);

    if( StrOp.equalsi(wSysCmd.name(), cmd) && parm1 != NULL) {
      /* sys,go */
      node = NodeOp.inst(wSysCmd.name(), NULL, ELEMENT_NODE);
      wSysCmd.setcmd( node, parm1);
    }

    else if( StrOp.equalsi( wFeedback.name(), cmd ) && parm1 != NULL && parm2 != NULL ) {
      /* fb,<id>,true,<ident> */
      node = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
      wFeedback.setid( node, parm1 );
      wFeedback.setstate( node, StrOp.equalsi("true", parm2) );
      if( parm3 != NULL )
        wFeedback.setidentifier(node, parm3);
    }

    else if( StrOp.equalsi( wLoc.name(), cmd ) && parm1 != NULL && parm2 != NULL ) {
      /* lc,<id>,go */
      node = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setid( node, parm1 );
      wLoc.setcmd( node, parm2 );
      if( parm3 != NULL ) {
        /* lc,<id>,gotoblock,<bkid> */
        if( StrOp.equalsi(wLoc.gotoblock, parm2) )
          wLoc.setblockid(node, parm3);
        /* lc,<id>,useschedule,<scid> */
        if( StrOp.equalsi(wLoc.useschedule, parm2) )
          wLoc.setscheduleid(node, parm3);
      }
    }



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
