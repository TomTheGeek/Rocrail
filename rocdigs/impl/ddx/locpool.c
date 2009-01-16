/* +----------------------------------------------------------------------+ */
/* | DDL - Digital Direct for Linux                                       | */
/* +----------------------------------------------------------------------+ */
/* | Copyright (c) 1999 - 2003 Vogt IT                                    | */
/* +----------------------------------------------------------------------+ */
/* | This source file is subject of the GNU general public license 2,     | */
/* | that is bundled with this package in the file COPYING, and is        | */
/* | available at through the world-wide-web at                           | */
/* | http://www.gnu.org/licenses/gpl.txt                                  | */
/* | If you did not receive a copy of the PHP license and are unable to   | */
/* | obtain it through the world-wide-web, please send a note to          | */
/* | gpl-license@vogt-it.com so we can mail you a copy immediately.       | */
/* +----------------------------------------------------------------------+ */
/* | Authors:   Torsten Vogt vogt@vogt-it.com                             | */
/* |                                                                      | */
/* +----------------------------------------------------------------------+ */

/***************************************************************/
/* erddcd - Electric Railroad Direct Digital Command Daemon    */
/*    generates without any other hardware digital commands    */
/*    to control electric model railroads                      */
/*                                                             */
/* file: cycles.c                                              */
/* job : implements some cycles to generate base voltages on   */
/*       the track.                                            */
/*                                                             */
/* Torsten Vogt, march 1999                                    */
/*                                                             */
/* last changes:                                               */
/*               Torsten Vogt, december 2001                   */
/*               Torsten Vogt, september 2000                  */
/*               Rob Verslius, 2007                            */
/*               Torbjörn Björk, 2007                          */
/***************************************************************/

#include "rocdigs/impl/ddx_impl.h"
#include "init.h"
#include "queue.h"
#include "locpool.h"
#include "motorola.h"
#include "nmra.h"

#include "rocs/public/trace.h"
#include "rocs/public/thread.h"
#include "rocs/public/system.h"
#include "rocs/public/mutex.h"
#include "rocs/public/serial.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define MAXDATA      52
char idle_data[MAXDATA];
char NMRA_idle_data[PKTSIZE];

typedef struct _locorefreshdata {
  int last_refreshed_loco;
  int last_refreshed_fx;
  int last_refreshed_nmra_loco;
  int nmra_fx_refresh;
  int maerklin_refresh;
  int mm_locorefresh;
  int dcc_locorefresh;
}
locorefreshdata ;

/****** routines and data types for maerklin packet pool ******/

static iOMutex maerklin_pktpool_mutex = NULL;
static int isMaerklinPackedPoolInitialized = False;

tMaerklinPacketPool MaerklinPacketPool;

int monitor_NrOfMLocos() {
  return MaerklinPacketPool.NrOfKnownAdresses;
}

int init_MaerklinPacketPool(obj inst) {
  iODDXData data = Data((iODDX)inst);
  int i,j;
  int error;

  maerklin_pktpool_mutex = MutexOp.inst( NULL, True );

  MutexOp.wait(maerklin_pktpool_mutex);
  for (i=0; i<=MAX_MARKLIN_ADDRESS; i++) {
    MaerklinPacketPool.knownAdresses[i]=0;
    strcpy(MaerklinPacketPool.packets[i].info.protocol,"M2");
    MaerklinPacketPool.packets[i].info.addr=i;
    MaerklinPacketPool.packets[i].info.speed=0;
    MaerklinPacketPool.packets[i].info.speed_max=14;
    MaerklinPacketPool.packets[i].info.direction=1;
    MaerklinPacketPool.packets[i].info.func=0;
    MaerklinPacketPool.packets[i].info.nro_f=4;
    for (j=0; j<8; j++)
      MaerklinPacketPool.packets[i].info.f[j]=0;
  }
  MaerklinPacketPool.NrOfKnownAdresses = 1;
  MaerklinPacketPool.knownAdresses[MaerklinPacketPool.NrOfKnownAdresses-1]=81;
  /* generate idle packet */
  for (i=0; i<4; i++) {
    MaerklinPacketPool.packets[81].packet[2*i]   = HI;
    MaerklinPacketPool.packets[81].packet[2*i+1] = LO;
    for (j=0; j<4; j++) {
      MaerklinPacketPool.packets[81].f_packets[j][2*i]   = HI;
      MaerklinPacketPool.packets[81].f_packets[j][2*i+1] = LO;
    }
  }
  for (i=4; i<9; i++) {
    MaerklinPacketPool.packets[81].packet[2*i]   = LO;
    MaerklinPacketPool.packets[81].packet[2*i+1] = LO;
    for (j=0; j<4; j++) {
      MaerklinPacketPool.packets[81].f_packets[j][2*i]   = LO;
      MaerklinPacketPool.packets[81].f_packets[j][2*i+1] = LO;
    }
  }
  isMaerklinPackedPoolInitialized = True;
  MutexOp.post(maerklin_pktpool_mutex);

  /* generate idle_data */
  for (i=0; i<MAXDATA; i++)
    idle_data[i]=0x55;      /* 0x55 = 01010101 */
  for (i=0; i<PKTSIZE; i++)
    NMRA_idle_data[i]=0x55;
  /* ATTENTION: if nmra dcc mode is activated idle_data[] and NMRA_idle_data
                must be overidden from init_NMRAPacketPool().
                This means, that init_NMRAPacketPool()
                must be called after init_MaerklinPacketPool().
   */

  TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "Maerklin packet pool OK" );
  return 0;
}

