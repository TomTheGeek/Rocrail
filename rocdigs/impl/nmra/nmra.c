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

/* http://www.nmra.org/standards/DCC/standards_rps/rp923.html */
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


#include "nmra.h"

#include "rocs/public/trace.h"
#include "rocs/public/system.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
   char *pattern;
   int  patternlength;
   int  value;
} tTranslateData;

typedef struct {
   int  value;
   int  patternlength;
} tTranslateData_v3;

static const tTranslateData TranslateData[] = {
    { "0"    , 1, 0xF0 },
    { "00"   , 2, 0xC6 },
    { "01"   , 2, 0x78 },
    { "10"   , 2, 0xE1 },
    { "001"  , 3, 0x66 },
    { "010"  , 3, 0x96 },
    { "011"  , 3, 0x5C },
    { "100"  , 3, 0x99 },
    { "101"  , 3, 0x71 },
    { "110"  , 3, 0xC5 },
    { "0111" , 4, 0x56 },
    { "1011" , 4, 0x59 },
    { "1101" , 4, 0x65 },
    { "1110" , 4, 0x95 },
    { "11111", 5, 0x55 }
};

// number of translatable patterns
static int DataCnt = sizeof (TranslateData) / sizeof (TranslateData[0]);

static const tTranslateData_v3 TranslateData_v3[32][2] = {
    {{ lLl   , 2  },{ ll   , 1  }},
    {{ lLl   , 2  },{ ll   , 1  }},
    {{ lLl   , 2  },{ ll   , 1  }},
    {{ lLl   , 2  },{ ll   , 1  }},
    {{ lLHl  , 3  },{ lLh  , 2  }},
    {{ lLHl  , 3  },{ lLh  , 2  }},
    {{ lLHh  , 3  },{ lLh  , 2  }},
    {{ lLHh  , 3  },{ lLh  , 2  }},
    {{ lHLl  , 3  },{ lHl  , 2  }},
    {{ lHLl  , 3  },{ lHl  , 2  }},
    {{ lHLh  , 3  },{ lHl  , 2  }},
    {{ lHLh  , 3  },{ lHl  , 2  }},
    {{ lHHl  , 3  },{ lHh  , 2  }},
    {{ lHHl  , 3  },{ lHh  , 2  }},
    {{ lHHh  , 3  },{ lHh  , 2  }},
    {{ lHHHh , 4  },{ lHHh , 3  }},
    {{ hLl   , 2  },{ hl   , 1  }},
    {{ hLl   , 2  },{ hl   , 1  }},
    {{ hLl   , 2  },{ hl   , 1  }},
    {{ hLl   , 2  },{ hl   , 1  }},
    {{ hLHl  , 3  },{ hLh  , 2  }},
    {{ hLHl  , 3  },{ hLh  , 2  }},
    {{ hLHh  , 3  },{ hLh  , 2  }},
    {{ hLHHh , 4  },{ hLHh , 3  }},
    {{ hHLl  , 3  },{ hHl  , 2  }},
    {{ hHLl  , 3  },{ hHl  , 2  }},
    {{ hHLh  , 3  },{ hHl  , 2  }},
    {{ hHLHh , 4  },{ hHLh , 3  }},
    {{ hHHl  , 3  },{ hHHl , 3  }},
    {{ hHHLh , 4  },{ hHHl , 3  }},
    {{ hHHHl , 4  },{ hHHHl, 4  }},
    {{ hHHHHh, 5  },{ hHHHHh, 5 }}
};

static char *preamble = "111111111111111";
static const int BUFFERSIZE = 360;


#define PKTSIZE 60
#define MAXDATA 52
static char idle_data[MAXDATA];
static char NMRA_idle_data[PKTSIZE];
static Boolean IdleInit = False;
static int IdlePacketSize = 0;

static int translateabel(char *bs) {     /* the result is only an index, no warranty */
   int  i,size;
   char *pbs;
   size=strlen(bs);
   for (i=(DataCnt-1); i>=0; i--) {
      pbs=bs+(size-TranslateData[i].patternlength);
      if (strcmp(pbs, TranslateData[i].pattern)==0)
         return 1;
   }
   return 0;
}

static int read_next_six_bits (char *Bitstream) {

   int i, bits = 0;
   for (i = 0; i < 6; i++)
      bits = (bits << 1) | (*Bitstream++ == '0' ? 0 : 1);
   return bits;
}


