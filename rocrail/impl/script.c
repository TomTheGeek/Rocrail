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



#include "rocrail/impl/script_impl.h"

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
#include "rocrail/wrapper/public/Block.h"
#include "rocrail/wrapper/public/Item.h"

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


static void __stripNewline(char* str) {
  int i = 0;
  int len = StrOp.len(str);
  for( i=0; i < len; i++ ) {
    if( str[i] == '\n' )
      str[i] = '\0';
  }
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
      __stripNewline(nodename);
    }
    if( StrTokOp.hasMoreTokens( tok ) )  {
      parm1 = StrTokOp.nextToken( tok );
      __stripNewline(parm1);
    }
    if( StrTokOp.hasMoreTokens( tok ) )  {
      parm2 = StrTokOp.nextToken( tok );
      __stripNewline(parm2);
    }
    if( StrTokOp.hasMoreTokens( tok ) )  {
      parm3 = StrTokOp.nextToken( tok );
      __stripNewline(parm3);
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "parsing command: %s", nodename);

    if( StrOp.equalsi( wFeedback.name(), nodename ) && parm1 != NULL && parm2 != NULL ) {
      /* fb,<id>,<true>,<ident> */
      node = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
      wFeedback.setid( node, parm1 );
      wFeedback.setstate( node, StrOp.equalsi("true", parm2) );
      if( parm3 != NULL )
        wFeedback.setidentifier(node, parm3);
    }

    else if( StrOp.equalsi( "pause", nodename ) && parm1 != NULL ) {
      int seconds = atoi(parm1);
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "play: pause %d", seconds );
      ThreadOp.sleep(1000*seconds);
    }

    else if( StrOp.equalsi( wLoc.name(), nodename ) && parm1 != NULL && parm2 != NULL ) {
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

    else if( StrOp.equalsi( wSwitch.name(), nodename ) && parm1 != NULL && parm2 != NULL ) {
      /* sw,<id>,straight */
      node = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
      wSwitch.setid( node, parm1 );
      wSwitch.setcmd( node, parm2 );
    }

    else if( StrOp.equalsi( wOutput.name(), nodename ) && parm1 != NULL && parm2 != NULL ) {
      /* co,<id>,on */
      node = NodeOp.inst( wOutput.name(), NULL, ELEMENT_NODE );
      wOutput.setid( node, parm1 );
      wOutput.setcmd( node, parm2 );
    }

    else if( StrOp.equalsi( wSignal.name(), nodename ) && parm1 != NULL && parm2 != NULL ) {
      /* sg,<id>,green */
      node = NodeOp.inst( wSignal.name(), NULL, ELEMENT_NODE );
      wSignal.setid( node, parm1 );
      wSignal.setcmd( node, parm2 );
    }

    else if( StrOp.equalsi( wBlock.name(), nodename ) && parm1 != NULL && parm2 != NULL ) {
      /* bk,<id>,open */
      node = NodeOp.inst( wBlock.name(), NULL, ELEMENT_NODE );
      wBlock.setid( node, parm1 );
      wBlock.setcmd( node, parm2 );
    }

    else if( parm1 != NULL && parm2 != NULL ) {
      node = NodeOp.inst( nodename, NULL, ELEMENT_NODE );
      wItem.setid( node, parm1 );
      NodeOp.setStr( node, "cmd", parm2);
    }

    StrTokOp.base.del(tok);
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
    if( data->pline != NULL ) {
      data->pline++;
      data->linenr++;
    }
    else {
      data->playing = False;
    }
  }
  return node;
}


static int __analyseScript(iOScript inst, const char* script) {
  iOScriptData data = Data(inst);
  int len = 0;
  int idx = 0;

  if( script == NULL ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "no script available");
    return 0;
  }

  data->nrlines = 0;
  len = StrOp.len(script);
  while( idx < len ) {
    if( script[idx] == '\n' ) {
      data->nrlines++;
    }
    idx++;
  }
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "script has %d lines", data->nrlines );

  return 0;
}


