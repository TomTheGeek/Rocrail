/*
 Rocrail - Model Railroad Software
 
 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>
 Some parts are copied from the DDL project of Torsten Vogt: http://www.vogt-it.com/OpenSource/DDL
 
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
#include "rocdigs/impl/ddx_impl.h"
#include "s88.h"

#include "rocs/public/mem.h"
#include "rocs/public/trace.h"
#include "rocs/public/system.h"
#include "rocs/public/thread.h"

// signals on the S88-Bus
#define S88_QUIET 0x00 // all signals low
#define S88_RESET 0x04 // reset signal high
#define S88_LOAD  0x02 // load signal high
#define S88_CLOCK 0x01 // clock signal high
#define S88_DATA1 0x40 // mask for data form S88 bus 1 (ACK)
#define S88_DATA2 0x80 // mask for data from S88 bus 2 (BUSY) !inverted
#define S88_DATA3 0x20 // mask for data from S88 bus 3 (PEND)
#define S88_DATA4 0x10 // mask for data from S88 bus 4 (SEL)

// possible io-addresses for the parallel port
static const unsigned long LPT_BASE[] = {
                                          0x3BC, 0x378, 0x278
                                        };
// number of possible parallel ports
static const unsigned int LPT_NUM = 3;
// values of the bits in a byte
static const char BIT_VALUES[] = {
                                   0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80
                                 };

void rocrail_ddxStateChanged(obj inst);
void rocrail_ddxFbListener( obj inst, int addr, int state );

void thr_dos88polling(void *v);

// Output a Signal to the Bus
static void S88_WRITE(int port, int s88clock_scale, int b ) {
  int i;
  for( i = 0; i < s88clock_scale; i++ )
    SystemOp.writePort( port, b );
}



/****************************************************************
* function s88init                                              *
*                                                               *
* purpose: test the parralel port for the s88bus and initializes*
*          the bus. The portadress must be one of LPT_BASE, the *
*          port must be accessable through ioperm and there must*
*          be an real device at the adress.                     *
*                                                               *
* in:      ---                                                  *
* out:     return value: 1 if testing and initializing was      *
*                        successfull, otherwise 0               *
*                                                               *
* remarks: tested MW, 20.11.2000                                *
*                                                               *
****************************************************************/
int s88init(obj inst) {
  iODDXData data = Data((iODDX)inst);
  unsigned int i; // loop counter
  int isin = 0; // reminder for checking

  // is the port disabled from user, everything is fine
  if (!data->s88port) {
    TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "s88 port is disabled" );
    return 0;
  }
  // test, whether S88DEV is a valid io-address for a parallel device
  for (i=0;i<LPT_NUM;i++)
    isin = isin || (data->s88port == LPT_BASE[i]);
  if (isin) {
    // test, whether we can access the port
    if ( SystemOp.accessPort(data->s88port,3) ) {
      // test, whether there is a real device on the S88DEV-port by writing and
      // reading data to the port. If the written data is returned, a real port
      // is there
      SystemOp.writePort(data->s88port, 0x00);
      isin = (SystemOp.readPort(data->s88port)==0);
      SystemOp.writePort(data->s88port, 0xFF);
      isin = (SystemOp.readPort(data->s88port)==0xFF) && isin;
      if (isin) {
        unsigned char reg = 0;
        // initialize the S88 by doing a reset
        // for ELEKTOR-Modul the reset must be on the load line
        S88_WRITE(data->s88port,data->s88clockscale,S88_QUIET);
        S88_WRITE(data->s88port,data->s88clockscale,(S88_RESET & S88_LOAD));
        S88_WRITE(data->s88port,data->s88clockscale,S88_QUIET);

        // Give power to hardware, set nInitialize high and set nSel and nStrobe low, as they are inverted, 
        reg = SystemOp.readPort( data->s88port+2 );
        SystemOp.writePort( data->s88port+2, ((reg & 0xF6) | 0x04) );

      } else {
        TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "There is no port for s88 at 0x%X.",data->s88port );
        SystemOp.releasePort(data->s88port,3); // stopping access to port address
        return 0;
      }
    } else {
      TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "Access to port 0x%X denied.",data->s88port );
      return 0;
    }
  } else {
    TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "0x%X is not valid port adress for s88 device.",data->s88port );
    return 0;
  }

  TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "s88 port successfully initialized at 0x%X.",data->s88port );
  /* reset the PT relais for getting a known status */
  setPT(inst, 0);

  start_polling_s88(inst);
  return 1;
}


static void __setPTintern(obj inst, int on) {
  iODDXData data = Data(inst);
  /* AUTO-FEED = base addr + 2 bit 1*/
  TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "set PT %s", on?"ON":"OFF" );
  if ( SystemOp.accessPort(data->s88port,3) ) {

    /* preserve other control bits: */
    unsigned char reg = SystemOp.readPort( data->s88port+2 );

    if(on)
      reg = reg | 0x02;
    else
      reg = reg & (~0x02);

    SystemOp.writePort( data->s88port+2, reg );
  } else {
    TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "not possible to access port 0x%04X", data->s88port );
  }
}

