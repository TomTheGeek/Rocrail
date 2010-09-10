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

#include "rocdigs/impl/srcp07_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"


#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/Program.h"


static int instCnt = 0;

static Boolean __srcpConnect ( iOSRCP07Data o );
static int __srcpSendCommand( iOSRCP07Data o, Boolean recycle, const char* szCommand, char *szRetVal);

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
  iOSRCP07Data data = Data(inst);
  return (char*)data->iid;
}
static void __del(void* inst) {
  iOSRCP07Data data = Data(inst);
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
***** Public functions.
*/
static Boolean _setListener( obj inst, obj listenerObj,
              const digint_listener listenerFun )
{
  iOSRCP07Data data = Data(inst);
  return True;
}

static int ACKok(iOSocket sckt) {
   char buf[20];
   int  ack;

   memset(buf,0,20);
   SocketOp.read(sckt,buf,13);

   if (strncmp(buf, "INFO -1",8)==0) return 2;

   ack=atoi(buf+11);

   return ack;
}

/**
* @param node <sw unit="1" pin="1" cmd="straight"/>
* @param node <lc addr="1" prot="M" dir="true" V="0" V_max="100" V_min="10" V_mode="kmh" fn="false"/>
* @param node <fn addr="1" f1="false" f2="false" f3="false" f4="false"/>
* @param node <fb unit="1"/>
* @param node <sys cmd="stop"/>
* @param srcp Request buffer.
*/
static iONode __translate( iOSRCP07Data o, iONode node, char* srcp )
{
  char tmpCommand[1024];
  iONode rsp = NULL;
  int i;
  srcp[0] = '\0';

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Translating command.");
  /* Switch command. */
  if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int mod = wSwitch.getaddr1( node );
    int pin  = wSwitch.getport1( node );
    int addr = 0;
    int port  = 1;
    int action = 1;
    int activationTime = 500;

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      port = 0;

    addr = (mod-1)*4+pin;

    sprintf(srcp,"SET GA %s %d %d %d %d\n", wSwitch.getprot( node ), addr, port, action, activationTime);
    return NULL;
  }
  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int mod = wOutput.getaddr( node );
    int pin  = wOutput.getport( node );
    int addr = 0;
    int port = wOutput.getgate( node );
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 1:0;
    int activationTime = -1;

    addr = (mod-1)*4+pin;

    sprintf(srcp,"SET GA %s %d %d %d %d\n", wOutput.getprot( node ), addr, port, action, activationTime);
    return NULL;
  }
  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
    return NULL;
  }

  /* Loc or Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ||
           StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) )
  {
    int fncnt = wLoc.getfncnt( node );
    const char* prot = wLoc.getprot(node);
    int V_max = wLoc.getV_max( node );
    int speed = wLoc.getV(node) != -1 ? wLoc.getV(node):0;

    if( StrOp.equals( wLoc.prot_L, wLoc.getprot( node ) ) ) {
      prot = wLoc.prot_N;
    }

    if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
      V_max = 100;

    if( fncnt > 12 )
      fncnt = 12;

    sprintf( srcp, "SET GL %s%d %d %d %d %d %d %d",
      prot, wLoc.getprotver(node), wLoc.getaddr(node), wLoc.isdir( node )?1:0,
      speed, V_max, wLoc.isfn(node)? 1:0, fncnt );

    if( fncnt > 0 )
      wFunCmd.isf1 (node) ? strcat(srcp," 1"):strcat(srcp," 0");
    if( fncnt > 1 )
      wFunCmd.isf2 (node) ? strcat(srcp," 1"):strcat(srcp," 0");
    if( fncnt > 2 )
      wFunCmd.isf3 (node) ? strcat(srcp," 1"):strcat(srcp," 0");
    if( fncnt > 3 )
      wFunCmd.isf4 (node) ? strcat(srcp," 1"):strcat(srcp," 0");
    if( fncnt > 4 )
      wFunCmd.isf5 (node) ? strcat(srcp," 1"):strcat(srcp," 0");
    if( fncnt > 5 )
      wFunCmd.isf6 (node) ? strcat(srcp," 1"):strcat(srcp," 0");
    if( fncnt > 6 )
      wFunCmd.isf7 (node) ? strcat(srcp," 1"):strcat(srcp," 0");
    if( fncnt > 7 )
      wFunCmd.isf8 (node) ? strcat(srcp," 1"):strcat(srcp," 0");
    if( fncnt > 8 )
      wFunCmd.isf9 (node) ? strcat(srcp," 1"):strcat(srcp," 0");
    if( fncnt > 9 )
      wFunCmd.isf10(node) ? strcat(srcp," 1"):strcat(srcp," 0");
    if( fncnt > 10 )
      wFunCmd.isf11(node) ? strcat(srcp," 1"):strcat(srcp," 0");
    if( fncnt > 11 )
      wFunCmd.isf12(node) ? strcat(srcp," 1"):strcat(srcp," 0");

    strcat(srcp,"\n");

    return NULL;
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) )
  {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) )
    {
      sprintf(tmpCommand,"SET POWER OFF\n");
      __srcpSendCommand(o,False,tmpCommand,NULL);
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) )
    {
      sprintf(tmpCommand,"SET POWER ON\n");
      __srcpSendCommand(o,False,tmpCommand,NULL);
    }

    return NULL;
  }
  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    char buf[160];
    rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    if( o->iid != NULL )
      wProgram.setiid( rsp, o->iid );
    wProgram.setcmd( rsp, wProgram.getcmd( node ) );

    if( wProgram.getcmd( node ) == wProgram.set ) {
      int cv = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int ack = 0;
      Boolean pom = wProgram.ispom( node );
      if( !pom ) {
        sprintf(tmpCommand,"SET POWER OFF\n");
        __srcpSendCommand(o,False,tmpCommand,NULL);
      }
      sprintf (tmpCommand, "WRITE GL NMRA CV %d %d\n", cv-1, value );
      __srcpSendCommand(o,False,tmpCommand,NULL);

       wProgram.setvalue( rsp, value );
      if( !pom ) {
        ack = ACKok(o->cmdSocket);
        if( ack != 1 )
          wProgram.setvalue( rsp, -1 );
      }
      return rsp;
    }
    else if( wProgram.getcmd( node ) == wProgram.get ) {
      int ack = 0;
      int value = 0;
      int cv = wProgram.getcv( node );

      sprintf(tmpCommand,"SET POWER OFF\n");
      __srcpSendCommand(o,False,tmpCommand,NULL);

      for( value = 0; value < 256 && !ack; value++ ) {
        sprintf (tmpCommand, "VERIFY GL NMRA CV %d %d\n", cv-1, value );
        __srcpSendCommand(o,False,tmpCommand,NULL);
        ack = ACKok(o->cmdSocket);
      }
      if( ack == 1 ) {
        wProgram.setvalue( rsp, value-1 );
      }
      else {
        wProgram.setvalue( rsp, -1 );
      }
      return rsp;
    }
  }
  return NULL;
}

static iONode _cmd( obj inst, const iONode nodeA ) {
  iOSRCP07Data data = Data(inst);
  iONode response = NULL;
  char srcp[1024] = {0};

  if( TraceOp.getLevel(NULL) & TRCLEVEL_DEBUG ) {
    char* str = NodeOp.base.toString( nodeA );
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Received command:\n%s", str);
    StrOp.free( str );
  }

  response = __translate( data, nodeA, srcp );

  if( strlen( srcp ) > 0 )
    __srcpSendCommand(data, True, srcp, NULL);

  /* Cleanup Node1 */
  NodeOp.base.del(nodeA);


  return response;
}

