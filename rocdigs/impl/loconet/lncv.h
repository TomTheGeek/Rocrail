/*
-------------------------------------------------------------------
 Project     Rocrail - Model Railroad Software
-------------------------------------------------------------------
 File name   rocdigs/impl/loconet/lncv.h
-------------------------------------------------------------------
 Author      Rob Versluis
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
 Description LocoNet CV's utilities
-------------------------------------------------------------------
*/
#ifndef LNCV_H_
#define LNCV_H_

#define UB_REQUEST  0xED
#define UB_RESPONSE 0xE5
#define UB_LNCVLEN  0x0F

#define UB_LNCVSTART 0x80
#define UB_LNCVEND   0x40

#define UB_LNCVGET  0x21
#define UB_LNCVSET  0x20

#define UB_SRC_MASTER 0
#define UB_SRC_KPU    1
#define UB_SRC_DAISY  2
#define UB_SRC_FRED   3
#define UB_SRC_IBSW   4
#define UB_SRC_LNMOD  5
#define UB_SRC_PC     8

#define UB_DST_SPU   0x4249    // "IB"
#define UB_DST_KPU   0x4b49    // "IK"
#define UB_DST_DAISY 0x5944    // "DY"
#define UB_DST_IBSW  0x5349    // "IS"
#define UB_DST_PC    0x0008

Boolean isLNCV(byte* msg);
Boolean evaluateLNCV(byte *msg, int* type, int* addr, int* cv, int* val);
int makereqLNCV(byte *msg, int type, int addr, int cv, int val, Boolean setreq, int lncvcmd);
int makequitreqLNCV(byte *msg);


