General Files:
------------------------------------------------------------------------------
Readme.txt	- This file
crontab.lst	- A sample crontab file for keeping anetmon up to date
anetmonenv	- General configuration file for anetmon
getservers.sh	- Script to download servers.txt from update server
htget.pl	- perl script to fetch a web page via http

Files involved in generating game.html, the over-all-servers game stats page:
------------------------------------------------------------------------------
ckgame.sh	- The top level script to generate game.html
DownloadGameServerFile.java	- downloads stats file from servers
mkgame.pl	- Compiles the stats into game.html
game.in		- Template for game.html used by mkgame.pl

Files involved in generating server.html, the server health summary page:
------------------------------------------------------------------------------
ckserver.sh	- The top level script to generate server.html
load/load	- Anet program for testing server health.
	Note: load is obsolete and should be replaced by a bench script.
mkserv.pl	- Compiles the load stats into server.html 
server.in	- Template for server.html used by mkserv.pl

Files involved in generating runsrvfil.html, the server updater form:
------------------------------------------------------------------------------
daily.sh	- The top level script to generate runsrvfil.html.
	Also downloads servers.txt, which is used by all scripts.
mkforms.pl	- Expands runsrvfil.in using the server list servers.txt
runsrvfil.in	- Template for runsrvfil.html

Obsolete files, soon to be deleted
------------------------------------------------------------------------------
ckgame1.sh
mkgame1.pl
game1.in
