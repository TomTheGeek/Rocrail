/** ------------------------------------------------------------
  * A U T O   G E N E R A T E D  (First time only!)
  * Generator: Rocs ogen (build Nov  9 2006 08:04:42)
  * Module: RocDigs
  * XML: $Source: /cvsroot/rojav/rocdigs/rocdigs.xml,v $
  * XML: $Revision: 1.14 $
  * Object: OpenDCC
  * Date: Mon Mar  3 17:05:53 2008
  * ------------------------------------------------------------
  * $Source$
  * $Author$
  * $Date$
  * $Revision$
  * $Name$
  */

#include "rocdigs/impl/opendcc_impl.h"

#include "rocint/public/digint.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/OpenDCC.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/BinCmd.h"
#include "rocrail/wrapper/public/Response.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/Clock.h"

#include "rocs/public/mem.h"
#include "rocs/public/lib.h"
#include "rocs/public/system.h"

#include "rocutils/public/addr.h"

#include <time.h>

static int instCnt = 0;

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iOOpenDCCData data = Data(inst);
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
  iOOpenDCCData data = Data(inst);
  return data->ini;
}

/** ----- OOpenDCC ----- */


static int __normalizeSteps(int insteps ) {
  /* SPEEDSTEPS: vaild: 14, 28, 126 */
  if( insteps < 20 )
    return 14;
  if( insteps > 100 )
    return 126;
  return 28;
}


