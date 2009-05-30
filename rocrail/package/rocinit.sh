
ROCRAIL_HOME=~/rocrail

# Load the properties file created by make install
# rocprops.sh should be in the same directory as rocinit.sh
if [ -e "`dirname $0`/rocprops.sh" ] ; then
	. "`dirname $0`/rocprops.sh"
elif [ -n "`which rocprops.sh`" ] ; then
	. rocprops.sh
else
	echo "ERROR: `dirname $0`/rocprops.sh not found"
	exit 5
fi


if [ ! -e $ROCRAIL_HOME ] ; then
	# Create the ROCHOME directory
	mkdir $ROCRAIL_HOME
fi

# Update user directory

# Copy common files
if [ ! -e $ROCRAIL_HOME/plan.xml ] ; then
	cp $ROCRAIL_DATADIR/default/plan.xml $ROCRAIL_HOME
fi
if [ ! -e $ROCRAIL_HOME/neustadt.xml ] ; then
	cp $ROCRAIL_DATADIR/default/neustadt.xml $ROCRAIL_HOME
fi


# Copy rocrail files
if [ ! -e $ROCRAIL_HOME/rocrail.ini ] ; then
	cp $ROCRAIL_DATADIR/default/rocrail.ini $ROCRAIL_HOME
fi
if [ ! -e $ROCRAIL_HOME/plan.xml ] ; then
	cp $ROCRAIL_DATADIR/default/plan.xml $ROCRAIL_HOME
fi
if [ ! -e $ROCRAIL_HOME/neustadt.xml ] ; then
	cp $ROCRAIL_DATADIR/default/neustadt.xml $ROCRAIL_HOME
fi


# Copy rocview files
if [ ! -e $ROCRAIL_HOME/rocview.ini -a -e $ROCRAIL_DATADIR/default/rocview.ini ] ; then
	cp $ROCRAIL_DATADIR/default/rocview.ini $ROCRAIL_HOME
elif [ ! -e $ROCRAIL_HOME/rocgui.ini ] ; then
	cp $ROCRAIL_DATADIR/default/rocgui.ini $ROCRAIL_HOME
fi
if [ ! -e $ROCRAIL_HOME/icons ] ; then
	# Link icons
	ln -s $ROCRAIL_DATADIR/icons $ROCRAIL_HOME/icons
fi


# Copy rocviewmod files
if [ ! -e $ROCRAIL_HOME/rocrail.htb -a -e $ROCRAIL_DATADIR/rocrail.htb ] ; then
	cp $ROCRAIL_DATADIR/rocrail.htb $ROCRAIL_HOME
fi


# Update directories
for rocDir in images stylesheets svg symbols xpm ; do
	mkdir -p $ROCRAIL_HOME/$rocDir
	cp -Rp $ROCRAIL_DATADIR/$rocDir/* $ROCRAIL_HOME/$rocDir
done
