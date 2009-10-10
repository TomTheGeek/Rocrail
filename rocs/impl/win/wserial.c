/*
 Rocs - OS independent C library

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

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
#ifdef _WIN32

#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>

#include "rocs/impl/serial_impl.h"
#include "rocs/public/trace.h"
#include "rocs/public/thread.h"
#include "rocs/public/system.h"

#ifndef __ROCS_SERIAL__
    #pragma message("*** Win32 OSerial is disabled. (define __ROCS_SERIAL__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */

//OVERLAPPED overlapped_global;


Boolean rocs_serial_close( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  if( o->handle ) {
    int rc = CloseHandle( o->handle );
    if( !o->blocking ) {
      freeMem( o->overlapped );
      timeEndPeriod(1);
    }
    o->handle = INVALID_HANDLE_VALUE;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serial[%s] closed.", o->device );
  }
#endif
  return True;
}

Boolean rocs_serial_open( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);

  Boolean timeoutSet = False;
  Boolean lineSet    = False;
  int rc = 0;
  char port[14];
  strcpy (port,"\\\\.\\");
  strncat (port, o->device ,5 );
  rocs_serial_close( inst );

  if( o->portbase == 0 ) {
    if( StrOp.equals( "com1", o->device ) )
      o->portbase = 0x3F8;
    else if( StrOp.equals( "com2", o->device ) )
      o->portbase = 0x2F8;
    else if( StrOp.equals( "com3", o->device ) )
      o->portbase = 0x3E8;
    else if( StrOp.equals( "com4", o->device ) )
      o->portbase = 0x2E8;
  }

  o->handle = CreateFile( port,
                          GENERIC_READ | GENERIC_WRITE,
                          0,
                          NULL,
                          OPEN_EXISTING,
                          o->blocking == True ? 0: FILE_FLAG_OVERLAPPED,
                          NULL
                        );

  rc = GetLastError();
  TraceOp.terrno( name, TRCLEVEL_INFO, __LINE__, 9999, rc, "Opening serial[%s]  [return code=%d]", o->device, rc );

  if( o->handle != INVALID_HANDLE_VALUE ) {
    if (!o->blocking) {
      o->overlapped = (void *)allocMem( sizeof( struct _OVERLAPPED ) );
      ((OVERLAPPED *)o->overlapped)->Internal = 0;
      ((OVERLAPPED *)o->overlapped)->InternalHigh = 0;
      ((OVERLAPPED *)o->overlapped)->Offset = 0;
      ((OVERLAPPED *)o->overlapped)->OffsetHigh = 0;
      timeBeginPeriod(1);
    }
    /*Is portbase defined (com1-4)? Can we access it by giveio.sys?*/
    if ( o->portbase && SystemOp.accessPort(o->portbase,8) ) {
      /* Simple test for 16550 compatible Uart by writing to and read back from scratch register */
      SystemOp.writePort( o->portbase + 7, 0x55 );
      if (SystemOp.readPort( o->portbase + 7 )==0x55)
        o->directIO= True;
      else
        o->directIO= False;
    } else
      o->directIO= False;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "blocking[%d] directIO[%d]", o->blocking, o->directIO);

    COMMTIMEOUTS        cto;
    GetCommTimeouts( o->handle, &cto );

    /* CommTimeout */
    cto.ReadIntervalTimeout         = o->timeout.read;
    cto.ReadTotalTimeoutMultiplier  = 0;
    cto.ReadTotalTimeoutConstant    = o->timeout.read;

    cto.ReadTotalTimeoutMultiplier  = 0;
    cto.ReadTotalTimeoutConstant    = o->timeout.write;

    timeoutSet = SetCommTimeouts( o->handle, &cto );
    rc = GetLastError();
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Serial[%s] timeout setted.[rc=%d]", o->device, rc );

    if( timeoutSet ) {
      /* CommState */
      DCB   dcb;
      memset( &dcb, 0, sizeof( dcb ) );

      dcb.DCBlength         = sizeof(DCB) ;
      dcb.BaudRate          = o->line.bps;
      dcb.fBinary           = True;
      dcb.fParity           = False;
      dcb.fOutxCtsFlow      = o->line.flow == cts ? True:False;
      dcb.fOutxDsrFlow      = o->line.flow == dsr ? True:False;
      dcb.fDtrControl       = o->line.flow == dsr ? DTR_CONTROL_HANDSHAKE:DTR_CONTROL_DISABLE;
      dcb.fDsrSensitivity   = False;
      dcb.fTXContinueOnXoff = False;
      dcb.fOutX             = o->line.flow == xon ? True:False;
      dcb.fInX              = o->line.flow == xon ? True:False;
      dcb.fErrorChar        = False;
      dcb.fNull             = False;
      dcb.fRtsControl       = o->line.flow == cts ? RTS_CONTROL_HANDSHAKE:RTS_CONTROL_DISABLE;
      dcb.fAbortOnError     = False;
      dcb.wReserved         = 0;
      dcb.ByteSize          = o->line.bits;

      if( o->line.parity == even )
        dcb.Parity = EVENPARITY;
      else if( o->line.parity == odd )
        dcb.Parity = ODDPARITY;
      else if( o->line.parity == none )
        dcb.Parity = NOPARITY;

      if( o->line.stopbits == onestopbit )
        dcb.StopBits = ONESTOPBIT;
      if( o->line.stopbits == twostopbits )
        dcb.StopBits = TWOSTOPBITS;

      lineSet = SetCommState( o->handle, &dcb );
      rc = GetLastError();
      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Serial[%s] line setted.[rc=%d]", o->device, rc );
    }
    return True;
  }

