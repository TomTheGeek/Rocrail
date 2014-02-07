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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "rocdigs/impl/srcp_impl.h"

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

#include "rocdigs/impl/srcp/serial.h"
#include "rocdigs/impl/srcp/tcpip.h"
#include "rocdigs/impl/srcp/srcp.h"

static int instCnt = 0;

static int __srcpInitServer( iOSRCP inst );
static void __handleSM(iOSRCPData o, const char* sm);

/* ***** OBase functions. */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static const char* __name( void ) {
  return name;
}

static unsigned char* __serialize( void * inst, long * size ) {
  return NULL;
}

static void __deserialize( void * inst, unsigned char * a ) {
}

static char* __toString( void * inst ) {
  iOSRCPData data = Data( inst );
  return ( char * ) data->iid;
}

static void __del( void * inst ) {
  iOSRCPData data = Data( inst );
  freeMem( data );
  freeMem( inst );
  instCnt--;
}

static void* __properties( void * inst ) {
  return NULL;
}

static struct OBase* __clone( void * inst ) {
  return NULL;
}

static Boolean __equals( void * inst1, void * inst2 ) {
  return False;
}

static int __count( void )
{
  return instCnt;
}


/* ***** Public functions. */
static Boolean _setListener( obj inst, obj listenerObj, const digint_listener listenerFun ) {
  iOSRCPData data = Data( inst );
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}


static Boolean _setRawListener(obj inst, obj listenerObj, const digint_rawlistener listenerFun ) {
  return True;
}

static byte* _cmdRaw( obj inst, const byte* cmd ) {
  return NULL;
}

/* Command write */
static int __srcpWrite( obj inst, const char* cmd, char* rsp) {
  iOSRCPData o = Data( inst );
  int rc = o->subWrite( inst, cmd, rsp, False);

  if( rsp != NULL && SRCP_OK(rc)) {
    char* sm = StrOp.findi(rsp, "SM" );
    if( sm != NULL ) {
      __handleSM(o, sm);
    }
  }

  return rc;
}

static Boolean __srcpInitConnect ( iOSRCP inst ) {
  iOSRCPData o = Data( inst );
  char cmd[1024];
  char data[1024];

  StrOp.fmtb(cmd, "SET PROTOCOL SRCP 0.8\n" );
  if (SRCP_ERROR( __srcpWrite( (obj)inst, cmd, data ) ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: %s",data);
    o->subDisconnect((obj)inst, False);
    return False;
  }

  StrOp.fmtb(cmd, "SET CONNECTIONMODE SRCP COMMAND\n" );
  if (SRCP_ERROR( __srcpWrite( (obj)inst, cmd, data ) ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: %s",data);
    o->subDisconnect((obj)inst, False);
    return False;
  }

  StrOp.fmtb(cmd, "GO\n" );
  if (SRCP_ERROR( __srcpWrite( (obj)inst, cmd, data ) ) ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: %s",data);
    o->subDisconnect((obj)inst, False);
    return False;
  }

  __srcpInitServer(inst);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Handshake completed.");
  return True;
}


static void __handleSM(iOSRCPData o, const char* sm) {
  /*1367776426.478 100 INFO 1 SM 0 CV 1 0 */
  int addr  = 0;
  const char* CV = "-";
  int nr    = 0;
  int value = 0;
  iONode rsp = NULL;
  iOStrTok tok = StrTokOp.inst( sm, ' ');
  if( StrTokOp.hasMoreTokens( tok ) ) {
    /* SM */
    StrTokOp.nextToken( tok );
    if( StrTokOp.hasMoreTokens( tok ) ) {
      /* address */
      addr = atoi(StrTokOp.nextToken( tok ));
      if( StrTokOp.hasMoreTokens( tok ) ) {
        /* CV */
        CV = StrTokOp.nextToken( tok );
        if( StrTokOp.hasMoreTokens( tok ) ) {
          /* nr */
          nr = atoi(StrTokOp.nextToken( tok ));
          if( StrTokOp.hasMoreTokens( tok ) ) {
            /* value */
            value = atoi(StrTokOp.nextToken( tok ));
          }
        }
      }
    }
  }
  StrTokOp.base.del(tok);

  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "addr=%d CV=%s nr=%d value=%d", addr, CV, nr, value );

  rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
  wProgram.setdecaddr( rsp, addr );
  wProgram.setcv( rsp, nr );
  wProgram.setvalue( rsp, value );
  wProgram.setcmd( rsp, wProgram.datarsp );
  if ( o->listenerFun != NULL && o->listenerObj != NULL )
    o->listenerFun( o->listenerObj, rsp, TRCLEVEL_INFO );

}


static Boolean __initGA( iOSRCP inst, iONode node, int ga_bus, int addr ) {
  iOSRCPData o = Data(inst);
  char tmpCommand[1024];

  StrOp.fmtb(tmpCommand, "GET %d GA %d 0\n", ga_bus, addr );
  if (__srcpWrite((obj)inst, tmpCommand,NULL) != 100 ) {
    const char* prot = wSwitch.getprot( node );
    if( StrOp.equals( wSwitch.prot_DEF, wSwitch.getprot( node )) )
      prot = "P";
    StrOp.fmtb(tmpCommand,"INIT %d GA %d %s\n", ga_bus, addr, prot );
    if (!__srcpWrite((obj)inst, tmpCommand,NULL)) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Problem initializing GA %d", addr );
      return False;
    }
    StrOp.fmtb(tmpCommand,"GET %d GA %d 0\n", ga_bus, addr );
    if (!__srcpWrite((obj)inst, tmpCommand,NULL)) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Problem initializing GA %d", addr);
      return False;
    }
  }
  return True;
}