char *get_maerklin_packet(int adr, int fx) {
  return MaerklinPacketPool.packets[adr].f_packets[fx];
}

int get_maerklin_direction(int addr) {
  return MaerklinPacketPool.packets[addr].info.direction;
}

int get_maerklin_speed(int addr) {
  return MaerklinPacketPool.packets[addr].info.speed;
}

tLocoInfo *get_MaerklinPacketPool_LocoInfo(int addr) {
  return &(MaerklinPacketPool.packets[addr].info);
}

void update_MaerklinPacketPool_Loco_Data(int addr, int direction, int speed, int func,
    int f1, int f2, int f3, int f4) {

  MaerklinPacketPool.packets[addr].info.speed=speed;
  MaerklinPacketPool.packets[addr].info.direction=direction;
  MaerklinPacketPool.packets[addr].info.func=func;
  MaerklinPacketPool.packets[addr].info.f[0]=f1;
  MaerklinPacketPool.packets[addr].info.f[1]=f2;
  MaerklinPacketPool.packets[addr].info.f[2]=f3;
  MaerklinPacketPool.packets[addr].info.f[3]=f4;
}

void update_MaerklinPacketPool(int adr, char *sd_packet, char *f1, char *f2,
                               char *f3, char *f4) {

  int i, found;

  found = 0;
  for (i=0; i<MaerklinPacketPool.NrOfKnownAdresses && !found; i++)
    if (MaerklinPacketPool.knownAdresses[i]==adr)
      found=True;

  MutexOp.wait(maerklin_pktpool_mutex);
  memcpy(MaerklinPacketPool.packets[adr].packet,sd_packet,18);
  memcpy(MaerklinPacketPool.packets[adr].f_packets[0],f1,18);
  memcpy(MaerklinPacketPool.packets[adr].f_packets[1],f2,18);
  memcpy(MaerklinPacketPool.packets[adr].f_packets[2],f3,18);
  memcpy(MaerklinPacketPool.packets[adr].f_packets[3],f4,18);
  MutexOp.post(maerklin_pktpool_mutex);

  if (MaerklinPacketPool.NrOfKnownAdresses==1 &&
      MaerklinPacketPool.knownAdresses[0]==81) {
    MaerklinPacketPool.NrOfKnownAdresses=0;
  }
  if (!found) {
    MaerklinPacketPool.knownAdresses[MaerklinPacketPool.NrOfKnownAdresses]=adr;
    MaerklinPacketPool.NrOfKnownAdresses++;
  }
}

/**********************************************************/

/****** routines and data types for NMRA packet pool ******/

static iOMutex nmra_pktpool_mutex = NULL;
static int isNMRAPackedPoolInitialized = False;
tNMRAPacketPool NMRAPacketPool;

int monitor_NrOfNLocos() {
  return NMRAPacketPool.NrOfKnownAdresses;
}

