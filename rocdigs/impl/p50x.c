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

#include "rocdigs/impl/p50x_impl.h"

#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/attr.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
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
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/BinCmd.h"

#include "rocdigs/impl/common/fada.h"


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
  iOP50xData data = Data(inst);
  return (char*)data->iid;
}
static void __del(void* inst) {
  iOP50xData data = Data(inst);
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
 ***** Private functions.
 */
static p50state __cts( iOP50xData o ) {
  /* CTS */
  int wait4cts = 0;

  if( o->dummyio )
    return P50_OK;

  while( wait4cts < o->ctsretry ) {
    int rc = SerialOp.isCTS( o->serial );
    if( rc == -1 ) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device error; switch to dummy mode" );
      o->dummyio = True;
      return P50_CTSERR;
    }
    if( rc > 0 ) {
      return P50_OK;
    }
    ThreadOp.sleep( 10 );
    wait4cts++;
  }
  TraceOp.trc(name, TRCLEVEL_WARNING, __LINE__, 9999, "CTS not ready");
  return P50_CTSERR;
}

static Boolean __flushP50x( iOP50xData o ) {
  /* Read all pending information on serial port. Interface Hickups if data is pending from previous init! */
  if( !o->dummyio ) {
    byte buffer[256];
    int bAvail = SerialOp.available(o->serial);
    if( bAvail > 0 && bAvail < 256 ) {
      TraceOp.trc(name, TRCLEVEL_WARNING, __LINE__, 9999, "Flushing %d bytes...", bAvail);
      SerialOp.read( o->serial, buffer, bAvail );
      TraceOp.dump( NULL, TRCLEVEL_WARNING, buffer, bAvail );
    }
    else if(bAvail >= 256) {
      TraceOp.trc(name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Can not flush %d bytes, check your hardware!", bAvail);
      return False;
    }
    else {
      TraceOp.trc(name, TRCLEVEL_DEBUG, __LINE__, 9999, "flushed");
    }
  }
  return True;
}



static Boolean __transact( iOP50xData o, char* out, int outsize, char* in, int insize, int inendbyte, int muxwait ) {
  if( MutexOp.trywait( o->mux, muxwait ) ) {
    Boolean rc = False;
    p50state state = P50_OK;

    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "outsize=%d insize=%d inendbyte=0x%02X", outsize, insize, inendbyte);
    TraceOp.dump( NULL, TRCLEVEL_BYTE, out, outsize );

    if( !o->serialOK ) {
      MutexOp.post( o->mux );
      return False;
    }

    if( !__flushP50x(o) ) {
      MutexOp.post( o->mux );
      return False;
    }

    if( o->tok)
      printf( "\n*****token!!! B\n\n" );
    o->tok = True;

    state = __cts(o);

    if( state == P50_OK ) {

      if( o->dummyio ) {
        MutexOp.post( o->mux );
        return True;
      }


      if( SerialOp.write( o->serial, out, outsize ) ) {
        state = P50_OK;
        if( insize == -1 ) {
          /* First in byte tells how much bytes are comming. */
          if( SerialOp.read( o->serial, in, 1 ) ) {
            state = P50_OK;
            insize = in[0];
          }
          else {
            state = P50_RCVERR;
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unable to read first byte of %d", insize);
          }
        }
        if( insize > 0 ) {
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "insize=%d, inendbyte=0x%02X", insize, inendbyte);
          if( inendbyte != -1 ) {
            int readCnt = 0;
            while( state == P50_OK && readCnt < insize ) {
              Boolean readOK = SerialOp.read( o->serial, in+readCnt, 1 );
              state = (readOK ? P50_OK:P50_RCVERR);
              if( in[readCnt] == inendbyte ) {
                TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "inendbyte detected at position %d", readCnt);
                TraceOp.dump( NULL, TRCLEVEL_BYTE, in, readCnt+1 );
                break;
              }
              readCnt++;
            }
            TraceOp.dump( NULL, TRCLEVEL_BYTE, in, readCnt );

            if( state == P50_RCVERR ) {
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unable to read in end byte [0x%02X]", inendbyte);
            }
          }
          else {
            if( SerialOp.read( o->serial, in, insize ) ) {
              TraceOp.dump( NULL, TRCLEVEL_BYTE, in, insize );
              state = P50_OK;
            }
            else {
              state = P50_RCVERR;
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unable to read %d bytes", insize);
            }
          }
        }
      }
      else
        state = P50_SNDERR;
    }
    if( state != P50_OK ) {
      const char* strState = state == P50_RCVERR?"RCVERR":"SNDERR";
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
        "ERROR in transact!!! rc=%d state=%s\n", SerialOp.getRc( o->serial ), strState );
    }
    o->tok = False;
    MutexOp.post( o->mux );

    if( state != o->state ) {
      /* inform listener: Node3 */
      int errLevel = TRCLEVEL_INFO;
      iONode nodeC = NodeOp.inst( wDigInt.name(), NULL, ELEMENT_NODE );
      wResponse.setsender( nodeC, o->device );

      switch( state ) {
      case P50_OK:
        wResponse.setmsg( nodeC, "OK." );
        wResponse.setstate( nodeC, wResponse.ok );
        errLevel = TRCLEVEL_INFO;
        break;
      case P50_CTSERR:
        wResponse.setmsg( nodeC, "CTS error." );
        wResponse.setstate( nodeC, wResponse.ctserr );
        errLevel = TRCLEVEL_EXCEPTION;
        break;
      case P50_SNDERR:
        wResponse.setmsg( nodeC, "Write error." );
        wResponse.setstate( nodeC, wResponse.snderr );
        errLevel = TRCLEVEL_EXCEPTION;
        break;
      case P50_RCVERR:
        wResponse.setmsg( nodeC, "Read error." );
        wResponse.setstate( nodeC, wResponse.rcverr );
        errLevel = TRCLEVEL_EXCEPTION;
        break;
      }

      if( o->listenerFun != NULL && o->listenerObj != NULL )
        o->listenerFun( o->listenerObj, nodeC, errLevel );
      o->state = state;
    }

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "_transact state=%d outsize=%d insize=%d",
                   state, outsize, insize );
    return state == P50_OK ? True:False;
  }
  return False;
}

