#!/bin/sh
#
#    Copyright (C) 2002-2012 Rob Versluis, Rocrail.net
#
#    Without an official permission commercial use is not permitted.
#    Forking this project is not permitted.  
#
echo ""
echo "*** Rocrail makemac.sh starting (see www.rocrail.net)..."
echo ""

# Check params
# rocrail-[revno]-[dist]  (md5)

DIST=$1

echo "Checking Parameters..."

if [ !  $1 ]; then
  echo "Error: Missing parameters:"
  echo ""
  echo "    Usage: makemac.sh <dist>"
  echo ""
  echo "    Example: \"makemac.sh lion\" will build "
  echo "    \"rocrail-XXXX-osx-lion.dmg\" where \"XXXX\" is "
  echo "    the Bazaar revision number or \"user\" if Bazaar is not installed."
  echo ""
  exit $?
else
  echo "    Parameters okay"
  echo ""
fi

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

echo "Building rocrail-$BAZAARREV-osx-$DIST.dmg"
echo ""

cd rocview
pwd
sed s/\<BZR\>/$BAZAARREV/ < Info.plist.template > Info.plist
cd ..

cd rocview; make macapp; cd ..;


TMP=tmp 
DMG_FILE=rocrail.dmg

rm -rf $TMP
mkdir -p $TMP

# copy the files
mkdir -p $TMP/Rocrail/rocdata/
mkdir -p $TMP/Rocrail/rocdata/trace/

cp -r rocrail/package/images $TMP/Rocrail/rocdata/
cp -r decspecs $TMP/Rocrail/rocdata/
cp -r stylesheets $TMP/Rocrail/rocdata/
cp -r unxbin/Rocrail.app $TMP/Rocrail
cp rocrail/package/plan.xml $TMP/Rocrail/rocdata/
cp -r rocview/svg/themes $TMP/Rocrail/rocdata/
cp rocview/mac_default_rocview.ini $TMP/Rocrail/rocdata/rocview.ini

# pimp the dmg
cp doc/rocrail-logo-dmg.png $TMP/background.png
SetFile -a V $TMP/background.png
cp rocview/xpm/VolumeIcon.icns $TMP/.VolumeIcon.icns
SetFile -c icnC $TMP/.VolumeIcon.icns
cp rocview/_DS_Store $TMP/.DS_Store
SetFile -a V $TMP/.DS_Store

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

mv $DMG_FILE package/rocrail-$BAZAARREV-osx-$DIST.dmg
