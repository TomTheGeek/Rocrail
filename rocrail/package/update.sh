#!/bin/sh

DIST=$2
ARCH=$3

if [ !  $1 ]; then
  echo "usage  : sudo ./update.sh revision"
  echo "example: sudo ./update.sh 6000 debian6 i386"
  exit $?
fi

if [ ! $2 ]; then
  DIST="debian6"
fi

if [ ! $3 ]; then
  ARCH="i386"
fi


echo "stop rocraild service"
service rocraild stop
echo "download revision $1 $DIST $ARCH"
wget http://rocrail.net/software/rocrail-snapshot/server/rocrail-server-$1-$DIST-$ARCH.deb
echo "install revision $1 $DIST $ARCH"
dpkg -i rocrail-server-$1-$DIST-$ARCH.deb
echo "start rocraild service"
service rocraild start
echo "Ready!"
