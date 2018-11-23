rem Example of how to run multiple copies of gtest on one machine
rem without using the LAN.
rem Usage: spawn N [gamename]
rem If N is 0, hosts a game named gamename.
rem If N is greater than 0, joins the first game named gamename.
rem If gamename is not specified, defaults to "Demo Game".
rem
rem Note: to quit all copies of gtest with a single command,
rem join the game, create a player, and issue the 'remotequit' command.
rem You might also need to check 'close on exit' in the Properties
rem field (hiding in a pulldown from the upper-left-hand-corner of the
rem window) of one of the gtest windows.

rem Prepare script u%1.txt for this player.
del u%1.txt
if not %1==0 goto join
rem User zero hosts a game
echo host %2    > u%1.txt
goto user
:join
echo sess       > u%1.txt
echo wait       >> u%1.txt
echo join %2    >> u%1.txt
echo wait       >> u%1.txt
:user
echo new user%1 >> u%1.txt
echo wait       >> u%1.txt
echo sleep 10000>> u%1.txt

rem Run a new copy of gtest in its own window, using the win95 'start' command.
rem Must run in its own directory to get its own log file, since
rem we can't specify .ini file or .log file name on commandline
mkdir user%1
chdir user%1
copy ..\gtest.ini
rem ..\..\..\..\win\bin\gtestd -s -n:..\..\..\..\win\dll\wloopd.dll < ..\u%1.txt > o%1.txt
rem del u%1.txt
..\..\..\..\win\bin\gtestd -s -n:..\..\..\..\win\dll\wloopd.dll < ..\u%1.txt
