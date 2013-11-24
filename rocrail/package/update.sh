#!/bin/sh
if [ !  $1 ]; then
  echo "usage  : sudo ./update.sh revision"
  echo "example: sudo ./mkdeb.sh 6000"
  exit $?
fi

echo "stop rocraild service"
service rocraild stop
echo "download revision $1"
wget http://rocrail.net/software/rocrail-snapshot/raspi/rocrail-server-$1-wheezy-armhf.deb
echo "install revision $1"
dpkg -i rocrail-server-$1-wheezy-armhf.deb
echo "start rocraild service"
service rocraild start
echo "Ready!"
