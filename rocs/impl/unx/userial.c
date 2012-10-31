/*
 Rocs - OS independent C library

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public License
 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#if defined __linux__ || defined _AIX || defined __unix__ || defined __APPLE__

#include "rocs/impl/serial_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/thread.h"
#include "rocs/public/system.h"
#include "rocs/public/str.h"

#include <fcntl.h>
#include <sys/types.h>
#ifdef __hpux
#include <sys/modem.h>
#endif
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

#include <stdlib.h>
#include <string.h>

/*#define __USE_MISC*/

#if defined __APPLE__ || defined __OpenBSD__
#include <sys/ioctl.h>
#include <paths.h>
#include <termios.h>
#include <sysexits.h>
#include <sys/param.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#else
#include <termio.h>
#include <termios.h>
#include "linux/serial.h"
#include <sys/io.h>
#endif

#ifndef __ROCS_SERIAL__
    #pragma message("*** Unix OSerial is disabled. (define __ROCS_SERIAL__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */
Boolean rocs_serial_close( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int rc = close( o->sh );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocs_serial_close rc=%d", errno );
  return rc ? False:True;
#else

  return False;
#endif
}

/* Conversions */
static speed_t __symbolicSpeed( int bps ) {
  /* AIX does not support speeds above 38k. */


#ifdef B1000000
  if( bps >= 1000000 )
    return B1000000;
#endif


#ifdef B500000
  if( bps >= 500000 )
    return B500000;
#endif


#ifdef B230400
  if( bps >= 230400 )
    return B230400;
#endif


#ifdef B115200
  if( bps >= 115200 )
    return B115200;
#endif

#ifdef B57600

  if( bps >=  57600 )
    return B57600;
#endif

  if( bps >=  38400 )
    return B38400;
  if( bps >=  19200 )
    return B19200;
  if( bps >=   9600 )
    return B9600;
  if( bps >=   4800 )
    return B4800;
  if( bps >=   2400 )
    return B2400;
  if( bps >=   1200 )
    return B1200;
  return B600;
}
static int __symbolicBits( int bits ) {
  if( bits >= 8 )
    return CS8;
  if( bits >= 7 )
    return CS7;
  if( bits >= 6 )
    return CS6;
  if( bits >= 5 )
    return CS5;
  return CS8;
}
/**
 * /dev/ttyS0 == com1
 * /dev/ttyS1 == com2
 */
