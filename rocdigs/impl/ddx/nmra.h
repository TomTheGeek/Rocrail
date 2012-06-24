/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
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
#ifndef __IMPL_DDX_NMRA_H__
#define __IMPL_DDX_NMRA_H__

/**********************************************************************
 data format:

 look at the srcp specification

 protocol formats:

 NB: NMRA baseline decoder
 (implemented)

 N1: NMRA 4-function decoder, 7-bit address, 28 speed steps
 (implemented)

 N2: NMRA 4-function decoder, 7-bit address, 128 speed steps
 (implemented)

 N3: NMRA 4-function decoder, 14-bit address, 28 speed steps
 (implemented)

 N4: NMRA 4-function decoder, 14-bit address, 128 speed steps
 (implemented)

 NA: accessory digital decoders
 (implemented)

 service mode instruction packets for direct mode
 (verify cv, write cv, cv bit manipulation)
 (implemented)

 service mode instruction packets for adress-only mode
 (verify address contents, write address contents)
 (NOT implemented)

 service mode instruction packets for physical register addressing
 (verify register contents, write register contents)
 (implemented)

 service mode instruction packets paged cv addressing
 (verify/write register/cv contents)
 (NOT implemented)

 general notes:

   configuration of the serial port:

      startbit: 1
      stopbit : 1
      databits: 8
      baudrate: 19200

      ==> one serial bit takes 52.08 usec.

      ==> NMRA-0-Bit: 01         (52 usec low and 52 usec high)
          NMRA-1-Bit: 0011       (at least 100 usec low and high)

      serial stream (only start/stop bits):

      0_______10_______10_______10_______10_______10_______10___ ...

      problem: how to place the NMRA-0- and NMRA-1-Bits in the serial stream

      examples:

      0          0xF0     _____-----
      00         0xC6     __--___---
      01         0x78     ____----_-
      10         0xE1     _-____----
      001        0x66     __--__--_-
      010        0x96     __--_-__--
      011        0x5C     ___---_-_-
      100        0x99     _-__--__--
      101        0x71     _-___---_-
      110        0xC5     _-_-___---
      0111       0x56     __--_-_-_-
      1011       0x59     _-__--_-_-
      1101       0x65     _-_-__--_-
      1110       0x95     _-_-_-__--
      11111      0x55     _-_-_-_-_-
                          ^        ^
                          start-   stop-
                          bit      bit

   known bugs (of version 1 of the nmra dcc translation routine):
   (i hope version 2 don't have these bugs ;-) )

      following packets are not translateable:

        N1 031 1 06 0 0 0 0 0
        N1 047 0 07 0 0 0 0 0

        N2 031 0 091 0 0 0 0 0
        N2 031 1 085 0 0 0 0 0
        N2 031 1 095 0 0 0 0 0
        N2 047 0 107 0 0 0 0 0
        N2 047 1 103 0 0 0 0 0
        N2 047 1 111 0 0 0 0 0
        N2 048 1 112 0 0 0 0 0
        N2 051 1 115 0 0 0 0 0
        N2 053 1 117 0 0 0 0 0
        N2 056 0 124 0 0 0 0 0
        N2 057 1 113 0 0 0 0 0
        N2 058 1 114 0 0 0 0 0
        N2 059 1 115 0 0 0 0 0
        N2 060 1 116 0 0 0 0 0
        N2 061 1 117 0 0 0 0 0
        N2 062 1 118 0 0 0 0 0

     I think, that these are not really problems. The only consequence is
     e.g. that some addresses has 127 speed steps instead of 128. Thats
     life, don't worry.

     New: completely new algorithm to generate the nmra packet stream
     (i call it 'version 3' of the translate routines)

     The idea in this approach to generate nmra patterns is, to split the
     starting and ending bit in each pattern and share it with the next
     pattern. Therefore the patterns, which could be generated, are coded with
     h, H, l and L, lowercase describing the half bits. The longest possible
     pattern contains two half h bits and four H bits. For the access into
     the coding table, the index of course doesn't differentiate between half
     and full bits, because the first one is always half and the last one
     determined by this table. This table shows, which bit pattern will be
     replaced by which pattern on the serial line. There is only one pattern
     left, which could not be directly translated. This pattern starts with
     an h, so we have to look at the patterns, which end with an h, if we want
     to avoid this pattern. All of the patterns, we access in the first try,
     contain an l or an L, which could be enlarged, to get rid of at least on
     bit in this pattern and don't have our problem in the next pattern. With
     the only exception of hHHHHh, but this pattern simply moves our problem
     into one byte before or upto the beginning of the sequence. And there, we
     could always add a bit. So we are sure, to be able, to translate any
     given sequence of bits.

    Because only the case of hHHHHh realy requires 6 bits, the translationtable
    could be left at 32 entries. The other case, which has the same first five
    bits is our problem, so we have to handle it seperatly anyway.

    Of course the resulting sequence is not dc free. But if this is required,
    one could replace the bytes in the TranslateData by an index in another
    table which holds for each data containing at least an L or two l
    replacements with different dc components. This way one could get a dc
    free signal or at will a signal with a given dc part.

    #define ll          0xf0	_____-----

    #define lLl         0xcc	___--__---		000000  000001  000010 000011 000100 000101 000110  000111

    #define lHl         0xe8	____-_----

    #define lHLl        0x9a	__-_--__--		010000  010001  010010  010011

    #define lLHl        0xa6	__--__-_--		001000  001001  001010  001011

    #define lHHl        0xd4	___-_-_---		011000  011001  011010  011011

    #define lHHHl       0xaa	__-_-_-_--



    #define lh          0x00	_________-

    #define lLh         0x1c	___---___-

    #define lHh         0x40	_______-_-

    #define lLHh        0x4c	___--__-_-		001100  001101  001110  001111

    #define lHLh        0x34	___-_--__-		010100  010101  010110  010111

    #define lHHh        0x50	_____-_-_-		011100  011101

    #define lHHHh       0x54	___-_-_-_-		011110  011111



    #define hLh         0x0f	_----____-

    #define hHLh        0x1d	_-_---___-		110100  110101

    #define hLHh        0x47	_---___-_-		101100  101101

    #define hHHLh       0x35	_-_-_--__-		111010  111011

    #define hHLHh       0x4d	_-_--__-_-		110110  110111

    #define hLHHh       0x53	_--__-_-_-      101110  101111

    #define hHHHHh      0x55	_-_-_-_-_-		111111



    #define hl          0xff	_---------

    #define hLl         0xc7	_---___---		100000  100001  100010  100011  100100  100101  100110  100111

    #define hHl         0xfd	_-_-------

    #define hHLl        0xcd	_-_--__---		110000  110001  110010  110011

    #define hLHl        0xD3       	_--__-_---      101000  101001  101010  101011

    #define hHHl        0xF5       	_-_-_-----      111000  111001

    #define hHHHl       0xd5	_-_-_-_---      111100  111101

    not directly translatetable     111110

****************************************************************/

