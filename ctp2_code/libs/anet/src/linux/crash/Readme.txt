***** Activenet Crash Log Analysis Server ****

Configuration files:
crashenv	- sets location of input and output files
aehtab.in	- template for old daily crash analysis web page
crontab.lst	- example crontab to get analysis to run automatically

Web interface:
These files should be placed in $CRASH_WEB
logs.html	- an index and cgi form for searching the crash logs.
logsearch.cgi	- a cgi script for searching the crash logs.

Main scripts:
dosplit.sh	- downloads crashlogs, splits them into subdirectories by session
			  type and build, and analyzes them.  The new main script.
dodays.sh	- downloads and analyzes crash logs, and outputs old daily
			  summary web page for today and N previous days

Utilities used internally:
aehsplit	- splits a binary crash log up by session type and build into
			  subdirectories
analcrsh	- analyzes binary crash logs and outputs human readable logs.
procserv	- processes binary crash logs in various ways.
DownloadGameServerFile.java	- download a file from multiple hosts, save in /tmp
aehtab.pl	- generates old daily HTML index for analyzed crash logs
genindex.sh	- invokes aehtab
getservers.sh	- downloads server list and game list files from master server
htget.pl	- downloads a file via http
onedate.sh	- downloads, analyses, and outputs old daily web page for the
			  given day
updatmap.sh	- downloads game map files

Data files generated:
aehsplit.dat	- tracks current location in binary log.  Don't delete unless
				  you want to reanalyze everything!
*.bin			- binary crash logs

The new dosplit.sh script sets up a logs directory in $CRASH_WEB by default,
which contains aehsplit.dat and all the directory structure aehsplit creates.

The old dodays.sh script lives in a crash subdirectory of $CRASH_WEB by
default, and sets up its directory structure and web pages there.
