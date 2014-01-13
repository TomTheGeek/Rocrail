/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2014 Rob Versluis, Rocrail.net

 


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
#include "rocs/public/thread.h"

#include "rocnetnode/public/i2c.h"

#ifdef __arm__
#include <stdio.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

int raspiWriteRegI2C(int descriptor, unsigned char dev_addr, unsigned char reg_addr, unsigned char data) {
  unsigned char buff[2];
  struct i2c_rdwr_ioctl_data packets;
  struct i2c_msg messages[1];
  buff[0] = reg_addr;
  buff[1] = data;
  messages[0].addr = dev_addr;
  messages[0].flags = 0;
  messages[0].len = sizeof(buff);
  messages[0].buf = buff;
  packets.msgs = messages;
  packets.nmsgs = 1;
  TraceOp.trc( "raspi-i2c", TRCLEVEL_BYTE, __LINE__, 9999, "write I2C register: addr=0x%02X reg=0x%02X msg=0x%02X", dev_addr, reg_addr, data );
  return ioctl(descriptor, I2C_RDWR, &packets);
}

int raspiWriteI2C(int descriptor, unsigned char dev_addr, unsigned char data) {
  unsigned char buff[1];
  struct i2c_rdwr_ioctl_data packets;
  struct i2c_msg messages[1];
  buff[0] = data;
  messages[0].addr = dev_addr;
  messages[0].flags = 0;
  messages[0].len = sizeof(buff);
  messages[0].buf = buff;
  packets.msgs = messages;
  packets.nmsgs = 1;
  TraceOp.trc( "raspi-i2c", TRCLEVEL_BYTE, __LINE__, 9999, "write I2C: addr=0x%02X msg=0x%02X", dev_addr, data );
  return ioctl(descriptor, I2C_RDWR, &packets);
}

int raspiReadRegI2C(int descriptor, unsigned char dev_addr, unsigned char reg_addr, unsigned char* data) {
  unsigned char *inbuff, outbuff;
  struct i2c_rdwr_ioctl_data packets;
  struct i2c_msg messages[2];
  outbuff = reg_addr;
  messages[0].addr = dev_addr;
  messages[0].flags= 0;
  messages[0].len = sizeof(outbuff);
  messages[0].buf = &outbuff;
  inbuff = data;
  messages[1].addr = dev_addr;
  messages[1].flags = I2C_M_RD;
  messages[1].len = sizeof(*inbuff);
  messages[1].buf = inbuff;
  packets.msgs = messages;
  packets.nmsgs = 2;
  TraceOp.trc( "raspi-i2c", TRCLEVEL_BYTE, __LINE__, 9999, "read I2C register: addr=0x%02X reg=0x%02X", dev_addr, reg_addr );
  return ioctl(descriptor, I2C_RDWR, &packets);
}

int raspiReadI2C(int descriptor, unsigned char dev_addr, unsigned char* data) {
  unsigned char *inbuff;
  struct i2c_rdwr_ioctl_data packets;
  struct i2c_msg messages[1];
  inbuff = data;
  messages[0].addr = dev_addr;
  messages[0].flags = I2C_M_RD;
  messages[0].len = sizeof(*inbuff);
  messages[0].buf = inbuff;
  packets.msgs = messages;
  packets.nmsgs = 1;
  TraceOp.trc( "raspi-i2c", TRCLEVEL_BYTE, __LINE__, 9999, "read I2C: addr=0x%02X", dev_addr );
  return ioctl(descriptor, I2C_RDWR, &packets);
}

int raspiOpenI2C( const char* i2cdevice ) {
  return open(i2cdevice, O_RDWR);
}

int raspiCloseI2C( int i2cdescriptor ) {
  return close(i2cdescriptor);
}

#else

int raspiWriteRegI2C(int descriptor, unsigned char dev_addr, unsigned char reg_addr, unsigned char data) {
  return -1;
}

int raspiWriteI2C(int descriptor, unsigned char dev_addr, unsigned char data) {
  return -1;
}