void setPT(obj inst, int on) {
  iODDXData data = Data(inst);
  if( !on )
    __setPTintern(inst, on);

  TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "%s s88 polling (PT mode)", on?"STOP":"START" );
  data->s88flag = on?0:1;

  if( on ) {
    /* Wait for thread cycle: */
    ThreadOp.sleep(100);
    __setPTintern(inst, on);
  }
}

int isPT(obj inst) {
  iODDXData data = Data(inst);
  if( data->s88flag )
    TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "DDX is not in PT mode! PROGRAMMING NOT POSSIBLE!" );
  return !data->s88flag;
}

void start_polling_s88(obj inst) {
  iODDXData data = Data(inst);
  int  error,i;
  char retbuf[40];
  int thread_created = 0;

  /* start the polling thread if it is not already running: */

  if( data->s88pollt == NULL ) {
    data->s88flag = 1;
    data->s88pollt = ThreadOp.inst( "s88poll", &thr_dos88polling, inst );
    ThreadOp.start( data->s88pollt );

    thread_created = 1;
    rocrail_ddxStateChanged(inst);
  }
}

void thr_dos88polling(void* threadinst) {
  iOThread th = (iOThread)threadinst;
  iODDX inst = (iODDX)ThreadOp.getParm( th );
  iODDXData data = Data(inst);

  char delta, retbuf[40];
  unsigned long sleepmsec = data->s88refresh;
  int bus;
  int ports[4] = { data->s88b0modcnt,data->s88b1modcnt,data->s88b2modcnt,data->s88b3modcnt };
  int maxports = S88_MAXPORTSB;
  int i;
//  typedef char s88array[S88_MAXPORTSB*S88_MAXBUSSES];
  char* s88data = allocMem( S88_MAXPORTSB*S88_MAXBUSSES * sizeof( char ));
  char* s88old  = allocMem( S88_MAXPORTSB*S88_MAXBUSSES * sizeof( char ));


  for( bus = 0; bus < 4; bus++ )
    TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "s88 polling %d modules on bus %d", ports[bus], bus );

  if( data->s88buses > 0 ) {
    maxports = 0;
    for( i = 0; i < data->s88buses; i++ ) {
      if( ports[i] > maxports )
        maxports = ports[i];
    }
  }

  // clear the s88old array
  MemOp.set( s88old , 0, S88_MAXPORTSB*S88_MAXBUSSES * sizeof( char ) );
  SystemOp.accessPort(data->s88port,3);

  while(!ThreadOp.isQuit(th)) {
    ThreadOp.sleep(sleepmsec);

    if( !data->s88flag ) {
      /* in PT mode */
      continue;
    }
    /* initialize the s88data array */
    MemOp.set( s88data, 0, S88_MAXPORTSB*S88_MAXBUSSES * sizeof( char ) );

    /* if port is disabled do nothing */
    if (data->s88port) {
      int port;
      /* load the bus */
      S88_WRITE(data->s88port,data->s88clockscale,S88_LOAD);
      S88_WRITE(data->s88port,data->s88clockscale,(S88_LOAD|S88_CLOCK));
      S88_WRITE(data->s88port,data->s88clockscale,S88_QUIET);
      S88_WRITE(data->s88port,data->s88clockscale,S88_RESET);
      S88_WRITE(data->s88port,data->s88clockscale,S88_QUIET);
      // reading the data
      for( port = 0; port < maxports; port++) {
        int bit;
        for( bit = 0; bit < 8; bit++ ) {
          // reading from port
          int inbyte = SystemOp.readPort(data->s88port+1);
          // interpreting the four busses
          if (  inbyte&S88_DATA1)
            s88data[port                ] += BIT_VALUES[bit];
          if (!(inbyte&S88_DATA2))
            s88data[port+  S88_MAXPORTSB] += BIT_VALUES[bit];
          if (  inbyte&S88_DATA3)
            s88data[port+2*S88_MAXPORTSB] += BIT_VALUES[bit];
          if (  inbyte&S88_DATA4)
            s88data[port+3*S88_MAXPORTSB] += BIT_VALUES[bit];
          // getting the next data
          S88_WRITE(data->s88port,data->s88clockscale,S88_CLOCK);
          S88_WRITE(data->s88port,data->s88clockscale,S88_QUIET);
        }
      }
    } else
      continue; // no busses to scan, quit!

    for( bus = 0; bus < data->s88buses; bus++ ) { // scan all busses
      int portidx;

      for( portidx = 0; portidx < ports[bus]; portidx++ ) { // check the s88 ports
        int port = S88_MAXPORTSB * bus + portidx;
        delta = (s88data[port] ^ s88old[port]);

        if( delta ) { // something has changed
          int bit;
          for( bit = 0; bit < 8; bit++ ) { // check the delta bits
            if( ( delta & BIT_VALUES[bit] ) ) { // Event happened
              int fbport = port * 8 + bit + 1;
              int fbstate = ( ( s88data[port] & BIT_VALUES[bit] ) != 0 );
              rocrail_ddxFbListener( (obj)inst, fbport, fbstate );
            }
          }
          s88old[port] = s88data[port]; // set the triggers
        }
      }
    }

  }
  freeMem( s88data );
  freeMem( s88old );
  TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "s88 polling stopped" );
}
