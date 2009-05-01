/*
 Rocrail - Model Railroad Software

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


void eventEnter( iOLcDriver inst, const char* blockId, Boolean curBlockEvent, Boolean dstBlockEvent );
void eventPre2In( iOLcDriver inst, const char* blockId, Boolean curBlockEvent, Boolean dstBlockEvent );
void eventIn( iOLcDriver inst, const char* blockId, iIBlockBase block, Boolean curBlockEvent, Boolean dstBlockEvent, Boolean shortIn );
void eventExit( iOLcDriver inst, const char* blockId, Boolean curBlockEvent, Boolean dstBlockEvent );
