/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>
 http://www.rocrail.net

 This program is free software you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef CBUSCONST_H_
#define CBUSCONST_H_

    #define SERVICE_HANDLE 255


    #define DEFAULT_STANDARD_ID 0x7a
    #define DEFAULT_EXTENDED_ID 0x7a

    /**
     * CBUS Opcodes
     */
    // Opcodes with no data
    #define CBUS_HLT 0x02
    #define CBUS_BON 0x03
    #define CBUS_TOF 0x04
    #define CBUS_TON 0x05
    #define CBUS_ESTOP 0x06
    #define CBUS_ARST 0x07
    #define CBUS_RTOF 0x08
    #define CBUS_RTON 0x09
    #define CBUS_RESTP 0x0A

    #define CBUS_RDPAR 0x10

    // Opcodes with 1 data
    #define CBUS_KLOC 0x21

    // Opcodes with 2 data
    #define CBUS_RLOC 0x40
    #define CBUS_SNN 0x42
    #define CBUS_STMOD 0x44
    #define CBUS_PCON 0x45
    #define CBUS_DSPD 0x47
    #define CBUS_DFLG 0x48
    #define CBUS_SSTAT 0x4C

    #define CBUS_NNACK 0x50
    #define CBUS_NNREL 0x51
    #define CBUS_NNREF 0x52
    #define CBUS_NNLRN 0x53
    #define CBUS_NNULN 0x54
    #define CBUS_NNCLR 0x55
    #define CBUS_NNEVN 0x56
    #define CBUS_BOOT 0x5C

    // Opcodes with 3 data
    #define CBUS_DFUN 0x60
    #define CBUS_ERR 0x63

    #define CBUS_ENNLF 0x70
    #define CBUS_NVRD 0x71

    // Opcodes with 4 data
    #define CBUS_RDCC3 0x80
    #define CBUS_WCVO 0x82
    #define CBUS_WCVB 0x83
    #define CBUS_QCVS 0x84
    #define CBUS_PCVS 0x85

    #define CBUS_ACON 0x90
    #define CBUS_ACOF 0x91
    #define CBUS_AREQ 0x92

    #define CBUS_EVRD 0x94
    #define CBUS_EVULN 0x95
    #define CBUS_NVSET 0x96
    #define CBUS_NVANS 0x97

    #define CBUS_ASON 0x98
    #define CBUS_ASOF 0x99

    // OPcodes with 5 data
    #define CBUS_WCVS 0xA2

    #define CBUS_ACON1 0xB0
    #define CBUS_ACOF1 0xB1

    // Opcodes with 6 data
    #define CBUS_WCVOA 0xC1

    #define CBUS_EVLRN 0xD2
    #define CBUS_EVANS 0xD3

    // Opcodes with 7 data
    #define CBUS_PLOC 0xE1

    /**
     * Programming modes
     */
    #define CBUS_PROG_DIRECT_BYTE 0
    #define CBUS_PROG_DIRECT_BIT 1
    #define CBUS_PROG_PAGED 2
    #define CBUS_PROG_REGISTER 3
    #define CBUS_PROG_ADDRESS 4
    #define CBUS_OPS_BYTE 5

    /**
     * Error codes returned by CBUS_ERR
     */
    #define ERR_ADDR_FULL 1
    #define ERR_ADDR_TAKEN 2
    #define ERR_SESS_LOCO_NOT_FOUND 3
    #define ERR_NO_MORE_ENGINES 4
    #define ERR_ENGINE_NOT_FOUND 5

    /**
     * Status codes for OPC_SSTAT
     */
    #define SSTAT_NO_ACK 1
    #define SSTAT_OVLD 2
    #define SSTAT_WR_ACK 3
    #define SSTAT_BUSY 4
    #define SSTAT_CV_ERROR 5

    /**
     * Event types
     */
    #define EVENT_ON 0
    #define EVENT_OFF 1
    #define EVENT_EITHER 2

    /**
     * CBUS Priorities
     */
    #define PRIORITY_HIGH 0
    #define PRIORITY_ABOVE 1
    #define PRIORITY_NORMAL 2
    #define PRIORITY_LOW 3

    /**
     * Event Table
     */
    #define MAX_TABLE_EVENTS 5000

    /**
     * Function bits for group1
     */
    #define CBUS_F0 16
    #define CBUS_F1 1
    #define CBUS_F2 2
    #define CBUS_F3 4
    #define CBUS_F4 8

    /**
     * Function bits for group2
     */
    #define CBUS_F5 1
    #define CBUS_F6 2
    #define CBUS_F7 4
    #define CBUS_F8 8

    /**
     * Function bits for group3
     */
    #define CBUS_F9 1
    #define CBUS_F10 2
    #define CBUS_F11 4
    #define CBUS_F12 8

    /**
     * Throttle modes
     */
    #define CBUS_SS_128 0
    #define CBUS_SS_14 1
    #define CBUS_SS_28_INTERLEAVE 2
    #define CBUS_SS_28 3


#endif /* CBUSCONST_H_ */
