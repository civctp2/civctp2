#!/bin/sh
#Download server status pages and compile the information into
#/home/httpd/html/etc/game.html

JAVADIR=/usr/local/jdk1.1.3
cd ~/etc
$JAVADIR/bin/java -classpath $JAVADIR/lib/classes.zip:. DownloadGameServerFile -f anet2/servstat.html
perl mkgame1.pl
if [ -s game1.html ]; then
   mv game1.html /home/httpd/html/etc
elif [ -f game1.html ]; then
   rm -f game1.html
fi
i=0
while [ -f /tmp/$i.html ]; do
   rm -f /tmp/$i.html
   i=$[$i+1]
done