int init_NMRAPacketPool(obj inst) {
  iODDXData data = Data((iODDX)inst);
  int i,j;
  int error;
  char idle_packet[] = "11111111111111101111111100000000001111111110";
  char idle_pktstr[PKTSIZE];

  nmra_pktpool_mutex = MutexOp.inst( NULL, True );

  MutexOp.wait(nmra_pktpool_mutex);
  for (i=0; i<=MAX_NMRA_ADDRESS; i++) {
    NMRAPacketPool.knownAdresses[i]=0;
    strcpy(NMRAPacketPool.packets[i].info.protocol,"NB");
    if (i<ADDR14BIT_OFFSET)
      NMRAPacketPool.packets[i].info.addr=i;
    else
      NMRAPacketPool.packets[i].info.addr=i-ADDR14BIT_OFFSET;
    NMRAPacketPool.packets[i].info.speed=0;
    NMRAPacketPool.packets[i].info.speed_max=14;
    NMRAPacketPool.packets[i].info.direction=1;
    NMRAPacketPool.packets[i].info.func=0;
    NMRAPacketPool.packets[i].info.nro_f=0;
    for (j=0; j<8; j++)
      NMRAPacketPool.packets[i].info.f[j]=0;
  }
  NMRAPacketPool.NrOfKnownAdresses = 0;
  isNMRAPackedPoolInitialized=True;
  MutexOp.post(nmra_pktpool_mutex);

  /* put idle packet in packet pool */
  j=translateBitstream2Packetstream(idle_packet, idle_pktstr);
  update_NMRAPacketPool(255, idle_pktstr, j, idle_pktstr, j);

  /* generate idle_data */
  if (data->dcc) {
    for (i=0; i<MAXDATA; i++)
      idle_data[i]=idle_pktstr[i % j];
    for (i=(MAXDATA/j)*j; i<MAXDATA; i++)
      idle_data[i]=0xC6;
  }
  memcpy(NMRA_idle_data,idle_pktstr,j);

  return 0;
}

tLocoInfo *get_NMRAPacketPool_LocoInfo(int addr) {
  return &(NMRAPacketPool.packets[addr].info);
}

void update_NMRAPacketPool_LocoInfo(char *protocol, int addr, int direction,
                                    int speed, int speed_max, int func,
                                    int nro_f, int f1, int f2, int f3,
                                    int f4, int f5, int f6, int f7, int f8) {
  speed=abs(speed);

  strncpy(NMRAPacketPool.packets[addr].info.protocol,protocol,2);
  NMRAPacketPool.packets[addr].info.speed=speed;
  NMRAPacketPool.packets[addr].info.speed_max=speed_max;
  NMRAPacketPool.packets[addr].info.direction=direction;
  NMRAPacketPool.packets[addr].info.func=func;
  NMRAPacketPool.packets[addr].info.nro_f=nro_f;
  NMRAPacketPool.packets[addr].info.f[0]=f1;
  NMRAPacketPool.packets[addr].info.f[1]=f2;
  NMRAPacketPool.packets[addr].info.f[2]=f3;
  NMRAPacketPool.packets[addr].info.f[3]=f4;
  NMRAPacketPool.packets[addr].info.f[4]=f5;
  NMRAPacketPool.packets[addr].info.f[5]=f6;
  NMRAPacketPool.packets[addr].info.f[6]=f7;
  NMRAPacketPool.packets[addr].info.f[7]=f8;

}

void update_NMRAPacketPool(int adr, char *packet, int packet_size,
                           char *fx_packet, int fx_packet_size) {

  int i, found;

  found = 0;
  for (i=0; i<=NMRAPacketPool.NrOfKnownAdresses && !found; i++)
    if (NMRAPacketPool.knownAdresses[i]==adr)
      found=True;

  MutexOp.wait(nmra_pktpool_mutex);
  memcpy(NMRAPacketPool.packets[adr].packet,packet,packet_size);
  NMRAPacketPool.packets[adr].packet_size=packet_size;
  memcpy(NMRAPacketPool.packets[adr].fx_packet,fx_packet,fx_packet_size);
  NMRAPacketPool.packets[adr].fx_packet_size=fx_packet_size;
  MutexOp.post(nmra_pktpool_mutex);

  if (NMRAPacketPool.NrOfKnownAdresses==1 &&
      NMRAPacketPool.knownAdresses[0]==255) {
    NMRAPacketPool.NrOfKnownAdresses=0;
  }
  if (!found) {
    NMRAPacketPool.knownAdresses[NMRAPacketPool.NrOfKnownAdresses]=adr;
    NMRAPacketPool.NrOfKnownAdresses++;
  }
}

/**********************************************************/

/**********************************************************/


