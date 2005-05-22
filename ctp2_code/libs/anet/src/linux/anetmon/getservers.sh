#!/bin/sh
. ~/anetmon/anetmonenv

perl htget.pl -o /tmp/servers$$.txt -q -t 300 http://updater.activision.com/updater/etc/servers.txt
if grep -q -i '<html>' /tmp/servers$$.txt; then
	rm /tmp/servers$$.txt
else
	mv /tmp/servers$$.txt $ANETMON_SERVERS
fi
