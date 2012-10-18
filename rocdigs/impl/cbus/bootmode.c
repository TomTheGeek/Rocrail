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

#include "rocdigs/impl/cbus_impl.h"

#include "rocdigs/impl/cbus/utils.h"

#include "rocs/public/trace.h"
#include "rocs/public/mem.h"
#include "rocs/public/str.h"
#include "rocs/public/system.h"
#include "rocs/public/thread.h"

#include "rocrail/wrapper/public/DigInt.h"
#include "rocrail/wrapper/public/CBus.h"

/*
|start of line
||byte count(2)
|| |address(4)
|| |   |type(2)
|| |   | |data(cnt*2)
|| |   | |   |checksum(2)
:020000040000FA
:040000001FD00ED02F
:0400080004EF04F00D
:040018000CEF04F0F5
:10002000A66E05A01200550EA76EAA0EA76EA68298
:100030000000A6B2FED712000A26D8B00B2A120082
...
:10156000E12AFAD76051E71403E06037142AFAD76A
:10157000630E1460C1EC0AF061C0E1FF62C0E2FFDB
:0E1580001200070E58EC0AF01468149E1200B8
:020000040030CA
:0300010006061ED2
:020005008081F8
:060008000FC00FE00F40E5
:0200000400F00A
:060000007F00000000007B
:0A0086000000000000000000000070
:100090000000000000000000000000000000000060
:1000A0000000000000000000000000000000000050
:1000B0000000000000000000000000000000000040
:1000C0000000000000000000000000000000000030
:1000D0000000000000000000000000000000000020
:1000E0000000000000000000000000000000000010
:1000F0000000000000000000000000000000000000
:00000001FF
*/

/* block types */
#define NO_BLOCK -1
#define PROGRAM_BLOCK 0
#define CONFIG_BLOCK 1
#define EEPROM_BLOCK 2

#define PROGRAM_SIZE 0xC000
#define CONFIG_SIZE 0x20
#define EEPROM_SIZE 0x1000

struct BootData {
  int block;
  int offset[3];
  char* data[3];
  int count[3];
  int checksum;
};

static Boolean evaluateLine(const char* hexline, struct BootData* bootData) {
  char s[5] = {0,0,0,0,0};
  if( hexline[8] == '4' ) {
    /* extended linear address record */
    MemOp.copy( s, hexline+9, 4);
    int offset = (int)strtol( s, NULL, 16);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "extended linear address record: %d(0x%08X)", offset, offset );

    switch( bootData->block ) {
    case NO_BLOCK:
      if( offset == 0x00 ) {
        bootData->block = PROGRAM_BLOCK;
        bootData->offset[PROGRAM_BLOCK] = offset;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----- switching to program block" );
      }
      else {
        TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "not a bootable file!" );
        return False;
      }
      break;
    case PROGRAM_BLOCK:
      if( offset == 0x30 ) {
        bootData->block = CONFIG_BLOCK;
        bootData->offset[CONFIG_BLOCK] = offset;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----- switching to config block" );
      }
      else if( offset == 0xF0 ) {
        bootData->block = EEPROM_BLOCK;
        bootData->offset[EEPROM_BLOCK] = offset;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----- switching to eeprom block" );
      }
      break;
    case CONFIG_BLOCK:
      if( offset == 0xF0 ) {
        bootData->block = EEPROM_BLOCK;
        bootData->offset[EEPROM_BLOCK] = offset;
        TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----- switching to eeprom block" );
      }
      break;
    }
  }
  else if( hexline[8] == '0' && bootData->block != NO_BLOCK ) {
    /* data record */
    MemOp.copy( s, hexline+1, 2);
    int cnt = (int)strtol( s, NULL, 16);
    MemOp.copy( s, hexline+3, 4);
    int addr = (int)strtol( s, NULL, 16);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999,
        "data record with start address %d(0x%08X) with %d bytes for block %d", addr, addr, cnt, bootData->block );
    int i = 0;
    for( i = addr; i < (addr+cnt); i++ ) {
      bootData->data[bootData->block][i*2] = hexline[9+(i-addr)*2];
      bootData->data[bootData->block][i*2+1] = hexline[9+(i-addr)*2+1];
    }
    bootData->count[bootData->block] = addr + cnt;
  }
  else if( hexline[8] == '1' ) {
    /* end of file record */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "end of file" );
  }

  return True;
}


