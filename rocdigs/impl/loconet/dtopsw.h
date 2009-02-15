/*
 Rocrail - Model Railroad Software

 Copyright (C) Rob Versluis <r.j.versluis@rocrail.net>

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
#ifndef DTOPSW_H_
#define DTOPSW_H_

#include "rocdigs/impl/loconet/lnconst.h"

int DT_idxBDL16[] = {1,3,5,6,7,9,10,11,12,13,19,25,26,36,37,38,39,40,42,43,44,0};
const char* DT_BDL16[] = {
    /* 01 */ "Common rail wiring",
    /* 03 */ "Reverse polarity for detection",
    /* 05 */ "Enable transponding",
    /* 06 */ "Reserved (Unset if RX4 connected)",
    /* 07 */ "Reserved (Unset if RX4 connected)",
    /* 09 */ "Show unoccupied when power off",
    /* 10 */ "Section 16 used to sense power",
    /* 11 */ "Do not allow BDL16 to be LocoNet master",
    /* 12 */ "Do not allow BDL16 to terminate LocoNet",
    /* 13 */ "Delay only 1/2 second at power up",
    /* 19 */ "High threshold sense (10kohms)",
    /* 25 */ "Drive LEDs from switch commands, not occupancy",
    /* 26 */ "Decode switch commands from LocoNet",
    /* 36 */ "Ignore GPON messages, only reply to interrogate",
    /* 37 */ "Long detection delay (BDL168 only)",
    /* 38 */ "Extra long detection delay (BDL168 only)",
    /* 39 */ "Transponder Tracking (BDL168 only)",
    /* 40 */ "Restore factory default, including address",
    /* 42 */ "Turn off power-on interogate (BDL168 only)",
    /* 43 */ "Anti-chatter filtering (BDL168 only)",
    /* 44 */ "Anti-chatter filter sensitivity (BDL168 only)",
    NULL
  };

int DT_idxDS64[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,0};
const char* DT_DS64[] = {
    /* 01*/  "Static Output Type (Pulse if off)",
    /* 02 */ "Pulse Timeout  200ms",
    /* 03 */ "Pulse Timeout  400ms",
    /* 04 */ "Pulse Timeout  800ms",
    /* 05 */ "Pulse Timeout 1600ms",
    /* 06 */ "Output Power Management - Wait for 1st command",
    /* 07 */ "Reset Functions to Factory Default",
    /* 08 */ "Double normal startup delay",
    /* 09 */ "Turn off static outputs after 16sec",
    /* 10 */ "DS64 accepts computer commands only",
    /* 11 */ "Routes work from input lines",
    /* 12 */ "Either input high causes toggle",
    /* 13 */ "All eight inputs send sensor messages",
    /* 14 */ "Switch commands from track only",
    /* 15 */ "Outputs ignore inputs",
    /* 16 */ "Disable routes",
    /* 17 */ "Output 1 is crossing gate",
    /* 18 */ "Output 2 is crossing gate",
    /* 19 */ "Output 3 is crossing gate",
    /* 20 */ "Output 4 is crossing gate",
    /* 21 */ "Send turnout sensor messages (general sensor messages if off)",
    NULL
    };

int DT_idxSE8[] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,17,18,19,20,21,0};
const char* DT_SE8[] = {
    /* 01*/  "Reserved",
    /* 02*/  "Two aspects (one turnout address) per head",
    /* 03*/  "DRV1-4: see manual",
    /* 04*/  "DRV1-4",
    /* 05*/  "DRV1-4",
    /* 06*/  "DRV5-8: see manual",
    /* 07*/  "DRV5-8",
    /* 08*/  "DRV5-8",
    /* 09*/  "4th Aspect: see manual",
    /* 10*/  "4th Aspect",
    /* 11*/  "Semaphore mode",
    /* 12*/  "Pulsed switch outputs",
    /* 13*/  "Disable DS input",
    /* 14*/  "Enable switch command from loconet",
    /* 15*/  "Disable local switch control",
    /* 17*/  "Next switch command sets signal address",
    /* 18*/  "Next switch command sets broadcast address",
    /* 19*/  "Next switch command sets semaphore address",
    /* 20*/  "Restore factory default, including address",
    /* 21*/  "Show LED exercise pattern",
    NULL
};

const char* DT_BoardTypes[] = {"BDL16","DS64","SE8",NULL};
const char** DT_OpSw[] = {DT_BDL16, DT_DS64, DT_SE8};
int* DT_idxOpSw[] = {DT_idxBDL16, DT_idxDS64, DT_idxSE8};
int DT_msgType[] = {BOARD_TYPE_BDL16,BOARD_TYPE_DS64,BOARD_TYPE_SE8};

#endif