Boolean isShortcut(iOSerial serial, int shortcutchecking, int shortcutdelay, int inversedsr, Boolean* scdetected, unsigned long* scdelay ) {
  if ( shortcutchecking && ( SerialOp.isDSR(serial) && !inversedsr ) ) {
    /* the system tick runs at 10ms, but is system dependent... */
    TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "shortcut detected" );

    if( *scdetected && (SystemOp.getTick() - *scdelay) > (shortcutdelay/10) ) {
      *scdelay = 0;
      *scdetected = False;
      TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "shortcut detected!" );
      return True;
    } else if(!*scdetected) {
      TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "shortcut timer started [%dms]", shortcutdelay );
      *scdelay = SystemOp.getTick();
      *scdetected = True;
    }
  } else {
    *scdelay = 0;
    *scdetected = False;
  }

  return False;
}

/* arguments for marklin loco decoders (19200baud) */
static int bfr19K = 1025;
static int btw19K = 1250;
static int end19K = 1700;

/* arguments for marklin solenoids/func decoders (38400baud) */
static int bfr38K = 600;
static int btw38K = 600;
static int end38K = 800;


Boolean send_packet(iOSerial serial, int addr, char *packet, int packet_size, int packet_type, int refresh) {

  int i,j,laps;
  int remaining;
  char Temp_packet[20];

  if( packet_size == 0 || packet == NULL ) {
    return False;
  }
  remaining=SerialOp.getWaiting( serial );
  //    TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "I%d",remaining);
  switch (packet_type) {
  case QM1LOCOPKT:
  case QM2LOCOPKT:
    //        TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "QMXLOCOPKT" );
    while( !SerialOp.isUartEmpty( serial, True ) )
      ;
    SerialOp.waitMM(serial,bfr19K+5000,bfr19K);
    SerialOp.setSerialMode(serial,mm);
    if (refresh)
      laps=2;
    else
      laps=4;
    for (i=0; i<laps; i++) {
      if(!SerialOp.write(serial,packet,packet_size))
        return False;
      SerialOp.waitMM(serial,btw19K+(packet_size*208),btw19K);
      if(!SerialOp.write(serial,packet,packet_size))
        return False;
      SerialOp.waitMM(serial,end19K+(packet_size*208),end19K);
    }
    break;
  case QM2FXPKT:
    //        TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "QM2FXPKT" );
    while( !SerialOp.isUartEmpty( serial, True ) )
      ;
    SerialOp.waitMM(serial,bfr19K+5000,bfr19K);
    SerialOp.setSerialMode(serial,mm);
    if (refresh)
      laps=2;
    else
      laps=3;
    for (i=0; i<laps; i++) {
      if(!SerialOp.write(serial,packet,packet_size))
        return False;
      SerialOp.waitMM(serial,btw19K+(packet_size*208),btw19K);
      if(!SerialOp.write(serial,packet,packet_size))
        return False;
      SerialOp.waitMM(serial,end19K+(packet_size*208),end19K);
    }
    break;
  case QM1SOLEPKT:
  case QM1FUNCPKT:
    //         TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "QM1FUNCPKT" );
    memset(Temp_packet, 0, sizeof(Temp_packet));
    for (j =0; j < 9; j++) {
      switch (packet[j]) {
      case LO_38K:
        Temp_packet[j*2] = LO_115K;
        Temp_packet[j*2 + 1] = LO_115K;
        break;
      case HI_38K:
        Temp_packet[j*2] = HI_115K;
        Temp_packet[j*2 + 1] = HI_115K;
        break;
      case OP_38K:
        Temp_packet[j*2] = HI_115K;
        Temp_packet[j*2 + 1] = LO_115K;
        break;
      default:
        TraceOp.trc( __FILE__, TRCLEVEL_ERROR, __LINE__, 9999, "Error creating MMA 115k packet");
      }
    }
    /*Hack! Trying to support as many varities of MMA decoders as possible. *
    *First send command pairs with improved timing and then send them again *
    *with standard timing. Increase pause within repeat loop.               */
    while( !SerialOp.isUartEmpty( serial, True ) )
      ;
    SerialOp.waitMM(serial,bfr38K+5000,bfr38K);
    SerialOp.setSerialMode(serial,mma);
    for (i=0; i<3; i++) {
      if(!SerialOp.write(serial,Temp_packet,packet_size*2))
        return False;
      SerialOp.waitMM(serial,btw38K+(packet_size*208+i*50),btw38K+i*50);
      if(!SerialOp.write(serial,Temp_packet,packet_size*2))
        return False;
      SerialOp.waitMM(serial,end38K+i*100+(packet_size*208),end38K+i*100);
    }
    
    SerialOp.setSerialMode(serial,mm);
    for (i=0; i<3; i++) {
      if(!SerialOp.write(serial,packet,packet_size))
        return False;
      SerialOp.waitMM(serial,btw38K+i*50+(packet_size*208),btw38K+i*50);
      if(!SerialOp.write(serial,packet,packet_size))
        return False;
      SerialOp.waitMM(serial,end38K+i*200+(packet_size*208),end38K+i*200);
    }
    
    break;
  case QNBACCPKT:
  case QNBLOCOPKT:
    //         TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "QNBLOCOPKT" );
    SerialOp.setSerialMode(serial,dcc);
    if(!SerialOp.write(serial,packet,packet_size))
      return False;
    if(!SerialOp.write(serial,NMRA_idle_data,13))
      return False;
    if(!SerialOp.write(serial,packet,packet_size))
      return False;
    if(!SerialOp.write(serial,NMRA_idle_data,13))
      return False;
    /* Each byte takes 10/19200 seconds (0.52ms) to send (start bit + 8 bits + stop bit)/baud rate*/
    remaining=SerialOp.getWaiting( serial );
    //        TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "A%d",remaining);
    if (remaining>2 ) {
      /* If directIO, wait sligthly less than remaining*0,52ms
       * else wait sligthly more than remaining*0,52ms*/
      ThreadOp.sleep(remaining*502/1000-1);
    }
    //        remaining=SerialOp.getWaiting( serial );
    //if (remaining==0)
    //  TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "B%d",remaining);
    break;
  }
  return True;
}

