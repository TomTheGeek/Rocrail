#!/bin/sh
#File:  MyBundle.app/Contents/MacOS/startup.command
#echo "this is a test"
#pwd
mypath=`dirname "$0"`
#echo $mypath
$mypath/Rocrail -console -l $mypath -x $mypath/../../../rocdata/plan.xml -i $mypath/../../../rocdata/rocrail.ini -w $mypath/../../../rocdata/ -img $mypath/../../../rocdata/images/ -t $mypath/../../../rocdata/trace/rocrail.trace
