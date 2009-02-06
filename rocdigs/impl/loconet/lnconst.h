/**
 * Description:		Constants to represent values seen in LocoNet traffic
 * @author			Bob Jacobsen   Copyright (C) 2001
 * @version $Revision: 1.7 $

 * Note that the values in this class have been taken from the llnmom C program of
 * Ron W. Auld, which included some work of John Kabat.  The symbol names
 * are copied from the loconet.h file, CVS revision 1.1.1.1, program release 0.3.0  Those
 * parts are (C) Copyright 2001 Ron W. Auld, and are used with direct
 * permission of the copyright holder.
 * <P>
 * Most major comment blocks here are quotes from the Digitrax Loconet(r)
 * OPCODE SUMMARY: found in the Loconet(r) Personal Edition 1.
 * <P>
 * Al Silverstein provided the reverse-engineering effort for the
 * OPC_MULTI_SENSE message.
 * <P>
 * Some of the message formats used in this class are Copyright Digitrax, Inc.
 * and used with permission as part of the JMRI project.  That permission
 * does not extend to uses in other software products.  If you wish to
 * use this code, algorithm or these message formats outside of JMRI, please
 * contact Digitrax Inc for separate permission.
 */
/* various bit masks */
#define DIRF_DIR          0x20  /* direction bit    */
#define DIRF_F0           0x10  /* Function 0 bit   */
#define DIRF_F4           0x08  /* Function 1 bit   */
#define DIRF_F3           0x04  /* Function 2 bit   */
#define DIRF_F2           0x02  /* Function 3 bit   */
#define DIRF_F1           0x01  /* Function 4 bit   */
#define SND_F8            0x08  /* Sound 4/Function 8 bit */
#define SND_F7            0x04  /* Sound 3/Function 7 bit */
#define SND_F6            0x02  /* Sound 2/Function 6 bit */
#define SND_F5            0x01  /* Sound 1/Function 5 bit */

#define CONFIG_SLOT       127   /* system config slot */


#define OPC_SW_ACK_CLOSED 0x20  /* command switch closed/open bit   */
#define OPC_SW_ACK_OUTPUT 0x10  /* command switch output on/off bit */

#define OPC_INPUT_REP_CB  0x40  /* control bit, reserved otherwise      */
#define OPC_INPUT_REP_SW  0x20  /* input is switch input, aux otherwise */
#define OPC_INPUT_REP_HI  0x10  /* input is HI, LO otherwise            */

#define OPC_SW_REP_SW     0x20  /* switch input, aux input otherwise    */
#define OPC_SW_REP_HI     0x10  /* input is HI, LO otherwise            */
#define OPC_SW_REP_CLOSED 0x20  /* 'Closed' line is ON, OFF otherwise   */
#define OPC_SW_REP_THROWN 0x10  /* 'Thrown' line is ON, OFF otherwise   */
#define OPC_SW_REP_INPUTS 0x40  /* sensor inputs, outputs otherwise     */

#define OPC_SW_REQ_DIR    0x20  /* switch direction - closed/thrown     */
#define OPC_SW_REQ_OUT    0x10  /* output On/Off                        */

#define OPC_LOCO_SPD_ESTOP 0x01 /* emergency stop command               */

#define OPC_MULTI_SENSE_MSG     0x60 // byte 1
#define OPC_MULTI_SENSE_PRESENT 0x20 // MSG field: transponder seen
#define OPC_MULTI_SENSE_ABSENT  0x00 // MSG field: transponder lost
#define OPC_MULTI_SENSE_POWER   0x60 // MSG field: Power message