Boolean rocs_serial_open( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  char* device = o->device;
  int r,w;

  /* open read/write, no controlling terminal */
  if( StrOp.equals( "com1", o->device ) )
    device = "/dev/ttyS0";
  else if( StrOp.equals( "com2", o->device ) )
    device = "/dev/ttyS1";
  else if( StrOp.equals( "com3", o->device ) )
    device = "/dev/ttyS2";
  else if( StrOp.equals( "com4", o->device ) )
    device = "/dev/ttyS3";

  /*
  ttyS0 3F8
  ttyS1 2F8
  ttyS2 3E8
  ttyS3 2E8
  */
  if( o->portbase == 0 ) {
    if( StrOp.equals( "/dev/ttyS0", device ) )
      o->portbase = 0x3F8;
    else if( StrOp.equals( "/dev/ttyS1", device ) )
      o->portbase = 0x2F8;
    else if( StrOp.equals( "/dev/ttyS2", device ) )
      o->portbase = 0x3E8;
    else if( StrOp.equals( "/dev/ttyS3", device ) )
      o->portbase = 0x2E8;
  }

#if defined __APPLE__ || defined __OpenBSD__
    o->directIO=False;
#else
  if( o->portbase > 0 && ioperm(o->portbase, 8, 1) == 0 ) {
    /* Simple test for 16550 compatible Uart by writing to and read back from scratch register */
    SystemOp.writePort( o->portbase + 7, 0x55 );
    if (SystemOp.readPort( o->portbase + 7 )==0x55)
      o->directIO= True;
    else
      o->directIO= False;
  } else {
    o->directIO=False;
  }
#endif

  //    o->directIO=False; //Test, remove
  errno = 0;
  o->sh = open( device, O_RDWR | O_TRUNC | O_NONBLOCK | O_NOCTTY  );

  r = access( device, R_OK );
  w = access( device, W_OK );

  TraceOp.terrno( name, TRCLEVEL_INFO, __LINE__, 9999, errno, "rocs_serial_open:open rc=%d read=%d write=%d", errno, r, w );

  if( o->sh > 0 ) {
    struct termios tio;

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "blocking[%d] directIO[%d]", o->blocking, o->directIO);
#if defined __APPLE__ || defined __OpenBSD__
#else
    if( o->divisor > 0 ) {
      struct serial_struct serial;
      ioctl(o->sh, TIOCGSERIAL, &serial);
      serial.custom_divisor = o->divisor;
      serial.flags &= ~ASYNC_USR_MASK;
      serial.flags |= ASYNC_SPD_CUST | ASYNC_LOW_LATENCY;
      ioctl(o->sh, TIOCSSERIAL, &serial);
    } else {
      struct serial_struct serial;
      ioctl(o->sh, TIOCGSERIAL, &serial);
      serial.custom_divisor = 0;
      serial.flags = 0;
      ioctl(o->sh, TIOCSSERIAL, &serial);
    }
#endif

    tcgetattr( o->sh, &tio );

    /***** CONTROL FLAGS: reset */
    tio.c_cflag = 0;
    /* hardwareflow */
#ifdef CRTSCTS

    if( o->line.flow == cts )
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocs_serial_open: set CRTSCTS" );
    tio.c_cflag |= o->line.flow == cts ? CRTSCTS:0;
#endif
    /* ignore modem, enable receiver */
    tio.c_cflag |= CLOCAL | CREAD;

    /* set data bits */
    tio.c_cflag |= __symbolicBits( o->line.bits );

    /* set parity */
    tio.c_cflag |= o->line.parity != none ? PARENB:0;
    tio.c_cflag |= o->line.parity == odd  ? PARODD:0;

    /* set stop bits */
    tio.c_cflag |= o->line.stopbits == twostopbits ? CSTOPB:0;

    /***** INPUT FLAGS: reset all input control processing */
    /*tio.c_iflag = (IGNBRK | IGNCR);*/
    tio.c_iflag = o->line.parity != none ? INPCK:0;

    /***** OUTPUT FLAGS: reset all output control processing */
    tio.c_oflag = 0;

    /***** LOCAL FLAGS: raw mode */
    tio.c_lflag = NOFLSH;

    /* minimal receive count */
    tio.c_cc[VMIN]  = 0;
    /* set timeout int 1/10 seconds */
    tio.c_cc[VTIME] = (o->timeout.read / 100) ? (o->timeout.read / 100):0;

    /* insert speed */
    cfsetospeed( &tio, __symbolicSpeed(o->line.bps) );
    cfsetispeed( &tio, __symbolicSpeed(o->line.bps) );

    errno = 0;
    /* set attribute now */
    tcsetattr( o->sh, TCSANOW, &tio );

    //    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocs_serial_open:tcsetattr rc=%d", errno );
  }

  //  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "rocs_serial_open rc=%d", errno );
  return o->sh < 0 ? False:True;
#else

  return False;
#endif
}


static int __last_msr = 0;
static void __printmsr(int msr) {
  if( msr != __last_msr && (TraceOp.getLevel(NULL) & TRCLEVEL_DEBUG) ) {
    char *le  ="   ";
    char *st  ="   ";
    char *sr  ="   ";
    char *rts ="   ";
    char *cts ="   ";
    char *dsr ="   ";
    char *dtr ="   ";
    char *ri  ="   ";
    char *car ="   ";

    __last_msr = msr;

    if(msr & TIOCM_LE)
      le ="LE ";
    if(msr & TIOCM_ST)
      st ="ST ";
    if(msr & TIOCM_SR)
      sr ="SR ";
    if(msr & TIOCM_DSR)
      dsr="DSR";
    if(msr & TIOCM_RI )
      ri ="RI ";
    if(msr & TIOCM_CTS)
      cts="CTS";
    if(msr & TIOCM_RTS)
      rts="RTS";
    if(msr & TIOCM_DTR)
      dtr="DTR";
    if(msr & TIOCM_CAR)
      dtr="CAR";
    printf( "[%s][%s][%s][%s][%s][%s][%s][%s][%s][%04X]\n", le,st,sr,rts,cts,dsr,dtr,ri,car, msr);
  }
}

