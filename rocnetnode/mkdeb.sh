#!/bin/sh


DIST=$1
ARCH=$2
REM=$3

if [ !  $1 ]; then
  echo "usage  : mkdeb.sh dist [arch] [remark]"
  echo "example: mkdeb.sh raspbian armhf"
  exit $?
fi

if [ ! $2 ]; then
  ARCH="armhf"
fi

if [ ! "$3" ]; then
  REM="General fixes release."
fi

echo "Getting Bazaar revision number..."
if which git > /dev/null
then
  BAZAARREV=`git rev-list --count HEAD`
  echo "    Revision number is $BAZAARREV"
  echo ""
else
  BAZAARREV="user"
  echo "    Revision number not found or Bazaar not installed, using \"user\""
  echo ""
fi

if [ ! -e ./raspbian ] ; then
  mkdir ./raspbian
fi

sed s/\<BZR\>/$BAZAARREV/ < ./package/control.template > ./raspbian/control.tmp
sed s/\<ARCH\>/$ARCH/ < ./raspbian/control.tmp > ./raspbian/control
rm ./raspbian/control.tmp

sed s/\<BZR\>/$BAZAARREV/ < ./package/rocnetnode.xml.template > ./raspbian/rocnetnode.xml.tmp
sed s/\<REM\>/"$REM"/ < ./raspbian/rocnetnode.xml.tmp > ./raspbian/rocnetnode.xml
rm ./raspbian/rocnetnode.xml.tmp


cd ./raspbian
rm -Rf *.deb
mkdir -p debian/DEBIAN
mkdir -p debian/opt/rocnet
mkdir -p debian/etc/init.d

cp control debian/DEBIAN/control

cp ../../unxbin/rocnetnode debian/opt/rocnet
cp ../../unxbin/dcc232.so debian/opt/rocnet
cp ../../unxbin/sprog.so debian/opt/rocnet
cp ../../unxbin/rfid12.so debian/opt/rocnet

strip debian/opt/rocnet/rocnetnode
strip debian/opt/rocnet/*.so

cp ../package/update.sh debian/opt/rocnet
cp ../package/update-offline.sh debian/opt/rocnet
cp ../package/rocnetnode.sh debian/opt/rocnet
cp ../package/rocnetnoded debian/etc/init.d
chmod +x debian/opt/rocnet/*.sh
#cp ../package/rocnetnode.ini debian/opt/rocnet
cp ../package/postinst debian/DEBIAN/postinst

fakeroot dpkg-deb --build debian
mv debian.deb rocnetnode-$BAZAARREV-$DIST-$ARCH.deb
rm -Rf debian
cd ..
