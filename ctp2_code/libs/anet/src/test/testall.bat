echo off
rem *******************************************************************
rem Run the ActiveNet library tests.
rem
rem Usage: testAll [log_file]
rem
rem This batch file sets up and runs the available library tests,
rem writing the results to log_file, or to con: if log_file is
rem not specified.  Tests are run until one of them fails, upon
rem which the batch file writes results and terminates.
rem *******************************************************************


rem *******************************************************************
rem Setup
rem *******************************************************************

set TESTDIR=d:\mw2\commdll\src\test

rem Check for argument, set TESTFILE to desired output file
if NOT x%1x==xx goto tfileok
set TESTFILE=con:
goto tfiledone
:tfileok
set TESTFILE=%1
:tfiledone

rem *******************************************************************
rem PSP Tests (is large enough player list visible?)
rem *******************************************************************

rem Move to correct directory and build test files
cd PSP
nmake -f PSP.MAK CFG="PSP - Win32 v2 dll debug"

rem Ensure necessary dlls are present.
if exist anetdll.dll goto pspdllok
call getdll.bat
:pspdllok

rem Can it even run?
set TESTNAME=PSP, 2 players, no packet loss, loopback driver
PSP2dd\PSP2dd -c=2 -i=0 -t=3000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP2.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%

rem Can it run with high packet loss?
set TESTNAME=PSP, 2 players, 30% packet loss, loopback driver
PSP2dd\PSP2dd -c=2 -i=30 -t=3000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP2b.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%

rem Does it work under load?
set TESTNAME=PSP, 14 players, 10% packet loss, loopback driver
PSP2dd\PSP2dd -c=14 -i=10 -t=9000 -d=..\..\..\win\dll\wloop2d.dll -l=PSP14.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%

rem Can it even run under internet?
set TESTNAME=PSP, 2 players, no packet loss, internet driver
PSP2dd\PSP2dd -c=2 -i=0 -t=3000 -p=20064 -d=..\..\..\win\dll\winets2d.dll -s=california12.activision.com -l=PSPi2.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%

rem Delete trash and restore directory
rm *.dll
rm *.log
deltree /y PSPdd
cd ..

rem *******************************************************************
rem TCA and TServ self tests
rem *******************************************************************
rem Make a directory to hold the log files
deltree /y tca
mkdir tca

rem Move to correct directory, build and run tcat test
cd ..\tca
set TESTNAME=TCA self-test
call mktcat.bat
tcat\tcat.exe > ..\test\tca\tcat.log
if ERRORLEVEL 1 goto error
type tcat\tca_speed.log >> ..\test\tca\tcat.log
echo %TESTNAME% test passed. >>%TESTFILE%

rem Build and run tserv SIMNET test
set TESTNAME=TServ SIMNET, 0%, 45% send retries
call mktts.bat
tservts\tservts.exe > ..\test\tca\tserv.log
if ERRORLEVEL 1 goto error
echo %TESTNAME% test passed. >>%TESTFILE%

rem Delete trash and restore directory
deltree /y tservts
deltree /y tcat
cd ..\test

rem *******************************************************************
rem Cleanup and Error Handling
rem *******************************************************************
set LIBTESTRESULT=0
echo src\test\testall.bat succeeded. >>%TESTFILE%
echo src\test\testall.bat succeeded. 
goto end
:error
set LIBTESTRESULT=1
echo src\test\testall.bat failed, because test %TESTNAME% failed. >>%TESTFILE%
echo src\test\testall.bat failed, because test %TESTNAME% failed. 
:end
