rem Send a src distribution file anetlin.zip to build server
rem  and have build server compile the new source and send back a new
rem  game distribution file linux86.zip

cd ..\3rdparty
wmake unpack
cd ..\..
del anetlin.zip
zip -r anetlin.zip h src\dp src\aeh src\tca src\score src\antp src\bhttp -x@src\eksclude
if errorlevel 1 goto ziperror
zip -r anetlin.zip src\linux src\test\load src\test\bench src\test\dpio src\test\dptab -x@src\eksclude
if errorlevel 1 goto ziperror
zip -r anetlin.zip src\test\test.sh src\test\pingt src\rembld demo\utils demo\gtest -x@src\eksclude
if errorlevel 1 goto ziperror
zip -r anetlin.zip src\sbd src\3rdparty\d3des src\3rdparty\md5 -x@src\eksclude
if errorlevel 1 goto ziperror
zip -0 anetlin.zip src\3rdparty\mysql.jar -x@src\eksclude
if errorlevel 1 goto ziperror
cd src\rembld

if exist ..\..\anetlin.zip goto ok1
:ziperror
  del anetlin.zip
  echo Error: can't make zipfile of sources ..\..\anetlin.zip
  goto error
:ok1

call vars.bat
deltree /y ..\..\linux
mkdir ..\..\linux
javac Client.java
if errorlevel 1 goto error

rem java -classpath .;%MYCLASSPATH% Client iserv.activision.com ..\..\anetlin.zip ..\..\linux86.zip ..\..\bldlinux.log 6789
rem java -classpath .;%MYCLASSPATH% Client 207.82.188.81 ..\..\anetlin.zip ..\..\linux86.zip ..\..\bldlinux.log 6789
java -classpath .;%MYCLASSPATH% Client 206.17.228.148 ..\..\anetlin.zip ..\..\linux86.zip ..\..\bldlinux.log 6789
if errorlevel 1 goto error
rem java -classpath .;%MYCLASSPATH% Client 206.17.228.140 ..\..\anetlin.zip ..\..\linux86-libc5.zip ..\..\bldlinux-libc5.log 6789
if errorlevel 1 goto error

if not exist ..\..\linux86.zip goto error
rem if not exist ..\..\linux86-libc5.zip goto error
rem if not exist ..\..\linux86-glibc.zip goto error

rem Pass success status back to anbuild\manbuildss.bat 
set BUILDSRVRESULT=0
echo Build of ..\..\linux86.zip successful.
goto end

:error
rem Pass failure status back to anbuild\manbuildss.bat 
set BUILDSRVRESULT=1
echo Build of ..\..\linux86.zip aborted.

:end
