/*
 * openthrottle.c
 *
 *  Created on: 14.10.2008
 *      Author: jmf
 */
#include "openthrottle.h"
#include "rocdigs/impl/lenz_impl.h"

#include "rocrail/wrapper/public/RocRail.h"
#include "rocrail/wrapper/public/Ctrl.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/FunCmd.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Feedback.h"
#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/Output.h"
#include "rocrail/wrapper/public/Signal.h"
#include "rocrail/wrapper/public/Program.h"
#include "rocrail/wrapper/public/Response.h"

int currLoc = 0;

void evaluateThrottle(iOLenz inst, char* in) {
  iOLenzData data = Data(inst);

  TraceOp.trc("ot", TRCLEVEL_INFO, __LINE__, 9999,
      "Hello Rocrail. id=%d key=%d val=%d", in[1], in[3], in[4]);


  // ????? So?
  /* no */
  /*iOList locList = ModelOp.getLocIDs(AppOp.getModel());*/
  int locListSize = 0;/*ListOp.size(locList);*/


  // Das wird dann noch schöner. (siehe: menu_navigator.c)
  if (in[3] == KEY_MEC_UP) {
    currLoc++;

    if( currLoc >= locListSize)
      currLoc = 1;
  }

  if (in[3] == KEY_MEC_DOWN){
    currLoc--;

    if( currLoc <= 0)
      currLoc = locListSize-1;
  }


  // ************ BAUSTELLE *******************************
  // Wie passiert am Besten die Kommunikation mit dem Model?



      // iONode lc = (iONode)ListOp.get( locList, 1 );
      // const char* id = wLoc.getid( lc );
  //CRASH
     // TraceOp.trc("ot", TRCLEVEL_INFO, __LINE__, 9999, "%s", id);


  //const char * message = LocOp.getId(loc);



  //sendToThrottle(inst, 4, 0, in[1], message);

}

void sendToThrottle(iOLenz inst, int row, int col, int slot,
    const char* message) {
  iOLenzData data = Data(inst);
  int i;
  int msgsize = sizeof(message) - 1;

  char* outc = allocMem(256);
  outc[0] = 0x34 + msgsize;
  outc[1] = slot; // slot
  outc[2] = 0x21; // answer
  outc[3] = col; // display col
  outc[4] = row; // display row

  for (i = 0; i < msgsize; i++) {
    outc[i + 5] = message[i];
  }

  TraceOp.trc("ot", TRCLEVEL_INFO, __LINE__, 9999, "%d", outc[0] & 0x0F);

  ThreadOp.post(data->transactor, (obj) outc);
}