/* Slot Status byte definitions and macros */
/***********************************************************************************
*   D7-SL_SPURGE     1=SLOT purge en,                                             *
*                    ALSO adrSEL (INTERNAL use only) (not seen on NET!)           *
*                                                                                  *
*   D6-SL_CONUP      CONDN/CONUP: bit encoding-Control double linked Consist List *
*                       11=LOGICAL MID CONSIST , Linked up AND down               *
*                       10=LOGICAL CONSIST TOP, Only linked downwards             *
*                       01=LOGICAL CONSIST SUB-MEMBER, Only linked upwards        *
*                       00=FREE locomotive, no CONSIST indirection/linking        *
*                    ALLOWS "CONSISTS of CONSISTS". Uplinked means that           *
*                    Slot SPD number is now SLOT adr of SPD/DIR and STATUS        *
*                    of consist. i.e. is an Indirect pointer. This Slot          *
*                    has same BUSY/ACTIVE bits as TOP of Consist. TOP is          *
*                    loco with SPD/DIR for whole consist. (top of list).          *
*                    BUSY/ACTIVE: bit encoding for SLOT activity                  *
*                                                                                  *
*   D5-SL_BUSY       11=IN_USE loco adr in SLOT -REFRESHED                        *
*                                                                                  *
*   D4-SL_ACTIVE     10=IDLE loco adr in SLOT -NOT refreshed                      *
*                    01=COMMON loco adr IN SLOT -refreshed                        *
*                    00=FREE SLOT, no valid DATA -not refreshed                   *
*                                                                                  *
*   D3-SL_CONDN      shows other SLOT Consist linked INTO this slot, see SL_CONUP *
*                                                                                  *
*   D2-SL_SPDEX      3 BITS for Decoder TYPE encoding for this SLOT               *
*                                                                                  *
*   D1-SL_SPD14      011=send 128 speed mode packets                              *
*                                                                                  *
*   D0-SL_SPD28      010=14 step MODE                                             *
*                    001=28 step. Generate Trinary packets for this               *
*                                 Mobile ADR                                      *
*                    000=28 step. 3 BYTE PKT regular mode                         *
*                    111=128 Step decoder, Allow Advanced DCC consisting          *
*                    100=28 Step decoder ,Allow Advanced DCC consisting           *
***********************************************************************************/

#define STAT1_SL_SPURGE   0x80  /* internal use only, not seen on net */
#define STAT1_SL_CONUP    0x40  /* consist status                     */
#define STAT1_SL_BUSY     0x20  /* used with STAT1_SL_ACTIVE,         */
#define STAT1_SL_ACTIVE   0x10  /*                                    */
#define STAT1_SL_CONDN    0x08  /*                                    */
#define STAT1_SL_SPDEX    0x04  /*                                    */
#define STAT1_SL_SPD14    0x02  /*                                    */
#define STAT1_SL_SPD28    0x01  /*                                    */
#define STAT2_SL_SUPPRESS 0x01  /* 1 Adv. Consisting supressed      */
#define STAT2_SL_NOT_ID   0x04  /* 1 ID1/ID2 is not ID usage        */
#define STAT2_SL_NOTENCOD 0x08  /* 1 ID1/ID2 is not encoded alias   */
#define STAT2_ALIAS_MASK  (STAT2_SL_NOTENCOD | STAT2_SL_NOT_ID)
#define STAT2_ID_IS_ALIAS STAT2_SL_NOT_ID

/* mask and values for consist determination */
#define CONSIST_MASK      (STAT1_SL_CONDN | STAT1_SL_CONUP)
#define CONSIST_MID       (STAT1_SL_CONDN | STAT1_SL_CONUP)
#define CONSIST_TOP       STAT1_SL_CONDN
#define CONSIST_SUB       STAT1_SL_CONUP
#define CONSIST_NO        0

/* mask and values for locomotive use determination */
#define LOCOSTAT_MASK     (STAT1_SL_BUSY  | STAT1_SL_ACTIVE)
#define LOCO_IN_USE       (STAT1_SL_BUSY  | STAT1_SL_ACTIVE)
#define LOCO_IDLE         STAT1_SL_BUSY
#define LOCO_COMMON       STAT1_SL_ACTIVE
#define LOCO_FREE         0