static void _setScript(iOScript inst, const char* script) {
  iOScriptData data = Data(inst);
  if( data->record != NULL ) {
    StrOp.free(data->record);
  }
  data->record = StrOp.dup(script);
  data->linenr = 0;
  data->pline  = data->record;
  __analyseScript(inst, data->record);
}


/**  */
static void _recordNode( struct OScript* inst ,iONode node ) {
  iOScriptData data = Data(inst);
  if( data->recording ) {
    char* scriptline = ScriptOp.convertNode(node);
    if( scriptline != NULL ) {
      if( data->prevtime > 0 ) {
        long diff = time(NULL) - data->prevtime;
        char pauseStr[64];
        StrOp.fmtb(pauseStr, "pause,%ld\n", diff);
        data->record = StrOp.cat( data->record, pauseStr );
        data->pline  = data->record;
      }
      data->record = StrOp.cat( data->record, scriptline );
      StrOp.free(scriptline);
      data->pline  = data->record;
      data->prevtime = time(NULL);
    }
  }
}


/**  */
static void _setRecording( struct OScript* inst ,Boolean recording ) {
  iOScriptData data = Data(inst);
  if( !data->recording && recording && data->record != NULL ) {
    StrOp.free( data->record );
    data->record = NULL;
    data->prevtime = 0;
    data->linenr  = 0;

  }
  data->recording = recording;
}


/**  */
static Boolean _isRecording( struct OScript* inst ) {
  iOScriptData data = Data(inst);
  return data->recording;
}


/**  */
static Boolean _isPlaying( struct OScript* inst, int* linenr ) {
  iOScriptData data = Data(inst);
  if( linenr != NULL ) {
    *linenr = data->linenr;
  }
  return data->playing;
}


/**  */
static void _Play( struct OScript* inst ) {
  iOScriptData data = Data(inst);
  if( !data->playing ) {
    data->pline   = data->record;
    data->linenr  = 0;
    data->playing = True;
    data->pause   = False;
  }
  else {
    data->pause = False;
  }
}


/**  */
static void _Pause( struct OScript* inst ) {
  iOScriptData data = Data(inst);
  if( data->playing )
    data->pause = !data->pause;
}


/**  */
static void _Stop( struct OScript* inst ) {
  iOScriptData data = Data(inst);
  data->playing = False;
  data->pause = False;
}

static void __player( void* threadinst ) {
  iOThread     th     = (iOThread)threadinst;
  iOScript     script = (iOScript)ThreadOp.getParm(th);
  iOScriptData data   = Data(script);

  ThreadOp.sleep(1000);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Script player started." );

  while(data->run) {
    ThreadOp.sleep(10);
    if( !data->pause && data->playing ) {
      iONode node = ScriptOp.nextLine(script);
      if( node != NULL ) {
        if( data->rcon != NULL ) {
          char* strCmd = NodeOp.base.toString( node );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "play: %s", strCmd );
          RConOp.write( data->rcon, strCmd );
          StrOp.free( strCmd );
          NodeOp.base.del(node);
        }
      }
    }
    else {
      ThreadOp.sleep(1000);
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Script player ended." );
}

static void _setCallback(iOScript inst, iORCon rcon) {
  iOScriptData data   = Data(inst);
  data->rcon = rcon;
}

/**  */
static struct OScript* _inst( const char* script ) {
  iOScript __Script = allocMem( sizeof( struct OScript ) );
  iOScriptData data = allocMem( sizeof( struct OScriptData ) );
  MemOp.basecpy( __Script, &ScriptOp, 0, sizeof( struct OScript ), data );

  /* Initialize data->xxx members... */
  ScriptOp.setScript(__Script, script);

  data->run = True;
  data->player = ThreadOp.inst( "scriptplayer", __player, __Script );
  ThreadOp.start( data->player );

  instCnt++;
  return __Script;
}




/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/script.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
