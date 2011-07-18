/*
 Rocrail - Model Railroad Software

 Copyright (C) 2002-2011 - Rob Versluis <r.j.versluis@rocrail.net>

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
#define NO_BLOCK 0
#define PROGRAM_BLOCK 1
#define CONFIG_BLOCK 2
#define EEPROM_BLOCK 3

static char program[14336 * 2];
static char config [32 * 2];
static char eeprom [512 * 2];

void evaluateLine(const char* hexline, int* offset, int* block) {
  char s[5] = {0,0,0,0,0};
  if( hexline[8] == '4' ) {
    /* extended linear address record */
    MemOp.copy( s, hexline+9, 4);
    *offset = (int)strtol( s, NULL, 16);

    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "extended linear address record: %d", *offset );

    switch( *block ) {
    case NO_BLOCK:
      *block = PROGRAM_BLOCK;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----- switching to program block" );
      break;
    case PROGRAM_BLOCK:
      *block = CONFIG_BLOCK;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----- switching to config block" );
      break;
    case CONFIG_BLOCK:
      *block = EEPROM_BLOCK;
      TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "----- switching to eeprom block" );
      break;
    }
  }
  else if( hexline[8] == '0' ) {
    /* data record */
    MemOp.copy( s, hexline+3, 4);
    int addr = (int)strtol( s, NULL, 16);
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "data record with start address %d", addr );
  }
  else if( hexline[8] == '1' ) {
    /* end of file record */
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "end of file" );
  }
}


void loadHEXFile(obj inst, const char* filename, int nodenr ) {
  int block  = 0;
  int offset = 0;

  MemOp.set( program, 'F', 14336 * 2 );
  MemOp.set( config , 'F', 32 * 2 );
  MemOp.set( eeprom , 'F', 512 * 2 );

  iOFile f = FileOp.inst( filename, OPEN_READONLY );

  if( f == NULL ) {
    TraceOp.trc( name, TRCLEVEL_EXCEPTION, __LINE__, 9999, "can't open [%s] for loading", filename );
    return;
  }

  char hexline[128];
  int nr = 0;
  while( FileOp.readStr( f, hexline ) ) {
    nr++;
    TraceOp.trc( name, TRCLEVEL_INFO, __LINE__, 9999, "parsing line %d: %s ", nr, hexline );
    if( hexline[0] == ':' ) {
      evaluateLine(hexline, &offset, &block);
    }
  }


  FileOp.base.del(f);

  ThreadOp.sleep(100);

}

