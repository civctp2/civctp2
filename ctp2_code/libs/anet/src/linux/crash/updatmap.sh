#!/bin/sh
# Get updated map files
#
# You should not need to edit this file. 
#
set -x

. ~/crash/crashenv

ftp -n dkegel2.activision.com <<EOF
user anonymous updater
passive
binary
get iservmap.zip /tmp/maps.zip
quit
EOF

if [ -s /tmp/maps.zip ]; then
   unzip -o -a /tmp/maps.zip modcrc.txt *.map -d $CRASH_HOME/maps
fi
rm /tmp/maps.zip
sed s/'\\'/'\/'/g $CRASH_HOME/maps/modcrc.txt > $CRASH_HOME/modcrc.tmp
if [ -s $CRASH_HOME/modcrc.tmp ]; then
   mv $CRASH_HOME/modcrc.tmp $CRASH_HOME/maps/modcrc.txt
elif [ -f $CRASH_HOME/modcrc.tmp ]; then
   rm $CRASH_HOME/modcrc.tmp
fi