/**  */
static iONode _cmd( obj inst ,const iONode cmd ) {
  iOOpenDCCData data = Data(inst);
  iONode response = NULL;

  /* TODO: check for OpenDCC specific commands like programming the CS
   * and make bincmd's for the p50x
   * wProgram.getlntype == wProgram.lntype_cs
   * wProgram.getcmd == wProgram.get or wProgram.set
   * wProgram.getcv  -> SO register
   * wProgram.getval -> SO value
   *  */

  /* Clock command. */
  if( StrOp.equals( NodeOp.getName( cmd ), wClock.name() ) && wOpenDCC.isfastclock( data->opendccini ) ) {
    /*
    # XClkSet (0xC0) - Length = 5 Bytes

    command bytes:
    0: 0xC0 XClkSet: fast clock
    1: TCODE0: 00mmmmmm, this denotes the minute, range 0..59.
    2: TCODE1: 100HHHHH, this denotes the hour, range 0..23
    3: TCODE2: 01000WWW, this denotes the day of week,
    0=Monday, 1=Tuesday, 2=Wednesday, 3=Thursday, 4=Friday, 5=Saturday, 6=Sunday.
    4: TCODE3: 110FFFFF, this denotes the acceleration factor, range 0..31;
    an acceleration factor of 0 means clock is stopped,
    a factor of 1 means clock is running real time,
    a factor of 2 means clock is running twice as fast a real time.

    Answer: 0 (Kommando okay)
    */
    iONode clockcmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
    char* byteStr = NULL;
    byte outBytes[6];
    long l_time = wClock.gettime(cmd);
    struct tm* lTime = localtime( &l_time );

    int mins    = lTime->tm_min;
    int hours   = lTime->tm_hour;
    int wday    = lTime->tm_wday;
    int divider = wClock.getdivider(cmd);

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999,
        "%s clock to %02d:%02d divider=%d", wClock.getcmd( cmd ), hours, mins, divider );

    if( StrOp.equals( wDigInt.p50x, data->sublibname )  && StrOp.equals( wClock.set, wClock.getcmd( cmd ) ) ) {
      outBytes[0] = (byte)'x';
      outBytes[1] = 0xC0;
      outBytes[2] = 0x00 + mins;
      outBytes[3] = 0x80 + hours;
      outBytes[4] = 0x40 + wday;
      outBytes[5] = 0xC0 + divider;

      byteStr = StrOp.byteToStr( outBytes, 6 );
      wBinCmd.setoutlen( clockcmd, 6 );
      wBinCmd.setinlen( clockcmd, 1 );
      wBinCmd.setout( clockcmd, byteStr );
      StrOp.free( byteStr );
      response = data->sublib->cmd((obj)data->sublib, clockcmd);
    }
    else {
      /* the lenz protocol knows the clock command */
      response = data->sublib->cmd((obj)data->sublib, cmd);
    }

  }

  /* Program command. */
  else if( StrOp.equals( NodeOp.getName( cmd ), wProgram.name() ) &&
      wProgram.getlntype( cmd ) == wProgram.lntype_cs )
  {
    iONode ptcmd = NULL;
    Boolean getCV = False;

    if(  wProgram.getcmd( cmd ) == wProgram.set ) {
      if( StrOp.equals( wDigInt.p50x, data->sublibname ) ) {
        ptcmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
        char* byteStr = NULL;
        byte outBytes[6];
        outBytes[0] = (byte)'x';
        outBytes[1] = 0xA3;
        outBytes[2] = wProgram.getcv(cmd) % 256;
        outBytes[3] = wProgram.getcv(cmd) / 256;
        outBytes[4] = wProgram.getvalue(cmd);

        byteStr = StrOp.byteToStr( outBytes, 5 );
        wBinCmd.setoutlen( ptcmd, 5 );
        wBinCmd.setinlen( ptcmd, 1 );
        wBinCmd.setout( ptcmd, byteStr );
        StrOp.free( byteStr );
      }
      else {
        /* lenz sublib
           0x24 0x29 AddrH AddrL DAT [XOR]
           SO write request. The answer is 0x24 0x28 AddrH AddrL DATA [XOR]
        */
        ptcmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
        char* byteStr = NULL;
        byte outBytes[6];
        outBytes[0] = 0x24;
        outBytes[1] = 0x29;
        outBytes[2] = wProgram.getcv(cmd) / 256;
        outBytes[3] = wProgram.getcv(cmd) % 256;
        outBytes[4] = wProgram.getvalue(cmd);

        byte bXor = 0;
        int i = 0;
        for( i = 0; i < 5; i++ ) {
          bXor ^= outBytes[ i ];
        }
        outBytes[5] = bXor;

        byteStr = StrOp.byteToStr( outBytes, 6 );
        wBinCmd.setoutlen( ptcmd, 6 );
        wBinCmd.setinlen( ptcmd, 6 );
        wBinCmd.setout( ptcmd, byteStr );
        StrOp.free( byteStr );

      }
    }
    else if(  wProgram.getcmd( cmd ) == wProgram.get ) {
      if( StrOp.equals( wDigInt.p50x, data->sublibname ) ) {
        ptcmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
        char* byteStr = NULL;
        byte outBytes[4];
        outBytes[0] = (byte)'x';
        outBytes[1] = 0xA4;
        outBytes[2] = wProgram.getcv(cmd) % 256;
        outBytes[3] = wProgram.getcv(cmd) / 256;

        byteStr = StrOp.byteToStr( outBytes, 4 );
        wBinCmd.setoutlen( ptcmd, 4 );
        wBinCmd.setinlen( ptcmd, 2 );
        wBinCmd.setout( ptcmd, byteStr );
        StrOp.free( byteStr );

        getCV = True;
      }
      else {
        /* lenz sublib
           0x23 0x28 AddrH AddrL [XOR]
           SO (special option) read request; this allows to read the internal configuration values.
           AddrH AddrL denotes the address of the CV.
           The answer is 0x24 0x28 AddrH AddrL DATA [XOR]
         */
        ptcmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
        char* byteStr = NULL;
        byte outBytes[5];
        outBytes[0] = 0x23;
        outBytes[1] = 0x28;
        outBytes[2] = wProgram.getcv(cmd) / 256;
        outBytes[3] = wProgram.getcv(cmd) % 256;

        byte bXor = 0;
        int i = 0;
        for( i = 0; i < 4; i++ ) {
          bXor ^= outBytes[ i ];
        }
        outBytes[4] = bXor;

        byteStr = StrOp.byteToStr( outBytes, 5 );
        wBinCmd.setoutlen( ptcmd, 5 );
        wBinCmd.setinlen( ptcmd, 6 );
        wBinCmd.setout( ptcmd, byteStr );
        StrOp.free( byteStr );

        getCV = True;
      }
    }

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "XSO%s for register %d", (getCV?"Get":"Set"), wProgram.getcv(cmd) );


    if( ptcmd != NULL ) {
      byte* inData = NULL;
      response = data->sublib->cmd((obj)data->sublib, ptcmd);
      /* TODO: convert response incase of a bincmd */
      if( response != NULL ) {
        inData = StrOp.strToByte( wResponse.getdata( response ) );
        NodeOp.base.del(response);
        response = (iONode)NodeOp.base.clone(cmd);
        if( getCV && inData[0] == 0 ) {
          wProgram.setvalue( response, inData[1] );
          wOpenDCC.setlib( response, wOpenDCC.getlib(data->opendccini) );
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
              "Successfully XSO%s [%d][%d]", (getCV?"Get":"Set"), wProgram.getcv(cmd), inData[1] );
        }
        else if( inData[0] != 0 ) {
          /* Error PT command */
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
              "Error [%s] XSO%d [%d]", (getCV?"Get":"Set"), inData[0], wProgram.getcv(response) );
        }
        freeMem(inData);
      }
    }

    /* Cleanup command node */
    NodeOp.base.del(cmd);
  }


  /* Program command. */
  else if( wProgram.ispom( cmd ) && wProgram.isacc( cmd ) && wProgram.getcmd( cmd ) == wProgram.set )
  {
    iONode ptcmd = NULL;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "POM set for ACC %d %d=%d",
        wProgram.getaddr(cmd), wProgram.getcv(cmd), wProgram.getvalue(cmd) );

    if( StrOp.equals( wDigInt.p50x, data->sublibname ) ) {
      /*
      XDCC_PA (0xDF)- Länge = 1+5 Bytes
      0: 0xDF XDCC_PA (= Accessory-Programmieren auf dem Hauptgleis = POM)
      1: LSB der Zubehöradresse
      2: MSB der Zubehöradresse (1-510)
      3: Low Byte der CV-Adresse, welche zu schreiben ist.
      4: High Byte der CV-Adresse, welche zu schreiben ist. (1..1024)
      5: Wert
      Antwort: 0 = Ok, accepted
      0x80 = busy, command ignored
      */
      ptcmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
      char* byteStr = NULL;
      byte outBytes[7];
      outBytes[0] = (byte)'x';
      outBytes[1] = 0xDF;
      outBytes[2] = wProgram.getaddr(cmd) % 256;
      outBytes[3] = wProgram.getaddr(cmd) / 256;
      outBytes[4] = wProgram.getcv(cmd) % 256;
      outBytes[5] = wProgram.getcv(cmd) / 256;
      outBytes[6] = wProgram.getvalue(cmd);

      byteStr = StrOp.byteToStr( outBytes, 7 );
      wBinCmd.setoutlen( ptcmd, 7 );
      wBinCmd.setinlen( ptcmd, 1 );
      wBinCmd.setout( ptcmd, byteStr );
      StrOp.free( byteStr );
    }
    else {
      /* lenz sublib
         0xE6 0x30 AddrH AddrL 0xF0+C CV DAT [XOR]
         Operations Mode Programming write request for accessory decoder;
         CV is given as 0..1023; C are the two upper bits.
         Address is coded like with locomotives: if value is >= 100, then the high part is OR'ed with 0xC0.
       */
      ptcmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
      char* byteStr = NULL;
      byte outBytes[8];
      outBytes[0] = 0xE6;
      outBytes[1] = 0x30;
      outBytes[2] = wProgram.getaddr(cmd) / 256;
      outBytes[3] = wProgram.getaddr(cmd) % 256;
      outBytes[4] = wProgram.getcv(cmd) / 256;
      outBytes[5] = wProgram.getcv(cmd) % 256;
      outBytes[6] = wProgram.getvalue(cmd);

      byte bXor = 0;
      int i = 0;
      for( i = 0; i < 7; i++ ) {
        bXor ^= outBytes[ i ];
      }
      outBytes[7] = bXor;

      byteStr = StrOp.byteToStr( outBytes, 8 );
      wBinCmd.setoutlen( ptcmd, 8 );
      wBinCmd.setinlen( ptcmd, 6 );
      wBinCmd.setout( ptcmd, byteStr );
      StrOp.free( byteStr );
    }

    if( ptcmd != NULL ) {
      byte* inData = NULL;
      response = data->sublib->cmd((obj)data->sublib, ptcmd);
      /* TODO: convert response incase of a bincmd */
      if( response != NULL ) {
        inData = StrOp.strToByte( wResponse.getdata( response ) );
        NodeOp.base.del(response);
        response = (iONode)NodeOp.base.clone(cmd);
        if( inData[0] != 0 ) {
          /* Error PT command */
          TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
              "Error AccSet [%d] err[%d]", wProgram.getcv(response), inData[0] );
        }
        freeMem(inData);
      }
    }

    /* Cleanup command node */
    NodeOp.base.del(cmd);
  }

  /*
     XDCC_PDR (0xDA) - Länge = 1+4 Bytes
       Befehlsbytes:
       0: 0xDA XDCC_PDR (= Lok-Programmieren auf dem Hauptgleis = POM, CV-Lesen)
       1: LSB der Lokadresse
       2: MSB der Lokadresse (1-10239)
       3: Low Byte der <a href="dcc_cv.html">CV-Adresse</a>, welche zu lesen ist.
       4: High Byte der <a href="dcc_cv.html">CV-Adresse</a>, welche zu lesen ist. (1..1024)

        Antwort: 0 = Ok, accepted.
                 0x80 = busy, command ignored
   */
  else if( StrOp.equals( NodeOp.getName( cmd ), wProgram.name() ) &&
           wProgram.ispom( cmd ) &&
           wProgram.getcmd( cmd ) == wProgram.get )
  {
    iONode ptcmd = NULL;
    if( StrOp.equals( wDigInt.p50x, data->sublibname ) ) {
      ptcmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
      char* byteStr = NULL;
      byte outBytes[6];
      outBytes[0] = (byte)'x';
      outBytes[1] = 0xDA;
      outBytes[2] = wProgram.getaddr(cmd) % 256;
      outBytes[3] = wProgram.getaddr(cmd) / 256;
      outBytes[4] = wProgram.getcv(cmd) % 256;
      outBytes[5] = wProgram.getcv(cmd) / 256;

      byteStr = StrOp.byteToStr( outBytes, 6 );
      wBinCmd.setoutlen( ptcmd, 6 );
      wBinCmd.setinlen( ptcmd, 1 );
      wBinCmd.setout( ptcmd, byteStr );
      StrOp.free( byteStr );
      data->sublib->cmd((obj)data->sublib, ptcmd);
    }
    else {
      /* lenz sublib */
      response = data->sublib->cmd((obj)data->sublib, cmd);
    }
  }

  /* switch command */
  else if( StrOp.equals( NodeOp.getName( cmd ), wSwitch.name() ) ) {
    /* OpenDCC does not switch off the gate */
    iONode cmdoff = (iONode)NodeOp.base.clone(cmd);
    int swtime = wSwitch.getdelay(cmd);
    if( swtime == 0 )
      swtime = wDigInt.getswtime(data->ini);
    response = data->sublib->cmd((obj)data->sublib, cmd);

    if( swtime > 0 ) {
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "posting to switch delay thread" );
      wSwitch.setdelay( cmdoff, swtime );
      wSwitch.setdelaytime( cmdoff, SystemOp.getMillis() );
      ThreadOp.post( data->swdelay, (obj)cmdoff );
    }
    else {
      /* free up the unused node */
      NodeOp.base.del(cmdoff);
    }
  }


  /* TODO: Extended DCC accessory command.
   *
    OpenDCC from V0.23 on supports the extended accessory command of DCC.
    This allows for a easier control of signals with more than two aspects.
    For access, please use the following command extensions to p50x:

    XTrntX (0x91)- length = 1+2 bytes

    Bytes:
    0: 0x91 XTrntX
    1: LSB decoderaddress (A7 ... A0)
    2: MSB decoderadress and signal aspect
    bit# 7 6 5 4 3 2 1 0
    +-----+-----+-----+-----+-----+-----+-----+-----+
    | B4 | B3 | B2 | B1 | B0 | A10 | A9 | A8 |
    +-----+-----+-----+-----+-----+-----+-----+-----+
    A10..A0: Addr of Decoder
    B4..B0: Aspect

    XpressNet:

    0x13 0x01 B+AddrH AddrL

    DCC extended accessory command request. (compact form)
    AddrH (bit A10..A8) and AddrL (bit A7..A0) form a 11-bit decoder address, B (=B4..B0) form the desired aspect.
    All bits B4..B0 equal 0 define the 'stop'-aspect.
    The command station shall issue a DCC extended accessory command as defined in NMRA 9.2.1.

   */
  else if( StrOp.equals( NodeOp.getName( cmd ), wSignal.name() ) ) {
    int mod = wSignal.getaddr( cmd );
    int pin = wSignal.getport1( cmd );
    int addr = 0;
    int gate = wSignal.getgate1( cmd );

    if( pin == 0 )
      AddrOp.fromFADA( mod, &mod, &pin, &gate );
    else if( mod == 0 && pin > 0 )
      AddrOp.fromPADA( pin, &mod, &pin );

    addr = (mod-1) * 4 + pin;

    if( StrOp.equals( wDigInt.p50x, data->sublibname ) ) {
      /* p50x sublib */
      iONode sgcmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
      char* byteStr = NULL;
      byte outBytes[6];
      outBytes[0] = (byte)'x';
      outBytes[1] = 0x91; /* XTrntX */
      outBytes[2] = addr % 256;
      outBytes[3] = addr / 256;
      outBytes[3] |= wSignal.getaspect(cmd) << 3;

      byteStr = StrOp.byteToStr( outBytes, 4 );
      wBinCmd.setoutlen( sgcmd, 4 );
      wBinCmd.setinlen( sgcmd, 1 ); /* ? */
      wBinCmd.setout( sgcmd, byteStr );
      StrOp.free( byteStr );
      data->sublib->cmd((obj)data->sublib, sgcmd);
    }
    else {
      /* lenz sublib */
      iONode sgcmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
      char* byteStr = NULL;
      byte outBytes[5];
      outBytes[0] = 0x13;
      outBytes[1] = 0x01;
      outBytes[2] = addr / 256;
      outBytes[2] |= wSignal.getaspect(cmd) << 3;
      outBytes[3] = addr % 256;

      byte bXor = 0;
      int i = 0;
      for( i = 0; i < 4; i++ ) {
        bXor ^= outBytes[ i ];
      }
      outBytes[4] = bXor;

      byteStr = StrOp.byteToStr( outBytes, 5 );
      wBinCmd.setoutlen( sgcmd, 5 );
      wBinCmd.setinlen( sgcmd, 0 ); /* ? */
      wBinCmd.setout( sgcmd, byteStr );
      StrOp.free( byteStr );
      data->sublib->cmd((obj)data->sublib, sgcmd);
    }
  }


  else if( StrOp.equals( NodeOp.getName( cmd ), wLoc.name() ) ) {
    if( StrOp.equals( wLoc.shortid, wLoc.getcmd(cmd) ) ) {
      /* send short ID to OpenDCC */
      if( StrOp.equals( wDigInt.p50x, data->sublibname ) ) {
        /* add the loco to the data bank, or overwrite the existing */
        iONode lccmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
        char* str = StrOp.fmt( "XLOCADD %d,%d,DCC,%s\r", wLoc.getaddr(cmd),
            __normalizeSteps(wLoc.getspcnt(cmd)), wLoc.getshortid(cmd) );
        char* byteStr = StrOp.byteToStr( str, StrOp.len(str) );
        TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, str );
        wBinCmd.setoutlen( lccmd, StrOp.len(str) );
        wBinCmd.setinlen( lccmd, 256 );
        wBinCmd.setinendbyte( lccmd, '\r' );
        wBinCmd.setout( lccmd, byteStr );
        StrOp.free( byteStr );
        StrOp.free( str );
        response = data->sublib->cmd((obj)data->sublib, lccmd);
      }
      else {
        /* lenz sublib */
      }
    }
    else {
      response = data->sublib->cmd((obj)data->sublib, cmd);
    }
  }
  else if( StrOp.equals( NodeOp.getName( cmd ), wSysCmd.name() ) ) {
    if( StrOp.equals( wSysCmd.txshortids, wSysCmd.getcmd(cmd) ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "send short IDs to the throttle" );
      if( StrOp.equals( wDigInt.p50x, data->sublibname ) ) {
        /* dump the loco data base in the throttle */
        iONode lccmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
        char* cmd = "XLOCXMT\r";
        char* byteStr = StrOp.byteToStr( cmd, StrOp.len(cmd) );
        wBinCmd.setoutlen( lccmd, StrOp.len(cmd) );
        wBinCmd.setinlen( lccmd, 256 );
        wBinCmd.setinendbyte( lccmd, '\r' );
        wBinCmd.setout( lccmd, byteStr );
        StrOp.free( byteStr );
        response = data->sublib->cmd((obj)data->sublib, lccmd);
      }
      else {
        /* lenz sublib */
      }
    }
    else if( StrOp.equals( wSysCmd.clearshortids, wSysCmd.getcmd(cmd) ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "clear short IDs" );
      if( StrOp.equals( wDigInt.p50x, data->sublibname ) ) {
        /* dump the loco data base in the throttle */
        iONode lccmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
        char* cmd = "XLOCCLEAR\r";
        char* byteStr = StrOp.byteToStr( cmd, StrOp.len(cmd) );
        wBinCmd.setoutlen( lccmd, StrOp.len(cmd) );
        wBinCmd.setinlen( lccmd, 256 );
        wBinCmd.setinendbyte( lccmd, '\r' );
        wBinCmd.setout( lccmd, byteStr );
        StrOp.free( byteStr );
        response = data->sublib->cmd((obj)data->sublib, lccmd);
      }
      else {
        /* lenz sublib */
      }
    }
    else if( StrOp.equals( wSysCmd.ebreak, wSysCmd.getcmd(cmd) ) ) {
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "emergency break" );
      if( StrOp.equals( wDigInt.p50x, data->sublibname ) ) {
        iONode lccmd = NodeOp.inst( wBinCmd.name(), NULL, ELEMENT_NODE );
        char* cmd = "XHALT\r";
        char* byteStr = StrOp.byteToStr( cmd, StrOp.len(cmd) );
        wBinCmd.setoutlen( lccmd, StrOp.len(cmd) );
        wBinCmd.setinlen( lccmd, 256 );
        wBinCmd.setinendbyte( lccmd, '\r' );
        wBinCmd.setout( lccmd, byteStr );
        StrOp.free( byteStr );
        response = data->sublib->cmd((obj)data->sublib, lccmd);
      }
      else {
        /* lenz sublib */
      }
    }
    else {
      response = data->sublib->cmd((obj)data->sublib, cmd);
    }
  }
  else {
    response = data->sublib->cmd((obj)data->sublib, cmd);
  }




  return response;
}


