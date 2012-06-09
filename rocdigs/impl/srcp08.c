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

#include "rocdigs/impl/srcp08_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/strtok.h"
#include "rocs/public/system.h"


#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SRCP.h"
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

#define SRCP_OK(x) ( x < 400 )
#define SRCP_ERROR(x) ( x >= 400 )

static int __srcpSendCommand( iOSRCP08Data o, Boolean recycle, const char* szCommand, char *szRetVal);
static int __srcpInitServer( iOSRCP08Data o);
static Boolean __srcpConnect ( iOSRCP08Data o );

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
  iOSRCP08Data data = Data(inst);
  return (char*)data->iid;
}
static void __del(void* inst) {
  iOSRCP08Data data = Data(inst);
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
  iOSRCP08Data data = Data(inst);
  return True;
}


static Boolean _setRawListener(obj inst, obj listenerObj, const digint_rawlistener listenerFun ) {
  return True;
}

static byte* _cmdRaw( obj inst, const byte* cmd ) {
  return NULL;
}


static Boolean __initGA( iOSRCP08Data o, iONode node, int ga_bus, int addr ) {
  char tmpCommand[1024];
  sprintf(tmpCommand,"GET %d GA %d 0\n", ga_bus, addr );
  if (__srcpSendCommand(o, True, tmpCommand,NULL) != 100 ) {
    sprintf(tmpCommand,"INIT %d GA %d %s\n", ga_bus, addr, wSwitch.getprot( node ) );
    if (!__srcpSendCommand(o, True, tmpCommand,NULL))
    {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Problem initializing GA %d", addr );
      return False;
    }
    sprintf(tmpCommand,"GET %d GA %d 0\n", ga_bus, addr );
    if (!__srcpSendCommand(o, True, tmpCommand,NULL))
    {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Problem initializing GA %d", addr);
      return False;
    }
  }
  return True;
}


