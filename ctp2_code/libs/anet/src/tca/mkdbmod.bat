mkdir dbmod
cd dbmod
cl /W3 /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" /I..\..\..\h /I..\..\dp /I..\..\..\demo\utils ..\dbmod.c ..\tcapw.c ..\crypttab.c ..\..\3rdparty\d3des\d3des.c ..\..\3rdparty\md5\md5c.c ..\..\dp\assoctab.c ..\..\dp\dynatab.c ..\..\..\demo\utils\eclock.c ..\..\..\demo\utils\mywcs.c
if errorlevel 1 goto fail
cd ..
