#!/bin/sh
echo Starting dodays.sh
# Download all logs up until $1 days before today,
# download all maps, and analyze all logs.
# Call daily with an argument of 0.
# To set up a crash monitor initially, call with a
# large argument (365, say).
set -e
set -x

. ~/crash/crashenv

if [ $# != 1 ]; then
	echo "Usage: sh dodays.sh <daysold>"
	echo "    Gets and processes all logs up to <daysold> days before today."
	exit 1
fi
daysold=$1

# this is here so we can run this script from crontab
cd $CRASH_HOME

# get a full set of game map files
sh $CRASH_HOME/getservers.sh
sh $CRASH_HOME/updatmap.sh

# download all logs from i days before today
i=$daysold
while [ $i -ge 0 ]; do
    Date=`date --date "$i days ago" '+%y-%m-%d'`
	if sh $CRASH_HOME/onedate.sh $Date ; then
		true
	else
		echo "onedate failed"
		exit 1
	fi
    i=$[$i-1]
done

# generate html table
sh $CRASH_HOME/genindex.sh
