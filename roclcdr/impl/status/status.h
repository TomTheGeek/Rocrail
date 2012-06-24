/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2012 Rob Versluis, Rocrail.net

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

void statusPause( iILcDriverInt inst, Boolean reverse );
void statusIdle( iILcDriverInt inst, Boolean reverse );
void statusFindDest( iILcDriverInt inst );
void statusInitDest( iILcDriverInt inst );
void statusCheckRoute( iILcDriverInt inst );
void statusPre2Go( iILcDriverInt inst );
void statusGo( iILcDriverInt inst );
void statusExit( iILcDriverInt inst );
void statusOut( iILcDriverInt inst );
void statusEnter( iILcDriverInt inst, Boolean re_enter );
void statusPre2In( iILcDriverInt inst );
void statusIn( iILcDriverInt inst );
void statusWait( iILcDriverInt inst, Boolean reverse );
void statusTimer( iILcDriverInt inst, Boolean reverse );
void statusWait4Event( iILcDriverInt inst );

