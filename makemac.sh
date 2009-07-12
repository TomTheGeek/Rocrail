#!/bin/sh
#
# pac-mac.sh -- Copyright 2009 Rocrail.net.  See www.rocrail.net for license details
#
echo ""
echo "*** Rocrail makemac.sh starting (see www.rocrail.net)..."
echo ""

# Check params
# rocrail-setup-[version].[patch]-rev[revno]-[type]-[dist].exe  (md5)

VERSION=$1
PATCH=$2
TYPE=$3
DIST=$4

echo "Checking Parameters..."

if [ !  $1 ] || [ ! $2 ] || [ ! $3 ] || [ ! $4 ]; then
  echo "Error: Missing parameters:"
  echo ""
  echo "    Usage: makemac.sh <version> <patch> <type> <dist>"
  echo ""
  echo "    Example: \"makemac.sh 1.2 999 snapshot x64\" will build "
  echo "    \"rocrail-1.2.999-revXXX-snapshot-x64.dmg\" where \"XXX\" is "
  echo "    the Bazaar revision number or \"user\" if Bazaar is not installed."
  echo ""
  exit $?
else
  echo "    Parameters okay"
  echo ""
fi

# create the .apps
cd rocrail; make macapp; cd ..;
cd rocgui; make macapp; cd ..;

echo "Getting Bazaar revision number..."
if which bzr > /dev/null
then
	BAZAARREV=`bzr revno`
	echo "    Revision number is $BAZAARREV"
	echo ""
else
	BAZAARREV="user"
	echo "    Revision number not found or Bazaar not installed, using \"user\""
	echo ""
fi

echo "Building rocrail-$VERSION.$PATCH-rev$BAZAARREV-$TYPE-$DIST.dmg"
echo ""

# Create an initial disk image (200 megs)
hdiutil create -size 200m -fs HFS+ -volname "Rocrail" myimg.dmg

# Mount the disk image
hdiutil attach myimg.dmg

# Obtain device information
DEVS=$(hdiutil attach myimg.dmg | cut -f 1)
DEV=$(echo $DEVS | cut -f 1 -d ' ')

DEST=$(hdiutil attach myimg.dmg | cut -f 3 )

# copy the files
mkdir -p $DEST/Rocrail/rocdata/
mkdir -p $DEST/Rocrail/rocdata/trace/

cp -r rocrail/package/images $DEST/Rocrail/rocdata/

cp -r unxbin/Rocrail.app $DEST/Rocrail
cp -r unxbin/Rocview.app $DEST/Rocrail
cp -r unxbin/Modview.app $DEST/Rocrail

cp rocrail/package/plan.xml $DEST/Rocrail/rocdata/
cp rocrail/package/rocrail.ini $DEST/Rocrail/rocdata/
cp rocrail/package/rocview.ini_dmg $DEST/Rocrail/rocdata/rocview.ini

cp -r rocgui/svg/themes $DEST/Rocrail/rocdata/

# pimp the dmg
cp doc/rocrail-logo-dmg.png $DEST/background.png
setFile -a V $DEST/background.png
cp rocrail/rocrail.icns $DEST/VolumeIcon.icns
setFile -a V $DEST/VolumeIcon.icns
cp rocgui/_DS_Store $DEST/.DS_Store
setFile -a V $DEST/.DS_Store
cp rocgui/_DS_Store_Rocrail $DEST/Rocrail/.DS_Store
setFile -a V $DEST/Rocrail/.DS_Store

# Unmount the disk image
hdiutil detach $DEV

# Convert the disk image to read-only
hdiutil convert myimg.dmg -format UDZO -o rocrail.dmg

#clean up
rm -f myimg.dmg

if [ ! -e package ] ; then
	echo "    package not found, creating it"
	mkdir package
fi

mv rocrail.dmg package/rocrail-$VERSION.$PATCH-rev$BAZAARREV-$TYPE-$DIST.dmg
