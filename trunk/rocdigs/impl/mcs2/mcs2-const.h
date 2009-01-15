/*
 Rocrail - Model Railroad Control System

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

/*
 * CAN over TCP/IP 13 byte format:
 *
 *  |  0   |  1    | 2 | 3 |  4  | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 |
 *  | PRIO CMD RSP | HASH  | DLC |             DATA                 |
 *
 *
 *
 *
 */


#ifndef MCS2CONST_H_
#define MCS2CONST_H_

/* SYSTEM */
#define CMD_SYSTEM            0x00
#define ID_SYSTEM             0x00
#define CMD_SYSSUB_STOP       0x00
#define CMD_SYSSUB_GO         0x01
#define CMD_SYSSUB_HALT       0x02
#define CMD_SYSSUB_EMBREAK    0x03
#define CMD_SYSSUB_STOPCYCLE  0x04
#define CMD_SYSSUB_NEWREGNR   0x09
#define CMD_SYSSUB_OVERLOAD   0x0A
#define CMD_SYSSUB_STATUS     0x0B


/* LOCOs */
#define CMD_LOCO_DISCOVERY    0x01
#define ID_LOCO_DISCOVERY     0x02

#define CMD_LOCO_BIND         0x02
#define ID_LOCO_BIND          0x04

#define CMD_LOCO_VERIFY       0x03
#define ID_LOCO_VERIFY        0x06

#define CMD_LOCO_VELOCITY     0x04
#define ID_LOCO_VELOCITY      0x08

#define CMD_LOCO_DIRECTION    0x05
#define ID_LOCO_DIRECTION     0x0A

#define CMD_LOCO_FUNCTION     0x06
#define ID_LOCO_FUNCTION      0x0C

#define CMD_LOCO_READ_CONFIG  0x07
#define ID_LOCO_READ_CONFIG   0x0E

#define CMD_LOCO_WRITE_CONFIG 0x08
#define ID_LOCO_WRITE_CONFIG  0x10


/* ACCESSORIES */
#define CMD_ACC_SWITCH        0x0B
#define ID_ACC_SWITCH         0x16


/* SOFTWARE */
#define CAN_CMD_PING         0x18
#define CAN_ID_PING          0x30



#endif /* MCS2CONST_H_ */
