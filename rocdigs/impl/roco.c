/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

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

#include "rocdigs/impl/roco_impl.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/State.h"
#include "rocutils/public/addr.h"

static int instCnt = 0;
int sensorstate[256];

/** ----- OBase ----- */
static const char* __id( void* inst ) {
  return NULL;
}

static void* __event( void* inst, const void* evt ) {
  return NULL;
}

static void __del( void* inst ) {
  if( inst != NULL ) {
    iORocoData data = Data(inst);
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

/** ----- ORoco ----- */


/**  */
static void _halt( obj inst, Boolean poweroff ) {
  iORocoData data = Data(inst);
  data->run = False;
  return;
}


/**  */
static Boolean _setListener( obj inst ,obj listenerObj ,const digint_listener listenerFun ) {
  iORocoData data = Data(inst);
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

/** bit0=power, bit1=programming, bit2=connection */
/* Status */ //TODO
static int _state( obj inst ) {
  iORocoData data = Data(inst);
  int state = 0;
  return state;
}


/* external shortcut event */
static void _shortcut(obj inst) {
  iORocoData data = Data( inst );
}



/**  */
static Boolean _supportPT( obj inst ) {
  return 1;
}


static void __setLocAddr( int addr, byte* addrbytes ) {
  if( addr < 100 ) {
    addrbytes[0] = 0;
    addrbytes[1] = addr & 0x00FF;
  }
  else {
    addrbytes[0] = ((addr & 0xFF00) >> 8) + 0xC0;
    addrbytes[1] = addr & 0x00FF;
  }
}

static int __getLocAddr( byte* addrbytes ) {
  int addr = addrbytes[1];
  if( (addrbytes[0] & 0xC0) == 0xC0 ) {
    int addrH = (addrbytes[0] & ~0xC0);
    addr = addr + (addrH << 8);
  }
  return addr;
}

static void __handleSensor(iORoco roco, int addr, int value) {
    iORocoData data = Data(roco);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "fb %d = %d", addr, value);

    // inform listener: NodeC
    iONode nodeC = NodeOp.inst( wFeedback.name(), NULL, ELEMENT_NODE );
    wFeedback.setaddr( nodeC, addr );

    if( data->iid != NULL )
      wFeedback.setiid( nodeC, data->iid );

    wFeedback.setstate( nodeC, value?True:False );
    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );

}


static void __handleSwitch(iORoco roco, int addr, int port, int value) {
  iORocoData data = Data(roco);
  int valuew = value;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sw %d %d = %s", addr+1, port, value?"thrown":"straight" );

   {
    iONode nodeC = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );

    wSwitch.setaddr1( nodeC, ( addr+1  ));
    wSwitch.setport1( nodeC, ( port  ));

    if( data->iid != NULL )
      wSwitch.setiid( nodeC, data->iid );

    wSwitch.setstate( nodeC, value?"turnout":"straight" );

    data->listenerFun( data->listenerObj, nodeC, TRCLEVEL_INFO );
  }
}

/* extract the bits */
static void __dec2bin(int *b0, int num) {
  int i, rest;
  for (i =0; i < 8; i++){
    rest = num % 2;
    num = num / 2;
    b0[7-i] = rest;
  }
}