static int translateBitstream2Packetstream(char *Bitstream, char *Packetstream) {

   /* this routine assumes, that any Bitstream starts with a 1 Bit. */
   /* This could be changed, if necessary */

   char Buffer[BUFFERSIZE+20];  /* keep room for additional pre and postamble */
   char *read_ptr = Buffer+1;   /* here the real sequence starts */

   char *restart_read = Buffer; /* one more 1 in the beginning for successful restart */
   char *last_restart = Buffer-1; /* this necessary, only to verify our assumptions */

   char *buf_end;

   int  restart_packet = 0;
   int  generate_packet = 0;

   int  second_try = False;
   int  act_six;

   read_ptr = strcpy (Buffer, "11");

   /* one bit, because we start with a half-bit, so we have to put in the left half */
   /* one bit, to be able, to back up one bit, if we run into a 111110 pattern */

   strncat (Buffer, Bitstream, BUFFERSIZE-1);

   buf_end = Buffer + strlen (Buffer);  /* for simply testing, whether our job is done */

   strcat (Buffer, "111111");

   /* at most six trailing bits are possibly necessary */

   memset(Packetstream, 0, PKTSIZE);

   while (generate_packet < PKTSIZE && read_ptr < buf_end) {
      act_six = read_next_six_bits (read_ptr);
      if (act_six == 0x3e /* 111110*/){/*did we reach an untranslateble value */
          /* try again from last position, where a shorter translation */
          /* could be choosen                                          */
          second_try = True;
          generate_packet = restart_packet;
          if (restart_read == last_restart)
          TraceOp.trc( __FILE__, TRCLEVEL_WARNING, __LINE__, 9999, "sorry, restart algorithm doesn't work as expected for NMRA-Packet %s", Bitstream);
          last_restart = restart_read;
          read_ptr = restart_read;
          act_six = read_next_six_bits (read_ptr);
      }

      Packetstream[generate_packet] = TranslateData_v3[act_six >> 1][second_try ? 1 : 0]. value;

      if (act_six < 0x3e /* 111110*/) { /* is translation fixed upto here ? */
         restart_packet = generate_packet;
         restart_read = read_ptr;
      }
      read_ptr += TranslateData_v3[act_six >> 1][second_try ? 1 : 0]. patternlength;
      generate_packet ++;
      second_try = False;
   }

   return generate_packet;             /* return nr of bytes in packetstream */
}


int idlePacket(char* packetstream, Boolean longIdle) {
  int i,j;
  int error;
  char idle_packet[] = "11111111111111101111111100000000001111111110";
  char idle_pktstr[PKTSIZE];

  if( !IdleInit ) {
    j = translateBitstream2Packetstream(idle_packet, idle_pktstr);
    IdlePacketSize = j;
  
    /* generate idle_data */
    for (i=0; i<MAXDATA; i++)
      idle_data[i]=idle_pktstr[i % j];
    for (i=(MAXDATA/j)*j; i<MAXDATA; i++)
      idle_data[i]=0xC6;
    memcpy(NMRA_idle_data,idle_pktstr,j);
    
    IdleInit = True;
  }

  if( longIdle )
    memcpy(packetstream, idle_data, MAXDATA);
  else
    memcpy(packetstream, NMRA_idle_data, IdlePacketSize);

  return longIdle ? MAXDATA:IdlePacketSize;

}


/*** some useful functions to calculate NMRA-DCC bytes (char arrays) ***/

static void calc_7bit_address_byte(char *byte, int address) {
   /* calculating address byte: 0AAAAAAA */

   int i,j;

   memset(byte, 0, 9);
   byte[0]='0';
   for (i=7; i>0; i--) {
      j=address%2;
      address=address/2;
      switch (j) {
         case 0: byte[i]='0'; break;
         case 1: byte[i]='1'; break;
      }
   }
}

static void calc_14bit_address_byte(char *byte1, char *byte2, int address) {
   /* calculating address bytes: 11AAAAAA AAAAAAAA */

   int i,j;

   memset(byte1, 0, 9);
   memset(byte2, 0, 9);
   byte1[0]='1';
   byte1[1]='1';
   for (i=13; i>=0; i--) {
      j=address%2;
      address=address/2;
      if (i>=6) {
         switch (j) { // set bit 7 to bit 0 of addr-byte 2
            case 0: byte2[i-6]='0'; break;
            case 1: byte2[i-6]='1'; break;
         }
      }
      else {
         switch (j) { // set bit 7 to bit 2 of addr-byte 1
            case 0: byte1[2+i]='0'; break;
            case 1: byte1[2+i]='1'; break;
         }
      }
   }
}

static void calc_baseline_speed_byte(char *byte, int direction, int speed) {
   /* calculating speed byte2: 01DUSSSS  */

   int i,j;

   memset(byte, 0, 9);
   byte[0]='0';  byte[1]='1';
   byte[3]='1';
   if (direction==1) byte[2]='1'; else byte[2]='0';

   for (i=7; i>3; i--) {
      j=speed%2;
      speed=speed/2;
      switch (j) {
         case 0: byte[i]='0'; break;
         case 1: byte[i]='1'; break;
      }
   }
}

static void calc_28spst_speed_byte(char *byte, int direction, int speed) {
   /* calculating speed byte: 01DSSSSS */

   int i,j;

   memset(byte, 0, 9);
   byte[0]='0';  byte[1]='1';
   if (direction == 1) byte[2]='1'; else byte[2]='0';
   if (speed>1) {
      if (speed % 2 == 1)  {
         byte[3]='1';
         speed = (speed+1) / 2;
      }
      else {
         byte[3]='0';
         speed = (speed+2) / 2;
      }
   }
   else {
      byte[3]='0';
   }
   for (i=7; i>3; i--) {
      j=speed%2;
      speed=speed/2;
      switch (j) {
         case 0: byte[i]='0'; break;
         case 1: byte[i]='1'; break;
      }
   }
}

