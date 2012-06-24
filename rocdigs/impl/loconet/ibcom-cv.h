/*
 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.
 */

#ifndef IBCOMCV_H_
#define IBCOMCV_H_
#include "rocs/public/rocs.h"

int makeIBComCVPacket(int cv, int value, byte* buffer, Boolean write);
int startIBComPT(byte* buffer);
int stopIBComPT(byte* buffer);
void initIBCom(iOLocoNet loconet);

#endif /* IBCOMCV_H_ */
