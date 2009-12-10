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

#ifndef ROCNETCONST_H_
#define ROCNETCONST_H_


/* groups */
#define RN_GROUP_CS            1
#define RN_GROUP_MOBILE        2
#define RN_GROUP_STATIONARY    3
#define RN_GROUP_PT_MOBILE     4
#define RN_GROUP_PT_STATIONARY 5
#define RN_GROUP_GP_DATA       6
#define RN_GROUP_CLOCK         7
#define RN_GROUP_SENSOR        8
#define RN_GROUP_OUTPUT        9
#define RN_GROUP_INPUT        10

/* command station group actions */
#define RN_CS_NOP            0
#define RN_CS_QUERY          1
#define RN_CS_TRACKPOWER     2
#define RN_CS_TRACKPOWER_ON  1 /* data byte */
#define RN_CS_TRACKPOWER_OFF 0 /* data byte */
#define RN_CS_HALT           3
#define RN_CS_VERSION        4
#define RN_CS_PT             5
#define RN_CS_PT_ON          1 /* data byte */
#define RN_CS_PT_OFF         0 /* data byte */

/* mobile group actions */
#define RN_MOBILE_SETUP          1
#define RN_MOBILE_VELOCITY       2
#define RN_MOBILE_FUNCTIONS      3
#define RN_MOBILE_QUERY          4

#define RN_MOBILE_DIR_FORWARDS  0x01
#define RN_MOBILE_LIGHTS_ON     0x01

#define RN_MOBILE_PROT_256      0
#define RN_MOBILE_PROT_DCC28    1
#define RN_MOBILE_PROT_DCC128   2
#define RN_MOBILE_PROT_DCC14    3
#define RN_MOBILE_PROT_MM       4



/* stationary group actions */
#define RN_OUTPUT_SWITCH       1
#define RN_OUTPUT_SWITCH_MULTI 2
#define RN_OUTPUT_ON           1
#define RN_OUTPUT_OFF          0

/* stationary group actions */
#define RN_STATIONARY_SINGLE_PORT        1
#define RN_STATIONARY_PORT_PAIR          2
#define RN_STATIONARY_MULTI_PORT         3
#define RN_STATIONARY_QUERY_SINGLE_PORT  4
#define RN_STATIONARY_QUERY_PORT_PAIR    5
#define RN_STATIONARY_QUERY_MULTI_PORT   6
#define RN_STATIONARY_REPORT             7
#define RN_STATIONARY_QUERY              8


/* programming group actions */
#define RN_PROGRAMMING_WRITE  1
#define RN_PROGRAMMING_READ   2
#define RN_PROGRAMMING_COPY   3


/* extended packet offsets */
#define RN_PACKET_NETID  0
#define RN_PACKET_RCPTH  1
#define RN_PACKET_RCPTL  2
#define RN_PACKET_SNDRH  3
#define RN_PACKET_SNDRL  4
#define RN_PACKET_GROUP  5
#define RN_PACKET_ACTION 6
#define RN_PACKET_LEN    7
#define RN_PACKET_DATA   8

#define RN_ACTION_CODE_MASK  0x1F
#define RN_ACTION_TYPE_MASK  0xE0

/* action types */
#define RN_ACTIONTYPE_REQUEST  0
#define RN_ACTIONTYPE_EVENT    1
#define RN_ACTIONTYPE_REPLY    2
#define RN_ACTIONTYPE_REPLYEXP 3

/* clock group actions */
#define RN_CLOCK_SET    1
#define RN_CLOCK_SYNC   2

/* sensor group actions */
#define RN_SENSOR_REPORT    1


#endif /*ROCNETCONST_H_*/
