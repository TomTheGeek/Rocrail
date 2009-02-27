#!/bin/sh
#
# makewin_full.sh -- Copyright 2009 Rocrail.net.  See www.rocrail.net for license details
#

# Check params
# rocrail-setup-[version].[patch]-rev-[relname]-[dist].exe  (md5)

echo ""
echo "*** Rocrail makewin_full.sh starting (see www.rocrail.net)..."
echo ""

VERSION=$1
PATCH=$2
TYPE=$3
DIST=$4

echo "Checking Parameters..."

if [ !  $1 ] || [ ! $2 ] || [ ! $3 ] || [ ! $4 ]; then
  echo "Error: Missing parameters:"
  echo ""
  echo "    Usage: makewin_full.sh <version> <patch> <type> <dist>"
  echo ""
  echo "    Example: \"makewin_full.sh 1.2 999 snapshot unicode\" will build "
  echo "    \"rocrail-setup-1.2.999-revXXX-snapshot-unicode.exe\" where \"XXX\" is "
  echo "    the Bazaar revision number or \"user\" if Bazaar is not installed."
  echo ""
  exit $?
else
  echo "    Parameters okay"
  echo ""
fi

# Get the Bazaar revision number if available

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

# Show the user the filename being built

echo "Building rocrail-setup-$VERSION.$PATCH-rev$BAZAARREV-$TYPE-$DIST.exe"
echo ""

# Make all

echo "Making All"

cd ..
make all PLATFORM=WIN32 TOOLPREFIX=i586-mingw32msvc- LIBSUFFIX=-i586-mingw32msvc MINGWINSTALL=/usr/i586-mingw32msvc

echo "    Done"
echo ""

# Strip binaries

echo "Stripping Windows Binaries"

cd winbin
i586-mingw32msvc-strip *.dll
i586-mingw32msvc-strip *.exe

echo "    Done"
echo ""

# Extract mingw runtime dll to winbin directory

echo "Copying mingwm10.dll"

if [ -f /usr/share/doc/mingw32-runtime/mingwm10.dll.gz ] 
then
  echo "    Using /usr/share/doc/mingw32-runtime/mingwm10.dll.gz"

  sudo gunzip /usr/share/doc/mingw32-runtime/mingwm10.dll.gz
  cp /usr/share/doc/mingw32-runtime/mingwm10.dll .
  sudo gzip /usr/share/doc/mingw32-runtime/mingwm10.dll

elif [ -f /usr/share/doc/mingw32-runtime/mingwm10.dll ] 
then
  echo "    Copying existing /usr/share/doc/mingw32-runtime/mingwm10.dll"

  cp /usr/share/doc/mingw32-runtime/mingwm10.dll .

else
  echo "Error: mingwm10.dll not found!"
  echo ""
  echo "    Cannot find mingwm10.dll.gz or mingwm10.dll in /usr/share/doc/mingw32-runtime."
  echo "    Please reinstall MinGW32 and make sure this file exists."
  echo ""
  exit $?
fi

echo "    Done"
echo ""

# How to automate the Windows Installer run?
# How to pass in rev and other version numbers?
#   Sed the file
#     replace "revxxxx" with rev<bzr #>
#     replace "1.2.0" with snapshot number  

# Substitute variables in the default rocrail.iss file

echo "Creating Inno Setup installer file..."

# Run sed to swap version, bazaar rev number, release type and distribution

cd ../rocrail/package
pwd
sed s/\<BZR\>/$BAZAARREV/ < rocrail_template.iss > rocrail_temp2.iss
mv rocrail_temp2.iss rocrail_temp.iss

sed s/\<VER\>/$VERSION/ < rocrail_temp.iss > rocrail_temp2.iss
mv rocrail_temp2.iss rocrail_temp.iss

sed s/\<PATCH\>/$PATCH/ < rocrail_temp.iss > rocrail_temp2.iss
mv rocrail_temp2.iss rocrail_temp.iss

sed s/\<DIST\>/$DIST/ < rocrail_temp.iss > rocrail_temp2.iss
mv rocrail_temp2.iss rocrail_temp.iss

sed s/\<TYPE\>/$TYPE/ < rocrail_temp.iss > rocrail_temp2.iss
mv rocrail_temp2.iss rocrail_temp.iss

cat rocrail_temp.iss

echo "    Done"
echo ""

# Run Inno Setup via WINE

echo Using Wine to run Inno Setup

wine "c:\\Program Files\\Inno Setup 5\\ISCC.exe" rocrail_temp.iss
rm rocrail_temp.iss

echo "    Done"
echo ""

# Show finished script

echo "    Done building windows package"
echo ""

# Return to start directory

cd ../..
pwd