/**
 * @param node <sw unit="1" pin="1" cmd="straight"/>
 * @param node <lc addr="1" speed="0" fn="false" sw="false"/>
 * @param node <fn addr="1" f1="false" f2="false" f3="false" f4="false"/>
 * @param node <fb unit="1"/>
 * @param node <sys cmd="stop"/>
 * @param p50 Request buffer.
 * @param insize Responce size.
 * @return Request size.
 */
static int __translate( iOP50xData o, iONode node, unsigned char* p50, int* insize, int* inendbyte ) {
  *insize = 0;
  /* BinCmd command. */
  if( StrOp.equals( NodeOp.getName( node ), wBinCmd.name() ) ) {
    *insize = wBinCmd.getinlen(node);
    *inendbyte = wBinCmd.getinendbyte(node);
    int outLen = wBinCmd.getoutlen(node);
    byte* outBytes = StrOp.strToByte( wBinCmd.getout(node));
    MemOp.copy( p50, outBytes, outLen );
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "bin command: outlen=%d inlen=%d", outLen, *insize );
    return outLen;

  }
  /* Switch command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {
    int mod = wSwitch.getaddr1( node );
    int pin = wSwitch.getport1( node );
    byte cmd = 0x80;
    const char* cmdstr = "straight";
    int addr = 0;
    int gate = wSwitch.getgate1( node );

    if( pin == 0 )
      fromFADA( mod, &mod, &pin, &gate );
    else if( mod == 0 && pin > 0 )
      fromPADA( pin, &mod, &pin );

    addr = (mod-1) * 4 + pin;

    if( addr < 1 )
      return 0;

    if( StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ) {
      cmd = 0x00;
      cmdstr = "thrown";
    }

    /* to get two decouples at one address: */
    if( StrOp.equals( wSwitch.gettype(node), wSwitch.decoupler ) )
      cmd = wSwitch.getgate1( node )==0?0x00:0x80;

    p50[0] = (byte)'x';
    p50[1] = 0x90;
    p50[2] = (byte)(addr&0xFF);
    p50[3] = (addr >> 8) & 0x07;
    p50[3] |= cmd;
    p50[3] |= (wSwitch.isactivate( node )?0x40:0x00); /* Set active */
    *insize = 1; /* Return code from P50x. */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "turnout %d %s", addr, cmdstr );
    return 4;
  }

  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {
    int mod = wOutput.getaddr( node );
    int pin = wOutput.getport( node );
    int addr = 0;
    int gate = wOutput.getgate( node );
    byte action = 0;
    byte cmd = 0;
    const char* cmdstr = NULL;

    if( pin == 0 )
      fromFADA( mod, &mod, &pin, &gate );
    else if( mod == 0 && pin > 0 )
      fromPADA( pin, &mod, &pin );

    action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x40:0x00;
    cmd = gate==0?0x00:0x80;
    cmdstr = action?"on":"off";

    addr = (mod-1) * 4 + pin;

    if( addr < 1 )
      return 0;

    p50[0] = (byte)'x';
    p50[1] = 0x90;
    p50[2] = (byte)(addr&0xFF);
    p50[3] = (addr >> 8) & 0x07;
    p50[3] |= cmd;
    p50[3] |= action; /* Set active */
    *insize = 1; /* Return code from P50x. */
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d at gate %d %s", addr, gate, cmdstr );
    return 4;
  }


  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
    return 0;
  }


  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    Boolean fn = wLoc.isfn( node );
    int    dir = wLoc.isdir( node );

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * 127) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * 127) / wLoc.getV_max( node );
    }
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d speed=%d lights=%s dir=%s",
        addr, speed, fn?"on":"off", dir?"forwards":"reverse" );

    p50[0] = (byte)'x';
    p50[1] = 0x80;
    p50[2] = (byte)(addr&0xFF);
    p50[3] = (addr >> 8) & 0xFF;
    p50[4] = speed;
    p50[5] = 0; /* reset */
    p50[5] |= fn?0x10:0x00;
    p50[5] |= dir?0x20:0x00;
    p50[5] |= 0x40; /* Force */
    *insize = 1; /* Return code from P50x. */
    return 6;
  }
  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );
    int group = wFunCmd.getgroup(node);
    if( group > 2 ) {
      Boolean f1 = wFunCmd.isf9 ( node );
      Boolean f2 = wFunCmd.isf10( node );
      Boolean f3 = wFunCmd.isf11( node );
      Boolean f4 = wFunCmd.isf12( node );
      Boolean f5 = wFunCmd.isf13( node );
      Boolean f6 = wFunCmd.isf14( node );
      Boolean f7 = wFunCmd.isf15( node );
      Boolean f8 = wFunCmd.isf16( node );
      int   info = (f1?0x01:0) + (f2?0x02:0) + (f3?0x04:0) + (f4?0x08:0) + (f5?0x10:0) + (f6?0x20:0) + (f7?0x40:0) + (f8?0x80:0);
      p50[0] = (byte)'x';
      p50[1] = 0x89;
      p50[2] = (byte)(addr&0xFF);
      p50[3] = (addr >> 8) & 0xFF;
      p50[4] = (unsigned char)info;
      *insize = 1; /* Return code from P50x. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
              "loc %d f9=%s f10=%s f11=%s f12=%s f13=%s f14=%s f15=%s f16=%s",
              addr, f1?"on":"off", f2?"on":"off", f3?"on":"off", f4?"on":"off",
              f5?"on":"off", f6?"on":"off", f7?"on":"off", f8?"on":"off" );
      return 5;
    }
    else {
      Boolean f1 = wFunCmd.isf1( node );
      Boolean f2 = wFunCmd.isf2( node );
      Boolean f3 = wFunCmd.isf3( node );
      Boolean f4 = wFunCmd.isf4( node );
      Boolean f5 = wFunCmd.isf5( node );
      Boolean f6 = wFunCmd.isf6( node );
      Boolean f7 = wFunCmd.isf7( node );
      Boolean f8 = wFunCmd.isf8( node );
      int   info = (f1?0x01:0) + (f2?0x02:0) + (f3?0x04:0) + (f4?0x08:0) + (f5?0x10:0) + (f6?0x20:0) + (f7?0x40:0) + (f8?0x80:0);
      p50[0] = (byte)'x';
      p50[1] = 0x88;
      p50[2] = (byte)(addr&0xFF);
      p50[3] = (addr >> 8) & 0xFF;
      p50[4] = (unsigned char)info;
      *insize = 1; /* Return code from P50x. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
              "loc %d f1=%s f2=%s f3=%s f4=%s f5=%s f6=%s f7=%s f8=%s",
              addr, f1?"on":"off", f2?"on":"off", f3?"on":"off", f4?"on":"off",
              f5?"on":"off", f6?"on":"off", f7?"on":"off", f8?"on":"off" );
      return 5;
    }
  }
  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );
    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      p50[0] = (byte)'x';
      p50[1] = 0xa6;
      *insize = 1; /* Return code from P50x. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      return 2;
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      p50[0] = (byte)'x';
      p50[1] = 0xa7;
      *insize = 1; /* Return code from P50x. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      return 2;
    }
    else if( StrOp.equals( cmd, wSysCmd.loccnfg ) ) {
      int addr = wSysCmd.getval( node );
      int decformat = wSysCmd.getvalA( node );
      int speedsteps = wSysCmd.getvalB( node );
      if( speedsteps == 128 )
        speedsteps = 126;
      p50[0] = (byte)'x';
      p50[1] = 0x86;
      p50[2] = addr & 0x00FF;
      p50[3] = ((addr & 0xFF00) >> 8 );
      p50[4] = decformat;
      p50[5] = speedsteps;
      *insize = 1; /* Return code from P50x. */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Loc[%d] Config: format=%d, steps=%d", addr, decformat, speedsteps  );
      return 6;
    }
  }
  /* Feedback command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFeedback.name() ) ) {
    int mod = wFeedback.getaddr( node )/16;
    p50[0] = (unsigned char)(192+mod);
    *insize = 2;
    return 1;
  }
  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {
    if(  wProgram.getcmd( node ) == wProgram.ptstat ) {
      p50[0] = (byte)'x';
      p50[1] = 0xE0;
      *insize = 1;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT stat" );
      return 2;
    }
    else if(  wProgram.getcmd( node ) == wProgram.pton ) {
      p50[0] = (byte)'x';
      p50[1] = 0xE1;
      *insize = 1;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT ON" );
      return 2;
    }
    else if( wProgram.getcmd( node ) == wProgram.ptoff ) {
      p50[0] = (byte)'x';
      p50[1] = 0xE2;
      *insize = 1;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT OFF" );
      return 2;
    }
    else if( wProgram.getcmd( node ) == wProgram.get ) {
      int cv = wProgram.getcv( node );
      o->cv_nr = cv;
      p50[0] = (byte)'x';
      p50[1] = 0xF0;
      p50[2] = cv & 0xFF;
      p50[3] = cv >> 8;
      *insize = 1;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "get CV%d", cv );
      return 4;
    }
    else if( wProgram.getcmd( node ) == wProgram.set ) {
      int addr = wProgram.getaddr( node );
      int cv = wProgram.getcv( node );
      int val = wProgram.getvalue( node );
      o->cv_nr = cv;
      o->cv_val = val;
      if( wProgram.ispom(node) ) {
        /*
        XDCC_PD (0xDE)- Länge = 1+5 Bytes
                0: 0xDE XDCC_PD (= Lok-Programmieren auf dem Hauptgleis = POM)
                1: LSB der Lokadresse
                2: MSB der Lokadresse (1-10239)
                3: Low Byte der CV-Adresse, welche zu schreiben ist.
                4: High Byte der CV-Adresse, welche zu schreiben ist. (1..1024)
                5: Wert

        Antwort: 0 = Ok, accepted
                 0x80 = busy, command ignored
        */

        p50[0] = (byte)'x';
        p50[1] = 0xDE;
        p50[2] = addr & 0xFF;
        p50[3] = addr >> 8;
        p50[4] = cv & 0xFF;
        p50[5] = cv >> 8;
        p50[6] = val;
        *insize = 1;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "POM: set CV%d to %d", cv, val );
        return 7;
      }
      else {
        p50[0] = (byte)'x';
        p50[1] = 0xF1;
        p50[2] = cv & 0xFF;
        p50[3] = cv >> 8;
        p50[4] = val;
        *insize = 1;
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "set CV%d to %d", cv, val );
        return 5;
      }
    }
  }

  return 0;
}