/*
Table of the LNCV's available on 63 350 LocoNet feedback modules
 LocoNet-CV   Description                                               Value range    Default value
       0      Module address                                           1 through 65535       65535
     1-8      Feedback addresses for blocks 1 through 8                 1 through 2048  1 (for block #1)
      17      Report address                                            1 through 2048        1017
      20      Module configuration:                                                             0
              Bit #0 = 0 Automatic address assignment                          0
              Bit #0 = 1 Individual block address assignment                   1
              Bit #1 = 0 All delays from LNCV #21 and #41                      0
              Bit #1 = 1 Individual block delays                               2
              Bit #2 = 0 No status reporting upon LocoNet power-up             0
              Bit #2 = 1 Automatic status repair upon LocoNet power-up         4
    21-28     "On" time delay for input 1 through 16                    0 through 255   3 (for block #1)
    41-48     "Off" time delay for input 1 through 16                   0 through 255  30 (for block #1)


Table of the LNCV's available on 63 340 LocoNet feedback modules
 LocoNet-CV   Description                                               Value range    Default value
       0      Module address                                           1 through 65535       65535
     1-8      Feedback addresses for blocks 1 through 8                 1 through 2048  1 (for block #1)
      17      Report address                                            1 through 2048        1017
      20      Module configuration:
              Bit #0 = 0 Automatic address assignment                          0                0
              Bit #0 = 1 Individual block address assignment                   1
              Bit #1 = 0 All delays from LNCV #21 and #41                      0
              Bit #1 = 1 Individual block delays                               2
              Bit #2 = 0 No status reporting upon LocoNet power-up             0
              Bit #2 = 1 Automatic status repair upon LocoNet power-up         4
    21-28     "On" time delay for block 1 through 8                     0 through 255   3 (for block #1)
    41-48     "Off" time delay for block 1 through 8                    0 through 255  30 (for block #1)


Table of LNCV’s available on 63 410 LocoNet Switch Module
LNCV Description                                          Value Range   Default Value
  0  Module address                                          0-65534          1
  1  Programming Assistant                                  11-20483          0
  2  Blink rate generator 1 for outputs 1-8                   1-255          10
  3  Blink rate generator 1 for outputs 9-16                  1-255          10
  4  Blink rate generator 1 for outputs 17-20                 1-255          10
  5  Blink rate generator 2 for outputs 1-8                   1-255          10
  6  Blink rate generator 2 for outputs 9-16                  1-255          10
  7  Blink rate generator 2 for outputs 17-20                 1-255          10
  8  Outputs 1 – 16 damped on/off switching                  0-65535          0
  9  Outputs 17 – 20 damped on/off switching                   0-15           0
 10  Overlap time in 32ms increments                          0-255          16
 11  Light signal configuration for outputs 1 to 4          11-20487          0
 12  Light signal configuration for outputs 5 to 8          11-20487          0
 13  Light signal configuration for outputs 9 to 12         11-20487          0
 14  Light signal configuration for outputs 13 to 16        11-20487          0
 15  Light signal configuration for outputs 17 to 20        11-20487          0
 16  Not used
 17  Restart time after short circuit in 600μs increments  Do not alter      32
 18  Short circuit detection threshold                     Do not alter      25
 19  Switch on delay in 0.5 second increments                 1-255           1
 20  Software Version                                            -            -
 21  Switch on command for output 1                         10-20483        1991
 to
 40  Switch on command for output 20                        10-20483        2181
 41  Switch off command for output 1                        10-20483        1990
 to
 60  Switch off command for output 20                       10-20483        2180
 61  Switching mode for output 1  0
 to
 80  Switching mode for output 20 0



------------------------------------------------------------
Monitored with the LocoNet Tool and 63120:

Uhlenbrock programming ON 63340 module 2
BB 00 00 44 (OPC_RQ_SL_DATA)
E7 0E 00 00 00 00 00 04 00 00 08 00 00 1A (OPC_SL_RD_DATA)
ED 0F 01 05 00 21 41 3E 18 00 00 02 00 00 5D (OPC_IMM_PACKET)
E5 0F 05 49 4B 1F 01 3E 18 00 00 02 00 00 28 (OPC_PEER_XFER)

Uhlenbrock programming OFF 63340 module 2
A0 0C 0B 58 (OPC_LOCO_SPD)




Uhlenbrock programming READ ALL 63340 module 2
ED 0F 01 05 00 21 01 3E 18 00 00 00 00 00 1F (OPC_IMM_PACKET)
E5 0F 05 49 4B 1F 01 3E 18 00 00 02 00 00 28 (OPC_PEER_XFER)
ED 0F 01 05 00 21 01 3E 18 01 00 00 00 00 1E
E5 0F 05 49 4B 1F 01 3E 18 01 00 01 00 00 2A
ED 0F 01 05 00 21 01 3E 18 02 00 00 00 00 1D
E5 0F 05 49 4B 1F 31 3E 18 02 00 7F 7F 00 18
ED 0F 01 05 00 21 01 3E 18 03 00 00 00 00 1C
E5 0F 05 49 4B 1F 31 3E 18 03 00 7F 7F 00 19
ED 0F 01 05 00 21 01 3E 18 04 00 00 00 00 1B
E5 0F 05 49 4B 1F 31 3E 18 04 00 7F 7F 00 1E
ED 0F 01 05 00 21 01 3E 18 05 00 00 00 00 1A
E5 0F 05 49 4B 1F 31 3E 18 05 00 7F 7F 00 1F
ED 0F 01 05 00 21 01 3E 18 06 00 00 00 00 19
E5 0F 05 49 4B 1F 31 3E 18 06 00 7F 7F 00 1C
ED 0F 01 05 00 21 01 3E 18 07 00 00 00 00 18
E5 0F 05 49 4B 1F 31 3E 18 07 00 7F 7F 00 1D
ED 0F 01 05 00 21 01 3E 18 08 00 00 00 00 17
E5 0F 05 49 4B 1F 31 3E 18 08 00 7F 7F 00 12
ED 0F 01 05 00 21 01 3E 18 09 00 00 00 00 16
E5 0F 05 49 4B 1F 31 3E 18 09 00 7F 7F 00 13
ED 0F 01 05 00 21 01 3E 18 0A 00 00 00 00 15
E5 0F 05 49 4B 1F 31 3E 18 0A 00 7F 7F 00 10
ED 0F 01 05 00 21 01 3E 18 0B 00 00 00 00 14
E5 0F 05 49 4B 1F 31 3E 18 0B 00 7F 7F 00 11
ED 0F 01 05 00 21 01 3E 18 0C 00 00 00 00 13
E5 0F 05 49 4B 1F 31 3E 18 0C 00 7F 7F 00 16
ED 0F 01 05 00 21 01 3E 18 0D 00 00 00 00 12
E5 0F 05 49 4B 1F 31 3E 18 0D 00 7F 7F 00 17
ED 0F 01 05 00 21 01 3E 18 0E 00 00 00 00 11
E5 0F 05 49 4B 1F 31 3E 18 0E 00 7F 7F 00 14
ED 0F 01 05 00 21 01 3E 18 0F 00 00 00 00 10
E5 0F 05 49 4B 1F 31 3E 18 0F 00 7F 7F 00 15
ED 0F 01 05 00 21 01 3E 18 10 00 00 00 00 0F
E5 0F 05 49 4B 1F 31 3E 18 10 00 7F 7F 00 0A
ED 0F 01 05 00 21 01 3E 18 11 00 00 00 00 0E
E5 0F 05 49 4B 1F 11 3E 18 11 00 79 03 00 51
ED 0F 01 05 00 21 01 3E 18 12 00 00 00 00 0D
E5 0F 05 49 4B 1F 11 3E 18 12 00 7F 00 00 57
ED 0F 01 05 00 21 01 3E 18 13 00 00 00 00 0C
E5 0F 05 49 4B 1F 11 3E 18 13 00 7F 00 00 56
ED 0F 01 05 00 21 01 3E 18 14 00 00 00 00 0B
E5 0F 05 49 4B 1F 01 3E 18 14 00 00 00 00 3E
ED 0F 01 05 00 21 01 3E 18 15 00 00 00 00 0A
E5 0F 05 49 4B 1F 01 3E 18 15 00 03 00 00 3C
ED 0F 01 05 00 21 01 3E 18 16 00 00 00 00 09
E5 0F 05 49 4B 1F 11 3E 18 16 00 7F 00 00 53
ED 0F 01 05 00 21 01 3E 18 17 00 00 00 00 08
E5 0F 05 49 4B 1F 11 3E 18 17 00 7F 00 00 52
ED 0F 01 05 00 21 01 3E 18 18 00 00 00 00 07
E5 0F 05 49 4B 1F 11 3E 18 18 00 7F 00 00 5D
ED 0F 01 05 00 21 01 3E 18 19 00 00 00 00 06
E5 0F 05 49 4B 1F 11 3E 18 19 00 7F 00 00 5C
ED 0F 01 05 00 21 01 3E 18 1A 00 00 00 00 05
E5 0F 05 49 4B 1F 11 3E 18 1A 00 7F 00 00 5F
ED 0F 01 05 00 21 01 3E 18 1B 00 00 00 00 04
E5 0F 05 49 4B 1F 11 3E 18 1B 00 7F 00 00 5E
ED 0F 01 05 00 21 01 3E 18 1C 00 00 00 00 03
E5 0F 05 49 4B 1F 11 3E 18 1C 00 7F 00 00 59
ED 0F 01 05 00 21 01 3E 18 1D 00 00 00 00 02
E5 0F 05 49 4B 1F 11 3E 18 1D 00 7F 00 00 58
ED 0F 01 05 00 21 01 3E 18 1E 00 00 00 00 01
E5 0F 05 49 4B 1F 11 3E 18 1E 00 7F 00 00 5B
ED 0F 01 05 00 21 01 3E 18 1F 00 00 00 00 00
E5 0F 05 49 4B 1F 11 3E 18 1F 00 7F 00 00 5A
ED 0F 01 05 00 21 01 3E 18 20 00 00 00 00 3F
E5 0F 05 49 4B 1F 11 3E 18 20 00 7F 00 00 65
ED 0F 01 05 00 21 01 3E 18 21 00 00 00 00 3E
E5 0F 05 49 4B 1F 11 3E 18 21 00 7F 00 00 64
ED 0F 01 05 00 21 01 3E 18 22 00 00 00 00 3D
E5 0F 05 49 4B 1F 11 3E 18 22 00 7F 00 00 67
ED 0F 01 05 00 21 01 3E 18 23 00 00 00 00 3C
E5 0F 05 49 4B 1F 11 3E 18 23 00 7F 00 00 66
ED 0F 01 05 00 21 01 3E 18 24 00 00 00 00 3B
E5 0F 05 49 4B 1F 11 3E 18 24 00 7F 00 00 61
ED 0F 01 05 00 21 01 3E 18 25 00 00 00 00 3A
E5 0F 05 49 4B 1F 11 3E 18 25 00 7F 00 00 60
ED 0F 01 05 00 21 01 3E 18 26 00 00 00 00 39
E5 0F 05 49 4B 1F 11 3E 18 26 00 7F 00 00 63
ED 0F 01 05 00 21 01 3E 18 27 00 00 00 00 38
E5 0F 05 49 4B 1F 11 3E 18 27 00 7F 00 00 62
ED 0F 01 05 00 21 01 3E 18 28 00 00 00 00 37
E5 0F 05 49 4B 1F 11 3E 18 28 00 7F 00 00 6D
ED 0F 01 05 00 21 01 3E 18 29 00 00 00 00 36
E5 0F 05 49 4B 1F 01 3E 18 29 00 14 00 00 17
ED 0F 01 05 00 21 01 3E 18 2A 00 00 00 00 35
E5 0F 05 49 4B 1F 11 3E 18 2A 00 7F 00 00 6F
ED 0F 01 05 00 21 01 3E 18 2B 00 00 00 00 34
E5 0F 05 49 4B 1F 11 3E 18 2B 00 7F 00 00 6E
ED 0F 01 05 00 21 01 3E 18 2C 00 00 00 00 33
E5 0F 05 49 4B 1F 11 3E 18 2C 00 7F 00 00 69
ED 0F 01 05 00 21 01 3E 18 2D 00 00 00 00 32
E5 0F 05 49 4B 1F 11 3E 18 2D 00 7F 00 00 68
ED 0F 01 05 00 21 01 3E 18 2E 00 00 00 00 31
E5 0F 05 49 4B 1F 11 3E 18 2E 00 7F 00 00 6B
ED 0F 01 05 00 21 01 3E 18 2F 00 00 00 00 30
E5 0F 05 49 4B 1F 11 3E 18 2F 00 7F 00 00 6A
ED 0F 01 05 00 21 01 3E 18 30 00 00 00 00 2F
E5 0F 05 49 4B 1F 11 3E 18 30 00 7F 00 00 75


Uhlenbrock programming WRITE ALL 63340 module 2 (10,…17)
A0 0C 0B 58
ED 0F 01 05 00 20 01 3E 18 01 00 0A 00 00 15F (OPC_IMM_PACKET)
B4 6D 7F 59 (OPC_LONG_ACK)
ED 0F 01 05 00 20 01 3E 18 02 00 0B 00 00 17
B4 6D 7F 59
ED 0F 01 05 00 20 01 3E 18 03 00 0C 00 00 11
B4 6D 7F 59
ED 0F 01 05 00 20 01 3E 18 04 00 0D 00 00 17
B4 6D 7F 59
ED 0F 01 05 00 20 01 3E 18 05 00 0E 00 00 15
B4 6D 7F 59
ED 0F 01 05 00 20 01 3E 18 06 00 0F 00 00 17
B4 6D 7F 59
ED 0F 01 05 00 20 01 3E 18 07 00 10 00 00 09
B4 6D 7F 59
ED 0F 01 05 00 20 01 3E 18 08 00 11 00 00 07
B4 6D 7F 59
A0 0C 0B 58


*/


#endif /*LNCV_H_*/