static void __evaluateResponse( iORoco roco, byte* in, int datalen ) {
  iORocoData data = Data(roco);

  int i0 = in[0];
  int i1 = in[1];
  int i2 = in[2];
  int i3 = in[3];

  int b0[8], b1[8], b2[8], b3[8];

  __dec2bin( &b0[0], i0);
  __dec2bin( &b1[0], i1);
  __dec2bin( &b2[0], i2);
  __dec2bin( &b3[0], i3);

  /* switch */
  if ( i0 == 0x00 && i1 == 0x42 && i2 <= 0x80 && (b3[1] == 0 && b3[2] == 0) || (b3[1] == 0 && b3[2] == 1)) {
    int baseadress = i2;
    int k, start;

    if( b3[3] == 0 )
      start = 1;
    else
      start = 3;
    /* two bit status reply 00 = not operated yet, 01 = gate 1 activated, 10 = gate 2 activated, 11 = error */
    for (k = 0; k < 2; k++) {
      if( (b3[7-k*2] + b3[6-k*2]) == 1 ) {       // only handle changed turnouts ignore those unchanged (00) or invalid (11)
        __handleSwitch(roco, baseadress, start+k, b3[7-k*2]);
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Switch status change address %d port %d", baseadress+1, start+k );
      } else {
        if( (b3[7-k*2] + b3[6-k*2]) == 2 )       // turnout reported invalid position
          TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Switch reports invalid position address %d port %d", baseadress+1, start+k );
        else                                     // turnout not yet operated since power on
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Switch not operated yet address %d port %d", baseadress+1, start+k );
      }
    }

  } /* end switch */


  /* sensor */
  if ( in[0] == 0x20 ) {

    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Roco feedback ...");

    int nomodules = ((int) in[1] & 0x0F) - 2;
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Connected FB: %d",
      nomodules);

    int b0[8];
    int k,l,i0;
    int count = 0;

    for (k = 0; k < nomodules; k++) {

      i0 = in[3+k];
      __dec2bin( &b0[0], i0);

      //  process the inputs:
      for (l = 0; l < 8; l++) {
        if( sensorstate[count] != b0[7-l] ) {
          __handleSensor(roco, k*8+l+1, b0[7-l]);
          sensorstate[count] = b0[7-l];
        }
        count++;
      }
    }

  } /* end sensor */

  /* SM response Direct CV mode; cv numbers have -1 offset; cv 1 is returned as 0
     0x44 is answer on cv read, 0x42 on cv write */
  if( (in[0] == 0x44 || in[0] == 0x42) && in[1] == 0xF2 ) {
    int cv = in[2] + 1;
    int value = in[3];
    iONode node = NULL;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "cv %d %s a value of %d", cv, in[0]==0x42?"set to":"has", value );

    node = NodeOp.inst( wProgram.name(), NULL, ELEMENT_NODE );
    wProgram.setcv( node, cv );
    wProgram.setvalue( node, value );
    wProgram.setcmd( node, wProgram.datarsp );
    if( data->iid != NULL )
      wProgram.setiid( node, data->iid );

    if( data->listenerFun != NULL && data->listenerObj != NULL )
      data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

    /* turn off Pt after answer */
    byte* outb = allocMem(256);
    outb[0] = 2;
    outb[1] = 0x40;
    outb[2] = 0xF0;
    ThreadOp.post( data->transactor, (obj)outb );
  } /* end SM response Direct CV mode: */

}

static Boolean __sendRequest( iORoco roco, byte* outin ) {
  iORocoData data = Data(roco);
  int len = outin[0]+1;
  int i = 0;
  Boolean rc = True;
  byte bXor = 0;
  unsigned char out[len];

  // remove length header
  for (i = 0; i < len-1; i++)
     out[i] = (unsigned char) outin[i+1];

  // calculate xor
  for ( i = 1; i < len-1; i++ ) {
    bXor ^= out[i];
  }
  out[len-1] = bXor;

  // NO XOR for ok byte
  if ( out[0] == 0x10 )
    len = 1;

  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "OUT: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X %d",
      out[0], out[1], out[2], out[3], out[4], out[5], out[6], len);

  // write out
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "wait for mutex..." );
  if( MutexOp.trywait( data->mux, 1000 ) ) {
    TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "out buffer" );
    TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)out, len );
    if( !data->dummyio ) {
      rc = SerialOp.write( data->serial, (char*)out, len );
    }
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "packet written" );
    MutexOp.post( data->mux );
  }

  return rc;
}