/**  */
static void _halt( obj inst, Boolean poweroff ) {
  iOOpenDCCData data = Data(inst);
  data->run = False;
  data->sublib->halt((obj)data->sublib, poweroff);
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iOOpenDCCData data = Data(inst);
  return data->sublib->setListener( (obj)data->sublib , listenerObj, listenerFun );
}


static Boolean _setRawListener(obj inst, obj listenerObj, const digint_rawlistener listenerFun ) {
  return True;
}

static byte* _cmdRaw( obj inst, const byte* cmd ) {
  return NULL;
}

/** bit0=power, bit1=programming, bit2=connection */
static int _state( obj inst ) {
  iOOpenDCCData data = Data(inst);
  return data->sublib->state((obj)data->sublib);
}


/* external shortcut event */
static void _shortcut(obj inst) {
  iOOpenDCCData data = Data( inst );
}


/**  */
static Boolean _supportPT( obj inst ) {
  iOOpenDCCData data = Data(inst);
  return data->sublib->supportPT((obj)data->sublib);
}


static void __swdelayThread( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iOOpenDCC opendcc = (iOOpenDCC)ThreadOp.getParm( th );
  iOOpenDCCData data = Data(opendcc);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "OpenDCC switch delay thread started." );

  while( data->run ) {
    obj post = ThreadOp.getPost( th );
    if( post != NULL ) {
      iONode cmdoff = (iONode)post;
      long delaytime = wSwitch.getdelaytime( cmdoff );
      if( (delaytime + wSwitch.getdelay(cmdoff)) - SystemOp.getMillis() > 0 ) {
        /* sleep the delay in ms */
        ThreadOp.sleep((delaytime + wSwitch.getdelay(cmdoff)) - SystemOp.getMillis());
      }
      /* deactovate the output */
      wSwitch.setactivate(cmdoff, False );
      data->sublib->cmd((obj)data->sublib, cmdoff);
    }
    else {
      ThreadOp.sleep(10);
    }

  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "OpenDCC switch delay thread ended." );
}


