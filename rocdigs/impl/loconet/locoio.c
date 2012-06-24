/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
*/
#include "rocs/public/rocs.h"
#include "rocs/public/objbase.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/trace.h"

#include "rocdigs/impl/loconet/lnconst.h"
#include "rocdigs/impl/loconet/locoio.h"

Boolean isLocoIOSV(byte* msg) {
  if( msg[0] == OPC_PEER_XFER && msg[4] == LOCOIO_DSTH && (msg[6] == SV_READ || msg[6] == SV_WRITE)) {
    /* could be a hit... */
    return True;
  }
  return False;
}
Boolean isLocoIOMP(byte* msg) {
  if( msg[0] == OPC_PEER_XFER && msg[4] == LOCOIO_DSTH && (msg[6] == MP_READ || msg[6] == MP_WRITE)) {
    /* could be a hit... */
    return True;
  }
  return False;
}


Boolean isLNOPSW(byte* msg) {
  if( msg[0] == OPC_LONG_ACK && msg[1] == 0x00 ) {
    /* could be a hit... */
    return True;
  }
  return False;
}


/*
  Loconet Program Packet Layout (reply from LocoIO to the PC)

  0xE5     OP Code
  0x10     message length
  SRCL     Source low address
  DSTL     Destination low address
  DSTH     Source and Destination high address
  PXCT1    High order bit of Version
  D1       Original command
  D2       SV number requested
  D3       Lower 7 bits of LocoIO Version
  D4
  PXCT2    High order bit of requested data
  D5
  D6       Requested Data
  D7       Requested Data + 1
  D8       Requested Data + 2
  CHK Checksum

  Example
  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
  E5 10 51 50 01 04 02 01 07 00 08 00 51 01 40 12
  This reply is from LocoIO # 81 (0x51) and saying that SV 1 has the value 81 (0x51)


*/

Boolean evaluateLocoIOSV(byte *msg, int* addr, int* subaddr, int* sv, int* val, int* ver) {
  unsigned char  ucSRC = msg[2];
  unsigned char  ucREQ;
  unsigned short usDST;
  unsigned char  aucData[8];
  int i = 0;;

  usDST = msg[3];
  usDST <<= 8;
  usDST |= msg[4];

  for( i = 0; i < 4; i++)
  {
    aucData[i] = msg[6+i];
    if (msg[5] & (1<<i)) aucData[i] |= 0x80;  // PXCT1.n is MSB of Dn
  }
  for( i = 0; i < 4; i++)
  {
    aucData[i+4] = msg[11+i];
    if (msg[10] & (1<<i)) aucData[i+4] |= 0x80;  // PXCT1.n is MSB of Dn
  }

  ucREQ = aucData[0];

  // LocoIO low address
  *addr = msg[2];
  // LocoIO sub address
  *subaddr = msg[11];

  // CV index
  *sv = aucData[1];

  // CV value
  if( ucREQ==SV_WRITE )
    *val = aucData[7];
  else
    *val = aucData[5];

  // version
  *ver = aucData[2];

  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999,
      "evaluateLocoIOSV addr=%d-%d sv=%d val=%d opc=%s ver=%d",
      *addr, *subaddr, *sv, *val, (ucREQ==SV_WRITE)?"write":"read", *ver );

  return (ucREQ==SV_WRITE)?1:0;
}


/*

  0xE5      OP Code
  0x10      message length
  SRCL      Source low address
  DSTL      Destination low address
  DSTH      Source and Destination high address
  PXCT1
  D1        Command
  D2        Register
  D3
  D4        Data
  PXCT2
  D5
  D6
  D7
  D8
  CHK Checksum

  Example
  00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F
  E5 10 50 51 01 00 02 01 00 00 00 00 00 00 00 09
  This is sending out a read command to LocoIO # 81 (0x51) and requesting the data from SV number 1.

*/

int makereqLocoIOSV(byte *msg, int addr, int subaddr, int sv, int val, Boolean writereq) {
  byte PXCT1  = 0;
  byte PXCT2  = 0;
  int i = 0;

  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999,
      "makereqLNSV addr=%d-%d sv=%d val=%d",
      addr, subaddr, sv, val );

  msg[0] = OPC_PEER_XFER;
  msg[1] = 0x10;
  msg[2] = 0x50;

  // DST
  msg[3] = addr & 0x7F;
  msg[4] = LOCOIO_DSTH;

  // spare
  msg[5] = 0;

  // set or get request:
  msg[6] = writereq?SV_WRITE:SV_READ;

  // cv index
  msg[7] = sv & 0xFF;

  // spare
  msg[8] = 0;

  // cv value
  msg[9] = val & 0xFF;

  // spare
  msg[10] = 0;

  // sub address
  msg[11] = subaddr;

  // spare
  msg[12] = 0;
  msg[13] = 0;
  msg[14] = 0;

  for( i = 0; i < 4; i++ ) {
    if( msg[6+i] & 0x80 ) {
      PXCT1 |= 1 << i;
      msg[6+i] = msg[6+i] & 0x7F;
    }
  }
  msg[5] = PXCT1;

  for( i = 0; i < 4; i++ ) {
    if( msg[11+i] & 0x80 ) {
      PXCT2 |= 1 << i;
      msg[11+i] = msg[11+i] & 0x7F;
    }
  }
  msg[10] = PXCT2;

  return 0x10;
}

