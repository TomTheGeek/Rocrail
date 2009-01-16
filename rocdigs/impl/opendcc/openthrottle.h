/*
 * openthrottle.h
 *
 *  Created on: 14.10.2008
 *      Author: jmf
 */

#ifndef OPENTHROTTLE_H_
#define OPENTHROTTLE_H_

#include "rocdigs/public/lenz.h"

#include "rocs/public/mem.h"
#include "rocs/public/objbase.h"
#include "rocs/public/string.h"

#define  KEY_SOFT_1           1
#define  KEY_SOFT_2           2
#define  KEY_SOFT_3           3
#define  KEY_SOFT_4           4

#define  KEY_MEC_UP           5
#define  KEY_MEC_DOWN         6
#define  KEY_MEC_OK           7
#define  KEY_MEC_LEFT         8
#define  KEY_MEC_RIGHT        9

#define  KEY_SHIFT           10
#define  KEY_NOTHALT         20

#define  KEY_KNOB_PUSHED     50
#define  KEY_KNOB_TURNED     51

#define  KEY_PARSER          60      // this is a dummy keystroke
                                     // issued by parser, to force an update


void evaluateThrottle( iOLenz inst, char* in );

void sendToThrottle( iOLenz data, int row, int col, int slot, const char* message );


#endif /* OPENTHROTTLE_H_ */
