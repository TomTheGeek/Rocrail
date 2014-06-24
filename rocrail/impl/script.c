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
#include "rocrail/wrapper/public/Stage.h"
#include "rocrail/wrapper/public/Clock.h"
#include "rocrail/wrapper/public/FunCmd.h"

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
static char* _convertNode(iONode node, Boolean addstamp) {
  char* scriptline = NULL;
  char* nodestring = NULL;

  /*<clock divider="2" time="1384775340" temp="20"/>*/

  if( node != NULL ) {
    scriptline = StrOp.fmt( "%s,%s", NodeOp.getName(node), NodeOp.getStr(node, "id", "?") );
    if( StrOp.equals( wClock.name(), NodeOp.getName(node))) {
      if( NodeOp.getStr(node, "divider", NULL) != NULL ) {
        scriptline = StrOp.cat( scriptline, ",");
        scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, "divider", NULL));
      }
      if( NodeOp.getStr(node, "time", NULL) != NULL ) {
        scriptline = StrOp.cat( scriptline, ",");
        scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, "time", NULL));
      }
    }

    if( StrOp.equals( wFunCmd.name(), NodeOp.getName(node))) {
      if( NodeOp.getStr(node, "fnchanged", NULL) != NULL ) {
        int fnchanged = wFunCmd.getfnchanged(node);
        char f[32] = {'\0'};
        scriptline = StrOp.cat( scriptline, ",");
        scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, "fnchanged", "0"));
        scriptline = StrOp.cat( scriptline, ",");
        StrOp.fmtb(f, "f%d", fnchanged);
        scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, f, "false"));
      }
    }

    if( NodeOp.getStr(node, "cmd", NULL) != NULL ) {
      scriptline = StrOp.cat( scriptline, ",");
      scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, "cmd", NULL));
    }
    if( NodeOp.getStr(node, "state", NULL) != NULL ) {
      scriptline = StrOp.cat( scriptline, ",");
      scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, "state", NULL));
    }
    if( NodeOp.getStr(node, "blockid", NULL) != NULL ) {
      scriptline = StrOp.cat( scriptline, ",");
      scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, "blockid", NULL));
    }
    if( NodeOp.getStr(node, "locid", NULL) != NULL ) {
      scriptline = StrOp.cat( scriptline, ",");
      scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, "locid", NULL));
    }
    if( NodeOp.getStr(node, "exitstate", NULL) != NULL ) {
      scriptline = StrOp.cat( scriptline, ",");
      scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, "exitstate", NULL));
    }
    if( NodeOp.getStr(node, "V", NULL) != NULL ) {
      scriptline = StrOp.cat( scriptline, ",");
      scriptline = StrOp.cat( scriptline, "V");
      scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, "V", NULL));
      scriptline = StrOp.cat( scriptline, ",");
      scriptline = StrOp.cat( scriptline, NodeOp.getStr(node, "dir", "true"));
    }

    if( addstamp ) {
      char* stamp = StrOp.createStamp();
      scriptline = StrOp.cat( scriptline, ",");
      scriptline = StrOp.cat( scriptline, stamp);
      StrOp.free(stamp);
    }
    scriptline = StrOp.cat( scriptline, "\n");
  }

  nodestring = NodeOp.toEscString(node);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "node converted: [%s] %s", scriptline, nodestring);
  StrOp.free(nodestring);

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
    char* nodename = NULL;
    char* parm1    = NULL; /* id */
    char* parm2    = NULL; /* cmd */
    char* parm3    = NULL;
    char* parm4    = NULL;

    iOStrTok tok = StrTokOp.inst( scriptline, ',' );

    if( StrTokOp.hasMoreTokens( tok ) )  {
      nodename = (char*)StrTokOp.nextToken( tok );
      __stripNewline(nodename);
    }
    if( StrTokOp.hasMoreTokens( tok ) )  {
      parm1 = (char*)StrTokOp.nextToken( tok );
      __stripNewline(parm1);
    }
    if( StrTokOp.hasMoreTokens( tok ) )  {
      parm2 = (char*)StrTokOp.nextToken( tok );
      __stripNewline(parm2);
    }
    if( StrTokOp.hasMoreTokens( tok ) )  {
      parm3 = (char*)StrTokOp.nextToken( tok );
      __stripNewline(parm3);
    }
    if( StrTokOp.hasMoreTokens( tok ) )  {
      parm4 = (char*)StrTokOp.nextToken( tok );
      __stripNewline(parm4);
    }

    if( nodename != NULL ) {
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
        if( parm2[0]=='V' ) {
          wLoc.setV(node, atoi(parm2+1));
          if( parm3 != NULL )
            wLoc.setdir(node, StrOp.equals(parm3, "true") );
        }
        else {
          wLoc.setcmd( node, parm2 );
          if( parm3 != NULL ) {
            /* lc,<id>,gotoblock,<bkid> */
            if( StrOp.equalsi(wLoc.gotoblock, parm2) )
              wLoc.setblockid(node, parm3);
            /* lc,<id>,useschedule,<scid> */
            if( StrOp.equalsi(wLoc.useschedule, parm2) )
              wLoc.setscheduleid(node, parm3);
            if( StrOp.equalsi(wLoc.assigntrain, parm2) )
              wLoc.settrain(node, parm3);
          }
        }
      }

      else if( StrOp.equalsi( wFunCmd.name(), nodename ) && parm1 != NULL && parm2 != NULL && parm3 != NULL ) {
        /* fn,<id>,fnchanged,true */
        char f[32] = {'\0'};
        node = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
        wFunCmd.setid( node, parm1 );
        wFunCmd.setfnchanged(node, atoi(parm2));
        StrOp.fmtb(f, "f%s", parm2);
        NodeOp.setStr(node, f, parm3);
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
        if( StrOp.equals(parm2, wBlock.closed) || StrOp.equals(parm2, wBlock.open) ) {
          wBlock.setstate(node, parm2);
        }
        else {
          wBlock.setcmd( node, parm2 );
          if( StrOp.equalsi(wBlock.loc, parm2) )
            wBlock.setlocid(node, parm3);
        }
      }

      else if( StrOp.equalsi( wStage.name(), nodename ) && parm1 != NULL && parm2 != NULL ) {
        /* sb,<id>,open */
        node = NodeOp.inst( wStage.name(), NULL, ELEMENT_NODE );
        wStage.setid( node, parm1 );
        if( StrOp.equals( wBlock.closed, parm2 ) || StrOp.equals( wBlock.open, parm2 ) ) {
          wStage.setexitstate( node, parm2 );
        }
        else {
          wStage.setcmd( node, parm2 );
        }
      }

      else if( StrOp.equalsi( wClock.name(), nodename ) && parm1 != NULL && parm2 != NULL ) {
        /* clock,<id>,divider,time */
        node = NodeOp.inst( wClock.name(), NULL, ELEMENT_NODE );
        wClock.setdivider( node, atoi(parm1) );
        wClock.settime( node, atol(parm2) );
      }

      else if( parm1 != NULL && parm2 != NULL ) {
        node = NodeOp.inst( nodename, NULL, ELEMENT_NODE );
        wItem.setid( node, parm1 );
        NodeOp.setStr( node, "cmd", parm2);
      }
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
  else {
    data->playing = False;
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
    char* scriptline = ScriptOp.convertNode(node, data->stamp);
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
static void _setRecording( struct OScript* inst ,Boolean recording, Boolean addstamp ) {
  iOScriptData data = Data(inst);
  if( !data->recording && recording && data->record != NULL ) {
    StrOp.free( data->record );
    data->record = NULL;
    data->prevtime = 0;
    data->linenr  = 0;

  }
  data->recording = recording;
  data->stamp = addstamp;
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
  data->pline = data->record;
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
        if( data->callback != NULL ) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "play: %s", NodeOp.getName(node) );
          data->callback->event( data->callback, node );
        }
        else
          NodeOp.base.del(node);
      }
    }
    else {
      ThreadOp.sleep(1000);
    }
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Script player ended." );
}

static void _setCallback(iOScript inst, obj callback) {
  iOScriptData data   = Data(inst);
  data->callback = callback;
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
