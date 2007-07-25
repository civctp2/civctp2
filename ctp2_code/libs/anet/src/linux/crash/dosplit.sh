#!/bin/sh
set -e
set -x
. ~/crash/crashenv

cd $CRASH_HOME
perl htget.pl -t 300 -o $CRASH_WEB/logs/atvilog.bin http://aeh.activision.com/anet2/atvilog.bin
./aehsplit -c $CRASH_WEB/logs $CRASH_WEB/logs/atvilog.bin
find $CRASH_WEB/logs/*/ -name '*.bin' -print | ./analcrsh -u -p -r ../txt -m maps/modcrc.txt
find $CRASH_WEB/logs/*/ -mindepth 1 -maxdepth 1 -name '*.*' -type d -exec /bin/sh -c "cd {} ; perl $CRASH_HOME/summary.pl txt/*.txt" \;
