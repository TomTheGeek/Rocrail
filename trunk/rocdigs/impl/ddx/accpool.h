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
/* file: stack.h                                               */
/* job : export routines and types from stack.c                */
/*                                                             */
/* Martin Wolf, december 2000                                  */
/*                                                             */
/* last changes: december 2000                                 */
/*                                                             */
/***************************************************************/   

#ifndef __DDX_ACCPOOL_H__
#define __DDX_ACCPOOL_H__

#include "queue.h"

/* Maximal possible GAs in the Maerklin format */
#define MAX_MAERKLIN_GA  324
/* Maximal possible GAs in the NMWA-DCC format */
#define MAX_NMRA_GA 4096
/* Outputs per GA */
#define OUTPUT_PER_GA 2

/* notes on implementation: because the random access to GA-information (e.g. GET GA, 
   update at SET GA) is much more used as the access to all valid information 
   (login into feedbackport) and there is no refresh-cycle, I decided to notice the 
   validity of information in the
   GaPacket-types and not to store all valid addresses in an seperate array (like
   done for locos in cycle.h) 

   Since also there are only four different packets to handle one GA and Torsten
   told in a discussion calculating them is time-consuming, I decided do save 
   these packets. The comp_... function tries to get a packet. If this was successfull, 
   the contents of that package a sent, otherwise the packet will be calculated, sent and
   saved for later use.

   A problem are the GAs controlled by an impulse in to discrete states, like the most
   turnouts are. To get known of their (possible) position, we will save the port of
   an GA, that was the last activated. (The switch-on decided, since all decoders (I know) 
   switch off the a port of an GA, if the other port is activated.) This implementation 
   enhances the actual SRCP-Version and is discussed at de.rec.modelle.bahn at the moment
   (January 2001). These code-secments are marked with //MW enh.

   Martin Wolf (MW), January 2001
*/

/***************************************************************/
/*          Types and functions for GAs addressed by           */
/*                 Maerklin-Motorola-protocol                  */
/***************************************************************/
/* saving the data of a Maerklin GA Port */
typedef struct _tMaerklinGaPortPacket {
    char    isSet;        /* is the information of this struct valid? */
	                      /* = 0 : no information valid */
	                      /* = 1 : packet[0] und info valid (bit 0 set) */
	                      /* = 2 : packet[1] und info valid (bit 1 set) */
	                      /* = 3 : everything valid (bit 0 and bit 1 set) */
	char    packet[2][9]; /* packet[0] to deactivate, packet[1] to aktivate that port */
    char    state;        /* status of the GA-Port*/
} tMaerklinGaPortPacket;

/* Problem with maerklin-decoders: IMHO there is only one switch-off command for the whole 
   k83 decoder and all self-build copies with the 4051.  */

/* saving the data of a Maerklin GA */
typedef struct _tMaerklinGaPacket {
	tMaerklinGaPortPacket  port[2];
//MW enh start
	char lastActivated; /* nr of the port last activated; information is only valid, if
	                       the activation of the stored port was done in the past 
	                       (isSet&2>0)*/
//MW enh end
} tMaerklinGaPacket;

/* saving the data of all the Maerklin GAs */
typedef tMaerklinGaPacket tMaerklinGaPacketPool[MAX_MAERKLIN_GA];

/* Initialize the packet pool */
void initMaerklinGaPacketPool(void);
/* Update the information */
int updateMaerklinGaPacketPool(int nr, int port, int action, char *packet);
/* Get a Packet */
void getMaerklinGaPacket(int nr, int port, int state, char *(*packet));

/***************************************************************/
/*          Types and functions for GAs addressed by           */
/*                      NMRA-DCC-protocol                      */
/***************************************************************/
/* saving the data of a NMRA GA port */
typedef struct _tNMRAGaPortPacket {
    char    isSet;        /* is the information of this struct valid? */
	                      /* = 0 : no information valid */
	                      /* = 1 : packet[0] und info valid (bit 0 set) */
	                      /* = 2 : packet[1] und info valid (bit 1 set) */
	                      /* = 3 : everything valid (bit 0 and bit 1 set) */
	char    packetLength[2];    /* real length of packets */
	char    packet[2][PKTSIZE]; /* packet[0] to deactivate, packet[1] to activate that port */
	char    state;        /* status of the GA-Port*/
} tNMRAGaPortPacket;

/* saving the data of a NMRA GA */
typedef struct _tNMRAGaPacket {
	tNMRAGaPortPacket  port[2];
//MW enh start
	char lastActivated; /* nr of the port last activated; information is only valid, if
	                       the activation of the stored port was done in the past 
	                       (isSet&2>0)*/
//MW enh end
} tNMRAGaPacket;

/* saving the data of all the NMRA GAs */
typedef tNMRAGaPacket tNMRAGaPacketPool[MAX_NMRA_GA];

/* Initialize the packet pool */
void initNMRAGaPacketPool(void);
/* Update the information */
int updateNMRAGaPacketPool(int nr, int port, int action, char *packet, char packetLength);
/* Get a Packet */
int getNMRAGaPacket(int nr, int port, int state, char *(*packet));


#endif

/****************************************************************
* end of ga_manager.h                                           *
****************************************************************/
