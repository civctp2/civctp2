mkdir tcat
cd tcat
cl /W3 /DPTIMER_ENABLE /I..\..\..\h /I..\..\dp /I..\..\..\demo\utils ..\tcat.c ..\tca.c ..\tcapw.c ..\crypttab.c ..\..\3rdparty\d3des\d3des.c ..\..\3rdparty\md5\md5c.c ..\..\dp\assoctab.c ..\..\dp\dynatab.c ..\..\..\demo\utils\eclock.c ..\..\dp\ptimer.c ..\..\..\demo\utils\mywcs.c
if errorlevel 1 goto fail
tcat
if errorlevel 1 goto fail
cd ..
echo Pass
goto end
:fail
cd ..
echo Fail
:end
