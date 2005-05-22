#!/bin/sh
# Gets a new copy of servers.txt
# Create new remote server runsrvfil page.
# Run this every day from crontab
set -x

. ~/anetmon/anetmonenv

cd $ANETMON_HOME
sh getservers.sh
perl mkforms.pl
if [ ! -d $ANETMON_WEBHOME ]; then
	mkdir $ANETMON_WEBHOME
fi
if [ -s runsrvfil.html ]; then
   mv runsrvfil.html $ANETMON_WEBHOME
elif [ -f runsrvfil.html ]; then
   rm -f runsrvfil.html
fi