/**
 * Function Group One Instruction (100)[5]
 * The format of this instruction is 100DDDDD
 * Up to 5 auxiliary functions (functions FL and F1-F4) can be controlled by the
 * Function Group One instruction.  Bits 0-3  shall define the value of
 * functions F1-F4 with function F1 being controlled by bit 0 and function F4 being
 * controlled by bit 3. A value of "1" shall indicate that the function is "on"
 * while a value of "0" shall indicate that the function is "off".  If Bit 1 of CV#29
 * has a value of one (1), then bit 4 controls function FL, otherwise bit 4 has no meaning.
 *
 *
 * Function Group Two Instruction (101)[6]
 * This instruction has the format 101SDDDD
 * Up to 8 additional auxiliary functions (F5-F12) can be controlled by a Function
 * Group Two instruction.  Bit 4 defines the use of Bits 0-3.  When bit 4 (S) is '1',
 * Bits 0-3 (DDDD) shall define the value of functions F5-F8 with function F5 being
 * controlled by bit 0 and function F8 being controlled by bit 3. When bit 4 (S) is '0',
 * Bits 0-3 (DDDD) shall define the value of functions F9-F12 with function F9 being
 * controlled by bit 0 and function F12 being controlled by bit 3.  A value of "1"
 * shall indicate that the function is "on" while a value of "0" shall indicate that the
 * function is "off".
 *
 * Feature Expansion Instruction (110)
 * The format of two byte instructions in this group is:   110CCCCC 0 DDDDDDDD.
 * The format of three byte instructions in this group is: 110CCCC 0 DDDDDDDD 0 DDDDDDDD
 *
 * CCCCC = 11110:  F13-F20 Function Control
 * The least significant bit (Bit 0) controlling F13, and the most significant bit (bit 7) controlling F20.
 *
 * CCCCC = 11111:  F21-F28 Function Control
 * The least significant bit (Bit 0) controlling F21, and the most significant bit (bit 7) controlling F28.
 */
static void calc_function_group(char* byte1, char* byte2, int group, Boolean f[]) {
   /* calculating function byte:
    * group 0 = f0-f4, 1 = f5-f8, 2 = f9-12, 3 = f13-16, 4 = f17-20, 5 = f21-24, 6 = f25-28
    */

  if( group > 0 )
    group--; /* function group from Rocview starts with 1 */
  TraceOp.trc( "nmra", TRCLEVEL_BYTE, __LINE__, 9999,"function group %d", group);

  if( byte2 == NULL && group > 2 ) {
    TraceOp.trc( "nmra", TRCLEVEL_WARNING, __LINE__, 9999,"no byte reserved for function group %d", group);
    return;
  }

   if( group == 0 ) {
     byte1[0] ='1';
     byte1[1] ='0';
     byte1[2] ='0';
     byte1[3] = f[0] ? '1':'0'; /* F0*/
     byte1[4] = f[4] ? '1':'0';
     byte1[5] = f[3] ? '1':'0';
     byte1[6] = f[2] ? '1':'0';
     byte1[7] = f[1] ? '1':'0';
     byte1[8] = 0;
   }
   else if( group == 1 ) {
     byte1[0] = '1';
     byte1[1] = '0';
     byte1[2] = '1';
     byte1[3] = '1'; /* F5-F8*/
     byte1[4] = f[8] ? '1':'0';
     byte1[5] = f[7] ? '1':'0';
     byte1[6] = f[6] ? '1':'0';
     byte1[7] = f[5] ? '1':'0';
     byte1[8] = 0;
   }
   else if( group == 2 ) {
     byte1[0] = '1';
     byte1[1] = '0';
     byte1[2] = '1';
     byte1[3] = '0'; /* F9-F12*/
     byte1[4] = f[12] ? '1':'0';
     byte1[5] = f[11] ? '1':'0';
     byte1[6] = f[10] ? '1':'0';
     byte1[7] = f[ 9] ? '1':'0';
     byte1[8] = 0;
   }
   else if( group == 3 || group == 4 ) {
     byte1[0] = '1';
     byte1[1] = '1';
     byte1[2] = '0';
     byte1[3] = '1';
     byte1[4] = '1';
     byte1[5] = '1';
     byte1[6] = '1';
     byte1[7] = '0';
     byte1[8] = 0;
     byte2[0] = f[20] ? '1':'0';
     byte2[1] = f[19] ? '1':'0';
     byte2[2] = f[18] ? '1':'0';
     byte2[3] = f[17] ? '1':'0';
     byte2[4] = f[16] ? '1':'0';
     byte2[5] = f[15] ? '1':'0';
     byte2[6] = f[14] ? '1':'0';
     byte2[7] = f[13] ? '1':'0';
     byte2[8] = 0;
   }
   else if( group == 5 || group == 6 ) {
     byte1[0] = '1';
     byte1[1] = '1';
     byte1[2] = '0';
     byte1[3] = '1';
     byte1[4] = '1';
     byte1[5] = '1';
     byte1[6] = '1';
     byte1[7] = '1';
     byte1[8] = 0;
     byte2[0] = f[28] ? '1':'0';
     byte2[1] = f[27] ? '1':'0';
     byte2[2] = f[26] ? '1':'0';
     byte2[3] = f[25] ? '1':'0';
     byte2[4] = f[24] ? '1':'0';
     byte2[5] = f[23] ? '1':'0';
     byte2[6] = f[22] ? '1':'0';
     byte2[7] = f[21] ? '1':'0';
     byte2[8] = 0;
   }
   TraceOp.trc( "nmra", TRCLEVEL_BYTE, __LINE__, 9999,"function datagram %s %s", byte1, byte2!=NULL?byte2:"");
}

