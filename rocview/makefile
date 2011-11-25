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
#   make TOOLPREFIX=i586-mingw32msvc- LIBSUFFIX=-i586-mingw32msvc PLATFORM=WIN32 MINGWINSTALL=/usr/i586-mingw32msvc all
#
# commandline for mingw cross compiling:
#   make PLATFORM=WIN32 NATIVE=WIN32 clean
#   make TOOLPREFIX=i586-mingw32msvc- LIBSUFFIX=-i586-mingw32msvc PLATFORM=WIN32 NATIVE=WIN32 MINGWINSTALL=/usr/i586-mingw32msvc all
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
WXSUBINCL=/wx-2.8
WXMSVCINCL=
WXCONFIG=wx-config
wxversion=2.8
LINKTOOL=c++

ifeq ($(NATIVE),WIN32)
	FS=$\\
	CS=&
	COPY=copy
	REMOVE=del /Q
	GENSUFFIX=.exe
	MKDIR=mkdir
	WXSUBINCL=
	WXMSVCINCL=-I$(MINGWINSTALL)$(FS)include$(FS)msvc
	LINKTOOL=g++
	WINLIBS=-liphlpapi -lmpr -lws2_32 -ladvapi32 -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32 -lcomctl32 -lrpcrt4 -lwinmm
endif





PLATFORM=LINUX
ARCH=
LIBRARY=unicode
LIBEXT=u
MINGWINSTALL=$(FS)usr$(FS)i586-mingw32msvc


ifeq ($(PLATFORM),WIN32)
	LIBS=-liphlpapi -lmpr
	LNK_FLAGS=`$(FS)usr$(FS)i586-mingw32msvc$(FS)bin$(FS)wx-config --libs` -mthreads
	WX_FLAGS=`$(MINGWINSTALL)$(FS)bin$(FS)wx-config --cflags`
	ifeq ($(NATIVE),WIN32)
		LNK_FLAGS=-L$(MINGWINSTALL)\lib-$(LIBRARY) -mthreads -Wl,--subsystem,windows -mwindows -lwx_msw$(LIBEXT)_html-$(wxversion) -lwx_msw$(LIBEXT)_core-$(wxversion) -lwx_base$(LIBEXT)-$(wxversion) -lwxtiff-$(wxversion) -lwxjpeg-$(wxversion) -lwxpng-$(wxversion) -lwxzlib-$(wxversion) -lwxregex$(LIBEXT)-$(wxversion) -lwxexpat-$(wxversion) -lwx_msw$(LIBEXT)_adv-$(wxversion) -lwx_msw$(LIBEXT)_qa-$(wxversion) -lwx_base$(LIBEXT)_xml-$(wxversion) -lwx_base$(LIBEXT)_net-$(wxversion)
		WX_FLAGS=-c $(DEBUG) -D__WXMSW__ -O2 -fno-strict-aliasing -mthreads -mwindows -DWX_PRECOMP -DNO_GCC_PRAGMA
	endif
	DIRPREFIX=win
	CC_EXTRA_FLAGS=-mthreads
	BINSUFFIX=.exe
	LIBSUFFIX=-i586-mingw32msvc
	WINRESOURCE=$(TMPOUTDIR)$(FS)rocview_rc.o
	WX_INCL=-I$(MINGWINSTALL)$(FS)include$(WXSUBINCL)
else ifeq ($(PLATFORM),WIN64)
	LIBS=-liphlpapi -lmpr -lmswsock -lws2_32 -ladvapi32
	LNK_FLAGS=`$(FS)usr$(FS)amd64-mingw32msvc$(FS)bin$(FS)wx-config --libs`
	WX_FLAGS=`$(MINGWINSTALL)$(FS)bin$(FS)wx-config --cflags`
	DIRPREFIX=win
	CC_EXTRA_FLAGS=
	BINSUFFIX=.exe
	LIBSUFFIX=-amd64-mingw32msvc
	WINRESOURCE=$(TMPOUTDIR)$(FS)rocview_rc.o
	WX_INCL=-I$(MINGWINSTALL)$(FS)include$(WXSUBINCL)
else ifeq ($(PLATFORM),OPENBSD)
	LIBS=-pthread
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	BINSUFFIX=
	LNK_FLAGS=`$(WXCONFIG) --libs`
	WX_FLAGS=`$(WXCONFIG) --cflags`
