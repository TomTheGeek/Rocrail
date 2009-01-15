#!/bin/sh
if [ ! -e ~/rocrail ] ; then
	mkdir ~/rocrail
fi

if [ ! -e ~/rocrail/rocrail.ini ] ; then
	cp /opt/rocrail/default/rocrail.ini ~/rocrail
	cp /opt/rocrail/default/plan.xml ~/rocrail
	cp /opt/rocrail/default/neustadt.xml ~/rocrail
fi

cd ~/rocrail

if ps --no-heading -C rocrail
  then
    echo "rocrail is running"
  else
    echo "rocrail is not running. start..."
#    gnome-terminal --hide-menubar --title=Rocrail --working-directory="~/rocrail" --geometry=132x50 -e "/opt/rocrail/rocrail -console -l /opt/rocrail"
    /opt/rocrail/rocrail -console -l /opt/rocrail
fi

