
 /**
 * A utility class for formatting LocoNet packets
 *              into human-readable text.
 * <P>
 * Much of this file is a Java-recoding of the display.c file from the
 * llnmon package of John Jabour.  Some of the conversions involve explicit
 * decoding of structs defined in loconet.h in that same package.  Those
 * parts are (C) Copyright 2001 Ron W. Auld.  Use of these parts is by
 * direct permission of the author.
 * <P>
 * Most major comment blocks here are quotes from the Digitrax Loconet(r)
 * OPCODE SUMMARY: found in the Loconet(r) Personal Edition 1.
 * <P>
 * Some of the message formats used in this class are Copyright Digitrax, Inc.
 * and used with permission as part of the JMRI project.  That permission
 * does not extend to uses in other software products.  If you wish to
 * use this code, algorithm or these message formats outside of JMRI, please
 * contact Digitrax Inc for separate permission.
 * <P>
 * Note that the formatted strings end in a \n, and may contain more than
 * one line separated by \n.  Someday this should be converted to
 * proper Java line handling, but for now it has to be handled in locomon,
 * the sole user of this. (It could be handled by moving the code from
 * locomon into the display member here)
 * <P>
 * Reverse engineering of OPC_MULTI_SENSE was provided by Al Silverstein,
 * used with permission.
 *
 * @author      Bob Jacobsen  Copyright 2001, 2002, 2003
 * @version      $Revision: 1.37 $
 */

#include "rocs/public/rocs.h"
#include "rocs/public/objbase.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"

#include "rocdigs/impl/loconet/lnconst.h"
#include "rocdigs/impl/loconet/lncv.h"

    // control data
static Boolean  showTrackStatus = True;   /* if TRUE, show track status on every slot read    */
static int      trackStatus     = -1;     /* most recent track status value                   */

/**
 * Factor out the PM power messages
 * @param l
 * @return human readable string, no \n on end
 */
static void __powerMultiSenseMessage(byte* msg) {
    int pCMD = (msg[3] & 0xF0);

    if ( (pCMD == 0x30)|| (pCMD == 0x10)) {
        // autoreverse
        int cm1 = msg[3];
        int cm2 = msg[4];
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
            "PM4 %d ch1 %s %s ch2 %s %s ch3 %s %s ch4 %s %s",
            (msg[2]+1),
            ((cm1&1)!=0 ? "AR " : "SC "), ((cm2&1)!=0 ? "ACT;" : "OK;"),
            ((cm1&2)!=0 ? "AR " : "SC "), ((cm2&2)!=0 ? "ACT;" : "OK;"),
            ((cm1&4)!=0 ? "AR " : "SC "), ((cm2&4)!=0 ? "ACT;" : "OK;"),
            ((cm1&8)!=0 ? "AR " : "SC "), ((cm2&8)!=0 ? "ACT;" : "OK;")
            );
    } else if (pCMD == 0x70) {
        // programming
        const char* device;
        if ( (msg[3]&0x7) == 0) device = "PM ";
        else if ( (msg[3]&0x7) == 1) device = "BD ";
        else if ( (msg[3]&0x7) == 2) device = "SE ";
        else device = "(unknown type) ";

        int bit = (msg[4]&0x0E)/2;
        int val = (msg[4]&0x01);
        int wrd = (msg[4]&0x70)/16;
        int opsw = (msg[4]&0x7E)/2+1;
        int bdaddr = msg[2]+1;
        if ((msg[1]&0x1) != 0 ) bdaddr += 128;
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "%d%d %s %d,%d (opsw %d) val=%d %s",
            device, bdaddr,
            ((msg[1]&0x10)!=0) ? " write config bit ":" read config bit ",
            wrd, bit, opsw, val,
            (val==1 ? "(closed)":"(thrown)") );
    } else  // beats me
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
            "OPC_MULTI_SENSE power message PM4 %d unknown CMD=0x%02X",
            msg[2]+1, pCMD);
}

static const char* __idString(int id1, int id2) {
    static char s[256];
    return StrOp.fmtb( s, "0x02X (%d)",(id2&0x7F)*128+(id1&0x7F),((id2/4&0)&0x3f) );
}

static void __peerToPeerMessage(byte* msg) {
    /***********************************************************************************
     * OPC_PEER_XFER    0xE5   ; move 8 bytes PEER to PEER, SRC->DST                    *
     *                         ; Message has response                                   *
     *                         ; <0xE5>,<10>,<SRC>,<DSTL><DSTH>,<PXCT1>,<D1>,           *
     *                         ;        <D2>,<D3>,<D4>,<PXCT2>,<D5>,<D6>,<D7>,          *
     *                         ;        <D8>,<CHK>                                      *
     *                         ;   SRC/DST are 7 bit args. DSTL/H=0 is BROADCAST msg    *
     *                         ;   SRC=0 is MASTER                                      *
     *                         ;   SRC=0x70-0x7E are reserved                           *
     *                         ;   SRC=7F is THROTTLE msg xfer,                         *
     *                         ;        <DSTL><DSTH> encode ID#,                        *
     *                         ;        <0><0> is THROT B'CAST                          *
     *                         ;   <PXCT1>=<0,XC2,XC1,XC0 - D4.7,D3.7,D2.7,D1.7>        *
     *                         ;        XC0-XC2=ADR type CODE-0=7 bit Peer TO Peer adrs *
     *                         ;           1=<D1>is SRC HI,<D2>is DST HI                *
     *                         ;   <PXCT2>=<0,XC5,XC4,XC3 - D8.7,D7.7,D6.7,D5.7>        *
     *                         ;        XC3-XC5=data type CODE- 0=ANSI TEXT string,     *
     *                         ;           balance RESERVED                             *
     ***********************************************************************************/

    int src   = msg[2];              // source of transfer
    int dst_l = msg[3];            // ls 7 bits of destination
    int dst_h = msg[4];            // ms 7 bits of destination
    int pxct1 = msg[5];
    int pxct2 = msg[10];

    byte d[8] = {msg[6],msg[7],msg[8],msg[9],msg[11],msg[12],msg[13],msg[14]};

    // check for a specific type - download message
    if ( (src == 0x7F) && (dst_l == 0x7F) && (dst_h == 0x7F)
         && ((pxct1&0x70) == 0x40) ) {

        // yes - format as such
        // decode subtype
        int sub = pxct2&0x70;
        switch (sub) {
            case 0x00:  // setup
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Download message, setup" );
                break;
            case 0x10:  // set address
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Download message, set address %02X%02X%02X",d[0],d[1],d[2] );
                break;
            case 0x20:  // send data
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Download message, send data %02X %02X %02X %02X %02X %02X %02X %02X",
                              d[0],d[1],d[2],d[3],d[4],d[5],d[6],d[7]);
                break;
            case 0x30:  // verify
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Download message, verify");
                break;
            case 0x40:  // end op
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Download message, end operation");
                break;
            default:    // everything else isn't understood, go to default
                break;
        }
    }


    // no specific type, return generic format
    TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
        "Peer to Peer transfer: SRC=0x%02X DSTL=0x%02X DSTH=0x%02X PXCT1=0x%02X PXCT2=0x%02X D1=0x%02X D2=0x%02X D3=0x%02X D4=0x%02X D5=0x%02X D6=0x%02X D7=0x%02X D8=0x%02X",
        src, dst_l, dst_h, pxct1, pxct2, d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7] );

}

static int LOCO_ADR(int a1, int a2) {
  return (((a1 & 0x7f) * 128) + (a2 & 0x7f));
}

/**
 * Convert bytes from LocoNet packet into a 1-based address for
 * a sensor or turnout.
 * @param a1 Byte containing the upper bits
 * @param a2 Byte containing the lower bits
 * @return 1-4096 address
 */
static int SENSOR_ADR(int a1, int a2) {
  return (((a2 & 0x0f) * 128) + (a1 & 0x7f)) + 1;
}




/**
 * This function creates a string representation of the loco address in
 *     addressLow & addressHigh in a form appropriate for the type of address
 *     (2 or 4 digit) using the Digitrax 'mixed mode' if necessary.
 */

static const char* __convertToMixed( int addressLow, int addressHigh)
{
  static char s[256];
  if (addressHigh == 0) {
    if (addressLow >= 120)
      return StrOp.fmtb( s, "c%d (%d)",addressLow-120, addressLow );
    else if (addressLow >= 110)
      return StrOp.fmtb( s, "b%d (%d)", addressLow-110, addressLow);
    else if (addressLow >= 100)
      return StrOp.fmtb( s, "a%d (%d)", addressLow-100, addressLow);
    else
      return StrOp.fmtb( s, "%d",addressLow & 0x7f);
  } else {
    /* return the full 4 digit address */
    return StrOp.fmtb( s, "%d",LOCO_ADR(addressHigh, addressLow));
  }
}

/**
 * Global flag to indicate the message was not fully parsed,
 * so the hex should be included.
 */

/**
 * Format the message into a text string.  If forceHex is set
 * upon return, the message was not fully parsed.
 * @param l Message to parse
 * @return String representation
 */