/* VERSION: */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  iOOpenDCCData data = Data(inst);
  return vmajor*10000 + vminor*100 + patch;
}


typedef iIDigInt (* LPFNROCGETDIGINT)( const iONode ,const iOTrace );

/**  */
static struct OOpenDCC* _inst( const iONode ini ,const iOTrace trc ) {
  iOOpenDCC __OpenDCC = allocMem( sizeof( struct OOpenDCC ) );
  iOOpenDCCData data = allocMem( sizeof( struct OOpenDCCData ) );
  MemOp.basecpy( __OpenDCC, &OpenDCCOp, 0, sizeof( struct OOpenDCC ), data );

  /* Initialize data->xxx members... */
  TraceOp.set( trc );

  /* clone the ini node for further use and reference */
  data->ini = (iONode)NodeOp.base.clone(ini);

  data->opendccini = wDigInt.getopendcc(data->ini);
  data->iid        = wDigInt.getiid( data->ini );

  if( data->opendccini == NULL )
    data->opendccini = NodeOp.inst( wOpenDCC.name(), ini, ELEMENT_NODE );

  data->sublibname = wOpenDCC.getlib( data->opendccini );
  if( StrOp.equals( wDigInt.lenz, data->sublibname ) ) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set sublib from [lenz] to [xpressnet]" );
    data->sublibname = wDigInt.xpressnet;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "opendcc %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "http://www.opendcc.de/" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "iid    = %s", data->iid );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sublib = %s", data->sublibname );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  /* load sub library */
  {
    iOLib    pLib = NULL;
    LPFNROCGETDIGINT pInitFun = (void *) NULL;
    /* TODO: get the library path! */
    char* libpath = StrOp.fmt( "%s%c%s", wDigInt.getlibpath(data->ini),
        SystemOp.getFileSeparator(), data->sublibname );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "try to load [%s]", libpath );
    pLib = LibOp.inst( libpath );
    StrOp.free( libpath );


    if (pLib != NULL) {
      pInitFun = (LPFNROCGETDIGINT)LibOp.getProc(pLib,"rocGetDigInt");
      if (pInitFun != NULL) {
        if( StrOp.equals( wDigInt.xpressnet, data->sublibname ) ) {
          /* inform the xpressnet library to adde opendcc support */
          wDigInt.setsublib( data->ini, wDigInt.opendcc );
        }
        data->sublib = pInitFun( data->ini, trc );
      }
    }
  }

  if( data->sublib == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "unable to load [%s]", wOpenDCC.getlib( data->opendccini ) );
    freeMem( data );
    freeMem(__OpenDCC);
    return NULL;
  }

  data->run = True;

  data->swdelay = ThreadOp.inst( "swdelay", &__swdelayThread, __OpenDCC );
  ThreadOp.start( data->swdelay );


  instCnt++;
  return __OpenDCC;
}

/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}


/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/opendcc.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
