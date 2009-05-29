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
include ../makefile.common


all: build

generate: ;

build: generate

clean: ;

install:
	-mkdir -p $(DESTDIR)$(BINDIR)
	-cp -p ../rocrail/package/rocrail.sh $(DESTDIR)$(BINDIR)/rocrail
	-cp -p ../rocrail/package/rocview.sh $(DESTDIR)$(BINDIR)/rocview
	-cp -p ../rocrail/package/rocviewmod.sh $(DESTDIR)$(BINDIR)/rocviewmod
	-cp -p ../rocrail/package/rocinit.sh $(DESTDIR)$(BINDIR)/rocinit.sh
	-echo ROCRAIL_LIBDIR=$(ROCRAIL_LIBDIR) > $(DESTDIR)$(BINDIR)/rocprops.sh
	-echo ROCRAIL_LIBEXECDIR=$(ROCRAIL_LIBEXECDIR) >> $(DESTDIR)$(BINDIR)/rocprops.sh
	-echo ROCRAIL_DATADIR=$(ROCRAIL_DATADIR) >> $(DESTDIR)$(BINDIR)/rocprops.sh
	-echo ROCRAIL_HOME=$(ROCRAIL_HOME) >> $(DESTDIR)$(BINDIR)/rocprops.sh
	-chmod +x $(DESTDIR)$(BINDIR)/rocrail
	-chmod +x $(DESTDIR)$(BINDIR)/rocview
	-chmod +x $(DESTDIR)$(BINDIR)/rocviewmod
	-chmod +x $(DESTDIR)$(BINDIR)/rocinit.sh
	-chmod +x $(DESTDIR)$(BINDIR)/rocprops.sh

	-mkdir -p $(DESTDIR)$(ROCRAIL_DATADIR)/default
	-cp -Rp ../stylesheets $(DESTDIR)$(ROCRAIL_DATADIR)
	-cp -Rp ../rocrail/symbols $(DESTDIR)$(ROCRAIL_DATADIR)
	-cp -Rp ../rocrail/package/images $(DESTDIR)$(ROCRAIL_DATADIR)
	-cp -p ../rocrail/package/*.xml $(DESTDIR)$(ROCRAIL_DATADIR)/default
	-cp -p ../rocrail/package/*.ini $(DESTDIR)$(ROCRAIL_DATADIR)/default
	-cp -Rp ../rocgui/icons $(DESTDIR)$(ROCRAIL_DATADIR)
	-cp -Rp ../rocgui/svg $(DESTDIR)$(ROCRAIL_DATADIR)
	-cp -Rp ../rocgui/xpm $(DESTDIR)$(ROCRAIL_DATADIR)

	-mkdir -p $(DESTDIR)$(PIXMAPSDIR)
	-cp -p ../rocrail/rocraild.png $(DESTDIR)$(PIXMAPSDIR)
	-cp -p ../rocgui/xpm/rocrail.xpm $(DESTDIR)$(PIXMAPSDIR)
	
	-mkdir -p $(DESTDIR)$(ROCRAIL_INFODIR)
	-cp -p ../COPYING $(DESTDIR)$(ROCRAIL_INFODIR)

#
# Note: this installs icons according to the FreeDesktop standard.  How to handle KDE?
#
	-mkdir -p $(DESTDIR)$(DIRECTORIESDIR)
	-cp -p ../rocrail/package/Rocrail.directory $(DESTDIR)$(DIRECTORIESDIR)
	-mkdir -p $(DESTDIR)$(APPLICATIONSDIR)
	-cp -p ../rocrail/package/Rocrail.desktop $(DESTDIR)$(APPLICATIONSDIR)
	-cp -p ../rocrail/package/Rocview.desktop $(DESTDIR)$(APPLICATIONSDIR)

	-mkdir -p $(DESTDIR)/etc/xdg/menus/applications-merged
	-cp -p ../rocrail/package/Rocrail.menu $(DESTDIR)/etc/xdg/menus/applications-merged

uninstall:
	$(RM) $(DESTDIR)$(BINDIR)/rocrail
	$(RM) $(DESTDIR)$(BINDIR)/rocview
	$(RM) $(DESTDIR)$(BINDIR)/rocviewmod
	$(RM) $(DESTDIR)$(BINDIR)/rocinit.sh
	$(RM) $(DESTDIR)$(BINDIR)/rocprops.sh

	$(RM) -r $(DESTDIR)$(ROCRAIL_DATADIR)/stylesheets
	$(RM) -r $(DESTDIR)$(ROCRAIL_DATADIR)/symbols
	$(RM) -r $(DESTDIR)$(ROCRAIL_DATADIR)/images
	$(RM) -r $(DESTDIR)$(ROCRAIL_DATADIR)/icons
	$(RM) -r $(DESTDIR)$(ROCRAIL_DATADIR)/svg
	$(RM) -r $(DESTDIR)$(ROCRAIL_DATADIR)/xpm
	$(RM) $(DESTDIR)$(ROCRAIL_DATADIR)/default/*.xml
	$(RM) $(DESTDIR)$(ROCRAIL_DATADIR)/default/*.ini
	
	$(RM) -r $(DESTDIR)$(ROCRAIL_INFODIR)/COPYING

	$(RM) $(DESTDIR)$(DIRECTORIESDIR)/Rocrail.directory
	$(RM) $(DESTDIR)$(APPLICATIONSDIR)/Rocrail.desktop
	$(RM) $(DESTDIR)$(APPLICATIONSDIR)/Rocview.desktop
	$(RM) $(DESTDIR)/etc/xdg/menus/applications-merged/Rocrail.menu

	$(RM) $(DESTDIR)$(PIXMAPSDIR)/rocraild.png
	$(RM) $(DESTDIR)$(PIXMAPSDIR)/rocrail.xpm