static void __initializer( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iORoco roco = (iORoco)ThreadOp.getParm( th );
  iORocoData data = Data(roco);
  int i;

  for ( i= 0; i < 256; i++)
    sensorstate[i] = 0;


  // three times the confirmation
  byte* outa = allocMem(256);
  outa[0] = 1;
  outa[1] = 0x10;
  ThreadOp.post( data->transactor, (obj)outa );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** send confirmation. until response or 6 times");

  // put off programming track
  byte* outa0 = allocMem(256);
  outa0[0] = 2;
  outa0[1] = 0x40;
  outa0[2] = 0xF0;
  ThreadOp.post( data->transactor, (obj)outa0 );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** put off programming track..." );


  // 00 F3 0A 00 00 F9
  byte* outa1 = allocMem(256);
  outa1[0] = 5;
  outa1[1] = 0x00;
  outa1[2] = 0xF3;
  outa1[3] = 0x0A;
  outa1[4] = 0x00;
  outa1[5] = 0x00;
  ThreadOp.post( data->transactor, (obj)outa1 );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** sending: 00 F3 0A 00 00 F9 ..." );


  // set sensor repeat at rate 1
  byte* outb = allocMem(256);
  outb[0] = 3;
  outb[1] = 0x21;
  outb[2] = 0xf1;
  outb[3] = data->readfb ? 0x01:0x00;
  ThreadOp.post( data->transactor, (obj)outb );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*** set sensor repeat at rate %d", outb[3] );

  // Infobyte FB
  byte* oute = allocMem(256);
  oute[0] = 4;
  oute[1] = 0x23;
  oute[2] = 0xF2;
  oute[3] = 0x00;
  oute[4] = 0x00;
  ThreadOp.post( data->transactor, (obj)oute );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** Setting FB info byte for grp. 0 ..." );

  // Infobyte FB
  byte* outf = allocMem(256);
  outf[0] = 4;
  outf[1] = 0x23;
  outf[2] = 0xF2;
  outf[3] = 0x01;
  outf[4] = 0x10;
  ThreadOp.post( data->transactor, (obj)outf );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** Setting FB info byte for grp. 1 ..." );

  // Global Power ON
  byte* outc = allocMem(256);
  outc[0] = 3;
  outc[1] = 0x00;
  outc[2] = 0x21;
  outc[3] = 0x81;
  ThreadOp.post( data->transactor, (obj)outc );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** Global Power ON ..." );


  //00 F0 F0, gives response 00 02 16 80 90
  byte* outc1 = allocMem(256);
  outc1[0] = 2;
  outc1[1] = 0x00;
  outc1[2] = 0xF0;
  ThreadOp.post( data->transactor, (obj)outc1 );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** sending: 00 F0 F0..." );

  //00 21
  byte* outc2 = allocMem(256);
  outc2[0] = 3;
  outc2[1] = 0x00;
  outc2[2] = 0x21;
  outc2[3] = 0x21;
  ThreadOp.post( data->transactor, (obj)outc2 );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** sending: 00 21 ..." );

  //00 F3 0B 00 00 F8
  byte* outc3 = allocMem(256);
  outc3[0] = 5;
  outc3[1] = 0x00;
  outc3[2] = 0xF3;
  outc3[3] = 0x0B;
  outc3[4] = 0x00;
  outc3[5] = 0x00;
  ThreadOp.post( data->transactor, (obj)outc3 );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** sending: 00 F3 0B 00 00 ..." );

  /*00 E3 00 00 03 E0 not found in rocomotion sniffer trace
  byte* outc4 = allocMem(256);
  outc4[0] = 5;
  outc4[1] = 0x00;
  outc4[2] = 0xE3;
  outc4[3] = 0x0B;
  outc4[4] = 0x00;
  outc4[5] = 0x00;
  ThreadOp.post( data->transactor, (obj)outc4 );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** sending: 00 E3 00 00 03 ..." );
  */

  // Setting no of fb in group 0 to %X
  byte* outd = allocMem(256);
  outd[0] = 4;
  outd[1] = 0x22;
  outd[2] = 0xF2;
  outd[3] = 0x00;
  outd[4] = data->fbmod;
  ThreadOp.post( data->transactor, (obj)outd );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "*** Setting no of fb in group 0 to %X ...", outd[4] );

   // Setting no of fb in group 1 to %X //TODO
  byte* outg = allocMem(256);
  outg[0] = 4;
  outg[1] = 0x22;
  outg[2] = 0xF2;
  outg[3] = 0x01;
  outg[4] = 0x00;
  ThreadOp.post( data->transactor, (obj)outg );
  TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "*** Setting no of fb in group 1 to 0 ..." );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Roco initalized." );
}


