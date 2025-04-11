mkdir tservts
cd tservts
set OLDINC=%INCLUDE%
set INCLUDE=%INCLUDE%;..\..\..\h;..\..\..\demo\utils;..\..\dp 
cl /D_DEBUG /DSIMNET /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH" /W3 /Zi ..\tservt.c  ..\tserv.c ..\tserva.c ..\tca.c ..\tcapw.c ..\crypttab.c ..\..\..\demo\utils\mywcs.c ..\..\3rdparty\d3des\d3des.c ..\..\3rdparty\md5\md5c.c ..\..\..\demo\utils\rawwin.c ..\..\..\demo\utils\eclock.c ..\..\dp\assoctab.c ..\..\dp\dynatab.c /MDd version.lib advapi32.lib user32.lib /Fetservts.exe
set INCLUDE=%OLDINC%
cd ..