void traceLocoNet(byte* msg) {

    Boolean showStatus = False;   /* show track status in this message? */

    int minutes;  // temporary time values
    int hours;
    int frac_mins;

    if( !(TraceOp.getLevel(NULL) & TRCLEVEL_MONITOR) ) {
      /* return if monitor level is not set */
      return;
    }


    switch (msg[0]) {
        /***************************
         * ; 2 Byte MESSAGE OPCODES *
         * ; FORMAT = <OPC>,<CKSUM> *
         * ;                        *
         ***************************/

        /*************************************************
         * OPC_BUSY         0x81   ;MASTER busy code, NUL *
         *************************************************/
    case OPC_GPBUSY:                /* page 8 of Loconet PE */
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Master is busy" );
        break;

        /****************************************************
         * OPC_GPOFF        0x82   ;GLOBAL power OFF request *
         ****************************************************/
    case OPC_GPOFF:                 /* page 8 of Loconet PE */
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Global Power OFF" );
        break;

        /***************************************************
         * OPC_GPON         0x83   ;GLOBAL power ON request *
         ***************************************************/
    case OPC_GPON:                  /* page 8 of Loconet PE */
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Global Power ON" );
        break;

        /**********************************************************************
         * OPC_IDLE         0x85   ;FORCE IDLE state, Broadcast emergency STOP *
         **********************************************************************/
    case OPC_IDLE:                  /* page 8 of Loconet PE */
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Force Idle, Emergency STOP" );
        break;

        /*****************************************
         * ; 4 byte MESSAGE OPCODES               *
         * ; FORMAT = <OPC>,<ARG1>,<ARG2>,<CKSUM> *
         * :                                      *
         *  CODES 0xA8 to 0xAF have responses     *
         *  CODES 0xB8 to 0xBF have responses     *
         *****************************************/

        /***************************************************************************
         * OPC_LOCO_ADR     0xBF   ; REQ loco ADR                                   *
         *                         ; Follow on message: <E7>SLOT READ               *
         *                         ; <0xBF>,<0>,<ADR>,<CHK> REQ loco ADR            *
         *                         ; DATA return <E7>, is SLOT#, DATA that ADR was  *
         *                         : found in.                                      *
         *                         ; IF ADR not found, MASTER puts ADR in FREE slot *
         *                         ; and sends DATA/STATUS return <E7>......        *
         *                         ; IF no FREE slot, Fail LACK,0 is returned       *
         *                         ; [<B4>,<3F>,<0>,<CHK>]                          *
         ***************************************************************************/
    case OPC_LOCO_ADR: {             /* page 8 of Loconet PE */
        int adrHi = msg[1];  // Hi address listed as zero above
        int adrLo = msg[2];  // ADR above, the low part
        int addr = LOCO_ADR(adrHi,adrLo);
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Request slot for loco address %d", addr );
        break;
    }

        /*****************************************************************************
         * OPC_SW_ACK       0xBD   ; REQ SWITCH WITH acknowledge function (not DT200) *
         *                         ; Follow on message: LACK                          *
         *                         ; <0xBD>,<SW1>,<SW2>,<CHK> REQ SWITCH function     *
         *                         ;       <SW1> =<0,A6,A5,A4- A3,A2,A1,A0>           *
         *                         ;               7 ls adr bits.                     *
         *                         ;               A1,A0 select 1 of 4 input pairs    *
         *                         ;               in a DS54                          *
         *                         ;       <SW2> =<0,0,DIR,ON- A10,A9,A8,A7>          *
         *                         ;               Control bits and 4 MS adr bits.    *
         *                         ;               DIR=1 for Closed/GREEN             *
         *                         ;                  =0 for Thrown/RED               *
         *                         ;               ON=1 for Output ON                 *
         *                         ;                 =0 FOR output OFF                *
         *                         ; response is:                                     *
         *                         ; <0xB4><3D><00> if DCS100 FIFO is full, rejected. *
         *                         ; <0xB4><3D><7F> if DCS100 accepted                *
         *****************************************************************************/
    case OPC_SW_ACK: {               /* page 8 of Loconet PE */
        int sw1 = msg[1];
        int sw2 = msg[2];
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Request switch %d %s %s with Acknowledge",
            SENSOR_ADR(sw1, sw2),
            ((sw2 & OPC_SW_ACK_CLOSED)!=0 ? " Closed/Green" : " Thrown/Red"),
            ((sw2 & OPC_SW_ACK_OUTPUT)!=0 ? " (Output On)" : " (Output Off)") );
        break;
    }

        /*************************************************************************
         * OPC_SW_STATE     0xBC   ; REQ state of SWITCH                          *
         *                         ; Follow on message: LACK                      *
         *                         ; <0xBC>,<SW1>,<SW2>,<CHK> REQ state of SWITCH *
         *************************************************************************/
    case OPC_SW_STATE: {             /* page 8 of Loconet PE */
        int sw1 = msg[1];
        int sw2 = msg[2];
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Request state of switch %d", SENSOR_ADR(sw1, sw2) );
        break;
    }

        /************************************************************************************
         * OPC_RQ_SL_DATA   0xBB   ; Request SLOT DATA/status block                          *
         *                         ; Follow on message: <E7>SLOT READ                        *
         *                         ; <0xBB>,<SLOT>,<0>,<CHK> Request SLOT DATA/status block. *
         ************************************************************************************/
    case OPC_RQ_SL_DATA: {           /* page 8 of Loconet PE */
        int slot = msg[1];
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Request data/status for slot %d", slot );
        break;
    }

        /*******************************************************************************
         * OPC_MOVE_SLOTS   0xBA   ; MOVE slot SRC to DEST                              *
         *                         ; Follow on message: <E7>SLOT READ                   *
         *                         ; <0xBA>,<SRC>,<DEST>,<CHK> Move SRC to DEST if      *
         *                         ; SRC or LACK etc is NOT IN_USE, clr SRC             *
         *                         ; SPECIAL CASES:                                     *
         *                         ; If SRC=0 ( DISPATCH GET) , DEST=dont care,         *
         *                         ;    Return SLOT READ DATA of DISPATCH Slot          *
         *                         ; IF SRC=DEST (NULL move) then SRC=DEST is set to    *
         *                         ;    IN_USE , if legal move.                         *
         *                         ; If DEST=0, is DISPATCH Put, mark SLOT as DISPATCH  *
         *                         ;    RETURN slot status <0xE7> of DESTINATION slot   *
         *                         ;       DEST if move legal                           *
         *                         ;    RETURN Fail LACK code if illegal move           *
         *                         ;       <B4>,<3A>,<0>,<chk>, illegal to move to/from *
         *                         ;       slots 120/127                                *
         *******************************************************************************/
    case OPC_MOVE_SLOTS: {           /* page 8 of Loconet PE */
        int src = msg[1];
        int dest = msg[2];

        /* check special cases */
        if (src == 0) {                              /* DISPATCH GET */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Get most recently dispatched slot" );
        } else if (src == dest) {                  /* IN USE       */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Set status of slot %d to IN_USE", src );
        } else if (dest == 0) {                        /* DISPATCH PUT */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Mark slot %d as DISPATCHED to slot %d", src, dest );
        } else {                                            /* general move */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Move data in slot %d to slot %d", src, dest );
        }
    }

        /********************************************************************************
         * OPC_LINK_SLOTS   0xB9   ; LINK slot ARG1 to slot ARG2                         *
         *                         ; Follow on message: <E7>SLOT READ                    *
         *                         ; <0xB9>,<SL1>,<SL2>,<CHK> SLAVE slot SL1 to slot SL2 *
         *                         ; Master LINKER sets the SL_CONUP/DN flags            *
         *                         ; appropriately. Reply is return of SLOT Status       *
         *                         ; <0xE7>. Inspect to see result of Link, invalid      *
         *                         ; Link will return Long Ack Fail <B4>,<39>,<0>,<CHK>  *
         ********************************************************************************/
    case OPC_LINK_SLOTS: {           /* page 9 of Loconet PE */
        int src = msg[1];
        int dest = msg[2];
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Consist loco in slot %d to loco in slot %d", src, dest );
        break;
    }

        /*******************************************************************************************
         * OPC_UNLINK_SLOTS 0xB8   ;UNLINK slot ARG1 from slot ARG2                                 *
         *                         ; Follow on message: <E7>SLOT READ                               *
         *                         ; <0xB8>,<SL1>,<SL2>,<CHK> UNLINK slot SL1 from SL2              *
         *                         ; UNLINKER executes unlink STRATEGY and returns new SLOT#        *
         *                         ; DATA/STATUS of unlinked LOCO . Inspect data to evaluate UNLINK *
         *******************************************************************************************/
    case OPC_UNLINK_SLOTS: {         /* page 9 of Loconet PE */
        int src = msg[1];
        int dest = msg[2];
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Remove loco in slot %d from consist with loco in slot %d", src, dest );
        break;
    }

        /*************************************************************************************
         * OPC_CONSIST_FUNC 0xB6   ; SET FUNC bits in a CONSIST uplink element                *
         *                         ; <0xB6>,<SLOT>,<DIRF>,<CHK> UP consist FUNC bits          *
         *                         ; NOTE this SLOT adr is considered in UPLINKED slot space. *
         *************************************************************************************/
    case OPC_CONSIST_FUNC:  {        /* page 9 of Loconet PE */
        int slot = msg[1];
        int dirf = msg[2];
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Set consist in slot %d direction to %s F0=%sF1=%sF2=%sF3=%sF4=%s",slot,
            ((dirf & DIRF_DIR)!=0 ? "REV"   : "FWD"),
            ((dirf & DIRF_F0)!=0  ? "On, "  : "Off,"),
            ((dirf & DIRF_F1)!=0  ? "On, "  : "Off,"),
            ((dirf & DIRF_F2)!=0  ? "On, "  : "Off,"),
            ((dirf & DIRF_F3)!=0  ? "On, "  : "Off,"),
            ((dirf & DIRF_F4)!=0  ? "On"    : "Off") );
        break;
    }


        /********************************************************************
         * OPC_SLOT_STAT1   0xB5   ; WRITE slot stat1                        *
         *                         ; <0xB5>,<SLOT>,<STAT1>,<CHK> WRITE stat1 *
         ********************************************************************/
    case OPC_SLOT_STAT1:  {          /* page 9 of Loconet PE */
        int slot = msg[1];
        int stat = msg[2];
        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Write slot %d with status value %d (0x%02X)",slot, stat, stat );
        break;
    }

        /*******************************************************************************
         * OPC_LONG_ACK     0xB4   ; Long acknowledge                                   *
         *                         ; <0xB4>,<LOPC>,<ACK1>,<CHK> Long acknowledge        *
         *                         ; <LOPC> is COPY of OPCODE responding to (msb=0).    *
         *                         ; LOPC=0 (unused OPC) is also VALID fail code        *
         *                         ; <ACK1> is appropriate response code for the OPCode *
         *******************************************************************************/
    case OPC_LONG_ACK: {            /* page 9 of Loconet PE */
        int opcode = msg[1];
        int ack1 = msg[2];

        switch (opcode | 0x80) {
        case (OPC_LOCO_ADR):             /* response for OPC_LOCO_ADR */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: No free slot" );
            break;

        case (OPC_LINK_SLOTS):           /* response for OPC_LINK_SLOTS */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: Invalid consist" );
            break;

        case (OPC_SW_ACK):               /* response for OPC_SW_ACK   */
            if (ack1 == 0) {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: The DCS-100 FIFO is full, the switch command was rejected" );
                break;
            } else if (ack1 == 0x7f) {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: The DCS-100 accepted the switch command" );
                break;
            } else {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: Unknown response to 'Request Switch with ACK' command, 0x%02X",ack1 );
                break;
            }

        case (OPC_SW_REQ):               /* response for OPC_SW_REQ */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: Switch request Failed!" );
            break;

        case (OPC_WR_SL_DATA):
            if (ack1 == 0) {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: The Slot Write command was rejected" );
                break;
            } else if (ack1 == 0x01) {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: The Slot Write command was accepted" );
                break;
            } else if (ack1 == 0x40) {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: The Slot Write command was accepted blind (no response will be sent)");
                break;
            } else if (ack1 == 0x7f) {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: Function not implemented, no reply will follow");
                break;
            } else {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: Unknown response to Write Slot Data message 0x%02X",ack1);
                break;
            }

        case (OPC_SW_STATE):
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: Command station response to switch state request 0x%02X %s",ack1,( ((ack1&0x20)!=0) ? " (Closed)" : " (Thrown)"));
            break;

        case (OPC_MOVE_SLOTS):
            if (ack1 == 0) {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: The Move Slots command was rejected");
            } else if (ack1 == 0x7f) {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: The Move Slots command was accepted");
            } else {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: unknown reponse to Move Slots message 0x%02X",ack1);
            }
            break;

        case OPC_IMM_PACKET:      /* special response to OPC_IMM_PACKET */
            if (ack1 == 0) {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: the Send IMM Packet command was rejected, the buffer is full/busy");
            } else if (ack1 == 0x7f) {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: the Send IMM Packet command was accepted");
            } else {
                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Unknown reponse to Send IMM Packet message 0x%02X",ack1);
            }
            break;

        case OPC_IMM_PACKET_2:    /* special response to OPC_IMM_PACKET */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: the Lim Master responded to the Send IMM Packet command with %d (0x%02X)", ack1, ack1);
            break;


        default:
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "LONG_ACK: Response %d (0x%02X) to opcode 0x%02X not decoded", ack1, ack1, opcode);
            break;
        }
        break;
    }

        /********************************************************************************************
         * OPC_INPUT_REP    0xB2   ; General SENSOR Input codes                                      *
         *                         ; <0xB2>, <IN1>, <IN2>, <CHK>                                     *
         *                         ;   <IN1> =<0,A6,A5,A4- A3,A2,A1,A0>,                             *
         *                         ;           7 ls adr bits.                                        *
         *                         ;           A1,A0 select 1 of 4 inputs pairs in a DS54.           *
         *                         ;   <IN2> =<0,X,I,L- A10,A9,A8,A7>,                               *
         *                         ;           Report/status bits and 4 MS adr bits.                 *
         *                         ;           "I"=0 for DS54 "aux" inputs                           *
         *                         ;              =1 for "switch" inputs mapped to 4K SENSOR space.  *
         *                         ;                                                                 *
         *                         ;           (This is effectively a least significant adr bit when *
         *                         ;            using DS54 input configuration)                      *
         *                         ;                                                                 *
         *                         ;           "L"=0 for input SENSOR now 0V (LO),                   *
         *                         ;              =1 for Input sensor >=+6V (HI)                     *
         *                         ;           "X"=1, control bit,                                   *
         *                         ;              =0 is RESERVED for future!                         *
         ********************************************************************************************/
    case OPC_INPUT_REP:  {           /* page 9 of Loconet PE */
        int in1 = msg[1];
        int in2 = msg[2];
        int sensoraddr = SENSOR_ADR(in1,in2);

        int ch = 0;

        if ( ((in1/2) & 3) == 0 ) ch = 0;
        else if ( ((in1/2) & 3) == 1 ) ch = 4;
        else if ( ((in1/2) & 3) == 2 ) ch = 8;
        else ch = 12;

        if ( ((in1 & 1) !=0) &&  (in2 & OPC_INPUT_REP_SW) ) ch+=4;
        else if (  (in1 & 1) && !(in2 & OPC_INPUT_REP_SW) ) ch+=3;
        else if ( !(in1 & 1) &&  (in2 & OPC_INPUT_REP_SW) ) ch+=2;
        else ch+=1;

        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
            "Sensor report: contact %d is %s",
            ((sensoraddr-1) * 2 + ( (in2 & OPC_INPUT_REP_SW) ? 2:1) ),
            ((in2 & OPC_INPUT_REP_HI) ? "Hi" : "Lo") );
        break;
    }

        /***************************************************************************************
         * OPC_SW_REP       0xB1   ; Turnout SENSOR state REPORT                                *
         *                         ; <0xB1>,<SN1>,<SN2>,<CHK> SENSOR state REPORT               *
         *                         ;   <SN1> =<0,A6,A5,A4- A3,A2,A1,A0>,                        *
         *                         ;           7 ls adr bits.                                   *
         *                         ;           A1,A0 select 1 of 4 input pairs in a DS54        *
         *                         ;   <SN2> =<0,1,I,L- A10,A9,A8,A7>                           *
         *                         ;           Report/status bits and 4 MS adr bits.            *
         *                         ;           this <B1> opcode encodes input levels            *
         *                         ;           for turnout feedback                             *
         *                         ;           "I" =0 for "aux" inputs (normally not feedback), *
         *                         ;               =1 for "switch" input used for               *
         *                         ;                  turnout feedback for DS54                 *
         *                         ;                  ouput/turnout # encoded by A0-A10         *
         *                         ;           "L" =0 for this input 0V (LO),                   *
         *                         ;               =1 this input > +6V (HI)                     *
         *                         ;                                                            *
         *                         ;   alternately;                                             *
         *                         ;                                                            *
         *                         ;   <SN2> =<0,0,C,T- A10,A9,A8,A7>                           *
         *                         ;           Report/status bits and 4 MS adr bits.            *
         *                         ;           this <B1> opcode encodes current OUTPUT levels   *
         *                         ;           "C" =0 if "Closed" ouput line is OFF,            *
         *                         ;               =1 "closed" output line is ON                *
         *                         ;                  (sink current)                            *
         *                         ;           "T" =0 if "Thrown" output line is OFF,           *
         *                         ;               =1 "thrown" output line is ON                *
         *                         ;                  (sink I)                                  *
         ***************************************************************************************/
    case OPC_SW_REP: {               /* page 9 of Loconet PE */
        int sn1 = msg[1];
        int sn2 = msg[2];

        if ((sn2 & OPC_SW_REP_INPUTS)!=0) {
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Turnout %d %s is %s",
                SENSOR_ADR(sn1, sn2),
                ((sn2 & OPC_SW_REP_SW) !=0 ? " Switch input" : " Aux input"),
                (((sn2 & OPC_SW_REP_HI)!=0) ? "Closed (input off)" : "Thrown (input on)"));
        } else {  // OPC_SW_REP_INPUTS is 0
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Turnout %d output state: Closed output is %s, Thrown output is %s",
                SENSOR_ADR(sn1, sn2),
                ((sn2 & OPC_SW_REP_CLOSED)!=0 ? "ON (sink)" : "OFF (open)"),
                ((sn2 & OPC_SW_REP_THROWN)!=0 ? "ON (sink)" : "OFF (open)"));
        }
        break;
    }


        /*******************************************************************************************
         * OPC_SW_REQ       0xB0   ; REQ SWITCH function                                            *
         *                         ; <0xB0>,<SW1>,<SW2>,<CHK> REQ SWITCH function                   *
         *                         ;   <SW1> =<0,A6,A5,A4- A3,A2,A1,A0>,                            *
         *                         ;           7 ls adr bits.                                       *
         *                         ;           A1,A0 select 1 of 4 input pairs in a DS54            *
         *                         ;   <SW2> =<0,0,DIR,ON- A10,A9,A8,A7>                            *
         *                         ;           Control bits and 4 MS adr bits.                      *
         *                         ;   DIR  =1 for Closed,/GREEN,                                   *
         *                         ;        =0 for Thrown/RED                                       *
         *                         ;   ON   =1 for Output ON,                                       *
         *                         ;        =0 FOR output OFF                                       *
         *                         ;                                                                *
         *                         ;   Note-Immediate response of <0xB4><30><00> if command failed, *
         *                         ;        otherwise no response "A" CLASS codes                   *
         *                         ;                                     *
         *                         ;   Special form:  broadcast (PE page 12)               *
         *                         ;                                     *
         *                         ;                                     *
         *                         ;   Special form:  LocoNet interrogate (PE page 13)           *
         *                         ;                                     *
         *                         ;                                     *
         *                         ;                                     *
         *                         ;                                     *
         *                         ;                                     *
         *                         ;                                     *
         *                         ;                                     *
         *                         ;                                     *

        *******************************************************************************************/
    case OPC_SW_REQ: {               /* page 9 of Loconet PE */
        int sw1 = msg[1];
        int sw2 = msg[2];

        // check for special forms first
        if ( ((sw2 & 0xCF) == 0x0F)  && ((sw1 & 0xFC) == 0x78) ) { // broadcast address LPU V1.0 page 12
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Request Switch to broadcast address with bits a=%s c=%d b=%d Output %s",
                ((sw2&0x20)>>5)+((sw2 & OPC_SW_REQ_DIR)!=0 ? " (Closed)" : " (Thrown)"),
                " c="+ ((sw1 & 0x02)>>1),
                " b="+ ((sw1 & 0x01)),
                ((sw2 & OPC_SW_REQ_OUT)!=0 ? "On"     : "Off"));

        } else if ( ((sw2 & 0xCF) == 0x07)  && ((sw1 & 0xFC) == 0x78) ) { // broadcast address LPU V1.0 page 13
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Request switch command is Interrogate LocoNet with bits a=%d c=%d b=%d Output %s Dir %s %s",
                ((sw2 & 0x20)>>5),
                ((sw1&0x02)>>1),
                ((sw1&0x01)),
                ((sw2 & OPC_SW_REQ_OUT)!=0 ? "On"     : "Off"), ((sw2 & OPC_SW_REQ_DIR)!=0 ? "Closed" : "Thrown"),
                ( ( (sw2&0x10) == 0 ) ? "" : "\tNote 0x10 bit in sw2 is unexpectedly 0\n") );

        } else { // normal command
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Requesting Switch at %d to %s(%d) (output %s)",
                SENSOR_ADR(sw1, sw2),
                ((sw2 & OPC_SW_REQ_DIR)!=0 ? "Closed" : "Thrown"),
                (sw2 & OPC_SW_REQ_DIR) >> 5,
                ((sw2 & OPC_SW_REQ_OUT)!=0 ? "On"     : "Off"));
        }

        break;
    }

        /****************************************************
         * OPC_LOCO_SND     0xA2   ;SET SLOT sound functions *
         ****************************************************/
    case OPC_LOCO_SND:    {          /* page 10 of Loconet PE */
        int slot = msg[1];
        int snd  = msg[2];

        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Set loco in slot %d  F5=%s F6=%s F7=%s F8=%s",
            slot,
            ((snd & SND_F5) != 0  ? "On"  : "Off"),
            ((snd & SND_F6) != 0  ? "On"  : "Off"),
            ((snd & SND_F7) != 0  ? "On"  : "Off"),
            ((snd & SND_F8) != 0  ? "On"  : "Off")
            );
        break;
    }

        /****************************************************
         * OPC_LOCO_DIRF    0xA1   ;SET SLOT dir, F0-4 state *
         ****************************************************/
    case OPC_LOCO_DIRF:             /* page 10 of Loconet PE */
        {
            int slot = msg[1];
            int dirf = msg[2];

            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Set loco in slot %d dir to %s F0=%s F1=%s F2=%s F3=%s F4=%s",
                slot,
                ((dirf & DIRF_DIR) ? "REV"   : "FWD"),
                ((dirf & DIRF_F0)  ? "On, "  : "Off,"),
                ((dirf & DIRF_F1)  ? "On, "  : "Off,"),
                ((dirf & DIRF_F2)  ? "On, "  : "Off,"),
                ((dirf & DIRF_F3)  ? "On, "  : "Off,"),
                ((dirf & DIRF_F4)  ? "On"    : "Off")
                );
            break;
        }


        /***********************************************************************
         * OPC_LOCO_SPD     0xA0   ;SET SLOT speed e.g. <0xA0><SLOT#><SPD><CHK> *
         ***********************************************************************/
    case OPC_LOCO_SPD:     {         /* page 10 of Loconet PE */
        int slot = msg[1];
        int spd  = msg[2];

        if (spd == OPC_LOCO_SPD_ESTOP) { /* emergency stop */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Set speed of loco in slot %d to EMERGENCY STOP!", slot);
        } else {
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Set speed of loco in slot %d to %d", slot, spd);
        }
        break;
    }

        /*******************************************************
         * ; 6 byte MESSAGE OPCODES                             *
         * ; FORMAT = <OPC>,<ARG1>,<ARG2>,<ARG3>,<ARG4>,<CKSUM> *
         * :                                                    *
         *  CODES 0xC8 to 0xCF have responses                   *
         *  CODES 0xD8 to 0xDF have responses                   *
         ********************************************************/

        /************************************************************************
         * OPC_MULTI_SENSE     0xD0 messages about power management              *
         *                          and transponding                             *
         *                                                                       *
         *  If byte 1 high nibble is 0x20 or 0x00 this is a transponding message *
         *************************************************************************/
    case OPC_MULTI_SENSE:     {         // definition courtesy Al Silverstein
        int type = msg[1] & OPC_MULTI_SENSE_MSG;
        char* m;

        const char* zone;
        if      ((msg[2]&0x0F) == 0x00) zone = "A";
        else if ((msg[2]&0x0F) == 0x02) zone = "B";
        else if ((msg[2]&0x0F) == 0x04) zone = "C";
        else if ((msg[2]&0x0F) == 0x06) zone = "D";
        else if ((msg[2]&0x0F) == 0x08) zone = "E";
        else if ((msg[2]&0x0F) == 0x0A) zone = "F";
        else if ((msg[2]&0x0F) == 0x0C) zone = "G";
        else if ((msg[2]&0x0F) == 0x0E) zone = "H";
        else zone="<unknown>";

        int boardaddr    = ( ( ( (msg[1]&0x1F) * 128 ) + msg[2] ) >> 4 ) + 1;

        int addr = 0;
        if (msg[3]==0x7D)
          addr=msg[4];
        else
          addr=msg[3]*128+msg[4];

        switch (type) {
        case OPC_MULTI_SENSE_POWER:
            __powerMultiSenseMessage(msg);
            break;
        case OPC_MULTI_SENSE_PRESENT:  // from transponding app note
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Transponder present in BDL %d zone %s decoder address %d", boardaddr, zone,addr );
            break;
        case OPC_MULTI_SENSE_ABSENT:
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Transponder absent in BDL %d zone %s decoder address %d", boardaddr, zone,addr );
            break;
        default:
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "OPC_MULTI_SENSE unknown format");
            break;
        }
        break;
    }

        /********************************************************************
         * ; VARIABLE Byte MESSAGE OPCODES                                   *
         * ; FORMAT = <OPC>,<COUNT>,<ARG2>,<ARG3>,...,<ARG(COUNT-3)>,<CKSUM> *
         ********************************************************************/

        /**********************************************************************************************
         * OPC_WR_SL_DATA   0xEF   ; WRITE SLOT DATA, 10 bytes                                         *
         *                         ; Follow on message: LACK                                           *
         *                         ; <0xEF>,<0E>,<SLOT#>,<STAT>,<ADR>,<SPD>,<DIRF>,                    *
         *                         ;        <TRK>,<SS2>,<ADR2>,<SND>,<ID1>,<ID2>,<CHK>                 *
         *                         ; SLOT DATA WRITE, 10 bytes data /14 byte MSG                       *
         ***********************************************************************************************
         * OPC_SL_RD_DATA   0xE7   ; SLOT DATA return, 10 bytes                                        *
         *                         ; <0xE7>,<0E>,<SLOT#>,<STAT>,<ADR>,<SPD>,<DIRF>,                    *
         *                         ;        <TRK>,<SS2>,<ADR2>,<SND>,<ID1>,<ID2>,<CHK>                 *
         *                         ; SLOT DATA READ, 10 bytes data /14 byte MSG                        *
         *                         ;                                                                   *
         *                         ; NOTE; If STAT2.2=0 EX1/EX2 encodes an ID#,                        *
         *                         ;       [if STAT2.2=1 the STAT.3=0 means EX1/EX2                    *
         *                         ;        are ALIAS]                                                 *
         *                         ;                                                                   *
         *                         ; ID1/ID2 are two 7 bit values encoding a 14 bit                    *
         *                         ;         unique DEVICE usage ID.                                   *
         *                         ;                                                                   *
         *                         ;   00/00 - means NO ID being used                                  *
         *                         ;                                                                   *
         *                         ;   01/00 - ID shows PC usage.                                      *
         *                         ;    to         Lo nibble is TYP PC#                                *
         *                         ;   7F/01       (PC can use hi values)                              *
         *                         ;                                                                   *
         *                         ;   00/02 -SYSTEM reserved                                          *
         *                         ;    to                                                             *
         *                         ;   7F/03                                                           *
         *                         ;                                                                   *
         *                         ;   00/04 -NORMAL throttle RANGE                                    *
         *                         ;    to                                                             *
         *                         ;   7F/7E                                                           *
         ***********************************************************************************************
         * Notes:                                                                                      *
         * The SLOT DATA bytes are, in order of TRANSMISSION for <E7> READ or <EF> WRITE.              *
         * NOTE SLOT 0 <E7> read will return MASTER config information bytes.                          *
         *                                                                                             *
         * 0) SLOT NUMBER:                                                                             *
         *                                                                                             *
         * ; 0-7FH, 0 is special SLOT,                                                                 *
         *                     ; 070H-07FH DIGITRAX reserved:                                          *
         *                                                                                             *
         * 1) SLOT STATUS1:                                                                            *
         *                                                                                             *
         *     D7-SL_SPURGE    ; 1=SLOT purge en,                                                      *
         *                     ; ALSO adrSEL (INTERNAL use only) (not seen on NET!)                    *
         *                                                                                             *
         *     D6-SL_CONUP     ; CONDN/CONUP: bit encoding-Control double linked Consist List          *
         *                     ;    11=LOGICAL MID CONSIST , Linked up AND down                        *
         *                     ;    10=LOGICAL CONSIST TOP, Only linked downwards                      *
         *                     ;    01=LOGICAL CONSIST SUB-MEMBER, Only linked upwards                 *
         *                     ;    00=FREE locomotive, no CONSIST indirection/linking                 *
         *                     ; ALLOWS "CONSISTS of CONSISTS". Uplinked means that                    *
         *                     ; Slot SPD number is now SLOT adr of SPD/DIR and STATUS                 *
         *                     ; of consist. i.e. is ;an Indirect pointer. This Slot                   *
         *                     ; has same BUSY/ACTIVE bits as TOP of Consist. TOP is                   *
         *                     ; loco with SPD/DIR for whole consist. (top of list).                   *
         *                     ; BUSY/ACTIVE: bit encoding for SLOT activity                           *
         *                                                                                             *
         *     D5-SL_BUSY      ; 11=IN_USE loco adr in SLOT -REFRESHED                                 *
         *                                                                                             *
         *     D4-SL_ACTIVE    ; 10=IDLE loco adr in SLOT -NOT refreshed                               *
         *                     ; 01=COMMON loco adr IN SLOT -refreshed                                 *
         *                     ; 00=FREE SLOT, no valid DATA -not refreshed                            *
         *                                                                                             *
         *     D3-SL_CONDN     ; shows other SLOT Consist linked INTO this slot, see SL_CONUP          *
         *                                                                                             *
         *     D2-SL_SPDEX     ; 3 BITS for Decoder TYPE encoding for this SLOT                        *
         *                                                                                             *
         *     D1-SL_SPD14     ; 011=send 128 speed mode packets                                       *
         *                                                                                             *
         *     D0-SL_SPD28     ; 010=14 step MODE                                                      *
         *                     ; 001=28 step. Generate Trinary packets for this                        *
         *                     ;              Mobile ADR                                               *
         *                     ; 000=28 step. 3 BYTE PKT regular mode                                  *
         *                     ; 111=128 Step decoder, Allow Advanced DCC consisting                   *
         *                     ; 100=28 Step decoder ,Allow Advanced DCC consisting                    *
         *                                                                                             *
         * 2) SLOT LOCO ADR:                                                                           *
         *                                                                                             *
         *     LOCO adr Low 7 bits (byte sent as ARG2 in ADR req opcode <0xBF>)                        *
         *                                                                                             *
         * 3) SLOT SPEED:                                                                              *
         *     0x00=SPEED 0 ,STOP inertially                                                           *
         *     0x01=SPEED 0 EMERGENCY stop                                                             *
         *     0x02->0x7F increasing SPEED,0x7F=MAX speed                                              *
         *     (byte also sent as ARG2 in SPD opcode <0xA0> )                                          *
         *                                                                                             *
         * 4) SLOT DIRF byte: (byte also sent as ARG2 in DIRF opcode <0xA1>)                           *
         *                                                                                             *
         *     D7-0        ; always 0                                                                  *
         *     D6-SL_XCNT  ; reserved , set 0                                                          *
         *     D5-SL_DIR   ; 1=loco direction FORWARD                                                  *
         *     D4-SL_F0    ; 1=Directional lighting ON                                                 *
         *     D3-SL_F4    ; 1=F4 ON                                                                   *
         *     D2-SL_F3    ; 1=F3 ON                                                                   *
         *     D1-SL_F2    ; 1=F2 ON                                                                   *
         *     D0-SL_F1    ; 1=F1 ON                                                                   *
         *                                                                                             *
         *                                                                                             *
         *                                                                                             *
         *                                                                                             *
         * 5) TRK byte: (GLOBAL system /track status)                                                  *
         *                                                                                             *
         *     D7-D4       Reserved                                                                    *
         *     D3          GTRK_PROG_BUSY 1=Programming TRACK in this Master is BUSY.                  *
         *     D2          GTRK_MLOK1     1=This Master IMPLEMENTS LocoNet 1.1 capability,             *
         *                                0=Master is DT200                                            *
         *     D1          GTRK_IDLE      0=TRACK is PAUSED, B'cast EMERG STOP.                        *
         *     D0          GTRK_POWER     1=DCC packets are ON in MASTER, Global POWER up              *
         *                                                                                             *
         * 6) SLOT STATUS:                                                                             *
         *                                                                                             *
         *     D3          1=expansion IN ID1/2, 0=ENCODED alias                                       *
         *     D2          1=Expansion ID1/2 is NOT ID usage                                           *
         *     D0          1=this slot has SUPPRESSED ADV consist-7)                                   *
         *                                                                                             *
         * 7) SLOT LOCO ADR HIGH:                                                                      *
         *                                                                                             *
         * Locomotive address high 7 bits. If this is 0 then Low address is normal 7 bit NMRA SHORT    *
         * address. If this is not zero then the most significant 6 bits of this address are used in   *
         * the first LONG address byte ( matching CV17). The second DCC LONG address byte matches CV18 *
         * and includes the Adr Low 7 bit value with the LS bit of ADR high in the MS postion of this  *
         * track adr byte.                                                                             *
         *                                                                                             *
         * Note a DT200 MASTER will always interpret this as 0.                                        *
         *                                                                                             *
         * 8) SLOT SOUND:                                                                              *
         *                                                                                             *
         *     Slot sound/ Accesory Function mode II packets. F5-F8                                    *
         *     (byte also sent as ARG2 in SND opcode)                                                  *
         *                                                                                             *
         *     D7-D4           reserved                                                                *
         *     D3-SL_SND4/F8                                                                           *
         *     D2-SL_SND3/F7                                                                           *
         *     D1-SL_SND2/F6                                                                           *
         *     D0-SL_SND1/F5   1= SLOT Sound 1 function 1active (accessory 2)                          *
         *                                                                                             *
         * 9) EXPANSION RESERVED ID1:                                                                  *
         *                                                                                             *
         *     7 bit ls ID code written by THROTTLE/PC when STAT2.4=1                                  *
         *                                                                                             *
         * 10) EXPANSION RESERVED ID2:                                                                 *
         *                                                                                             *
         *     7 bit ms ID code written by THROTTLE/PC when STAT2.4=1                                  *
         **********************************************************************************************/
    case OPC_WR_SL_DATA:            /* page 10 of Loconet PE */
    case OPC_SL_RD_DATA:            /* page 10 of Loconet PE */
        {
            char* mode = NULL;
            char* locoAdrStr = NULL;
            const char* mixedAdrStr = NULL;
            char* logString = NULL;

            // rwSlotData = (rwSlotDataMsg *) msgBuf;
            int command   = msg[0];
            int mesg_size = msg[1];     // size of the message in bytes
            int slot      = msg[2];     // slot number for this request
            int stat      = msg[3];     // slot status
            int adr       = msg[4];     // loco address
            int spd       = msg[5];     // command speed
            int dirf      = msg[6];     // direction and F0-F4 bits
            int trk       = msg[7];     // track status
            int ss2       = msg[8];     // slot status 2 (tells how to use ID1/ID2 & ADV Consist)
            int adr2      = msg[9];     // loco address high
            int snd       = msg[10];    // Sound 1-4 / F5-F8
            int id1       = msg[11];    // ls 7 bits of ID code
            int id2       = msg[12];    // ms 7 bits of ID code

            /* build loco address string */
            mixedAdrStr = __convertToMixed(adr, adr2);

            /* figure out the alias condition, and create the loco address string */
            if (adr2 == 0x7f) {
                if ((ss2 & STAT2_ALIAS_MASK) == STAT2_ID_IS_ALIAS) {
                    /* this is an aliased address and we have the alias*/
                    locoAdrStr = StrOp.fmt("%d (Alias for loco %s)",
                        LOCO_ADR(id2, id1),
                        mixedAdrStr
                        );
                } else {
                    /* this is an aliased address and we don't have the alias */
                    locoAdrStr = StrOp.fmt( "%s (via Alias)", mixedAdrStr );
                }
            } else {
                /* regular 4 digit address, 128 to 9983 */
                locoAdrStr = StrOp.fmt( "%s", mixedAdrStr);
            }

            /*
             *  These share a common data format with the only
             *  difference being whether we are reading or writing
             *  the slot data.
             */

            if (command == OPC_WR_SL_DATA) {
                mode = "Write";
            } else {
                mode = "Read";
            }

            if (slot == CONFIG_SLOT) {
              TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "system config slot" );
            }
            else if (slot == FC_SLOT) {
                /**********************************************************************************************
                 * FAST Clock:                                                                                 *
                 * ===========                                                                                 *
                 * The system FAST clock and parameters are implemented in Slot#123 <7B>.                      *
                 *                                                                                             *
                 * Use <EF> to write new clock information, Slot read of 0x7B,<BB><7B>.., will return current  *
                 * System clock information, and other throttles will update to this SYNC. Note that all       *
                 * attached display devices keep a current clock calculation based on this SYNC read value,    *
                 * i.e. devices MUST not continuously poll the clock SLOT to generate time, but use this       *
                 * merely to restore SYNC and follow current RATE etc. This clock slot is typically "pinged"   *
                 * or read SYNC'd every 70 to 100 seconds , by a single user, so all attached devices can      *
                 * synchronise any phase drifts. Upon seeing a SYNC read, all devices should reset their local *
                 * sub-minute phase counter and invalidate the SYNC update ping generator.                     *
                 *                                                                                             *
                 * Clock Slot Format:                                                                          *
                 *                                                                                             *
                 * <0xEF>,<0E>,<7B>,<CLK_RATE>,<FRAC_MINSL>,<FRAC_MINSH>,<256-MINS_60>,                        *
                 * <TRK><256-HRS_24>,<DAYS>,<CLK_CNTRL>,<ID1>,<1D2>,<CHK>                                      *
                 *                                                                                             *
                 *     <CLK_RATE>      0=Freeze clock,                                                         *
                 *                     1=normal 1:1 rate,                                                      *
                 *                     10=10:1 etc, max VALUE is 7F/128 to 1                                   *
                 *     <FRAC_MINSL>    FRAC mins hi/lo are a sub-minute counter , depending                    *
                 *                         on the CLOCK generator                                              *
                 *     <FRAC_MINSH>    Not for ext. usage. This counter is reset when valid                    *
                 *                         <E6><7B> SYNC msg seen                                              *
                 *     <256-MINS_60>   This is FAST clock MINUTES subtracted from 256. Modulo 0-59             *
                 *     <256-HRS_24>    This is FAST clock HOURS subtracted from 256. Modulo 0-23               *
                 *     <DAYS>          number of 24 Hr clock rolls, positive count                             *
                 *     <CLK_CNTRL>     Clock Control Byte                                                      *
                 *                         D6- 1=This is valid Clock information,                              *
                 *                             0=ignore this <E6><7B>, SYNC reply                              *
                 *     <ID1>,<1D2>     This is device ID last setting the clock.                               *
                 *                         <00><00> shows no set has happened                                  *
                 *     <7F><7x>        are reserved for PC access                                              *
                 **********************************************************************************************/

                /* make message easier to deal with internally */
                // fastClock = (fastClockMsg *)msgBuf;
                int clk_rate   = msg[3];   // 0 = Freeze clock, 1 = normal, 10 = 10:1 etc. Max is 0x7f
                int frac_minsl = msg[4];   // fractional minutes. not for external use.
                int frac_minsh = msg[5];
                int mins_60    = msg[6];   // 256 - minutes
                int track_stat = msg[7];   // track status
                int hours_24   = msg[8];   // 256 - hours
                int days       = msg[9];   // clock rollovers
                int clk_cntrl  = msg[10];  // bit 6 = 1; data is valid clock info
                //  "  "   0; ignore this reply
                // id1/id2 is device id of last device to set the clock
                //  "   "  = zero shows not set has happened

                /* recover hours and minutes values */
                minutes = ((255 - mins_60) & 0x7f) % 60;
                hours   = ((256 - hours_24)& 0x7f) % 24;
                hours   = (24 - hours) % 24;
                minutes = (60 - minutes) % 60;
                frac_mins = 0x3FFF - ( frac_minsl + ( frac_minsh << 7 ) ) ;

                /* check track status value and display */
                if ((trackStatus != track_stat) || showTrackStatus) {
                    trackStatus = track_stat;
                    showStatus  = True;
                }

                if (showStatus) {
                    TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                        "%s  Fast Clock: (Data is %s)\n\t%s, rate is %d:1. Day %d, %d:%d.%d. Last set by ID %s\n\tMaster controller %s,\n\tTrack Status is %s %s,\n\tProgramming Track is %s",
                        mode, ((clk_cntrl & 0x20) != 0 ? "Valid" : "Invalid - ignore"),
                        (clk_rate != 0 ? "Running" : "Frozen"),
                        clk_rate,
                        days, hours, minutes, frac_mins,
                        __idString(id1, id2),
                        ((track_stat & GTRK_MLOK1)!=0 ? "implements LocoNet 1.1" : "is a DT-200"),
                        ((track_stat & GTRK_POWER)!=0  ? " On," : " Off,"),
                        ((track_stat & GTRK_IDLE)!=0  ? " Paused " : " Running "),
                        ((track_stat & GTRK_PROG_BUSY)!=0 ? "Busy" : "Available")
                        );
                } else {
                    TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                        "%s  Fast Clock: (Data is %s)\n\t%s, rate is %d:1. Day %d, %d:%d.%d. Last set by ID %s",
                        mode, ((clk_cntrl & 0x20) != 0 ? "Valid" : "Invalid - ignore"),
                        (clk_rate != 0 ? "Running" : "Frozen"),
                        clk_rate,
                        days, hours, minutes, frac_mins,
                        __idString(id1, id2)
                        );
                }
                // end fast clock block

            } else if (slot == PRG_SLOT) {


                /**********************************************************************************************
                 * Programmer track:                                                                           *
                 * =================                                                                           *
                 * The programmer track is accessed as Special slot #124 ( $7C, 0x7C). It is a full            *
                 * asynchronous shared system resource.                                                        *
                 *                                                                                             *
                 * To start Programmer task, write to slot 124. There will be an immediate LACK acknowledge    *
                 * that indicates what programming will be allowed. If a valid programming task is started,    *
                 * then at the final (asynchronous) programming completion, a Slot read <E7> from slot 124     *
                 * will be sent. This is the final task status reply.                                          *
                 *                                                                                             *
                 * Programmer Task Start:                                                                      *
                 * ----------------------                                                                      *
                 * <0xEF>,<0E>,<7C>,<PCMD>,<0>,<HOPSA>,<LOPSA>,<TRK>;<CVH>,<CVL>,                              *
                 *        <DATA7>,<0>,<0>,<CHK>                                                                *
                 *                                                                                             *
                 * This OPC leads to immediate LACK codes:                                                     *
                 *     <B4>,<7F>,<7F>,<chk>    Function NOT implemented, no reply.                             *
                 *     <B4>,<7F>,<0>,<chk>     Programmer BUSY , task aborted, no reply.                       *
                 *     <B4>,<7F>,<1>,<chk>     Task accepted , <E7> reply at completion.                       *
                 *     <B4>,<7F>,<0x40>,<chk>  Task accepted blind NO <E7> reply at completion.                *
                 *                                                                                             *
                 * Note that the <7F> code will occur in Operations Mode Read requests if the System is not    *
                 * configured for and has no Advanced Acknowlegement detection installed.. Operations Mode     *
                 * requests can be made and executed whilst a current Service Mode programming task is keeping *
                 * the Programming track BUSY. If a Programming request is rejected, delay and resend the      *
                 * complete request later. Some readback operations can keep the Programming track busy for up *
                 * to a minute. Multiple devices, throttles/PC's etc, can share and sequentially use the       *
                 * Programming track as long as they correctly interpret the response messages. Any Slot RD    *
                 * from the master will also contain the Programmer Busy status in bit 3 of the <TRK> byte.    *
                 *                                                                                             *
                 * A <PCMD> value of <00> will abort current SERVICE mode programming task and will echo with  *
                 * an <E6> RD the command string that was aborted.                                             *
                 *                                                                                             *
                 * <PCMD> Programmer Command:                                                                  *
                 * --------------------------                                                                  *
                 * Defined as                                                                                  *
                 *     D7 -0                                                                                   *
                 *     D6 -Write/Read  1= Write,                                                               *
                 *                     0=Read                                                                  *
                 *     D5 -Byte Mode   1= Byte operation,                                                      *
                 *                     0=Bit operation (if possible)                                           *
                 *     D4 -TY1 Programming Type select bit                                                     *
                 *     D3 -TY0 Prog type select bit                                                            *
                 *     D2 -Ops Mode    1=Ops Mode on Mainlines,                                                *
                 *                     0=Service Mode on Programming Track                                     *
                 *     D1 -0 reserved                                                                          *
                 *     D0 -0-reserved                                                                          *
                 *                                                                                             *
                 * Type codes:                                                                                 *
                 * -----------                                                                                 *
                 *     Byte Mode   Ops Mode   TY1   TY0   Meaning                                              *
                 *        1           0        0     0    Paged mode byte Read/Write on Service Track          *
                 *        1           0        0     0    Paged mode byte Read/Write on Service Track          *
                 *        1           0        0     1    Direct mode byteRead/Write on Service Track          *
                 *        0           0        0     1    Direct mode bit Read/Write on Service Track          *
                 *        x           0        1     0    Physical Register byte Read/Write on Service Track   *
                 *        x           0        1     1    Service Track- reserved function                     *
                 *        1           1        0     0    Ops mode Byte program, no feedback                   *
                 *        1           1        0     1    Ops mode Byte program, feedback                      *
                 *        0           1        0     0    Ops mode Bit program, no feedback                    *
                 *        0           1        0     1    Ops mode Bit program, feedback                       *
                 *                                                                                             *
                 *     <HOPSA>Operations Mode Programming                                                      *
                 *         7 High address bits of Loco to program, 0 if Service Mode                           *
                 *     <LOPSA>Operations Mode Programming                                                      *
                 *         7 Low address bits of Loco to program, 0 if Service Mode                            *
                 *     <TRK> Normal Global Track status for this Master,                                       *
                 *         Bit 3 also is 1 WHEN Service Mode track is BUSY                                     *
                 *     <CVH> High 3 BITS of CV#, and ms bit of DATA.7                                          *
                 *         <0,0,CV9,CV8 - 0,0, D7,CV7>                                                         *
                 *     <CVL> Low 7 bits of 10 bit CV address.                                                  *
                 *         <0,CV6,CV5,CV4-CV3,CV2,CV1,CV0>                                                     *
                 *     <DATA7>Low 7 BITS OF data to WR or RD COMPARE                                           *
                 *         <0,D6,D5,D4 - D3,D2,D1,D0>                                                          *
                 *         ms bit is at CVH bit 1 position.                                                    *
                 *                                                                                             *
                 * Programmer Task Final Reply:                                                                *
                 * ----------------------------                                                                *
                 * (if saw LACK <B4>,<7F>,<1>,<chk> code reply at task start)                                  *
                 *                                                                                             *
                 * <0xE7>,<0E>,<7C>,<PCMD>,<PSTAT>,<HOPSA>,<LOPSA>,<TRK>;<CVH>,<CVL>,                          *
                 * <DATA7>,<0>,<0>,<CHK>                                                                       *
                 *                                                                                             *
                 *     <PSTAT> Programmer Status error flags. Reply codes resulting from                       *
                 *             completed task in PCMD                                                          *
                 *         D7-D4 -reserved                                                                     *
                 *         D3    -1= User Aborted this command                                                 *
                 *         D2    -1= Failed to detect READ Compare acknowledge response                        *
                 *                   from decoder                                                              *
                 *         D1    -1= No Write acknowledge response from decoder                                *
                 *         D0    -1= Service Mode programming track empty- No decoder detected                 *
                 *                                                                                             *
                 * This <E7> response is issued whenever a Programming task is completed. It echos most of the *
                 * request information and returns the PSTAT status code to indicate how the task completed.   *
                 * If a READ was requested <DATA7> and <CVH> contain the returned data, if the PSTAT indicates *
                 * a successful readback (typically =0). Note that if a Paged Read fails to detect a           *
                 * successful Page write acknowledge when first setting the Page register, the read will be    *
                 * aborted, showing no Write acknowledge flag D1=1.                                            *
                 **********************************************************************************************/
                const char*  operation;
                const char*  progMode;
                int          cvData;
                Boolean      opsMode = False;
                int          cvNumber = 0;

                // progTask   = (progTaskMsg *) msgBuf;
                // slot - slot number for this request - slot 124 is programmer
                int pcmd   = msg[3];  // programmer command
                int pstat  = msg[4];  // programmer status error flags in reply message
                int hopsa  = msg[5];  // Ops mode - 7 high address bits of loco to program
                int lopsa  = msg[6];  // Ops mode - 7 low  address bits of loco to program
                /* trk - track status. Note: bit 3 shows if prog track is busy */
                int cvh    = msg[8];  // hi 3 bits of CV# and msb of data7
                int cvl    = msg[9];  // lo 7 bits of CV#
                int data7  = msg[10]; // 7 bits of data to program, msb is in cvh above

                cvData     =  (((cvh & CVH_D7) << 6) | (data7 & 0x7f));  // was PROG_DATA
                cvNumber   = (((((cvh & CVH_CV8_CV9) >> 3) | (cvh & CVH_CV7)) * 128)
                              + (cvl & 0x7f))+1;   // was PROG_CV_NUM(progTask)

                /* generate loco address, mixed mode or true 4 digit */
                mixedAdrStr = __convertToMixed(lopsa, hopsa);

                /* determine programming mode for printing */
                if ((pcmd & PCMD_MODE_MASK) == PAGED_ON_SRVC_TRK) {
                    progMode = "Byte in Paged Mode on Service Track";
                } else if ((pcmd & PCMD_MODE_MASK) == DIR_BYTE_ON_SRVC_TRK) {
                    progMode = "Byte in Direct Mode on Service Track";
                } else if ((pcmd & PCMD_MODE_MASK) == DIR_BIT_ON_SRVC_TRK) {
                    progMode = "Bits in Direct Mode on Service Track";
                } else if (((pcmd & ~PCMD_BYTE_MODE) & PCMD_MODE_MASK)
                           == REG_BYTE_RW_ON_SRVC_TRK) {
                    progMode = "Byte in Physical Register R/W Mode on Service Track";
                } else if ((pcmd & PCMD_MODE_MASK) == OPS_BYTE_NO_FEEDBACK) {
                    progMode = "Byte in OP's Mode (NO feedback)";
                    opsMode = True;
                } else if ((pcmd & PCMD_MODE_MASK) == OPS_BYTE_FEEDBACK) {
                    progMode = "Byte in OP's Mode";
                    opsMode = True;
                } else if ((pcmd & PCMD_MODE_MASK) == OPS_BIT_NO_FEEDBACK) {
                    progMode = "Bits in OP's Mode (NO feedback)";
                    opsMode = True;
                } else if ((pcmd & PCMD_MODE_MASK) == OPS_BIT_FEEDBACK) {
                    progMode = "Bits in OP's Mode";
                    opsMode = True;
                } else if (((pcmd & ~PCMD_BYTE_MODE) & PCMD_MODE_MASK)
                           == SRVC_TRK_RESERVED) {
                    progMode = "SERVICE TRACK RESERVED MODE DETECTED!";
                } else {
                    progMode = "Unknown mode; Could be IB-Com";
                }

                /* are we sending or receiving? */
                if ((pcmd & PCMD_RW) != 0) {
                    /* sending a command */
                    operation = "Programming Track: Write";

                    /* printout based on whether we're doing Ops mode or not */
                    if (opsMode) {
                        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                            "%s %s %s Setting CV%d of Loco %s to %d (0x%02X)",
                            mode,
                            operation,
                            progMode,
                            cvNumber,
                            mixedAdrStr, cvData, cvData );
                    } else {
                        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                            "%s %s %s Setting CV%d to %d (0x%02X)",
                            mode,
                            operation,
                            progMode,
                            cvNumber,
                            cvData, cvData );
                    }
                } else {
                    /* receiving a reply */
                    operation = "Programming Track: Read";

                    /* printout based on whether we're doing Ops mode or not */
                    if (opsMode) {
                        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                            "%s %s %s Setting CV%d of Loco %s to %d (0x%02X)",
                            mode,
                            operation,
                            progMode,
                            cvNumber,
                            mixedAdrStr, cvData, cvData );
                    } else {
                        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                            "%s %s %s Setting CV%d to %d (0x%02X)",
                            mode,
                            operation,
                            progMode,
                            cvNumber,
                            cvData, cvData );
                    }

                    /* if we're reading the slot back, check the status        */
                    /* this is supposed to be the Programming task final reply */
                    /* and will have the resulting status byte                 */

                    if (command == OPC_SL_RD_DATA) {
                        if ((pstat&0x0F) != 0) {
                            if ((pstat & PSTAT_USER_ABORTED) != 0) {
                                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Status = Failed, User Aborted" );
                            }

                            if ((pstat & PSTAT_READ_FAIL) != 0) {
                                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Status = Failed, Read Compare Acknowledge not detected");
                            }

                            if ((pstat & PSTAT_WRITE_FAIL) != 0 ) {
                                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Status = Failed, No Write Acknowledge from decoder");
                            }

                            if ((pstat & PSTAT_NO_DECODER) != 0 ) {
                                TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Status = Failed, Service Mode programming track empty");
                            }
                        } else {
                            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Status = Success" );
                        }
                    }
                }
                // end programming track block

            } else {
                /**************************************************
                 * normal slot read/write message - see info above *
                 **************************************************/

                if ((trackStatus != trk) || showTrackStatus) {
                    trackStatus = trk;
                    showStatus  = True;
                }

                if (showStatus) {
                    TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                        "%s slot=%d addr=%s, %s, %s, %s, dir=%s speed=%d fun=%s,%s,%s,%s,%s,%s,%s,%s,%s",
                        mode,
                        slot,
                        locoAdrStr,
                        CONSIST_STAT(stat),
                        LOCO_STAT(stat),
                        DEC_MODE(stat),
                        ((dirf & DIRF_DIR)!=0 ? "rev":"fwd"),
                        spd,
                        ((dirf & DIRF_F0) != 0 ? "on":"off"),
                        ((dirf & DIRF_F1) != 0 ? "on":"off"),
                        ((dirf & DIRF_F2) != 0 ? "on":"off"),
                        ((dirf & DIRF_F3) != 0 ? "on":"off"),
                        ((dirf & DIRF_F4) != 0 ? "on":"off"),
                        ((snd  & SND_F5)  != 0 ? "on":"off"),
                        ((snd  & SND_F6)  != 0 ? "on":"off"),
                        ((snd  & SND_F7)  != 0 ? "on":"off"),
                        ((snd  & SND_F8)  != 0 ? "on":"off")
                        );
                        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                            "track=%s power=%s PT=%s",
                            (trk & GTRK_IDLE)      ? "ON"   : "OFF",
                            (trk & GTRK_POWER)     ? "UP"   : "DOWN",
                            (trk & GTRK_PROG_BUSY) ? "busy" : "avail" );
