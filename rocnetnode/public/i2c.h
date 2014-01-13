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

#ifndef ROCNETNODE_I2C_H_
#define ROCNETNODE_I2C_H_

#define PWM_SUBADR1      0x02
#define PWM_SUBADR2      0x03
#define PWM_SUBADR3      0x04
#define PWM_MODE1        0x00
#define PWM_PRESCALE     0xFE
#define PWM_LED0_ON_L    0x06
#define PWM_LED0_ON_H    0x07
#define PWM_LED0_OFF_L   0x08
#define PWM_LED0_OFF_H   0x09
#define PWM_ALLLED_ON_L  0xFA
#define PWM_ALLLED_ON_H  0xFB
#define PWM_ALLLED_OFF_L 0xFC
#define PWM_ALLLED_OFF_H 0xFD


int raspiWriteRegI2C( int descriptor, unsigned char dev_addr, unsigned char regaddr, unsigned char data );
int raspiReadRegI2C( int descriptor, unsigned char dev_addr, unsigned char regaddr, unsigned char* data );
int raspiWriteI2C( int descriptor, unsigned char dev_addr, unsigned char data );
int raspiReadI2C(int descriptor, unsigned char dev_addr, unsigned char* data);
int raspiOpenI2C( const char* devicename );
int raspiCloseI2C( int descriptor );

int pwmSetFreq(int descriptor, unsigned char dev_addr, int freq);
int pwmSetChannel(int descriptor, unsigned char dev_addr, int channel, int on, int off);
int pwmStop(int descriptor, unsigned char dev_addr);
int pwmStart(int descriptor, unsigned char dev_addr);

#endif