int makereqLNOPSW(byte *msg, int addr, int type, int opsw, int val, Boolean writereq) {

  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999,
      "makereqLNOPSW addr=%d type=%d opsw=%d val=%d",
      addr, type, opsw, val );

  msg[0] = OPC_MULTI_SENSE;

  if (!writereq) {
    // read op
    int element = 0x62;
    if ( (addr&0x80) != 0 )
      element|= 1;
    msg[1] = element;
    msg[2] = addr & 0x7F;
    msg[3] = type;
    {
      int loc = (opsw-1)/8;
      int bit = (opsw-1)-loc*8;
      msg[4] = loc * 16 + bit * 2;
    }
  }
  else {
    //write op
    int element = 0x72;
    if ( (addr&0x80) != 0 )
      element|= 1;
    msg[1] = element;
    msg[2] = addr & 0x7F;
    msg[3] = type;
    {
      int loc = (opsw-1)/8;
      int bit = (opsw-1)-loc*8;
      msg[4] = loc * 16 + bit * 2 + val;
    }
  }

  return 6;
}

Boolean evaluateLNOPSW(byte *msg, int* addr, int* opsw, int* val) {
  *addr = 0;
  *opsw = 0;

  if(msg[2] != 0x7F)
    *val = msg[2] & 0x20 ? 1:0;
  else
    *val = 0; /* write reply */

  return (msg[2]==0x7F)?True:False;
}




int makereqLocoIOMultiPort(byte *msg, int addr, int subaddr, int mask, int val, Boolean writereq) {
  byte PXCT1  = 0;
  byte PXCT2  = 0;
  int i = 0;

  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999,
      "makereqLocoIOMultiPort addr=%d-%d mask=0x%04X val=0x%04X",
      addr, subaddr, mask, val );

  msg[0] = OPC_PEER_XFER;
  msg[1] = 0x10;
  msg[2] = 0x50;

  // DST
  msg[3] = addr & 0x7F;
  msg[4] = LOCOIO_DSTH;

  // pxct1
  msg[5] = 0;

  // set or get request:
  msg[6] = writereq?MP_WRITE:MP_READ;

  // spare
  msg[7] = 0;

  // spare
  msg[8] = 0;

  // sub address
  msg[9] = subaddr;

  // pxct2
  msg[10] = 0;

  // lmask
  msg[11] = writereq ? (mask&0x00FF):0;

  // lval
  msg[12] = writereq ? (val&0x00FF):0;

  // hmask
  msg[13] = writereq ? ((mask&0xFF00)>>8):0;

  // hval
  msg[14] = writereq ? ((val&0xFF00)>>8):0;

  for( i = 0; i < 4; i++ ) {
    if( msg[6+i] & 0x80 ) {
      PXCT1 |= 1 << i;
      msg[6+i] = msg[6+i] & 0x7F;
    }
  }
  msg[5] = PXCT1;

  for( i = 0; i < 4; i++ ) {
    if( msg[11+i] & 0x80 ) {
      PXCT2 |= 1 << i;
      msg[11+i] = msg[11+i] & 0x7F;
    }
  }
  msg[10] = PXCT2;

  return 0x10;
}

Boolean evaluateLocoIOMultiPort(byte *msg, int* addr, int* subaddr, int* mask, int* val, int* ver) {
  int lmask = 0;
  int lval = 0;
  int hmask = 0;
  int hval = 0;
  unsigned char  ucSRC = msg[2];
  unsigned char  ucREQ;
  unsigned short usDST;
  unsigned char  aucData[8];
  int i = 0;

  usDST = msg[3];
  usDST <<= 8;
  usDST |= msg[4];

  for( i = 0; i < 4; i++)
  {
    aucData[i] = msg[6+i];
    if (msg[5] & (1<<i)) aucData[i] |= 0x80;  // PXCT1.n is MSB of Dn
  }
  for( i = 0; i < 4; i++)
  {
    aucData[i+4] = msg[11+i];
    if (msg[10] & (1<<i)) aucData[i+4] |= 0x80;  // PXCT1.n is MSB of Dn
  }

  ucREQ = aucData[0];

  // LocoIO low address
  *addr = msg[2];
  // LocoIO sub address
  *subaddr = aucData[3];

  // version
  *ver = aucData[2];

  lmask = aucData[4];
  lval = aucData[5];
  hmask = aucData[6];
  hval = aucData[7];

  *mask = lmask | (hmask << 8);
  *val  = lval  | (hval << 8);

  TraceOp.trc( "locoio", TRCLEVEL_INFO, __LINE__, 9999,
      "evaluateLocoIOMultiPort addr=%d-%d mask=0x%04X val=0x%04X opc=%s ver=%d",
      *addr, *subaddr, *mask, *val, (ucREQ==MP_WRITE)?"write":"read", *ver );

  return (ucREQ==MP_WRITE)?1:0;
}

void getQueryAddresses(void) {
  /*
    offset:   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F |
    --------------------------------------------------------- |
    00000000: E5 10 50 00 01 00 02 00 00 00 00 00 00 00 00 59 | broadcast get sv 0

    00000000: E5 10 51 50 01 04 02 00 14 7B 00 01 00 51 01 32 | response:
    evaluateLocoIOSV addr=81-1 sv=0 val=0 opc=read ver=148
  */
}

