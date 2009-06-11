#!/bin/sh

if [ ! -e ~/rocrail ] ; then
	mkdir ~/rocrail
fi

if [ "`which rocprops.sh`" != "" ] ; then
	source rocprops.sh
else
	# Use default property values
	ROCRAIL_LIBDIR=/usr/local/lib/rocrail
	ROCRAIL_LIBEXECDIR=/usr/local/libexec/rocrail
	ROCRAIL_DATADIR=/usr/local/share/rocrail
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

cd ~/rocrail

if ps --no-heading -C rocrail
  then
    echo "rocrail is running"
  else
    echo "rocrail is not running. start..."
#    gnome-terminal --hide-menubar --title=Rocrail --working-directory="~/rocrail" --geometry=132x50 -e "/opt/rocrail/rocrail -console -l /opt/rocrail"
    $ROCRAIL_LIBEXECDIR/rocrail -console -l $ROCRAIL_LIBDIR
fi
