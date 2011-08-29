#ifndef __OPCODES_H
#define __OPCODES_H


/* CBUS definitions for use with C18 compiler
 * These definitions taken from CBUS specifications document rev 7f
 * updated from opcodes.h originally by Andrew Crosland.
 * Pete Brownlow  6/4/11
 * Roger Healey	  6/4/11 - Add OPC_CMDERR response codes
 * Pete Brownlow  7/6/11 - Updated to spec ver 7f and add new module ids
 */



// Manufacturers

#define MANU_MERG		 165
// Module types
#define MTYP_SLIM        0  // default NN for SLiM nodes
#define MTYP_CANACC4     1	// Solenoid point driver
#define MTYP_CANACC5     2	// Motorised point driver
#define MTYP_CANACC8     3	// 8 digital outputs
#define MTYP_CANACE3     4	// Control panel switch/button encoder
#define MTYP_CANACE8C    5	// 8 digital inputs
#define MTYP_CANLED      6	// 64 led driver
#define MTYP_CANLED64    7	// 64 led driver (multi leds per event)
#define MTYP_CANACC4_2   8  // 12v version of CANACC4
#define MTYP_CANCAB      9  // CANCAB hand throttle
#define MTYP_CANCMD      10 // CANCMD command station
#define	MTYP_CANSERVO	   11	// 8 servo driver (on canacc8 or similar hardware)
#define MTYP_CANBC	 	   12 // BC1a command station
#define MTYP_CANRPI		   13	// RPI and RFID interface
#define MTYP_CANTTCA	   14	// Turntable controller (turntable end)
#define MTYP_CANTTCB	   15 // Turntable controller (control panel end)


#define MANU_ROCRAIL 70
// Module types
#define MTYP_CANGC2 2 // 16 I/O


//  CBUS opcodes list
//  Ref: CBUS Specifications dcoument

// Packets with no data bytes

#define OPC_ACK     0x00    // General ack
#define OPC_NAK     0x01    // General nak
#define OPC_HLT     0x02    // Bus Halt
#define OPC_BON     0x03    // Bus on
#define OPC_TOF     0x04    // Track off
#define OPC_TON     0x05    // Track on
#define OPC_ESTOP   0x06    // Track stopped
#define OPC_ARST    0x07    // System reset
#define OPC_RTOF    0x08    // Rest track off
#define OPC_RTON    0x09    // Rest track on
#define OPC_RESTP   0x0a    // Rest emergency stop all
#define OPC_RVER    0x0b    // Rest firmware version
#define OPC_RSTAT   0x0c    // Rest node status
#define OPC_RQNN    0x0d    // Rest node number
#define OPC_RQNP    0x10    // Read node parameters

// Packets with 1 data byte

#define OPC_STAT    0x20    // Command station status report
#define OPC_KLOC    0x21    // Release engine by handle
#define OPC_QLOC    0x22    // Query engine by handle
#define OPC_KEEP    0x23    // Keep alive for cab

#define OPC_DBG1    0x30    // Debug message with 1 status byte
#define OPC_EXTC    0x3F    // Extended opcode

// Packets with 2 data bytes

#define OPC_RLOC    0x40    // Rest session for loco
#define OPC_QCON    0x41    // Query consist
#define OPC_SNN     0x42    // Set node number
#define OPC_STMOD   0x44    // Set Throttlle mode
#define OPC_PCON    0x45    // Consist loco
#define OPC_KCON    0x46    // De-consist loco
#define OPC_DSPD    0x47    // Loco speed/dir
#define OPC_DFLG    0x48    // Set engine flags
#define OPC_FNON    0x49    // Set engine function on by number
#define OPC_FNOF    0x4A    // Set engine function off by number
#define OPC_SSTAT   0x4C    // Service mode status

#define OPC_NNACK	0x50    // Node number acknowledge
#define OPC_NNREL	0x51    // Node number release
#define OPC_NNREF	0x52    // Node keepalive
#define OPC_NNLRN	0x53    // Set learn mode
#define OPC_NNULN	0x54    // Release learn mode
#define OPC_NNCLR	0x55    // Clear all events
#define OPC_NNEVN	0x56    // Read available event slots
#define OPC_NERD    0x57    // Read all stored events
#define OPC_RQEVN   0x58    // Read number of stored events
#define OPC_WRACK	0x59	// Write acknowledge
#define OPC_RQDAT	0x5A	// Request node data event
#define OPC_BOOT	0x5C    // Put node into boot mode
#define OPC_EXTC1   0x5F    // Extended opcode with 1 data byte

// Packets with 3 data bytes

#define OPC_DFUN    0x60    // Set engine functions
#define OPC_ERR     0x63    // Command station error
#define OPC_CMDERR  0x6F    // Errors from nodes during config

#define OPC_EVNLF	0x70    // Event slots left response
#define OPC_NVRD	0x71    // Rest read of node variable
#define OPC_NENRD   0x72    // Reqeust read stored event by index
#define OPC_RQNPN	0x73	// Rest read module paramters
#define OPC_NUMEV   0x74    // Number of events stored response
#define OPC_EXTC2   0x7F    // Extended opcode with 2 data bytes

// Packets with 4 data bytes

