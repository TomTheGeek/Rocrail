#ifndef LOCOIO_H_
#define LOCOIO_H_

/*
 * Programming SV's with OPC_PEER_XFER */
 
/*
  Current addressing
  
  The PC is 01 80
  The default LocoIO is 01 81 (you will have to change this if you have more than 1 LocoIO). Digitrax has
  assigned the 01 high address for LocoIO devices.
  Note: A broadcast packet can be sent out using a value of 0 in the DSTL Destination low address
  field. This allows you to set the SV1 value to an initial value (assuming you didn’t get the PIC
  programmed from me) or to fix SV1 if you accidentally changed it to an unknown value.
  Loconet Program Packet Layout (from PC to LocoIO)
  In order to program the SV’s you use Loconet Peer to Peer messages (OPC_PEER_XFER). The syntax of
  this message is documented in the Loconet Personal Edition 1.0. This document will address the field’s
  LocoIO uses.
  
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
#define SV_WRITE  0x01
#define SV_READ   0x02

#define LOCOIO_DSTH 0x01

#define MP_WRITE 0x03
#define MP_READ  0x04

Boolean isLocoIOSV(byte* msg);
Boolean isLocoIOMP(byte* msg);
Boolean evaluateLocoIOSV(byte *msg, int* addr, int* subaddr, int* sv, int* val, int* ver);
int makereqLocoIOSV(byte *msg, int addr, int subaddr, int sv, int val, Boolean writereq);

int makereqLocoIOMultiPort(byte *msg, int addr, int subaddr, int mask, int val, Boolean writereq);
Boolean evaluateLocoIOMultiPort(byte *msg, int* addr, int* subaddr, int* mask, int* val, int* ver);



#endif /*LOCOIO_H_*/
