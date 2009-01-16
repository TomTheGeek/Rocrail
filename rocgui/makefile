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
#   make clean
#   make TOOLPREFIX=i586-mingw32msvc- LIBSUFFIX=-i586-mingw32msvc PLATFORM=WIN32 MINGWINSTALL=/usr/i586-mingw32msvc all
#
#   ansi libs are installed locally:
#   make TOOLPREFIX=i586-mingw32msvc- LIBSUFFIX= PLATFORM=WIN32 MINGWINSTALL=/usr/i586-mingw32msvc LIBRARY=ansi ANSILIBS=/home/rob/wx-2.8.3-mingw/lib-ansi ANSIINSTALL=/home/rob/wx-2.8.3-mingw all
#
PLATFORM=LINUX
LIBRARY=unicode
LIBEXT=u
MINGWINSTALL=/usr/i586-mingw32msvc
ANSIINSTALL=$(MINGWINSTALL)
ANSILIBS=

ifeq ($(PLATFORM),WIN32)
	ifeq ($(LIBRARY),ansi)
		LIBEXT=
		LIBS=-liphlpapi -lmpr -lwsock32 -ladvapi32 -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lcomctl32 -lrpcrt4 -lwsock32 -lwinmm
		LNK_FLAGS=-L$(ANSILIBS) -mthreads -Wl,--subsystem,windows -mwindows
		WX_FLAGS=-I$(ANSIINSTALL)/include -I$(ANSIINSTALL)/lib/wx/include/i586-mingw32msvc-msw-ansi-release-static-2.8  -I$(ANSIINSTALL)/lib/wx/include/msw-ansi-release-static-2.8 -I$(ANSIINSTALL)/include/wx-2.8 -D__WXMSW__ -mthreads -DNO_GCC_PRAGMA
		WX_LIBS=-lwx_msw$(LIBEXT)_html-2.8$(LIBSUFFIX) -lwx_msw$(LIBEXT)_core-2.8$(LIBSUFFIX) -lwx_base$(LIBEXT)-2.8$(LIBSUFFIX) -lwxtiff-2.8$(LIBSUFFIX) -lwxjpeg-2.8$(LIBSUFFIX) -lwxpng-2.8$(LIBSUFFIX) -lwxzlib-2.8$(LIBSUFFIX) -lwxregex$(LIBEXT)-2.8$(LIBSUFFIX) -lwxexpat-2.8$(LIBSUFFIX) -lwx_msw$(LIBEXT)_adv-2.8$(LIBSUFFIX) -lwx_msw$(LIBEXT)_qa-2.8$(LIBSUFFIX) -lwx_base$(LIBEXT)_xml-2.8$(LIBSUFFIX) -lwx_base$(LIBEXT)_net-2.8$(LIBSUFFIX)
	else
		LIBS=-liphlpapi -lmpr
		LNK_FLAGS=`/usr/i586-mingw32msvc/bin/wx-config --libs`
		WX_FLAGS=`$(MINGWINSTALL)/bin/wx-config --cflags`
	endif
	DIRPREFIX=win
	CC_EXTRA_FLAGS=
	BINSUFFIX=.exe
	LIBSUFFIX=-i586-mingw32msvc
	WINRESOURCE=$(TMPOUTDIR)\rocgui_rc.o
	WX_INCL=-I$(MINGWINSTALL)/include/wx-2.8
else
	LIBS=-lpthread -ldl
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	BINSUFFIX=
	LNK_FLAGS=`wx-config --libs`
	WX_FLAGS=`wx-config --cflags`
endif

MOUNTPOINT=..
OUTDIR=../$(DIRPREFIX)bin
GENDIR=../$(DIRPREFIX)gen
TMPOUTDIR=$(GENDIR)/rocgui/bin
DEBUG=-g
DESTDIR=/opt/rocrail
BINDIR=../rocs/bin

CPP=$(TOOLPREFIX)c++
CC=$(TOOLPREFIX)gcc
LNK=$(TOOLPREFIX)c++
INCL_PATH=$(MOUNTPOINT)
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -I$(INCL_PATH) -I$(GENDIR)

CC_WXFLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) $(WX_FLAGS) $(WX_INCL) -I$(INCL_PATH) -I$(GENDIR)
RRLIBS=$(OUTDIR)/librocrail.a $(OUTDIR)/libwrapper.a $(OUTDIR)/librocs.a

