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
#include "rocdigs/impl/raspi_impl.h"
#include "rocdigs/impl/raspi/io.h"

#include "rocs/public/trace.h"
#include "rocs/public/str.h"
#include "rocs/public/thread.h"
#include "rocs/public/system.h"

#include "rocrail/wrapper/public/DigInt.h"

// http://elinux.org/RPi_Low-level_peripherals#GPIO_Code_examples

#ifdef __arm__
// Access from ARM Running Linux

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

// I/O access
volatile unsigned *gpio;


// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0


//
// Set up a memory regions to access GPIO
//
int raspiSetupIO(obj inst)
{
  iORasPiData data = Data(inst);
  int g,rep;

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setup I/O [%s]", wDigInt.getiid( data->ini ) );

   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "can't open /dev/mem" );
      return -1;
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); //No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "mmap error %d", (int)gpio_map );//errno also set!
      return -1;
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;

   // Set GPIO pins 7-11 to output
   for (g=7; g<=11; g++)
   {
     INP_GPIO(g); // must use INP_GPIO before we can use OUT_GPIO
     OUT_GPIO(g);
   }

   for (rep=0; rep<10; rep++)
   {
      for (g=7; g<=11; g++)
      {
        GPIO_SET = 1<<g;
        sleep(1);
      }
      for (g=7; g<=11; g++)
      {
        GPIO_CLR = 1<<g;
        sleep(1);
      }
   }
   return 0;
} // setup_io

#else

int raspiSetupIO(obj inst) {
  iORasPiData data = Data(inst);
  // Dummy
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "setup I/O [%s]", wDigInt.getiid( data->ini ) );
  return 0;
}

#endif

