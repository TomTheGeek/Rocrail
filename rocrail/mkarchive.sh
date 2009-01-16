#!/bin/sh
# call example: ./mkarchive.sh 1.0.0 harvest-rc1 HEAD
VERSION=$1
RELEASE=$2
REVISION=$3


if [ ! -e ../package ] ; then
	mkdir ../package
fi
mkdir /tmp/rocrail-$VERSION
#cd /tmp/rocrail-$VERSION
svn co -r $REVISION https://rocrail.svn.sourceforge.net/svnroot/rocrail/Rocrail /tmp/rocrail-$VERSION/Rocrail
#cd ..

rm -f /tmp/rocrail-$VERSION/rocrail-$VERSION-$RELEASE.tar
echo "creating tar..."
tar -cf /tmp/rocrail-$VERSION-$RELEASE.tar /tmp/rocrail-$VERSION
echo "zipping tar..."
gzip -f /tmp/rocrail-$VERSION-$RELEASE.tar
cp /tmp/rocrail-$VERSION-$RELEASE.tar.gz ../package 

rm /tmp/rocrail-$VERSION-$RELEASE.tar.gz 
rm -Rf /tmp/rocrail-$VERSION