static void sendData(obj inst, struct BootData* bootData, int nodenr, Boolean eeprom) {
  iOCBUSData data = Data(inst);
  Boolean eewrite = eeprom;

  data->bootmode = True;

  /* Program block */

  byte* frame = allocMem(32);

  // offset 0x200 for pic program
  StrOp.copy( frame+1, ":X00080004N000200000D020000;" );

  frame[0] = StrOp.len(frame+1);
  ThreadOp.post(data->writer, (obj)frame);

  int i = 0;
  int nrlines = bootData->count[PROGRAM_BLOCK] / 8;
  if(bootData->count[PROGRAM_BLOCK] % 8 > 0 ) {
    nrlines++;
  }

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sending %d PROGRAM blocks...", nrlines );

  // skip first 0x200 bytes, bootloader code is there
  for( i = 64; i < nrlines; i++ ) {
    int j;

    if( i % 16 == 0 )
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sending PROGRAM block[%d of %d] (%d)", i+1, nrlines, i*8*2 );
    frame = allocMem(32);
    StrOp.copy( frame+1, ":X00080005N");
    MemOp.copy( frame+1+11, &bootData->data[PROGRAM_BLOCK][i*8*2], 16 );
    StrOp.copy( frame+1+11+16, ";");
    frame[0] = StrOp.len(frame+1);
    ThreadOp.sleep(wCBus.getloadertime(data->cbusini));
    ThreadOp.post(data->writer, (obj)frame);
    // compute checksum
    for ( j = 0; j < 8; j++ ) {
      char s[3] = {0,0,0};
      MemOp.copy( s, &bootData->data[PROGRAM_BLOCK][(i*8*2)+(j*2)], 2);
      int val = (int)strtol( s, NULL, 16);
      bootData->checksum += val;
    }
  }

  /* Config block */
  if( bootData->count[CONFIG_BLOCK] > 0 ) {
    byte* frame = allocMem(32);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sending CONFIG block" );
    StrOp.copy( frame+1, ":X00080004N000030000D000000;" );
    frame[0] = StrOp.len(frame+1);
    ThreadOp.sleep(wCBus.getloadertime(data->cbusini) + 50);
    ThreadOp.post(data->writer, (obj)frame);

    nrlines = 32 / 8;

    for( i = 0; i < nrlines; i++ ) {
      int j;

      TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "sending CONFIG block[%d of %d]", i+1, nrlines );
      frame = allocMem(32);
      StrOp.copy( frame+1, ":X00080005N");
      MemOp.copy( frame+1+11, &bootData->data[CONFIG_BLOCK][i*8*2], 16 );
      StrOp.copy( frame+1+11+16, ";");
      frame[0] = StrOp.len(frame+1);
      ThreadOp.sleep(wCBus.getloadertime(data->cbusini) + 50);
      ThreadOp.post(data->writer, (obj)frame);
      for ( j = 0; j < 8; j++ ) {
        char s[3] = {0,0,0};
        MemOp.copy( s, &bootData->data[CONFIG_BLOCK][(i*8*2)+(j*2)], 2);
        int val = (int)strtol( s, NULL, 16);
        bootData->checksum += val;
      }
    }
  }

  /* EEProm block */
  if( eewrite && bootData->count[EEPROM_BLOCK] > 0 ) {
    byte* frame = allocMem(32);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sending EEPROM block" );
    StrOp.copy( frame+1, ":X00080004N0000F0000D000000;" );
    frame[0] = StrOp.len(frame+1);
    ThreadOp.sleep(wCBus.getloadertime(data->cbusini) + 50);
    ThreadOp.post(data->writer, (obj)frame);

    nrlines = 512 / 8;

    for( i = 0; i < nrlines; i++ ) {
      int j;

      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "sending EEPROM block[%d of %d]", i+1, nrlines );
      frame = allocMem(32);
      StrOp.copy( frame+1, ":X00080005N");
      MemOp.copy( frame+1+11, &bootData->data[EEPROM_BLOCK][i*8*2], 16 );
      StrOp.copy( frame+1+11+16, ";");
      frame[0] = StrOp.len(frame+1);
      ThreadOp.sleep(wCBus.getloadertime(data->cbusini) + 20);
      ThreadOp.post(data->writer, (obj)frame);
      for ( j = 0; j < 8; j++ ) {
        char s[3] = {0,0,0};
        MemOp.copy( s, &bootData->data[EEPROM_BLOCK][(i*8*2)+(j*2)], 2);
        int val = (int)strtol( s, NULL, 16);
        bootData->checksum += val;
      }
    }
  }

  /* Checksum. */
  int checksum = 65536 - (bootData->checksum & 0xFFFF);
  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "checksum data bytes: 0x%04X", checksum );
  frame = allocMem(32);
  StrOp.fmtb( frame+1, ":X00080004N000000000D03%02X%02X;", checksum & 0xFF, (checksum >> 8) & 0xFF );
  frame[0] = StrOp.len(frame+1);
  ThreadOp.sleep(wCBus.getloadertime(data->cbusini) + 50);
  ThreadOp.post(data->writer, (obj)frame);

  TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "reset" );
  frame = allocMem(32);
  StrOp.copy( frame+1, ":X00080004N000000000D010000;" );
  frame[0] = StrOp.len(frame+1);
  ThreadOp.sleep(wCBus.getloadertime(data->cbusini) + 50);
  ThreadOp.post(data->writer, (obj)frame);

  data->bootmode = False;
}