int refresh_loco(iOSerial serial, locorefreshdata* locorefresh) {

  int adr;
  int rc = 0;

  if (locorefresh->mm_locorefresh && (locorefresh->maerklin_refresh || !locorefresh->dcc_locorefresh)) {
    adr = MaerklinPacketPool.knownAdresses[locorefresh->last_refreshed_loco];
    if (locorefresh->last_refreshed_fx<0)
      rc = send_packet(serial, adr,
                       MaerklinPacketPool.packets[adr].packet,
                       18,QM2LOCOPKT,True);
    else
      rc = send_packet(serial, adr,
                       MaerklinPacketPool.packets[adr].f_packets[locorefresh->last_refreshed_fx],
                       18,QM2FXPKT,True);
    locorefresh->last_refreshed_fx++;
    if (locorefresh->last_refreshed_fx==4) {
      locorefresh->last_refreshed_fx=-1;
      locorefresh->last_refreshed_loco++;
      if (locorefresh->last_refreshed_loco>=MaerklinPacketPool.NrOfKnownAdresses) {
        locorefresh->last_refreshed_loco=0;
      }
    }
  }
  if (locorefresh->dcc_locorefresh && (!locorefresh->maerklin_refresh || !locorefresh->mm_locorefresh)) {
    adr = NMRAPacketPool.knownAdresses[locorefresh->last_refreshed_nmra_loco];
    if (adr>=0) {
      if (locorefresh->nmra_fx_refresh<0) {
        rc = send_packet(serial, adr,NMRAPacketPool.packets[adr].packet,
                         NMRAPacketPool.packets[adr].packet_size,QNBLOCOPKT,True);
        locorefresh->nmra_fx_refresh=0;
      } else {
        rc = send_packet(serial, adr,NMRAPacketPool.packets[adr].fx_packet,
                         NMRAPacketPool.packets[adr].fx_packet_size,QNBLOCOPKT,True);
        locorefresh->nmra_fx_refresh=1;
      }
    }

    if (locorefresh->nmra_fx_refresh==1) {
      locorefresh->last_refreshed_nmra_loco++;
      locorefresh->nmra_fx_refresh=-1;
      if (locorefresh->last_refreshed_nmra_loco>=NMRAPacketPool.NrOfKnownAdresses) {
        locorefresh->last_refreshed_nmra_loco=0;
      }
    }
  }
  locorefresh->maerklin_refresh = !locorefresh->maerklin_refresh;
  //  if( rc <= 0 )
  //    TraceOp.trc( "locpool", TRCLEVEL_EXCEPTION, __LINE__, 9999, "Refresh Loco rc=%d errno=%d\n", rc, errno );
}

void cancel_refresh_cycle(obj inst) {
  iODDXData data = Data((iODDX)inst);
  SerialOp.setDTR(data->serial, False);
  data->powerflag = 0;
  rocrail_ddxStateChanged(inst);
  TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "power off" );
}

