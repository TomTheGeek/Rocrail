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
MKDIR=mkdir -p

ifeq ($(NATIVE),WIN32)
	FS=$\\
	CS=&
	COPY=copy
	REMOVE=del /Q
	GENSUFFIX=.exe
	MKDIR=mkdir
endif


PLATFORM=LINUX
ifeq ($(PLATFORM),WIN32)
	LIBS=-liphlpapi -lmpr -lws2_32 -ladvapi32
	DIRPREFIX=win
	CC_EXTRA_FLAGS=
	LNK_FLAGS=--def analyser.def
	LNK=$(TOOLPREFIX)dllwrap
	SHAREDSUFFIX=.dll
else ifeq ($(PLATFORM),WIN64)
	DIRPREFIX=win
	LIBS=-liphlpapi -lmpr -lmswsock -lws2_32 -ladvapi32
	LNK=$(TOOLPREFIX)dllwrap
	LNK_FLAGS=--def analyser.def
	SHAREDSUFFIX=.dll
	CC_EXTRA_FLAGS=
else ifeq ($(PLATFORM),MACOSX)
	LIBS=-lpthread -ldl
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	LNK_FLAGS=-dynamiclib -flat_namespace -undefined suppress
	LNK=gcc
	SHAREDSUFFIX=.dylib
else ifeq ($(PLATFORM),OPENBSD)
	LIBS=-pthread
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	LNK_FLAGS=-shared
	LNK=gcc
	SHAREDSUFFIX=.so
else
	LIBS=-lpthread -ldl
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	LNK_FLAGS=-shared
	LNK=gcc
	SHAREDSUFFIX=.so
endif

MOUNTPOINT=..
OUTDIR=..$(FS)$(DIRPREFIX)bin
GENDIR=..$(FS)$(DIRPREFIX)gen
TMPOUTDIR=$(GENDIR)$(FS)roclcdr$(FS)bin
DEBUG=-g
DESTDIR=$(FS)opt$(FS)rocrail
BINDIR=..$(FS)rocs$(FS)bin

CPP=$(TOOLPREFIX)gcc
INCL_PATH=$(MOUNTPOINT)
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -I$(INCL_PATH) -I$(GENDIR)
RRLIBS=$(OUTDIR)$(FS)libwrapper.a $(OUTDIR)$(FS)librocs.a

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))

all: $(OBJS) $(OUTDIR)$(FS)analyser$(SHAREDSUFFIX)
#	-$(REMOVE) $(TMPOUTDIR)$(FS)*.o

# ------------------------------------------------------------
# The Analyser as shared library.
# ------------------------------------------------------------
$(OUTDIR)$(FS)analyser$(SHAREDSUFFIX): $(TMPOUTDIR)$(FS)analyse.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)analyser$(SHAREDSUFFIX) $(TMPOUTDIR)$(FS)analyse.o $(RRLIBS) $(LIBS)

# ------------------------------------------------------------
# The RocLcDr objects.
# ------------------------------------------------------------
$(TMPOUTDIR)/%.o: impl/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

clean:
	-$(MKDIR) $(OUTDIR)
	-$(MKDIR) $(GENDIR)$(FS)analyser
	-$(MKDIR) $(TMPOUTDIR)
	-$(REMOVE) $(TMPOUTDIR)$(FS)*.o
	cd $(GENDIR)$(FS)analyser$(CS) ..$(FS)$(BINDIR)$(FS)ogen$(GENSUFFIX) ..$(FS)..$(FS)analyser$(FS)analyser.xml ..$(FS)

install:
  ifeq ($(PLATFORM),LINUX)
	-cp -p $(OUTDIR)$(FS)analyser.so $(DESTDIR)
  endif