else
	LIBS=-lpthread -ldl
	DIRPREFIX=unx
	CC_EXTRA_FLAGS=-fPIC
	BINSUFFIX=
	LNK_FLAGS=`$(WXCONFIG) --libs`
	WX_FLAGS=`$(WXCONFIG) --cflags`
endif

MOUNTPOINT=..
OUTDIR=..$(FS)$(DIRPREFIX)bin
GENDIR=..$(FS)$(DIRPREFIX)gen
TMPOUTDIR=$(GENDIR)$(FS)rocview$(FS)bin
DEBUG=-g
DESTDIR=$(FS)opt$(FS)rocrail
BINDIR=..$(FS)rocs$(FS)bin

CPP=$(TOOLPREFIX)c++
CC=$(TOOLPREFIX)gcc
LNK=$(TOOLPREFIX)$(LINKTOOL)
INCL_PATH=$(MOUNTPOINT)
CC_FLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) -I$(INCL_PATH) -I$(GENDIR)

CC_WXFLAGS=-c $(CC_EXTRA_FLAGS) $(DEBUG) $(WX_FLAGS) $(WX_INCL) -I$(INCL_PATH) -I$(GENDIR)
RRLIBS=$(OUTDIR)$(FS)librocrail.a $(OUTDIR)$(FS)libwrapper.a $(OUTDIR)$(FS)librocs.a

SRCS=impl$(FS)guiapp.cpp impl$(FS)item.cpp impl$(FS)locdlg.cpp impl$(FS)planpanel.cpp impl$(FS)propdlg.cpp impl$(FS)swdlg.cpp impl$(FS)streetdlg.cpp impl$(FS)bktext.cpp

