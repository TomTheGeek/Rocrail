# ------------------------------------------------------------
# Rocrail - Model Railroad Software
#
# Copyright (C) 2002-2013 Rob Versluis, Rocrail.net
#
# Without an official permission commercial use is not permitted.
# Forking this project is not permitted.  
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
#   make PLATFORM=WIN32 clean
#   make TOOLPREFIX=i386-mingw32- PLATFORM=WIN32 all
#
# commandline for mingw native compiling:
#   make PLATFORM=WIN32 NATIVE=WIN32 clean
#   make TOOLPREFIX=i386-mingw32- PLATFORM=WIN32 NATIVE=WIN32 all
#
# commandline for osx compiling:
#   make PLATFORM=MACOSX clean all
#
FS=/
CS=;
COPY=cp
REMOVE=rm -f
GENSUFFIX=
ANSI=-ansi
MKDIR=mkdir -p
CPUPDATE=u
LNK_FLAGS=
MAKE ?= make
QUOT="

ifeq ($(NATIVE),WIN32)
	FS=$\\
	CS=&
	COPY=copy
	REMOVE=del /Q
	GENSUFFIX=.exe
	ANSI=
	MKDIR=mkdir
	QUOT=
endif


PLATFORM=LINUX
ARCH=
ifeq ($(PLATFORM),WIN32)
	LIBS=-liphlpapi -lmpr -lws2_32 -ladvapi32 -lwinmm
	DIRPREFIX=win
	CC_EXTRA_FLAGS=-mthreads
	LNK_FLAGS=-mthreads -static
	BINSUFFIX=.exe
	SHAREDSUFFIX=.dll
else ifeq ($(PLATFORM),WIN64)
	DIRPREFIX=win
	LIBS=-liphlpapi -lmpr -lmswsock -lws2_32 -ladvapi32 -lwinmm
	BINSUFFIX=.exe
	SHAREDSUFFIX=.dll
	CC_EXTRA_FLAGS=
else ifeq ($(PLATFORM),MACOSX)
	LIBS=-lpthread -ldl
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	BINSUFFIX=
	SHAREDSUFFIX=.dylib
else ifeq ($(PLATFORM),OPENBSD)
	LIBS=-pthread
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	BINSUFFIX=
	CPUPDATE=
	SHAREDSUFFIX=.so
else
	LIBS=-lpthread -ldl
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	BINSUFFIX=
	SHAREDSUFFIX=.so
endif

MOUNTPOINT=..
OUTDIR=..$(FS)$(DIRPREFIX)bin
GENDIR=..$(FS)$(DIRPREFIX)gen
TMPOUTDIR=$(GENDIR)$(FS)rocnetnode$(FS)bin
DEBUG=-g
DESTDIR=$(FS)opt$(FS)rocnet
BINDIR=..$(FS)rocs$(FS)bin

CPP=$(TOOLPREFIX)gcc
LNK=$(TOOLPREFIX)gcc
INCL_PATH=$(MOUNTPOINT)
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) $(ANSI) -I$(INCL_PATH) -I$(GENDIR)
RRLIBS=$(OUTDIR)$(FS)libwrapper.a $(OUTDIR)$(FS)librocutils.a $(OUTDIR)$(FS)librocs.a $(OUTDIR)$(FS)librocrail.a

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))
RNOBJS=../unxgen/rocdigs/bin/rocnet/rn-utils.o

TARGET=$(OUTDIR)$(FS)rocnetnode$(BINSUFFIX)

all: version $(TARGET)

$(TARGET): $(OBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(TARGET) $(OBJS) $(RNOBJS) $(RRLIBS) $(LIBS)

# ------------------------------------------------------------
# The RocNetNode objects.
# ------------------------------------------------------------
$(TMPOUTDIR)/%.o: impl/%.c $(GENDIR)$(FS)rocrail$(FS)wrapper$(FS)public$(FS)wrapper.h
	$(CPP) $(CC_FLAGS) $< -o $@

clean:
	-$(MKDIR) $(OUTDIR)
	-$(MKDIR) $(TMPOUTDIR)
	-$(MKDIR) $(GENDIR)$(FS)rocnetnode
	-$(REMOVE) $(OUTDIR)$(FS)*.o
	-$(REMOVE) $(GENDIR)$(FS)rocnetnode$(FS)bin$(FS)*
	-$(REMOVE) $(GENDIR)$(FS)rocnetnode$(FS)public$(FS)*
	cd $(GENDIR)$(FS)rocnetnode$(CS) ..$(FS)$(BINDIR)$(FS)ogen$(GENSUFFIX) ..$(FS)..$(FS)rocnetnode$(FS)rocnetnode.xml ..$(FS)


install:
	-$(MKDIR) $(DESTDIR)
	-$(COPY) -p $(OUTDIR)$(FS)rocnetnode$(BINSUFFIX) $(DESTDIR)
	-$(COPY) -p $(OUTDIR)$(FS)dcc232$(SHAREDSUFFIX) $(DESTDIR)
	-$(COPY) -p $(OUTDIR)$(FS)sprog$(SHAREDSUFFIX) $(DESTDIR)
	-$(COPY) -p $(OUTDIR)$(FS)rfid12$(SHAREDSUFFIX) $(DESTDIR)
	-cp -p package$(FS)rocnetnode.sh $(DESTDIR)
	-cp -p package$(FS)update.sh $(DESTDIR)
#	-cp -p package$(FS)rocnetnode.ini $(DESTDIR)

version:
	echo $(QUOT)const int revisionnr = $(QUOT) > ..$(FS)common$(FS)version.h
	git rev-list --count HEAD >> ..$(FS)common$(FS)version.h
	echo $(QUOT);$(QUOT) >> ..$(FS)common$(FS)version.h
	echo $(QUOT)const char* commithash = $(QUOT) >> ..$(FS)common$(FS)version.h
	git log -n 1 --pretty=format:\"%H\" >> ..$(FS)common$(FS)version.h
	echo $(QUOT);$(QUOT) >> ..$(FS)common$(FS)version.h

