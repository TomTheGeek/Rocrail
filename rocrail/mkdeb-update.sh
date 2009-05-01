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
mkdir -p debian/opt/rocrail

cp ../rocrail/package/control debian/DEBIAN

cp ../unxbin/rocrail debian/opt/rocrail
cp ../unxbin/rocview debian/opt/rocrail
cp ../unxbin/*.so debian/opt/rocrail

strip debian/opt/rocrail/rocrail
strip debian/opt/rocrail/rocview
strip debian/opt/rocrail/*.so

cp -R ../COPYING debian/opt/rocrail

dpkg-deb --build debian
mv debian.deb rocrail-update-$VERSION.$PATCH-rev$SVN-$RELEASNAME-$DIST-i386.deb
rm -Rf debian
cd ../rocrail
