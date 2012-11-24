#!/bin/sh
if [ ! -e ~/rocrail ] ; then
	mkdir ~/rocrail
fi

if [ ! -e ~/rocrail/plan.xml ] ; then
	cp /opt/rocrail/default/plan.xml ~/rocrail
fi

if [ ! -e ~/rocrail/svg ] ; then
	ln -s /opt/rocrail/svg ~/rocrail/svg
fi

if [ ! -e ~/rocrail/images ] ; then
	cp -r /opt/rocrail/images ~/rocrail/images
fi

if [ ! -e ~/rocrail/decspecs ] ; then
  cp -r /opt/rocrail/decspecs ~/rocrail/decspecs
fi

if [ ! -e ~/rocrail/stylesheets ] ; then
  cp -r /opt/rocrail/stylesheets ~/rocrail/stylesheets
fi

cd ~/rocrail

/opt/rocrail/rocview -sp /opt/rocrail $*