static Boolean __initGL( iOSRCP inst, iONode node, int* bus ) {
  iOSRCPData o = Data( inst );
  char tmpCommand[1024];
  char key[64] = {'\0'};
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

  StrOp.fmtb(key, "lc_%d", wLoc.getaddr(node));

  if (! MapOp.haskey(o->knownObjects, key) ) {
    StrOp.fmtb(tmpCommand,"GET %d GL %d\n", gl_bus, wLoc.getaddr(node) );
    if( __srcpWrite((obj)inst, tmpCommand,NULL) != 100 ) {
      StrOp.fmtb(tmpCommand,"INIT %d GL %d %s %d %d %d\n", gl_bus,
              wLoc.getaddr(node), prot, wLoc.getprotver( node ),
              wLoc.getspcnt( node ), wLoc.getfncnt( node ) + 1 );

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s", tmpCommand);
      if (!__srcpWrite((obj)inst, tmpCommand,NULL)) {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Problem initializing GL %d", wLoc.getaddr(node));
        return False;
      }
    }
    MapOp.put(o->knownObjects, key, (obj)StrOp.dup(key));

    StrOp.fmtb(tmpCommand,"GET %d GL %d\n", gl_bus, wLoc.getaddr(node) );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "%s", tmpCommand);
    if (!__srcpWrite((obj)inst, tmpCommand,NULL)) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Problem getting GL %d", wLoc.getaddr(node));
      return False;
    }
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "successfully got GL %d", wLoc.getaddr(node));
  }

  return True;
}



/**
 * create a bitmap (int, max 31) of used srcp buses
 */
