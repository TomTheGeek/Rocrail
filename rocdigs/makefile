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
PLATFORM=LINUX
ifeq ($(PLATFORM),WIN32)
	LIBS=-liphlpapi -lmpr -lwsock32 -ladvapi32 -lwinmm
	DIRPREFIX=win
	CC_EXTRA_FLAGS=
	LNK_FLAGS=--def rocdigs.def
	LNK=$(TOOLPREFIX)dllwrap
	SHAREDSUFFIX=.dll
else ifeq ($(PLATFORM),MACOSX)
	LIBS=-lpthread -ldl
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	LNK_FLAGS=-dynamiclib -flat_namespace -undefined suppress
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
OUTDIR=../$(DIRPREFIX)bin
GENDIR=../$(DIRPREFIX)gen
TMPOUTDIR=$(GENDIR)/rocdigs/bin
BUILDBINDIR=../rocs/bin
DEBUG=-g
DESTDIR?=
PREFIX?=/usr/local
LIBDIR?=$(PREFIX)/lib
ROCRAIL_LIBDIR=$(LIBDIR)/rocrail

CPP=$(TOOLPREFIX)gcc
INCL_PATH=$(MOUNTPOINT)
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -I$(INCL_PATH) -I$(GENDIR)
DDL_CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -O2 -I$(INCL_PATH) -I$(GENDIR)
RRLIBS=$(OUTDIR)/libwrapper.a $(OUTDIR)/librocs.a

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))

