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
include ../makefile.common

LIBRARY=unicode
LIBEXT=u
MINGWINSTALL=/usr/i586-mingw32msvc
ANSIINSTALL=$(MINGWINSTALL)
ANSILIBS=

ifeq ($(PLATFORM),WIN32)
	ifeq ($(LIBRARY),ansi)
		LIBEXT=
		LIBS+= -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lcomctl32 -lrpcrt4 -lwsock32
		LDXXFLAGS=-L$(ANSILIBS) -mthreads -Wl,--subsystem,windows -mwindows
		WX_FLAGS=-I$(ANSIINSTALL)/include -I$(ANSIINSTALL)/lib/wx/include/i586-mingw32msvc-msw-ansi-release-static-2.8  -I$(ANSIINSTALL)/lib/wx/include/msw-ansi-release-static-2.8 -I$(ANSIINSTALL)/include/wx-2.8 -D__WXMSW__ -mthreads -DNO_GCC_PRAGMA
		WX_LIBS=-lwx_msw$(LIBEXT)_html-2.8$(LIBSUFFIX) -lwx_msw$(LIBEXT)_core-2.8$(LIBSUFFIX) -lwx_base$(LIBEXT)-2.8$(LIBSUFFIX) -lwxtiff-2.8$(LIBSUFFIX) -lwxjpeg-2.8$(LIBSUFFIX) -lwxpng-2.8$(LIBSUFFIX) -lwxzlib-2.8$(LIBSUFFIX) -lwxregex$(LIBEXT)-2.8$(LIBSUFFIX) -lwxexpat-2.8$(LIBSUFFIX) -lwx_msw$(LIBEXT)_adv-2.8$(LIBSUFFIX) -lwx_msw$(LIBEXT)_qa-2.8$(LIBSUFFIX) -lwx_base$(LIBEXT)_xml-2.8$(LIBSUFFIX) -lwx_base$(LIBEXT)_net-2.8$(LIBSUFFIX)
	else
		LDXXFLAGS=`/usr/i586-mingw32msvc/bin/wx-config --libs`
		WX_FLAGS=`$(MINGWINSTALL)/bin/wx-config --cflags`
	endif
	LIBSUFFIX=-i586-mingw32msvc
	WINRESOURCE=$(TMPOUTDIR)\rocgui_rc.o
	WX_INCL=-I$(MINGWINSTALL)/include/wx-2.8
else
	LDXXFLAGS=`wx-config --libs`
	WX_FLAGS=`wx-config --cflags`
endif

TMPOUTDIR=$(GENDIR)/rocgui/bin

CC_WXFLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) $(WX_FLAGS) $(WX_INCL) -I$(INCL_PATH) -I$(GENDIR)
RRLIBS=$(OUTDIR)/librocrail.a $(OUTDIR)/libwrapper.a $(OUTDIR)/librocs.a
VIEWLIBS=$(TMPOUTDIR)/libviewwrapper.a $(TMPOUTDIR)/libviewres.a

SRCS=impl/guiapp.cpp impl/item.cpp impl/locdlg.cpp impl/planpanel.cpp impl/propdlg.cpp impl/swdlg.cpp impl/streetdlg.cpp impl/bktext.cpp

OBJS=$(patsubst impl/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard impl/*.cpp))
DOBJS=$(patsubst dialogs/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard dialogs/*.cpp))
CDOBJS=$(patsubst dialogs/controllers/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard dialogs/controllers/*.cpp))
DECOBJS=$(patsubst dialogs/decoders/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard dialogs/decoders/*.cpp))
SOBJS=$(patsubst symbols/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard symbols/*.cpp))

TARGET=$(OUTDIR)/rocview$(BINSUFFIX)

all: build

generate: $(GENDIR)/rocgui/wrapper

$(GENDIR)/rocgui/wrapper: $(GENDIR)/rocgui public/wrapper.xml res/messages.xml ../common/version.xml
	$(RM) -r $(GENDIR)/rocgui/wrapper
	cd $(GENDIR)/rocgui; ../$(BUILDBINDIR)/wgen ../../rocgui/public/wrapper.xml
	cd $(GENDIR)/rocgui; ../$(BUILDBINDIR)/xml2cstr ../../rocgui/res/messages.xml res/messages.c messages
	cd $(GENDIR)/rocgui; ../$(BUILDBINDIR)/xml2cstr ../../rocgui/public/wrapper.xml wrapper/impl/guiwrapperinfo.c guiwrapperinfo
	cd $(GENDIR)/rocgui; ../$(BUILDBINDIR)/xml2cstr ../../common/version.xml wrapper/impl/version.c svnLog

$(GENDIR)/rocgui:
	$(RM) -r $(GENDIR)/rocgui
	-mkdir -p $(OUTDIR)
	-mkdir -p $(GENDIR)/rocgui
	-mkdir -p $(GENDIR)/rocgui/res
	-mkdir -p $(TMPOUTDIR)

build: generate $(TARGET)

$(TARGET): $(DOBJS) $(CDOBJS) $(DECOBJS) $(SOBJS) $(OBJS) $(VIEWLIBS) $(RRLIBS) $(WINRESOURCE)
	$(LDXX) -o $(TARGET) $(DOBJS) $(CDOBJS) $(DECOBJS) $(SOBJS) $(OBJS) $(VIEWLIBS) $(RRLIBS) $(WINRESOURCE) $(LDXXFLAGS) $(WX_LIBS) $(LIBS)

$(WINRESOURCE): rocgui.rc
	$(TOOLPREFIX)windres -irocgui.rc -o$(WINRESOURCE)

$(TMPOUTDIR)/%.o: impl/%.cpp
	$(CXX) $(CC_WXFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: dialogs/%.cpp
	$(CXX) $(CC_WXFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: dialogs/controllers/%.cpp
	$(CXX) $(CC_WXFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: dialogs/decoders/%.cpp
	$(CXX) $(CC_WXFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: symbols/%.cpp
	$(CXX) $(CC_WXFLAGS) $< -o $@

$(TMPOUTDIR)/libviewwrapper.a: generate
	$(MAKE) -f makefile.gen $@

$(TMPOUTDIR)/libviewres.a: generate
	$(MAKE) -f makefile.gen $@

clean:
	$(RM) $(TARGET)
	$(RM) $(OUTDIR)/rocgui$(BINSUFFIX)
	$(RM) -r $(GENDIR)/rocgui

install:
	-mkdir -p $(DESTDIR)$(ROCRAIL_LIBEXECDIR)
	-cp -p $(OUTDIR)/rocview $(DESTDIR)$(ROCRAIL_LIBEXECDIR)

uninstall:
	$(RM) $(DESTDIR)$(ROCRAIL_LIBEXECDIR)/rocview
