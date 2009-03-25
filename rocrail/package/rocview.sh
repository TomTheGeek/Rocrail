#!/bin/sh

if [ ! -e ~/rocrail ] ; then
	mkdir ~/rocrail
fi

if [ "`which rocprops.sh`" != "" ] ; then
	source rocprops.sh
else
	# Use default property values
	ROCRAIL_LIBEXECDIR=/usr/local/libexec/rocrail
	ROCRAIL_DATADIR=/usr/local/share/rocrail
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

cd ~/rocrail

if [ ! -e ~/rocrail/icons ] ; then
	ln -s $ROCRAIL_DATADIR/icons ~/rocrail/icons
fi

if [ ! -e ~/rocrail/svg ] ; then
	ln -s $ROCRAIL_DATADIR/svg ~/rocrail/svg
fi

if [ ! -e ~/rocrail/images ] ; then
	cp -r $ROCRAIL_DATADIR/images ~/rocrail/images
fi

$ROCRAIL_LIBEXECDIR/rocview $*
