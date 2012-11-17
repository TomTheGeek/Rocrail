#!/bin/sh
#
#    Copyright (C) 2002-2012 Rob Versluis, Rocrail.net
#
#    Without an official permission commercial use is not permitted.
#    Forking this project is not permitted.  
#

echo ""
echo "*** Rocrail makewin-full.sh starting (see www.rocrail.net)..."
echo ""

# Check params
# rocrail-setup-rev[revno].exe  (md5)

echo "Checking Parameters..."

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

#make all PLATFORM=WIN32 TOOLPREFIX=i586-mingw32msvc- LIBSUFFIX=-i586-mingw32msvc MINGWINSTALL=/usr/i586-mingw32msvc
#make all PLATFORM=WIN64 TOOLPREFIX=x86_64-w64-mingw32- LIBSUFFIX=-x86_64-w64-mingw32 MINGWINSTALL=/usr/x86_64-w64-mingw32
echo "    Done"
echo ""

# Strip binaries

echo "Stripping Windows Binaries"

cd winbin
i586-mingw32msvc-strip *.dll
#x86_64-w64-mingw32-strip *.dll
i586-mingw32msvc-strip *.exe
#x86_64-w64-mingw32-strip *.exe

echo "    Done"
echo ""

# Extract mingw runtime dll to winbin directory

echo "Copying mingwm10.dll"


if [ -f /usr/share/doc/mingw32-runtime/mingwm10.dll ] 
then
  echo "    Copying existing /usr/share/doc/mingw32-runtime/mingwm10.dll"

  cp /usr/share/doc/mingw32-runtime/mingwm10.dll .
elif [ -f /usr/share/doc/mingw32-runtime/mingwm10.dll.gz ] 
then
  echo "    Using /usr/share/doc/mingw32-runtime/mingwm10.dll.gz"

  sudo gunzip /usr/share/doc/mingw32-runtime/mingwm10.dll.gz
  cp /usr/share/doc/mingw32-runtime/mingwm10.dll .
  #sudo gzip /usr/share/doc/mingw32-runtime/mingwm10.dll
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
sed s/\<BZR\>/$BAZAARREV/ < rocrail-template.iss > rocrail-temp2.iss
mv rocrail-temp2.iss rocrail-temp.iss

echo "    Done"
echo ""

# Run Inno Setup via WINE

echo "Using Wine to run Inno Setup..."
echo ""

wine "c:\\Program Files\\Inno Setup 5\\ISCC.exe" rocrail-temp.iss
rm rocrail-temp.iss

echo "    Done"
echo ""

# Show finished script

echo "    Done building windows package"
echo ""

# Return to start directory

cd ../..
pwd