void thr_refresh_cycle(void *threadinst) {
  iOThread th = (iOThread)threadinst;
  obj inst = ThreadOp.getParm( th );
  iODDXData data = Data((iODDX)inst);

  int packet_size;
  int packet_type;
  char packet[PKTSIZE];
  int addr;
  int rc;
  Boolean scdetected = False;
  unsigned long scdelay = 0;
  Boolean run = True;
  Boolean pauseTrig = True;
  int remaining;

  locorefreshdata locorefresh;

  locorefresh.last_refreshed_loco      = 0;
  locorefresh.last_refreshed_fx        = -1;
  locorefresh.last_refreshed_nmra_loco = 0;
  locorefresh.nmra_fx_refresh          = -1;
  locorefresh.maerklin_refresh         = 0;
  locorefresh.mm_locorefresh           = data->mm;
  locorefresh.dcc_locorefresh          = data->dcc;



  TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "starting refresh cycle..." );

  while( run && !ThreadOp.isQuit(th) ) {
    if( ThreadOp.isPause(th) ) {
      if( !pauseTrig ) {
        pauseTrig = True;
        cancel_refresh_cycle(inst);
      }
      ThreadOp.sleep(100);
      continue;
    } else if(pauseTrig) {
      pauseTrig = False;
      TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "power on" );
      SerialOp.setSerialMode(data->serial,dcc);
      SerialOp.setDTR(data->serial, True);
      SerialOp.setOutputFlow(data->serial, True);
      data->powerflag = 1;
      rocrail_ddxStateChanged(inst);
      ThreadOp.sleep(50);
    }

    //    TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "get queue package..." );
    packet_type = queue_get(&addr,packet,&packet_size);

    //    TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "processing package..." );
    /* now,look at commands */
    if( packet_type > QNOVALIDPKT ) {
      while( packet_type > QNOVALIDPKT ) {
        if ( isShortcut(data->serial,data->shortcutchecking,data->shortcutdelay,data->inversedsr, &scdetected, &scdelay)) {
          cancel_refresh_cycle(inst);
          ThreadOp.pause( th, True );
          break;
        }

        TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "new queue package..." );

        if(!send_packet(data->serial, addr,packet,packet_size,packet_type,False)) {
          TraceOp.trc( __FILE__, TRCLEVEL_EXCEPTION, __LINE__, 9999, "send packet failed! rc=%d errno=%d", rc, errno );
          run = 0;
          break;
        }

        packet_type=queue_get(&addr,packet,&packet_size);
      }
    } else {                          /* no commands? Then we do a refresh */
      if( isShortcut(data->serial,data->shortcutchecking,data->shortcutdelay,data->inversedsr, &scdetected, &scdelay) ) {
        cancel_refresh_cycle(inst);
        ThreadOp.pause( th, True );
        TraceOp.trc( __FILE__, TRCLEVEL_WARNING, __LINE__, 9999, "SCD" );
      } else {
        //        TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "refresh loco..." );
        refresh_loco(data->serial,&locorefresh);

        /*Send Idle data, so the this thread can sleep for a while, not to destroing system responsiveness*/
        /*To get a clean data stream we will try to match sleep time to system time slices*/
        if (data->dcc) {
          SerialOp.setSerialMode(data->serial,dcc);
          SerialOp.write(data->serial,idle_data,MAXDATA);
          remaining=SerialOp.getWaiting( data->serial );
          if ( data->queuecheck && remaining>2 ) {
            /* If directIO, wait sligthly less than remaining*0,52ms
             * else wait slightly more than remaining*0,52ms*/
            ThreadOp.sleep(remaining*502/1000-1);
          }
        } else {
          SerialOp.setSerialMode(data->serial,mm);
          SerialOp.write(data->serial,idle_data,MAXDATA);
          remaining=SerialOp.getWaiting( data->serial );
          if ( data->queuecheck && remaining>5 ) {
            /* If directIO, wait sligthly less than remaining*0,208ms
             * else wait slightly more than remaining*0,208ms*/
            ThreadOp.sleep(remaining*208/1000-1);
          }
        }
        //if (remaining==0)
        //  TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "E2");

      }
    }
  };

  cancel_refresh_cycle(inst);
  close_comport(inst);
  TraceOp.trc( __FILE__, TRCLEVEL_WARNING, __LINE__, 9999, "refresh thread stopped." );
  return;
}