static void calc_128spst_adv_op_bytes(char *byte1, char *byte2,
                               int direction, int speed) {

   int i,j;

   memset(byte1, 0, 9);
   memset(byte2, 0, 9);
   strcpy(byte1, "00111111");
   if (direction == 1) byte2[0]='1'; else byte2[0]='0';
   for (i=7; i>0; i--) {
      j=speed%2;
      speed=speed/2;
      switch (j) {
         case 0: byte2[i]='0'; break;
         case 1: byte2[i]='1'; break;
      }
   }
}

static void calc_acc_address_byte(char *byte, char *rest, int address) {
   /* calculating address byte: 10AAAAAA , returning rest */

   int i,j;
   char dummy[10];

   memset(dummy,0,10);
   for (i=8; i>=0; i--) {
      j=address%2;
      address=address/2;
      switch (j) {
         case 0: dummy[i]='0'; break;
         case 1: dummy[i]='1'; break;
      }
   }
   memset(byte, 0, 9);
   byte[0]='1'; byte[1]='0';
   for (i=8; i>2; i--) {
      byte[i-1]=dummy[i];
   }
   memset(rest, 0, 3);
   for (i=2; i>=0; i--) rest[i]=dummy[i];
}

static void calc_acc_instr_byte(char *byte, char *rest, int activate, int pairnr, int output) {

   int i;

   memset(byte, 0, 9);
   if (output)   byte[7]='1'; else byte[7]='0';
   if (activate) byte[4]='1'; else byte[4]='0';
   switch (pairnr) {
      case  0: byte[6]='0'; byte[5]='0'; break;
      case  1: byte[6]='1'; byte[5]='0'; break;
      case  2: byte[6]='0'; byte[5]='1'; break;
      case  3: byte[6]='1'; byte[5]='1'; break;
      default: byte[6]='0'; byte[5]='0'; break;
   }
   for (i=3; i>0; i--) {
      switch (rest[i-1]) {
         case '0': byte[i]='1'; break;
         case '1': byte[i]='0'; break;
         default : byte[i]='1'; break;
      }
   }
   byte[0]='1';
}

static void xor_two_bytes(char *byte, char *byte1, char *byte2) {

   int i;

   memset(byte, 0, 9);
   for (i=0; i<8; i++) {
      if (byte1[i]==byte2[i]) byte[i]='0'; else byte[i]='1';
   }
}

/*** functions to generate NMRA-DCC data packets ***/
/**
 * address 1...1023
 * pairnr 1...4
 * gate 0...1
 */
int compAccessory(char* packetstream, int address, int pairnr, int gate, int activate) {

   char byte1[9];
   char byte2[9];
   char byte3[9];
   char bitstream[BUFFERSIZE];
   char *p_packetstream;

   char rest[3];

   if( address < 0 || pairnr < 1 || pairnr > 4 || gate < 0 || gate > 1 ) {
     TraceOp.trc( "nmra", TRCLEVEL_WARNING, __LINE__, 9999,
         "accessory(NMRA) out of range: %d %d %d %s", address, pairnr, gate, (activate?"ON":"OFF") );
     return 0;
   }

   TraceOp.trc( "nmra", TRCLEVEL_DEBUG, __LINE__, 9999,
       "accessory(NMRA): %d %d %d %s", address, pairnr, gate, (activate?"ON":"OFF") );

    /* packet is not available */
    p_packetstream=packetstream;

    calc_acc_address_byte(byte1, rest, address);

    calc_acc_instr_byte(byte2, rest, activate, pairnr-1, gate);

    xor_two_bytes(byte3, byte2, byte1);

    /* putting all together in a 'bitstream' (char array) */
    memset(bitstream, 0, 100);
    strcat(bitstream, preamble);
    strcat(bitstream, "0");
    strcat(bitstream, byte1);
    strcat(bitstream, "0");
    strcat(bitstream, byte2);
    strcat(bitstream, "0");
    strcat(bitstream, byte3);
    strcat(bitstream, "1");

   return translateBitstream2Packetstream(bitstream, packetstream);
}

int compSpeed14(char* packetstream, int address, int direction, int speed) {

   char byte1[9];
   char byte2[9];
   char byte3[9];
   char bitstream[BUFFERSIZE];

   int adr       = 0;

   adr=address;

   /* no special error handling, it's job of the clients */
   if (address<1 || address>127 || direction<0 || direction>1 || speed<0 || speed>15)
      return 1;

   calc_7bit_address_byte(byte1, address);
   calc_baseline_speed_byte(byte2,direction,speed);
   xor_two_bytes(byte3, byte2, byte1);

   /* putting all together in a 'bitstream' (char array) */
   memset(bitstream, 0, 100);
   strcat(bitstream, preamble);
   strcat(bitstream, "0");
   strcat(bitstream, byte1);
   strcat(bitstream, "0");
   strcat(bitstream, byte2);
   strcat(bitstream, "0");
   strcat(bitstream, byte3);
   strcat(bitstream, "1");

   return translateBitstream2Packetstream(bitstream, packetstream);
}


