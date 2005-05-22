@echo off
rem Batch file to build the Activenet java binding.

call ..\vars.bat
if not "%MYCLASSPATH%"=="" goto ok
echo Error setting variables.
goto end
:ok

echo Classpath is %MYCLASSPATH%
rem del *.class
javac -classpath %MYCLASSPATH% ANetResult.java
if errorlevel 1 goto end
javac -classpath %MYCLASSPATH% ANet.java
if errorlevel 1 goto end
javah -classpath %MYCLASSPATH% -jni Activision.ANet
if errorlevel 1 goto end
javah -classpath %MYCLASSPATH% -jni Activision.ANet$Player
if errorlevel 1 goto end
javah -classpath %MYCLASSPATH% -jni Activision.ANet$Session
if errorlevel 1 goto end
javah -classpath %MYCLASSPATH% -jni Activision.ANet$InstalledApp
if errorlevel 1 goto end
javah -classpath %MYCLASSPATH% -jni Activision.ANet$ANetTable
if errorlevel 1 goto end
javadoc ANet.java
rem Careful which library you link to.  dpd requires msvcrtd.dll.
cl -MDd -Zi -I%MYJDK%\include -I%MYJDK%\include\win32 -I%MYASDK%\h -I..\..\..\demo\utils -LD -DDPRNT -Ddp_ANET2 -DWIN32 ANetj.c ..\..\..\demo\utils\eclock.c %MYASDK%\win\lib\anet2d.lib -FeANetj2d.dll advapi32.lib > dlog2
if errorlevel 1 more < dlog2
if errorlevel 1 echo Error compiling.  
if errorlevel 1 goto end
if not exist ANetj2d.dll echo Error compiling.  No C compiler?
if not exist ANetj2d.dll goto end
cl -MD -Zi -I%MYJDK%\include -I%MYJDK%\include\win32 -I%MYASDK%\h -I..\..\..\demo\utils -LD -Ddp_ANET2 -DWIN32 ANetj.c ..\..\..\demo\utils\eclock.c %MYASDK%\win\lib\anet2.lib -FeANetj2.dll advapi32.lib > log2
if errorlevel 1 more < log2
if errorlevel 1 echo Error compiling.  
if errorlevel 1 goto end
if not exist ANetj2.dll echo Error compiling.  No C compiler?
if not exist ANetj2.dll goto end
:end

set MYJDK=
set MYASDK=
set MYCLASSPATH=
