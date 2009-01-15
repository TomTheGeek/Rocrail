/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
/** ------------------------------------------------------------
 * Module: RocRail
 * Object: Throttle
 * ------------------------------------------------------------
 */

/*
TODO:

- Start Stop automode OK
- Stop button OK
- Select schedule
- Select block
- Set Lok to block
- Clear block
- Goto block
- Nicer display layout
- wrapper for different displays
*/


#include "rocrail/impl/throttle_impl.h"

#include "rocrail/public/app.h"
#include "rocrail/public/model.h"

#include "rocrail/wrapper/public/Throttle.h"
#include "rocrail/wrapper/public/ThrottleCmd.h"

#include "rocrail/wrapper/public/Loc.h"
#include "rocrail/wrapper/public/Plan.h"
#include "rocrail/wrapper/public/SysCmd.h"
#include "rocrail/wrapper/public/ModelCmd.h"
#include "rocrail/wrapper/public/AutoCmd.h"
#include "rocrail/wrapper/public/State.h"
#include "rocrail/wrapper/public/Exception.h"

#include "rocrail/wrapper/public/Switch.h"
#include "rocrail/wrapper/public/SwitchList.h"

#include "rocs/public/doc.h"
#include "rocs/public/trace.h"
#include "rocs/public/node.h"
#include "rocs/public/thread.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"

static int instCnt = 0;

/** ----- OBase ----- */
static void __del(void* inst) {
  if (inst != NULL) {
    iOThrottleData data = Data(inst);
    /* Cleanup data->xxx members...*/

    freeMem( data );
    freeMem( inst );
    instCnt--;
  }
  return;
}

static const char* __name(void) {
  return name;
}

static unsigned char* __serialize(void* inst, long* size) {
  return NULL;
}

static void __deserialize(void* inst, unsigned char* bytestream) {
  return;
}

static char* __toString(void* inst) {
  return NULL;
}

static int __count(void) {
  return instCnt;
}

static struct OBase* __clone(void* inst) {
  return NULL;
}

static Boolean __equals(void* inst1, void* inst2) {
  return False;
}

static void* __properties(void* inst) {
  return NULL;
}

static const char* __id(void* inst) {
  return NULL;
}



/** ----- OThrottle ----- */

#define MENU_OPEN  1
#define MENU_UPDATE  2
#define MENU_CLOSE  3


#define SCREEN_LOK          0
#define SCREEN_WEICHE       1
#define SCREEN_CV           2
#define SCREEN_SETUP        3
#define SCREEN_LOKSEL       4
#define SCREEN_FEEDBACK     5
#define SCREEN_AUTOMODE     6

/* KEYS FROM key.h */
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

#define  KEY_PARSER          60      /* this is a dummy keystroke
                                        issued by parser, to force an update*/

typedef struct
  {
    unsigned char key;
    unsigned char menucode;
    unsigned char slot;
    signed char value;
  }
  t_navicode;

typedef struct
  {
    unsigned char key;
    unsigned char index_of_target_screen;
  }
  t_navigate;


typedef void (*Ptr_To_Function)(t_navicode par);

struct Navigate {
    int key;
    int index_of_target_screen;  /* if key==0, last entry*/
};

struct MenuRecord {
    char * name;
    Ptr_To_Function action; /* Parameters: OPEN, CLOSE, UPDATE+Key*/
};

static Boolean send_string_message(int slot, int x, int y, const char * str) {
  if( StrOp.len( str) > 10) {
      return 0;
  }

  iOControl control = AppOp.getControl();

  iONode event = NodeOp.inst( wThrottleCmd.name(), NULL, ELEMENT_NODE );

  NodeOp.setStr(event, "str", str);
  NodeOp.setInt( event, "slot", slot );
  NodeOp.setInt( event, "type", 0x10 );
  NodeOp.setInt( event, "xpos", x );
  NodeOp.setInt( event, "ypos", y );

  ControlOp.cmd(control, (iONode) NodeOp.base.clone(event), NULL );
  NodeOp.base.del(event);

  return 1;
}

static Boolean send_clear_display(int slot){
  send_string_message(slot,0,0,"$");
  return 1;
}

static Boolean send_int_message(int slot, int x, int y, int data){

  char r0[6];
  sprintf( r0, "%4.0d", data );

  if( data == 0) {
    r0[0] = ' ';
    r0[1] = ' ';
    r0[2] = ' ';
    r0[3] = '0';
  }

  send_string_message(slot, x, y, r0);
}

