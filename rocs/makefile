# ------------------------------------------------------------
# Rocs - OS independent C library
#
# Copyright (C) 2002-2007 - Rob Versluis <r.j.versluis@rocrail.net>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# ------------------------------------------------------------
# commandline:
#   make -f logon.mak AR=... PLATFORM=... DEBUG=... MOUNTPOINT=... OUTDIR=...
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
	LIBS=-liphlpapi -lmpr -lwsock32 -ladvapi32 -lwinmm
	COREDIR=win
	BINSUFFIX=.exe
	CC_EXTRA_FLAGS=
else
	COREDIR=unx
	BINSUFFIX=
	CC_EXTRA_FLAGS=-fPIC
endif
ifeq ($(PLATFORM),LINUX)
	LIBS=-lpthread -ldl
endif
ifeq ($(PLATFORM),MACOSX)
	LIBS=-lpthread -ldl
endif
ifeq ($(PLATFORM),HP)
	LIBS=-lpthread -ldld
endif
ifeq ($(PLATFORM),SOLARIS)
	LIBS=-lsocket -lnsl -lpthread -ldl
endif

TOOLPREFIX=
SRCMOUNTPOINT=..
GENMOUNTPOINT=../$(COREDIR)gen
OUTDIR=../$(COREDIR)bin
TMPOUTDIR=$(OUTDIR)
DEBUG=-g
OPENSSL=
SSLLIBS=
AR=$(TOOLPREFIX)ar

CPP=$(TOOLPREFIX)gcc
LNK=$(TOOLPREFIX)gcc

# --- compile flags ---
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) $(OPENSSL) -I$(SRCMOUNTPOINT) -I$(GENMOUNTPOINT)

# --- link flags ---
LNK_FLAGS=

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))

COREOBJS=$(patsubst impl/$(COREDIR)/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/$(COREDIR)/*.c))

all: $(OUTDIR)/librocs.a $(OUTDIR)/wgen$(BINSUFFIX) $(OUTDIR)/ogen$(BINSUFFIX) $(OUTDIR)/xml2cstr$(BINSUFFIX) \
$(OUTDIR)/addlang$(BINSUFFIX) $(OUTDIR)/mergelang$(BINSUFFIX) $(OUTDIR)/mdoc$(BINSUFFIX) $(OUTDIR)/lang2po$(BINSUFFIX) $(OUTDIR)/po2lang$(BINSUFFIX)

gen:
	bin/ogen rocs.xml

$(OUTDIR)/librocs.a: $(OBJS) $(COREOBJS)
	$(AR) rcs $(OUTDIR)/librocs.a $(OBJS) $(COREOBJS)



$(OUTDIR)/wgen$(BINSUFFIX): $(TMPOUTDIR)/wgen.o $(OUTDIR)/librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/wgen$(BINSUFFIX) $(TMPOUTDIR)/wgen.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)
	cp $(OUTDIR)/wgen$(BINSUFFIX) bin

$(OUTDIR)/ogen$(BINSUFFIX): $(TMPOUTDIR)/ogen.o $(OUTDIR)/librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/ogen$(BINSUFFIX) $(TMPOUTDIR)/ogen.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)
	cp $(OUTDIR)/ogen$(BINSUFFIX) bin

$(OUTDIR)/xml2cstr$(BINSUFFIX): $(TMPOUTDIR)/xml2cstr.o $(OUTDIR)/librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/xml2cstr$(BINSUFFIX) $(TMPOUTDIR)/xml2cstr.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)
	cp $(OUTDIR)/xml2cstr$(BINSUFFIX) bin

$(OUTDIR)/addlang$(BINSUFFIX): $(TMPOUTDIR)/addlang.o $(OUTDIR)/librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/addlang$(BINSUFFIX) $(TMPOUTDIR)/addlang.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)/mergelang$(BINSUFFIX): $(TMPOUTDIR)/mergelang.o $(OUTDIR)/librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/mergelang$(BINSUFFIX) $(TMPOUTDIR)/mergelang.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)/mdoc$(BINSUFFIX): $(TMPOUTDIR)/mdoc.o $(OUTDIR)/librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/mdoc$(BINSUFFIX) $(TMPOUTDIR)/mdoc.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)/lang2po$(BINSUFFIX): $(TMPOUTDIR)/lang2po.o $(OUTDIR)/librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/lang2po$(BINSUFFIX) $(TMPOUTDIR)/lang2po.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)/po2lang$(BINSUFFIX): $(TMPOUTDIR)/po2lang.o $(OUTDIR)/librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)/po2lang$(BINSUFFIX) $(TMPOUTDIR)/po2lang.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)

$(TMPOUTDIR)/%.o: impl/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/%.o: gen/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/%.o: impl/$(COREDIR)/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

clean:
	-mkdir -p $(OUTDIR)
	-mkdir -p $(TMPOUTDIR)
	-mkdir -p $(GENMOUNTPOINT)/rocs
	-rm -f $(TMPOUTDIR)/*.o
	-rm -f $(OUTDIR)/librocs.a
	cd $(GENMOUNTPOINT)/rocs; \
	../../rocs/bin/ogen ../../rocs/rocs.xml
	-rm $(GENMOUNTPOINT)/rocs-gen.tar.gz
	cd $(GENMOUNTPOINT); \
	tar -cf rocs-gen.tar rocs; gzip rocs-gen.tar; cp rocs-gen.tar.gz ../rocs;

fromtar:
	-mkdir -p $(OUTDIR)
	-mkdir -p $(TMPOUTDIR)
	-mkdir -p $(GENMOUNTPOINT)/rocs
	-rm -f $(TMPOUTDIR)/*.o
	-rm -f $(OUTDIR)/librocs.a
	cd $(GENMOUNTPOINT); \
	cp ../rocs/rocs-gen.tar.gz .; gzip -d rocs-gen.tar.gz; tar -xf rocs-gen.tar


ssl: OPENSSL=-D__OPENSSL__
ssl: SSLLIBS=-lssl
ssl: all
