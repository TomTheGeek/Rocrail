#!/bin/bash
cd /opt/rocnet/
rm -f nohup.out
nohup ./rocnetnode -l /opt/rocnet&
echo "$!" > /tmp/rocnetnoded.pid
