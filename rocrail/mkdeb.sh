#!/bin/sh


VERSION=$1
PATCH=$2
RELEASNAME=$3
SVN=$4
DIST=$5

if [ !  $1 ] || [ ! $2 ] || [ ! $3 ]; then
  echo "usage: mkdeb.sh version patch relname svn dist"
  exit $?
fi

if [ ! -e ../package ] ; then
	mkdir ../package
fi
cd ../package
rm -Rf *.deb
mkdir -p debian/DEBIAN
mkdir -p debian/opt/rocrail/default
mkdir -p debian/opt/rocrail/icons
mkdir -p debian/opt/rocrail/stylesheets
mkdir -p debian/opt/rocrail/symbols
mkdir -p debian/opt/rocrail/images
mkdir -p debian/opt/rocrail/svg
mkdir -p debian/usr/share/applications
mkdir -p debian/usr/share/desktop-directories
mkdir -p debian/etc/init.d

cp ../rocrail/package/control debian/DEBIAN

cp ../unxbin/rocrail debian/opt/rocrail
cp ../unxbin/rocview debian/opt/rocrail
cp ../unxbin/*.so debian/opt/rocrail

strip debian/opt/rocrail/rocrail
strip debian/opt/rocrail/rocview
strip debian/opt/rocrail/*.so

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

cp -R ../rocrail/package/images/*.* debian/opt/rocrail/images
cp -R ../rocgui/icons/*.* debian/opt/rocrail/icons
cp -R ../stylesheets/*.* debian/opt/rocrail/stylesheets
cp -R ../rocgui/svg/* debian/opt/rocrail/svg
cp -R ../rocrail/symbols/*.* debian/opt/rocrail/symbols
cp -R ../COPYING debian/opt/rocrail

dpkg-deb --build debian
mv debian.deb rocrail-setup-$VERSION.$PATCH-rev$SVN-$RELEASNAME-$DIST-i386.deb
rm -Rf debian
cd ../rocrail
