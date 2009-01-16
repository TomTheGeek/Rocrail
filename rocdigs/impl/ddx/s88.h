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
#ifndef __IMPL_DDX_S88_H__
#define __IMPL_DDX_S88_H__

/*maximal number of bytes read from one s88-bus*/
#define S88_MAXPORTSB 62 
/*maximal number of s88-busses*/
#define S88_MAXBUSSES 4
/*maximal number of ports*/
#define S88_MAXPORTS S88_MAXPORTSB*8*S88_MAXBUSSES

int  s88init(obj inst); /* test and open the S88PORT and initalizes the bus*/
void start_polling_s88(obj inst);
void setPT(obj inst, int state);
int  isPT(obj inst);

typedef struct _tDelayedAccCmd {
   char acc_p;
   int  acc_addr;
   int  acc_port;
   int  acc_gate;
   int  action;
   int  delay;
   int  busnr;
   int  devicegroup;
} tDelayedAccCmd;

#endif