static int __getSRCPbusList( iOSRCPData o ) {
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


static iONode __translate( iOSRCP inst, iONode node, char* srcp ) {
  iOSRCPData o = Data( inst );
  char   tmpCommand[1024];
  char   tmpRsp[1024];
  iONode rsp = NULL;
  int    i   = 0;

  srcp[0] = '\0';

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Translating command.");
  /* Switch command. */
  if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    char key[64] = {'\0'};
    int addr = wSwitch.getaddr1( node );
    int port = wSwitch.getport1( node );
    int action = 1;
    int activationTime = wDigInt.getswtime( o->ini );
    if( wSwitch.getdelay(node) > 0 )
      activationTime = wSwitch.getdelay(node);

    int ga_bus = wSRCP.getsrcpbusGA_ps( o->srcpini );
    if( StrOp.equals( wSwitch.prot_M, wSwitch.getprot( node ) ) )
      ga_bus = wSRCP.getsrcpbusGA_m( o->srcpini );
    else if( StrOp.equals( wSwitch.prot_N, wSwitch.getprot( node ) ) )
      ga_bus = wSRCP.getsrcpbusGA_n( o->srcpini );

    StrOp.fmtb(key, "sw_%d_%d", wSwitch.getaddr1( node ), wSwitch.getport1( node ));
    if (! MapOp.haskey(o->knownObjects, key) ) {
      if( __initGA(inst, node, ga_bus, addr) ) {
        MapOp.put( o->knownObjects, key, (obj)StrOp.dup(key));
      }
      else {
        return NULL;
      }
    }

    if( wSwitch.issinglegate(node) ) {
      action = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 1:0;
      StrOp.fmtb( srcp, "SET %d GA %d %d %d %d\n", ga_bus, addr, port, action, -1 );
    }
    else {
      if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) )
        port++;
      StrOp.fmtb( srcp, "SET %d GA %d %d %d %d\n", ga_bus, addr, port, action, activationTime );
    }

    return NULL;
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    char key[64] = {'\0'};
    int addr = wOutput.getaddr( node );
    int port = wOutput.getport( node );
    int gate = wOutput.getgate( node );
    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 1:0;
    int activationTime = -1;

    int ga_bus = wSRCP.getsrcpbusGA_ps( o->srcpini );
    if( StrOp.equals( wOutput.prot_M, wOutput.getprot( node ) ) )
      ga_bus = wSRCP.getsrcpbusGA_m( o->srcpini );
    else if( StrOp.equals( wOutput.prot_N, wOutput.getprot( node ) ) )
      ga_bus = wSRCP.getsrcpbusGA_n( o->srcpini );

    StrOp.fmtb(key, "sw_%d_%d", wOutput.getaddr( node ), wOutput.getport( node ));
    if (! MapOp.haskey(o->knownObjects, key) ) {
      if( __initGA(inst, node, ga_bus, addr) ) {
        MapOp.put( o->knownObjects, key, (obj)StrOp.dup(key));
      }
      else {
        return NULL;
      }
    }

    if( StrOp.equals( wOutput.getcmd( node ), wOutput.value ) || StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ) {
      action = wOutput.getvalue( node );
    }
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

    if( !__initGL( inst, node, &bus ) )
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
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) || StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      int busList = __getSRCPbusList( o );
      int intBits = sizeof( int ) * 8 ;
      int i;

      for( i = 0 ; i < intBits ; i++ ) {
        if( busList & ( 1 << i ) ) {
          StrOp.fmtb(tmpCommand,"SET %d POWER OFF\n", i );
          if( __srcpWrite((obj)inst,tmpCommand,NULL) == -1) {
            break;
          }
        }
      }
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      int busList = __getSRCPbusList( o );
      int intBits = sizeof( int ) * 8 ;
      int i;

      for( i = 0 ; i < intBits ; i++ ) {
        if( busList & ( 1 << i ) ) {
          StrOp.fmtb(tmpCommand,"SET %d POWER ON\n", i );
          if( __srcpWrite((obj)inst,tmpCommand,NULL) == -1) {
            break;
          }
        }
      }
    }
    return NULL;
  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    int addr = wProgram.getaddr( node );
    int cv = wProgram.getcv( node );
    int value = wProgram.getvalue( node );

    rsp = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );

    wProgram.setaddr( rsp, addr );
    wProgram.setcv( rsp, cv );
    wProgram.setcmd( rsp, wProgram.statusrsp );

    if( o->iid != NULL )
      wProgram.setiid( rsp, o->iid );

    if( wProgram.getcmd( node ) == wProgram.set ) {
      int ack = 0;
      StrOp.fmtb (tmpCommand, "SET %d SM %d CV %d %d\n", wSRCP.getsrcpbusGL_ns( o->srcpini ), addr, cv, value );
      ack = __srcpWrite((obj)inst,tmpCommand,tmpRsp);
      if( ack == 200 ) {
        wProgram.setvalue( rsp, value );
      }
      else if( ack == 0 ) {
        NodeOp.base.del(rsp);
        rsp = NULL;
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
      StrOp.fmtb (tmpCommand, "GET %d SM %d CV %d\n", wSRCP.getsrcpbusGL_ns( o->srcpini ), addr, cv );
      ack = __srcpWrite((obj)inst,tmpCommand,tmpRsp);
      if( ack == 200 ) {
        wProgram.setvalue( rsp, value-1 );
        wProgram.setcmd( rsp, wProgram.datarsp );
      }
      else if( ack == 0 ) {
        NodeOp.base.del(rsp);
        rsp = NULL;
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
  iOSRCPData data = Data( inst );
  char cmd[1024] = {0};
  iONode rsp = NULL;

  if( nodeA != NULL ) {
    int rc = data->subConnect((obj)inst, False);

    if( rc == SRCPCONNECT_RECONNECTED ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reconnected");
      __srcpInitConnect((iOSRCP)inst);
    }

    rc = 0;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "node=%s cmd=%s", NodeOp.getName(nodeA), wLoc.getcmd(nodeA)!=NULL?wLoc.getcmd(nodeA):"-" );
    rsp = __translate( (iOSRCP)inst, nodeA, cmd );

    if( StrOp.len( cmd ) > 0 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "translation = %s...", cmd);
      rc = __srcpWrite((obj)inst, cmd, NULL);
    }

    if( rc == -1 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "trying to reconnect...");
      if( data->subConnect((obj)inst, False) == SRCPCONNECT_ERROR ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reconnect at next command...");
      }
      else {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reconnected");
        if( __srcpInitConnect((iOSRCP)inst) ) {
          rsp = __translate( (iOSRCP)inst, nodeA, cmd );
          if( StrOp.len( cmd ) > 0 ) {
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "retransmit = %s...", cmd);
            __srcpWrite((obj)inst, cmd, NULL);
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "could not initialize the connection");
        }
      }
    }

    nodeA->base.del(nodeA);
  }

  return rsp;
}


