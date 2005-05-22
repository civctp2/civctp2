#!/bin/sh
# Monitor number of players in each game supported by the Activenet game servers.
# Result placed in $ANETMON_WEBHOME/game.html
# Gets info by downloading server status pages at http://$server/anet2/index.html
# Gets list of servers from file servers.txt
# Run this every 15 minutes from crontab

. ~/anetmon/anetmonenv

cd $ANETMON_HOME

if [ ! -f DownloadGameServerFile.class  ]; then
	$JAVADIR/bin/javac DownloadGameServerFile.java
fi

# Download index.html from all game servers to /tmp/0.html, /tmp/1.html, ...
$JAVADIR/bin/java -classpath $JAVADIR/lib/classes.zip:. DownloadGameServerFile -f anet2/index.html -s file://$ANETMON_SERVERS -p ckgametmp$$.

# Add up all the statistics from game server index.html's and output summary
perl mkgame.pl ckgametmp$$.
if [ ! -d $ANETMON_WEBHOME ]; then
	mkdir $ANETMON_WEBHOME
fi
if [ -s game.html ]; then
   mv game.html $ANETMON_WEBHOME
elif [ -f game.html ]; then
   rm -f game.html
fi

# delete the temporary files
rm -f /tmp/ckgametmp$$.*.html
