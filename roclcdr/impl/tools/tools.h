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

void unlockBlockGroup( iOLcDriver inst, iONode group);
void reserveSecondNextBlock( iOLcDriver inst, const char* gotoBlock, iIBlockBase fromBlock, iORoute fromRoute, iIBlockBase* toBlock, iORoute* toRoute );
void listBlocks(iOLcDriver inst);
void resetNext2( iOLcDriver inst, Boolean unLock );


Boolean initializeDestination( iOLcDriver inst, iIBlockBase block, iORoute street, iIBlockBase curBlock, Boolean dir );
Boolean initializeGroup( iOLcDriver inst, iIBlockBase block );
Boolean initializeSwap( iOLcDriver inst, iORoute route );
const char* getBlockV_hint( iILcDriverInt inst, iIBlockBase block, Boolean onexit, iORoute street );


void resetSignals(iOLcDriver inst );
Boolean setSignals(iOLcDriver inst, Boolean onEnter );

void checkRouteFunction( iILcDriverInt inst, iORoute route, iIBlockBase block );
void checkScheduleActions( iILcDriverInt inst, int state);
Boolean checkScheduleEntryActions( iILcDriverInt inst );

Boolean checkScheduleTime( iILcDriverInt inst, const char* scheduleID, int scheduleIdx );
