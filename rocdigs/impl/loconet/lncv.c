/*
-------------------------------------------------------------------
 Project     Rocrail - Model Railroad Software
-------------------------------------------------------------------
 File name   rocdigs/impl/loconet/lncv.c
-------------------------------------------------------------------
 Author      Stefan Bormann (LoconetOverTcp), Rob Versluis
-------------------------------------------------------------------
 License:    This piece of code is part of the Rocrail
             project (http://www.rocrail.net) and therefore
             published under the terms of the GNU General Public
             Licence (http://www.gnu.org/licenses/gpl.html).
-------------------------------------------------------------------
 Note:       The messages that are parsed by this code are owned
             by Uhlenbrock. Contact Uhlenbrock, if you want to
             use them!
-------------------------------------------------------------------
 Description LNCV's utilities
-------------------------------------------------------------------
 LNCV steps: IB initial sends a msg with type=0xFFFF, cv=0x0000, val=0xFFFF
             default the 63350 has an module addr of 65535=0xFFFF
             and could react on this broadcast:
             ED 0F 01 05 00 21 33 7F 7F 00 00 7F 7F 00 0B

             But if the 63350 has a real module address it won't.
             Next step is to set the module type(6335) and address(1):
             ED 0F 01 05 00 21 41 3F 18 00 00 01 00 00 5F

             the module with this address will respond and blinks it LED:
             E5 0F 05 49 4B 1F 01 3F 18 00 00 01 00 00 2A

             after this positive response you can go on and send LNCV
             get's and set's
             Set's are ack with a long ack:
             B4 6D 7F 59

             IB sends this message for end of programming:
             E5 0F 01 05 00 21 03 7F 7F 00 00 03 00 40 70
-------------------------------------------------------------------
 message:    [00] = opcode
             [01] = message length, in this case 15 (0x0F)
             [02] = source; the one who sends
             [03] = destination low byte
             [04] = destination high byte
             [05] = request ID
             [06] = high bits of the following 7 data bytes
             [07] = module art.nr. low byte
             [08] = module art.nr. high byte
             [09] = cv low byte
             [0A] = cv high byte
             [0B] = value low byte
             [0C] = value high byte
             [0D] = extra flags: 0x40 = quit programming mode
             [0E] = checksum
-------------------------------------------------------------------

Traces from the LocoNet Tool:

broadcast to module 68610 with addr 65535:

HEX: ED 0F 01 05 00 21 71 4D 1A 00 00 7F 7F 00 1E
HEX: E5 0F 05 49 4B 1F 01 4D 1A 00 00 01 00 00 5A

read all:

046150,430 - HEX: ED 0F 01 05 00 21 01 4D 1A 01 00 00 00 00 6F
046150,430 - HEX: E5 0F 05 49 4B 1F 01 4D 1A 01 00 02 00 00 58
046150,480 - HEX: ED 0F 01 05 00 21 01 4D 1A 02 00 00 00 00 6C
046150,480 - HEX: E5 0F 05 49 4B 1F 01 4D 1A 02 00 01 00 00 58

...

046161,500 - HEX: ED 0F 01 05 00 21 01 4D 1A 7E 00 00 00 00 10
046161,530 - HEX: E5 0F 05 49 4B 1F 01 4D 1A 7E 00 00 00 00 25
046161,600 - HEX: ED 0F 01 05 00 21 01 4D 1A 7F 00 00 00 00 11
046161,630 - HEX: E5 0F 05 49 4B 1F 01 4D 1A 7F 00 00 00 00 24


set address of second sensor, lncv 1, to 2:

ED 0F 01 05 00 20 01 4D 1A 01 00 02 00 00 6C
B4 6D 7F 59




*/
#include "rocs/public/rocs.h"
#include "rocs/public/objbase.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"

#include "rocdigs/impl/loconet/lnconst.h"
#include "rocdigs/impl/loconet/lncv.h"



const char* getSRC(byte src)
{
  switch (src)
  {
    case UB_SRC_MASTER: return "master";
    case UB_SRC_KPU:    return "KPU";
    case UB_SRC_DAISY:  return "DAISY";
    case UB_SRC_FRED:   return "FRED";
    case UB_SRC_IBSW:   return "IB-Switch";
    case UB_SRC_LNMOD:  return "LocoNet module";
    case UB_SRC_PC:     return "PC";

    default: return "Unknown";
  }
}

