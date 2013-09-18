#!/bin/bash
cd /opt/rocrail/
rm -f nohup.out
nohup ./rocrail -l /opt/rocrail&
echo "$!" > /tmp/rocraild.pid
