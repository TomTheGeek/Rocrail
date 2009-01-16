#!/bin/bash
# variable 1 should be something like "0.9"
# variable 2 should be something like "0" == patch 0
# variable 3: CVS revision tag (rocrail-vXXX or HEAD)
# variable 4: cvs user
# variable 5: cpu for redhat i386, suse i586, suses390 s390

VERSION=$1
RELEASE=$2
REVISION=$3
CVSUSER=$4
CPU=$5
BUILDROOT=/home/$USER/rpmbuild
#BUILDROOT=/usr/src/redhat

if [ !  $1 ] || [ ! $2 ] || [ ! $3 ] || [ ! $4 ] || [ ! $5 ]; then
  echo "usage: mkrpm.sh version release revision cvsuser cpu"
  exit $?
fi
if [ ! -e ~/.rpmmacros ] ; then
	cp rocrail/_rpmmacros ~/.rpmmacros
fi

mkdir -p $BUILDROOT/out
mkdir -p $BUILDROOT/SOURCES
mkdir -p $BUILDROOT/BUILD
mkdir -p $BUILDROOT/SPECS
mkdir -p $BUILDROOT/RPMS
mkdir -p $BUILDROOT/SRPMS
mkdir -p $BUILDROOT/$CPU

mkdir rocrail-$VERSION
cd rocrail-$VERSION
svn co -r $REVISION https://rocrail.svn.sourceforge.net/svnroot/rocrail/Rocrail Rocrail
cd ..

if [ -e rocrail-$VERSION-$RELEASE.tar ] ; then
	rm -f rocrail-$VERSION-$RELEASE.tar
fi
echo "creating tar..."
tar -cf rocrail-$VERSION-$RELEASE.tar rocrail-$VERSION
echo "zipping tar..."
gzip -f rocrail-$VERSION-$RELEASE.tar
echo "copy to SOURCES..."
cp rocrail-$VERSION-$RELEASE.tar.gz $BUILDROOT

echo "executing rpmbuild..."
rpmbuild -v -bb --buildroot $BUILDROOT rocrail-$VERSION/Rocrail/rocrail/rocrail.spec
echo "cleanup..."
#rm -Rf rocrail-$VERSION
mkdir ../releases
mv rocrail-$VERSION-$RELEASE.tar.gz ../releases/rocrail-$VERSION-$RELEASE.tar.gz
cp $BUILDROOT/$CPU/rocrail*.rpm ../releases
rm -Rf $BUILDROOT/BUILD/rocrail-$VERSION