static void __transactor( void* threadinst ) {
  iOThread th = (iOThread)threadinst;
  iORoco roco = (iORoco)ThreadOp.getParm(th);
  iORocoData data = Data(roco);

  ThreadOp.setDescription( th, "Transactor for Roco" );
  TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Transactor started." );
  ThreadOp.setHigh( th );

  obj post = NULL;
  Boolean responceRecieved = True;
  Boolean ok = False;

  byte out[256];
  byte* outtmp;

  int datalen = 0;
  byte bXor = 0;
  byte in[256];

  int i = 0;

  int numtries = 5;
  do {
    if (responceRecieved) {

      post = ThreadOp.getPost( th );
      numtries = data->dummyio ? 0:5;
      if (post != NULL) {
        outtmp = (byte*) post;
        for (i = 0; i < outtmp[0]+1; i++)
          out[i] = (byte) outtmp[i];
        freeMem( post);
      }
      if (post != NULL) {

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "COMMAND FIRST: 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X",
          out[0], out[1], out[2], out[3], out[4], out[5], out[6]);

        responceRecieved = !__sendRequest( roco, out );
        }
    } else {
      if( post != NULL && numtries > 0) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Command again %d ...", numtries );
        __sendRequest( roco, out );
        numtries--;
      } else {
        responceRecieved = True;
      }
    }

    // Give cs time to answer
    ThreadOp.sleep( 25 );

    if ( !data->dummyio && SerialOp.available(data->serial) ) {

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "wait for mutex..." );
      if( MutexOp.trywait( data->mux, 1000 ) ) {
        if( !SerialOp.read( data->serial, (char*) in, 2 ) ) {
          MutexOp.post( data->mux );
          continue;
        }
        datalen = (in[1] & 0x0f) + 2; // add 1 for the xor byte and 1 for the info
        ok = SerialOp.read( data->serial, (char*)in+2, datalen-1 );
        MutexOp.post( data->mux );

        if( !ok )
          continue;

        TraceOp.trc( name, TRCLEVEL_BYTE, __LINE__, 9999, "in buffer" );
        TraceOp.dump( NULL, TRCLEVEL_BYTE, (char*)in, datalen+2 );

        bXor = 0;
        for( i = 1; i < datalen; i++ ) { // add one for the header byte
          bXor ^= in[i];
        }

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "IN:  0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X %d",
          in[0], in[1], in[2], in[3], in[4], in[5], in[6], datalen+1);

        if( bXor != in[datalen] && !( in[0] == 0x00 && in[1] == 0x02 && in[2] == 0x16 && in[3] == 0x80 && in[4] == 0x90 )) {
          // message 00 02 16 80 90 is response to 00 F0 F0 initialisation and comes without xor
           TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "xor bytes are not equal!" );
             continue;
        }

        // allways send confirm ...
        byte confirm = 0x10;
        SerialOp.write( data->serial, (char*)&confirm, 1 );

        // handshake
        if( in[0] == 0x00 && in[1] == 0x01 && in[2] == 0x00) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Roco command received without error");
          if( !data->dummyio )
            responceRecieved = True;
        }

        // undocumented response to undocumented 00 F0 F0 initialisation command
        else if( in[0] == 0x00 && in[1] == 0x02 && in[2] == 0x16 && in[3] == 0x80 && in[4] == 0x90 ) {
          TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "00 F0 F0 response");
          responceRecieved = True;
        }

        /* INCOMING COMMANDS */
         // Track Power OFF
        else if( in[0] == 0x00 && in[1] == 0x61 && in[2] == 0x00) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Track power OFF");
          data->power = False;

          iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
          if( data->iid != NULL )
            wState.setiid( node, data->iid );
          wState.setpower( node, False );
          wState.settrackbus( node, False );
          wState.setsensorbus( node, False );
          wState.setaccessorybus( node, False );

          if( data->listenerFun != NULL && data->listenerObj != NULL )
            data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

          responceRecieved = True;
        }
        // Normal operation resumed
        else if( in[0] == 0x00 && in[1] == 0x61 && in[2] == 0x01) {
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Normal operation resumed.");
          data->power = True;

          iONode node = NodeOp.inst( wState.name(), NULL, ELEMENT_NODE );
          if( data->iid != NULL )
            wState.setiid( node, data->iid );
          wState.setpower( node, True );
          wState.settrackbus( node, True );
          wState.setsensorbus( node, True );
          wState.setaccessorybus( node, True );

          if( data->listenerFun != NULL && data->listenerObj != NULL )
            data->listenerFun( data->listenerObj, node, TRCLEVEL_INFO );

          responceRecieved = True;
        }
        // CS busy
        else if ( in[0] == 0x00 && in[1] == 0x61 && in[2] == 0x81){
           TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "cs busy ... trying again");
        }
        // XOR error
        else if ( in[0] == 0x00 && in[1] == 0x01 && in[2] == 0x01){
           TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "XOR error");
        }
        // Command not known
        else if ( in[0] == 0x00 && in[1] == 0x61 && in[2] == 0x82){
           TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Command not known.");
           responceRecieved = True;
        }
        // Shortcut
        else if ( in[0] == 0x00 && in[1] == 0x61 && in[2] == 0x12){
           TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Shortcut!");
           responceRecieved = True;
        }
        // No data
        else if ( in[0] == 0x00 && in[1] == 0x61 && in[2] == 0x13){
           TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "No Data");
           responceRecieved = True;
        }

        // Answer cv read/write; cv 1 is returned with 0
        else if ( (in[0] == 0x44 || in[0] == 0x42)&& in[1] == 0xF2 ){
           TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "CV %d = %d", in[2] + 1, in[3]);
           responceRecieved = True;
        }

        // Answer SW-Version
        else if ( in[0] == 0x00 && in[1] == 0x02 ){
           int major = (int) (in[2] & 0xF0 )/16;
           int minor = (int) in[2] & 0x0F;
           TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Roco interface version: %d.%d compiler %d",
           major , minor, in[3] );
           responceRecieved = True;
        }

        // anything will go to rocview ...
        __evaluateResponse( roco, in, datalen );

      }
    }

    // Give up timeslize:
    ThreadOp.sleep( 1 );
  } while( data->run );

}


