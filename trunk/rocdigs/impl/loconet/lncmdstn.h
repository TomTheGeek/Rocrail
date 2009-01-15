#ifndef LNCMDSTN_H_
#define LNCMDSTN_H_

#define MIN_OPTION        1
#define MAX_OPTION        51

const int opsw_dcs100[] = {2,3,5,9,10,11,12,13,14,15,17,18,20,21,22,23,25,26,27,28,31,33,34,36,37,38,39,41,42,43,44,45,47,0};
const int opsw_db150[]  = {2,3,5,13,14,15,17,18,20,21,22,23,25,27,33,34,36,38,39,41,42,43,49,50,51,0};
const int opsw_intellibox[]  = {0,0};
const int opsw_dcs50[] = {1,2,3,5,6,13,14,15,17,18,20,21,22,23,27,28,33,34,36,39,43,45,0};

const char* cmdstnOPTIONS[] = {
/* 00 */  "",
/* 01 */  "01 Jump Port(s)",
/* 02 */  "02 Booster/throttle only override",
/* 03 */  "03 Booster is autoreversing",
/* 04 */  "",
/* 05 */  "05 Command station master mode",
/* 06 */  "06 Function 3 is non-latching",
/* 07 */  "",
/* 08 */  "",
/* 09 */  "09 Allow Motorola trinary echo for 1-256",
/* 10 */  "10 Expand trinary switch echo",
/* 11 */  "11 Make certain trinary switches long duration",
/* 12 */  "12 Trinary addresses 1-80 allowed",
/* 13 */  "13 Purge time increased to 600 seconds",
/* 14 */  "14 Purging is disabled",
/* 15 */  "15 Purge will force a loco to Stop or 0 speed",
/* 16 */  "",
/* 17 */  "17 Automatic advanced consists are disabled",
/* 18 */  "18 Extend booster short shutdown to 1/2 second",
/* 19 */  "",
/* 20 */  "20 Disable address 0 analog operation",
/* 21 */  "21 Global default for new loco: see manual",
/* 22 */  "22 Global default for new loco: see manual",
/* 23 */  "23 Global default for new loco: see manual",
/* 24 */  "",
/* 25 */  "25 Disable aliasiing (do not change DB150)",
/* 26 */  "26 Enable routes",
/* 27 */  "27 Disable normal switch commands (Bushby bit)",
/* 28 */  "28 Disable DS54 interrogate at power on",
/* 29 */  "",
/* 30 */  "",
/* 31 */  "31 Meter route/switch output rate when not trinary",
/* 32 */  "",
/* 33 */  "33 Restore track power to prior state at power on",
/* 34 */  "34 Allow track to power up to run state",
/* 35 */  "",
/* 36 */  "36 Clear all mobile decoder info and consists",
/* 37 */  "37 Clear all routes",
/* 38 */  "38 Clear the loco roster",
/* 39 */  "39 Clear all internal memory",
/* 40 */  "",
/* 41 */  "41 Diagnostic click when LocoNet command received",
/* 42 */  "42 Disable 3 beeps when loco address purged",
/* 43 */  "43 Disable LocoNet update of track status",
/* 44 */  "44 Expand slots to 120",
/* 45 */  "45 Disable reply for switch state request",
/* 46 */  "",
/* 47 */  "47 Program track is brake generator",
/* 48 */  "",
/* 49 */  "49 No beep when DB150 sends program command",
/* 50 */  "50 Longer booster short circuit recovery time",
/* 51 */  "51 Do not allow EXT voltage RESTART"
};

#endif /*LNCMDSTN_H_*/
