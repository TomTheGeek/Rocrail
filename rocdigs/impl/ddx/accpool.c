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
/* file: stack.c                                               */
/* job : implements routines to handle the requests of         */
/*       information about General Accessory and feeds the     */
/*       info-port with the changes                            */
/*                                                             */
/* Martin Wolf, december 2000                                  */
/*                                                             */
/* last changes: december 2000                                 */
/*                                                             */
/***************************************************************/   

#include "init.h"
#include "accpool.h"

/* declaring the two packet-pools */
tMaerklinGaPacketPool MaerklinGaPacketPool;
tNMRAGaPacketPool     NMRAGaPacketPool;

/* remember, whether the pools are initialized */
static int isMaerklinGaPacketPoolInitialized = 0;
static int isNMRAGaPacketPoolInitialized = 0;

/****************************************************************
* function initMaerklinGaPacketPool                             *
*                                                               *
* purpose: initializes the MaerklinGaPacketPool by setting in   *
*          every packet                                         *
*          - isSet = 0                                          *
*          Setting isMaerklinGaPacketPoolInitialized=1 if done. *                                                     *
* in:      ---                                                  *
* out:     ---                                                  *
*                                                               *
* remarks: implemented MW, 27.12.2000                           *
*                                                               *
****************************************************************/
void initMaerklinGaPacketPool(void) {
	int i,j;

	for (i=0;i<MAX_MAERKLIN_GA;i++) {
	  for (j=0;j<OUTPUT_PER_GA;j++)
		  MaerklinGaPacketPool[i].port[j].isSet = 0;
	}
    
    isMaerklinGaPacketPoolInitialized = 1;
}

/****************************************************************
* function updateMaerklinGaPacketPool                           *
*                                                               *
* purpose: updates the data in MaerklinPacketPool for the port  *
*          "port" of GA number "nr".                            *
*                                                               *
* in:      nr (int): number of the GA [1..MAX_MAERKLIN_GA]      *
*          port (int): port of GA "nr" [0,1]                    *
*          action (int): state of the port of the GA [0,1]      *
*                       0 = deactivated, 1 = activated          *
*          packet (char *): pointer to packet string that       *
*                       performs the action.                    * 
* out:     return value: 1 if update was successfull,           * 
*                        otherwise 0                            *
*                                                               *
* remarks: implemented MW 27/12/2000                            *
*                                                               *
****************************************************************/
int updateMaerklinGaPacketPool(int nr, int port, int action, char *packet) {

	int i;
	
	/* the range-checking is the task of the client */
	/* pool must be initialized before getting any packet */
	if ((nr < 1) || (nr > MAX_MAERKLIN_GA) || (port - (port & 1)) || 
		(action - (action & 1)) || !isMaerklinGaPacketPoolInitialized) 
		return 0;

	/* if no valid packet availible or action changed the state */
	if (!(MaerklinGaPacketPool[nr].port[port].isSet & (action+1)) ||
		(MaerklinGaPacketPool[nr].port[port].state != action)) {
		MaerklinGaPacketPool[nr].port[port].state = action;
        /* store the packet */
		for (i=0;i<9;i++)
			MaerklinGaPacketPool[nr].port[port].packet[action][i] = packet[i];
		/* mark the information as valid */
		MaerklinGaPacketPool[nr].port[port].isSet |= action+1;
#ifdef _DEBUG_
		fprintf(stdout,"MaerklinGaPacketPool[%d][%d] updated to action %d\n",
			nr,port,MaerklinGaPacketPool[nr].port[port].state);
#endif
	}
//MW enh start
	/* save the number of the activated port */
	if (action)
		MaerklinGaPacketPool[nr].lastActivated = port;
//MW enh end
	return 1;
}

/****************************************************************
* function getMaerklinGaPacket                                  *
*                                                               *
* purpose: returns an pointer to the packet string, that        *
*          switches the port "port" of the GA "nr" to the       *
*          desired state "state". If that packet is not         *
*          avalible or the parameters exeed the allowed range,  *
*          the NULL pointer is returned.                        *
*                                                               *
* in:      nr (int): number of the GA [1..MAX_MAERKLIN_GA]      *
*          port (int): port of GA "nr" [0,1]                    *
*          state (int): state of the port of the GA [0,1]       *
*                       0 = deactivated, 1 = activated          *
*          packet (char **): pointer to packet string if        *
*                       avalible, otherwise the NULL pointer    *
* out:     ---                                                  *
*                                                               *
* remarks: implemented MW 27/12/2000                            *
*                                                               *
****************************************************************/
void getMaerklinGaPacket(int nr, int port, int state, char *(*packet)) {

	*packet = NULL;

	/* the range-checking is the task of the client */
	/* pool must be initialized before getting any packet */
	if ((nr < 1) || (nr > MAX_MAERKLIN_GA) || (port - (port & 1)) || 
		(state - (state & 1)) || !isMaerklinGaPacketPoolInitialized) 
		return;

	/* if valid information availible, return the pointer to it */
	/* else return NULL */
	if ((MaerklinGaPacketPool[nr].port[port].isSet & (state+1))>0)
		*packet = MaerklinGaPacketPool[nr].port[port].packet[state];
	return;
}
  

