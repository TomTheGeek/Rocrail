#!/bin/sh


DIST=$1
ARCH=$2

if [ !  $1 ]; then
  echo "usage  : mkdeb.sh dist arch"
  echo "example: mkdeb.sh raspbian armhf"
  exit $?
fi

if [ ! $2 ]; then
  ARCH="armhf"
fi

echo "Getting Bazaar revision number..."
if which bzr > /dev/null
then
  BAZAARREV=`bzr revno .`
  echo "    Revision number is $BAZAARREV"
  echo ""
else
  BAZAARREV="user"
  echo "    Revision number not found or Bazaar not installed, using \"user\""
  echo ""
fi

sed s/\<BZR\>/$BAZAARREV/ < ./package/control.template > ./package/control.tmp
sed s/\<ARCH\>/$ARCH/ < ./package/control.tmp > ./package/control
rm ./package/control.tmp


if [ ! -e ./raspbian ] ; then
	mkdir ./raspbian
fi
cd ./raspbian
rm -Rf *.deb
mkdir -p debian/DEBIAN
mkdir -p debian/opt/rocnet
mkdir -p debian/etc/init.d

cp ../package/control debian/DEBIAN/control

cp ../../unxbin/rocnetnode debian/opt/rocnet
cp ../../unxbin/dcc232.so debian/opt/rocnet
cp ../../unxbin/sprog.so debian/opt/rocnet
cp ../../unxbin/rfid12.so debian/opt/rocnet

strip debian/opt/rocnet/rocnetnode
strip debian/opt/rocnet/*.so

cp ../package/update.sh debian/opt/rocnet
cp ../package/rocnetnode.sh debian/opt/rocnet
cp ../package/rocnetnoded debian/etc/init.d
chmod +x debian/opt/rocnet/*.sh
cp ../package/rocnetnode.ini debian/opt/rocnet
cp ../package/postinst debian/DEBIAN/postinst

fakeroot dpkg-deb --build debian
mv debian.deb rocnetnode-$BAZAARREV-$DIST-$ARCH.deb
rm -Rf debian
cd ..