static int __last_val = 0;
static Boolean __printport(iOSerial inst, int* richg) {
  iOSerialData o = Data(inst);
#if defined __APPLE__ || defined __OpenBSD__
    return True;
#else
  if( ioperm(o->portbase, 7, 1) == 0 ) {
    int lsr = inb(o->portbase+5);
    int msr = inb(o->portbase+6);
    if( (lsr+msr) != __last_val  && (TraceOp.getLevel(NULL) & TRCLEVEL_DEBUG) ) {
      printf( "lsr[0x%02X] msr[0x%02X] %s%s%s \n", lsr,msr, lsr&0x40?"UART is empty ":"", msr&0x02?"DSR changed ":"", msr&0x04?"RI changed ":"" );
      __last_val=lsr+msr;
    }
    /* RI changed */
    if( richg != NULL )
      *richg = msr&0x04?1:0;

    /*UART is empty*/
    if(lsr&0x40)
      return True;
    else
      return False;
  }
  return True;
#endif
}

void rocs_serial_setRTS( iOSerial inst, Boolean rts ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int msr = 0;
  int rc  = 0;
  if (o->directIO) {
    __printport(inst, NULL);
  }
  rc = ioctl( o->sh, TIOCMGET, &msr );
  if (o->directIO) {
    __printmsr(msr);
  }
  if( rts )
    msr |= TIOCM_RTS;
  else
    msr &= ~TIOCM_RTS;
  rc = ioctl( o->sh, TIOCMSET, &msr );
#else

  return;
#endif
}

void rocs_serial_setCTS( iOSerial inst, Boolean cts ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int msr = 0;
  int rc  = 0;
  if (o->directIO) {
    __printport(inst, NULL);
  }
  rc = ioctl( o->sh, TIOCMGET, &msr );
  if (o->directIO) {
    __printmsr(msr);
  }
  if( cts )
    msr |= TIOCM_CTS;
  else
    msr &= ~TIOCM_CTS;
  rc = ioctl( o->sh, TIOCMSET, &msr );
#else

  return;
#endif
}

void rocs_serial_setDTR( iOSerial inst, Boolean dtr ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int msr = 0;
  int rc  = 0;
  if (o->directIO) {
    __printport(inst, NULL);
  }
  rc = ioctl( o->sh, TIOCMGET, &msr );
  if (o->directIO) {
    __printmsr(msr);
  }
  if( dtr )
    msr |= TIOCM_DTR;
  else
    msr &= ~TIOCM_DTR;
  rc = ioctl( o->sh, TIOCMSET, &msr );
#else

  return;
#endif
}

Boolean rocs_serial_isCTS( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int msr = 0xFFFF;
  int rc  = 0;
  if (o->directIO) {
    __printport(inst, NULL);
  }
  rc = ioctl( o->sh, TIOCMGET, &msr );
  if (o->directIO) {
    __printmsr(msr);
  }
  if( rc < 0 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,"TIOCMGET returns rc=%d errno=%d\n", rc, errno );
    if( errno == ENXIO )
      return -1;
  }
  if( msr & TIOCM_CTS )
    return True;
  else
    return False;
#else

  return False;
#endif
}

Boolean rocs_serial_isDSR( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int msr = 0xFFFF;
  int rc  = 0;
  if (o->directIO) {
    __printport(inst, NULL);
  }
  rc = ioctl( o->sh, TIOCMGET, &msr );
  if (o->directIO) {
    __printmsr(msr);
  }
  if( rc < 0 )
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999,"TIOCMGET returns rc=%d errno=%d\n", rc, errno );
  if (msr & TIOCM_DSR)
    return True;
  else
    return False;
#else

  return False;
#endif
}

Boolean rocs_serial_isRI( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
#if defined __APPLE__ || defined __OpenBSD__
    return True;
#else
  iOSerialData o = Data(inst);
  int msr = 0;
  int result, arg;
  if (o->directIO) {
    msr = inb(o->portbase+6);
  }
  result = ioctl(o->sh, TIOCMGET, &arg);
  if ((result>=0)&&((!(arg&TIOCM_RI))||(msr&0x04)))
    return True;
#endif
#endif

  return False;
}

