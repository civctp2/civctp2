mkdir scorerept
cd scorerept
set OLDINC=%INCLUDE%
set INCLUDE=%INCLUDE%;..\..\..\h;..\..\dp 
cl /D_DEBUG /DSIMNET /D "_CRT_SECURE_NO_DEPRECATE" /D "_CRT_NONSTDC_NO_DEPRECATE" /D "WINDOWS_IGNORE_PACKING_MISMATCH"  /W3 /Zi ..\scorerept.c  ..\scorerep.c ..\..\dp\assoctab.c ..\..\dp\dynatab.c /Fescorerept.exe
set INCLUDE=%OLDINC%
cd ..