static Boolean __getversion( iOP50x inst ) {
  iOP50xData data = Data(inst);
  char out[8] = {'x',0xA0};
  char in[256];
  char rl = 0;
  int idx = 0;
  int outsize = 2;
  int insize = 0;
  p50state state = P50_OK;
  Boolean ok = False;
  memset(in,0,32);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Sending XNOP..." );
  out[0]= 'x';
  out[1]= 0xC4;

  if( !data->dummyio && MutexOp.trywait( data->mux, data->timeout ) ) {

    if( SerialOp.write( data->serial, (char*)out, 2 ) ) {
      if( !SerialOp.read( data->serial, (char*)in, 1 ) )
        state = P50_RCVERR;
    }
    else
      state = P50_SNDERR;

    if( state = P50_OK ) {
    int bAvail = 0;
    ThreadOp.sleep( 500 );
    bAvail = SerialOp.available(data->serial);
    if( bAvail > 0 && bAvail < 32 )
      SerialOp.read( data->serial, (char*)in, bAvail );
      if( bAvail == 1 ) {
        TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "p50 mode detected!!!" );
        MutexOp.post( data->mux );
        return False;
      }
    }

    MutexOp.post( data->mux );
  }


  if( !data->dummyio && MutexOp.trywait( data->mux, data->timeout ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Version info requested..." );
    out[0]= 'x';
    out[1]= 0xA0;

    if( SerialOp.write( data->serial, (char*)out, 2 ) ) {
      /* First in byte tells how much bytes are comming. */
      if( SerialOp.read( data->serial, in, 1 ) ) {
        state = P50_OK;
        insize = in[0];
      }
      else {
        state = P50_RCVERR;
        MutexOp.post( data->mux );
        return False;
      }

      if( SerialOp.read( data->serial, in, insize ) ) {
        state = P50_OK;
      }
      else {
        state = P50_RCVERR;
        MutexOp.post( data->mux );
        return False;
      }

      TraceOp.dump( NULL, TRCLEVEL_BYTE, in, insize );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                   "Intellibox version --- %c.%c%c%c ---",
                   ((in[1] & 0xF0) >> 4) + 0x30,
                   (in[1] & 0x0F) + 0x30,
                   ((in[0] & 0xF0) >> 4) + 0x30,
                   (in[0] & 0x0F) + 0x30
                   );

      /* Read the rest of the XVer: */
      do {
        rl = 0;
        ok = SerialOp.read( data->serial, &rl, 1 );
        if( ok && rl > 0 ) {
          ok = SerialOp.read( data->serial, in, rl );
          if( ok && idx == 4 && rl == 5 ) {
            /* Should be sn: */
            TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
                         "Intellibox serial number --- %c%c%c%c%c%c%c%c%c%c ---",
                         ((in[0] & 0xF0) >> 4) + 0x30,
                         (in[0] & 0x0F) + 0x30,
                         ((in[1] & 0xF0) >> 4) + 0x30,
                         (in[1] & 0x0F) + 0x30,
                         ((in[2] & 0xF0) >> 4) + 0x30,
                         (in[2] & 0x0F) + 0x30,
                         ((in[3] & 0xF0) >> 4) + 0x30,
                         (in[3] & 0x0F) + 0x30,
                         ((in[4] & 0xF0) >> 4) + 0x30,
                         (in[4] & 0x0F) + 0x30
                        );
          }
        }
        else if( rl == 0 ) {
          /* End of sequence. */
          break;
        }
        idx++;
      } while( ok && rl > 0 && idx < 10 );

      MutexOp.post( data->mux );


    }
    else {
      MutexOp.post( data->mux );
      return False;
    }
  }

  return True;
}


