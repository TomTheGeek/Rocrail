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

DCC_LIBS=opendcc p50 p50x srcp hsi88 lenz roco zimo dinamo om32 ddx slx inter10 barjut ecos loconet rocnet virtual mcs2 clock sprog

ifeq ($(PLATFORM),WIN32)
	LDFLAGS=--def rocdigs.def
else ifeq ($(PLATFORM),MACOSX)
	LDFLAGS=-dynamiclib -flat_namespace -undefined suppress
else
	LDFLAGS=-shared
endif

TMPOUTDIR=$(GENDIR)/rocdigs/bin

DDL_CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -O2 -I$(INCL_PATH) -I$(GENDIR)
RRLIBS=$(OUTDIR)/libwrapper.a $(OUTDIR)/librocs.a

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))

DDXOBJS=$(patsubst impl/ddx/%.c,$(TMPOUTDIR)/ddx/%.o,$(wildcard impl/ddx/*.c))
LNOBJS=$(patsubst impl/loconet/%.c,$(TMPOUTDIR)/loconet/%.o,$(wildcard impl/loconet/*.c))
ECOSOBJS=$(patsubst impl/ecos/%.c,$(TMPOUTDIR)/ecos/%.o,$(wildcard impl/ecos/*.c))
COMMONOBJS=$(patsubst impl/common/%.c,$(TMPOUTDIR)/common/%.o,$(wildcard impl/common/*.c))
ROCNETOBJS=$(patsubst impl/rocnet/%.c,$(TMPOUTDIR)/rocnet/%.o,$(wildcard impl/rocnet/*.c))
OPENDCCOBJS=$(patsubst impl/opendcc/%.c,$(TMPOUTDIR)/opendcc/%.o,$(wildcard impl/opendcc/*.c))

all: build

generate: $(GENDIR)/rocdigs

$(GENDIR)/rocdigs: rocdigs.xml ../common/version.xml
	$(RM) -r $(GENDIR)/rocdigs
	-mkdir -p $(OUTDIR)
	-mkdir -p $(TMPOUTDIR)/ddx
	-mkdir -p $(TMPOUTDIR)/loconet
	-mkdir -p $(TMPOUTDIR)/rocnet
	-mkdir -p $(TMPOUTDIR)/ecos
	-mkdir -p $(TMPOUTDIR)/common
	-mkdir -p $(TMPOUTDIR)/opendcc
	-mkdir -p $(GENDIR)/rocdigs
	$(RM) $(TMPOUTDIR)/*.o
	$(RM) $(TMPOUTDIR)/ddx/*.o
	$(RM) $(TMPOUTDIR)/loconet/*.o
	$(RM) $(TMPOUTDIR)/rocnet/*.o
	$(RM) $(TMPOUTDIR)/ecos/*.o
	$(RM) $(TMPOUTDIR)/common/*.o
	$(RM) $(TMPOUTDIR)/opendcc/*.o
	@for dcc_lib in $(DCC_LIBS) ; do  rm -f $(OUTDIR)/$$dcc_lib$(SHAREDSUFFIX) ; done
	cd $(GENDIR)/rocdigs; ../$(BUILDBINDIR)/ogen ../../rocdigs/rocdigs.xml ../
	cd $(GENDIR)/rocdigs; ../$(BUILDBINDIR)/xml2cstr ../../common/version.xml impl/version.c svnLog

build: generate $(OBJS) $(OUTDIR)/opendcc$(SHAREDSUFFIX) $(OUTDIR)/p50$(SHAREDSUFFIX) $(OUTDIR)/p50x$(SHAREDSUFFIX) $(OUTDIR)/hsi88$(SHAREDSUFFIX) $(OUTDIR)/srcp$(SHAREDSUFFIX) \
	$(OUTDIR)/lenz$(SHAREDSUFFIX) $(OUTDIR)/roco$(SHAREDSUFFIX) $(OUTDIR)/zimo$(SHAREDSUFFIX) $(OUTDIR)/dinamo$(SHAREDSUFFIX) $(OUTDIR)/om32$(SHAREDSUFFIX) \
	$(OUTDIR)/ddx$(SHAREDSUFFIX) $(OUTDIR)/slx$(SHAREDSUFFIX) $(OUTDIR)/ecos$(SHAREDSUFFIX) \
	$(OUTDIR)/inter10$(SHAREDSUFFIX) $(OUTDIR)/barjut$(SHAREDSUFFIX) $(OUTDIR)/loconet$(SHAREDSUFFIX) \
	$(OUTDIR)/virtual$(SHAREDSUFFIX) $(OUTDIR)/rocnet$(SHAREDSUFFIX) $(OUTDIR)/mcs2$(SHAREDSUFFIX) \
	$(OUTDIR)/clock$(SHAREDSUFFIX) $(OUTDIR)/sprog$(SHAREDSUFFIX)
	

# ------------------------------------------------------------
# The Digital Interfaces as shared libraries.
# ------------------------------------------------------------
$(OUTDIR)/opendcc$(SHAREDSUFFIX): $(TMPOUTDIR)/opendcc.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/opendcc$(SHAREDSUFFIX) $(TMPOUTDIR)/opendcc.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/p50$(SHAREDSUFFIX): $(TMPOUTDIR)/p50.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/p50$(SHAREDSUFFIX) $(TMPOUTDIR)/p50.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/p50x$(SHAREDSUFFIX): $(TMPOUTDIR)/p50x.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/p50x$(SHAREDSUFFIX) $(TMPOUTDIR)/p50x.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/srcp$(SHAREDSUFFIX): $(TMPOUTDIR)/srcp.o $(TMPOUTDIR)/srcp07.o $(TMPOUTDIR)/srcp08.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/srcp$(SHAREDSUFFIX) $(TMPOUTDIR)/srcp.o $(TMPOUTDIR)/srcp07.o $(TMPOUTDIR)/srcp08.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/hsi88$(SHAREDSUFFIX): $(TMPOUTDIR)/hsi88.o $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/hsi88$(SHAREDSUFFIX) $(TMPOUTDIR)/hsi88.o $(RRLIBS) $(LIBS)
$(OUTDIR)/lenz$(SHAREDSUFFIX): $(TMPOUTDIR)/lenz.o $(COMMONOBJS) $(RRLIBS) $(OPENDCCOBJS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/lenz$(SHAREDSUFFIX) $(TMPOUTDIR)/lenz.o $(OPENDCCOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/roco$(SHAREDSUFFIX): $(TMPOUTDIR)/roco.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/roco$(SHAREDSUFFIX) $(TMPOUTDIR)/roco.o $(COMMONOBJS) $(RRLIBS) $(LIBS)	
$(OUTDIR)/virtual$(SHAREDSUFFIX): $(TMPOUTDIR)/virtual.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/virtual$(SHAREDSUFFIX) $(TMPOUTDIR)/virtual.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/zimo$(SHAREDSUFFIX): $(TMPOUTDIR)/zimo.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/zimo$(SHAREDSUFFIX) $(TMPOUTDIR)/zimo.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/dinamo$(SHAREDSUFFIX): $(TMPOUTDIR)/dinamo.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/dinamo$(SHAREDSUFFIX) $(TMPOUTDIR)/dinamo.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/om32$(SHAREDSUFFIX): $(TMPOUTDIR)/om32.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/om32$(SHAREDSUFFIX) $(TMPOUTDIR)/om32.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/slx$(SHAREDSUFFIX): $(TMPOUTDIR)/slx.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/slx$(SHAREDSUFFIX) $(TMPOUTDIR)/slx.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/inter10$(SHAREDSUFFIX): $(TMPOUTDIR)/inter10.o $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/inter10$(SHAREDSUFFIX) $(TMPOUTDIR)/inter10.o $(RRLIBS) $(LIBS)
$(OUTDIR)/barjut$(SHAREDSUFFIX): $(TMPOUTDIR)/barjut.o $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/barjut$(SHAREDSUFFIX) $(TMPOUTDIR)/barjut.o $(RRLIBS) $(LIBS)
$(OUTDIR)/loconet$(SHAREDSUFFIX): $(LNOBJS) $(COMMONOBJS) $(TMPOUTDIR)/loconet.o $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/loconet$(SHAREDSUFFIX) $(TMPOUTDIR)/loconet.o $(LNOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/ecos$(SHAREDSUFFIX): $(ECOSOBJS) $(COMMONOBJS) $(TMPOUTDIR)/ecos.o $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/ecos$(SHAREDSUFFIX) $(TMPOUTDIR)/ecos.o $(ECOSOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/ddx$(SHAREDSUFFIX): $(DDXOBJS) $(COMMONOBJS) $(TMPOUTDIR)/ddx.o $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/ddx$(SHAREDSUFFIX) $(TMPOUTDIR)/ddx.o $(DDXOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)	
$(OUTDIR)/rocnet$(SHAREDSUFFIX): $(ROCNETOBJS) $(COMMONOBJS) $(TMPOUTDIR)/rocnet.o $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/rocnet$(SHAREDSUFFIX) $(TMPOUTDIR)/rocnet.o $(ROCNETOBJS) $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/mcs2$(SHAREDSUFFIX): $(TMPOUTDIR)/mcs2.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/mcs2$(SHAREDSUFFIX) $(TMPOUTDIR)/mcs2.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/clock$(SHAREDSUFFIX): $(TMPOUTDIR)/clock.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/clock$(SHAREDSUFFIX) $(TMPOUTDIR)/clock.o $(COMMONOBJS) $(RRLIBS) $(LIBS)
$(OUTDIR)/sprog$(SHAREDSUFFIX): $(TMPOUTDIR)/sprog.o $(COMMONOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(OUTDIR)/sprog$(SHAREDSUFFIX) $(TMPOUTDIR)/sprog.o $(COMMONOBJS) $(RRLIBS) $(LIBS)

# ------------------------------------------------------------
# The RocDigs objects.
# ------------------------------------------------------------
$(TMPOUTDIR)/%.o: impl/%.c
	$(CC) $(CFLAGS) $< -o $@

$(TMPOUTDIR)/ddx/%.o: impl/ddx/%.c
	$(CC) $(DDL_CC_FLAGS) $< -o $@

$(TMPOUTDIR)/loconet/%.o: impl/loconet/%.c
	$(CC) $(CFLAGS) $< -o $@

$(TMPOUTDIR)/ecos/%.o: impl/ecos/%.c
	$(CC) $(CFLAGS) $< -o $@

$(TMPOUTDIR)/common/%.o: impl/common/%.c
	$(CC) $(CFLAGS) $< -o $@

$(TMPOUTDIR)/rocnet/%.o: impl/rocnet/%.c
	$(CC) $(CFLAGS) $< -o $@
	
$(TMPOUTDIR)/opendcc/%.o: impl/opendcc/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	@for dcc_lib in $(DCC_LIBS) ; do  rm -f $(OUTDIR)/$$dcc_lib$(SHAREDSUFFIX) ; done
	$(RM) -r $(GENDIR)/rocdigs

install:
	-mkdir -p $(DESTDIR)$(ROCRAIL_LIBDIR)
	@for dcc_lib in $(DCC_LIBS) ; do  cp -p $(OUTDIR)/$$dcc_lib$(SHAREDSUFFIX) $(DESTDIR)$(ROCRAIL_LIBDIR) ; done

uninstall:
	@for dcc_lib in $(DCC_LIBS) ; do  $(RM) $(DESTDIR)$(ROCRAIL_LIBDIR)/$$dcc_lib$(SHAREDSUFFIX) ; done
