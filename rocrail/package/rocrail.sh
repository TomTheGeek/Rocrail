#!/bin/sh
if [ ! -e ~/rocrail ] ; then
	mkdir ~/rocrail
fi

if [ ! $NO_GNOME_PLEASE ] ; then
  gnome_terminal=`command -v gnome-terminal`
fi

cd ~/rocrail

if ps --no-heading -C rocrail
  then
    echo "rocrail is running"
  else
    echo "rocrail is not running. start..."
    
    if [ $gnome_terminal ]
      then
        $gnome_terminal --hide-menubar --title=Rocrail --geometry=132x50 -e "/opt/rocrail/rocrail -console -l /opt/rocrail $*" 
      else
        /opt/rocrail/rocrail -console -l /opt/rocrail $*
    fi
fi    