#define OPC_RDCC3   0x80    // 3 byte DCC packet
#define OPC_WCVO    0x82    // Write CV byte Ops mode by handle
#define OPC_WCVB    0x83    // Write CV bit Ops mode by handle
#define OPC_QCVS    0x84    // Read CV
#define OPC_PCVS    0x85    // Report CV

#define OPC_ACON	0x90	// on event
#define OPC_ACOF	0x91	// off event
#define OPC_AREQ	0x92    // Accessory rest event
#define OPC_ARSPO   0x93    // Accessory response event on
#define OPC_ARSPN   0x94    // Accessory response event off
#define OPC_EVULN	0x95    // Unlearn event
#define OPC_NVSET	0x96    // Set a node variable
#define OPC_NVANS	0x97    // Node variable value response
#define OPC_ASON	0x98    // Short event on
#define OPC_ASOF	0x99    // Short event off
#define OPC_ASRQ	0x9A    // Short rest event
#define OPC_PARAN   0x9B    // Single node parameter response
#define OPC_REVAL   0x9C    // Rest read of event variable
#define OPC_EXTC3   0x9F    // Extended opcode with 3 data bytes

// Packets with 5 data bytes

#define OPC_RDCC4   0xA0    // 4 byte DCC packet
#define OPC_WCVS    0xA2    // Write CV service mode

#define OPC_ACON1   0xB0    // On event with one data byte
#define OPC_ACOF1   0xB1    // Off event with one data byte
#define OPC_REQEV	0xB2    // Read event variable in learn mode
#define OPC_ARSP1O  0xB4    // Accessory on response
#define OPC_NEVAL   0xB5    // Event variable by index read response
#define OPC_ARSP1N  0xB6    // Accessory off response
#define OPC_EXTC4   0xBF    // Extended opcode with 4 data bytes

// Packets with 6 data bytes

#define OPC_RDCC5   0xC0    // 5 byte DCC packet
#define OPC_WCVOA   0xC1    // Write CV ops mode by address

#define OPC_ACON2   0xD0    // On event with two data bytes
#define OPC_ACOF2   0xD1    // Off event with two data bytes
#define OPC_EVLRN	0xd2    // Teach event
#define OPC_EVANS	0xd3    // Event variable read response in learn mode
#define OPC_ARSP2O  0xD4    // Accessory on response
#define OPC_ARSP2N  0xD5    // Accessory off response
#define OPC_EXTC5   0xBF    // Extended opcode with 5 data bytes

// Packets with 7 data bytes

#define OPC_RDCC6  	0xE0    // 6 byte DCC packets
#define OPC_PLOC	0xE1    // Loco session report
#define OPC_ACDAT   0xE3    // Accessory data event: 5 bytes of node data (eg: RFID)
#define OPC_ARDAT	0xE4	// Accessory data response 
#define OPC_PARAMS	0xEF    // Node parameters response

#define OPC_ACON3   0xF0    // On event with 3 data bytes
#define OPC_ACOF3   0xF1    // Off event with 3 data bytes
#define OPC_ENRSP	0xF2    // Read node events response
#define OPC_ARSP3O  0xF3    // Accessory on response
#define OPC_ARSP3N  0xF4    // Accessory off response
#define OPC_EVLRNI	0xF5    // Teach event using event indexing

// Modes for STMOD

#define TMOD_SPD_MASK   	3
#define TMOD_SPD_128    	0
#define TMOD_SPD_14     	1
#define TMOD_SPD_28I    	2
#define TMOD_SPD_28     	3

// Error codes for OPC_ERR

#define ERR_LOCO_STACK_FULL     	1
#define ERR_LOCO_ADDR_TAKEN     	2
#define ERR_SESSION_NOT_PRESENT 	3
#define ERR_NO_MORE_ENGINES     	4
#define ERR_ENGINE_NOT_FOUND    	5

// Status codes for OPC_SSTAT

#define SSTAT_NO_ACK    	1
#define SSTAT_OVLD      	2
#define SSTAT_WR_ACK    	3
#define SSTAT_BUSY      	4
#define SSTAT_CV_ERROR  	5

// Error codes for OPC_CMDERR

#define CMDERR_INV_CMD			1
#define CMDERR_NOT_LRN			2
#define CMDERR_NOT_SETUP		3
#define CMDERR_TOO_MANY_EVS 	4
#define CMDERR_NO_EV			5
#define CMDERR_INV_EV_IDX		6
#define CMDERR_NO_EVENTS		7
#define CMDERR_INV_EN_IDX		8
#define CMDERR_INV_PARAM_IDX	9

#define CVMODE_DIRECTBYTE 0
#define CVMODE_DIRECTBIT 1
#define CVMODE_PAGE 2
#define CVMODE_REGISTER 3
#define CVMODE_ADDRONLY 4

// Boot mode codes in extended frames.
#define BOOTCMD_NOP 0
#define BOOTCMD_RESET 1
#define BOOTCMD_INIT 2
#define BOOTCMD_CHECK 3
#define BOOTCMD_TEST 4

#define BOOTMSG_ERR 0
#define BOOTMSG_OK 1
#define BOOTMSG_ACK 2 // Response to the test command.

#endif		// __OPCODES_H


