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

#include "init.h"
#include "queue.h"
#include "locpool.h"
#include "s88.h"
#include "nmra.h"
#include "accpool.h"

#include "rocs/public/trace.h"
#include "rocs/public/serial.h"
#include "rocs/public/system.h"
#include "rocs/public/thread.h"
#include "rocs/public/mutex.h"


/* functions to open, initialize and close comport */

Boolean init_serialport(obj inst) {
  iODDXData data = Data((iODDX)inst);
  int rc = 0;
  data->communicationflag = 0;
  rocrail_ddxStateChanged(inst);

  if( SystemOp.accessDev(data->boosterport, True ) ) {
    TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "OK to access device [%s]", data->boosterport );
  } else {
    TraceOp.trc( __FILE__, TRCLEVEL_EXCEPTION, __LINE__, 9999, "no access rights to open device [%s]", data->boosterport );
    data->serial = NULL;
    return False;                             /* theres no chance to continue */
  }

  TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "opening device [%s]...", data->boosterport );
  data->serial = SerialOp.inst( data->boosterport );
  TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "device descriptor [0x%08X]", data->serial );

  if (data->serial == NULL) {
    TraceOp.trc( __FILE__, TRCLEVEL_EXCEPTION, __LINE__, 9999, "device %s can not be opened for writing. Abort!", data->boosterport );
    return False;
  }
  SerialOp.setPortBase( data->serial, data->portbase );
  SerialOp.setFlow( data->serial, 0 );
  SerialOp.setBlocking( data->serial, False );
  SerialOp.setLine( data->serial, 19200, 8, 0, 0 );
  //  SerialOp.setLine( data->serial, 38400, 6, 0, 0 );
  SerialOp.setCTS( data->serial, False); /*Don't use CTS handshake*/

  /* setting serial device to default mode */
  TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "opening device %s...", data->boosterport );
  if (!SerialOp.open( data->serial )) {
    TraceOp.trc( __FILE__, TRCLEVEL_EXCEPTION, __LINE__, 9999, "error initializing device %s. Abort!", data->boosterport );
    data->serial = NULL;
    return False;                             /* theres no chance to continue */
  }

  TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "setting the lines..." );
  /* now set some serial lines */
  SerialOp.setOutputFlow(data->serial,False);          /* suspend output */
  SerialOp.setRTS(data->serial,True);  /* +12V for ever on RTS   */
  SerialOp.setDTR(data->serial,False); /* disable booster output */

  TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "finished initializing device %s", data->boosterport );
  data->communicationflag = 1;
  rocrail_ddxStateChanged(inst);
  return True;
}

void close_comport(obj inst) {
  iODDXData data = Data((iODDX)inst);
  if (data->serial != NULL) {
    TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "closing device [0x%08X]", data->serial );
    SerialOp.close(data->serial);
  }
  data->communicationflag = 0;
  rocrail_ddxStateChanged(inst);
}

void start_voltage(obj inst) {
  iODDXData data = Data((iODDX)inst);
  if (data->communicationflag==1) {
    TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "starting refresh cycle..." );

    if (data->cycleThread==NULL) {
      TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "starting refresh cycle thread..." );
      data->cycleThread = ThreadOp.inst("locpool", &thr_refresh_cycle, inst);

      TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "setting thread policy and priority..." );
      ThreadOp.start( data->cycleThread );
      ThreadOp.setHigh( data->cycleThread );
    }

    ThreadOp.pause( data->cycleThread, False );
  } else {
    TraceOp.trc( __FILE__, TRCLEVEL_WARNING, __LINE__, 9999, "Can't power on, serial port is not open!" );
  }
}


void stop_voltage(obj inst) {
  iODDXData data = Data((iODDX)inst);
  if (data->cycleThread!=NULL) {
    ThreadOp.pause( data->cycleThread, True );
  }
}

void quit(obj inst) {
  iODDXData data = Data((iODDX)inst);
  if (data->s88pollt!=NULL) {
    ThreadOp.requestQuit( data->s88pollt );
  }
  if (data->cycleThread!=NULL) {
    ThreadOp.requestQuit( data->cycleThread );
    ThreadOp.sleep( 200 );
  }
}

int ddx_entry(obj inst, iONode ddx_ini) {
  iODDXData data = Data((iODDX)inst);
  if (init_serialport(inst)) {
    init_MaerklinPacketPool(inst);
    initMaerklinGaPacketPool();
    TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "marklin packet pool initialized." );
    init_NMRAPacketPool(inst);
    initNMRAGaPacketPool();
    TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "nmra packet pool initialized." );
    queue_init();
    TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "packet queue initialized" );
  } else {
    TraceOp.trc( __FILE__, TRCLEVEL_WARNING, __LINE__, 9999, "Serial port misfunction. Data output disabled" );
  }

  if (!s88init(inst)) {
    /* s88init traces out the progress and problems */
    TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "S88 port misfunction. S88 disabled" );
  }

  return 0;
}

