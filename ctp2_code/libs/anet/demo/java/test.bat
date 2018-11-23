@echo off
rem Batch file to compile and run java test program for Activenet.
rem Test program should connect to a server and join a session on it,
rem then pick an installed Activenet application at random and
rem launch it in that session.
rem
rem Reads environment variables JDK and ASDK if set.
rem Sets environment variables MYJDK, MYASDK, and MYCLASSPATH.

set MYJDK=%JDK%
set MYJRE=%JRE%
set MYASDK=%ASDK%

if %MYJDK%.==.  set MYJDK=d:\jdk
if %MYJRE%.==.  set MYJRE=d:\jre
if %MYASDK%.==. set MYASDK=d:\mw2\commdll
set MYCLASSPATH=%MYJDK%\lib\classes.zip;%MYASDK%\java\atvi.jar;.
rem set MYCLASSPATH=%MYJRE%\lib\rt.jar;%MYASDK%\java\atvi.jar;.

if not "%MYCLASSPATH%"=="" goto test
echo Out of environment space
goto end

:test
if exist %MYJDK%\lib\classes.zip goto ok
echo Bad JDK location; can't find %MYJDK%\lib\classes.zip 
goto end

:ok
javac -classpath %MYCLASSPATH% ExecApp.java
if errorlevel 1 goto end
javac -classpath %MYCLASSPATH% test.java
if errorlevel 1 goto end
mkdir anet

rem Pick one DLL
deltree /y anet
mkdir anet
rem For loopback testing - as if IPX
rem copy %MYASDK%\win\dll\wloopnb.dll anet
rem For loopback testing - as if internet
rem copy %MYASDK%\win\dll\wloopnb.dll anet
rem For Internet testing
rem copy %MYASDK%\win\dll\winet.dll anet
copy %MYASDK%\win\dll\winets2.dll anet
rem For IPX testing
rem copy %MYASDK%\win\dll\wipx2.dll anet
rem For modem testing
rem copy %MYASDK%\win\dll\wmodem2.dll anet
rem For null modem testing
rem copy %MYASDK%\win\dll\wserial2.dll anet

rem For updater testing
rem copy %MYASDK%\win\dll\updatej.dll 
rem if not exist updatej.dll echo updatej.dll not found
rem copy %MYASDK%\src\win\3rdparty\updater\dkupddll.dll 
rem if not exist dkupddll.dll echo dkupddll.dll not found

copy %MYASDK%\win\dll\anetj2.dll anet
copy %MYASDK%\win\dll\anet2d.dll anet2.dll

rem No longer need exec.exe; we now use a native method for that.
rem if not exist exec.exe cl exec.c
rem if errorlevel 1 goto end

java -classpath %MYCLASSPATH% test
rem jre -classpath %MYCLASSPATH% test
:end

rem Clear local environment variables.
set MYJDK=
set MYJRE=
set MYASDK=
set MYCLASSPATH=
