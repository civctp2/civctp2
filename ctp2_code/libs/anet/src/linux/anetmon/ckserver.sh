#!/bin/sh
# Monitor response time of the Activenet game servers.
# Result placed in $ANETMON_WEBHOME/server.html
# Gets info by running old load app
# Gets list of servers from file servers.txt
# Run this every hour from crontab
set -x

. ~/anetmon/anetmonenv

cd $ANETMON_HOME
cd load
room=`awk '{printf "\$%s\|Chat\|", $2}' $ANETMON_SERVERS`
./load -c=1 -t=0 -r="$room!" -g=65 -s=- -k -e=dkegel@activision.com,dank@alumni.caltech.edu
cd ..
if [ -f /tmp/senderr.tmp ]; then
   rm -f /tmp/senderr.tmp
fi
perl mkserv.pl
if [ ! -d $ANETMON_WEBHOME ]; then
	mkdir $ANETMON_WEBHOME
fi
if [ -s server.html ]; then
   mv server.html $ANETMON_WEBHOME
elif [ -f server.html ]; then
   rm -f server.html
fi
