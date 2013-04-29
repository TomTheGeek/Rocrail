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
  StrOp.fmtb(tmpCommand, "GET %d GA %d 0\n", ga_bus, addr );
  if (__srcpSendCommand(o, True, tmpCommand,NULL) != 100 ) {
    StrOp.fmtb(tmpCommand,"INIT %d GA %d %s\n", ga_bus, addr, wSwitch.getprot( node ) );
    if (!__srcpSendCommand(o, True, tmpCommand,NULL))
    {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Problem initializing GA %d", addr );
      return False;
    }
    StrOp.fmtb(tmpCommand,"GET %d GA %d 0\n", ga_bus, addr );
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

  if( wLoc.getaddr(node) > 16383) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "loco address out of range: %d", wLoc.getaddr(node));
    return False;
  }

  if (! o->locInited[wLoc.getaddr(node)] )
  {

    StrOp.fmtb(tmpCommand,"GET %d GL %d\n", gl_bus, wLoc.getaddr(node) );
    if( __srcpSendCommand(o, True, tmpCommand,NULL) != 100 ) {

      StrOp.fmtb(tmpCommand,"INIT %d GL %d %s %d %d %d\n", gl_bus,
              wLoc.getaddr(node), prot, wLoc.getprotver( node ),
              wLoc.getspcnt( node ), wLoc.getfncnt( node ) + 1 );

      if (!__srcpSendCommand(o, True, tmpCommand,NULL))
      {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Problem initializing GL %d", wLoc.getaddr(node));
        return False;
      }
    }
    o->locInited[wLoc.getaddr(node)] = True;

    StrOp.fmtb(tmpCommand,"GET %d GL %d\n", gl_bus, wLoc.getaddr(node) );

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

   MemOp.set( buf, 0, 20 );
   SocketOp.read( sckt, buf, 13 );

   if( StrOp.startsWithi( buf, "INFO -1" ) )
     return 2;

   ack = atoi( buf+11 );

   return ack;
}


/**
 * create a bitmap (int, max 31) of used srcp buses
 */
static int __getSRCPbusList( iOSRCP08Data o ) {
  int busList = 0;
  
  if( 0 != wSRCP.getsrcpbus_server( o->srcpini ) ) {
    /* Ouch, server must use value 0 */
  }
  busList |= ( 1 << wSRCP.getsrcpbusGL_m( o->srcpini ) ) ;
  busList |= ( 1 << wSRCP.getsrcpbusGL_ns( o->srcpini ) ) ;
  busList |= ( 1 << wSRCP.getsrcpbusGL_nl( o->srcpini ) ) ;
  busList |= ( 1 << wSRCP.getsrcpbusGL_ps( o->srcpini ) ) ;
  busList |= ( 1 << wSRCP.getsrcpbusGA_m( o->srcpini ) ) ;
  busList |= ( 1 << wSRCP.getsrcpbusGA_n( o->srcpini ) ) ;
  busList |= ( 1 << wSRCP.getsrcpbusGA_ps( o->srcpini ) ) ;
  busList |= ( 1 << wSRCP.getsrcpbusFB_s88( o->srcpini ) ) ;
  busList |= ( 1 << wSRCP.getsrcpbusFB_m6051( o->srcpini ) ) ;
  busList |= ( 1 << wSRCP.getsrcpbusFB_i8255( o->srcpini ) ) ;
  /* clean bit for server bus 0 (in case some buses are not used and were still on default 0) */
  busList &= ~1 ;

  return busList;
}