/*
 ***** Public functions.
 */
static Boolean _setListener( obj inst, obj listenerObj,
                             const digint_listener listenerFun ) {
  iOP50xData data = Data(inst);
  data->listenerObj = listenerObj;
  data->listenerFun = listenerFun;
  return True;
}

static iONode _cmd( obj inst, const iONode nodeA ) {
  iOP50xData o = Data(inst);
  unsigned char out[256];
  unsigned char in [512];
  int    insize    = 0;
  int    inendbyte = -1;
  iONode nodeB     = NULL;

  MemOp.set( in, 0x00, sizeof( in ) );

  if( nodeA != NULL ) {
    int size = __translate( o, nodeA, out, &insize, &inendbyte );

    if( StrOp.equals( NodeOp.getName( nodeA ), wSysCmd.name() ) && StrOp.equals( wSysCmd.getcmd( nodeA ), "stopio" ) ) {
      o->stopio = True;
    }
    else if( StrOp.equals( NodeOp.getName( nodeA ), wSysCmd.name() ) && StrOp.equals( wSysCmd.getcmd( nodeA ), "startio" ) ) {
      o->stopio = False;
    }

    else if( __transact( o, (char*)out, size, (char*)in, insize, inendbyte, o->timeout ) ) {
      /* inform listener */
      if( insize > 0 ) {
        if( StrOp.equals( NodeOp.getName( nodeA ), wSwitch.name() ) ) {
          if( in[0] == 0x10 || in[0] == 0x09 ) {
            /* XNOISPC: resend later... */
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "XTrnt rc=%02Xh; fifo buffer full", in[0] );
            nodeB = (iONode)NodeOp.base.clone( nodeA );
            wResponse.setretry( nodeB, True );
          }
          else if( in[0] != 0 ) {
            /* Error / Warning */
            TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "XTrnt rc=%02Xh", in[0] );
            TraceOp.dump( NULL, TRCLEVEL_EXCEPTION, (char*)out, size );
            nodeB = NodeOp.inst( NodeOp.getName( nodeA ), NULL, ELEMENT_NODE );
            wResponse.setrc( nodeB, 0 );
            /*wResponse.setrc( nodeB, (int)in[0] );*/
            /*wResponse.seterror( nodeB, True );*/
          }
        }
        else {
          char* s = StrOp.byteToStr( in, insize );
          nodeB = NodeOp.inst( NodeOp.getName( nodeA ), NULL, ELEMENT_NODE );
          wResponse.setdata( nodeB, s );
          StrOp.free(s);
        }
      }
    }
    /* Cleanup Node1 */
    nodeA->base.del(nodeA);
  }
  /* return Node2 */
  return nodeB;
}