/* function-decoder with 7-bit address */
int compFunctionShortAddr(char* packetstream, int address, int group, Boolean f[]) {

   char addrbyte[9] = {0};
   char funcbyte[9] = {0};
   char funcbyte2[9] = {0};
   char errdbyte[9] = {0};
   char bitstream[BUFFERSIZE];

   int adr = 0;
   int i;

   adr=address;
   /* no special error handling, it's job of the clients */
   if (address<1 || address>127 )
      return 1;

   calc_7bit_address_byte(addrbyte, address);
   calc_function_group(funcbyte, funcbyte2, group, f);

   xor_two_bytes(errdbyte, addrbyte, funcbyte);


   /* putting all together in a 'bitstream' (char array) (functions) */
   memset(bitstream, 0, 100);
   strcat(bitstream, preamble);
   strcat(bitstream, "0");
   strcat(bitstream, addrbyte);
   strcat(bitstream, "0");
   strcat(bitstream, funcbyte);
   strcat(bitstream, "0");
   if(funcbyte2[0] != 0 ) {
     char tmp[9] = {0};
     strcpy( tmp, errdbyte );
     xor_two_bytes(errdbyte, tmp, funcbyte2);
     strcat(bitstream, funcbyte2);
     strcat(bitstream, "0");
     TraceOp.trc( "nmra", TRCLEVEL_BYTE, __LINE__, 9999,
         "extended function group %d, errdbyte=%s", group, errdbyte);
   }
   strcat(bitstream, errdbyte);
   strcat(bitstream, "1");

   TraceOp.trc( "nmra", TRCLEVEL_BYTE, __LINE__, 9999,
       "7 bit addr bitstream: %s", bitstream);

   return translateBitstream2Packetstream(bitstream, packetstream);
}


/* function-decoder with 14-bit address */
int compFunctionLongAddr(char* packetstream, int address, int group, Boolean f[]) {

   char addrbyte1[9] = {0};
   char addrbyte2[9] = {0};
   char funcbyte[9] = {0};
   char funcbyte2[9] = {0};
   char errdbyte[9] = {0};
   char dummy[9] = {0};
   char bitstream[BUFFERSIZE];

   int adr       = 0;
   int i;

   adr=address;

   /* no special error handling, it's job of the clients */
   if (address<1 || address>10239)
      return 1;

   calc_14bit_address_byte(addrbyte1, addrbyte2, address);
   calc_function_group(funcbyte, funcbyte2, group, f);

   xor_two_bytes(dummy, addrbyte1, addrbyte2);
   xor_two_bytes(errdbyte, dummy, funcbyte);


   /* putting all together in a 'bitstream' (char array) (functions) */
   memset(bitstream, 0, 100);
   strcat(bitstream, preamble);
   strcat(bitstream, "0");
   strcat(bitstream, addrbyte1);
   strcat(bitstream, "0");
   strcat(bitstream, addrbyte2);
   strcat(bitstream, "0");
   strcat(bitstream, funcbyte);
   strcat(bitstream, "0");
   if(funcbyte2[0] != 0 ) {
     char tmp[9] = {0};
     strcpy( tmp, errdbyte );
     xor_two_bytes(errdbyte, tmp, funcbyte2);
     strcat(bitstream, funcbyte2);
     strcat(bitstream, "0");
   }
   strcat(bitstream, errdbyte);
   strcat(bitstream, "1");

   TraceOp.trc( "nmra", TRCLEVEL_BYTE, __LINE__, 9999,
       "14 bit addr bitstream: %s", bitstream);

   return translateBitstream2Packetstream(bitstream, packetstream);
}


int compSpeed28ShortAddr(char* packetstream, int address, int direction, int speed) {
     /* 4-function-decoder with 7-bit address and 28 speed steps */
     /* N1 001 1 18 1 0 0 0 0                                    */

   char addrbyte[9];
   char spdrbyte[9];
   char funcbyte[18];
   char errdbyte[9];
   char bitstream[BUFFERSIZE];

   int adr       = 0;
   int i;

   adr=address;

   /* no special error handling, it's job of the clients */
   if (address<1 || address>127 || direction<0 || direction>1 || speed<0 || speed>28) {
     /* out of range */
     return 0;
   }

   calc_7bit_address_byte(addrbyte, address);
   calc_28spst_speed_byte(spdrbyte, direction, speed);
   xor_two_bytes(errdbyte, addrbyte, spdrbyte);

   /* putting all together in a 'bitstream' (char array) (speed & direction) */
   memset(bitstream, 0, 100);
   strcat(bitstream, preamble);
   strcat(bitstream, "0");
   strcat(bitstream, addrbyte);
   strcat(bitstream, "0");
   strcat(bitstream, spdrbyte);
   strcat(bitstream, "0");
   strcat(bitstream, errdbyte);
   strcat(bitstream, "1");

   return translateBitstream2Packetstream(bitstream, packetstream);
}


int compSpeed128ShortAddr(char* packetstream, int address, int direction, int speed) {
     /* 4-function-decoder with 7-bit address and 128 speed steps */
     /* N2 001 1 057 1 0 0 0 0                                    */

   char addrbyte[9];
   char spdrbyte1[9];
   char spdrbyte2[9];
   char funcbyte[18];
   char errdbyte[9];
   char dummy[9];
   char bitstream[BUFFERSIZE];

   int adr       = 0;
   int i;

   adr=address;

   /* no special error handling, it's job of the clients */
   if (address<1 || address>127 || direction<0 || direction>1 || speed<0 || speed>128)
      return 0;

   calc_7bit_address_byte(addrbyte, address);
   calc_128spst_adv_op_bytes(spdrbyte1, spdrbyte2, direction, speed);
   xor_two_bytes(dummy, addrbyte, spdrbyte1);
   xor_two_bytes(errdbyte, dummy, spdrbyte2);

   /* putting all together in a 'bitstream' (char array) (speed & direction) */
   memset(bitstream, 0, 100);
   strcat(bitstream, preamble);
   strcat(bitstream, "0");
   strcat(bitstream, addrbyte);
   strcat(bitstream, "0");
   strcat(bitstream, spdrbyte1);
   strcat(bitstream, "0");
   strcat(bitstream, spdrbyte2);
   strcat(bitstream, "0");
   strcat(bitstream, errdbyte);
   strcat(bitstream, "1");

   return translateBitstream2Packetstream(bitstream, packetstream);
}