OBJS=$(patsubst impl/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard impl/*.cpp))
DOBJS=$(patsubst dialogs/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard dialogs/*.cpp))
CDOBJS=$(patsubst dialogs/controllers/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard dialogs/controllers/*.cpp))
DECOBJS=$(patsubst dialogs/decoders/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard dialogs/decoders/*.cpp))
SOBJS=$(patsubst symbols/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard symbols/*.cpp))
WOBJS=$(patsubst $(GENDIR)/rocview/wrapper/impl/%.c,$(GENDIR)/rocview/wrapper/bin/%.o,$(wildcard $(GENDIR)/rocview/wrapper/impl/*.c))
ROBJS=$(patsubst $(GENDIR)/rocview/res/%.c,$(TMPOUTDIR)/%.o,$(wildcard $(GENDIR)/rocview/res/*.c))
IOBJS=$(patsubst $(GENDIR)/rocview/res/%.cpp,$(TMPOUTDIR)/%.o,$(wildcard $(GENDIR)/rocview/res/*.cpp))

TARGET=$(OUTDIR)$(FS)rocview$(BINSUFFIX)

all: $(TARGET)

$(TARGET): $(ROBJS) $(IOBJS) $(WOBJS) $(DOBJS) $(CDOBJS) $(DECOBJS) $(SOBJS) $(OBJS) $(RRLIBS) $(WINRESOURCE)
	$(LNK) -o $(TARGET) $(ROBJS) $(IOBJS) $(WOBJS) $(DOBJS) $(CDOBJS) $(DECOBJS) $(SOBJS) $(OBJS) $(WINRESOURCE) $(RRLIBS) $(LNK_FLAGS) $(WX_LIBS) $(LIBS) $(WINLIBS)

$(WINRESOURCE): rocview.rc
	$(TOOLPREFIX)windres -irocview.rc -o$(WINRESOURCE)


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

$(GENDIR)/rocview/wrapper/bin/%.o: $(GENDIR)/rocview/wrapper/impl/%.c
	$(CC) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/%.o: $(GENDIR)/rocview/res/%.c
	$(CC) $(CC_FLAGS) $< -o $@

$(TMPOUTDIR)/%.o: $(GENDIR)/rocview/res/%.cpp
	$(CPP) $(CC_WXFLAGS) $< -o $@

clean:
	-$(MKDIR) $(OUTDIR)
	-$(MKDIR) $(GENDIR)$(FS)rocview
	-$(MKDIR) $(GENDIR)$(FS)rocview$(FS)res
	-$(MKDIR) $(TMPOUTDIR)
	-$(REMOVE) $(TARGET)
	-$(REMOVE) $(OUTDIR)$(FS)rocview$(BINSUFFIX)
	-$(REMOVE) $(TMPOUTDIR)$(FS)*.o
	-$(REMOVE) $(GENDIR)$(FS)rocview$(FS)wrapper$(FS)public$(FS)*
	-$(REMOVE) $(GENDIR)$(FS)rocview$(FS)wrapper$(FS)impl$(FS)*
	-$(REMOVE) $(GENDIR)$(FS)rocview$(FS)wrapper$(FS)bin$(FS)*
	-$(REMOVE) $(GENDIR)$(FS)rocview$(FS)wrapper$(FS)doc$(FS)*
	cd $(GENDIR)$(FS)rocview$(CS) ..$(FS)$(BINDIR)$(FS)wgen$(GENSUFFIX) ..$(FS)..$(FS)rocview$(FS)public$(FS)wrapper.xml
	cd $(GENDIR)$(FS)rocview$(CS) ..$(FS)$(BINDIR)$(FS)xml2cstr$(GENSUFFIX) ..$(FS)..$(FS)rocview$(FS)res$(FS)messages.xml res$(FS)messages.c messages
	cd $(GENDIR)$(FS)rocview$(CS) ..$(FS)$(BINDIR)$(FS)png2wx$(GENSUFFIX) ..$(FS)..$(FS)rocview$(FS)icons res$(FS)icons
	cd $(GENDIR)$(FS)rocview$(CS) ..$(FS)$(BINDIR)$(FS)xml2cstr$(GENSUFFIX) ..$(FS)..$(FS)rocview$(FS)public$(FS)wrapper.xml wrapper$(FS)impl$(FS)guiwrapperinfo.c guiwrapperinfo
	cd $(GENDIR)$(FS)rocview$(CS) ..$(FS)$(BINDIR)$(FS)xml2cstr$(GENSUFFIX) ..$(FS)..$(FS)common$(FS)version.xml wrapper$(FS)impl$(FS)version.c svnLog

wrapper:
	cd $(GENDIR)$(FS)rocview$(CS) ..$(FS)$(BINDIR)$(FS)wgen$(GENSUFFIX) ..$(FS)..$(FS)rocview$(FS)public$(FS)wrapper.xml

messages:
	cd $(GENDIR)$(FS)rocview$(CS) ..$(FS)$(BINDIR)$(FS)xml2cstr$(GENSUFFIX) ..$(FS)..$(FS)rocview$(FS)res$(FS)messages.xml res$(FS)messages.c messages
	cd $(GENDIR)$(FS)rocview$(CS) ..$(FS)$(BINDIR)$(FS)xml2cstr$(GENSUFFIX) ..$(FS)..$(FS)rocview$(FS)public$(FS)wrapper.xml wrapper$(FS)impl$(FS)guiwrapperinfo.c guiwrapperinfo
	cd $(GENDIR)$(FS)rocview$(CS) ..$(FS)$(BINDIR)$(FS)xml2cstr$(GENSUFFIX) ..$(FS)..$(FS)common$(FS)version.xml wrapper$(FS)impl$(FS)version.c svnLog
	cd $(GENDIR)$(FS)rocview$(CS) ..$(FS)$(BINDIR)$(FS)png2wx$(GENSUFFIX) ..$(FS)..$(FS)rocview$(FS)icons res$(FS)icons

install:
	-cp -p $(OUTDIR)$(FS)rocview $(DESTDIR)
	
macapp:
	rm -rf Rocrail.app
	-mkdir Rocrail.app
	-mkdir Rocrail.app/Contents
	-mkdir Rocrail.app/Contents/MacOS
	-mkdir Rocrail.app/Contents/Resources
	-mkdir Rocrail.app/Contents/Resources/icons
	cp xpm/rocrail.icns Rocrail.app/Contents/Resources/rocrail.icns
	cp Info.plist Rocrail.app/Contents/	
	echo -n 'APPL????' > Rocrail.app/Contents/PkgInfo
	cp ../unxbin/rocview Rocrail.app/Contents/MacOS/rocview
	cp ../unxbin/rocrail Rocrail.app/Contents/MacOS/rocrail
	cp ../unxbin/*.dylib Rocrail.app/Contents/MacOS
	cp Run Rocrail.app/Contents/MacOS/
	-cp /usr/lib/libwx* Rocrail.app/Contents/MacOS/
	-cp /usr/local/lib/libwx* Rocrail.app/Contents/MacOS/
	rm -rf ../unxbin/Rocrail.app
	mv -f Rocrail.app ../unxbin
	
	