static void _halt( obj inst, Boolean poweroff ) {
  iOSRCPData o = Data( inst );
  o->run = False;
  ThreadOp.sleep(100);
}

static Boolean _supportPT( obj inst ) {
  iOSRCPData data = Data(inst);
  return True;
}


static Boolean __initInfoConnection(iOSRCP inst) {
  iOSRCPData o = Data(inst);
  int retry = 0;
  char inbuf[1024] = { 0 };
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Info Connected; trying the handshake..." );

  o->handshakeerror = False;

  while( retry < 10 && !o->subAvailable((obj)inst) ) {
    ThreadOp.sleep(100);
    retry++;
    if( retry >= 10 ) {
      StrOp.copy(inbuf, "SRCP 0.8" );
    }
  };


  if ( retry >= 10 || o->subRead( (obj)inst, inbuf, True ) ) {
    StrOp.replaceAll( inbuf, '\n', ' ' );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, inbuf );
    if( StrOp.findi( inbuf, "SRCP 0.8" ) ) {
      char cmd[256];

      if( !o->handshakeerror ) {
        StrOp.fmtb(cmd, "%s\n", "SET PROTOCOL SRCP 0.8");
        o->subWrite((obj)inst, cmd, inbuf, True);
        StrOp.replaceAll( cmd, '\n', ' ' );
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "%s",cmd);

        /*"OK PROTOCOL SRCP"*/
        if( !StrOp.find( inbuf, "201" ) ) {
          /* error */
          o->handshakeerror = True;
          StrOp.replaceAll( inbuf, '\n', ' ' );
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: expecting 201, received [%s]",inbuf);
        }
      }

      if( !o->handshakeerror ) {
        StrOp.fmtb(cmd, "%s\n", "SET CONNECTIONMODE SRCP INFO");
        o->subWrite((obj)inst, cmd, inbuf, True);
        StrOp.replaceAll( cmd, '\n', ' ' );
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "%s",cmd);

        /*"OK CONNECTION MODE"*/
        if( !StrOp.find( inbuf, "202" ) ) {
          /* error */
          o->handshakeerror = True;
          StrOp.replaceAll( inbuf, '\n', ' ' );
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: expecting 202, received [%s]",inbuf);
        }
      }

      if( !o->handshakeerror ) {
        StrOp.fmtb(cmd, "%s\n", "GO");
        o->subWrite((obj)inst, cmd, inbuf, True);
        StrOp.replaceAll( cmd, '\n', ' ' );
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "%s",cmd);

        /*"OK GO"*/
        if( !StrOp.find( inbuf, "200" ) ) {
          /* error */
          o->handshakeerror = True;
          StrOp.replaceAll( inbuf, '\n', ' ' );
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "ERROR handshaking: expecting 200, received [%s]",inbuf);
        }
      }
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "No response from server..." );
  }
  return o->handshakeerror ? False:True;
}


