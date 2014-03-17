/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net




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

#ifndef ZIMOCAN_CONST_H_
#define ZIMOCAN_CONST_H_

#define HEADER 0x5A32
#define TAIL   0x325A

#define MODE_REQ 0x00
#define MODE_CMD 0x01
#define MODE_EVT 0x03

#define SYSTEM_CONTROL_GROUP 0X00
#define SYSTEM_POWER 0X00
#define SYSTEM_POWER_ON 0X00
#define SYSTEM_POWER_OFF 0X20
#define SYSTEM_POWER_TRACK1 0X00
#define SYSTEM_POWER_TRACK2 0X01

#define ACCESSORY_COMMAND_GROUP 0X01
#define ACCESSORY_PORT 0X04

#define MOBILE_CONTROL_GROUP 0X02
#define MOBILE_STATE 0X00
#define MOBILE_MODE 0X01
#define MOBILE_SPEED 0X02
#define MOBILE_FUNCTION 0X04

#define NETWORK_GROUP 0X0A
#define NETWORK_PING  0X00
#define NETWORK_MODULINFO  0X08

#endif
