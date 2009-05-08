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

/*

  ecos-parser.c

  Parses commands from the ECoS.  What is this doing, and where is it called?

 * ecos_reader -- Tries to read an incoming message from <REPLY or <EVENT to <END
 * __parseCallParams -- breaks parameters out of a message coming from the ECoS?
 * __parseREPLY -- returns a reply in replyline
 * __parseEVENT -- Returns an event string in replyline
 * __parseRow
 * __parseEND
 * ecos_parser -- parse the reply and put it in a node.

*/

#include "rocs/public/socket.h"
#include "rocs/public/trace.h"
#include "rocs/public/thread.h"

#include "rocdigs/impl/ecos/ecos-parser.h"

/*

Sample streams coming from the ECoS:


  -- Getting info about a loc

<REPLY queryObjects(10, name)>
1000 name[„Big Boy“]
<END 0 (OK)>


  -- Getting info about the ECoS

Informationen über die ECoS:
(Information about the ECoS)
<REPLY get(1, info)>
1 ECoS
1 ProtocolVersion[0.1]
1 ApplicationVersion[1.0.1]
1 HardwareVersion[1.3]
<END 0 (OK)>


  -- Getting status of the ECoS

Aktuelle Statusinformationen abfragen:
(Current status information query)
<REPLY get(1, status)>
1 Status[val]
<END 0 (OK)>
val=STOP
val=GO
val=SHUTDOWN


  -- Reply coming from ECoS???

<REPLY/EVENT set/get/create/delete/request/release/link/unlink/queryObjects(ID, var[val], ...)>
ID var[val], ...
<END rc (string)>


  -- Loc control protocol names

LC:
protocol[val]
Setzen des Protokolls. val kann folgende Werte annehmen:
(Set minutes val can the following values assume)

Acceptable values for the protocol ???

  MM14, MM27, MM28, DCC14, DCC28, DCC128, SX32, MMFKT


  -- Switch control protocol names

SW:
protocol[val]
Setzen des Protokolls (val entweder MM oder DCC)
(Set minutes (val either mm or DCC))

Set the protocol to either MM or DCC


 */


/**
 * ecos_reader -- Tries to read an incoming message from <REPLY or <EVENT to <END
 *
 * Returns an iONode with results if successful, NULL otherwise
 *
 */