const char* getDST( int dst )
{
  switch (dst)
  {
    case UB_DST_SPU:  return "SPU";       // "IB"
    case UB_DST_KPU:  return "KPU";       // "IK"
    case UB_DST_DAISY:return "DAISY";     // "DY"
    case UB_DST_IBSW: return "IB-Switch"; // "IS"
    case UB_DST_PC:   return "PC";

    default: return "Unknown";
  }
}


/**
 * try to figure out if it is a Uhlenbrock LNCV message
 */
Boolean isLNCV(byte* msg) {
  if( msg[0] == OPC_IMM_PACKET || msg[0] == OPC_PEER_XFER ) {
    if( msg[1] == UB_LNCVLEN ) {
      /* could be a hit... */
      return True;
    }
  }
  return False;
}


Boolean evaluateLNCV(byte *msg, int* type, int* addr, int* cv, int* val)
{
  unsigned char  ucSRC = msg[2];
  unsigned char  ucREQ = msg[5];
  unsigned short usDST;
  int            bLED;
  unsigned char  aucData[7];
  int i = 0;;

  usDST = msg[4];
  usDST <<= 8;
  usDST |= msg[3];

  for( i = 0; i < 7; i++)
  {
    aucData[i] = msg[7+i];
    if (msg[6] & (1<<i)) aucData[i] |= 0x80;  // PXCT1.n is MSB of Dn
  }

  // Module type (Uhlenbrock Art.Nr./10)
  *type = aucData[1];
  *type <<= 8;
  *type |= aucData[0];

  // CV index
  *cv = aucData[3];
  *cv <<= 8;
  *cv |= aucData[2];

  // CV value
  *val = aucData[5];
  *val <<= 8;
  *val |= aucData[4];

  // LED/Key message???
  bLED = aucData[6] == 0xFF? 1:0;

  return (ucREQ==UB_LNCVSET)?1:0;
}


/**
 * req  0 = get, 1 = set
 *
 * checksum is done on a higher level
 * extracmd 0 is normal get/set, 1 start, 2 end
 */
int makereqLNCV(byte *msg, int type, int addr, int cv, int val, Boolean setreq, int extracmd )
{
  byte PXCT1  = 0;
  byte DAT[7] = {0,0,0,0,0,0,0};
  int i = 0;

  TraceOp.trc( "lncv", TRCLEVEL_INFO, __LINE__, 9999, "makereqLNCV type=%d addr=%d cv=%d val=%d", type, addr, cv, val );

  msg[0] = OPC_IMM_PACKET;
  msg[1] = UB_LNCVLEN;
  msg[2] = UB_SRC_KPU;

  // DST (TODO: make converter for word to array)
  msg[3] = 0x05;
  msg[4] = 0x00;

  // set or get request:
  msg[5] = setreq?UB_LNCVSET:UB_LNCVGET;

  // Module type (Uhlenbrock Art.Nr./10)
  msg[7+0] = (type & 0x00FF);
  msg[7+1] = (type & 0xFF00) >> 8;

  // cv index
  msg[7+2] = (cv & 0x00FF);
  msg[7+3] = (cv & 0xFF00) >> 8;

  // cv value
  msg[7+4] = (val & 0x00FF);
  msg[7+5] = (val & 0xFF00) >> 8;

  // extra info
  if( extracmd == 1 )
    msg[7+6] = UB_LNCVSTART;
  else if( extracmd == 2 ) {
    msg[0] = UB_RESPONSE;
    msg[7+0] = (0xFFFF & 0x00FF);
    msg[7+1] = (0xFFFF & 0xFF00) >> 8;
    msg[7+6] = UB_LNCVEND;
  }
  else
    msg[7+6] = 0x00;

  for( i = 0; i < 7; i++ ) {
    if( msg[7+i] & 0x80 ) {
      PXCT1 |= 1 << i;
      msg[7+i] = msg[7+i] & 0x7F;
    }
  }
  msg[6] = PXCT1;

  return UB_LNCVLEN;
}

