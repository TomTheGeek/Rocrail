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
include makefile.common

PACKAGE=$(shell basename `pwd`)

ifdef VERSION
TARBALL=$(PACKAGE)-$(VERSION).tar.gz
else
TARBALL=$(PACKAGE).tar.gz
endif

SUBDIRS=rocs rocint rocrail rocdigs roclcdr rocgui rocpkg


all: online

online: version build

offline: build

generate:
	@for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@ || exit 2 ; done

build:
	@for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@  || exit 3 ; done

fromtarprep:
	$(MAKE)  -C rocs  $@

fromtar: fromtarprep all

release: DEBUG=
release: all

clean:
	@for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@  || exit 4 ; done
	$(RM) -r $(DIRPREFIX)gen

install:
	@for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@  || exit 5 ; done

uninstall:
	@for d in $(SUBDIRS) ; do $(MAKE) -C $$d $@  || exit 6 ; done
	$(RM) -r $(DESTDIR)$(ROCRAIL_LIBDIR)
	$(RM) -r $(DESTDIR)$(ROCRAIL_LIBEXECDIR)
	$(RM) -r $(DESTDIR)$(ROCRAIL_DATADIR)
	$(RM) -r $(DESTDIR)$(ROCRAIL_INFODIR)

archive: clean version
	-mkdir -p ../package
	cd .. && tar czf package/$(TARBALL)  $(PACKAGE)

version:
	echo "const int bzr = " >  common/version.h
	bzr revno >> common/version.h
	echo ";" >> common/version.h

svnversion:
	-svn log --limit 1 -q --xml https://rocrail.svn.sourceforge.net/svnroot/rocrail/Rocrail/trunk > common/version.xml