static void __translate( iORoco roco, iONode node ) {
  iORocoData data = Data(roco);


  /* Switch command. */
  if( StrOp.equals( NodeOp.getName( node ), wSwitch.name() ) ) {

    int addr = wSwitch.getaddr1( node );
    int port = wSwitch.getport1( node );
    int gate = wSwitch.getgate1( node );

    if( port == 0 )
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      AddrOp.fromPADA( port, &addr, &port );

    if( port > 0 ) port--;
    if( addr > 0 ) addr--;

    int gate1  = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 0x00:0x01; //0 = use gate 1, 1 = use gate 2
    int action = StrOp.equals( wSwitch.getcmd( node ), wSwitch.turnout ) ? 0x00:0x08; //0 = gate off, 8 = gate on

    // make message:
    byte* outb = allocMem(256);
    outb[0] = 4;
    outb[1] = 0x00;
    outb[2] = 0x52;
    outb[3] = addr;

    if( wSwitch.issinglegate( node ) ) {
      //when single gate turn gate on (cmd straight) or off (cmd turnout)
      outb[4] = 0x90 | action | (port << 1) | gate; //first rocomotion trace shows roco uses 0x9 as high nibble against 0x8 as official xpressnet
      ThreadOp.post( data->transactor, (obj)outb );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "turnout gate %d %d %d %s",
        addr+1, port+1, gate, action==0?"off":"on" );
    } else {
      //otherwise turn gate 1 (cmd turnout) or gate 2 (cmd straight) on and 100 ms later off again
      outb[4] = 0x90 | 0x08 | (port << 1) | gate1;  //turn gate on
      ThreadOp.post( data->transactor, (obj)outb );

      ThreadOp.sleep(100);

      byte* outbb = allocMem(256);
      outbb[0] = 4;
      outbb[1] = 0x00;
      outbb[2] = 0x52;
      outbb[3] = addr;
      outbb[4] = 0x90 | 0x00 | (port << 1) | gate1;  //turn gate off
      ThreadOp.post( data->transactor, (obj)outbb );

      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "turnout %d %d %s",
          addr+1, port+1, wSwitch.getcmd( node ) );
    }
  }
  /* Output command. */
  else if( StrOp.equals( NodeOp.getName( node ), wOutput.name() ) ) {

    int addr   = wOutput.getaddr( node );
    int port   = wOutput.getport( node );
    int gate   = wOutput.getgate( node );

    if( port == 0 )
      AddrOp.fromFADA( addr, &addr, &port, &gate );
    else if( addr == 0 && port > 0 )
      AddrOp.fromPADA( port, &addr, &port );

    if( port > 0 ) port--;
    if( addr > 0 ) addr--;

    int action = StrOp.equals( wOutput.getcmd( node ), wOutput.on ) ? 0x08:0x00;

    // make message:
    byte* outb = allocMem(256);
    outb[0] = 4;
    outb[1] = 0x00;
    outb[2] = 0x52;
    outb[3] = addr;
    outb[4] = 0x90 | action | (port << 1) | gate;  //same nibble story as with the turnouts
    ThreadOp.post( data->transactor, (obj)outb );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "output %d %d %d %s",
        addr+1, port+1, gate, wOutput.getcmd( node ) );
  }
  /* Signal command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSignal.name() ) ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,
        "Signal commands are no longer supported at this level." );
  }


  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() )  && StrOp.equals( wLoc.shortid, wLoc.getcmd(node) ) ) {
    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "send short ID to the throttle" );
    /*
      Command to send:
      Byte1 Info byte
      Byte2 Header E+number of bytes
      Byte3 F1
      Byte4 Loc Address AH
      Byte5 Loc Address AL
      Byte6 MultiMaus listnumber, offset 00
      Byte7 Total addresses to send
      Byte8-xx loc Name 1 to 5 bytes
      X-OR X-0r
      Response 0x61 0x81 0xE0, CS busy
                0x61 0x82 0xE3, Command unknown
      On CS Busy, resend the command
      On Command unknown, ignore because command is not for the CS but for the
      MultiMaus.
      Example of command(s) so send:
      00 EA F1 00 0C 05 07 44 42 32 31 32 22 ..êñ....DB212"
    */
    const char* shortID = wLoc.getshortid(node);
    byte* outa = allocMem(32);
    outa[0]  = 12;
    outa[1]  = 0x00;
    outa[2]  = 0xEA;
    outa[3]  = 0xF1;
    outa[4]  = wLoc.getaddr(node)/256;
    outa[5]  = wLoc.getaddr(node)%256;
    outa[6]  = wLoc.getthrottlenr(node);
    outa[7]  = 0x01;
    outa[8]  = shortID[0];
    outa[9]  = shortID[1];
    outa[10] = shortID[2];
    outa[11] = shortID[3];
    outa[12] = shortID[4];
    ThreadOp.post( data->transactor, (obj)outa );
  }

  /* Loc command. */
  else if( StrOp.equals( NodeOp.getName( node ), wLoc.name() ) ) {
    int   addr = wLoc.getaddr( node );
    int  speed = 0;
    int  lenzspeed = 0;
    Boolean fn = wLoc.isfn( node );
    int    dir = wLoc.isdir( node );
    int  spcnt = wLoc.getspcnt( node );

    int reqid = 0x12; /* default 28 speed steps */
    switch( spcnt ) {
      case 27:
        reqid = 0x11;
        break;
      case 14:
        reqid = 0x10;
        break;
      case 127:
      case 128:
        reqid = 0x13;
        spcnt = 127;
        break;
      default:
        reqid = 0x12;
        spcnt = 28;
        break;
    }
    /*
      General Lenz speed values

      14
      Step 0 Stop
      Step 1 E-Stop
      Step 2-15 Train in motion

      27
      Step 0 Stop
      Step 1 Not used
      Step 2 E-Stop
      Step 3 Not used
      Step 4-30 Train in motion
      Step 31 Not used

      28
      Step 0 Stop
      Step 1 Not used
      Step 2 E-Stop
      Step 3 Not used
      Step 4-31 Train in motion

      128
      Step 0 Stop
      Step 1 E-Stop
      Step 2-127 Train in motion
     */

    if( wLoc.getV( node ) != -1 ) {
      if( StrOp.equals( wLoc.getV_mode( node ), wLoc.V_mode_percent ) )
        speed = (wLoc.getV( node ) * spcnt) / 100;
      else if( wLoc.getV_max( node ) > 0 )
        speed = (wLoc.getV( node ) * spcnt) / wLoc.getV_max( node );
      TraceOp.trc( name, TRCLEVEL_USER1, __LINE__, 9999, "speed=%d", speed );
    }

    lenzspeed = speed;

    /* Remove e-stop in 14 and 127 speed step mode */
    if ( spcnt == 14 || spcnt == 127 ){
      if ( lenzspeed > 0 )
        lenzspeed = lenzspeed + 1;
    }

    /* Adjust bit positions for lenz command stations */
    if (( spcnt == 27 ) || ( spcnt == 28 )){

      /* Remove 2 unused speed step and e-stop speed step */
      if ( lenzspeed > 0 )
        lenzspeed = lenzspeed + 3;

      /* Transfer LSB to front of bit 0-3 */
      if ( lenzspeed & 0x01 )
        lenzspeed = lenzspeed | 0x20;

      /* Move all bit in right position */
      lenzspeed = lenzspeed >> 1;
    }

    byte* outb = allocMem(256);
    outb[0] = 6;
    outb[1] = 0x00;
    outb[2] = 0xE4; // rocomotion traces sometimes show E5 against E4 with additional 7th byte contianing 02 or 03, but working with E4
    outb[3] = reqid;
    __setLocAddr( addr, outb+4 );
    outb[6] = dir ? 0x80:0x00;
    outb[6] |= lenzspeed;
    ThreadOp.post( data->transactor, (obj)outb );

    TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "loc %d velocity=%d direction=%s", addr, speed, (dir?"fwd":"rev") );
  }
  /* Function command. */
  else if( StrOp.equals( NodeOp.getName( node ), wFunCmd.name() ) ) {
    int   addr = wFunCmd.getaddr( node );
    int   fncnt = wLoc.getfncnt( node ); //Lokmaus does not support 12 functions, Multimaus does, check number of functions of loco and decide wether or not to send function groups 2 and 3 accordingly
    Boolean f0 = wFunCmd.isf0( node );
    Boolean f1 = wFunCmd.isf1( node );
    Boolean f2 = wFunCmd.isf2( node );
    Boolean f3 = wFunCmd.isf3( node );
    Boolean f4 = wFunCmd.isf4( node );
    Boolean f5 = wFunCmd.isf5( node );
    Boolean f6 = wFunCmd.isf6( node );
    Boolean f7 = wFunCmd.isf7( node );
    Boolean f8 = wFunCmd.isf8( node );
    Boolean f9  = wFunCmd.isf9 ( node );
    Boolean f10 = wFunCmd.isf10( node );
    Boolean f11 = wFunCmd.isf11( node );
    Boolean f12 = wFunCmd.isf12( node );
    byte functions1 = (f1?0x01:0) + (f2?0x02:0) + (f3?0x04:0) + (f4?0x08:0) + (f0?0x10:0);
    byte functions2 = (f5?0x01:0) + (f6?0x02:0) + (f7?0x04:0) + (f8?0x08:0);
    byte functions3 = (f9?0x01:0) + (f10?0x02:0) + (f11?0x04:0) + (f12?0x08:0);

    if ( fncnt <=4 )
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "function %d light=%s f1=%s f2=%s f3=%s f4=%s",
        addr, (f0?"ON":"OFF"), (f1?"ON":"OFF"), (f2?"ON":"OFF"), (f3?"ON":"OFF"), (f4?"ON":"OFF") );
    else if ( fncnt <= 8 )
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "function %d light=%s f1=%s f2=%s f3=%s f4=%s f5=%s f6=%s f7=%s f8=%s",
        addr, (f0?"ON":"OFF"), (f1?"ON":"OFF"), (f2?"ON":"OFF"), (f3?"ON":"OFF"), (f4?"ON":"OFF"),
        (f5?"ON":"OFF"), (f6?"ON":"OFF"), (f7?"ON":"OFF"), (f8?"ON":"OFF") );
    else
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999,
        "function %d light=%s f1=%s f2=%s f3=%s f4=%s f5=%s f6=%s f7=%s f8=%s f9=%s f10=%s f11=%s f12=%s",
        addr, (f0?"ON":"OFF"), (f1?"ON":"OFF"), (f2?"ON":"OFF"), (f3?"ON":"OFF"), (f4?"ON":"OFF"),
        (f5?"ON":"OFF"), (f6?"ON":"OFF"), (f7?"ON":"OFF"), (f8?"ON":"OFF"),
        (f9?"ON":"OFF"), (f10?"ON":"OFF"), (f11?"ON":"OFF"), (f12?"ON":"OFF") );

    byte* outa = allocMem(256);
    outa[0] = 6;
    outa[1] = 0x00;
    outa[2] = 0xE4;
    outa[3] = 0x20;
    __setLocAddr( addr, outa+4 );
    outa[6] = functions1;
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "function group 1" );
    ThreadOp.post( data->transactor, (obj)outa );

    if ( fncnt >= 5 ) {
      byte* outb = allocMem(256);
      outb[0] = 6;
      outb[1] = 0x00;
      outb[2] = 0xE4;
      outb[3] = 0x21;
      __setLocAddr( addr, outb+4 );
      outb[6] = functions2;
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "function group 2" );
      ThreadOp.post( data->transactor, (obj)outb );
    }

    if ( fncnt >= 9 ) {
      byte* outc = allocMem(256);
      outc[0] = 6;
      outc[1] = 0x00;
      outc[2] = 0xE4;
      outc[3] = 0x22;
      __setLocAddr( addr, outc+4 );
      outc[6] = functions3;
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "function group 3" );
      ThreadOp.post( data->transactor, (obj)outc );
    }

    /* save the function1 byte to use for setting the lights function... */
    data->lcfn[addr] = functions1;

  }
  /* System command. */
  else if( StrOp.equals( NodeOp.getName( node ), wSysCmd.name() ) ) {
    const char* cmd = wSysCmd.getcmd( node );

    byte* outa = allocMem(256);
    if( StrOp.equals( cmd, wSysCmd.stop ) ) {
      outa[0] = 3;
      outa[1] = 0x00;
      outa[2] = 0x21;
      outa[3] = 0x80;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power OFF" );
      ThreadOp.post( data->transactor, (obj)outa );
    }
    else if( StrOp.equals( cmd, wSysCmd.ebreak ) ) {
      outa[0] = 3;
      outa[1] = 0x00;
      outa[2] = 0x80;
      outa[3] = 0x80;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Emergency break" );
      ThreadOp.post( data->transactor, (obj)outa );
    }
    else if( StrOp.equals( cmd, wSysCmd.go ) ) {
      outa[0] = 3;
      outa[1] = 0x00;
      outa[2] = 0x21;
      outa[3] = 0x81;
      TraceOp.trc( name, TRCLEVEL_MONITOR, __LINE__, 9999, "Power ON" );
      ThreadOp.post( data->transactor, (obj)outa );
    }


  }

    /* Program command. */
  else if( StrOp.equals( NodeOp.getName( node ), wProgram.name() ) ) {

    if( wProgram.getcmd( node ) == wProgram.get ) {
      int cv = wProgram.getcv( node );
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "get CV%d...", cv );

      if (cv > 0) cv--;

      byte* outa = allocMem(256);
      outa[0] = 6;
      outa[1] = 0x41;
      outa[2] = 0xF4;
      outa[3] = 0x78;
      outa[4] = cv & 0xFF;
      outa[5] = 0xE8;
      outa[6] = outa[3] ^ outa[4] ^ outa[5];
      ThreadOp.post( data->transactor, (obj)outa );

    }
    else if( wProgram.getcmd( node ) == wProgram.set ) {
      int cv = wProgram.getcv( node );
      int value = wProgram.getvalue( node );
      int decaddr = wProgram.getdecaddr( node );

      // POM ?
      if( wProgram.ispom(node) ) {
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "POM: set CV%d of loc %d to %d...", cv, decaddr, value );

        if (cv > 0) cv--;

        byte* outb = allocMem(256);
        outb[0] = 8;
        outb[1] = 0x00;
        outb[2] = 0xE6;
        outb[3] = 0x30;
        __setLocAddr( decaddr, outb+4 );
        outb[6] = ((cv & 0xFF00) >> 8) + 0xEC;
        outb[7] = cv & 0x00FF;
        outb[8] = value & 0xFF;

        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "POM: 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X",
          outb[0], outb[1], outb[2], outb[3], outb[4], outb[5], outb[6]);

        if ( cv != 0 )
          ThreadOp.post( data->transactor, (obj)outb );
        else
          TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "POM does not allow writing of adress!");

      } else {

        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set CV%d to %d...", cv, value );

        if (cv > 0) cv--;

        byte* outa = allocMem(256);
        outa[0] = 6;
        outa[1] = 0x40;
        outa[2] = 0xF4;
        outa[3] = 0x7C;
        outa[4] = cv & 0xFF;
        outa[5] = value & 0xFF;
        outa[6] = outa[3] ^ outa[4] ^ outa[5];
        ThreadOp.post( data->transactor, (obj)outa );

      }

    }
    else if(  wProgram.getcmd( node ) == wProgram.pton ) {
       // CS will go ton Pt on on first programming request
    }  // PT off, send: All ON"
    else if( wProgram.getcmd( node ) == wProgram.ptoff ) {
      byte* outb = allocMem(256);
      outb[0] = 2;
      outb[1] = 0x40;
      outb[2] = 0xF0;
      ThreadOp.post( data->transactor, (obj)outb );
    }
  }

}