static void _halt( obj inst, Boolean poweroff ) {
  iOP50xData data = Data(inst);
  unsigned char p50[2];

  data->run = False;
  if( poweroff ) {
    p50[0] = (unsigned char)97;
    __transact( data, (char*)p50, 1, NULL, 0, -1, 10 );
  }
  SerialOp.close( data->serial );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shutting down <%s>...", data->iid );
}


static Boolean _supportPT( obj inst ) {
  iOP50xData data = Data(inst);
  return True;
}


static void __evaluateState( iOP50xData o, unsigned char* fb1, unsigned char* fb2, int size ) {
  int i = 0;
  for( i = 0; i < size; i++ ) {
    if( fb1[i] != fb2[i] ) {
      int n = 0;
      int addr = 0;
      int state = 0;
      for( n = 0; n < 8; n++ ) {
        if( (fb1[i] & (0x01 << n)) != (fb2[i] & (0x01 << n)) ) {
          addr = i * 8 + (7-n);
          state = (fb2[i] & (0x01 << n)) ? 1:0;
          /*
          TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "fb2[%d] i=%d, n=%d", i - i%2, i, n );
          TraceOp.dump ( name, TRCLEVEL_BYTE, &fb2[i-i%2], 2 );
          */
          addr++;
          TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "fb %d = %d", addr, state );
          {
            /* inform listener: Node3 */
            iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
            wFeedback.setaddr( nodeC, addr );
            wFeedback.setstate( nodeC, state?True:False );
            if( o->iid != NULL )
              wFeedback.setiid( nodeC, o->iid );

            if( o->listenerFun != NULL && o->listenerObj != NULL )
              o->listenerFun( o->listenerObj, nodeC, TRCLEVEL_INFO );
          }
        }
      }
    }
  }
}


static void __evaluateLocoNet( iOP50xData o, int module, byte* value ) {
  /* assuming Lissy */
  int identifier = (value[1] << 8) + value[0];
  iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
  wFeedback.setaddr( nodeC, module );
  wFeedback.setstate( nodeC, identifier > 0 ? True:False );
  wFeedback.setidentifier( nodeC, identifier );
  if( o->iid != NULL )
    wFeedback.setiid( nodeC, o->iid );

  if( o->listenerFun != NULL && o->listenerObj != NULL )
    o->listenerFun( o->listenerObj, nodeC, TRCLEVEL_INFO );
}


static void __evaluatePTevent( iOP50x p50, byte* in, int size ) {
  iOP50xData o = Data(p50);

  if( size >= 1 ) {
    int cmd = 0;
    int status = in[0];
    int val = -1;

    if( size == 1 ) {
      char* statusstring = "-";
      switch( status ) {
        case 0xFF:
          statusstring = "Timeout";
          break;
        case 0xFE:
          statusstring = "No acknowledge";
          break;
        case 0xFD:
          statusstring = "Short cut!";
          break;
        case 0xFC:
          statusstring = "No decoder detected";
          break;
        case 0xFB:
          statusstring = "Generic error";
          break;
        case 0xFA:
          statusstring = "Error during DCC direct bit mode operation";
          break;
        case 0xF5:
          statusstring = "No PT event to report or pending operation...";
          break;
        case 0xF4:
          statusstring = "Task terminated";
          break;
        case 0x00:
          statusstring = "Command completed, no errors";
          val = o->cv_val;
          break;
        default:
          statusstring = "**Check manual for meaning of status**";
          break;
      }
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "PT [%s] status=%d", statusstring, status );
      cmd = wProgram.statusrsp;
    }
    else {
      val = in[1]; /* cv value or status */
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "CV value=%d", val );
      cmd = wProgram.datarsp;
    }

    /* inform listener */
    iONode node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setcv( node, o->cv_nr );
    wProgram.setvalue( node, val );
    wProgram.setcmd( node, cmd );
    if( o->iid != NULL )
      wProgram.setiid( node, o->iid );

    if( o->listenerFun != NULL && o->listenerObj != NULL )
      o->listenerFun( o->listenerObj, node, TRCLEVEL_INFO );

  }
}


static void __PTeventReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOP50x p50 = (iOP50x)ThreadOp.getParm( th );
  iOP50xData o = Data(p50);
  p50state state = P50_OK;
  byte out[8];
  byte in [32];

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "PTevent reader started." );
  do {

    ThreadOp.sleep( 250 );

    out[0] = (byte)'x';
    out[1] = 0xCE;

    if( !o->stopio && !o->dummyio && MutexOp.trywait( o->mux, o->timeout ) ) {
      Boolean ptEvent = False;
      out[1] = 0xC8;
      state = __cts( o );
      if( state == P50_OK ) {
        if( SerialOp.write( o->serial, (char*)out, 2 ) ) {
          byte evt[3] = {0,0,0};
          if( SerialOp.read( o->serial, (char*)&evt[0], 1 ) ) {
            /* 1st flag TODO: evaulate
            TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                "1st event flags = 0x%02X", evt[0] ); */
            if( evt[0] & 0x80 ) {
              if( SerialOp.read( o->serial, (char*)&evt[1], 1 ) ) {
                /* 2nd flag TODO: evaulate */
                TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                    "2nd event flags = 0x%02X", evt[1] );
                if( evt[1] & 0x80 ) {
                  if( SerialOp.read( o->serial, (char*)&evt[2], 1 ) ) {
                    /* 3rd flag TODO: evaulate */
                    ptEvent = (evt[2] & 0x01) ? True:False;
                    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
                        "3rd event flags = 0x%02X", evt[2] );
                  }
                }
              }
            }
          }
        }
      }

      if( ptEvent ) {
        out[1] = 0xCE;
        state = __cts( o );
        if( state == P50_OK ) {
          if( SerialOp.write( o->serial, (char*)out, 2 ) ) {
            byte evt = 0;
            if( SerialOp.read( o->serial, (char*)&evt, 1 ) ) {
              if( evt != 0xF5 && evt < 32 ) {
                if( SerialOp.read( o->serial, (char*)in, evt ) ) {
                  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "PT Event." );
                  TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, evt );
                  __evaluatePTevent( p50, in, evt );
                }
              }
              else {
                TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "PT status = %d (0x%02X)", evt, evt );
              }
            }
          }
        }
      }

      MutexOp.post( o->mux );
    }

  } while( o->run );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Event reader ended." );
}