iONode ecos_reader( iOSocket socket ) {

  iONode node = NULL;

  char tmp[ 1024 ]  = {'\0'};
  char* reply       = NULL;
  Boolean bstart    = False;
  Boolean bend      = False;

    /* While connected and haven't found end yet */

  while ( SocketOp.isConnected( socket ) && !bend ) {

    if ( SocketOp.peek( socket, tmp, 1 )) {

        /* read a line */

      if ( SocketOp.readln( socket, tmp )) {

          /* check for start of reply or event */
        TraceOp.trc( "ecosparser", TRCLEVEL_MONITOR, __LINE__, 9999, tmp );

        if ( !bstart ) {
          if ( StrOp.startsWith( tmp, "<REPLY" ) || StrOp.startsWith( tmp, "<EVENT" )) {
            bstart = True;
            reply = StrOp.cat( reply, tmp );    /* Add the line */

          }

          /* check if still in body of message */

        } else if ( !bend ) {

          if ( StrOp.startsWith( tmp, "<END" )) {

              /* found the end */

            bend  = True;
            reply = StrOp.cat( reply, tmp );    /* Add the line */

            break;

          } else {
            reply = StrOp.cat( reply, tmp );    /* Add the line */
          }
        }

      } else {

          /* Error, can't read line */

        TraceOp.trc( "ecosparser", TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: unable to read next line" );
        break;
      }
    }

      /* Wait for IO -- Farkle, we should really wait, not just sleep */

    ThreadOp.sleep(10);
  };

  TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999, "end of reading:\n%s", reply );

    /* Check for errors parsing the message */

  if ( !bstart && !bend ) {
    StrOp.free(reply);
    TraceOp.trc( "ecosparser", TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: unexpected end of stream" );
    return NULL;
  } else if ( bstart && !bend ) {
    StrOp.free(reply);
    TraceOp.trc( "ecosparser", TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR: unexpected end of reply" );
    return NULL;
  }

  node = ecos_parser( reply );
  StrOp.free(reply);

  return node;
}


/**
 * __parseCallParams -- breaks parameters out of a message coming from the ECoS?
 *
 * input:
 * output:
 *
 */
static void __parseCallParms( iONode node, const char* p_replyline ) {

  char replyline[ 1024 ]  = { '\0' };
  char* var               = NULL;
  char* val               = NULL;

  StrOp.copy( replyline, p_replyline );
  var = StrOp.findc( replyline, ',' );    /* skip past command */

  while ( var != NULL ) {
    char* tmp = NULL;

      /* skip comma */

    var++;

      /* skip white space */

    while ( var[ 0 ] == ' ' ) {
      var++;
    }

      /* Find the open bracket */

    tmp = StrOp.findc( var, '[' );
    if ( tmp != NULL ) {

      val = tmp + 1;

        /* end of var name */

      *tmp = '\0';

      tmp = StrOp.findc( val, ']' );
      if ( tmp != NULL ) {

        *tmp = '\0';
        NodeOp.setStr( node, var, val );
        var = StrOp.findc( tmp, ',' );

      } else {

          /* exception */

        TraceOp.trc( "ecosparser", TRCLEVEL_EXCEPTION, __LINE__, 9999,
                      "ERROR: unexpected format" );
        break;
      }
    } else {

        /* no params */

      TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999,
                    "no params found in reply" );
      break;
    }
  };

}


/**
 * __parseREPLY -- returns a reply in replyline
 *
 * input : <REPLY queryObjects(10, name)>
 * output: <reply cmd="queryObjects" oid="10">
 *
 */
static void __parseREPLY( iONode node, const char* replyline ) {

  char replytype[ 32 ]  = { '\0' };
  char replycmd[ 64 ]   = { '\0' };
  int oid               = 0;

  char *buffer = allocMem(StrOp.len(replyline)+1);

  char oidbuffer[ 64 ]  = { '\0' };
  char *bufptr;

  char tmp;

    /*

      Get the keyword and string remainder
      FARKLE -- sscanf sucks

    */

  sscanf( replyline, "<%s %s", replytype, buffer );
  TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999,
                "__parseReply: replytype = [%s], buffer = [%s]", replytype, buffer );

  int i       = 0;
  int donecmd = 0;
  bufptr      = buffer;
  while ( *bufptr ) {

      /* add the current char into the correct secondary buffer */

    if ( !donecmd )
      replycmd[ i++ ] = *bufptr++;
    else
      oidbuffer[ i++ ] = *bufptr++;

      /* Check for token end */

    if ( *bufptr == '(' ) {           /* command ends with '(', stop parsing command */

      replycmd[ i ] = '\0';
      bufptr++;

      donecmd       = 1;
      i             = 0;

    } else if ( *bufptr == ',' ) {    /* ECoS id ends with ',', stop parsing oid */

      oidbuffer[ i ] = '\0';
      break;                          /* skip rest of line */

    }
  }

    /*

      Get the oid from the oidbuffer
      FARKLE -- sscanf sucks

    */

  sscanf( oidbuffer, "%d", &oid );

    /* save the values into the node */

  NodeOp.setStr( node, "cmd", replycmd );
  NodeOp.setInt( node, "oid", oid );

  /* TraceOp.trc( "ecosparser", TRCLEVEL_MONITOR, __LINE__, 9999,
                "header: type=%s cmd=%s oid=%d", replytype, replycmd, oid ); */

    /* parse call parameters */

  __parseCallParms(node, replyline);

  /* clean up */
  freeMem(buffer);
}


/**
 * __parseEVENT -- Saves the oid for an incoming event in a given node
 *
 * input : <EVENT 10>
 * output: <event oid="10">
 */
