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

if [ ! -e ~/rocrail/rocrail.htb ] ; then
	cp $ROCRAIL_DATADIR/rocrail.htb ~/rocrail
fi

if [ ! -e ~/rocrail/rocgui.ini ] ; then
	cp $ROCRAIL_DATADIR/default/rocgui.ini ~/rocrail
fi

if [ ! -e ~/rocrail/plan.xml ] ; then
	cp $ROCRAIL_DATADIR/default/plan.xml ~/rocrail
fi

if [ ! -e ~/rocrail/neustadt.xml ] ; then
	cp $ROCRAIL_DATADIR/default/neustadt.xml ~/rocrail
fi

cd ~/rocrail

if [ ! -e ~/rocrail/icons ] ; then
	ln -s $ROCRAIL_DATADIR/icons ~/rocrail/icons
fi

$ROCRAIL_LIBEXECDIR/rocview $* -modview
