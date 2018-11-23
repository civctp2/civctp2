#!/bin/sh

ps aux | grep sbdtest
( sleep 2; ./sbdtestc iserv $1 $2 > sbdc.out; ) &
./sbdtests $1 $2 $3
