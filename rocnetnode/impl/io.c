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
#include "rocs/public/thread.h"
#include "rocs/public/system.h"

#include "rocnetnode/public/io.h"

#ifdef __arm__
/* Access from ARM Running Linux */

#define BCM2708_PERI_BASE        0x20000000
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int  mem_fd;
void *gpio_map;

/* I/O access */
volatile unsigned *gpio = NULL;


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

/* Set up a memory regions to access GPIO */
int raspiSetupIO(int mask)
{
  int g, rep, port, i;

  TraceOp.trc( "raspi", TRCLEVEL_INFO, __LINE__, 9999, "setup RasPi I/O 0x%08X", mask );

   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      TraceOp.trc( "raspi", TRCLEVEL_EXCEPTION, __LINE__, 9999, "can't open /dev/mem" );
      return -1;
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,
      BLOCK_SIZE,
      PROT_READ|PROT_WRITE,
      MAP_SHARED,
      mem_fd,
      GPIO_BASE
   );

   close(mem_fd); /* No need to keep mem_fd open after mmap */

   if (gpio_map == MAP_FAILED) {
      TraceOp.trc( "raspi", TRCLEVEL_EXCEPTION, __LINE__, 9999, "mmap error %d", (int)gpio_map );
      return -1;
   }

   /* Always use volatile pointer! */
   gpio = (volatile unsigned *)gpio_map;


   if( mask != -1) {
     for( i = 0; i < 32; i++ ) {
       /* Always use INP_GPIO(x) before using OUT_GPIO(x) */
       INP_GPIO(i);
       if( mask & (1 << i) ) {
         /* input */
       }
       else {
         /* output */
         OUT_GPIO(i);
       }
     }
   }

   return 0;
}


void raspiConfigPort(int port, int type) {
  INP_GPIO(port);
  if( type == 0 )
    OUT_GPIO(port);
}

int raspiRead(int port) {
  return GPIO_READ(port);
}

void raspiWrite(int port, int val) {
  if(val)
    GPIO_SET = 1 << port;
  else
    GPIO_CLR = 1 << port;
}


int raspiDummy(void) {
  return 0;
}

#else

int raspiDummy(void) {
  return 1;
}

void raspiGPIOAlt(int g, int alt) {
}

int raspiSetupIO(int mask) {
  TraceOp.trc( "raspi", TRCLEVEL_INFO, __LINE__, 9999, "dummy raspiSetupIO(0x%04X)", mask );
  return 0;
}

void raspiConfigPort(int port, int type) {
  TraceOp.trc( "raspi", TRCLEVEL_INFO, __LINE__, 9999, "dummy raspiConfigPort(%d, %d)", port, type );
}

int raspiRead(int port) {
  TraceOp.trc( "raspi", TRCLEVEL_DEBUG, __LINE__, 9999, "dummy raspiRead(%d)", port );
  return rand() & 0x01;
}
void raspiWrite(int port, int val) {
  TraceOp.trc( "raspi", TRCLEVEL_INFO, __LINE__, 9999, "dummy raspiWrite(%d, %d)", port, val );
}


#endif
