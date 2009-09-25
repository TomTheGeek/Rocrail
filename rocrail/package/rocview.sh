#!/bin/sh
if [ ! -e ~/rocrail ] ; then
	mkdir ~/rocrail
fi

if [ ! -e ~/rocrail/rocview.ini ] ; then
	cp /opt/rocrail/plan.xml ~/rocrail
fi

if [ ! -e ~/rocrail/icons ] ; then
	ln -s /opt/rocrail/icons ~/rocrail/icons
fi

if [ ! -e ~/rocrail/svg ] ; then
	ln -s /opt/rocrail/svg ~/rocrail/svg
fi

if [ ! -e ~/rocrail/images ] ; then
	cp -r /opt/rocrail/images ~/rocrail/images
fi

cd ~/rocrail

/opt/rocrail/rocview -sp /opt/rocrail -themespath . $1 $2 $3