static iONode _cmd( obj inst ,const iONode nodeA ) {
  iORocoData data = Data(inst);

  if( nodeA != NULL ) {
    __translate( (iORoco)inst, nodeA );

    /* Cleanup Node1 */
    nodeA->base.del(nodeA);
  }

  /* return Node2 */
  return NULL;
}

/** vmajor*1000 + vminor*100 + patch */
static int vmajor = 2;
static int vminor = 0;
static int patch  = 0;
static int _version( obj inst ) {
  return vmajor*10000 + vminor*100 + patch;
}


/**  */
static struct ORoco* _inst( const iONode ini ,const iOTrace trc ) {
  iORoco __Roco = allocMem( sizeof( struct ORoco ) );
  iORocoData data = allocMem( sizeof( struct ORocoData ) );
  MemOp.basecpy( __Roco, &RocoOp, 0, sizeof( struct ORoco ), data );

  TraceOp.set( trc );

  /* Initialize data->xxx members... */
  data->ini    = ini;
  data->iid    = StrOp.dup( wDigInt.getiid( ini ) );
  data->mux    = MutexOp.inst( StrOp.fmt( "serialMux%08X", data ), True );
  data->fbmod  = wDigInt.getfbmod( ini );
  data->readfb = wDigInt.isreadfb( ini );
  data->dummyio= wDigInt.isdummyio( ini );

  data->serial = SerialOp.inst( wDigInt.getdevice( ini ) );

  MemOp.set( data->fbState, 0, sizeof( data->fbState ) );

  SerialOp.setFlow( data->serial, none );
  SerialOp.setLine( data->serial, wDigInt.getbps( ini ), 8, 1, none, wDigInt.isrtsdisabled( ini ) );
  SerialOp.setTimeout( data->serial, wDigInt.gettimeout( ini ), wDigInt.gettimeout( ini ) );


  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "roco %d.%d.%d", vmajor, vminor, patch );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----------------------------------------" );

  if( !SerialOp.open( data->serial ) && !data->dummyio ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Could not init Roco port!" );
  }
  else {
    data->run = True;

    data->transactor = ThreadOp.inst( "transactor", &__transactor, __Roco );
    ThreadOp.start( data->transactor );

    data->initializer = ThreadOp.inst( "initializer", &__initializer, __Roco );
    ThreadOp.start( data->initializer );
  }

  instCnt++;
  return __Roco;
}

/* Support for dynamic Loading */
iIDigInt rocGetDigInt( const iONode ini ,const iOTrace trc )
{
  return (iIDigInt)_inst(ini,trc);
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocdigs/impl/roco.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