static Boolean send_bye_throttle(int slot){
  iOControl control = AppOp.getControl();

  iONode event = NodeOp.inst( wThrottleCmd.name(), NULL, ELEMENT_NODE );

  NodeOp.setInt( event, "slot", slot );
  NodeOp.setInt( event, "type", 0x00 );
  NodeOp.setInt( event, "xpos", 0x00 );

  ControlOp.cmd(control, (iONode) NodeOp.base.clone(event), NULL );
  NodeOp.base.del(event);

}

static void send_loc_dir(int slot, int dir) {
  if( dir) {
      send_string_message(slot, 0, 3, ">");
    } else {
      send_string_message(slot, 0, 3, "<");
    }
}


/*========================================================================
 This is the central menu structure */
struct s_Menu {
  Ptr_To_Function action;
  t_navigate *navigate;
};

/* each screen must have a function*/
void action_lok(t_navicode par);
void action_weiche(t_navicode par);
void action_cv(t_navicode par);
void action_setup(t_navicode par);
void action_loksel(t_navicode par);
void action_feedback(t_navicode par);
void action_automode(t_navicode par);

t_navigate navigate_lok[] = {
    { KEY_MEC_RIGHT, SCREEN_WEICHE },
    { KEY_MEC_LEFT, SCREEN_LOKSEL },
    { 0, 0 },
    };

t_navigate navigate_weiche[] = {
    { KEY_MEC_RIGHT, SCREEN_AUTOMODE },
    { KEY_MEC_LEFT,
    SCREEN_LOK }, { 0, 0 },
    };

t_navigate navigate_automode[] = {
    { KEY_MEC_RIGHT, SCREEN_CV },
    { KEY_MEC_LEFT, SCREEN_LOK },
    { 0, 0 },
    };

t_navigate navigate_cv[] = {
    { KEY_MEC_RIGHT, SCREEN_SETUP },
    { KEY_MEC_LEFT, SCREEN_LOK },
    { 0, 0 },
    };

t_navigate navigate_setup[] = { { KEY_MEC_RIGHT, SCREEN_FEEDBACK }, {
    KEY_MEC_LEFT, SCREEN_LOK }, { 0, 0 },
    };

t_navigate navigate_feedback[] = { { KEY_MEC_RIGHT, SCREEN_LOK }, {
    KEY_MEC_LEFT, SCREEN_LOK }, { 0, 0 },
    };

t_navigate navigate_loksel[] = { { KEY_MEC_RIGHT, SCREEN_LOK }, { KEY_MEC_LEFT,
    SCREEN_LOK }, { KEY_KNOB_PUSHED, SCREEN_LOK }, { 0, 0 },
    };

struct s_Menu Menu[] = { [SCREEN_LOK] = {action_lok, navigate_lok},
    [SCREEN_WEICHE] = {action_weiche, navigate_weiche},
    [SCREEN_CV] = {action_cv, navigate_cv},
    [SCREEN_SETUP] = {action_setup, navigate_setup},
    [SCREEN_LOKSEL] = {action_loksel, navigate_loksel},
    [SCREEN_FEEDBACK] = {action_feedback, navigate_feedback},
    [SCREEN_AUTOMODE] = {action_automode, navigate_automode} };


Boolean lok_init;
int dreh_incx;
int currLoc;
int currSpeed;
Boolean currDir;
int oldSpeed;

Boolean powerState;

int currTurnout0;
int currTurnout1;

int drehInc;

iOList locIDs;
int currSlot;

iOMap turnouts;
iOList turnoutList;

static void touch_curr_loc() {
  /* touch the loc */
  iOLoc loc = ModelOp.getLoc(AppOp.getModel(), (const char*) ListOp.get(locIDs, currLoc));
  iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
  LocOp.cmd( loc, cmd);
}

static void touch_curr_turnout() {
  /* touch the turnout
  iOSwitch to0 = (iOSwitch) ListOp.get(turnoutList, currTurnout0);
  iONode cmd0 = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
  SwitchOp.cmd( to0, cmd0, True, 0);

  iOSwitch to1 = (iOSwitch) ListOp.get(turnoutList, currTurnout1);
  iONode cmd1 = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
  SwitchOp.cmd( to1, cmd1, False, 0);
*/
}

