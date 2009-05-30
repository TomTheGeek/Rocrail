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

TMPOUTDIR=$(GENDIR)/rocrail/bin

CFLAGS+= -ansi
RRLIBS=$(OUTDIR)/libwrapper.a $(OUTDIR)/librocs.a $(OUTDIR)/librocrail.a

OBJS=$(patsubst impl/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/*.c))
WEBOBJS=$(patsubst impl/web/%.c,$(TMPOUTDIR)/%.o,$(wildcard impl/web/*.c))

LIBOBJS=$(TMPOUTDIR)/rcon.o

TARGET=$(OUTDIR)/rocrail$(BINSUFFIX)

all: build

generate: $(GENDIR)/rocrail/wrapper

$(GENDIR)/rocrail/wrapper: $(GENDIR)/rocrail public/wrapper.xml ../common/version.xml
	$(RM) -r $(GENDIR)/rocrail/wrapper
	cd $(GENDIR)/rocrail; ../$(BUILDBINDIR)/wgen ../../rocrail/public/wrapper.xml
	cd $(GENDIR)/rocrail; ../$(BUILDBINDIR)/xml2cstr ../../rocrail/public/wrapper.xml wrapper/impl/wrapperinfo.c wrapperinfo
	cd $(GENDIR)/rocrail; ../$(BUILDBINDIR)/xml2cstr wrapper/doc/wrapper-en.html wrapper/impl/rocrail_doc.c rocrail_doc
	cd $(GENDIR)/rocrail; ../$(BUILDBINDIR)/xml2cstr ../../common/version.xml wrapper/impl/version.c svnLog

$(GENDIR)/rocrail: rocrail.xml
	$(RM) -r $(GENDIR)/rocrail
	-mkdir -p $(OUTDIR)
	-mkdir -p $(TMPOUTDIR)
	-mkdir -p $(GENDIR)/rocrail
	cd $(GENDIR)/rocrail; ../$(BUILDBINDIR)/ogen ../../rocrail/rocrail.xml ../

build: generate $(TARGET)

$(TARGET): $(OBJS) $(WEBOBJS) $(RRLIBS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS) $(WEBOBJS) $(RRLIBS) $(LIBS)

# ------------------------------------------------------------
# The client connection archive.
# ------------------------------------------------------------
$(OUTDIR)/librocrail.a: $(LIBOBJS)
	$(TOOLPREFIX)ar rcs $(OUTDIR)/librocrail.a $(LIBOBJS)

# ------------------------------------------------------------
# The wrapper archive.
# ------------------------------------------------------------
$(OUTDIR)/libwrapper.a: generate $(OUTDIR)/librocs.a
	$(MAKE) -f makefile.gen $@

# ------------------------------------------------------------
# The RocRail objects.
# ------------------------------------------------------------
$(TMPOUTDIR)/%.o: impl/%.c $(GENDIR)/rocrail/wrapper/public/wrapper.h
	$(CC) $(CFLAGS) $< -o $@

$(TMPOUTDIR)/%.o: impl/web/%.c $(GENDIR)/rocrail/wrapper/public/wrapper.h
	$(CC) $(CFLAGS) $< -o $@

#$(GENDIR)/rocrail/wrapper/bin/%.o: $(GENDIR)/rocrail/wrapper/impl/%.c
#	$(CC) $(CFLAGS) $< -o $@

clean:
	$(RM) $(TARGET)
	$(RM) $(OUTDIR)/*.o
	$(RM) $(OUTDIR)/librocrail.a
	$(RM) $(OUTDIR)/libwrapper.a
	$(RM) -r $(GENDIR)/rocrail

# ------------------------------------------------------------
# Install a new copy, overwriting existing files but not deleting additional user-created files
# ------------------------------------------------------------
install:
	-mkdir -p $(DESTDIR)$(ROCRAIL_LIBEXECDIR)
	-cp -p $(OUTDIR)/rocrail$(BINSUFFIX) $(DESTDIR)$(ROCRAIL_LIBEXECDIR)

uninstall:
	$(RM) $(DESTDIR)$(ROCRAIL_LIBEXECDIR)/rocrail$(BINSUFFIX)