int raspiReadRegI2C(int descriptor, unsigned char dev_addr, unsigned char reg_addr, unsigned char* data) {
  return -1;
}

int raspiReadI2C(int descriptor, unsigned char dev_addr, unsigned char* data) {
  return -1;
}

int raspiOpenI2C( const char* devicename ) {
  return -1;
}

int raspiCloseI2C( int descriptor ) {
  return -1;
}

#endif


int pwmStop(int descriptor, unsigned char dev_addr) {
  unsigned char curmode = 0;
  int rc = raspiReadRegI2C(descriptor, dev_addr, PWM_MODE1, &curmode);
  if( rc < 0 ) return rc;
  TraceOp.trc( "raspi-i2c", TRCLEVEL_INFO, __LINE__, 9999, "PWM stop mode1=0x%02X", curmode & 0xFF );
  return raspiWriteRegI2C( descriptor, dev_addr, PWM_MODE1, curmode | 0x10);
}


int pwmStart(int descriptor, unsigned char dev_addr) {
  unsigned char curmode = 0;
  int rc = raspiReadRegI2C(descriptor, dev_addr, PWM_MODE1, &curmode);
  if( rc < 0 ) return rc;
  TraceOp.trc( "raspi-i2c", TRCLEVEL_INFO, __LINE__, 9999, "PWM start mode1=0x%02X", curmode & 0xFF );
  return raspiWriteRegI2C( descriptor, dev_addr, PWM_MODE1, curmode & 0xEF);
}


int pwmSetFreq(int descriptor, unsigned char dev_addr, int freq) {
  unsigned char curmode = 0;
  int rc = 0;

  /* Sets the PWM frequency */
  float prescaleval = 25000000.0;    /* 25MHz */
  float prescale = 0.0;
  prescaleval /= 4096.0;       /* 12-bit */
  prescaleval /= (float)freq;
  prescaleval -= 1.0;
  prescale = (int)(prescaleval + 0.5);

  TraceOp.trc( "raspi-i2c", TRCLEVEL_INFO, __LINE__, 9999, "set PWM freq. to %d on device 0x%02X prescale=%f", freq, dev_addr, prescale );

  rc = raspiReadRegI2C(descriptor, dev_addr, PWM_MODE1, &curmode);
  if( rc < 0 ) return rc;
  TraceOp.trc( "raspi-i2c", TRCLEVEL_INFO, __LINE__, 9999, "PWM mode1=0x%02X", curmode & 0xFF );
  rc = raspiWriteRegI2C( descriptor, dev_addr, PWM_MODE1, 0x11);
  if( rc < 0 ) return rc;
  rc = raspiWriteRegI2C( descriptor, dev_addr, PWM_PRESCALE, (int)prescale );
  if( rc < 0 ) return rc;
  rc = raspiWriteRegI2C( descriptor, dev_addr, PWM_MODE1, 0x01);
  if( rc < 0 ) return rc;
  ThreadOp.sleep(10);
  return raspiWriteRegI2C( descriptor, dev_addr, PWM_MODE1, 0x81);
}

int pwmSetChannel(int descriptor, unsigned char dev_addr, int channel, int on, int off) {
  int rc = 0;
  TraceOp.trc( "raspi-i2c", TRCLEVEL_BYTE, __LINE__, 9999, "set PWM channel %d on device 0x%02X to %d,%d", channel, dev_addr, on, off );
  if( on != -1 ) {
    rc = raspiWriteRegI2C( descriptor, dev_addr, PWM_LED0_ON_L+4*channel, on & 0xFF);
    if( rc < 0 ) return rc;
    rc = raspiWriteRegI2C( descriptor, dev_addr, PWM_LED0_ON_H+4*channel, on >> 8);
    if( rc < 0 ) return rc;
  }
  rc = raspiWriteRegI2C( descriptor, dev_addr, PWM_LED0_OFF_L+4*channel, off & 0xFF);
  if( rc < 0 ) return rc;
  rc = raspiWriteRegI2C( descriptor, dev_addr, PWM_LED0_OFF_H+4*channel, off >> 8);
  return rc;
}