static void __parseEVENT( iONode node, const char* replyline ) {

  char replytype[ 32 ]  = { '\0' };
  int oid               = 0;

  sscanf( replyline, "<%s %d>", replytype, &oid );
  NodeOp.setInt( node, "oid", oid );
  TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999,
                "__parseEvent: header: type=%s oid=%d", replytype, oid );
}


/**
 * __parseRow
 *
 * input  1: 1000 name[„Big Boy“]
 * input  2: 1000 speed[40]
 * output 1: <1000 name="Big Boy">
 * output 2: <1000 name="Big Boy" speed="40">
 *
 * Always check if a childnode already exist for the given OID.
 * All attributes are comming with a separate line.
 */
static void __parseRow( iONode node, const char* p_replyline ) {

  char replyline[ 1024 ]  = { '\0' };
  char oid[ 32 ]          = { '\0' };
  char* attr              = NULL;
  char* lastattr          = NULL;
  char* newlinepos        = NULL;
  iONode child            = NULL;

  TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999, "__parseRow: p_replyline =  [%s]", p_replyline );

    /* Check for non-existant node */

  if ( node == NULL ) {
    TraceOp.trc( "ecosparser", TRCLEVEL_EXCEPTION, __LINE__, 9999,
                  "unexpected NULL node" );
    return;
  }

    /* Check for empty reply */

  if ( p_replyline == NULL || StrOp.len( p_replyline ) == 0 ) {
    TraceOp.trc( "ecosparser", TRCLEVEL_WARNING, __LINE__, 9999, "no row!" );
    return;
  }

    /* Process the reply line */

  StrOp.copy( replyline, p_replyline );   /* Save the reply into the buffer */
  sscanf( replyline, "%s", oid );

    /* create child node if not already exist */

  child = NodeOp.findNode( node, oid );
  if ( child == NULL ) {
    /* TraceOp.trc( "ecosparser", TRCLEVEL_MONITOR, __LINE__, 9999, "Adding child for oid [%s]", oid ); */
    child = NodeOp.inst( oid, node, ELEMENT_NODE );
    NodeOp.addChild( node, child );
  } else {
    /* TraceOp.trc( "ecosparser", TRCLEVEL_MONITOR, __LINE__, 9999, "Found child for oid [%s]", oid ); */
  }

    /* Get all attributes from this line upto a line feed */

  attr        = StrOp.findc( replyline, ' ' );    /* skip past ID number */
  newlinepos  = StrOp.findc( replyline, '\n' );   /* find linefeed */

  if( newlinepos == NULL )
    newlinepos = replyline + StrOp.len(replyline);

  while ( attr != NULL ) {
    if ( attr != NULL ) {

      int len   = 0;
      int i     = 0;
      char* val = NULL;

        /* Read up to '[' */

      attr++;                          /* skip blank */
      len = StrOp.len( attr );
      for ( i = 0; i < len; i++ ) {
        if ( attr[ i ] == '[' ) {
          attr[ i ] = '\0';           /* end attribute name */
          val = attr + i + 1;
          lastattr  = val;
          break;
        }
      }

        /* Check for empty string after name */

      if ( val == NULL ) {

          /* unexpected */

        TraceOp.trc( "ecosparser", TRCLEVEL_EXCEPTION, __LINE__, 9999, "not well formed, ends with '['" );
        return;
      }

        /* determine attribute value */

      len = StrOp.len( val );
      for ( i = 0; i < len; i++ ) {
        if ( val[ i ] == ']' ) {
          val[ i ] = '\0';         /* end attribute value */
          lastattr  = val + i + 1;
          break;
        }
      }

        /* set attribute */
        /* Note:  want to add name to this node, not addr */

      NodeOp.setStr( child, attr, val );
      TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999,
                    "added attribute %s=%s to oid %s", attr, val, oid );

    }

    attr = StrOp.findc( lastattr, ' ' );   /* Get to next space */
    if ( attr > newlinepos ) {
      attr = NULL;
    }
 }
}


