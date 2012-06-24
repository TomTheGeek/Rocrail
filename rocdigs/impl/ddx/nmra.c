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

/* http://www.nmra.org/standards/DCC/standards_rps/rp923.html */

#include "rocdigs/impl/ddx_impl.h"
#include "init.h"
#include "queue.h"
#include "nmra.h"
#include "locpool.h"
#include "accpool.h"

#include "rocs/public/trace.h"
#include "rocs/public/serial.h"
#include "rocs/public/system.h"
#include "rocs/public/thread.h"

#include <stdlib.h>
#include <string.h>

//extern iOSerial COM_DEVICE;
extern char NMRA_idle_data[];

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
static const int STACKSIZE  = 300;
static const int BUFFERSIZE = 360;

int translateabel(char *bs) {     /* the result is only an index, no warranty */
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

int read_next_six_bits (char *Bitstream) {

   int i, bits = 0;
   for (i = 0; i < 6; i++)
      bits = (bits << 1) | (*Bitstream++ == '0' ? 0 : 1);
   return bits;
}


int translateBitstream2Packetstream(char *Bitstream, char *Packetstream) {

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
          TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "sorry, restart algorithm doesn't work as expected for NMRA-Packet %s", Bitstream);
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


/*** some useful functions to calculate NMRA-DCC bytes (char arrays) ***/

void calc_7bit_address_byte(char *byte, int address) {
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

void calc_14bit_address_byte(char *byte1, char *byte2, int address) {
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

void calc_baseline_speed_byte(char *byte, int direction, int speed) {
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

void calc_28spst_speed_byte(char *byte, int direction, int speed) {
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
void calc_function_group(char* byte1, char* byte2, int group, int* f) {
   /* calculating function byte:
    * group 0 = f0-f4, 1 = f5-f8, 2 = f9-12, 3 = f13-16, 4 = f17-20, 5 = f21-24, 6 = f25-28
    */

  if( group > 0 )
    group--; /* function group from Rocview starts with 1 */
  TraceOp.trc( "nmra", TRCLEVEL_MONITOR, __LINE__, 9999,"function group %d", group);

  if( byte2 == NULL && group > 2 ) {
    TraceOp.trc( "nmra", TRCLEVEL_WARNING, __LINE__, 9999,"no byte reserved for function group %d", group);
    return;
  }

   if( group == 0 ) {
     byte1[0] ='1';
     byte1[1] ='0';
     byte1[2] ='0';
     byte1[3] = f[0]==1 ? '1':'0'; /* F0*/
     byte1[4] = f[4]==1 ? '1':'0';
     byte1[5] = f[3]==1 ? '1':'0';
     byte1[6] = f[2]==1 ? '1':'0';
     byte1[7] = f[1]==1 ? '1':'0';
     byte1[8] = 0;
   }
   else if( group == 1 ) {
     byte1[0] = '1';
     byte1[1] = '0';
     byte1[2] = '1';
     byte1[3] = '1'; /* F5-F8*/
     byte1[4] = f[8]==1 ? '1':'0';
     byte1[5] = f[7]==1 ? '1':'0';
     byte1[6] = f[6]==1 ? '1':'0';
     byte1[7] = f[5]==1 ? '1':'0';
     byte1[8] = 0;
   }
   else if( group == 2 ) {
     byte1[0] = '1';
     byte1[1] = '0';
     byte1[2] = '1';
     byte1[3] = '0'; /* F9-F12*/
     byte1[4] = f[12]==1 ? '1':'0';
     byte1[5] = f[11]==1 ? '1':'0';
     byte1[6] = f[10]==1 ? '1':'0';
     byte1[7] = f[ 9]==1 ? '1':'0';
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
     byte2[0] = f[20]==1 ? '1':'0';
     byte2[1] = f[19]==1 ? '1':'0';
     byte2[2] = f[18]==1 ? '1':'0';
     byte2[3] = f[17]==1 ? '1':'0';
     byte2[4] = f[16]==1 ? '1':'0';
     byte2[5] = f[15]==1 ? '1':'0';
     byte2[6] = f[14]==1 ? '1':'0';
     byte2[7] = f[13]==1 ? '1':'0';
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
     byte2[0] = f[28]==1 ? '1':'0';
     byte2[1] = f[27]==1 ? '1':'0';
     byte2[2] = f[26]==1 ? '1':'0';
     byte2[3] = f[25]==1 ? '1':'0';
     byte2[4] = f[24]==1 ? '1':'0';
     byte2[5] = f[23]==1 ? '1':'0';
     byte2[6] = f[22]==1 ? '1':'0';
     byte2[7] = f[21]==1 ? '1':'0';
     byte2[8] = 0;
   }
   TraceOp.trc( "nmra", TRCLEVEL_BYTE, __LINE__, 9999,"function datagram %s %s", byte1, byte2!=NULL?byte2:"");
}

void calc_128spst_adv_op_bytes(char *byte1, char *byte2,
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

void calc_acc_address_byte(char *byte, char *rest, int address) {
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

void calc_acc_instr_byte(char *byte, char *rest, int activate, int pairnr, int output) {

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

void xor_two_bytes(char *byte, char *byte1, char *byte2) {

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
int comp_nmra_accessory(int address, int pairnr, int gate, int activate) {

   char byte1[9];
   char byte2[9];
   char byte3[9];
   char bitstream[BUFFERSIZE];
   char packetstream[PKTSIZE];
   char *p_packetstream;

   char rest[3];

   int j;

   if( address < 0 || pairnr < 1 || pairnr > 4 || gate < 0 || gate > 1 ) {
     TraceOp.trc( "nmra", TRCLEVEL_WARNING, __LINE__, 9999,
         "accessory(NMRA) out of range: %d %d %d %s", address, pairnr, gate, (activate?"ON":"OFF") );
     return 0;
   }

   TraceOp.trc( "nmra", TRCLEVEL_MONITOR, __LINE__, 9999,
       "accessory(NMRA): %d %d %d %s", address, pairnr, gate, (activate?"ON":"OFF") );

   /* get the calculated packet if available */
   j=getNMRAGaPacket((address-1)*4+pairnr,gate,activate,&p_packetstream);
   if (j==0) {
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

      j=translateBitstream2Packetstream(bitstream, packetstream);
   }

   if (j>0) {
      queue_add(address,p_packetstream,QNBACCPKT,j);
      updateNMRAGaPacketPool((address-1)*4+pairnr,gate,activate,p_packetstream,j);
      return 0;
   }

   return 1;
}

int comp_nmra_baseline(int address, int direction, int speed) {

   char byte1[9];
   char byte2[9];
   char byte3[9];
   char bitstream[BUFFERSIZE];
   char packetstream[PKTSIZE];

   int adr       = 0;
   int j;

   adr=address;

   /* no special error handling, it's job of the clients */
   if (address<1 || address>127 || direction<0 || direction>1 ||
       speed<0 || speed>15)
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

   j=translateBitstream2Packetstream(bitstream, packetstream);

   if (j>0) {
      update_NMRAPacketPool(adr,packetstream,j,packetstream,j);
      queue_add(adr, packetstream,QNBLOCOPKT,j);

      return 0;
   }

   return 1;
}


/* function-decoder with 7-bit address */
int comp_nmra_fb7(int address, int group, int* f) {

   char addrbyte[9] = {0};
   char funcbyte[9] = {0};
   char funcbyte2[9] = {0};
   char errdbyte[9] = {0};
   char bitstream[BUFFERSIZE];
   char packetstream[PKTSIZE];

   int adr = 0;
   int i,j;

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

   j=translateBitstream2Packetstream(bitstream, packetstream);

   if (j>0) {
      update_NMRAPacketPool(adr,NULL,0,packetstream,j);
      queue_add(adr,packetstream,QNBLOCOPKT,j);
      return 0;
   }

   return 1;
}
/* function-decoder with 14-bit address */
int comp_nmra_fb14(int address, int group, int* f) {

   char addrbyte1[9] = {0};
   char addrbyte2[9] = {0};
   char funcbyte[9] = {0};
   char funcbyte2[9] = {0};
   char errdbyte[9] = {0};
   char dummy[9] = {0};
   char bitstream[BUFFERSIZE];
   char packetstream[PKTSIZE];

   int adr       = 0;
   int i,j;

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

   j=translateBitstream2Packetstream(bitstream, packetstream);

   if (j>0) {
      update_NMRAPacketPool(adr+ADDR14BIT_OFFSET,NULL,0,packetstream,j);
      queue_add(adr+ADDR14BIT_OFFSET,packetstream,QNBLOCOPKT,j);
      return 0;
   }

   return 1;
}



int comp_nmra_f4b7s28(int address, int direction, int speed, int *f) {
     /* 4-function-decoder with 7-bit address and 28 speed steps */
     /* N1 001 1 18 1 0 0 0 0                                    */

   char addrbyte[9];
   char spdrbyte[9];
   char funcbyte[18];
   char errdbyte[9];
   char bitstream[BUFFERSIZE];
   char bitstream2[BUFFERSIZE];
   char packetstream[PKTSIZE];
   char packetstream2[PKTSIZE];

   int adr       = 0;
   int i,j,jj;

   adr=address;

   /* no special error handling, it's job of the clients */
   if (address<1 || address>127 || direction<0 || direction>1 ||
       speed<0 || speed>28)
      return 1;
   for (i=0; i<5; i++)
      if (f[i]<0 || f[i]>1)
         return 1;

   calc_7bit_address_byte(addrbyte, address);
   calc_28spst_speed_byte(spdrbyte, direction, speed);
   calc_function_group(funcbyte, NULL, 0, f);
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

   xor_two_bytes(errdbyte, addrbyte, funcbyte);

   /* putting all together in a 'bitstream' (char array) (functions) */
   memset(bitstream2, 0, 100);
   strcat(bitstream2, preamble);
   strcat(bitstream2, "0");
   strcat(bitstream2, addrbyte);
   strcat(bitstream2, "0");
   strcat(bitstream2, funcbyte);
   strcat(bitstream2, "0");
   strcat(bitstream2, errdbyte);
   strcat(bitstream2, "1");

   j=translateBitstream2Packetstream(bitstream, packetstream);
   jj=translateBitstream2Packetstream(bitstream2, packetstream2);

   if (j>0 && jj>0) {
      update_NMRAPacketPool(adr,packetstream,j,packetstream2,jj);
      queue_add(adr,packetstream,QNBLOCOPKT,j);
      queue_add(adr,packetstream2,QNBLOCOPKT,jj);

      return 0;
   }

   return 1;
}

int comp_nmra_f4b7s128(int address, int direction, int speed, int* f) {
     /* 4-function-decoder with 7-bit address and 128 speed steps */
     /* N2 001 1 057 1 0 0 0 0                                    */

   char addrbyte[9];
   char spdrbyte1[9];
   char spdrbyte2[9];
   char funcbyte[18];
   char errdbyte[9];
   char dummy[9];
   char bitstream[BUFFERSIZE];
   char bitstream2[BUFFERSIZE];
   char packetstream[PKTSIZE];
   char packetstream2[PKTSIZE];

   int adr       = 0;
   int i,j,jj;

   adr=address;

   /* no special error handling, it's job of the clients */
   if (address<1 || address>127 || direction<0 || direction>1 ||
       speed<0 || speed>128)
      return 1;
   for (i=0; i<5; i++)
      if (f[i]<0 || f[i]>1)
         return 1;

   calc_7bit_address_byte(addrbyte, address);
   calc_128spst_adv_op_bytes(spdrbyte1, spdrbyte2, direction, speed);
   calc_function_group(funcbyte, NULL, 0, f);
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

   xor_two_bytes(errdbyte, addrbyte, funcbyte);

   /* putting all together in a 'bitstream' (char array) (functions) */
   memset(bitstream2, 0, 100);
   strcat(bitstream2, preamble);
   strcat(bitstream2, "0");
   strcat(bitstream2, addrbyte);
   strcat(bitstream2, "0");
   strcat(bitstream2, funcbyte);
   strcat(bitstream2, "0");
   strcat(bitstream2, errdbyte);
   strcat(bitstream2, "1");

   j=translateBitstream2Packetstream(bitstream, packetstream);
   jj=translateBitstream2Packetstream(bitstream2, packetstream2);

   if (j>0 && jj>0) {
      update_NMRAPacketPool(adr,packetstream,j,packetstream2,jj);
      queue_add(adr,packetstream,QNBLOCOPKT,j);
      queue_add(adr,packetstream2,QNBLOCOPKT,jj);

      return 0;
   }

   return 1;
}

int comp_nmra_f4b14s28(int address, int direction, int speed, int* f) {
     /* 4-function-decoder with 14-bit address and 28 speed steps */
     /* N3 0001 1 18 1 0 0 0 0                                    */

   char addrbyte1[9];
   char addrbyte2[9];
   char spdrbyte[9];
   char funcbyte[18];
   char errdbyte[9];
   char dummy[9];
   char bitstream[BUFFERSIZE];
   char bitstream2[BUFFERSIZE];
   char packetstream[PKTSIZE];
   char packetstream2[PKTSIZE];

   int adr       = 0;
   int i,j,jj;

   adr=address;

   /* no special error handling, it's job of the clients */
   if (address<1 || address>10239 || direction<0 || direction>1 ||
       speed<0 || speed>28)
      return 1;
   for (i=0; i<5; i++)
      if (f[i]<0 || f[i]>1)
         return 1;

   calc_14bit_address_byte(addrbyte1, addrbyte2, address);
   calc_28spst_speed_byte(spdrbyte, direction, speed);
   calc_function_group(funcbyte, NULL, 0, f);

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

   xor_two_bytes(dummy, addrbyte1, addrbyte2);
   xor_two_bytes(errdbyte, dummy, funcbyte);

   /* putting all together in a 'bitstream' (char array) (functions) */
   memset(bitstream2, 0, 100);
   strcat(bitstream2, preamble);
   strcat(bitstream2, "0");
   strcat(bitstream2, addrbyte1);
   strcat(bitstream2, "0");
   strcat(bitstream2, addrbyte2);
   strcat(bitstream2, "0");
   strcat(bitstream2, funcbyte);
   strcat(bitstream2, "0");
   strcat(bitstream2, errdbyte);
   strcat(bitstream2, "1");

   j=translateBitstream2Packetstream(bitstream, packetstream);
   jj=translateBitstream2Packetstream(bitstream2, packetstream2);

   if (j>0 && jj>0) {
      update_NMRAPacketPool(adr+ADDR14BIT_OFFSET,packetstream,j,packetstream2,jj);
      queue_add(adr+ADDR14BIT_OFFSET,packetstream,QNBLOCOPKT,j);
      queue_add(adr+ADDR14BIT_OFFSET,packetstream2,QNBLOCOPKT,jj);

      return 0;
   }

   return 1;
}

int comp_nmra_f4b14s128(int address, int direction, int speed, int* f) {
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
   char bitstream2[BUFFERSIZE];
   char packetstream[PKTSIZE];
   char packetstream2[PKTSIZE];

   int adr       = 0;
   int i,j,jj;

   adr=address;

   /* no special error handling, it's job of the clients */
   if (address<1 || address>10239 || direction<0 || direction>1 ||
       speed<0 || speed>128)
      return 1;
   for (i=0; i<5; i++)
      if (f[i]<0 || f[i]>1)
         return 1;

   calc_14bit_address_byte(addrbyte1, addrbyte2, address);
   calc_128spst_adv_op_bytes(spdrbyte1, spdrbyte2, direction, speed);
   calc_function_group(funcbyte, NULL, 0, f);
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

   xor_two_bytes(dummy, addrbyte1, addrbyte2);
   xor_two_bytes(errdbyte, dummy, funcbyte);

   /* putting all together in a 'bitstream' (char array) (functions) */
   memset(bitstream2, 0, 100);
   strcat(bitstream2, preamble);
   strcat(bitstream2, "0");
   strcat(bitstream2, addrbyte1);
   strcat(bitstream2, "0");
   strcat(bitstream2, addrbyte2);
   strcat(bitstream2, "0");
   strcat(bitstream2, funcbyte);
   strcat(bitstream2, "0");
   strcat(bitstream2, errdbyte);
   strcat(bitstream2, "1");

   j=translateBitstream2Packetstream(bitstream, packetstream);
   jj=translateBitstream2Packetstream(bitstream2, packetstream2);

   if (j>0 && jj>0) {
      update_NMRAPacketPool(adr+ADDR14BIT_OFFSET,packetstream,j,packetstream2,jj);
      queue_add(adr+ADDR14BIT_OFFSET,packetstream,QNBLOCOPKT,j);
      queue_add(adr+ADDR14BIT_OFFSET,packetstream2,QNBLOCOPKT,jj);

      return 0;
   }

   return 1;
}

/*** the following function(s) supports the implementation of NMRA- ***
 *** programmers. It is recommended to use a programming track to   ***
 *** programm your locos. In every case it is useful to stop the    ***
 *** refresh-cycle on the track when using one of the following     ***
 *** servicemode functions.                                         ***/

static int sm_initialized = False;

static char resetstream[PKTSIZE];
static int  rs_size = 0;
static char idlestream[PKTSIZE];
static int  is_size = 0;
static char pagepresetstream[PKTSIZE];
static int  ps_size = 0;

static char *longpreamble  = "111111111111111111111111111111";
static char reset_packet[] = "11111111111111111111111111111100000000000000000000000000010";
static char page_preset_packet[] = "11111111111111111111111111111100111110100000000100111110010";
static char idle_packet[] = "11111111111111111111111111111101111111100000000001111111110";

void sm_init() {
   memset(resetstream, 0, PKTSIZE);
   rs_size=translateBitstream2Packetstream(reset_packet, resetstream);
   memset(idlestream, 0, PKTSIZE);
   is_size=translateBitstream2Packetstream(idle_packet, idlestream);
   memset(pagepresetstream, 0, PKTSIZE);
   ps_size=translateBitstream2Packetstream(page_preset_packet,pagepresetstream);
   sm_initialized = True;
}

int scanACK(iOSerial serial) {
  Boolean ack = SerialOp.isRI(serial);
  if( ack ) {
    TraceOp.trc( __FILE__, TRCLEVEL_INFO, __LINE__, 9999, "PT: ACK detected.");
  }

  return ack ? 1:0;
}

int waitUARTempty_scanACK(iOSerial serial) {
   int result;
   int ack = 0;
   Boolean uartempty = False;
   do {         /* wait until UART is empty */
      if (scanACK(serial)) ack=1;  /* scan ACK */
      uartempty = SerialOp.isUartEmpty(serial, True);
      if(!uartempty) {
        ThreadOp.sleep(1);
      }
   } while(!uartempty);
   return ack;
}

int handle2ACK(iOSerial serial, int ack1, int ack2) {
  int ack   = 0;

  if ( ack1==0 && ack2 == 1 ) {
    return 1;
  }

  ack = scanACK(serial);
  if ( ack1==0 && ack2 == 0 && ack == 1 ) {
    return 1;
  }

  if ( ack1 == 0 && ack2 == 0 && ack == 0 ) {
    return 0;
  }
  // ack not supported
  TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: ACK not supported or shortcut." );
  return 2;
}


int protocol_nmra_sm_direct_cvbyte(obj inst, int cv, int value, int verify, int pom) {
   /* direct cv access */
   iODDXData data = Data((iODDX)inst);

   char byte2[9];
   char byte3[9];
   char byte4[9];
   char byte5[9];
   char bitstream[100];
   char packetstream[PKTSIZE];
   char SendStream[2048];

   int i,j,l,ack1,ack2;
   int ack = 0;

   /* no special error handling, it's job of the clients */
   if (cv<0 || cv>1024 || value<0 || value>255) return 0;

   if (!sm_initialized) sm_init();

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

   TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "PT: enable booster output");
   /* enable booster output */
   SerialOp.setDTR(data->serial,True);

   TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "PT: writing %d bytes...", l);

   if( !pom ) {
     ack1 = scanACK(data->serial);
     SerialOp.write(data->serial,SendStream,l);
     ack2 = waitUARTempty_scanACK(data->serial);
   }

   if( verify )
     ack = handle2ACK(data->serial, ack1, ack2 );

   if( verify && ack > 0 )
     TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: ack = %d", ack);

   TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "PT: disable booster output");
   /* disable booster output */
   SerialOp.setDTR(data->serial,False);

   return verify?ack:1;
}

int protocol_nmra_sm_write_cvbyte(obj inst, int cv, int value, int pom) {
   return protocol_nmra_sm_direct_cvbyte(inst, cv, value, False, pom);
}

int protocol_nmra_sm_verify_cvbyte(obj inst, int cv, int value) {
   return protocol_nmra_sm_direct_cvbyte(inst, cv, value, True, False);
}



/**
  * NMRA RP 9.2.3 section E
  * Long Preamble - In Service Mode the Command Station/Programmer will increase the preamble
  * of the packet from the minimum (per S-9.2) to at least 20 bits to allow extra time for
  * the Digital Decoder to process the packets.  This is designated as "long-preamble"
  * in the packet descriptions within this RP.
  */
int __createCVgetpacket(int cv, int value, char* SendStream, int start) {
   /* direct cv access */

   char byte2[9];
   char byte3[9];
   char byte4[9];
   char byte5[9];
   char bitstream[100];
   char packetstream[PKTSIZE];

   int i, j, l, packetsize, sendsize;
   int rc = 0;

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


int nmragetcvbyte(obj inst, int cv) {
   /* direct cv access */

   iODDXData data = Data((iODDX)inst);
   char SendStream[2048];

   int ack;
   int start, i;
   int value = 0;
   int sendsize = 0;
   int fastcvget = data->fastcvget;

   TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: cvget for %d", cv);

   /* no special error handling, it's job of the clients */
   if (cv<0 || cv>1024) return -1;

   if (!sm_initialized) sm_init();

   TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: enable booster output");
   /* enable booster output */
   SerialOp.setDTR(data->serial,True);

   /**
    * NMRA RP 9.2.3 section E
    * Power On Cycle -  Upon applying power to the track, the Command Station/Programmer must
    * transmit at least 20 valid packets to the Digital Decoder to allow it time to stabilize
    * internal operation before any Service Mode operations are initiated.
    */
   TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: power on cycle");

   TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: start polling...");

   start = 1;
   do {
     SerialOp.flush(data->serial);
     ack = scanACK(data->serial);
     sendsize = __createCVgetpacket(cv, value, SendStream, start);
     if( value % 10 == 0 || !fastcvget )
       TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: sending %d bytes checking value %d...", sendsize, value);
     SerialOp.write(data->serial,SendStream,sendsize);
     if (start)
       ThreadOp.sleep(240);
     else if( !fastcvget )
       ThreadOp.sleep(40);
     ack = 0;
     /* wait for UART: */
     ack=waitUARTempty_scanACK(data->serial);
     for( i = 0; i < (fastcvget ? 5:120) && ack == 0; i++ ) {
       ack = scanACK(data->serial);
	 /* Some USB2Serial adapter dont give a chance to detect uart empty
	  * We use waitMM as a fix for this. If detection is impossible,
	  * first wait argument is used, else second wait argument is used.
	  * This will cause pauses between values, where output will drain.
	  * For real Uart, this will not happen.
	  *ACK is 6-8ms long, poll att least every 6ms*/
       if( !fastcvget )
         SerialOp.waitMM(data->serial,5000,100);
     }


     /* init for next loop: */
     start = 0;
     if(ack==0) {
       value++;
     }
     else {
       /* 1 or more Reset Packets if an acknowledgement is detected */
       for( i = 0; i < 3; i++ ) {
         SerialOp.write(data->serial,resetstream,rs_size);
       }
     }

     TraceOp.trc( __FILE__, TRCLEVEL_DEBUG, __LINE__, 9999, "PT: next value %d...", value);
   } while( ack == 0 && value < 256);


   TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: ack = %d", ack);

   TraceOp.trc( __FILE__, TRCLEVEL_MONITOR, __LINE__, 9999, "PT: disable booster output");
   /* disable booster output */
   SerialOp.setDTR(data->serial,False);

   if( ack == 0 )
     value = -1;
   return value;
}
