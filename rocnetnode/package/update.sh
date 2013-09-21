#!/bin/sh
if [ !  $1 ]; then
  echo "usage  : sudo ./update.sh revision"
  echo "example: sudo ./mkdeb.sh 6000"
  exit $?
fi

echo "stop rocnetnoded service"
service rocnetnoded stop
echo "download revision $1"
wget http://rocrail.net/software/rocrail-snapshot/raspi/rocnetnode-$1-raspbian-armhf.deb
echo "install revision $1"
dpkg -i rocnetnode-$1-raspbian-armhf.deb
echo "start rocnetnoded service"
service rocnetnoded start
echo "Ready!"
