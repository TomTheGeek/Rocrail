/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2011 - Rob Versluis <r.j.versluis@rocrail.net>

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


#ifndef CBUS_UTILS_H_
#define CBUS_UTILS_H_
/*
 * CBUS Priorities
 */
#define PRIORITY_HIGH 0
#define PRIORITY_ABOVE 1
#define PRIORITY_NORMAL 2
#define PRIORITY_LOW 3

#define OFFSET_TYPE 6
#define OFFSET_OPC 7
#define OFFSET_D1 9
#define OFFSET_D2 11
#define OFFSET_D3 13
#define OFFSET_D4 15
#define OFFSET_D5 17
#define OFFSET_D6 19
#define OFFSET_D7 21

int makeFrame(obj inst, byte* frame, int prio, byte* cmd, int datalen );

#endif /* CBUS_UTILS_H_ */