void action_lok(t_navicode par) {
  iOModel model = AppOp.getModel();
  iOControl control = AppOp.getControl();

  /* TODO: sort the list and throw out locos not shown ? */

  int nLocList = ListOp.size( locIDs );
  const char* id;

  /* first run */
  if (!lok_init) {
    par.menucode = MENU_OPEN;
  }
  switch (par.menucode) {
  case MENU_OPEN:
    send_clear_display(par.slot);
    send_string_message(par.slot, 0, 1, "-R- Lok");

    send_string_message(par.slot,  0, 2, "     ");
    send_string_message(par.slot,  0, 2, (const char*) ListOp.get(locIDs, currLoc));
    send_string_message(par.slot,  0, 4, "GO   ");
    send_string_message(par.slot,  5, 4, "STOP ");

    send_string_message(par.slot, 10, 4, "UP   ");
    send_string_message(par.slot, 15, 4, "DOWN ");

    touch_curr_loc();

    currSpeed = 0;
    oldSpeed = 0;
    lok_init = True;
    break;
  case MENU_UPDATE:
    switch (par.key) {
    case KEY_KNOB_PUSHED:
      {
        /* Loc dirchange */
        iOLoc loc = ModelOp.getLoc(model, (const char*) ListOp.get(locIDs, currLoc));
        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setV( cmd, 0 );
        wLoc.setdir( cmd, !LocOp.getDir( loc) );
        LocOp.cmd( loc, cmd);
      }
      break;
    case KEY_MEC_UP:
      currLoc++;
      if( currLoc >= nLocList)
        currLoc = 0;

      touch_curr_loc();

      /* Hat die ID eine feste Länge? */
      send_string_message(par.slot, 0, 2, "       ");
      send_string_message(par.slot, 0, 2, (const char*) ListOp.get(locIDs, currLoc));
      currSpeed = 0;
      break;
    case KEY_MEC_DOWN:
      currLoc--;
      if( currLoc < 0)
        currLoc = nLocList-1;

      touch_curr_loc();

      /* Hat die ID eine feste Länge? */
      send_string_message(par.slot, 0, 2, "       ");
      send_string_message(par.slot, 0, 2, (const char*) ListOp.get(locIDs, currLoc));
      currSpeed = 0;
      break;
    case KEY_KNOB_TURNED:
      {

        iOLoc loc = ModelOp.getLoc(model, (const char*) ListOp.get(locIDs, currLoc));
        iONode props = LocOp.base.properties(loc);

        /*
        int spcnt = wLoc.getspcnt( props);
        float step = (float) vMax/spcnt;
        TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "spcnt [%f] ", step);
        */

        int vMax = wLoc.getV_max( props);
        int v = LocOp.getV( loc)+par.value;

        if ( v < 0) v = 0;
        if ( v > vMax) v = vMax;

        iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
        wLoc.setdir( cmd, LocOp.getDir( loc) );
        wLoc.setV( cmd, v );
        LocOp.cmd( loc, cmd);

      }
      break;
    case KEY_SOFT_1: /* GO */
    {
      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setcmd( cmd, wSysCmd.go );
      LocOp.cmd( ModelOp.getLoc(model, (const char*) ListOp.get(locIDs, currLoc)),
          cmd);
      break;
    }
    case KEY_SOFT_2: /* STOP */
    {
      iONode cmd = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setcmd( cmd, wSysCmd.stop );
      LocOp.cmd( ModelOp.getLoc(model, (const char*)ListOp.get(locIDs, currLoc)),
          cmd);
      break;
    }

    case KEY_SOFT_3: /* GOSC */
    {
      iONode cmd0 = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setcmd( cmd0, wLoc.useschedule );
      wLoc.setscheduleid( cmd0, (const char*) "01-up");
      LocOp.cmd( ModelOp.getLoc(model, (const char*)ListOp.get(locIDs, currLoc)),
          cmd0);

      iONode cmd1 = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setcmd( cmd1, wSysCmd.go );
      LocOp.cmd( ModelOp.getLoc(model, (const char*)ListOp.get(locIDs, currLoc)),
          cmd1);
      break;
    }
    case KEY_SOFT_4: /* SC */
    {
      iONode cmd0 = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setcmd( cmd0, wLoc.useschedule );
      wLoc.setscheduleid( cmd0, (const char*) "02-down");
      LocOp.cmd( ModelOp.getLoc(model, (const char*)ListOp.get(locIDs, currLoc)),
          cmd0);

      iONode cmd1 = NodeOp.inst( wLoc.name(), NULL, ELEMENT_NODE );
      wLoc.setcmd( cmd1, wSysCmd.go );
      LocOp.cmd( ModelOp.getLoc(model, (const char*)ListOp.get(locIDs, currLoc)),
          cmd1);
      break;
    }

  }
  case MENU_CLOSE:
    break;
  }
}

