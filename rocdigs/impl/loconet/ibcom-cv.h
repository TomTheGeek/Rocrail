/*
 * ibcom-cv.h
 *
 *  Created on: Sep 12, 2009
 *      Author: rob
 */

#ifndef IBCOMCV_H_
#define IBCOMCV_H_
#include "rocs/public/rocs.h"

int makeIBComCVPacket(int cv, byte* buffer);
int evaluateIBComCVPacket(byte* buffer);

#endif /* IBCOMCV_H_ */