Boolean rocs_serial_write( iOSerial inst, char* buffer, int size ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int written = write( o->sh, buffer, size );
  if (o->blocking)
    tcdrain(o->sh);
  if( size != written ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "rocs_serial_write size=%d written=%d errno=%d", size, written, errno );
  }
  return written != size ? False:True;
#else

  return False;
#endif
}

int rocs_serial_avail( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int rc = 0;
  int nbytes = 0;
#ifdef FIONREAD

  rc = ioctl( o->sh, FIONREAD, &nbytes );
  if( rc<0 ) {
    TraceOp.terrno( name, TRCLEVEL_WARNING, __LINE__, 9999, errno, "ioctl FIONREAD error" );
    /*if(errno == ENXIO || errno == EIO)*/
    if(errno == ENXIO)
      return -1;
  }
#elif defined TIOCINQ

  rc = ioctl( o->sh, TIOCINQ, &nbytes );
  if( rc<0 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "ioctl TIOCINQ error" );
  }
#endif

  return nbytes;
#endif
}

int rocs_serial_getWaiting( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int rc = 0;
  int nbytes = 0;
  /*For USB serial devices TIOCOUTQ may return much earlier
   * (report empty earlier) than ordinary UART.
   * It may not even support TIOCOUTQ... */
  rc = ioctl(o->sh,TIOCOUTQ, &nbytes);
  if( rc<0 ) {
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "ioctl TIOCOUTQ error" );
  }
  return nbytes;
#endif
}

Boolean rocs_serial_isUartEmpty( iOSerial inst, Boolean soft ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
#if defined __APPLE__ || defined __OpenBSD__
    return True;
#else

  if( soft || !o->directIO) {
    int rc = 0;
    int result;
    /*First try TIOCOUTQ, if queue not empty, the Uart will not be either*/
    if (rocs_serial_getWaiting( inst ))
      return False;
    rc = ioctl( o->sh,TIOCSERGETLSR, &result );
    if( rc<0 ) {
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "ioctl TIOCSERGETLSR error" );
      /* If TIOCSERGETLSR fails, the serial port probably does not support this ioctl query
       * (TIOCSERGETLSR is not POSIX compliant, it's Linux specific),
       * last resort is to wait for port to drain. For USB serial devices both TIOCOUTQ and tcdrain
       * may return much earlier (report empty earlier) than ordinary UART, because of buffering
       * in the USB device, which driver sometimes doesn't know about. */
      rc = tcdrain(o->sh);
      if( rc<0 ) {
        TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "tcdrain error" );
      }
      return True;
    } else
      /* TIOCSERGETLSR worked, return result*/
      return result ? True:False;
  } else if( ioperm(o->portbase, 7, 1) == 0 ) {
    int lsr = inb(o->portbase+5);
    /*UART is empty*/
    if(lsr&0x40)
      return True;
    else
      return False;
  } else
    return True;
#endif
#endif
}


void rocs_serial_setOutputFlow( iOSerial inst, Boolean flow ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int rc = 0;
  /*rc = tcflow(o->sh, flow?TCOON:TCOOFF);*/
#endif
}


void rocs_serial_flush( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int rc = 0;
  rc = tcflush(o->sh, TCOFLUSH);
  if( rc<0 ) {
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "tcflush error" );
  }
#endif
}


Boolean rocs_serial_read( iOSerial inst, char* buffer, int size ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int readcnt = 0;
  int rc     = 0;
  int tries  = 0;
  int maxtries = o->timeout.read / 10;
  Boolean timeout = False;
  tracelevel level = TRCLEVEL_DEBUG;
  o->rc = 0;
  o->read = 0;
  do {
    rc = read( o->sh, buffer + readcnt, size - readcnt );
    if( rc < 0 ) {
      /* fix for slow USB devices */
      if( errno == EAGAIN )
        rc = 0;
      else
        o->rc = errno;
    }
    if( rc > 0 )
      readcnt += rc;
    /* Check for timeout: */
    if( rc == 0 && o->rc == 0 ) {
      tries++;
      ThreadOp.sleep(10);
    }
  } while( tries < maxtries && rc >= 0 && readcnt < size && o->rc == 0);

  /* timeout? */
  if( size > readcnt && rc == 0 && o->rc == 0 ) {
    timeout = True;
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999,
                 "***READ TIMEOUT*** size=%d rc=%d read=%d errno=%d tries=%d",
                 size, rc, readcnt, o->rc, tries );
  }

  if( size != readcnt && o->rc != 0 )
    level = TRCLEVEL_EXCEPTION;

  TraceOp.trc( name, level, __LINE__, 9999,
               "%s size=%d rc=%d read=%d errno=%d",
               (timeout?"***READ TIMEOUT***":"read"), size, rc, readcnt, o->rc );
  o->read = readcnt;
  return readcnt == size ? True:False;
