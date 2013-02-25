/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

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

#ifndef BIDIBUDP_H_
#define BIDIBUDP_H_

Boolean udpInit( obj inst );
Boolean udpConnect( obj inst );
void  udpDisconnect( obj inst );

int udpRead ( obj inst, unsigned char *msg );
Boolean udpWrite( obj inst, unsigned char *path, unsigned char code, unsigned char* data, int datalen, int seq );
Boolean udpAvailable( obj inst );

#endif /* BIDIBUDP_H_ */
