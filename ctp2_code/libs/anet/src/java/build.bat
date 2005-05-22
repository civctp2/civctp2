@echo off
rem This is pretty poor, but I'm using absolute paths for the moment,
rem until we're more comfortable building things in Java.
rem $Log: build.bat $
rem Revision 1.9  1997/09/09 22:35:06  anitalee
rem copy dp2 dll versions to \win\dll.
rem Revision 1.8  1997/08/13 20:39:27  dkegel
rem Also install the debugging version.
rem Revision 1.7  1997/07/16 03:23:57  dkegel
rem Use double-quotes when testing MYCLASSPATH for benefit of nt's cmd.exe
rem Revision 1.6  1997/07/11 00:26:10  dkegel
rem Avoid setting global environment variables.
rem Set classpath explicitly on commandline.
rem Obey global environment variables JDK and ASDK.
rem Revision 1.5  1997/06/23 03:18:23  dkegel
rem Added doc!  And put the native DLL in the native DLL folder (duh).


rem Note directory name (Activision) must be same as Java package name.
cd Activision
call build.bat
cd ..
if errorlevel 1 goto end

call vars.bat
if not "%MYCLASSPATH%"=="" goto ok
echo Error setting variables.
goto end
:ok

rem Put the class file into the output java folder
deltree /y %MYASDK%\java
mkdir %MYASDK%\java

jar -0cvf %MYASDK%\java\atvi.jar Activision\*.class 

rem Put the native DLL into the output folder for the architecture in question
copy Activision\ANetj.dll %MYASDK%\win\dll\ANetj.dll
copy Activision\ANetjd.dll %MYASDK%\win\dll\ANetjd.dll
copy Activision\ANetj2.dll %MYASDK%\win\dll\ANetj2.dll
copy Activision\ANetj2d.dll %MYASDK%\win\dll\ANetj2d.dll
rem copy Activision\Updatej.dll %MYASDK%\win\dll\Updatej.dll

rem Put the doc for the Java class into the doc directory
mkdir %MYASDK%\doc\java
copy Activision\*.html %MYASDK%\doc\java

:end

set MYJDK=
set MYASDK=
set MYCLASSPATH=