/****************************************************************
* function initNMRAGaPacketPool                                 *
*                                                               *
* purpose: initializes the NMRAGaPacketPool by setting in       *
*          every packet                                         *
*          - isSet = 0                                          *
*          Setting isNMRAGaPacketPoolInitialized=1 if done.     *                                                     *
*                                                               *
* in:      ---                                                  *
* out:     ---                                                  *
*                                                               *
* remarks: implemented MW, 27.12.2000                           *
*                                                               *
****************************************************************/
void initNMRAGaPacketPool(void) {
	int i,j;

	for (i=0;i<MAX_NMRA_GA;i++) {
		for (j=0;j<OUTPUT_PER_GA;j++)
			NMRAGaPacketPool[i].port[j].isSet = 0;
	}

    isNMRAGaPacketPoolInitialized = 1;
}

/****************************************************************
* function updateNMRAGaPacketPool                               *
*                                                               *
* purpose: updates the data in NMRAPacketPool for the port      *
*          "port" of GA number "nr".                            *
*                                                               *
* in:      nr (int): number of the GA [1..MAX_NMRA_GA]          *
*          port (int): port of GA "nr" [0,1]                    *
*          action (int): state of the port of the GA [0,1]      *
*                       0 = deactivated, 1 = activated          *
*          packet (char *): pointer to packet string that       *
*                       performs the action.                    * 
*          packetLength (char): length of data in packet        *
* out:     return value: 1 if update was successfull,           * 
*                        otherwise 0                            *
*                                                               *
* remarks: implemented MW 27/12/2000                            *
*                                                               *
****************************************************************/
int updateNMRAGaPacketPool(int nr, int port, int action, char *packet, char packetLength) { 

	int i;
	
	/* the range-checking is the task of the client */
	/* pool must be initialized before getting any packet */
	if ((nr < 1) || (nr > MAX_NMRA_GA) || (port - (port & 1)) || 
		(action - (action & 1)) || !isNMRAGaPacketPoolInitialized) {
#ifdef _DEBUG_
		fprintf(stdout,"NMRAGaPacketPool[%d][%d] update failed.\n",
			nr,port);
#endif
		return 0;
	}

	if ((!(NMRAGaPacketPool[nr].port[port].isSet & (action+1))) ||
		(NMRAGaPacketPool[nr].port[port].state != action)) {
		NMRAGaPacketPool[nr].port[port].state = action;
		for (i=0;i<=packetLength;i++)
			NMRAGaPacketPool[nr].port[port].packet[action][i] = packet[i];
        NMRAGaPacketPool[nr].port[port].packetLength[action]=packetLength;
		NMRAGaPacketPool[nr].port[port].isSet |= action+1;
#ifdef _DEBUG_
		fprintf(stdout,"NMRAGaPacketPool[%d][%d] updated to action %d\n",
			nr,port,NMRAGaPacketPool[nr].port[port].state);
		fprintf(stdout,"NMRAGaPacketPool[%d][%d].isSet = %d\n",
			nr,port,NMRAGaPacketPool[nr].port[port].isSet);
#endif
	}
//MW enh start
	/* save the number of the activated port */
	if (action)
		NMRAGaPacketPool[nr].lastActivated = port;
//MW enh end
	return 1;

}

/****************************************************************
* function getNMRAGaPacket                                      *
*                                                               *
* purpose: returns an pointer to the packet string, that        *
*          switches the port "port" of the GA "nr" to the       *
*          desired state "state". If that packet is not         *
*          avalible or the parameters exeed the allowed range,  *
*          the NULL pointer is returned.                        *
*                                                               *
* in:      nr (int): number of the GA [1..MAX_NMRA_GA]          *
*          port (int): port of GA "nr" [0,1]                    *
*          state (int): state of the port of the GA [0,1]       *
*                       0 = deactivated, 1 = activated          *
*          packet (char **): pointer to packet string if        *
*                       avalible, otherwise the NULL pointer    *
* out:     return value: Length of the packet, 0 if packet not  *
*                        avalible                               *
*                                                               *
* remarks: implemented MW 27/12/2000                            *
*                                                               *
****************************************************************/
int getNMRAGaPacket(int nr, int port, int state, char *(*packet)) {

	*packet = NULL;

	/* the range-checking is the task of the client */
	/* pool must be initialized before getting any packet */
	if ((nr < 1) || (nr > MAX_NMRA_GA) || (port - (port & 1)) || 
		(state - (state & 1)) || !isNMRAGaPacketPoolInitialized) {
#ifdef _DEBUG_
		fprintf(stdout,"getNMRAGaPacket failed for range (nr:port:state %d:%d:%d)\n",nr,port,state);
#endif
		return 0;
	}

	/* if valid information availible, return the pointer to it */
	/* else return NULL */
	if (NMRAGaPacketPool[nr].port[port].isSet & (state+1)) {
		*packet = NMRAGaPacketPool[nr].port[port].packet[state];
		return NMRAGaPacketPool[nr].port[port].packetLength[state];
	}
	else {
#ifdef _DEBUG_
		fprintf(stdout,"getNMRAGaPacket: no packet avalible (nr:port:state %d:%d:%d)\n",nr,port,state);
#endif
		return 0;
	}
}