#endif
  return False;
}

void rocs_serial_setRTS( iOSerial inst, Boolean rts ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int func = rts?SETRTS:CLRRTS;
  EscapeCommFunction( o->handle, func );
#else

  return;
#endif
}

void rocs_serial_setCTS( iOSerial inst, Boolean cts ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  /* TODO: set CTS */
#else

  return;
#endif
}

void rocs_serial_setDTR( iOSerial inst, Boolean dtr ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int func = dtr?SETDTR:CLRDTR;
  int rc = EscapeCommFunction( o->handle, func );
  if( rc == 0 ) {
    rc = GetLastError();
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Serial[%s] setDTR [error=%d][function=%d]", o->device, rc, func );
  }
#else
  return;
#endif
}

Boolean rocs_serial_isCTS( iOSerial inst ) {
  long state = 0;
#ifdef __ROCS_SERIAL__

  iOSerialData o = Data(inst);
  GetCommModemStatus( o->handle, &state );
#endif

  return state & MS_CTS_ON ? True:False;
}

Boolean rocs_serial_isDSR( iOSerial inst ) {
  long state = 0;
#ifdef __ROCS_SERIAL__

  iOSerialData o = Data(inst);
  GetCommModemStatus( o->handle, &state );
#endif

  return state & MS_DSR_ON ? True:False;
}

Boolean rocs_serial_isRI( iOSerial inst ) {
  long state = 0;
#ifdef __ROCS_SERIAL__

  iOSerialData o = Data(inst);
  if (o->directIO) {
    int msr = SystemOp.readPort( o->portbase + 6 );
    if( msr & 0x04 ) /* RI changed */
      return True;
  }
  GetCommModemStatus( o->handle, &state );
  return state & MS_RING_ON ? False:True;
#endif
}

int rocs_serial_getWaiting( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  if (o->blocking) {
    //      int lsr = SystemOp.readPort( o->portbase + 5 );
    /* 0x40 = Transmitter Holding Register is empty and the shift register too */
    /* this flag turns high on empty */
    //      return (lsr & 0x40) ? 0:1;
    return 0;  /*When using blocking io there are no waiting bytes, as write hangs until empty.*/
  } else {

    COMSTAT comStat;
    DWORD   dwErrors;

    // Get and clear current errors on the port.
    ClearCommError(o->handle, &dwErrors, &comStat);
    return comStat.cbOutQue;
  }
#endif
}


Boolean rocs_serial_isUartEmpty( iOSerial inst, Boolean soft ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  if (o->blocking) {
    return True; /*When using blocking io there are no waiting bytes, as write hangs until empty.*/
  } else {
    if ((o->directIO) || (!soft)) {
      if (!rocs_serial_getWaiting(inst)) {
        int lsr = SystemOp.readPort( o->portbase + 5 );
        /* 0x40 = Transmitter Holding Register is empty and the shift register too */
        /* this flag turns high on empty */
        return (lsr & 0x40) ? True:False;
      } else
        return False;
    } else {
      if (!rocs_serial_getWaiting(inst))
        return True; //TODO: add code for non-directIO.
      else
        return False;
    }
  }
#endif
}


void rocs_serial_setOutputFlow( iOSerial inst, Boolean flow ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int rc = 0;
  /* TODO: output flow on/off */
#endif
}


void rocs_serial_flush( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  int rc = PurgeComm( o->handle, PURGE_TXABORT | PURGE_TXCLEAR );
  if( rc == 0 ) {
    rc = GetLastError();
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Serial[%s] Flush error [error=%d]", o->device, rc );
  }
#endif
}



