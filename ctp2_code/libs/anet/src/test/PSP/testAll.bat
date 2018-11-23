echo off
rem *******************************************************************
rem Run the PSP tests.
rem
rem Usage: testAll [log_file]
rem
rem This batch file sets up and runs the available PSP tests,
rem appending the results to log_file, or to con: if log_file is
rem not specified.  Tests are run until one of them fails, upon
rem which the batch file writes results and terminates.
rem
rem $Log: testAll.bat $
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
if exist anet2.dll goto pspdllok
call getdll.bat
:pspdllok


rem *******************************************************************
rem PSP Tests (is large enough player list visible?)
rem First, run with loopback driver
rem *******************************************************************

rem Build test files
nmake -f PSP.MAK CFG="PSP - Win32 v2 dll Release"

rem Can it even run?
set TESTNAME=PSP, 2 players, no packet loss, loopback driver
PSP2dr\PSP2dr -c=2 -i=0 -t=3000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP2.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%

goto skipPsp3x10Loop
rem Can it run with a lot of entering and exiting
set TESTNAME=PSP, 3 players, 10 times, no packet loss, loopback driver
PSP2dr\PSP2dr -c=3 -n=10 -i=0 -t=3000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP2.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:skipPsp3x10Loop

rem Can it run with high packet loss?
set TESTNAME=PSP, 2 players, 40% packet loss, loopback driver
PSP2dr\PSP2dr -c=2 -i=40 -t=3000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP2b.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%

rem Does it work with lots of players?
set TESTNAME=PSP, 14 players, 10% packet loss, loopback driver
PSP2dr\PSP2dr -c=14 -i=10 -t=9000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP14.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%


rem *******************************************************************
rem PSP Tests (is large enough player list visible?)
rem Run with internet driver
rem *******************************************************************

rem Build test files
rem nmake -f PSP.MAK CFG="PSP - Win32 v2 dll debug"

rem Can it even run?
set TESTNAME=PSP, 2 players, no packet loss, loopback driver
PSP2dr\PSP2dr -c=2 -i=0 -t=3000 -p=20064 -d=..\..\..\win\dll\winets2d.dll -s=california3.activision.com -l=PSP2.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%

goto skipPsp3x10Inet
rem Can it run with a lot of entering and exiting
set TESTNAME=PSP, 3 players, 10 times, no packet loss, loopback driver
PSP2dr\PSP2dr -c=3 -n=10 -i=0 -p=21064 -t=3000 -d=..\..\..\win\dll\winets2d.dll -s=california3.activision.com -l=PSP2.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%
:skipPsp3x10Inet

rem Can it run with high packet loss?
set TESTNAME=PSP, 2 players, 40% packet loss, loopback driver
PSP2dr\PSP2dr -c=2 -i=40 -p=22064 -t=3000 -d=..\..\..\win\dll\winets2d.dll -s=california3.activision.com -l=PSP2b.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%

rem Does it work with lots of players?
set TESTNAME=PSP, 14 players, 10% packet loss, loopback driver
PSP2dr\PSP2dr -c=14 -i=10 -p=23064 -t=9000 -d=..\..\..\win\dll\winets2d.dll -s=california3.activision.com -l=PSP14.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%


rem *******************************************************************
rem Cleanup and Error Handling
rem *******************************************************************
:cleanup
rem Delete trash
rm *.dll
rm *.log
deltree /y PSP2dr

goto end
:error
echo %TESTNAME% test failed. >>%TESTFILE%
:end

