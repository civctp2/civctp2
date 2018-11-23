rem Run first with N=1 for host, and second with N=2 for client
rem Two-host test for freeze/thaw support in dp.  Both hosts behave so:
rem  Initialize dp (this allocates address 1.0.0.0 and 2.0.0.0, respectively)
rem  server creates session; client joins session
rem  send reliable packet to other host explicitly (non-broadcast)
rem  freeze
rem  quit and start new gtest
rem  thaw
rem  send reliable packet to other host explicitly (non-broadcast)
rem The test succeeds if both hosts receive both packets.
rem Gtest can't currently check to make sure both are received,
rem so you have to eyeball it.
rem If this test passes, have the client try to create a new player after
rem the thaw; this failed in an earlier test.

if #%1==#1 goto host
if #%1==#2 goto client
echo Usage: loopdp N [1 or 2]
goto bad

:host
rem Prepare script u%1.txt for this player.
del u%1.txt
echo host loopdp             > u%1.txt
echo newplayer theHost       >> u%1.txt
echo wait                    >> u%1.txt
echo sleep 10                >> u%1.txt
echo players                 >> u%1.txt
echo assert n_players != 1   >> u%1.txt
echo send thisPlayer anyPlayer>> u%1.txt
echo sleep 10                >> u%1.txt
echo quit                    >> u%1.txt

goto run

:client
rem Prepare script u%1.txt for this player.
del u%1.txt
echo sleep 1                 >  u%1.txt
echo sessions                >> u%1.txt
echo wait                    >> u%1.txt
echo assert n_sessions == 1  >> u%1.txt
echo join loopdp             >> u%1.txt
echo wait                    >> u%1.txt
echo newplayer theClient     >> u%1.txt
echo wait                    >> u%1.txt
echo players                 >> u%1.txt
echo assert n_players == 2   >> u%1.txt
echo send thisPlayer anyPlayer>> u%1.txt
echo sleep 4                 >> u%1.txt
echo freeze                  >> u%1.txt
echo quit                    >> u%1.txt

rem Prepare script u%1.tx2 for this player.
del u%1.tx2
echo thaw                     > u%1.tx2
echo players                  >> u%1.tx2
echo assert n_players == 2    >> u%1.tx2
echo send thisPlayer anyPlayer>> u%1.tx2
echo sleep 2                  >> u%1.tx2
echo newplayer extraPlayer    >> u%1.tx2
echo wait                     >> u%1.tx2
echo players                  >> u%1.tx2
echo assert n_players == 3    >> u%1.tx2
echo quit                     >> u%1.tx2

goto run

:run
rem Run a new copy of gtest in its own window, using the win95 'start' command.
rem Must run in its own directory to get its own log file, since
rem we can't specify .ini file or .log file name on commandline
deltree /y user%1
mkdir user%1
chdir user%1
copy ..\gtest.ini
del *.log
..\..\..\..\win\bin\gtestd -s -n:..\..\..\..\win\dll\wloopd.dll < ..\u%1.txt
ren dp.log 1.log
if #%1==#1 goto done
..\..\..\..\win\bin\gtestd -c -s -n:..\..\..\..\win\dll\wloopd.dll < ..\u%1.tx2
ren dp.log 2.log
:done
:bad
