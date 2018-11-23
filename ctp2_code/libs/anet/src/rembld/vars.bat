rem Reads environment variables JDK and ASDK if set.
rem Sets environment variables MYJDK, MYASDK, and MYCLASSPATH.
rem On failure, clears MYCLASSPATH.
set MYJDK=%JDK%
set MYASDK=%ASDK%
if %MYJDK%.==.  set MYJDK=d:\jdk
if %MYASDK%.==. set MYASDK=d:\mw2\commdll
set MYCLASSPATH=%MYJDK%\lib\classes.zip;%MYASDK%\src\java;.
if not "%MYCLASSPATH%"=="" goto test
echo Out of environment space
goto end
:test
if exist %MYJDK%\lib\classes.zip goto ok
echo Bad JDK location; can't find %MYJDK%\lib\classes.zip 
set MYCLASSPATH=
goto end
:ok
:end