static Boolean __initGL( iOSRCP08Data o, iONode node, int* bus ) {
  char tmpCommand[1024];

  int gl_bus = 0;
  const char* prot = wLoc.getprot( node );

  gl_bus = wSRCP.getsrcpbusGL_ps( o->srcpini );
  if( StrOp.equals( wLoc.prot_M, wLoc.getprot( node ) ) )
    gl_bus = wSRCP.getsrcpbusGL_m( o->srcpini );
  else if( StrOp.equals( wLoc.prot_N, wLoc.getprot( node ) ) )
    gl_bus = wSRCP.getsrcpbusGL_ns( o->srcpini );
  else if( StrOp.equals( wLoc.prot_L, wLoc.getprot( node ) ) ) {
    gl_bus = wSRCP.getsrcpbusGL_nl( o->srcpini );
    prot = wLoc.prot_N;
  }

  *bus = gl_bus;

  if (! o->locInited[wLoc.getaddr(node)] )
  {

    sprintf(tmpCommand,"GET %d GL %d\n", gl_bus, wLoc.getaddr(node) );
    if( __srcpSendCommand(o, True, tmpCommand,NULL) != 100 ) {

      /* 14 Speedsteps and 4 Funs hardcoded for now. DDW ignores this values */
      sprintf(tmpCommand,"INIT %d GL %d %s %d %d %d\n", gl_bus,
              wLoc.getaddr(node), prot, wLoc.getprotver( node ),
              wLoc.getspcnt( node ), wLoc.getfncnt( node ) + 1 );

      if (!__srcpSendCommand(o, True, tmpCommand,NULL))
      {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Problem initializing GL %d", wLoc.getaddr(node));
        return False;
      }
    }
    o->locInited[wLoc.getaddr(node)] = True;

    sprintf(tmpCommand,"GET %d GL %d\n", gl_bus, wLoc.getaddr(node) );

    o->locInited[wLoc.getaddr(node)] = True;

    if (!__srcpSendCommand(o, True, tmpCommand,NULL))
    {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Problem getting GL %d", wLoc.getaddr(node));
      return False;
    }
  }
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
static iONode __translate( iOSRCP08Data o, iONode node, char* srcp )
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
    int activationTime = wDigInt.getswtime( o->ini );
    if( wSwitch.getdelay(node) > 0 )
      activationTime = wSwitch.getdelay(node);

    int ga_bus = wSRCP.getsrcpbusGA_ps( o->srcpini );
    if( StrOp.equals( wSwitch.prot_M, wSwitch.getprot( node ) ) )
      ga_bus = wSRCP.getsrcpbusGA_m( o->srcpini );
    else if( StrOp.equals( wSwitch.prot_N, wSwitch.getprot( node ) ) )
      ga_bus = wSRCP.getsrcpbusGA_n( o->srcpini );

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
      port = 0;

    addr = (mod-1)*4+pin;

    if (! o->knownSwitches[ addr ] )
    {
      o->knownSwitches[ addr ] = __initGA(o, node, ga_bus, addr);
    }

    if ( o->knownSwitches[ addr ] )
      sprintf( srcp, "SET %d GA %d %d %d %d\n", ga_bus, addr, port, action, activationTime );
    return NULL;
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int mod = wOutput.getaddr( node );
    int pin = wOutput.getport( node );
    int addr = 0;
    int port = wOutput.getgate( node );
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 1:0;
    int activationTime = -1;

    int ga_bus = wSRCP.getsrcpbusGA_ps( o->srcpini );
    if( StrOp.equals( wOutput.prot_M, wOutput.getprot( node ) ) )
      ga_bus = wSRCP.getsrcpbusGA_m( o->srcpini );
    else if( StrOp.equals( wOutput.prot_N, wOutput.getprot( node ) ) )
      ga_bus = wSRCP.getsrcpbusGA_n( o->srcpini );

    addr = (mod-1)*4+pin;

    if (! o->knownSwitches[ addr ] )
    {
      o->knownSwitches[ addr ] = __initGA(o, node, ga_bus, addr);
    }

    /* send the output command... */
    if ( o->knownSwitches[ addr ] )
      sprintf( srcp, "SET %d GA %d %d %d %d\n", ga_bus, addr, port, action, activationTime );

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
    int bus;
    int V_max = wLoc.getV_max( node );
    int speed = wLoc.getV(node) != -1 ? wLoc.getV(node):0;

    if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
      V_max = 100;

    if( !__initGL( o, node, &bus ) )
      return NULL;

    sprintf( srcp, "SET %d GL %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
        bus,
        wLoc.getaddr( node ),
        wLoc.isdir( node ),
        speed,
        V_max,
        wLoc.isfn(node),
        wFunCmd.isf1(node),
        wFunCmd.isf2(node),
        wFunCmd.isf3(node),
        wFunCmd.isf4(node),
        wFunCmd.isf5(node),
        wFunCmd.isf6(node),
        wFunCmd.isf7(node),
        wFunCmd.isf8(node),
        wFunCmd.isf9(node),
        wFunCmd.isf10(node),
        wFunCmd.isf11(node),
        wFunCmd.isf12(node) );

    return NULL;
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) )
  {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) || StrOp.equals( cmd, wSysCmd.ebreak ) )
    {
      sprintf(tmpCommand,"SET 1 POWER OFF\n");
      __srcpSendCommand(o,False,tmpCommand,NULL);
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) )
    {
      sprintf(tmpCommand,"SET 1 POWER ON\n");
      __srcpSendCommand(o,False,tmpCommand,NULL);
    }
    return NULL;
  }

  /* Program command. */
  /* ToDo: after sending a set cmd, an OK should be come as reply... */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    if( o->iid != NULL )
      wProgram.setiid( rsp, o->iid );
    wProgram.setcmd( rsp, wProgram.getcmd( node ) );

    if( !wProgram.ispom( node ) ) {
      sprintf(tmpCommand,"SET 1 POWER OFF\n");
      __srcpSendCommand(o,False,tmpCommand,NULL);
    }

    if( wProgram.getcmd( node ) == wProgram.set ) {
      int addr = wProgram.getaddr( node );
      int cv = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int ack = 0;
      sprintf (tmpCommand, "SET %d SM %d CV %d %d\n", wSRCP.getsrcpbusGL_ns( o->srcpini ), addr, cv-1, value );
      __srcpSendCommand(o,False,tmpCommand,NULL);
      ack = ACKok(o->cmdSocket);
      if( ack == 1 ) {
        wProgram.setvalue( rsp, value );
      }
      else {
        wProgram.setvalue( rsp, -1 );
      }
    }
    else if( wProgram.getcmd( node ) == wProgram.get ) {
      int addr = wProgram.getaddr( node );
      int cv = wProgram.getcv( node );
      int value = 0;
      int ack = 0;
      for( value = 0; value < 256 && !ack; value++ ) {
        sprintf (tmpCommand, "GET %d SM %d CV %d %d\n", wSRCP.getsrcpbusGL_ns( o->srcpini ), addr, cv-1, value );
        __srcpSendCommand(o,False,tmpCommand,NULL);
        ack = ACKok(o->cmdSocket);
      }
      if( ack == 1 ) {
        wProgram.setvalue( rsp, value-1 );
      }
      else {
        wProgram.setvalue( rsp, -1 );
      }
    }
    return rsp;
  }
  return NULL;
}