/* mask and values for decoder type encoding for this slot */
#define DEC_MODE_MASK     (STAT1_SL_SPDEX | STAT1_SL_SPD14 | STAT1_SL_SPD28)
/* Advanced consisting allowed for the next two */
#define DEC_MODE_128A     (STAT1_SL_SPDEX | STAT1_SL_SPD14 | STAT1_SL_SPD28)
#define DEC_MODE_28A      STAT1_SL_SPDEX
/* normal modes */
#define DEC_MODE_128      (STAT1_SL_SPD14 | STAT1_SL_SPD28)
#define DEC_MODE_14       STAT1_SL_SPD14
#define DEC_MODE_28TRI    STAT1_SL_SPD28
#define DEC_MODE_28       0

/* values for track status encoding for this slot */
#define GTRK_PROG_BUSY    0x08      /* 1 programming track in this master is Busy         */
#define GTRK_MLOK1        0x04      /* 0 Master is DT200, 1=Master implements LocoNet 1.1 */
#define GTRK_IDLE         0x02      /* 0=TRACK is PAUSED, B'cast EMERG STOP.                */
#define GTRK_POWER        0x01      /* 1=DCC packets are ON in MASTER, Global POWER up      */

#define FC_SLOT           0x7b      /* Fast clock is in this slot                           */
#define PRG_SLOT          0x7c      /* This slot communicates with the programming track    */

/* values and macros to decode programming messages */
#define PCMD_RW           0x40      /* 1 write, 0 read                                  */
#define PCMD_BYTE_MODE    0x20      /* 1 byte operation, 0 bit operation (if possible)  */
#define PCMD_TY1          0x10      /* TY1 Programming type select bit                      */
#define PCMD_TY0          0x08      /* TY0 Programming type select bit                      */
#define PCMD_OPS_MODE     0x04      /* 1 Ops mode, 0 Service Mode                       */
#define PCMD_RSVRD1       0x02      /* reserved                                             */
#define PCMD_RSVRD0       0x01      /* reserved                                             */

/* programming mode mask */
#define PCMD_MODE_MASK    (PCMD_BYTE_MODE | PCMD_OPS_MODE | PCMD_TY1 | PCMD_TY0)

/*
 *  programming modes
 */
/* Paged mode  byte R/W on Service Track */
#define PAGED_ON_SRVC_TRK       PCMD_BYTE_MODE

/* Direct mode byte R/W on Service Track */
#define DIR_BYTE_ON_SRVC_TRK    (PCMD_BYTE_MODE | PCMD_TY0)

/* Direct mode bit  R/W on Service Track */
#define DIR_BIT_ON_SRVC_TRK     PCMD_TY0

/* Physical Register byte R/W on Service Track */
#define REG_BYTE_RW_ON_SRVC_TRK PCMD_TY1

/* Service Track Reserved function */
#define SRVC_TRK_RESERVED       (PCMD_TY1 | PCMD_TY0)

/* Ops mode byte program - no feedback */
#define OPS_BYTE_NO_FEEDBACK    (PCMD_BYTE_MODE | PCMD_OPS_MODE)

/* Ops mode byte program - feedback */
#define OPS_BYTE_FEEDBACK       (OPS_BYTE_NO_FEEDBACK | PCMD_TY0)

/* Ops mode bit program - no feedback */
#define OPS_BIT_NO_FEEDBACK     PCMD_OPS_MODE

/* Ops mode bit program - feedback */
#define OPS_BIT_FEEDBACK        (OPS_BIT_NO_FEEDBACK | PCMD_TY0)

/* Programmer Status error flags */
#define PSTAT_USER_ABORTED  0x08    /* User aborted this command */
#define PSTAT_READ_FAIL     0x04    /* Failed to detect Read Compare Acknowledge from decoder */
#define PSTAT_WRITE_FAIL    0x02    /* No Write acknowledge from decoder                      */
#define PSTAT_NO_DECODER    0x01    /* Service mode programming track empty                   */

/* bit masks for CVH */
#define CVH_CV8_CV9         0x30    /* mask for CV# bits 8 and 9    */
#define CVH_CV7             0x01    /* mask for CV# bit 7           */
#define CVH_D7              0x02    /* MSbit for data value         */

// The following two are commented out pending some decisions as to (a) whether
// they belong here or in the parser and (b) understanding what they say about
// a data format note use of a pointer dereference

