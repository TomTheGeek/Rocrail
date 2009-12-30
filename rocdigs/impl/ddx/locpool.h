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
/* file: cycles.h                                              */
/* job : exports the functions from cycles.c                   */
/*                                                             */
/* Torsten Vogt, march 1999                                    */
/*                                                             */
/* last changes: Torsten Vogt, march 2000                      */
/*                                                             */
/***************************************************************/

#ifndef __LOCPOOL_H__
#define __LOCPOOL_H__

typedef struct tLocoInfo {
  char        protocol[3];    /* possible values: "M1", .. "PS"              */
  int         addr;           /* possible values: "0000" .. "9999"           */
  int         speed;          /* possible values: "000" .. "127"             */
  int         speed_max;
  int         direction;      /* possible values: '0'(backward), '1'(forward)*/
  int         func;           /* state of func                               */
  int         nro_f;          /* number of functions                         */
  int         f[8];           /* state of f1, ... ,f8                        */
}
tLocoInfo;

#define MAX_MARKLIN_ADDRESS 256
#define MAX_NMRA_ADDRESS 10367 /* idle-addr + 127 basic addr's + 10239 long's */

#define ADDR14BIT_OFFSET 128   /* internal offset of the long addresses       */

typedef struct _tMaerklinPacket {
  char      packet[18];
  char      f_packets[4][18];
  tLocoInfo info;
}
tMaerklinPacket;
typedef struct _tMaerklinPacketPool {
  tMaerklinPacket packets[MAX_MARKLIN_ADDRESS+1];
  int             knownAdresses[MAX_MARKLIN_ADDRESS+1];
  int             NrOfKnownAdresses;
}
tMaerklinPacketPool;

typedef struct _tNMRAPacket {
  char      packet[PKTSIZE];
  int       packet_size;
  char      fx_packet[PKTSIZE];
  int       fx_packet_size;
  tLocoInfo info;
}
tNMRAPacket;
typedef struct _tNMRAPacketPool {
  tNMRAPacket     packets[MAX_NMRA_ADDRESS+1];
  int             knownAdresses[MAX_NMRA_ADDRESS+1];
  int             NrOfKnownAdresses;
}
tNMRAPacketPool;

int init_MaerklinPacketPool(obj inst, iONode ddx_ini);
char *get_maerklin_packet(int adr, int fx);
int get_maerklin_direction(int addr);
void update_MaerklinPacketPool(int adr, char *sd_packet, char *f1, char *f2,
                               char *f3, char *f4);
void update_MaerklinPacketPool_LocoInfo(char *protocol, int addr, int direction,
                                        int speed, int speed_max, int func,
                                        int nro_f, int f1, int f2, int f3,
                                        int f4);
tLocoInfo *get_MaerklinPacketPool_LocoInfo(int addr);

int init_NMRAPacketPool(obj inst);
void update_NMRAPacketPool(int adr, char *packet, int packet_size,
                           char *fx_packet, int fx_packet_size);
void update_NMRAPacketPool_LocoInfo(char *protocol, int addr, int direction,
                                    int speed, int speed_max, int func,
                                    int nro_f, int f1, int f2, int f3,
                                    int f4, int f5, int f6, int f7, int f8);
tLocoInfo *get_NMRAPacketPool_LocoInfo(int addr);

void thr_refresh_cycle(void *threadinst);
void cancel_refresh_cycle(obj inst);


int monitor_NrOfMLocos();
int monitor_NrOfNLocos();
int monitor_LastRfrCmd();
char monitor_Cycle();

#endif
