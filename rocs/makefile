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
include ../makefile.common

TMPOUTDIR=$(GENDIR)/rocs/bin
OPENSSL=
SSLLIBS=

# --- compile flags ---
CFLAGS+= $(OPENSSL)

# --- link flags ---
LDFLAGS=

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))

COREOBJS=$(patsubst impl/$(DIRPREFIX)/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/$(DIRPREFIX)/*.c))

all: build

generate: $(GENDIR)/rocs

$(GENDIR)/rocs: rocs.xml
	$(RM) -r $(GENDIR)/rocs
	-mkdir -p $(OUTDIR)
	-mkdir -p $(TMPOUTDIR)
	-mkdir -p $(GENDIR)/rocs
	$(RM) $(TMPOUTDIR)/*.o
	$(RM) $(OUTDIR)/librocs.a
	cd $(GENDIR)/rocs; \
	../$(BUILDBINDIR)/ogen ../../rocs/rocs.xml
	$(RM) $(GENDIR)/rocs-gen.tar.gz
	cd $(GENDIR); \
	tar -cf rocs-gen.tar rocs; gzip rocs-gen.tar; cp rocs-gen.tar.gz ../rocs;

build: generate $(OUTDIR)/librocs.a $(OUTDIR)/wgen$(BINSUFFIX) $(OUTDIR)/ogen$(BINSUFFIX) $(OUTDIR)/xml2cstr$(BINSUFFIX) \
$(OUTDIR)/addlang$(BINSUFFIX) $(OUTDIR)/mergelang$(BINSUFFIX) $(OUTDIR)/mdoc$(BINSUFFIX) $(OUTDIR)/lang2po$(BINSUFFIX) $(OUTDIR)/po2lang$(BINSUFFIX)


$(OUTDIR)/librocs.a: $(OBJS) $(COREOBJS)
	$(AR) rcs $(OUTDIR)/librocs.a $(OBJS) $(COREOBJS)


$(OUTDIR)/wgen$(BINSUFFIX): $(TMPOUTDIR)/wgen.o $(OUTDIR)/librocs.a
	$(LD) $(LDFLAGS) -o $(OUTDIR)/wgen$(BINSUFFIX) $(TMPOUTDIR)/wgen.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)
	cp $(OUTDIR)/wgen$(BINSUFFIX) bin

$(OUTDIR)/ogen$(BINSUFFIX): $(TMPOUTDIR)/ogen.o $(OUTDIR)/librocs.a
	$(LD) $(LDFLAGS) -o $(OUTDIR)/ogen$(BINSUFFIX) $(TMPOUTDIR)/ogen.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)
	cp $(OUTDIR)/ogen$(BINSUFFIX) bin

$(OUTDIR)/xml2cstr$(BINSUFFIX): $(TMPOUTDIR)/xml2cstr.o $(OUTDIR)/librocs.a
	$(LD) $(LDFLAGS) -o $(OUTDIR)/xml2cstr$(BINSUFFIX) $(TMPOUTDIR)/xml2cstr.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)
	cp $(OUTDIR)/xml2cstr$(BINSUFFIX) bin

$(OUTDIR)/addlang$(BINSUFFIX): $(TMPOUTDIR)/addlang.o $(OUTDIR)/librocs.a
	$(LD) $(LDFLAGS) -o $(OUTDIR)/addlang$(BINSUFFIX) $(TMPOUTDIR)/addlang.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)/mergelang$(BINSUFFIX): $(TMPOUTDIR)/mergelang.o $(OUTDIR)/librocs.a
	$(LD) $(LDFLAGS) -o $(OUTDIR)/mergelang$(BINSUFFIX) $(TMPOUTDIR)/mergelang.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)/mdoc$(BINSUFFIX): $(TMPOUTDIR)/mdoc.o $(OUTDIR)/librocs.a
	$(LD) $(LDFLAGS) -o $(OUTDIR)/mdoc$(BINSUFFIX) $(TMPOUTDIR)/mdoc.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)/lang2po$(BINSUFFIX): $(TMPOUTDIR)/lang2po.o $(OUTDIR)/librocs.a
	$(LD) $(LDFLAGS) -o $(OUTDIR)/lang2po$(BINSUFFIX) $(TMPOUTDIR)/lang2po.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)/po2lang$(BINSUFFIX): $(TMPOUTDIR)/po2lang.o $(OUTDIR)/librocs.a
	$(LD) $(LDFLAGS) -o $(OUTDIR)/po2lang$(BINSUFFIX) $(TMPOUTDIR)/po2lang.o $(OUTDIR)/librocs.a $(LIBS) $(SSLLIBS)

$(TMPOUTDIR)/%.o: impl/%.c
	$(CC) $(CFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: gen/%.c
	$(CC) $(CFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: impl/$(DIRPREFIX)/%.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(OUTDIR)/librocs.a
	$(RM) $(OUTDIR)/wgen$(BINSUFFIX)
	$(RM) $(OUTDIR)/ogen$(BINSUFFIX)
	$(RM) $(OUTDIR)/xml2cstr$(BINSUFFIX)
	$(RM) $(OUTDIR)/addlang$(BINSUFFIX)
	$(RM) $(OUTDIR)/mergelang$(BINSUFFIX)
	$(RM) $(OUTDIR)/mdoc$(BINSUFFIX)
	$(RM) $(OUTDIR)/lang2po$(BINSUFFIX)
	$(RM) $(OUTDIR)/po2lang$(BINSUFFIX)
	$(RM) $(GENDIR)/rocs-gen.tar.gz
	$(RM) -r $(GENDIR)/rocs

fromtarprep:
	-mkdir -p $(OUTDIR)
	-mkdir -p $(TMPOUTDIR)
	-mkdir -p $(GENDIR)/rocs
	$(RM) $(TMPOUTDIR)/*.o
	$(RM) $(OUTDIR)/librocs.a
	cd $(GENDIR); \
	cp ../rocs/rocs-gen.tar.gz .; gzip -d rocs-gen.tar.gz; tar -xf rocs-gen.tar

install: ;

uninstall: ;

ssl: OPENSSL=-D__OPENSSL__
ssl: SSLLIBS=-lssl
ssl: all