enum Focus {left, right};
enum Focus focus = left;

void action_weiche(t_navicode par) {
  iOModel model = AppOp.getModel();
  iOControl control = AppOp.getControl();


  int noTurnouts = MapOp.size( turnouts );
  const char* id;

  switch (par.menucode) {
  case MENU_OPEN:
    send_clear_display(par.slot);
    send_string_message(par.slot, 0, 1, "-R- Weiche");

    send_string_message(par.slot,  2, 4, "0");
    send_string_message(par.slot,  7, 4, "1");

    send_string_message(par.slot, 12, 4, "0");
    send_string_message(par.slot, 17, 4, "1");

    iOSwitch to0 = (iOSwitch) ListOp.get(turnoutList, currTurnout0);
    send_string_message(par.slot, 0, 2,  SwitchOp.getId( to0));

    iOSwitch to1 = (iOSwitch) ListOp.get(turnoutList, currTurnout1);
    send_string_message(par.slot, 10, 2,  SwitchOp.getId( to1));

    touch_curr_turnout();
    break;
  case MENU_UPDATE:
    switch (par.key) {
      case KEY_KNOB_PUSHED:

        if( focus == left) focus = right;
        else focus = left;

      break;
    case KEY_SOFT_1:
      {
        iOSwitch to = (iOSwitch) ListOp.get(turnoutList, currTurnout0);
        iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
        wSwitch.setcmd(cmd, wSwitch.straight);
        SwitchOp.cmd( to, cmd, True, 0);
      }
      break;
    case KEY_SOFT_2:
      {
         iOSwitch to = (iOSwitch) ListOp.get(turnoutList, currTurnout0);
         iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
         wSwitch.setcmd(cmd, wSwitch.turnout);
         SwitchOp.cmd( to, cmd, True, 0);
       }
      break;
    case KEY_SOFT_3:
      {
        iOSwitch to = (iOSwitch) ListOp.get(turnoutList, currTurnout1);
        iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
        wSwitch.setcmd(cmd, wSwitch.straight);
        SwitchOp.cmd( to, cmd, True, 0);
      }
      break;
    case KEY_SOFT_4:
      {
         iOSwitch to = (iOSwitch) ListOp.get(turnoutList, currTurnout1);
         iONode cmd = NodeOp.inst( wSwitch.name(), NULL, ELEMENT_NODE );
         wSwitch.setcmd(cmd, wSwitch.turnout);
         SwitchOp.cmd( to, cmd, True, 0);
       }
      break;
    case KEY_KNOB_TURNED:
      if( focus == left) {
        currTurnout0 += par.value;
        if (currTurnout0 < 0) currTurnout0 = noTurnouts-1;
        if (currTurnout0 >= noTurnouts) currTurnout0 = 0;
        iOSwitch to = (iOSwitch) ListOp.get(turnoutList, currTurnout0);
        send_string_message(par.slot, 0, 2,  SwitchOp.getId( to));
        touch_curr_turnout();
      } else if( focus == right) {
        currTurnout1 += par.value;
        if (currTurnout1 < 0) currTurnout1 = noTurnouts-1;
        if (currTurnout1 >= noTurnouts) currTurnout1 = 0;
        iOSwitch to = (iOSwitch) ListOp.get(turnoutList, currTurnout1);
        send_string_message(par.slot, 10, 2,  SwitchOp.getId( to));
        touch_curr_turnout();
      }
      break;
    }
  case MENU_CLOSE:

    break;
  }
}

void action_automode(t_navicode par) {
  iOModel model = AppOp.getModel();
  iOControl control = AppOp.getControl();

  switch (par.menucode) {
  case MENU_OPEN:
    send_clear_display(par.slot);
    send_string_message(par.slot, 0, 1, "-R- Auto");
    send_string_message(par.slot,  0, 4, "ON  ");
    send_string_message(par.slot,  5, 4, "OFF ");
    break;
  case MENU_UPDATE:
    switch (par.key) {
      case KEY_KNOB_PUSHED:
      break;
    case KEY_SOFT_1:
      {
        iONode cmd = NodeOp.inst( wAutoCmd.name(), NULL, ELEMENT_NODE );
        wAutoCmd.setcmd(cmd, wAutoCmd.on);
        ModelOp.cmd( model, cmd);
      }
      break;
    case KEY_SOFT_2:
      {
        iONode cmd = NodeOp.inst( wAutoCmd.name(), NULL, ELEMENT_NODE );
        wAutoCmd.setcmd(cmd, wAutoCmd.off);
        ModelOp.cmd( model, cmd);
      }
      break;
    }
  case MENU_CLOSE:
    break;
  }
}