Boolean rocs_serial_write( iOSerial inst, char* buffer, int size ) {
  int rc     = 0;
#ifdef __ROCS_SERIAL__

  iOSerialData o = Data(inst);
  long sended = 0;
  int ok     = 0;

  if (o->blocking) {
    ok = WriteFile( o->handle, buffer, size, &sended, NULL );
  } else {
    ok = WriteFile( o->handle, buffer, size, &sended, o->overlapped );
  }
  if( !ok ) {
    rc = GetLastError();
    //    SerialOp.available( inst );
  }

  if( o->blocking && sended != size )
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "Serial[%s] %d of %d sent.[rc=%d]", o->device, sended, size, rc );
  else
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Serial[%s] %d sent.[rc=%d]", o->device, sended, rc );
  if (rc != ERROR_IO_PENDING) {
    o->ioState = ok;
    o->rc = rc;
    return rc > 0 ? False:True;
  } else {
    o->ioState = 1;
    o->rc = 0;
    return True;
  }
#endif
}

int rocs_serial_avail( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  unsigned long etat;
  struct _COMSTAT comstat;
  ClearCommError( o->handle, &etat, &comstat );
  return comstat.cbInQue;
#endif
}


Boolean rocs_serial_read( iOSerial inst, char* buffer, int size ) {
  int rc     = 0;
#ifdef __ROCS_SERIAL__

  iOSerialData o = Data(inst);
  long readed = 0;
  int  ok     = 0;
  long cnt    = 0;
  int  tries  = 0;

  do {
    if (o->blocking) {
      ok = ReadFile( o->handle, buffer, size, &cnt, NULL );
    } else {
      ok = ReadFile( o->handle, buffer, size, &cnt, o->overlapped );
    }

    if( !ok ) {
      rc = GetLastError();
      SerialOp.available( inst );
    }
    o->rc = rc;
    if( rc == 0 )
      readed += cnt;
    /* Check for timeout: */
    if( rc == 0 && cnt == 0 ) {
      tries++;
    }
  } while( rc == 0 && readed < size && tries < 3 );

  if( readed != size && rc == 0 )
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Timeout on serial read[%s] %d of %d read.[rc=%d] tries=%d", o->device, readed, size, rc, tries );
  else if( rc > 0 )
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "Serial[%s] %d of %d read.[rc=%d] tries=%d", o->device, readed, size, rc, tries );
  else
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "Serial[%s] %d read.[rc=%d]", o->device, readed, rc );

  o->rc = rc;
  o->ioState = ok;
  return readed == size ? True:False;
#else

  return False;
#endif
}

void rocs_serial_setSerialMode( iOSerial inst, serial_mode mode ) {
  iOSerialData o = Data(inst);
  DCB   dcb;
  int rc = 0;
  if (!o->directIO) {
    memset( &dcb, 0, sizeof( dcb ) );
    if (!GetCommState( o->handle, &dcb )) {
      rc = GetLastError();
      TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "GetCommState failed.[rc=%d]", rc );
      return;
    }
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
        dcb.BaudRate = 38400;
        dcb.ByteSize = 6;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
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
        dcb.BaudRate = 19200;
        dcb.ByteSize = 8;
        dcb.Parity = NOPARITY;
        dcb.StopBits = ONESTOPBIT;
      }
    }
    break;
  case mma:
    /* Set Uart register for 115200baud Marklin/Motorola Accessory */
    if (o->currserialmode != mma) {
      o->currserialmode = mma;
      if (o->directIO) {
        SystemOp.writePort( o->portbase + 3, 0x80 );
        SystemOp.writePort( o->portbase + 0, 0x01 );
        SystemOp.writePort( o->portbase + 1, 0x00 );
        SystemOp.writePort( o->portbase + 3, 0x1F );
      } else {
        dcb.BaudRate = 115200;
        dcb.ByteSize = 8;
        dcb.Parity = EVENPARITY;
        dcb.StopBits = TWOSTOPBITS;
      }
      break;
    }
    default: TraceOp.trc( __FILE__, TRCLEVEL_ERROR, __LINE__, 9999, "Error setting Serial mode!");
  }
  /* set attribute now */
  if (!o->directIO && !SetCommState( o->handle, &dcb )) {
    rc = GetLastError();
    TraceOp.trc( name, TRCLEVEL_WARNING, __LINE__, 9999, "SetCommState failed.[rc=%d]", rc );
  }
  //        tcgetattr( o->sh, &tio );
  //    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Current output baud rate is %d\n", (int) cfgetospeed(&tio) );

}

void rocs_serial_waitMM( iOSerial inst, int usperiod, int uspause  ) {
  iOSerialData o = Data(inst);
  if( o->directIO ) {
    while( !SerialOp.isUartEmpty( inst, True ) )
      ;
    if (usperiod>16000) /*If wait is longer than a process time slice, use normal sleep*/
      ThreadOp.sleep(uspause/1000);
    else
      SystemOp.uBusyWait(uspause);
  } else {
    if (usperiod>16000) /*If wait is longer than a process time slice, use normal sleep*/
      ThreadOp.sleep(usperiod/1000);
    else
      SystemOp.uBusyWait(usperiod);
  }
}


#endif
