# ------------------------------------------------------------
# Rocrail - Model Railroad Software
#
# Copyright (C) 2002-2010 - Rob Versluis <r.j.versluis@rocrail.net>
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

ifeq ($(NATIVE),WIN32)
	FS=$\\
	CS=&
	COPY=copy
	REMOVE=del /Q
	GENSUFFIX=.exe
	ANSI=
	MKDIR=mkdir
endif


PLATFORM=LINUX
ARCH=
ifeq ($(PLATFORM),WIN32)
	LIBS=-liphlpapi -lmpr -lwsock32 -ladvapi32 -lwinmm
	DIRPREFIX=win
	CC_EXTRA_FLAGS=
	BINSUFFIX=.exe
else
	LIBS=-lpthread -ldl
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	BINSUFFIX=
endif

MOUNTPOINT=..
OUTDIR=..$(FS)$(DIRPREFIX)bin
GENDIR=..$(FS)$(DIRPREFIX)gen
TMPOUTDIR=$(GENDIR)$(FS)rocet$(FS)bin
DEBUG=-g
DESTDIR=$(FS)opt$(FS)rocet
BINDIR=..$(FS)rocs$(FS)bin

CPP=$(TOOLPREFIX)gcc
LNK=$(TOOLPREFIX)gcc
INCL_PATH=$(MOUNTPOINT)
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) $(ANSI) -I$(INCL_PATH) -I$(GENDIR)
LNK_FLAGS=
RRLIBS=$(OUTDIR)$(FS)libwrapper.a $(OUTDIR)$(FS)librocs.a $(OUTDIR)$(FS)librocutils.a

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))
WOBJS=$(patsubst $(GENDIR)/rocet/wrapper/impl/%.c,$(GENDIR)/rocet/wrapper/bin/%.o,$(wildcard $(GENDIR)/rocet/wrapper/impl/*.c))

TARGET=$(OUTDIR)$(FS)rocet$(BINSUFFIX)

all: $(TARGET)

$(TARGET): $(WOBJS) $(OBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(TARGET) $(OBJS) $(RRLIBS) $(LIBS)

# ------------------------------------------------------------
# The Rocet objects.
# ------------------------------------------------------------
$(TMPOUTDIR)/%.o: impl/%.c $(GENDIR)$(FS)rocet$(FS)wrapper$(FS)public$(FS)wrapper.h
	$(CPP) $(CC_FLAGS) $< -o $@

$(GENDIR)/rocet/wrapper/bin/%.o: $(GENDIR)$(FS)rocet$(FS)wrapper$(FS)impl$(FS)%.c
	$(CPP) $(CC_FLAGS) $< -o $@

clean:
	-$(MKDIR) $(OUTDIR)
	-$(MKDIR) $(TMPOUTDIR)
	-$(MKDIR) $(GENDIR)$(FS)rocet
	-$(REMOVE) $(OUTDIR)$(FS)*.o
	-$(REMOVE) $(GENDIR)$(FS)rocet$(FS)wrapper$(FS)public$(FS)*
	-$(REMOVE) $(GENDIR)$(FS)rocet$(FS)wrapper$(FS)impl$(FS)*
	-$(REMOVE) $(GENDIR)$(FS)rocet$(FS)wrapper$(FS)bin$(FS)*
	-$(REMOVE) $(GENDIR)$(FS)rocet$(FS)wrapper$(FS)doc$(FS)*
	cd $(GENDIR)$(FS)rocet$(CS) ..$(FS)$(BINDIR)$(FS)wgen$(GENSUFFIX) ..$(FS)..$(FS)rocet$(FS)public$(FS)wrapper.xml
	cd $(GENDIR)$(FS)rocet$(CS) ..$(FS)$(BINDIR)$(FS)ogen$(GENSUFFIX) ..$(FS)..$(FS)rocet$(FS)rocet.xml ..$(FS)

install:
	-$(COPY) -p $(OUTDIR)$(FS)rocet$(BINSUFFIX) $(DESTDIR)