void loadHEXFile(obj inst, const char* filename, int nodenr, Boolean eeprom ) {
  iOCBUSData data = Data(inst);

  iOFile f = FileOp.inst( filename, OPEN_READONLY );

  if( f == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "can't open [%s] for loading", filename );
    return;
  }

  struct BootData* bootData = allocMem(sizeof(struct BootData));

  bootData->block = NO_BLOCK;
  bootData->checksum = 0;

  bootData->offset[PROGRAM_BLOCK] = 0;
  bootData->offset[CONFIG_BLOCK ] = 0;
  bootData->offset[EEPROM_BLOCK ] = 0;

  bootData->count[PROGRAM_BLOCK] = 0;
  bootData->count[CONFIG_BLOCK ] = 0;
  bootData->count[EEPROM_BLOCK ] = 0;

  bootData->data[PROGRAM_BLOCK] = allocMem(PROGRAM_SIZE * 2);
  bootData->data[CONFIG_BLOCK ] = allocMem(CONFIG_SIZE  * 2);
  bootData->data[EEPROM_BLOCK ] = allocMem(EEPROM_SIZE  * 2);

  MemOp.set( bootData->data[PROGRAM_BLOCK], 'F', PROGRAM_SIZE * 2 );
  MemOp.set( bootData->data[CONFIG_BLOCK ], 'F', CONFIG_SIZE  * 2 );
  MemOp.set( bootData->data[EEPROM_BLOCK ], 'F', EEPROM_SIZE  * 2 );

  char hexline[128];
  int nr = 0;
  Boolean ok = True;
  while( ok && FileOp.readStr( f, hexline ) ) {
    nr++;
    TraceOp.trc( name, TRCLEVEL_DEBUG, __LINE__, 9999, "parsing line %d: %s ", nr, hexline );
    if( hexline[0] == ':' ) {
      ok = evaluateLine(hexline, bootData);
    }
  }

  if(ok) {
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "nr data bytes in program block: %d", bootData->count[PROGRAM_BLOCK] );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "nr data bytes in config block : %d", bootData->count[CONFIG_BLOCK] );
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "nr data bytes in eeprom block : %d", bootData->count[EEPROM_BLOCK] );

    int checksum = 65536 - (bootData->checksum & 0xFFFF);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "checksum data bytes: 0x%04X", checksum );

    TraceOp.setDumpsize(NULL, PROGRAM_SIZE * 2);
    TraceOp.dump( name, TRCLEVEL_BYTE, bootData->data[PROGRAM_BLOCK], PROGRAM_SIZE * 2 );
    TraceOp.setDumpsize(NULL, CONFIG_SIZE * 2);
    TraceOp.dump( name, TRCLEVEL_BYTE, bootData->data[CONFIG_BLOCK ], CONFIG_SIZE  * 2 );
    TraceOp.setDumpsize(NULL, EEPROM_SIZE * 2);
    TraceOp.dump( name, TRCLEVEL_BYTE, bootData->data[EEPROM_BLOCK ], EEPROM_SIZE  * 2 );

    sendData(inst, bootData, nodenr, eeprom);
  }


  FileOp.base.del(f);

  freeMem(bootData->data[PROGRAM_BLOCK]);
  freeMem(bootData->data[CONFIG_BLOCK ]);
  freeMem(bootData->data[EEPROM_BLOCK ]);

  freeMem(bootData);

}

