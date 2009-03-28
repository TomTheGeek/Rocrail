#!/bin/sh
#
#    Copyright 2009 Rocrail.net.  See www.rocrail.net for license details
#

#
# Set program locations
#

if [ "`which rocprops.sh`" != "" ] ; then
	. rocprops.sh
else
	# Use default property values
	ROCRAIL_LIBEXECDIR=/usr/local/libexec/rocrail
	ROCRAIL_DATADIR=/usr/local/share/rocrail
fi

#
# Create user directory and copy default ini and plan files
#

if [ ! -e ~/rocrail ] ; then
	mkdir ~/rocrail
fi
if [ ! -e ~/rocrail/rocgui.ini ] ; then
	cp $ROCRAIL_DATADIR/default/rocgui.ini ~/rocrail
fi
if [ ! -e ~/rocrail/plan.xml ] ; then
	cp $ROCRAIL_DATADIR/default/plan.xml ~/rocrail
fi
if [ ! -e ~/rocrail/images ] ; then
	cp -Rp $ROCRAIL_DATADIR/images ~/rocrail
fi

#
# Link artwork directories
#

if [ ! -e ~/rocrail/icons ] ; then
	ln -s $ROCRAIL_DATADIR/icons ~/rocrail/icons
fi
if [ ! -e ~/rocrail/svg ] ; then
	ln -s $ROCRAIL_DATADIR/svg ~/rocrail/svg
fi

# Run Rocview

cd ~/rocrail
$ROCRAIL_LIBEXECDIR/rocview $*