/**
 *
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

    if( addr > 1023 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "switch address out of range: %d:%d", mod, pin);
      return NULL;
    }

    if (! o->knownSwitches[ addr ] )
    {
      o->knownSwitches[ addr ] = __initGA(o, node, ga_bus, addr);
    }

    if ( o->knownSwitches[ addr ] )
      StrOp.fmtb( srcp, "SET %d GA %d %d %d %d\n", ga_bus, addr, port, action, activationTime );
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

    if( addr > 1023 ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "output address out of range: %d:%d", mod, pin);
      return NULL;
    }

    if (! o->knownSwitches[ addr ] )
    {
      o->knownSwitches[ addr ] = __initGA(o, node, ga_bus, addr);
    }

    /* send the output command... */
    if ( o->knownSwitches[ addr ] )
      StrOp.fmtb( srcp, "SET %d GA %d %d %d %d\n", ga_bus, addr, port, action, activationTime );

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

    int loSpcnt = wLoc.getspcnt(node);
    int currStep = (speed * loSpcnt) / V_max;    
    int loFnCnt = wLoc.getfncnt(node);
    int loFx = 0;

    if( wFunCmd.isf1( node) ) loFx |= 0x00000001;
    if( wFunCmd.isf2( node) ) loFx |= 0x00000002;
    if( wFunCmd.isf3( node) ) loFx |= 0x00000004;
    if( wFunCmd.isf4( node) ) loFx |= 0x00000008;
    if( wFunCmd.isf5( node) ) loFx |= 0x00000010;
    if( wFunCmd.isf6( node) ) loFx |= 0x00000020;
    if( wFunCmd.isf7( node) ) loFx |= 0x00000040;
    if( wFunCmd.isf8( node) ) loFx |= 0x00000080;
    if( wFunCmd.isf9( node) ) loFx |= 0x00000100;
    if( wFunCmd.isf10(node) ) loFx |= 0x00000200;
    if( wFunCmd.isf11(node) ) loFx |= 0x00000400;
    if( wFunCmd.isf12(node) ) loFx |= 0x00000800;
    if( wFunCmd.isf13(node) ) loFx |= 0x00001000;
    if( wFunCmd.isf14(node) ) loFx |= 0x00002000;
    if( wFunCmd.isf15(node) ) loFx |= 0x00004000;
    if( wFunCmd.isf16(node) ) loFx |= 0x00008000;
    if( wFunCmd.isf17(node) ) loFx |= 0x00010000;
    if( wFunCmd.isf18(node) ) loFx |= 0x00020000;
    if( wFunCmd.isf19(node) ) loFx |= 0x00040000;
    if( wFunCmd.isf20(node) ) loFx |= 0x00080000;
    if( wFunCmd.isf21(node) ) loFx |= 0x00100000;
    if( wFunCmd.isf22(node) ) loFx |= 0x00200000;
    if( wFunCmd.isf23(node) ) loFx |= 0x00400000;
    if( wFunCmd.isf24(node) ) loFx |= 0x00800000;
    if( wFunCmd.isf25(node) ) loFx |= 0x01000000;
    if( wFunCmd.isf26(node) ) loFx |= 0x02000000;
    if( wFunCmd.isf27(node) ) loFx |= 0x04000000;
    if( wFunCmd.isf28(node) ) loFx |= 0x08000000;

    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "__translate: lc/fn loFnCnt[%d] loFx[0x%08.8X] (manual)",
        loFnCnt, loFx );

    char funcString[1023] = {'\0'};
    int i, mask;
    for( i=0 ; i < loFnCnt ; i++ ) {
      mask = 1 << i ;
      funcString[2*i] = ' ';
      funcString[2*i+1] = loFx&mask?'1':'0';
    }
    funcString[2*loFnCnt] = '\0';

    StrOp.fmtb( srcp, "SET %d GL %d %d %d %d %d%s\n",
        bus,
        wLoc.getaddr( node ),
        wLoc.isdir( node ),
        currStep,
        wLoc.getspcnt(node),
        wLoc.isfn(node)?1:0,
        funcString );

    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "__translate: lc/fn [%s]", srcp );

    return NULL;
  }

  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) )
  {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) || StrOp.equals( cmd, wSysCmd.ebreak ) )
    {
      int busList = __getSRCPbusList( o );
      int intBits = sizeof( int ) * 8 ;
      int i;

      for( i = 0 ; i < intBits ; i++ ) {
        if( busList & ( 1 << i ) ) {
          StrOp.fmtb(tmpCommand,"SET %d POWER OFF\n", i );
          __srcpSendCommand(o,False,tmpCommand,NULL);
        }
      }
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) )
    {
      int busList = __getSRCPbusList( o );
      int intBits = sizeof( int ) * 8 ;
      int i;

      for( i = 0 ; i < intBits ; i++ ) {
        if( busList & ( 1 << i ) ) {
          StrOp.fmtb(tmpCommand,"SET %d POWER ON\n", i );
          __srcpSendCommand(o,False,tmpCommand,NULL);
        }
      }
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
      StrOp.fmtb(tmpCommand,"SET 1 POWER OFF\n");
      __srcpSendCommand(o,False,tmpCommand,NULL);
    }

    if( wProgram.getcmd( node ) == wProgram.set ) {
      int addr = wProgram.getaddr( node );
      int cv = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int ack = 0;
      StrOp.fmtb (tmpCommand, "SET %d SM %d CV %d %d\n", wSRCP.getsrcpbusGL_ns( o->srcpini ), addr, cv-1, value );
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
        StrOp.fmtb (tmpCommand, "GET %d SM %d CV %d %d\n", wSRCP.getsrcpbusGL_ns( o->srcpini ), addr, cv-1, value );
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
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "not connected in SendCommand (recycle=%d socket=0x%08X)", recycle, o->cmdSocket);
    if(o->cmdSocket != NULL) {
      SocketOp.base.del(o->cmdSocket);
      o->cmdSocket = NULL;
    }
    return -1;
  }

  if (!SocketOp.write( o->cmdSocket, szCommand, (int)strlen(szCommand)))
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not send: %s", szCommand );
    return -1;
  }
  else {
    StrOp.replaceAll((char*)szCommand, '\n', ' ');
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sent: %s",szCommand);
  }

  /* Read server response: */
  if (! SocketOp.readln(o->cmdSocket,inbuf) )
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SendCommand: could not read response");
    SocketOp.disConnect(o->cmdSocket);
    SocketOp.base.del(o->cmdSocket);
    o->cmdSocket = NULL;
    return -1;
  }

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "in: %s",inbuf);

  /* Scan for SM return? */
  MemOp.set(szResponse,0,900);
  sscanf(inbuf,"%*s %d %900c",&retstate,szResponse);

  StrOp.replaceAll(szResponse, '\n', ' ');
  if (!SRCP_OK(retstate))
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "SRCP Response: %s",szResponse);
  else
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "SRCP Response: %s",szResponse);


  o->state = (SRCP_OK(retstate)?SRCP_STATE_OK:SRCP_STATE_ERROR);

  if (szRetVal)
    StrOp.copy( szRetVal, szResponse );

  return retstate;
}

