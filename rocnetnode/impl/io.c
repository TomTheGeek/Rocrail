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
#include "rocs/public/system.h"

#include "rocnetnode/public/io.h"

#if defined __arm__ &! defined __ARM_ARCH_7A__

/* Access from ARM Running Linux */

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define PWM_BASE                 (BCM2708_PERI_BASE + 0x20C000) /* PWM controller */
#define CLOCK_BASE               (BCM2708_PERI_BASE + 0x101000)

#define PWM_CTL  0
#define PWM_RNG1 4
#define PWM_DAT1 5

#define PWMCLK_CNTL 40
#define PWMCLK_DIV  41


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;

/* I/O access */
volatile unsigned *gpio = NULL;
volatile unsigned *pwm = NULL;
volatile unsigned *clk = NULL;


/* GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y) */
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  /* sets   bits which are 1 ignores bits which are 0 */
#define GPIO_CLR *(gpio+10) /* clears bits which are 1 ignores bits which are 0 */
#define GPIO_READ(g) *(gpio + 13) &= (1<<(g))

void raspiGPIOAlt(int g, int alt) {
  if( gpio != NULL ) {
    INP_GPIO(g);
    SET_GPIO_ALT(g,alt);
  }
}

/* map 4k register memory for direct access from user space and return a user space pointer to it */
volatile unsigned *mapRegisterMemory(int base)
{
  static int mem_fd = 0;
  char *mem, *map;

  /* open /dev/mem */
  if (!mem_fd) {
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      TraceOp.trc( "raspi", TRCLEVEL_EXCEPTION, __LINE__, 9999, "can't open /dev/mem" );
      return NULL;
    }
  }

  /* mmap register */

  /* Allocate MAP block */
  if ((mem = malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL) {
    return NULL;
  }

  /* Make sure pointer is on 4K boundary */
  if ((unsigned long)mem % PAGE_SIZE)
    mem += PAGE_SIZE - ((unsigned long)mem % PAGE_SIZE);

  /* Now map it */
  map = (char *)mmap(
    (void*)mem,
    BLOCK_SIZE,
    PROT_READ|PROT_WRITE,
    MAP_SHARED|MAP_FIXED,
    mem_fd,
    base
  );

  if ((long)map == -1) {
    TraceOp.terrno( "raspi", TRCLEVEL_EXCEPTION, __LINE__, 9999, errno, "mmap error" );
    return NULL;
  }

  /* Always use volatile pointer! */
  return (volatile unsigned *)map;
}

/* Set up a memory regions to access GPIO */
int raspiSetupIO(void)
{
  TraceOp.trc( "raspi", TRCLEVEL_INFO, __LINE__, 9999, "setup RasPi I/O" );

  /* Always use volatile pointer! */
   gpio = mapRegisterMemory(GPIO_BASE);
   pwm  = mapRegisterMemory(PWM_BASE);
   clk  = mapRegisterMemory(CLOCK_BASE);

   return 0;
}


void raspiConfigPort(int port, int type) {
  if( gpio == NULL )
    return;

  INP_GPIO(port);
  if( type == 0 )
    OUT_GPIO(port);
}

int raspiRead(int port) {
  if( gpio == NULL )
    return 0;
  return GPIO_READ(port);
}

void raspiWrite(int port, int val) {
  if( gpio == NULL )
    return;

  if(val)
    GPIO_SET = 1 << port;
  else
    GPIO_CLR = 1 << port;
}


int raspiDummy(void) {
  return 0;
}

void raspiSetPWM(int percent)
{
  int bitCount;
  unsigned int bits = 0;

  if( pwm == NULL )
    return;

  bitCount = (32 * percent) / 100;
  if (bitCount > 32) bitCount = 32;
  bits = 0;
  while (bitCount) {
    bits <<= 1;
    bits |= 1;
    bitCount--;
  }
  *(pwm + PWM_DAT1) = bits;
}

/* init hardware */
void raspiInitPWM(int maxpwm)
{
  if( gpio == NULL )
    return;

  /* set PWM alternate function for GPIO18 */
  SET_GPIO_ALT(18, 5);

  /* disable PWM */
  *(pwm + PWM_CTL) = 0;

  /* needs some time until the PWM module gets disabled, without the delay the PWM module crashs */
  usleep(100);

  /* stop clock and waiting for busy flag doesn't work, so kill clock */
  *(clk + PWMCLK_CNTL) = 0x5A000000 | (1 << 5);
  usleep(100);

  /* set frequency 19200000 / 20000 = 960 */
  int idiv = 28;
  *(clk + PWMCLK_DIV)  = 0x5A000000 | (idiv<<12);

  /* source=osc and enable clock */
  *(clk + PWMCLK_CNTL) = 0x5A000011;

  /* disable PWM */
  *(pwm + PWM_CTL) = 0;

  /* needs some time until the PWM module gets disabled, without the delay the PWM module crashs */
  usleep(100);

  *(pwm + PWM_RNG1) = 32 + ((100-maxpwm)*64)/100;
  usleep(100);

  /* 32 bits = 2 milliseconds, init with 1 millisecond */
  raspiSetPWM(0);

  /* start PWM1 in serializer mode */
  *(pwm + PWM_CTL) = 3;
  usleep(100);
}


#else

void raspiSetPWM(int percent) {

}

void raspiInitPWM(int maxPWM) {

}

int raspiDummy(void) {
  return 1;
}

void raspiGPIOAlt(int g, int alt) {
}

int raspiSetupIO(void) {
  TraceOp.trc( "raspi", TRCLEVEL_INFO, __LINE__, 9999, "dummy raspiSetupIO()" );
  return 0;
}

void raspiConfigPort(int port, int type) {
  TraceOp.trc( "raspi", TRCLEVEL_INFO, __LINE__, 9999, "dummy raspiConfigPort(%d, %d)", port, type );
}

int raspiRead(int port) {
  TraceOp.trc( "raspi", TRCLEVEL_DEBUG, __LINE__, 9999, "dummy raspiRead(%d)", port );
  return 1;
}
void raspiWrite(int port, int val) {
  TraceOp.trc( "raspi", TRCLEVEL_DEBUG, __LINE__, 9999, "dummy raspiWrite(%d, %d)", port, val );
}


#endif
