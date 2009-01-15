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
#ifndef __IMPL_DDX_INIT_H__
#define __IMPL_DDX_INIT_H__ 

#include "rocrail/wrapper/public/DDX.h"
#include "rocs/public/node.h"
#include "rocs/public/serial.h"

//void rocrail_ddxFbListener( obj inst, int addr, int state );
//void rocrail_ddxStateChanged( obj inst);

int ddx_entry(obj inst, iONode ddx_ini);
Boolean init_serialport(obj inst); 
void close_comport(obj inst);
void start_voltage(obj inst);
void stop_voltage(obj inst);
void quit(obj inst);

#endif