int compSpeed28LongAddr(char* packetstream, int address, int direction, int speed) {
     /* 4-function-decoder with 14-bit address and 28 speed steps */
     /* N3 0001 1 18 1 0 0 0 0                                    */

   char addrbyte1[9];
   char addrbyte2[9];
   char spdrbyte[9];
   char funcbyte[18];
   char errdbyte[9];
   char dummy[9];
   char bitstream[BUFFERSIZE];

   int adr       = 0;
   int i;

   adr=address;

   /* no special error handling, it's job of the clients */
   if (address<1 || address>10239 || direction<0 || direction>1 || speed<0 || speed>28)
      return 0;

   calc_14bit_address_byte(addrbyte1, addrbyte2, address);
   calc_28spst_speed_byte(spdrbyte, direction, speed);
   xor_two_bytes(dummy, addrbyte1, addrbyte2);
   xor_two_bytes(errdbyte, dummy, spdrbyte);

   /* putting all together in a 'bitstream' (char array) (speed & direction) */
   memset(bitstream, 0, 100);
   strcat(bitstream, preamble);
   strcat(bitstream, "0");
   strcat(bitstream, addrbyte1);
   strcat(bitstream, "0");
   strcat(bitstream, addrbyte2);
   strcat(bitstream, "0");
   strcat(bitstream, spdrbyte);
   strcat(bitstream, "0");
   strcat(bitstream, errdbyte);
   strcat(bitstream, "1");


   return translateBitstream2Packetstream(bitstream, packetstream);
}


int compSpeed128LongAddr(char* packetstream, int address, int direction, int speed) {
     /* 4-function-decoder with 14-bit address and 128 speed steps */
     /* N4 001 1 057 1 0 0 0 0                                    */

   char addrbyte1[9];
   char addrbyte2[9];
   char spdrbyte1[9];
   char spdrbyte2[9];
   char funcbyte[18];
   char errdbyte[9];
   char dummy[9];
   char bitstream[BUFFERSIZE];

   int adr       = 0;
   int i;

   adr=address;

   /* no special error handling, it's job of the clients */
   if (address<1 || address>10239 || direction<0 || direction>1 || speed<0 || speed>128)
      return 0;

   calc_14bit_address_byte(addrbyte1, addrbyte2, address);
   calc_128spst_adv_op_bytes(spdrbyte1, spdrbyte2, direction, speed);
   xor_two_bytes(errdbyte, addrbyte1, addrbyte2);
   xor_two_bytes(dummy, errdbyte, spdrbyte1);
   xor_two_bytes(errdbyte, dummy, spdrbyte2);

   /* putting all together in a 'bitstream' (char array) (speed & direction) */
   memset(bitstream, 0, 100);
   strcat(bitstream, preamble);
   strcat(bitstream, "0");
   strcat(bitstream, addrbyte1);
   strcat(bitstream, "0");
   strcat(bitstream, addrbyte2);
   strcat(bitstream, "0");
   strcat(bitstream, spdrbyte1);
   strcat(bitstream, "0");
   strcat(bitstream, spdrbyte2);
   strcat(bitstream, "0");
   strcat(bitstream, errdbyte);
   strcat(bitstream, "1");

   return translateBitstream2Packetstream(bitstream, packetstream);
}


/*
 * NMRA Operation Mode Packet:
 *
 * 1110CCVV 0 VVVVVVVV 0 DDDDDDDD
 *
 * C = Instruction Type field
 * V = CV Address Bit
 * D = Data
 *
 * The defined values for Instruction type (CC) are:
 *    CC=00 Reserved for future use
 *    CC=01 Verify byte
 *    CC=11 Write byte
 *    CC=10 Bit manipulation
 *
 *
 * address=2225 cvNum=3 data=15
 * 111111111111111 0 11001000 0 10110001 0 11101100 0 00000010 0 00001111 0 10011000 1
 * preamble          address1   address2   agr1       arg2       arg3       errd
 */
