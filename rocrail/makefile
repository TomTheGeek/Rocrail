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
	BINSUFFIX=.exe
else
	LIBS=-lpthread -ldl
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	BINSUFFIX=
endif

MOUNTPOINT=..
OUTDIR=../$(DIRPREFIX)bin
GENDIR=../$(DIRPREFIX)gen
TMPOUTDIR=$(GENDIR)/rocrail/bin
BUILDBINDIR=../rocs/bin
DEBUG=-g
DESTDIR?=
PREFIX?=/usr/local
BINDIR?=$(PREFIX)/bin
LIBDIR?=$(PREFIX)/lib
LIBEXECDIR?=$(PREFIX)/libexec
DATADIR?=$(PREFIX)/share
PIXMAPSDIR?=$(PREFIX)/share/pixmaps
APPLICATIONSDIR=$(PREFIX)/share/applications
ROCRAIL_LIBDIR=$(LIBDIR)/rocrail
ROCRAIL_LIBEXECDIR=$(LIBEXECDIR)/rocrail
ROCRAIL_DATADIR=$(DATADIR)/rocrail

CPP=$(TOOLPREFIX)gcc
LNK=$(TOOLPREFIX)gcc
INCL_PATH=$(MOUNTPOINT)
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -ansi -I$(INCL_PATH) -I$(GENDIR)
RRLIBS=$(OUTDIR)/libwrapper.a $(OUTDIR)/librocs.a $(OUTDIR)/librocrail.a

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))
WEBOBJS=$(patsubst impl/web/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/web/*.c))
WOBJS=$(patsubst $(GENDIR)/rocrail/wrapper/impl/%.c,$(GENDIR)/rocrail/wrapper/bin/%.o,$(wildcard $(GENDIR)/rocrail/wrapper/impl/*.c))

LIBOBJS=$(TMPOUTDIR)/rcon.o

TARGET=$(OUTDIR)/rocrail$(BINSUFFIX)

all: $(TARGET)

$(TARGET): $(WOBJS) $(OBJS) $(WEBOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(TARGET) $(OBJS) $(WEBOBJS) $(RRLIBS) $(LIBS)

# ------------------------------------------------------------
# The client connection archive.
# ------------------------------------------------------------
$(OUTDIR)/librocrail.a: $(LIBOBJS)
	$(TOOLPREFIX)ar rcs $(OUTDIR)/librocrail.a $(LIBOBJS)

# ------------------------------------------------------------
# The wrapper archive.
# ------------------------------------------------------------
$(OUTDIR)/libwrapper.a: $(WOBJS) $(OUTDIR)/librocs.a
	$(TOOLPREFIX)ar rcs $(OUTDIR)/libwrapper.a $(WOBJS) $(OUTDIR)/librocs.a

# ------------------------------------------------------------
# The RocRail objects.
# ------------------------------------------------------------
$(TMPOUTDIR)/%.o: impl/%.c $(GENDIR)/rocrail/wrapper/public/wrapper.h
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/%.o: impl/web/%.c $(GENDIR)/rocrail/wrapper/public/wrapper.h
	$(CPP) $(CC_FLAGS) $< -o $@

$(GENDIR)/rocrail/wrapper/bin/%.o: $(GENDIR)/rocrail/wrapper/impl/%.c
	$(CPP) $(CC_FLAGS) $< -o $@

clean:
	-mkdir -p $(OUTDIR)
	-mkdir -p $(TMPOUTDIR)
	-mkdir -p $(GENDIR)/rocrail
	-rm -f $(TARGET)
	-rm -f $(OUTDIR)/*.o
	-rm -f $(GENDIR)/rocrail/wrapper/public/*
	-rm -f $(GENDIR)/rocrail/wrapper/impl/*
	-rm -f $(GENDIR)/rocrail/wrapper/bin/*
	-rm -f $(GENDIR)/rocrail/wrapper/doc/*
	cd $(GENDIR)/rocrail; ../$(BUILDBINDIR)/wgen ../../rocrail/public/wrapper.xml
	cd $(GENDIR)/rocrail; ../$(BUILDBINDIR)/ogen ../../rocrail/rocrail.xml ../
	cd $(GENDIR)/rocrail; ../$(BUILDBINDIR)/xml2cstr ../../rocrail/public/wrapper.xml wrapper/impl/wrapperinfo.c wrapperinfo
	cd $(GENDIR)/rocrail; ../$(BUILDBINDIR)/xml2cstr wrapper/doc/wrapper-en.html wrapper/impl/rocrail_doc.c rocrail_doc
	cd $(GENDIR)/rocrail; ../$(BUILDBINDIR)/xml2cstr ../../common/version.xml wrapper/impl/version.c svnLog

# TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL)
rocrail:
	cd ../rocs; make clean PLATFORM=$(PLATFORM); make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL);
	cd ../rocint; make clean PLATFORM=$(PLATFORM); make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL);
	make clean PLATFORM=$(PLATFORM); make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL);
	cd ../rocdigs; make clean PLATFORM=$(PLATFORM); make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL);
	cd ../roclcdr; make clean PLATFORM=$(PLATFORM); make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL);
	cd ../rocgui; make clean PLATFORM=$(PLATFORM); make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL);

fromtar:
	cd ../rocs; make fromtar PLATFORM=$(PLATFORM); make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL);
	cd ../rocint; make clean PLATFORM=$(PLATFORM); make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL);
	make clean PLATFORM=$(PLATFORM); make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL);
	cd ../rocdigs; make clean PLATFORM=$(PLATFORM); make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL);
	cd ../roclcdr; make clean PLATFORM=$(PLATFORM); make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL);
	cd ../rocgui; make clean PLATFORM=$(PLATFORM); make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL);

install:
	-mkdir -p $(DESTDIR)$(BINDIR)
	-mkdir -p $(DESTDIR)$(ROCRAIL_LIBDIR)
	-mkdir -p $(DESTDIR)$(ROCRAIL_LIBEXECDIR)
	-cp -p $(OUTDIR)/rocrail$(BINSUFFIX) $(DESTDIR)$(ROCRAIL_LIBEXECDIR)
	cd ../rocdigs; make install DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) TOOLPREFIX=$(TOOLPREFIX) PLATFORM=$(PLATFORM);
	cd ../roclcdr; make install DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) TOOLPREFIX=$(TOOLPREFIX) PLATFORM=$(PLATFORM);
	cd ../rocgui; make install DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) TOOLPREFIX=$(TOOLPREFIX) PLATFORM=$(PLATFORM);
	-cp -p package/rocrail.sh $(DESTDIR)$(BINDIR)/rocrail
	-cp -p package/rocview.sh $(DESTDIR)$(BINDIR)/rocview
	-cp -p package/rocviewmod.sh $(DESTDIR)$(BINDIR)/rocviewmod
	-echo ROCRAIL_LIBDIR=$(ROCRAIL_LIBDIR) > $(DESTDIR)$(BINDIR)/rocprops.sh
	-echo ROCRAIL_LIBEXECDIR=$(ROCRAIL_LIBEXECDIR) >> $(DESTDIR)$(BINDIR)/rocprops.sh
	-echo ROCRAIL_DATADIR=$(ROCRAIL_DATADIR) >> $(DESTDIR)$(BINDIR)/rocprops.sh
	-chmod +x $(DESTDIR)$(BINDIR)/rocrail
	-chmod +x $(DESTDIR)$(BINDIR)/rocview
	-chmod +x $(DESTDIR)$(BINDIR)/rocviewmod
	-chmod +x $(DESTDIR)$(BINDIR)/rocprops.sh
	-mkdir -p $(DESTDIR)$(ROCRAIL_DATADIR)/default
	-cp -Rp ../stylesheets $(DESTDIR)$(ROCRAIL_DATADIR)
	-cp -Rp symbols $(DESTDIR)$(ROCRAIL_DATADIR)
	-cp -Rp package/images $(DESTDIR)$(ROCRAIL_DATADIR)
	-cp -p package/*.xml $(DESTDIR)$(ROCRAIL_DATADIR)/default
	-cp -p package/*.ini $(DESTDIR)$(ROCRAIL_DATADIR)/default
	-mkdir -p $(DESTDIR)$(PIXMAPSDIR)
	-cp -p rocraild.png $(DESTDIR)$(PIXMAPSDIR)
	-cp -p ../rocgui/xpm/rocrail.xpm $(DESTDIR)$(PIXMAPSDIR)
	-mkdir -p $(DESTDIR)$(APPLICATIONSDIR)
	-cp -p package/Rocrail.desktop $(DESTDIR)$(APPLICATIONSDIR)
	-cp -p package/Rocview.desktop $(DESTDIR)$(APPLICATIONSDIR)
