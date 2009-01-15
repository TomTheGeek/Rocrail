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

#ifndef FADA_H_
#define FADA_H_

/*
 FADA = (addr-1) * 8 + (port-1) * 2 + gate

 Calculating back from FADA to module addressing:
   addr = (FADA / 8) + 1
   port = (FADA % 8) / 2 + 1
   gate = (FADA % 8) % 2
*/

int toFADA( int module, int port, int gate );
int toPADA( int module, int port );
void fromFADA( int fada, int* module, int* port, int* gate );
void fromPADA( int pada, int* module, int* port );

#endif /*FADA_H_*/