#define ll          0xf0
#define lLl         0xcc
#define lHl         0xe8
#define lHLl        0x9a
#define lLHl        0xa6
#define lHHl        0xd4
#define lHHHl       0xaa
#define lh          0x00
#define lLh         0x1c
#define lHh         0x40
#define lLHh        0x4c
#define lHLh        0x34
#define lHHh        0x50
#define lHHHh       0x54
#define hLh         0x0f
#define hHLh        0x1d
#define hLHh        0x47
#define hHHLh       0x35
#define hHLHh       0x4d
#define hLHHh       0x53
#define hHHHHh      0x55
#define hl          0xff
#define hLl         0xc7
#define hHl         0xfd
#define hHLl        0xcd
#define hLHl        0xD3
#define hHHl        0xF5
#define hHHHl       0xd5


int translateBitstream2Packetstream(char *Bitstream, char *Packetstream);

/* signal generating functions for nmra dcc */

/* NMRA standard decoder      */
int comp_nmra_baseline(int address, int direction, int speed);

/* 4-func,7-bit-addr,28 sp-st.*/
int comp_nmra_f4b7s28(int address, int direction, int speed, int* f);

/* 4-func,7-bit-addr,128 sp-st*/
int comp_nmra_f4b7s128(int address, int direction, int speed, int* f);

/* 4-func,14-bit-addr,28 sp-st*/
int comp_nmra_f4b14s28(int address, int direction, int speed, int* f);

/* 4-func,14-bit-addr,128 sp-st*/
int comp_nmra_f4b14s128(int address, int direction, int speed, int* f);

/* NMRA accessory decoder     */
int comp_nmra_accessory(int addr, int port, int gate, int activate);

/* service mode functions */
int protocol_nmra_sm_write_cvbyte(obj inst, int cv, int value, int pom);
int protocol_nmra_sm_verify_cvbyte(obj inst, int cv, int value);

int comp_nmra_fb7(int address, int group, int* f);
int comp_nmra_fb14(int address, int group, int* f);


#endif