int dccPOM(char* packetstream, int address, Boolean longaddr, int cvNum, int data, Boolean verify) {
  int arg1 = (verify?0xE4:0xEC) + (((cvNum-1)>>8)&0x03);
  int arg2 = (cvNum-1)&0xFF;
  int arg3 = data&0xFF;
  int i = 0;

	char addrbyte1[9];
	char addrbyte2[9];
	char arg1byte[9];
	char arg2byte[9];
	char arg3byte[18];
	char errdbyte[9];
	char dummy[9];
	char bitstream[BUFFERSIZE];
	
	/* translate the arg bytes */
  arg1byte[8] = 0;
	for( i = 0; i < 8; i++ ) {
	  if( arg1 & (0x01 << i) )  arg1byte[7-i] = '1';
	  else                         arg1byte[7-i] = '0';
	}
  arg2byte[8] = 0;
	for( i = 0; i < 8; i++ ) {
	  if( arg2 & (0x01 << i) )  arg2byte[7-i] = '1';
	  else                         arg2byte[7-i] = '0';
	}
  arg3byte[8] = 0;
	for( i = 0; i < 8; i++ ) {
	  if( arg3 & (0x01 << i) )  arg3byte[7-i] = '1';
	  else                         arg3byte[7-i] = '0';
	}
  
  if( longaddr ) {
    calc_14bit_address_byte(addrbyte1, addrbyte2, address);
    xor_two_bytes(errdbyte, addrbyte1, addrbyte2);
    xor_two_bytes(dummy, errdbyte, arg1byte);
    xor_two_bytes(errdbyte, dummy, arg2byte);
    char tmp[9] = {0};
    strcpy( tmp, errdbyte );
    xor_two_bytes(errdbyte, tmp, arg3byte);

		/* putting all together in a 'bitstream' (char array) */
		memset(bitstream, 0, 100);
		strcat(bitstream, preamble);
		strcat(bitstream, "0");
		strcat(bitstream, addrbyte1);
		strcat(bitstream, "0");
		strcat(bitstream, addrbyte2);
		strcat(bitstream, "0");
		strcat(bitstream, arg1byte);
		strcat(bitstream, "0");
		strcat(bitstream, arg2byte);
		strcat(bitstream, "0");
		strcat(bitstream, arg3byte);
		strcat(bitstream, "0");
		strcat(bitstream, errdbyte);
		strcat(bitstream, "1");
  }
  else {
    calc_7bit_address_byte(addrbyte1, address);
    xor_two_bytes(dummy, addrbyte1, arg1byte);
    xor_two_bytes(errdbyte, dummy, arg2byte);
    char tmp[9] = {0};
    strcpy( tmp, errdbyte );
    xor_two_bytes(errdbyte, tmp, arg3byte);

		/* putting all together in a 'bitstream' (char array) */
		memset(bitstream, 0, 100);
		strcat(bitstream, preamble);
		strcat(bitstream, "0");
		strcat(bitstream, addrbyte1);
		strcat(bitstream, "0");
		strcat(bitstream, arg1byte);
		strcat(bitstream, "0");
		strcat(bitstream, arg2byte);
		strcat(bitstream, "0");
		strcat(bitstream, arg3byte);
		strcat(bitstream, "0");
		strcat(bitstream, errdbyte);
		strcat(bitstream, "1");
  }

  TraceOp.trc( __FILE__, TRCLEVEL_BYTE, __LINE__, 9999, "NMRA Operation Mode Packet:\n%s", bitstream);

  return translateBitstream2Packetstream(bitstream, packetstream);
}



int compSpeed(char* packetstream, int address, Boolean longaddr, int direction, int speed, int steps) {
  if( longaddr && steps == 128 )
    return compSpeed128LongAddr( packetstream, address, direction, speed);
  else if( longaddr && steps == 28 )
    return compSpeed28LongAddr( packetstream, address, direction, speed);
  else if( !longaddr && steps == 128 )
    return compSpeed128ShortAddr( packetstream, address, direction, speed);
  else if( !longaddr && steps == 28 )
    return compSpeed28ShortAddr( packetstream, address, direction, speed);
  else
    return compSpeed14( packetstream, address, direction, speed);
}

int compFunction(char* packetstream, int address, Boolean longaddr, int group, Boolean f[]) {
  if( longaddr )
    return compFunctionLongAddr( packetstream, address, group, f);
  else
    return compFunctionShortAddr( packetstream, address, group, f);
}



/* PT */
static int smInitialized = False;
static char resetstream[PKTSIZE];
static int  rs_size = 0;
static char *longpreamble  = "111111111111111111111111111111";
static char reset_packet[] = "11111111111111111111111111111100000000000000000000000000010";
static char page_preset_packet[] = "11111111111111111111111111111100111110100000000100111110010";
static char idle_packet[] = "11111111111111111111111111111101111111100000000001111111110";
static char idlestream[PKTSIZE];
static int  is_size = 0;
static char pagepresetstream[PKTSIZE];
static int  ps_size = 0;


static void smInit() {
   memset(resetstream, 0, PKTSIZE);
   rs_size=translateBitstream2Packetstream(reset_packet, resetstream);
   memset(idlestream, 0, PKTSIZE);
   is_size=translateBitstream2Packetstream(idle_packet, idlestream);
   memset(pagepresetstream, 0, PKTSIZE);
   ps_size=translateBitstream2Packetstream(page_preset_packet,pagepresetstream);
   smInitialized = True;
}

char* getResetStream(int *rsSize) {
  if (!smInitialized)
    smInit();
  *rsSize = rs_size;
  return resetstream;
}



/**
  * NMRA RP 9.2.3 section E
  * Long Preamble - In Service Mode the Command Station/Programmer will increase the preamble
  * of the packet from the minimum (per S-9.2) to at least 20 bits to allow extra time for
  * the Digital Decoder to process the packets.  This is designated as "long-preamble"
  * in the packet descriptions within this RP.
  */
