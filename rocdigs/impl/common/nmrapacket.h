
int oneBytePacket(byte* retVal, int address, Boolean longAddr, byte arg1);
int twoBytePacket(byte* retVal, int address, Boolean longAddr, byte arg1, byte arg2);
int threeBytePacket(byte* retVal, int address, Boolean longAddr, byte arg1, byte arg2, byte arg3);

int accDecoderPkt2(byte* retVal, int addr, int active, int outputChannel);
int accDecoderPktOpsMode2(byte* retVal, int addr, int active, int outputChannel, int cvNum, int data);

int accDecPktOpsModeLegacy(byte* retVal, int addr, int cvNum, int data);

int accSignalDecoderPkt(byte* retVal, int outputAddr, int aspect);
int accDecoderPkt(byte* retVal, int number, Boolean closed);
int accDecoderPktOpsMode(byte* retVal, int number, int cvNum, int data);
int accDecoderPktOpsModeLegacy(byte* retVal, int number, int cvNum, int data);

int opsCvWriteByte(byte* retVal, int address, Boolean longAddr, int cvNum, int data );

int speedStep128Packet(byte* retVal, int address, Boolean longAddr, int speed, Boolean fwd );
int speedStep28Packet(byte* retVal, int address, Boolean longAddr, int speed, Boolean fwd );
int speedStep14Packet(byte* retVal, int address, Boolean longAddr, int speed, Boolean fwd, Boolean F0);
int function0Through4Packet(byte* retVal, int address, Boolean longAddr, Boolean f0, Boolean f1, Boolean f2, Boolean f3, Boolean f4 );
int function5Through8Packet(byte* retVal, int address, Boolean longAddr, Boolean f5, Boolean f6, Boolean f7, Boolean f8 );
int function9Through12Packet(byte* retVal, int address, Boolean longAddr, Boolean f9, Boolean f10, Boolean f11, Boolean f12 );

int function13Through20Packet(byte* retVal, int address, Boolean longAddr,
                    Boolean f13, Boolean f14, Boolean f15, Boolean f16,
                    Boolean f17, Boolean f18, Boolean f19, Boolean f20 );
int function21Through28Packet(byte* retVal, int address, Boolean longAddr,
                    Boolean f21, Boolean f22, Boolean f23, Boolean f24,
                    Boolean f25, Boolean f26, Boolean f27, Boolean f28 );
int analogControl(byte* retVal, int address, Boolean longAddr, int function, int value);
int consistControl(byte* retVal, int address, Boolean longAddr, int consist, Boolean directionNormal);

Boolean addressCheck(int address, Boolean longAddr);