static void __handleSwitch(iOP50x p50x, int pada, int state) {
  iOP50xData data = Data(p50x);

  int port;
  int addr;
  int value;

  if( state == 0x80) value = 1;
  else value = 0;

  fromPADA( pada, &addr, &port );

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sw %d %d = %s", addr, port, value?"straight":"thrown" );

  {
    iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );

    wSwitch.setaddr1( nodeC, ( addr  ));
    wSwitch.setport1( nodeC, ( port  ));

    if( data->iid != NULL )
      wSwitch.setiid( nodeC, data->iid );

    wSwitch.setstate( nodeC, value?"straight":"turnout" );

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}


static void __handleLoco(iOP50x p50x, byte* status) {
  iOP50xData data = Data(p50x);

  iONode nodeC = NULL;

  int addrL = status[2];
  int addrH = status[3] & 0x3F;
  int addr  = addrL + (addrH << 8);

  TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)status, 5 );

  /* inform listener: Node3 */
  nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  if( data->iid != NULL )
    wLoc.setiid( nodeC, data->iid );
  wLoc.setaddr( nodeC, addr );
  wLoc.setV_raw( nodeC, status[0] );
  wLoc.setV_rawMax( nodeC, 127 );
  wLoc.setfn( nodeC, (status[3] & 0x40) ? True:False);
  wLoc.setdir( nodeC, (status[3] & 0x80) ? True:False );
  wLoc.setthrottleid( nodeC, "p50x" );
  wLoc.setcmd( nodeC, wLoc.direction );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "addr=%d V_raw=%d dir=%s fn=%s [%02X]",
      addr, status[0], wLoc.isdir(nodeC)?"fwd":"rev", wLoc.isfn(nodeC)?"on":"off", status[3] );
  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

  nodeC = NodeOp.inst( wFunCmd.name(), NULL, ELEMENT_NODE );
  if( data->iid != NULL )
    wLoc.setiid( nodeC, data->iid );
  wFunCmd.setaddr( nodeC, addr );
  wFunCmd.setf0( nodeC, (status[3] & 0x40) ? True:False );
  wFunCmd.setf1( nodeC, (status[1] & 0x01) ? True:False );
  wFunCmd.setf2( nodeC, (status[1] & 0x02) ? True:False );
  wFunCmd.setf3( nodeC, (status[1] & 0x04) ? True:False );
  wFunCmd.setf4( nodeC, (status[1] & 0x08) ? True:False );
  wFunCmd.setf5( nodeC, (status[1] & 0x10) ? True:False );
  wFunCmd.setf6( nodeC, (status[1] & 0x20) ? True:False );
  wFunCmd.setf7( nodeC, (status[1] & 0x40) ? True:False );
  wFunCmd.setf8( nodeC, (status[1] & 0x80) ? True:False );
  wLoc.setthrottleid( nodeC, "p50x" );
  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );


}

static void __dummy( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOP50x p50 = (iOP50x)ThreadOp.getParm( th );
  iOP50xData data = Data(p50);
  iONode nodeC = NULL;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "tester started." );
  ThreadOp.sleep( 2000 );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sending throttle event..." );
  nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  if( data->iid != NULL )
    wLoc.setiid( nodeC, data->iid );
  wLoc.setaddr( nodeC, 1 );
  wLoc.setV_raw( nodeC, 10 );
  wLoc.setV_rawMax( nodeC, 127 );
  wLoc.setthrottleid( nodeC, "dummy" );
  wLoc.setcmd( nodeC, wLoc.direction );
  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

  ThreadOp.sleep( 1000 );
  nodeC = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  if( data->iid != NULL )
    wLoc.setiid( nodeC, data->iid );
  wLoc.setaddr( nodeC, 1 );
  wLoc.setV_raw( nodeC, 77 );
  wLoc.setV_rawMax( nodeC, 127 );
  wLoc.setthrottleid( nodeC, "dummy" );
  wLoc.setcmd( nodeC, wLoc.direction );
  data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "tester ended." );
}


