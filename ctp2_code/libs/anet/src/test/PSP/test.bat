echo off
rem *******************************************************************
rem Run the PSP tests.
rem
rem Usage: test [log_file]
rem
rem This batch file sets up and runs the available PSP tests,
rem appending the results to log_file, or to con: if log_file is
rem not specified.  Tests are run until one of them fails, upon
rem which the batch file writes results and terminates.
rem
rem $Log: test.bat $
rem Revision 1.1  1997/10/01 00:15:49  lbennett
rem Initial revision
rem Revision 1.2  1997/09/30 06:39:08  lbennett
rem Changed to release version.
rem Revision 1.1  1997/09/29 18:08:38  lbennett
rem Initial revision
rem *******************************************************************


rem *******************************************************************
rem Setup
rem *******************************************************************

rem Check for argument, set TESTFILE to desired output file
if x%1x NEQ xx goto testfileok
set TESTFILE=con:
goto testfiledone
:testfileok
set TESTFILE=%1
:testfiledone

rem Ensure necessary dlls are present.
if exist anet2d.dll goto pspdllok
call getdll.bat
:pspdllok


rem *******************************************************************
rem PSP Tests (is large enough player list visible?)
rem First, run with loopback driver
rem *******************************************************************

rem Build test files
nmake -f PSP.MAK CFG="PSP - Win32 v2 dll debug"

goto Loop2Skip
rem Can it even run?
set TESTNAME=PSP, 2 players, no packet loss, loopback driver
PSP2dd\PSP2dd -c=2 -i=0 -t=3000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP2.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Loop2Skip

goto Loop3x10Skip
rem Can it run with a lot of entering and exiting
set TESTNAME=PSP, 3 players, 10 times, no packet loss, loopback driver
PSP2dd\PSP2dd -c=3 -n=10 -i=0 -t=3000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP3.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Loop3x10Skip

rem goto Loop10allSkip
rem Does it work with a bit of everything?
set TESTNAME=PSP, 3 players, 7 times, a group, 5 player vars, no packet loss, loopback driver
PSP2dd\PSP2dd -c=3 -n=7 -g -t=9000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP3a.log -v=5
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Loop10allSkip

rem goto Loop2z40Skip
rem Can it run with high packet loss?
set TESTNAME=PSP, 2 players, 40% packet loss, loopback driver
PSP2dd\PSP2dd -c=2 -i=40 -t=3000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP2l.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Loop2z40Skip

goto Loop14Skip
rem Does it work with lots of players?
set TESTNAME=PSP, 14 players, 10% packet loss, loopback driver
PSP2dd\PSP2dd -c=14 -i=10 -t=9000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP14.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Loop14Skip

rem goto Loop10gSkip
rem Does it work with lots of players entering a group?
set TESTNAME=PSP, 10 players, a group, 10% packet loss, loopback driver
PSP2dd\PSP2dd -c=10 -i=10 -t=9000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP10g.log -g
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Loop10gSkip

rem goto Loop8vSkip
rem Does it work with lots of player variables?
set TESTNAME=PSP, 8 players, 20 player vars, no packet loss, loopback driver
PSP2dd\PSP2dd -c=8 -t=9000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP8v.log -v=20
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Loop8vSkip

rem *******************************************************************
rem PSP Tests (is large enough player list visible?)
rem Run with internet driver
rem *******************************************************************

rem Build test files
rem nmake -f PSP.MAK CFG="PSP - Win32 v2 dll debug"

goto Inet2Skip
rem Can it even run?
set TESTNAME=PSP, 2 players, no packet loss, internet driver
PSP2dd\PSP2dd -c=2 -i=0 -t=3000 -p=20064 -d=..\..\..\win\dll\winets2d.dll -s=california3.activision.com -l=PSP2i.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Inet2Skip

goto Inet3x10Skip
rem Can it run with a lot of entering and exiting
set TESTNAME=PSP, 3 players, 10 times, no packet loss, internet driver
PSP2dd\PSP2dd -c=3 -n=10 -i=0 -p=21064 -t=3000 -d=..\..\..\win\dll\winets2d.dll -s=california3.activision.com -l=PSP3i.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Inet3x10Skip

goto Inet10allSkip
rem Does it work with a bit of everything?
set TESTNAME=PSP, 3 players, 7 times, a group, 5 player vars, no packet loss, internet driver
PSP2dd\PSP2dd -c=3 -n=7 -p=23064 -t=9000 -d=..\..\..\win\dll\winets2d.dll -s=california3.activision.com -l=PSP3ia.log -v=5 -g
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Inet10allSkip

goto Inet2z40Skip
rem Can it run with high packet loss?
set TESTNAME=PSP, 2 players, 40% packet loss, internet driver
PSP2dd\PSP2dd -c=2 -i=40 -p=22064 -t=3000 -d=..\..\..\win\dll\winets2d.dll -s=california3.activision.com -l=PSP2il.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Inet2z40Skip

goto Inet14Skip
rem Does it work with lots of players?
set TESTNAME=PSP, 14 players, 10% packet loss, internet driver
PSP2dd\PSP2dd -c=14 -i=10 -p=23064 -t=9000 -d=..\..\..\win\dll\winets2d.dll -s=california3.activision.com -l=PSP14i.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Inet14Skip

goto Inet10gSkip
rem Does it work with lots of players entering a group?
set TESTNAME=PSP, 10 players, a group, 10% packet loss, internet driver
PSP2dd\PSP2dd -c=10 -i=10 -p=23064 -t=9000 -d=..\..\..\win\dll\winets2d.dll -s=california3.activision.com -l=PSP10ig.log -g
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Inet10gSkip

goto Inet8vSkip
rem Does it work with lots of player variables?
set TESTNAME=PSP, 8 players, 20 player vars, no packet loss, internet driver
PSP2dd\PSP2dd -c=8 -p=23064 -t=9000 -d=..\..\..\win\dll\winets2d.dll -s=california3.activision.com -l=PSP8iv.log -v=20
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:Inet8vSkip

rem *******************************************************************
rem Cleanup and Error Handling
rem *******************************************************************
:cleanup
rem Delete trash
rm *.dll
rm *.log
deltree /y PSP2dd

goto end
:error
echo %TESTNAME% test failed. >>%TESTFILE%
:end

