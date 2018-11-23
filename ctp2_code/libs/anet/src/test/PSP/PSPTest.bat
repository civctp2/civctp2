echo off
rem Run the PSP test.
rem
rem This is set up to run a distributed PSP test.  It requires that
rem a server be running on iserv.activision.com, port 4000.  Each
rem machine running the test should be assigned a different set of
rem the tests to be run, and the -b (begin at) and -e (end before)
rem switches should be modified to reflect those choices (e.g., if
rem the machine is expected to run tests 4, 5, 6,and 7, then the
rem switches should be set to -b=4 -e=8).  The first test is 0, and
rem the last test is one less than the -c= switch value.

if not exist anet2.dll call getdll.bat

rem *******************************************************************
rem Run the test
rem *******************************************************************
echo Be sure your -b and -e switches have been modified correctly.
echo Press <space> to run test, ^C to abort and check switches.
pause
PSP2dd\PSP2dd -b=0 -e=12 -c=12 -t=30000 -p=4000 -d=winets2d.dll -s=iserv.activision.com:4000 -l=pspRun.log

:end