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

int toFADA( int module, int port, int gate ) {
  return (module-1) * 8 + (port-1) * 2 + gate;
}

int toPADA( int module, int port ) {
  return (module-1) * 4 + port;
}


void fromFADA( int fada, int* module, int* port, int* gate ) {
  int addr = fada;
  *module = addr / 8 + 1;
  *port = (addr % 8) / 2 + 1;
  *gate = (addr % 8) % 2;
}

void fromPADA( int pada, int* module, int* port ) {
  *module = (pada-1) / 4 + 1;
  *port   = (pada-1) % 4 + 1;
}