DDXOBJS=$(patsubst impl/ddx/%.c,$(TMPOUTDIR)/ddx/%.o,$(wildcard impl/ddx/*.c))
LNOBJS=$(patsubst impl/loconet/%.c,$(TMPOUTDIR)/loconet/%.o,$(wildcard impl/loconet/*.c))
ECOSOBJS=$(patsubst impl/ecos/%.c,$(TMPOUTDIR)/ecos/%.o,$(wildcard impl/ecos/*.c))
COMMONOBJS=$(patsubst impl/common/%.c,$(TMPOUTDIR)/common/%.o,$(wildcard impl/common/*.c))
ROCNETOBJS=$(patsubst impl/rocnet/%.c,$(TMPOUTDIR)/rocnet/%.o,$(wildcard impl/rocnet/*.c))
OPENDCCOBJS=$(patsubst impl/opendcc/%.c,$(TMPOUTDIR)/opendcc/%.o,$(wildcard impl/opendcc/*.c))

all: $(OBJS) $(OUTDIR)/opendcc$(SHAREDSUFFIX) $(OUTDIR)/p50$(SHAREDSUFFIX) $(OUTDIR)/p50x$(SHAREDSUFFIX) $(OUTDIR)/hsi88$(SHAREDSUFFIX) $(OUTDIR)/srcp$(SHAREDSUFFIX) \
	$(OUTDIR)/lenz$(SHAREDSUFFIX) $(OUTDIR)/roco$(SHAREDSUFFIX) $(OUTDIR)/zimo$(SHAREDSUFFIX) $(OUTDIR)/dinamo$(SHAREDSUFFIX) $(OUTDIR)/om32$(SHAREDSUFFIX) \
	$(OUTDIR)/ddx$(SHAREDSUFFIX) $(OUTDIR)/slx$(SHAREDSUFFIX) $(OUTDIR)/ecos$(SHAREDSUFFIX) \
	$(OUTDIR)/inter10$(SHAREDSUFFIX) $(OUTDIR)/barjut$(SHAREDSUFFIX) $(OUTDIR)/loconet$(SHAREDSUFFIX) \
	$(OUTDIR)/virtual$(SHAREDSUFFIX) $(OUTDIR)/rocnet$(SHAREDSUFFIX) $(OUTDIR)/mcs2$(SHAREDSUFFIX) \
	$(OUTDIR)/clock$(SHAREDSUFFIX) $(OUTDIR)/railcom$(SHAREDSUFFIX) $(OUTDIR)/sprog$(SHAREDSUFFIX)
	

# ------------------------------------------------------------
# The Digital Interfaces as shared libraries.
# ------------------------------------------------------------
$(OUTDIR)/opendcc$(SHAREDSUFFIX): $(TMPOUTDIR)/opendcc.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/opendcc$(SHAREDSUFFIX) $(TMPOUTDIR)/opendcc.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/p50$(SHAREDSUFFIX): $(TMPOUTDIR)/p50.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/p50$(SHAREDSUFFIX) $(TMPOUTDIR)/p50.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/p50x$(SHAREDSUFFIX): $(TMPOUTDIR)/p50x.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/p50x$(SHAREDSUFFIX) $(TMPOUTDIR)/p50x.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/srcp$(SHAREDSUFFIX): $(TMPOUTDIR)/srcp.o $(TMPOUTDIR)/srcp07.o $(TMPOUTDIR)/srcp08.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/srcp$(SHAREDSUFFIX) $(TMPOUTDIR)/srcp.o $(TMPOUTDIR)/srcp07.o $(TMPOUTDIR)/srcp08.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/hsi88$(SHAREDSUFFIX): $(TMPOUTDIR)/hsi88.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/hsi88$(SHAREDSUFFIX) $(TMPOUTDIR)/hsi88.o $(RRLIBS) $(LIBS)
$(OUTDIR)/lenz$(SHAREDSUFFIX): $(TMPOUTDIR)/lenz.o $(COMMONOBJS) $(RRLIBS) $(OPENDCCOBJS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/lenz$(SHAREDSUFFIX) $(TMPOUTDIR)/lenz.o $(OPENDCCOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/roco$(SHAREDSUFFIX): $(TMPOUTDIR)/roco.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/roco$(SHAREDSUFFIX) $(TMPOUTDIR)/roco.o $(COMMONOBJS) $(RRLIBS) $(LIBS)	
$(OUTDIR)/virtual$(SHAREDSUFFIX): $(TMPOUTDIR)/virtual.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/virtual$(SHAREDSUFFIX) $(TMPOUTDIR)/virtual.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/zimo$(SHAREDSUFFIX): $(TMPOUTDIR)/zimo.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/zimo$(SHAREDSUFFIX) $(TMPOUTDIR)/zimo.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/dinamo$(SHAREDSUFFIX): $(TMPOUTDIR)/dinamo.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/dinamo$(SHAREDSUFFIX) $(TMPOUTDIR)/dinamo.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/om32$(SHAREDSUFFIX): $(TMPOUTDIR)/om32.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/om32$(SHAREDSUFFIX) $(TMPOUTDIR)/om32.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/slx$(SHAREDSUFFIX): $(TMPOUTDIR)/slx.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/slx$(SHAREDSUFFIX) $(TMPOUTDIR)/slx.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/inter10$(SHAREDSUFFIX): $(TMPOUTDIR)/inter10.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/inter10$(SHAREDSUFFIX) $(TMPOUTDIR)/inter10.o $(RRLIBS) $(LIBS)
$(OUTDIR)/barjut$(SHAREDSUFFIX): $(TMPOUTDIR)/barjut.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/barjut$(SHAREDSUFFIX) $(TMPOUTDIR)/barjut.o $(RRLIBS) $(LIBS)
$(OUTDIR)/loconet$(SHAREDSUFFIX): $(LNOBJS) $(COMMONOBJS) $(TMPOUTDIR)/loconet.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/loconet$(SHAREDSUFFIX) $(TMPOUTDIR)/loconet.o $(LNOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/ecos$(SHAREDSUFFIX): $(ECOSOBJS) $(COMMONOBJS) $(TMPOUTDIR)/ecos.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/ecos$(SHAREDSUFFIX) $(TMPOUTDIR)/ecos.o $(ECOSOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/ddx$(SHAREDSUFFIX): $(DDXOBJS) $(COMMONOBJS) $(TMPOUTDIR)/ddx.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/ddx$(SHAREDSUFFIX) $(TMPOUTDIR)/ddx.o $(DDXOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)	
$(OUTDIR)/rocnet$(SHAREDSUFFIX): $(ROCNETOBJS) $(COMMONOBJS) $(TMPOUTDIR)/rocnet.o $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/rocnet$(SHAREDSUFFIX) $(TMPOUTDIR)/rocnet.o $(ROCNETOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/mcs2$(SHAREDSUFFIX): $(TMPOUTDIR)/mcs2.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/mcs2$(SHAREDSUFFIX) $(TMPOUTDIR)/mcs2.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/clock$(SHAREDSUFFIX): $(TMPOUTDIR)/clock.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/clock$(SHAREDSUFFIX) $(TMPOUTDIR)/clock.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/railcom$(SHAREDSUFFIX): $(TMPOUTDIR)/railcom.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/railcom$(SHAREDSUFFIX) $(TMPOUTDIR)/railcom.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/sprog$(SHAREDSUFFIX): $(TMPOUTDIR)/sprog.o $(COMMONOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/sprog$(SHAREDSUFFIX) $(TMPOUTDIR)/sprog.o $(COMMONOBJS) $(RRLIBS) $(LIBS)

# ------------------------------------------------------------
# The RocDigs objects.
# ------------------------------------------------------------
$(TMPOUTDIR)/%.o: impl/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/ddx/%.o: impl/ddx/%.c
	$(CPP) $(DDL_CC_FLAGS) $< -o $@

$(TMPOUTDIR)/loconet/%.o: impl/loconet/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/ecos/%.o: impl/ecos/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/common/%.o: impl/common/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/rocnet/%.o: impl/rocnet/%.c
	$(CPP) $(CC_FLAGS) $< -o $@
	
$(TMPOUTDIR)/opendcc/%.o: impl/opendcc/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

clean:
	-mkdir -p $(OUTDIR)
	-mkdir -p $(TMPOUTDIR)/ddx
	-mkdir -p $(TMPOUTDIR)/loconet
	-mkdir -p $(TMPOUTDIR)/rocnet
	-mkdir -p $(TMPOUTDIR)/ecos
	-mkdir -p $(TMPOUTDIR)/common
	-mkdir -p $(TMPOUTDIR)/opendcc
	-mkdir -p $(GENDIR)/rocdigs
	-rm -f $(TMPOUTDIR)/*.o
	-rm -f $(TMPOUTDIR)/ddx/*.o
	-rm -f $(TMPOUTDIR)/loconet/*.o
	-rm -f $(TMPOUTDIR)/rocnet/*.o
	-rm -f $(TMPOUTDIR)/ecos/*.o
	-rm -f $(TMPOUTDIR)/common/*.o
	-rm -f $(TMPOUTDIR)/opendcc/*.o
	-rm -f $(OUTDIR)/p50$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/p50x$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/srcp$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/hsi88$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/lenz$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/roco$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/zimo$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/dinamo$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/om32$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/ddx$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/slx$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/inter10$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/barjut$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/ecos$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/loconet$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/rocnet$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/virtual$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/mcs2$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/clock$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/railcom$(SHAREDSUFFIX)
	-rm -f $(OUTDIR)/sprog$(SHAREDSUFFIX)
	cd $(GENDIR)/rocdigs; ../$(BUILDBINDIR)/ogen ../../rocdigs/rocdigs.xml ../
	cd $(GENDIR)/rocdigs; ../$(BUILDBINDIR)/xml2cstr ../../common/version.xml impl/version.c svnLog

install:
	-mkdir -p $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/p50$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/p50x$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/hsi88$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/srcp$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/lenz$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/roco$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/zimo$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/dinamo$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/om32$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/ddx$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/slx$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/inter10$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/barjut$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/ecos$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/loconet$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/rocnet$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/virtual$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/mcs2$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/clock$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/railcom$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	-cp -p $(OUTDIR)/sprog$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR)
	
