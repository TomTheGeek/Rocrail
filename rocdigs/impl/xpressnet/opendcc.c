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

#include "rocdigs/impl/xpressnet_impl.h"
#include "rocdigs/impl/xpressnet/opendcc.h"
#include "rocdigs/impl/xpressnet/li101.h"

#include "rocrail/wrapper/public/Clock.h"

#include <time.h>


Boolean opendccConnect(obj xpressnet) {
  return li101Connect(xpressnet);
}

void opendccDisConnect(obj xpressnet) {
  li101DisConnect(xpressnet);
}

Boolean opendccAvail(obj xpressnet) {
  return li101Avail(xpressnet);
}

void opendccInit(obj xpressnet) {
  li101Init(xpressnet);
}
int opendccRead(obj xpressnet, byte* buffer) {
  return li101Read(xpressnet, buffer);
}
Boolean opendccWrite(obj xpressnet, byte* buffer, int* rspexpected) {
  return li101Write(xpressnet, buffer, rspexpected);
}


void opendccTranslate( obj xpressnet, void* node ) {
  iOXpressNetData data = Data(xpressnet);

  /* Clock command. */
  if( StrOp.equals( NodeOp.getName( node ), wClock.name() ) ) {
    /*
    from Slave to Command Station / and from Command Station to Slave:

    0x00 0x01 TCODE0 {TCODE1 TCODE2 TCODE3} Timecode transfer, accelerated layout time.
    A TCODE consists of one byte, coded binary as CCDDDDDD, where CC denotes the type of
    code and DDDDDDD the corresponding data.
    TCODE Content
    CC=00 DDDDDD = mmmmmm, this denotes the minute, range 0..59.
    CC=10 DDDDDD = 0HHHHHH, this denotes the hour, range 0..23
    CC=01 DDDDDD = 000WWW, this denotes the day of week,
    0=Monday, 1=Tuesday, 2=Wednesday, 3=Thursday, 4=Friday, 5=Saturday, 6=Sunday.
    CC=11 DDDDDD = 00FFFFF, this denotes the acceleration factor, range 0..31;
    an acceleration factor of 0 means clock is stopped, a factor of 1 means clock is running
    real time, a factor of 2 means clock is running twice as fast a real time.
    This message is issued as broadcast once every (layout-) minute. The command is not repeated.

    When no Parameters are given, it is a query and the answer will be sent only to the requesting slave.
     */
    if( data->fastclock && StrOp.equals( wClock.set, wClock.getcmd( node ) ) ) {
      long l_time = wClock.gettime(node);
      struct tm* lTime = localtime( &l_time );

      int mins    = lTime->tm_min;
      int hours   = lTime->tm_hour;
      int wday    = lTime->tm_wday;
      int divider = wClock.getdivider(node);
      byte* outa  = NULL;

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "set clock to %02d:%02d divider=%d", hours, mins, divider );

      outa = allocMem(32);
      outa[0] = 0x05;
      outa[1] = 0xF1;
      outa[2] = 0x00 + mins;
      outa[3] = 0x80 + hours;
      outa[4] = 0x40 + wday;
      outa[5] = 0xC0 + divider;
      ThreadOp.post( data->transactor, (obj)outa );
    }
  }
}