/**
 * __parseEND
 *
 * input : <END 0 (OK)>
 * output: <reply cmd="queryObjects" oid="10" rc="0" msg="OK">
 */
static void __parseEND( iONode node, const char* replyline ) {

  char replytype[ 32 ]  = { '\0' };
  char msg[ 256 ]       = {'\0'};
  int rc                = 0;

  sscanf( replyline, "<%s %d (%s)>", replytype, &rc, msg );
  NodeOp.setInt( node, "rc", rc );
  NodeOp.setStr( node, "msg", msg );
  TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999,
                "trailer: rc=%d msg=%s", rc, msg );
}


/**
 * ecos_parser -- parse the reply and put it in a node.
 *
 *    <REPLY/EVENT set/get/create/delete/request/release/link/unlink/queryObjects(ID, var[val], ...)>
 *    ID var[val], ...
 *    <END rc (string)>
 */
iONode ecos_parser( const char* reply ) {

  iONode node           = NULL;
  int line              = 0;
  int linelen           = 0;
  int lines             = 0;
  const char* replyline = NULL;

    /* Check for empty reply */

  if ( reply == NULL || StrOp.len( reply ) == 0 ) {
    TraceOp.trc( "ecosparser", TRCLEVEL_WARNING, __LINE__, 9999, "no reply" );
    return NULL;
  }

    /* Dump the reply to the console for byte tracing */
  TraceOp.setDumpsize(NULL, StrOp.len( reply ));
  TraceOp.dump( NULL, TRCLEVEL_BYTE, ( char* )reply, StrOp.len( reply ));

    /* Get number of lines and full buffer */

  lines     = StrOp.getCntLinefeeds( reply );
  replyline = reply;
  TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999, "lines=%d", lines );

    /* For each line */

  for ( line = 0; line < lines; line++ ) {
    /* make a copy and replace line feeds with zero's because we only want to parse one line */
    char* cpreplyline = StrOp.dup(replyline);
    StrOp.replaceAll( cpreplyline, '\n', '\0' );

    TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999, "parsing line: %s", cpreplyline );
    if ( node == NULL && StrOp.startsWith( replyline, "<REPLY" )) {

        /* This is start of reply */

      TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999, "parsing reply..." );
      node = NodeOp.inst( "reply", NULL, ELEMENT_NODE );
      NodeOp.setInt( node, "rtype", REPLY_TYPE_REPLY );
      __parseREPLY( node, cpreplyline );

    } else if ( node == NULL && StrOp.startsWith( replyline, "<EVENT" )) {

        /* This is start of event */

      TraceOp.trc( "ecosparser", TRCLEVEL_INFO, __LINE__, 9999, "parsing event..." );
      node = NodeOp.inst( "event", NULL, ELEMENT_NODE );
      NodeOp.setInt( node, "rtype", REPLY_TYPE_EVENT );
      __parseEVENT( node, cpreplyline );

    } else if ( node != NULL &&  StrOp.startsWith( replyline, "<END" )) {

        /* This is end of reply or event */

      TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999, "parsing end..." );
      __parseEND( node, cpreplyline );

    } else {

        /* Body line from reply or event */

      TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999, "parsing row..." );
      __parseRow( node, cpreplyline );

    }

      /* Get the next line */

    if ( replyline != NULL ) {
      StrOp.free( cpreplyline);
      TraceOp.trc( "ecosparser", TRCLEVEL_DEBUG, __LINE__, 9999, "next line..." );
      replyline = StrOp.getNextLine( replyline, &linelen );

    } else {

        /* ERROR: unexpected end in middle of body */

      TraceOp.trc( "ecosparser", TRCLEVEL_WARNING, __LINE__, 9999, "WARNING: unexpected end" );
      break;

    }

      /* Sleep for next line */

    ThreadOp.sleep(10);
  }

  return node;
}