static void __handleFB(iOSRCP srcp, int busnr, int addr, int val) {
  iOSRCPData o = Data( srcp );
  if( o->listenerFun != NULL ) {
    iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
    wFeedback.setbus( nodeC, busnr );
    wFeedback.setaddr( nodeC, addr );
    wFeedback.setstate( nodeC, val ? True : False );
    if ( o->iid != NULL )
      wFeedback.setiid( nodeC, o->iid );
    o->listenerFun( o->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}

static void __handleGL(iOSRCP srcp, int addr, int V, int dir, int steps, int f0, int idxFn, int srcpFx) {
  iOSRCPData o = Data( srcp );
  if( o->listenerFun != NULL ) {
    iONode nodeC  = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
    iONode nodeFn = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );

    wLoc.setthrottleid( nodeC, "srcp" );
    wLoc.setcmd( nodeC, wLoc.dirfun );
    wLoc.setaddr( nodeC, addr );
    wLoc.setspcnt( nodeC, steps );
    wLoc.setV( nodeC, V );
    wLoc.setV_raw( nodeC, V );
    wLoc.setfn( nodeC, f0 );
    wLoc.setdir( nodeC, dir );
    if ( o->iid != NULL )
      wLoc.setiid( nodeC, o->iid );

    wLoc.setthrottleid( nodeFn, "srcp" );
    if ( o->iid != NULL )
      wLoc.setiid( nodeFn, o->iid );

    wFunCmd.setaddr( nodeFn, addr );
    wFunCmd.setf28( nodeFn,(srcpFx & 0x08000000)?True:False);
    wFunCmd.setf27( nodeFn,(srcpFx & 0x04000000)?True:False);
    wFunCmd.setf26( nodeFn,(srcpFx & 0x02000000)?True:False);
    wFunCmd.setf25( nodeFn,(srcpFx & 0x01000000)?True:False);
    wFunCmd.setf24( nodeFn,(srcpFx & 0x00800000)?True:False);
    wFunCmd.setf23( nodeFn,(srcpFx & 0x00400000)?True:False);
    wFunCmd.setf22( nodeFn,(srcpFx & 0x00200000)?True:False);
    wFunCmd.setf21( nodeFn,(srcpFx & 0x00100000)?True:False);
    wFunCmd.setf20( nodeFn,(srcpFx & 0x00080000)?True:False);
    wFunCmd.setf19( nodeFn,(srcpFx & 0x00040000)?True:False);
    wFunCmd.setf18( nodeFn,(srcpFx & 0x00020000)?True:False);
    wFunCmd.setf17( nodeFn,(srcpFx & 0x00010000)?True:False);
    wFunCmd.setf16( nodeFn,(srcpFx & 0x00008000)?True:False);
    wFunCmd.setf15( nodeFn,(srcpFx & 0x00004000)?True:False);
    wFunCmd.setf14( nodeFn,(srcpFx & 0x00002000)?True:False);
    wFunCmd.setf13( nodeFn,(srcpFx & 0x00001000)?True:False);
    wFunCmd.setf12( nodeFn,(srcpFx & 0x00000800)?True:False);
    wFunCmd.setf11( nodeFn,(srcpFx & 0x00000400)?True:False);
    wFunCmd.setf10( nodeFn,(srcpFx & 0x00000200)?True:False);
    wFunCmd.setf9(  nodeFn,(srcpFx & 0x00000100)?True:False);
    wFunCmd.setf8(  nodeFn,(srcpFx & 0x00000080)?True:False);
    wFunCmd.setf7(  nodeFn,(srcpFx & 0x00000040)?True:False);
    wFunCmd.setf6(  nodeFn,(srcpFx & 0x00000020)?True:False);
    wFunCmd.setf5(  nodeFn,(srcpFx & 0x00000010)?True:False);
    wFunCmd.setf4(  nodeFn,(srcpFx & 0x00000008)?True:False);
    wFunCmd.setf3(  nodeFn,(srcpFx & 0x00000004)?True:False);
    wFunCmd.setf2(  nodeFn,(srcpFx & 0x00000002)?True:False);
    wFunCmd.setf1(  nodeFn,(srcpFx & 0x00000001)?True:False);
    wFunCmd.setf0(  nodeFn, f0 );

    o->listenerFun( o->listenerObj, nodeC, TRCLEVEL_INFO );
    o->listenerFun( o->listenerObj, nodeFn, TRCLEVEL_INFO );
  }
}


static void __handleGA(iOSRCP srcp, int addr, int port, int val ) {
  iOSRCPData o = Data( srcp );
  if( o->listenerFun != NULL ) {
    iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
    wSwitch.setaddr1( nodeC, addr );
    wSwitch.setport1( nodeC, port );
    if( val == 1 )
      wSwitch.setstate( nodeC, port ? wSwitch.straight : wSwitch.turnout );
    else
      wSwitch.setstate( nodeC, wSwitch.straight );
    if ( o->iid != NULL )
      wSwitch.setiid( nodeC, o->iid );
    o->listenerFun( o->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}


static void __infoReader( void * threadinst ) {
  iOThread th = ( iOThread )threadinst;
  iOSRCP srcp = ( iOSRCP )ThreadOp.getParm( th );
  iOSRCPData o = Data( srcp );
  Boolean exception = False;
  char inbuf[1024] = { 0 };

  ThreadOp.sleep(500);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "info reader started" );

  while( o->run && !o->handshakeerror ) {
    int inlen = 0;
    int rc = o->subConnect((obj)srcp, True);
    if( (rc == SRCPCONNECT_RECONNECTED ) && __initInfoConnection(srcp) ) {
      /* OK */
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "info reader: connected" );
    }
    else if( rc == SRCPCONNECT_ERROR ) {
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "info reader: error connecting" );
      ThreadOp.sleep(1000);
      continue;
    }

    if( !o->subAvailable((obj)srcp) ) {
      ThreadOp.sleep(10);
      continue;
    }

    inlen = o->subRead( (obj)srcp, inbuf, True );
    if( inlen == 0 ) {
      ThreadOp.sleep(10);
      continue;
    }

    if( inlen > 0 ) {
      char*    fbAddrStr   = NULL;
      iOStrTok tok         = NULL;
      int      infotype    = -1; /* 0=FB, 1=GA , 2=GL*/
      Boolean  ignoreRest  = False;
      char*    infotypeStr = "";
      int      msgnr       = 0;
      int      busnr       = 0;
      const char* group    = NULL;

      StrOp.replaceAll(inbuf, '\n', ' ');
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "inforeader: [%s]", inbuf );

      if( StrOp.find( inbuf, "INFO ") ) {

        tok = StrTokOp.inst( inbuf, ' ' );
        if( StrTokOp.hasMoreTokens( tok ) ) {
          /* timestamp */
          const char* nr = StrTokOp.nextToken( tok );
          if( StrTokOp.hasMoreTokens( tok ) ) {
            nr = StrTokOp.nextToken( tok );
            msgnr = atoi(nr);
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "info number = %d(%s)", msgnr, nr);
          }
          if( StrTokOp.hasMoreTokens( tok ) ) {
            const char* infoType = StrTokOp.nextToken( tok );
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "info type = %s", infoType);
          }
          if( StrTokOp.hasMoreTokens( tok ) ) {
            const char* infoBus = StrTokOp.nextToken( tok );
            busnr = atoi(infoBus);
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "info bus = %s(%d)", infoBus, busnr);
          }
          if( StrTokOp.hasMoreTokens( tok ) ) {
            group = StrTokOp.nextToken( tok );
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "info group = %s", group);
          }
        }
        tok->base.del(tok);

        if( StrOp.equals( group, "FB") ) {
          infotype = 0;
          infotypeStr = "sensor";
        }
        else if( StrOp.equals( inbuf, "GA") ) {
          infotype = 1;
          infotypeStr = "accessory";
        }
        else if( StrOp.equals( inbuf, "GL") ) {
          infotype = 2;
          infotypeStr = "locomotive";
        }

        if( !StrOp.find( inbuf, "FB POWER") ) {
          if( infotype == 0 )
            fbAddrStr = StrOp.find( inbuf, "FB ");
          else if( infotype == 1 )
            fbAddrStr = StrOp.find( inbuf, "GA ");
          else if( infotype == 2 )
            fbAddrStr = StrOp.find( inbuf, "GL ");
        }

      }

      if( fbAddrStr == NULL ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "no FB/GA/GL info..." );
        ThreadOp.sleep( 10 );
        continue;
      }

      tok = StrTokOp.inst( fbAddrStr, ' ' );

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "%s addresses: [%s]", infotypeStr, fbAddrStr );

      if( StrTokOp.hasMoreTokens( tok ) ) {
        const char* leadinStr = StrTokOp.nextToken( tok );
      }

      while ( !ignoreRest && StrTokOp.hasMoreTokens( tok ) ) {
        const char* addrStr = StrTokOp.nextToken( tok );
        if ( StrTokOp.hasMoreTokens( tok ) ) {
          const char* valStr = NULL;
          int addr  = atoi( addrStr );
          int port  = 0;
          int val   = 0;
          int V     = 0;
          int steps = 0;
          int dir   = 0;
          int f0    = 0;
          int idxFn = 0;
          int srcpFx= 0;

          if( infotype == 1 ) {
            /* GA */
            if( msgnr != 100 ) {
              ignoreRest = True;
              break;
            }
            valStr = StrTokOp.nextToken( tok );
            port = atoi( valStr );
            if( StrTokOp.hasMoreTokens( tok ) ) {
              valStr = StrTokOp.nextToken( tok );
              val = atoi( valStr );
            }
            else {
              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "unexpected end of GA %d, %d info", addr, port );
              break;
            }

            if( val != 1 ) {
              TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "not interrested in GA %d OFF state", addr );
              break;
            }
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "switch %d port %d = %d", addr, port, val );
            __handleGA(srcp, addr, port, val );
          }
          else if( infotype == 2 ) {
            /* 100 INFO 2 GL 56 1 44 128 0 0 0 0 0 */
            /* 101 INFO 2 GL 56 N 1 128 5] */
            if( msgnr != 100 ) {
              ignoreRest = True;
              break;
            }

            valStr = StrTokOp.nextToken( tok );
            dir = atoi( valStr );
            if( StrTokOp.hasMoreTokens( tok ) ) {
              valStr = StrTokOp.nextToken( tok );
              V = atoi( valStr );
              if( StrTokOp.hasMoreTokens( tok ) ) {
                valStr = StrTokOp.nextToken( tok );
                steps = atoi( valStr );
                if( StrTokOp.hasMoreTokens( tok ) ) {
                  valStr = StrTokOp.nextToken( tok );
                  f0 = atoi( valStr );

                  /* put max 28 function states in a bitmap (integer, 32bit) */
                  while( idxFn < 28 && StrTokOp.hasMoreTokens( tok ) ) {
                    valStr = StrTokOp.nextToken( tok );
                    if( valStr[0] == '1') {
                      srcpFx |= 1 << idxFn;
                      TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "info: F%d[1] srcpFx[0x%08.8X]", (idxFn+1), srcpFx );
                    }
                    idxFn++;
                  }

                }
              }
            }
            else {
              TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "unexpected end of GL %d info", addr );
              break;
            }
            ignoreRest = True;
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loco %d V=%d dir=%d steps=%d fn=%d idxFn[%d] srcpFx[0x%08.8X]",
                addr, V, dir, steps, f0, idxFn, srcpFx );
            __handleGL(srcp, addr, V, dir, steps, f0, idxFn, srcpFx);
          }
          else {
            /* FB */
            valStr = StrTokOp.nextToken( tok );
            val = atoi( valStr );
            TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "sensor %d:%d=%d", busnr, addr, val );
            __handleFB(srcp, busnr, addr, val);
          }

        }
        else {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "not an INFO line:" );
          TraceOp.dump( NULL, TRCLEVEL_DEBUG, inbuf, StrOp.len( inbuf ) );
        }

      };
      /* end while */

      tok->base.del(tok);

    }
    /* end if */
    else {
      exception = True;
    }

    if( exception ) {
      int rcConnect = 0;
      exception = False;
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Try to reconnect..." );

      o->subDisconnect( (obj)srcp, True);
      o->subDisconnect( (obj)srcp, False);

      ThreadOp.sleep( 1000 );
      rcConnect = o->subConnect( (obj)srcp, True );
      if( rcConnect == SRCPCONNECT_RECONNECTED || rcConnect == SRCPCONNECT_OK) {
        __initInfoConnection(srcp);
      }
    }
    else
      ThreadOp.sleep( 10 );
  };


  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Info reader ended" );

}