static void __statusReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOP50x p50 = (iOP50x)ThreadOp.getParm( th );
  iOP50xData o = Data(p50);
  iOList evtList = ListOp.inst();
  byte out[8];
  byte in[512];
  p50state state = P50_OK;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Status reader started." );
  ThreadOp.sleep( 1000 );
  if( !o->dummyio )
    __getversion(p50);

  do {

    ThreadOp.sleep( 250 );

    if( !o->stopio && !o->dummyio && MutexOp.trywait( o->mux, o->timeout ) ) {

      if( !__flushP50x(o) ) {
        MutexOp.post( o->mux );
        continue;
      }

      out[0] = (byte)'x';
      out[1] = 0xA2; /* xStatus */
      if( SerialOp.write( o->serial, (char*)out, 2 ) ) {
        if( SerialOp.read( o->serial, (char*)in, 1 ) ) {
          Boolean power = (in[0] & 0x08) ? True:False;
          Boolean hot   = (in[0] & 0x04) ? True:False;
          Boolean halt  = (in[0] & 0x10) ? True:False;

          if( power != o->power || hot != o->hot || halt != o->halt ) {
            iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
            if( o->iid != NULL )
              wState.setiid( node, o->iid );
            wState.setpower( node, power );
            wState.settrackbus( node, !halt );
            wState.setsensorbus( node, power );
            wState.setaccessorybus( node, power );

            if( o->listenerFun != NULL && o->listenerObj != NULL )
              o->listenerFun( o->listenerObj, node, TRCLEVEL_INFO );

            o->power = power;
            o->hot   = hot;
            o->halt  = halt;
          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no xStatus reply" );
        }
      }



      /* Turnout event */
      out[0] = (byte)'x';
      out[1] = 0xCA; // ask for turnout changes
      if( SerialOp.write( o->serial, (char*)out, 2 ) ) {
        byte ans = 0;
        if( SerialOp.read( o->serial, (char*)&ans, 1 ) ) {
          if (ans > 0x00) {
            if( SerialOp.read( o->serial, (char*)in, (int) ans*2 ) ) {
              int i = 0;
              for ( i = 0; i < ans; i++) {
                 __handleSwitch(p50, in[i*2], in[i*2+1]);
              }
            }
            else {
              TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unable to read switch event");
              TraceOp.dump( name, TRCLEVEL_WARNING, in, SerialOp.getReadCnt(o->serial) );
            }

          }
        }
        else {
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no XEvtTrnt reply" );
        }
      }


      /* Locomotive 'event' */
      out[0] = (byte)'x';
      out[1] = 0xC9;
      /* ask for locomotive changes */
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "check for XEvtLok..." );
      TraceOp.dump( name, TRCLEVEL_DEBUG, out, 2 );
      ListOp.clear(evtList);
      if( SerialOp.write( o->serial, (char*)out, 2 ) ) {
        byte* evt = NULL;
        do {
          Boolean read = SerialOp.read( o->serial, (char*)&in[0], 1 ) ;
          if( read ) {
            TraceOp.dump( name, TRCLEVEL_DEBUG, in, 1 );
            if (in[0] < 0x80 ) {
              if( SerialOp.read( o->serial, (char*)in+1, 4 ) ) {
                TraceOp.dump( name, TRCLEVEL_DEBUG, in, 5 );
                evt = allocMem(5);
                MemOp.copy( evt, in, 5);
                ListOp.add(evtList, (obj)evt);
              }
              else {
                TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "unable to read loco event");
              }
            }
            else {
              break;
            }
          }
          else {
            TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "no XEvtLok reply" );
            break;
          }
        } while(in[0] != 0x80);
      }


      MutexOp.post( o->mux );

      /* process the loco events */
      if( ListOp.size(evtList) > 0) {
        int i = 0;
        for( i = 0; i < ListOp.size(evtList); i++ ) {
          byte* evt = (byte*)ListOp.get(evtList, i );
          __handleLoco(p50, evt);
          freeMem(evt);
        }
        ListOp.clear(evtList);
      }

    }

  } while( o->run );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Status reader ended." );

}

static void __feedbackReader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOP50x p50 = (iOP50x)ThreadOp.getParm( th );
  iOP50xData o = Data(p50);
  unsigned char* fb = allocMem(256);
  byte out[256];
  byte in [512];
  byte tmp [8];
  byte into [512];
  p50state state = P50_OK;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback p50x reader started." );
  /* set byte arrays to a defined state: */
  MemOp.set( out, 0, 256 );
  MemOp.set(  in, 0, 512 );
  MemOp.set(into, 0, 512 );

  out[0] = 'x';
  out[1] = 0x99;
  __transact( o, (char*)out, 2, in, 1, -1, o->timeout );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback p50x reader initialized." );
  do {

    ThreadOp.sleep( 250 );

    out[0] = (byte)'x';
    out[1] = 0xCB;

    if( !o->stopio && !o->dummyio && MutexOp.trywait( o->mux, o->timeout ) ) {
      if( o->tok)
        printf( "\n*****token!!! A\n\n" );
      o->tok = True;
      state = __cts( o );
      if( state == P50_OK ) {
        if( SerialOp.write( o->serial, (char*)out, 2 ) ) {
          byte module = 0;
          state = P50_OK;
          if( SerialOp.read( o->serial, (char*)&module, 1 ) ) {
            /* TODO: modules > 31 are loconet */
            while( module > 0 && state == P50_OK ) {
              TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "fbModule = %d", module );

              if( !SerialOp.read( o->serial, (char*)tmp, 2 ) ) {
                state = P50_RCVERR;
                break;
              }

              if( module < 32 ) {
                /* s88 */
                in[(module-1)*2] = tmp[0];
                in[((module-1)*2)+1] = tmp[1];
              }
              else {
                /* loconet */
                __evaluateLocoNet( o, module, tmp );
              }

              if( !SerialOp.read( o->serial, (char*)&module, 1 ) ) {
                state = P50_RCVERR;
                break;
              }
            };
          }
          else
            state = P50_RCVERR;
        }
        else
          state = P50_SNDERR;

      }


      if( state != P50_OK ) {
        const char* strState = state == P50_RCVERR?"RCVERR":"SNDERR";
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
          "ERROR reading feedbacks!!! rc=%d state=%s\n", SerialOp.getRc( o->serial ), strState );
      }

      o->tok = False;
      MutexOp.post( o->mux );

      /* only compare if communication was OK: */
      if( state == P50_OK ) {
        if( memcmp( fb, in, o->fbmod * 2 ) != 0 ) {
          /* inform listener */
          __evaluateState( o, fb, in, o->fbmod * 2);
          memcpy( fb, in, o->fbmod * 2 );
        }
      }

    }

  } while( o->run );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback p50x reader ended." );
}


