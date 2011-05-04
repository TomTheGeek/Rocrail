# ------------------------------------------------------------
# Rocs - OS independent C library
#
# Copyright (C) 2002-2010 - Rob Versluis <r.j.versluis@rocrail.net>
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
ARCH=
ifeq ($(PLATFORM),WIN32)
	LIBS=-liphlpapi -lmpr -lws2_32 -ladvapi32 -lwinmm
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
ifeq ($(PLATFORM),OPENBSD)
	LIBS=-pthread
endif

TOOLPREFIX=
SRCMOUNTPOINT=..
GENMOUNTPOINT=..$(FS)$(COREDIR)gen
OUTDIR=..$(FS)$(COREDIR)bin
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
AR_FLAGS=

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))

COREOBJS=$(patsubst impl/$(COREDIR)/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/$(COREDIR)/*.c))

all: $(OUTDIR)$(FS)librocs.a $(OUTDIR)$(FS)wgen$(BINSUFFIX) $(OUTDIR)$(FS)ogen$(BINSUFFIX) $(OUTDIR)$(FS)xml2cstr$(BINSUFFIX) $(OUTDIR)$(FS)png2wx$(BINSUFFIX) \
$(OUTDIR)$(FS)addlang$(BINSUFFIX) $(OUTDIR)$(FS)mergelang$(BINSUFFIX) $(OUTDIR)$(FS)mdoc$(BINSUFFIX) $(OUTDIR)$(FS)lang2po$(BINSUFFIX) $(OUTDIR)$(FS)po2lang$(BINSUFFIX) \
 $(OUTDIR)$(FS)keygen$(BINSUFFIX)

gen:
	bin$(FS)ogen$(GENSUFFIX) rocs.xml

$(OUTDIR)$(FS)librocs.a: $(OBJS) $(COREOBJS)
	$(AR) rcs $(OUTDIR)$(FS)librocs.a $(OBJS) $(COREOBJS)



$(OUTDIR)$(FS)wgen$(BINSUFFIX): $(TMPOUTDIR)$(FS)wgen.o $(OUTDIR)$(FS)librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)wgen$(BINSUFFIX) $(TMPOUTDIR)$(FS)wgen.o $(OUTDIR)$(FS)librocs.a $(LIBS) $(SSLLIBS)
	$(COPY) $(OUTDIR)$(FS)wgen$(BINSUFFIX) bin

$(OUTDIR)$(FS)ogen$(BINSUFFIX): $(TMPOUTDIR)$(FS)ogen.o $(OUTDIR)$(FS)librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)ogen$(BINSUFFIX) $(TMPOUTDIR)$(FS)ogen.o $(OUTDIR)$(FS)librocs.a $(LIBS) $(SSLLIBS)
	$(COPY) $(OUTDIR)$(FS)ogen$(BINSUFFIX) bin

$(OUTDIR)$(FS)xml2cstr$(BINSUFFIX): $(TMPOUTDIR)$(FS)xml2cstr.o $(OUTDIR)$(FS)librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)xml2cstr$(BINSUFFIX) $(TMPOUTDIR)$(FS)xml2cstr.o $(OUTDIR)$(FS)librocs.a $(LIBS) $(SSLLIBS)
	$(COPY) $(OUTDIR)$(FS)xml2cstr$(BINSUFFIX) bin

$(OUTDIR)$(FS)png2wx$(BINSUFFIX): $(TMPOUTDIR)$(FS)png2wx.o $(OUTDIR)$(FS)librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)png2wx$(BINSUFFIX) $(TMPOUTDIR)$(FS)png2wx.o $(OUTDIR)$(FS)librocs.a $(LIBS) $(SSLLIBS)
	$(COPY) $(OUTDIR)$(FS)png2wx$(BINSUFFIX) bin

$(OUTDIR)$(FS)addlang$(BINSUFFIX): $(TMPOUTDIR)$(FS)addlang.o $(OUTDIR)$(FS)librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)addlang$(BINSUFFIX) $(TMPOUTDIR)$(FS)addlang.o $(OUTDIR)$(FS)librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)$(FS)mergelang$(BINSUFFIX): $(TMPOUTDIR)$(FS)mergelang.o $(OUTDIR)$(FS)librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)mergelang$(BINSUFFIX) $(TMPOUTDIR)$(FS)mergelang.o $(OUTDIR)$(FS)librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)$(FS)mdoc$(BINSUFFIX): $(TMPOUTDIR)$(FS)mdoc.o $(OUTDIR)$(FS)librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)mdoc$(BINSUFFIX) $(TMPOUTDIR)$(FS)mdoc.o $(OUTDIR)$(FS)librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)$(FS)lang2po$(BINSUFFIX): $(TMPOUTDIR)$(FS)lang2po.o $(OUTDIR)$(FS)librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)lang2po$(BINSUFFIX) $(TMPOUTDIR)$(FS)lang2po.o $(OUTDIR)$(FS)librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)$(FS)po2lang$(BINSUFFIX): $(TMPOUTDIR)$(FS)po2lang.o $(OUTDIR)$(FS)librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)po2lang$(BINSUFFIX) $(TMPOUTDIR)$(FS)po2lang.o $(OUTDIR)$(FS)librocs.a $(LIBS) $(SSLLIBS)

$(OUTDIR)$(FS)keygen$(BINSUFFIX): $(TMPOUTDIR)$(FS)keygen.o $(OUTDIR)$(FS)librocs.a
	$(LNK) $(LNK_FLAGS) -o $(OUTDIR)$(FS)keygen$(BINSUFFIX) $(TMPOUTDIR)$(FS)keygen.o $(OUTDIR)$(FS)librocs.a $(LIBS) $(SSLLIBS)

$(TMPOUTDIR)/%.o: impl/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)$(CF)%.o: gen$(CF)%.c
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/%.o: impl/$(COREDIR)/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

clean:
	-$(MKDIR) $(OUTDIR)
	-$(MKDIR) $(TMPOUTDIR)
	-$(MKDIR) $(GENMOUNTPOINT)$(FS)rocs
	-$(REMOVE) $(TMPOUTDIR)$(FS)*.o
	-$(REMOVE) -f $(OUTDIR)$(FS)librocs.a
	cd $(GENMOUNTPOINT)$(FS)rocs$(CS) \
	..$(FS)..$(FS)rocs$(FS)bin$(FS)ogen$(GENSUFFIX) ..$(FS)..$(FS)rocs$(FS)rocs.xml
	-$(REMOVE) $(GENMOUNTPOINT)$(FS)rocs-gen.tar.gz
	cd $(GENMOUNTPOINT)$(CS) \
	tar -cf rocs-gen.tar rocs$(CS) gzip rocs-gen.tar$(CS) $(COPY) rocs-gen.tar.gz ..$(FS)rocs

fromtar:
	-$(MKDIR) $(OUTDIR)
	-$(MKDIR) $(TMPOUTDIR)
	-$(MKDIR) $(GENMOUNTPOINT)$(FS)rocs
	-$(REMOVE) $(TMPOUTDIR)$(FS)*.o
	-$(REMOVE) $(OUTDIR)$(FS)librocs.a
	cd $(GENMOUNTPOINT)$(CS) \
	$(COPY) ..$(FS)rocs$(FS)rocs-gen.tar.gz .$(CS) gzip -df rocs-gen.tar.gz$(CS) tar -xf rocs-gen.tar


ssl: OPENSSL=-D__OPENSSL__
ssl: SSLLIBS=-lssl
ssl: all
