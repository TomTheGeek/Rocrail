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
# commandline for osx compiling:
#   make PLATFORM=MACOSX clean all
#
PLATFORM=LINUX
ifeq ($(PLATFORM),WIN32)
	LIBS=-liphlpapi -lmpr -lwsock32 -ladvapi32
	DIRPREFIX=win
	CC_EXTRA_FLAGS=
	LNK_FLAGS=--def lcdriver.def
	LNK=$(TOOLPREFIX)dllwrap
	SHAREDSUFFIX=.dll
else ifeq ($(PLATFORM),MACOSX)
	LIBS=-lpthread -ldl
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	LNK_FLAGS=-dynamiclib -flat_namespace -undefined suppress
	LNK=gcc
	SHAREDSUFFIX=.dylib
else
	LIBS=-lpthread -ldl
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	LNK_FLAGS=-shared
	LNK=gcc
	SHAREDSUFFIX=.so
endif

MOUNTPOINT=..
OUTDIR=../$(DIRPREFIX)bin
GENDIR=../$(DIRPREFIX)gen
TMPOUTDIR=$(GENDIR)/roclcdr/bin
DEBUG=-g
DESTDIR=/opt/rocrail
BINDIR=../rocs/bin

CPP=$(TOOLPREFIX)gcc
INCL_PATH=$(MOUNTPOINT)
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -ansi -I$(INCL_PATH) -I$(GENDIR)
RRLIBS=$(OUTDIR)/libwrapper.a $(OUTDIR)/librocs.a

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))
EVENTSOBJS=$(patsubst impl/events/%.c,$(TMPOUTDIR)/events/%.o,$(wildcard impl/events/*.c))
STATUSOBJS=$(patsubst impl/status/%.c,$(TMPOUTDIR)/status/%.o,$(wildcard impl/status/*.c))
TOOLSOBJS=$(patsubst impl/tools/%.c,$(TMPOUTDIR)/tools/%.o,$(wildcard impl/tools/*.c))

all: $(OBJS) $(OUTDIR)/lcdriver$(SHAREDSUFFIX)
#	-rm -f $(TMPOUTDIR)/*.o

# ------------------------------------------------------------
# The LocDrivers as shared libraries.
# ------------------------------------------------------------
$(OUTDIR)/lcdriver$(SHAREDSUFFIX): $(TMPOUTDIR)/lcdriver.o $(RRLIBS) $(EVENTSOBJS) $(STATUSOBJS) $(TOOLSOBJS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/lcdriver$(SHAREDSUFFIX) $(TMPOUTDIR)/lcdriver.o $(EVENTSOBJS) $(STATUSOBJS) $(TOOLSOBJS) $(RRLIBS) $(LIBS)

# ------------------------------------------------------------
# The RocLcDr objects.
# ------------------------------------------------------------
$(TMPOUTDIR)/%.o: impl/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/%.o: impl/events/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/%.o: impl/status/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/%.o: impl/tools/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

clean:
	-mkdir -p $(OUTDIR)
	-mkdir -p $(GENDIR)/roclcdr
	-mkdir -p $(TMPOUTDIR)
	-mkdir -p $(TMPOUTDIR)/events
	-mkdir -p $(TMPOUTDIR)/status
	-mkdir -p $(TMPOUTDIR)/tools
	-rm -f $(TMPOUTDIR)/*.o
	-rm -f $(TMPOUTDIR)/events/*.o
	-rm -f $(TMPOUTDIR)/status/*.o
	-rm -f $(TMPOUTDIR)/tools/*.o
	cd $(GENDIR)/roclcdr; ../$(BINDIR)/ogen ../../roclcdr/roclcdr.xml ../

install:
  ifeq ($(PLATFORM),LINUX)
	-cp -p $(OUTDIR)/lcdriver.so $(DESTDIR)
  endif

