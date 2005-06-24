#!/bin/sh
set -x

. ~/crash/crashenv

# Generate a single table of contents for all .bin/.txt pairs
cd $CRASH_HOME
perl aehtab.pl $CRASH_WEBHOME $CRASH_HOME/types.txt $CRASH_HOME/aehtab.in
if [ -s $CRASH_WEBHOME/index.tmp ]; then
	mv $CRASH_WEBHOME/index.tmp $CRASH_WEBHOME/index.html
elif [ -f $CRASH_WEBHOME/index.tmp ]; then
	rm -f $CRASH_WEBHOME/index.tmp
fi

echo Genindex done.
