/*
 * ibcom-cv.c
 *
 *  Created on: Sep 12, 2009
 *      Author: rob
 */

/*
PC -> IB: BB 7F 00 3B                                     OPC_RQ_SL_DATA, 127(Command Station Options ), 0
IB -> PC: B4 3B 00 70                                     OPC_LONG_ACK, on OPC_RQ_SL_DATA, 0

# start of programming session
PC -> IB: E5 07 01 49 42 41 56                            OPC_PEER_XFER, src=7, dst=9345, ??
PC -> IB: 82 7D                                           OPC_GPOFF

# read cv 1                    CV CV
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

# write cv 1                   CV CV VV
PC -> IB: ED 1F 01 49 42 71 71 01 00 03 70 00 00 00 00 10 OPC_IMM_PACKET
          00 00 00 00 00 00 00 00 00 00 00 00 00 00 65
IB -> PC: B4 6D 01 27                                     OPC_LONG_ACK, on OPC_IMM_PACKET
# cv 1 has value 3                      VV
IB -> PC: E7 0E 7C 00 00 00 71 06 00 00 03 00 00 1E       OPC_SL_RD_DATA, len, PT slot,

# end off programming session
PC -> IB: E5 07 01 49 42 40 57                            OPC_PEER_XFER, src=7, dst=

*/


