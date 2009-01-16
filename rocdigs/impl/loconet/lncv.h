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

*/


#endif /*LNCV_H_*/
