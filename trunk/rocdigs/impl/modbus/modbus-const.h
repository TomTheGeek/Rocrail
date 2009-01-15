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
 * modbus-const.h
 * Specs: http://www.modbus.org/specs.php
 */

#ifndef MODBUSCONST_H_
#define MODBUSCONST_H_

/*
Modbus Protocol Implementation of Omni Proprietary
Function Code 67: Transmit Unsolicited Custom Data Packet.
A typical unsolicited transmission Modbus protocol message format using
Function Code 67 is as follows:

Modbus Protocol Message Format using Function Code 67
+-------+----------+--------+--------+------------------+------------+
| DEVICE|FUNCTION  | BYTE   | CUSTOM |                  |CRC ERROR   |
|       |          |        | PACKET |      DATA        |            |
|ADDRESS|CODE 67   |COUNT   | ADDRESS|                  |CHECK BYTES |
+-------+----------+--------+--------+------------------+------------+
|  XX   | 43HEX    | XX     |XXXXHEX |DD DD ... DD DD   |CRC CRC     |
+-------+----------+--------+--------+------------------+------------+

*/

/*-macros-------------------------------------------------------------------*/
#define SWAPSHORT(s) ((s >> 8)+((s << 8) & 0xFF00))
#define SWAPLONG(l) (((l & 0xFF000000) >> 24)+((l & 0x00FF0000) >>  8)+((l & 0x0000FF00) <<  8)+((l & 0x000000FF) << 24))

/*-modbus-functions---------------------------------------------------------*/
#define READ_COIL_STATUS           1
#define READ_INPUT_STATUS          2
#define READ_OUTPUT_REGISTERS      3
#define READ_INPUT_REGISTERS       4
#define WRITE_SINGLE_COIL          5
#define WRITE_SINGLE_REGISTER      6
#define READ_EXEPTION_STATUS       7
#define EXECUTE_DIAGNOSTICS        8
#define GET_COMMS_EVENT_COUNTER   11
#define GET_COMMS_EVENT_LOG       12
#define WRITE_MULTIPLE_COILS      15
#define WRITE_MULTIPLE_REGISTERS  16
#define REPORT_SLAVE_ID           17
#define READ_FILE_RECORD          20
#define WRITE_FILE_RECORD         21
#define MASK_WRITE_REGISTER       22
#define RW_MULTIPLE_REGISTERS     23
#define READ_FIFO_QUEUE           24

#define TRANSMIT_UNSOLICITED_DATA 67



#endif /* MODBUSCONST_H_ */
