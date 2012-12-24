/** ------------------------------------------------------------
  * $Source: /home/cvs/xspooler/rocs/impl/os2/oserial.c,v $
  * $Author: rob $
  * $Date: 2005-02-08 06:49:52 $
  * $Revision: 1.9 $
  * $Name:  $
  * ------------------------------------------------------------ */
#if defined __OS2__

#include <stdlib.h>
#include <string.h>

#include "rocs/impl/serial_impl.h"
#include "rocs/public/trace.h"

#define INCL_DOS
#define INCL_DEV
#define INCL_DOSDEVIOCTL
#include <os2.h>
/*#include <bsedev.h>*/

#ifndef __ROCS_SERIAL__
	#pragma message("*** OS/2 OSerial is disabled. (define __ROCS_SERIAL__ in rocs.h) ***")
#endif
/*
 ***** __Private functions.
 */
Boolean rocs_serial_close( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  unsigned long handle = (unsigned long)o->handle;
  if( o->handle ) {
    int rc = DosClose( handle );
    o->handle = NULL;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serial[%s][%d] closed.", o->device, rc );
  }
  return True;
#else
  return False;
#endif
}

Boolean rocs_serial_open( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);

  Boolean timeoutSet = False;
  Boolean lineSet    = False;
  int rc = 0;

  void*         ParmList        = NULL;
  unsigned long ParmLengthMax   = 0;
  unsigned long ParmLengthInOut = 0;
  unsigned long DataLengthMax   = 0;
  unsigned long DataLengthInOut = 0;
  void*         DataArea        = NULL;
  unsigned long action          = 0;

  rocs_serial_close( inst );

  rc = DosOpen( o->device, (unsigned long *)&o->handle, &action,
                0L,
                FILE_NORMAL,
			          FILE_OPEN,
			          OPEN_ACCESS_READWRITE | OPEN_SHARE_DENYNONE,
			          0L
              );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serial[%s] opened.[rc=%d]", o->device, rc );

  if( rc == 0 ) {
    DCBINFO       DCBInfo;

    DCBInfo.usWriteTimeout = o->timeout.write;
    DCBInfo.usReadTimeout  = o->timeout.read;
    DCBInfo.fbCtlHndShake  = o->line.flow == cts ? MODE_CTS_HANDSHAKE:0;
    DCBInfo.fbFlowReplace  = 0;
    DCBInfo.fbTimeout      = MODE_READ_TIMEOUT;

    ParmList        = NULL;
    ParmLengthInOut = 0;
    DataLengthInOut = 0;
    DataLengthMax   = 0;
    DataArea        = NULL;

    DosDevIOCtl( (unsigned long)o->handle,
                 IOCTL_ASYNC,
                 ASYNC_GETDCBINFO,
                 ParmList,
                 ParmLengthMax,
                 &ParmLengthInOut,
                 &DCBInfo,
                 sizeof (DCBInfo),
                 &DataLengthInOut
                );


    ParmList        = NULL;
    ParmLengthInOut = 0;
    DataLengthInOut = 0;
    DataLengthMax   = 0;
    DataArea        = NULL;

    rc = DosDevIOCtl( (unsigned long)o->handle,
                      IOCTL_ASYNC,
                      ASYNC_SETDCBINFO,
                      &DCBInfo,
                      sizeof (DCBInfo),
                      &ParmLengthInOut,
                      DataArea,
                      DataLengthMax,
                      &DataLengthInOut
                     );

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serial[%s] timeout setted.[rc=%d]", o->device, rc );
    timeoutSet = rc == 0 ? True:False;
  }

  if( timeoutSet ) {
    LINECONTROL   LineCtrl;

    /***** Set data rate to 2400 bps *****/

    ParmList        = NULL;
    ParmLengthInOut = 0;
    ParmLengthMax   = sizeof( o->line.bps );
    DataLengthInOut = 0;
    DataLengthMax   = 0;
    DataArea        = NULL;

    rc = DosDevIOCtl( (unsigned long)o->handle,
                      IOCTL_ASYNC,
                      ASYNC_SETBAUDRATE,
                      &o->line.bps,
                      ParmLengthMax,
                      &ParmLengthInOut,
                      DataArea,
                      DataLengthMax,
                      &DataLengthInOut
                     );


    /***** Set line format to N82 *****/

    if( o->line.stopbits == onestopbit )
      LineCtrl.bStopBits = 0;
    if( o->line.stopbits == twostopbits )
      LineCtrl.bStopBits = 2;

    if( o->line.parity == even )
      LineCtrl.bParity = 0;
    else if( o->line.parity == odd )
      LineCtrl.bParity = 0;
    else if( o->line.parity == none )
      LineCtrl.bParity = 0;

    LineCtrl.fTransBreak = FALSE;
    LineCtrl.bDataBits   = o->line.bits;

    ParmList        = NULL;
    ParmLengthInOut = 0;
    ParmLengthMax   = sizeof( LineCtrl );
    DataLengthInOut = 0;
    DataLengthMax   = 0;
    DataArea        = NULL;

    rc = DosDevIOCtl( (unsigned long)o->handle,
                      IOCTL_ASYNC,
                      ASYNC_SETLINECTRL,
                      &LineCtrl,
                      ParmLengthMax,
                      &ParmLengthInOut,
                      DataArea,
                      DataLengthMax,
                      &DataLengthInOut);


    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serial[%s] line setted.[rc=%d]", o->device, rc );
    lineSet = rc == 0 ? True:False;
  }

  return lineSet;
#else
  return False;
#endif
}

Boolean rocs_serial_isCTS( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  unsigned char state           = 0;
  unsigned long ParmLengthInOut = 0;
  unsigned long DataLengthInOut = 0;
  int rc = DosDevIOCtl( (unsigned long)o->handle,
                    IOCTL_ASYNC,
                    ASYNC_GETMODEMINPUT,
                    &state,
                    0,
                    &ParmLengthInOut,
                    NULL,
                    sizeof( state ),
                    &DataLengthInOut);
  if( rc == 0 && state & CTS_ON )
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
  unsigned char state           = 0;
  unsigned long ParmLengthInOut = 0;
  unsigned long DataLengthInOut = 0;
  int rc = DosDevIOCtl( (unsigned long)o->handle,
                    IOCTL_ASYNC,
                    ASYNC_GETMODEMINPUT,
                    &state,
                    0,
                    &ParmLengthInOut,
                    NULL,
                    sizeof( state ),
                    &DataLengthInOut);
  if( rc == 0 && state & DSR_ON )
    return True;
  else
    return False;
#else
  return False;
#endif
}

Boolean rocs_serial_write( iOSerial inst, char* buffer, int size ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  unsigned long handle = (unsigned long)o->handle;

  unsigned long sended = 0;
  int rc     = 0;

  rc = DosWrite( handle, buffer, size, &sended );

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serial[%s] %d sended.[rc=%d]", o->device, sended, rc );

  return True;
#else
  return False;
#endif
}

int rocs_serial_avail( iOSerial inst ) {
#ifdef __ROCS_SERIAL__
  int rc = 0;
  int nbytes = 0;
  return nbytes;
#endif
}


Boolean rocs_serial_read( iOSerial inst, char* buffer, int size ) {
#ifdef __ROCS_SERIAL__
  iOSerialData o = Data(inst);
  unsigned long handle = (unsigned long)o->handle;

  unsigned long readed = 0;
  int rc     = 0;

  rc = DosRead( handle, buffer, size, &readed );
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "Serial[%s] %d readed.[rc=%d]", o->device, readed, rc );

  return True;
#else
  return False;
#endif
}


#endif
