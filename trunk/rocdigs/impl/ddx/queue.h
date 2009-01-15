/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>
 Some parts are copied from the DDL project of Torsten Vogt: http://www.vogt-it.com/OpenSource/DDL

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
#ifndef __IMPL_DDX_QUEUE_H__
#define __IMPL_DDX_QUEUE_H__


#define PKTSIZE     40

#define QEMPTY      -1
#define QNOVALIDPKT 0
#define QM1LOCOPKT  1
#define QM2LOCOPKT  2
#define QM2FXPKT    3
#define QM1FUNCPKT  4
#define QM1SOLEPKT  5 
#define QNBLOCOPKT  6
#define QNBACCPKT   7     

int  queue_init();
void queue_add(int addr, char *packet, int packet_type, int packet_size);
int  queue_get(int *addr, char *packet, int *packet_size); 

#endif
