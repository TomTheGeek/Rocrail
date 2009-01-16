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

#ifndef ROCNETPARSER_H_
#define ROCNETPARSER_H_

#include "rocdigs/impl/rocnet/rocnet-const.h"
#include "rocs/public/rocs.h"

Boolean rocnetIsThis( iOrocNet rocnet, byte* rn );
const char* rocnetGetProtocolStr(byte prot);

byte* rocnetParseMobile( iOrocNet rocnet, byte* rn );
byte* rocnetParseGeneral( iOrocNet rocnet, byte* rn );
byte* rocnetParseOutput( iOrocNet rocnet, byte* rn );




#endif /* ROCNETPARSER_H_ */
