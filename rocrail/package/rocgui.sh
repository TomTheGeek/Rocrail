#!/bin/sh
if [ ! -e ~/rocrail ] ; then
	mkdir ~/rocrail
fi

if [ ! -e ~/rocrail/rocgui.ini ] ; then
	cp /opt/rocrail/default/rocgui.ini ~/rocrail
	cp /opt/rocrail/default/plan.xml ~/rocrail
fi

cd ~/rocrail

if [ ! -e ~/rocrail/icons ] ; then
	ln -s /opt/rocrail/icons ~/rocrail/icons
fi

/opt/rocrail/rocview $1 $2 $3