void action_cv(t_navicode par) {
  switch (par.menucode) {
  case MENU_OPEN:
    send_clear_display(10);
    send_string_message(10, 0, 1, "-R- Prog");
    break;
  case MENU_UPDATE:
    switch (par.key) {
    case KEY_KNOB_PUSHED:
      break;
    }
  case MENU_CLOSE:
    break;
  }
}
void action_setup(t_navicode par) {
  switch (par.menucode) {
  case MENU_OPEN:
    send_clear_display(par.slot);
    send_string_message(par.slot, 0, 1, "-R- Setup");
    send_string_message(par.slot, 17, 4, "bye");
    break;
  case MENU_UPDATE:
    switch (par.key) {
    case KEY_KNOB_PUSHED:
      break;

    case KEY_SOFT_4:
      send_bye_throttle(par.slot);
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Throttle [%d] disconnected.", par.slot);
      break;
    }
  case MENU_CLOSE:
    break;
  }
}
void action_loksel(t_navicode par) {
  switch (par.menucode) {
  case MENU_OPEN:
    send_clear_display(par.slot);
    send_string_message(par.slot, 0, 1, "-R- LokSel");
    break;
  case MENU_UPDATE:
    switch (par.key) {
    case KEY_KNOB_PUSHED:
      break;
    }
  case MENU_CLOSE:
    break;
  }
}
void action_feedback(t_navicode par) {
  switch (par.menucode) {
  case MENU_OPEN:
    send_clear_display(par.slot);
    send_string_message(par.slot, 0, 1, "-R- FB");
    break;
  case MENU_UPDATE:
    switch (par.key) {
    case KEY_KNOB_PUSHED:
      break;
    }
  case MENU_CLOSE:
    break;
  }
}

unsigned char actual_screen = SCREEN_LOK;

/** Incomming throttle commands from the digints layer. */
static void _cmd(struct OThrottle* inst, iONode throttlecmd) {
  iOThrottleData data = Data(inst);
  iOControl control = AppOp.getControl();
  iOModel model = AppOp.getModel();

  int val = 0;

  if (throttlecmd != NULL) {

    t_navigate *actual_navi;
    t_navicode temp;

    TraceOp.trc(name, TRCLEVEL_DEBUG, __LINE__, 9999,
        "Throttle command received");

    temp.slot = NodeOp.getInt(throttlecmd, "slot", val);
    int type = NodeOp.getInt(throttlecmd, "type", val);
    temp.key = NodeOp.getInt(throttlecmd, "key", val);
    int val = NodeOp.getInt(throttlecmd, "val", val);

    if( val == 1) {
      temp.value = 1;
    } else {
      temp.value = -1;
    }

    TraceOp.trc(name, TRCLEVEL_DEBUG, __LINE__, 9999,
        "tCmd 0x%2X 0x%X 0x%X 0x%X", temp.slot, type, temp.key, val);


    if (type == 0x00 && temp.key == 0x01) {
     TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Throttle [%d] connected.", temp.slot);


     currSlot = temp.slot;
     actual_screen = SCREEN_LOK;
     lok_init = False;
    }

    /* mhh */
    if (type == 0x00 && temp.key == 0x00) {
      TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Throttle [%d] said goodbye.", temp.slot);
    }


    if (temp.key == KEY_NOTHALT) { /* Hotkey = Special*/
      TraceOp.trc(name, TRCLEVEL_DEBUG, __LINE__, 9999, "tCmd OFF/ON");
      {
        iONode cmd = NodeOp.inst( wSysCmd.name(), NULL, ELEMENT_NODE );
        if( powerState ) {
          wSysCmd.setcmd( cmd, wSysCmd.stop );
        } else {
          wSysCmd.setcmd( cmd, wSysCmd.go );
        }
        ControlOp.cmd( AppOp.getControl(), cmd, NULL );
      }

    }

    /* now evaluate key and scan for navigate */
    actual_navi = &Menu[actual_screen].navigate[0];

    int i = 0;
    while (actual_navi->key != 0) {
      if (actual_navi->key == temp.key) {
        /* hey, lets go away */
        temp.menucode = MENU_CLOSE;
        Menu[actual_screen].action(temp);
        /* this is our new screen */
        actual_screen = actual_navi->index_of_target_screen;
        temp.menucode = MENU_OPEN;
        Menu[actual_screen].action(temp);
        return;
      }
      actual_navi = &Menu[actual_screen].navigate[i++];
    }

    /* nothing to go away - forward keystroke to action. */
    temp.menucode = MENU_UPDATE;
    Menu[actual_screen].action(temp);
  }
  return;
}

