# ------------------------------------------------------------
# Rocrail - Model Railroad Software
#
# Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# ------------------------------------------------------------
# commandline:
#   make DEBUG=... MOUNTPOINT=... OUTDIR=...
#
# commandline for mingw cross compiling:
#   make TOOLPREFIX=i386-mingw32- PLATFORM=WIN32 clean
#   make TOOLPREFIX=i386-mingw32- PLATFORM=WIN32 all
#
include ../makefile.common

ifeq ($(PLATFORM),WIN32)
	LDFLAGS=--def lcdriver.def
else ifeq ($(PLATFORM),MACOSX)
	LDFLAGS=-dynamiclib -flat_namespace -undefined suppress
else
	LDFLAGS=-shared
endif

TMPOUTDIR=$(GENDIR)/roclcdr/bin

CFLAGS+= -ansi
RRLIBS=$(OUTDIR)/libwrapper.a $(OUTDIR)/librocs.a

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))
EVENTSOBJS=$(patsubst impl/events/%.c,$(TMPOUTDIR)/events/%.o,$(wildcard impl/events/*.c))
STATUSOBJS=$(patsubst impl/status/%.c,$(TMPOUTDIR)/status/%.o,$(wildcard impl/status/*.c))
TOOLSOBJS=$(patsubst impl/tools/%.c,$(TMPOUTDIR)/tools/%.o,$(wildcard impl/tools/*.c))

all: build

generate: $(GENDIR)/roclcdr

$(GENDIR)/roclcdr: roclcdr.xml
	$(RM) -r $(GENDIR)/roclcdr
	-mkdir -p $(OUTDIR)
	-mkdir -p $(GENDIR)/roclcdr
	-mkdir -p $(TMPOUTDIR)
	-mkdir -p $(TMPOUTDIR)/events
	-mkdir -p $(TMPOUTDIR)/status
	-mkdir -p $(TMPOUTDIR)/tools
	$(RM) $(TMPOUTDIR)/*.o
	$(RM) $(TMPOUTDIR)/events/*.o
	$(RM) $(TMPOUTDIR)/status/*.o
	$(RM) $(TMPOUTDIR)/tools/*.o
	cd $(GENDIR)/roclcdr; ../$(BUILDBINDIR)/ogen ../../roclcdr/roclcdr.xml ../

build: generate $(OBJS) $(OUTDIR)/lcdriver$(SHAREDSUFFIX)

# ------------------------------------------------------------
# The LocDrivers as shared libraries.
# ------------------------------------------------------------
$(OUTDIR)/lcdriver$(SHAREDSUFFIX): $(TMPOUTDIR)/lcdriver.o $(RRLIBS) $(EVENTSOBJS) $(STATUSOBJS) $(TOOLSOBJS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/lcdriver$(SHAREDSUFFIX) $(TMPOUTDIR)/lcdriver.o $(EVENTSOBJS) $(STATUSOBJS) $(TOOLSOBJS) $(RRLIBS) $(LIBS)

# ------------------------------------------------------------
# The RocLcDr objects.
# ------------------------------------------------------------
$(TMPOUTDIR)/%.o: impl/%.c
	$(CC) $(CFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: impl/events/%.c
	$(CC) $(CFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: impl/status/%.c
	$(CC) $(CFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: impl/tools/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(OUTDIR)/lcdriver$(SHAREDSUFFIX)
	$(RM) -r $(GENDIR)/roclcdr

install:
  ifeq ($(PLATFORM),LINUX)
	-mkdir -p $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/lcdriver.so $(DESTDIR)$(ROCRAIL_LIBDIR)
  endif

uninstall:
	$(RM) $(DESTDIR)$(ROCRAIL_LIBDIR)/lcdriver.so