SRCS=impl/guiapp.cpp impl/item.cpp impl/locdlg.cpp impl/planpanel.cpp impl/propdlg.cpp impl/swdlg.cpp impl/streetdlg.cpp impl/bktext.cpp

OBJS=$(patsubst impl/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard impl/*.cpp))
DOBJS=$(patsubst dialogs/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard dialogs/*.cpp))
CDOBJS=$(patsubst dialogs/controllers/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard dialogs/controllers/*.cpp))
DECOBJS=$(patsubst dialogs/decoders/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard dialogs/decoders/*.cpp))
SOBJS=$(patsubst symbols/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard symbols/*.cpp))
WOBJS=$(patsubst $(GENDIR)/rocgui/wrapper/impl/%.c,$(GENDIR)/rocgui/wrapper/bin/%.o,$(wildcard $(GENDIR)/rocgui/wrapper/impl/*.c))
ROBJS=$(patsubst $(GENDIR)/rocgui/res/%.c,$(TMPOUTDIR)/%.o,$(wildcard $(GENDIR)/rocgui/res/*.c))

TARGET=$(OUTDIR)/rocgui$(BINSUFFIX)

all: $(TARGET)

$(TARGET): $(ROBJS) $(WOBJS) $(DOBJS) $(CDOBJS) $(DECOBJS) $(SOBJS) $(OBJS) $(RRLIBS) $(WINRESOURCE)
	$(LNK) -o $(TARGET) $(ROBJS) $(WOBJS) $(DOBJS) $(CDOBJS) $(DECOBJS) $(SOBJS) $(OBJS) $(WINRESOURCE) $(RRLIBS) $(LNK_FLAGS) $(WX_LIBS) $(LIBS)

$(WINRESOURCE): rocgui.rc
	$(TOOLPREFIX)windres -irocgui.rc -o$(WINRESOURCE)

$(TMPOUTDIR)/%.o: impl/%.cpp
	$(CPP) $(CC_WXFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: dialogs/%.cpp
	$(CPP) $(CC_WXFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: dialogs/controllers/%.cpp
	$(CPP) $(CC_WXFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: dialogs/decoders/%.cpp
	$(CPP) $(CC_WXFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: symbols/%.cpp
	$(CPP) $(CC_WXFLAGS) $< -o $@

$(GENDIR)/rocgui/wrapper/bin/%.o: $(GENDIR)/rocgui/wrapper/impl/%.c
	$(CC) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/%.o: $(GENDIR)/rocgui/res/%.c
	$(CC) $(CC_FLAGS) $< -o $@

clean:
	-mkdir -p $(OUTDIR)
	-mkdir -p $(GENDIR)/rocgui
	-mkdir -p $(GENDIR)/rocgui/res
	-mkdir -p $(TMPOUTDIR)
	-rm -f $(TMPOUTDIR)/*.o
	-rm -f $(GENDIR)/rocgui/wrapper/public/*
	-rm -f $(GENDIR)/rocgui/wrapper/impl/*
	-rm -f $(GENDIR)/rocgui/wrapper/bin/*
	-rm -f $(GENDIR)/rocgui/wrapper/doc/*
	cd $(GENDIR)/rocgui; ../$(BINDIR)/wgen ../../rocgui/public/wrapper.xml
	cd $(GENDIR)/rocgui; ../$(BINDIR)/xml2cstr ../../rocgui/res/messages.xml res/messages.c messages
	cd $(GENDIR)/rocgui; ../$(BINDIR)/xml2cstr ../../rocgui/public/wrapper.xml wrapper/impl/guiwrapperinfo.c guiwrapperinfo
	cd $(GENDIR)/rocgui; ../$(BINDIR)/xml2cstr ../../common/version.xml wrapper/impl/version.c svnLog

messages:
	cd $(GENDIR)/rocgui; ../$(BINDIR)/xml2cstr ../../rocgui/res/messages.xml res/messages.c messages
	cd $(GENDIR)/rocgui; ../$(BINDIR)/xml2cstr ../../rocgui/public/wrapper.xml wrapper/impl/guiwrapperinfo.c guiwrapperinfo
	cd $(GENDIR)/rocgui; ../$(BINDIR)/xml2cstr ../../common/version.xml wrapper/impl/version.c svnLog

install:
	-cp -p $(OUTDIR)/rocgui $(DESTDIR)
	