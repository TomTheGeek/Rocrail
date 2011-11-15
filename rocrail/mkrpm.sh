#!/bin/bash
# variable 1 should be something like "0.9"
# variable 2 should be something like "0" == patch 0
# variable 3: CVS revision tag (rocrail-vXXX or HEAD)
# variable 4: cvs user
# variable 5: cpu for redhat i386, suse i586, suses390 s390, 64bit x86_64

VERSION=$1
CPU=$2
BUILDROOT=/home/$USER/rpmbuild
BAZAARREV=`bzr revno`

if [ !  $1 ] || [ ! $2 ]; then
  echo "usage: mkrpm.sh version cpu"
  exit $?
fi
if [ ! -e ~/.rpmmacros ] ; then
	cp _rpmmacros ~/.rpmmacros
fi

mkdir -p $BUILDROOT

if [ -e rocrail-$VERSION ] ; then
	rm -Rf rocrail-$VERSION
fi

mkdir rocrail-$VERSION
cd rocrail-$VERSION
pwd
bzr co --lightweight https://launchpad.net/rocrail Rocrail
sed s/\<BZR\>/$BAZAARREV/ < Rocrail/rocrail/rocrail-template.spec > Rocrail/rocrail/rocrail.spec
cd ..

if [ -e rocrail-$VERSION-$BAZAARREV.tar ] ; then
	rm -f rocrail-$VERSION-$BAZAARREV.tar
fi
echo "creating tar..."
tar -cf rocrail-$VERSION-$BAZAARREV.tar rocrail-$VERSION
echo "zipping tar..."
gzip -f rocrail-$VERSION-$BAZAARREV.tar
echo "copy to SOURCES..."
cp rocrail-$VERSION-$BAZAARREV.tar.gz $BUILDROOT

echo "executing rpmbuild..."
rpmbuild -v -ba rocrail-$VERSION/Rocrail/rocrail/rocrail.spec
echo "cleanup..."
#rm -Rf rocrail-$VERSION
cp $BUILDROOT/$CPU/rocrail*.rpm ../package
#rm -Rf $BUILDROOT/BUILD/rocrail-$VERSION