static void __feedbackP50Reader( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOP50x p50x = (iOP50x)ThreadOp.getParm( th );
  iOP50xData data = Data(p50x);
  unsigned char* fb = allocMem(256);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback p50 reader started." );
  do {
    unsigned char out[256];
    unsigned char in [512];

    ThreadOp.sleep( 200 );
    if( data->stopio || data->fbmod == 0 )
      continue;

    out[0] = (unsigned char)(128 + data->fbmod);
    if( __transact( data, (char*)out, 1, (char*)in, data->fbmod * 2, -1, data->timeout ) ) {
      if( memcmp( fb, in, data->fbmod * 2 ) != 0 ) {
        /* inform listener */
        __evaluateState( data, fb, in, data->fbmod * 2);
        memcpy( fb, in, data->fbmod * 2 );
      }
    }
  } while( data->run );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Feedback p50 reader ended." );
}

/* Status */
static int _state( obj inst ) {
  iOP50xData data = Data(inst);
  int state = 0;
  return state;
}

/* external shortcut event */
static void _shortcut(obj inst) {
  iOP50xData data = Data( inst );
}


/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOP50xData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}

static iOP50x _inst( const iONode settings, const iOTrace trace ) {
  iOP50x     p50x = allocMem( sizeof( struct OP50x ) );
  iOP50xData data = allocMem( sizeof( struct OP50xData ) );
  iOAttr attr = NULL;
  const char* flow = NULL;
  const char* parity = NULL;

  TraceOp.set( trace );

  /* OBase */
  MemOp.basecpy( p50x, &P50xOp, 0, sizeof( struct OP50x ), data );


  data->mux = MutexOp.inst( NULL, True );

  /* Evaluate attributes. */
  data->device   = StrOp.dup( wDigInt.getdevice( settings ) );
  data->iid      = StrOp.dup( wDigInt.getiid( settings ) );

  data->bps      = wDigInt.getbps( settings );
  data->bits     = wDigInt.getbits( settings );
  data->stopBits = wDigInt.getstopbits( settings );
  data->timeout  = wDigInt.gettimeout( settings );
  data->fbmod    = wDigInt.getfbmod( settings );
  data->swtime   = wDigInt.getswtime( settings );
/*  data->psleep   = wDigInt.getpsleep( settings, "psleep"  , 100    );*/
  data->dummyio  = wDigInt.isdummyio( settings );
  data->ctsretry = wDigInt.getctsretry( settings );
  data->readfb   = wDigInt.isreadfb( settings );
  data->run      = True;

  data->serialOK = False;
  data->initOK = False;


  parity      = wDigInt.getparity( settings );
  flow        = wDigInt.getflow( settings );

  if( StrOp.equals( wDigInt.even, parity ) )
    data->parity = even;
  else if( StrOp.equals( wDigInt.odd, parity ) )
    data->parity = odd;
  else if( StrOp.equals( wDigInt.none, parity ) )
    data->parity = none;

  if( StrOp.equals( wDigInt.dsr, flow ) )
    data->flow = dsr;
  else if( StrOp.equals( wDigInt.cts, flow ) )
    data->flow = cts;
  else if( StrOp.equals( wDigInt.xon, flow ) )
    data->flow = xon;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "p50x %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iOP50x[%s]: %s,%d,%s,%d,%d,%s",
        wDigInt.getiid( settings ) != NULL ? wDigInt.getiid( settings ):"",
        data->device, data->bps, parity, data->bits, data->stopBits, flow );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "p50x timeout=%d", data->timeout );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "p50x ctsretry=%d", data->ctsretry );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "p50x readfb=%d", data->readfb );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "p50x fbmod=%d", data->fbmod );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "p50x swtime=%d", data->swtime );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  data->serial = SerialOp.inst( data->device );
  SerialOp.setFlow( data->serial, data->flow );
  SerialOp.setLine( data->serial, data->bps, data->bits, data->stopBits, data->parity, wDigInt.isrtsdisabled( settings ) );
  SerialOp.setTimeout( data->serial, data->timeout, data->timeout );
  data->serialOK = SerialOp.open( data->serial );

  if( data->serialOK ) {

    if( wDigInt.isptsupport( settings ) ) {
      data->eventReader = ThreadOp.inst( "evtreader", &__PTeventReader, p50x );
      ThreadOp.start( data->eventReader );
    }

    data->statusReader = ThreadOp.inst( "statreader", &__statusReader, p50x );
    ThreadOp.start( data->statusReader );

    if( data->readfb && data->fbmod > 0 ) {
      if( wDigInt.isfbpoll( settings ) )
        data->feedbackReader = ThreadOp.inst( "fbreader", &__feedbackP50Reader, p50x );
      else
        data->feedbackReader = ThreadOp.inst( "fbreader", &__feedbackReader, p50x );
      ThreadOp.start( data->feedbackReader );
    }
  }
  else {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init p50x port!" );

    /*
    data->statusReader = ThreadOp.inst( "dummy", &__dummy, p50x );
    ThreadOp.start( data->statusReader );
    */
  }

  instCnt++;

  return p50x;
}

/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/p50x.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
