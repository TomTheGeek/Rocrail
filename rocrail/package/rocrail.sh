#!/bin/sh
#
#    Copyright 2009 Rocrail.net.  See www.rocrail.net for license details
#

# Check if we are running

if [ "`ps a | grep 'rocrail -console' | grep -v 'grep'`" != "" ] ; then
  echo "Rocrail is already running."
  exit
else
  echo "Starting Rocrail..."
fi

#
# Set program locations
#

if [ "`which rocprops.sh`" != "" ] ; then

#       Use locations from make install

	. rocprops.sh
else

#       Use default locations

	ROCRAIL_LIBDIR=/usr/local/lib/rocrail
	ROCRAIL_LIBEXECDIR=/usr/local/libexec/rocrail
	ROCRAIL_DATADIR=/usr/local/share/rocrail
fi

#
# Create user directory and copy default ini and plan files
#

if [ ! -e ~/rocrail ] ; then
	mkdir ~/rocrail
fi
if [ ! -e ~/rocrail/rocrail.ini ] ; then
	cp $ROCRAIL_DATADIR/default/rocrail.ini ~/rocrail
fi
if [ ! -e ~/rocrail/plan.xml ] ; then
	cp $ROCRAIL_DATADIR/default/plan.xml ~/rocrail
fi
if [ ! -e ~/rocrail/neustadt.xml ] ; then
	cp $ROCRAIL_DATADIR/default/neustadt.xml ~/rocrail
fi

# Run Rocrail

cd ~/rocrail
echo "Starting Rocrail..."
$ROCRAIL_LIBEXECDIR/rocrail -console -l $ROCRAIL_LIBDIR

