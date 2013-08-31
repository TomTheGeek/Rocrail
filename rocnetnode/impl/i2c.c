/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2013 Rob Versluis, Rocrail.net

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

#include "rocs/public/trace.h"
#include "rocs/public/str.h"

#include "rocnetnode/public/i2c.h"

#ifdef __arm__
#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

int raspiWriteRegI2C(const char* devicename, int descriptor, unsigned char reg_addr, unsigned char data) {
  unsigned char buff[2];
  struct i2c_rdwr_ioctl_data packets;
  struct i2c_msg messages[1];
  buff[0] = reg_addr;
  buff[1] = data;
  messages[0].addr = devicename;
  messages[0].flags = 0;
  messages[0].len = sizeof(buff);
  messages[0].buf = buff;
  packets.msgs = messages;
  packets.nmsgs = 1;
  return ioctl(descriptor, I2C_RDWR, &packets);
}

int raspiReadRegI2C(const char* devicename, int descriptor, unsigned char reg_addr, unsigned char* data) {
  unsigned char *inbuff, outbuff;
  struct i2c_rdwr_ioctl_data packets;
  struct i2c_msg messages[2];
  outbuff = reg_addr;
  messages[0].addr = devicename;
  messages[0].flags= 0;
  messages[0].len = sizeof(outbuff);
  messages[0].buf = &outbuff;
  inbuff = data;
  messages[1].addr = devicename;
  messages[1].flags = I2C_M_RD;
  messages[1].len = sizeof(*inbuff);
  messages[1].buf = inbuff;
  packets.msgs = messages;
  packets.nmsgs = 2;
  return ioctl(descriptor, I2C_RDWR, &packets);
}

int raspiOpenI2C( const char* i2cdevice ) {
  return open(i2cdevice, O_RDWR);
}

int raspiCloseI2C( int i2cdescriptor ) {
  return close(i2cdescriptor);
}

#else

int raspiWriteRegI2C(const char* devicename, int descriptor, unsigned char reg_addr, unsigned char data) {
  return 0;
}

int raspiReadRegI2C(const char* devicename, int descriptor, unsigned char reg_addr, unsigned char* data) {
  return 0;
}

int raspiOpenI2C( const char* devicename ) {
  return 0;
}

int raspiCloseI2C( int descriptor ) {
  return 0;
}

#endif


