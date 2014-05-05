#!/bin/bash
cd /opt/rocrail/
rm -f nohup.out
#nohup ./rocrail -l /opt/rocrail&
nohup ./rocrail -l /opt/rocrail > nohup.out 2>&1 &
echo "$!" > /tmp/rocraild.pid
