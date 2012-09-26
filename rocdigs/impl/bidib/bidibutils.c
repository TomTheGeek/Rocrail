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

#include "rocs/public/str.h"
#include "rocrail/wrapper/public/BiDiBnode.h"

char* bidibGetClassName(int classid ) {
  char* classname = NULL;
  if( classid & 0x80 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_bridge);
  }
  if( classid & 0x40 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_sensor);
  }
  if( classid & 0x20 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_ui);
  }
  if( classid & 0x10 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_dcc_loco);
  }
  if( classid & 0x08 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_dcc_acc);
  }
  if( classid & 0x04 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_prog);
  }
  if( classid & 0x02 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_booster);
  }
  if( classid & 0x01 ) {
    if( classname != NULL ) classname = StrOp.cat( classname, ",");
    classname = StrOp.cat( classname, wBiDiBnode.class_switch);
  }

  if( classname == NULL )
    classname = StrOp.dup("");
  return classname;
}