static iONode _cmd( obj inst, const iONode nodeA ) {
  iOSRCP08Data data = Data(inst);
  iONode response = NULL;
  char srcp[1024] = {0};
  char* str = NodeOp.base.toString( nodeA );

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Received command:\n%s", str);
  StrOp.free( str );
  response = __translate( data, nodeA, srcp );

  if( strlen( srcp ) > 0 )
    __srcpSendCommand(data, True, srcp, NULL);

  /* Cleanup Node1 */
  NodeOp.base.del(nodeA);

  return response;
}

static void _halt( obj inst, Boolean poweroff ) {
  iOSRCP08Data data = Data(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
}

static Boolean _supportPT( obj inst ) {
  iOSRCP08Data data = Data(inst);
  return True;
}


static int __srcpSendCommand( iOSRCP08Data o, Boolean recycle, const char* szCommand, char *szRetVal)
{
  char inbuf[1024] = { 0 };
  char szResponse[1024];
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

  if (!SocketOp.write( o->cmdSocket, szCommand, (int)strlen(szCommand)))
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not send: %s", szCommand );
    return -1;
  }
  else
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sent: %s",szCommand);

  /* Read server response: */
  if (! SocketOp.readln(o->cmdSocket,inbuf) )
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SendCommand: could not read response");
    return -1;
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "in: %s",inbuf);

  /* Scan for SM return? */
  MemOp.set(szResponse,0,900);
  sscanf(inbuf,"%*s %d %900c",&retstate,szResponse);

  if (!SRCP_OK(retstate))
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SRCP Response: %s",szResponse);
  else
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCP Response: %s",szResponse);


  o->state = (SRCP_OK(retstate)?SRCP_STATE_OK:SRCP_STATE_ERROR);

  if (szRetVal)
    strcpy(szRetVal,szResponse);

  return retstate;
}

static Boolean __srcpConnect ( iOSRCP08Data o )
{
  char inbuf[1024]; /* Will be enough. spec says, no line longer than 1000 chars. */
  char *token;
  char id[1024],data[1024];
  /*Boolean found = False;*/


  if (SRCP_ERROR( __srcpSendCommand(o,False,"SET PROTOCOL SRCP 0.8\n",data)))
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: %s",data);
    SocketOp.disConnect(o->cmdSocket);
    return False;
  }
  if (SRCP_ERROR( __srcpSendCommand(o,False,"SET CONNECTIONMODE SRCP COMMAND\n",data)))
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: %s",data);
    SocketOp.disConnect(o->cmdSocket);
    return False;
  }
  if (SRCP_ERROR( __srcpSendCommand(o,False,"GO\n",data)))
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: %s",data);
    SocketOp.disConnect(o->cmdSocket);
    return False;
  }


  __srcpInitServer(o);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Handshake completed.");
  return True;
}

static int __srcpInitServer( iOSRCP08Data o)
{
  int i;
  char tmpCommand[1024];
  sprintf(tmpCommand,"GET 1 POWER\n");
  if( __srcpSendCommand(o,False,tmpCommand,NULL) != 100 ) {
    sprintf(tmpCommand,"INIT 1 POWER\n");
    __srcpSendCommand(o,False,tmpCommand,NULL);
  }
  return 0;
}

/* Status */
static int _state( obj inst ) {
  iOSRCP08Data data = Data(inst);
  int state = 0;
  return state;
}

/* external shortcut event */
static void _shortcut(obj inst) {
  iOSRCP08Data data = Data( inst );
}


/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOSRCP08Data data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

static iOSRCP08 _inst( const iONode settings, const iOTrace trace, const iOSocket cmdsocket ) {
  iOSRCP08     srcp = allocMem( sizeof( struct OSRCP08 ) );
  iOSRCP08Data data = allocMem( sizeof( struct OSRCP08Data ) );
  int i=0;

  TraceOp.set( trace );

  /* OBase */
  MemOp.basecpy( srcp, &SRCP08Op, 0, sizeof( struct OSRCP08 ), data );

  data->ini = settings;
  data->srcpini = wDigInt.getsrcp(data->ini);

  if( data->srcpini == NULL ) {
    data->srcpini = NodeOp.inst( wSRCP.name(), data->ini, ELEMENT_NODE );
    NodeOp.addChild(data->ini, data->srcpini );
  }

  data->cmdSocket = cmdsocket;

  data->knownSwitches = allocMem( 256 * sizeof(Boolean) );
  MemOp.set(data->knownSwitches,0,256*sizeof(Boolean));

  data->knownLocos = allocMem( 256 * sizeof(Boolean));
  MemOp.set( data->knownLocos, 0, 256*sizeof(Boolean));

  data->locInited = allocMem( 256 * sizeof(Boolean));
  MemOp.set( data->locInited, 0, 256*sizeof(Boolean));

  /* Evaluate attributes. */
  data->iid       = StrOp.dup( wDigInt.getiid( settings ) );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "srcp08 %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  __srcpConnect( data );
  instCnt++;

  return srcp;
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/srcp08.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
