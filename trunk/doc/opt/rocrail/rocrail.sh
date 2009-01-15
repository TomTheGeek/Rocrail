#!/bin/bash
cd /opt/rocrail/
rm -f nohup.out
nohup ./rocrail -l /opt/rocrail -lcd &
echo "$!" > rocrail.pid
