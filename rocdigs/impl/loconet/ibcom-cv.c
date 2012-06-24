/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */

/*
PC -> IB: BB 7F 00 3B                                     OPC_RQ_SL_DATA, 127(Command Station Options ), 0
IB -> PC: B4 3B 00 70                                     OPC_LONG_ACK, on OPC_RQ_SL_DATA, 0

# start of programming session
PC -> IB: E5 07 01 49 42 41 56                            OPC_PEER_XFER, src=7, dst=9345, ??
PC -> IB: 82 7D                                           OPC_GPOFF

# read cv 1                  R CV CV
PC -> IB: ED 1F 01 49 42 71 72 01 00 00 70 00 00 00 00 10 OPC_IMM_PACKET
          00 00 00 00 00 00 00 00 00 00 00 00 00 00 65
IB -> PC: B4 6D 01 27                                     OPC_LONG_ACK, on OPC_IMM_PACKET
# cv 1 has value 3                      VV
IB -> PC: E7 0E 7C 00 00 00 72 06 00 00 03 00 00 1D       OPC_SL_RD_DATA, len, PT slot,

# end off programming session
PC -> IB: E5 07 01 49 42 40 57                            OPC_PEER_XFER, src=7, dst=


# start of programming session
PC -> IB: E5 07 01 49 42 41 56                            OPC_PEER_XFER, src=7, dst=
PC -> IB: 82 7D                                           OPC_GPOFF

# write cv 1                 W CV CV VV
PC -> IB: ED 1F 01 49 42 71 71 01 00 03 70 00 00 00 00 10 OPC_IMM_PACKET
          00 00 00 00 00 00 00 00 00 00 00 00 00 00 65
IB -> PC: B4 6D 01 27                                     OPC_LONG_ACK, on OPC_IMM_PACKET
# cv 1 has value 3                      VV
IB -> PC: E7 0E 7C 00 00 00 71 06 00 00 03 00 00 1E       OPC_SL_RD_DATA, len, PT slot,

# end off programming session
PC -> IB: E5 07 01 49 42 40 57                            OPC_PEER_XFER, src=7, dst=



# write 254 in cv 27
               HB  W CV CV VV
ED 1F 01 49 42 79 71 1B 00 7E 70 00 00 00 00 10
00 00 00 00 00 00 00 00 00 00 00 00 00 00 0A
# HB = high bit for CV value

# response
                        HB    VV
E7 0E 7C 00 00 00 71 06 02 00 7E 00 00 61



# write 255 in cv 545
               HB  W CV CV VV
ED 1F 01 49 42 79 71 21 02 7F 70 00 00 00 00 10
00 00 00 00 00 00 00 00 00 00 00 00 00 00 33

# read cv 393
               HB  R CV CV VV
ED 1F 01 49 42 73 72 09 02 00 70 00 00 00 00 10
00 00 00 00 00 00 00 00 00 00 00 00 00 00 6D


*/
#include "rocdigs/impl/loconet_impl.h"

#include "rocdigs/impl/loconet/ibcom-cv.h"
#include "rocdigs/public/loconet.h"
#include "rocdigs/impl/loconet/lnconst.h"


int makeIBComCVPacket(int cv, int value, byte* buffer, Boolean write) {
  MemOp.set( buffer, 0, 0x1F );
  buffer[ 0] = OPC_IMM_PACKET;
  buffer[ 1] = 0x1F;
  buffer[ 2] = 0x01;
  buffer[ 3] = 0x49;
  buffer[ 4] = 0x42;
  buffer[ 5] = 0x71;
  buffer[ 6] = write ? 0x71:0x72;
  buffer[ 7] = cv % 256; /*CV*/
  buffer[ 8] = cv / 256; /*CV*/
  if( buffer[ 7] & 0x80 ) {
    buffer[ 5] |= 0x02;
    buffer[ 7] &= 0x7F;
  }
  buffer[ 9] = value; /*value*/
  if( buffer[ 9] & 0x80 ) {
    buffer[ 5] |= 0x08;
    buffer[ 9] &= 0x7F;
  }
  buffer[10] = 0x70;
  buffer[15] = 0x10;
  buffer[0x1F-1] = LocoNetOp.checksum( buffer, 0x1F-1 );
; /*checksum*/
  return 0x1F;
}


int startIBComPT(byte* buffer) {
  buffer[0] = OPC_PEER_XFER;
  buffer[1] = 0x07;
  buffer[2] = 0x01;
  buffer[3] = 0x49;
  buffer[4] = 0x42;
  buffer[5] = 0x41;
  buffer[6] = 0x56; /* checksum */
  return 7;
}

int stopIBComPT(byte* buffer) {
  buffer[0] = OPC_PEER_XFER;
  buffer[1] = 0x07;
  buffer[2] = 0x01;
  buffer[3] = 0x49;
  buffer[4] = 0x42;
  buffer[5] = 0x40;
  buffer[6] = 0x57; /* checksum */
  return 7;
}

void initIBCom(iOLocoNet loconet) {
  byte cmd0[] = {0xE6, 0x15, 0x00, 0x01, 0x20, 0x1A, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x52, 0x11, 0x00, 0x00, 0x00, 0x00, 0x72};
  byte cmd1[] = {0xED, 0x0F, 0x01, 0x49, 0x42, 0x0D, 0x00, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  byte cmd2[] = {0xE5, 0x0F, 0x00, 0x49, 0x4B, 0x0B, 0x04, 0x1A, 0x00, 0x52, 0x11, 0x1A, 0x00, 0x00, 0x5B};
  /*
  ThreadOp.sleep(100);
  LocoNetOp.transact( loconet, cmd0, 21, NULL, NULL, 0, 0, False );
  ThreadOp.sleep(100);
  LocoNetOp.transact( loconet, cmd1, 15, NULL, NULL, 0, 0, False );
  ThreadOp.sleep(100);
  LocoNetOp.transact( loconet, cmd2, 15, NULL, NULL, 0, 0, False );
  */
}



