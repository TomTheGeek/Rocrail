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
TMPOUTDIR=$(GENDIR)$(FS)rocrail$(FS)bin
DEBUG=-g
DESTDIR=$(FS)opt$(FS)rocrail
BINDIR=..$(FS)rocs$(FS)bin

CPP=$(TOOLPREFIX)gcc
LNK=$(TOOLPREFIX)gcc
INCL_PATH=$(MOUNTPOINT)
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) $(ANSI) -I$(INCL_PATH) -I$(GENDIR)
LNK_FLAGS=
RRLIBS=$(OUTDIR)$(FS)libwrapper.a $(OUTDIR)$(FS)librocs.a $(OUTDIR)$(FS)librocutils.a $(OUTDIR)$(FS)librocrail.a

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))
WEBOBJS=$(patsubst impl/web/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/web/*.c))
WOBJS=$(patsubst $(GENDIR)/rocrail/wrapper/impl/%.c,$(GENDIR)/rocrail/wrapper/bin/%.o,$(wildcard $(GENDIR)/rocrail/wrapper/impl/*.c))

LIBOBJS=$(TMPOUTDIR)$(FS)rcon.o

TARGET=$(OUTDIR)$(FS)rocrail$(BINSUFFIX)

all: $(TARGET)

$(TARGET): $(WOBJS) $(OBJS) $(WEBOBJS) $(RRLIBS)
	$(LNK) $(LNK_FLAGS) -o $(TARGET) $(OBJS) $(WEBOBJS) $(RRLIBS) $(LIBS)

# ------------------------------------------------------------
# The client connection archive.
# ------------------------------------------------------------
$(OUTDIR)$(FS)librocrail.a: $(LIBOBJS)
	$(TOOLPREFIX)ar rcs $(OUTDIR)$(FS)librocrail.a $(LIBOBJS)

# ------------------------------------------------------------
# The wrapper archive.
# ------------------------------------------------------------
$(OUTDIR)$(FS)libwrapper.a: $(WOBJS) $(OUTDIR)$(FS)librocs.a
	$(TOOLPREFIX)ar rcs $(OUTDIR)$(FS)libwrapper.a $(WOBJS) $(OUTDIR)$(FS)librocs.a

# ------------------------------------------------------------
# The RocRail objects.
# ------------------------------------------------------------
$(TMPOUTDIR)/%.o: impl/%.c $(GENDIR)$(FS)rocrail$(FS)wrapper$(FS)public$(FS)wrapper.h
	$(CPP) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/%.o: impl/web/%.c $(GENDIR)$(FS)rocrail$(FS)wrapper$(FS)public$(FS)wrapper.h
	$(CPP) $(CC_FLAGS) $< -o $@

$(GENDIR)/rocrail/wrapper/bin/%.o: $(GENDIR)$(FS)rocrail$(FS)wrapper$(FS)impl$(FS)%.c
	$(CPP) $(CC_FLAGS) $< -o $@

clean:
	-$(MKDIR) $(OUTDIR)
	-$(MKDIR) $(TMPOUTDIR)
	-$(MKDIR) $(GENDIR)$(FS)rocrail
	-$(REMOVE) $(OUTDIR)$(FS)*.o
	-$(REMOVE) $(GENDIR)$(FS)rocrail$(FS)wrapper$(FS)public$(FS)*
	-$(REMOVE) $(GENDIR)$(FS)rocrail$(FS)wrapper$(FS)impl$(FS)*
	-$(REMOVE) $(GENDIR)$(FS)rocrail$(FS)wrapper$(FS)bin$(FS)*
	-$(REMOVE) $(GENDIR)$(FS)rocrail$(FS)wrapper$(FS)doc$(FS)*
	cd $(GENDIR)$(FS)rocrail$(CS) ..$(FS)$(BINDIR)$(FS)wgen$(GENSUFFIX) ..$(FS)..$(FS)rocrail$(FS)public$(FS)wrapper.xml
	cd $(GENDIR)$(FS)rocrail$(CS) ..$(FS)$(BINDIR)$(FS)ogen$(GENSUFFIX) ..$(FS)..$(FS)rocrail$(FS)rocrail.xml ..$(FS)
	cd $(GENDIR)$(FS)rocrail$(CS) ..$(FS)$(BINDIR)$(FS)xml2cstr$(GENSUFFIX) ..$(FS)..$(FS)rocrail$(FS)public$(FS)wrapper.xml wrapper$(FS)impl$(FS)wrapperinfo.c wrapperinfo
	cd $(GENDIR)$(FS)rocrail$(CS) ..$(FS)$(BINDIR)$(FS)xml2cstr$(GENSUFFIX) wrapper$(FS)doc$(FS)wrapper-en.html wrapper$(FS)impl$(FS)rocrail_doc.c rocrail_doc
	cd $(GENDIR)$(FS)rocrail$(CS) ..$(FS)$(BINDIR)$(FS)xml2cstr$(GENSUFFIX) ..$(FS)..$(FS)common$(FS)version.xml wrapper$(FS)impl$(FS)version.c svnLog

# TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL)
rocrail:
	cd ..$(FS)rocs$(CS) make clean PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	cd ..$(FS)rocutils$(CS) make clean PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	cd ..$(FS)rocint$(CS) make clean PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	make clean PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	cd ..$(FS)rocdigs$(CS) make clean PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	cd ..$(FS)roclcdr$(CS) make clean PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	cd ..$(FS)analyser$(CS) make clean PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	cd ..$(FS)rocview$(CS) make clean PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)

fromtar:
	cd ..$(FS)rocs$(CS) make fromtar PLATFORM=$(PLATFORM)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	cd ..$(FS)rocutils$(CS) make clean PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	cd ..$(FS)rocint$(CS) make clean PLATFORM=$(PLATFORM)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	make clean PLATFORM=$(PLATFORM)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	cd ..$(FS)rocdigs$(CS) make clean PLATFORM=$(PLATFORM)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	cd ..$(FS)roclcdr$(CS) make clean PLATFORM=$(PLATFORM)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	cd ..$(FS)analyser$(CS) make clean PLATFORM=$(PLATFORM)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)
	cd ..$(FS)rocview$(CS) make clean PLATFORM=$(PLATFORM)$(CS) make all DEBUG=$(DEBUG) TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL)


# for unix based systems only
install_all:
	-rm -Rf $(DESTDIR)
#	-mkdir -p ~$(FS)rocrail$(FS)locxpm
	-mkdir -p $(DESTDIR)
	-mkdir -p $(DESTDIR)
	-cp -p $(OUTDIR)$(FS)rocrail$(BINSUFFIX) $(DESTDIR)
	cd ..$(FS)rocdigs$(CS) make install TOOLPREFIX=$(TOOLPREFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)
	cd ..$(FS)roclcdr$(CS) make install TOOLPREFIX=$(TOOLPREFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)
	cd ..$(FS)analyser$(CS) make install TOOLPREFIX=$(TOOLPREFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)
	cd ..$(FS)rocview$(CS) make install TOOLPREFIX=$(TOOLPREFIX) PLATFORM=$(PLATFORM) NATIVE=$(NATIVE)
	-cp -p rocraild.png $(DESTDIR)
	-cp -p ..$(FS)rocview$(FS)xpm$(FS)rocrail.xpm $(DESTDIR)
	-cp -Rp ..$(FS)rocview$(FS)icons $(DESTDIR)
	-cp -Rp ..$(FS)rocview$(FS)svg $(DESTDIR)
	-cp -Rpu symbols $(DESTDIR)
	-cp -Rpu ..$(FS)stylesheets $(DESTDIR)
	-cp -Rpu package$(FS)images $(DESTDIR)
	-cp -pu package$(FS)plan.xml $(DESTDIR)
	-cp -p package$(FS)rocrail.sh $(DESTDIR)
	-cp -p package$(FS)rocview.sh $(DESTDIR)
	-chmod +x $(DESTDIR)$(FS)rocrail.sh
	-chmod +x $(DESTDIR)$(FS)rocview.sh
	-cp -p package$(FS)Rocrail.desktop $(FS)usr$(FS)share$(FS)applications
	-cp -p package$(FS)Rocview.desktop $(FS)usr$(FS)share$(FS)applications
	-cp -p package$(FS)Rocrail.directory $(FS)usr$(FS)share$(FS)desktop-directories
#	-chmod -R go+rw ~$(FS)rocrail

uninstall_all:
	-$(REMOVE) -Rf $(DESTDIR)
	-$(REMOVE) $(FS)usr$(FS)share$(FS)applications$(FS)Rocrail.desktop
	-$(REMOVE) $(FS)usr$(FS)share$(FS)applications$(FS)Rocview.desktop
	-$(REMOVE) $(FS)usr$(FS)share$(FS)desktop-directories$(FS)Rocrail.directory

install:
	-$(COPY) -p $(OUTDIR)$(FS)rocrail$(BINSUFFIX) $(DESTDIR)


