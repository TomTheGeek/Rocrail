#    Rocrail Copyright (C) 2002 - 2007 Rob Versluis 
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
PLATFORM=LINUX

DESTDIR?=/opt/rocrail
PREFIX?=/usr/local

offlineall: version
	cd rocrail; make rocrail TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL)

all: version
	cd rocrail; make rocrail TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL)

fromtar: version
	cd rocrail; make fromtar TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL)

release: version
	cd rocrail; make rocrail TOOLPREFIX=$(TOOLPREFIX) LIBSUFFIX=$(LIBSUFFIX) PLATFORM=$(PLATFORM) MINGWINSTALL=$(MINGWINSTALL) DEBUG=
	
install:
	cd rocrail; make install DESTDIR=$(DESTDIR) PREFIX=$(PREFIX) TOOLPREFIX=$(TOOLPREFIX) PLATFORM=$(PLATFORM);

version:
	echo "const int bzr = " >  common/version.h
	bzr revno >> common/version.h
	echo ";" >> common/version.h

svnversion:
	-svn log --limit 1 -q --xml https://rocrail.svn.sourceforge.net/svnroot/rocrail/Rocrail/trunk > common/version.xml

