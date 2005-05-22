#!/bin/sh
set -e

perl htget.pl -o servers.txt -q -t 300 http://updater.activision.com/updater/etc/servers.txt
perl htget.pl -o types.txt -q -t 300 http://updater.activision.com/updater/etc/types.txt