static Boolean __srcpConnect ( iOSRCP08Data o ) {
  char cmd[1024];
  char data[1024];

  StrOp.fmtb(cmd, "SET PROTOCOL SRCP 0.8\n" );
  if (SRCP_ERROR( __srcpSendCommand( o, False, cmd, data ) ) )
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: %s",data);
    SocketOp.disConnect(o->cmdSocket);
    return False;
  }

  StrOp.fmtb(cmd, "SET CONNECTIONMODE SRCP COMMAND\n" );
  if (SRCP_ERROR( __srcpSendCommand( o, False, cmd, data ) ) )
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: %s",data);
    SocketOp.disConnect(o->cmdSocket);
    return False;
  }

  StrOp.fmtb(cmd, "GO\n" );
  if (SRCP_ERROR( __srcpSendCommand( o, False, cmd, data ) ) )
  {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: %s",data);
    SocketOp.disConnect(o->cmdSocket);
    return False;
  }


  __srcpInitServer(o);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Handshake completed.");
  return True;
}

static int __srcpInitServer( iOSRCP08Data o) {
  char tmpCommand[1024];

  StrOp.fmtb(tmpCommand,"GET 1 POWER\n");
  if( __srcpSendCommand(o,False,tmpCommand,NULL) != 100 ) {
    StrOp.fmtb(tmpCommand,"INIT 1 POWER\n");
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


static Boolean _isConnected(iOSRCP08 inst) {
  iOSRCP08Data data = Data( inst );
  return data->cmdSocket != NULL;
}

static void _setConnection(iOSRCP08 inst, iOSocket socket) {
  iOSRCP08Data data = Data( inst );
  data->cmdSocket = socket;
  __srcpConnect(data);
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

  data->knownSwitches = allocMem( 1024 * sizeof(Boolean) );
  MemOp.set(data->knownSwitches,0,1024*sizeof(Boolean));

  data->knownLocos = allocMem( 1024 * sizeof(Boolean));
  MemOp.set( data->knownLocos, 0, 1024*sizeof(Boolean));

  data->locInited = allocMem( 16384 * sizeof(Boolean));
  MemOp.set( data->locInited, 0, 16384*sizeof(Boolean));

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