/*
                        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                            "CS %s",((trk  & GTRK_MLOK1) !=0 ? "implements LocoNet 1.1" : "is a DT-200") );
                        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                            "Programming Track is %s",((trk  & GTRK_PROG_BUSY) != 0 ? "Busy" : "Available") );
                        TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                            "SS2=0x%02X, ID=%s",ss2, __idString(id1, id2));
*/
                } else {
                    TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                        "%s slot %d Loco %s is %s, %s, operating in %s SS mode, and is going %s at speed %d\n\tF0=%s F1=%s F2=%s F3=%s F4=%s F5=%s F6=%s F7=%s F8=%s",
                        mode,
                        slot,
                        locoAdrStr,
                        CONSIST_STAT(stat),
                        LOCO_STAT(stat),
                        DEC_MODE(stat),
                        ((dirf & DIRF_DIR)!=0 ? "in Reverse" : "Foward"),
                        spd,
                        ((dirf & DIRF_F0) != 0 ? "On, "  : "Off,"),
                        ((dirf & DIRF_F1) != 0 ? "On, "  : "Off,"),
                        ((dirf & DIRF_F2) != 0 ? "On, "  : "Off,"),
                        ((dirf & DIRF_F3) != 0 ? "On, "  : "Off,"),
                        ((dirf & DIRF_F4) != 0 ? "On, "  : "Off,"),
                        ((snd  & SND_F5) != 0 ? "On, "  : "Off,"),
                        ((snd  & SND_F6) != 0 ? "On, "  : "Off,"),
                        ((snd  & SND_F7) != 0 ? "On, "  : "Off,"),
                        ((snd  & SND_F8) != 0 ? "On"    : "Off")
                        );
                }
                // end normal slot read/write case
            }

            // end of OPC_WR_SL_DATA, OPC_SL_RD_DATA case
            break;
        } // end case


    case 0xEE:
    case 0xE6:
        if (msg[1]==0x10) {
            // ALM read and write messages
            char* msg3[4] = {"(ID)","","(RD)","(WR)"};

            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                "%s %d ATASK=%d %s BLKL=%d BLKH=%d LOGIC=%d\nARG1L=0x%02X ARG1H=0x%02X ARG2L=0x%02X ARG2H=0x%02X ARG3L=0x%02X ARG3H=0x%02X ARG4L=0x%02X ARG4H=0x%02X",
                (msg[0]==0xEE?"Write ALM msg":"Read ALM msg (Write reply)"),
                msg[2], msg[3], msg3[msg[3]&0x03], msg[4], msg[5], msg[6],
                msg[7],msg[8],msg[9],msg[10],msg[11],msg[12],msg[13],msg[14]);

        } else if (msg[1] == 0x15) {
            // write extended master message
            if (msg[0]==0xEE)
              TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Write extended slot: ");
            else
              TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Read extended slot (Write reply): ");

        } else
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                "0XE6/0xEE message with unexpected length %d", msg[1]);
        break;

    case 0xE5:
        // there are several different formats for 0xE5 messages, with
        // the length apparently the distinquishing item.
        switch (msg[1]) {
        case 0x07: {
            /* IB-Com */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "IB-Com PT command; ON/OFF" );
            break;
        }
        case 0x10: {
            __peerToPeerMessage(msg);
            break;
        }
        case 0x0A: {
            // throttle status
            int tcntrl = msg[2];
            const char* stat;
            if (tcntrl==0x40) stat = " (OK) ";
            else if (tcntrl==0x7F) stat = " (no key, immed, ignored) ";
            else if (tcntrl==0x43) stat = " (+ key during msg) ";
            else if (tcntrl==0x42) stat = " (- key during msg) ";
            else if (tcntrl==0x41) stat = " (R/S key during msg, aborts) ";
            else stat=" (unknown) ";

            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                "Throttle status TCNTRL=%02X %s ID1,ID2=%02X,%02X SLA=%02X SLB=%02X", tcntrl,
                stat, msg[3], msg[4], msg[7], msg[8] );
            break;
        }
        case 0x0F: {
          if( msg[5] == 0x11 ) {
            /* IB response s88 status */
            /* E5 0F 00 49 4B 11 06 05 7F 7F 00 00 0A 44 4B */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
              "IB s88 module %d reports 1-8=0x%02X 9-16=0x%02X PXCT1=0x%02X", msg[7]+1, msg[12], msg[13], msg[6] );
          }
          else if( msg[5] == 0x1F ) {
            /* Uhlenbrock LocoNet module response */
            /* E5 0F 05 49 4B 1F 01 3F 18 00 00 01 00 00 2A */
            int type, addr, cv, val;
            int iSet = evaluateLNCV( msg, &type, &addr, &cv, &val);
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
              "LNCV %s response type=%d, cv%d=%d", iSet?"SET":"GET", type, cv, val );
          }
          break;
        }

        default: {
            // 0xE5 message of unknown format
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Message with opcode 0xE5 and unknown format" );
            break;

        }
        } // end of 0xE5 switch statement
        break;


        /***********************************************************************************
         *                  0xE4   ;                                                        *
         *                         ;                                                        *
         *                         ; <0xE4>,<0x09>,...                                      *
         ***********************************************************************************/
    case 0XE4: {
        switch (msg[1]) {
        case 0x08 : {  // Format LISSY message
          int unit = (msg[4]&0x7F);
          if( msg[2] == 0 ) {
            int address = (msg[6]&0x7F)+128*(msg[5]&0x7F);
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                "Lissy %d: Loco %d moving %s", unit, address, ((msg[3]&0x20)==0 ? "north":"south") );
          }
          else if( msg[2] == 0x01 ) {
            int wc = (msg[6]&0x7F)+128*(msg[5]&0x7F);
            unit = (msg[4] & 0x7F) + (128 * ( msg[3] & 0x7F ));
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                "Wheel counter %d = [%d] ", unit, wc );
          }
          break;
        }

        case 0x0A : {  // Format special message
            int element = msg[2]*128+msg[3];
            int stat1 = msg[5];
            int stat2 = msg[6];
            char* status;
            char* status1;
            char* status2;
            if ( (stat1&0x10) !=0 )
                if ( (stat1&0x20) !=0 )
                    status = " AX, XA reserved; ";
                else
                    status = " AX reserved; ";
            else
                if ( (stat1&0x20) !=0 )
                    status = " XA reserved; ";
                else
                    status = " no reservation; ";
            if ( (stat2&0x01) !=0 )
              status1="Turnout thrown; ";
            else
              status1="Turnout closed; ";
            if ( (stat1&0x01) !=0 )
              status2="Occupied";
            else
              status2="Not occupied";
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                "SE%d (%d) reports AX:%d XA:%d %s%s%s",(element+1), element, msg[7], msg[8], status, status1, status2 );
            break;
        }

        default :
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Unrecognized command varient");
            break;
        }
        break;
    }
    break;

        /**************************************************************************
         * OPC_IMM_PACKET   0xED   ;SEND n-byte packet immediate LACK              *
         *                         ; Follow on message: LACK                       *
         *                         ; <0xED>,<0B>,<7F>,<REPS>,<DHI>,<IM1>,<IM2>,    *
         *                         ;        <IM3>,<IM4>,<IM5>,<CHK>                *
         *                         ;   <DHI>=<0,0,1,IM5.7-IM4.7,IM3.7,IM2.7,IM1.7> *
         *                         ;   <REPS>  D4,5,6=#IM bytes,                   *
         *                         ;           D3=0(reserved);                     *
         *                         ;           D2,1,0=repeat CNT                   *
         *                         ; IF Not limited MASTER then                    *
         *                         ;   LACK=<B4>,<7D>,<7F>,<chk> if CMD ok         *
         *                         ; IF limited MASTER then Lim Masters respond    *
         *                         ;   with <B4>,<7E>,<lim adr>,<chk>              *
         *                         ; IF internal buffer BUSY/full respond          *
         *                         ;   with <B4>,<7D>,<0>,<chk>                    *
         *                         ;   (NOT IMPLEMENTED IN DT200)                  *
         **************************************************************************/
    case OPC_IMM_PACKET: {           /* page 11 of Loconet PE */
        //sendPkt = (sendPktMsg *) msgBuf;
        int val7f = msg[2];         /* fixed value of 0x7f                                  */
        int reps  = msg[3];         /* repeat count                                         */
        int dhi   = msg[4];         /* high bits of data bytes                              */
        int im1   = msg[5];
        int im2   = msg[6];
        int im3   = msg[7];
        int im4   = msg[8];
        int im5   = msg[9];

        if( msg[1] == 0x1F ) {
          TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "IB-Com PT command" );
          break;
        }

        /* see if it really is a 'Send Packet' as defined in Loconet PE */
        if (val7f == 0x7f) {
            int len = ((reps & 0x70) >> 4);
            byte packet[32];
            /* it is */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
                "Send packet immediate: %d bytes, repeat count %d\n\tDHI=0x%02X, IM1=0x%02X, IM2=0x%02X, IM3=0x%02X, IM4=0x%02X, IM5=0x%02X",
                ((reps & 0x70) >> 4), (reps & 0x07), dhi, im1, im2, im3, im4, im5 );

        }
        /* s88 bus on IB request status: */
        /* ED 0F 01 49 42 13 00 05 00 00 00 00 00 00 01 */
        else if( val7f == 0x01 && reps == 0x49 && dhi == 0x42 && im1 == 0x13 ) {
          TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Request s88 module status %d", im3+1 );
        }
        else if( im1 == UB_LNCVGET || im1 == UB_LNCVSET ) {
          int type, addr, cv, val;
          evaluateLNCV( msg, &type, &addr, &cv, &val);
          TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999,
              "LNCV %s command type=%d, cv%d=%d", im1 == UB_LNCVGET ? "GET":"SET", type, cv, val );
        }
        else {
            /* Hmmmm... */
            TraceOp.trc( "lnmon", TRCLEVEL_MONITOR, __LINE__, 9999, "Weird Send Packet Immediate, 3rd byte id 0x%02X not 0x7f",val7f );
        }
        break;
    }
    break;

    default:
        TraceOp.trc( "lnmon", TRCLEVEL_DEBUG, __LINE__, 9999, "Command 0x%02X is not defined in Loconet Personal Use Edition 1.0", msg[0] );

    }  // end switch over opcode type - default handles unrecognized cases, so can't reach here
}  // end of format() member function