/** Expecting broadcasts from other objects here. */
static void* __event(void* inst, const void* evt) {
  iOThrottleData data = Data(inst);
  iONode event = (iONode) evt;

  TraceOp.trc(name, TRCLEVEL_DEBUG, __LINE__, 9999, "**** EVENT %s %s",
      NodeOp.getName( event ), wState.name());

  /* TODO: processing the node and feed the throttle with bits when needed... */
  if( StrOp.equals( NodeOp.getName( event ), wSwitch.name() )  &&
    (actual_screen == SCREEN_WEICHE) ) {

    iOSwitch to0 = (iOSwitch) ListOp.get(turnoutList, currTurnout0);
    iOSwitch to1 = (iOSwitch) ListOp.get(turnoutList, currTurnout1);

    if( StrOp.equals(wSwitch.getid( event), (const char*) SwitchOp.getId(to0 ) ) ) {
      if (StrOp.equals( wSwitch.getstate( event ), wSwitch.straight ) ) {
        send_string_message(currSlot, 1, 4,  "*");
        send_string_message(currSlot, 6, 4,  " ");
      } else {
        send_string_message(currSlot, 1, 4,  " ");
        send_string_message(currSlot, 6, 4,  "*");
      }
    }

    if( StrOp.equals(wSwitch.getid( event), (const char*) SwitchOp.getId(to1 ) ) ) {
      if (StrOp.equals( wSwitch.getstate( event ), wSwitch.straight ) ) {
        send_string_message(currSlot, 11, 4,  "*");
        send_string_message(currSlot, 16, 4,  " ");
      } else {
        send_string_message(currSlot, 11, 4,  " ");
        send_string_message(currSlot, 16, 4,  "*");
      }
    }


  } else if( StrOp.equals( NodeOp.getName( event ), wLoc.name() ) &&
      (actual_screen == SCREEN_LOK) ) {
    /*
     * Our loc
     */
    if( StrOp.equals(wLoc.getid( event), (const char*) ListOp.get(locIDs, currLoc) ) ) {

      iOLoc loc = ModelOp.getLoc(AppOp.getModel(),
          (const char*) ListOp.get(locIDs, currLoc));

      send_loc_dir(currSlot, LocOp.getDir( loc));
      send_int_message(currSlot, 2, 3, wLoc.getV( event ));
    }
  } else if( StrOp.equals( NodeOp.getName( event ), wState.name() )  ) {

    /* POWER event */
    powerState = wState.ispower( event );
    if( wState.ispower( event ))
      send_string_message(currSlot, 17, 1, " ON");
    else
      send_string_message(currSlot, 17, 1, "OFF");

  } else if( StrOp.equals( NodeOp.getName( event ), wException.name() )  ) {

    /*
    TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "EXIT %s %s",
          NodeOp.getName( event ), wException.gettext( event));
    */

  }

  NodeOp.base.del(event);

  return NULL;
}

/**  */
static struct OThrottle* _inst(iONode ini) {
  iOThrottle __Throttle = allocMem( sizeof( struct OThrottle ) );
  iOThrottleData data = allocMem( sizeof( struct OThrottleData ) );
  MemOp.basecpy(__Throttle, &ThrottleOp, 0, sizeof(struct OThrottle), data);

  /* Initialize data->xxx members... */
  data->ini = ini;

  lok_init = False;
  currTurnout0 = 0;
  currTurnout1 = 0;
  drehInc = 0;
  currLoc = 0;
  currDir = 0;
  currSlot = 0;
  locIDs = ModelOp.getLocIDs( AppOp.getModel() );
  turnouts = ModelOp.getSwitchMap( AppOp.getModel() );
  turnoutList = MapOp.getList(turnouts);

  TraceOp.trc(name, TRCLEVEL_INFO, __LINE__, 9999, "Throttle instantiated.");

  instCnt++;
  return __Throttle;
}

/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
#include "rocrail/impl/throttle.fm"
/* ----- DO NOT REMOVE OR EDIT THIS INCLUDE LINE! -----*/