#else

  return False;
#endif
}

void rocs_serial_setSerialMode( iOSerial inst, serial_mode mode ) {
  iOSerialData o = Data(inst);
  int errno;
#if defined __APPLE__
#else
  struct termios tio;
  if (!o->directIO) {
    tcgetattr( o->sh, &tio );
    tio.c_cflag &= ~PARENB;
    tio.c_cflag &= ~CSTOPB;
    tio.c_cflag &= ~CSIZE;
  }
  switch (mode) {
  case mm:
  /* Set Uart register for 38400baud Marklin/Motorola*/
    if (o->currserialmode != mm) {
      o->currserialmode = mm;
      if (o->directIO) {
        SystemOp.writePort( o->portbase + 3, 0x80 );
        SystemOp.writePort( o->portbase + 0, 0x03 );
        SystemOp.writePort( o->portbase + 1, 0x00 );
        SystemOp.writePort( o->portbase + 3, 0x01 );
      } else {
        tio.c_cflag |= CS6;
        cfsetospeed( &tio, B38400 );
        cfsetispeed( &tio, B38400 );
      }
    }
    break;
  case dcc:
  /* Set Uart register for 19200baud NMRA DCC */
    if (o->currserialmode != dcc) {
      o->currserialmode = dcc;
      if (o->directIO) {
        SystemOp.writePort( o->portbase + 3, 0x80 );
        SystemOp.writePort( o->portbase + 0, 0x06 );
        SystemOp.writePort( o->portbase + 1, 0x00 );
        SystemOp.writePort( o->portbase + 3, 0x03 );
      } else {
        tio.c_cflag |= CS8;
        cfsetospeed( &tio, B19200 );
        cfsetispeed( &tio, B19200 );
      }
    }
    break;
  case mma:
    if (o->currserialmode != mma) {
      o->currserialmode = mma;
      if (o->directIO) {
        SystemOp.writePort( o->portbase + 3, 0x80 );
        SystemOp.writePort( o->portbase + 0, 0x01 );
        SystemOp.writePort( o->portbase + 1, 0x00 );
        SystemOp.writePort( o->portbase + 3, 0x1F );
      } else {
        tio.c_cflag |= CS8;
        tio.c_cflag |= PARENB;
        tio.c_cflag |= CSTOPB;
        cfsetospeed( &tio, B115200 );
        cfsetispeed( &tio, B115200 );
      }
      break;
    }
    default: TraceOp.trc( __FILE__, TRCLEVEL_ERROR, __LINE__, 9999, "Error setting Serial mode!");
  }
  /* set attribute now */
  if (!o->directIO && tcsetattr( o->sh, TCSAFLUSH, &tio ) != 0)
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "tcsetattr failed!" );
  //        tcgetattr( o->sh, &tio );
  //    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Current output baud rate is %d\n", (int) cfgetospeed(&tio) );
#endif
}

void rocs_serial_waitMM( iOSerial inst, int usperiod, int uspause  ) {
#if defined __APPLE__
#else
  iOSerialData o = Data(inst);
  int rc;
  int result;
  if( o->directIO ) {
    while (!rocs_serial_isUartEmpty(inst,True))
      ;
    if (usperiod>10000) /*If wait is longer than a process time slice, use normal sleep*/
      ThreadOp.sleep(uspause/1000);
    else
      SystemOp.uBusyWait(uspause);
  } else {
    if (usperiod>10000) /*If wait is longer than a process time slice, use normal sleep*/
      ThreadOp.sleep(usperiod/1000);
    else
      SystemOp.uBusyWait(usperiod);
  }
#endif
}

#endif
