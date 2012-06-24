#    Copyright (C) 2002-2012 Rob Versluis, Rocrail.net
#
#    Without an official permission commercial use is not permitted.
#    Forking this project is not permitted.  
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


# commandline for mingw cross compiling:
#   make clean
#   make TOOLPREFIX=i586-mingw32msvc- LIBSUFFIX=-i586-mingw32msvc PLATFORM=WIN32 MINGWINSTALL=/usr/i586-mingw32msvc all
#
# commandline for osx compiling:
#   make PLATFORM=MACOSX
#
FS=/
CS=;
COPY=cp
REMOVE=rm -f
GENSUFFIX=
QUOT="
MAKE ?= make
WXCONFIG=wx-config

ifeq ($(NATIVE),WIN32)
	FS=$\\
	CS=&
	COPY=copy
	REMOVE=del /Q
	GENSUFFIX=.exe
	QUOT=
endif



PLATFORM=LINUX
ARCH=

all: version
	cd rocrail$(CS) $(MAKE) rocrail TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) ARCH=$(ARCH) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL) WXCONFIG=$(WXCONFIG)

offlineall:
	cd rocrail$(CS) $(MAKE) rocrail TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) ARCH=$(ARCH) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL) WXCONFIG=$(WXCONFIG)

fromtar: version
	cd rocrail$(CS) $(MAKE) fromtar TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) ARCH=$(ARCH) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL) WXCONFIG=$(WXCONFIG)

release: version
	cd rocrail$(CS) $(MAKE) rocrail TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) ARCH=$(ARCH) NATIVE=$(NATIVE) MINGWINSTALL=$(MINGWINSTALL) WXCONFIG=$(WXCONFIG) DEBUG= 
	
install:
	cd rocrail$(CS) $(MAKE) install_all

uninstall:
	cd rocrail$(CS) $(MAKE) uninstall_all


version:
	echo $(QUOT)const int bzr = $(QUOT) > common$(FS)version.h
	bzr revno >> common$(FS)version.h
	echo $(QUOT);$(QUOT) >> common$(FS)version.h

