#!/bin/sh
#
# mkdeb.sh -- Copyright 2009 Rocrail.net.  See www.rocrail.net for license details
#

echo ""
echo "*** Rocrail mkdeb-full.sh starting (see www.rocrail.net)..."
echo ""

# Check parameters

echo "Checking parameters..."

# Check for debug flag

DEBUGFLAG=0
MAKEFLAG=0
FLAGERROR=
while getopts 'dm' OPTION
do
  case $OPTION in
  d)	DEBUGFLAG=1
		;;
  m)	MAKEFLAG=1
		;;
  ?)	FLAGERROR=1
		;;
  esac
done

if [ ! $FLAGERROR ]; then
  shift $(($OPTIND - 1))
fi

# Check for required params

VERSION=$1
PATCH=$2
RELEASNAME=$3
BAZAARREVNO=$4
DIST=$5

if [ !  $1 ] || [ ! $2 ] || [ ! $3 ] || [ "$FLAGERROR" = "1" ]; then
  if [ "$FLAGERROR" = "1" ]; then
    echo "Error: Invalid switch."
  else
    echo "Error: Missing parameters."
  fi
  echo ""
  echo "    Usage: mkdeb-full.sh [-d] [-m] <version> <patch> <relname> [<revno> ] [<dist>]"
  echo ""
  echo "    Example: \"mkdeb.sh 1.2 999 snapshot 162 debian\" will build "
  echo "    \"rocrail-setup-1.2.999-rev162-snapshot-debian-i386.deb\".  "
  echo ""
  echo "    Use [-d] to make a debugging package for local use only.  \"-dbg\""
  echo "    will be appended to the distribution (\"<dist>\") name."
  echo ""
  echo "    Use [-m] to run \"make all\" before building the package."
  echo ""
  echo "    If <revno> is not specified, or is \"auto\", Bazaar will be "
  echo "    used to get the revision number if possible, otherwise \"user\""
  echo "    will be used."
  echo ""
  echo "    If <dist> is not specified, \"debian\" will be used.  You must"
  echo "    specify <revno> to set <dist>.  Set <revno> to \"auto\" if you "
  echo "    want to use Bazaar to get the revision number."
  echo ""
  exit $?
else
  echo "    Parameters okay"
  echo ""
fi

# Check and/or get Bazaar revision number

if [ ! $4 ] || [ "$4" = "auto" ]; then
  echo "Getting Bazaar revision number..."
  if which bzr > /dev/null
  then
    BAZAARREVNO=`bzr revno`
    echo "    Revision number is $BAZAARREVNO"
    echo ""
  else
    BAZAARREVNO="user"
    echo "    Revision number not found or Bazaar not installed, using \"user\""
    echo ""
  fi
fi

# Check and/or set distribution

echo "Setting distribution..."

if [ ! $5 ]; then
  DIST="debian"
  echo "    <dist> parameter not specified , using \"debian\"..."
fi

if [ $DEBUGFLAG = 1 ]; then
  DIST="$DIST-dbg"
fi
echo "    Distribution is \"$DIST\""
echo ""

# Show the user the filename being built

PACKAGENAME="rocrail-setup-$VERSION.$PATCH-rev$BAZAARREVNO-$RELEASNAME-$DIST-i386.deb"
echo "Building $PACKAGENAME in ../package/"
if [ $DEBUGFLAG = 1 ]; then
  echo "  -- Building a debug package"
fi
if [ $MAKEFLAG = 1 ]; then
  echo "  -- Running \"make all\" (this may take some time)"
fi
echo ""

# Get to root of Rocrail tree

cd ..

# Make all

if [ $MAKEFLAG = 1 ]; then
  echo "Running make..."
  make
  echo "    Done"
else
  echo "Skipping make."
fi
echo ""

# Make sure package exists

echo "Checking package directory..."

if [ ! -e ../package ] ; then
	echo "    package not found, creating it"
	mkdir ../package
fi

echo "    Done"
echo ""

# Change to package directory and set up subdirectories

echo "Removing prevous packages..."

cd package
rm -Rf *.deb

echo "    Done"
echo ""

# Set up subdirectories

echo "Setting up temp directories..."

mkdir -p debian/DEBIAN
mkdir -p debian/opt/rocrail/default
mkdir -p debian/opt/rocrail/icons
mkdir -p debian/opt/rocrail/stylesheets
mkdir -p debian/opt/rocrail/symbols
mkdir -p debian/opt/rocrail/svg
mkdir -p debian/usr/share/applications
mkdir -p debian/usr/share/desktop-directories
mkdir -p debian/etc/init.d

echo "    Done"
echo ""

# Copy objects and libraries

echo "Copying objects and libraries..."

cp ../rocrail/package/control debian/DEBIAN

cp ../unxbin/rocrail debian/opt/rocrail
cp ../unxbin/rocview debian/opt/rocrail
cp ../unxbin/*.so debian/opt/rocrail

echo "    Done"
echo ""

# Strip binaries

if [ $DEBUGFLAG = 0 ]; then
  echo "Stripping Debian Binaries..."

  strip debian/opt/rocrail/rocrail
  strip debian/opt/rocrail/rocview
  strip debian/opt/rocrail/*.so

  echo "    Done"
  echo ""
else
  echo "Leaving debugging information in binaries."
  echo ""
fi

# Copy art and additional files

echo "Copying art and additional files..."

cp ../rocrail/package/Rocrail.directory debian/usr/share/desktop-directories
cp ../rocrail/package/Roc*.desktop debian/usr/share/applications
cp ../rocrail/package/roc*.sh debian/opt/rocrail
cp ../rocrail/package/rocraild debian/etc/init.d
chmod +x debian/opt/rocrail/*.sh
cp ../rocrail/package/rocraild.png debian/opt/rocrail
cp ../rocrail/package/rocrail.xpm debian/opt/rocrail
cp -u ../rocrail/package/roc*.ini debian/opt/rocrail/default
cp -u ../rocrail/package/plan.xml debian/opt/rocrail/default
cp -u ../rocrail/package/neustadt.xml debian/opt/rocrail/default

cp -R ../rocgui/icons/*.* debian/opt/rocrail/icons
cp -R ../stylesheets/*.* debian/opt/rocrail/stylesheets
cp -R ../rocgui/svg/* debian/opt/rocrail/svg
cp -R ../rocrail/symbols/*.* debian/opt/rocrail/symbols
cp -R ../COPYING debian/opt/rocrail

echo "    Done"
echo ""

# Build the package

echo "Building debian package..."

dpkg-deb --build debian

echo "    Done"
echo ""

# Clean up

echo "Cleaning up..."

mv debian.deb $PACKAGENAME
rm -Rf debian
cd ../rocrail

echo "    Done building $PACKAGENAME in ../package"
echo ""