int createCVgetpacket(int cv, int value, char* SendStream, int start) {
   /* direct cv access */

   char byte2[9];
   char byte3[9];
   char byte4[9];
   char byte5[9];
   char bitstream[100];
   char packetstream[PKTSIZE];

   int i, j, l, packetsize, sendsize;
   int rc = 0;

   if (!smInitialized)
     smInit();

   /* calculating byte3: AAAAAAAA (rest of CV#) */
   memset(byte3, 0, 9);
   for (i=7; i>=0; i--) {
      j=cv%2;
      cv=cv/2;
      switch (j) {
         case 0: byte3[i]='0'; break;
         case 1: byte3[i]='1'; break;
      }
   }

   /* calculating byte2: 011111AA (instruction byte1) */
   memset(byte2, 0, 9);
   strcpy(byte2, "01110100");
   for (i=7; i>=6; i--) {
      j=cv%2;
      cv=cv/2;
      switch (j) {
         case 0: byte2[i]='0'; break;
         case 1: byte2[i]='1'; break;
      }
   }

   /* calculating byte4: DDDDDDDD (data) */
   memset(byte4, 0, 9);
   for (i=7; i>=0; i--) {
      j=value%2;
      value=value/2;
      switch (j) {
         case 0: byte4[i]='0'; break;
         case 1: byte4[i]='1'; break;
      }
   }

   /* calculating byte5: EEEEEEEE (error detection byte) */
   memset(byte5, 0, 9);
   for (i=0; i<8; i++) {
      if (byte2[i]==byte3[i]) byte5[i]='0'; else byte5[i]='1';
      if (byte4[i]==byte5[i]) byte5[i]='0'; else byte5[i]='1';
   }

   /* putting all together in a 'bitstream' (char array) */
   memset(bitstream, 0, 100);
   strcat(bitstream, longpreamble);
   strcat(bitstream, "0");
   strcat(bitstream, byte2);
   strcat(bitstream, "0");
   strcat(bitstream, byte3);
   strcat(bitstream, "0");
   strcat(bitstream, byte4);
   strcat(bitstream, "0");
   strcat(bitstream, byte5);
   strcat(bitstream, "1");

   packetsize = translateBitstream2Packetstream(bitstream, packetstream);

   memset(SendStream,0,2048);

   if (start) {
      for (l=0; l<20; l++) strcat(SendStream, idlestream);
      for (l=0; l<5; l++) strcat(SendStream, resetstream);
      for (l=0; l<11; l++) strcat(SendStream, packetstream);
      sendsize=20*is_size+5*rs_size+11*packetsize;
   }
   else {
      for (l=0; l<4; l++) strcat(SendStream, resetstream);
      for (l=0; l<6; l++) strcat(SendStream, packetstream);
      sendsize=4*rs_size+6*packetsize;
   }
  return sendsize;
}



int createCVsetpacket(int cv, int value, char* SendStream, int verify) {
   /* direct cv access */

   char byte2[9];
   char byte3[9];
   char byte4[9];
   char byte5[9];
   char bitstream[100];
   char packetstream[PKTSIZE];

   int i,j,l,ack1,ack2;
   int ack = 0;

   /* no special error handling, it's job of the clients */
   if (cv<0 || cv>1024 || value<0 || value>255) return 0;

   if (!smInitialized)
     smInit();

   /* calculating byte3: AAAAAAAA (rest of CV#) */
   memset(byte3, 0, 9);
   for (i=7; i>=0; i--) {
      j=cv%2;
      cv=cv/2;
      switch (j) {
         case 0: byte3[i]='0'; break;
         case 1: byte3[i]='1'; break;
      }
   }

   /* calculating byte2: 011111AA (instruction byte1) */
   memset(byte2, 0, 9);
   if (verify)
      strcpy(byte2, "01110100");
   else
      strcpy(byte2, "01111100");
   for (i=7; i>=6; i--) {
      j=cv%2;
      cv=cv/2;
      switch (j) {
         case 0: byte2[i]='0'; break;
         case 1: byte2[i]='1'; break;
      }
   }

   /* calculating byte4: DDDDDDDD (data) */
   memset(byte4, 0, 9);
   for (i=7; i>=0; i--) {
      j=value%2;
      value=value/2;
      switch (j) {
         case 0: byte4[i]='0'; break;
         case 1: byte4[i]='1'; break;
      }
   }

   /* calculating byte5: EEEEEEEE (error detection byte) */
   memset(byte5, 0, 9);
   for (i=0; i<8; i++) {
      if (byte2[i]==byte3[i]) byte5[i]='0'; else byte5[i]='1';
      if (byte4[i]==byte5[i]) byte5[i]='0'; else byte5[i]='1';
   }

   /* putting all together in a 'bitstream' (char array) */
   memset(bitstream, 0, 100);
   strcat(bitstream, longpreamble);
   strcat(bitstream, "0");
   strcat(bitstream, byte2);
   strcat(bitstream, "0");
   strcat(bitstream, byte3);
   strcat(bitstream, "0");
   strcat(bitstream, byte4);
   strcat(bitstream, "0");
   strcat(bitstream, byte5);
   strcat(bitstream, "1");

   j=translateBitstream2Packetstream(bitstream, packetstream);

   memset(SendStream,0,2048);

   if (!verify) {
      for (l=0; l<50; l++) strcat(SendStream, idlestream);
      for (l=0; l<15; l++) strcat(SendStream, resetstream);
      for (l=0; l<20; l++) strcat(SendStream, packetstream);
      l=50*is_size+15*rs_size+20*j;
   }
   else {
      for (l=0; l<15; l++) strcat(SendStream, idlestream);
      for (l=0; l<5; l++) strcat(SendStream, resetstream);
      for (l=0; l<11; l++) strcat(SendStream, packetstream);
      l=15*is_size+5*rs_size+11*j;
   }


   return l;
}


