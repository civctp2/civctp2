#!/bin/sh
# usage: runtest.sh port npkts timeout nclients

ps aux | grep sbdtest
toread=`perl -e "print ($2*$4);"`
( sleep 2; i=0; while [ $i -lt $4 ]; do ./sbdtestc iserv $1 $2 > sbdc$i.out & i=`perl -e "print $i+1;"`; done; ) &
./sbdtests $1 $toread $3