static void _halt( obj inst, Boolean poweroff ) {
  iOSRCP07Data data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
  __srcpSendCommand( data, False, "LOGOUT", NULL );
}

static Boolean _supportPT( obj inst ) {
  iOSRCP07Data data = Data(inst);
  return True;
}


static int __srcpSendCommand( iOSRCP07Data o, Boolean recycle, const char* szCommand, char *szRetVal)
{
  char inbuf[ 1024 ]    = { 0 };
  char tracestr[ 1024 ] = { 0 };
  char szResponse[ 1024 ];
  int  retstate = 0;

  if (szRetVal)
    szRetVal[0]= '\0';
  if ((o->cmdSocket == NULL) || (!SocketOp.isConnected( o->cmdSocket )))
  {
    if (recycle)
    {
      if (__srcpConnect(o))
        return __srcpSendCommand(o, False, szCommand, szRetVal);
    }
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "not connected in SendCommand");
    return -1;
  }

  if ( !SocketOp.write( o->cmdSocket, szCommand, ( int )strlen( szCommand ))) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not send: %s", szCommand );
    return -1;
  } else {
    strncpy( tracestr, szCommand, ( strlen( szCommand ) - 1 ));
    tracestr[ strlen( szCommand ) ] = '0';
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Sent %s", tracestr );
  }

  /* No reading in 0.7.3 for now */
  if (True)
  {
    return 200;
  }
}

static Boolean __srcpConnect ( iOSRCP07Data o )
{
  char inbuf[1024]; /* Will be enough. spec says, no line longer than 1000 chars. */
  char *token;
  char id[1024],data[1024];
  /*Boolean found = False;*/

  return True;
}

/* Status */
static int _state( obj inst ) {
  iOSRCP07Data data = Data(inst);
  int state = 0;
  return state;
}

/* external shortcut event */
static void _shortcut(obj inst) {
  iOSRCP07Data data = Data( inst );
}


/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOSRCP07Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

static iOSRCP07 _inst( const iONode settings, const iOTrace trace, const iOSocket cmdsocket ) {
  iOSRCP07     srcp = allocMem( sizeof( struct OSRCP07 ) );
  iOSRCP07Data data = allocMem( sizeof( struct OSRCP07Data ) );
  int i=0;

  TraceOp.set( trace );

  /* OBase */
  MemOp.basecpy( srcp, &SRCP07Op, 0, sizeof( struct OSRCP07 ), data );

  data->ini = settings;
  data->cmdSocket = cmdsocket;

  /* Evaluate attributes. */
  data->iid       = StrOp.dup( wDigInt.getiid( settings ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "srcp07 %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  __srcpConnect( data );
  instCnt++;

  return srcp;
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/srcp07.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