static int __srcpInitServer( iOSRCP inst) {
  iOSRCPData o = Data(inst);
  char tmpCommand[1024];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "init server..." );
  StrOp.fmtb(tmpCommand,"GET 1 POWER\n");
  if( __srcpWrite((obj)inst,tmpCommand,NULL) != 100 ) {
    StrOp.fmtb(tmpCommand,"INIT 1 POWER\n");
    __srcpWrite((obj)inst,tmpCommand,NULL);
  }
  return 0;
}


/* Status */
static int _state( obj inst ) {
  iOSRCPData data = Data(inst);
  int state = 0;
  return state;
}


/* external shortcut event */
static void _shortcut(obj inst) {
  iOSRCPData data = Data( inst );
}


/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOSRCPData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


static iOSRCP _inst( const iONode settings, const iOTrace trace ) {
  iOSRCP srcp = allocMem( sizeof( struct OSRCP ) );
  iOSRCPData data = allocMem( sizeof( struct OSRCPData ) );
  int i = 0;

  TraceOp.set( trace );

  /* OBase */
  MemOp.basecpy( srcp, & SRCPOp, 0, sizeof( struct OSRCP ), data );

  data->ini   = settings;
  data->trace = trace;

  data->srcpini = wDigInt.getsrcp(data->ini);

  if( data->srcpini == NULL ) {
    data->srcpini = NodeOp.inst( wSRCP.name(), data->ini, ELEMENT_NODE );
    NodeOp.addChild(data->ini, data->srcpini );
  }

  /* Evaluate attributes. */
  data->iid = StrOp.dup( wDigInt.getiid( settings ) );

  data->host   = wDigInt.gethost( settings );
  data->port   = wSRCP.getcmdport( data->srcpini );
  data->run    = True;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "SRCP %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  iid       : %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "  sublib    : %s", wDigInt.getsublib(data->ini) );

  data->knownObjects = MapOp.inst();

  /* choose interface: */
  if( StrOp.equals( wDigInt.sublib_default, wDigInt.getsublib( data->ini ) ) ||
      StrOp.equals( wDigInt.sublib_tcp, wDigInt.getsublib( data->ini ) ) )
  {
    /* tcpip */
    data->subInit       = tcpipInit;
    data->subConnect    = tcpipConnect;
    data->subDisconnect = tcpipDisconnect;
    data->subRead       = tcpipRead;
    data->subWrite      = tcpipWrite;
    data->subAvailable  = tcpipAvailable;
  }
  else if( StrOp.equals( wDigInt.sublib_serial, wDigInt.getsublib( data->ini ) ) ) {
    /* serial */
    data->subInit       = serialInit;
    data->subConnect    = serialConnect;
    data->subDisconnect = serialDisconnect;
    data->subRead       = serialRead;
    data->subWrite      = serialWrite;
    data->subAvailable  = serialAvailable;
  }

  data->subInit((obj)srcp);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  {
    char * infoname = StrOp.fmt( "info%08X", srcp );
    data->infoReader = ThreadOp.inst( infoname, & __infoReader, srcp );
    ThreadOp.start( data->infoReader );
  }

  instCnt++;

  return srcp;
}


/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini, const iOTrace trc )
{
  return ( iIDigInt )_inst( ini, trc );
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! ----- */
#include "rocdigs/impl/srcp.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! ----- */
