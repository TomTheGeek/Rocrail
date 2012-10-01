/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

 Without an official permission commercial use is not permitted.
 Forking this project is not permitted.

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

#include "rocs/public/str.h"
#include "rocrail/wrapper/public/BiDiBnode.h"
#include "rocdigs/impl/bidib/bidib_messages.h"


char* bidibGetClassName(int classid ) {
  char* classname = NULL;
  if( classid & 0x80 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_bridge);
  }
  if( classid & 0x40 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_sensor);
  }
  if( classid & 0x20 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_ui);
  }
  if( classid & 0x10 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_dcc_loco);
  }
  if( classid & 0x08 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_dcc_acc);
  }
  if( classid & 0x04 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_prog);
  }
  if( classid & 0x02 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_booster);
  }
  if( classid & 0x01 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_switch);
  }

  if( classname == NULL )
    classname = StrOp.dup("");
  return classname;
}

const char* bidibGetFeatureName(int feature) {

  /* Occupancy detector */
  if( feature == FEATURE_BM_SIZE ) return "number of sensors";
  if( feature == FEATURE_BM_ON ) return "activated sensors events";
  if( feature == FEATURE_BM_SECACK_AVAILABLE ) return "secure-ACK available";
  if( feature == FEATURE_BM_SECACK_ON ) return "secure-ACK interval";
  if( feature == FEATURE_BM_CURMEAS_AVAILABLE ) return "current measurement available";
  if( feature == FEATURE_BM_CURMEAS_INTERVAL ) return "current measurement interval";
  if( feature == FEATURE_BM_DC_MEAS_AVAILABLE ) return "replacement detection available";
  if( feature == FEATURE_BM_DC_MEAS_ON ) return "replacement detection enabled";
  if( feature == FEATURE_BM_ADDR_DETECT_AVAILABLE ) return "address detection available";
  if( feature == FEATURE_BM_ADDR_DETECT_ON ) return "address detection enabled";
  if( feature == FEATURE_BM_ADDR_AND_DIR ) return "direction available";
  if( feature == FEATURE_BM_ISTSPEED_AVAILABLE ) return "dcc-speed available";
  if( feature == FEATURE_BM_ISTSPEED_ON ) return "dcc-speed enabled";
  if( feature == FEATURE_BM_CV_AVAILABLE ) return "cv-messages available";
  if( feature == FEATURE_BM_CV_ON ) return "cv-messages enabled";

  /* Booster */
  if( feature == FEATURE_BST_VOLT_ADJUSTABLE ) return "adjustable output voltage";
  if( feature == FEATURE_BST_VOLT ) return "output voltage value in V";
  if( feature == FEATURE_BST_CUTOUT_AVAIALABLE ) return "cutout available";
  if( feature == FEATURE_BST_CUTOUT_ON ) return "cutout enabled";
  if( feature == FEATURE_BST_TURNOFF_TIME ) return "turnoff time";
  if( feature == FEATURE_BST_INRUSH_TURNOFF_TIME ) return "inrush turnoff time";
  if( feature == FEATURE_BST_AMPERE_ADJUSTABLE ) return "ampere adjustable";
  if( feature == FEATURE_BST_AMPERE ) return "ampere";

  /* Control */
  if( feature == FEATURE_CTRL_INPUT_COUNT ) return "input count";
  if( feature == FEATURE_CTRL_INPUT_NOTIFY ) return "input notify";
  if( feature == FEATURE_CTRL_SPORT_COUNT ) return "short port count";
  if( feature == FEATURE_CTRL_LPORT_COUNT ) return "long port count";
  if( feature == FEATURE_CTRL_SERVO_COUNT ) return "servo count";
  if( feature == FEATURE_CTRL_SOUND_COUNT ) return "sound count";
  if( feature == FEATURE_CTRL_MOTOR_COUNT ) return "motor count";
  if( feature == FEATURE_CTRL_ANALOG_COUNT ) return "analog count";
  if( feature == FEATURE_CTRL_MAC_LEVEL ) return "MAC level";
  if( feature == FEATURE_CTRL_MAC_SAVE ) return "MAC save";
  if( feature == FEATURE_CTRL_MAC_COUNT ) return "MAC count";
  if( feature == FEATURE_CTRL_MAC_SIZE ) return "MAC size";
  if( feature == FEATURE_CTRL_MAC_START_MAN ) return "MAC start manuel";
  if( feature == FEATURE_CTRL_MAC_START_DCC ) return "MAC start DCC";

  /* DCC Gen */
  if( feature == FEATURE_GEN_SPYMODE ) return "DCC spymode";
  if( feature == FEATURE_GEN_WATCHDOG ) return "DCC watchdog";
  if( feature == FEATURE_GEN_DRIVE_ACK ) return "DCC drive ack";
  if( feature == FEATURE_GEN_SWITCH_ACK ) return "DCC switch ack";
  if( feature == FEATURE_GEN_LOK_DB_SIZE ) return "DCC loco DB size";
  if( feature == FEATURE_GEN_LOK_DB_STRING ) return "DCC loco DB string";
  if( feature == FEATURE_GEN_SERVICE_MODES ) return "DCC service modes";

  if( feature == FEATURE_FW_UPDATE_MODE ) return "firmware update mode";
  if( feature == FEATURE_EXTENSION ) return "reserved for future expansion";

  return "*** unknown feature ***";
}
