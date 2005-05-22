rem Usage: loopio N [1 or 2]
rem Run first with N=1 for host, and second with N=2 for client
rem Two-host test for freeze/thaw support in dpio.  Both hosts behave so:
rem  Initialize dpio (this allocates address 1.0.0.0 and 2.0.0.0, respectively)
rem  wait for a few seconds in case other host hasn't started yet
rem  open handle to other host
rem  send reliable packet to other host explicitly (non-broadcast)
rem  freeze
rem  quit and start new gtest
rem  thaw
rem  send reliable packet to other host explicitly (non-broadcast)
rem The test succeeds if both hosts receive both packets.
rem Gtest can't currently check to make sure both are received,
rem so you have to eyeball it.
rem If this test passes, run loopdp.bat, which tests the same thing using
rem dp rather than dpio.

rem Determine other host's address (depends on way wloopd.dll works)
set other=
if #%1==#1 set other=2
if #%1==#2 set other=1
rem Make sure set succeeded
if %other%#==# goto bad

rem Prepare script u%1.txt for this player.
del u%1.txt
echo dpio_create             > u%1.txt
echo sleep 4                 >> u%1.txt
echo dpio_open %other%.0.0.0 >> u%1.txt
echo wait                    >> u%1.txt
echo dpio_put_reliable 2     >> u%1.txt
echo sleep 3                 >> u%1.txt
echo dpio_freeze             >> u%1.txt
echo quit                    >> u%1.txt

rem Prepare script u%1.tx2 for this player.
del u%1.tx2
echo dpio_thaw               > u%1.tx2
echo sleep 3                 >> u%1.tx2
echo dpio_put_reliable 2     >> u%1.tx2
echo sleep 10000             >> u%1.tx2

rem Run a new copy of gtest in its own window, using the win95 'start' command.
rem Must run in its own directory to get its own log file, since
rem we can't specify .ini file or .log file name on commandline
deltree /y user%1
mkdir user%1
chdir user%1
copy ..\gtest.ini
del *.log
..\..\..\..\win\bin\gtestd -c -s -n:..\..\..\..\win\dll\wloopd.dll < ..\u%1.txt
ren dp.log 1.log
..\..\..\..\win\bin\gtestd -c -s -n:..\..\..\..\win\dll\wloopd.dll < ..\u%1.tx2
ren dp.log 2.log
:bad
