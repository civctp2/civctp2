rem Test host migration.
rem Meant to be called by mig4.bat.
rem
rem Usage: mig N [gamename]
rem If N is 0, hosts a game named gamename.
rem If N is greater than 0, joins the first game named gamename.
rem Host will close after ten seconds.  Other machines will try
rem to create players after he quits.  This will fail unless host
rem migration is working.

rem Prepare script u%1.txt for this player.
del u%1.txt
if not %1==0 goto join
rem User zero hosts a game
echo host %2 10   > u%1.txt
goto user
:join
echo sess       > u%1.txt
echo wait       >> u%1.txt
echo join %2    >> u%1.txt
echo wait       >> u%1.txt
:user
echo new user%1 >> u%1.txt
echo wait       >> u%1.txt
if not %1==0 goto sleep 
rem user 0 quits a few seconds into the game
echo sleep 10>> u%1.txt
echo dpClose>> u%1.txt
:sleep
echo sleep 12>> u%1.txt
echo newp foo%1>> u%1.txt
echo sleep 10000>> u%1.txt

rem Run a new copy of gtest in its own window, using the win95 'start' command.
rem Must run in its own directory to get its own log file, since
rem we can't specify .ini file or .log file name on commandline
mkdir user%1
chdir user%1
copy ..\mig.ini gtest.ini
rem ..\..\..\..\win\bin\gtestd -s -n:..\..\..\..\win\dll\wloopd.dll < ..\u%1.txt > o%1.txt
rem del u%1.txt
copy ..\..\..\..\win\dll\anet2d.dll
..\..\..\..\win\bin\gtest2dd -s -n:..\..\..\..\win\dll\wloopd.dll < ..\u%1.txt
