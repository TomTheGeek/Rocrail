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
  mkdir ~/rocrail/images
fi
cp -pur /opt/rocrail/images ~/rocrail

if [ ! -e ~/rocrail/decspecs ] ; then
  mkdir ~/rocrail/decspecs
fi
cp -pur /opt/rocrail/decspecs ~/rocrail

if [ ! -e ~/rocrail/stylesheets ] ; then
  mkdir ~/rocrail/stylesheets
fi
cp -pur /opt/rocrail/stylesheets ~/rocrail


cd ~/rocrail

/opt/rocrail/rocview -sp /opt/rocrail $*

