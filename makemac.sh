#!/bin/sh
#
# makemac.sh -- Copyright 2010 Rocrail.net.  See www.rocrail.net for license details
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

cd rocview; make macapp; cd ..;

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

TMP=tmp 
DMG_FILE=rocrail.dmg

rm -rf $TMP
mkdir -p $TMP

# copy the files
mkdir -p $TMP/Rocrail/rocdata/
mkdir -p $TMP/Rocrail/rocdata/trace/

cp -r rocrail/package/images $TMP/Rocrail/rocdata/
cp -r unxbin/Rocrail.app $TMP/Rocrail
cp rocrail/package/plan.xml $TMP/Rocrail/rocdata/
cp -r rocview/svg/themes $TMP/Rocrail/rocdata/
cp rocview/mac_default_rocview.ini $TMP/Rocrail/rocdata/rocview.ini

# pimp the dmg
cp doc/rocrail-logo-dmg.png $TMP/background.png
setFile -a V $TMP/background.png
cp rocview/xpm/VolumeIcon.icns $TMP/.VolumeIcon.icns
SetFile -c icnC $TMP/.VolumeIcon.icns
cp rocview/_DS_Store $TMP/.DS_Store
setFile -a V $TMP/.DS_Store

# Create an initial disk image
hdiutil create -srcfolder $TMP -volname Rocrail -format UDRW -ov raw-$DMG_FILE

rm -rf $TMP
mkdir -p $TMP
hdiutil attach raw-$DMG_FILE -mountpoint $TMP

SetFile -a C $TMP
hdiutil detach $TMP
rm -rf $TMP
rm -f $DMG_FILE
hdiutil convert raw-$DMG_FILE -format UDZO -o $DMG_FILE
rm -f raw-$DMG_FILE

if [ ! -e package ] ; then
	echo "    package not found, creating it"
	mkdir package
fi

mv $DMG_FILE package/rocrail-$VERSION.$PATCH-rev$BAZAARREV-$TYPE-$DIST.dmg