/* build data byte from programmer message */
//#define PROG_DATA(ptr)      (((ptr->cvh & CVH_D7) << 6) | (ptr->data7 & 0x7f))

/* build CV # from programmer message */
//#define PROG_CV_NUM(ptr)    (((((ptr->cvh & CVH_CV8_CV9) >> 3) | (ptr->cvh & CVH_CV7)) * 128)   \
//                            + (ptr->cvl & 0x7f))


/* loconet opcodes */
#define OPC_GPBUSY        0x81
#define OPC_GPOFF         0x82
#define OPC_GPON          0x83
#define OPC_IDLE          0x85
#define OPC_LOCO_SPD      0xa0
#define OPC_LOCO_DIRF     0xa1
#define OPC_LOCO_SND      0xa2
#define OPC_SW_REQ        0xb0
#define OPC_SW_REP        0xb1
#define OPC_INPUT_REP     0xb2
#define OPC_UNKNOWN       0xb3
#define OPC_LONG_ACK      0xb4
#define OPC_SLOT_STAT1    0xb5
#define OPC_CONSIST_FUNC  0xb6
#define OPC_UNLINK_SLOTS  0xb8
#define OPC_LINK_SLOTS    0xb9
#define OPC_MOVE_SLOTS    0xba
#define OPC_RQ_SL_DATA    0xbb
#define OPC_SW_STATE      0xbc
#define OPC_SW_ACK        0xbd
#define OPC_LOCO_ADR      0xbf
#define OPC_MULTI_SENSE   0xd0
#define OPC_LISSY_REP     0xe4
#define OPC_PEER_XFER     0xe5
#define OPC_SL_RD_DATA    0xe7
#define OPC_IMM_PACKET    0xed
#define OPC_IMM_PACKET_2  0xee
#define OPC_WR_SL_DATA    0xef
#define OPC_WR_SL_DATA_EXP 0xee
#define OPC_MASK          0x7f  /* mask for acknowledge opcodes */

// start of values not from llnmon.c

// Expanded slot index values
#define EXP_MAST            0
#define EXP_SLOT            0x01
#define EXPD_LENGTH         16
//offsets into message
#define EXPD_STAT           0
#define EXPD_ADRL           1
#define EXPD_ADRH           2
#define EXPD_FLAGS          3
#define EXPD_SPD            4
#define EXPD_F28F20F12      5
#define EXPD_DIR_F0F4_F1    6
#define EXPD_F11_F5         7
#define EXPD_F19_F13        8
#define EXPD_F27_F21        9

//  opcode keys used to express interest in various messages
//            note these are _not_ the loconet opcode values!

#define KEY_GPBUSY 			1<<  0
#define KEY_GPOFF 			1<<  1
#define KEY_GPON 			1<<  2
#define KEY_IDLE  			1<<  3

#define KEY_LOCO_SPD		1<<  4
#define KEY_LOCO_DIRF		1<<  5
#define KEY_LOCO_SND		1<<  6
#define KEY_SW_REQ			1<<  7

#define KEY_SW_REP			1<<  8
#define KEY_INPUT_REP		1<<  9
#define KEY_LONG_ACK		1<< 10
#define KEY_SLOT_STAT1		1<< 11

#define KEY_CONSIST_FUNC	1<< 12
#define KEY_UNLINK_SLOTS	1<< 13
#define KEY_LINK_SLOTS		1<< 14
#define KEY_MOVE_SLOTS		1<< 15

#define KEY_RQ_SL_DATA		1<< 16
#define KEY_SW_STATE		1<< 17
#define KEY_SW_ACK			1<< 18
#define KEY_LOCO_ADR		1<< 19

#define KEY_PEER_XFR		1<< 20
#define KEY_IMM_PACKET		1<< 21
#define KEY_WR_SL_DATA		1<< 22

#define SENSOR_TYPE_DEFAULT      0
#define SENSOR_TYPE_LISSY        1
#define SENSOR_TYPE_TRANSPONDING 2

